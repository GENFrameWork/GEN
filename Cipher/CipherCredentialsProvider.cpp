/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherCredentialsProvider.cpp
* @class      CIPHERCREDENTIALSPROVIDER
* @brief      Abstract source of certificate chains, private keys and secrets for Cipher consumers
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"

#include "CipherCredentialsProvider.h"
#include "CipherCredentialsLoader.h"

#include "GEN_Control.h"


CIPHERSECRETPROVIDER::CIPHERSECRETPROVIDER()
{
}


CIPHERSECRETPROVIDER::~CIPHERSECRETPROVIDER()
{
}


bool CIPHERSECRETPROVIDER::Secret_Release(XSTRING& secret)
{
  if(secret.Get())
    {
      volatile XCHAR* data = secret.Get();
      for(XDWORD c=0; c<secret.GetSize(); c++) data[c]=0;
    }

  secret.Empty();

  return true;
}


CIPHERSECRETPROVIDERSTRING::CIPHERSECRETPROVIDERSTRING()
{
  source = NULL;
}


CIPHERSECRETPROVIDERSTRING::~CIPHERSECRETPROVIDERSTRING()
{
  source = NULL;
}


void CIPHERSECRETPROVIDERSTRING::SetSource(XSTRING* source)
{
  this->source = source;
}


bool CIPHERSECRETPROVIDERSTRING::Secret_Get(XSTRING& secret)
{
  Secret_Release(secret);

  if(!source || source->IsEmpty()) return true;

  return secret.Set((*source));
}


CIPHERCREDENTIALSPROVIDER::CIPHERCREDENTIALSPROVIDER()
{
}


CIPHERCREDENTIALSPROVIDER::~CIPHERCREDENTIALSPROVIDER()
{
}


CIPHERCREDENTIALSPROVIDERBUFFER::CIPHERCREDENTIALSPROVIDERBUFFER()
{
  certificatedata = NULL;
  privatekeydata  = NULL;
  secretprovider  = NULL;
}


CIPHERCREDENTIALSPROVIDERBUFFER::~CIPHERCREDENTIALSPROVIDERBUFFER()
{
  certificatedata = NULL;
  privatekeydata  = NULL;
  secretprovider  = NULL;
}


void CIPHERCREDENTIALSPROVIDERBUFFER::SetCertificateData(XBUFFER* certificatedata)
{
  this->certificatedata = certificatedata;
}


void CIPHERCREDENTIALSPROVIDERBUFFER::SetPrivateKeyData(XBUFFER* privatekeydata)
{
  this->privatekeydata = privatekeydata;
}


void CIPHERCREDENTIALSPROVIDERBUFFER::SetSecretProvider(CIPHERSECRETPROVIDER* secretprovider)
{
  this->secretprovider = secretprovider;
}


bool CIPHERCREDENTIALSPROVIDERBUFFER::Credentials_Load(XVECTOR<XBUFFER*>& certificatechain, CIPHERKEY*& privatekey)
{
  if(!certificatedata || !privatekeydata) return false;

  XSTRING secret;
  XCHAR* password = NULL;

  if(secretprovider)
    {
      if(!secretprovider->Secret_Get(secret))
        {
          secretprovider->Secret_Release(secret);
          return false;
        }

      if(!secret.IsEmpty()) password = secret.Get();
    }

  bool status = CIPHERCREDENTIALSLOADER::Credentials_Load((*certificatedata), (*privatekeydata),
                                                           password, certificatechain, privatekey);

  if(secretprovider) secretprovider->Secret_Release(secret);
  else
    {
      if(secret.Get())
        {
          volatile XCHAR* data = secret.Get();
          for(XDWORD c=0; c<secret.GetSize(); c++) data[c]=0;
        }

      secret.Empty();
    }

  return status;
}
