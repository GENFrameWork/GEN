/**-------------------------------------------------------------------------------------------------------------------
*
* @file       CipherChaCha20Poly1305.h
*
* @class      CIPHERCHACHA20POLY1305
* @brief      Cipher ChaCha20-Poly1305 AEAD (RFC 8439) class
* @ingroup    CIPHER
*
* @copyright  EndoraSoft. All rights reserved.
*
* --------------------------------------------------------------------------------------------------------------------*/

#pragma once

#include "Cipher.h"

#define CIPHERCHACHA20POLY1305_KEYSIZE    32
#define CIPHERCHACHA20POLY1305_NONCESIZE  12
#define CIPHERCHACHA20POLY1305_TAGSIZE    16

class CIPHERCHACHA20POLY1305 : public CIPHER
{
  public:
                          CIPHERCHACHA20POLY1305       ();
    virtual              ~CIPHERCHACHA20POLY1305       ();

    bool                  SetKey                       (CIPHERKEY* key, bool integritycheck = false);

    bool                  IsAEAD                       ();
    XDWORD                GetAEADNonceSize             ();
    XDWORD                GetAEADTagSize               ();

    bool                  CipherAEAD                   (XBYTE* input, XDWORD size, XBUFFER& nonce, XBUFFER& additionaldata, XBUFFER& tag);
    bool                  UncipherAEAD                 (XBYTE* input, XDWORD size, XBUFFER& nonce, XBUFFER& additionaldata, XBUFFER& tag);

  private:
    static XDWORD         Load32LE                     (const XBYTE* data);
    static void           Store32LE                    (XBYTE* data, XDWORD value);
    static void           Store64LE                    (XBYTE* data, XQWORD value);
    static XDWORD         RotateLeft32                 (XDWORD value, int bits);
    static void           QuarterRound                 (XDWORD& a, XDWORD& b, XDWORD& c, XDWORD& d);
    static void           Block                        (const XBYTE key[CIPHERCHACHA20POLY1305_KEYSIZE], XDWORD counter,
                                                       const XBYTE nonce[CIPHERCHACHA20POLY1305_NONCESIZE], XBYTE output[64]);
    static void           Crypt                        (const XBYTE key[CIPHERCHACHA20POLY1305_KEYSIZE], XDWORD counter,
                                                       const XBYTE nonce[CIPHERCHACHA20POLY1305_NONCESIZE],
                                                       const XBYTE* input, XBYTE* output, XDWORD size);
    static void           Poly1305                     (const XBYTE key[32], const XBYTE* message, XQWORD size, XBYTE tag[16]);
    static bool           CalculateTag                 (const XBYTE key[CIPHERCHACHA20POLY1305_KEYSIZE],
                                                       const XBYTE nonce[CIPHERCHACHA20POLY1305_NONCESIZE],
                                                       XBUFFER& additionaldata, const XBYTE* ciphertext, XDWORD size, XBYTE tag[16]);
    bool                  GetRawKey                    (XBYTE key[CIPHERCHACHA20POLY1305_KEYSIZE]);
    void                  Clean                        ();

    bool                  keyisready;
};
