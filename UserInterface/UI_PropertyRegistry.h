/**-------------------------------------------------------------------------------------------------------------------
*
* @file       UI_PropertyRegistry.h
*
* @class      UI_PROPERTYREGISTRY
* @brief      User Interface : formal legacy<->CSS property alias table and shared CSS shorthand parsing.
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
* SCOPE (Phase 1 -- "estilo calculado tipado", first increment)
*   Two things that today live as ad-hoc, copy-pasted logic scattered across UI_MANAGER::GetLayoutElement_Base()
*   and UI_ELEMENT::ReapplyStyleVisual() become single, shared, tested entry points here:
*
*   1. The legacy-attribute-name <-> CSS-natural-name alias pattern ("bckgrdcolor"/"background-color",
*      "textalignment"/"text-align"): GetAliased() is the ONE place that decides "first hit wins", so load
*      time and re-style-on-state-change time can never drift apart on which spelling wins a tie.
*
*   2. The CSS 1-to-4-value box shorthand grammar (used today by "padding" and "border-radius", independently
*      re-implemented in each with an identical tokenizer): ExpandCSSShorthand4() is the one implementation,
*      so a third consumer ("margin", see UI_Manager.cpp) does not need a third copy.
*
*   This is deliberately NOT yet the full typed UI_COMPUTEDSTYLE / UI_LENGTH value-object layer described in the
*   UI/CSS analysis report's Phase 1 section -- that is a larger, separately-verified increment. This header is
*   the alias/shorthand groundwork it builds on, extracted now because it was already duplicated and already
*   risked drifting.
*
* @author     Abraham J. Velez / EndoraSoft
*
* ---------------------------------------------------------------------------------------------------------------------*/

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XString.h"

#include "UI_Style.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


/**
* @brief  UI_PROPERTYREGISTRY has no instance state: every member is a static, pure-logic helper, so it can be
*         called from both load-time building (UI_MANAGER::GetLayoutElement_Base()) and re-style-on-state-change
*         (UI_ELEMENT::ReapplyStyleVisual()) without either one owning a copy of the alias table.
*/
class UI_PROPERTYREGISTRY
{
  public:

    // First-hit-wins lookup: tries "primarykey" (the historical GEN attribute name) first, then "secondarykey"
    // (its CSS-natural alias) only if the primary key was absent or resolved to an empty string. Mirrors the
    // exact precedence already documented at each of this pattern's call sites ("bckgrdcolor" before
    // "background-color", "textalignment" before "text-align").
    static bool                    GetAliased                  (UI_STYLE& style, XCHAR* primarykey, XCHAR* secondarykey, XSTRING& value);

    // Tokenizes "raw" on runs of comma/space/tab into up to "maxvalues" numbers (XSTRING::ConvertToDouble() per
    // token). Returns the count actually found (0..maxvalues). Shared by ExpandCSSShorthand4() and by any
    // shorthand that needs its own value count before deciding how to interpret it (see "margin" in
    // UI_Manager.cpp, which keeps its legacy 4-value order but reuses this tokenizer instead of UnFormat()).
    static XDWORD                  TokenizeNumbers              (XSTRING& raw, double* outvalues, XDWORD maxvalues);

    // The CSS "1 to 4 numbers" box shorthand expansion shared by "padding", "border-radius" and the new
    // 1-3 value form of "margin": whatever TokenizeNumbers() finds is expanded into out[0..3] using the
    // standard CSS positional rule --
    //     1 value  : out[0]=out[1]=out[2]=out[3] = A
    //     2 values : out[0]=out[2] = A ;  out[1]=out[3] = B
    //     3 values : out[0] = A ;  out[1]=out[3] = B ;  out[2] = C
    //     4 values : out[0]=A, out[1]=B, out[2]=C, out[3]=D
    // The caller assigns meaning to out[0..3] (TOP/RIGHT/BOTTOM/LEFT for padding/margin; TL/TR/BR/BL for
    // border-radius) -- the expansion arithmetic itself does not know or care which convention it feeds.
    static void                    ExpandCSSShorthand4          (XSTRING& raw, double out[4]);

    // Shared "box-shadow" grammar, extracted so UI_MANAGER::GetLayoutElement_Base() (load time) and
    // UI_ELEMENT::ReapplyStyleVisual() (re-style on pseudo-class state change) parse the exact same subset
    // instead of carrying two independent tokenizers that could drift:
    //     box-shadow: <offset-x> <offset-y> <color>
    //     box-shadow: <offset-x> <offset-y> <blur> <color>
    //   No spread, no inset, no multi-shadow list. Whitespace-separated; the last non-numeric token is the
    //   colour (GEN colours never contain spaces). Returns false (outputs left untouched) unless at least the
    //   two mandatory offsets and a colour were found -- same "only latch on a valid parse" rule the original
    //   inline version used.
    static bool                    ParseBoxShadow               (XSTRING& raw, double& outoffsetx, double& outoffsety, double& outblur, XSTRING& outcolor);

    // Resolves "margin" shorthand + optional margin-top/right/bottom/left longhands into out_lrud[4] =
    // LEFT, RIGHT, UP, DOWN (same slot order UI_ELEMENT::SetMargin uses).
    //   use_css_trbl     : 4-value shorthand is CSS TOP,RIGHT,BOTTOM,LEFT; false keeps historical
    //                      LEFT,RIGHT,UP,DOWN (XML-only layouts / UI_Options).
    //   apply_longhands  : when true, margin-* longhands override the shorthand last (CSS Lite only;
    //                      pass false with use_css_trbl=false so unknown longhand keys cannot perturb
    //                      a legacy layout).
    // Returns true if any margin key was present (caller should SetMargin); false leaves out_lrud untouched.
    static bool                    ResolveMarginEdges           (UI_STYLE& style, bool use_css_trbl, bool apply_longhands, double out_lrud[4]);
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
