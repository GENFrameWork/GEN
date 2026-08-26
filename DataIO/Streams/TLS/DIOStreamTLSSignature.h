/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamTLSSignature.h
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

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XBuffer.h"



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/




/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class CIPHERKEY;


class DIOSTREAMTLSSIGNATURE
{
  public:

    static bool                             IsSupported                                      (XWORD signaturescheme, CIPHERKEY* key);
    static bool                             Verify                                           (XWORD signaturescheme, CIPHERKEY* key, XBUFFER& content, XBUFFER& signature);

    // Server-side counterpart of Verify(). Supports RSA-PSS and ECDSA (P-256/P-384/P-521, see
    // CIPHERECDSA::Parameters_Set()). publickey is required in addition to privatekey: for RSA,
    // CIPHERKEYPRIVATERSA only carries P/Q/D (see SetKey()'s CIPHERKEYTYPE_RSA_PRIVATE branch in CipherRSA.cpp),
    // N/E come from the local certificate's public key; for ECDSA it is not strictly needed to produce a
    // signature, but is used the same way CIPHERRSA uses it -- as an integrity cross-check that privatekey and
    // publickey actually belong to the same key pair (see CIPHERECDSA::SetKey()'s private-key branch).
    static bool                             Sign                                             (XWORD signaturescheme, CIPHERKEY* privatekey, CIPHERKEY* publickey, XBUFFER& content, XBUFFER& signature);
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/


