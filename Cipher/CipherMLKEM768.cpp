/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherMLKEM768.cpp
* @class      CIPHERMLKEM768
* @brief      ML-KEM-768 (FIPS 203) key encapsulation mechanism
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/
#include "GEN_Defines.h"

#include "CipherMLKEM768.h"

#include "XFactory.h"
#include "XRand.h"

#include "GEN_Control.h"

CIPHERMLKEM768::CIPHERMLKEM768()
{
  Clean();
}

CIPHERMLKEM768::~CIPHERMLKEM768()
{
  Clean();
}

bool CIPHERMLKEM768::Random(XBYTE* data, XDWORD size)
{
  if(!data || !size) return false;

  XRAND* xrand = GEN_XFACTORY.CreateRand();
  if(!xrand) return false;

  bool status = false;
  if(xrand->Ini() && xrand->IsCryptographicallySecure()) status = xrand->Generate(data, size);
  GEN_XFACTORY.DeleteRand(xrand);
  return status;
}

bool CIPHERMLKEM768::KeyPair_Create(XBUFFER& publickey, XBUFFER& privatekey)
{
  XBYTE d[32];
  XBYTE z[32];
  XBYTE pk[CIPHERMLKEM768_PUBLICKEYSIZE];
  XBYTE sk[CIPHERMLKEM768_PRIVATEKEYSIZE];

  publickey.Delete();
  privatekey.FillBuffer(0);
  privatekey.Delete();

  bool status = Random(d, sizeof(d)) && Random(z, sizeof(z)) &&
                CIPHERMLKEM768CORE::KeyPair(d, z, pk, sk) &&
                publickey.Add(pk, sizeof(pk)) && privatekey.Add(sk, sizeof(sk));

  memset(d,  0, sizeof(d));
  memset(z,  0, sizeof(z));
  memset(pk, 0, sizeof(pk));
  memset(sk, 0, sizeof(sk));
  return status;
}

bool CIPHERMLKEM768::Encapsulate(XBUFFER& publickey, XBUFFER& ciphertext, XBUFFER& sharedsecret)
{
  XBYTE randomness[32];
  XBYTE ct[CIPHERMLKEM768_CIPHERTEXTSIZE];
  XBYTE ss[CIPHERMLKEM768_SHAREDSECRETSIZE];

  ciphertext.Delete();
  sharedsecret.FillBuffer(0);
  sharedsecret.Delete();

  if(publickey.GetSize() != CIPHERMLKEM768_PUBLICKEYSIZE) return false;

  bool status = Random(randomness, sizeof(randomness)) &&
                CIPHERMLKEM768CORE::Encapsulate(randomness, publickey.Get(), ct, ss) &&
                ciphertext.Add(ct, sizeof(ct)) && sharedsecret.Add(ss, sizeof(ss));

  memset(randomness, 0, sizeof(randomness));
  memset(ct,         0, sizeof(ct));
  memset(ss,         0, sizeof(ss));
  return status;
}

bool CIPHERMLKEM768::Decapsulate(XBUFFER& privatekey, XBUFFER& ciphertext, XBUFFER& sharedsecret)
{
  XBYTE ss[CIPHERMLKEM768_SHAREDSECRETSIZE];

  sharedsecret.FillBuffer(0);
  sharedsecret.Delete();

  if(privatekey.GetSize() != CIPHERMLKEM768_PRIVATEKEYSIZE || ciphertext.GetSize() != CIPHERMLKEM768_CIPHERTEXTSIZE)
    {
      return false;
    }

  bool status = CIPHERMLKEM768CORE::Decapsulate(privatekey.Get(), ciphertext.Get(), ss) &&
                sharedsecret.Add(ss, sizeof(ss));
  memset(ss, 0, sizeof(ss));
  return status;
}

bool CIPHERMLKEM768::PublicKey_Check(XBUFFER& publickey)
{
  return publickey.GetSize() == CIPHERMLKEM768_PUBLICKEYSIZE && CIPHERMLKEM768CORE::PublicKey_Check(publickey.Get());
}

void CIPHERMLKEM768::Clean()
{
}
