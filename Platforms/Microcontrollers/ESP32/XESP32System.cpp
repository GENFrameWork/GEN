/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XESP32System.cpp
* 
* @class      XESP32SYSTEM
* @brief      eXtended utils ESP32 system class
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XESP32System.h"

#include <stdio.h>

#include "XString.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XESP32SYSTEM::XESP32SYSTEM()
* @brief      Constructor of class
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
XESP32SYSTEM::XESP32SYSTEM() : XSYSTEM()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XESP32SYSTEM::~XESP32SYSTEM()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
XESP32SYSTEM::~XESP32SYSTEM()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSYSTEM_HARDWARETYPE XESP32SYSTEM::GetTypeHardware(int* revision)
* @brief      Get type hardware
* @ingroup    PLATFORM_ESP32
*
* @param[in]  revision :
*
* @return     XSYSTEM_HARDWARETYPE :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSYSTEM_HARDWARETYPE XESP32SYSTEM::GetTypeHardware(int* revision)
{
  return XSYSTEM_HARDWARETYPE_MICRO_ESP32;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSYSTEM_PLATFORM XESP32SYSTEM::GetPlatform(XSTRING* namestring)
* @brief      Get platform
* @ingroup    PLATFORM_ESP32
*
* @param[in]  namestring : 
* 
* @return     XSYSTEM_PLATFORM : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSYSTEM_PLATFORM XESP32SYSTEM::GetPlatform(XSTRING* namestring)
{
  if(namestring)  namestring->Set(__L("ESP32"));
    
  /*
  #if defined(ESP32F072xB)
  if(namestring)  namestring->Set(__L("ESP32F072xB"));
  #endif
  */

  return XSYSTEM_PLATFORM_ESP32;  
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XESP32SYSTEM::GetMemoryInfo(XDWORD& total,XDWORD& free)
* @brief      Get memory info
* @ingroup    PLATFORM_ESP32
*
* @param[in]  total :
* @param[in]  free :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XESP32SYSTEM::GetMemoryInfo(XDWORD& total,XDWORD& free)
{
  total = 0;
  free  = 0;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XESP32SYSTEM::ShutDown(XSYSTEM_CHANGESTATUSTYPE type)
* @brief      Shut down
* @ingroup    PLATFORM_ESP32
*
* @param[in]  type :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XESP32SYSTEM::ShutDown(XSYSTEM_CHANGESTATUSTYPE type)
{
  switch(type)
    {
      case XSYSTEM_CHANGESTATUSTYPE_REBOOT            : break;
      case XSYSTEM_CHANGESTATUSTYPE_SESSION_LOGOFF    : break;
      case XSYSTEM_CHANGESTATUSTYPE_POWEROFF          : break;
                                              default : break;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XESP32SYSTEM::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
void XESP32SYSTEM::Clean()
{

}


#pragma endregion

