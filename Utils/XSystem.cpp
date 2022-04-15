/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XSystem.cpp
*
* @class      XSYSTEM
* @brief      System class
* @note        Can�t be construct Factory + singelton without depends of system. IT�S NOT A SINGLETON.
* @ingroup    UTILS
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

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XPath.h"
#include "XLanguage_ISO_639_3.h"

#include "XSystem.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

XSYSTEM* XSYSTEM::instance = NULL;

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSYSTEM::XSYSTEM()
* @brief      Constructor
* @ingroup    UTILS
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XSYSTEM::XSYSTEM()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSYSTEM::~XSYSTEM()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    UTILS
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XSYSTEM::~XSYSTEM()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::GetIsInstanced()
* @brief      GetIsInstanced
* @note       STATIC
* @ingroup    UTILS
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::GetIsInstanced()
{
  return instance!=NULL;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSYSTEM& XSYSTEM::GetInstance()
* @brief      GetInstance
* @note       STATIC
* @ingroup    UTILS
*
* @return     XSYSTEM& :
*
*---------------------------------------------------------------------------------------------------------------------*/
XSYSTEM& XSYSTEM::GetInstance()
{
  if(!instance) instance = new XSYSTEM();

  return (*instance);
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::SetInstance(XSYSTEM* instance)
* @brief      SetInstance
* @note       STATIC
* @ingroup    UTILS
*
* @param[in]  _instance : new instance
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::SetInstance(XSYSTEM* _instance)
{
  if(!_instance) return false;

  instance = _instance;

  return (instance)?true:false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::DelInstance()
* @brief      DelInstance
* @note       STATIC
* @ingroup    UTILS
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
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
* @brief      Get Type Hardware
* @note       VIRTUAL
* @ingroup    UTILS
*
* @param[in]  revision : revision (Pi rules)
*
* @return     XSYSTEM_HARDWARETYPE : Type Hardware (enum XSYSTEM_HARDWARETYPE)
*
*---------------------------------------------------------------------------------------------------------------------*/
XSYSTEM_HARDWARETYPE  XSYSTEM::GetTypeHardware(int* revision)
{
  return XSYSTEM_HARDWARETYPE_UNKNOWN;
}






/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::HardwareUseLittleEndian()
* @brief      Hardware Use LittleEndian
* @note       In ARM can be changed, can be used BIG ENDIAN or LITTLE ENDIAN
* @ingroup    UTILS
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
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
      case XSYSTEM_HARDWARETYPE_MICRO_SAMD5XE5X : return true;

                                      default   : return true;
    }

  //return false;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSYSTEM_PLATFORM XSYSTEM::GetPlatform(XSTRING* namestring = NULL)
* @brief      GetPlatform
* @ingroup    UTILS
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
* @brief      GetOperativeSystemID
* @ingroup    UTILS
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
* @fn         XLANGUAGE_CODE XSYSTEM::GetLanguage()
* @brief      Get Language Code
* @note       VIRTUAL
* @ingroup    UTILS
*
* @return     XLANGUAGE_CODE : Language code (enum XLANGUAGE_CODE)
*
*---------------------------------------------------------------------------------------------------------------------*/
XDWORD XSYSTEM::GetLanguageSO()
{
  return XLANGUAGE_ISO_639_3_CODE_INVALID;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XSYSTEM::GetSerialNumber()
* @brief      GetSerialNumber
* @ingroup    UTILS
*
* @return     XSTRING* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XSYSTEM::GetSerialNumber()
{
  return &serialnumber;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::GetMemoryInfo(XDWORD& total,XDWORD& free)
* @brief      GetMemoryInfo
* @note       VIRTUAL
* @ingroup    UTILS
*
* @param[out] total : total memory in bytes.
* @param[out] free :  free  memory in bytes
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::GetMemoryInfo(XDWORD& total,XDWORD& free)
{
  total = 0;
  free  = 0;

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XSYSTEM::GetFreeMemoryPercent()
* @brief      Get Free Memory Percent
* @ingroup    UTILS
*
* @return     int :
*
*---------------------------------------------------------------------------------------------------------------------*/
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
* @brief      FreeCacheMemory
* @ingroup    UTILS
*
* @return     bool : true if is succesful. 
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::FreeCacheMemory()
{
  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XSYSTEM::GetCPUUsageTotal()
* @brief      GetCPUUsageTotal
* @ingroup    UTILS
*
* @return     int : 
*
*---------------------------------------------------------------------------------------------------------------------*/
int XSYSTEM::GetCPUUsageTotal()
{
  return 0;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XSYSTEM::GetCPUUsageForProcessName(XCHAR* processname)
* @brief      GetCPUUsageForProcessName
* @ingroup    UTILS
*
* @param[in]  processname : 
*
* @return     int : 
*
*---------------------------------------------------------------------------------------------------------------------*/
int XSYSTEM::GetCPUUsageForProcessName(XCHAR* processname)
{
  return 0;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XSYSTEM::GetCPUUsageForProcessID(XDWORD processID)
* @brief      GetCPUUsageForProcessID
* @ingroup    UTILS
*
* @param[in]  processID : 
*
* @return     int : 
*
*---------------------------------------------------------------------------------------------------------------------*/
int XSYSTEM::GetCPUUsageForProcessID(XDWORD processID)
{
  return 0;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* XSYSTEM::GetEnviromentVariable(XCHAR* variablename)
* @brief      GetEnviromentVariable
* @ingroup    UTILS
*
* @param[in]  variablename :
*
* @return     XCHAR* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XCHAR* XSYSTEM::GetEnviromentVariable(XCHAR* variablename)
{
  return __L("");
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* XSYSTEM::GetEnviromentVariable(XSTRING& variablename)
* @brief      GetEnviromentVariable
* @ingroup    UTILS
*
* @param[in]  variablename :
*
* @return     XCHAR* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XCHAR* XSYSTEM::GetEnviromentVariable(XSTRING& variablename)
{
  return GetEnviromentVariable(variablename.Get());
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::SetEnviromentVariable(XCHAR* variablename, XCHAR* value)
* @brief      SetEnviromentVariable
* @ingroup    UTILS
*
* @param[in]  variablename :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::SetEnviromentVariable(XCHAR* variablename, XCHAR* value)
{
  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::SetEnviromentVariable(XSTRING& variablename, XSTRING& value)
* @brief      SetEnviromentVariable
* @ingroup    UTILS
*
* @param[in]  variablename :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::SetEnviromentVariable(XSTRING& variablename, XSTRING& value)
{
  return SetEnviromentVariable(variablename.Get(), value.Get());
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::DelEnviromentVariable(XCHAR* namevariable)
* @brief      DelEnviromentVariable
* @ingroup    UTILS
*
* @param[in]  namevariable :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::DelEnviromentVariable(XCHAR* namevariable)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::DelEnviromentVariable(XSTRING& namevariable);
* @brief      DelEnviromentVariable
* @ingroup    UTILS
*
* @param[in]  namevariable) :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::DelEnviromentVariable(XSTRING& namevariable)
{
  return DelEnviromentVariable(namevariable.Get());
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::GetUserAndDomain(XSTRING& user, XSTRING& domain)
* @brief      GetUserAndDomain
* @ingroup    UTILS
*
* @param[in]  user : 
* @param[in]  domain : 
*
* @return     bool : true if is succesful. 
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::GetUserAndDomain(XSTRING& user, XSTRING& domain)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::ShutDown(XSYSTEM_CHANGESTATUSTYPE type)
* @brief      ShutDown
* @ingroup    UTILS
*
* @param[in]  type : type of shutdown (enum XSYSTEM_CHANGESTATUSTYPE)
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::ShutDown(XSYSTEM_CHANGESTATUSTYPE type)
{
  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XSYSTEM::Sound_GetLevel()
* @brief      Sound_GetLevel
* @ingroup    UTILS
*
* @return     int : 
*
*---------------------------------------------------------------------------------------------------------------------*/
int XSYSTEM::Sound_GetLevel()
{
  return 0;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::Sound_SetLevel(int level)
* @brief      Sound_SetLevel
* @ingroup    UTILS
*
* @param[in]  level : 
*
* @return     bool : true if is succesful. 
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::Sound_SetLevel(int level)
{
  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSYSTEM::Sound_SetMutex(bool on)
* @brief      Sound_SetMutex
* @ingroup    UTILS
*
* @param[in]  on : 
*
* @return     bool : true if is succesful. 
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSYSTEM::Sound_SetMutex(bool on)
{
  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSYSTEM::GetBatteryLevel (bool& isincharge, XBYTE& levelpercent)
* @brief      GetBatteryLevel
* @ingroup    UTILS
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



#ifndef MICROCONTROLLER

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSYSTEM::HostFile(bool add, XCHAR* domain, XCHAR* IP)
* @brief      HostFile
* @ingroup    UTILS
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
* @brief      HostFile
* @ingroup    UTILS
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
* @brief      IsLineInHostFile
* @ingroup    UTILS
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
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    UTILS
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XSYSTEM::Clean()
{

}
















