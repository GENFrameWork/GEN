/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTLSConfig.cpp
* 
* @class      DIOSTREAMTLSCONFIG
* @brief      Data Input/Output Stream TLS (Transport Layer Security) Config class
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

#include "DIOStreamTLSConfig.h"

#include "XFactory.h"
#include "XDateTime.h"
#include "XRand.h"

#include "CipherCertificateX509.h"
#include "CipherAESGCM.h"
#include "CipherKeySymmetrical.h"
#include "CipherKeyPrivateRSA.h"
#include "CipherKeyECDSA.h"
#include "CipherTrustedRootCertificatesX509.h"

#include "DIOStreamTLSSignature.h"

#include "DIOFactory.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


static bool DIOSTREAMTLSCONFIG_ServerNameMatch(XSTRING& pattern, XCHAR* servername)
{
  XSTRING hostname;
  int     dot;

  if(!servername || !servername[0] || pattern.IsEmpty()) return false;

  hostname = servername;

  if(hostname.Character_GetLast() == __C('.')) hostname.DeleteLastCharacter();
  if(pattern.Character_GetLast()  == __C('.')) pattern.DeleteLastCharacter();

  if(!pattern.Compare(hostname, true)) return true;

  if((pattern.GetSize() < 3) || (pattern[0] != __C('*')) || (pattern[1] != __C('.')) ||
     (pattern.FindCharacter(__C('*'), 1) >= 0)) return false;

  dot = hostname.FindCharacter(__C('.'));
  if(dot <= 0) return false;

  XSTRING hostnamesuffix(&hostname.Get()[dot]);
  XSTRING patternsuffix(&pattern.Get()[1]);

  return !hostnamesuffix.Compare(patternsuffix, true);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLSSERVERCREDENTIALS::DIOSTREAMTLSSERVERCREDENTIALS()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSSERVERCREDENTIALS::DIOSTREAMTLSSERVERCREDENTIALS()
{
  privatekey = NULL;

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLSSERVERCREDENTIALS::~DIOSTREAMTLSSERVERCREDENTIALS()
* @brief      Destructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSSERVERCREDENTIALS::~DIOSTREAMTLSSERVERCREDENTIALS()
{
  Delete();
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOSTREAMTLSSERVERCREDENTIALS::GetServerName()
* @brief      Get the SNI server name pattern associated with these credentials
* @ingroup    DATAIO
*
* @return     XSTRING* : Server name pattern.
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOSTREAMTLSSERVERCREDENTIALS::GetServerName()
{
  return &servername;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<XBUFFER*>* DIOSTREAMTLSSERVERCREDENTIALS::GetCertificateChain()
* @brief      Get the certificate chain associated with this SNI name
* @ingroup    DATAIO
*
* @return     XVECTOR<XBUFFER*>* : Certificate chain.
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XBUFFER*>* DIOSTREAMTLSSERVERCREDENTIALS::GetCertificateChain()
{
  return &certificatechain;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSERVERCREDENTIALS::Certificate_Add(XBUFFER& certificate)
* @brief      Add a copied DER certificate to the SNI certificate chain
* @ingroup    DATAIO
*
* @param[in]  certificate : DER certificate.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSERVERCREDENTIALS::Certificate_Add(XBUFFER& certificate)
{
  XBUFFER* copy;

  if(certificate.IsEmpty()) return false;

  copy = GEN_NEW XBUFFER();
  if(!copy) return false;

  if(!copy->Add(certificate) || !certificatechain.Add(copy))
    {
      GEN_DELETE copy;
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSERVERCREDENTIALS::Certificates_Delete()
* @brief      Delete the SNI certificate chain
* @ingroup    DATAIO
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSERVERCREDENTIALS::Certificates_Delete()
{
  certificatechain.DeleteContents();
  certificatechain.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         CIPHERKEY* DIOSTREAMTLSSERVERCREDENTIALS::GetPrivateKey()
* @brief      Get the private key associated with this SNI name
* @ingroup    DATAIO
*
* @return     CIPHERKEY* : Private key.
*
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERKEY* DIOSTREAMTLSSERVERCREDENTIALS::GetPrivateKey()
{
  return privatekey;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSERVERCREDENTIALS::SetPrivateKey(CIPHERKEY* privatekey)
* @brief      Copy the private key associated with this SNI name
* @ingroup    DATAIO
*
* @param[in]  privatekey : Private key to copy.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSERVERCREDENTIALS::SetPrivateKey(CIPHERKEY* privatekey)
{
  CIPHERKEY* copy = NULL;

  if(!privatekey || !privatekey->GetSizeInBytes()) return false;

  switch(privatekey->GetType())
    {
      case CIPHERKEYTYPE_RSA_PRIVATE :
        {
          CIPHERKEYPRIVATERSA* rsacopy = GEN_NEW CIPHERKEYPRIVATERSA();
          if(!rsacopy) return false;

          if(!rsacopy->CopyFrom((CIPHERKEYPRIVATERSA*)privatekey))
            {
              GEN_DELETE rsacopy;
              return false;
            }

          copy = rsacopy;
        }
        break;

      case CIPHERKEYTYPE_ECDSA_SECP256R1_PRIVATE :
      case CIPHERKEYTYPE_ECDSA_SECP384R1_PRIVATE :
      case CIPHERKEYTYPE_ECDSA_SECP521R1_PRIVATE :
        {
          CIPHERKEYECDSA* ecdsacopy = GEN_NEW CIPHERKEYECDSA();
          if(!ecdsacopy) return false;

          if(!ecdsacopy->CopyFrom((CIPHERKEYSYMMETRICAL*)privatekey))
            {
              GEN_DELETE ecdsacopy;
              return false;
            }

          copy = ecdsacopy;
        }
        break;

                                        default : return false;
    }

  if(this->privatekey) GEN_DELETE this->privatekey;

  this->privatekey = copy;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSERVERCREDENTIALS::HasCredentials()
* @brief      Check whether this SNI entry contains a certificate chain and private key
* @ingroup    DATAIO
*
* @return     bool : true if credentials are configured; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSERVERCREDENTIALS::HasCredentials()
{
  return !certificatechain.IsEmpty() && privatekey;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSERVERCREDENTIALS::Delete()
* @brief      Delete all credentials owned by this SNI entry
* @ingroup    DATAIO
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSERVERCREDENTIALS::Delete()
{
  Certificates_Delete();

  if(privatekey)
    {
      GEN_DELETE privatekey;
      privatekey = NULL;
    }

  servername.Empty();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLSSERVERCREDENTIALS::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLSSERVERCREDENTIALS::Clean()
{
  servername.Empty();
  privatekey = NULL;
}


static XQWORD DIOSTREAMTLSCONFIG_CurrentEpoch()
{
  XDATETIME* datetime = GEN_XFACTORY.CreateDateTime();
  XQWORD     epoch    = 0;

  if(datetime)
    {
      if(datetime->Read(false)) epoch = datetime->GetEPOCHFormat();
      GEN_XFACTORY.DeleteDateTime(datetime);
    }

  return epoch;
}


DIOSTREAMTLS13SESSIONTICKET::DIOSTREAMTLS13SESSIONTICKET()
{
  Clean();
}


DIOSTREAMTLS13SESSIONTICKET::~DIOSTREAMTLS13SESSIONTICKET()
{
  Delete();
  Clean();
}


XSTRING* DIOSTREAMTLS13SESSIONTICKET::GetServerName() { return &servername; }
XBUFFER* DIOSTREAMTLS13SESSIONTICKET::GetTicket() { return &ticket; }
XBUFFER* DIOSTREAMTLS13SESSIONTICKET::GetPSK() { return &psk; }
XDWORD DIOSTREAMTLS13SESSIONTICKET::GetTicketAgeAdd() { return ticketageadd; }
void DIOSTREAMTLS13SESSIONTICKET::SetTicketAgeAdd(XDWORD ageadd) { ticketageadd = ageadd; }
XDWORD DIOSTREAMTLS13SESSIONTICKET::GetLifetime() { return lifetime; }
void DIOSTREAMTLS13SESSIONTICKET::SetLifetime(XDWORD lifetime) { this->lifetime = lifetime; }
XQWORD DIOSTREAMTLS13SESSIONTICKET::GetReceivedEpoch() { return receivedepoch; }
void DIOSTREAMTLS13SESSIONTICKET::SetReceivedEpoch(XQWORD epoch) { receivedepoch = epoch; }
XWORD DIOSTREAMTLS13SESSIONTICKET::GetCipherSuite() { return ciphersuite; }
void DIOSTREAMTLS13SESSIONTICKET::SetCipherSuite(XWORD ciphersuite) { this->ciphersuite = ciphersuite; }
DIOSTREAMTLS_ALPN_TYPE DIOSTREAMTLS13SESSIONTICKET::GetApplicationProtocol() { return applicationprotocol; }
void DIOSTREAMTLS13SESSIONTICKET::SetApplicationProtocol(DIOSTREAMTLS_ALPN_TYPE protocol) { applicationprotocol = protocol; }


bool DIOSTREAMTLS13SESSIONTICKET::IsExpired()
{
  XQWORD now = DIOSTREAMTLSCONFIG_CurrentEpoch();
  if(!now || !receivedepoch || !lifetime) return true;
  return (now < receivedepoch) || ((now - receivedepoch) >= lifetime);
}


XDWORD DIOSTREAMTLS13SESSIONTICKET::GetObfuscatedAge()
{
  XQWORD now = DIOSTREAMTLSCONFIG_CurrentEpoch();
  if(!now || !receivedepoch || now < receivedepoch) return ticketageadd;

  XQWORD agems = (now - receivedepoch) * 1000ULL;
  return (XDWORD)((agems + ticketageadd) & 0xFFFFFFFFULL);
}


bool DIOSTREAMTLS13SESSIONTICKET::Delete()
{
  if(!psk.IsEmpty()) psk.FillBuffer(0);
  psk.Delete();
  ticket.Delete();
  servername.Empty();
  Clean();
  return true;
}


void DIOSTREAMTLS13SESSIONTICKET::Clean()
{
  ticketageadd        = 0;
  lifetime            = 0;
  receivedepoch       = 0;
  ciphersuite         = 0;
  applicationprotocol = DIOSTREAMTLS_ALPN_TYPE_UNKNOWN;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLSCONFIG::DIOSTREAMTLSCONFIG() : DIOSTREAMTCPIPCONFIG()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSCONFIG::DIOSTREAMTLSCONFIG() : DIOSTREAMTCPIPCONFIG()
{
  localprivatekey = NULL;

  Clean();

  SetIsTLS(true);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLSCONFIG::~DIOSTREAMTLSCONFIG()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSCONFIG::~DIOSTREAMTLSCONFIG()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLSCONFIG::GetCipherSuite()
* @brief      Get the TLS 1.3 cipher suite
* @ingroup    DATAIO
* 
* @return     XWORD : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLSCONFIG::GetCipherSuite()
{
  if(ciphersuites.IsEmpty()) return 0;

  return ciphersuites.Get(0);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLSCONFIG::SetCipherSuite(XWORD ciphersuite)
* @brief      Set the TLS 1.3 cipher suite
* @ingroup    DATAIO
* 
* @param[in]  ciphersuite : Cipher suite value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLSCONFIG::SetCipherSuite(XWORD ciphersuite)
{
  ciphersuites.DeleteAll();
  ciphersuites.Add(ciphersuite);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<XWORD>* DIOSTREAMTLSCONFIG::GetCipherSuites()
* @brief      Get the ordered TLS 1.3 cipher suite capabilities
* @ingroup    DATAIO
*
* @return     XVECTOR<XWORD>* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XWORD>* DIOSTREAMTLSCONFIG::GetCipherSuites()
{
  return &ciphersuites;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::CipherSuite_Add(XWORD ciphersuite)
* @brief      Add a supported TLS 1.3 cipher suite capability
* @ingroup    DATAIO
*
* @param[in]  ciphersuite : Cipher suite value.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::CipherSuite_Add(XWORD ciphersuite)
{
  if((ciphersuite != DIOSTREAMTLS_MSG_CIPHER_AES_128_GCM_SHA256) &&
     (ciphersuite != DIOSTREAMTLS_MSG_CIPHER_AES_256_GCM_SHA384)) return false;

  for(XDWORD c=0; c<ciphersuites.GetSize(); c++)
    {
      if(ciphersuites.Get(c) == ciphersuite) return false;
    }

  return ciphersuites.Add(ciphersuite);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::CipherSuites_Delete()
* @brief      Delete all TLS 1.3 cipher suite capabilities
* @ingroup    DATAIO
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::CipherSuites_Delete()
{
  ciphersuites.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<XWORD>* DIOSTREAMTLSCONFIG::GetSupportedGroups()
* @brief      Get the ordered key exchange group capabilities
* @ingroup    DATAIO
*
* @return     XVECTOR<XWORD>* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XWORD>* DIOSTREAMTLSCONFIG::GetSupportedGroups()
{
  return &supportedgroups;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::SupportedGroup_Add(XWORD supportedgroup)
* @brief      Add a supported key exchange group capability
* @ingroup    DATAIO
*
* @param[in]  supportedgroup : Supported group value.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::SupportedGroup_Add(XWORD supportedgroup)
{
  if((supportedgroup != DIOSTREAMTLS_MSG_CURVEID_X25519) &&
     (supportedgroup != DIOSTREAMTLS_MSG_CURVEID_SECP256R1) &&
     (supportedgroup != DIOSTREAMTLS_MSG_CURVEID_SECP384R1)) return false;

  for(XDWORD c=0; c<supportedgroups.GetSize(); c++)
    {
      if(supportedgroups.Get(c) == supportedgroup) return false;
    }

  return supportedgroups.Add(supportedgroup);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::SupportedGroups_Delete()
* @brief      Delete all key exchange group capabilities
* @ingroup    DATAIO
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::SupportedGroups_Delete()
{
  supportedgroups.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<XWORD>* DIOSTREAMTLSCONFIG::GetSignatureSchemes()
* @brief      Get the ordered TLS CertificateVerify signature capabilities
* @ingroup    DATAIO
*
* @return     XVECTOR<XWORD>* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XWORD>* DIOSTREAMTLSCONFIG::GetSignatureSchemes()
{
  return &signatureschemes;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::SignatureScheme_Add(XWORD signaturescheme)
* @brief      Add a supported TLS CertificateVerify signature capability
* @ingroup    DATAIO
*
* @param[in]  signaturescheme : Signature scheme value.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::SignatureScheme_Add(XWORD signaturescheme)
{
  switch(signaturescheme)
    {
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP256R1_SHA256 :
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP384R1_SHA384 :
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP521R1_SHA512 :
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA256 :
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA384 :
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA512 : break;
                                                        default : return false;
    }

  for(XDWORD c=0; c<signatureschemes.GetSize(); c++)
    {
      if(signatureschemes.Get(c) == signaturescheme) return false;
    }

  return signatureschemes.Add(signaturescheme);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::SignatureSchemes_Delete()
* @brief      Delete all TLS CertificateVerify signature capabilities
* @ingroup    DATAIO
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::SignatureSchemes_Delete()
{
  signatureschemes.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<XWORD>* DIOSTREAMTLSCONFIG::GetCertificateSignatureSchemes()
* @brief      Get the ordered X.509 certificate signature capabilities
* @ingroup    DATAIO
*
* @return     XVECTOR<XWORD>* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XWORD>* DIOSTREAMTLSCONFIG::GetCertificateSignatureSchemes()
{
  return &certificatesignatureschemes;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::CertificateSignatureScheme_Add(XWORD signaturescheme)
* @brief      Add a supported X.509 certificate signature capability
* @ingroup    DATAIO
*
* @param[in]  signaturescheme : Signature scheme value.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::CertificateSignatureScheme_Add(XWORD signaturescheme)
{
  switch(signaturescheme)
    {
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP256R1_SHA256 :
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP384R1_SHA384 :
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP521R1_SHA512 :
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA256 :
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA384 :
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA512 :
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PKCS1_SHA256     :
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PKCS1_SHA384     :
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PKCS1_SHA512     : break;
                                                        default : return false;
    }

  for(XDWORD c=0; c<certificatesignatureschemes.GetSize(); c++)
    {
      if(certificatesignatureschemes.Get(c) == signaturescheme) return false;
    }

  return certificatesignatureschemes.Add(signaturescheme);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::CertificateSignatureSchemes_Delete()
* @brief      Delete all X.509 certificate signature capabilities
* @ingroup    DATAIO
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::CertificateSignatureSchemes_Delete()
{
  certificatesignatureschemes.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<DIOSTREAMTLS_ALPN_TYPE>* DIOSTREAMTLSCONFIG::GetApplicationProtocols()
* @brief      Get the ordered ALPN application protocol capabilities
* @ingroup    DATAIO
*
* @return     XVECTOR<DIOSTREAMTLS_ALPN_TYPE>* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<DIOSTREAMTLS_ALPN_TYPE>* DIOSTREAMTLSCONFIG::GetApplicationProtocols()
{
  return &applicationprotocols;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::ApplicationProtocol_Add(DIOSTREAMTLS_ALPN_TYPE applicationprotocol)
* @brief      Add an ALPN application protocol capability
* @ingroup    DATAIO
*
* @param[in]  applicationprotocol : Application protocol value.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::ApplicationProtocol_Add(DIOSTREAMTLS_ALPN_TYPE applicationprotocol)
{
  switch(applicationprotocol)
    {
      case DIOSTREAMTLS_ALPN_TYPE_HTTP_1_1 :
      case DIOSTREAMTLS_ALPN_TYPE_HTTP_2   :
      case DIOSTREAMTLS_ALPN_TYPE_HTTP_3   : break;
                                      default : return false;
    }

  for(XDWORD c=0; c<applicationprotocols.GetSize(); c++)
    {
      if(applicationprotocols.Get(c) == applicationprotocol) return false;
    }

  return applicationprotocols.Add(applicationprotocol);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::ApplicationProtocols_Delete()
* @brief      Delete all ALPN application protocol capabilities
* @ingroup    DATAIO
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::ApplicationProtocols_Delete()
{
  applicationprotocols.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* DIOSTREAMTLSCONFIG::GetServerName()
* @brief      Get the expected server name and SNI override
* @ingroup    DATAIO
* 
* @return     XSTRING* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOSTREAMTLSCONFIG::GetServerName()
{
  return &servername;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XBUFFER*>* DIOSTREAMTLSCONFIG::GetTrustedRoots()
* @brief      Get the explicitly configured DER trust anchors
* @ingroup    DATAIO
* 
* @return     XVECTOR<XBUFFER*>* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XBUFFER*>* DIOSTREAMTLSCONFIG::GetTrustedRoots()
{
  return &trustedroots;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLSCONFIG::TrustedRoot_Add(XBUFFER& root)
* @brief      Add a copied DER trust anchor
* @ingroup    DATAIO
* 
* @param[in]  root : DER certificate to copy.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::TrustedRoot_Add(XBUFFER& root)
{
  XBUFFER* copy;

  if(root.IsEmpty()) return false;

  copy = GEN_NEW XBUFFER();
  if(!copy) return false;

  if(!copy->Add(root) || !trustedroots.Add(copy))
    {
      GEN_DELETE copy;
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::TrustedRoots_Load(CIPHERTRUSTPROVIDERX509& provider)
* @brief      Replace the configured trust anchors with the roots supplied by a GEN trust provider
* @ingroup    DATAIO
*
* @param[in]  provider : Trust provider to load.
*
* @return     bool : true if at least one trust anchor is loaded; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::TrustedRoots_Load(CIPHERTRUSTPROVIDERX509& provider)
{
  XVECTOR<XBUFFER*>* roots;

  if(!provider.Load()) return false;

  roots = provider.GetRoots();
  if(!roots || roots->IsEmpty()) return false;

  TrustedRoots_Delete();

  for(XDWORD c=0; c<roots->GetSize(); c++)
    {
      XBUFFER* root = roots->Get(c);

      if(root && !TrustedRoot_Add((*root)))
        {
          TrustedRoots_Delete();
          return false;
        }
    }

  return !trustedroots.IsEmpty();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::TrustedRoots_AddDefaults()
* @brief      Add the trust anchors supplied by the embedded GEN trust provider
* @ingroup    DATAIO
*
* @return     bool : true if at least one trust anchor is added; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::TrustedRoots_AddDefaults()
{
  CIPHERTRUSTPROVIDERX509GEN provider;

  if(!trustedroots.IsEmpty()) return false;

  return TrustedRoots_Load(provider);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLSCONFIG::TrustedRoots_Delete()
* @brief      Delete all copied trust anchors
* @ingroup    DATAIO
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::TrustedRoots_Delete()
{
  trustedroots.DeleteContents();
  trustedroots.DeleteAll();

  return true;
}


DIOSTREAMTLS_CLIENTAUTHENTICATION_MODE DIOSTREAMTLSCONFIG::GetClientAuthenticationMode()
{
  return clientauthenticationmode;
}


void DIOSTREAMTLSCONFIG::SetClientAuthenticationMode(DIOSTREAMTLS_CLIENTAUTHENTICATION_MODE mode)
{
  clientauthenticationmode = mode;
}


XVECTOR<XBUFFER*>* DIOSTREAMTLSCONFIG::GetClientTrustedRoots()
{
  return &clienttrustedroots;
}


bool DIOSTREAMTLSCONFIG::ClientTrustedRoot_Add(XBUFFER& root)
{
  XBUFFER* copy;

  if(root.IsEmpty()) return false;

  copy = GEN_NEW XBUFFER();
  if(!copy) return false;

  if(!copy->Add(root) || !clienttrustedroots.Add(copy))
    {
      GEN_DELETE copy;
      return false;
    }

  return true;
}


bool DIOSTREAMTLSCONFIG::ClientTrustedRoots_Load(CIPHERTRUSTPROVIDERX509& provider)
{
  XVECTOR<XBUFFER*>* roots;

  if(!provider.Load()) return false;

  roots = provider.GetRoots();
  if(!roots || roots->IsEmpty()) return false;

  ClientTrustedRoots_Delete();

  for(XDWORD c=0; c<roots->GetSize(); c++)
    {
      XBUFFER* root = roots->Get(c);
      if(root && !ClientTrustedRoot_Add((*root)))
        {
          ClientTrustedRoots_Delete();
          return false;
        }
    }

  return !clienttrustedroots.IsEmpty();
}


bool DIOSTREAMTLSCONFIG::ClientTrustedRoots_Delete()
{
  clienttrustedroots.DeleteContents();
  clienttrustedroots.DeleteAll();
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<XBUFFER*>* DIOSTREAMTLSCONFIG::GetLocalCertificateChain()
* @brief      Get the copied DER certificate chain of the local end
* @ingroup    DATAIO
*
* @return     XVECTOR<XBUFFER*>* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XBUFFER*>* DIOSTREAMTLSCONFIG::GetLocalCertificateChain()
{
  return &localcertificatechain;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::LocalCertificate_Add(XBUFFER& certificate)
* @brief      Add a copied DER certificate to the local chain
* @ingroup    DATAIO
*
* @param[in]  certificate : DER certificate to copy. The leaf certificate must be added first.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::LocalCertificate_Add(XBUFFER& certificate)
{
  XBUFFER* copy;

  if(certificate.IsEmpty()) return false;

  copy = GEN_NEW XBUFFER();
  if(!copy) return false;

  if(!copy->Add(certificate) || !localcertificatechain.Add(copy))
    {
      GEN_DELETE copy;
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::LocalCertificates_Delete()
* @brief      Delete all copied certificates of the local chain
* @ingroup    DATAIO
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::LocalCertificates_Delete()
{
  localcertificatechain.DeleteContents();
  localcertificatechain.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         CIPHERKEY* DIOSTREAMTLSCONFIG::GetLocalPrivateKey()
* @brief      Get the copied private key of the local end
* @ingroup    DATAIO
*
* @return     CIPHERKEY* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERKEY* DIOSTREAMTLSCONFIG::GetLocalPrivateKey()
{
  return localprivatekey;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::SetLocalPrivateKey(CIPHERKEY* privatekey)
* @brief      Copy the private key of the local end
* @note       Accepts either an RSA private key or an ECDSA private key for one of the curves CIPHERECDSA
*             implements (P-256/P-384/P-521 -- see CIPHERECDSA::Parameters_Set()); any other key type is
*             rejected. The matching CertificateSignatureScheme_Add()/SignatureScheme_Add() calls (see
*             DoDefault() below) still need to be picked to match whichever key ends up here -- callers building
*             a server profile from a loaded key, such as APPFLOWWEBSERVER::Ini_BuildTLSConfig(), should offer
*             only the scheme(s) that pair with this key's type.
* @ingroup    DATAIO
*
* @param[in]  privatekey : Private key to copy.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::SetLocalPrivateKey(CIPHERKEY* privatekey)
{
  if(!privatekey || !privatekey->GetSizeInBytes()) return false;

  switch(privatekey->GetType())
    {
      case CIPHERKEYTYPE_RSA_PRIVATE :
        {
          CIPHERKEYPRIVATERSA* copy;

          copy = GEN_NEW CIPHERKEYPRIVATERSA();
          if(!copy) return false;

          if(!copy->CopyFrom((CIPHERKEYPRIVATERSA*)privatekey))
            {
              GEN_DELETE copy;
              return false;
            }

          if(localprivatekey) GEN_DELETE localprivatekey;

          localprivatekey = copy;
        }
        return true;

      case CIPHERKEYTYPE_ECDSA_SECP256R1_PRIVATE :
      case CIPHERKEYTYPE_ECDSA_SECP384R1_PRIVATE :
      case CIPHERKEYTYPE_ECDSA_SECP521R1_PRIVATE :
        {
          CIPHERKEYECDSA* copy;

          copy = GEN_NEW CIPHERKEYECDSA();
          if(!copy) return false;

          if(!copy->CopyFrom((CIPHERKEYSYMMETRICAL*)privatekey))
            {
              GEN_DELETE copy;
              return false;
            }

          if(localprivatekey) GEN_DELETE localprivatekey;

          localprivatekey = copy;
        }
        return true;

                                        default : break;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::HasLocalCredentials()
* @brief      Check whether the local certificate chain and private key are configured
* @ingroup    DATAIO
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::HasLocalCredentials()
{
  if(localcertificatechain.IsEmpty() || !localprivatekey) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::LocalCredentials_Validate()
* @brief      Validate the local TLS certificate chain and private key before using them in server mode
* @ingroup    DATAIO
*
* @return     bool : true if the local credentials are coherent and usable; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::LocalCredentials_Validate()
{
  localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_NONE;

  if(!Credentials_Validate(&localcertificatechain, localprivatekey)) return false;

  for(XDWORD c=0; c<servercredentials.GetSize(); c++)
    {
      DIOSTREAMTLSSERVERCREDENTIALS* credentials = servercredentials.Get(c);

      if(!credentials || credentials->GetServerName()->IsEmpty() ||
         !Credentials_Validate(credentials->GetCertificateChain(), credentials->GetPrivateKey()))
        {
          if(localcredentialserror == DIOSTREAMTLS_LOCALCREDENTIALSERROR_NONE)
            {
              localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_NOTCONFIGURED;
            }

          return false;
        }

      XBUFFER* certificateDER = credentials->GetCertificateChain()->Get(0);
      CIPHERCERTIFICATEX509 certificate;
      XSTRING servernametest((*credentials->GetServerName()));
      bool    servernamematch = false;

      if(certificateDER && certificate.Decode((*certificateDER)))
        {
          if((servernametest.GetSize() >= 2) && (servernametest[0] == __C('*')) && (servernametest[1] == __C('.')))
            {
              for(XDWORD d=0; d<certificate.GetSubjectAlternativeNamesDNS()->GetSize(); d++)
                {
                  XSTRING* subjectname = certificate.GetSubjectAlternativeNamesDNS()->Get(d);
                  if(!subjectname) continue;

                  XSTRING normalized((*subjectname));
                  if(normalized.Character_GetLast() == __C('.')) normalized.DeleteLastCharacter();

                  if(!normalized.Compare(servernametest, true))
                    {
                      servernamematch = true;
                      break;
                    }
                }
            }
           else
            {
              servernamematch = certificate.IsServerNameValid(servernametest.Get());
            }
        }

      if(!servernamematch)
        {
          localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDSERVERNAME;
          return false;
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::Credentials_Validate(XVECTOR<XBUFFER*>* certificatechain, CIPHERKEY* privatekey)
* @brief      Validate one server certificate chain/private key pair
* @note       INTERNAL
* @ingroup    DATAIO
*
* @param[in]  certificatechain : Certificate chain to validate.
* @param[in]  privatekey : Private key associated with the leaf certificate.
*
* @return     bool : true if the credentials are coherent and usable; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::Credentials_Validate(XVECTOR<XBUFFER*>* certificatechain, CIPHERKEY* privatekey)
{
  XVECTOR<CIPHERCERTIFICATEX509*> certificates;
  XBUFFER                         content;
  XBUFFER                         signature;

  if(!certificatechain || certificatechain->IsEmpty() || !privatekey)
    {
      localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_NOTCONFIGURED;
      return false;
    }

  for(XDWORD c=0; c<certificatechain->GetSize(); c++)
    {
      XBUFFER*               certificateDER = certificatechain->Get(c);
      CIPHERCERTIFICATEX509* certificate;

      if(!certificateDER || certificateDER->IsEmpty())
        {
          localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDCERTIFICATE;
          certificates.DeleteContents();
          certificates.DeleteAll();
          return false;
        }

      certificate = GEN_NEW CIPHERCERTIFICATEX509();
      if(!certificate || !certificate->Decode((*certificateDER)))
        {
          if(certificate) GEN_DELETE certificate;
          localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDCERTIFICATE;
          certificates.DeleteContents();
          certificates.DeleteAll();
          return false;
        }

      if(!certificates.Add(certificate))
        {
          GEN_DELETE certificate;
          localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDCERTIFICATE;
          certificates.DeleteContents();
          certificates.DeleteAll();
          return false;
        }
    }

  for(XDWORD c=0; c<certificates.GetSize(); c++)
    {
      CIPHERCERTIFICATEX509* certificate = certificates.Get(c);

      if(!certificate || certificate->HasUnknownCriticalExtension())
        {
          localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDCERTIFICATE;
          certificates.DeleteContents();
          certificates.DeleteAll();
          return false;
        }

      if(!certificate->IsValidDates())
        {
          localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDDATE;
          certificates.DeleteContents();
          certificates.DeleteAll();
          return false;
        }

      if(!c)
        {
          if(!certificate->IsPublicCipherKeyValid())
            {
              localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDCERTIFICATE;
              certificates.DeleteContents();
              certificates.DeleteAll();
              return false;
            }

          if(certificate->IsCertificateAuthority() ||
             (certificate->HasKeyUsage() && !certificate->IsKeyUsageDigitalSignature()) ||
             (certificate->HasExtendedKeyUsage() && !certificate->IsExtendedKeyUsageServerAuthentication()))
            {
              localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDKEYUSAGE;
              certificates.DeleteContents();
              certificates.DeleteAll();
              return false;
            }
        }
       else
        {
          if(!certificate->IsCertificateAuthority() ||
             (certificate->HasKeyUsage() && !certificate->IsKeyUsageCertificateSign()))
            {
              localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDKEYUSAGE;
              certificates.DeleteContents();
              certificates.DeleteAll();
              return false;
            }

          if((certificate->GetBasicConstraintsPathLength() >= 0) &&
             ((int)c - 1 > certificate->GetBasicConstraintsPathLength()))
            {
              localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDCHAIN;
              certificates.DeleteContents();
              certificates.DeleteAll();
              return false;
            }
        }
    }

  for(XDWORD c=0; c+1<certificates.GetSize(); c++)
    {
      CIPHERCERTIFICATEX509* certificate = certificates.Get(c);
      CIPHERCERTIFICATEX509* issuer      = certificates.Get(c+1);

      if(!certificate->GetIssuerData()->Compare((*issuer->GetSubjectData())) ||
         !certificate->VerifySignature(issuer->GetPublicCipherKey()))
        {
          localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDCHAIN;
          certificates.DeleteContents();
          certificates.DeleteAll();
          return false;
        }
    }

  CIPHERCERTIFICATEX509* leaf      = certificates.Get(0);
  CIPHERKEY*             publickey = leaf?leaf->GetPublicCipherKey():NULL;

  if(!publickey)
    {
      localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_UNSUPPORTEDKEY;
      certificates.DeleteContents();
      certificates.DeleteAll();
      return false;
    }

  static const XBYTE testcontent[] = { 0x47, 0x45, 0x4E, 0x2D, 0x54, 0x4C, 0x53, 0x2D,
                                       0x43, 0x52, 0x45, 0x44, 0x45, 0x4E, 0x54, 0x49,
                                       0x41, 0x4C, 0x53 };

  if(!content.Add((XBYTE*)testcontent, sizeof(testcontent)))
    {
      localcredentialserror = DIOSTREAMTLS_LOCALCREDENTIALSERROR_KEYMISMATCH;
      certificates.DeleteContents();
      certificates.DeleteAll();
      return false;
    }

  bool havesupportedscheme = false;
  bool keypairvalid        = false;

  for(XDWORD c=0; c<signatureschemes.GetSize(); c++)
    {
      XWORD signaturescheme = signatureschemes.Get(c);

      if(!DIOSTREAMTLSSIGNATURE::IsSupported(signaturescheme, publickey)) continue;

      havesupportedscheme = true;
      signature.Delete();

      if(DIOSTREAMTLSSIGNATURE::Sign(signaturescheme, privatekey, publickey, content, signature) &&
         DIOSTREAMTLSSIGNATURE::Verify(signaturescheme, publickey, content, signature))
        {
          keypairvalid = true;
          break;
        }
    }

  if(!havesupportedscheme || !keypairvalid)
    {
      localcredentialserror = havesupportedscheme?DIOSTREAMTLS_LOCALCREDENTIALSERROR_KEYMISMATCH:
                                                 DIOSTREAMTLS_LOCALCREDENTIALSERROR_UNSUPPORTEDKEY;
      certificates.DeleteContents();
      certificates.DeleteAll();
      return false;
    }

  certificates.DeleteContents();
  certificates.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS_LOCALCREDENTIALSERROR DIOSTREAMTLSCONFIG::GetLocalCredentialsError()
* @brief      Get the result of the last local credential validation
* @ingroup    DATAIO
*
* @return     DIOSTREAMTLS_LOCALCREDENTIALSERROR : Last validation result.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_LOCALCREDENTIALSERROR DIOSTREAMTLSCONFIG::GetLocalCredentialsError()
{
  return localcredentialserror;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::LocalCredentials_Delete()
* @brief      Delete the copied certificate chain and private key of the local end
* @ingroup    DATAIO
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::LocalCredentials_Delete()
{
  LocalCertificates_Delete();

  if(localprivatekey)
    {
      GEN_DELETE localprivatekey;
      localprivatekey = NULL;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLSSERVERCREDENTIALS* DIOSTREAMTLSCONFIG::ServerCredentials_Add(XCHAR* servername)
* @brief      Add a server credential set selected by SNI
* @ingroup    DATAIO
*
* @param[in]  servername : Exact DNS name or single-label wildcard pattern.
*
* @return     DIOSTREAMTLSSERVERCREDENTIALS* : New credential set; NULL on error.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSSERVERCREDENTIALS* DIOSTREAMTLSCONFIG::ServerCredentials_Add(XCHAR* servername)
{
  DIOSTREAMTLSSERVERCREDENTIALS* credentials;
  XSTRING                         pattern;

  if(!servername || !servername[0]) return NULL;

  pattern = servername;
  if(pattern.Character_GetLast() == __C('.')) pattern.DeleteLastCharacter();
  if(pattern.IsEmpty()) return NULL;

  if(pattern.FindCharacter(__C('*')) >= 0)
    {
      if((pattern.GetSize() < 3) || (pattern[0] != __C('*')) || (pattern[1] != __C('.')) ||
         (pattern.FindCharacter(__C('*'), 1) >= 0)) return NULL;
    }

  for(XDWORD c=0; c<servercredentials.GetSize(); c++)
    {
      DIOSTREAMTLSSERVERCREDENTIALS* existing = servercredentials.Get(c);

      if(existing && !existing->GetServerName()->Compare(pattern, true)) return NULL;
    }

  credentials = GEN_NEW DIOSTREAMTLSSERVERCREDENTIALS();
  if(!credentials) return NULL;

  credentials->GetServerName()->Set(pattern);

  if(!servercredentials.Add(credentials))
    {
      GEN_DELETE credentials;
      return NULL;
    }

  return credentials;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<DIOSTREAMTLSSERVERCREDENTIALS*>* DIOSTREAMTLSCONFIG::GetServerCredentials()
* @brief      Get all SNI-specific server credentials
* @ingroup    DATAIO
*
* @return     XVECTOR<DIOSTREAMTLSSERVERCREDENTIALS*>* : Credential list.
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<DIOSTREAMTLSSERVERCREDENTIALS*>* DIOSTREAMTLSCONFIG::GetServerCredentials()
{
  return &servercredentials;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::ServerCredentials_Select(XCHAR* servername, XVECTOR<XBUFFER*>*& certificatechain, CIPHERKEY*& privatekey)
* @brief      Select server credentials for an SNI hostname, falling back to the default credentials
* @ingroup    DATAIO
*
* @param[in]  servername : Requested SNI DNS name; NULL or empty when SNI is absent.
* @param[out] certificatechain : Selected certificate chain.
* @param[out] privatekey : Selected private key.
*
* @return     bool : true if usable credentials are available; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::ServerCredentials_Select(XCHAR* servername, XVECTOR<XBUFFER*>*& certificatechain, CIPHERKEY*& privatekey)
{
  certificatechain = NULL;
  privatekey        = NULL;

  if(servername && servername[0])
    {
      // Exact names always win over wildcard entries regardless of insertion order.
      for(XDWORD c=0; c<servercredentials.GetSize(); c++)
        {
          DIOSTREAMTLSSERVERCREDENTIALS* credentials = servercredentials.Get(c);
          if(!credentials || !credentials->HasCredentials()) continue;

          XSTRING pattern((*credentials->GetServerName()));
          if(pattern.FindCharacter(__C('*')) >= 0) continue;

          if(DIOSTREAMTLSCONFIG_ServerNameMatch(pattern, servername))
            {
              certificatechain = credentials->GetCertificateChain();
              privatekey        = credentials->GetPrivateKey();
              return true;
            }
        }

      for(XDWORD c=0; c<servercredentials.GetSize(); c++)
        {
          DIOSTREAMTLSSERVERCREDENTIALS* credentials = servercredentials.Get(c);
          if(!credentials || !credentials->HasCredentials()) continue;

          XSTRING pattern((*credentials->GetServerName()));
          if(pattern.FindCharacter(__C('*')) < 0) continue;

          if(DIOSTREAMTLSCONFIG_ServerNameMatch(pattern, servername))
            {
              certificatechain = credentials->GetCertificateChain();
              privatekey        = credentials->GetPrivateKey();
              return true;
            }
        }
    }

  if(!HasLocalCredentials()) return false;

  certificatechain = &localcertificatechain;
  privatekey        = localprivatekey;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::ServerCredentials_Delete()
* @brief      Delete all SNI-specific server credentials
* @ingroup    DATAIO
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::ServerCredentials_Delete()
{
  servercredentials.DeleteContents();
  servercredentials.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLSCONFIG::IsAllowUnauthenticatedServer()
* @brief      Check whether the explicit test-only unauthenticated mode is enabled
* @ingroup    DATAIO
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::IsAllowUnauthenticatedServer()
{
  return allowunauthenticatedserver;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLSCONFIG::SetAllowUnauthenticatedServer(bool allowunauthenticatedserver)
* @brief      Set the explicit test-only unauthenticated mode
* @ingroup    DATAIO
* 
* @param[in]  allowunauthenticatedserver : Test-only permission value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLSCONFIG::SetAllowUnauthenticatedServer(bool allowunauthenticatedserver)
{
  this->allowunauthenticatedserver = allowunauthenticatedserver;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::IsActiveAIAFetch()
* @brief      Check whether a missing-intermediate chain may be completed via AuthorityInfoAccess fetching
* @ingroup    DATAIO
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::IsActiveAIAFetch()
{
  return aiafetchactive;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLSCONFIG::AIAFetch_Activate(bool activate)
* @brief      Activate or deactivate AuthorityInfoAccess fetching
* @ingroup    DATAIO
*
* @param[in]  activate : true to activate; false to deactivate.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLSCONFIG::AIAFetch_Activate(bool activate)
{
  aiafetchactive = activate;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOSTREAMTLSCONFIG::GetAIAFetchTimeout()
* @brief      Get the AuthorityInfoAccess fetch timeout, in seconds
* @ingroup    DATAIO
*
* @return     int : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMTLSCONFIG::GetAIAFetchTimeout()
{
  return aiafetchtimeout;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::SetAIAFetchTimeout(int timeout)
* @brief      Set the AuthorityInfoAccess fetch timeout, in seconds
* @ingroup    DATAIO
*
* @param[in]  timeout : Timeout value, in seconds. Must be greater than zero.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::SetAIAFetchTimeout(int timeout)
{
  if(timeout <= 0) return false;

  aiafetchtimeout = timeout;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD DIOSTREAMTLSCONFIG::GetMinVersion()
* @brief      Get the minimum TLS version configured for this endpoint
* @ingroup    DATAIO
*
* @return     XWORD : DIOSTREAMTLS_MSG_VERSION_TLS_1_2 or DIOSTREAMTLS_MSG_VERSION_TLS_1_3.
*
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLSCONFIG::GetMinVersion()
{
  return minversion;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::SetMinVersion(XWORD version)
* @brief      Set the minimum TLS version configured for this endpoint
* @note       The current server-side DIOSTREAMTLS implementation supports TLS 1.3 only; a server configured
*             with any other version window is rejected by DIOSTREAMTLS::Open() and DIOWEBSERVER::Ini().
* @note       A TLS 1.2 fallback (minversion == TLS_1_2, maxversion == TLS_1_3) does not add anything to
*             GetCipherSuites(): DIOSTREAMTLS<T> tries TLS 1.3 first with that list completely unchanged, and
*             only on a handshake-stage failure retries the whole connection using the parallel
*             DIOSTREAMTLS12HANDSHAKECLIENT, whose own two ECDHE-RSA-GCM suites are hardcoded and unrelated to
*             this list.
* @ingroup    DATAIO
*
* @param[in]  version : DIOSTREAMTLS_MSG_VERSION_TLS_1_2 or DIOSTREAMTLS_MSG_VERSION_TLS_1_3.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::SetMinVersion(XWORD version)
{
  if((version != DIOSTREAMTLS_MSG_VERSION_TLS_1_2) && (version != DIOSTREAMTLS_MSG_VERSION_TLS_1_3)) return false;
  if(version > maxversion) return false;

  minversion = version;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD DIOSTREAMTLSCONFIG::GetMaxVersion()
* @brief      Get the maximum TLS version configured for this endpoint
* @ingroup    DATAIO
*
* @return     XWORD : DIOSTREAMTLS_MSG_VERSION_TLS_1_2 or DIOSTREAMTLS_MSG_VERSION_TLS_1_3.
*
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLSCONFIG::GetMaxVersion()
{
  return maxversion;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSCONFIG::SetMaxVersion(XWORD version)
* @brief      Set the maximum TLS version configured for this endpoint
* @note       The current server-side DIOSTREAMTLS implementation supports TLS 1.3 only.
* @ingroup    DATAIO
*
* @param[in]  version : DIOSTREAMTLS_MSG_VERSION_TLS_1_2 or DIOSTREAMTLS_MSG_VERSION_TLS_1_3.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::SetMaxVersion(XWORD version)
{
  if((version != DIOSTREAMTLS_MSG_VERSION_TLS_1_2) && (version != DIOSTREAMTLS_MSG_VERSION_TLS_1_3)) return false;
  if(version < minversion) return false;

  maxversion = version;

  return true;
}



bool DIOSTREAMTLSCONFIG::IsSessionResumptionActive()
{
  return sessionresumptionactive;
}


void DIOSTREAMTLSCONFIG::SessionResumption_Activate(bool active)
{
  sessionresumptionactive = active;

  if(!active)
    {
      SessionTickets_Delete();
      if(!sessionticketserverkey.IsEmpty()) sessionticketserverkey.FillBuffer(0);
      sessionticketserverkey.Delete();
    }
}


XDWORD DIOSTREAMTLSCONFIG::GetSessionTicketLifetime()
{
  return sessionticketlifetime;
}


bool DIOSTREAMTLSCONFIG::SetSessionTicketLifetime(XDWORD lifetime)
{
  if(!lifetime || (lifetime > DIOSTREAMTLS13_SESSIONTICKET_MAX_LIFETIME)) return false;
  sessionticketlifetime = lifetime;
  return true;
}


bool DIOSTREAMTLSCONFIG::SessionResumption_ServerInitialize()
{
  if(!sessionresumptionactive) return true;
  if(sessionticketserverkey.GetSize() == 32) return true;

  XRAND* random = GEN_XFACTORY.CreateRand();
  if(!random) return false;

  XBYTE key[32];
  bool status = random->Ini();
  if(status) status = random->Generate(key, sizeof(key));
  GEN_XFACTORY.DeleteRand(random);

  if(status)
    {
      sessionticketserverkey.Delete();
      status = sessionticketserverkey.Add(key, sizeof(key));
    }

  volatile XBYTE* clean = key;
  for(XDWORD c=0; c<sizeof(key); c++) clean[c] = 0;
  return status;
}


bool DIOSTREAMTLSCONFIG::SessionTicket_Store(XCHAR* servername, XBUFFER& ticket, XBUFFER& PSK, XDWORD ageadd,
                                              XDWORD lifetime, XWORD ciphersuite,
                                              DIOSTREAMTLS_ALPN_TYPE applicationprotocol)
{
  if(!sessionresumptionactive || !servername || !servername[0] || ticket.IsEmpty() || PSK.IsEmpty() ||
     !lifetime || (lifetime > DIOSTREAMTLS13_SESSIONTICKET_MAX_LIFETIME)) return false;

  for(int c=(int)sessiontickets.GetSize()-1; c>=0; c--)
    {
      DIOSTREAMTLS13SESSIONTICKET* current = sessiontickets.Get(c);
      if(!current || current->IsExpired() || !current->GetServerName()->Compare(servername, true))
        {
          sessiontickets.Delete(current);
          if(current) GEN_DELETE current;
        }
    }

  while(sessiontickets.GetSize() >= DIOSTREAMTLS13_SESSIONTICKET_MAX_CACHED)
    {
      DIOSTREAMTLS13SESSIONTICKET* oldest = sessiontickets.Get(0);
      sessiontickets.Delete(oldest);
      if(oldest) GEN_DELETE oldest;
    }

  DIOSTREAMTLS13SESSIONTICKET* stored = GEN_NEW DIOSTREAMTLS13SESSIONTICKET();
  if(!stored) return false;

  stored->GetServerName()->Set(servername);
  stored->SetTicketAgeAdd(ageadd);
  stored->SetLifetime(lifetime);
  stored->SetReceivedEpoch(DIOSTREAMTLSCONFIG_CurrentEpoch());
  stored->SetCipherSuite(ciphersuite);
  stored->SetApplicationProtocol(applicationprotocol);

  if(!stored->GetTicket()->Add(ticket) || !stored->GetPSK()->Add(PSK) || !stored->GetReceivedEpoch() ||
     !sessiontickets.Add(stored))
    {
      GEN_DELETE stored;
      return false;
    }

  return true;
}


DIOSTREAMTLS13SESSIONTICKET* DIOSTREAMTLSCONFIG::SessionTicket_Get(XCHAR* servername)
{
  if(!sessionresumptionactive || !servername || !servername[0]) return NULL;

  for(int c=(int)sessiontickets.GetSize()-1; c>=0; c--)
    {
      DIOSTREAMTLS13SESSIONTICKET* ticket = sessiontickets.Get(c);
      if(!ticket) continue;

      if(ticket->IsExpired())
        {
          sessiontickets.Delete(ticket);
          GEN_DELETE ticket;
          continue;
        }

      if(!ticket->GetServerName()->Compare(servername, true)) return ticket;
    }

  return NULL;
}


bool DIOSTREAMTLSCONFIG::SessionTickets_Delete()
{
  sessiontickets.DeleteContents();
  sessiontickets.DeleteAll();
  return true;
}


bool DIOSTREAMTLSCONFIG::SessionTicket_Seal(XBUFFER& PSK, XWORD ciphersuite,
                                             DIOSTREAMTLS_ALPN_TYPE applicationprotocol, XCHAR* servername,
                                             XDWORD lifetime, XDWORD ageadd, XBUFFER& ticket)
{
  if(!sessionresumptionactive || PSK.IsEmpty() || !lifetime ||
     (lifetime > DIOSTREAMTLS13_SESSIONTICKET_MAX_LIFETIME) ||
     !SessionResumption_ServerInitialize()) return false;

  XBUFFER plain;
  XBUFFER nonce;
  XBUFFER aad;
  XBUFFER tag;
  CIPHERKEYSYMMETRICAL key;
  CIPHERAESGCM cipher;
  XRAND* random = GEN_XFACTORY.CreateRand();
  XQWORD issueepoch = DIOSTREAMTLSCONFIG_CurrentEpoch();
  XBYTE namelength = 0;

  if(!issueepoch || PSK.GetSize() > 255) return false;
  if(servername)
    {
      XDWORD length = 0;
      while(servername[length])
        {
          if((XDWORD)servername[length] > 0x7F || length >= 255) return false;
          length++;
        }
      namelength = (XBYTE)length;
    }

  if(!random) return false;
  if(!nonce.Resize(CIPHERAESGCM_NONCESIZE)) { GEN_XFACTORY.DeleteRand(random); return false; }
  bool status = random->Ini() && random->Generate(nonce.Get(), nonce.GetSize());
  GEN_XFACTORY.DeleteRand(random);
  if(!status) return false;

  if(!plain.Add((XBYTE)1) || !plain.Add(issueepoch) || !plain.Add(lifetime) || !plain.Add(ageadd) ||
     !plain.Add(ciphersuite) || !plain.Add((XWORD)applicationprotocol) || !plain.Add((XBYTE)PSK.GetSize()) ||
     !plain.Add(PSK) || !plain.Add(namelength)) return false;

  for(XDWORD c=0; c<namelength; c++) if(!plain.Add((XBYTE)servername[c])) return false;

  static XBYTE aadbytes[] = { 'G','E','N','-','T','L','S','1','3','-','T','I','C','K','E','T','-','1' };
  if(!aad.Add(aadbytes, sizeof(aadbytes)) || !key.Set(sessionticketserverkey) || !cipher.SetKey(&key) ||
     !cipher.CipherAEAD(plain.Get(), plain.GetSize(), nonce, aad, tag) || !cipher.GetResult()) return false;

  ticket.Delete();
  if(!ticket.Add(nonce) || !ticket.Add((*cipher.GetResult())) || !ticket.Add(tag)) return false;
  return true;
}


bool DIOSTREAMTLSCONFIG::SessionTicket_Open(XBUFFER& ticket, XBUFFER& PSK, XWORD& ciphersuite,
                                             DIOSTREAMTLS_ALPN_TYPE& applicationprotocol, XSTRING& servername,
                                             XQWORD& issueepoch, XDWORD& lifetime, XDWORD& ageadd)
{
  if(!sessionresumptionactive || sessionticketserverkey.GetSize() != 32 ||
     ticket.GetSize() <= (CIPHERAESGCM_NONCESIZE + CIPHERAESGCM_TAGSIZE)) return false;

  XBUFFER nonce;
  XBUFFER ciphertext;
  XBUFFER tag;
  XBUFFER aad;
  CIPHERKEYSYMMETRICAL key;
  CIPHERAESGCM cipher;
  XDWORD ciphertextsize = ticket.GetSize() - CIPHERAESGCM_NONCESIZE - CIPHERAESGCM_TAGSIZE;

  if(!nonce.Add(ticket.Get(), CIPHERAESGCM_NONCESIZE) ||
     !ciphertext.Add(ticket.Get() + CIPHERAESGCM_NONCESIZE, ciphertextsize) ||
     !tag.Add(ticket.Get() + CIPHERAESGCM_NONCESIZE + ciphertextsize, CIPHERAESGCM_TAGSIZE)) return false;

  static XBYTE aadbytes[] = { 'G','E','N','-','T','L','S','1','3','-','T','I','C','K','E','T','-','1' };
  if(!aad.Add(aadbytes, sizeof(aadbytes)) || !key.Set(sessionticketserverkey) || !cipher.SetKey(&key) ||
     !cipher.UncipherAEAD(ciphertext.Get(), ciphertext.GetSize(), nonce, aad, tag) || !cipher.GetResult()) return false;

  XBUFFER plain;
  if(!plain.Add((*cipher.GetResult()))) return false;

  XBYTE version = 0;
  XWORD protocol = 0;
  XBYTE psklength = 0;
  XBYTE namelength = 0;

  if(!plain.Extract(version) || version != 1 || !plain.Extract(issueepoch) || !plain.Extract(lifetime) ||
     !plain.Extract(ageadd) || !plain.Extract(ciphersuite) || !plain.Extract(protocol) ||
     !plain.Extract(psklength) || !psklength || plain.GetSize() < ((XDWORD)psklength + 1)) return false;

  PSK.Delete();
  if(!PSK.Resize(psklength) || plain.Extract(PSK.Get(), 0, psklength) != psklength || !plain.Extract(namelength) ||
     plain.GetSize() != namelength) return false;

  servername.Empty();
  for(XDWORD c=0; c<namelength; c++)
    {
      XBYTE character = 0;
      if(!plain.Extract(character)) return false;
      servername.Add((XCHAR)character);
    }

  applicationprotocol = (DIOSTREAMTLS_ALPN_TYPE)protocol;
  XQWORD now = DIOSTREAMTLSCONFIG_CurrentEpoch();
  if(!now || !issueepoch || now < issueepoch || !lifetime ||
     lifetime > DIOSTREAMTLS13_SESSIONTICKET_MAX_LIFETIME || (now - issueepoch) >= lifetime)
    {
      PSK.FillBuffer(0);
      PSK.Delete();
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLSCONFIG::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLSCONFIG::Clean()
{
  // Default negotiation window is TLS 1.3 only, matching every previous phase exactly: nothing changes for an
  // existing caller unless it explicitly calls SetMinVersion(DIOSTREAMTLS_MSG_VERSION_TLS_1_2).
  minversion = DIOSTREAMTLS_MSG_VERSION_TLS_1_3;
  maxversion = DIOSTREAMTLS_MSG_VERSION_TLS_1_3;

  CipherSuites_Delete();
  CipherSuite_Add(DIOSTREAMTLS_MSG_CIPHER_AES_128_GCM_SHA256);
  CipherSuite_Add(DIOSTREAMTLS_MSG_CIPHER_AES_256_GCM_SHA384);

  SupportedGroups_Delete();
  SupportedGroup_Add(DIOSTREAMTLS_MSG_CURVEID_X25519);
  SupportedGroup_Add(DIOSTREAMTLS_MSG_CURVEID_SECP256R1);
  SupportedGroup_Add(DIOSTREAMTLS_MSG_CURVEID_SECP384R1);

  // Default handshake-signing profile. Keep the existing RSA-PSS defaults for backwards compatibility. The trust
  // anchor source is configured independently through CIPHERTRUSTPROVIDERX509, so applications can extend these
  // schemes without coupling that decision to the selected Root CA provider.
  SignatureSchemes_Delete();
  SignatureScheme_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA256);
  SignatureScheme_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA384);
  SignatureScheme_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA512);

  CertificateSignatureSchemes_Delete();
  CertificateSignatureScheme_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA256);
  CertificateSignatureScheme_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA384);
  CertificateSignatureScheme_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA512);
  CertificateSignatureScheme_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PKCS1_SHA256);
  CertificateSignatureScheme_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PKCS1_SHA384);
  CertificateSignatureScheme_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PKCS1_SHA512);
  CertificateSignatureScheme_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP256R1_SHA256);
  CertificateSignatureScheme_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP384R1_SHA384);
  CertificateSignatureScheme_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP521R1_SHA512);

  ApplicationProtocols_Delete();

  servername.Empty();
  TrustedRoots_Delete();
  ClientTrustedRoots_Delete();
  clientauthenticationmode = DIOSTREAMTLS_CLIENTAUTHENTICATION_MODE_NONE;
  LocalCredentials_Delete();
  ServerCredentials_Delete();
  localcredentialserror       = DIOSTREAMTLS_LOCALCREDENTIALSERROR_NONE;
  allowunauthenticatedserver  = false;

  aiafetchactive  = true;
  aiafetchtimeout = 5;

  SessionTickets_Delete();
  if(!sessionticketserverkey.IsEmpty()) sessionticketserverkey.FillBuffer(0);
  sessionticketserverkey.Delete();
  sessionresumptionactive = true;
  sessionticketlifetime   = DIOSTREAMTLS13_SESSIONTICKET_DEFAULT_LIFETIME;
}
