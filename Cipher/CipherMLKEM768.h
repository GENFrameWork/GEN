/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherMLKEM768.h
* @class      CIPHERMLKEM768
* @brief      ML-KEM-768 (FIPS 203) key encapsulation mechanism
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/
#pragma once

#include "XBuffer.h"
#include "CipherMLKEM768Core.h"

class CIPHERMLKEM768
{
  public:
                          CIPHERMLKEM768       ();
    virtual              ~CIPHERMLKEM768       ();

    bool                  KeyPair_Create       (XBUFFER& publickey, XBUFFER& privatekey);
    bool                  Encapsulate          (XBUFFER& publickey, XBUFFER& ciphertext, XBUFFER& sharedsecret);
    bool                  Decapsulate          (XBUFFER& privatekey, XBUFFER& ciphertext, XBUFFER& sharedsecret);
    bool                  PublicKey_Check      (XBUFFER& publickey);

  private:
    bool                  Random               (XBYTE* data, XDWORD size);
    void                  Clean                ();
};
