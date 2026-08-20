/**-------------------------------------------------------------------------------------------------------------------
*
* @file       CipherCertificateX509Validator.h
*
* @class      CIPHERCERTIFICATEX509VALIDATOR
* @brief      Cipher Certificate X.509 validation class
* @ingroup    CIPHER
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

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XVector.h"

#include "CipherCertificateX509.h"



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


enum CIPHERCERTIFICATEX509VALIDATOR_ERROR
{
  CIPHERCERTIFICATEX509VALIDATOR_ERROR_NONE                         = 0 ,
  CIPHERCERTIFICATEX509VALIDATOR_ERROR_INVALIDPARAMETER                 ,
  CIPHERCERTIFICATEX509VALIDATOR_ERROR_INVALIDCERTIFICATE               ,
  CIPHERCERTIFICATEX509VALIDATOR_ERROR_UNSUPPORTEDALGORITHM             ,
  CIPHERCERTIFICATEX509VALIDATOR_ERROR_INVALIDDATE                      ,
  CIPHERCERTIFICATEX509VALIDATOR_ERROR_INVALIDNAME                      ,
  CIPHERCERTIFICATEX509VALIDATOR_ERROR_INVALIDKEYUSAGE                  ,
  CIPHERCERTIFICATEX509VALIDATOR_ERROR_INVALIDCA                        ,
  CIPHERCERTIFICATEX509VALIDATOR_ERROR_INVALIDSIGNATURE                 ,
  CIPHERCERTIFICATEX509VALIDATOR_ERROR_UNTRUSTEDROOT                    ,
  CIPHERCERTIFICATEX509VALIDATOR_ERROR_UNKNOWNCRITICALEXTENSION         ,
  CIPHERCERTIFICATEX509VALIDATOR_ERROR_PATHLENGTH                       ,
};



/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class CIPHERCERTIFICATEX509VALIDATOR
{
  public:
                                            CIPHERCERTIFICATEX509VALIDATOR          ();
    virtual                                ~CIPHERCERTIFICATEX509VALIDATOR          ();

    bool                                    Validate                                 (XVECTOR<XBUFFER*>* certificatechain, XVECTOR<XBUFFER*>* trustedroots, XCHAR* servername, XDATETIME* datetime = NULL);

    CIPHERCERTIFICATEX509VALIDATOR_ERROR    GetError                                 ();
    CIPHERCERTIFICATEX509*                  GetLeafCertificate                       ();
    XVECTOR<CIPHERCERTIFICATEX509*>*        GetCertificateChain                      ();

    void                                    End                                      ();

  private:

    bool                                    IsSignatureAlgorithmSupported             (CIPHERCERTIFICATEX509* certificate);
    bool                                    IsSamePublicKey                           (CIPHERKEY* key1, CIPHERKEY* key2);
    bool                                    SetError                                  (CIPHERCERTIFICATEX509VALIDATOR_ERROR error);
    void                                    Clean                                     ();

    CIPHERCERTIFICATEX509VALIDATOR_ERROR    error;
    XVECTOR<CIPHERCERTIFICATEX509*>         certificates;
    XVECTOR<CIPHERCERTIFICATEX509*>         roots;
};



/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

