/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherCertificateX509PathBuilder.h
* @class      CIPHERCERTIFICATEX509PATHBUILDER
* @brief      X.509 certification path discovery, independent from validation policy
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/
#pragma once

#include "XVector.h"
#include "CipherCertificateX509.h"

#define CIPHERCERTIFICATEX509PATHBUILDER_MAX_PATHS  32
#define CIPHERCERTIFICATEX509PATHBUILDER_MAX_SEARCH_NODES  4096

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
    bool                                    BuildAll                           (XBUFFER& leaf,
                                                                                XVECTOR<XBUFFER*>* intermediates,
                                                                                XVECTOR<XBUFFER*>* trustedroots,
                                                                                XVECTOR<XVECTOR<XBUFFER*>*>& paths,
                                                                                XDWORD maximumdepth = 10,
                                                                                XDWORD maximumpaths = CIPHERCERTIFICATEX509PATHBUILDER_MAX_PATHS);
    static void                             Path_Delete                        (XVECTOR<XBUFFER*>& path);
    static void                             Paths_Delete                       (XVECTOR<XVECTOR<XBUFFER*>*>& paths);

  private:
    bool                                    SearchAll                          (CIPHERCERTIFICATEX509* current,
                                                                                XVECTOR<CIPHERCERTIFICATEX509*>& candidates,
                                                                                XVECTOR<CIPHERCERTIFICATEX509*>& roots,
                                                                                XVECTOR<XDWORD>& selected,
                                                                                XVECTOR<XVECTOR<XDWORD>*>& results,
                                                                                XDWORD maximumdepth,
                                                                                XDWORD maximumpaths,
                                                                                XDWORD& searchednodes);
    bool                                    IsTrusted                          (CIPHERCERTIFICATEX509* certificate,
                                                                                XVECTOR<CIPHERCERTIFICATEX509*>& roots);
};
