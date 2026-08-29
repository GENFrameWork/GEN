#pragma once

#include <stdint.h>
#include <stddef.h>

#define CIPHERMLKEM768_PUBLICKEYSIZE      1184
#define CIPHERMLKEM768_PRIVATEKEYSIZE     2400
#define CIPHERMLKEM768_CIPHERTEXTSIZE     1088
#define CIPHERMLKEM768_SHAREDSECRETSIZE   32
#define CIPHERMLKEM768_SEEDSIZE           32

class CIPHERMLKEM768CORE
{
  public:
    static bool KeyPair(const uint8_t d[32], const uint8_t z[32], uint8_t publickey[CIPHERMLKEM768_PUBLICKEYSIZE], uint8_t privatekey[CIPHERMLKEM768_PRIVATEKEYSIZE]);
    static bool Encapsulate(const uint8_t randomness[32], const uint8_t publickey[CIPHERMLKEM768_PUBLICKEYSIZE], uint8_t ciphertext[CIPHERMLKEM768_CIPHERTEXTSIZE], uint8_t sharedsecret[CIPHERMLKEM768_SHAREDSECRETSIZE]);
    static bool Decapsulate(const uint8_t privatekey[CIPHERMLKEM768_PRIVATEKEYSIZE], const uint8_t ciphertext[CIPHERMLKEM768_CIPHERTEXTSIZE], uint8_t sharedsecret[CIPHERMLKEM768_SHAREDSECRETSIZE]);
    static bool PublicKey_Check(const uint8_t publickey[CIPHERMLKEM768_PUBLICKEYSIZE]);
};
