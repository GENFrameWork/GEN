/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       Script_Lib_Function.cpp
* 
* @class      SCRIPT_LIB_FUNCTION
* @brief      Script Library Function class
* @ingroup    SCRIPT
* 
* @copyright  EndoraSoft. All rights reserved.
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

#include "GEN_Defines.h"



/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "Script_Lib_Function.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/




/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SCRIPT_LIB_FUNCTION::SCRIPT_LIB_FUNCTION(SCRIPT_LIB* library, XCHAR* name, SCRFUNCIONLIBRARY functionlibrary)
* @brief      Constructor of class
* @ingroup    SCRIPT
* 
* @param[in]  library : Library pointer to use.
* @param[in]  name : Name to use.
* @param[in]  functionlibrary : Functionlibrary value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_LIB_FUNCTION::SCRIPT_LIB_FUNCTION(SCRIPT_LIB* library, XCHAR* name, SCRFUNCIONLIBRARY functionlibrary)
{
  Clean();

  this->library         = library;
  this->name            = name;
  this->functionlibrary = functionlibrary;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SCRIPT_LIB_FUNCTION::SCRIPT_LIB_FUNCTION(SCRIPT_LIB* library, XSTRING& name, SCRFUNCIONLIBRARY functionlibrary)
* @brief      Constructor of class
* @ingroup    SCRIPT
* 
* @param[in]  library : Library pointer to use.
* @param[in]  name : Name to use.
* @param[in]  functionlibrary : Functionlibrary value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_LIB_FUNCTION::SCRIPT_LIB_FUNCTION(SCRIPT_LIB* library, XSTRING& name, SCRFUNCIONLIBRARY functionlibrary)
{
  Clean();

  this->library         = library;
  this->name            = name;
  this->functionlibrary = functionlibrary;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SCRIPT_LIB_FUNCTION::~SCRIPT_LIB_FUNCTION()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    SCRIPT
* 
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_LIB_FUNCTION::~SCRIPT_LIB_FUNCTION()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SCRIPT_LIB* SCRIPT_LIB_FUNCTION::GetLibrary()
* @brief      Get library
* @ingroup    SCRIPT
* 
* @return     SCRIPT_LIB* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_LIB* SCRIPT_LIB_FUNCTION::GetLibrary()
{
  return library;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_LIB_FUNCTION::SetLibrary(SCRIPT_LIB* library)
* @brief      Set library
* @ingroup    SCRIPT
* 
* @param[in]  library : Library pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_LIB_FUNCTION::SetLibrary(SCRIPT_LIB* library)
{
  if(!library) return false;
  this->library = library;
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* SCRIPT_LIB_FUNCTION::GetName()
* @brief      Get name
* @ingroup    SCRIPT
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* SCRIPT_LIB_FUNCTION::GetName()
{
  return &name;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SCRFUNCIONLIBRARY SCRIPT_LIB_FUNCTION::GetFunctionLibrary()
* @brief      Get function library
* @ingroup    SCRIPT
* 
* @return     SCRFUNCIONLIBRARY : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
SCRFUNCIONLIBRARY SCRIPT_LIB_FUNCTION::GetFunctionLibrary()
{
  return functionlibrary;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_LIB_FUNCTION::SetFunctionLibrary(SCRFUNCIONLIBRARY functionlibrary)
* @brief      Set function library
* @ingroup    SCRIPT
* 
* @param[in]  functionlibrary : Functionlibrary value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_LIB_FUNCTION::SetFunctionLibrary(SCRFUNCIONLIBRARY functionlibrary)
{
  this->functionlibrary = functionlibrary;
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void SCRIPT_LIB_FUNCTION::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    SCRIPT
* 
* --------------------------------------------------------------------------------------------------------------------*/
void SCRIPT_LIB_FUNCTION::Clean()
{
  name.Empty();

  functionlibrary = NULL;
}


