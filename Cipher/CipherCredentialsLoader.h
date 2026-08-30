/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherCredentialsLoader.h
* @class      CIPHERCREDENTIALSLOADER
* @brief      Central certificate/private-key format loader for TLS and other Cipher consumers
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/
#pragma once

#include "XBuffer.h"
#include "XString.h"
#include "XVector.h"
#include "CipherKey.h"

class CIPHERCREDENTIALSLOADER
{
  public:
    static bool                             Certificates_Load              (XBUFFER& filedata, XVECTOR<XBUFFER*>& certificatechain);
    static bool                             PrivateKey_Load                (XBUFFER& filedata, XCHAR* password,
                                                                           CIPHERKEYTYPE expectedpublickeytype,
                                                                           CIPHERKEY*& privatekey);
    static bool                             Credentials_Load               (XBUFFER& certificatedata, XBUFFER& privatekeydata,
                                                                           XCHAR* password, XVECTOR<XBUFFER*>& certificatechain,
                                                                           CIPHERKEY*& privatekey);
    static void                             Certificates_Delete           (XVECTOR<XBUFFER*>& certificatechain);
    static void                             PrivateKey_Delete              (CIPHERKEY*& privatekey);

  private:
    static bool                             PEMBlocks_Decode               (XBUFFER& filedata, const char* label,
                                                                           XVECTOR<XBUFFER*>& blocks);
    static bool                             PrivateKeyDER_Decode           (XBUFFER& DER, CIPHERKEYTYPE expectedpublickeytype,
                                                                           CIPHERKEY*& privatekey);
};

