/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamTLS13Session.h
*
* @class      DIOSTREAMTLS13SESSION
* @brief      Data Input/Output Stream TLS 1.3 role-neutral Session class
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

#include "XBuffer.h"

#include "CipherECDSA.h"
#include "CipherECDSAX25519.h"

#include "DIOStreamTLS13KeySchedule.h"
#include "DIOStreamTLSRecord.h"



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


#define DIOSTREAMTLS13SESSION_MAXHANDSHAKESIZE                    DIOSTREAMTLS_MSG_MAXHANDSHAKESIZE
#define DIOSTREAMTLS13SESSION_MAXRECORDINPUTSIZE                  (4*1024*1024)
#define DIOSTREAMTLS13SESSION_MAXLOCALKEYUPDATES                  ((((XQWORD)1) << 48) - 1)


enum DIOSTREAMTLS13SESSION_RESULT
{
  DIOSTREAMTLS13SESSION_RESULT_ERROR       = -1 ,
  DIOSTREAMTLS13SESSION_RESULT_INCOMPLETE  =  0 ,
  DIOSTREAMTLS13SESSION_RESULT_COMPLETE         ,
};


enum DIOSTREAMTLS13SESSION_EPOCH
{
  DIOSTREAMTLS13SESSION_EPOCH_CLEAR        = 0 ,
  DIOSTREAMTLS13SESSION_EPOCH_HANDSHAKE        ,
  DIOSTREAMTLS13SESSION_EPOCH_APPLICATION      ,
};




/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class DIOSTREAMTLS13SESSION
{
  public:
                                            DIOSTREAMTLS13SESSION                              ();
    virtual                                ~DIOSTREAMTLS13SESSION                              ();

    bool                                    Ini                                              (XWORD ciphersuite, DIOSTREAMTLSKEYSCHEDULE_ROLE role);
    void                                    End                                              ();
    bool                                    IsIni                                            ();

    DIOSTREAMTLSKEYSCHEDULE_ROLE            GetRole                                          ();
    DIOSTREAMTLS13SESSION_EPOCH               GetEpoch                                         (DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction);

    DIOSTREAMTLS13KEYSCHEDULE*                GetKeySchedule                                   ();
    DIOSTREAMTLSRECORD*                     GetRecord                                         ();
    CIPHERECDSAX25519*                      GetKeyExchange                                    ();
    bool                                    KeyExchange_Generate                              (XWORD group, XBUFFER& publickey);
    bool                                    KeyExchange_SharedSecret                          (XWORD group, XBUFFER& publickey, XBUFFER& sharedsecret);
    void                                    KeyExchange_Delete                                ();
    bool                                    CipherSuite_Select                                (XWORD ciphersuite);

    XBUFFER*                                GetRecordInput                                    ();
    bool                                    RecordInput_Add                                  (XBYTE* data, XDWORD size);
    bool                                    RecordInput_Add                                  (XBUFFER& data);
    DIOSTREAMTLS13SESSION_RESULT              Record_Extract                                   (DIOSTREAMTLS_CONTENTTYPE& contenttype, XBUFFER& plain);
    DIOSTREAMTLS_ALERT_DESCRIPTION          GetLastRecordAlertDescription                     ();

    XBUFFER*                                GetHandshakeInput                                 ();
    bool                                    HandshakeInput_Add                               (XBYTE* data, XDWORD size);
    bool                                    HandshakeInput_Add                               (XBUFFER& data);
    DIOSTREAMTLS13SESSION_RESULT              Handshake_Extract                                (XBUFFER& message);

    XBUFFER*                                GetTranscript                                    ();
    bool                                    Transcript_Add                                   (XBUFFER& message);
    bool                                    TranscriptHash                                   (XBUFFER& transcripthash);

    bool                                    HandshakeKeys_Activate                           (XBUFFER& sharedsecret, XBUFFER* PSK = NULL);
    bool                                    ApplicationTrafficSecrets_Calculate              ();
    bool                                    ApplicationKeys_Activate                         (DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction);

    XBUFFER*                                GetApplicationInput                              ();
    bool                                    ApplicationData_Protect                          (XBYTE* data, XDWORD size, XBUFFER& records);
    bool                                    ApplicationData_Protect                          (XBUFFER& data, XBUFFER& records);
    XDWORD                                  ApplicationData_Read                             (XBYTE* data, XDWORD size);
    DIOSTREAMTLS13SESSION_RESULT              ApplicationData_Process                          ();
    bool                                    KeyUpdate_Create                                 (bool requestpeer, XBUFFER& records);
    bool                                    PostHandshakeOutput_Add                           (XBUFFER& records);
    bool                                    PostHandshakeOutput_Extract                       (XBUFFER& records);
    bool                                    NewSessionTicket_Extract                          (XBUFFER& message);

    bool                                    Alert_Create                                     (DIOSTREAMTLS_ALERT_LEVEL level, DIOSTREAMTLS_ALERT_DESCRIPTION description, XBUFFER& records);
    bool                                    CloseNotify_Create                               (XBUFFER& records);

    bool                                    IsCloseNotifySent                                ();
    bool                                    IsCloseNotifyReceived                            ();
    bool                                    IsError                                          ();
    bool                                    IsTransportClosedWithoutNotify                   ();
    DIOSTREAMTLS_ALERT_LEVEL                GetReceivedAlertLevel                            ();
    DIOSTREAMTLS_ALERT_DESCRIPTION          GetReceivedAlertDescription                      ();

    bool                                    TransportClosed                                  ();

  private:

    bool                                    KeyUpdate_Process                                (DIOSTREAMTLS_MSG_HANDSHAKE& handshake);
    void                                    Clean                                            ();

    DIOSTREAMTLSKEYSCHEDULE_ROLE            role;
    bool                                    isini;
    bool                                    applicationsecretscalculated;

    DIOSTREAMTLS13SESSION_EPOCH               epoch[DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS];

    DIOSTREAMTLS13KEYSCHEDULE                 keyschedule;
    DIOSTREAMTLSRECORD                      record;
    CIPHERECDSAX25519                       keyexchange;
    CIPHERECDSA                             keyexchangep256;
    XSECUREBUFFER                           keyexchangep256private;
    XBUFFER                                 keyexchangep256public;
    CIPHERECDSA                             keyexchangep384;
    XSECUREBUFFER                           keyexchangep384private;
    XBUFFER                                 keyexchangep384public;

    XBUFFER                                 recordinput;
    DIOSTREAMTLS_ALERT_DESCRIPTION          lastrecordalertdescription;
    XBUFFER                                 handshakeinput;
    XBUFFER                                 transcript;
    XBUFFER                                 applicationinput;
    XBUFFER                                 posthandshakeoutput;
    XBUFFER                                 newsessionticketinput;

    XQWORD                                  keyupdates[DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS];
    bool                                    keyupdaterequestpending;
    bool                                    keyupdateresponsepending;

    bool                                    closenotifysent;
    bool                                    closenotifyreceived;
    bool                                    iserror;
    bool                                    transportclosedwithoutnotify;
    DIOSTREAMTLS_ALERT_LEVEL                receivedalertlevel;
    DIOSTREAMTLS_ALERT_DESCRIPTION          receivedalertdescription;
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
