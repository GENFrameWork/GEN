/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOLightSensorLDRAnalog.cpp
* 
* @class      DIOLIGHTSENSORLDRANALOG
* @brief      Data Input/Output IO Sensor Light LDR (Light Dependent Resistor) Analog in port digital class
*             http://www.raspberrypi-spy.co.uk/2012/08/reading-analogue-sensors-with-one-gpio-pin/
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XFactory.h"
#include "XSleep.h"
#include "XThreadCollected.h"
#include "XTimer.h"

#include "DIOLightSensorLDRAnalog.h"

#include "XMemory_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOLIGHTSENSORLDRANALOG::DIOLIGHTSENSORLDRANALOG( DIOGPIO* diogpio, int pindata, bool activatecache)
* @brief      Constructor
* @ingroup    DATAIO
* 
* @param[in]   DIOGPIO* diogpio : 
* @param[in]   int pindata : 
* @param[in]   bool activatecache : 
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOLIGHTSENSORLDRANALOG::DIOLIGHTSENSORLDRANALOG( DIOGPIO* diogpio, int pindata, bool activatecache) : DIODEVICE()
{
  Clean();

  this->diogpio = diogpio;
  this->pindata     = pindata;

  GEN_XFACTORY_CREATE(xtimer, CreateTimer())
  
  if(activatecache)
    {
      GEN_XFACTORY_CREATE(xmutexread, Create_Mutex())
      if(xmutexread)
        {
          threadcache  = CREATEXTHREAD(XTHREADGROUPID_DIOSTREAM, __L("DIOLIGHTSENSORLDRANALOG::DIOLIGHTSENSORLDRANALOG"), ThreadRunFunction, (void*)this);
          if(!threadcache) return;

          //threadcache->SetPriority(THREADPRIORITY_REALTIME);
        }
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOLIGHTSENSORLDRANALOG::~DIOLIGHTSENSORLDRANALOG()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOLIGHTSENSORLDRANALOG::~DIOLIGHTSENSORLDRANALOG()
{
  End();

  if(threadcache)
    {
      threadcache->End();
      DELETEXTHREAD(XTHREADGROUPID_DIOSTREAM, threadcache);
    }

  if(xmutexread)  GEN_XFACTORY.Delete_Mutex(xmutexread);

  if(xtimer) GEN_XFACTORY.DeleteTimer(xtimer);

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLIGHTSENSORLDRANALOG::Ini()
* @brief      Ini
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLIGHTSENSORLDRANALOG::Ini()
{
  if(!threadcache) return false;

  if(!threadcache->Ini()) return false;

  return DIODEVICE::Ini();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLIGHTSENSORLDRANALOG::ReadFromCache(XDWORD& level)
* @brief      ReadFromCache
* @ingroup    DATAIO
* 
* @param[in]  level : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLIGHTSENSORLDRANALOG::ReadFromCache(XDWORD& level)
{
  if(!xmutexread) return false;

  if(!IsInitialized()) return false;

  if(!nreads) return false;

  xmutexread->Lock();

  level = levelcache;

  xmutexread->UnLock();

  return true;

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLIGHTSENSORLDRANALOG::ReadDirect(XDWORD& level)
* @brief      ReadDirect
* @ingroup    DATAIO
* 
* @param[in]  level : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLIGHTSENSORLDRANALOG::ReadDirect(XDWORD& level)
{
  if(!diogpio)      return false;
  if(!xtimer)           return false;
  if(!IsInitialized())  return false;

  level = 0;

  xtimer->Reset();

  // Discharge capacitor
  diogpio->SetMode(pindata, false);
  diogpio->Set(pindata    , false);
  GEN_XSLEEP.MilliSeconds(100);

  diogpio->SetMode(pindata,  true);
  while(!diogpio->Get(pindata))
    {
      level++;
      if(!(level%100))
        {
          if(xtimer->GetMeasureSeconds()>=1) return false;
        }
    }

  if(!level) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLIGHTSENSORLDRANALOG::End()
* @brief      End
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLIGHTSENSORLDRANALOG::End()
{
  if(!DIODEVICE::End()) return false;

  if(threadcache) threadcache->End();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOLIGHTSENSORLDRANALOG::ThreadRunFunction(void* param)
* @brief      ThreadRunFunction
* @ingroup    DATAIO
* 
* @param[in]  param : 
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOLIGHTSENSORLDRANALOG::ThreadRunFunction(void* param)
{
  DIOLIGHTSENSORLDRANALOG* sensor = (DIOLIGHTSENSORLDRANALOG*)param;
  if(!sensor) return;

  if(!sensor->IsInitialized()) return;

  XDWORD level;

  bool status = sensor->ReadDirect(level);
  if(status)
    {
      if(sensor->xmutexread) sensor->xmutexread->Lock();

      sensor->levelcache  = level;
      sensor->nreads++;

      if(sensor->xmutexread) sensor->xmutexread->UnLock();
    }

  if(sensor->xtimer)
    {
      if(sensor->threadcache->IsRunning()) GEN_XSLEEP.Seconds(1);
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOLIGHTSENSORLDRANALOG::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOLIGHTSENSORLDRANALOG::Clean()
{
  diogpio       = NULL;
  pindata           = DIOGPIO_PINNOTUSE;

  xtimer            = NULL;
  threadcache       = NULL;
  xmutexread        = NULL;

  nreads            = 0;
  levelcache        = 0;
}
