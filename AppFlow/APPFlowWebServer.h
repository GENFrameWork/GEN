/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       APPFlowWebServer.h
* 
* @class      APPFLOWWEBSERVER
* @brief      Application Flow Web Server class
* @ingroup    APPFLOW
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

#include "XString.h"
#include "XPath.h"
#include "XObserver.h"

#include "DIOWebServer_QueryStrings.h"
#include "DIOWebPageHTMLCreator.h"
#include "DIOWebServer.h"

#ifdef DIO_STREAMTLS_ACTIVE
#include "CipherKey.h"
#endif



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/




/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class XEVENT;
class XBUFFER;
class DIOWEBSERVER_PLUGIN_PHP;
class DIOWEBSERVER_XEVENT;
class DIOWEBSERVER;
class APPFLOWCFG;
class DIOSTREAMTLSCONFIG;


class APPFLOWWEBSERVER : public XOBSERVER, public XSUBJECT
{
  public:
                                APPFLOWWEBSERVER                  ();
    virtual                    ~APPFLOWWEBSERVER                  ();

    bool                        Ini                               (APPFLOWCFG* cfg, bool doinitialconnectitivitytest, bool isapirestonly, bool chekuseragentid);
    bool                        Ini_Authentication                (APPFLOWCFG* cfg);
    bool                        Ini                               (XDWORD port, bool doinitialconnectitivitytest, int timeoutserverpage, XSTRING* addrlocal);

    // Secondary listener on its own port (e.g. WebSocket) that follows the same cfg->WebServer_IsTLS() decision
    // and credentials as the main Ini(APPFLOWCFG*, ...) listener above, instead of always running plain.
    bool                        Ini                               (APPFLOWCFG* cfg, XDWORD port, int timeoutserverpage, XSTRING* addrlocal);

    // HTTPS variant: see DIOWEBSERVER::Ini(DIOSTREAMTLSCONFIG*, ...) for the ownership and credential
    // requirements on tlsconfig (ownership transfers to the internal DIOWEBSERVER instance).
    bool                        Ini                               (DIOSTREAMTLSCONFIG* tlsconfig, XDWORD port, int timeoutserverpage, XSTRING* addrlocal);

    DIOWEBSERVER*               GetWebServer                      ();

    bool                        GetIsAuthenticatedAccess          ();
    bool                        GetIsApiRestOnly                  ();

    virtual bool                SendRequest                       (DIOWEBSERVER_CONNECTION* connection, DIOWEBHEADER_RESULT headerresult, XBUFFER* data , int timeout, XSTRING* addhead = NULL);
    bool                        SendRequest                       (DIOWEBSERVER_CONNECTION* connection, DIOWEBHEADER_RESULT headerresult, XSTRING& data , int timeout, XSTRING* addhead = NULL);
    bool                        SendRequest                       (DIOWEBSERVER_CONNECTION* connection, DIOWEBHEADER_RESULT headerresult, XPATH& file   , int timeout, XSTRING* addhead = NULL);

    virtual bool                ResolveRequest                    (DIOWEBSERVER* server, DIOWEBSERVER_CONNECTION* connection, DIOWEBSERVER_REQUEST*  request);

    virtual bool                GenerateMessagePage               (XCHAR* leyend, DIOWEBPAGEHTMLCREATOR& webHTMLpage);
    bool                        GenerateMessagePage               (XSTRING& leyend, DIOWEBPAGEHTMLCREATOR& webHTMLpage);
    bool                        GenerateResponse_Error            (DIOWEBSERVER_CONNECTION* connection, DIOWEBHEADER_RESULT result, XCHAR* leyend);
    bool                        GenerateResponse_Error            (DIOWEBSERVER_CONNECTION* connection, DIOWEBHEADER_RESULT result, XSTRING& leyend);
    bool                        GenerateResponse_Unauthorized     (DIOWEBSERVER_CONNECTION* connection);

    bool                        End                               ();

  private:

    void                        HandleEvent_WebServer             (DIOWEBSERVER_XEVENT* event);
    void                        HandleEvent                       (XEVENT* xevent);

    void                        Ini_RegisterEvents                ();

    #ifdef DIO_STREAMTLS_ACTIVE
    // Resolves a WebServer_PathPrivateKey()/WebServer_PathCertificate() configuration field to the actual file
    // to open. Unlike WebServer_PathResources() (which may be completely empty, see ResolveRequest()), this
    // field can never be totally empty: it must at least carry the file name. If cfgpath only carries a file
    // name (no directory part of its own), it is resolved against XPATHSMANAGERSECTIONTYPE_CERTIFICATES -- the
    // usual place certificate/key files live. If cfgpath already carries a directory part (relative or
    // absolute) in addition to the file name, it is used as-is, exactly as before -- this is what lets the
    // credentials be loaded from any other directory when needed. Fails if cfgpath is NULL/empty or does not
    // even carry a file name.
    bool                        Ini_ResolveCertificatePath         (XPATH* cfgpath, XPATH& resolvedpath);

    // Reads the whole file at path into filedata (raw bytes) -- used to sniff the format (CIPHERPEMCODEC::IsPEM())
    // before deciding how to parse a credential file.
    bool                        Ini_ReadFile                        (XPATH& path, XBUFFER& filedata);

    // Reads the whole file at path as text lines into lines (newly allocated XSTRING*, caller must
    // DeleteContents()+DeleteAll() when done) -- used to feed CIPHERPEMCODEC::PrivateKeyBlock_Decode().
    bool                        Ini_ReadLines                       (XPATH& path, XVECTOR<XSTRING*>& lines);

    // Loads an RSA private key file and sets it as the local private key of tlsconfig. Accepts, auto-detected:
    // PKCS#1 PEM ("-----BEGIN RSA PRIVATE KEY-----"), unencrypted PKCS#8 PEM ("-----BEGIN PRIVATE KEY-----"), or
    // GEN's own legacy plain text format (3 lines in hexadecimal: prime1factor, prime2factor, exponent -- see
    // CIPHERKEYPRIVATERSA::Set). Used by Ini_LoadTLSCredentials() once the certificate's public key type is
    // known to be RSA.
    bool                        Ini_LoadTLSPrivateKey_RSA           (XPATH* pathkey, DIOSTREAMTLSCONFIG* tlsconfig);

    // Loads an ECDSA private key file and sets it as the local private key of tlsconfig. Accepts, auto-detected:
    // SEC1 PEM ("-----BEGIN EC PRIVATE KEY-----"), unencrypted PKCS#8 PEM ("-----BEGIN PRIVATE KEY-----"), or
    // GEN's own legacy plain text format (1 line in hexadecimal: the private scalar D). Used by
    // Ini_LoadTLSCredentials() once the certificate's public key type/curve is known.
    bool                        Ini_LoadTLSPrivateKey_ECDSA        (XPATH* pathkey, CIPHERKEYTYPE privatekeytype, XDWORD coordinatesize, DIOSTREAMTLSCONFIG* tlsconfig);

    // Loads the private key / certificate configured in cfg (APPFLOWCFG::WebServer_PathPrivateKey() /
    // WebServer_PathCertificate()) into tlsconfig. Used by Ini(APPFLOWCFG*, ...) when cfg->WebServer_IsTLS().
    // The certificate is decoded first: its public key type (RSA or one of the supported ECDSA curves) drives
    // the private key file format expected (see Ini_LoadTLSPrivateKey_ECDSA() for the ECDSA case).
    bool                        Ini_LoadTLSCredentials            (APPFLOWCFG* cfg, DIOSTREAMTLSCONFIG* tlsconfig);

    // Builds a DIOSTREAMTLSCONFIG (cipher suite / group / signature scheme / ALPN + credentials via
    // Ini_LoadTLSCredentials()) from cfg, ready to hand to Ini(DIOSTREAMTLSCONFIG*, ...). Returns NULL if the
    // credentials could not be loaded. Ownership of the returned object belongs to the caller.
    DIOSTREAMTLSCONFIG*         Ini_BuildTLSConfig                 (APPFLOWCFG* cfg);
    #endif

    void                        Clean                             ();

    APPFLOWCFG*                 cfg;

    DIOWEBSERVER*               webserver;

    bool                        isauthenticatedaccess;
    bool                        isapirestonly;

    XSTRING                     page_login;
    XSTRING                     page_password;

    DIOWEBSERVER_PLUGIN_PHP*    pluginPHP;
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/





