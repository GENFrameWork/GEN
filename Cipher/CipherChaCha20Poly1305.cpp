/**-------------------------------------------------------------------------------------------------------------------
*
* @file       CipherChaCha20Poly1305.cpp
*
* @class      CIPHERCHACHA20POLY1305
* @brief      Cipher ChaCha20-Poly1305 AEAD (RFC 8439) class
* @ingroup    CIPHER
*
* @copyright  EndoraSoft. All rights reserved.
*
* --------------------------------------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"

#include "CipherChaCha20Poly1305.h"

#include "CipherKeySymmetrical.h"

#include "GEN_Control.h"


CIPHERCHACHA20POLY1305::CIPHERCHACHA20POLY1305() : CIPHER()
{
  Clean();

  type         = CIPHERTYPE_CHACHA20_POLY1305;
  chainingmode = CIPHERCHAININGMODE_POLY1305;
}


CIPHERCHACHA20POLY1305::~CIPHERCHACHA20POLY1305()
{
  Clean();
}


bool CIPHERCHACHA20POLY1305::SetKey(CIPHERKEY* key, bool integritycheck)
{
  keyisready = false;

  if(!key || key->GetType() != CIPHERKEYTYPE_SYMMETRICAL || key->GetSizeInBytes() != CIPHERCHACHA20POLY1305_KEYSIZE)
    {
      return false;
    }

  if(!CIPHER::SetKey(key, integritycheck)) return false;

  keyisready = true;
  return true;
}


bool CIPHERCHACHA20POLY1305::IsAEAD()
{
  return true;
}


XDWORD CIPHERCHACHA20POLY1305::GetAEADNonceSize()
{
  return CIPHERCHACHA20POLY1305_NONCESIZE;
}


XDWORD CIPHERCHACHA20POLY1305::GetAEADTagSize()
{
  return CIPHERCHACHA20POLY1305_TAGSIZE;
}


XDWORD CIPHERCHACHA20POLY1305::Load32LE(const XBYTE* data)
{
  return ((XDWORD)data[0])        |
         ((XDWORD)data[1] <<  8) |
         ((XDWORD)data[2] << 16) |
         ((XDWORD)data[3] << 24);
}


void CIPHERCHACHA20POLY1305::Store32LE(XBYTE* data, XDWORD value)
{
  data[0] = (XBYTE)value;
  data[1] = (XBYTE)(value >> 8);
  data[2] = (XBYTE)(value >> 16);
  data[3] = (XBYTE)(value >> 24);
}


void CIPHERCHACHA20POLY1305::Store64LE(XBYTE* data, XQWORD value)
{
  for(int c=0; c<8; c++) data[c] = (XBYTE)(value >> (c*8));
}


XDWORD CIPHERCHACHA20POLY1305::RotateLeft32(XDWORD value, int bits)
{
  return (value << bits) | (value >> (32-bits));
}


void CIPHERCHACHA20POLY1305::QuarterRound(XDWORD& a, XDWORD& b, XDWORD& c, XDWORD& d)
{
  a += b; d ^= a; d = RotateLeft32(d, 16);
  c += d; b ^= c; b = RotateLeft32(b, 12);
  a += b; d ^= a; d = RotateLeft32(d,  8);
  c += d; b ^= c; b = RotateLeft32(b,  7);
}


void CIPHERCHACHA20POLY1305::Block(const XBYTE key[CIPHERCHACHA20POLY1305_KEYSIZE], XDWORD counter,
                                   const XBYTE nonce[CIPHERCHACHA20POLY1305_NONCESIZE], XBYTE output[64])
{
  static const XDWORD constants[4] = { 0x61707865, 0x3320646e, 0x79622d32, 0x6b206574 };
  XDWORD state[16];
  XDWORD working[16];

  state[0] = constants[0]; state[1] = constants[1]; state[2] = constants[2]; state[3] = constants[3];
  for(int c=0; c<8; c++) state[4+c] = Load32LE(&key[c*4]);
  state[12] = counter;
  state[13] = Load32LE(&nonce[0]);
  state[14] = Load32LE(&nonce[4]);
  state[15] = Load32LE(&nonce[8]);

  for(int c=0; c<16; c++) working[c] = state[c];

  for(int c=0; c<10; c++)
    {
      QuarterRound(working[0], working[4], working[ 8], working[12]);
      QuarterRound(working[1], working[5], working[ 9], working[13]);
      QuarterRound(working[2], working[6], working[10], working[14]);
      QuarterRound(working[3], working[7], working[11], working[15]);
      QuarterRound(working[0], working[5], working[10], working[15]);
      QuarterRound(working[1], working[6], working[11], working[12]);
      QuarterRound(working[2], working[7], working[ 8], working[13]);
      QuarterRound(working[3], working[4], working[ 9], working[14]);
    }

  for(int c=0; c<16; c++) Store32LE(&output[c*4], working[c] + state[c]);

  memset(state,   0, sizeof(state));
  memset(working, 0, sizeof(working));
}


void CIPHERCHACHA20POLY1305::Crypt(const XBYTE key[CIPHERCHACHA20POLY1305_KEYSIZE], XDWORD counter,
                                   const XBYTE nonce[CIPHERCHACHA20POLY1305_NONCESIZE],
                                   const XBYTE* input, XBYTE* output, XDWORD size)
{
  XBYTE block[64];
  XDWORD offset = 0;

  while(offset < size)
    {
      Block(key, counter++, nonce, block);
      XDWORD n = ((size-offset) > sizeof(block)) ? (XDWORD)sizeof(block) : (size-offset);
      for(XDWORD c=0; c<n; c++) output[offset+c] = input[offset+c] ^ block[c];
      offset += n;
    }

  memset(block, 0, sizeof(block));
}


void CIPHERCHACHA20POLY1305::Poly1305(const XBYTE key[32], const XBYTE* message, XQWORD size, XBYTE tag[16])
{
  XDWORD r0 = ( Load32LE(key + 0)                    ) & 0x3ffffff;
  XDWORD r1 = ((Load32LE(key + 3) >> 2)              ) & 0x3ffff03;
  XDWORD r2 = ((Load32LE(key + 6) >> 4)              ) & 0x3ffc0ff;
  XDWORD r3 = ((Load32LE(key + 9) >> 6)              ) & 0x3f03fff;
  XDWORD r4 = ((Load32LE(key +12) >> 8)              ) & 0x00fffff;
  XDWORD s1 = r1 * 5;
  XDWORD s2 = r2 * 5;
  XDWORD s3 = r3 * 5;
  XDWORD s4 = r4 * 5;
  XDWORD h0 = 0, h1 = 0, h2 = 0, h3 = 0, h4 = 0;

  while(size >= 16)
    {
      h0 += ( Load32LE(message + 0)                    ) & 0x3ffffff;
      h1 += ((Load32LE(message + 3) >> 2)              ) & 0x3ffffff;
      h2 += ((Load32LE(message + 6) >> 4)              ) & 0x3ffffff;
      h3 += ((Load32LE(message + 9) >> 6)              ) & 0x3ffffff;
      h4 += ((Load32LE(message +12) >> 8)              ) | (1 << 24);

      XQWORD d0 = ((XQWORD)h0*r0) + ((XQWORD)h1*s4) + ((XQWORD)h2*s3) + ((XQWORD)h3*s2) + ((XQWORD)h4*s1);
      XQWORD d1 = ((XQWORD)h0*r1) + ((XQWORD)h1*r0) + ((XQWORD)h2*s4) + ((XQWORD)h3*s3) + ((XQWORD)h4*s2);
      XQWORD d2 = ((XQWORD)h0*r2) + ((XQWORD)h1*r1) + ((XQWORD)h2*r0) + ((XQWORD)h3*s4) + ((XQWORD)h4*s3);
      XQWORD d3 = ((XQWORD)h0*r3) + ((XQWORD)h1*r2) + ((XQWORD)h2*r1) + ((XQWORD)h3*r0) + ((XQWORD)h4*s4);
      XQWORD d4 = ((XQWORD)h0*r4) + ((XQWORD)h1*r3) + ((XQWORD)h2*r2) + ((XQWORD)h3*r1) + ((XQWORD)h4*r0);

      XDWORD c = (XDWORD)(d0 >> 26); h0 = (XDWORD)d0 & 0x3ffffff; d1 += c;
      c = (XDWORD)(d1 >> 26); h1 = (XDWORD)d1 & 0x3ffffff; d2 += c;
      c = (XDWORD)(d2 >> 26); h2 = (XDWORD)d2 & 0x3ffffff; d3 += c;
      c = (XDWORD)(d3 >> 26); h3 = (XDWORD)d3 & 0x3ffffff; d4 += c;
      c = (XDWORD)(d4 >> 26); h4 = (XDWORD)d4 & 0x3ffffff; h0 += c * 5;
      c = h0 >> 26; h0 &= 0x3ffffff; h1 += c;

      message += 16;
      size    -= 16;
    }

  if(size)
    {
      XBYTE last[16] = {0};
      for(XDWORD c=0; c<(XDWORD)size; c++) last[c] = message[c];
      last[size] = 1;

      h0 += ( Load32LE(last + 0)                    ) & 0x3ffffff;
      h1 += ((Load32LE(last + 3) >> 2)              ) & 0x3ffffff;
      h2 += ((Load32LE(last + 6) >> 4)              ) & 0x3ffffff;
      h3 += ((Load32LE(last + 9) >> 6)              ) & 0x3ffffff;
      h4 += ((Load32LE(last +12) >> 8)              );

      XQWORD d0 = ((XQWORD)h0*r0) + ((XQWORD)h1*s4) + ((XQWORD)h2*s3) + ((XQWORD)h3*s2) + ((XQWORD)h4*s1);
      XQWORD d1 = ((XQWORD)h0*r1) + ((XQWORD)h1*r0) + ((XQWORD)h2*s4) + ((XQWORD)h3*s3) + ((XQWORD)h4*s2);
      XQWORD d2 = ((XQWORD)h0*r2) + ((XQWORD)h1*r1) + ((XQWORD)h2*r0) + ((XQWORD)h3*s4) + ((XQWORD)h4*s3);
      XQWORD d3 = ((XQWORD)h0*r3) + ((XQWORD)h1*r2) + ((XQWORD)h2*r1) + ((XQWORD)h3*r0) + ((XQWORD)h4*s4);
      XQWORD d4 = ((XQWORD)h0*r4) + ((XQWORD)h1*r3) + ((XQWORD)h2*r2) + ((XQWORD)h3*r1) + ((XQWORD)h4*r0);

      XDWORD c = (XDWORD)(d0 >> 26); h0 = (XDWORD)d0 & 0x3ffffff; d1 += c;
      c = (XDWORD)(d1 >> 26); h1 = (XDWORD)d1 & 0x3ffffff; d2 += c;
      c = (XDWORD)(d2 >> 26); h2 = (XDWORD)d2 & 0x3ffffff; d3 += c;
      c = (XDWORD)(d3 >> 26); h3 = (XDWORD)d3 & 0x3ffffff; d4 += c;
      c = (XDWORD)(d4 >> 26); h4 = (XDWORD)d4 & 0x3ffffff; h0 += c * 5;
      c = h0 >> 26; h0 &= 0x3ffffff; h1 += c;

      memset(last, 0, sizeof(last));
    }

  XDWORD c = h1 >> 26; h1 &= 0x3ffffff; h2 += c;
  c = h2 >> 26; h2 &= 0x3ffffff; h3 += c;
  c = h3 >> 26; h3 &= 0x3ffffff; h4 += c;
  c = h4 >> 26; h4 &= 0x3ffffff; h0 += c * 5;
  c = h0 >> 26; h0 &= 0x3ffffff; h1 += c;

  XDWORD g0 = h0 + 5;
  c = g0 >> 26; g0 &= 0x3ffffff;
  XDWORD g1 = h1 + c; c = g1 >> 26; g1 &= 0x3ffffff;
  XDWORD g2 = h2 + c; c = g2 >> 26; g2 &= 0x3ffffff;
  XDWORD g3 = h3 + c; c = g3 >> 26; g3 &= 0x3ffffff;
  XDWORD g4 = h4 + c - (1 << 26);

  XDWORD mask = (g4 >> 31) - 1;
  XDWORD nmask = ~mask;
  h0 = (h0 & nmask) | (g0 & mask);
  h1 = (h1 & nmask) | (g1 & mask);
  h2 = (h2 & nmask) | (g2 & mask);
  h3 = (h3 & nmask) | (g3 & mask);
  h4 = (h4 & nmask) | (g4 & mask);

  // Serialize the 26-bit limbs into four 32-bit words before adding the
  // Poly1305 pad.  The truncation to 32 bits is part of the algorithm;
  // keeping the complete shifted limbs in a 64-bit value would propagate
  // bits twice through the carry chain and produce an invalid tag.
  XQWORD f0 = (XDWORD)( h0        | (h1 << 26));
  XQWORD f1 = (XDWORD)((h1 >>  6) | (h2 << 20));
  XQWORD f2 = (XDWORD)((h2 >> 12) | (h3 << 14));
  XQWORD f3 = (XDWORD)((h3 >> 18) | (h4 <<  8));

  f0 += Load32LE(key + 16);
  f1 += Load32LE(key + 20) + (f0 >> 32); f0 &= 0xffffffffULL;
  f2 += Load32LE(key + 24) + (f1 >> 32); f1 &= 0xffffffffULL;
  f3 += Load32LE(key + 28) + (f2 >> 32); f2 &= 0xffffffffULL;

  Store32LE(tag +  0, (XDWORD)f0);
  Store32LE(tag +  4, (XDWORD)f1);
  Store32LE(tag +  8, (XDWORD)f2);
  Store32LE(tag + 12, (XDWORD)f3);
}


bool CIPHERCHACHA20POLY1305::CalculateTag(const XBYTE key[CIPHERCHACHA20POLY1305_KEYSIZE],
                                           const XBYTE nonce[CIPHERCHACHA20POLY1305_NONCESIZE],
                                           XBUFFER& additionaldata, const XBYTE* ciphertext, XDWORD size, XBYTE tag[16])
{
  XBYTE block[64];
  XBYTE polykey[32];
  XBUFFER authenticated;

  Block(key, 0, nonce, block);
  memcpy(polykey, block, sizeof(polykey));

  if(additionaldata.GetSize() && !authenticated.Add(additionaldata)) return false;
  while(authenticated.GetSize() & 15) authenticated.Add((XBYTE)0);
  if(size && !authenticated.Add((XBYTE*)ciphertext, size)) return false;
  while(authenticated.GetSize() & 15) authenticated.Add((XBYTE)0);

  XBYTE lengths[16];
  Store64LE(lengths,     (XQWORD)additionaldata.GetSize());
  Store64LE(lengths + 8, (XQWORD)size);
  if(!authenticated.Add(lengths, sizeof(lengths))) return false;

  Poly1305(polykey, authenticated.Get(), authenticated.GetSize(), tag);

  memset(block,   0, sizeof(block));
  memset(polykey, 0, sizeof(polykey));
  memset(lengths, 0, sizeof(lengths));
  return true;
}


bool CIPHERCHACHA20POLY1305::GetRawKey(XBYTE key[CIPHERCHACHA20POLY1305_KEYSIZE])
{
  if(!keyisready) return false;

  CIPHERKEYSYMMETRICAL* symmetricalkey = (CIPHERKEYSYMMETRICAL*)GetKey();
  if(!symmetricalkey || !symmetricalkey->Get() || symmetricalkey->Get()->GetSize() != CIPHERCHACHA20POLY1305_KEYSIZE)
    {
      return false;
    }

  memcpy(key, symmetricalkey->Get()->Get(), CIPHERCHACHA20POLY1305_KEYSIZE);
  return true;
}


bool CIPHERCHACHA20POLY1305::CipherAEAD(XBYTE* input, XDWORD size, XBUFFER& nonce, XBUFFER& additionaldata, XBUFFER& tag)
{
  XBYTE key[CIPHERCHACHA20POLY1305_KEYSIZE];
  XBYTE authtag[CIPHERCHACHA20POLY1305_TAGSIZE];

  if((size && !input) || nonce.GetSize() != CIPHERCHACHA20POLY1305_NONCESIZE || !GetRawKey(key)) return false;

  result->Delete();
  if(size)
    {
      if(!result->Resize(size)) return false;
      Crypt(key, 1, nonce.Get(), input, result->Get(), size);
    }

  if(!CalculateTag(key, nonce.Get(), additionaldata, result->Get(), size, authtag)) return false;

  tag.Delete();
  bool status = tag.Add(authtag, sizeof(authtag));
  memset(key,     0, sizeof(key));
  memset(authtag, 0, sizeof(authtag));
  return status;
}


bool CIPHERCHACHA20POLY1305::UncipherAEAD(XBYTE* input, XDWORD size, XBUFFER& nonce, XBUFFER& additionaldata, XBUFFER& tag)
{
  XBYTE key[CIPHERCHACHA20POLY1305_KEYSIZE];
  XBYTE expected[CIPHERCHACHA20POLY1305_TAGSIZE];

  if((size && !input) || nonce.GetSize() != CIPHERCHACHA20POLY1305_NONCESIZE ||
     tag.GetSize() != CIPHERCHACHA20POLY1305_TAGSIZE || !GetRawKey(key)) return false;

  if(!CalculateTag(key, nonce.Get(), additionaldata, input, size, expected)) return false;
  if(!CIPHER::CompareConstantTime(expected, tag.Get(), sizeof(expected)))
    {
      memset(key,      0, sizeof(key));
      memset(expected, 0, sizeof(expected));
      return false;
    }

  result->Delete();
  if(size)
    {
      if(!result->Resize(size)) return false;
      Crypt(key, 1, nonce.Get(), input, result->Get(), size);
    }

  memset(key,      0, sizeof(key));
  memset(expected, 0, sizeof(expected));
  return true;
}


void CIPHERCHACHA20POLY1305::Clean()
{
  keyisready = false;
}
