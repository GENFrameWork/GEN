/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStream.h
* 
* @class      DIOSTREAM
* @brief      Data Input/Output Stream class
* @ingroup    DATAIO
* 
* @copyright  GEN Group. All rights reserved.
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

#ifndef _DIOSTREAM_H_
#define _DIOSTREAM_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XSleep.h"
#include "XUUID.h"
#include "XString.h"
#include "XSubject.h"
#include "XTimer.h"

#include "DIOStreamConfig.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


enum DIOSTREAMSTATUS
{
  DIOSTREAMSTATUS_DISCONNECTED          = 0 ,
  DIOSTREAMSTATUS_CONNECTED                 ,
  DIOSTREAMSTATUS_GETTINGCONNECTION         ,
};

enum DIOSTREAMERROR
{
  DIOSTREAMERROR_NONE                   = 0 ,
  DIOSTREAMERROR_UNKNOWN                    ,
  DIOSTREAMERROR_URLNOTVALID                ,
  DIOSTREAMERROR_ADDRINUSE                  ,
};



#ifndef MICROCONTROLLER
#define DIOSTREAM_MAXBUFFER             1024*10  // 10k
#else
#define DIOSTREAM_MAXBUFFER             32
#endif

#define DIOSTREAM_SOMETHINGTOREAD       -1

#define DIOSTREAM_TIMEINWAITFUNCTIONS   10       // milliseconds


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class XBUFFER;
class DIOSTREAMCONFIG;
class CIPHER;


class DIOSTREAM : public XSUBJECT
{
  public:
                                DIOSTREAM                       ();
    virtual                    ~DIOSTREAM                       ();

    virtual DIOSTREAMCONFIG*    GetConfig                       ()                            = 0;
    virtual bool                SetConfig                       (DIOSTREAMCONFIG* config)     = 0;

    DIOSTREAMSTATUS             GetStatus                       ();
    void                        SetStatus                       (DIOSTREAMSTATUS status);

    virtual bool                Open                            ()                            = 0;

    bool                        IsConnected                     ();
    bool                        IsDisconnected                  ();

    bool                        WaitToConnected                 (int timeout);
    bool                        WaitToDisconnected              (int timeout);

    virtual XDWORD              Read                            (XBYTE* buffer, XDWORD size);
    virtual XDWORD              Write                           (XBYTE* buffer, XDWORD size);

    XDWORD                      Read                            (XBUFFER& xbuffer);
    XDWORD                      Write                           (XBUFFER& xbuffer);

    virtual bool                WaitToFilledReadingBuffer       (int filledto = DIOSTREAM_SOMETHINGTOREAD, int timeout = XTIMER_INFINITE);

    bool                        ReadStr                         (char* str);
    bool                        ReadStr                         (XSTRING& string);
    bool                        ReadStr                         (XSTRING& string, int timeout);

    bool                        WriteStr                        (const char* str);
    bool                        WriteStr                        (XCHAR* string);
    bool                        WriteStr                        (XSTRING& string);

    bool                        IsBlockRead                     ();
    void                        SetIsBlockRead                  (bool isblockread);

    bool                        IsBlockWrite                    ();
    void                        SetIsBlockWrite                 (bool isblockwrite);

    virtual bool                Disconnect                      ();

    virtual bool                Close                           ()                          = 0;

    DIOSTREAMTYPE               GetType                         ();
    void                        SetType                         (DIOSTREAMTYPE type);

    XBUFFER*                    GetInXBuffer                    ();
    XBUFFER*                    GetOutXBuffer                   ();

    bool                        ResetXBuffers                   ();
    bool                        ResetInXBuffer                  ();
    bool                        ResetOutXBuffer                 ();

    bool                        IsFlushOutXBuffer               ();
    bool                        IsFlushXBuffers                 ();

    bool                        WaitToFlushOutXBuffer           (int timeout = XTIMER_INFINITE);
    bool                        WaitToFlushXBuffers             (int timeout = XTIMER_INFINITE);

    virtual XTIMER*             GetXTimerConnection             ();
    virtual XTIMER*             GetXTimerNotActivity            ();

    virtual XDWORD              GetNBytesRead                   ();
    virtual XDWORD              GetNBytesWrite                  ();

    bool                        ResetConnectionStatistics       ();

    DIOSTREAMERROR              GetLastDIOError                 ();
    void                        SetLastDIOError                 (DIOSTREAMERROR error);

    void                        Wait                            (int milliseconds);

    XBUFFER*                    inbuffer;
    XBUFFER*                    outbuffer;

  protected:

    DIOSTREAMTYPE               type;

    DIOSTREAMSTATUS             status;
    DIOSTREAMERROR              error;

    XTIMER*                     xtimerconnection;
    XTIMER*                     xtimernotactivity;
    XTIMER*                     xtimerout;

    XMUTEX*                     xmutextimerout;

    bool                        isblockread;
    bool                        isblockwrite;

    XDWORD                      nbytesread;
    XDWORD                      nbyteswrite;

  private:

    void                        Clean                           ();

};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

