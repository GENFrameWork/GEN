/**-------------------------------------------------------------------------------------------------------------------
*
* @file       CipherPEMCodec.h
*
* @class      CIPHERPEMCODEC
* @brief      Cipher PEM/DER private key codec class
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

#include "XBuffer.h"
#include "XString.h"
#include "XVector.h"
#include "XMPInteger.h"



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/




/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

// Stateless decoding helpers for the private-key container formats standard tools (openssl, certbot, acme.sh...)
// actually produce: PKCS#1 "RSA PRIVATE KEY", SEC1 "EC PRIVATE KEY", and unencrypted PKCS#8 "PRIVATE KEY". Only
// decoding (import) is implemented -- GEN's own credential files (DER certificate + 1/3 line hex private key, see
// APPFLOWWEBSERVER::Ini_LoadTLSCredentials()) keep working exactly as before; this class only adds the ability to
// also accept the standard formats, detected automatically. Certificate PEM bundles (single leaf or a full chain,
// e.g. Let's Encrypt's fullchain.pem) are handled separately by the already-existing CIPHERTRUSTEDROOTCERTIFICATESX509
// (see CipherTrustedRootCertificatesX509.h) -- reused as-is, not duplicated here.
class CIPHERPEMCODEC
{
  public:

    // true if filedata looks like a PEM file (starts with "-----BEGIN ", ignoring leading whitespace).
    static bool             IsPEM                             (XBUFFER& filedata);

    // Scans lines for the first complete "-----BEGIN xxx PRIVATE KEY----- ... -----END xxx PRIVATE KEY-----"
    // block (RSA / EC / plain PKCS#8) and base64-decodes its body into DER. blockheader receives the exact
    // "-----BEGIN ...-----" line found, so the caller can tell RSA / EC / PKCS#8 apart (see the three decoders
    // below). Returns false if no complete block is found.
    static bool              PrivateKeyBlock_Decode            (XVECTOR<XSTRING*>& lines, XSTRING& blockheader, XBUFFER& der);

    // PKCS#1 RSAPrivateKey (already unwrapped DER, e.g. the body of a "RSA PRIVATE KEY" PEM block, or the inner
    // key of a PKCS#8 container whose algorithm is RSA). Only the three fields CIPHERKEYPRIVATERSA::Set() needs
    // are extracted (privateExponent/prime1/prime2); exponent1/exponent2/coefficient are ignored.
    static bool              RSAPrivateKey_Decode              (XBUFFER& der, XMPINTEGER& prime1, XMPINTEGER& prime2, XMPINTEGER& exponent);

    // SEC1 ECPrivateKey (already unwrapped DER, e.g. the body of an "EC PRIVATE KEY" PEM block, or the inner key
    // of a PKCS#8 container whose algorithm is EC). Only the privateKey OCTET STRING (the scalar D) is
    // extracted; the optional parameters/publicKey fields are ignored (the curve is already known from the
    // certificate's public key, see Ini_LoadTLSCredentials()).
    static bool              ECPrivateKey_Decode               (XBUFFER& der, XBUFFER& privatekey);

    // Unencrypted PKCS#8 PrivateKeyInfo (already unwrapped DER, e.g. the body of a plain "PRIVATE KEY" PEM
    // block). Identifies the algorithm from its OID (rsaEncryption / id-ecPublicKey) and returns the nested key
    // bytes (itself a RSAPrivateKey or ECPrivateKey DER blob, ready for the two decoders above). Encrypted PKCS#8
    // ("ENCRYPTED PRIVATE KEY") is not supported.
    static bool              PKCS8PrivateKey_Decode            (XBUFFER& der, bool& isrsa, bool& isec, XBUFFER& innerkey);

  private:

    static bool              DER_ReadLength                    (XBYTE* data, XDWORD size, XDWORD& index, XDWORD& length);
    static bool              DER_ReadTagLength                 (XBYTE* data, XDWORD size, XDWORD& index, XBYTE& tag, XDWORD& valueoffset, XDWORD& valuelength);
    static bool              DER_ReadInteger                   (XBYTE* data, XDWORD size, XDWORD& index, XMPINTEGER& value);
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/


