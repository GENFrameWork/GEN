/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherX25519MLKEM768.h
* @class      CIPHERX25519MLKEM768
* @brief      X25519MLKEM768 hybrid key agreement (RFC 10024)
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/
#pragma once

#include "XBuffer.h"
#include "CipherECDSAX25519.h"
#include "CipherMLKEM768.h"

#define CIPHERX25519MLKEM768_CLIENTSHARESIZE  1216
#define CIPHERX25519MLKEM768_SERVERSHARESIZE  1120
#define CIPHERX25519MLKEM768_SHAREDSECRETSIZE 64

class CIPHERX25519MLKEM768
{
  public:
                          CIPHERX25519MLKEM768          ();
    virtual              ~CIPHERX25519MLKEM768          ();

    bool                  ClientKeyShare_Create         (XBUFFER& clientshare);
    bool                  ClientSharedSecret_Create     (XBUFFER& servershare, XBUFFER& sharedsecret, bool* invalidpeershare = NULL);
    bool                  ServerKeyShare_Create         (XBUFFER& clientshare, XBUFFER& servershare, XBUFFER& sharedsecret,
                                                         bool* invalidpeershare = NULL);
    void                  Delete                        ();

  private:
    bool                  X25519KeyPair_Create          (CIPHERECDSAX25519& x25519, XBUFFER& publickey);
    bool                  X25519SharedSecret_Create     (CIPHERECDSAX25519& x25519, XBYTE* peerpublic, XBUFFER& sharedsecret,
                                                         bool* invalidpeershare);
    void                  Clean                         ();

    CIPHERECDSAX25519      x25519;
    CIPHERMLKEM768         mlkem;
    XSECUREBUFFER          mlkemprivate;
};
