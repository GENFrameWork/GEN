/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_Element_Option.cpp
* 
* @class      UI_ELEMENT_OPTION
* @brief      User Interface Element Option class
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @copyright  Copyright(c) 2005 - 2020 GEN Group.
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
* ---------------------------------------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "GRPBitmap.h"

#include "UI_Element_Option.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT_OPTION::UI_ELEMENT_OPTION()
* @brief      Constructor
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT_OPTION::UI_ELEMENT_OPTION()    
{ 
  Clean();   

  SetType(UI_ELEMENT_TYPE_OPTION);
  GetTypeString()->Set(__L("option"));   
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT_OPTION::~UI_ELEMENT_OPTION()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT_OPTION::~UI_ELEMENT_OPTION()    
{   
  Clean();                            
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE UI_ELEMENT_OPTION::GetAllocationTextType()
* @brief      GetAllocationTextType
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @return     UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE UI_ELEMENT_OPTION::GetAllocationTextType()
{
  return allocationtexttype;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT_OPTION::SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE allocationtexttype)
* @brief      SetAllocationTextType
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @param[in]  allocationtexttype : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT_OPTION::SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE allocationtexttype)
{
  this->allocationtexttype = allocationtexttype;
}




/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT_TEXT* UI_ELEMENT_OPTION::Get_UIText()
* @brief      Get_UIText
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @return     UI_ELEMENT_TEXT* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT_TEXT* UI_ELEMENT_OPTION::Get_UIText()
{ 
  return UItext;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT_OPTION::Set_UIText(UI_ELEMENT_TEXT* elementtext)
* @brief      Set_UIText
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @param[in]  elementtext : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT_OPTION::Set_UIText(UI_ELEMENT_TEXT* elementtext)
{
  this->UItext = elementtext;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT_ANIMATION* UI_ELEMENT_OPTION::Get_UIAnimation()
* @brief      Get_UIAnimation
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @return     UI_ELEMENT_ANIMATION* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT_ANIMATION* UI_ELEMENT_OPTION::Get_UIAnimation()
{
  return UIanimation;
}
  	


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT_OPTION::Set_UIAnimation(UI_ELEMENT_ANIMATION* elementanimation)
* @brief      Set_UIAnimation
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @param[in]  elementanimation : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT_OPTION::Set_UIAnimation(UI_ELEMENT_ANIMATION* elementanimation)
{
  this->UIanimation = elementanimation;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT_OPTION::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT_OPTION::Clean()
{  
  allocationtexttype  = UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_NONE;

  UItext              = NULL;
  UIanimation         = NULL; 
}