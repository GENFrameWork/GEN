/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamTLS13HandshakeServer.h
*
* @class      DIOSTREAMTLS13HANDSHAKESERVER
* @brief      Data Input/Output Stream TLS 1.3 Server Handshake class
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

#include "DIOStreamTLS13Session.h"
#include "DIOStreamTLSMessagesHandShakeServerFlight.h"
#include "DIOStreamTLSMessagesHandShakeClientHello.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


// v1 server scope (see DIOStreamTLS.h server Open() branch): TLS 1.3 only, RSA-PSS server certificate only,
// HelloRetryRequest for (EC)DHE key_share correction, no PSK/session resumption and no client certificate
// authentication. A CertificateRequest is never sent, so after the server flight the only message waited for
// is the client Finished.
enum DIOSTREAMTLS13HANDSHAKESERVER_STATE
{
  DIOSTREAMTLS13HANDSHAKESERVER_STATE_NONE                       = 0 ,
  DIOSTREAMTLS13HANDSHAKESERVER_STATE_WAIT_CLIENTHELLO_RETRY          ,
  DIOSTREAMTLS13HANDSHAKESERVER_STATE_WAIT_FINISHED                  ,
  DIOSTREAMTLS13HANDSHAKESERVER_STATE_HANDSHAKE_COMPLETED            ,
  DIOSTREAMTLS13HANDSHAKESERVER_STATE_ERROR                          ,
};




/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class DIOSTREAMTLSCONFIG;


class DIOSTREAMTLS13HANDSHAKESERVER
{
  public:
                                            DIOSTREAMTLS13HANDSHAKESERVER                    ();
    virtual                                ~DIOSTREAMTLS13HANDSHAKESERVER                    ();

    bool                                    Ini                                              (DIOSTREAMTLS13SESSION* session, DIOSTREAMTLSCONFIG* config);
    void                                    End                                              ();
    bool                                    IsIni                                            ();

    DIOSTREAMTLS13HANDSHAKESERVER_STATE     GetState                                         ();
    bool                                    IsHandshakeCompleted                             ();
    bool                                    IsWaitingClientHelloRetry                        ();

    bool                                    IsApplicationProtocolNegotiated                  ();
    DIOSTREAMTLS_ALPN_TYPE                  GetApplicationProtocol                           ();
    DIOSTREAMTLS_ALERT_DESCRIPTION          GetErrorAlertDescription                         ();
    bool                                    ShouldSendErrorAlert                             ();

    bool                                    ClientHello_Process                              (XBUFFER& clienthello, XBUFFER& records);

    bool                                    RecordInput_Add                                  (XBYTE* data, XDWORD size);
    bool                                    RecordInput_Add                                  (XBUFFER& data);
    bool                                    Process                                          ();
    bool                                    Handshake_Process                                (XBUFFER& message);

  private:

    bool                                    Finished_Process                                 (XBUFFER& message);
    bool                                    HelloRetryRequest_Create                         (DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO* clienthello, XBUFFER& clienthellobuffer, XWORD ciphersuite, XWORD group, XBUFFER& records);
    bool                                    ClientHelloRetry_Validate                        (DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO* clienthello);

    bool                                    CipherSuite_Select                               (XVECTOR<XWORD>& offered, XWORD& selected);
    bool                                    Group_Select                                     (DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO* clienthello, XWORD& selectedgroup, XBUFFER& peerpublickey, bool& helloretryrequestrequired);
    bool                                    SignatureScheme_Select                           (XVECTOR<XWORD>& offered, CIPHERKEY* leafpublickey, XWORD& selected);
    void                                    ApplicationProtocol_Select                       (XVECTOR<DIOSTREAMTLS_ALPN_TYPE>& offered);

    bool                                    SetError                                         (DIOSTREAMTLS_ALERT_DESCRIPTION alertdescription = DIOSTREAMTLS_ALERT_DESCRIPTION_INTERNAL_ERROR, bool sendalert = true);
    void                                    Clean                                            ();

    DIOSTREAMTLS13SESSION*                  session;
    DIOSTREAMTLSCONFIG*                     config;
    DIOSTREAMTLS13HANDSHAKESERVER_STATE     state;
    bool                                    isini;

    bool                                    applicationprotocolnegotiated;
    DIOSTREAMTLS_ALPN_TYPE                  applicationprotocol;
    DIOSTREAMTLS_ALERT_DESCRIPTION          erroralertdescription;
    bool                                    senderroralert;

    XBUFFER                                 firstclienthello;
    XWORD                                   retryselectedgroup;
    XWORD                                   retryciphersuite;
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
