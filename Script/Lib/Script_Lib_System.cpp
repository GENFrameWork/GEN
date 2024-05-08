/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       Script_Lib_System.cpp
* 
* @class      SCRIPT_LIB_SYSTEM
* @brief      Script Library System
* @ingroup    SCRIPT
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


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "Script_Lib_System.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "XFactory.h"
#include "XSystem.h"

#include "Script.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE


#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         SCRIPT_LIB_SYSTEM::SCRIPT_LIB_SYSTEM()
* @brief      Constructor
* @ingroup    SCRIPT
*
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_LIB_SYSTEM::SCRIPT_LIB_SYSTEM() : SCRIPT_LIB(SCRIPT_LIB_SYSTEM_NAME)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         SCRIPT_LIB_SYSTEM::~SCRIPT_LIB_SYSTEM()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    SCRIPT
*
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_LIB_SYSTEM::~SCRIPT_LIB_SYSTEM()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool SCRIPT_LIB_SYSTEM::AddLibraryFunctions(SCRIPT* script)
* @brief      AddLibraryFunctions
* @ingroup    SCRIPT
*
* @param[in]  script :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_LIB_SYSTEM::AddLibraryFunctions(SCRIPT* script)
{
  if(!script) return false;

  this->script = script;

  script->AddLibraryFunction(this, __L("System_GetType")              , Call_System_GetType);
  script->AddLibraryFunction(this, __L("System_Reboot")               , Call_System_Reboot);
  script->AddLibraryFunction(this, __L("System_PowerOff")             , Call_System_PowerOff);
  script->AddLibraryFunction(this, __L("System_Logout")               , Call_System_Logout);
  script->AddLibraryFunction(this, __L("System_GetEnviromentVar")     , Call_System_GetEnviromentVar);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void SCRIPT_LIB_SYSTEM::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    SCRIPT
*
* --------------------------------------------------------------------------------------------------------------------*/
void SCRIPT_LIB_SYSTEM::Clean()
{

}


#pragma endregion


/*---- LIBRARY FUNCTIONS ---------------------------------------------------------------------------------------------*/
#pragma region LIBRARY_FUNCTIONS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void Call_System_GetType(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
* @brief      Call_System_GetType
* @ingroup    SCRIPT
* 
* @param[in]  library : 
* @param[in]  script : 
* @param[in]  params : 
* @param[in]  returnvalue : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void Call_System_GetType(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
{
  if(!library)      return;
  if(!script)       return;
  if(!params)       return;
  if(!returnvalue)  return;

  returnvalue->Set();

  switch(GEN_XSYSTEM.GetPlatform())
    {
      case XSYSTEM_PLATFORM_UNKNOWN         :
                      default               : (*returnvalue) = "";                    break;
      case XSYSTEM_PLATFORM_WINDOWS         : (*returnvalue) = "Windows";             break; 
      case XSYSTEM_PLATFORM_LINUX           : (*returnvalue) = "Linux";               break;
      case XSYSTEM_PLATFORM_LINUX_EMBEDDED  : (*returnvalue) = "LinuxEmbedded";       break;
      case XSYSTEM_PLATFORM_ANDROID         : (*returnvalue) = "Android";             break;
      case XSYSTEM_PLATFORM_STM32           : (*returnvalue) = "STM32";               break;
    };
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void Call_System_Reboot(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
* @brief      Call_System_Reboot
* @ingroup    SCRIPT
*
* @param[in]  library : 
* @param[in]  script : 
* @param[in]  params : 
* @param[in]  returnvalue : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void Call_System_Reboot(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
{
  if(!library)      return;
  if(!script)       return;
  if(!params)       return;
  if(!returnvalue)  return;

  returnvalue->Set();

  (*returnvalue) = GEN_XSYSTEM.ShutDown(XSYSTEM_CHANGESTATUSTYPE_REBOOT);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void Call_System_PowerOff(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
* @brief      Call_System_PowerOff
* @ingroup    SCRIPT
*
* @param[in]  library : 
* @param[in]  script : 
* @param[in]  params : 
* @param[in]  returnvalue : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void Call_System_PowerOff(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
{
  if(!library)      return;
  if(!script)       return;
  if(!params)       return;
  if(!returnvalue)  return;

  returnvalue->Set();
  
  (*returnvalue) = GEN_XSYSTEM.ShutDown(XSYSTEM_CHANGESTATUSTYPE_POWEROFF);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void Call_System_Logout(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
* @brief      Call_System_Logout
* @ingroup    SCRIPT
*
* @param[in]  library : 
* @param[in]  script : 
* @param[in]  params : 
* @param[in]  returnvalue : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void Call_System_Logout(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
{
  if(!library)      return;
  if(!script)       return;
  if(!params)       return;
  if(!returnvalue)  return;

  returnvalue->Set();
  
  (*returnvalue) = GEN_XSYSTEM.ShutDown(XSYSTEM_CHANGESTATUSTYPE_SESSION_LOGOFF);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void Call_System_GetEnviromentVar(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
* @brief      Call_System_GetEnviromentVar
* @ingroup    SCRIPT
* 
* @param[in]  library : 
* @param[in]  script : 
* @param[in]  params : 
* @param[in]  returnvalue : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void Call_System_GetEnviromentVar(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
{
  if(!library)      return;
  if(!script)       return;
  if(!params)       return;
  if(!returnvalue)  return;

  returnvalue->Set();

  if(!params->GetSize())
    {
      script->HaveError(SCRIPT_ERRORCODE_INSUF_PARAMS);
      return;
    }

  XSTRING* namevar = (XSTRING*)params->Get(0)->GetData();    
  if(!namevar)  return;

  XCHAR* variable = GEN_XSYSTEM.GetEnviromentVariable(namevar->Get());
  if(variable)
    {
      (*returnvalue) = variable;
    }   
}


#pragma endregion

