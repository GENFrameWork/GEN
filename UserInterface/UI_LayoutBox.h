/**-------------------------------------------------------------------------------------------------------------------
*
* @file       UI_LayoutBox.h
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
* SCOPE (Phase 3 -- "caja, unidades y posicionamiento", third increment: "pequeños primero")
*   The roadmap's first Phase 3 bullet is "árbol de layout top-left independiente de las coordenadas de pintura
*   GEN (borde inferior)". This is that tree, and ONLY that tree: a pure geometry data structure (content box +
*   padding/border/margin edges, in CSS's own top-left/+Y-down convention -- see UI_CSSAdapter.h for the existing
*   bridge to GEN's bottom-anchored storage) plus owned parent/child links, with NOTHING behind it yet --
*
*     - it is not built FROM a UI_ELEMENT tree anywhere in this increment (a caller populates it by hand, exactly
*       like the tests below do);
*     - it does not run any layout algorithm (no flow, no percentage resolution, no min/max clamping applied --
*       though UI_COMPUTEDSTYLE::ClampToMinMax()/ResolveContentSize() from the previous increment are exactly
*       the kind of helper a future layout pass would call while filling one of these in);
*     - it does not replace or touch UI_SKINCANVAS::CalculePosition() (UI_Manager.cpp/UI_SkinCanvas.cpp are
*       untouched by this increment);
*     - it is not read by UI_CSSAdapter.h's UI_CSSBox_Get/Set (those still talk directly to UI_ELEMENT).
*
*   That wiring -- actually walking father/compose_elements, actually computing sizes from UI_COMPUTEDSTYLE, and
*   actually driving GEN's rendering through it -- is the fourth (and by far largest) Phase 3 increment: the
*   real UI_LAYOUTENGINE. Until then, UI_LAYOUTBOX exists purely so that increment can be built and tested
*   against a known-correct, already-tested geometry container instead of inventing one under time pressure.
*
* SCOPE ADDENDUM (fourth increment, second sub-step: "position:relative/absolute + insets")
*   Added here: a UI_POSITION tag per box, and a per-edge "specified or not" inset set (UI_LAYOUTBOX_INSETS --
*   NOT reusing UI_LAYOUTBOX_EDGES, because a CSS inset can be legitimately absent/"auto" on any one edge, unlike
*   padding/border/margin which are always numeric). Still pure data: UI_LAYOUTBOX itself applies no positioning
*   math -- that is UI_LAYOUTENGINE::ApplyPositioning()'s job (UI_LayoutEngine.h), reading exactly what is set
*   here. A caller (test, or the engine once wired) populates SetPosition()/SetInsets() by hand; this sub-step
*   still does not read UI_COMPUTEDSTYLE's "position"/"top"/"right"/"bottom"/"left" properties anywhere. The
*   third (and last) sub-step of this increment wires UI_LAYOUTENGINE's output to UI_CSSBox_Set() with the two
*   strategies (legacy/CSS) selectable -- it does NOT add CSS-rule-driven position/insets either: there is
*   currently no per-UI_ELEMENT stored computed style to read them from (GetLayoutElement_Base() in UI_Manager.cpp
*   builds a UI_COMPUTEDSTYLE transiently per XML node at parse time and does not persist it on the element), so
*   making "position:relative" in an actual stylesheet drive an actual UI_ELEMENT remains future work, tracked
*   there rather than promised here.
*
* @author     Abraham J. Velez / EndoraSoft
*
* ---------------------------------------------------------------------------------------------------------------------*/

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XVector.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


/**
* @struct  UI_LAYOUTBOX_EDGES
* @brief   Four edge widths (CSS order: top/right/bottom/left), reused identically for padding, border and margin.
*/
struct UI_LAYOUTBOX_EDGES
{
  double top;
  double right;
  double bottom;
  double left;
};


/**
* @enum   UI_POSITION
* @brief  CSS "position" property, restricted to the values this engine currently understands.
*/
enum UI_POSITION
{
  UI_POSITION_STATIC = 0,           // default: participates in normal flow, never a containing block for others
  UI_POSITION_RELATIVE,             // offset from its own static position; still a containing block for descendants
  UI_POSITION_ABSOLUTE              // positioned against its nearest non-STATIC ancestor (or the tree root)
};


/**
* @struct  UI_LAYOUTBOX_INSET
* @brief   One CSS inset edge (top/right/bottom/left): either unspecified ("auto", the default) or a resolved value.
*/
struct UI_LAYOUTBOX_INSET
{
  bool   specified;
  double value;
};


/**
* @struct  UI_LAYOUTBOX_INSETS
* @brief   The four CSS inset edges, each independently specified-or-not.
*/
struct UI_LAYOUTBOX_INSETS
{
  UI_LAYOUTBOX_INSET top;
  UI_LAYOUTBOX_INSET right;
  UI_LAYOUTBOX_INSET bottom;
  UI_LAYOUTBOX_INSET left;
};


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class UI_LAYOUTBOX
{
  public:
                                    UI_LAYOUTBOX                 ();
    virtual                        ~UI_LAYOUTBOX                 ();

    // Content box: CSS top-left origin, occupies [top, top+height) x [left, left+width). Whatever a future
    // layout pass decides the CONTENT size is (see UI_COMPUTEDSTYLE::ResolveContentSize() from the previous
    // increment) -- padding/border/margin below are layered OUTWARD from this, never inward.
    void                            SetContentBox                 (double left, double top, double width, double height);
    double                          GetContentLeft                ();
    double                          GetContentTop                  ();
    double                          GetContentWidth                ();
    double                          GetContentHeight               ();

    // Padding / border / margin, one edge-width set each. Negative input is clamped to 0 (CSS never has a
    // negative padding/border/margin edge, whatever a bad computed value might otherwise say).
    void                            SetPadding                     (double top, double right, double bottom, double left);
    void                            SetBorder                      (double top, double right, double bottom, double left);
    void                            SetMargin                      (double top, double right, double bottom, double left);
    UI_LAYOUTBOX_EDGES&              GetPadding                     ();
    UI_LAYOUTBOX_EDGES&              GetBorder                      ();
    UI_LAYOUTBOX_EDGES&              GetMargin                      ();

    // Derived rects, each the content box expanded outward by one more layer (padding; padding+border;
    // padding+border+margin) -- the standard CSS "padding box" / "border box" / "margin box" terms.
    void                            GetPaddingBox                  (double& left, double& top, double& width, double& height);
    void                            GetBorderBox                   (double& left, double& top, double& width, double& height);
    void                            GetMarginBox                   (double& left, double& top, double& width, double& height);

    // Tree: "child" is OWNED from this call on (deleted, recursively, by this node's own destructor) and its
    // parent link is set to "this". Deliberately mirrors UI_ELEMENT's own father/compose_elements shape (see
    // UI_Element.h) rather than inventing a different convention, since a future increment sources one tree
    // from the other.
    void                            AddChild                       (UI_LAYOUTBOX* child);
    XVECTOR<UI_LAYOUTBOX*>&          GetChildren                    ();
    UI_LAYOUTBOX*                    GetParent                      ();

    // Position tag and insets: pure storage, defaults to STATIC / nothing specified (i.e. "auto" on all four
    // edges). See the SCOPE ADDENDUM above -- UI_LAYOUTENGINE::ApplyPositioning() is what interprets these.
    void                            SetPosition                    (UI_POSITION position);
    UI_POSITION                     GetPosition                    ();
    void                            SetInsets                      (UI_LAYOUTBOX_INSETS& insets);
    UI_LAYOUTBOX_INSETS&             GetInsets                      ();

  private:

    static void                     ClampEdgesToZero               (UI_LAYOUTBOX_EDGES& edges);

    double                           contentleft;
    double                           contenttop;
    double                           contentwidth;
    double                           contentheight;

    UI_LAYOUTBOX_EDGES                padding;
    UI_LAYOUTBOX_EDGES                border;
    UI_LAYOUTBOX_EDGES                margin;

    UI_POSITION                       position;
    UI_LAYOUTBOX_INSETS                insets;

    UI_LAYOUTBOX*                     parent;            // borrowed -- never owned, never deleted by this class
    XVECTOR<UI_LAYOUTBOX*>            children;          // owned
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
