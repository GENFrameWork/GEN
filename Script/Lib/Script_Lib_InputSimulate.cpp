/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       Script_Lib_InputSimulate.cpp
* 
* @class      SCRIPT_LIB_INPUTSIMULATE
* @brief      Script Lib Input Simulate class
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

/*---- PRECOMPILATION INCLUDES ----------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#include "Script_Lib_InputSimulate.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XVariant.h"
#include "XProcessManager.h"

#include "APPBase.h"
#include "APPMain.h"

#include "INPFactory.h"
#include "INPSimulate.h"

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
* @fn        SCRIPT_LIB_INPUTSIMULATE::SCRIPT_LIB_INPUTSIMULATE()
* @brief      Constructor
* @ingroup    SCRIPT
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_LIB_INPUTSIMULATE::SCRIPT_LIB_INPUTSIMULATE() : SCRIPT_LIB(SCRIPT_LIB_NAME_INPSIMULATE)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn        SCRIPT_LIB_INPUTSIMULATE::~SCRIPT_LIB_INPUTSIMULATE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    SCRIPT
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_LIB_INPUTSIMULATE::~SCRIPT_LIB_INPUTSIMULATE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         boolSCRIPT_LIB_INPUTSIMULATE::AddLibraryFunctions(SCRIPT* script)
* @brief      AddLibraryFunctions
* @ingroup    SCRIPT
* 
* @param[in]  script : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_LIB_INPUTSIMULATE::AddLibraryFunctions(SCRIPT* script)
{
  if(!script) return false;

  this->script = script;

  script->AddLibraryFunction(this, __L("InpSim_PressKey")             , Call_PressKey);
  script->AddLibraryFunction(this, __L("InpSim_PressKeyByLiteral")    , Call_PressKeyByLiteral);
  script->AddLibraryFunction(this, __L("InpSim_PressKeyByText")       , Call_PressKeyByText);
  script->AddLibraryFunction(this, __L("InpSim_SetMousePos")          , Call_SetMousePos);
  script->AddLibraryFunction(this, __L("InpSim_SetMouseClick")        , Call_SetMouseClick);
      
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         voidSCRIPT_LIB_INPUTSIMULATE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    SCRIPT
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void SCRIPT_LIB_INPUTSIMULATE::Clean()
{

}


/*---- LIBRARY FUNCTIONS ---------------------------------------------------------------------------------------------*/
#pragma region LIBRARY_FUNCTIONS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void Call_OpenURL(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
* @brief      Call_OpenURL
* @ingroup    SCRIPT
* 
* @param[in]  library : 
* @param[in]  script : 
* @param[in]  params : 
* @param[in]  returnvalue : 
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void Call_PressKey(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
{
  if(!library)      return;
  if(!script)       return;
  if(!params)       return;
  if(!returnvalue)  return;

  returnvalue->Set();

  bool status  = false;

  if(!params->GetSize())
    {
      script->HaveError(SCRIPT_ERRORCODE_INSUF_PARAMS);
      return;
    }

  int key          = 0;
  int pressuretime = 0;

  library->GetParamConverted(params->Get(0), key);
  library->GetParamConverted(params->Get(1), pressuretime);
  
  INPSIMULATE* inpsimulate = GEN_INPFACTORY.CreateSimulator();
  if(!inpsimulate)
    {
      (*returnvalue) = status;
      return;
    }
  
  status = inpsimulate->PressKey((XBYTE)key,pressuretime);

  GEN_INPFACTORY.DeleteSimulator(inpsimulate);

  (*returnvalue) = status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void Call_PressKeyByLiteral(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
* @brief      Call_PressKeyByLiteral
* @ingroup    SCRIPT
* 
* @param[in]  library : 
* @param[in]  script : 
* @param[in]  params : 
* @param[in]  returnvalue : 
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void Call_PressKeyByLiteral(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
{
  if(!library)      return;
  if(!script)       return;
  if(!params)       return;
  if(!returnvalue)  return;

  returnvalue->Set();

  bool status  = false;

  if(!params->GetSize())
    {
      script->HaveError(SCRIPT_ERRORCODE_INSUF_PARAMS);
      return;
    }

  XVARIANT* variant;
  variant = params->Get(0);

  XSTRING literal      = (*variant);
  int     pressuretime = 0;

  library->GetParamConverted(params->Get(1), pressuretime);
    
  INPSIMULATE* inpsimulate = GEN_INPFACTORY.CreateSimulator();
  if(!inpsimulate)
    {
      (*returnvalue) = status;
      return;
    }
  
  status = inpsimulate->PressKeyByLiteral(literal.Get(), pressuretime);

  GEN_INPFACTORY.DeleteSimulator(inpsimulate);

  (*returnvalue) = status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void Call_PressKeyByText(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
* @brief      Call_PressKeyByText
* @ingroup    SCRIPT
* 
* @param[in]  library : 
* @param[in]  script : 
* @param[in]  params : 
* @param[in]  returnvalue : 
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void Call_PressKeyByText(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
{
  if(!library)      return;
  if(!script)       return;
  if(!params)       return;
  if(!returnvalue)  return;

  returnvalue->Set();

  bool status  = false;

  if(!params->GetSize())
    {
      script->HaveError(SCRIPT_ERRORCODE_INSUF_PARAMS);
      return;
    }

  XVARIANT* variant;
  variant = params->Get(0);

  XSTRING text         = (*variant);
  int     pressuretime = 0;

  library->GetParamConverted(params->Get(1), pressuretime);
    
  INPSIMULATE* inpsimulate = GEN_INPFACTORY.CreateSimulator();
  if(!inpsimulate)
    {
      (*returnvalue) = status;
      return;
    }
  
  status = inpsimulate->PressKeyByText(text.Get(), pressuretime);

  GEN_INPFACTORY.DeleteSimulator(inpsimulate);

  (*returnvalue) = status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void Call_SetMousePos(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
* @brief      Call_SetMousePos
* @ingroup    SCRIPT
* 
* @param[in]  library : 
* @param[in]  script : 
* @param[in]  params : 
* @param[in]  returnvalue : 
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void Call_SetMousePos(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
{
  if(!library)      return;
  if(!script)       return;
  if(!params)       return;
  if(!returnvalue)  return;

  returnvalue->Set();

  bool status  = false;

  if(params->GetSize()<2)
    {
      script->HaveError(SCRIPT_ERRORCODE_INSUF_PARAMS);
      return;
    }

  int x = 0;
  int y = 0;

  library->GetParamConverted(params->Get(0), x);
  library->GetParamConverted(params->Get(1), y);
  
  INPSIMULATE* inpsimulate = GEN_INPFACTORY.CreateSimulator();
  if(!inpsimulate)
    {
      (*returnvalue) = status;
      return;
    }
  
  status = inpsimulate->SetMousePos(x, y);

  GEN_INPFACTORY.DeleteSimulator(inpsimulate);
  
  (*returnvalue) = status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void Call_SetMouseClick(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
* @brief      Call_SetMouseClick
* @ingroup    SCRIPT
* 
* @param[in]  library : 
* @param[in]  script : 
* @param[in]  params : 
* @param[in]  returnvalue : 
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void Call_SetMouseClick(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
{
  if(!library)      return;
  if(!script)       return;
  if(!params)       return;
  if(!returnvalue)  return;

  returnvalue->Set();

  bool status  = false;

  if(params->GetSize()<2)
    {
      script->HaveError(SCRIPT_ERRORCODE_INSUF_PARAMS);
      return;
    }

  int x = 0;
  int y = 0;

  library->GetParamConverted(params->Get(0), x);
  library->GetParamConverted(params->Get(1), y);

  INPSIMULATE* inpsimulate = GEN_INPFACTORY.CreateSimulator();
  if(!inpsimulate)
    {
      (*returnvalue) = status;
      return;
    }
  
  status = inpsimulate->SetMouseClick(x, y);

  GEN_INPFACTORY.DeleteSimulator(inpsimulate);
  
  (*returnvalue) = status;
}


#pragma endregion

#pragma endregion

