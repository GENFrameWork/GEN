/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOTemHumSensorAM2301.cpp
* 
* @class      DIOTEMHUMSENSORAM2301
* @brief      Data Input/Output AOSONG AM2301 Sensor (Temperature and humidity module) class
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

/*---- PRECOMPILATION INCLUDES ----------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOTemHumSensorAM2301.h"

#include <stdio.h>
#include <stdlib.h>

#include "XFactory.h"
#include "XSleep.h"
#include "XTimer.h"
#include "XTrace.h"
#include "XThreadCollected.h"

#include "DIOGPIO.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOTEMHUMSENSORAM2301::DIOTEMHUMSENSORAM2301(int pindata, bool activatecache)
* @brief      Constructor
* @ingroup    DATAIO
* 
* @param[in]  pindata : 
* @param[in]  activatecache : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOTEMHUMSENSORAM2301::DIOTEMHUMSENSORAM2301(int pindata, bool activatecache) : DIODEVICE()
{
  Clean();

  this->pindata = pindata;

  GEN_XFACTORY_CREATE(xtimer, CreateTimer())
  
  if(activatecache)
    {
      GEN_XFACTORY_CREATE(xmutexread, Create_Mutex())
      if(xmutexread)
        {
          threadcache  = CREATEXTHREAD(XTHREADGROUPID_DIOSTREAM, __L("DIOTEMHUMSENSORAM2301::DIOSENSORAOSONGAM2301"), ThreadRunFunction, (void*)this);
          if(!threadcache) return;

          threadcache->SetPriority(XTHREADPRIORITY_REALTIME);
        }
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOTEMHUMSENSORAM2301::~DIOTEMHUMSENSORAM2301()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOTEMHUMSENSORAM2301::~DIOTEMHUMSENSORAM2301()
{
  End();

  if(threadcache) DELETEXTHREAD(XTHREADGROUPID_DIOSTREAM, threadcache);

  if(xmutexread)  GEN_XFACTORY.Delete_Mutex(xmutexread);
  if(xtimer)      GEN_XFACTORY.DeleteTimer(xtimer);

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOTEMHUMSENSORAM2301::Ini(int cadenceread)
* @brief      Ini
* @ingroup    DATAIO
* 
* @param[in]  cadenceread : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOTEMHUMSENSORAM2301::Ini(int cadenceread)
{
  if(!threadcache) return false;

  this->cadenceread = cadenceread;

  if(!threadcache->Ini()) return false;

  return DIODEVICE::Ini();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOTEMHUMSENSORAM2301::GetPinData()
* @brief      GetPinData
* @ingroup    DATAIO
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOTEMHUMSENSORAM2301::GetPinData()
{ 
  return pindata;                   
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOTEMHUMSENSORAM2301::ReadFromCache(float& temperature, float& humidity)
* @brief      ReadFromCache
* @ingroup    DATAIO
* 
* @param[in]  temperature : 
* @param[in]  humidity : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOTEMHUMSENSORAM2301::ReadFromCache(float& temperature, float& humidity)
{
  if(!xmutexread)      return false;
  if(!IsInitialized()) return false;

  if(!nreads) return false;

  xmutexread->Lock();

  temperature = temperaturecache;
  humidity    = humiditycache;

  xmutexread->UnLock();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOTEMHUMSENSORAM2301::ReadDirect(float& temperature, float& humidity)
* @brief      ReadDirect
* @ingroup    DATAIO
* 
* @param[in]  temperature : 
* @param[in]  humidity : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOTEMHUMSENSORAM2301::ReadDirect(float& temperature, float& humidity)
{
  if(!xtimer)           return false;
  if(!IsInitialized())  return false;

  int    timeelapsedlow;
  int    timeelapsedhigh;
  XBYTE  value[5] = { 0, 0, 0, 0, 0 };

  // Leave it high for a while
  GEN_DIOGPIO.SetMode(pindata, DIOGPIO_MODE_OUTPUT);
  GEN_DIOGPIO.SetValue(pindata, DIOTEMHUMSENSORAM2301_HIGH);
  GEN_XSLEEP.MilliSeconds(1);

  // Set it low to give the start signal
  GEN_DIOGPIO.SetValue(pindata, DIOTEMHUMSENSORAM2301_LOW);
  GEN_XSLEEP.MilliSeconds(1);
  GEN_DIOGPIO.SetValue(pindata, DIOTEMHUMSENSORAM2301_HIGH);

  // Now set the pin high to let the sensor start communicating
  GEN_DIOGPIO.SetMode(pindata, DIOGPIO_MODE_INPUT);

  GEN_XSLEEP.NanoSeconds(30);

  // Read ACK
  WaitTo(false, 50);

  WaitTo(true , 100);
  WaitTo(false, 100);

  // Read values
  for(int d=0; d<5; d++)
    {
      value[d] = 0x00;

      for(int c=0; c<8; c++)
        {
          WaitTo(false, 50);
          WaitTo(true , 100, &timeelapsedlow);
          WaitTo(false, 100, &timeelapsedhigh);

          value[d]|=(timeelapsedlow>timeelapsedhigh?0:1);

          if(c<7)  value[d] <<= 1;
        }
    }


  // Final State of SDA
  GEN_DIOGPIO.SetMode(pindata, DIOGPIO_MODE_OUTPUT);
  GEN_DIOGPIO.SetValue(pindata, DIOTEMHUMSENSORAM2301_HIGH);

  GEN_XSLEEP.MilliSeconds(1);
 
  // XTRACE_PRINTCOLOR(XTRACE_PRINTCOLOR, __L(" %X %X %X %X "), value[0], value[1], value[2],  value[3]);
 
  // Verify checksum
  XBYTE x = value[0] + value[1] + value[2] + value[3];
  if(x != value[4]) return false;

  humidity     = (float)(((XWORD)value[0] << 8) | (XWORD)value[1]);
  humidity    /= 10.0f;

  temperature  = (float)(((XWORD)value[2] << 8) | (XWORD)value[3]);
  temperature /= 10.0f;

  if((humidity > 100.0) || (humidity <= 0.0) || (temperature > 80.0)  || (temperature < -40.0)) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOTEMHUMSENSORAM2301::End()
* @brief      End
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOTEMHUMSENSORAM2301::End()
{
  if(!DIODEVICE::End()) return false;

  if(threadcache) threadcache->End();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOTEMHUMSENSORAM2301::WaitTo(bool tohigh, int timeout, int* timeelapsed)
* @brief      WaitTo
* @ingroup    DATAIO
* 
* @param[in]  tohigh : 
* @param[in]  timeout : 
* @param[in]  timeelapsed : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOTEMHUMSENSORAM2301::WaitTo(bool tohigh, int timeout, int* timeelapsed)
{
  if(!xtimer) return false;

  bool state;
  bool status = true;

  int _timeelapsed = 0;

  do{ state = GEN_DIOGPIO.GetValue(pindata);

      _timeelapsed++;

      if(tohigh)
        {
          if(state) break;
        }
       else
        {
          if(!state) break;
        }

      if(_timeelapsed > timeout)
        {
          status = false;
          break;
        }

    } while(1);

  if(timeelapsed) (*timeelapsed) = _timeelapsed;

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOTEMHUMSENSORAM2301::ThreadRunFunction(void* param)
* @brief      ThreadRunFunction
* @ingroup    DATAIO
* 
* @param[in]  param : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOTEMHUMSENSORAM2301::ThreadRunFunction(void* param)
{
  DIOTEMHUMSENSORAM2301* sensor = (DIOTEMHUMSENSORAM2301*)param;
  if(!sensor) return;

  if(!sensor->IsInitialized()) return;

  float temperature;
  float humidity;

  bool  status    = sensor->ReadDirect(temperature, humidity);
  if(status)
    {
      if(sensor->xmutexread) sensor->xmutexread->Lock();

      if(sensor->nreads>1)
        {
          if((abs((int)sensor->temperaturecache - (int)temperature)>10) || (abs((int)sensor->humiditycache - (int)humidity)>10)) status = false;
        }

      if(status)
        {
          sensor->temperaturecache  = temperature;
          sensor->humiditycache     = humidity;
          sensor->nreads++;
        }

      if(sensor->xmutexread) sensor->xmutexread->UnLock();
    }

  if(sensor->xtimer) GEN_XSLEEP.MilliSeconds(500);

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOTEMHUMSENSORAM2301::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOTEMHUMSENSORAM2301::Clean()
{
  pindata           = DIOGPIO_PINNOTUSE;

  cadenceread       = 0;

  xtimer            = NULL;
  threadcache       = NULL;
  xmutexread        = NULL;

  nreads            = 0;
  temperaturecache  = 0;
  humiditycache     = 0;
}


#pragma endregion

