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

bool CIPHERX25519MLKEM768::X25519SharedSecret_Create(CIPHERECDSAX25519& x25519, XBYTE* peerpublic, XBUFFER& sharedsecret,
                                                      bool* invalidpeershare)
{
  if(invalidpeershare) (*invalidpeershare) = false;

  sharedsecret.SecureDelete();
  if(!peerpublic || !x25519.GetKey(CIPHERECDSAX25519_TYPEKEY_PRIVATE)) return false;

  // CIPHERECDSAX25519::CreateSharedKey() returns false for the all-zero result. With a fresh local private
  // scalar that result is a peer-share validation failure, as required by RFC 10024 / RFC 7748.
  if(!x25519.CreateSharedKey(peerpublic))
    {
      if(invalidpeershare) (*invalidpeershare) = true;
      return false;
    }

  XBYTE* secret = x25519.GetKey(CIPHERECDSAX25519_TYPEKEY_SHARED);
  if(!secret) return false;

  XBYTE any = 0;
  for(int c=0; c<CIPHERECDSAX25519_MAXKEY; c++) any |= secret[c];
  if(!any)
    {
      if(invalidpeershare) (*invalidpeershare) = true;
      return false;
    }

  return sharedsecret.Add(secret, CIPHERECDSAX25519_MAXKEY);
}

bool CIPHERX25519MLKEM768::ClientKeyShare_Create(XBUFFER& clientshare)
{
  XBUFFER mlkempublic;
  XBUFFER xpublic;

  Delete();
  clientshare.Delete();

  bool status = mlkem.KeyPair_Create(mlkempublic, mlkemprivate) &&
                X25519KeyPair_Create(x25519, xpublic) &&
                clientshare.Add(mlkempublic) && clientshare.Add(xpublic) &&
                (clientshare.GetSize() == CIPHERX25519MLKEM768_CLIENTSHARESIZE);

  if(!status)
    {
      clientshare.Delete();
      Delete();
    }

  return status;
}

bool CIPHERX25519MLKEM768::ClientSharedSecret_Create(XBUFFER& servershare, XBUFFER& sharedsecret, bool* invalidpeershare)
{
  XBUFFER ciphertext;
  XSECUREBUFFER mlkemsecret;
  XSECUREBUFFER xsecret;
  bool status = false;

  if(invalidpeershare) (*invalidpeershare) = false;
  sharedsecret.SecureDelete();

  do
    {
      if(servershare.GetSize() != CIPHERX25519MLKEM768_SERVERSHARESIZE)
        {
          if(invalidpeershare) (*invalidpeershare) = true;
          break;
        }

      if(mlkemprivate.GetSize() != CIPHERMLKEM768_PRIVATEKEYSIZE) break;

      if(!ciphertext.Add(servershare.Get(), CIPHERMLKEM768_CIPHERTEXTSIZE) ||
         !mlkem.Decapsulate(mlkemprivate, ciphertext, mlkemsecret) ||
         !X25519SharedSecret_Create(x25519, servershare.Get()+CIPHERMLKEM768_CIPHERTEXTSIZE,
                                    xsecret, invalidpeershare)) break;

      // RFC 10024 fixes this order: ML-KEM shared secret first, then X25519.
      status = sharedsecret.Add(mlkemsecret) && sharedsecret.Add(xsecret) &&
               (sharedsecret.GetSize() == CIPHERX25519MLKEM768_SHAREDSECRETSIZE);
    }
  while(false);

  if(!status) sharedsecret.SecureDelete();
  Delete();
  return status;
}

bool CIPHERX25519MLKEM768::ServerKeyShare_Create(XBUFFER& clientshare, XBUFFER& servershare, XBUFFER& sharedsecret,
                                                  bool* invalidpeershare)
{
  XBUFFER mlkempublic;
  XBUFFER ciphertext;
  XSECUREBUFFER mlkemsecret;
  XBUFFER xpublic;
  XSECUREBUFFER xsecret;
  bool status = false;

  if(invalidpeershare) (*invalidpeershare) = false;
  Delete();
  servershare.Delete();
  sharedsecret.SecureDelete();

  do
    {
      if(clientshare.GetSize() != CIPHERX25519MLKEM768_CLIENTSHARESIZE)
        {
          if(invalidpeershare) (*invalidpeershare) = true;
          break;
        }

      if(!mlkempublic.Add(clientshare.Get(), CIPHERMLKEM768_PUBLICKEYSIZE)) break;
      if(!mlkem.PublicKey_Check(mlkempublic))
        {
          // FIPS 203 encapsulation-key check required before consuming encapsulation randomness.
          if(invalidpeershare) (*invalidpeershare) = true;
          break;
        }

      if(!X25519KeyPair_Create(x25519, xpublic) ||
         !X25519SharedSecret_Create(x25519, clientshare.Get()+CIPHERMLKEM768_PUBLICKEYSIZE,
                                    xsecret, invalidpeershare) ||
         !mlkem.Encapsulate(mlkempublic, ciphertext, mlkemsecret)) break;

      status = servershare.Add(ciphertext) && servershare.Add(xpublic) &&
               sharedsecret.Add(mlkemsecret) && sharedsecret.Add(xsecret) &&
               (servershare.GetSize() == CIPHERX25519MLKEM768_SERVERSHARESIZE) &&
               (sharedsecret.GetSize() == CIPHERX25519MLKEM768_SHAREDSECRETSIZE);
    }
  while(false);

  x25519.CleanAllKeys();
  if(!status)
    {
      servershare.Delete();
      sharedsecret.SecureDelete();
    }

  return status;
}

void CIPHERX25519MLKEM768::Delete()
{
  x25519.CleanAllKeys();
  mlkemprivate.SecureDelete();
}

void CIPHERX25519MLKEM768::Clean()
{
}
