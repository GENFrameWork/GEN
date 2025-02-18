/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOWINDOWSStreamUART.h
* 
* @class      DIOWINDOWSSTREAMUART
* @brief      WINDOWS Data Input/Output Stream UART class
* @ingroup    PLATFORM_WINDOWS
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

#ifndef _DIOWINDOWSSTREAMUART_H_
#define _DIOWINDOWSSTREAMUART_H_


#if defined(DIO_ACTIVE) && defined(DIO_STREAMUART_ACTIVE)


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include <windows.h>

#include "XFSMachine.h"
#include "XThreadCollected.h"

#include "DIOStreamUART.h"
#include "DIOStreamUARTConfig.h"


#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


enum DIOWINDOWSUARTFSMEVENTS
{
  DIOWINDOWSUARTFSMEVENT_NONE               = 0 ,
  DIOWINDOWSUARTFSMEVENT_CONNECTED              ,
  DIOWINDOWSUARTFSMEVENT_WAITINGTOREAD          ,
  DIOWINDOWSUARTFSMEVENT_DISCONNECTING          ,

  DIOWINDOWSUART_LASTEVENT
};


enum DIOWINDOWSUARTFSMSTATES
{
  DIOWINDOWSUARTFSMSTATE_NONE               = 0 ,
  DIOWINDOWSUARTFSMSTATE_CONNECTED              ,
  DIOWINDOWSUARTFSMSTATE_WAITINGTOREAD          ,
  DIOWINDOWSUARTFSMSTATE_DISCONNECTING          ,

  DIOWINDOWSUART_LASTSTATE
};


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class DIOWINDOWSSTREAMUART : public DIOSTREAMUART , public XFSMACHINE
{
  public:
                          DIOWINDOWSSTREAMUART            ();
    virtual              ~DIOWINDOWSSTREAMUART            ();

    DIOSTREAMSTATUS       GetStatus                       ();

    bool                  Open                            ();

    bool                  Config                          (XWORD mask = DIOSTREAMUARTMASK_ALL);

    XDWORD                ReadDirect                      (XBYTE* buffer,XDWORD size);
    XDWORD                WriteDirect                     (XBYTE* buffer,XDWORD size);

    bool                  Disconnect                      ();
    bool                  Close                           ();

    bool                  GetCTS                          ();
    bool                  GetDSR                          ();
    bool                  GetRing                         ();
    bool                  GetRLSD                         ();

    bool                  SetRTS                          (bool on=true);
    bool                  SetDTR                          (bool on=true);

    bool                  CleanBuffers                    ();

  protected:

    bool                  SetMask                         (XDWORD mask);
    bool                  SetTimeouts                     ();

    static void           ThreadConnection                (void* data);

    void                  Clean                           ();

    XTHREADCOLLECTED*     threadconnection;

    HANDLE                hcom;
    XDWORD                mask;
    HANDLE                hevent;
    OVERLAPPED            ovi;

    int                   readtimeout;
    int                   writetimeout;
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif


#endif

