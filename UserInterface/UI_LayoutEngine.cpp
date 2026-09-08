/**-------------------------------------------------------------------------------------------------------------------
*
* @file       UI_LayoutEngine.cpp
*
* @class      UI_LAYOUTENGINE
* @brief      User Interface : builds a UI_LAYOUTBOX tree from a UI_ELEMENT tree.
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

#include "UI_LayoutEngine.h"
#include "UI_CSSAdapter.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_LAYOUTBOX* UI_LAYOUTENGINE::BuildTree(UI_ELEMENT* root)
* @brief      Build tree
* @note       STATIC
* @ingroup    USERINTERFACE
*
* @param[in]  root : Element to build a UI_LAYOUTBOX (sub)tree for; NULL returns NULL.
*
* @return     UI_LAYOUTBOX* : Owned tree root, or NULL if "root" is NULL.
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUTBOX* UI_LAYOUTENGINE::BuildTree(UI_ELEMENT* root)
{
  if(!root) return NULL;

  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  UI_CSSBOX cssbox = UI_CSSBox_Get(root);
  box->SetContentBox(cssbox.left, cssbox.top, cssbox.width, cssbox.height);

  box->SetPadding(root->GetPadding(UI_ELEMENT_TYPE_ALIGN_UP),
                   root->GetPadding(UI_ELEMENT_TYPE_ALIGN_RIGHT),
                   root->GetPadding(UI_ELEMENT_TYPE_ALIGN_DOWN),
                   root->GetPadding(UI_ELEMENT_TYPE_ALIGN_LEFT));

  double borderwidth = root->GetBorderWidth();
  if(borderwidth < 0.0) borderwidth = 0.0;    // -1 means "skin default": a painting concern, not a layout one

  box->SetBorder(borderwidth, borderwidth, borderwidth, borderwidth);

  box->SetMargin(root->GetMargin(UI_ELEMENT_TYPE_ALIGN_UP),
                  root->GetMargin(UI_ELEMENT_TYPE_ALIGN_RIGHT),
                  root->GetMargin(UI_ELEMENT_TYPE_ALIGN_DOWN),
                  root->GetMargin(UI_ELEMENT_TYPE_ALIGN_LEFT));

  XVECTOR<UI_ELEMENT*>* childelements = root->GetComposeElements();
  if(childelements)
    {
      for(XDWORD c=0; c<childelements->GetSize(); c++)
        {
          UI_ELEMENT* childelement = childelements->Get(c);
          if(!childelement) continue;

          UI_LAYOUTBOX* childbox = BuildTree(childelement);
          if(childbox) box->AddChild(childbox);
        }
    }

  return box;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTENGINE::ApplyPositioning(UI_LAYOUTBOX* root)
* @brief      Apply positioning
* @note       STATIC
* @ingroup    USERINTERFACE
*
* @param[in]  root : Tree to reposition in place (root and every descendant). Ignored if NULL.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTENGINE::ApplyPositioning(UI_LAYOUTBOX* root)
{
  if(!root) return;

  ApplyPositioningRecursive(root, root, NULL);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTENGINE::ApplyPositioningRecursive(UI_LAYOUTBOX* box, UI_LAYOUTBOX* rootbox, UI_LAYOUTBOX* nearestpositionedancestor)
* @brief      Apply positioning recursive
* @note       INTERNAL / STATIC
* @ingroup    USERINTERFACE
*
* @param[in]  box : Node being visited.
* @param[in]  rootbox : Tree root, used as the containing block fallback when no ancestor is positioned.
* @param[in]  nearestpositionedancestor : Nearest ancestor (of "box") whose position is not STATIC, or NULL.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTENGINE::ApplyPositioningRecursive(UI_LAYOUTBOX* box, UI_LAYOUTBOX* rootbox, UI_LAYOUTBOX* nearestpositionedancestor)
{
  if(!box) return;

  switch(box->GetPosition())
    {
      case UI_POSITION_RELATIVE:
        ApplyRelativeOffset(box);
        break;

      case UI_POSITION_ABSOLUTE:
        ApplyAbsolutePosition(box, nearestpositionedancestor ? nearestpositionedancestor : rootbox);
        break;

      case UI_POSITION_STATIC:
      default:
        break;
    }

  UI_LAYOUTBOX* childpositionedancestor = (box->GetPosition() != UI_POSITION_STATIC) ? box : nearestpositionedancestor;

  XVECTOR<UI_LAYOUTBOX*>& children = box->GetChildren();
  for(XDWORD c=0; c<children.GetSize(); c++)
    {
      UI_LAYOUTBOX* child = children.Get(c);
      if(child) ApplyPositioningRecursive(child, rootbox, childpositionedancestor);
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTENGINE::ApplyRelativeOffset(UI_LAYOUTBOX* box)
* @brief      Apply relative offset
* @note       INTERNAL / STATIC
* @ingroup    USERINTERFACE
*
* @param[in]  box : Node to offset in place from its own current (static-flow) content box position.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTENGINE::ApplyRelativeOffset(UI_LAYOUTBOX* box)
{
  if(!box) return;

  UI_LAYOUTBOX_INSETS& insets = box->GetInsets();

  double horizontaloffset = 0.0;
  if(insets.left.specified)            horizontaloffset =  insets.left.value;
  else if(insets.right.specified)      horizontaloffset = -insets.right.value;

  double verticaloffset = 0.0;
  if(insets.top.specified)             verticaloffset =  insets.top.value;
  else if(insets.bottom.specified)     verticaloffset = -insets.bottom.value;

  box->SetContentBox(box->GetContentLeft() + horizontaloffset, box->GetContentTop() + verticaloffset,
                      box->GetContentWidth(), box->GetContentHeight());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTENGINE::ApplyAbsolutePosition(UI_LAYOUTBOX* box, UI_LAYOUTBOX* containingblock)
* @brief      Apply absolute position
* @note       INTERNAL / STATIC
* @ingroup    USERINTERFACE
*
* @param[in]  box : Node to reposition in place.
* @param[in]  containingblock : Box whose PADDING box "box" is positioned against (nearest positioned ancestor,
*                                or the tree root).
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTENGINE::ApplyAbsolutePosition(UI_LAYOUTBOX* box, UI_LAYOUTBOX* containingblock)
{
  if(!box || !containingblock) return;

  double cbleft, cbtop, cbwidth, cbheight;
  containingblock->GetPaddingBox(cbleft, cbtop, cbwidth, cbheight);

  UI_LAYOUTBOX_INSETS& insets = box->GetInsets();

  double newleft = box->GetContentLeft();
  if(insets.left.specified)            newleft = cbleft + insets.left.value;
  else if(insets.right.specified)      newleft = cbleft + cbwidth - insets.right.value - box->GetContentWidth();

  double newtop = box->GetContentTop();
  if(insets.top.specified)             newtop = cbtop + insets.top.value;
  else if(insets.bottom.specified)     newtop = cbtop + cbheight - insets.bottom.value - box->GetContentHeight();

  box->SetContentBox(newleft, newtop, box->GetContentWidth(), box->GetContentHeight());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTENGINE::WriteBackTree(UI_ELEMENT* root, UI_LAYOUTBOX* tree)
* @brief      Write back tree
* @note       STATIC
* @ingroup    USERINTERFACE
*
* @param[in]  root : Element tree to write geometry into (root and every descendant). Ignored if NULL.
* @param[in]  tree : UI_LAYOUTBOX tree "root" was built from (see BuildTree()). Ignored if NULL.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTENGINE::WriteBackTree(UI_ELEMENT* root, UI_LAYOUTBOX* tree)
{
  if(!root || !tree) return;

  WriteBackRecursive(root, tree);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTENGINE::WriteBackRecursive(UI_ELEMENT* element, UI_LAYOUTBOX* box)
* @brief      Write back recursive
* @note       INTERNAL / STATIC
* @ingroup    USERINTERFACE
*
* @param[in]  element : Element to write "box"'s content box into, then recurse into its children.
* @param[in]  box : Box paired with "element" (see the SCOPE ADDENDUM in UI_LayoutEngine.h for the pairing rule).
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTENGINE::WriteBackRecursive(UI_ELEMENT* element, UI_LAYOUTBOX* box)
{
  if(!element || !box) return;

  UI_CSSBOX cssbox;
  cssbox.left   = box->GetContentLeft();
  cssbox.top    = box->GetContentTop();
  cssbox.width  = box->GetContentWidth();
  cssbox.height = box->GetContentHeight();

  UI_CSSBox_Set(element, cssbox);

  XVECTOR<UI_ELEMENT*>* elementchildren = element->GetComposeElements();
  if(!elementchildren) return;

  XVECTOR<UI_LAYOUTBOX*>& boxchildren = box->GetChildren();

  // Pairs each element child with its box child by walking both in lockstep, skipping NULL element entries --
  // the SAME rule BuildTree() used to decide which elements got a box in the first place (see BuildTree()
  // above), so the two child lists line up index-for-index despite any NULL holes in "elementchildren".
  XDWORD boxindex = 0;
  for(XDWORD c=0; c<elementchildren->GetSize() && boxindex<boxchildren.GetSize(); c++)
    {
      UI_ELEMENT* childelement = elementchildren->Get(c);
      if(!childelement) continue;

      WriteBackRecursive(childelement, boxchildren.Get(boxindex));
      boxindex++;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LAYOUTENGINE::RunLayout(UI_ELEMENT* root, UI_LAYOUTSTRATEGY strategy)
* @brief      Run layout
* @note       STATIC
* @ingroup    USERINTERFACE
*
* @param[in]  root : Element tree to lay out. Ignored if NULL.
* @param[in]  strategy : UI_LAYOUTSTRATEGY_LEGACY (no-op) or UI_LAYOUTSTRATEGY_CSS (runs the new engine).
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LAYOUTENGINE::RunLayout(UI_ELEMENT* root, UI_LAYOUTSTRATEGY strategy)
{
  if(!root) return;
  if(strategy != UI_LAYOUTSTRATEGY_CSS) return;   // LEGACY: existing geometry stands untouched, by design

  UI_LAYOUTBOX* tree = BuildTree(root);
  if(!tree) return;

  ApplyPositioning(tree);
  WriteBackTree(root, tree);

  GEN_DELETE tree;
}
