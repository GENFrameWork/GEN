/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOANDROIDStreamWifiRemoteEnumDevices.cpp
* 
* @class      DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES
* @brief      ANDROID Data Input/Output Stream Wifi Remote Enum Devices class
* @ingroup    PLATFORM_ANDROID
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


#if defined(DIO_ACTIVE) && defined(DIO_STREAMTWIFI_ACTIVE)


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOANDROIDStreamWifiRemoteEnumDevices.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <syslog.h>
#include <signal.h>
#include <time.h>

#include "XVector.h"
#include "XString.h"
#include "XThreadCollected.h"
#include "XTrace.h"
#include "XANDROIDThread.h"

#include "DIOStream.h"
#include "DIOStreamDeviceWifi.h"
#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES::DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES()
* @brief      Constructor
* @ingroup    PLATFORM_ANDROID
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES::DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES() : DIOSTREAMWIFIREMOTEENUMDEVICES() , XFSMACHINE(0)
{
  Clean();

  AddState( DIOANDROIDWIFIENUMFSMSTATE_NONE               ,
            DIOANDROIDWIFIENUMFSMEVENT_SEARCH             , DIOANDROIDWIFIENUMFSMSTATE_SEARCH           ,
            DIOANDROIDWIFIENUMFSMEVENT_SEARCHEND          , DIOANDROIDWIFIENUMFSMSTATE_SEARCHEND        ,
            XFSMACHINESTATE_EVENTDEFEND);

  AddState( DIOANDROIDWIFIENUMFSMSTATE_SEARCH             ,
            DIOANDROIDWIFIENUMFSMEVENT_SEARCHEND          , DIOANDROIDWIFIENUMFSMSTATE_SEARCHEND        ,
            XFSMACHINESTATE_EVENTDEFEND);

  AddState( DIOANDROIDWIFIENUMFSMSTATE_SEARCHEND          ,
            DIOANDROIDWIFIENUMFSMEVENT_SEARCH             , DIOANDROIDWIFIENUMFSMSTATE_SEARCH           ,
            XFSMACHINESTATE_EVENTDEFEND);

  threadenumdevices = CREATEXTHREAD(__L("DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES::DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES"), ThreadEnumDevices, (void*)this);
  if(threadenumdevices) threadenumdevices->Ini();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES::~DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_ANDROID
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES::~DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES()
{
  StopSearch(true);

  if(threadenumdevices)
    {
      threadenumdevices->End();
      DELETEXTHREAD(threadenumdevices);
    }

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES::Search()
* @brief      Search
* @ingroup    PLATFORM_ANDROID
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES::Search()
{
  if(!threadenumdevices)       return false;

  DelAllDevices();

  SetEvent(DIOANDROIDWIFIENUMFSMEVENT_SEARCH);

  threadenumdevices->Run(true);

  sleep(1);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES::StopSearch(bool waitend)
* @brief      StopSearch
* @ingroup    PLATFORM_ANDROID
* 
* @param[in]  waitend : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES::StopSearch(bool waitend)
{
  if(IsSearching())
    {
      SetEvent(DIOANDROIDWIFIENUMFSMEVENT_SEARCHEND);

      if(waitend)
        {
          while(GetCurrentState()!=DIOANDROIDWIFIENUMFSMEVENT_SEARCHEND)
            {
              usleep(1000);
            }
        }
    }

  if(threadenumdevices) threadenumdevices->Run(false);

  usleep(1000);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES::IsSearching()
* @brief      IsSearching
* @ingroup    PLATFORM_ANDROID
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES::IsSearching()
{
  if(!threadenumdevices)              return false;
  if(!threadenumdevices->IsRunning()) return false;

  if(GetCurrentState()!=DIOANDROIDWIFIENUMFSMSTATE_SEARCH) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES::ThreadEnumDevices(void* param)
* @brief      ThreadEnumDevices
* @ingroup    PLATFORM_ANDROID
* 
* @param[in]  param : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES::ThreadEnumDevices(void* param)
{
  DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES* enumeration = (DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES*)param;
  if(!enumeration) return;

  if(enumeration->GetEvent()==DIOANDROIDWIFIENUMFSMEVENT_NONE) // No hay nuevos Eventos
    {
      switch(enumeration->GetCurrentState())
        {
          case DIOANDROIDWIFIENUMFSMSTATE_NONE            : break;

          case DIOANDROIDWIFIENUMFSMSTATE_SEARCH          : enumeration->SetEvent(DIOANDROIDWIFIENUMFSMEVENT_SEARCHEND);
                                                          break;

          case DIOANDROIDWIFIENUMFSMSTATE_SEARCHEND       : break;


        }
    }
   else
    {
      if(enumeration->GetEvent()<DIOANDROIDWIFIENUM_LASTEVENT)
        {
          enumeration->CheckTransition();

          switch(enumeration->GetCurrentState())
            {
              case DIOANDROIDWIFIENUMFSMSTATE_NONE            : break;

              case DIOANDROIDWIFIENUMFSMSTATE_SEARCH          : { wireless_scan_head  head;
                                                                wireless_scan*      result = NULL;
                                                                iwrange             range;
                                                                int                 sock;
                                                                int                 index = 0;

                                                                memset(&head, 0, sizeof(wireless_scan_head));
                                                                memset(&range, 0, sizeof(iwrange));

                                                                /* Open socket to kernel */
                                                                sock = iw_sockets_open();
                                                                if(sock==-1) break;

                                                                /* Get some metadata to use for scanning */
                                                                if (iw_get_range_info(sock, "wlan0", &range)<0) break;

                                                                /* Perform the scan */
                                                                if(iw_scan(sock, (char*)"wlan0", range.we_version_compiled, &head)<0) break;

                                                                /* Traverse the results */
                                                                result = head.result;
                                                                while (NULL != result)
                                                                  {
                                                                    DIOSTREAMDEVICEWIFI* device = new DIOSTREAMDEVICEWIFI();
                                                                    if(device)
                                                                      {
                                                                        device->SetIndex(index);
                                                                        (*device->GetName()) = result->b.essid;

                                                                        enumeration->devices.Add(device);

                                                                        index++;
                                                                      }

                                                                    result = result->next;
                                                                  }
                                                              }
                                                              break;

              case DIOANDROIDWIFIENUMFSMSTATE_SEARCHEND       : break;
           }
        }
    }

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_ANDROID
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOANDROIDSTREAMWIFIREMOTEENUMDEVICES::Clean()
{
  threadenumdevices = NULL;

}


#pragma endregion


#endif

