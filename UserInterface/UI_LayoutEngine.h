/**-------------------------------------------------------------------------------------------------------------------
*
* @file       UI_LayoutEngine.h
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
* SCOPE (Phase 3 -- "caja, unidades y posicionamiento", fourth increment, first sub-step: "pequeños primero")
*   The first sub-step of the real layout engine: BuildTree() walks a UI_ELEMENT tree (father/compose_elements,
*   see UI_Element.h) and produces the EQUIVALENT UI_LAYOUTBOX tree, one box per element, populated from that
*   element's ALREADY-RESOLVED legacy geometry -- exactly what UI_SKINCANVAS::CalculePosition() and the padding/
*   border/margin setters (UI_Manager.cpp's builders) already computed and stored on each UI_ELEMENT today.
*
*   Deliberately still NOT a layout algorithm:
*
*     - it reads, it does not compute. Content box geometry comes straight from UI_CSSBox_Get() (already
*       convention-independent -- see UI_CSSAdapter.h); padding/border/margin come straight from
*       UI_ELEMENT::GetPadding()/GetBorderWidth()/GetMargin(). Nothing here resolves a percentage, applies
*       min/max, or interprets box-sizing -- those are exactly the kind of thing the SUBSEQUENT sub-steps
*       (position:relative/absolute + insets, then the real UI_CSSBox_Set() wiring) will start doing, once
*       there is a reason to diverge from what CalculePosition() already produced.
*     - it does not modify "root" or any descendant -- a read-only snapshot, same promise UI_LAYOUTBOX itself
*       already makes about not touching UI_ELEMENT/UI_CSSAdapter.h (see UI_LayoutBox.h's own SCOPE comment).
*     - UI_ELEMENT::GetBorderWidth() can return -1.0 ("use the skin's default width" -- a PAINTING concern, not
*       a geometry one). BuildTree() treats that the same as "no border for layout purposes" (0.0) rather than
*       guessing at a skin default it has no way to know; UI_LAYOUTBOX::SetBorder() would clamp a literal -1.0
*       to 0.0 anyway (see UI_LayoutBox.h), so this is spelled out here rather than left as an accidental
*       side effect of that clamp.
*
*   This exists so the position/insets sub-step (next) has a tree to work FROM instead of reinventing tree
*   traversal at the same time as positioning math.
*
* SCOPE ADDENDUM (fourth increment, second sub-step: "position:relative/absolute + insets")
*   ApplyPositioning() walks an ALREADY-BUILT UI_LAYOUTBOX tree (e.g. BuildTree()'s output, or one a test built
*   by hand) and, for every box whose UI_LAYOUTBOX::GetPosition() is not STATIC, moves its content box per CSS:
*
*     - RELATIVE: offset from the box's OWN current (static-flow) position. Horizontal offset is "left" if
*       specified, else "-right" if specified, else 0 (never both -- CSS says a RELATIVE box with both left and
*       right specified and no explicit width uses left; here both is disambiguated the same way: left wins).
*       Vertical is the analogous top/-bottom/0. Size is never touched, only position.
*     - ABSOLUTE: positioned against the PADDING box (UI_LAYOUTBOX::GetPaddingBox()) of its nearest ancestor
*       whose position is not STATIC, or the tree root's padding box if there is no such ancestor (a documented
*       simplification standing in for CSS's "initial containing block" -- there is no separate viewport object
*       at this layer). If "left" is specified: newleft = cbleft + left. Else if "right" is specified:
*       newleft = cbleft + cbwidth - right - boxwidth. Else: the box's current left is left untouched (CSS's
*       "auto on both" falls back to the box's static position; this engine has no separate static-position pass
*       yet, so "leave it where BuildTree()/the caller already put it" stands in for that fallback). Vertical is
*       the analogous top/bottom/cbheight/boxheight.
*
*   Still deliberately incomplete:
*
*     - does not read UI_COMPUTEDSTYLE's "position"/"top"/"right"/"bottom"/"left" properties -- callers set
*       UI_LAYOUTBOX::SetPosition()/SetInsets() directly (by hand in tests, exactly like every UI_LAYOUTBOX field
*       before it).
*     - position:fixed and position:sticky are not modeled (no UI_POSITION value for either yet).
*     - "auto" margins used for centering an ABSOLUTE box are not special-cased -- out of scope here, exactly
*       like every other unresolved-percentage/min-max concern this file has deferred since sub-step one.
*
* SCOPE ADDENDUM (fourth increment, third and last sub-step: "cableado a UI_CSSBox_Set(), legacy/CSS seleccionables")
*   RunLayout() is the real entry point the roadmap asked for: "UI_CSSBox_Set() pasa de puente aditivo sin
*   consumidor a punto de entrada real del nuevo motor", with both strategies (legacy XML layout / CSS layout)
*   selectable by the caller through UI_LAYOUTSTRATEGY, exactly as asked -- and nothing else:
*
*     - UI_LAYOUTSTRATEGY_LEGACY is a pure no-op: whatever CalculePosition() (or any other existing caller) has
*       already written into "root" and its descendants is left completely untouched. This is what makes the two
*       strategies genuinely "selectable" rather than the CSS path being forced on -- calling RunLayout() with
*       LEGACY anywhere in the existing pipeline changes nothing.
*     - UI_LAYOUTSTRATEGY_CSS runs the pipeline built across this increment's three sub-steps: BuildTree() reads
*       "root"'s current resolved geometry into a UI_LAYOUTBOX tree, ApplyPositioning() repositions the non-STATIC
*       boxes in that tree, and the new WriteBackTree() walks the (element tree, box tree) pair together and
*       drives each element's geometry back through UI_CSSBox_Set() -- the first real consumer that header has
*       ever had. The temporary tree is discarded (GEN_DELETE) once written back; "root" and its descendants keep
*       their own storage (UI_ELEMENT), nothing about UI_ELEMENT's shape changes.
*     - WriteBackTree() pairs each UI_LAYOUTBOX with the UI_ELEMENT it came from by walking BOTH trees in lockstep
*       with the SAME "skip NULL compose_elements entries" rule BuildTree() already uses to build the box tree in
*       the first place -- so the two traversals visit the same elements in the same order without UI_LAYOUTBOX
*       needing to store a back-pointer to UI_ELEMENT (which would break UI_LAYOUTBOX's own "independent of
*       UI_ELEMENT" promise -- see UI_LayoutBox.h's SCOPE comment). WriteBackTree() is exposed as its own public
*       method (not folded into RunLayout()'s body) specifically so it -- and therefore the position-application-
*       and-writeback path -- can be tested directly against a hand-built, already-positioned UI_LAYOUTBOX tree,
*       the same "populate by hand" pattern every UI_LAYOUTBOX-level test in this file already uses.
*
*   Deliberately NOT in scope, and NOT promised by anything above:
*
*     - reading "position"/"top"/"right"/"bottom"/"left" out of a real stylesheet and using that to set
*       UI_LAYOUTBOX::SetPosition()/SetInsets() before BuildTree() hands the tree to ApplyPositioning(). There is
*       currently no per-UI_ELEMENT stored UI_COMPUTEDSTYLE to read at layout time -- GetLayoutElement_Base() in
*       UI_Manager.cpp builds one transiently per XML node, at parse time, and does not keep it attached to the
*       UI_ELEMENT afterwards. Making that happen is a separate structural change (persisting computed style on
*       UI_ELEMENT, or threading it through some other path) that touches code well outside this file, and is
*       left as explicit future work rather than folded into this sub-step under time pressure.
*     - nothing in the existing pipeline (UI_Manager.cpp/UI_SkinCanvas.cpp, all ~30 CalculePosition() call sites)
*       calls RunLayout() yet, with either strategy. This file adds the entry point; deciding WHEN a real
*       widget-building pass should call it with UI_LAYOUTSTRATEGY_CSS is a product decision for a later step,
*       not something to sneak in as a side effect of adding the method.
*
* @author     Abraham J. Velez / EndoraSoft
*
* ---------------------------------------------------------------------------------------------------------------------*/

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "UI_LayoutBox.h"
#include "UI_Element.h"


/**
* @enum   UI_LAYOUTSTRATEGY
* @brief  Which layout strategy RunLayout() should drive "root"'s geometry with.
*/
enum UI_LAYOUTSTRATEGY
{
  UI_LAYOUTSTRATEGY_LEGACY = 0,      // no-op: whatever the existing XML layout (CalculePosition et al.) produced stands
  UI_LAYOUTSTRATEGY_CSS               // BuildTree() + ApplyPositioning() + WriteBackTree(), driven through UI_CSSBox_Set()
};


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


/**
* @brief  UI_LAYOUTENGINE has no instance state today: every method here is a static, pure-logic helper, the same
*         shape UI_PROPERTYREGISTRY already uses (see UI_PropertyRegistry.h) for the same reason -- nothing here
*         needs to persist between calls yet.
*/
class UI_LAYOUTENGINE
{
  public:

    // Builds and returns a new UI_LAYOUTBOX tree mirroring "root"'s UI_ELEMENT subtree (root and every
    // descendant reachable through GetComposeElements(), recursively). NULL if "root" is NULL. The caller
    // owns the returned tree (GEN_DELETE it -- UI_LAYOUTBOX's own destructor recursively frees every child).
    static UI_LAYOUTBOX*           BuildTree                    (UI_ELEMENT* root);

    // Walks "root" and every descendant, moving the content box of each non-STATIC node per CSS position/inset
    // rules (see the SCOPE ADDENDUM above). Ignored if "root" is NULL. Sizes are never touched, only positions.
    static void                    ApplyPositioning              (UI_LAYOUTBOX* root);

    // Walks "root" (a UI_ELEMENT) and "tree" (the UI_LAYOUTBOX BuildTree() built FROM "root", typically after
    // ApplyPositioning() has repositioned it) together, driving each element's geometry from its paired box
    // through UI_CSSBox_Set(). Ignored if either is NULL. Does not touch padding/border/margin on the element
    // (UI_CSSBox_Set() never has -- those remain whatever the legacy builders already set).
    static void                    WriteBackTree                 (UI_ELEMENT* root, UI_LAYOUTBOX* tree);

    // The real entry point: runs "strategy" against "root". LEGACY is a no-op (existing geometry stands as-is);
    // CSS runs BuildTree() -> ApplyPositioning() -> WriteBackTree() and discards the temporary tree. Ignored if
    // "root" is NULL.
    static void                    RunLayout                     (UI_ELEMENT* root, UI_LAYOUTSTRATEGY strategy);

  private:

    static void                    ApplyPositioningRecursive     (UI_LAYOUTBOX* box, UI_LAYOUTBOX* rootbox, UI_LAYOUTBOX* nearestpositionedancestor);
    static void                    ApplyRelativeOffset           (UI_LAYOUTBOX* box);
    static void                    ApplyAbsolutePosition         (UI_LAYOUTBOX* box, UI_LAYOUTBOX* containingblock);
    static void                    WriteBackRecursive             (UI_ELEMENT* element, UI_LAYOUTBOX* box);
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
