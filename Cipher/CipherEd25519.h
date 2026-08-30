/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherEd25519.h
* @class      CIPHERED25519
* @brief      Ed25519 signature algorithm (RFC 8032)
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/
#pragma once

#include "XBuffer.h"

#define CIPHERED25519_PRIVATEKEYSIZE 32
#define CIPHERED25519_PUBLICKEYSIZE  32
#define CIPHERED25519_SIGNATURESIZE  64

class CIPHERED25519
{
  public:
                          CIPHERED25519       ();
    virtual              ~CIPHERED25519       ();

    bool                  KeyPair_Create      (XBUFFER& privatekey, XBUFFER& publickey);
    bool                  PublicKey_Create    (XBUFFER& privatekey, XBUFFER& publickey);
    bool                  PublicKey_IsValid   (XBUFFER& publickey);
    bool                  Sign                (XBUFFER& privatekey, XBUFFER& publickey, XBUFFER& input, XBUFFER& signature);
    bool                  Verify              (XBUFFER& publickey, XBUFFER& input, XBUFFER& signature);

  private:
    bool                  Random              (XBYTE* data, XDWORD size);
    void                  Clean               ();
};
