/**-------------------------------------------------------------------------------------------------------------------
*
* @file       UI_StyleSheet.h
*
* @class      UI_STYLESHEET
* @brief      User Interface : parsed CSS stylesheet (rule set + selector-based resolver over UI_STYLE bag).
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
* SINGLE PURPOSE
*   In-memory representation of a parsed CSS stylesheet, restricted to the subset that the UI module can already
*   consume through UI_STYLE. A rule is a (selector, declaration-block) pair. Declarations are stored as a
*   plain UI_STYLE bag so that Resolve() just copies key/value pairs on top of a target UI_STYLE, mirroring what
*   the XML front-end already does via FillFromXMLElement().
*
*   The supported selector grammar is deliberately minimal, aligned with GEN's widget model rather than with a
*   full CSS 2.1 engine:
*
*     Selector      := Universal | Compound ("," Compound)*
*     Universal     := "*"
*     Compound      := TypeSelector? ("#" Id)? ("." Class)* (":" Pseudo)*    // at least one of the parts
*     TypeSelector  := IdentifierMatchingUI_ELEMENT_TYPE_STRING
*
*   No descendant/child combinators, no attribute selectors, no @media, no @import. Pseudo-classes are
*   syntactically accepted (parsed into UI_CSSSELECTOR::pseudos) but only ":root" is honoured in this step,
*   as the anchor for CSS custom properties (theme variables). Any other pseudo-class parses cleanly, adds
*   10 to the selector's specificity (parity with regular classes), and renders the selector unmatchable at
*   Resolve() time -- so rules like "button:hover" are silently inert until state-based cascade is wired in
*   a later step. Specificity: id=100, class/pseudo=10, type=1.
*
* THEME VARIABLES (":root" + "var(--name[, fallback])")
*   Any declaration block whose selector is exactly ":root" is intercepted by the parser and its declarations
*   are moved into UI_STYLESHEET::variables instead of becoming a regular rule. The stylesheet then walks
*   every remaining declaration value looking for "var(--name)" / "var(--name, fallback)" tokens and
*   substitutes them in place. Substitution runs at parse time (once), so Resolve() at load time sees fully
*   expanded values and never re-parses anything. Variables can reference other variables (up to a small
*   fixed number of expansion passes; deep recursion is not supported and is not intended to be).
*
*   Cascade ordering, when several rules match one element: apply in ascending specificity, ties broken by
*   source order (later rule wins). Higher-specificity rules therefore always override lower ones, and equal
*   specificity resolves in the order the author wrote them - exactly what CSS does for this subset.
*
* @author     Abraham J. Velez / EndoraSoft
*
* ---------------------------------------------------------------------------------------------------------------------*/

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XString.h"
#include "XVector.h"

#include "UI_Style.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


/**
* @brief  One parsed compound selector (single alternative inside a comma-separated list). Empty type/id/class
*         lists mean "unrestricted along that axis". A UI_CSSSELECTOR with all three empty is the universal
*         selector ("*") and matches every element.
*/
class UI_CSSSELECTOR
{
  public:
                                    UI_CSSSELECTOR              ();
    virtual                        ~UI_CSSSELECTOR              ();

    XSTRING&                        GetType                     ()    { return type;    }
    XSTRING&                        GetID                       ()    { return id;      }
    XVECTOR<XSTRING*>&              GetClasses                  ()    { return classes; }
    XVECTOR<XSTRING*>&              GetPseudos                  ()    { return pseudos; }

    void                            AddClass                    (XCHAR* classname);
    void                            AddPseudo                   (XCHAR* pseudoname);

    // Precomputed CSS-style specificity: id * 100 + (class + pseudo) * 10 + type * 1. Universal selector = 0.
    int                             GetSpecificity              () const { return specificity; }
    void                            RecomputeSpecificity        ();

    // True iff this selector is exactly ":root" (no type, no id, no classes, single pseudo "root"). Used by
    // the parser to intercept ":root { ... }" blocks and move their declarations to the stylesheet's
    // variable table instead of treating them as a normal rule.
    bool                            IsRootOnly                  ();

    // True iff this selector carries at least one pseudo-class (other than ":root", which is intercepted at
    // parse time and never reaches selectors). Used by the loader as a cheap "does this rule depend on state?"
    // discriminator to decide whether an element must subscribe to state re-resolution.
    bool                            HasPseudos                  ()    { return pseudos.GetSize() > 0; }

    // Matches (element_type, element_id, element_classlist, active-pseudos) against this selector. Comparisons
    // are case-insensitive to keep parity with the XML attribute lookup semantics used elsewhere in the UI
    // module. Every pseudo required by the selector must be present in `activepseudos`. Passing an empty
    // `activepseudos` therefore behaves as "no state active" (load-time resolution), and selectors carrying
    // pseudos will not match -- reproducing step 2's behaviour without a code path change.
    bool                            Match                       (XSTRING& elementtype, XSTRING& elementid, XVECTOR<XSTRING*>& elementclasses, XVECTOR<XSTRING*>& activepseudos);

  private:

    void                            Clean                       ();

    XSTRING                         type;                 // empty = no type restriction
    XSTRING                         id;                   // empty = no id restriction
    XVECTOR<XSTRING*>               classes;              // empty = no class restriction; owned strings
    XVECTOR<XSTRING*>               pseudos;              // empty = no pseudo restriction; owned strings
    int                             specificity;
};


/**
* @brief  A CSS rule: one or more compound selectors sharing the same declaration block. The declarations are
*         held as a plain UI_STYLE bag; applying the rule to an element is a straight-forward key-by-key
*         Set() into that element's own UI_STYLE.
*/
class UI_CSSRULE
{
  public:
                                    UI_CSSRULE                  ();
    virtual                        ~UI_CSSRULE                  ();

    XVECTOR<UI_CSSSELECTOR*>&       GetSelectors                ()    { return selectors;    }
    UI_STYLE&                       GetDeclarations             ()    { return declarations; }

    // Source order index (assigned by the parser). Used to break specificity ties in the cascade: later wins.
    int                             GetSourceIndex              () const { return sourceindex; }
    void                            SetSourceIndex              (int idx)                     { sourceindex = idx; }

  private:

    void                            Clean                       ();

    XVECTOR<UI_CSSSELECTOR*>        selectors;            // owned
    UI_STYLE                        declarations;
    int                             sourceindex;
};


/**
* @brief  A parsed stylesheet: an ordered list of rules plus a cascade resolver. The parser (UI_CSSPARSER) is
*         the only writer; the UI_MANAGER is the only consumer (via UI_STYLE::FillFromCSSDeclarations()).
*/
class UI_STYLESHEET
{
  public:
                                    UI_STYLESHEET               ();
    virtual                        ~UI_STYLESHEET               ();

    bool                            Rules_Add                   (UI_CSSRULE* rule);
    XVECTOR<UI_CSSRULE*>*           Rules_GetAll                ()    { return &rules; }
    int                             Rules_Count                 ()       { return (int)rules.GetSize(); }
    void                            Rules_DeleteAll             ();

    // --- Theme variables (CSS custom properties: "--name" keyed) ----------------------------------------------
    // The parser routes every ":root { --name: value; }" declaration into this bag (later assignments overwrite
    // earlier ones, matching CSS cascade semantics). Consumers should not write here directly; ExpandVariables()
    // is what folds these values into rule declarations. Exposed public so the parser can populate it.
    UI_STYLE&                       Variables                   ()    { return variables; }
    int                             Variables_Count             ()       { return variables.GetProperties() ? (int)variables.GetProperties()->GetSize() : 0; }
    bool                            Variables_Get               (XCHAR* name, XSTRING& out);

    // One-shot substitution of every "var(--name[, fallback])" token embedded in a rule's declaration value.
    // Called once by the parser at end-of-parse; safe to call again if variables were amended by hand.
    // Also self-expands variable-to-variable references (up to a small fixed number of iterations, so a
    // circular reference stabilizes at the last resolved form rather than looping forever).
    void                            ExpandVariables             ();

    // Cascade resolver: writes into `out` every declaration whose rule matches (elementtype, elementid,
    // elementclasses, active-pseudos). Rules are applied in ascending specificity, ties by source order, so on
    // return `out` holds the effective per-property value for this element. Existing keys in `out` are
    // overwritten by any matching rule (CSS-wins-over-XML semantics, when the caller has pre-filled `out`
    // from the XML front-end). Pass an empty `activepseudos` for load-time / stateless resolution.
    bool                            Resolve                     (XSTRING& elementtype, XSTRING& elementid, XVECTOR<XSTRING*>& elementclasses, XVECTOR<XSTRING*>& activepseudos, UI_STYLE& out);

    // Cheap "does any rule in this sheet require pseudo-classes to match a given element?" query. Used by the
    // loader to decide whether an element must snapshot its baseline visual state and subscribe to state
    // re-resolution, or can skip that overhead entirely.
    bool                            HasPseudoRulesFor           (XSTRING& elementtype, XSTRING& elementid, XVECTOR<XSTRING*>& elementclasses);

  private:

    void                            Clean                       ();

    // Substitute every var(--name[, fallback]) occurrence in `in` and place the result in `out`. Returns true
    // if at least one substitution ran; false if `in` contained no var() references at all. Missing variables
    // without fallback expand to empty string (matches CSS behaviour of "invalid at computed-value time" for
    // our simple string-typed values).
    bool                            SubstituteVars              (XSTRING& in, XSTRING& out);

    XVECTOR<UI_CSSRULE*>            rules;                // owned, in source order
    UI_STYLE                        variables;            // ":root" declarations, keyed by "--name"
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
