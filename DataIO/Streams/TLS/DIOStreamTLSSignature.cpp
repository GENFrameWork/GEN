/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamTLSSignature.cpp
*
* @class      DIOSTREAMTLSSIGNATURE
* @brief      Data Input/Output Stream TLS Signature class
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

#include "DIOStreamTLSSignature.h"
#include "DIOStreamTLSMessages.h"

#include "CipherKey.h"
#include "CipherRSA.h"
#include "CipherECDSA.h"
#include "HashSHA2.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSIGNATURE::IsSupported(XWORD signaturescheme, CIPHERKEY* key)
* @brief      Check whether a TLS signature scheme is supported for a key
* @ingroup    DATAIO
*
* @param[in]  signaturescheme : TLS signature scheme value.
* @param[in]  key : Cipher key to check.
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSIGNATURE::IsSupported(XWORD signaturescheme, CIPHERKEY* key)
{
  if(!key) return false;

  if((key->GetType() == CIPHERKEYTYPE_ECDSA_SECP256R1_PUBLIC) &&
     (signaturescheme == DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP256R1_SHA256)) return true;

  if((key->GetType() == CIPHERKEYTYPE_ECDSA_SECP384R1_PUBLIC) &&
     (signaturescheme == DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP384R1_SHA384)) return true;

  if((key->GetType() == CIPHERKEYTYPE_ECDSA_SECP521R1_PUBLIC) &&
     (signaturescheme == DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP521R1_SHA512)) return true;

  if(key->GetType() != CIPHERKEYTYPE_RSA_PUBLIC) return false;

  switch(signaturescheme)
    {
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA256 :
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA384 :
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA512 : return true;
                                                        default : break;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSIGNATURE::Verify(XWORD signaturescheme, CIPHERKEY* key, XBUFFER& content, XBUFFER& signature)
* @brief      Verify a TLS signature with the scheme-specific hash and padding parameters
* @ingroup    DATAIO
*
* @param[in]  signaturescheme : TLS signature scheme value.
* @param[in]  key : Public cipher key.
* @param[in]  content : Signed content.
* @param[in]  signature : Signature value.
*
* @return     bool : true if the signature is valid; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSIGNATURE::Verify(XWORD signaturescheme, CIPHERKEY* key, XBUFFER& content, XBUFFER& signature)
{
  if(!IsSupported(signaturescheme, key) || content.IsEmpty() || signature.IsEmpty()) return false;

  if(signaturescheme == DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP256R1_SHA256)
    {
      CIPHERECDSA ECDSA(CIPHERTYPE_ECDSA_SECP256R1);
      HASHSHA2    hash(HASHSHA2TYPE_256);

      return ECDSA.SetKey(key, true) && ECDSA.Verify(content, signature, &hash);
    }

  if(signaturescheme == DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP384R1_SHA384)
    {
      CIPHERECDSA ECDSA(CIPHERTYPE_ECDSA_SECP384R1);
      HASHSHA2    hash(HASHSHA2TYPE_384);

      return ECDSA.SetKey(key, true) && ECDSA.Verify(content, signature, &hash);
    }

  if(signaturescheme == DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP521R1_SHA512)
    {
      CIPHERECDSA ECDSA(CIPHERTYPE_ECDSA_SECP521R1);
      HASHSHA2    hash(HASHSHA2TYPE_512);

      return ECDSA.SetKey(key, true) && ECDSA.Verify(content, signature, &hash);
    }

  CIPHERRSA RSA;
  if(!RSA.SetKey(key, true)) return false;

  switch(signaturescheme)
    {
      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA256 : { HASHSHA2 hash(HASHSHA2TYPE_256);
                                                                    return RSA.Verify(content, signature, &hash,
                                                                                      CIPHERRSAPKCS1VERSIONV21,
                                                                                      HASHSHA2_256_DIGEST_SIZE);
                                                                  }

      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA384 : { HASHSHA2 hash(HASHSHA2TYPE_384);
                                                                    return RSA.Verify(content, signature, &hash,
                                                                                      CIPHERRSAPKCS1VERSIONV21,
                                                                                      HASHSHA2_384_DIGEST_SIZE);
                                                                  }

      case DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA512 : { HASHSHA2 hash(HASHSHA2TYPE_512);
                                                                    return RSA.Verify(content, signature, &hash,
                                                                                      CIPHERRSAPKCS1VERSIONV21,
                                                                                      HASHSHA2_512_DIGEST_SIZE);
                                                                  }

                                                        default : break;
    }

  return false;
}
