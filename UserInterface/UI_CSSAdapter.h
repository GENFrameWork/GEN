/**-------------------------------------------------------------------------------------------------------------------
*
* @file       UI_CSSAdapter.h
*
* @class      UI_CSSADAPTER (free helpers)
* @brief      User Interface : top-left / CSS box-model boundary.
* @ingroup    USERINTERFACE
*
* SINGLE PURPOSE
*   Bridge between the CSS box model (origin TOP-LEFT, +X right, +Y down, a box occupies [top, top+height])
*   and GEN's internal UI coordinate storage, WITHOUT changing that internal storage.
*
*   GEN keeps its working, bottom-anchored convention (a stored element 'y' is its BOTTOM edge; see
*   UI_BoundaryLine.h). A future HTML/CSS layout layer (e.g. litehtml) thinks entirely in CSS coordinates
*   and is meant to COEXIST with the XML layout. Rather than flipping the whole engine, the two worlds meet
*   ONLY here: CSS-minded code reads/writes element rectangles through these two helpers, and nothing else in
*   the UI module needs to know about CSS.
*
* READ side  (UI_CSSBox_Get) is already convention-independent: it routes through the element's resolved
*            screen edges (GetLeftX/GetTopY), which themselves go through the single coordinate-conversion
*            point (UI_BOUNDARYLINE_Edge*). It keeps working unchanged whatever the internal convention is.
*
* WRITE side (UI_CSSBox_Set) drives GEN rendering from a CSS box by setting the element's RESOLVED position
*            directly (so an external CSS layout engine can position GEN elements without running the XML
*            CalculePosition pass). Exactly ONE line is convention-dependent (the internal Y anchor); it is
*            isolated and documented below, so if the internal storage is ever flipped to top-left that single
*            line is all that changes.
*
* This header is purely additive: it touches no existing code path and changes no current behaviour.
*
* @author     Abraham J. Velez / EndoraSoft
*
* ---------------------------------------------------------------------------------------------------------------------*/

#pragma once

#include "UI_Element.h"
#include "UI_BoundaryLine.h"


/**
* @struct  UI_CSSBOX
* @brief   A rectangle expressed in the CSS box model: top-left origin, occupies [top, top+height].
*/
struct UI_CSSBOX
{
  double left;
  double top;
  double width;
  double height;
};


/**-------------------------------------------------------------------------------------------------------------------
* @fn       UI_CSSBOX UI_CSSBox_Get(UI_ELEMENT* element)
* @brief    Read an element's resolved screen rectangle as a CSS box (top-left origin).
* @param    element : resolved UI element (its position must already be laid out).
* @return   the element's box in CSS coordinates. (left,top,width,height); zeroed if element is null.
* ---------------------------------------------------------------------------------------------------------------------*/
inline UI_CSSBOX UI_CSSBox_Get(UI_ELEMENT* element)
{
  UI_CSSBOX box = { 0.0, 0.0, 0.0, 0.0 };

  if(!element) return box;

  box.left   = element->GetLeftX();                    // resolved LEFT edge  (already convention-independent)
  box.top    = element->GetTopY();                     // resolved TOP  edge  (already convention-independent)
  box.width  = element->GetBoundaryLine()->width;
  box.height = element->GetBoundaryLine()->height;

  return box;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn       void UI_CSSBox_Set(UI_ELEMENT* element, const UI_CSSBOX& box)
* @brief    Position and size an element from a CSS box (top-left origin), driving GEN rendering from an
*           external CSS layout. Sets the RESOLVED position directly (bypasses the XML CalculePosition pass).
* @param    element : the UI element to place.
* @param    box     : the target rectangle in CSS coordinates.
* ---------------------------------------------------------------------------------------------------------------------*/
inline void UI_CSSBox_Set(UI_ELEMENT* element, const UI_CSSBOX& box)
{
  if(!element) return;

  element->GetBoundaryLine()->width  = box.width;
  element->GetBoundaryLine()->height = box.height;

  element->SetXPosition(box.left);                      // internal X stores the LEFT edge (same in both conventions)

  // --- ONLY convention-dependent line -------------------------------------------------------------------------------
  // GEN's internal storage keeps the element BOTTOM edge in GetYPosition(): bottom = top + height.
  // If the internal storage is ever flipped to top-left, change this single line to:  element->SetYPosition(box.top);
  element->SetYPosition(box.top + box.height);
  // ------------------------------------------------------------------------------------------------------------------
}
