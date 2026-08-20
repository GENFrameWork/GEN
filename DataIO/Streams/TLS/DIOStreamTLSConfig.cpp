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
  return ciphersuite;
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
  this->ciphersuite = ciphersuite;
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
* @brief      Add the supported RSA trust anchors from the embedded GEN CA bundle
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

          if(root.Decode((*rootDER)) && root.IsCertificateAuthority() &&
             root.GetPublicCipherKey() && (root.GetPublicCipherKey()->GetType() == CIPHERKEYTYPE_RSA_PUBLIC))
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
* @note       The initial server profile accepts the RSA private key already implemented by GEN.
* @ingroup    DATAIO
*
* @param[in]  privatekey : Private key to copy.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSCONFIG::SetLocalPrivateKey(CIPHERKEY* privatekey)
{
  CIPHERKEYPRIVATERSA* copy;

  if(!privatekey || (privatekey->GetType() != CIPHERKEYTYPE_RSA_PRIVATE) || !privatekey->GetSizeInBytes()) return false;

  copy = GEN_NEW CIPHERKEYPRIVATERSA();
  if(!copy) return false;

  if(!copy->CopyFrom((CIPHERKEYPRIVATERSA*)privatekey))
    {
      GEN_DELETE copy;
      return false;
    }

  if(localprivatekey) GEN_DELETE localprivatekey;

  localprivatekey = copy;

  return true;
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
* @fn         void DIOSTREAMTLSCONFIG::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLSCONFIG::Clean()
{
  ciphersuite                 = DIOSTREAMTLS_MSG_CIPHER_AES_128_GCM_SHA256;
  servername.Empty();
  TrustedRoots_Delete();
  LocalCredentials_Delete();
  allowunauthenticatedserver  = false;
}
