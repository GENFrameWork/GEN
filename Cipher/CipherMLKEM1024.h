/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherMLKEM1024.h
* @class      CIPHERMLKEM1024
* @brief      ML-KEM-1024 (FIPS 203) key encapsulation mechanism
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/
#pragma once

#include "XBuffer.h"
#include "CipherMLKEM1024Core.h"

class CIPHERMLKEM1024
{
  public:
                          CIPHERMLKEM1024       ();
    virtual              ~CIPHERMLKEM1024       ();

    bool                  KeyPair_Create       (XBUFFER& publickey, XBUFFER& privatekey);
    bool                  Encapsulate          (XBUFFER& publickey, XBUFFER& ciphertext, XBUFFER& sharedsecret);
    bool                  Decapsulate          (XBUFFER& privatekey, XBUFFER& ciphertext, XBUFFER& sharedsecret);
    bool                  PublicKey_Check      (XBUFFER& publickey);

  private:
    bool                  Random               (XBYTE* data, XDWORD size);
    void                  Clean                ();
};
