/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_Element.cpp
* 
* @class      UI_ELEMENT
* @brief      User Interface Element class
* @ingroup    USERINTERFACE
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

#include "UI_Element.h"

#include "XFactory.h"
#include "XTimer.h"

#include "UI_Color.h"
#include "UI_Element_Scroll.h"
#include "UI_Manager.h"

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
* @fn         UI_ELEMENT::UI_ELEMENT()
* @brief      Constructor
* @ingroup    USERINTERFACE
*
* ---------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT::UI_ELEMENT()    
{ 
  Clean();     

  isvisible      = true;  
  isactive       = true;

  GEN_XFACTORY_CREATE(blink_xtimer, CreateTimer())
  blink_state    = true; 
  
  z_level        = 1; 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT::~UI_ELEMENT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    USERINTERFACE
*
* ---------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT::~UI_ELEMENT()    
{ 
  if(blink_xtimer)
    {
      GEN_XFACTORY.DeleteTimer(blink_xtimer);
      blink_xtimer = NULL;
    }

  DeleteAllComposeElements();

  Clean();                            
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* UI_ELEMENT::GetName()
* @brief      GetName
* @ingroup    USERINTERFACE
*
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* UI_ELEMENT::GetName()
{
  return &name;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT_TYPE UI_ELEMENT::GetType()
* @brief      GetType
* @ingroup    USERINTERFACE
*
* @return     UI_ELEMENT_TYPE : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT_TYPE UI_ELEMENT::GetType()
{
  return type;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetType(UI_ELEMENT_TYPE type)
* @brief      SetType
* @ingroup    USERINTERFACE
*
* @param[in]  type : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetType(UI_ELEMENT_TYPE type)
{
  this->type = type;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* UI_ELEMENT::GetTypeString()
* @brief      GetTypeString
* @ingroup    USERINTERFACE
*
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* UI_ELEMENT::GetTypeString()
{
  return &type_string;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_ELEMENT::GetFather()
* @brief      GetFather
* @ingroup    USERINTERFACE
*
* @return     UI_ELEMENT* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_ELEMENT::GetFather()
{
  return father;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetFather(UI_ELEMENT* father)
* @brief      SetFather
* @ingroup    USERINTERFACE
*
* @param[in]  father : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetFather(UI_ELEMENT* father)
{
  this->father = father;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::IsDetached()
* @brief      IsDetached
* @ingroup    USERINTERFACE
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::IsDetached()
{
  return isdetached;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetIsDetached(bool hasfather)
* @brief      SetIsDetached
* @ingroup    USERINTERFACE
*
* @param[in]  hasfather : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetIsDetached(bool isdetached)
{
  this->isdetached = isdetached;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_COLOR* UI_ELEMENT::GetColor()
* @brief      GetColor
* @ingroup    USERINTERFACE
*
* @return     UI_COLOR* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
UI_COLOR* UI_ELEMENT::GetColor()
{
  return &color;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_COLOR* UI_ELEMENT::GetBackgroundColor()
* @brief      GetBackgroundColor
* @ingroup    USERINTERFACE
*
* @return     UI_COLOR* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
UI_COLOR* UI_ELEMENT::GetBackgroundColor()
{
  return &backgroundcolor;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT::GetXPosition()
* @brief      GetXPosition
* @ingroup    USERINTERFACE
*
* @return     double : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetXPosition()
{
  return x_position;
}
	

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SetXPosition(double x_position)
* @brief      SetXPosition
* @ingroup    USERINTERFACE
*
* @param[in]  x_position : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::SetXPosition(double x_position)   
{
  this->x_position = x_position;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT::GetYPosition()
* @brief      GetYPosition
* @ingroup    USERINTERFACE
*
* @return     double : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetYPosition()
{
  return y_position;  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SetYPosition(double y_position)
* @brief      SetYPosition
* @ingroup    USERINTERFACE
*
* @param[in]  y_position : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::SetYPosition(double y_position)
{
  this->y_position = y_position;

  return true;
}		


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD UI_ELEMENT::GetZLevel()
* @brief      GetZLevel
* @ingroup    USERINTERFACE
*
* @return     XDWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XDWORD UI_ELEMENT::GetZLevel()
{
  return z_level;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SetZLevel(XDWORD z_level)
* @brief      SetZLevel
* @ingroup    USERINTERFACE
*
* @param[in]  z_level : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::SetZLevel(XDWORD z_level)
{
  this->z_level = z_level;

  return true;
}		


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT_TYPE_DIRECTION UI_ELEMENT::GetDirection()
* @brief      GetDirection
* @ingroup    USERINTERFACE
*
* @return     UI_ELEMENT_TYPE_DIRECTION : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT_TYPE_DIRECTION UI_ELEMENT::GetDirection()
{
  return direction;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SetDirection(UI_ELEMENT_TYPE_DIRECTION direction)
* @brief      SetDirection
* @ingroup    USERINTERFACE
*
* @param[in]  direction : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::SetDirection(UI_ELEMENT_TYPE_DIRECTION direction)
{
  this->direction = direction;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT::GetXPositionWithScroll()
* @brief      GetXPositionWithScroll
* @ingroup    USERINTERFACE
*
* @return     double : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetXPositionWithScroll()
{
  return x_positionwithscroll;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SetXPositionWithScroll(double x_positionwithscroll)
* @brief      SetXPositionWithScroll
* @ingroup    USERINTERFACE
*
* @param[in]  x_positionwithscroll : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::SetXPositionWithScroll(double x_positionwithscroll)
{
  this->x_positionwithscroll = x_positionwithscroll;

  return true;
}		


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT::GetYPositionWithScroll()
* @brief      GetYPositionWithScroll
* @ingroup    USERINTERFACE
*
* @return     double : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetYPositionWithScroll()
{
  return y_positionwithscroll;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SetYPositionWithScroll(double y_positionwithscroll)
* @brief      SetYPositionWithScroll
* @ingroup    USERINTERFACE
*
* @param[in]  y_positionwithscroll : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::SetYPositionWithScroll(double y_positionwithscroll)
{
  this->y_positionwithscroll = y_positionwithscroll;
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_BOUNDARYLINE* UI_ELEMENT::GetBoundaryLine()
* @brief      GetBoundaryLine
* @ingroup    USERINTERFACE
*
* @return     UI_BOUNDARYLINE* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
UI_BOUNDARYLINE* UI_ELEMENT::GetBoundaryLine()
{
  return &boundaryline;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetBoundaryLine(XRECT& boundaryline)
* @brief      SetBoundaryLine
* @ingroup    USERINTERFACE
*
* @param[in]  boundaryline : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetBoundaryLine(UI_BOUNDARYLINE& boundaryline)
{
  this->boundaryline.CopyFrom(boundaryline);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT::GetMargin(UI_ELEMENT_TYPE_ALIGN position)
* @brief      GetMargin
* @ingroup    USERINTERFACE
*
* @param[in]  position : 
* 
* @return     double : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetMargin(UI_ELEMENT_TYPE_ALIGN position)
{
  switch(position)
    {
      case UI_ELEMENT_TYPE_ALIGN_LEFT   : return margin[0]; break;
      case UI_ELEMENT_TYPE_ALIGN_RIGHT	: return margin[1]; break;
      case UI_ELEMENT_TYPE_ALIGN_UP		  : return margin[2]; break;
      case UI_ELEMENT_TYPE_ALIGN_DOWN		: return margin[3]; break;
    }
  
  return 0.0f;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetMargin(UI_ELEMENT_TYPE_ALIGN position, double value)
* @brief      SetMargin
* @ingroup    USERINTERFACE
*
* @param[in]  position : 
* @param[in]  value : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetMargin(UI_ELEMENT_TYPE_ALIGN position, double value)
{
  switch(position)
    {
      case UI_ELEMENT_TYPE_ALIGN_LEFT   : margin[0] = value; break;
      case UI_ELEMENT_TYPE_ALIGN_RIGHT	: margin[1] = value; break;
      case UI_ELEMENT_TYPE_ALIGN_UP		  : margin[2] = value; break;
      case UI_ELEMENT_TYPE_ALIGN_DOWN		: margin[3] = value; break;
    } 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::IsActive()
* @brief      IsActive
* @ingroup    USERINTERFACE
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::IsActive()
{
  return isactive;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetActive(bool isactive)
* @brief      SetActive
* @ingroup    USERINTERFACE
*
* @param[in]  isactive : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetActive(bool isactive)
{
  this->isactive = isactive;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::IsPreSelect()
* @brief      IsPreSelect
* @ingroup    USERINTERFACE
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::IsPreSelect()
{
  return ispreselect;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetPreSelect(bool ispreselect)
* @brief      SetPreSelect
* @ingroup    USERINTERFACE
*
* @param[in]  ispreselect : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetPreSelect(bool ispreselect)
{
  this->ispreselect = ispreselect;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::IsSelected()
* @brief      IsSelected
* @ingroup    USERINTERFACE
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::IsSelected()
{  
  return isselected;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetSelected(bool isselected)
* @brief      SetSelected
* @ingroup    USERINTERFACE
*
* @param[in]  isselected : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetSelected(bool isselected)
{
  this->isselected = isselected;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::IsVisible()
* @brief      IsVisible
* @ingroup    USERINTERFACE
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::IsVisible()
{
  return isvisible;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetVisible(bool isvisible)
* @brief      SetVisible
* @ingroup    USERINTERFACE
*
* @param[in]  isvisible : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetVisible(bool isvisible)
{
  if(this->isvisible != isvisible) 
    {      
      GEN_USERINTERFACE.Elements_SetToRedraw(this);
       
      SetTransitionStateShow(isvisible?UI_ELEMENT_TRANSITION_STATE_SHOW_TOSHOW:UI_ELEMENT_TRANSITION_STATE_SHOW_TOHIDE);
      this->isvisible = isvisible;      
    } 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD UI_ELEMENT::GetRoundRect()
* @brief      GetRoundRect
* @ingroup    USERINTERFACE
*
* @return     XDWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XDWORD UI_ELEMENT::GetRoundRect()
{
  return roundrect;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetRoundRect(XDWORD roundrect)
* @brief      SetRoundRect
* @ingroup    USERINTERFACE
*
* @param[in]  roundrect : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetRoundRect(XDWORD roundrect)
{
  this->roundrect = roundrect;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD UI_ELEMENT::IsBlinking()
* @brief      IsBlinking
* @ingroup    USERINTERFACE
*
* @return     XDWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XDWORD UI_ELEMENT::IsBlinking()
{
  return blink_time;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SetBlink(XDWORD blink_time)
* @brief      SetBlink
* @ingroup    USERINTERFACE
*
* @param[in]  blink_time : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::SetBlink(XDWORD blink_time)
{
  if(!blink_xtimer) return false;

  this->blink_time = blink_time; 

  blink_state = true;
  
  blink_xtimer->Reset();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::GetStateBlink()
* @brief      GetStateBlink
* @ingroup    USERINTERFACE
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::GetStateBlink()
{
  return blink_state;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SwicthStateBlink()
* @brief      SwicthStateBlink
* @ingroup    USERINTERFACE
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::SwicthStateBlink()
{
  if(!blink_nchanges)
    {
      blink_state = true;
      blink_nchanges++;

      return true;
    }

  blink_state = !blink_state;  

  blink_nchanges++;

  blink_xtimer->Reset();

  return true;
}

	
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XTIMER* UI_ELEMENT::GetTimerBlink()
* @brief      GetTimerBlink
* @ingroup    USERINTERFACE
*
* @return     XTIMER* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XTIMER* UI_ELEMENT::GetTimerBlink()
{
  return blink_xtimer;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT_TRANSITION_STATE_SHOW UI_ELEMENT::GetTransitionStateShow()
* @brief      GetTransitionStateShow
* @ingroup    USERINTERFACE
*
* @return     UI_ELEMENT_TRANSITION_STATE_SHOW : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT_TRANSITION_STATE_SHOW UI_ELEMENT::GetTransitionStateShow()
{
  return transitionstateshow;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetTransitionStateShow(UI_ELEMENT_TRANSITION_STATE_SHOW transitionstateshow)
* @brief      SetTransitionStateShow
* @ingroup    USERINTERFACE
*
* @param[in]  transitionstateshow : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetTransitionStateShow(UI_ELEMENT_TRANSITION_STATE_SHOW transitionstateshow)
{
  this->transitionstateshow = transitionstateshow;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::HasScroll()
* @brief      HasScroll
* @ingroup    USERINTERFACE
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::HasScroll()
{
  return hasscroll;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetHasScroll(bool hasscroll)
* @brief      SetHasScroll
* @ingroup    USERINTERFACE
*
* @param[in]  hasscroll : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetHasScroll(bool hasscroll)
{
  this->hasscroll = hasscroll;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* UI_ELEMENT::GetExtra()
* @brief      GetExtra
* @ingroup    USERINTERFACE
*
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* UI_ELEMENT::GetExtra()
{
  return &extra;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::MustReDraw()
* @brief      MustReDraw
* @ingroup    USERINTERFACE
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::MustReDraw()
{
  return mustredraw;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetMustReDraw(bool mustredraw)
* @brief      SetMustReDraw
* @ingroup    USERINTERFACE
*
* @param[in]  mustredraw : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetMustReDraw(bool mustredraw)
{
  this->mustredraw = mustredraw;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<UI_ELEMENT*>* UI_ELEMENT::GetComposeElements()
* @brief      GetComposeElements
* @ingroup    USERINTERFACE
*
* @return     XVECTOR<UI_ELEMENT*>* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XVECTOR<UI_ELEMENT*>* UI_ELEMENT::GetComposeElements()
{
  return &compose_elements;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::DeleteAllComposeElements()
* @brief      DeleteAllComposeElements
* @ingroup    USERINTERFACE
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::DeleteAllComposeElements()
{
  if(compose_elements.IsEmpty()) return false;

  compose_elements.DeleteContents();
  compose_elements.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::Clean()
{
  type                    = UI_ELEMENT_TYPE_UNKNOWN;

  father                  = NULL;
  isdetached              = false;

  x_position              = 0.0f;
	y_position              = 0.0f;

  z_level                 = 0;

  direction               = UI_ELEMENT_TYPE_DIRECTION_UNKWOWN;

	x_positionwithscroll    = 0.0f;
  y_positionwithscroll    = 0.0f;
	
  for(XDWORD c=0; c<UI_ELEMENT_MARGIN_MAX; c++)
    {
	    margin[c] = 0.0f;		
    }

  isactive                = false;
  ispreselect             = false;
  isselected              = false;
  isvisible               = false;

  roundrect               = 0;

  blink_time              = 0;
  blink_state             = false;
	blink_xtimer            = NULL;	
  blink_nchanges          = 0;	
  
  transitionstateshow     = UI_ELEMENT_TRANSITION_STATE_SHOW_NONE;

  hasscroll               = false;

  mustredraw              = false;
}


#pragma endregion

