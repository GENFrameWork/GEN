/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTLSConfig.h
* 
* @class      DIOSTREAMTLSCONFIG
* @brief      Data Input/Output Stream TLS (Transport Layer Security) Config class
* @ingroup    DATAIO
* 
* @copyright  EndoraSoft. All rights reserved.
* 
* @cond
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
* documentation files(the "Software"), to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense, and/ or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of
* the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
* THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* @endcond
* 
* --------------------------------------------------------------------------------------------------------------------*/

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DIOIP.h"
#include "DIOURL.h"

#include "DIOStreamTCPIPConfig.h"
#include "DIOStreamConfigString.h"

#include "DIOStreamTLSMessages.h"



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/




/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class CIPHERKEY;


class DIOSTREAMTLSCONFIG  : public DIOSTREAMTCPIPCONFIG
{
  public:
                            DIOSTREAMTLSCONFIG                ();
    virtual                ~DIOSTREAMTLSCONFIG                ();

    XWORD                   GetCipherSuite                    ();
    void                    SetCipherSuite                    (XWORD ciphersuite);

    XVECTOR<XWORD>*         GetCipherSuites                   ();
    bool                    CipherSuite_Add                   (XWORD ciphersuite);
    bool                    CipherSuites_Delete               ();

    XVECTOR<XWORD>*         GetSupportedGroups                ();
    bool                    SupportedGroup_Add                (XWORD supportedgroup);
    bool                    SupportedGroups_Delete            ();

    XVECTOR<XWORD>*         GetSignatureSchemes               ();
    bool                    SignatureScheme_Add               (XWORD signaturescheme);
    bool                    SignatureSchemes_Delete           ();

    XVECTOR<XWORD>*         GetCertificateSignatureSchemes    ();
    bool                    CertificateSignatureScheme_Add    (XWORD signaturescheme);
    bool                    CertificateSignatureSchemes_Delete ();

    XVECTOR<DIOSTREAMTLS_ALPN_TYPE>* GetApplicationProtocols  ();
    bool                    ApplicationProtocol_Add           (DIOSTREAMTLS_ALPN_TYPE applicationprotocol);
    bool                    ApplicationProtocols_Delete       ();

    XSTRING*                GetServerName                     ();

    XVECTOR<XBUFFER*>*      GetTrustedRoots                   ();
    bool                    TrustedRoot_Add                   (XBUFFER& root);
    bool                    TrustedRoots_AddDefaults          ();
    bool                    TrustedRoots_Delete               ();

    XVECTOR<XBUFFER*>*      GetLocalCertificateChain          ();
    bool                    LocalCertificate_Add              (XBUFFER& certificate);
    bool                    LocalCertificates_Delete          ();

    CIPHERKEY*              GetLocalPrivateKey                ();
    bool                    SetLocalPrivateKey                (CIPHERKEY* privatekey);

    bool                    HasLocalCredentials               ();
    bool                    LocalCredentials_Delete           ();

    bool                    IsAllowUnauthenticatedServer      ();
    void                    SetAllowUnauthenticatedServer     (bool allowunauthenticatedserver);

    // Version negotiation window (Phase 5). Both default to TLS 1.3, i.e. the exact behavior of every previous
    // phase: nothing changes unless the caller explicitly widens the window. Lowering minversion to
    // DIOSTREAMTLS_MSG_VERSION_TLS_1_2 enables a TLS 1.2 fallback (via the parallel DIOSTREAMTLS12SESSION /
    // DIOSTREAMTLS12HANDSHAKECLIENT classes) for servers that do not speak TLS 1.3, and automatically enables the
    // two ECDHE-RSA-GCM suites those classes offer (DIOSTREAMTLS12_CIPHER_ECDHE_RSA_WITH_AES_*_GCM_SHA*) so the
    // hybrid ClientHello has something a TLS 1.2-only server can select. Only DIOSTREAMTLS_MSG_VERSION_TLS_1_2 and
    // DIOSTREAMTLS_MSG_VERSION_TLS_1_3 are valid values, and minversion must never be set above maxversion.
    XWORD                   GetMinVersion                     ();
    bool                    SetMinVersion                     (XWORD version);

    XWORD                   GetMaxVersion                     ();
    bool                    SetMaxVersion                     (XWORD version);

  protected:

  private:

    void                    Clean                             ();

    XWORD                   minversion;
    XWORD                   maxversion;

    XVECTOR<XWORD>          ciphersuites;
    XVECTOR<XWORD>          supportedgroups;
    XVECTOR<XWORD>          signatureschemes;
    XVECTOR<XWORD>          certificatesignatureschemes;
    XVECTOR<DIOSTREAMTLS_ALPN_TYPE> applicationprotocols;
    XSTRING                 servername;
    XVECTOR<XBUFFER*>       trustedroots;
    XVECTOR<XBUFFER*>       localcertificatechain;
    CIPHERKEY*              localprivatekey;
    bool                    allowunauthenticatedserver;
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
