/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_XEvent.cpp
* 
* @class      UI_XEVENT
* @brief      User Interface eXtended Event class
* @ingroup    USERINTERFACE
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

#include "UI_XEvent.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_XEVENT::UI_XEVENT(XSUBJECT* subject, XDWORD type, XDWORD family) : XEVENT(subject, type, family)
* @brief      Constructor of class
* @ingroup    USERINTERFACE
* 
* @param[in]  subject : Subject pointer to use.
* @param[in]  type : Type value.
* @param[in]  family : Family value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_XEVENT::UI_XEVENT(XSUBJECT* subject, XDWORD type, XDWORD family) : XEVENT(subject, type, family)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_XEVENT::~UI_XEVENT()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    USERINTERFACE
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_XEVENT::~UI_XEVENT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD UI_XEVENT::GetXPos()
* @brief      Get X pos
* @ingroup    USERINTERFACE
* 
* @return     XDWORD : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD UI_XEVENT::GetXPos()
{
  return xpos;
}
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_XEVENT::SetXPos(XDWORD xpos)
* @brief      Set X pos
* @ingroup    USERINTERFACE
* 
* @param[in]  xpos : Xpos value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_XEVENT::SetXPos(XDWORD xpos)
{
  this->xpos = xpos;
}

  
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD UI_XEVENT::GetYPos()
* @brief      Get Y pos
* @ingroup    USERINTERFACE
* 
* @return     XDWORD : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD UI_XEVENT::GetYPos()
{
  return ypos;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_XEVENT::SetYPos(XDWORD ypos)
* @brief      Set Y pos
* @ingroup    USERINTERFACE
* 
* @param[in]  ypos : Ypos value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_XEVENT::SetYPos(XDWORD ypos)
{
  this->ypos = ypos;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_XEVENT::GetElement()
* @brief      Get element
* @ingroup    USERINTERFACE
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_XEVENT::GetElement()
{
  return element;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         INPCURSORMOTION* UI_XEVENT::GetCursorMotion()
* @brief      Get cursor motion
* @ingroup    USERINTERFACE
* 
* @return     INPCURSORMOTION* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
INPCURSORMOTION* UI_XEVENT::GetCursorMotion()
{
  return cursormotion;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_XEVENT::SetCursorMotion(INPCURSORMOTION* cursormotion)
* @brief      Set cursor motion
* @ingroup    USERINTERFACE
* 
* @param[in]  cursormotion : Cursormotion pointer to use.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_XEVENT::SetCursorMotion(INPCURSORMOTION* cursormotion)
{
  this->cursormotion = cursormotion;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_XEVENT::SetElement(UI_ELEMENT* element)
* @brief      Set element
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_XEVENT::SetElement(UI_ELEMENT* element)
{
  if(!element) return false;

  this->element = element;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_SKIN* UI_XEVENT::GetSkin()
* @brief      Get skin
* @ingroup    USERINTERFACE
* 
* @return     UI_SKIN* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_SKIN* UI_XEVENT::GetSkin()
{
  return skin;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_XEVENT::SetSkin(UI_SKIN* skin)
* @brief      Set skin
* @ingroup    USERINTERFACE
* 
* @param[in]  skin : Skin pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_XEVENT::SetSkin(UI_SKIN* skin)
{
  this->skin = skin;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* UI_XEVENT::GetMaskTextValue()
* @brief      Get mask text value
* @ingroup    USERINTERFACE
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* UI_XEVENT::GetMaskTextValue()
{
  return &masktextvalue;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* UI_XEVENT::GetMaskTextResolved()
* @brief      Get mask text resolved
* @ingroup    USERINTERFACE
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* UI_XEVENT::GetMaskTextResolved()
{
  return &masktextresolved;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_XEVENT::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    USERINTERFACE
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_XEVENT::Clean()
{
  xpos          = 0;
  ypos          = 0; 

  cursormotion  = NULL;

  element       = NULL; 
  skin          = NULL;
}




