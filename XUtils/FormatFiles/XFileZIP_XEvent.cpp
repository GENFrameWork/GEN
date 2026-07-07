/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XFileZIP_XEvent.cpp
* 
* @class      XFILEZIP_XEVENT
* @brief      eXtended Utils ZIP File eXtended Event class
* @ingroup    XUTILS
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

#include "XFileZIP_XEvent.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEZIP_XEVENT::XFILEZIP_XEVENT(XSUBJECT* subject, XDWORD type, XDWORD family) : XEVENT(subject, type, family)
* @brief      Constructor of class
* @ingroup    XUTILS
* 
* @param[in]  subject : Subject pointer to use.
* @param[in]  type : Type value.
* @param[in]  family : Family value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XFILEZIP_XEVENT::XFILEZIP_XEVENT(XSUBJECT* subject, XDWORD type, XDWORD family) : XEVENT(subject, type, family)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEZIP_XEVENT::~XFILEZIP_XEVENT()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    XUTILS
* 
* --------------------------------------------------------------------------------------------------------------------*/
XFILEZIP_XEVENT::~XFILEZIP_XEVENT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XPATH* XFILEZIP_XEVENT::GetFileName()
* @brief      Get file name
* @ingroup    XUTILS
* 
* @return     XPATH* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XPATH* XFILEZIP_XEVENT::GetFileName()
{
  return &filename;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILECMPZIP* XFILEZIP_XEVENT::GetFileCompress()
* @brief      Get file compress
* @ingroup    XUTILS
* 
* @return     XFILECMPZIP* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XFILECMPZIP* XFILEZIP_XEVENT::GetFileCompress()
{
  return filecmp;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XFILEZIP_XEVENT::SetFileCompress(XFILECMPZIP* filecmp)
* @brief      Set file compress
* @ingroup    XUTILS
* 
* @param[in]  filecmp : Filecmp pointer to use.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XFILEZIP_XEVENT::SetFileCompress(XFILECMPZIP* filecmp)
{
  this->filecmp = filecmp;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         float XFILEZIP_XEVENT::GetOperationPercent()
* @brief      Get operation percent
* @ingroup    XUTILS
* 
* @return     float : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
float XFILEZIP_XEVENT::GetOperationPercent()
{
  return operationpercent;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XFILEZIP_XEVENT::SetOperationPercent(float operationpercent)
* @brief      Set operation percent
* @ingroup    XUTILS
* 
* @param[in]  operationpercent : Operationpercent value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XFILEZIP_XEVENT::SetOperationPercent(float operationpercent)
{
  this->operationpercent = operationpercent;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEZIP_ERROR XFILEZIP_XEVENT::GetError()
* @brief      Get error
* @ingroup    XUTILS
* 
* @return     XFILEZIP_ERROR : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XFILEZIP_ERROR XFILEZIP_XEVENT::GetError()
{
  return error;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XFILEZIP_XEVENT::SetError(XFILEZIP_ERROR error)
* @brief      Set error
* @ingroup    XUTILS
* 
* @param[in]  error : Error value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XFILEZIP_XEVENT::SetError(XFILEZIP_ERROR error)
{
  this->error = error;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XFILEZIP_XEVENT::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    XUTILS
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XFILEZIP_XEVENT::Clean()
{
  filecmp            = NULL;
  operationpercent   = 0.0f;
  error              = XFILEZIP_ERROR_NONE;
}



