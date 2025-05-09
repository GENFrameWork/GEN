/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       APPFlow_XEvent.cpp
* 
* @class      APPFLOW_XEVENT
* @brief      Application Flow eXtended Event class
* @ingroup    APPFLOW
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

#include "APPFlow_XEvent.h"


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
* @fn         APPFLOW_XEVENT::APPFLOW_XEVENT(XSUBJECT* subject, XDWORD type, XDWORD family) : XEVENT(subject, type, family)
* @brief      Constructor of class
* @ingroup    APPFLOW
* 
* @param[in]  subject : 
* @param[in]  type : 
* @param[in]  family : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
APPFLOW_XEVENT::APPFLOW_XEVENT(XSUBJECT* subject, XDWORD type, XDWORD family) : XEVENT(subject, type, family)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         APPFLOW_XEVENT::~APPFLOW_XEVENT()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    APPFLOW
* 
* --------------------------------------------------------------------------------------------------------------------*/
APPFLOW_XEVENT::~APPFLOW_XEVENT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSYSTEM_CHANGESTATUSTYPE APPFLOW_XEVENT::GetChangeStatusType()
* @brief      Get change status type
* @ingroup    APPFLOW
* 
* @return     XSYSTEM_CHANGESTATUSTYPE : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSYSTEM_CHANGESTATUSTYPE APPFLOW_XEVENT::GetChangeStatusType()
{
  return changestatustype;
}
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void APPFLOW_XEVENT::SetChangeStatusType(XSYSTEM_CHANGESTATUSTYPE changestatustype)
* @brief      Set change status type
* @ingroup    APPFLOW
* 
* @param[in]  changestatustype : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void APPFLOW_XEVENT::SetChangeStatusType(XSYSTEM_CHANGESTATUSTYPE changestatustype)
{
  this->changestatustype = changestatustype;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void APPFLOW_XEVENT::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    APPFLOW
* 
* --------------------------------------------------------------------------------------------------------------------*/
void APPFLOW_XEVENT::Clean()
{
  changestatustype = XSYSTEM_CHANGESTATUSTYPE_NONE;
}


#pragma endregion
