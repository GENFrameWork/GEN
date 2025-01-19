/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOLINUXStreamWifiRemoteEnumDevices.h
* 
* @class      DIOLINUXSTREAMWIFIREMOTEENUMDEVICES
* @brief      LINUX Data Input/Output Stream Wifi Remote Enum Devices class
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

#ifndef _DIOLINUXSTREAMWIFIREMOTEENUMDEVICES_H_
#define _DIOLINUXSTREAMWIFIREMOTEENUMDEVICES_H_


#if defined(DIO_ACTIVE) && defined(DIO_STREAMTWIFI_ACTIVE)


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES


#include "XFSMachine.h"
#include "XThreadCollected.h"

#include "DIOMAC.h"
#include "DIOStreamWifiRemoteEnumDevices.h"


#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


enum DIOLINUXWIFIENUMFSMFSMEVENTS
{
  DIOLINUXWIFIENUMFSMEVENT_NONE             = 0 ,
  DIOLINUXWIFIENUMFSMEVENT_SEARCH               ,
  DIOLINUXWIFIENUMFSMEVENT_SEARCHEND            ,

  DIOLINUXWIFIENUM_LASTEVENT
};


enum DIOLINUXWIFIENUMFSMSTATES
{
  DIOLINUXWIFIENUMFSMSTATE_NONE             = 0 ,
  DIOLINUXWIFIENUMFSMSTATE_SEARCH               ,
  DIOLINUXWIFIENUMFSMSTATE_SEARCHEND            ,

  DIOLINUXWIFIENUM_LASTSTATE
};


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class DIOLINUXSTREAMWIFIREMOTEENUMDEVICES :  public DIOSTREAMWIFIREMOTEENUMDEVICES, public XFSMACHINE
{
  public:
                              DIOLINUXSTREAMWIFIREMOTEENUMDEVICES             ();
    virtual                  ~DIOLINUXSTREAMWIFIREMOTEENUMDEVICES             ();

    bool                      Search                                          ();
    bool                      StopSearch                                      (bool waitend);
    bool                      IsSearching                                     ();

  private:
  
    static void               ThreadEnumDevices                               (void* thread); 

    void                      Clean                                           ();

    XTHREADCOLLECTED*         threadenumdevices;
};

#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif


#endif
