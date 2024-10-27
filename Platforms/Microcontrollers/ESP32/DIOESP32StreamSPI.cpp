/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOESP32StreamSPI.cpp
*
* @class      DIOESP32STREAMSPI
* @brief      Data Input/Output ESP32 Stream SPI class
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

#if defined(DIO_ACTIVE) && defined(DIO_STREAMSPI_ACTIVE)

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XFactory.h"
#include "XThreadCollected.h"

#include "DIOStreamXEvent.h"
#include "DIOStreamSPIConfig.h"

#include "DIOESP32StreamSPI.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

DIOESP32STREAMSPIPORT*    DIOESP32STREAMSPI::ports[DIOESP32STREAMSPI_MAXPORTS]     = { NULL, NULL };
DIOESP32STREAMSPI*        DIOESP32STREAMSPI::handles[DIOESP32STREAMSPI_MAXHANDLES] = { NULL, NULL };

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/
/*  DIOESP32STREAMSPIPORT                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOESP32STREAMSPIPORT::DIOESP32STREAMSPIPORT()
* @brief      Constructor
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOESP32STREAMSPIPORT::DIOESP32STREAMSPIPORT()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOESP32STREAMSPIPORT::~DIOESP32STREAMSPIPORT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOESP32STREAMSPIPORT::~DIOESP32STREAMSPIPORT()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOESP32STREAMSPIPORT::GetCounterRef()
* @brief      GetCounterRef
* @ingroup    PLATFORM_ESP32
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOESP32STREAMSPIPORT::GetCounterRef()
{
  return counterref;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOESP32STREAMSPIPORT::SetCounterRef(XDWORD counterref)
* @brief      SetCounterRef
* @ingroup    PLATFORM_ESP32
*
* @param[in]  counterref :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOESP32STREAMSPIPORT::SetCounterRef(XDWORD counterref)
{
  this->counterref = counterref;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         SPI_HandleTypeDef* DIOESP32STREAMSPIPORT::GetHandleSPI()
* @brief      GetHandleSPI
* @ingroup    PLATFORM_ESP32
*
* @return     SPI_HandleTypeDef* :
*
* --------------------------------------------------------------------------------------------------------------------*/
SPI_HandleTypeDef* DIOESP32STREAMSPIPORT::GetHandleSPI()
{
  return hspi;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOESP32STREAMSPIPORT::SetHandleSPI(SPI_HandleTypeDef* hspi)
* @brief      SetHandleSPI
* @ingroup    PLATFORM_ESP32
*
* @param[in]  hspi :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOESP32STREAMSPIPORT::SetHandleSPI(SPI_HandleTypeDef* hspi)
{
  this->hspi = hspi;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOESP32STREAMSPIPORT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOESP32STREAMSPIPORT::Clean()
{
  counterref        = 0;
  hspi              = NULL;
}




/*--------------------------------------------------------------------------------------------------------------------*/
/*  DIOESP32STREAMSPI                                                                                                 */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOESP32STREAMSPI::DIOESP32STREAMSPI()
* @brief      Constructor
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOESP32STREAMSPI::DIOESP32STREAMSPI() : DIOSTREAMSPI()
{
  Clean();

  AddState( DIOSTREAMSPI_FSMSTATE_NONE            ,
            DIOSTREAMSPI_FSMEVENT_CONNECTED       , DIOSTREAMSPI_FSMSTATE_CONNECTED        ,
            DIOSTREAMSPI_FSMEVENT_DISCONNECTING   , DIOSTREAMSPI_FSMSTATE_DISCONNECTING    ,
            XFSMACHINESTATE_EVENTDEFEND);

  AddState( DIOSTREAMSPI_FSMSTATE_CONNECTED       ,
            DIOSTREAMSPI_FSMEVENT_WAITINGTOREAD   , DIOSTREAMSPI_FSMSTATE_WAITINGTOREAD    ,
            DIOSTREAMSPI_FSMEVENT_DISCONNECTING   , DIOSTREAMSPI_FSMSTATE_DISCONNECTING    ,
            XFSMACHINESTATE_EVENTDEFEND);

  AddState( DIOSTREAMSPI_FSMSTATE_WAITINGTOREAD   ,
            DIOSTREAMSPI_FSMEVENT_CONNECTED       , DIOSTREAMSPI_FSMSTATE_CONNECTED        ,
            DIOSTREAMSPI_FSMEVENT_DISCONNECTING   , DIOSTREAMSPI_FSMSTATE_DISCONNECTING    ,
            XFSMACHINESTATE_EVENTDEFEND);

  AddState( DIOSTREAMSPI_FSMSTATE_DISCONNECTING   ,
            DIOSTREAMSPI_FSMEVENT_CONNECTED       , DIOSTREAMSPI_FSMSTATE_CONNECTED        ,
            DIOSTREAMSPI_FSMEVENT_WAITINGTOREAD   , DIOSTREAMSPI_FSMSTATE_WAITINGTOREAD    ,
            XFSMACHINESTATE_EVENTDEFEND);

  threadconnection = CREATEXTHREAD(XTHREADGROUPID_DIOSTREAMSPI, __L("DIOESP32STREAMSPI::DIOESP32STREAMSPI"), ThreadConnection, (void*)this);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOESP32STREAMSPI::~DIOESP32STREAMSPI()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOESP32STREAMSPI::~DIOESP32STREAMSPI()
{
  Close();

  if(threadconnection) DELETEXTHREAD(XTHREADGROUPID_DIOSTREAMSPI, threadconnection);

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMSTATUS DIOESP32STREAMSPI::GetStatus()
* @brief      GetStatus
* @ingroup    PLATFORM_ESP32
*
* @return     DIOSTREAMSTATUS :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMSTATUS DIOESP32STREAMSPI::GetStatus()
{
  if(!config) return DIOSTREAMSTATUS_DISCONNECTED;

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOESP32STREAMSPI::Open()
* @brief      Open
* @ingroup    PLATFORM_ESP32
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOESP32STREAMSPI::Open()
{
  if(!threadconnection)  return false;
  if(!config)           return false;

 for(int c=0; c<DIOESP32STREAMSPI_MAXHANDLES; c++)
    {
      if(DIOESP32STREAMSPI::handles[c] == this) return false;

      if(!DIOESP32STREAMSPI::handles[c])
        {
          indexhandle = c;
          DIOESP32STREAMSPI::handles[indexhandle] = this;
          break;
        }
    }

  if(indexhandle == -1) return false;

  indexport = config->GetPort()-1;

  if(!DIOESP32STREAMSPI::ports[indexport])
    {
      DIOESP32STREAMSPIPORT* port = new DIOESP32STREAMSPIPORT();
      if(!port) return false;

      hspi = new SPI_HandleTypeDef();
      if(!hspi)
        {
          delete port;
          return false;
        }

      switch(config->GetPort())
        {
          case  1 : hspi->Instance = SPI1;       break;
          case  2 : hspi->Instance = SPI2;       break;
          default : return false;
        }

     

      hspi->Init.Mode               = SPI_MODE_MASTER;
      hspi->Init.Direction          = SPI_DIRECTION_2LINES;
      hspi->Init.DataSize           = SPI_DATASIZE_8BIT;
      hspi->Init.CLKPolarity        = SPI_POLARITY_LOW;
      hspi->Init.CLKPhase           = SPI_PHASE_1EDGE;
      hspi->Init.NSS                = SPI_NSS_HARD_OUTPUT;
      hspi->Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_2;
      hspi->Init.FirstBit           = SPI_FIRSTBIT_MSB;
      hspi->Init.TIMode             = SPI_TIMODE_DISABLE;
      hspi->Init.CRCCalculation     = SPI_CRCCALCULATION_DISABLE;
      hspi->Init.CRCPolynomial      = 7;
      hspi->Init.CRCLength          = SPI_CRC_LENGTH_DATASIZE;
      hspi->Init.NSSPMode           = SPI_NSS_PULSE_ENABLE;

      if(HAL_SPI_Init(hspi) != HAL_OK)
        {
          return false;
        }

      port->SetHandleSPI(hspi);
      DIOESP32STREAMSPI::ports[indexport] = port;
    }
   else
    {
      hspi = DIOESP32STREAMSPI::ports[indexport]->GetHandleSPI();
      DIOESP32STREAMSPI::ports[indexport]->SetCounterRef(DIOESP32STREAMSPI::ports[indexport]->GetCounterRef()+1);
    }

  SetEvent(DIOSTREAMSPI_FSMEVENT_CONNECTED);

  status = DIOSTREAMSTATUS_CONNECTED;

  memset(bufferdata, 0, DIOSTREAM_MAXBUFFER);

  ResetXBuffers();
  ResetConnectionStatistics();

  return threadconnection->Ini();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOESP32STREAMSPI::WaitToFilledReadingBuffer(int filledto, int timeout)
* @brief      WaitToFilledReadingBuffer
* @ingroup    PLATFORM_ESP32
*
* @param[in]  filledto :
* @param[in]  timeout :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOESP32STREAMSPI::WaitToFilledReadingBuffer(int filledto, int timeout)
{
  if(filledto == DIOSTREAM_SOMETHINGTOREAD) sizebufferdata = 1; else sizebufferdata = filledto;

  switch(config->GetMode())
    {
      case DIOSTREAMMODE_MASTER     :
      case DIOSTREAMMODE_SEMIMASTER :
      case DIOSTREAMMODE_SLAVE      : HAL_SPI_Transmit_IT(hspi, (uint8_t*)bufferdata, sizebufferdata);
                                      break;
                           default  : break;
    }

  return DIOSTREAM::WaitToFilledReadingBuffer(sizebufferdata, timeout);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOESP32STREAMSPI::ReadDirect(XBYTE* buffer, XDWORD size)
* @brief      ReadDirect
* @ingroup    PLATFORM_ESP32
*
* @param[in]  buffer :
* @param[in]  size :
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOESP32STREAMSPI::ReadDirect(XBYTE* buffer, XDWORD size)
{
  if(!config)                                          return 0;
  if(GetStatus()==DIOSTREAMSTATUS_DISCONNECTED) return 0;

  XDWORD br = 0;

  switch(config->GetMode())
    {
      case DIOSTREAMMODE_MASTER     :
      case DIOSTREAMMODE_SEMIMASTER :
      case DIOSTREAMMODE_SLAVE      : if(HAL_SPI_Receive(hspi, buffer, size, 100) == HAL_OK) br = size;
                                      break;
                           default  : break;
    }

  return br;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOESP32STREAMSPI::WriteDirect(XBYTE* buffer, XDWORD size)
* @brief      WriteDirect
* @ingroup    PLATFORM_ESP32
*
* @param[in]  buffer :
* @param[in]  size :
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOESP32STREAMSPI::WriteDirect(XBYTE* buffer, XDWORD size)
{
  if(!config)                                          return 0;
  if(GetStatus()==DIOSTREAMSTATUS_DISCONNECTED) return 0;

  XDWORD bw = 0;

  if(!size) return 0;

   switch(config->GetMode())
    {
      case DIOSTREAMMODE_MASTER       :
      case DIOSTREAMMODE_SEMIMASTER   :
      case DIOSTREAMMODE_SLAVE        : if(HAL_SPI_Transmit(hspi, buffer, size, 100) == HAL_OK) bw = size;
                                        break;
                             default  : break;
    }

  return bw;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOESP32STREAMSPI::Write(XBYTE* buffer, XDWORD size)
* @brief      Write
* @ingroup    PLATFORM_ESP32
*
* @param[in]  buffer :
* @param[in]  size :
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOESP32STREAMSPI::Write(XBYTE* buffer, XDWORD size)
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
* @fn         bool DIOESP32STREAMSPI::Disconnect()
* @brief      Disconnect
* @ingroup    PLATFORM_ESP32
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOESP32STREAMSPI::Disconnect()
{
  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOESP32STREAMSPI::Close()
* @brief      Close
* @ingroup    PLATFORM_ESP32
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOESP32STREAMSPI::Close()
{
  if(!threadconnection) return false;

  threadconnection->End();

  if(GetStatus()==DIOSTREAMSTATUS_DISCONNECTED) return false;

  if(!DIOESP32STREAMSPI::handles[indexhandle]) return false;

  DIOESP32STREAMSPI::handles[indexhandle] = NULL;

  if(!DIOESP32STREAMSPI::ports[indexport]->GetCounterRef())
    {
      HAL_SPI_DeInit(DIOESP32STREAMSPI::ports[indexport]->GetHandleSPI());

      delete DIOESP32STREAMSPI::ports[indexport]->GetHandleSPI();
      delete DIOESP32STREAMSPI::ports[indexport];

      DIOESP32STREAMSPI::ports[indexport] = NULL;
    }
   else
   {
     DIOESP32STREAMSPI::ports[indexport]->SetCounterRef(DIOESP32STREAMSPI::ports[indexport]->GetCounterRef()-1);
   }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOESP32STREAMSPI::CleanBuffers()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_ESP32
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOESP32STREAMSPI::CleanBuffers()
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void HAL_SPI_MasterRxCpltCallback(SPI_HandleTypeDef* hspi)
* @brief      AL_SPI_MasterRxCpltCallback
* @ingroup    PLATFORM_ESP32
*
* @param[in]  hspi :
*
* --------------------------------------------------------------------------------------------------------------------*/
void HAL_SPI_MasterRxCpltCallback(SPI_HandleTypeDef* hspi)
{
  int port = -1;

  if(hspi->Instance == SPI1) port = 0;
  if(hspi->Instance == SPI2) port = 1;

  for(int c=0; c<DIOESP32STREAMSPI_MAXHANDLES; c++)
    {
      if(DIOESP32STREAMSPI::handles[c])
        {
          if(DIOESP32STREAMSPI::handles[c]->indexport  == port)
            {
              if(DIOESP32STREAMSPI::handles[c]->GetStatus() == DIOSTREAMSTATUS_DISCONNECTED) return;

              if(!DIOESP32STREAMSPI::handles[c]->IsBlockRead())
                {
                   DIOESP32STREAMSPI::handles[c]->inbuffer->Add(DIOESP32STREAMSPI::handles[c]->bufferdata, DIOESP32STREAMSPI::handles[c]->sizebufferdata);
                }

              DIOESP32STREAMSPI::handles[c]->sizebufferdata = 0;
            }
        }
    }
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void HAL_SPI_SlaveRxCpltCallback(SPI_HandleTypeDef* hspi)
* @brief      AL_SPI_SlaveRxCpltCallback
* @ingroup    PLATFORM_ESP32
*
* @param[in]  hspi :
*
* --------------------------------------------------------------------------------------------------------------------*/
void HAL_SPI_SlaveRxCpltCallback(SPI_HandleTypeDef* hspi)
{
  HAL_SPI_MasterRxCpltCallback(hspi);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void HAL_SPI_ErrorCallback(SPI_HandleTypeDef*hspi)
* @brief      AL_SPI_ErrorCallback
* @ingroup    PLATFORM_ESP32
*
* @param[in]  SPI_HandleTypeDef*hspi :
*
* --------------------------------------------------------------------------------------------------------------------*/
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("SPI comunication error [%d]"), hspi->ErrorCode);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOESP32STREAMSPI::ThreadConnection(void* param)
* @brief      ThreadConnection
* @ingroup    PLATFORM_ESP32
*
* @param[in]  param :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOESP32STREAMSPI::ThreadConnection(void* param)
{
  DIOESP32STREAMSPI* diostream = (DIOESP32STREAMSPI*)param;
  if(!diostream) return;

  //HAL_NVIC_DisableIRQ(TIM3_IRQn);

  if(diostream->GetEvent()==DIOSTREAMSPI_FSMEVENT_NONE) // No hay nuevos Eventos
    {
      switch(diostream->GetCurrentState())
        {
          case DIOSTREAMSPI_FSMSTATE_NONE                   : break;

          case DIOSTREAMSPI_FSMSTATE_CONNECTED              : break;

          case DIOSTREAMSPI_FSMSTATE_WAITINGTOREAD          : if(!diostream->IsBlockRead())
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
                                                                          diostream->WriteDirect(diostream->writebuffer, esize);
                                                                        }
                                                                    }
                                                                }
                                                              break;

          case DIOSTREAMSPI_FSMSTATE_DISCONNECTING          : break;

        }
    }
   else
    {
      if(diostream->GetEvent()<DIOSTREAMSPI_LASTEVENT)
        {
          diostream->CheckTransition();

          switch(diostream->GetCurrentState())
            {
              case DIOSTREAMSPI_FSMSTATE_NONE               : break;

              case DIOSTREAMSPI_FSMSTATE_CONNECTED          : { DIOSTREAMXEVENT xevent(diostream,DIOSTREAMXEVENT_TYPE_CONNECTED);
                                                                xevent.SetDIOStream(diostream);
                                                                diostream->PostEvent(&xevent);

                                                                diostream->SetEvent(DIOSTREAMSPI_FSMEVENT_WAITINGTOREAD);
                                                              }
                                                              break;

              case DIOSTREAMSPI_FSMSTATE_WAITINGTOREAD      : break;

              case DIOSTREAMSPI_FSMSTATE_DISCONNECTING      : { DIOSTREAMXEVENT xevent(diostream,DIOSTREAMXEVENT_TYPE_DISCONNECTED);
                                                                xevent.SetDIOStream(diostream);
                                                                diostream->PostEvent(&xevent);

                                                                diostream->status = DIOSTREAMSTATUS_DISCONNECTED;
                                                              }
                                                              break;
            }
        }
    }

  //HAL_NVIC_EnableIRQ(TIM3_IRQn);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOESP32STREAMSPI::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOESP32STREAMSPI::Clean()
{
  hspi              = NULL;
  localaddress      = 0;
  remoteaddress     = 0;
  threadconnection   = NULL;
  indexhandle       = -1;
  indexport         = -1;
  sizebufferdata    = 0;
}





#endif
