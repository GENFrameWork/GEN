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

#include "CipherCertificateX509.h"
#include "CipherKeyPrivateRSA.h"
#include "CipherKeyECDSA.h"
#include "CipherTrustedRootCertificatesX509.h"

#include "DIOFactory.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


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
* @fn         bool DIOSTREAMTLSCONFIG::TrustedRoots_AddDefaults()
* @brief      Add the supported RSA/ECDSA-P256 trust anchors from the embedded GEN CA bundle
* @ingroup    DATAIO
*
* @return     bool : true if at least one trust anchor is added; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::TrustedRoots_AddDefaults()
{
  CIPHERTRUSTEDROOTCERTIFICATESX509 defaultroots;

  if(!trustedroots.IsEmpty()) return false;
  if(!defaultroots.Certificates_Decode()) return false;

  for(XDWORD c=0; c<defaultroots.Certificates_GetAll()->GetSize(); c++)
    {
      XBUFFER* rootDER = defaultroots.Certificates_GetAll()->Get(c);

      if(rootDER)
        {
          CIPHERCERTIFICATEX509 root;

          // Only load a root as a trust anchor when its key type is one CIPHERCERTIFICATEX509VALIDATOR can actually
          // verify a chain signature against (see VerifySignature()): RSA of any size, or ECDSA on P-256/P-384/P-521.
          // Silently admitting a root whose signature can never be checked would be worse than excluding it.
          if(root.Decode((*rootDER)) && root.IsCertificateAuthority() && root.GetPublicCipherKey() &&
             ((root.GetPublicCipherKey()->GetType() == CIPHERKEYTYPE_RSA_PUBLIC) ||
              (root.GetPublicCipherKey()->GetType() == CIPHERKEYTYPE_ECDSA_SECP256R1_PUBLIC) ||
              (root.GetPublicCipherKey()->GetType() == CIPHERKEYTYPE_ECDSA_SECP384R1_PUBLIC) ||
              (root.GetPublicCipherKey()->GetType() == CIPHERKEYTYPE_ECDSA_SECP521R1_PUBLIC)))
            {
              if(!TrustedRoot_Add((*rootDER)))
                {
                  TrustedRoots_Delete();
                  return false;
                }
            }
        }
    }

  return !trustedroots.IsEmpty();
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

  // Handshake-signing schemes offered in signature_algorithms. RSA-PSS only BY DEFAULT: this is the set that
  // has been in production use, and it is deliberately not widened without evidence, because every entry added
  // here changes which certificate a dual-certificate server decides to send -- confirmed for real against
  // example.com, whose CDN switched to an ECDSA certificate chain the moment ECDSA was offered here, and that
  // specific chain did not validate against the embedded trusted-root bundle (untrusted root), even though the
  // bundle already carries 40+ common ECDSA roots. Root-caused but not yet reproduced with the real chain, so
  // this stays RSA-PSS-only until it is.
  //
  // ECDSA (P-256/P-384/P-521) is verified correctly by DIOSTREAMTLSSIGNATURE and can be enabled per connection:
  //
  //     GetStreamTLSCFG()->SignatureScheme_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP256R1_SHA256);
  //
  // A server holding ONLY an ECDSA certificate aborts the handshake with handshake_failure unless that is done,
  // since it cannot sign CertificateVerify with anything offered here.
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
  LocalCredentials_Delete();
  allowunauthenticatedserver  = false;

  aiafetchactive  = true;
  aiafetchtimeout = 5;
}
