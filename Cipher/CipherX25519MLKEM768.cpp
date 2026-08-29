/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherX25519MLKEM768.cpp
* @class      CIPHERX25519MLKEM768
* @brief      X25519MLKEM768 hybrid key agreement (RFC 10024)
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/
#include "GEN_Defines.h"

#include "CipherX25519MLKEM768.h"

#include "GEN_Control.h"

CIPHERX25519MLKEM768::CIPHERX25519MLKEM768()
{
  Clean();
}

CIPHERX25519MLKEM768::~CIPHERX25519MLKEM768()
{
  Delete();
  Clean();
}

bool CIPHERX25519MLKEM768::X25519KeyPair_Create(CIPHERECDSAX25519& x25519, XBUFFER& publickey)
{
  publickey.Delete();
  x25519.CleanAllKeys();
  return x25519.GenerateRandomPrivateKey() && x25519.CreatePublicKey() &&
         publickey.Add(x25519.GetKey(CIPHERECDSAX25519_TYPEKEY_PUBLIC), CIPHERECDSAX25519_MAXKEY);
}

bool CIPHERX25519MLKEM768::X25519SharedSecret_Create(CIPHERECDSAX25519& x25519, XBYTE* peerpublic, XBUFFER& sharedsecret)
{
  sharedsecret.FillBuffer(0);
  sharedsecret.Delete();
  if(!peerpublic || !x25519.CreateSharedKey(peerpublic)) return false;

  XBYTE* secret = x25519.GetKey(CIPHERECDSAX25519_TYPEKEY_SHARED);
  if(!secret) return false;

  XBYTE any = 0;
  for(int c=0; c<CIPHERECDSAX25519_MAXKEY; c++) any |= secret[c];
  if(!any) return false;                    // RFC 10024 / RFC 7748 all-zero check

  return sharedsecret.Add(secret, CIPHERECDSAX25519_MAXKEY);
}

bool CIPHERX25519MLKEM768::ClientKeyShare_Create(XBUFFER& clientshare)
{
  XBUFFER mlkempublic;
  XBUFFER xpublic;

  Delete();
  clientshare.Delete();

  if(!mlkem.KeyPair_Create(mlkempublic, mlkemprivate) || !X25519KeyPair_Create(x25519, xpublic)) return false;

  return clientshare.Add(mlkempublic) && clientshare.Add(xpublic) &&
         clientshare.GetSize() == CIPHERX25519MLKEM768_CLIENTSHARESIZE;
}

bool CIPHERX25519MLKEM768::ClientSharedSecret_Create(XBUFFER& servershare, XBUFFER& sharedsecret)
{
  XBUFFER ciphertext;
  XBUFFER mlkemsecret;
  XBUFFER xsecret;

  sharedsecret.FillBuffer(0);
  sharedsecret.Delete();

  if(servershare.GetSize() != CIPHERX25519MLKEM768_SERVERSHARESIZE ||
     mlkemprivate.GetSize() != CIPHERMLKEM768_PRIVATEKEYSIZE) return false;

  ciphertext.Add(servershare.Get(), CIPHERMLKEM768_CIPHERTEXTSIZE);
  if(!mlkem.Decapsulate(mlkemprivate, ciphertext, mlkemsecret) ||
     !X25519SharedSecret_Create(x25519, servershare.Get()+CIPHERMLKEM768_CIPHERTEXTSIZE, xsecret)) return false;

  bool status = sharedsecret.Add(mlkemsecret) && sharedsecret.Add(xsecret) &&
                sharedsecret.GetSize() == CIPHERX25519MLKEM768_SHAREDSECRETSIZE;
  Delete();
  return status;
}

bool CIPHERX25519MLKEM768::ServerKeyShare_Create(XBUFFER& clientshare, XBUFFER& servershare, XBUFFER& sharedsecret)
{
  XBUFFER mlkempublic;
  XBUFFER ciphertext;
  XBUFFER mlkemsecret;
  XBUFFER xpublic;
  XBUFFER xsecret;

  servershare.Delete();
  sharedsecret.FillBuffer(0);
  sharedsecret.Delete();

  if(clientshare.GetSize() != CIPHERX25519MLKEM768_CLIENTSHARESIZE) return false;

  mlkempublic.Add(clientshare.Get(), CIPHERMLKEM768_PUBLICKEYSIZE);
  if(!mlkem.PublicKey_Check(mlkempublic)) return false;       // FIPS 203 encapsulation-key check required by RFC 10024

  if(!mlkem.Encapsulate(mlkempublic, ciphertext, mlkemsecret) ||
     !X25519KeyPair_Create(x25519, xpublic) ||
     !X25519SharedSecret_Create(x25519, clientshare.Get()+CIPHERMLKEM768_PUBLICKEYSIZE, xsecret)) return false;

  if(!servershare.Add(ciphertext) || !servershare.Add(xpublic) ||
     !sharedsecret.Add(mlkemsecret) || !sharedsecret.Add(xsecret)) return false;

  x25519.CleanAllKeys();
  return servershare.GetSize() == CIPHERX25519MLKEM768_SERVERSHARESIZE &&
         sharedsecret.GetSize() == CIPHERX25519MLKEM768_SHAREDSECRETSIZE;
}

void CIPHERX25519MLKEM768::Delete()
{
  x25519.CleanAllKeys();
  mlkemprivate.FillBuffer(0);
  mlkemprivate.Delete();
}

void CIPHERX25519MLKEM768::Clean()
{
}
