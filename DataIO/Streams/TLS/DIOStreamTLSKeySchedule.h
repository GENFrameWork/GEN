/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamTLSKeySchedule.h
*
* @class      DIOSTREAMTLSKEYSCHEDULE
* @brief      Data Input/Output Stream TLS Key Schedule (TLS 1.3, RFC 8446 section 7.1) class
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

#include "Hash.h"
#include "CipherHKDF.h"

#include "DIOStreamTLSMessages.h"



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


#define DIOSTREAMTLSKEYSCHEDULE_IVSIZE                        12                // Fixed by RFC 8446, section 5.3, for every cipher suite

// Dumping key material to the trace is only ever a debugging aid, for example to follow the trace of the RFC 8448.
// It must never be left active in a delivered build: it writes the secrets of the connection in the clear.
//#define DIOSTREAMTLSKEYSCHEDULE_TRACE_SECRETS

#define DIOSTREAMTLSKEYSCHEDULE_LABEL_DERIVED                 __L("derived")
#define DIOSTREAMTLSKEYSCHEDULE_LABEL_CLIENTHANDSHAKE         __L("c hs traffic")
#define DIOSTREAMTLSKEYSCHEDULE_LABEL_SERVERHANDSHAKE         __L("s hs traffic")
#define DIOSTREAMTLSKEYSCHEDULE_LABEL_CLIENTAPPLICATION       __L("c ap traffic")
#define DIOSTREAMTLSKEYSCHEDULE_LABEL_SERVERAPPLICATION       __L("s ap traffic")
#define DIOSTREAMTLSKEYSCHEDULE_LABEL_RESUMPTION              __L("res master")
#define DIOSTREAMTLSKEYSCHEDULE_LABEL_KEY                     __L("key")
#define DIOSTREAMTLSKEYSCHEDULE_LABEL_IV                      __L("iv")
#define DIOSTREAMTLSKEYSCHEDULE_LABEL_FINISHED                __L("finished")
#define DIOSTREAMTLSKEYSCHEDULE_LABEL_TRAFFICUPDATE           __L("traffic upd")


enum DIOSTREAMTLSKEYSCHEDULE_ROLE
{
  DIOSTREAMTLSKEYSCHEDULE_ROLE_CLIENT               = 0 ,
  DIOSTREAMTLSKEYSCHEDULE_ROLE_SERVER                   ,
};


// Every secret is asked for by direction, never by role. This is what lets the server of the second phase reuse
// this class untouched: only the role given to Ini() changes.

enum DIOSTREAMTLSKEYSCHEDULE_DIRECTION
{
  DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL           = 0 ,                       // What this end writes
  DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE              ,                       // What the other end writes

  DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS
};


enum DIOSTREAMTLSKEYSCHEDULE_LEVEL
{
  DIOSTREAMTLSKEYSCHEDULE_LEVEL_NONE                = 0 ,
  DIOSTREAMTLSKEYSCHEDULE_LEVEL_HANDSHAKE               ,
  DIOSTREAMTLSKEYSCHEDULE_LEVEL_APPLICATION             ,
};




/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class DIOSTREAMTLSKEYSCHEDULE
{
  public:
                                            DIOSTREAMTLSKEYSCHEDULE                           ();
    virtual                                ~DIOSTREAMTLSKEYSCHEDULE                           ();

    bool                                    Ini                                               (XWORD ciphersuite, DIOSTREAMTLSKEYSCHEDULE_ROLE role);
    void                                    End                                               ();

    bool                                    IsIni                                             ();

    XWORD                                   GetCipherSuite                                    ();
    DIOSTREAMTLSKEYSCHEDULE_ROLE            GetRole                                           ();

    XDWORD                                  GetHashSize                                       ();
    XDWORD                                  GetKeySize                                        ();
    XDWORD                                  GetIVSize                                         ();

    HASH*                                   GetHash                                           ();
    CIPHERHKDF*                             GetHKDF                                           ();

    bool                                    TranscriptHash                                    (XBUFFER& messages, XBUFFER& transcripthash);

    bool                                    EarlySecret_Calculate                             (XBUFFER* PSK = NULL);
    bool                                    HandshakeSecret_Calculate                         (XBUFFER& sharedsecret);
    bool                                    MasterSecret_Calculate                            ();

    bool                                    HandshakeTrafficSecrets_Calculate                 (XBUFFER& transcripthash);
    bool                                    ApplicationTrafficSecrets_Calculate               (XBUFFER& transcripthash);
    bool                                    ResumptionSecret_Calculate                        (XBUFFER& transcripthash);

    XBUFFER*                                GetEarlySecret                                    ();
    XBUFFER*                                GetHandshakeSecret                                ();
    XBUFFER*                                GetMasterSecret                                   ();
    XBUFFER*                                GetResumptionSecret                               ();

    XBUFFER*                                GetTrafficSecret                                  (DIOSTREAMTLSKEYSCHEDULE_LEVEL level, DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction);

    bool                                    GetTrafficKeys                                    (DIOSTREAMTLSKEYSCHEDULE_LEVEL level, DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction, XBUFFER& key, XBUFFER& IV);
    bool                                    GetFinishedKey                                    (DIOSTREAMTLSKEYSCHEDULE_LEVEL level, DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction, XBUFFER& finishedkey);

    bool                                    CalculateFinished                                 (DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction, XBUFFER& transcripthash, XBUFFER& verifydata);
    bool                                    VerifyFinished                                    (DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction, XBUFFER& transcripthash, XBUFFER& verifydata);

    bool                                    UpdateTrafficSecret                               (DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction);

  private:

    bool                                    IsDirectionOfTheClient                            (DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction);
    bool                                    DeriveEmptySecret                                 (XBUFFER& secret, XCHAR* label, XBUFFER& derivedsecret);

    void                                    Clean                                             ();

    XWORD                                   ciphersuite;
    DIOSTREAMTLSKEYSCHEDULE_ROLE            role;
    bool                                    isini;

    XDWORD                                  hashsize;
    XDWORD                                  keysize;
    XDWORD                                  IVsize;

    HASH*                                   hash;
    CIPHERHKDF*                             HKDF;

    XBUFFER                                 earlysecret;
    XBUFFER                                 handshakesecret;
    XBUFFER                                 mastersecret;
    XBUFFER                                 resumptionsecret;

    XBUFFER                                 clienthandshaketrafficsecret;
    XBUFFER                                 serverhandshaketrafficsecret;
    XBUFFER                                 clientapplicationtrafficsecret;
    XBUFFER                                 serverapplicationtrafficsecret;
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/



