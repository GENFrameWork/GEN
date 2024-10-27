/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOESP32StreamI2C.cpp
*
* @class      DIOESP32STREAMI2C
* @brief      Data Input/Output ESP32 Stream I2C class
* @ingroup    PLATFORM_ESP32
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

#if defined(DIO_ACTIVE) && defined(DIO_STREAMI2C_ACTIVE)

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XFactory.h"
#include "XThreadCollected.h"

#include "DIOStreamXEvent.h"
#include "DIOStreamI2CConfig.h"

#include "DIOESP32StreamI2C.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

DIOESP32STREAMI2CPORT*    DIOESP32STREAMI2C::ports[DIOESP32STREAMI2C_MAXPORTS]     = { NULL, NULL, NULL, NULL };
DIOESP32STREAMI2C*        DIOESP32STREAMI2C::handles[DIOESP32STREAMI2C_MAXHANDLES] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/*  DIOESP32STREAMI2CPORT                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOESP32STREAMI2CPORT::DIOESP32STREAMI2CPORT()
* @brief      Constructor
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOESP32STREAMI2CPORT::DIOESP32STREAMI2CPORT()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOESP32STREAMI2CPORT::~DIOESP32STREAMI2CPORT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOESP32STREAMI2CPORT::~DIOESP32STREAMI2CPORT()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOESP32STREAMI2CPORT::GetCounterRef()
* @brief      GetCounterRef
* @ingroup    PLATFORM_ESP32
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOESP32STREAMI2CPORT::GetCounterRef()
{
  return counterref;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOESP32STREAMI2CPORT::SetCounterRef(XDWORD counterref)
* @brief      SetCounterRef
* @ingroup    PLATFORM_ESP32
*
* @param[in]  counterref :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOESP32STREAMI2CPORT::SetCounterRef(XDWORD counterref)
{
  this->counterref = counterref;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         I2C_HandleTypeDef* DIOESP32STREAMI2CPORT::GetHandleI2C()
* @brief      GetHandleI2C
* @ingroup    PLATFORM_ESP32
*
* @return     I2C_HandleTypeDef* :
*
* --------------------------------------------------------------------------------------------------------------------*/
I2C_HandleTypeDef* DIOESP32STREAMI2CPORT::GetHandleI2C()
{
  return hi2c;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOESP32STREAMI2CPORT::SetHandleI2C(I2C_HandleTypeDef* hi2c)
* @brief      SetHandleI2C
* @ingroup    PLATFORM_ESP32
*
* @param[in]  hi2c :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOESP32STREAMI2CPORT::SetHandleI2C(I2C_HandleTypeDef* hi2c)
{
  this->hi2c = hi2c;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOESP32STREAMI2CPORT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOESP32STREAMI2CPORT::Clean()
{
  counterref        = 0;
  hi2c              = NULL;
}



/*--------------------------------------------------------------------------------------------------------------------*/
/*  DIOESP32STREAMI2C                                                                                                 */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOESP32STREAMI2C::DIOESP32STREAMI2C()
* @brief      Constructor
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOESP32STREAMI2C::DIOESP32STREAMI2C() : DIOSTREAMI2C()
{
  Clean();

  AddState( DIOSTREAMI2C_FSMSTATE_NONE            ,
            DIOSTREAMI2C_FSMEVENT_CONNECTED       , DIOSTREAMI2C_FSMSTATE_CONNECTED        ,
            DIOSTREAMI2C_FSMEVENT_DISCONNECTING   , DIOSTREAMI2C_FSMSTATE_DISCONNECTING    ,
            XFSMACHINESTATE_EVENTDEFEND);

  AddState( DIOSTREAMI2C_FSMSTATE_CONNECTED       ,
            DIOSTREAMI2C_FSMEVENT_WAITINGTOREAD   , DIOSTREAMI2C_FSMSTATE_WAITINGTOREAD    ,
            DIOSTREAMI2C_FSMEVENT_DISCONNECTING   , DIOSTREAMI2C_FSMSTATE_DISCONNECTING    ,
            XFSMACHINESTATE_EVENTDEFEND);

  AddState( DIOSTREAMI2C_FSMSTATE_WAITINGTOREAD   ,
            DIOSTREAMI2C_FSMEVENT_CONNECTED       , DIOSTREAMI2C_FSMSTATE_CONNECTED        ,
            DIOSTREAMI2C_FSMEVENT_DISCONNECTING   , DIOSTREAMI2C_FSMSTATE_DISCONNECTING    ,
            XFSMACHINESTATE_EVENTDEFEND);

  AddState( DIOSTREAMI2C_FSMSTATE_DISCONNECTING   ,
            DIOSTREAMI2C_FSMEVENT_CONNECTED       , DIOSTREAMI2C_FSMSTATE_CONNECTED        ,
            DIOSTREAMI2C_FSMEVENT_WAITINGTOREAD   , DIOSTREAMI2C_FSMSTATE_WAITINGTOREAD    ,
            XFSMACHINESTATE_EVENTDEFEND);

  threadconnection = CREATEXTHREAD(XTHREADGROUPID_DIOSTREAMI2C, __L("DIOESP32STREAMI2C::DIOESP32STREAMI2C"), ThreadConnection, (void*)this);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOESP32STREAMI2C::~DIOESP32STREAMI2C()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOESP32STREAMI2C::~DIOESP32STREAMI2C()
{
  Close();

  if(threadconnection) DELETEXTHREAD(XTHREADGROUPID_DIOSTREAMI2C, threadconnection);

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMSTATUS DIOESP32STREAMI2C::GetStatus()
* @brief      GetStatus
* @ingroup    PLATFORM_ESP32
*
* @return     DIOSTREAMSTATUS :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMSTATUS DIOESP32STREAMI2C::GetStatus()
{
  if(!config) return DIOSTREAMSTATUS_DISCONNECTED;

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOESP32STREAMI2C::Open()
* @brief      Open
* @ingroup    PLATFORM_ESP32
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOESP32STREAMI2C::Open()
{
  bool result = false;
  
  if(!threadconnection)  return false;
  if(!config)           return false;

  for(int c=0; c<DIOESP32STREAMI2C_MAXHANDLES; c++)
    {
      if(DIOESP32STREAMI2C::handles[c] == this) return false;

      if(!DIOESP32STREAMI2C::handles[c])
        {
          indexhandle = c;
          DIOESP32STREAMI2C::handles[indexhandle] = this;
          break;
        }
    }

  if(indexhandle == -1) return false;

  if(config->GetLocalAddress()  != DIOSTREAMI2CCONFIG_ADDRESSINVALID)  localaddress  = (config->GetLocalAddress()  << 1);
  if(config->GetRemoteAddress() != DIOSTREAMI2CCONFIG_ADDRESSINVALID)  remoteaddress = (config->GetRemoteAddress() << 1);

  indexport = config->GetPort()-1;

  if(!DIOESP32STREAMI2C::ports[indexport])
    {
      DIOESP32STREAMI2CPORT* port = new DIOESP32STREAMI2CPORT();
      if(!port) return false;

      hi2c = new I2C_HandleTypeDef();
      if(!hi2c)
        {
          delete port;
          return false;
        }

      switch(config->GetPort())
        {
          case  1 : hi2c->Instance = I2C1;       break;
          #if defined(ESP32F411xE) || defined(ESP32F446x)
          case  2 : hi2c->Instance = I2C2;       break;
          #endif
          #if defined(HW_ESP32WB55XX)
          case  3 : hi2c->Instance = I2C3;       break;
          #endif
          
          default : return false;
        }

      #if defined(HW_ESP32F0XX) || defined(HW_ESP32F3XX)
      hi2c->Init.Timing           = 0x2000090E;
      #endif
      
      #if defined(HW_ESP32WB55XX) 
      hi2c->Init.Timing           = 0x10707DBC;
      //hi2c->Init.Timing           = 0x00602173;   //  (400KHz)
      #endif
      
      #if defined(ESP32F411xE) || defined(ESP32F446x)
      hi2c->Init.ClockSpeed       = 100000;
      #endif

      hi2c->Init.OwnAddress1      = localaddress;

      switch(config->GetAddressSize())
        {
          case DIOSTREAMI2CCONFIG_ADDRESSSIZE_DEFAULT :
          case DIOSTREAMI2CCONFIG_ADDRESSSIZE_7BITS   : hi2c->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;    break;
          case DIOSTREAMI2CCONFIG_ADDRESSSIZE_10BITS  : hi2c->Init.AddressingMode   = I2C_ADDRESSINGMODE_10BIT;   break;
        }

      hi2c->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
      hi2c->Init.OwnAddress2      = 0;
      #if defined(HW_ESP32F0XX) || defined(HW_ESP32F3XX) || defined(HW_ESP32WB55XX) 
      hi2c->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
      #endif
      hi2c->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
      hi2c->Init.NoStretchMode    = config->IsStretchActive()?I2C_NOSTRETCH_ENABLE:I2C_NOSTRETCH_DISABLE;

      result =(HAL_I2C_Init(hi2c) == HAL_OK)?true:false;
      if(result)
        {      
          #if defined(HW_ESP32F0XX) || defined(HW_ESP32F3XX) || defined(HW_ESP32WB55XX) 
          result = (HAL_I2CEx_ConfigAnalogFilter(hi2c, I2C_ANALOGFILTER_ENABLE) == HAL_OK)?true:false;
          #endif
        
          if(result)
            {      
              result = (HAL_I2CEx_ConfigDigitalFilter(hi2c, 0) == HAL_OK)?true:false;        
              if(result)
                {
                  if(config->GetMode() == DIOSTREAMMODE_MASTER)
                    {
                      result = (HAL_I2C_IsDeviceReady(hi2c, (uint16_t)remoteaddress, 10, 1000) == HAL_OK)?true:false;                                                     
                    }
                }
            }
        }

      port->SetHandleI2C(hi2c);
      DIOESP32STREAMI2C::ports[indexport] = port;
    }
   else
    {
      hi2c = DIOESP32STREAMI2C::ports[indexport]->GetHandleI2C();
      DIOESP32STREAMI2C::ports[indexport]->SetCounterRef(DIOESP32STREAMI2C::ports[indexport]->GetCounterRef()+1);
      
      result = true;
    }

  if(result)
    {
      SetEvent(DIOSTREAMI2C_FSMEVENT_CONNECTED);

      status = DIOSTREAMSTATUS_CONNECTED;

      memset(bufferdata, 0, DIOSTREAM_MAXBUFFER);

      ResetXBuffers();
      ResetConnectionStatistics();

      result =threadconnection->Ini();
    }      
    
  return result;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOESP32STREAMI2C::WaitToFilledReadingBuffer(int filledto, int timeout)
* @brief      WaitToFilledReadingBuffer
* @ingroup    PLATFORM_ESP32
*
* @param[in]  filledto :
* @param[in]  timeout :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOESP32STREAMI2C::WaitToFilledReadingBuffer(int filledto, int timeout)
{
  if(filledto == DIOSTREAM_SOMETHINGTOREAD) sizebufferdata = 1; else sizebufferdata = filledto;

  switch(config->GetMode())
    {
      case DIOSTREAMMODE_MASTER     : HAL_I2C_Master_Receive_IT(hi2c, (uint16_t)remoteaddress, (uint8_t*)bufferdata, sizebufferdata);
                                      break;

      case DIOSTREAMMODE_SEMIMASTER :
      case DIOSTREAMMODE_SLAVE      : HAL_I2C_Slave_Receive_IT(hi2c, (uint8_t*)bufferdata, sizebufferdata);
                                      break;
                           default  : break;
    }

  return DIOSTREAM::WaitToFilledReadingBuffer(sizebufferdata, timeout);                                        
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOESP32STREAMI2C::ReadDirect(XBYTE* buffer, XDWORD size)
* @brief      ReadDirect
* @ingroup    PLATFORM_ESP32
*
* @param[in]  buffer :
* @param[in]  size :
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOESP32STREAMI2C::ReadDirect(XBYTE* buffer, XDWORD size)
{
  if(!config)                                          return 0;
  if(GetStatus()==DIOSTREAMSTATUS_DISCONNECTED) return 0;

  XDWORD br = 0;

  switch(config->GetMode())
    {
      case DIOSTREAMMODE_MASTER     : if(HAL_I2C_Master_Receive(hi2c, (uint16_t)remoteaddress, buffer, size, 1000) == HAL_OK) br = size;
                                      break;

      case DIOSTREAMMODE_SEMIMASTER :
      case DIOSTREAMMODE_SLAVE      : if(HAL_I2C_Slave_Receive(hi2c, buffer, size, 1000) == HAL_OK) br = size;
                                      break;

                            default : break;
    }

  return br;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOESP32STREAMI2C::WriteDirect(XBYTE* buffer, XDWORD size)
* @brief      WriteDirect
* @ingroup    PLATFORM_ESP32
*
* @param[in]  buffer :
* @param[in]  size :
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOESP32STREAMI2C::WriteDirect(XBYTE* buffer, XDWORD size)
{
  if(!config)                                          return 0;
  if(GetStatus()==DIOSTREAMSTATUS_DISCONNECTED) return 0;

  XDWORD bw = 0;

  if(!size) return 0;

   switch(config->GetMode())
    {
      case DIOSTREAMMODE_MASTER       :
      case DIOSTREAMMODE_SEMIMASTER   : //HAL_I2C_Mem_Write(&hi2c_fuel_gauge, LTC2942_ADDR, address, I2C_MEMADD_SIZE_8BIT, &value, 1, 10);
                                                
                                        if(HAL_I2C_Master_Transmit(hi2c, (uint16_t)remoteaddress, buffer, size, 1000) == HAL_OK) bw = size;
                                        break;

      case DIOSTREAMMODE_SLAVE        : if(HAL_I2C_Slave_Transmit(hi2c, buffer, size, 1000) == HAL_OK) bw = size;
                                        break;

                              default : break;
    }

  return bw;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOESP32STREAMI2C::Write(XBYTE* buffer, XDWORD size)
* @brief      Write
* @ingroup    PLATFORM_ESP32
*
* @param[in]  buffer :
* @param[in]  size :
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOESP32STREAMI2C::Write(XBYTE* buffer, XDWORD size)
{
  if(!outbuffer) return 0;

  while(outbuffer->IsBlocked())
    {

    }

  outbuffer->SetBlocked(true);
    
  if(!outbuffer->Add(buffer,size))  return 0;

  nbyteswrite+=size;
  if(xtimernotactivity) xtimernotactivity->Reset();

  outbuffer->SetBlocked(false);

  return size;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOESP32STREAMI2C::Disconnect()
* @brief      Disconnect
* @ingroup    PLATFORM_ESP32
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOESP32STREAMI2C::Disconnect()
{
  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOESP32STREAMI2C::Close()
* @brief      Close
* @ingroup    PLATFORM_ESP32
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOESP32STREAMI2C::Close()
{
  if(!threadconnection) return false;

  threadconnection->End();

  if(GetStatus()==DIOSTREAMSTATUS_DISCONNECTED) return false;

  if(!DIOESP32STREAMI2C::handles[indexhandle]) return false;

  DIOESP32STREAMI2C::handles[indexhandle] = NULL;

  if(!DIOESP32STREAMI2C::ports[indexport]->GetCounterRef())
    {
      HAL_I2C_DeInit(DIOESP32STREAMI2C::ports[indexport]->GetHandleI2C());

      delete DIOESP32STREAMI2C::ports[indexport]->GetHandleI2C();
      delete DIOESP32STREAMI2C::ports[indexport];

      DIOESP32STREAMI2C::ports[indexport] = NULL;
    }
   else
    {
      DIOESP32STREAMI2C::ports[indexport]->SetCounterRef(DIOESP32STREAMI2C::ports[indexport]->GetCounterRef()-1);
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOESP32STREAMI2C::CleanBuffers()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_ESP32
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOESP32STREAMI2C::CleanBuffers()
{
  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef* hi2c)
* @brief      AL_I2C_MasterRxCpltCallback
* @ingroup    PLATFORM_ESP32
*
* @param[in]  hi2c :
*
* --------------------------------------------------------------------------------------------------------------------*/
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef* hi2c)
{
  int indexport = -1;

  if(hi2c->Instance == I2C1) indexport = 0;
  //if(hi2c->Instance == I2C2) indexport = 1;

  for(int c=0; c<DIOESP32STREAMI2C_MAXHANDLES; c++)
    {
      if(DIOESP32STREAMI2C::handles[c])
        {
          if(DIOESP32STREAMI2C::handles[c]->indexport  == indexport)
            {
              if(DIOESP32STREAMI2C::handles[c]->GetStatus() == DIOSTREAMSTATUS_CONNECTED)
                {
                  if(!DIOESP32STREAMI2C::handles[c]->IsBlockRead())
                    {
                      if(!DIOESP32STREAMI2C::handles[c]->inbuffer->IsBlocked())
                        {
                          if(DIOESP32STREAMI2C::handles[c]->readcache[indexport].GetSize())
                            {
                              DIOESP32STREAMI2C::handles[c]->inbuffer->Add(DIOESP32STREAMI2C::handles[c]->readcache);
                              DIOESP32STREAMI2C::handles[c]->readcache[indexport].Delete();
                            }

                          DIOESP32STREAMI2C::handles[c]->inbuffer->Add(DIOESP32STREAMI2C::handles[c]->bufferdata, DIOESP32STREAMI2C::handles[c]->sizebufferdata);
                        }
                       else
                        {
                          DIOESP32STREAMI2C::handles[c]->readcache[indexport].Add(DIOESP32STREAMI2C::handles[c]->bufferdata, DIOESP32STREAMI2C::handles[c]->sizebufferdata);
                        }
                    }
                }

              DIOESP32STREAMI2C::handles[c]->sizebufferdata = 0;
            }
        }
    }
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef* hi2c)
* @brief      AL_I2C_SlaveRxCpltCallback
* @ingroup    PLATFORM_ESP32
*
* @param[in]  hi2c :
*
* --------------------------------------------------------------------------------------------------------------------*/
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef* hi2c)
{
  HAL_I2C_MasterRxCpltCallback(hi2c);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void HAL_I2C_ErrorCallback(I2C_HandleTypeDef*hi2c)
* @brief      AL_I2C_ErrorCallback
* @ingroup    PLATFORM_ESP32
*
* @param[in]  I2C_HandleTypeDef*hi2c :
*
* --------------------------------------------------------------------------------------------------------------------*/
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
  XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("I2C comunication error  %d"), hi2c->ErrorCode);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOESP32STREAMI2C::ThreadConnection(void* param)
* @brief      ThreadConnection
* @ingroup    PLATFORM_ESP32
*
* @param[in]  param :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOESP32STREAMI2C::ThreadConnection(void* param)
{
  DIOESP32STREAMI2C* diostream = (DIOESP32STREAMI2C*)param;
  if(!diostream) return;

  if(diostream->GetEvent()==DIOSTREAMI2C_FSMEVENT_NONE)
    {
      switch(diostream->GetCurrentState())
        {
          case DIOSTREAMI2C_FSMSTATE_NONE                   : break;

          case DIOSTREAMI2C_FSMSTATE_CONNECTED              : break;

          case DIOSTREAMI2C_FSMSTATE_WAITINGTOREAD          : if(!diostream->IsBlockRead())
                                                                {
                                                                  if(!diostream->inbuffer->IsBlocked())
                                                                    {
                                                                      if(diostream->readcache[diostream->indexport].GetSize())
                                                                        {
                                                                          diostream->inbuffer->Add(diostream->readcache[diostream->indexport]);
                                                                          diostream->readcache[diostream->indexport].Delete();
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
                                                                          diostream->outbuffer->Extract(diostream->writebuffer, 0, esize);
                                                                          XDWORD _esize = diostream->WriteDirect(diostream->writebuffer, esize);
                                                                          if(_esize != esize)
                                                                            {
                                                                              diostream->outbuffer->Add(diostream->writebuffer, esize);
                                                                            }                                                                        
                                                                        }
                                                                    }
                                                                }
                                                              break;

          case DIOSTREAMI2C_FSMSTATE_DISCONNECTING          : break;

        }
    }
   else
    {
      if(diostream->GetEvent()<DIOSTREAMI2C_LASTEVENT)
        {
          diostream->CheckTransition();

          switch(diostream->GetCurrentState())
            {
              case DIOSTREAMI2C_FSMSTATE_NONE               : break;

              case DIOSTREAMI2C_FSMSTATE_CONNECTED          : { DIOSTREAMXEVENT xevent(diostream,DIOSTREAMXEVENT_TYPE_CONNECTED);
                                                                xevent.SetDIOStream(diostream);
                                                                diostream->PostEvent(&xevent);

                                                                diostream->SetEvent(DIOSTREAMI2C_FSMEVENT_WAITINGTOREAD);
                                                              }
                                                              break;

              case DIOSTREAMI2C_FSMSTATE_WAITINGTOREAD      : break;

              case DIOSTREAMI2C_FSMSTATE_DISCONNECTING      : { DIOSTREAMXEVENT xevent(diostream,DIOSTREAMXEVENT_TYPE_DISCONNECTED);
                                                                xevent.SetDIOStream(diostream);
                                                                diostream->PostEvent(&xevent);

                                                                diostream->status = DIOSTREAMSTATUS_DISCONNECTED;
                                                              }
                                                              break;
            }
        }
    }
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOESP32STREAMI2C::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOESP32STREAMI2C::Clean()
{
  hi2c              = NULL;
  localaddress      = 0;
  remoteaddress     = 0;
  threadconnection   = NULL;
  indexhandle       = -1;
  indexport         = -1;
  sizebufferdata    = 0;
}



#endif
