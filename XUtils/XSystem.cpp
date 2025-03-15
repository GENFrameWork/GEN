/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XSystem.cpp
* 
* @class      XSYSTEM
* @brief      eXtended Utils System class
* @note       Cant be construct Factory + singelton without depends of system. ITS NOT A SINGLETON.
* @ingroup    XUTILS
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
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XSystem.h"

#include "XFactory.h"
#include "XPath.h"
#include "XLanguage_ISO_639_3.h"
#include "XThread.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

XSYSTEM* XSYSTEM::instance            = NULL;

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSYSTEM::XSYSTEM()
* @brief      Constructor of class
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
XSYSTEM::XSYSTEM()
{
  Clean();

  xmutexcheckCPUusage = GEN_XFACTORY.Create_Mutex();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSYSTEM::~XSYSTEM()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
XSYSTEM::~XSYSTEM()
{
  if(xmutexcheckCPUusage)
    {
      GEN_XFACTORY.Delete_Mutex(xmutexcheckCPUusage);
    }

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::GetIsInstanced()
* @brief      Get is instanced
* @note       STATIC
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::GetIsInstanced()
{
  return instance!=NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSYSTEM& XSYSTEM::GetInstance()
* @brief      Get instance
* @note       STATIC
* @ingroup    XUTILS
*
* @return     XSYSTEM& :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSYSTEM& XSYSTEM::GetInstance()
{
  if(!instance) instance = new XSYSTEM();

  return (*instance);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::SetInstance(XSYSTEM* _instance)
* @brief      Set instance
* @note       STATIC
* @ingroup    XUTILS
*
* @param[in]  _instance : new instance
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::SetInstance(XSYSTEM* _instance)
{
  if(!_instance) return false;

  instance = _instance;

  return (instance)?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::DelInstance()
* @brief      Del instance
* @note       STATIC
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::DelInstance()
{
  if(instance)
    {
      delete instance;
      instance = NULL;

      return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSYSTEM_HARDWARETYPE XSYSTEM::GetTypeHardware(int* revision)
* @brief      Get type hardware
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @param[in]  revision : revision (Pi rules)
*
* @return     XSYSTEM_HARDWARETYPE : Type Hardware (enum XSYSTEM_HARDWARETYPE)
*
* --------------------------------------------------------------------------------------------------------------------*/
XSYSTEM_HARDWARETYPE  XSYSTEM::GetTypeHardware(int* revision)
{
  return XSYSTEM_HARDWARETYPE_UNKNOWN;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::HardwareUseLittleEndian()
* @brief      Hardware use little endian
* @note       In ARM can be changed, can be used BIG ENDIAN or LITTLE ENDIAN
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::HardwareUseLittleEndian()
{
  switch(GetTypeHardware())
    {
      case XSYSTEM_HARDWARETYPE_UNKNOWN         : return true;

      case XSYSTEM_HARDWARETYPE_PC              : return true;
      case XSYSTEM_HARDWARETYPE_LEX3V700A       : return true;

      case XSYSTEM_HARDWARETYPE_ARMGENERIC      : return true;
      case XSYSTEM_HARDWARETYPE_ARTILA500       : return false;
      case XSYSTEM_HARDWARETYPE_GUMSTIXCONNEX   : return false;
      case XSYSTEM_HARDWARETYPE_RASPBERRYPI     : return true;
      case XSYSTEM_HARDWARETYPE_BEAGLEBONE      : return true;
      case XSYSTEM_HARDWARETYPE_NXP_IMX6        : return true;
      case XSYSTEM_HARDWARETYPE_NXP_IMX8        : return true;

      case XSYSTEM_HARDWARETYPE_MICRO_STM32     : return true;
      case XSYSTEM_HARDWARETYPE_MICRO_ESP32     : return true;
      case XSYSTEM_HARDWARETYPE_MICRO_SAMD5XE5X : return true;

                                      default   : return true;
    }

  //return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSYSTEM_PLATFORM XSYSTEM::GetPlatform(XSTRING* namestring = NULL)
* @brief      Get platform
* @ingroup    XUTILS
*
* @param[in]  namestring : 
* 
* @return     XSYSTEM_PLATFORM : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSYSTEM_PLATFORM XSYSTEM::GetPlatform(XSTRING* namestring)
{
  if(namestring)  namestring->Set(__L("Unknown"));

  return XSYSTEM_PLATFORM_UNKNOWN;  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSYSTEM::GetOperativeSystemID(XSTRING& ID)
* @brief      Get operative system ID
* @ingroup    XUTILS
*
* @param[in]  ID : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::GetOperativeSystemID(XSTRING& ID)
{
  ID.Empty();
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD XSYSTEM::GetLanguageSO()
* @brief      Get language SO
* @ingroup    XUTILS
* 
* @return     XDWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD XSYSTEM::GetLanguageSO()
{
  return XLANGUAGE_ISO_639_3_CODE_INVALID;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* XSYSTEM::GetBIOSSerialNumber()
* @brief      Get BIOS serial number
* @ingroup    XUTILS
* 
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* XSYSTEM::GetBIOSSerialNumber()
{
  return &BIOSserialnumber;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* XSYSTEM::GetCPUSerialNumber()
* @brief      Get CPU serial number
* @ingroup    XUTILS
* 
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* XSYSTEM::GetCPUSerialNumber()
{
  return &CPUserialnumber;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         float XSYSTEM::GetCPUTemperature()
* @brief      Get CPU temperature
* @ingroup    XUTILS
* 
* @return     float : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
float XSYSTEM::GetCPUTemperature()
{
  return 0.0f;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::GetMemoryInfo(XDWORD& total,XDWORD& free)
* @brief      Get memory info
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @param[out] total : total memory in bytes.
* @param[out] free :  free  memory in bytes
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::GetMemoryInfo(XDWORD& total,XDWORD& free)
{
  total = 0;
  free  = 0;

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XSYSTEM::GetFreeMemoryPercent()
* @brief      Get free memory percent
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XSYSTEM::GetFreeMemoryPercent()
{
  XDWORD total;
  XDWORD free;
  int    percent;

  if(!GetMemoryInfo(total,free)) return false;

  percent = ((100*free)/total);

  return percent;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::FreeCacheMemory()
* @brief      Free cache memory
* @ingroup    XUTILS
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::FreeCacheMemory()
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XSYSTEM::GetCPUUsageTotal()
* @brief      Get CPU usage total
* @ingroup    XUTILS
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int XSYSTEM::GetCPUUsageTotal()
{
  return XSYSTEM_CPUUSAGE_ERROR;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XSYSTEM::GetCPUUsageForProcessName(XCHAR* processname)
* @brief      Get CPU usage for process name
* @ingroup    XUTILS
*
* @param[in]  processname : 
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int XSYSTEM::GetCPUUsageForProcessName(XCHAR* processname)
{
  return XSYSTEM_CPUUSAGE_ERROR;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XSYSTEM::GetCPUUsageForProcessID(XDWORD processID)
* @brief      Get CPU usage for process ID
* @ingroup    XUTILS
*
* @param[in]  processID : 
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int XSYSTEM::GetCPUUsageForProcessID(XDWORD processID)
{
  return XSYSTEM_CPUUSAGE_ERROR;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XCHAR* XSYSTEM::GetEnviromentVariable(XCHAR* name)
* @brief      Get enviroment variable
* @ingroup    XUTILS
* 
* @param[in]  name : 
* 
* @return     XCHAR* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* XSYSTEM::GetEnviromentVariable(XCHAR* name)
{
  return __L("");
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XCHAR* XSYSTEM::GetEnviromentVariable(XSTRING& name)
* @brief      Get enviroment variable
* @ingroup    XUTILS
* 
* @param[in]  name : 
* 
* @return     XCHAR* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* XSYSTEM::GetEnviromentVariable(XSTRING& name)
{
  return GetEnviromentVariable(name.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSYSTEM::SetEnviromentVariable(XCHAR* name, XCHAR* value)
* @brief      Set enviroment variable
* @ingroup    XUTILS
* 
* @param[in]  name : 
* @param[in]  value : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::SetEnviromentVariable(XCHAR* name, XCHAR* value)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSYSTEM::SetEnviromentVariable(XSTRING& name, XSTRING& value)
* @brief      Set enviroment variable
* @ingroup    XUTILS
* 
* @param[in]  name : 
* @param[in]  value : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::SetEnviromentVariable(XSTRING& name, XSTRING& value)
{
  return SetEnviromentVariable(name.Get(), value.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSYSTEM::DelEnviromentVariable(XCHAR* name)
* @brief      Del enviroment variable
* @ingroup    XUTILS
* 
* @param[in]  name : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::DelEnviromentVariable(XCHAR* name)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSYSTEM::DelEnviromentVariable(XSTRING& name)
* @brief      Del enviroment variable
* @ingroup    XUTILS
* 
* @param[in]  name : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::DelEnviromentVariable(XSTRING& name)
{
  return DelEnviromentVariable(name.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::GetUserAndDomain(XSTRING& user, XSTRING& domain)
* @brief      Get user and domain
* @ingroup    XUTILS
*
* @param[in]  user : 
* @param[in]  domain : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::GetUserAndDomain(XSTRING& user, XSTRING& domain)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::ShutDown(XSYSTEM_CHANGESTATUSTYPE type)
* @brief      Shut down
* @ingroup    XUTILS
*
* @param[in]  type : type of shutdown (enum XSYSTEM_CHANGESTATUSTYPE)
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::ShutDown(XSYSTEM_CHANGESTATUSTYPE type)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XSYSTEM::Sound_GetLevel()
* @brief      Sound get level
* @ingroup    XUTILS
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int XSYSTEM::Sound_GetLevel()
{
  return 0;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::Sound_SetLevel(int level)
* @brief      Sound set level
* @ingroup    XUTILS
*
* @param[in]  level : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::Sound_SetLevel(int level)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::Sound_SetMutex(bool on)
* @brief      Sound set mutex
* @ingroup    XUTILS
*
* @param[in]  on : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::Sound_SetMutex(bool on)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSYSTEM::GetBatteryLevel (bool& isincharge, XBYTE& levelpercent)
* @brief      Get battery level 
* @ingroup    XUTILS
*
* @param[in]  isincharge : 
* @param[in]  levelpercent : 
* 
* @return     virtual : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::GetBatteryLevel (bool& isincharge, XBYTE& levelpercent)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int XSYSTEM::GetWifiRSSILevel()
* @brief      get wifi RSSIlevel
* @ingroup    XUTILS
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int XSYSTEM::GetWifiRSSILevel()
{
  return -1;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSYSTEM::GetPathExecApplication(XCHAR* appname, XPATH& apppath)
* @brief      get path exec application
* @ingroup    XUTILS
* 
* @param[in]  appname : 
* @param[in]  apppath : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::GetPathExecApplication(XCHAR* appname, XPATH& apppath)
{
  apppath.Empty();

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSYSTEM::GetPathExecApplication(XSTRING* appname, XPATH& apppath)
* @brief      get path exec application
* @ingroup    XUTILS
* 
* @param[in]  appname : 
* @param[in]  apppath : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::GetPathExecApplication(XSTRING* appname, XPATH& apppath)
{
  if(!appname)
    {
      return false;
    }

  return GetPathExecApplication(appname->Get(), apppath);
}


#ifndef MICROCONTROLLER

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSYSTEM::HostFile(bool add, XCHAR* domain, XCHAR* IP)
* @brief      Host file
* @ingroup    XUTILS
*
* @param[in]  add : 
* @param[in]  domain : 
* @param[in]  IP : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::HostFile(bool add, XCHAR* domain, XCHAR* IP)
{
  XPATH     xpath;
  XFILETXT  xfileTXT;
  bool      status = false;
  
  #ifdef WINDOWS
  xpath = __L("C:\\Windows\\System32\\drivers\\etc\\hosts");
  #endif

  #if defined(LINUX) || defined(ANDROID)
  xpath = __L("/etc/hosts");
  #endif

  if(xfileTXT.Open(xpath, false))
    {
      xfileTXT.ReadAllFile();

      if(add)
        {
          int nline = IsLineInHostFile(xfileTXT, domain, IP);                              
          if(nline == XSTRING_NOTFOUND)
            {
              XSTRING line;

              line.Format(__L("%s %s"), IP, domain);  

              xfileTXT.AddLine(line);    
              xfileTXT.WriteAllFile();
            }
        }
       else
        {  
          int nline = IsLineInHostFile(xfileTXT, domain, IP);                           
          if(nline != XSTRING_NOTFOUND)
            {
              xfileTXT.DeleteLine(nline);         
              status = xfileTXT.WriteAllFile();
            }
        }

      xfileTXT.Close();
    }

  return status;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSYSTEM::HostFile(bool add, XSTRING& domain, XSTRING& IP)
* @brief      Host file
* @ingroup    XUTILS
*
* @param[in]  add : 
* @param[in]  domain : 
* @param[in]  IP : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::HostFile(bool add, XSTRING& domain, XSTRING& IP)
{
  return HostFile(add, domain.Get(), IP.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int XSYSTEM::IsLineInHostFile(XFILETXT& xfileTXT, XCHAR* domain, XCHAR* IP)
* @brief      Is line in host file
* @ingroup    XUTILS
*
* @param[in]  xfileTXT : 
* @param[in]  domain : 
* @param[in]  IP : 
* 
* @return     int : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
int XSYSTEM::IsLineInHostFile(XFILETXT& xfileTXT, XCHAR* domain, XCHAR* IP)
{
  bool found = false;    
  int  c     = 0;

  if(!xfileTXT.GetNLines()) return XSTRING_NOTFOUND;

  do{ XSTRING* line = xfileTXT.GetLine(c); 
      if(line)
        {
          int index = line->Find(domain, true);  
          if(index != XSTRING_NOTFOUND) 
            {     
              found = true;

              for(XDWORD d=0; d<line->GetSize(); d++)
                {
                  XCHAR character = line->Get()[d];

                  if((character != __C('\t') && (character != __C(' '))))     
                    {
                      if(character == __C('#'))     
                        {
                          found = false;
                        }

                      break;
                    }               
                }
                      
              if(found) break; 
            }
        }

      c++;

    } while(c<xfileTXT.GetNLines());

  if(!found) return XSTRING_NOTFOUND;

  return c;
}

#endif


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XSYSTEM::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
void XSYSTEM::Clean()
{
  xmutexcheckCPUusage = NULL;
}


#pragma endregion

