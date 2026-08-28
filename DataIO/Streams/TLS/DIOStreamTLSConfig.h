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


enum DIOSTREAMTLS_CLIENTAUTHENTICATION_MODE
{
  DIOSTREAMTLS_CLIENTAUTHENTICATION_MODE_NONE = 0 ,
  DIOSTREAMTLS_CLIENTAUTHENTICATION_MODE_OPTIONAL ,
  DIOSTREAMTLS_CLIENTAUTHENTICATION_MODE_REQUIRED ,
};


#define DIOSTREAMTLS13_SESSIONTICKET_DEFAULT_LIFETIME      86400
#define DIOSTREAMTLS13_SESSIONTICKET_MAX_LIFETIME         604800
#define DIOSTREAMTLS13_SESSIONTICKET_MAX_CACHED                8


enum DIOSTREAMTLS_LOCALCREDENTIALSERROR
{
  DIOSTREAMTLS_LOCALCREDENTIALSERROR_NONE                         = 0 ,
  DIOSTREAMTLS_LOCALCREDENTIALSERROR_NOTCONFIGURED                   ,
  DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDCERTIFICATE              ,
  DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDDATE                     ,
  DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDKEYUSAGE                 ,
  DIOSTREAMTLS_LOCALCREDENTIALSERROR_UNSUPPORTEDKEY                  ,
  DIOSTREAMTLS_LOCALCREDENTIALSERROR_KEYMISMATCH                     ,
  DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDCHAIN                    ,
  DIOSTREAMTLS_LOCALCREDENTIALSERROR_INVALIDSERVERNAME               ,
};



/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class CIPHERKEY;
class CIPHERTRUSTPROVIDERX509;


class DIOSTREAMTLS13SESSIONTICKET
{
  public:
                            DIOSTREAMTLS13SESSIONTICKET       ();
    virtual                ~DIOSTREAMTLS13SESSIONTICKET       ();

    XSTRING*                GetServerName                      ();
    XBUFFER*                GetTicket                          ();
    XBUFFER*                GetPSK                             ();
    XDWORD                  GetTicketAgeAdd                    ();
    void                    SetTicketAgeAdd                    (XDWORD ageadd);
    XDWORD                  GetLifetime                        ();
    void                    SetLifetime                        (XDWORD lifetime);
    XQWORD                  GetReceivedEpoch                   ();
    void                    SetReceivedEpoch                   (XQWORD epoch);
    XWORD                   GetCipherSuite                     ();
    void                    SetCipherSuite                     (XWORD ciphersuite);
    DIOSTREAMTLS_ALPN_TYPE  GetApplicationProtocol             ();
    void                    SetApplicationProtocol             (DIOSTREAMTLS_ALPN_TYPE protocol);
    bool                    IsExpired                          ();
    XDWORD                  GetObfuscatedAge                   ();
    bool                    Delete                             ();

  private:

    void                    Clean                              ();

    XSTRING                 servername;
    XBUFFER                 ticket;
    XBUFFER                 psk;
    XDWORD                  ticketageadd;
    XDWORD                  lifetime;
    XQWORD                  receivedepoch;
    XWORD                   ciphersuite;
    DIOSTREAMTLS_ALPN_TYPE  applicationprotocol;
};


class DIOSTREAMTLSSERVERCREDENTIALS
{
  public:
                            DIOSTREAMTLSSERVERCREDENTIALS      ();
    virtual                ~DIOSTREAMTLSSERVERCREDENTIALS      ();

    XSTRING*                GetServerName                       ();

    XVECTOR<XBUFFER*>*      GetCertificateChain                ();
    bool                    Certificate_Add                    (XBUFFER& certificate);
    bool                    Certificates_Delete                ();

    CIPHERKEY*              GetPrivateKey                       ();
    bool                    SetPrivateKey                       (CIPHERKEY* privatekey);

    bool                    HasCredentials                      ();
    bool                    Delete                              ();

  private:

    void                    Clean                               ();

    XSTRING                 servername;
    XVECTOR<XBUFFER*>       certificatechain;
    CIPHERKEY*              privatekey;
};


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
    bool                    TrustedRoots_Load                 (CIPHERTRUSTPROVIDERX509& provider);
    bool                    TrustedRoots_AddDefaults          ();
    bool                    TrustedRoots_Delete               ();

    DIOSTREAMTLS_CLIENTAUTHENTICATION_MODE GetClientAuthenticationMode ();
    void                    SetClientAuthenticationMode       (DIOSTREAMTLS_CLIENTAUTHENTICATION_MODE mode);
    XVECTOR<XBUFFER*>*      GetClientTrustedRoots             ();
    bool                    ClientTrustedRoot_Add             (XBUFFER& root);
    bool                    ClientTrustedRoots_Load           (CIPHERTRUSTPROVIDERX509& provider);
    bool                    ClientTrustedRoots_Delete         ();

    XVECTOR<XBUFFER*>*      GetLocalCertificateChain          ();
    bool                    LocalCertificate_Add              (XBUFFER& certificate);
    bool                    LocalCertificates_Delete          ();

    CIPHERKEY*              GetLocalPrivateKey                ();
    bool                    SetLocalPrivateKey                (CIPHERKEY* privatekey);

    bool                    HasLocalCredentials               ();
    bool                    LocalCredentials_Validate         ();
    DIOSTREAMTLS_LOCALCREDENTIALSERROR GetLocalCredentialsError ();
    bool                    LocalCredentials_Delete           ();

    DIOSTREAMTLSSERVERCREDENTIALS* ServerCredentials_Add      (XCHAR* servername);
    XVECTOR<DIOSTREAMTLSSERVERCREDENTIALS*>* GetServerCredentials ();
    bool                    ServerCredentials_Select           (XCHAR* servername, XVECTOR<XBUFFER*>*& certificatechain, CIPHERKEY*& privatekey);
    bool                    ServerCredentials_Delete           ();

    bool                    IsAllowUnauthenticatedServer      ();
    void                    SetAllowUnauthenticatedServer     (bool allowunauthenticatedserver);

    bool                    IsActiveAIAFetch                  ();
    void                    AIAFetch_Activate                 (bool activate);

    int                     GetAIAFetchTimeout                ();
    bool                    SetAIAFetchTimeout                (int timeout);

    XWORD                   GetMinVersion                     ();
    bool                    SetMinVersion                     (XWORD version);

    XWORD                   GetMaxVersion                     ();
    bool                    SetMaxVersion                     (XWORD version);

    bool                    IsSessionResumptionActive         ();
    void                    SessionResumption_Activate        (bool active);
    XDWORD                  GetSessionTicketLifetime          ();
    bool                    SetSessionTicketLifetime          (XDWORD lifetime);
    bool                    SessionResumption_ServerInitialize ();
    bool                    SessionTicket_Store               (XCHAR* servername, XBUFFER& ticket, XBUFFER& PSK, XDWORD ageadd, XDWORD lifetime, XWORD ciphersuite, DIOSTREAMTLS_ALPN_TYPE applicationprotocol);
    DIOSTREAMTLS13SESSIONTICKET* SessionTicket_Get             (XCHAR* servername);
    bool                    SessionTickets_Delete             ();
    bool                    SessionTicket_Seal                (XBUFFER& PSK, XWORD ciphersuite, DIOSTREAMTLS_ALPN_TYPE applicationprotocol, XCHAR* servername, XDWORD lifetime, XDWORD ageadd, XBUFFER& ticket);
    bool                    SessionTicket_Open                (XBUFFER& ticket, XBUFFER& PSK, XWORD& ciphersuite, DIOSTREAMTLS_ALPN_TYPE& applicationprotocol, XSTRING& servername, XQWORD& issueepoch, XDWORD& lifetime, XDWORD& ageadd);

  protected:

  private:

    bool                    Credentials_Validate              (XVECTOR<XBUFFER*>* certificatechain, CIPHERKEY* privatekey);
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
    DIOSTREAMTLS_CLIENTAUTHENTICATION_MODE clientauthenticationmode;
    XVECTOR<XBUFFER*>       clienttrustedroots;
    XVECTOR<XBUFFER*>       localcertificatechain;
    CIPHERKEY*              localprivatekey;
    XVECTOR<DIOSTREAMTLSSERVERCREDENTIALS*> servercredentials;
    DIOSTREAMTLS_LOCALCREDENTIALSERROR localcredentialserror;
    bool                    allowunauthenticatedserver;

    bool                    aiafetchactive;
    int                     aiafetchtimeout;

    bool                    sessionresumptionactive;
    XDWORD                  sessionticketlifetime;
    XBUFFER                 sessionticketserverkey;
    XVECTOR<DIOSTREAMTLS13SESSIONTICKET*> sessiontickets;
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
