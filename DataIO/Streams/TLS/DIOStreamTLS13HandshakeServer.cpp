/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamTLS13HandshakeServer.cpp
*
* @class      DIOSTREAMTLS13HANDSHAKESERVER
* @brief      Data Input/Output Stream TLS 1.3 Server Handshake class
* @ingroup    DATAIO
*
* @copyright  EndoraSoft. All rights reserved.
*
* @cond
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
* documentation files(the "Software"), to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense, and/ or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of
* the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
* THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* @endcond
*
* --------------------------------------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"



/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DIOStreamTLS13HandshakeServer.h"

#include "DIOStreamTLSConfig.h"
#include "DIOStreamTLSSignature.h"
#include "DIOStreamTLSMessagesHandShakeClientHello.h"
#include "DIOStreamTLSMessagesHandShakeServerHello.h"

#include "CipherCertificateX509.h"
#include "CipherKey.h"

#include "XRand.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS13HANDSHAKESERVER::DIOSTREAMTLS13HANDSHAKESERVER()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS13HANDSHAKESERVER::DIOSTREAMTLS13HANDSHAKESERVER()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS13HANDSHAKESERVER::~DIOSTREAMTLS13HANDSHAKESERVER()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS13HANDSHAKESERVER::~DIOSTREAMTLS13HANDSHAKESERVER()
{
  End();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS13HANDSHAKESERVER::Ini(DIOSTREAMTLS13SESSION* session, DIOSTREAMTLSCONFIG* config)
* @brief      Initialize the handshake with the role-neutral session and the server configuration to use
* @ingroup    DATAIO
*
* @param[in]  session : TLS 1.3 session, already Ini() with DIOSTREAMTLSKEYSCHEDULE_ROLE_SERVER.
* @param[in]  config : Server configuration (cipher suites / groups / signature schemes / ALPN preference order,
*             plus the local certificate chain and private key to authenticate with).
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS13HANDSHAKESERVER::Ini(DIOSTREAMTLS13SESSION* session, DIOSTREAMTLSCONFIG* config)
{
  if(isini || !session || !session->IsIni() || (session->GetRole() != DIOSTREAMTLSKEYSCHEDULE_ROLE_SERVER) ||
     !config || !config->HasLocalCredentials())
    {
      return false;
    }

  Clean();

  this->session = session;
  this->config  = config;
  state          = DIOSTREAMTLS13HANDSHAKESERVER_STATE_NONE;
  isini          = true;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLS13HANDSHAKESERVER::End()
* @brief      End the handshake
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS13HANDSHAKESERVER::End()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS13HANDSHAKESERVER::IsIni()
* @brief      Check if the class is initialized
* @ingroup    DATAIO
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS13HANDSHAKESERVER::IsIni()
{
  return isini;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS13HANDSHAKESERVER_STATE DIOSTREAMTLS13HANDSHAKESERVER::GetState()
* @brief      Get the current handshake state
* @ingroup    DATAIO
*
* @return     DIOSTREAMTLS13HANDSHAKESERVER_STATE : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS13HANDSHAKESERVER_STATE DIOSTREAMTLS13HANDSHAKESERVER::GetState()
{
  return state;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS13HANDSHAKESERVER::IsHandshakeCompleted()
* @brief      Check whether the handshake has completed
* @ingroup    DATAIO
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS13HANDSHAKESERVER::IsHandshakeCompleted()
{
  return (state == DIOSTREAMTLS13HANDSHAKESERVER_STATE_HANDSHAKE_COMPLETED);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS13HANDSHAKESERVER::IsApplicationProtocolNegotiated()
* @brief      Check whether an ALPN protocol was negotiated
* @ingroup    DATAIO
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS13HANDSHAKESERVER::IsApplicationProtocolNegotiated()
{
  return applicationprotocolnegotiated;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS_ALPN_TYPE DIOSTREAMTLS13HANDSHAKESERVER::GetApplicationProtocol()
* @brief      Get the negotiated ALPN protocol
* @ingroup    DATAIO
*
* @return     DIOSTREAMTLS_ALPN_TYPE : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_ALPN_TYPE DIOSTREAMTLS13HANDSHAKESERVER::GetApplicationProtocol()
{
  return applicationprotocol;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS13HANDSHAKESERVER::CipherSuite_Select(XVECTOR<XWORD>& offered, XWORD& selected)
* @brief      Select the first configured cipher suite (server preference order) that the client also offered
* @note       INTERNAL
* @ingroup    DATAIO
*
* @param[in]  offered : Cipher suites offered by the client.
* @param[out] selected : Selected cipher suite.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS13HANDSHAKESERVER::CipherSuite_Select(XVECTOR<XWORD>& offered, XWORD& selected)
{
  for(XDWORD c=0; c<config->GetCipherSuites()->GetSize(); c++)
    {
      XWORD candidate = config->GetCipherSuites()->Get(c);

      for(XDWORD d=0; d<offered.GetSize(); d++)
        {
          if(offered.Get(d) == candidate)
            {
              selected = candidate;
              return true;
            }
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS13HANDSHAKESERVER::Group_Select(DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO* clienthello, XWORD& selectedgroup, XBUFFER& peerpublickey)
* @brief      Select the first configured group (server preference order) for which the client also sent a key_share
* @note       INTERNAL. If the client only listed the group in supported_groups but did not send a matching
*             key_share entry, a HelloRetryRequest would be required to continue -- out of v1 scope, so this
*             is treated as a negotiation failure instead.
* @ingroup    DATAIO
*
* @param[in]  clienthello : Decoded ClientHello body.
* @param[out] selectedgroup : Selected group.
* @param[out] peerpublickey : Client key_share public key for the selected group.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS13HANDSHAKESERVER::Group_Select(DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO* clienthello, XWORD& selectedgroup, XBUFFER& peerpublickey)
{
  DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE* keyshare = NULL;

  for(XDWORD c=0; c<clienthello->Extensions_GetAll()->GetSize(); c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION* extension = clienthello->Extensions_GetAll()->Get(c);

      if(extension && (extension->GetType() == DIOSTREAMTLS_MSG_EXTENSION_TYPE_KEYSHARE))
        {
          keyshare = (DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE*)extension;
          break;
        }
    }

  if(!keyshare) return false;

  for(XDWORD c=0; c<config->GetSupportedGroups()->GetSize(); c++)
    {
      XWORD candidate = config->GetSupportedGroups()->Get(c);

      for(XDWORD d=0; d<keyshare->List_Get()->GetSize(); d++)
        {
          DIOSTREAMTLS_MSG_EXTENSION_KEY* key = keyshare->List_Get()->Get(d);

          if(key && (key->GetKeyType() == candidate))
            {
              bool validsize = ((candidate == DIOSTREAMTLS_MSG_CURVEID_X25519)   && (key->GetKeyData()->GetSize() == CIPHERECDSAX25519_MAXKEY))      ||
                               ((candidate == DIOSTREAMTLS_MSG_CURVEID_SECP256R1) && (key->GetKeyData()->GetSize() == CIPHERECDSA_P256_PUBLICKEY_SIZE)) ||
                               ((candidate == DIOSTREAMTLS_MSG_CURVEID_SECP384R1) && (key->GetKeyData()->GetSize() == CIPHERECDSA_P384_PUBLICKEY_SIZE));

              if(!validsize) return false;

              selectedgroup = candidate;

              return peerpublickey.Add((*key->GetKeyData()));
            }
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS13HANDSHAKESERVER::SignatureScheme_Select(XVECTOR<XWORD>& offered, CIPHERKEY* leafpublickey, XWORD& selected)
* @brief      Select the first configured signature scheme (server preference order) offered by the client and
*             usable with the local leaf certificate's public key
* @note       INTERNAL
* @ingroup    DATAIO
*
* @param[in]  offered : Signature schemes offered by the client (signature_algorithms extension).
* @param[in]  leafpublickey : Public cipher key decoded from the local leaf certificate.
* @param[out] selected : Selected signature scheme.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS13HANDSHAKESERVER::SignatureScheme_Select(XVECTOR<XWORD>& offered, CIPHERKEY* leafpublickey, XWORD& selected)
{
  for(XDWORD c=0; c<config->GetSignatureSchemes()->GetSize(); c++)
    {
      XWORD candidate = config->GetSignatureSchemes()->Get(c);

      if(!DIOSTREAMTLSSIGNATURE::IsSupported(candidate, leafpublickey)) continue;

      for(XDWORD d=0; d<offered.GetSize(); d++)
        {
          if(offered.Get(d) == candidate)
            {
              selected = candidate;
              return true;
            }
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLS13HANDSHAKESERVER::ApplicationProtocol_Select(XVECTOR<DIOSTREAMTLS_ALPN_TYPE>& offered)
* @brief      Select the first configured ALPN protocol (server preference order) the client also offered
* @note       INTERNAL. Leaving applicationprotocolnegotiated false when nothing matches lets the handshake
*             continue without an ALPN extension in EncryptedExtensions rather than failing the connection.
* @ingroup    DATAIO
*
* @param[in]  offered : Application protocols offered by the client (ALPN extension).
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS13HANDSHAKESERVER::ApplicationProtocol_Select(XVECTOR<DIOSTREAMTLS_ALPN_TYPE>& offered)
{
  applicationprotocolnegotiated = false;
  applicationprotocol           = DIOSTREAMTLS_ALPN_TYPE_UNKNOWN;

  for(XDWORD c=0; c<config->GetApplicationProtocols()->GetSize(); c++)
    {
      DIOSTREAMTLS_ALPN_TYPE candidate = config->GetApplicationProtocols()->Get(c);

      for(XDWORD d=0; d<offered.GetSize(); d++)
        {
          if(offered.Get(d) == candidate)
            {
              applicationprotocol           = candidate;
              applicationprotocolnegotiated = true;
              return;
            }
        }
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS13HANDSHAKESERVER::ClientHello_Process(XBUFFER& clienthello, XBUFFER& records)
* @brief      Process the ClientHello and produce the complete server flight
* @ingroup    DATAIO
*
* @param[in]  clienthello : Complete encoded ClientHello, as received (with its handshake header).
* @param[out] records : Encrypted TLS records ready for transport: ServerHello (cleartext) followed by
*             EncryptedExtensions, Certificate, CertificateVerify and Finished (protected under the handshake
*             traffic keys).
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS13HANDSHAKESERVER::ClientHello_Process(XBUFFER& clienthello, XBUFFER& records)
{
  DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO> clienthellomessage;
  XBUFFER                                                          workbuffer;
  XVECTOR<XWORD>                                                   offeredciphersuites;
  XVECTOR<XWORD>                                                   offeredsignatureschemes;
  XVECTOR<DIOSTREAMTLS_ALPN_TYPE>                                  offeredapplicationprotocols;
  bool                                                              supportedversionsfound = false;
  XWORD                                                             ciphersuite;
  XWORD                                                             group;
  XBUFFER                                                           peerpublickey;
  XBUFFER                                                           serverpublickey;
  XBUFFER                                                           sharedsecret;
  XWORD                                                             signaturescheme;
  CIPHERCERTIFICATEX509                                            leafcertificate;
  XRAND*                                                            xrand;
  XBYTE                                                             random[DIOSTREAMTLS_MSG_RANDOM_SIZE];
  bool                                                              status;

  if(!isini || !session || (state != DIOSTREAMTLS13HANDSHAKESERVER_STATE_NONE) ||
     !session->GetTranscript()->IsEmpty() || (&clienthello == &records))
    {
      return SetError();
    }

  records.Delete();

  workbuffer.Add(clienthello);

  if(!clienthellomessage.GetFromBuffer(workbuffer, false) || !workbuffer.IsEmpty() ||
     (clienthellomessage.GetMsgType() != DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_CLIENT_HELLO))
    {
      return SetError();
    }

  DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO* body = clienthellomessage.GetBody();

  for(XDWORD c=0; c<body->GetCipherSuites()->GetSize(); c++)
    {
      if(!offeredciphersuites.Add(body->GetCipherSuites()->Get(c))) return SetError();
    }

  for(XDWORD c=0; c<body->Extensions_GetAll()->GetSize(); c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION* extension = body->Extensions_GetAll()->Get(c);
      if(!extension) return SetError();

      if(extension->GetType() == DIOSTREAMTLS_MSG_EXTENSION_TYPE_SUPPORTEDVERSIONS)
        {
          DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS* versions = (DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS*)extension;

          for(XDWORD d=0; d<versions->List_Get()->GetSize(); d++)
            {
              if(versions->List_Get()->Get(d) == DIOSTREAMTLS_MSG_VERSION_TLS_1_3) supportedversionsfound = true;
            }
        }

      if(extension->GetType() == DIOSTREAMTLS_MSG_EXTENSION_TYPE_SIGNATUREALGORITHMS)
        {
          DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS* algorithms = (DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS*)extension;

          for(XDWORD d=0; d<algorithms->List_Get()->GetSize(); d++)
            {
              if(!offeredsignatureschemes.Add(algorithms->List_Get()->Get(d))) return SetError();
            }
        }

      if(extension->GetType() == DIOSTREAMTLS_MSG_EXTENSION_TYPE_ALPN)
        {
          DIOSTREAMTLS_MSG_EXTENSION_ALPN* ALPN = (DIOSTREAMTLS_MSG_EXTENSION_ALPN*)extension;

          for(XDWORD d=0; d<ALPN->List_GetNProtocols(); d++)
            {
              DIOSTREAMTLS_ALPN_TYPE alpntype;

              if(!ALPN->List_Get(d, alpntype) || !offeredapplicationprotocols.Add(alpntype)) return SetError();
            }
        }
    }

  // Not TLS 1.3, no key_share, no signature_algorithms or nothing our leaf certificate can use, no shared
  // cipher suite, or nothing shared with what config->GetSupportedGroups() offers without a HelloRetryRequest:
  // all treated alike as a negotiation failure (v1 has no HelloRetryRequest / downgrade path).
  if(!supportedversionsfound || offeredsignatureschemes.IsEmpty() ||
     !config->GetLocalCertificateChain() || config->GetLocalCertificateChain()->IsEmpty() ||
     !leafcertificate.Decode((*config->GetLocalCertificateChain()->Get(0))) ||
     !CipherSuite_Select(offeredciphersuites, ciphersuite) ||
     !Group_Select(body, group, peerpublickey) ||
     !SignatureScheme_Select(offeredsignatureschemes, leafcertificate.GetPublicCipherKey(), signaturescheme))
    {
      return SetError();
    }

  ApplicationProtocol_Select(offeredapplicationprotocols);

  session->KeyExchange_Delete();

  if(!session->KeyExchange_Generate(group, serverpublickey) ||
     !session->KeyExchange_SharedSecret(group, peerpublickey, sharedsecret) ||
     !session->CipherSuite_Select(ciphersuite))
    {
      sharedsecret.FillBuffer(0);
      return SetError();
    }

  if(!session->Transcript_Add(clienthello))
    {
      sharedsecret.FillBuffer(0);
      return SetError();
    }

  // ---- ServerHello (cleartext: local direction is still epoch CLEAR at this point) --------------------------

  xrand = GEN_XFACTORY.CreateRand();
  if(!xrand)
    {
      sharedsecret.FillBuffer(0);
      return SetError();
    }

  status = xrand->Ini();
  if(status) status = xrand->Generate(random, sizeof(random));

  GEN_XFACTORY.DeleteRand(xrand);

  if(!status)
    {
      sharedsecret.FillBuffer(0);
      return SetError();
    }

  DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO> serverhellomessage;
  DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO*                           serverhellobody;
  XBUFFER                                                           serverhello;

  serverhellomessage.SetMsgType(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_SERVER_HELLO);

  serverhellobody = serverhellomessage.GetBody();
  if(!serverhellobody)
    {
      sharedsecret.FillBuffer(0);
      return SetError();
    }

  serverhellobody->SetLegacyVersion(DIOSTREAMTLS_MSG_VERSION_TLS_1_2);
  memcpy(serverhellobody->GetRandom(), random, sizeof(random));

  // Echo the client's legacy_session_id back for middlebox compatibility mode (RFC 8446 4.1.3).
  serverhellobody->SetSessionIDLength(body->GetSessionIDLength());
  if(body->GetSessionIDLength()) memcpy(serverhellobody->GetSessionID(), body->GetSessionID(), body->GetSessionIDLength());

  serverhellobody->SetCipherSuite(ciphersuite);
  serverhellobody->SetCompressionMethod(DIOSTREAMTLS_MSG_COMPRESS_METHOD_NULL);

  DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS_SERVER* supportedversions = GEN_NEW DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS_SERVER();
  if(!supportedversions)
    {
      sharedsecret.FillBuffer(0);
      return SetError();
    }

  supportedversions->SetVersion(DIOSTREAMTLS_MSG_VERSION_TLS_1_3);

  if(!serverhellobody->Extensions_Add(supportedversions))
    {
      GEN_DELETE supportedversions;
      sharedsecret.FillBuffer(0);
      return SetError();
    }

  DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE_SERVER* keyshare = GEN_NEW DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE_SERVER();
  if(!keyshare)
    {
      sharedsecret.FillBuffer(0);
      return SetError();
    }

  keyshare->GetKey()->SetKeyType(group);

  if(!keyshare->GetKey()->GetKeyData()->Add(serverpublickey) || !serverhellobody->Extensions_Add(keyshare))
    {
      GEN_DELETE keyshare;
      sharedsecret.FillBuffer(0);
      return SetError();
    }

  if(!serverhellomessage.SetToBuffer(serverhello, false) ||
     !session->GetRecord()->Protect(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE, serverhello, records) ||
     !session->Transcript_Add(serverhello) ||
     !session->HandshakeKeys_Activate(sharedsecret))
    {
      sharedsecret.FillBuffer(0);
      records.Delete();
      return SetError();
    }

  sharedsecret.FillBuffer(0);
  sharedsecret.Delete();

  // ---- EncryptedExtensions, Certificate, CertificateVerify, Finished (protected under handshake keys) -------

  DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_ENCRYPTEDEXTENSIONS> encryptedextensionsmessage;
  XBUFFER                                                                   encryptedextensions;
  XBUFFER                                                                   flightrecords;

  encryptedextensionsmessage.SetMsgType(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_ENCRYPTED_EXTENSIONS);

  if(applicationprotocolnegotiated)
    {
      DIOSTREAMTLS_MSG_EXTENSION_ALPN* ALPN = GEN_NEW DIOSTREAMTLS_MSG_EXTENSION_ALPN();
      if(!ALPN) return SetError();

      if(!ALPN->List_Add(applicationprotocol) || !encryptedextensionsmessage.GetBody()->Extensions_Add(ALPN))
        {
          GEN_DELETE ALPN;
          return SetError();
        }
    }

  if(!encryptedextensionsmessage.SetToBuffer(encryptedextensions, false) ||
     !session->GetRecord()->Protect(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE, encryptedextensions, flightrecords) ||
     !session->Transcript_Add(encryptedextensions) || !records.Add(flightrecords))
    {
      records.Delete();
      return SetError();
    }

  DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_CERTIFICATE> certificatemessage;
  XBUFFER                                                          certificatebuffer;

  certificatemessage.SetMsgType(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_CERTIFICATE);

  if(!certificatemessage.GetBody()->SetRequestContext(NULL, 0))
    {
      records.Delete();
      return SetError();
    }

  for(XDWORD c=0; c<config->GetLocalCertificateChain()->GetSize(); c++)
    {
      XBUFFER* dercertificate = config->GetLocalCertificateChain()->Get(c);
      DIOSTREAMTLS_MSG_CERTIFICATEENTRY* entry;

      if(!dercertificate)
        {
          records.Delete();
          return SetError();
        }

      entry = GEN_NEW DIOSTREAMTLS_MSG_CERTIFICATEENTRY();
      if(!entry)
        {
          records.Delete();
          return SetError();
        }

      if(!entry->GetCertificateData()->Add((*dercertificate)) || !certificatemessage.GetBody()->CertificateList_Add(entry))
        {
          GEN_DELETE entry;
          records.Delete();
          return SetError();
        }
    }

  flightrecords.Delete();

  if(!certificatemessage.SetToBuffer(certificatebuffer, false) ||
     !session->GetRecord()->Protect(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE, certificatebuffer, flightrecords) ||
     !session->Transcript_Add(certificatebuffer) || !records.Add(flightrecords))
    {
      records.Delete();
      return SetError();
    }

  static XBYTE certificateverifycontext[] = { 'T', 'L', 'S', ' ', '1', '.', '3', ',', ' ',
                                              's', 'e', 'r', 'v', 'e', 'r', ' ',
                                              'C', 'e', 'r', 't', 'i', 'f', 'i', 'c', 'a', 't', 'e', 'V', 'e', 'r', 'i', 'f', 'y' };

  DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_CERTIFICATEVERIFY> certificateverifymessage;
  XBUFFER                                                                certificateverifybuffer;
  XBUFFER                                                                transcripthash;
  XBUFFER                                                                signedcontent;
  XBUFFER                                                                signature;

  if(!session->TranscriptHash(transcripthash))
    {
      records.Delete();
      return SetError();
    }

  for(int c=0; c<64; c++)
    {
      if(!signedcontent.Add((XBYTE)0x20))
        {
          records.Delete();
          return SetError();
        }
    }

  if(!signedcontent.Add(certificateverifycontext, sizeof(certificateverifycontext)) || !signedcontent.Add((XBYTE)0x00) ||
     !signedcontent.Add(transcripthash) ||
     !DIOSTREAMTLSSIGNATURE::Sign(signaturescheme, config->GetLocalPrivateKey(), leafcertificate.GetPublicCipherKey(),
                                  signedcontent, signature))
    {
      records.Delete();
      return SetError();
    }

  certificateverifymessage.SetMsgType(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_CERTIFICATE_VERIFY);
  certificateverifymessage.GetBody()->SetAlgorithm(signaturescheme);

  flightrecords.Delete();

  if(!certificateverifymessage.GetBody()->GetSignature()->Add(signature) ||
     !certificateverifymessage.SetToBuffer(certificateverifybuffer, false) ||
     !session->GetRecord()->Protect(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE, certificateverifybuffer, flightrecords) ||
     !session->Transcript_Add(certificateverifybuffer) || !records.Add(flightrecords))
    {
      records.Delete();
      return SetError();
    }

  DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_FINISHED> finishedmessage;
  XBUFFER                                                       finishedbuffer;
  XBUFFER                                                       finishedtranscripthash;
  XBUFFER                                                       verifydata;

  if(!session->TranscriptHash(finishedtranscripthash) ||
     !session->GetKeySchedule()->CalculateFinished(DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL, finishedtranscripthash, verifydata))
    {
      records.Delete();
      return SetError();
    }

  finishedmessage.SetMsgType(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_FINISHED);

  flightrecords.Delete();

  if(!finishedmessage.GetBody()->GetVerifyData()->Add(verifydata) ||
     !finishedmessage.SetToBuffer(finishedbuffer, false) ||
     !session->GetRecord()->Protect(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE, finishedbuffer, flightrecords) ||
     !session->Transcript_Add(finishedbuffer) || !records.Add(flightrecords) ||
     !session->ApplicationKeys_Activate(DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL))
    {
      records.Delete();
      return SetError();
    }

  state = DIOSTREAMTLS13HANDSHAKESERVER_STATE_WAIT_FINISHED;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS13HANDSHAKESERVER::Finished_Process(XBUFFER& message)
* @brief      Verify the client Finished and activate remote application keys
* @note       INTERNAL
* @ingroup    DATAIO
*
* @param[in]  message : Complete handshake message, including its header.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS13HANDSHAKESERVER::Finished_Process(XBUFFER& message)
{
  DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_FINISHED> finished;
  XBUFFER                                                       workbuffer;
  XBUFFER                                                       transcripthash;

  if(state != DIOSTREAMTLS13HANDSHAKESERVER_STATE_WAIT_FINISHED)
    {
      return SetError();
    }

  workbuffer.Add(message);

  if(!finished.GetFromBuffer(workbuffer, false) || !workbuffer.IsEmpty() ||
     (finished.GetMsgType() != DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_FINISHED) ||
     (finished.GetBody()->GetVerifyData()->GetSize() != session->GetKeySchedule()->GetHashSize()) ||
     !session->TranscriptHash(transcripthash) ||
     !session->GetKeySchedule()->VerifyFinished(DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE,
                                                transcripthash, *finished.GetBody()->GetVerifyData()))
    {
      return SetError();
    }

  if(!session->Transcript_Add(message) ||
     !session->ApplicationKeys_Activate(DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE))
    {
      return SetError();
    }

  state = DIOSTREAMTLS13HANDSHAKESERVER_STATE_HANDSHAKE_COMPLETED;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS13HANDSHAKESERVER::RecordInput_Add(XBYTE* data, XDWORD size)
* @brief      Add received transport bytes to the TLS session
* @ingroup    DATAIO
*
* @param[in]  data : Bytes to add.
* @param[in]  size : Number of bytes to add.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS13HANDSHAKESERVER::RecordInput_Add(XBYTE* data, XDWORD size)
{
  if(!isini || !session)
    {
      return false;
    }

  return session->RecordInput_Add(data, size);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS13HANDSHAKESERVER::RecordInput_Add(XBUFFER& data)
* @brief      Add a received transport buffer to the TLS session
* @ingroup    DATAIO
*
* @param[in]  data : Buffer to add.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS13HANDSHAKESERVER::RecordInput_Add(XBUFFER& data)
{
  return RecordInput_Add(data.Get(), data.GetSize());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS13HANDSHAKESERVER::Process()
* @brief      Process every complete record and handshake message currently accumulated
* @note       An incomplete record or message is retained and is not an error.
* @ingroup    DATAIO
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS13HANDSHAKESERVER::Process()
{
  if(!isini || !session || (state == DIOSTREAMTLS13HANDSHAKESERVER_STATE_ERROR))
    {
      return false;
    }

  while(true)
    {
      DIOSTREAMTLS_CONTENTTYPE   contenttype = (DIOSTREAMTLS_CONTENTTYPE)0;
      DIOSTREAMTLS13SESSION_RESULT result;
      XBUFFER                    plain;

      result = session->Record_Extract(contenttype, plain);

      if(result == DIOSTREAMTLS13SESSION_RESULT_INCOMPLETE)
        {
          return true;
        }

      if(result == DIOSTREAMTLS13SESSION_RESULT_ERROR)
        {
          return SetError();
        }

      switch(contenttype)
        {
          case DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE          : if(!session->HandshakeInput_Add(plain)) return SetError();
                                                                break;

          case DIOSTREAMTLS_MSG_CONTENTTYPE_CHANGE_CIPHER_SPEC : if((plain.GetSize() != 1) || (plain.GetByte(0) != 1)) return SetError();
                                                                break;

          case DIOSTREAMTLS_MSG_CONTENTTYPE_ALERT              : { DIOSTREAMTLS_MSG_ALERT alert;

                                                                  alert.GetFromBuffer(plain, false);
                                                                  return SetError();
                                                                }

                                                       default :
                                                                 return SetError();
        }

      while(true)
        {
          XBUFFER handshake;

          result = session->Handshake_Extract(handshake);

          if(result == DIOSTREAMTLS13SESSION_RESULT_INCOMPLETE)
            {
              break;
            }

          if(result == DIOSTREAMTLS13SESSION_RESULT_ERROR)
            {
              return SetError();
            }

          if(!Handshake_Process(handshake))
            {
              return false;
            }

          // The client's Finished message just completed the handshake and switched the remote epoch to
          // DIOSTREAMTLS13SESSION_EPOCH_APPLICATION (see Finished_Process() / ApplicationKeys_Activate()). Any
          // bytes still queued past this point -- e.g. the client's very first request, pipelined right behind
          // its Finished and pulled in by the very same Transport_Read() -- are already application-data records,
          // not handshake ones: this loop (and the switch above, which only understands HANDSHAKE /
          // CHANGE_CIPHER_SPEC / ALERT) must not try to interpret them, or it fails the whole connection via the
          // "default: SetError()" case below. Leave them queued in the shared session for the normal
          // post-handshake path (DIOSTREAMTLS<T>::ApplicationInput_Process() -> session.ApplicationData_Process())
          // to pick up on its own next call.
          if(state == DIOSTREAMTLS13HANDSHAKESERVER_STATE_HANDSHAKE_COMPLETED) return true;
        }
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS13HANDSHAKESERVER::Handshake_Process(XBUFFER& message)
* @brief      Process one exact client handshake message according to the current state
* @ingroup    DATAIO
*
* @param[in]  message : Complete handshake message, including its header.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS13HANDSHAKESERVER::Handshake_Process(XBUFFER& message)
{
  DIOSTREAMTLS_MSG_HANDSHAKE genericmessage;
  XBUFFER                    workbuffer;

  if(!isini || !session || (state == DIOSTREAMTLS13HANDSHAKESERVER_STATE_ERROR))
    {
      return false;
    }

  workbuffer.Add(message);

  if(!genericmessage.GetFromBuffer(workbuffer, false) || !workbuffer.IsEmpty())
    {
      return SetError();
    }

  switch(genericmessage.GetMsgType())
    {
      // No client authentication in v1: a CertificateRequest is never sent, so the client is never expected to
      // reply with a Certificate / CertificateVerify of its own -- only its Finished is waited for.
      case DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_FINISHED : return Finished_Process(message);
                                                    default : return SetError();
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS13HANDSHAKESERVER::SetError()
* @brief      Move the handshake to its terminal error state
* @note       INTERNAL
* @ingroup    DATAIO
*
* @return     bool : Always false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS13HANDSHAKESERVER::SetError()
{
  state = DIOSTREAMTLS13HANDSHAKESERVER_STATE_ERROR;

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLS13HANDSHAKESERVER::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS13HANDSHAKESERVER::Clean()
{
  session                       = NULL;
  config                        = NULL;
  state                          = DIOSTREAMTLS13HANDSHAKESERVER_STATE_NONE;
  isini                          = false;
  applicationprotocolnegotiated = false;
  applicationprotocol            = DIOSTREAMTLS_ALPN_TYPE_UNKNOWN;
}
