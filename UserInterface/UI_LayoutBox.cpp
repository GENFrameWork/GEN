/**-------------------------------------------------------------------------------------------------------------------
*
* @file       UI_LayoutBox.cpp
*
* @class      UI_LAYOUTBOX
* @brief      User Interface : one node of the CSS top-left layout box tree -- pure geometry, no painting.
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

#include "UI_LayoutBox.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_LAYOUTBOX::UI_LAYOUTBOX()
* @brief      Constructor of class
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUTBOX::UI_LAYOUTBOX()
{
  contentleft   = 0.0;
  contenttop    = 0.0;
  contentwidth  = 0.0;
  contentheight = 0.0;

  padding.top = padding.right = padding.bottom = padding.left = 0.0;
  border.top  = border.right  = border.bottom  = border.left  = 0.0;
  margin.top  = margin.right  = margin.bottom  = margin.left  = 0.0;

  position = UI_POSITION_STATIC;

  insets.top.specified = insets.right.specified = insets.bottom.specified = insets.left.specified = false;
  insets.top.value     = insets.right.value     = insets.bottom.value     = insets.left.value     = 0.0;

  parent = NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_LAYOUTBOX::~UI_LAYOUTBOX()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUTBOX::~UI_LAYOUTBOX()
{
  for(XDWORD c=0; c<children.GetSize(); c++)
    {
      UI_LAYOUTBOX* child = children.Get(c);
      if(child) GEN_DELETE child;
    }

  children.DeleteAll();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTBOX::SetContentBox(double left, double top, double width, double height)
* @brief      Set content box
* @ingroup    USERINTERFACE
*
* @param[in]  left : Content box left edge (CSS top-left origin).
* @param[in]  top : Content box top edge.
* @param[in]  width : Content box width.
* @param[in]  height : Content box height.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTBOX::SetContentBox(double left, double top, double width, double height)
{
  contentleft   = left;
  contenttop    = top;
  contentwidth  = width;
  contentheight = height;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double UI_LAYOUTBOX::GetContentLeft()
* @brief      Get content left
* @ingroup    USERINTERFACE
*
* @return     double : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
double UI_LAYOUTBOX::GetContentLeft()
{
  return contentleft;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double UI_LAYOUTBOX::GetContentTop()
* @brief      Get content top
* @ingroup    USERINTERFACE
*
* @return     double : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
double UI_LAYOUTBOX::GetContentTop()
{
  return contenttop;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double UI_LAYOUTBOX::GetContentWidth()
* @brief      Get content width
* @ingroup    USERINTERFACE
*
* @return     double : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
double UI_LAYOUTBOX::GetContentWidth()
{
  return contentwidth;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double UI_LAYOUTBOX::GetContentHeight()
* @brief      Get content height
* @ingroup    USERINTERFACE
*
* @return     double : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
double UI_LAYOUTBOX::GetContentHeight()
{
  return contentheight;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTBOX::SetPadding(double top, double right, double bottom, double left)
* @brief      Set padding
* @ingroup    USERINTERFACE
*
* @param[in]  top : Top edge width.
* @param[in]  right : Right edge width.
* @param[in]  bottom : Bottom edge width.
* @param[in]  left : Left edge width.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTBOX::SetPadding(double top, double right, double bottom, double left)
{
  padding.top    = top;
  padding.right  = right;
  padding.bottom = bottom;
  padding.left   = left;

  ClampEdgesToZero(padding);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTBOX::SetBorder(double top, double right, double bottom, double left)
* @brief      Set border
* @ingroup    USERINTERFACE
*
* @param[in]  top : Top edge width.
* @param[in]  right : Right edge width.
* @param[in]  bottom : Bottom edge width.
* @param[in]  left : Left edge width.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTBOX::SetBorder(double top, double right, double bottom, double left)
{
  border.top    = top;
  border.right  = right;
  border.bottom = bottom;
  border.left   = left;

  ClampEdgesToZero(border);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTBOX::SetMargin(double top, double right, double bottom, double left)
* @brief      Set margin
* @ingroup    USERINTERFACE
*
* @param[in]  top : Top edge width.
* @param[in]  right : Right edge width.
* @param[in]  bottom : Bottom edge width.
* @param[in]  left : Left edge width.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTBOX::SetMargin(double top, double right, double bottom, double left)
{
  margin.top    = top;
  margin.right  = right;
  margin.bottom = bottom;
  margin.left   = left;

  ClampEdgesToZero(margin);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_LAYOUTBOX_EDGES& UI_LAYOUTBOX::GetPadding()
* @brief      Get padding
* @ingroup    USERINTERFACE
*
* @return     UI_LAYOUTBOX_EDGES& : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUTBOX_EDGES& UI_LAYOUTBOX::GetPadding()
{
  return padding;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_LAYOUTBOX_EDGES& UI_LAYOUTBOX::GetBorder()
* @brief      Get border
* @ingroup    USERINTERFACE
*
* @return     UI_LAYOUTBOX_EDGES& : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUTBOX_EDGES& UI_LAYOUTBOX::GetBorder()
{
  return border;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_LAYOUTBOX_EDGES& UI_LAYOUTBOX::GetMargin()
* @brief      Get margin
* @ingroup    USERINTERFACE
*
* @return     UI_LAYOUTBOX_EDGES& : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUTBOX_EDGES& UI_LAYOUTBOX::GetMargin()
{
  return margin;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTBOX::GetPaddingBox(double& left, double& top, double& width, double& height)
* @brief      Get padding box
* @ingroup    USERINTERFACE
*
* @param[out] left : Receives the padding box left edge.
* @param[out] top : Receives the padding box top edge.
* @param[out] width : Receives the padding box width.
* @param[out] height : Receives the padding box height.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTBOX::GetPaddingBox(double& left, double& top, double& width, double& height)
{
  left   = contentleft - padding.left;
  top    = contenttop  - padding.top;
  width  = contentwidth  + padding.left + padding.right;
  height = contentheight + padding.top  + padding.bottom;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTBOX::GetBorderBox(double& left, double& top, double& width, double& height)
* @brief      Get border box
* @ingroup    USERINTERFACE
*
* @param[out] left : Receives the border box left edge.
* @param[out] top : Receives the border box top edge.
* @param[out] width : Receives the border box width.
* @param[out] height : Receives the border box height.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTBOX::GetBorderBox(double& left, double& top, double& width, double& height)
{
  left   = contentleft - padding.left - border.left;
  top    = contenttop  - padding.top  - border.top;
  width  = contentwidth  + padding.left + padding.right  + border.left + border.right;
  height = contentheight + padding.top  + padding.bottom + border.top  + border.bottom;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTBOX::GetMarginBox(double& left, double& top, double& width, double& height)
* @brief      Get margin box
* @ingroup    USERINTERFACE
*
* @param[out] left : Receives the margin box left edge.
* @param[out] top : Receives the margin box top edge.
* @param[out] width : Receives the margin box width.
* @param[out] height : Receives the margin box height.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTBOX::GetMarginBox(double& left, double& top, double& width, double& height)
{
  left   = contentleft - padding.left - border.left - margin.left;
  top    = contenttop  - padding.top  - border.top  - margin.top;
  width  = contentwidth  + padding.left + padding.right  + border.left + border.right  + margin.left + margin.right;
  height = contentheight + padding.top  + padding.bottom + border.top  + border.bottom + margin.top  + margin.bottom;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTBOX::AddChild(UI_LAYOUTBOX* child)
* @brief      Add child
* @ingroup    USERINTERFACE
*
* @param[in]  child : Node to adopt; owned from this call on (this node's destructor deletes it). Ignored if NULL.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTBOX::AddChild(UI_LAYOUTBOX* child)
{
  if(!child) return;

  child->parent = this;

  children.Add(child);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<UI_LAYOUTBOX*>& UI_LAYOUTBOX::GetChildren()
* @brief      Get children
* @ingroup    USERINTERFACE
*
* @return     XVECTOR<UI_LAYOUTBOX*>& : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<UI_LAYOUTBOX*>& UI_LAYOUTBOX::GetChildren()
{
  return children;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_LAYOUTBOX* UI_LAYOUTBOX::GetParent()
* @brief      Get parent
* @ingroup    USERINTERFACE
*
* @return     UI_LAYOUTBOX* : Requested value (borrowed; NULL for a root node).
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUTBOX* UI_LAYOUTBOX::GetParent()
{
  return parent;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTBOX::SetPosition(UI_POSITION position)
* @brief      Set position
* @ingroup    USERINTERFACE
*
* @param[in]  position : CSS "position" tag for this box (STATIC/RELATIVE/ABSOLUTE).
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTBOX::SetPosition(UI_POSITION _position)
{
  position = _position;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_POSITION UI_LAYOUTBOX::GetPosition()
* @brief      Get position
* @ingroup    USERINTERFACE
*
* @return     UI_POSITION : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_POSITION UI_LAYOUTBOX::GetPosition()
{
  return position;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTBOX::SetInsets(UI_LAYOUTBOX_INSETS& insets)
* @brief      Set insets
* @ingroup    USERINTERFACE
*
* @param[in]  insets : Inset set to copy in (each edge independently specified-or-not).
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTBOX::SetInsets(UI_LAYOUTBOX_INSETS& _insets)
{
  insets = _insets;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_LAYOUTBOX_INSETS& UI_LAYOUTBOX::GetInsets()
* @brief      Get insets
* @ingroup    USERINTERFACE
*
* @return     UI_LAYOUTBOX_INSETS& : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUTBOX_INSETS& UI_LAYOUTBOX::GetInsets()
{
  return insets;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTBOX::ClampEdgesToZero(UI_LAYOUTBOX_EDGES& edges)
* @brief      Clamp edges to zero
* @note       INTERNAL / STATIC
* @ingroup    USERINTERFACE
*
* @param[in,out] edges : Edge set to clamp in place; any negative component becomes 0.0.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTBOX::ClampEdgesToZero(UI_LAYOUTBOX_EDGES& edges)
{
  if(edges.top    < 0.0) edges.top    = 0.0;
  if(edges.right  < 0.0) edges.right  = 0.0;
  if(edges.bottom < 0.0) edges.bottom = 0.0;
  if(edges.left   < 0.0) edges.left   = 0.0;
}
