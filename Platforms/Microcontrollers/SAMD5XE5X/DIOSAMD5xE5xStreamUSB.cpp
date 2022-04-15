/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOSAMD5xE5xStreamUSB.cpp
*
* @class      DIOSAMD5XE5XSTREAMUSB
* @brief      SAMD5xE5x Data IO Stream USB class
* @ingroup    PLATFORM_SAMD5XE5X
*
* @copyright  GEN Group. All right reserved.
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
*---------------------------------------------------------------------------------------------------------------------*/


/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


#if defined(DIO_ACTIVE) && defined(DIOUSB_ACTIVE)

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XSAMD5xE5x_HAL.h"

//--- ABRHAMD5XE5X ------------------
//#include "usbd_cdc_if.h"
//#include "usb_device.h"

#include "XFactory.h"
#include "XThreadCollected.h"

#include "DIOStreamXEvent.h"
#include "DIOStreamUSBConfig.h"

#include "DIOSAMD5xE5xStreamUSB.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

DIOSAMD5XE5XSTREAMUSB* diostreamusbptrhandle =  NULL;

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSAMD5XE5XSTREAMUSB::DIOSAMD5XE5XSTREAMUSB()
* @brief      Constructor
* @ingroup    PLATFORM_SAMD5XE5X
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOSAMD5XE5XSTREAMUSB::DIOSAMD5XE5XSTREAMUSB(): DIOSTREAMUSB(), XFSMACHINE(0)
{
  Clean();

  AddState( DIOSAMD5XE5XUSBFSMSTATE_NONE              ,
            DIOSAMD5XE5XUSBFSMEVENT_CONNECTED       , DIOSAMD5XE5XUSBFSMSTATE_CONNECTED        ,
            DIOSAMD5XE5XUSBFSMEVENT_DISCONNECTING   , DIOSAMD5XE5XUSBFSMSTATE_DISCONNECTING    ,
            XFSMACHINESTATE_EVENTDEFEND);

  AddState( DIOSAMD5XE5XUSBFSMSTATE_CONNECTED       ,
            DIOSAMD5XE5XUSBFSMEVENT_WAITINGTOREAD   , DIOSAMD5XE5XUSBFSMSTATE_WAITINGTOREAD    ,
            DIOSAMD5XE5XUSBFSMEVENT_DISCONNECTING   , DIOSAMD5XE5XUSBFSMSTATE_DISCONNECTING    ,
            XFSMACHINESTATE_EVENTDEFEND);

  AddState( DIOSAMD5XE5XUSBFSMSTATE_WAITINGTOREAD   ,
            DIOSAMD5XE5XUSBFSMEVENT_CONNECTED       , DIOSAMD5XE5XUSBFSMSTATE_CONNECTED        ,
            DIOSAMD5XE5XUSBFSMEVENT_DISCONNECTING   , DIOSAMD5XE5XUSBFSMSTATE_DISCONNECTING    ,
            XFSMACHINESTATE_EVENTDEFEND);

  AddState( DIOSAMD5XE5XUSBFSMSTATE_DISCONNECTING   ,
            DIOSAMD5XE5XUSBFSMEVENT_CONNECTED       , DIOSAMD5XE5XUSBFSMSTATE_CONNECTED        ,
            DIOSAMD5XE5XUSBFSMEVENT_WAITINGTOREAD   , DIOSAMD5XE5XUSBFSMSTATE_WAITINGTOREAD    ,
            XFSMACHINESTATE_EVENTDEFEND);

  threadconnection = CREATEXTHREAD(XTHREADGROUPID_DIOSTREAMUSB, __L("DIOSAMD5XE5XSTREAMUSB::DIOSAMD5XE5XSTREAMUSB"), ThreadConnection, (void*)this);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSAMD5XE5XSTREAMUSB::~DIOSAMD5XE5XSTREAMUSB()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_SAMD5XE5X
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOSAMD5XE5XSTREAMUSB::~DIOSAMD5XE5XSTREAMUSB()
{
  Close();

  if(threadconnection) DELETEXTHREAD(XTHREADGROUPID_DIOSTREAMUSB, threadconnection);

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMSTATUS DIOSAMD5XE5XSTREAMUSB::GetConnectStatus()
* @brief      GetConnectStatus
* @ingroup    PLATFORM_SAMD5XE5X
*
* @return     DIOSTREAMSTATUS :
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMSTATUS DIOSAMD5XE5XSTREAMUSB::GetConnectStatus()
{
  if(!config) return DIOSTREAMSTATUS_DISCONNECTED;

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSAMD5XE5XSTREAMUSB::Open()
* @brief      Open
* @ingroup    PLATFORM_SAMD5XE5X
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOSAMD5XE5XSTREAMUSB::Open()
{
  if(!threadconnection)  return false;

  //--- ABRHAMD5XE5X ------------------
  //MX_USB_DEVICE_Init();

  diostreamusbptrhandle = this;

  SetEvent(DIOSAMD5XE5XUSBFSMEVENT_CONNECTED);

  status = DIOSTREAMSTATUS_CONNECTED;

  ResetXBuffers();
  ResetConnectionStatistics();

  return threadconnection->Ini();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSAMD5XE5XSTREAMUSB::WriteDirect(XBYTE* buffer, XDWORD size)
* @brief      WriteDirect
* @ingroup    PLATFORM_SAMD5XE5X
*
* @param[in]  buffer :
* @param[in]  size :
*
* @return     XDWORD :
*
*---------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOSAMD5XE5XSTREAMUSB::WriteDirect(XBYTE* buffer, XDWORD size)
{
  if(GetConnectStatus()==DIOSTREAMSTATUS_DISCONNECTED) return 0;

  //--- ABRHAMD5XE5X ------------------
  //if(CDC_Transmit_FS((uint8_t*)buffer, (XWORD)size) == USBD_OK) return size;

  return (XDWORD)0;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSAMD5XE5XSTREAMUSB::Write(XBYTE* buffer, XDWORD size)
* @brief      Write
* @ingroup    PLATFORM_SAMD5XE5X
*
* @param[in]  buffer :
* @param[in]  size :
*
* @return     XDWORD :
*
*---------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOSAMD5XE5XSTREAMUSB::Write(XBYTE* buffer, XDWORD size)
{
  if(!outbuffer) return 0;

  while(outbuffer->IsBlocked())
    {

    }

  if(!outbuffer->Add(buffer,size))  return 0;

  nbyteswrite+=size;
  if(xtimernotactivity) xtimernotactivity->Reset();

  outbuffer->SetBlocked(false);

  return size;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSAMD5XE5XSTREAMUSB::Close()
* @brief      Close
* @ingroup    PLATFORM_SAMD5XE5X
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOSAMD5XE5XSTREAMUSB::Close()
{
  if(!threadconnection) return false;

  threadconnection->End();

  if(GetConnectStatus()==DIOSTREAMSTATUS_DISCONNECTED) return false;

  diostreamusbptrhandle = NULL;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSAMD5XE5XSTREAMUSB::CleanBuffers()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_SAMD5XE5X
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOSAMD5XE5XSTREAMUSB::CleanBuffers()
{
  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSAMD5XE5XSTREAMUSB::HAL_USB_RxCpltCallback(uint8_t* buffer, uint32_t*len)
* @brief      HAL_USB_RxCpltCallback
* @ingroup    PLATFORM_SAMD5XE5X
*
* @param[in]  buffer :
* @param[in]  uint32_t*len :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
//--- ABRHAMD5XE5X ------------------
/*
void DIOSAMD5XE5XSTREAMUSB::HAL_USB_RxCpltCallback(uint8_t* buffer, uint32_t *len)
{
  if(GetConnectStatus() == DIOSTREAMSTATUS_DISCONNECTED) return;
  if(IsBlockRead())                                      return;

  if(!inbuffer->IsBlocked())
    {
      if(readcache.GetSize())
        {
          inbuffer->Add(readcache);
          readcache.Delete();
        }

      inbuffer->Add((XBYTE*)buffer, (XDWORD)(*len));
    }
   else
    {
      readcache.Add((XBYTE*)buffer, (XDWORD)(*len));
    }
}
*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSAMD5XE5XSTREAMUSB::ThreadConnection(void* param)
* @brief      ThreadConnection
* @ingroup    PLATFORM_SAMD5XE5X
*
* @param[in]  param :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void DIOSAMD5XE5XSTREAMUSB::ThreadConnection(void* param)
{
  DIOSAMD5XE5XSTREAMUSB* diostream = (DIOSAMD5XE5XSTREAMUSB*)param;
  if(!diostream) return;

  if(diostream->GetEvent()==DIOSAMD5XE5XUSBFSMEVENT_NONE) // No hay nuevos Eventos
    {
      switch(diostream->GetCurrentState())
        {
          case DIOSAMD5XE5XUSBFSMSTATE_NONE                     : break;

          case DIOSAMD5XE5XUSBFSMSTATE_CONNECTED                : break;

          case DIOSAMD5XE5XUSBFSMSTATE_WAITINGTOREAD            : { if(!diostream->IsBlockRead())
                                                                      {
                                                                        if(!diostream->inbuffer->IsBlocked())
                                                                          {
                                                                            if(diostream->readcache.GetSize())
                                                                              {
                                                                                diostream->inbuffer->Add(diostream->readcache);
                                                                                diostream->readcache.Delete();
                                                                              }
                                                                          }
                                                                      }

                                                                    if(!diostream->IsBlockWrite())
                                                                      {
                                                                        if(!diostream->outbuffer->IsBlocked())
                                                                          {
                                                                            int esize = diostream->outbuffer->GetSize();
                                                                            if(esize>DIOSTREAM_MAXBUFFER) esize = DIOSTREAM_MAXBUFFER;

                                                                            if(esize)
                                                                              {
                                                                                diostream->outbuffer->Get(diostream->writebuffer, esize, 0);
                                                                                if(diostream->WriteDirect(diostream->writebuffer, esize))
                                                                                  {
                                                                                    diostream->outbuffer->Extract(NULL, 0, esize);
                                                                                  }
                                                                              }
                                                                          }
                                                                      }
                                                                  }
                                                                  break;

          case DIOSAMD5XE5XUSBFSMSTATE_DISCONNECTING            : break;

        }
    }
   else
    {
      if(diostream->GetEvent()<DIOSAMD5XE5XUSB_LASTEVENT)
        {
          diostream->CheckTransition();

          switch(diostream->GetCurrentState())
            {
              case DIOSAMD5XE5XUSBFSMSTATE_NONE               : break;

              case DIOSAMD5XE5XUSBFSMSTATE_CONNECTED            : { DIOSTREAMXEVENT xevent(diostream,DIOSTREAMXEVENT_TYPE_CONNECTED);
                                                                    xevent.SetDIOStream(diostream);
                                                                    diostream->PostEvent(&xevent);

                                                                    diostream->SetEvent(DIOSAMD5XE5XUSBFSMEVENT_WAITINGTOREAD);
                                                                  }
                                                                  break;

              case DIOSAMD5XE5XUSBFSMSTATE_WAITINGTOREAD        : break;

              case DIOSAMD5XE5XUSBFSMSTATE_DISCONNECTING        : { DIOSTREAMXEVENT xevent(diostream,DIOSTREAMXEVENT_TYPE_DISCONNECTED);
                                                                    xevent.SetDIOStream(diostream);
                                                                    diostream->PostEvent(&xevent);

                                                                    diostream->status = DIOSTREAMSTATUS_DISCONNECTED;
                                                                  }
                                                                  break;
            }
        }
    }
}



#endif



