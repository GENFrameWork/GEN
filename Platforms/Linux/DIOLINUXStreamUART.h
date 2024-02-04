/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOLINUXStreamUART.h
* 
* @class      DIOLINUXSTREAMUART
* @brief      LINUX Data Input/Output Stream UART class
* @ingroup    PLATFORM_LINUX
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

#ifndef _DIOLINUXSTREAMUART_H_
#define _DIOLINUXSTREAMUART_H_


#if defined(DIO_ACTIVE) && defined(DIO_STREAMUART_ACTIVE)


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES


#include "XBuffer.h"
#include "XFSMachine.h"
#include "XThreadCollected.h"

#include "DIOStreamUART.h"


#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


enum DIOLINUXUARTFSMEVENTS
{
  DIOLINUXUARTFSMEVENT_NONE              = 0  ,
  DIOLINUXUARTFSMEVENT_CONNECTED              ,
  DIOLINUXUARTFSMEVENT_WAITINGTOREAD          ,
  DIOLINUXUARTFSMEVENT_DISCONNECTING          ,

  DIOLINUXUART_LASTEVENT

};


enum DIOLINUXUARTFSMSTATES
{
  DIOLINUXUARTFSMSTATE_NONE              = 0  ,

  DIOLINUXUARTFSMSTATE_CONNECTED              ,
  DIOLINUXUARTFSMSTATE_WAITINGTOREAD          ,
  DIOLINUXUARTFSMSTATE_DISCONNECTING          ,

  DIOLINUXUART_LASTSTATE
};


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class DIOLINUXSTREAMUART : public DIOSTREAMUART , public XFSMACHINE
{
  public:
                          DIOLINUXSTREAMUART            ();
    virtual              ~DIOLINUXSTREAMUART            ();

    DIOSTREAMSTATUS       GetConnectStatus              ();

    bool                  Open                          ();

    bool                  Config                        (XWORD mask = DIOSTREAMUARTMASK_ALL);

    XDWORD                ReadDirect                    (XBYTE* buffer, XDWORD size);
    XDWORD                WriteDirect                   (XBYTE* buffer, XDWORD size);

    bool                  Disconnect                    ()                                { return false; };
    bool                  Close                         ();

    bool                  GetCTS                        ();
    bool                  GetDSR                        ();
    bool                  GetRing                       ();
    bool                  GetRLSD                       ();

    bool                  SetRTS                        (bool on = true);
    bool                  SetDTR                        (bool on = true);

    bool                  CleanBuffers                  ();

  protected:

    void                  Clean                         ();

    static void           ThreadConnection               (void* param);

    XTHREADCOLLECTED*     threadconnection;

    int                   fd;

    int                   readtimeout;
    int                   writetimeout;
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif


#endif


