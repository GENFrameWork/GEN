/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       Script_XEvent.cpp
* 
* @class      SCRIPT_XEVENT
* @brief      Script XEvent
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

#include "Script_XEvent.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE


#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         SCRIPT_XEVENT::SCRIPT_XEVENT(XSUBJECT* subject, XDWORD type, XDWORD family)
* @brief      Constructor
* @ingroup    SCRIPT
*
* @param[in]  subject :
* @param[in]  type :
* @param[in]  family :
*
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_XEVENT::SCRIPT_XEVENT(XSUBJECT* subject, XDWORD type, XDWORD family): XEVENT(subject, type, family)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         SCRIPT_XEVENT::~SCRIPT_XEVENT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    SCRIPT
*
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_XEVENT::~SCRIPT_XEVENT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SCRIPT* SCRIPT_XEVENT::GetScript()
* @brief      GetScript
* @ingroup    SCRIPT
* 
* @return     SCRIPT* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT* SCRIPT_XEVENT::GetScript()
{
  return script;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void SCRIPT_XEVENT::SetScript(SCRIPT* script)
* @brief      SetScript
* @ingroup    SCRIPT
* 
* @param[in]  script : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void SCRIPT_XEVENT::SetScript(SCRIPT* script)
{
  this->script = script;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* SCRIPT_XEVENT::GetNameScript()
* @brief      GetNameScript
* @ingroup    SCRIPT
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* SCRIPT_XEVENT::GetNameScript()
{
  return &namescript;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         SCRIPT_G_ERRORCODE SCRIPT_XEVENT::GetError()
* @brief      GetError
* @ingroup    SCRIPT
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int SCRIPT_XEVENT::GetError()
{
  return error;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void SCRIPT_XEVENT::SetError(SCRIPT_G_ERRORCODE error)
* @brief      SetError
* @ingroup    SCRIPT
*
* @param[in]  error :
*
* --------------------------------------------------------------------------------------------------------------------*/
void SCRIPT_XEVENT::SetError(int error)
{
  this->error = error;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* SCRIPT_XEVENT::GetErrorText()
* @brief      GetErrorText
* @ingroup    SCRIPT
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* SCRIPT_XEVENT::GetErrorText()
{
  return &errortext;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int SCRIPT_XEVENT::GetNLine()
* @brief      GetNLine
* @ingroup    SCRIPT
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int SCRIPT_XEVENT::GetNLine()
{
  return nline;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void SCRIPT_XEVENT::SetNLine(int nline)
* @brief      SetNLine
* @ingroup    SCRIPT
*
* @param[in]  nline :
*
* --------------------------------------------------------------------------------------------------------------------*/
void SCRIPT_XEVENT::SetNLine(int nline)
{
  this->nline = nline;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* SCRIPT_XEVENT::GetCurrentToken()
* @brief      GetCurrentToken
* @ingroup    SCRIPT
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* SCRIPT_XEVENT::GetCurrentToken()
{
  return &currenttoken;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void SCRIPT_XEVENT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    SCRIPT
*
* --------------------------------------------------------------------------------------------------------------------*/
void SCRIPT_XEVENT::Clean()
{
  script    = NULL;
  error     = 0;
  nline     = 0;
}


#pragma endregion

