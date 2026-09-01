/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherSecP384r1MLKEM1024.h
* @class      CIPHERSECP384R1MLKEM1024
* @brief      SecP384r1MLKEM1024 hybrid key agreement (RFC 10024)
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/
#pragma once

#include "XBuffer.h"
#include "CipherECDSA.h"
#include "CipherMLKEM1024.h"

#define CIPHERSECP384R1MLKEM1024_CLIENTSHARESIZE   1665
#define CIPHERSECP384R1MLKEM1024_SERVERSHARESIZE   1665
#define CIPHERSECP384R1MLKEM1024_SHAREDSECRETSIZE    80

class CIPHERSECP384R1MLKEM1024
{
  public:
                          CIPHERSECP384R1MLKEM1024         ();
    virtual              ~CIPHERSECP384R1MLKEM1024         ();

    bool                  ClientKeyShare_Create           (XBUFFER& clientshare);
    bool                  ClientSharedSecret_Create       (XBUFFER& servershare, XBUFFER& sharedsecret,
                                                           bool* invalidpeershare = NULL);
    bool                  ServerKeyShare_Create           (XBUFFER& clientshare, XBUFFER& servershare,
                                                           XBUFFER& sharedsecret, bool* invalidpeershare = NULL);
    void                  Delete                          ();

  private:
    void                  Clean                           ();

    CIPHERECDSA           secp384r1;
    XSECUREBUFFER         secp384r1private;
    XBUFFER               secp384r1public;
    CIPHERMLKEM1024        mlkem;
    XSECUREBUFFER         mlkemprivate;
};
