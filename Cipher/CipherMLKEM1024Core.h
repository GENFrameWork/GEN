#pragma once

#include <stdint.h>
#include <stddef.h>

#define CIPHERMLKEM1024_PUBLICKEYSIZE      1568
#define CIPHERMLKEM1024_PRIVATEKEYSIZE     3168
#define CIPHERMLKEM1024_CIPHERTEXTSIZE     1568
#define CIPHERMLKEM1024_SHAREDSECRETSIZE   32
#define CIPHERMLKEM1024_SEEDSIZE           32

class CIPHERMLKEM1024CORE
{
  public:
    static bool KeyPair(const uint8_t d[32], const uint8_t z[32], uint8_t publickey[CIPHERMLKEM1024_PUBLICKEYSIZE], uint8_t privatekey[CIPHERMLKEM1024_PRIVATEKEYSIZE]);
    static bool Encapsulate(const uint8_t randomness[32], const uint8_t publickey[CIPHERMLKEM1024_PUBLICKEYSIZE], uint8_t ciphertext[CIPHERMLKEM1024_CIPHERTEXTSIZE], uint8_t sharedsecret[CIPHERMLKEM1024_SHAREDSECRETSIZE]);
    static bool Decapsulate(const uint8_t privatekey[CIPHERMLKEM1024_PRIVATEKEYSIZE], const uint8_t ciphertext[CIPHERMLKEM1024_CIPHERTEXTSIZE], uint8_t sharedsecret[CIPHERMLKEM1024_SHAREDSECRETSIZE]);
    static bool PublicKey_Check(const uint8_t publickey[CIPHERMLKEM1024_PUBLICKEYSIZE]);
};
