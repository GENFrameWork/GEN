/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherCertificateX509Revocation.h
* @brief      Signed OCSP/CRL validation primitives for X.509 consumers
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/
#pragma once
#include "CipherCertificateX509.h"

#define CIPHERCERTIFICATEX509REVOCATION_MAX_OCSP_SIZE   (256*1024)
#define CIPHERCERTIFICATEX509REVOCATION_MAX_CRL_SIZE    (4*1024*1024)

enum CIPHERCERTIFICATEX509REVOCATION_RESULT
{
  CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID = 0,
  CIPHERCERTIFICATEX509REVOCATION_RESULT_GOOD,
  CIPHERCERTIFICATEX509REVOCATION_RESULT_REVOKED,
  CIPHERCERTIFICATEX509REVOCATION_RESULT_UNKNOWN
};

class CIPHERCERTIFICATEX509REVOCATION
{
  public:
    static CIPHERCERTIFICATEX509REVOCATION_RESULT ValidateOCSP (XBUFFER& response, CIPHERCERTIFICATEX509& certificate,
                                                                CIPHERCERTIFICATEX509& issuer);
    static CIPHERCERTIFICATEX509REVOCATION_RESULT ValidateCRL  (XBUFFER& CRL, CIPHERCERTIFICATEX509& certificate,
                                                                CIPHERCERTIFICATEX509& issuer);
};
