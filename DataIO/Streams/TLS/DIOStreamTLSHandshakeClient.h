/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamTLSHandshakeClient.h
*
* @class      DIOSTREAMTLSHANDSHAKECLIENT
* @brief      Data Input/Output Stream TLS 1.3 Client Handshake class
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

#include "DIOStreamTLSSession.h"
#include "DIOStreamTLSMessagesHandShakeServerFlight.h"

#include "CipherCertificateX509Validator.h"



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


enum DIOSTREAMTLSHANDSHAKECLIENT_STATE
{
  DIOSTREAMTLSHANDSHAKECLIENT_STATE_NONE                       = 0 ,
  DIOSTREAMTLSHANDSHAKECLIENT_STATE_WAIT_SERVERHELLO               ,
  DIOSTREAMTLSHANDSHAKECLIENT_STATE_WAIT_ENCRYPTEDEXTENSIONS       ,
  DIOSTREAMTLSHANDSHAKECLIENT_STATE_WAIT_CERTIFICATE               ,
  DIOSTREAMTLSHANDSHAKECLIENT_STATE_WAIT_CERTIFICATEVERIFY         ,
  DIOSTREAMTLSHANDSHAKECLIENT_STATE_WAIT_FINISHED                  ,
  DIOSTREAMTLSHANDSHAKECLIENT_STATE_SERVERFINISHED_VERIFIED        ,
  DIOSTREAMTLSHANDSHAKECLIENT_STATE_HANDSHAKE_COMPLETED            ,
  DIOSTREAMTLSHANDSHAKECLIENT_STATE_ERROR                          ,
};


enum DIOSTREAMTLSHANDSHAKECLIENT_AUTHENTICATIONERROR
{
  DIOSTREAMTLSHANDSHAKECLIENT_AUTHENTICATIONERROR_NONE             = 0 ,
  DIOSTREAMTLSHANDSHAKECLIENT_AUTHENTICATIONERROR_CONFIGURATION        ,
  DIOSTREAMTLSHANDSHAKECLIENT_AUTHENTICATIONERROR_CERTIFICATE          ,
  DIOSTREAMTLSHANDSHAKECLIENT_AUTHENTICATIONERROR_CERTIFICATEVERIFY    ,
};




/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class DIOSTREAMTLSHANDSHAKECLIENT
{
  public:
                                            DIOSTREAMTLSHANDSHAKECLIENT                      ();
    virtual                                ~DIOSTREAMTLSHANDSHAKECLIENT                      ();

    bool                                    Ini                                              (DIOSTREAMTLSSESSION* session, bool allowunauthenticatedserver);
    void                                    End                                              ();
    bool                                    IsIni                                            ();

    DIOSTREAMTLSHANDSHAKECLIENT_STATE       GetState                                         ();
    bool                                    IsServerFinishedVerified                         ();
    bool                                    IsHandshakeCompleted                             ();
    bool                                    IsCertificateRequested                           ();
    bool                                    IsUnauthenticatedServerAllowed                   ();
    bool                                    IsServerAuthenticated                            ();

    bool                                    Authentication_Set                               (XCHAR* servername, XVECTOR<XBUFFER*>* trustedroots, XDATETIME* datetime = NULL);
    DIOSTREAMTLSHANDSHAKECLIENT_AUTHENTICATIONERROR GetAuthenticationError                   ();
    CIPHERCERTIFICATEX509VALIDATOR_ERROR    GetCertificateValidationError                    ();

    DIOSTREAMTLS_MSG_HANDSHAKE_CERTIFICATE* GetServerCertificate                            ();
    DIOSTREAMTLS_MSG_HANDSHAKE_CERTIFICATEVERIFY* GetServerCertificateVerify                 ();

    bool                                    ClientHello_Create                               (XCHAR* servername, XBUFFER& clienthello, XBUFFER& records);
    bool                                    Start                                            (XBUFFER& clienthello);
    bool                                    ServerHello_Process                              (XBUFFER& serverhello);
    bool                                    ServerHello_Process                              (XBUFFER& serverhello, XBUFFER& sharedsecret);
    bool                                    ClientFinished_Create                            (XBUFFER& clientfinished, XBUFFER& records);

    bool                                    RecordInput_Add                                  (XBYTE* data, XDWORD size);
    bool                                    RecordInput_Add                                  (XBUFFER& data);
    bool                                    Process                                          ();
    bool                                    Handshake_Process                                (XBUFFER& message);

  private:

    bool                                    EncryptedExtensions_Process                      (XBUFFER& message);
    bool                                    CertificateRequest_Process                       (XBUFFER& message);
    bool                                    Certificate_Process                              (XBUFFER& message);
    bool                                    CertificateVerify_Process                        (XBUFFER& message);
    bool                                    Finished_Process                                 (XBUFFER& message);

    bool                                    SetError                                         ();
    bool                                    SetAuthenticationError                           (DIOSTREAMTLSHANDSHAKECLIENT_AUTHENTICATIONERROR error);
    void                                    Clean                                            ();

    DIOSTREAMTLSSESSION*                    session;
    DIOSTREAMTLSHANDSHAKECLIENT_STATE       state;
    bool                                    isini;
    bool                                    allowunauthenticatedserver;
    bool                                    certificaterequested;
    bool                                    authenticationconfigured;
    bool                                    serverauthenticated;

    DIOSTREAMTLSHANDSHAKECLIENT_AUTHENTICATIONERROR authenticationerror;
    CIPHERCERTIFICATEX509VALIDATOR_ERROR    certificatevalidationerror;

    XSTRING                                 expectedservername;
    XVECTOR<XBUFFER*>                       trustedroots;
    XDATETIME                               validationdatetime;
    bool                                    hasvalidationdatetime;

    CIPHERCERTIFICATEX509VALIDATOR          certificatevalidator;

    XBUFFER                                 legacysessionid;

    DIOSTREAMTLS_MSG_HANDSHAKE_CERTIFICATE* servercertificate;
    DIOSTREAMTLS_MSG_HANDSHAKE_CERTIFICATEVERIFY* servercertificateverify;
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/



