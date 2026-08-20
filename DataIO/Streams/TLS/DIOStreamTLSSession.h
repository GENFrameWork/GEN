/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamTLSSession.h
*
* @class      DIOSTREAMTLSSESSION
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

#include "CipherECDSAX25519.h"

#include "DIOStreamTLSKeySchedule.h"
#include "DIOStreamTLSRecord.h"



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


enum DIOSTREAMTLSSESSION_RESULT
{
  DIOSTREAMTLSSESSION_RESULT_ERROR       = -1 ,
  DIOSTREAMTLSSESSION_RESULT_INCOMPLETE  =  0 ,
  DIOSTREAMTLSSESSION_RESULT_COMPLETE         ,
};


enum DIOSTREAMTLSSESSION_EPOCH
{
  DIOSTREAMTLSSESSION_EPOCH_CLEAR        = 0 ,
  DIOSTREAMTLSSESSION_EPOCH_HANDSHAKE        ,
  DIOSTREAMTLSSESSION_EPOCH_APPLICATION      ,
};




/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class DIOSTREAMTLSSESSION
{
  public:
                                            DIOSTREAMTLSSESSION                              ();
    virtual                                ~DIOSTREAMTLSSESSION                              ();

    bool                                    Ini                                              (XWORD ciphersuite, DIOSTREAMTLSKEYSCHEDULE_ROLE role);
    void                                    End                                              ();
    bool                                    IsIni                                            ();

    DIOSTREAMTLSKEYSCHEDULE_ROLE            GetRole                                          ();
    DIOSTREAMTLSSESSION_EPOCH               GetEpoch                                         (DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction);

    DIOSTREAMTLSKEYSCHEDULE*                GetKeySchedule                                   ();
    DIOSTREAMTLSRECORD*                     GetRecord                                         ();
    CIPHERECDSAX25519*                      GetKeyExchange                                    ();

    XBUFFER*                                GetRecordInput                                    ();
    bool                                    RecordInput_Add                                  (XBYTE* data, XDWORD size);
    bool                                    RecordInput_Add                                  (XBUFFER& data);
    DIOSTREAMTLSSESSION_RESULT              Record_Extract                                   (DIOSTREAMTLS_CONTENTTYPE& contenttype, XBUFFER& plain);

    XBUFFER*                                GetHandshakeInput                                 ();
    bool                                    HandshakeInput_Add                               (XBYTE* data, XDWORD size);
    bool                                    HandshakeInput_Add                               (XBUFFER& data);
    DIOSTREAMTLSSESSION_RESULT              Handshake_Extract                                (XBUFFER& message);

    XBUFFER*                                GetTranscript                                    ();
    bool                                    Transcript_Add                                   (XBUFFER& message);
    bool                                    TranscriptHash                                   (XBUFFER& transcripthash);

    bool                                    HandshakeKeys_Activate                           (XBUFFER& sharedsecret);
    bool                                    ApplicationTrafficSecrets_Calculate              ();
    bool                                    ApplicationKeys_Activate                         (DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction);

    XBUFFER*                                GetApplicationInput                              ();
    bool                                    ApplicationData_Protect                          (XBYTE* data, XDWORD size, XBUFFER& records);
    bool                                    ApplicationData_Protect                          (XBUFFER& data, XBUFFER& records);
    XDWORD                                  ApplicationData_Read                             (XBYTE* data, XDWORD size);
    DIOSTREAMTLSSESSION_RESULT              ApplicationData_Process                          ();

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

    void                                    Clean                                            ();

    DIOSTREAMTLSKEYSCHEDULE_ROLE            role;
    bool                                    isini;
    bool                                    applicationsecretscalculated;

    DIOSTREAMTLSSESSION_EPOCH               epoch[DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS];

    DIOSTREAMTLSKEYSCHEDULE                 keyschedule;
    DIOSTREAMTLSRECORD                      record;
    CIPHERECDSAX25519                       keyexchange;

    XBUFFER                                 recordinput;
    XBUFFER                                 handshakeinput;
    XBUFFER                                 transcript;
    XBUFFER                                 applicationinput;

    bool                                    closenotifysent;
    bool                                    closenotifyreceived;
    bool                                    iserror;
    bool                                    transportclosedwithoutnotify;
    DIOSTREAMTLS_ALERT_LEVEL                receivedalertlevel;
    DIOSTREAMTLS_ALERT_DESCRIPTION          receivedalertdescription;
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/




