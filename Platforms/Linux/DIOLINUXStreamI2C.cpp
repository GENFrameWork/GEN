/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOLINUXStreamI2C.cpp
* 
* @class      DIOLINUXSTREAMI2C
* @brief      Linux Data Input/Output Stream I2C class
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <linux/types.h>


#include "XFactory.h"
#include "XBuffer.h"
#include "XThreadCollected.h"
#include "XSystem.h"
#include "XTrace.h"

#include "DIOFactory.h"
#include "DIOStreamXEvent.h"
#include "DIOStreamI2CConfig.h"

#include "DIOLINUXStreamI2C.h"

#include "XMemory_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOLINUXSTREAMI2C::DIOLINUXSTREAMI2C()
* @brief      Constructor
* @ingroup    PLATFORM_LINUX
* 
* @return     Does not return anything. 
*
--------------------------------------------------------------------------------------------------------------------*/
DIOLINUXSTREAMI2C::DIOLINUXSTREAMI2C() : DIOSTREAMI2C()
{
  Clean();

  threadconnection = CREATEXTHREAD(XTHREADGROUPID_DIOSTREAMI2C, __L("DIOLINUXSTREAMI2C::DIOLINUXSTREAMI2C"), ThreadConnection, (void*)this);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOLINUXSTREAMI2C::~DIOLINUXSTREAMI2C()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOLINUXSTREAMI2C::~DIOLINUXSTREAMI2C()
{
  if(threadconnection)
    {
      threadconnection->End();
      DELETEXTHREAD(XTHREADGROUPID_DIOSTREAMI2C, threadconnection);
      threadconnection = NULL;
    }

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXSTREAMI2C::Open()
* @brief      Open
* @ingroup    PLATFORM_LINUX
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXSTREAMI2C::Open()
{
  if(!config) return false;

  if(!threadconnection)  return false;
  
  XBUFFER charstr;
  
  (*config->GetLocalDeviceName()).ConvertToASCII(charstr);
  handle = open(charstr.GetPtrChar(), O_RDWR);  
  if(handle<0) return false;

  switch(config->GetAddressSize())
    {
      case DIOSTREAMI2CCONFIG_ADDRESSSIZE_DEFAULT :
      case DIOSTREAMI2CCONFIG_ADDRESSSIZE_7BITS   : break;
      case DIOSTREAMI2CCONFIG_ADDRESSSIZE_10BITS  : if(ioctl(handle, DIOLINUXSTREAMI2C_TENBIT, 1) < 0)  return false;
                                                    break;
    }

  if(ioctl(handle, DIOLINUXSTREAMI2C_SLAVE , config->GetRemoteAddress()) < 0)                 return false;

  SetEvent(DIOSTREAMI2C_FSMEVENT_CONNECTED);

  status = DIOSTREAMSTATUS_CONNECTED;

  ResetXBuffers();
  ResetConnectionStatistics();

  return threadconnection->Ini();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXSTREAMI2C::WaitToFilledReadingBuffer(int filledto, int timeout)
* @brief      WaitToFilledReadingBuffer
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  filledto : 
* @param[in]  timeout : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXSTREAMI2C::WaitToFilledReadingBuffer(int filledto, int timeout)
{
  if(filledto == DIOSTREAM_SOMETHINGTOREAD) sizeread = 1; else sizeread = filledto;

  bool status = DIOSTREAM::WaitToFilledReadingBuffer(sizeread, timeout);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD DIOLINUXSTREAMI2C::ReadDirect(XBYTE* buffer, XDWORD size)
* @brief      ReadDirect
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  buffer : 
* @param[in]  size : 
* 
* @return     XDWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOLINUXSTREAMI2C::ReadDirect(XBYTE* buffer, XDWORD size)
{
  if(!config) return false;

  DIOLINUXSTREAMI2C_RDWR_IOCTL_TICKET   msg_rdwr;
  DIOLINUXSTREAMI2C_MSG               msg;

  msg.addr        = config->GetRemoteAddress();
  msg.flags       = DIOLINUXSTREAMI2C_MSG_RD;
  msg.buffer      = buffer;
  msg.size        = size;

  msg_rdwr.msgs   = &msg;
  msg_rdwr.nmsgs  = 1;

  XDWORD br = 0;
  bool status = (ioctl(handle, DIOLINUXSTREAMI2C_RDWR, &msg_rdwr) < 0) ? false:true;
  if(status) br = size;

  //XTRACE_PRINTCOLOR(1, __L("Write [Addr %04X, size:%d]  status: %s"), address, size, status?__L("true"):__L("false"));

  return br;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD DIOLINUXSTREAMI2C::WriteDirect(XBYTE* buffer, XDWORD size)
* @brief      WriteDirect
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  buffer : 
* @param[in]  size : 
* 
* @return     XDWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOLINUXSTREAMI2C::WriteDirect(XBYTE* buffer, XDWORD size)
{
  if(!config) return false;

  DIOLINUXSTREAMI2C_RDWR_IOCTL_TICKET   msg_rdwr;
  DIOLINUXSTREAMI2C_MSG               msg;

  msg.addr        = config->GetRemoteAddress();
  msg.flags       = 0;
  msg.buffer      = buffer;
  msg.size        = size;

  msg_rdwr.msgs   = &msg;
  msg_rdwr.nmsgs  = 1;

  XDWORD bw = 0;

  bool status = (ioctl(handle, DIOLINUXSTREAMI2C_RDWR, &msg_rdwr) < 0) ? false:true;

  if(status) bw = size;

  //XTRACE_PRINTCOLOR(1, __L("Write [Addr %04X, size:%d]  status: %s"), address, size, status?__L("true"):__L("false"));

  return bw;

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXSTREAMI2C::Close()
* @brief      Close
* @ingroup    PLATFORM_LINUX
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXSTREAMI2C::Close()
{
  if(!threadconnection) return false;

  threadconnection->End();

  if(handle>=0)
    {
      close(handle);
      handle  = -1;
    }


  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOLINUXSTREAMI2C::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOLINUXSTREAMI2C::Clean()
{
  handle   = -1;
  sizeread = 0;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOLINUXSTREAMI2C::ThreadConnection(void* data)
* @brief      ThreadConnection
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  data : 
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOLINUXSTREAMI2C::ThreadConnection(void* data)
{
  DIOLINUXSTREAMI2C* diostream = (DIOLINUXSTREAMI2C*)data;
  if(!diostream) return;

  if(diostream->GetEvent()==DIOSTREAMI2C_FSMEVENT_NONE) // No hay nuevos Eventos
    {
      switch(diostream->GetCurrentState())
        {
          case DIOSTREAMI2C_FSMSTATE_NONE                     : break;

          case DIOSTREAMI2C_FSMSTATE_CONNECTED                : break;

          case DIOSTREAMI2C_FSMSTATE_WAITINGTOREAD            : {
                                                                  if(!diostream->IsBlockWrite())
                                                                    {
                                                                      XDWORD size  = diostream->outbuffer->GetSize();
                                                                      if(size)
                                                                        {
                                                                          diostream->outbuffer->SetBlocked(true);
                                                                          XDWORD bw = diostream->WriteDirect(diostream->outbuffer->Get(), size);
                                                                          diostream->outbuffer->SetBlocked(false);

                                                                          if(bw)
                                                                            {
                                                                              //XTRACE_PRINTDATABLOCKCOLOR(XTRACE_COLORRED, diostream->outbuffer->Get(), size);
                                                                              diostream->outbuffer->Extract(NULL, 0 , bw);
                                                                            }
                                                                        }
                                                                    }


                                                                  if(!diostream->IsBlockRead())
                                                                    {
                                                                      if(diostream->sizeread)
                                                                        {
                                                                          XBYTE* _data = new XBYTE[diostream->sizeread];
                                                                          if(_data)
                                                                            {
                                                                              if(diostream->ReadDirect(_data, diostream->sizeread))
                                                                                {
                                                                                  //XTRACE_PRINTDATABLOCKCOLOR(XTRACE_COLORGREEN, _data, diostream->sizeread);
                                                                                  diostream->inbuffer->Add(_data, diostream->sizeread);
                                                                                  diostream->sizeread = 0;
                                                                                }

                                                                              delete [] _data;
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                                break;


          case DIOSTREAMI2C_FSMSTATE_SENDINGDATA              : break;

          case DIOSTREAMI2C_FSMSTATE_DISCONNECTING            : break;

        }
    }
   else
    {
      if(diostream->GetEvent()<DIOSTREAMI2C_LASTEVENT)
        {
          diostream->CheckTransition();

          switch(diostream->GetCurrentState())
            {
              case DIOSTREAMI2C_FSMSTATE_NONE                 : break;

              case DIOSTREAMI2C_FSMSTATE_CONNECTED            : { DIOSTREAMXEVENT xevent(diostream,DIOSTREAMXEVENT_TYPE_CONNECTED);
                                                                  xevent.SetDIOStream(diostream);
                                                                  diostream->PostEvent(&xevent);

                                                                  diostream->SetEvent(DIOSTREAMI2C_FSMEVENT_WAITINGTOREAD);
                                                                }
                                                                break;

              case DIOSTREAMI2C_FSMSTATE_WAITINGTOREAD        : break;

              case DIOSTREAMI2C_FSMSTATE_SENDINGDATA          : break;

              case DIOSTREAMI2C_FSMSTATE_DISCONNECTING        : { DIOSTREAMXEVENT xevent(diostream,DIOSTREAMXEVENT_TYPE_DISCONNECTED);
                                                                  xevent.SetDIOStream(diostream);
                                                                  diostream->PostEvent(&xevent);

                                                                  diostream->threadconnection->Run(false);
                                                                  diostream->status       = DIOSTREAMSTATUS_DISCONNECTED;
                                                                }
                                                                break;
            }
        }
    }
}


