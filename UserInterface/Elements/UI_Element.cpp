/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_Element.cpp
* 
* @class      UI_ELEMENT
* @brief      User Interface Element class
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

#include "UI_Element.h"

#include "XFactory.h"
#include "XTimer.h"

#include "UI_Color.h"
#include "UI_Element_Scroll.h"
#include "UI_Manager.h"
#include "UI_Style.h"
#include "UI_StyleSheet.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT::UI_ELEMENT()
* @brief      Constructor of class
* @ingroup    USERINTERFACE
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT::UI_ELEMENT()    
{ 
  Clean();     

  isvisible      = true;  
  isactive       = true;

  blink_xtimer=GEN_XFACTORY.CreateTimer();
  blink_state    = true; 
  
  z_level        = 1; 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT::~UI_ELEMENT()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    USERINTERFACE
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT::~UI_ELEMENT()
{
  if(blink_xtimer)
    {
      GEN_XFACTORY.DeleteTimer(blink_xtimer);
      blink_xtimer = NULL;
    }

  // Owned per-class split view. SetClassNames() also clears this vector, but the destructor may be reached
  // without any Set call having ever run, so we still walk it defensively.
  for(XDWORD c=0; c<class_names.GetSize(); c++)
    {
      XSTRING* name = class_names.Get(c);
      if(name) GEN_DELETE name;
    }
  class_names.DeleteAll();

  DeleteAllComposeElements();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* UI_ELEMENT::GetName()
* @brief      Get name
* @ingroup    USERINTERFACE
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* UI_ELEMENT::GetName()
{
  return &name;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT_TYPE UI_ELEMENT::GetType()
* @brief      Get type
* @ingroup    USERINTERFACE
* 
* @return     UI_ELEMENT_TYPE : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT_TYPE UI_ELEMENT::GetType()
{
  return type;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetType(UI_ELEMENT_TYPE type)
* @brief      Set type
* @ingroup    USERINTERFACE
* 
* @param[in]  type : Type value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetType(UI_ELEMENT_TYPE type)
{
  this->type = type;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* UI_ELEMENT::GetTypeString()
* @brief      Get type string
* @ingroup    USERINTERFACE
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* UI_ELEMENT::GetTypeString()
{
  return &type_string;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* UI_ELEMENT::GetClassNamesRaw()
* @brief      Get raw "class" attribute string as authored ("chip primary big"). Empty when no class was set.
* @ingroup    USERINTERFACE
*
* @return     XSTRING* : Pointer to the raw class attribute string.
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* UI_ELEMENT::GetClassNamesRaw()
{
  return &class_names_raw;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<XSTRING*>* UI_ELEMENT::GetClassNames()
* @brief      Get the split class list (one XSTRING per class name). Empty vector when no class was set.
* @ingroup    USERINTERFACE
*
* @return     XVECTOR<XSTRING*>* : Pointer to the owned class-name vector.
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XSTRING*>* UI_ELEMENT::GetClassNames()
{
  return &class_names;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::SetClassNames(XCHAR* rawlist)
* @brief      Replace the class list. Splits `rawlist` on whitespace and rebuilds the vector view. Passing NULL
*             or an empty string clears the list. Idempotent.
* @ingroup    USERINTERFACE
*
* @param[in]  rawlist : Space-separated class list, as authored in the "class" XML attribute.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetClassNames(XCHAR* rawlist)
{
  // Discard any previous split view (owned).
  for(XDWORD c=0; c<class_names.GetSize(); c++)
    {
      XSTRING* name = class_names.Get(c);
      if(name) GEN_DELETE name;
    }
  class_names.DeleteAll();

  class_names_raw.Empty();
  if(!rawlist) return;

  class_names_raw.Set(rawlist);

  // Split on whitespace (space, tab, CR, LF). XSTRING::Split is single-char; we walk the string manually to
  // support the four whitespace characters at once and to skip empty runs.
  XDWORD  size = class_names_raw.GetSize();
  XDWORD  i    = 0;

  while(i < size)
    {
      // Skip leading whitespace.
      while(i < size)
        {
          XCHAR ch = class_names_raw[(int)i];
          if(ch != __C(' ') && ch != __C('\t') && ch != __C('\r') && ch != __C('\n')) break;
          i++;
        }

      if(i >= size) break;

      // Collect one class name.
      XDWORD start = i;
      while(i < size)
        {
          XCHAR ch = class_names_raw[(int)i];
          if(ch == __C(' ') || ch == __C('\t') || ch == __C('\r') || ch == __C('\n')) break;
          i++;
        }

      if(i > start)
        {
          XSTRING* one = GEN_NEW XSTRING();
          if(one)
            {
              class_names_raw.Copy((int)start, (int)i, *one);
              class_names.Add(one);
            }
        }
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::SetClassNames(XSTRING& rawlist)
* @brief      Convenience overload; forwards to SetClassNames(XCHAR*).
* @ingroup    USERINTERFACE
*
* @param[in]  rawlist : Space-separated class list, as authored in the "class" XML attribute.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetClassNames(XSTRING& rawlist)
{
  SetClassNames(rawlist.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_ELEMENT::HasClass(XCHAR* classname)
* @brief      Case-insensitive membership test against the split class list.
* @ingroup    USERINTERFACE
*
* @param[in]  classname : Class name to look up.
*
* @return     bool : true if the class is present; false otherwise (including a NULL/empty query).
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::HasClass(XCHAR* classname)
{
  if(!classname)      return false;
  if(!classname[0])   return false;

  for(XDWORD c=0; c<class_names.GetSize(); c++)
    {
      XSTRING* one = class_names.Get(c);
      if(one && one->Compare(classname, true) == 0) return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_ELEMENT_CHROMEROLE UI_ELEMENT::GetChromeRole()
* @brief      Get chrome role
* @ingroup    USERINTERFACE
* 
* @return     UI_ELEMENT_CHROMEROLE : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT_CHROMEROLE UI_ELEMENT::GetChromeRole()
{
  return chromerole;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetChromeRole(UI_ELEMENT_CHROMEROLE chromerole)
* @brief      Set chrome role
* @ingroup    USERINTERFACE
* 
* @param[in]  chromerole : Chromerole value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetChromeRole(UI_ELEMENT_CHROMEROLE chromerole)
{
  this->chromerole = chromerole;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_ELEMENT::GetFather()
* @brief      Get father
* @ingroup    USERINTERFACE
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_ELEMENT::GetFather()
{
  return father;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetFather(UI_ELEMENT* father)
* @brief      Set father
* @ingroup    USERINTERFACE
* 
* @param[in]  father : Father pointer to use.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetFather(UI_ELEMENT* father)
{
  this->father = father;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::IsDetached()
* @brief      Is detached
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::IsDetached()
{
  return isdetached;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetIsDetached(bool isdetached)
* @brief      Set is detached
* @ingroup    USERINTERFACE
* 
* @param[in]  isdetached : Isdetached value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetIsDetached(bool isdetached)
{
  this->isdetached = isdetached;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_COLOR* UI_ELEMENT::GetColor()
* @brief      Get color
* @ingroup    USERINTERFACE
* 
* @return     UI_COLOR* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_COLOR* UI_ELEMENT::GetColor()
{
  return &color;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_COLOR* UI_ELEMENT::GetBackgroundColor()
* @brief      Get background color
* @ingroup    USERINTERFACE
* 
* @return     UI_COLOR* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_COLOR* UI_ELEMENT::GetBackgroundColor()
{
  return &backgroundcolor;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_ELEMENT::IsColorSet()
* @brief      True iff the "color" property was authored (XML attribute or CSS declaration) on this element,
*             as opposed to left at its zero-initialised default. Consumers use this flag to skip inheriting
*             from ancestors, and to walk the parent chain when a child requests CSS "color: inherit".
* @ingroup    USERINTERFACE
*
* @return     bool : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::IsColorSet()
{
  return color_set;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::SetColorSet(bool value)
* @brief      Mark the "color" property as authored on this element.
* @ingroup    USERINTERFACE
*
* @param[in]  value : true = authored, false = default.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetColorSet(bool value)
{
  color_set = value;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_ELEMENT::IsBackgroundColorSet()
* @brief      True iff the "bckgrdcolor" (or CSS "background-color") property was authored on this element.
*             Draw_Form uses it to decide between the CSS-natural fill (background_color) and the pre-step-6
*             fallback of using the "color" property as the fill.
* @ingroup    USERINTERFACE
*
* @return     bool : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::IsBackgroundColorSet()
{
  return background_color_set;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::SetBackgroundColorSet(bool value)
* @brief      Mark the "bckgrdcolor" property as authored on this element.
* @ingroup    USERINTERFACE
*
* @param[in]  value : true = authored, false = default.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetBackgroundColorSet(bool value)
{
  background_color_set = value;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_ELEMENT::IsBoxShadowSet()
* @brief      True iff a CSS box-shadow was authored on this element. Consumers gate both the shadow render
*             and the rebuild-area expansion on this flag, so elements without a shadow pay nothing.
* @ingroup    USERINTERFACE
*
* @return     bool : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::IsBoxShadowSet()
{
  return box_shadow_set;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::SetBoxShadowSet(bool value)
* @brief      Latch the "shadow authored" flag. Called by the loader after the parser has filled the offset /
*             blur / colour components, and cleared to false when a stylesheet stops providing a shadow.
* @ingroup    USERINTERFACE
*
* @param[in]  value : Value to store.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetBoxShadowSet(bool value)
{
  box_shadow_set = value;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double UI_ELEMENT::GetShadowOffsetX()
* @brief      Shadow horizontal offset in pixels. Positive shifts the shadow to the right.
* @ingroup    USERINTERFACE
*
* @return     double : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetShadowOffsetX()
{
  return shadow_offset_x;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::SetShadowOffsetX(double value)
* @brief      Set shadow horizontal offset.
* @ingroup    USERINTERFACE
*
* @param[in]  value : Value to store.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetShadowOffsetX(double value)
{
  shadow_offset_x = value;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double UI_ELEMENT::GetShadowOffsetY()
* @brief      Shadow vertical offset in pixels. Positive shifts the shadow DOWN (screen y-down convention).
* @ingroup    USERINTERFACE
*
* @return     double : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetShadowOffsetY()
{
  return shadow_offset_y;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::SetShadowOffsetY(double value)
* @brief      Set shadow vertical offset.
* @ingroup    USERINTERFACE
*
* @param[in]  value : Value to store.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetShadowOffsetY(double value)
{
  shadow_offset_y = value;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double UI_ELEMENT::GetShadowBlur()
* @brief      Shadow blur radius in pixels. Stored for authoring parity with CSS, but not rendered in this
*             rebanada; the skin currently draws a hard-edged shadow regardless of this value.
* @ingroup    USERINTERFACE
*
* @return     double : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetShadowBlur()
{
  return shadow_blur;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::SetShadowBlur(double value)
* @brief      Set shadow blur radius (accepted but not yet rendered).
* @ingroup    USERINTERFACE
*
* @param[in]  value : Value to store.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetShadowBlur(double value)
{
  shadow_blur = value;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_COLOR* UI_ELEMENT::GetShadowColor()
* @brief      Get the shadow tint colour.
* @ingroup    USERINTERFACE
*
* @return     UI_COLOR* : Pointer to the owned colour object.
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_COLOR* UI_ELEMENT::GetShadowColor()
{
  return &shadow_color;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double UI_ELEMENT::GetXPosition()
* @brief      Get X position
* @ingroup    USERINTERFACE
* 
* @return     double : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetXPosition()
{
  return x_position;
}
	

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SetXPosition(double x_position)
* @brief      Set X position
* @ingroup    USERINTERFACE
* 
* @param[in]  x_position : X position value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::SetXPosition(double x_position)   
{
  this->x_position = x_position;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT::GetYPosition()
* @brief      Get Y position
* @ingroup    USERINTERFACE
* 
* @return     double : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetYPosition()
{
  return y_position;  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SetYPosition(double y_position)
* @brief      Set Y position
* @ingroup    USERINTERFACE
* 
* @param[in]  y_position : Y position value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::SetYPosition(double y_position)
{
  this->y_position = y_position;

  return true;
}		


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD UI_ELEMENT::GetZLevel()
* @brief      Get Z level
* @ingroup    USERINTERFACE
* 
* @return     XDWORD : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD UI_ELEMENT::GetZLevel()
{
  return z_level;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SetZLevel(XDWORD z_level)
* @brief      Set Z level
* @ingroup    USERINTERFACE
* 
* @param[in]  z_level : Z level value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::SetZLevel(XDWORD z_level)
{
  this->z_level = z_level;

  return true;
}		


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT_TYPE_DIRECTION UI_ELEMENT::GetDirection()
* @brief      Get direction
* @ingroup    USERINTERFACE
* 
* @return     UI_ELEMENT_TYPE_DIRECTION : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT_TYPE_DIRECTION UI_ELEMENT::GetDirection()
{
  return direction;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SetDirection(UI_ELEMENT_TYPE_DIRECTION direction)
* @brief      Set direction
* @ingroup    USERINTERFACE
* 
* @param[in]  direction : Direction value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::SetDirection(UI_ELEMENT_TYPE_DIRECTION direction)
{
  this->direction = direction;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT::GetXPositionWithScroll()
* @brief      Get X position with scroll
* @ingroup    USERINTERFACE
* 
* @return     double : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetXPositionWithScroll()
{
  return x_positionwithscroll;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SetXPositionWithScroll(double x_positionwithscroll)
* @brief      Set X position with scroll
* @ingroup    USERINTERFACE
* 
* @param[in]  x_positionwithscroll : X positionwithscroll value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::SetXPositionWithScroll(double x_positionwithscroll)
{
  this->x_positionwithscroll = x_positionwithscroll;

  return true;
}		


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT::GetYPositionWithScroll()
* @brief      Get Y position with scroll
* @ingroup    USERINTERFACE
* 
* @return     double : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetYPositionWithScroll()
{
  return y_positionwithscroll;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SetYPositionWithScroll(double y_positionwithscroll)
* @brief      Set Y position with scroll
* @ingroup    USERINTERFACE
* 
* @param[in]  y_positionwithscroll : Y positionwithscroll value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::SetYPositionWithScroll(double y_positionwithscroll)
{
  this->y_positionwithscroll = y_positionwithscroll;
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_BOUNDARYLINE* UI_ELEMENT::GetBoundaryLine()
* @brief      Get boundary line
* @ingroup    USERINTERFACE
* 
* @return     UI_BOUNDARYLINE* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_BOUNDARYLINE* UI_ELEMENT::GetBoundaryLine()
{
  return &boundaryline;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetBoundaryLine(UI_BOUNDARYLINE& boundaryline)
* @brief      Set boundary line
* @ingroup    USERINTERFACE
* 
* @param[in]  boundaryline : Boundaryline value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetBoundaryLine(UI_BOUNDARYLINE& boundaryline)
{
  this->boundaryline.CopyFrom(boundaryline);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT::GetLeftX()
* @brief      Resolved left screen edge from the stored position (single coordinate conversion point).
* @ingroup    USERINTERFACE
* 
* @return     double : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetLeftX()
{
  return UI_BOUNDARYLINE_EdgeLeft(GetXPosition(), GetBoundaryLine()->width);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT::GetRightX()
* @brief      Resolved right screen edge from the stored position (single coordinate conversion point).
* @ingroup    USERINTERFACE
* 
* @return     double : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetRightX()
{
  return UI_BOUNDARYLINE_EdgeRight(GetXPosition(), GetBoundaryLine()->width);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT::GetTopY()
* @brief      Resolved top screen edge from the stored position (single coordinate conversion point).
* @ingroup    USERINTERFACE
* 
* @return     double : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetTopY()
{
  return UI_BOUNDARYLINE_EdgeTop(GetYPosition(), GetBoundaryLine()->height);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT::GetBottomY()
* @brief      Resolved bottom screen edge from the stored position (single coordinate conversion point).
* @ingroup    USERINTERFACE
* 
* @return     double : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetBottomY()
{
  return UI_BOUNDARYLINE_EdgeBottom(GetYPosition(), GetBoundaryLine()->height);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT::GetMargin(UI_ELEMENT_TYPE_ALIGN position)
* @brief      Get margin
* @ingroup    USERINTERFACE
* 
* @param[in]  position : Position value.
* 
* @return     double : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetMargin(UI_ELEMENT_TYPE_ALIGN position)
{
  switch((int)position)
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
* @brief      Set margin
* @ingroup    USERINTERFACE
* 
* @param[in]  position : Position value.
* @param[in]  value : Value value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetMargin(UI_ELEMENT_TYPE_ALIGN position, double value)
{
  switch((int)position)
    {
      case UI_ELEMENT_TYPE_ALIGN_LEFT   : margin[0] = value; break;
      case UI_ELEMENT_TYPE_ALIGN_RIGHT	: margin[1] = value; break;
      case UI_ELEMENT_TYPE_ALIGN_UP		  : margin[2] = value; break;
      case UI_ELEMENT_TYPE_ALIGN_DOWN		: margin[3] = value; break;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double UI_ELEMENT::GetPadding(UI_ELEMENT_TYPE_ALIGN position)
* @brief      Get padding along one side (LEFT/RIGHT/UP/DOWN). Zero on unset elements.
* @ingroup    USERINTERFACE
*
* @param[in]  position : Side to query.
*
* @return     double : Requested value in pixels.
*
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetPadding(UI_ELEMENT_TYPE_ALIGN position)
{
  switch((int)position)
    {
      case UI_ELEMENT_TYPE_ALIGN_LEFT   : return padding[0]; break;
      case UI_ELEMENT_TYPE_ALIGN_RIGHT  : return padding[1]; break;
      case UI_ELEMENT_TYPE_ALIGN_UP     : return padding[2]; break;
      case UI_ELEMENT_TYPE_ALIGN_DOWN   : return padding[3]; break;
    }

  return 0.0f;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::SetPadding(UI_ELEMENT_TYPE_ALIGN position, double value)
* @brief      Set padding along one side.
* @ingroup    USERINTERFACE
*
* @param[in]  position : Side to set.
* @param[in]  value : Value in pixels.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetPadding(UI_ELEMENT_TYPE_ALIGN position, double value)
{
  switch((int)position)
    {
      case UI_ELEMENT_TYPE_ALIGN_LEFT   : padding[0] = value; break;
      case UI_ELEMENT_TYPE_ALIGN_RIGHT  : padding[1] = value; break;
      case UI_ELEMENT_TYPE_ALIGN_UP     : padding[2] = value; break;
      case UI_ELEMENT_TYPE_ALIGN_DOWN   : padding[3] = value; break;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double UI_ELEMENT::GetBorderWidth()
* @brief      Get border stroke width in pixels. -1 means "use skin default" (historically 1.0 for containers);
*             0 means "no border drawn"; positive values are honoured verbatim.
* @ingroup    USERINTERFACE
*
* @return     double : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetBorderWidth()
{
  return border_width;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::SetBorderWidth(double borderwidth)
* @brief      Set border stroke width.
* @ingroup    USERINTERFACE
*
* @param[in]  borderwidth : Border width in pixels (-1 = default, 0 = none, >0 = px).
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetBorderWidth(double borderwidth)
{
  border_width = borderwidth;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_COLOR* UI_ELEMENT::GetBorderColor()
* @brief      Get border colour (base-level equivalent of the per-type linecolor). Consumers should check
*             IsBorderColorSet() before using: an unset border_color is left at UI_COLOR's default state and
*             consumers must fall back to the per-type linecolor to preserve pre-step-5 rendering.
* @ingroup    USERINTERFACE
*
* @return     UI_COLOR* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_COLOR* UI_ELEMENT::GetBorderColor()
{
  return &border_color;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_ELEMENT::IsBorderColorSet()
* @brief      True iff border_color was explicitly authored via XML/CSS. Discriminates a valid transparent
*             authored value (rgba 0,0,0,0) from the "default, use fallback" state.
* @ingroup    USERINTERFACE
*
* @return     bool : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::IsBorderColorSet()
{
  return border_color_set;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::SetBorderColorFromString(XCHAR* string)
* @brief      Set border colour from an authored string ("red", "#RRGGBBAA", "R,G,B[,A]"). Marks as authored.
* @ingroup    USERINTERFACE
*
* @param[in]  string : Colour string.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetBorderColorFromString(XCHAR* string)
{
  if(!string) return;

  border_color.SetFromString(string);
  border_color_set = true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::SetBorderColorFromString(XSTRING& string)
* @brief      Convenience overload; forwards to SetBorderColorFromString(XCHAR*).
* @ingroup    USERINTERFACE
*
* @param[in]  string : Colour string.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetBorderColorFromString(XSTRING& string)
{
  SetBorderColorFromString(string.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double UI_ELEMENT::GetBorderRadius(UI_ELEMENT_BORDER_CORNER corner)
* @brief      Get raw per-corner border radius. Value -1 means "not authored". Use GetEffectiveBorderRadius()
*             for the resolved value with fallback to the element's uniform roundrect.
* @ingroup    USERINTERFACE
*
* @param[in]  corner : Corner index.
*
* @return     double : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetBorderRadius(UI_ELEMENT_BORDER_CORNER corner)
{
  if((corner < 0) || (corner >= UI_ELEMENT_BORDER_CORNER_MAX)) return -1.0;
  return border_radius[corner];
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::SetBorderRadius(UI_ELEMENT_BORDER_CORNER corner, double value)
* @brief      Set per-corner border radius (in pixels). Pass -1 to un-author a corner and re-enable the fallback
*             to the element's uniform roundrect.
* @ingroup    USERINTERFACE
*
* @param[in]  corner : Corner index.
* @param[in]  value : Radius in pixels, or -1 to clear.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetBorderRadius(UI_ELEMENT_BORDER_CORNER corner, double value)
{
  if((corner < 0) || (corner >= UI_ELEMENT_BORDER_CORNER_MAX)) return;
  border_radius[corner] = value;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double UI_ELEMENT::GetEffectiveBorderRadius(UI_ELEMENT_BORDER_CORNER corner)
* @brief      Resolved per-corner radius: the authored value if >= 0, otherwise the element's uniform roundrect.
*             Never returns a negative value; a corner with no radius data and no roundrect returns 0.
* @ingroup    USERINTERFACE
*
* @param[in]  corner : Corner index.
*
* @return     double : Requested value in pixels.
*
* --------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT::GetEffectiveBorderRadius(UI_ELEMENT_BORDER_CORNER corner)
{
  double raw = GetBorderRadius(corner);
  if(raw >= 0.0) return raw;

  return (double)roundrect;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_ELEMENT::HasAnyPerCornerRadius()
* @brief      True iff at least one corner carries an authored per-corner radius. Used by the skin as a
*             fast-path discriminator: when false, the classic single-radius canvas->RoundRect call is enough;
*             when true, the skin must build a custom path with per-corner arcs.
* @ingroup    USERINTERFACE
*
* @return     bool : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::HasAnyPerCornerRadius()
{
  for(int c=0; c<UI_ELEMENT_BORDER_CORNER_MAX; c++)
    {
      if(border_radius[c] >= 0.0) return true;
    }
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_ELEMENT::IsActive()
* @brief      Is active
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::IsActive()
{
  return isactive;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetActive(bool isactive)
* @brief      Set active
* @ingroup    USERINTERFACE
* 
* @param[in]  isactive : Isactive value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetActive(bool isactive)
{
  bool changed = (this->isactive != isactive);

  this->isactive = isactive;

  // State transition -> re-run CSS cascade with the new active-pseudo set. Cheap no-op when the element is not
  // targeted by any pseudo-carrying rule (guarded inside ReapplyStyleVisual by style_has_state_rules).
  if(changed) ReapplyStyleVisual();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::IsPreSelect()
* @brief      Is pre select
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::IsPreSelect()
{
  return ispreselect;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetPreSelect(bool ispreselect)
* @brief      Set pre select
* @ingroup    USERINTERFACE
* 
* @param[in]  ispreselect : Ispreselect value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetPreSelect(bool ispreselect)
{
  bool changed = (this->ispreselect != ispreselect);

  this->ispreselect = ispreselect;

  if(changed) ReapplyStyleVisual();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::IsSelected()
* @brief      Is selected
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::IsSelected()
{  
  return isselected;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetSelected(bool isselected)
* @brief      Set selected
* @ingroup    USERINTERFACE
* 
* @param[in]  isselected : Isselected value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetSelected(bool isselected)
{
  bool changed = (this->isselected != isselected);

  this->isselected = isselected;

  if(changed) ReapplyStyleVisual();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::IsVisible()
* @brief      Is visible
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::IsVisible()
{
  return isvisible;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetVisible(bool isvisible)
* @brief      Set visible
* @ingroup    USERINTERFACE
* 
* @param[in]  isvisible : Isvisible value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @brief      Get round rect
* @ingroup    USERINTERFACE
* 
* @return     XDWORD : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD UI_ELEMENT::GetRoundRect()
{
  return roundrect;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetRoundRect(XDWORD roundrect)
* @brief      Set round rect
* @ingroup    USERINTERFACE
* 
* @param[in]  roundrect : Roundrect value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetRoundRect(XDWORD roundrect)
{
  this->roundrect = roundrect;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD UI_ELEMENT::IsBlinking()
* @brief      Is blinking
* @ingroup    USERINTERFACE
* 
* @return     XDWORD : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD UI_ELEMENT::IsBlinking()
{
  return blink_time;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SetBlink(XDWORD blink_time)
* @brief      Set blink
* @ingroup    USERINTERFACE
* 
* @param[in]  blink_time : Blink time value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @brief      Get state blink
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::GetStateBlink()
{
  return blink_state;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::SwitchStateBlink()
* @brief      Swicth state blink
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::SwitchStateBlink()
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
* @brief      Get timer blink
* @ingroup    USERINTERFACE
* 
* @return     XTIMER* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XTIMER* UI_ELEMENT::GetTimerBlink()
{
  return blink_xtimer;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT_TRANSITION_STATE_SHOW UI_ELEMENT::GetTransitionStateShow()
* @brief      Get transition state show
* @ingroup    USERINTERFACE
* 
* @return     UI_ELEMENT_TRANSITION_STATE_SHOW : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT_TRANSITION_STATE_SHOW UI_ELEMENT::GetTransitionStateShow()
{
  return transitionstateshow;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetTransitionStateShow(UI_ELEMENT_TRANSITION_STATE_SHOW transitionstateshow)
* @brief      Set transition state show
* @ingroup    USERINTERFACE
* 
* @param[in]  transitionstateshow : Transitionstateshow value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetTransitionStateShow(UI_ELEMENT_TRANSITION_STATE_SHOW transitionstateshow)
{
  this->transitionstateshow = transitionstateshow;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::HasScroll()
* @brief      Has scroll
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::HasScroll()
{
  return hasscroll;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetHasScroll(bool hasscroll)
* @brief      Set has scroll
* @ingroup    USERINTERFACE
* 
* @param[in]  hasscroll : Hasscroll value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetHasScroll(bool hasscroll)
{
  this->hasscroll = hasscroll;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* UI_ELEMENT::GetExtra()
* @brief      Get extra
* @ingroup    USERINTERFACE
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* UI_ELEMENT::GetExtra()
{
  return &extra;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::MustReDraw()
* @brief      Must re draw
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::MustReDraw()
{
  return mustredraw;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT::SetMustReDraw(bool mustredraw)
* @brief      Set must re draw
* @ingroup    USERINTERFACE
* 
* @param[in]  mustredraw : Mustredraw value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetMustReDraw(bool mustredraw)
{
  this->mustredraw = mustredraw;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<UI_ELEMENT*>* UI_ELEMENT::GetComposeElements()
* @brief      Get compose elements
* @ingroup    USERINTERFACE
* 
* @return     XVECTOR<UI_ELEMENT*>* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<UI_ELEMENT*>* UI_ELEMENT::GetComposeElements()
{
  return &compose_elements;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT::DeleteAllComposeElements()
* @brief      Delete all compose elements
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::DeleteAllComposeElements()
{
  if(compose_elements.IsEmpty()) return false;

  compose_elements.DeleteContents();
  compose_elements.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::GetActivePseudos(XVECTOR<XSTRING*>& out)
* @brief      Populates `out` with the pseudo-class names implied by the element's current live state.
*             Strings are heap-allocated with GEN_NEW; the caller owns them and must delete them.
* @ingroup    USERINTERFACE
*
* @param[out] out : Vector to append pseudo names into.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::GetActivePseudos(XVECTOR<XSTRING*>& out)
{
  if(ispreselect)
    {
      XSTRING* p = GEN_NEW XSTRING(); if(p) { p->Set(__L("preselect")); out.Add(p); }
    }

  if(isselected)
    {
      XSTRING* p = GEN_NEW XSTRING(); if(p) { p->Set(__L("selected"));  out.Add(p); }
    }

  if(isactive)
    {
      XSTRING* p = GEN_NEW XSTRING(); if(p) { p->Set(__L("active"));    out.Add(p); }
    }
   else
    {
      XSTRING* p = GEN_NEW XSTRING(); if(p) { p->Set(__L("disabled"));  out.Add(p); }
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::SnapshotStyleVisual()
* @brief      Captures color / bckgrdcolor / roundrect as the baseline restored by every ReapplyStyleVisual() call.
*             UI_MANAGER calls this once at end-of-load, after XML and stateless CSS have populated the element.
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SnapshotStyleVisual()
{
  color          .CopyTo(&snapshot_color);
  backgroundcolor.CopyTo(&snapshot_backgroundcolor);
  snapshot_roundrect = roundrect;
  snapshot_taken     = true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::ReapplyStyleVisual()
* @brief      Restore the visual baseline and layer the active stylesheet's rules on top, using the current
*             pseudo-class set derived from state. No-op when no stylesheet is loaded, no snapshot was taken,
*             or the loader marked this element as unaffected by pseudo rules.
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::ReapplyStyleVisual()
{
  if(!style_has_state_rules) return;
  if(!snapshot_taken)        return;

  UI_STYLESHEET* sheet = NULL;
  if(UI_MANAGER::GetIsInstanced())
    {
      sheet = UI_MANAGER::GetInstance().GetStyleSheet();
    }

  if(!sheet) return;

  // Restore baseline BEFORE re-resolving, so keys the current cascade does not touch fall back to the authored
  // (XML + stateless CSS) value rather than remaining stuck at whatever the previous state left them.
  snapshot_color          .CopyTo(&color);
  snapshot_backgroundcolor.CopyTo(&backgroundcolor);
  roundrect = snapshot_roundrect;

  // Re-resolve. FillFromCSSDeclarations internally builds the active-pseudo list from the element's live state.
  UI_STYLE bag;
  bag.FillFromCSSDeclarations(sheet, this);

  // Apply the three baseline visual keys. Per-type keys (linecolor, gradientcolor, thickness, ...) are out of
  // scope for this step; they remain frozen at load-time values.
  XSTRING v;
  double  d;

  if(bag.Get(__L("color")      , v))  color          .SetFromString(v);
  if(bag.Get(__L("bckgrdcolor"), v))  backgroundcolor.SetFromString(v);
  if(bag.Get(__L("roundrect")  , d))  roundrect = (XDWORD)d;

  mustredraw = true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_ELEMENT::GetStyleHasStateRules()
* @brief      True iff at least one pseudo-carrying rule in the active stylesheet targets this element.
* @ingroup    USERINTERFACE
*
* @return     bool : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT::GetStyleHasStateRules()
{
  return style_has_state_rules;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::SetStyleHasStateRules(bool has)
* @brief      Marker set by UI_MANAGER at load time; enables the ReapplyStyleVisual() fast path.
* @ingroup    USERINTERFACE
*
* @param[in]  has : Value to set.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::SetStyleHasStateRules(bool has)
{
  style_has_state_rules = has;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_ELEMENT::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT::Clean()
{
  type                    = UI_ELEMENT_TYPE_UNKNOWN;
  chromerole              = UI_ELEMENT_CHROMEROLE_NONE;

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

  for(XDWORD c=0; c<UI_ELEMENT_MARGIN_MAX; c++)
    {
      padding[c] = 0.0f;
    }

  border_width = -1.0;    // -1 signals "unset": skin uses its historical default (1.0 for containers).

  border_color_set = false;
  for(int c=0; c<UI_ELEMENT_BORDER_CORNER_MAX; c++)
    {
      border_radius[c] = -1.0;   // -1 signals "unset": fall back to the element's uniform roundrect.
    }

  color_set            = false;
  background_color_set = false;

  box_shadow_set   = false;
  shadow_offset_x  = 0.0;
  shadow_offset_y  = 0.0;
  shadow_blur      = 0.0;

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

  snapshot_roundrect      = 0;
  snapshot_taken          = false;
  style_has_state_rules   = false;
}



