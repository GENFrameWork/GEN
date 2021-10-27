/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_Property_Editable.cpp
* 
* @class      UI_PROPERTY_EDITABLE
* @brief      User Interface Property Editable class
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

#include "UI_Property_Editable.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_PROPERTY_EDITABLE::UI_PROPERTY_EDITABLE()
* @brief      Constructor
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
UI_PROPERTY_EDITABLE::UI_PROPERTY_EDITABLE()    
{ 
  Clean();   
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_PROPERTY_EDITABLE::~UI_PROPERTY_EDITABLE()
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
UI_PROPERTY_EDITABLE::~UI_PROPERTY_EDITABLE()    
{   
  Clean();                            
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD UI_PROPERTY_EDITABLE::GetCursorPosition()
* @brief      GetCursorPosition
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @return     XDWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XDWORD UI_PROPERTY_EDITABLE::Cursor_GetPosition()
{
  return cursor_position;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_PROPERTY_EDITABLE::SetCursorPosition(XDWORD cursorposition)
* @brief      SetCursorPosition
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @param[in]  cursorposition : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_PROPERTY_EDITABLE::Cursor_SetPosition(XDWORD cursor_position)
{
  this->cursor_position = cursor_position;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD UI_PROPERTY_EDITABLE::GetMaxSize()
* @brief      GetMaxSize
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @return     XDWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XDWORD UI_PROPERTY_EDITABLE::GetMaxSize()
{
  return maxsize;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_PROPERTY_EDITABLE::SetMaxSizeText(XDWORD maxsize)
* @brief      SetMaxSize
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @param[in]  maxsizetext : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_PROPERTY_EDITABLE::SetMaxSize(XDWORD maxsize)
{
  this->maxsize = maxsize;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_PROPERTY_EDITABLE::Clean()
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
void UI_PROPERTY_EDITABLE::Clean()
{
  cursor_position   = 0;
  maxsize           = 0;
}