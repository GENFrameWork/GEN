/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherCredentialsProvider.h
* @class      CIPHERCREDENTIALSPROVIDER
* @brief      Abstract source of certificate chains, private keys and secrets for Cipher consumers
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/

#pragma once

#include "XBuffer.h"
#include "XString.h"
#include "XVector.h"
#include "CipherKey.h"


class CIPHERSECRETPROVIDER
{
  public:
                                CIPHERSECRETPROVIDER              ();
    virtual                    ~CIPHERSECRETPROVIDER              ();

    virtual bool                Secret_Get                         (XSTRING& secret) = 0;
    virtual bool                Secret_Release                     (XSTRING& secret);
};


class CIPHERSECRETPROVIDERSTRING : public CIPHERSECRETPROVIDER
{
  public:
                                CIPHERSECRETPROVIDERSTRING        ();
    virtual                    ~CIPHERSECRETPROVIDERSTRING        ();

    void                        SetSource                          (XSTRING* source);
    bool                        Secret_Get                         (XSTRING& secret);

  private:
    XSTRING*                    source;
};


class CIPHERCREDENTIALSPROVIDER
{
  public:
                                CIPHERCREDENTIALSPROVIDER         ();
    virtual                    ~CIPHERCREDENTIALSPROVIDER         ();

    virtual bool                Credentials_Load                   (XVECTOR<XBUFFER*>& certificatechain,
                                                                    CIPHERKEY*& privatekey) = 0;
};


class CIPHERCREDENTIALSPROVIDERBUFFER : public CIPHERCREDENTIALSPROVIDER
{
  public:
                                CIPHERCREDENTIALSPROVIDERBUFFER   ();
    virtual                    ~CIPHERCREDENTIALSPROVIDERBUFFER   ();

    void                        SetCertificateData                 (XBUFFER* certificatedata);
    void                        SetPrivateKeyData                  (XBUFFER* privatekeydata);
    void                        SetSecretProvider                  (CIPHERSECRETPROVIDER* secretprovider);

    bool                        Credentials_Load                   (XVECTOR<XBUFFER*>& certificatechain,
                                                                    CIPHERKEY*& privatekey);

  private:
    XBUFFER*                    certificatedata;
    XBUFFER*                    privatekeydata;
    CIPHERSECRETPROVIDER*       secretprovider;
};
