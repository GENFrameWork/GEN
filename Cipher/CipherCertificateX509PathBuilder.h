/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherCertificateX509PathBuilder.h
* @class      CIPHERCERTIFICATEX509PATHBUILDER
* @brief      X.509 certification path discovery, independent from validation policy
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/
#pragma once

#include "XVector.h"
#include "CipherCertificateX509.h"

class CIPHERCERTIFICATEX509PATHBUILDER
{
  public:
                                            CIPHERCERTIFICATEX509PATHBUILDER ();
    virtual                                ~CIPHERCERTIFICATEX509PATHBUILDER ();

    bool                                    Build                              (XBUFFER& leaf,
                                                                                XVECTOR<XBUFFER*>* intermediates,
                                                                                XVECTOR<XBUFFER*>* trustedroots,
                                                                                XVECTOR<XBUFFER*>& path,
                                                                                XDWORD maximumdepth = 10);
    static void                             Path_Delete                        (XVECTOR<XBUFFER*>& path);

  private:
    bool                                    Search                             (CIPHERCERTIFICATEX509* current,
                                                                                XVECTOR<CIPHERCERTIFICATEX509*>& candidates,
                                                                                XVECTOR<CIPHERCERTIFICATEX509*>& roots,
                                                                                XVECTOR<XDWORD>& selected,
                                                                                XVECTOR<XDWORD>& result,
                                                                                XDWORD maximumdepth);
    bool                                    IsTrusted                          (CIPHERCERTIFICATEX509* certificate,
                                                                                XVECTOR<CIPHERCERTIFICATEX509*>& roots);
};

