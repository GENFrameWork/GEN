/**-------------------------------------------------------------------------------------------------------------------
*
* @file       UI_CSS_UnitTests.cpp
*
* @brief      Unit tests for the GEN CSS Lite subsystem (UI_CSSPARSER / UI_STYLESHEET / UI_CSSSELECTOR / UI_STYLE).
* @ingroup    UNIT_TESTS
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
* SCOPE (Phase 0 -- "contrato, tests y correcciones inmediatas")
*   Freezes the observable behaviour of the CSS-over-XML subsystem described in the UI/CSS gap analysis before
*   any further refactor touches it: selector parsing, specificity, cascade order, pseudo-class matching,
*   ":root" custom properties, @import (including cycle detection), parser error recovery, and the alias
*   fallback pattern ("bckgrdcolor" / "background-color") that both GetLayoutElement_Base() (UI_Manager.cpp)
*   and UI_ELEMENT::ReapplyStyleVisual() rely on. These are pure-logic tests: UI_STYLESHEET::Resolve() takes
*   plain XSTRING/XVECTOR<XSTRING*> element identity, so no XFILEXMLELEMENT/UI_ELEMENT/UI_MANAGER/GRPSCREEN
*   graphics stack is required to exercise the parser and the cascade.
*
* SCOPE (Phase 1 -- "estilo calculado tipado", first increment)
*   Adds coverage for UI_PROPERTYREGISTRY (UI_PropertyRegistry.h/.cpp): the alias lookup GetAliased() extracted
*   from the two call sites above, and the shared CSS 1-to-4-value box shorthand expansion ExpandCSSShorthand4()
*   now used by "padding", "border-radius" and (via its own 1-to-3-value path) "margin" in UI_Manager.cpp.
*
* SCOPE (Phase 1 -- "estilo calculado tipado", second increment)
*   Adds coverage for UI_LENGTH (UI_Length.h/.cpp: NUMBER/PERCENT/KEYWORD classification and PERCENT
*   resolution) and UI_COMPUTEDSTYLE (UI_ComputedStyle.h/.cpp: typed GetLength()/GetColor(), plain and
*   alias-aware). UI_MANAGER::GetLayoutElement_Text() is the first per-widget builder migrated onto
*   UI_COMPUTEDSTYLE (verified by compiling and running UI_System, not by a test here -- these tests exercise
*   the type on its own, the same pure-logic style as the rest of this file).
*
* @author     Abraham J. Velez / EndoraSoft
*
* ---------------------------------------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


#ifdef GOOGLETEST_ACTIVE

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "gtest/gtest.h"

#include "UI_CSSParser.h"
#include "UI_StyleSheet.h"
#include "UI_Style.h"
#include "UI_PropertyRegistry.h"
#include "UI_Length.h"
#include "UI_ComputedStyle.h"
#include "UI_Color.h"
#include "UI_LayoutBox.h"
#include "UI_LayoutEngine.h"
#include "UI_Element.h"
#include "UI_CSSAdapter.h"

#include "XFileTXT.h"
#include "XPath.h"
#include "XString.h"
#include "XVector.h"
#include "XDir.h"
#include "XFactory.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define UI_CSS_UNITTESTS_TEMPDIRNAME      __L("ui_css_unittests_tmp")


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------------------
// Helpers

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         static void UI_CSS_UnitTests_GetTempDir(XPATH& out)
* @brief      Resolve the fixture directory used by @import/ParseFile() tests as an ABSOLUTE path (current
*             working directory + "ui_css_unittests_tmp"). A relative "./..." path was tried first and found
*             unreliable: XLINUXDIR::Make("./...", true) reports success but a follow-up Exist() on the same
*             relative string then reports false, so callers here always work from an absolute path instead
*             of debugging that relative-path corner case, which is out of Phase 0's scope.
* @ingroup    UNIT_TESTS
*
* @param[out] out : Receives the absolute fixture directory path.
*
* --------------------------------------------------------------------------------------------------------------------*/
static void UI_CSS_UnitTests_GetTempDir(XPATH& out)
{
  XDIR* dir = GEN_XFACTORY.Create_Dir();
  if(!dir) return;

  dir->GetActual(out);
  out.Slash_Add();
  out += UI_CSS_UNITTESTS_TEMPDIRNAME;

  GEN_XFACTORY.Delete_Dir(dir);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         static void UI_CSS_UnitTests_EnsureTempDir()
* @brief      Create the fixture directory used by @import/ParseFile() tests (recursive, idempotent -- GEN's
*             XDIR::Make() is a no-op if the directory already exists).
* @ingroup    UNIT_TESTS
*
* --------------------------------------------------------------------------------------------------------------------*/
static void UI_CSS_UnitTests_EnsureTempDir()
{
  XDIR* dir = GEN_XFACTORY.Create_Dir();
  if(!dir) return;

  XPATH temppath;
  UI_CSS_UnitTests_GetTempDir(temppath);

  dir->Make(temppath, true);

  GEN_XFACTORY.Delete_Dir(dir);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         static void UI_CSS_UnitTests_WriteFile(XCHAR* name, XCHAR* content, XPATH& outpath)
* @brief      Write `content` into a temp .css file (created with GEN's own XFILETXT, no STL file I/O) so
*             @import / ParseFile() tests can exercise real disk paths and relative directory resolution.
* @ingroup    UNIT_TESTS
*
* @param[in]  name    : File name (no directory), e.g. "base.css".
* @param[in]  content : Raw text to write, one AddLine() call per '\n'-separated fragment is not required --
*                        a single AddLine() with embedded content is enough for these small fixtures.
* @param[out] outpath : Receives the full path written to.
*
* --------------------------------------------------------------------------------------------------------------------*/
static void UI_CSS_UnitTests_WriteFile(XCHAR* name, XCHAR* content, XPATH& outpath)
{
  UI_CSS_UnitTests_EnsureTempDir();

  XFILETXT* file = GEN_NEW XFILETXT();
  if(!file) return;

  UI_CSS_UnitTests_GetTempDir(outpath);
  outpath.Slash_Add();
  outpath += name;

  // Open(path, isreadonly=false) maps to fopen mode "r+b" (file must already exist) on Linux -- Create() is the
  // correct API for a brand-new fixture file (fopen mode "w+b", creates/truncates).
  if(file->Create(outpath))
    {
      file->AddLine(content);
      file->WriteAllFile();
      file->Close();
    }

  GEN_DELETE file;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         static XSTRING* UI_CSS_UnitTests_NewStr(XCHAR* text)
* @brief      Heap-allocate an XSTRING for the XVECTOR<XSTRING*> lists UI_STYLESHEET::Resolve() expects
*             (elementclasses / activepseudos), matching the ownership pattern UI_ELEMENT::GetActivePseudos()
*             already uses. Callers own the returned pointer and must GEN_DELETE it (or DeleteContents() the
*             vector) once done.
* @ingroup    UNIT_TESTS
*
* @param[in]  text : Value to store.
*
* @return     XSTRING* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
static XSTRING* UI_CSS_UnitTests_NewStr(XCHAR* text)
{
  XSTRING* str = GEN_NEW XSTRING();
  if(str) str->Set(text);

  return str;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         static void UI_CSS_UnitTests_SetElementBox(UI_ELEMENT* element, double left, double top, double width, double height)
* @brief      Sets "element"'s resolved geometry so UI_CSSBox_Get(element) reports exactly (left,top,width,height)
*             in CSS's own top-left convention -- see UI_CSSAdapter.h for why Y needs the +height translation
*             (GEN's internal y_position stores the BOTTOM edge, not the top).
* @ingroup    UNIT_TESTS
*
* @param[in]  element : Element to set up.
* @param[in]  left : Content box left edge.
* @param[in]  top : Content box top edge.
* @param[in]  width : Content box width.
* @param[in]  height : Content box height.
*
* --------------------------------------------------------------------------------------------------------------------*/
static void UI_CSS_UnitTests_SetElementBox(UI_ELEMENT* element, double left, double top, double width, double height)
{
  UI_BOUNDARYLINE boundaryline;
  boundaryline.width  = width;
  boundaryline.height = height;
  element->SetBoundaryLine(boundaryline);

  element->SetXPosition(left);
  element->SetYPosition(top + height);   // internal Y stores the BOTTOM edge
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         static UI_LAYOUTBOX_INSETS UI_CSS_UnitTests_MakeInsets(bool hastop, double top, bool hasright, double right, bool hasbottom, double bottom, bool hasleft, double left)
* @brief      Builds a UI_LAYOUTBOX_INSETS with each edge independently specified-or-not, so tests can spell out
*             exactly the "auto" pattern they mean to exercise instead of relying on struct-literal field order.
* @ingroup    UNIT_TESTS
*
* @param[in]  hastop : Whether the top edge is specified.
* @param[in]  top : Top edge value (ignored if "hastop" is false).
* @param[in]  hasright : Whether the right edge is specified.
* @param[in]  right : Right edge value (ignored if "hasright" is false).
* @param[in]  hasbottom : Whether the bottom edge is specified.
* @param[in]  bottom : Bottom edge value (ignored if "hasbottom" is false).
* @param[in]  hasleft : Whether the left edge is specified.
* @param[in]  left : Left edge value (ignored if "hasleft" is false).
*
* @return     UI_LAYOUTBOX_INSETS : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
static UI_LAYOUTBOX_INSETS UI_CSS_UnitTests_MakeInsets(bool hastop, double top, bool hasright, double right,
                                                         bool hasbottom, double bottom, bool hasleft, double left)
{
  UI_LAYOUTBOX_INSETS insets;

  insets.top.specified    = hastop;
  insets.top.value        = top;
  insets.right.specified  = hasright;
  insets.right.value      = right;
  insets.bottom.specified = hasbottom;
  insets.bottom.value     = bottom;
  insets.left.specified   = hasleft;
  insets.left.value       = left;

  return insets;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @class      UI_CSS_UnitTests_FakeAncestors
* @brief      Phase 2 ("combinadores descendiente/hijo") test double: a fixed ancestor chain (index 0 = closest
*             ancestor, appended outward) with its own type/id/single-class per level, standing in for a real
*             UI_ELEMENT father chain -- so descendant/child combinator matching can be tested the same
*             pure-logic way as the rest of this file, no UI_ELEMENT/UI_MANAGER/GRPSCREEN graphics stack
*             required. See UI_CSSANCESTORPROVIDER's doc comment in UI_StyleSheet.h for the interface itself.
* @note       INTERNAL
* @ingroup    UNIT_TESTS
*
* --------------------------------------------------------------------------------------------------------------------*/
class UI_CSS_UnitTests_FakeAncestors : public UI_CSSANCESTORPROVIDER
{
  public:
    virtual ~UI_CSS_UnitTests_FakeAncestors()
    {
      types.DeleteContents();
      types.DeleteAll();

      ids.DeleteContents();
      ids.DeleteAll();

      for(XDWORD c=0; c<classlists.GetSize(); c++)
        {
          XVECTOR<XSTRING*>* classlist = classlists.Get(c);
          if(classlist)
            {
              classlist->DeleteContents();
              classlist->DeleteAll();
              GEN_DELETE classlist;
            }
        }

      classlists.DeleteAll();
    }

    // Appends one more ancestor, one level further out than whatever was added before.
    void AddAncestor(XCHAR* type, XCHAR* singleclass = NULL)
    {
      types.Add(UI_CSS_UnitTests_NewStr(type));
      ids.Add(UI_CSS_UnitTests_NewStr(__L("")));

      XVECTOR<XSTRING*>* classlist = GEN_NEW XVECTOR<XSTRING*>();
      if(classlist && singleclass) classlist->Add(UI_CSS_UnitTests_NewStr(singleclass));
      classlists.Add(classlist);
    }

    virtual bool GetAncestor(int depth, XSTRING** outtype, XSTRING** outid, XVECTOR<XSTRING*>** outclasses)
    {
      if(depth < 0 || (XDWORD)depth >= types.GetSize()) return false;

      if(outtype)    *outtype    = types.Get(depth);
      if(outid)      *outid      = ids.Get(depth);
      if(outclasses) *outclasses = classlists.Get(depth);

      return true;
    }

  private:

    XVECTOR<XSTRING*>            types;
    XVECTOR<XSTRING*>            ids;
    XVECTOR<XVECTOR<XSTRING*>*>  classlists;
};


//-----------------------------------------------------------------------------------------
// UI_CSSPARSER : selector parsing

TEST(UI_CSSParser, ParsesTypeIdClassAndPseudoOnOneCompoundSelector)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L("button#ok.primary:hover { color: 255,0,0,255; }"));

  EXPECT_TRUE(parser.ParseText(text, sheet));
  ASSERT_EQ(sheet.Rules_Count(), 1);

  UI_CSSRULE* rule = sheet.Rules_GetAll()->Get(0);
  ASSERT_TRUE(rule != NULL);
  ASSERT_EQ((int)rule->GetSelectors().GetSize(), 1);

  UI_CSSSELECTOR* selector = rule->GetSelectors().Get(0);
  ASSERT_TRUE(selector != NULL);

  EXPECT_EQ(selector->GetType().Compare(__L("button"), true), 0);
  EXPECT_EQ(selector->GetID().Compare(__L("ok"), true), 0);
  ASSERT_EQ((int)selector->GetClasses().GetSize(), 1);
  EXPECT_EQ(selector->GetClasses().Get(0)->Compare(__L("primary"), true), 0);
  ASSERT_EQ((int)selector->GetPseudos().GetSize(), 1);
  EXPECT_EQ(selector->GetPseudos().Get(0)->Compare(__L("hover"), true), 0);
}


TEST(UI_CSSParser, ParsesCommaSeparatedSelectorGroup)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L(".card, .panel { color: 1,2,3,255; }"));

  EXPECT_TRUE(parser.ParseText(text, sheet));
  ASSERT_EQ(sheet.Rules_Count(), 1);
  EXPECT_EQ((int)sheet.Rules_GetAll()->Get(0)->GetSelectors().GetSize(), 2);
}


//-----------------------------------------------------------------------------------------
// UI_CSSPARSER::ResolveLineColumn (Phase 2, "diagnóstico con línea real de fichero")

TEST(UI_CSSParser, ResolveLineColumnOnFirstLineReturnsLineOneAndOffsetPlusOneAsColumn)
{
  XSTRING text(__L("button { color: red; }"));
  int     line = -1, col = -1;

  UI_CSSPARSER::ResolveLineColumn(text, 7, line, col);

  EXPECT_EQ(line, 1);
  EXPECT_EQ(col, 8);   // 1-based: offset 7 is the 8th character
}

TEST(UI_CSSParser, ResolveLineColumnCountsEmbeddedNewlines)
{
  // Offsets: "button {" = 0..7, '\n' at 8, line 2 starts at 9: ' '(9) ' '(10) 'c'(11) 'o'(12) ...
  XSTRING text(__L("button {\n  color: red;\n}"));
  int     line = -1, col = -1;

  UI_CSSPARSER::ResolveLineColumn(text, 12, line, col);   // 'o' inside "color" on line 2

  EXPECT_EQ(line, 2);
  EXPECT_EQ(col, 4);   // 1-based column within line 2: linestart=9, offset=12 -> (12-9)+1
}

TEST(UI_CSSParser, ResolveLineColumnClampsAnOffsetPastTheEndOfText)
{
  XSTRING text(__L("a\nb"));
  int     line = -1, col = -1;

  UI_CSSPARSER::ResolveLineColumn(text, 999, line, col);

  EXPECT_EQ(line, 2);   // clamped to text length (3), which is on line 2 ("b")
  EXPECT_EQ(col, 2);
}

TEST(UI_CSSParser, ResolveLineColumnOnEmptyTextIsLineOneColumnOne)
{
  XSTRING text(__L(""));
  int     line = -1, col = -1;

  UI_CSSPARSER::ResolveLineColumn(text, 0, line, col);

  EXPECT_EQ(line, 1);
  EXPECT_EQ(col, 1);
}


//-----------------------------------------------------------------------------------------
// UI_CSSPARSER : error recovery across real multi-line source (Phase 2 regression guard -- the line-preserving
// flatten in ParseFile() must not change WHAT parses, only what a diagnostic reports)

TEST(UI_CSSParser, RecoversFromAMalformedRuleAcrossEmbeddedNewlinesJustLikeOnOneLine)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  // ".broken" has no ':' before ';' -- ReadDeclarationBlock() discards the declaration and keeps parsing the
  // block; ".ok" is a separate, well-formed rule that must still come through untouched.
  text.Set(__L(".broken {\n  colorred;\n}\n.ok {\n  color: red;\n}"));

  EXPECT_TRUE(parser.ParseText(text, sheet));
  ASSERT_EQ(sheet.Rules_Count(), 1);

  UI_CSSSELECTOR* selector = sheet.Rules_GetAll()->Get(0)->GetSelectors().Get(0);
  ASSERT_TRUE(selector != NULL);
  EXPECT_EQ((int)selector->GetClasses().GetSize(), 1);
  EXPECT_EQ(selector->GetClasses().Get(0)->Compare(__L("ok"), true), 0);
}


//-----------------------------------------------------------------------------------------
// UI_CSSPARSER : error recovery (a malformed rule/declaration must not poison the rest of the sheet)

TEST(UI_CSSParser, SkipsUnsupportedAtRuleAndKeepsParsingFollowingRules)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L("@media (min-width: 100) { .x { color: red; } } .ok { color: 1,2,3,255; }"));

  EXPECT_TRUE(parser.ParseText(text, sheet));
  ASSERT_EQ(sheet.Rules_Count(), 1);
  EXPECT_EQ(sheet.Rules_GetAll()->Get(0)->GetSelectors().Get(0)->GetClasses().Get(0)->Compare(__L("ok"), true), 0);
}


TEST(UI_CSSParser, DiscardsDeclarationWithoutColonButKeepsValidOnesInSameBlock)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;
  XSTRING        value;

  text.Set(__L(".a { colorred; color: 4,5,6,255; }"));

  EXPECT_TRUE(parser.ParseText(text, sheet));
  ASSERT_EQ(sheet.Rules_Count(), 1);
  EXPECT_TRUE(sheet.Rules_GetAll()->Get(0)->GetDeclarations().Get(__L("color"), value));
  EXPECT_EQ(value.Compare(__L("4,5,6,255"), true), 0);
}


TEST(UI_CSSParser, LastIdWinsWhenACompoundSelectorRepeatsHash)
{
  // Documented parser behaviour: a second "#id" fragment inside the same compound selector overwrites the
  // first one instead of being rejected.
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L("#first#second { color: red; }"));

  EXPECT_TRUE(parser.ParseText(text, sheet));
  ASSERT_EQ(sheet.Rules_Count(), 1);
  EXPECT_EQ(sheet.Rules_GetAll()->Get(0)->GetSelectors().Get(0)->GetID().Compare(__L("second"), true), 0);
}


//-----------------------------------------------------------------------------------------
// Phase 2 ("lexer con tokens/strings/escapes reales"): quoted values/URLs and escaped selector fragments.

TEST(UI_CSSParser, DeclarationValueKeepsASemicolonEmbeddedInsideAQuotedString)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;
  XSTRING        value;

  // Before this increment, the value scan stopped at the FIRST ';' it saw, quoted or not -- truncating this to
  // '"A' and leaving '; B"' to be misread as a second (malformed) declaration. The lexer's string-aware scan
  // must treat the whole quoted span as one atomic unit and keep the value intact.
  text.Set(__L(".a { font-family: \"A; B\"; color: red; }"));

  EXPECT_TRUE(parser.ParseText(text, sheet));
  ASSERT_EQ(sheet.Rules_Count(), 1);

  ASSERT_TRUE(sheet.Rules_GetAll()->Get(0)->GetDeclarations().Get(__L("font-family"), value));
  EXPECT_EQ(value.Compare(__L("\"A; B\""), true), 0);

  // The following declaration in the same block must still parse normally.
  ASSERT_TRUE(sheet.Rules_GetAll()->Get(0)->GetDeclarations().Get(__L("color"), value));
  EXPECT_EQ(value.Compare(__L("red"), true), 0);
}


TEST(UI_CSSParser, DeclarationValueKeepsAClosingBraceEmbeddedInsideAQuotedString)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;
  XSTRING        value;

  // Same gap as above but with '}' instead of ';': without string-awareness this would end the whole
  // declaration BLOCK early, at the quoted '}', discarding "color" below and corrupting the rest of the parse.
  text.Set(__L(".a { content: \"x}y\"; color: blue; }"));

  EXPECT_TRUE(parser.ParseText(text, sheet));
  ASSERT_EQ(sheet.Rules_Count(), 1);

  ASSERT_TRUE(sheet.Rules_GetAll()->Get(0)->GetDeclarations().Get(__L("content"), value));
  EXPECT_EQ(value.Compare(__L("\"x}y\""), true), 0);

  ASSERT_TRUE(sheet.Rules_GetAll()->Get(0)->GetDeclarations().Get(__L("color"), value));
  EXPECT_EQ(value.Compare(__L("blue"), true), 0);
}


TEST(UI_CSSParser, SkipToNextRuleResyncsPastAClosingBraceEmbeddedInsideAnUnsupportedAtRulesQuotedString)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  // "@media" is outside GEN's CSS subset, so ParseText() discards it via SkipToNextRule() -- called starting
  // right at the '@', not at end-of-text (unlike the malformed-rule path, which already has its own quote-aware
  // scan by the time it would ever reach SkipToNextRule()). The at-rule's quoted argument contains a '}': a
  // non-quote-aware scan stops right there -- INSIDE the string, before the at-rule's own block even starts --
  // and resumes mid-string ('y" { color: red; } .ok { color: blue; }'), which then gets misread as a spurious
  // EXTRA rule (garbage selector text "y\" ", but a syntactically valid "{ color: red; }" body) ahead of ".ok",
  // so the sheet ends up with 2 rules instead of 1. SkipToNextRule() must skip the quoted span atomically and
  // resync on the at-rule's OWN (real, unquoted) closing brace instead.
  text.Set(__L("@media \"x}y\" { color: red; } .ok { color: blue; }"));

  EXPECT_TRUE(parser.ParseText(text, sheet));
  ASSERT_EQ(sheet.Rules_Count(), 1);

  UI_CSSSELECTOR* selector = sheet.Rules_GetAll()->Get(0)->GetSelectors().Get(0);
  ASSERT_TRUE(selector != NULL);
  EXPECT_EQ((int)selector->GetClasses().GetSize(), 1);
  EXPECT_EQ(selector->GetClasses().Get(0)->Compare(__L("ok"), true), 0);
}


TEST(UI_CSSParser, BackslashEscapeInAClassNameEmbedsALiteralDelimiterCharacter)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  // ".foo\.bar" names a class literally containing a dot -- the escaped '.' must NOT be read as the start of a
  // second class fragment.
  text.Set(__L(".foo\\.bar { color: red; }"));

  EXPECT_TRUE(parser.ParseText(text, sheet));
  ASSERT_EQ(sheet.Rules_Count(), 1);

  UI_CSSSELECTOR* selector = sheet.Rules_GetAll()->Get(0)->GetSelectors().Get(0);
  ASSERT_TRUE(selector != NULL);
  ASSERT_EQ((int)selector->GetClasses().GetSize(), 1);
  EXPECT_EQ(selector->GetClasses().Get(0)->Compare(__L("foo.bar"), true), 0);
}


TEST(UI_CSSParser, ImportURLDecodesABackslashEscapeInsideTheQuotedString)
{
  // The imported FILE is a normal "lexer_escape_theme.css" on disk; the @import line names it as
  // "lexer_escape_th\eme.css" -- an escaped 'e' decodes to a literal 'e' (escaping a character that has no
  // special meaning is a no-op other than dropping the backslash), so this exercises ReadStringLiteral()'s
  // escape decoding on a real, filesystem-portable path: if the backslash were NOT decoded away, the importer
  // would look for a file literally named "lexer_escape_th\eme.css" and fail to find it.
  XPATH themepath;
  UI_CSS_UnitTests_WriteFile(__L("lexer_escape_theme.css"), __L(".imported { color: 8,8,8,255; }"), themepath);

  XSTRING basecontent = __L("@import \"lexer_escape_th\\eme.css\";");

  XPATH basepath;
  UI_CSS_UnitTests_WriteFile(__L("lexer_escape_base.css"), basecontent.Get(), basepath);

  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  ASSERT_TRUE(parser.ParseFile(basepath, sheet));
  ASSERT_EQ(sheet.Rules_Count(), 1);

  XSTRING              elementtype; elementtype.Set(__L("form"));
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("imported")));

  EXPECT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out));

  XSTRING color;
  ASSERT_TRUE(out.Get(__L("color"), color));
  EXPECT_EQ(color.Compare(__L("8,8,8,255"), true), 0);

  elementclasses.DeleteContents();
}


//-----------------------------------------------------------------------------------------
// Phase 2 ("combinadores descendiente/hijo"): descendant (" ") / child (">") combinator matching.

TEST(UI_CSSParser, DescendantCombinatorMatchesAnAncestorAtAnyDepthNotJustTheImmediateParent)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L("form .a { color: red; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  UI_CSS_UnitTests_FakeAncestors ancestors;
  ancestors.AddAncestor(__L("div"));    // depth 0: immediate parent, does NOT match "form"
  ancestors.AddAncestor(__L("form"));   // depth 1: grandparent, DOES match "form"

  XSTRING              elementtype; elementtype.Set(__L("button"));
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("a")));

  EXPECT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out, &ancestors));

  XSTRING color;
  ASSERT_TRUE(out.Get(__L("color"), color));
  EXPECT_EQ(color.Compare(__L("red"), true), 0);

  elementclasses.DeleteContents();
}


TEST(UI_CSSParser, DescendantCombinatorFailsWhenNoAncestorMatchesAtAll)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L("form .a { color: red; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  UI_CSS_UnitTests_FakeAncestors ancestors;
  ancestors.AddAncestor(__L("div"));   // the only ancestor, and it is not "form"

  XSTRING              elementtype; elementtype.Set(__L("button"));
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("a")));

  EXPECT_FALSE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out, &ancestors));

  elementclasses.DeleteContents();
}


TEST(UI_CSSParser, ChildCombinatorMatchesWhenTheImmediateParentIsExactlyRight)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L("form > .a { color: green; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  UI_CSS_UnitTests_FakeAncestors ancestors;
  ancestors.AddAncestor(__L("form"));   // depth 0: immediate parent, matches

  XSTRING              elementtype; elementtype.Set(__L("button"));
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("a")));

  EXPECT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out, &ancestors));

  XSTRING color;
  ASSERT_TRUE(out.Get(__L("color"), color));
  EXPECT_EQ(color.Compare(__L("green"), true), 0);

  elementclasses.DeleteContents();
}


TEST(UI_CSSParser, ChildCombinatorDoesNotMatchAGrandparentEvenThoughDescendantWould)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  // Same ancestor shape as DescendantCombinatorMatchesAnAncestorAtAnyDepthNotJustTheImmediateParent, but with
  // '>' instead of a plain space: the child combinator must reject it, since "form" is the GRANDPARENT here,
  // not the immediate parent ("div" is).
  text.Set(__L("form > .a { color: red; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  UI_CSS_UnitTests_FakeAncestors ancestors;
  ancestors.AddAncestor(__L("div"));
  ancestors.AddAncestor(__L("form"));

  XSTRING              elementtype; elementtype.Set(__L("button"));
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("a")));

  EXPECT_FALSE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out, &ancestors));

  elementclasses.DeleteContents();
}


TEST(UI_CSSParser, ACombinatorSelectorNeverMatchesWhenNoAncestorProviderIsGiven)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  // The target element's OWN identity (class "a") matches the subject compound fine; without an ancestor
  // provider (the default, NULL) the ancestor requirement can never be verified, so the whole selector must be
  // treated as non-matching -- conservative failure, never a false positive.
  text.Set(__L("form .a { color: red; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  XSTRING              elementtype; elementtype.Set(__L("button"));
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("a")));

  EXPECT_FALSE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out));

  elementclasses.DeleteContents();
}


TEST(UI_CSSParser, TwoHopCombinatorChainMatchesWhenEachStepMatchesItsOwnAncestor)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  // "form .a > .b": subject is ".b"; its IMMEDIATE parent must match ".a" (child combinator); some ancestor
  // ABOVE that must match "form" (descendant combinator, any depth).
  text.Set(__L("form .a > .b { color: purple; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  UI_CSS_UnitTests_FakeAncestors ancestors;
  ancestors.AddAncestor(__L("div"), __L("a"));   // depth 0: immediate parent, matches ".a" (child step)
  ancestors.AddAncestor(__L("section"));         // depth 1: does not match "form" yet
  ancestors.AddAncestor(__L("form"));            // depth 2: matches "form" (descendant step)

  XSTRING              elementtype; elementtype.Set(__L("span"));
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("b")));

  EXPECT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out, &ancestors));

  XSTRING color;
  ASSERT_TRUE(out.Get(__L("color"), color));
  EXPECT_EQ(color.Compare(__L("purple"), true), 0);

  elementclasses.DeleteContents();
}


TEST(UI_CSSParser, TwoHopCombinatorChainFailsWhenTheChildStepAloneDoesNotMatch)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  // Same as the success case above, EXCEPT the immediate parent (depth 0) does not carry class "a" -- the
  // child combinator does not backtrack/search further out, so this must fail even though "form" IS present
  // higher up the chain.
  text.Set(__L("form .a > .b { color: purple; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  UI_CSS_UnitTests_FakeAncestors ancestors;
  ancestors.AddAncestor(__L("div"));      // depth 0: immediate parent, does NOT carry class "a"
  ancestors.AddAncestor(__L("section"));
  ancestors.AddAncestor(__L("form"));

  XSTRING              elementtype; elementtype.Set(__L("span"));
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("b")));

  EXPECT_FALSE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out, &ancestors));

  elementclasses.DeleteContents();
}


TEST(UI_CSSSelector, CombinatorSequenceSpecificityIsTheSumOfEveryCompoundsOwnSpecificity)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  // "form"=type(1) + ".a"=class(10) + ".b"=class(10) -- real CSS sums specificity across the whole sequence,
  // not just the subject.
  text.Set(__L("form .a > .b { color: red; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  ASSERT_EQ(sheet.Rules_Count(), 1);

  UI_CSSSELECTOR* selector = sheet.Rules_GetAll()->Get(0)->GetSelectors().Get(0);
  ASSERT_TRUE(selector != NULL);
  EXPECT_EQ(selector->GetSpecificity(), 21);
}


TEST(UI_CSSParser, CombinatorFreeSelectorIsUnaffectedByCombinatorSupport)
{
  // Regression guard: a plain, single-compound selector must still parse and match EXACTLY as before -- no
  // ancestor steps attached, no ancestor provider needed at all.
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L(".a { color: red; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  ASSERT_EQ(sheet.Rules_Count(), 1);

  UI_CSSSELECTOR* selector = sheet.Rules_GetAll()->Get(0)->GetSelectors().Get(0);
  ASSERT_TRUE(selector != NULL);
  EXPECT_FALSE(selector->HasAncestorSteps());
  EXPECT_EQ((int)selector->GetAncestorSteps().GetSize(), 0);

  XSTRING              elementtype;
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("a")));

  EXPECT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out));

  XSTRING color;
  ASSERT_TRUE(out.Get(__L("color"), color));
  EXPECT_EQ(color.Compare(__L("red"), true), 0);

  elementclasses.DeleteContents();
}


//-----------------------------------------------------------------------------------------
// UI_CSSSELECTOR : specificity weights (id=100, class/pseudo=10 each, type=1)

TEST(UI_CSSSelector, SpecificityMatchesDocumentedWeights)
{
  UI_CSSSELECTOR selector;

  selector.GetType().Set(__L("button"));
  selector.GetID().Set(__L("myid"));
  selector.AddClass(__L("primary"));
  selector.AddPseudo(__L("hover"));
  selector.RecomputeSpecificity();

  EXPECT_EQ(selector.GetSpecificity(), 100 + 1 + 10 + 10);
}


TEST(UI_CSSSelector, UniversalSelectorHasZeroSpecificity)
{
  UI_CSSSELECTOR selector;
  selector.RecomputeSpecificity();

  EXPECT_EQ(selector.GetSpecificity(), 0);
}


//-----------------------------------------------------------------------------------------
// UI_STYLESHEET::Resolve() : cascade order (specificity, then source order on ties) and pseudo matching

TEST(UI_StyleSheet, HigherSpecificityIdBeatsLowerSpecificityClass)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L(".card { color: 1,1,1,255; }  #special { color: 2,2,2,255; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  XSTRING              elementtype;  elementtype.Set(__L("form"));
  XSTRING              elementid;    elementid.Set(__L("special"));
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("card")));

  EXPECT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out));

  XSTRING color;
  ASSERT_TRUE(out.Get(__L("color"), color));
  EXPECT_EQ(color.Compare(__L("2,2,2,255"), true), 0);

  elementclasses.DeleteContents();
}


TEST(UI_StyleSheet, EqualSpecificityTieBrokenBySourceOrderLaterWins)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L(".a { color: 1,1,1,255; }  .a { color: 2,2,2,255; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  XSTRING              elementtype;  elementtype.Set(__L("form"));
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("a")));

  EXPECT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out));

  XSTRING color;
  ASSERT_TRUE(out.Get(__L("color"), color));
  EXPECT_EQ(color.Compare(__L("2,2,2,255"), true), 0);

  elementclasses.DeleteContents();
}


TEST(UI_StyleSheet, PseudoRuleOnlyMatchesWhenThePseudoIsActive)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L("button.nav-hit:hover { background-color: 255,255,255,6; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  XSTRING              elementtype; elementtype.Set(__L("button"));
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             outnothover;
  UI_STYLE             outhover;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("nav-hit")));

  // No active pseudo: the rule must NOT match.
  EXPECT_FALSE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, outnothover));
  XSTRING dummy;
  EXPECT_FALSE(outnothover.Get(__L("background-color"), dummy));

  // ":hover" active: the rule DOES match. This is a direct regression test for the shipped dashboard.css
  // "button.nav-hit:hover { background-color: ... }" rule -- see UI_ELEMENT::ReapplyStyleVisual() Phase 0 fix.
  activepseudos.Add(UI_CSS_UnitTests_NewStr(__L("hover")));
  EXPECT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, outhover));

  XSTRING background;
  ASSERT_TRUE(outhover.Get(__L("background-color"), background));
  EXPECT_EQ(background.Compare(__L("255,255,255,6"), true), 0);

  elementclasses.DeleteContents();
  activepseudos.DeleteContents();
}


//-----------------------------------------------------------------------------------------
// Phase 2 ("índice de reglas por id/tipo/clase"): CollectCandidateRules() must be a pure performance change --
// every combination below exercises one axis of the type/id/class index (plus the "unrestricted" bucket for
// selectors that constrain none of them) and asserts Resolve()/HasPseudoRulesFor() still behave exactly as the
// pre-index full scan did.

TEST(UI_StyleSheet, UniversalSelectorRuleMatchesAnyElementViaTheUnrestrictedBucket)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L("* { color: 3,3,3,255; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  XSTRING              elementtype; elementtype.Set(__L("button"));
  XSTRING              elementid;   elementid.Set(__L("whatever"));
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("some-class")));

  EXPECT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out));

  XSTRING color;
  ASSERT_TRUE(out.Get(__L("color"), color));
  EXPECT_EQ(color.Compare(__L("3,3,3,255"), true), 0);

  elementclasses.DeleteContents();
}


TEST(UI_StyleSheet, TypeOnlySelectorMatchesAnElementWithNoIdOrClasses)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L("button { color: 4,4,4,255; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  XSTRING              elementtype; elementtype.Set(__L("button"));
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  EXPECT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out));

  XSTRING color;
  ASSERT_TRUE(out.Get(__L("color"), color));
  EXPECT_EQ(color.Compare(__L("4,4,4,255"), true), 0);

  // A DIFFERENT type must not pick up the type-bucket hit.
  XSTRING otherfromtype; otherfromtype.Set(__L("label"));
  UI_STYLE outother;
  EXPECT_FALSE(sheet.Resolve(otherfromtype, elementid, elementclasses, activepseudos, outother));
}


TEST(UI_StyleSheet, OneRuleWithSelectorsOnDifferentAxesMatchesEachAxisIndependently)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  // Single rule, comma-separated compound selectors landing in different index buckets (id vs class): the id
  // bucket and the class bucket must each independently surface this same UI_CSSRULE as a candidate.
  text.Set(__L("#only-id, .only-class { color: 5,5,5,255; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  XSTRING              elementtype; elementtype.Set(__L("form"));
  XVECTOR<XSTRING*>    activepseudos;

  // Match via the id axis only.
  {
    XSTRING            elementid; elementid.Set(__L("only-id"));
    XVECTOR<XSTRING*>  elementclasses;
    UI_STYLE           out;

    EXPECT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out));

    XSTRING color;
    ASSERT_TRUE(out.Get(__L("color"), color));
    EXPECT_EQ(color.Compare(__L("5,5,5,255"), true), 0);
  }

  // Match via the class axis only.
  {
    XSTRING            elementid;
    XVECTOR<XSTRING*>  elementclasses;
    elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("only-class")));
    UI_STYLE           out;

    EXPECT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out));

    XSTRING color;
    ASSERT_TRUE(out.Get(__L("color"), color));
    EXPECT_EQ(color.Compare(__L("5,5,5,255"), true), 0);

    elementclasses.DeleteContents();
  }

  // Neither axis: no match.
  {
    XSTRING            elementid; elementid.Set(__L("someone-else"));
    XVECTOR<XSTRING*>  elementclasses;
    UI_STYLE           out;

    EXPECT_FALSE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out));
  }
}


TEST(UI_StyleSheet, HasPseudoRulesForFindsABarePseudoSelectorViaTheUnrestrictedBucket)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  // ":hover" alone has no type/id/class, so it can only be found through index_unrestricted.
  text.Set(__L(":hover { color: 6,6,6,255; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  XSTRING              elementtype; elementtype.Set(__L("button"));
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;

  EXPECT_TRUE(sheet.HasPseudoRulesFor(elementtype, elementid, elementclasses));
}


TEST(UI_StyleSheet, HasPseudoRulesForReturnsFalseWhenNoPseudoRuleExistsAtAll)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L("button { color: 7,7,7,255; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  XSTRING              elementtype; elementtype.Set(__L("button"));
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;

  EXPECT_FALSE(sheet.HasPseudoRulesFor(elementtype, elementid, elementclasses));
}


//-----------------------------------------------------------------------------------------
// ":root" custom properties + var(...) expansion

TEST(UI_StyleSheet, RootVariableExpandsIntoDeclarationValue)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L(":root { --accent: 10,20,30,255; }  .x { color: var(--accent); }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  // A ":root"-only block never becomes a regular rule, so Rules_Count() reflects only ".x".
  ASSERT_EQ(sheet.Rules_Count(), 1);

  XSTRING              elementtype;
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("x")));

  ASSERT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out));

  XSTRING color;
  ASSERT_TRUE(out.Get(__L("color"), color));
  EXPECT_EQ(color.Compare(__L("10,20,30,255"), true), 0);

  elementclasses.DeleteContents();
}


TEST(UI_StyleSheet, UndefinedVariableFallsBackToItsFallbackValue)
{
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L(".y { color: var(--missing, 9,9,9,255); }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  XSTRING              elementtype;
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("y")));

  ASSERT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out));

  XSTRING color;
  ASSERT_TRUE(out.Get(__L("color"), color));
  EXPECT_EQ(color.Compare(__L("9,9,9,255"), true), 0);

  elementclasses.DeleteContents();
}


TEST(UI_StyleSheet, StylesheetMadeOnlyOfRootStillCountsAsHavingVariables)
{
  // Documented current behaviour (see gap analysis): a stylesheet with ONLY ":root" declarations has
  // Rules_Count() == 0 (UI_MANAGER::CreateLayouts() would discard it as "empty"), even though its variables
  // ARE populated. This test freezes that behaviour so a future Phase-2 fix is a deliberate, visible change.
  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;
  XSTRING        text;

  text.Set(__L(":root { --accent: 1,2,3,255; }"));
  ASSERT_TRUE(parser.ParseText(text, sheet));

  EXPECT_EQ(sheet.Rules_Count(), 0);
  EXPECT_GT(sheet.Variables_Count(), 0);

  XSTRING value;
  EXPECT_TRUE(sheet.Variables_Get(__L("--accent"), value));
  EXPECT_EQ(value.Compare(__L("1,2,3,255"), true), 0);
}


//-----------------------------------------------------------------------------------------
// @import : relative resolution + cycle detection

TEST(UI_CSSParser, ImportPullsRulesFromAnotherFileResolvedRelativeToTheImportingFile)
{
  XPATH subpath;
  UI_CSS_UnitTests_WriteFile(__L("import_sub.css"), __L(".imported { color: 7,7,7,255; }"), subpath);

  XPATH basepath;
  UI_CSS_UnitTests_WriteFile(__L("import_base.css"), __L("@import \"import_sub.css\"; .local { color: 8,8,8,255; }"), basepath);

  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;

  ASSERT_TRUE(parser.ParseFile(basepath, sheet));
  EXPECT_EQ(sheet.Rules_Count(), 2);

  XSTRING              elementtype;
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("imported")));

  ASSERT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out));

  XSTRING color;
  ASSERT_TRUE(out.Get(__L("color"), color));
  EXPECT_EQ(color.Compare(__L("7,7,7,255"), true), 0);

  elementclasses.DeleteContents();
}


TEST(UI_CSSParser, CyclicImportTerminatesInsteadOfRecursingForever)
{
  // a.css imports b.css, b.css imports a.css back. ParseFile() must return (proving the cycle guard fired)
  // rather than hang or crash; this test itself is the regression guard against a broken cycle detector.
  XPATH apath;
  XPATH bpath;
  UI_CSS_UnitTests_WriteFile(__L("cycle_a.css"), __L("@import \"cycle_b.css\"; .a { color: 1,1,1,255; }"), apath);
  UI_CSS_UnitTests_WriteFile(__L("cycle_b.css"), __L("@import \"cycle_a.css\"; .b { color: 2,2,2,255; }"), bpath);

  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;

  EXPECT_TRUE(parser.ParseFile(apath, sheet));

  // Both rules that do NOT depend on completing the cycle must still be present.
  EXPECT_GE(sheet.Rules_Count(), 1);
}


TEST(UI_CSSParser, ImporterOverridingAVariableAfterTheImportLineWinsOverTheImportedFilesOwnUse)
{
  // Phase 2 residual-variable-merge-order fix: the IMPORTED file (theme.css) uses "--brand-color" with a
  // sentinel fallback; the IMPORTING file (base.css) declares "@import" FIRST and only overrides
  // "--brand-color" in its OWN ":root" AFTER that line -- the exact ordering the report flagged as broken.
  // Before the fix, theme.css's own nested ParseText() call expanded var() immediately (against a variable
  // table that did not yet contain base.css's override) and permanently consumed the var() token, so
  // ".imported"'s background-color froze on the fallback forever, no matter what base.css declared later.
  XPATH themepath;
  UI_CSS_UnitTests_WriteFile(__L("varorder_theme.css"), __L(".imported { background-color: var(--brand-color, 1,1,1,255); }"), themepath);

  XPATH basepath;
  UI_CSS_UnitTests_WriteFile(__L("varorder_base.css"), __L("@import \"varorder_theme.css\"; :root { --brand-color: 9,9,9,255; }"), basepath);

  UI_CSSPARSER   parser;
  UI_STYLESHEET  sheet;

  ASSERT_TRUE(parser.ParseFile(basepath, sheet));

  XSTRING              elementtype;
  XSTRING              elementid;
  XVECTOR<XSTRING*>    elementclasses;
  XVECTOR<XSTRING*>    activepseudos;
  UI_STYLE             out;

  elementclasses.Add(UI_CSS_UnitTests_NewStr(__L("imported")));

  ASSERT_TRUE(sheet.Resolve(elementtype, elementid, elementclasses, activepseudos, out));

  XSTRING bckgrdcolor;
  ASSERT_TRUE(out.Get(__L("background-color"), bckgrdcolor));
  EXPECT_EQ(bckgrdcolor.Compare(__L("9,9,9,255"), true), 0);   // the importer's override, not the fallback

  elementclasses.DeleteContents();
}


//-----------------------------------------------------------------------------------------
// UI_STYLE : bag round-trip, inline declarations, and the background-color / bckgrdcolor alias pattern

TEST(UI_Style, GetReturnsFalseForAMissingKey)
{
  UI_STYLE bag;
  XSTRING  value;

  EXPECT_FALSE(bag.Get(__L("does-not-exist"), value));
}


TEST(UI_Style, SetThenGetRoundTripsTheSameValue)
{
  UI_STYLE bag;
  XSTRING  value;

  bag.Set(__L("width"), __L("120"));

  ASSERT_TRUE(bag.Get(__L("width"), value));
  EXPECT_EQ(value.Compare(__L("120"), true), 0);
}


TEST(UI_Style, InlineDeclarationsUseTheSameGrammarAsARuleBody)
{
  UI_CSSPARSER parser;
  UI_STYLE     bag;
  XSTRING      text;

  text.Set(__L("color: 1,2,3,255; background-color: 4,5,6,255;"));

  ASSERT_TRUE(parser.ParseInlineDeclarations(text, bag));

  XSTRING color;
  ASSERT_TRUE(bag.Get(__L("color"), color));
  EXPECT_EQ(color.Compare(__L("1,2,3,255"), true), 0);
}


TEST(UI_Style, BackgroundColorAliasIsReadableThroughEitherSpelling)
{
  // This is the building block both GetLayoutElement_Base() (UI_Manager.cpp, load time) and the Phase 0 fix
  // to UI_ELEMENT::ReapplyStyleVisual() (state changes) rely on: a value authored as the CSS-natural
  // "background-color" must be reachable through the same first-hit alias lookup as the legacy "bckgrdcolor".
  UI_STYLE bag;
  bag.Set(__L("background-color"), __L("11,22,33,255"));

  XSTRING resolved;
  bool    found = bag.Get(__L("bckgrdcolor"), resolved) && !resolved.IsEmpty();
  if(!found) found = bag.Get(__L("background-color"), resolved) && !resolved.IsEmpty();

  ASSERT_TRUE(found);
  EXPECT_EQ(resolved.Compare(__L("11,22,33,255"), true), 0);
}


TEST(UI_Style, LegacyBckgrdColorSpellingStillWorksUnchanged)
{
  UI_STYLE bag;
  bag.Set(__L("bckgrdcolor"), __L("44,55,66,255"));

  XSTRING resolved;
  bool    found = bag.Get(__L("bckgrdcolor"), resolved) && !resolved.IsEmpty();
  if(!found) found = bag.Get(__L("background-color"), resolved) && !resolved.IsEmpty();

  ASSERT_TRUE(found);
  EXPECT_EQ(resolved.Compare(__L("44,55,66,255"), true), 0);
}


//-----------------------------------------------------------------------------------------
// UI_PROPERTYREGISTRY : alias lookup + CSS 1-to-4-value shorthand expansion (Phase 1)

TEST(UI_PropertyRegistry, GetAliasedPrefersPrimaryKeyWhenBothArePresent)
{
  UI_STYLE bag;
  bag.Set(__L("bckgrdcolor")    , __L("1,2,3,255"));
  bag.Set(__L("background-color"), __L("9,9,9,255"));

  XSTRING resolved;
  ASSERT_TRUE(UI_PROPERTYREGISTRY::GetAliased(bag, __L("bckgrdcolor"), __L("background-color"), resolved));
  EXPECT_EQ(resolved.Compare(__L("1,2,3,255"), true), 0);
}


TEST(UI_PropertyRegistry, GetAliasedFallsBackToSecondaryKeyWhenPrimaryIsAbsent)
{
  UI_STYLE bag;
  bag.Set(__L("background-color"), __L("11,22,33,255"));

  XSTRING resolved;
  ASSERT_TRUE(UI_PROPERTYREGISTRY::GetAliased(bag, __L("bckgrdcolor"), __L("background-color"), resolved));
  EXPECT_EQ(resolved.Compare(__L("11,22,33,255"), true), 0);
}


TEST(UI_PropertyRegistry, GetAliasedReturnsFalseWhenNeitherKeyIsPresent)
{
  UI_STYLE bag;

  XSTRING resolved;
  EXPECT_FALSE(UI_PROPERTYREGISTRY::GetAliased(bag, __L("bckgrdcolor"), __L("background-color"), resolved));
}


TEST(UI_PropertyRegistry, ExpandCSSShorthand4WithOneValueAppliesToAllFourSlots)
{
  XSTRING text(__L("4"));
  double  out[4] = { -1.0, -1.0, -1.0, -1.0 };

  UI_PROPERTYREGISTRY::ExpandCSSShorthand4(text, out);

  EXPECT_EQ(out[0], 4.0); EXPECT_EQ(out[1], 4.0); EXPECT_EQ(out[2], 4.0); EXPECT_EQ(out[3], 4.0);
}


TEST(UI_PropertyRegistry, ExpandCSSShorthand4WithTwoValuesPairsOppositeSlots)
{
  XSTRING text(__L("4,8"));
  double  out[4] = { -1.0, -1.0, -1.0, -1.0 };

  UI_PROPERTYREGISTRY::ExpandCSSShorthand4(text, out);

  EXPECT_EQ(out[0], 4.0); EXPECT_EQ(out[2], 4.0);        // slot 0/2 pair (e.g. TOP/BOTTOM)
  EXPECT_EQ(out[1], 8.0); EXPECT_EQ(out[3], 8.0);        // slot 1/3 pair (e.g. RIGHT/LEFT)
}


TEST(UI_PropertyRegistry, ExpandCSSShorthand4WithThreeValuesMirrorsMiddleSlotAcrossOneTwoThree)
{
  XSTRING text(__L("4 8 12"));
  double  out[4] = { -1.0, -1.0, -1.0, -1.0 };

  UI_PROPERTYREGISTRY::ExpandCSSShorthand4(text, out);

  EXPECT_EQ(out[0], 4.0);
  EXPECT_EQ(out[1], 8.0); EXPECT_EQ(out[3], 8.0);
  EXPECT_EQ(out[2], 12.0);
}


TEST(UI_PropertyRegistry, ExpandCSSShorthand4WithFourValuesAssignsEachSlotInOrder)
{
  XSTRING text(__L("1,2,3,4"));
  double  out[4] = { -1.0, -1.0, -1.0, -1.0 };

  UI_PROPERTYREGISTRY::ExpandCSSShorthand4(text, out);

  EXPECT_EQ(out[0], 1.0); EXPECT_EQ(out[1], 2.0); EXPECT_EQ(out[2], 3.0); EXPECT_EQ(out[3], 4.0);
}


TEST(UI_PropertyRegistry, TokenizeNumbersStopsAtCapacityAndAcceptsMixedSeparators)
{
  XSTRING text(__L("1 2,3\t4 5"));
  double  vals[3] = { -1.0, -1.0, -1.0 };

  XDWORD  n = UI_PROPERTYREGISTRY::TokenizeNumbers(text, vals, 3);

  ASSERT_EQ(n, (XDWORD)3);
  EXPECT_EQ(vals[0], 1.0); EXPECT_EQ(vals[1], 2.0); EXPECT_EQ(vals[2], 3.0);
}


// ParseBoxShadow(): shared by UI_MANAGER::GetLayoutElement_Base() (load time) and UI_ELEMENT::
// ReapplyStyleVisual() (Step 8, re-style on pseudo-class state change).

TEST(UI_PropertyRegistry, ParseBoxShadowWithThreeTokensLeavesBlurAtZero)
{
  XSTRING raw(__L("2 4 red"));
  double  x = -1.0, y = -1.0, blur = -1.0;
  XSTRING color;

  ASSERT_TRUE(UI_PROPERTYREGISTRY::ParseBoxShadow(raw, x, y, blur, color));
  EXPECT_EQ(x, 2.0); EXPECT_EQ(y, 4.0); EXPECT_EQ(blur, 0.0);
  EXPECT_TRUE(!color.Compare(__L("red"), true));
}

TEST(UI_PropertyRegistry, ParseBoxShadowWithFourTokensReadsBlur)
{
  XSTRING raw(__L("2 4 8 #FF0000FF"));
  double  x = 0.0, y = 0.0, blur = 0.0;
  XSTRING color;

  ASSERT_TRUE(UI_PROPERTYREGISTRY::ParseBoxShadow(raw, x, y, blur, color));
  EXPECT_EQ(x, 2.0); EXPECT_EQ(y, 4.0); EXPECT_EQ(blur, 8.0);
  EXPECT_TRUE(!color.Compare(__L("#FF0000FF"), true));
}

TEST(UI_PropertyRegistry, ParseBoxShadowTreatsACommaTupleColorAsNonNumericEvenWhenItStartsWithADigit)
{
  // "0,0,0,120" starts with a digit like an offset would, but its interior comma marks it as a GEN
  // "R,G,B[,A]" colour tuple, not a third numeric (blur) token -- see the has_comma check.
  XSTRING raw(__L("2 4 0,0,0,120"));
  double  x = -1.0, y = -1.0, blur = -1.0;
  XSTRING color;

  ASSERT_TRUE(UI_PROPERTYREGISTRY::ParseBoxShadow(raw, x, y, blur, color));
  EXPECT_EQ(x, 2.0); EXPECT_EQ(y, 4.0); EXPECT_EQ(blur, 0.0);
  EXPECT_TRUE(!color.Compare(__L("0,0,0,120"), true));
}

TEST(UI_PropertyRegistry, ParseBoxShadowReturnsFalseWhenTheColorIsMissing)
{
  XSTRING raw(__L("2 4"));
  double  x = 0.0, y = 0.0, blur = 0.0;
  XSTRING color;

  EXPECT_FALSE(UI_PROPERTYREGISTRY::ParseBoxShadow(raw, x, y, blur, color));
}

TEST(UI_PropertyRegistry, ParseBoxShadowReturnsFalseForAnEmptyString)
{
  XSTRING raw(__L(""));
  double  x = 0.0, y = 0.0, blur = 0.0;
  XSTRING color;

  EXPECT_FALSE(UI_PROPERTYREGISTRY::ParseBoxShadow(raw, x, y, blur, color));
}


//-----------------------------------------------------------------------------------------
// UI_LENGTH : NUMBER / PERCENT / KEYWORD classification (Phase 1)

TEST(UI_Length, ParsesAPlainNumberAsNumberType)
{
  XSTRING   raw(__L("42"));
  UI_LENGTH length;

  ASSERT_TRUE(length.Parse(raw));
  EXPECT_EQ(length.GetType(), UI_LENGTH_TYPE_NUMBER);
  EXPECT_EQ(length.GetValue(), 42.0);
}


TEST(UI_Length, ParsesATrailingPercentSignAsPercentType)
{
  XSTRING   raw(__L("50%"));
  UI_LENGTH length;

  ASSERT_TRUE(length.Parse(raw));
  EXPECT_EQ(length.GetType(), UI_LENGTH_TYPE_PERCENT);
  EXPECT_EQ(length.GetValue(), 50.0);          // 0..100, not 0..1
}


TEST(UI_Length, ParsesANonNumericTokenAsKeywordType)
{
  XSTRING   raw(__L("left"));
  UI_LENGTH length;

  ASSERT_TRUE(length.Parse(raw));
  EXPECT_EQ(length.GetType(), UI_LENGTH_TYPE_KEYWORD);
  EXPECT_TRUE(length.IsKeyword(__L("LEFT")));  // case-insensitive, matching the rest of this subsystem
  EXPECT_FALSE(length.IsKeyword(__L("right")));
}


TEST(UI_Length, ParseReturnsFalseForAnEmptyString)
{
  XSTRING   raw;
  UI_LENGTH length;

  EXPECT_FALSE(length.Parse(raw));
  EXPECT_EQ(length.GetType(), UI_LENGTH_TYPE_UNDEFINED);
}


TEST(UI_Length, ResolveAppliesPercentAgainstTheGivenBasis)
{
  XSTRING   raw(__L("25%"));
  UI_LENGTH length;
  double    out = -1.0;

  ASSERT_TRUE(length.Parse(raw));
  ASSERT_TRUE(length.Resolve(200.0, out));
  EXPECT_EQ(out, 50.0);                        // 25% of 200
}


TEST(UI_Length, ResolveIgnoresBasisForANumber)
{
  XSTRING   raw(__L("42"));
  UI_LENGTH length;
  double    out = -1.0;

  ASSERT_TRUE(length.Parse(raw));
  ASSERT_TRUE(length.Resolve(999.0, out));
  EXPECT_EQ(out, 42.0);
}


TEST(UI_Length, ResolveReturnsFalseForAKeyword)
{
  XSTRING   raw(__L("auto"));
  UI_LENGTH length;
  double    out = -1.0;

  ASSERT_TRUE(length.Parse(raw));
  EXPECT_FALSE(length.Resolve(100.0, out));
}


//-----------------------------------------------------------------------------------------
// UI_LENGTH : em / rem / vw / vh units and calc() (Phase 3, first increment)

TEST(UI_Length, ParsesAnEmSuffixAsEmType)
{
  XSTRING   raw(__L("1.5em"));
  UI_LENGTH length;

  ASSERT_TRUE(length.Parse(raw));
  EXPECT_EQ(length.GetType(), UI_LENGTH_TYPE_EM);
  EXPECT_EQ(length.GetValue(), 1.5);
}


TEST(UI_Length, ParsesARemSuffixAsRemTypeNotEm)
{
  XSTRING   raw(__L("2rem"));
  UI_LENGTH length;

  ASSERT_TRUE(length.Parse(raw));
  EXPECT_EQ(length.GetType(), UI_LENGTH_TYPE_REM);   // "rem" must win over the shorter "em" suffix match
  EXPECT_EQ(length.GetValue(), 2.0);
}


TEST(UI_Length, ParsesAVwSuffixAsVwType)
{
  XSTRING   raw(__L("50vw"));
  UI_LENGTH length;

  ASSERT_TRUE(length.Parse(raw));
  EXPECT_EQ(length.GetType(), UI_LENGTH_TYPE_VW);
  EXPECT_EQ(length.GetValue(), 50.0);
}


TEST(UI_Length, ParsesAVhSuffixAsVhType)
{
  XSTRING   raw(__L("25vh"));
  UI_LENGTH length;

  ASSERT_TRUE(length.Parse(raw));
  EXPECT_EQ(length.GetType(), UI_LENGTH_TYPE_VH);
  EXPECT_EQ(length.GetValue(), 25.0);
}


TEST(UI_Length, ResolveWithContextAppliesEmAgainstTheElementsOwnFontSize)
{
  XSTRING            raw(__L("2em"));
  UI_LENGTH          length;
  UI_LENGTH_CONTEXT  context = { 0.0, 16.0, 10.0, 1440.0, 900.0 };
  double             out = -1.0;

  ASSERT_TRUE(length.Parse(raw));
  ASSERT_TRUE(length.Resolve(context, out));
  EXPECT_EQ(out, 32.0);                        // 2 * 16
}


TEST(UI_Length, ResolveWithContextAppliesRemAgainstTheRootFontSizeNotTheElementsOwn)
{
  XSTRING            raw(__L("2rem"));
  UI_LENGTH          length;
  UI_LENGTH_CONTEXT  context = { 0.0, 30.0, 10.0, 1440.0, 900.0 };   // element font-size deliberately different
  double             out = -1.0;

  ASSERT_TRUE(length.Parse(raw));
  ASSERT_TRUE(length.Resolve(context, out));
  EXPECT_EQ(out, 20.0);                        // 2 * 10 (root), not 2 * 30 (element)
}


TEST(UI_Length, ResolveWithContextAppliesVwAndVhAgainstTheViewport)
{
  XSTRING            rawwidth(__L("50vw"));
  XSTRING            rawheight(__L("10vh"));
  UI_LENGTH          width, height;
  UI_LENGTH_CONTEXT  context = { 0.0, 16.0, 16.0, 1440.0, 900.0 };
  double             outwidth = -1.0, outheight = -1.0;

  ASSERT_TRUE(width.Parse(rawwidth));
  ASSERT_TRUE(height.Parse(rawheight));
  ASSERT_TRUE(width.Resolve(context, outwidth));
  ASSERT_TRUE(height.Resolve(context, outheight));
  EXPECT_EQ(outwidth, 720.0);                  // 50% of 1440
  EXPECT_EQ(outheight, 90.0);                  // 10% of 900
}


TEST(UI_Length, OldBasisOnlyResolveOverloadReturnsFalseForANewUnitType)
{
  XSTRING   raw(__L("2em"));
  UI_LENGTH length;
  double    out = -1.0;

  ASSERT_TRUE(length.Parse(raw));
  EXPECT_FALSE(length.Resolve(100.0, out));    // needs a font-size, a bare basis is not enough
}


TEST(UI_Length, CalcAddsAPercentAndAPixelLength)
{
  XSTRING            raw(__L("calc(100% - 20)"));
  UI_LENGTH          length;
  UI_LENGTH_CONTEXT  context = { 200.0, 16.0, 16.0, 1440.0, 900.0 };
  double             out = -1.0;

  ASSERT_TRUE(length.Parse(raw));
  EXPECT_EQ(length.GetType(), UI_LENGTH_TYPE_CALC);
  ASSERT_TRUE(length.Resolve(context, out));
  EXPECT_EQ(out, 180.0);                       // 100% of 200 (=200) minus the bare number 20
}


TEST(UI_Length, CalcRespectsMultiplicationPrecedenceOverAddition)
{
  XSTRING            raw(__L("calc(10 + 2 * 3)"));
  UI_LENGTH          length;
  UI_LENGTH_CONTEXT  context = { 0.0, 16.0, 16.0, 1440.0, 900.0 };
  double             out = -1.0;

  ASSERT_TRUE(length.Parse(raw));
  ASSERT_TRUE(length.Resolve(context, out));
  EXPECT_EQ(out, 16.0);                        // 10 + (2*3), not (10+2)*3
}


TEST(UI_Length, CalcHonoursExplicitParentheses)
{
  XSTRING            raw(__L("calc((10 + 2) * 3)"));
  UI_LENGTH          length;
  UI_LENGTH_CONTEXT  context = { 0.0, 16.0, 16.0, 1440.0, 900.0 };
  double             out = -1.0;

  ASSERT_TRUE(length.Parse(raw));
  ASSERT_TRUE(length.Resolve(context, out));
  EXPECT_EQ(out, 36.0);                        // (10+2)*3
}


TEST(UI_Length, CalcCanMixEmAndVwOperands)
{
  XSTRING            raw(__L("calc(1em + 10vw)"));
  UI_LENGTH          length;
  UI_LENGTH_CONTEXT  context = { 0.0, 16.0, 16.0, 1000.0, 900.0 };
  double             out = -1.0;

  ASSERT_TRUE(length.Parse(raw));
  ASSERT_TRUE(length.Resolve(context, out));
  EXPECT_EQ(out, 116.0);                       // 1*16 (em) + 10% of 1000 (vw) = 16 + 100
}


TEST(UI_Length, CalcDivisionByZeroFailsResolveRatherThanReturningInfinity)
{
  XSTRING            raw(__L("calc(10 / 0)"));
  UI_LENGTH          length;
  UI_LENGTH_CONTEXT  context = { 0.0, 16.0, 16.0, 1440.0, 900.0 };
  double             out = -1.0;

  ASSERT_TRUE(length.Parse(raw));
  EXPECT_FALSE(length.Resolve(context, out));
}


TEST(UI_Length, MalformedCalcFallsBackToKeywordInsteadOfFailingParse)
{
  XSTRING   raw(__L("calc(100% -)"));           // dangling operator, no right-hand operand
  UI_LENGTH length;

  ASSERT_TRUE(length.Parse(raw));               // Parse() itself never fails except for empty input
  EXPECT_EQ(length.GetType(), UI_LENGTH_TYPE_KEYWORD);
  EXPECT_TRUE(length.IsKeyword(__L("calc(100% -)")));
}


TEST(UI_Length, LeadingAndTrailingWhitespaceIsTrimmedBeforeClassification)
{
  XSTRING   raw(__L("  42  "));
  UI_LENGTH length;

  ASSERT_TRUE(length.Parse(raw));
  EXPECT_EQ(length.GetType(), UI_LENGTH_TYPE_NUMBER);
  EXPECT_EQ(length.GetValue(), 42.0);
}


//-----------------------------------------------------------------------------------------
// UI_COMPUTEDSTYLE : typed accessors over the UI_STYLE bag (Phase 1)

TEST(UI_ComputedStyle, GetLengthParsesTheRawStringAtTheGivenKey)
{
  UI_COMPUTEDSTYLE style;
  style.Set(__L("width"), __L("75%"));

  UI_LENGTH width;
  ASSERT_TRUE(style.GetLength(__L("width"), width));
  EXPECT_EQ(width.GetType(), UI_LENGTH_TYPE_PERCENT);
  EXPECT_EQ(width.GetValue(), 75.0);
}


TEST(UI_ComputedStyle, GetLengthReturnsFalseForAMissingKey)
{
  UI_COMPUTEDSTYLE style;

  UI_LENGTH width;
  EXPECT_FALSE(style.GetLength(__L("width"), width));
}


TEST(UI_ComputedStyle, GetLengthAliasedPrefersThePrimaryKey)
{
  UI_COMPUTEDSTYLE style;
  style.Set(__L("xpos"), __L("10"));

  UI_LENGTH resolved;
  ASSERT_TRUE(style.GetLength(__L("xpos"), __L("x"), resolved));
  EXPECT_EQ(resolved.GetValue(), 10.0);
}


TEST(UI_ComputedStyle, GetColorParsesAnRGBATuple)
{
  UI_COMPUTEDSTYLE style;
  style.Set(__L("color"), __L("10,20,30,255"));

  UI_COLOR color;
  ASSERT_TRUE(style.GetColor(__L("color"), color));
  EXPECT_EQ(color.GetRed()  , 10);
  EXPECT_EQ(color.GetGreen(), 20);
  EXPECT_EQ(color.GetBlue() , 30);
}


TEST(UI_ComputedStyle, GetColorAliasedFallsBackToTheCSSNaturalName)
{
  UI_COMPUTEDSTYLE style;
  style.Set(__L("background-color"), __L("1,2,3,255"));

  UI_COLOR color;
  ASSERT_TRUE(style.GetColor(__L("bckgrdcolor"), __L("background-color"), color));
  EXPECT_EQ(color.GetRed(), 1);
}


TEST(UI_ComputedStyle, InheritsTheUnderlyingBagUnchanged)
{
  // UI_COMPUTEDSTYLE IS-A UI_STYLE: FillFromXMLElement/FillFromCSSDeclarations/FillFromInlineStyle and the
  // plain string/double Get()/Set() keep working exactly as before -- this is what lets it be a drop-in
  // replacement everywhere GetLayoutElement_Base() used to build a plain UI_STYLE.
  UI_COMPUTEDSTYLE style;
  style.Set(__L("sizefont"), __L("14"));

  double sizefont = 0.0;
  ASSERT_TRUE(style.Get(__L("sizefont"), sizefont));
  EXPECT_EQ(sizefont, 14.0);
}


//-----------------------------------------------------------------------------------------
// UI_COMPUTEDSTYLE : box-sizing and min/max clamping (Phase 3, second increment)

TEST(UI_ComputedStyle, GetBoxSizingReadsBorderBox)
{
  UI_COMPUTEDSTYLE style;
  style.Set(__L("box-sizing"), __L("border-box"));

  UI_BOXSIZING boxsizing = UI_BOXSIZING_CONTENTBOX;
  ASSERT_TRUE(style.GetBoxSizing(boxsizing));
  EXPECT_EQ(boxsizing, UI_BOXSIZING_BORDERBOX);
}


TEST(UI_ComputedStyle, GetBoxSizingReadsContentBoxExplicitly)
{
  UI_COMPUTEDSTYLE style;
  style.Set(__L("box-sizing"), __L("content-box"));

  UI_BOXSIZING boxsizing = UI_BOXSIZING_BORDERBOX;   // deliberately pre-set to the OTHER value
  ASSERT_TRUE(style.GetBoxSizing(boxsizing));
  EXPECT_EQ(boxsizing, UI_BOXSIZING_CONTENTBOX);
}


TEST(UI_ComputedStyle, GetBoxSizingDefaultsToContentBoxWhenAbsent)
{
  UI_COMPUTEDSTYLE style;

  UI_BOXSIZING boxsizing = UI_BOXSIZING_BORDERBOX;   // deliberately pre-set to the OTHER value
  EXPECT_FALSE(style.GetBoxSizing(boxsizing));       // key absent -- caller can tell, if it cares
  EXPECT_EQ(boxsizing, UI_BOXSIZING_CONTENTBOX);     // but "boxsizing" is still left holding the CSS default
}


TEST(UI_ComputedStyle, GetBoxSizingDefaultsToContentBoxForAnUnrecognizedKeyword)
{
  UI_COMPUTEDSTYLE style;
  style.Set(__L("box-sizing"), __L("padding-box"));  // real CSS, but not one GEN CSS Lite supports

  UI_BOXSIZING boxsizing = UI_BOXSIZING_BORDERBOX;
  EXPECT_FALSE(style.GetBoxSizing(boxsizing));
  EXPECT_EQ(boxsizing, UI_BOXSIZING_CONTENTBOX);
}


TEST(UI_ComputedStyle, ClampToMinMaxLeavesValueUnchangedWhenNeitherBoundIsPresent)
{
  UI_COMPUTEDSTYLE  style;
  UI_LENGTH_CONTEXT context = { 0.0, 16.0, 16.0, 1440.0, 900.0 };

  EXPECT_EQ(style.ClampToMinMax(150.0, __L("min-width"), __L("max-width"), context), 150.0);
}


TEST(UI_ComputedStyle, ClampToMinMaxPullsAValueUpToTheMinimum)
{
  UI_COMPUTEDSTYLE  style;
  UI_LENGTH_CONTEXT context = { 0.0, 16.0, 16.0, 1440.0, 900.0 };
  style.Set(__L("min-width"), __L("100"));

  EXPECT_EQ(style.ClampToMinMax(40.0, __L("min-width"), __L("max-width"), context), 100.0);
}


TEST(UI_ComputedStyle, ClampToMinMaxPullsAValueDownToTheMaximum)
{
  UI_COMPUTEDSTYLE  style;
  UI_LENGTH_CONTEXT context = { 0.0, 16.0, 16.0, 1440.0, 900.0 };
  style.Set(__L("max-width"), __L("200"));

  EXPECT_EQ(style.ClampToMinMax(500.0, __L("min-width"), __L("max-width"), context), 200.0);
}


TEST(UI_ComputedStyle, ClampToMinMaxResolvesBoundsThroughTheGivenContextIncludingPercent)
{
  UI_COMPUTEDSTYLE  style;
  UI_LENGTH_CONTEXT context = { 400.0, 16.0, 16.0, 1440.0, 900.0 };
  style.Set(__L("max-width"), __L("50%"));         // 50% of the 400 basis = 200

  EXPECT_EQ(style.ClampToMinMax(500.0, __L("min-width"), __L("max-width"), context), 200.0);
}


TEST(UI_ComputedStyle, ClampToMinMaxMinWinsOverMaxWhenTheAuthorContradictsThemselves)
{
  UI_COMPUTEDSTYLE  style;
  UI_LENGTH_CONTEXT context = { 0.0, 16.0, 16.0, 1440.0, 900.0 };
  style.Set(__L("min-width"), __L("300"));
  style.Set(__L("max-width"), __L("100"));         // max < min: CSS says MIN wins

  EXPECT_EQ(style.ClampToMinMax(50.0, __L("min-width"), __L("max-width"), context), 300.0);
}


TEST(UI_ComputedStyle, ClampToMinMaxTreatsMaxNoneAsUnconstrained)
{
  UI_COMPUTEDSTYLE  style;
  UI_LENGTH_CONTEXT context = { 0.0, 16.0, 16.0, 1440.0, 900.0 };
  style.Set(__L("max-width"), __L("none"));        // real CSS keyword for "no maximum"

  EXPECT_EQ(style.ClampToMinMax(5000.0, __L("min-width"), __L("max-width"), context), 5000.0);
}


TEST(UI_ComputedStyle, ResolveContentSizeIsAPassthroughForContentBox)
{
  EXPECT_EQ(UI_COMPUTEDSTYLE::ResolveContentSize(200.0, UI_BOXSIZING_CONTENTBOX, 30.0), 200.0);
}


TEST(UI_ComputedStyle, ResolveContentSizeSubtractsPaddingAndBorderForBorderBox)
{
  EXPECT_EQ(UI_COMPUTEDSTYLE::ResolveContentSize(200.0, UI_BOXSIZING_BORDERBOX, 30.0), 170.0);
}


TEST(UI_ComputedStyle, ResolveContentSizeNeverGoesNegative)
{
  EXPECT_EQ(UI_COMPUTEDSTYLE::ResolveContentSize(20.0, UI_BOXSIZING_BORDERBOX, 30.0), 0.0);
}


//-----------------------------------------------------------------------------------------
// UI_LAYOUTBOX : pure top-left layout box geometry tree (Phase 3, third increment)

TEST(UI_LayoutBox, DefaultsToAllZeroGeometry)
{
  UI_LAYOUTBOX box;

  EXPECT_EQ(box.GetContentLeft(), 0.0);
  EXPECT_EQ(box.GetContentTop(), 0.0);
  EXPECT_EQ(box.GetContentWidth(), 0.0);
  EXPECT_EQ(box.GetContentHeight(), 0.0);
  EXPECT_EQ(box.GetParent(), (UI_LAYOUTBOX*)NULL);
  EXPECT_EQ(box.GetChildren().GetSize(), (XDWORD)0);
}


TEST(UI_LayoutBox, SetContentBoxRoundTrips)
{
  UI_LAYOUTBOX box;
  box.SetContentBox(10.0, 20.0, 300.0, 150.0);

  EXPECT_EQ(box.GetContentLeft(), 10.0);
  EXPECT_EQ(box.GetContentTop(), 20.0);
  EXPECT_EQ(box.GetContentWidth(), 300.0);
  EXPECT_EQ(box.GetContentHeight(), 150.0);
}


TEST(UI_LayoutBox, SetPaddingBorderMarginRoundTripThroughTheirGetters)
{
  UI_LAYOUTBOX box;
  box.SetPadding(1.0, 2.0, 3.0, 4.0);
  box.SetBorder(5.0, 6.0, 7.0, 8.0);
  box.SetMargin(9.0, 10.0, 11.0, 12.0);

  EXPECT_EQ(box.GetPadding().top, 1.0);
  EXPECT_EQ(box.GetPadding().right, 2.0);
  EXPECT_EQ(box.GetPadding().bottom, 3.0);
  EXPECT_EQ(box.GetPadding().left, 4.0);

  EXPECT_EQ(box.GetBorder().top, 5.0);
  EXPECT_EQ(box.GetBorder().right, 6.0);
  EXPECT_EQ(box.GetBorder().bottom, 7.0);
  EXPECT_EQ(box.GetBorder().left, 8.0);

  EXPECT_EQ(box.GetMargin().top, 9.0);
  EXPECT_EQ(box.GetMargin().right, 10.0);
  EXPECT_EQ(box.GetMargin().bottom, 11.0);
  EXPECT_EQ(box.GetMargin().left, 12.0);
}


TEST(UI_LayoutBox, NegativePaddingBorderMarginAreClampedToZero)
{
  UI_LAYOUTBOX box;
  box.SetPadding(-1.0, -2.0, -3.0, -4.0);
  box.SetBorder(-5.0, 0.0, 5.0, -8.0);

  EXPECT_EQ(box.GetPadding().top, 0.0);
  EXPECT_EQ(box.GetPadding().right, 0.0);
  EXPECT_EQ(box.GetPadding().bottom, 0.0);
  EXPECT_EQ(box.GetPadding().left, 0.0);

  EXPECT_EQ(box.GetBorder().top, 0.0);     // clamped
  EXPECT_EQ(box.GetBorder().right, 0.0);   // was already 0
  EXPECT_EQ(box.GetBorder().bottom, 5.0);  // positive, untouched
  EXPECT_EQ(box.GetBorder().left, 0.0);    // clamped
}


TEST(UI_LayoutBox, GetPaddingBoxExpandsTheContentBoxOutwardByPaddingOnly)
{
  UI_LAYOUTBOX box;
  box.SetContentBox(100.0, 50.0, 200.0, 80.0);
  box.SetPadding(10.0, 20.0, 10.0, 20.0);
  box.SetBorder(5.0, 5.0, 5.0, 5.0);         // must NOT leak into the padding box

  double left = -1.0, top = -1.0, width = -1.0, height = -1.0;
  box.GetPaddingBox(left, top, width, height);

  EXPECT_EQ(left, 80.0);      // 100 - 20 (padding-left)
  EXPECT_EQ(top, 40.0);       // 50 - 10 (padding-top)
  EXPECT_EQ(width, 240.0);    // 200 + 20 + 20
  EXPECT_EQ(height, 100.0);   // 80 + 10 + 10
}


TEST(UI_LayoutBox, GetBorderBoxExpandsTheContentBoxOutwardByPaddingAndBorder)
{
  UI_LAYOUTBOX box;
  box.SetContentBox(100.0, 50.0, 200.0, 80.0);
  box.SetPadding(10.0, 20.0, 10.0, 20.0);
  box.SetBorder(5.0, 5.0, 5.0, 5.0);

  double left = -1.0, top = -1.0, width = -1.0, height = -1.0;
  box.GetBorderBox(left, top, width, height);

  EXPECT_EQ(left, 75.0);      // 100 - 20 - 5
  EXPECT_EQ(top, 35.0);       // 50 - 10 - 5
  EXPECT_EQ(width, 250.0);    // 200 + 20+20 + 5+5
  EXPECT_EQ(height, 110.0);   // 80 + 10+10 + 5+5
}


TEST(UI_LayoutBox, GetMarginBoxExpandsTheContentBoxOutwardByPaddingBorderAndMargin)
{
  UI_LAYOUTBOX box;
  box.SetContentBox(100.0, 50.0, 200.0, 80.0);
  box.SetPadding(10.0, 20.0, 10.0, 20.0);
  box.SetBorder(5.0, 5.0, 5.0, 5.0);
  box.SetMargin(15.0, 25.0, 15.0, 25.0);

  double left = -1.0, top = -1.0, width = -1.0, height = -1.0;
  box.GetMarginBox(left, top, width, height);

  EXPECT_EQ(left, 50.0);      // 100 - 20 - 5 - 25
  EXPECT_EQ(top, 20.0);       // 50 - 10 - 5 - 15
  EXPECT_EQ(width, 300.0);    // 200 + 40 + 10 + 50
  EXPECT_EQ(height, 140.0);   // 80 + 20 + 10 + 30
}


TEST(UI_LayoutBox, WithNoPaddingBorderOrMarginAllThreeDerivedBoxesEqualTheContentBox)
{
  UI_LAYOUTBOX box;
  box.SetContentBox(5.0, 6.0, 70.0, 80.0);

  double left, top, width, height;

  box.GetPaddingBox(left, top, width, height);
  EXPECT_EQ(left, 5.0); EXPECT_EQ(top, 6.0); EXPECT_EQ(width, 70.0); EXPECT_EQ(height, 80.0);

  box.GetBorderBox(left, top, width, height);
  EXPECT_EQ(left, 5.0); EXPECT_EQ(top, 6.0); EXPECT_EQ(width, 70.0); EXPECT_EQ(height, 80.0);

  box.GetMarginBox(left, top, width, height);
  EXPECT_EQ(left, 5.0); EXPECT_EQ(top, 6.0); EXPECT_EQ(width, 70.0); EXPECT_EQ(height, 80.0);
}


TEST(UI_LayoutBox, AddChildSetsTheChildsParentAndAppendsItToGetChildren)
{
  UI_LAYOUTBOX* root  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* child = GEN_NEW UI_LAYOUTBOX();

  root->AddChild(child);

  ASSERT_EQ(root->GetChildren().GetSize(), (XDWORD)1);
  EXPECT_EQ(root->GetChildren().Get(0), child);
  EXPECT_EQ(child->GetParent(), root);

  GEN_DELETE root;   // must also delete "child" -- see UI_LayoutBox_DestructorDeletesOwnedChildrenRecursively below
}


TEST(UI_LayoutBox, AddChildAppendsInCallOrderForMultipleChildren)
{
  UI_LAYOUTBOX* root   = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();

  root->AddChild(first);
  root->AddChild(second);

  ASSERT_EQ(root->GetChildren().GetSize(), (XDWORD)2);
  EXPECT_EQ(root->GetChildren().Get(0), first);
  EXPECT_EQ(root->GetChildren().Get(1), second);

  GEN_DELETE root;
}


TEST(UI_LayoutBox, AddChildIgnoresANullChildRatherThanCrashing)
{
  UI_LAYOUTBOX root;
  root.AddChild(NULL);

  EXPECT_EQ(root.GetChildren().GetSize(), (XDWORD)0);
}


TEST(UI_LayoutBox, DestructorDeletesOwnedChildrenRecursively)
{
  // Three-level tree (root -> mid -> leaf); relies on running under a memory-checked build/tool to actually
  // CATCH a leak, but at minimum this exercises the recursive delete path without crashing or double-freeing.
  UI_LAYOUTBOX* root = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* mid   = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* leaf  = GEN_NEW UI_LAYOUTBOX();

  mid->AddChild(leaf);
  root->AddChild(mid);

  GEN_DELETE root;   // should delete mid, which should delete leaf; no crash == pass
}


//-----------------------------------------------------------------------------------------
// UI_LAYOUTENGINE : builds a UI_LAYOUTBOX tree from a UI_ELEMENT tree (Phase 3, fourth increment, sub-step 1)

TEST(UI_LayoutEngine, BuildTreeReturnsNullForANullRoot)
{
  EXPECT_EQ(UI_LAYOUTENGINE::BuildTree(NULL), (UI_LAYOUTBOX*)NULL);
}


TEST(UI_LayoutEngine, BuildTreeCopiesTheElementsAlreadyResolvedContentBox)
{
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(element, 100.0, 50.0, 200.0, 80.0);

  UI_LAYOUTBOX* box = UI_LAYOUTENGINE::BuildTree(element);

  ASSERT_TRUE(box != NULL);
  EXPECT_EQ(box->GetContentLeft(), 100.0);
  EXPECT_EQ(box->GetContentTop(), 50.0);
  EXPECT_EQ(box->GetContentWidth(), 200.0);
  EXPECT_EQ(box->GetContentHeight(), 80.0);

  GEN_DELETE box;
  GEN_DELETE element;
}


TEST(UI_LayoutEngine, BuildTreeCopiesPaddingMarginAndAUniformBorderFromTheElement)
{
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(element, 0.0, 0.0, 100.0, 100.0);

  element->SetPadding(UI_ELEMENT_TYPE_ALIGN_UP, 1.0);
  element->SetPadding(UI_ELEMENT_TYPE_ALIGN_RIGHT, 2.0);
  element->SetPadding(UI_ELEMENT_TYPE_ALIGN_DOWN, 3.0);
  element->SetPadding(UI_ELEMENT_TYPE_ALIGN_LEFT, 4.0);

  element->SetMargin(UI_ELEMENT_TYPE_ALIGN_UP, 5.0);
  element->SetMargin(UI_ELEMENT_TYPE_ALIGN_RIGHT, 6.0);
  element->SetMargin(UI_ELEMENT_TYPE_ALIGN_DOWN, 7.0);
  element->SetMargin(UI_ELEMENT_TYPE_ALIGN_LEFT, 8.0);

  element->SetBorderWidth(3.0);

  UI_LAYOUTBOX* box = UI_LAYOUTENGINE::BuildTree(element);

  ASSERT_TRUE(box != NULL);
  EXPECT_EQ(box->GetPadding().top, 1.0);
  EXPECT_EQ(box->GetPadding().right, 2.0);
  EXPECT_EQ(box->GetPadding().bottom, 3.0);
  EXPECT_EQ(box->GetPadding().left, 4.0);

  EXPECT_EQ(box->GetMargin().top, 5.0);
  EXPECT_EQ(box->GetMargin().right, 6.0);
  EXPECT_EQ(box->GetMargin().bottom, 7.0);
  EXPECT_EQ(box->GetMargin().left, 8.0);

  EXPECT_EQ(box->GetBorder().top, 3.0);
  EXPECT_EQ(box->GetBorder().right, 3.0);
  EXPECT_EQ(box->GetBorder().bottom, 3.0);
  EXPECT_EQ(box->GetBorder().left, 3.0);

  GEN_DELETE box;
  GEN_DELETE element;
}


TEST(UI_LayoutEngine, BuildTreeTreatsTheSkinDefaultBorderWidthAsZeroForLayoutPurposes)
{
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(element, 0.0, 0.0, 100.0, 100.0);
  element->SetBorderWidth(-1.0);            // "use skin default" -- a painting concern, not layout

  UI_LAYOUTBOX* box = UI_LAYOUTENGINE::BuildTree(element);

  ASSERT_TRUE(box != NULL);
  EXPECT_EQ(box->GetBorder().top, 0.0);
  EXPECT_EQ(box->GetBorder().right, 0.0);
  EXPECT_EQ(box->GetBorder().bottom, 0.0);
  EXPECT_EQ(box->GetBorder().left, 0.0);

  GEN_DELETE box;
  GEN_DELETE element;
}


TEST(UI_LayoutEngine, BuildTreeMirrorsAParentWithTwoChildrenInComposeOrder)
{
  UI_ELEMENT* parent = GEN_NEW UI_ELEMENT();
  UI_ELEMENT* first   = GEN_NEW UI_ELEMENT();
  UI_ELEMENT* second  = GEN_NEW UI_ELEMENT();

  UI_CSS_UnitTests_SetElementBox(parent, 0.0, 0.0, 300.0, 200.0);
  UI_CSS_UnitTests_SetElementBox(first, 10.0, 10.0, 100.0, 50.0);
  UI_CSS_UnitTests_SetElementBox(second, 10.0, 70.0, 100.0, 50.0);

  first->SetFather(parent);
  second->SetFather(parent);
  parent->GetComposeElements()->Add(first);
  parent->GetComposeElements()->Add(second);

  UI_LAYOUTBOX* box = UI_LAYOUTENGINE::BuildTree(parent);

  ASSERT_TRUE(box != NULL);
  ASSERT_EQ(box->GetChildren().GetSize(), (XDWORD)2);

  UI_LAYOUTBOX* firstbox  = box->GetChildren().Get(0);
  UI_LAYOUTBOX* secondbox = box->GetChildren().Get(1);

  ASSERT_TRUE(firstbox != NULL);
  ASSERT_TRUE(secondbox != NULL);

  EXPECT_EQ(firstbox->GetContentTop(), 10.0);
  EXPECT_EQ(secondbox->GetContentTop(), 70.0);
  EXPECT_EQ(firstbox->GetParent(), box);
  EXPECT_EQ(secondbox->GetParent(), box);

  GEN_DELETE box;      // must also delete firstbox/secondbox -- UI_LAYOUTBOX owns its children
  GEN_DELETE parent;   // must also delete first/second -- UI_ELEMENT::DeleteAllComposeElements() owns its children
}


TEST(UI_LayoutEngine, BuildTreeIsAReadOnlySnapshotThatDoesNotTouchTheSourceElement)
{
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(element, 100.0, 50.0, 200.0, 80.0);

  UI_LAYOUTBOX* box = UI_LAYOUTENGINE::BuildTree(element);
  ASSERT_TRUE(box != NULL);

  EXPECT_EQ(element->GetLeftX(), 100.0);
  EXPECT_EQ(element->GetTopY(), 50.0);
  EXPECT_EQ(element->GetBoundaryLine()->width, 200.0);
  EXPECT_EQ(element->GetBoundaryLine()->height, 80.0);

  GEN_DELETE box;
  GEN_DELETE element;
}


// -- UI_LAYOUTBOX: position tag + insets storage (fourth increment, second sub-step) -----------------------------

TEST(UI_LayoutBox, DefaultsToStaticPositionWithNoInsetSpecified)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  EXPECT_EQ(box->GetPosition(), UI_POSITION_STATIC);

  UI_LAYOUTBOX_INSETS& insets = box->GetInsets();
  EXPECT_FALSE(insets.top.specified);
  EXPECT_FALSE(insets.right.specified);
  EXPECT_FALSE(insets.bottom.specified);
  EXPECT_FALSE(insets.left.specified);

  GEN_DELETE box;
}


TEST(UI_LayoutBox, SetPositionRoundTrips)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  box->SetPosition(UI_POSITION_ABSOLUTE);
  EXPECT_EQ(box->GetPosition(), UI_POSITION_ABSOLUTE);

  box->SetPosition(UI_POSITION_RELATIVE);
  EXPECT_EQ(box->GetPosition(), UI_POSITION_RELATIVE);

  GEN_DELETE box;
}


TEST(UI_LayoutBox, SetInsetsRoundTripsEachEdgeIndependently)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  UI_LAYOUTBOX_INSETS insets = UI_CSS_UnitTests_MakeInsets(true, 5.0, false, 0.0, true, 15.0, false, 0.0);
  box->SetInsets(insets);

  UI_LAYOUTBOX_INSETS& stored = box->GetInsets();
  EXPECT_TRUE(stored.top.specified);
  EXPECT_EQ(stored.top.value, 5.0);
  EXPECT_FALSE(stored.right.specified);
  EXPECT_TRUE(stored.bottom.specified);
  EXPECT_EQ(stored.bottom.value, 15.0);
  EXPECT_FALSE(stored.left.specified);

  GEN_DELETE box;
}


// -- UI_LAYOUTENGINE::ApplyPositioning (fourth increment, second sub-step) -----------------------------------------

TEST(UI_LayoutEngine, ApplyPositioningIgnoresANullRoot)
{
  UI_LAYOUTENGINE::ApplyPositioning(NULL);   // must not crash
}


TEST(UI_LayoutEngine, ApplyPositioningLeavesAStaticBoxExactlyWhereItWas)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();
  box->SetContentBox(10.0, 20.0, 100.0, 50.0);

  UI_LAYOUTBOX_INSETS insets = UI_CSS_UnitTests_MakeInsets(true, 999.0, true, 999.0, true, 999.0, true, 999.0);
  box->SetInsets(insets);   // insets on a STATIC box are simply not applied

  UI_LAYOUTENGINE::ApplyPositioning(box);

  EXPECT_EQ(box->GetContentLeft(), 10.0);
  EXPECT_EQ(box->GetContentTop(), 20.0);
  EXPECT_EQ(box->GetContentWidth(), 100.0);
  EXPECT_EQ(box->GetContentHeight(), 50.0);

  GEN_DELETE box;
}


TEST(UI_LayoutEngine, ApplyPositioningOffsetsARelativeBoxByLeftAndTopWhenBothAreSpecified)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();
  box->SetContentBox(10.0, 20.0, 100.0, 50.0);
  box->SetPosition(UI_POSITION_RELATIVE);

  UI_LAYOUTBOX_INSETS insets = UI_CSS_UnitTests_MakeInsets(true, 5.0, false, 0.0, false, 0.0, true, 8.0);
  box->SetInsets(insets);

  UI_LAYOUTENGINE::ApplyPositioning(box);

  EXPECT_EQ(box->GetContentLeft(), 18.0);    // 10 + left(8)
  EXPECT_EQ(box->GetContentTop(), 25.0);     // 20 + top(5)
  EXPECT_EQ(box->GetContentWidth(), 100.0);  // size untouched
  EXPECT_EQ(box->GetContentHeight(), 50.0);

  GEN_DELETE box;
}


TEST(UI_LayoutEngine, ApplyPositioningOffsetsARelativeBoxByNegativeRightAndBottomWhenLeftAndTopAreAbsent)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();
  box->SetContentBox(10.0, 20.0, 100.0, 50.0);
  box->SetPosition(UI_POSITION_RELATIVE);

  UI_LAYOUTBOX_INSETS insets = UI_CSS_UnitTests_MakeInsets(false, 0.0, true, 6.0, true, 9.0, false, 0.0);
  box->SetInsets(insets);

  UI_LAYOUTENGINE::ApplyPositioning(box);

  EXPECT_EQ(box->GetContentLeft(), 4.0);     // 10 - right(6)
  EXPECT_EQ(box->GetContentTop(), 11.0);     // 20 - bottom(9)

  GEN_DELETE box;
}


TEST(UI_LayoutEngine, ApplyPositioningPrefersLeftOverRightForARelativeBoxWhenBothAreSpecified)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();
  box->SetContentBox(0.0, 0.0, 100.0, 50.0);
  box->SetPosition(UI_POSITION_RELATIVE);

  UI_LAYOUTBOX_INSETS insets = UI_CSS_UnitTests_MakeInsets(false, 0.0, true, 40.0, false, 0.0, true, 12.0);
  box->SetInsets(insets);

  UI_LAYOUTENGINE::ApplyPositioning(box);

  EXPECT_EQ(box->GetContentLeft(), 12.0);    // left wins over right, per CSS

  GEN_DELETE box;
}


TEST(UI_LayoutEngine, ApplyPositioningPositionsAnAbsoluteBoxAgainstTheNearestPositionedAncestorsPaddingBox)
{
  UI_LAYOUTBOX* root = GEN_NEW UI_LAYOUTBOX();
  root->SetContentBox(0.0, 0.0, 500.0, 500.0);

  UI_LAYOUTBOX* relativeancestor = GEN_NEW UI_LAYOUTBOX();
  relativeancestor->SetContentBox(50.0, 60.0, 300.0, 200.0);
  relativeancestor->SetPadding(4.0, 4.0, 4.0, 4.0);
  relativeancestor->SetPosition(UI_POSITION_RELATIVE);
  root->AddChild(relativeancestor);

  UI_LAYOUTBOX* absolutebox = GEN_NEW UI_LAYOUTBOX();
  absolutebox->SetContentBox(0.0, 0.0, 20.0, 10.0);   // static placement, about to be overridden
  absolutebox->SetPosition(UI_POSITION_ABSOLUTE);

  UI_LAYOUTBOX_INSETS insets = UI_CSS_UnitTests_MakeInsets(true, 3.0, false, 0.0, false, 0.0, true, 7.0);
  absolutebox->SetInsets(insets);
  relativeancestor->AddChild(absolutebox);

  UI_LAYOUTENGINE::ApplyPositioning(root);

  // relativeancestor's padding box left/top = (50-4, 60-4) = (46, 56); absolutebox = that + (left 7, top 3)
  EXPECT_EQ(absolutebox->GetContentLeft(), 53.0);
  EXPECT_EQ(absolutebox->GetContentTop(), 59.0);
  EXPECT_EQ(absolutebox->GetContentWidth(), 20.0);    // size untouched
  EXPECT_EQ(absolutebox->GetContentHeight(), 10.0);

  GEN_DELETE root;
}


TEST(UI_LayoutEngine, ApplyPositioningPositionsAnAbsoluteBoxAgainstTheRootWhenNoAncestorIsPositioned)
{
  UI_LAYOUTBOX* root = GEN_NEW UI_LAYOUTBOX();
  root->SetContentBox(0.0, 0.0, 400.0, 300.0);
  root->SetPadding(2.0, 2.0, 2.0, 2.0);

  UI_LAYOUTBOX* staticparent = GEN_NEW UI_LAYOUTBOX();
  staticparent->SetContentBox(10.0, 10.0, 200.0, 150.0);   // UI_POSITION_STATIC: not a containing block
  root->AddChild(staticparent);

  UI_LAYOUTBOX* absolutebox = GEN_NEW UI_LAYOUTBOX();
  absolutebox->SetContentBox(0.0, 0.0, 30.0, 15.0);
  absolutebox->SetPosition(UI_POSITION_ABSOLUTE);

  UI_LAYOUTBOX_INSETS insets = UI_CSS_UnitTests_MakeInsets(false, 0.0, true, 5.0, false, 0.0, false, 0.0);
  absolutebox->SetInsets(insets);
  staticparent->AddChild(absolutebox);

  UI_LAYOUTENGINE::ApplyPositioning(root);

  // root's padding box left/width = (-2, 404); right-anchored: newleft = -2 + 404 - 5 - 30 = 367
  EXPECT_EQ(absolutebox->GetContentLeft(), 367.0);
  EXPECT_EQ(absolutebox->GetContentTop(), 0.0);   // no top/bottom specified -> static position kept

  GEN_DELETE root;
}


// -- UI_LAYOUTENGINE::WriteBackTree / RunLayout (fourth increment, third and last sub-step) -------------------------

TEST(UI_LayoutEngine, WriteBackTreeIgnoresANullRootOrANullTree)
{
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(element, 1.0, 2.0, 3.0, 4.0);

  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();
  box->SetContentBox(9.0, 9.0, 9.0, 9.0);

  UI_LAYOUTENGINE::WriteBackTree(NULL, box);      // must not crash
  UI_LAYOUTENGINE::WriteBackTree(element, NULL);  // must not crash

  // neither call should have touched the element
  UI_CSSBOX cssbox = UI_CSSBox_Get(element);
  EXPECT_EQ(cssbox.left, 1.0);
  EXPECT_EQ(cssbox.top, 2.0);

  GEN_DELETE box;
  GEN_DELETE element;
}


TEST(UI_LayoutEngine, WriteBackTreeDrivesTheElementsGeometryFromTheBoxsContentBoxThroughUiCssBoxSet)
{
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(element, 0.0, 0.0, 10.0, 10.0);   // starting geometry, about to be overwritten

  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();
  box->SetContentBox(40.0, 25.0, 120.0, 60.0);

  UI_LAYOUTENGINE::WriteBackTree(element, box);

  UI_CSSBOX cssbox = UI_CSSBox_Get(element);
  EXPECT_EQ(cssbox.left, 40.0);
  EXPECT_EQ(cssbox.top, 25.0);
  EXPECT_EQ(cssbox.width, 120.0);
  EXPECT_EQ(cssbox.height, 60.0);

  GEN_DELETE box;
  GEN_DELETE element;
}


TEST(UI_LayoutEngine, WriteBackTreeWritesBackToChildrenInComposeOrderEvenWithAPositionedOffset)
{
  UI_ELEMENT* parent = GEN_NEW UI_ELEMENT();
  UI_ELEMENT* first   = GEN_NEW UI_ELEMENT();
  UI_ELEMENT* second  = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(parent, 0.0, 0.0, 300.0, 200.0);
  UI_CSS_UnitTests_SetElementBox(first,  10.0, 10.0, 100.0, 50.0);
  UI_CSS_UnitTests_SetElementBox(second, 10.0, 70.0, 100.0, 50.0);
  first->SetFather(parent);
  second->SetFather(parent);
  parent->GetComposeElements()->Add(first);
  parent->GetComposeElements()->Add(second);

  UI_LAYOUTBOX* tree = UI_LAYOUTENGINE::BuildTree(parent);
  ASSERT_TRUE(tree != NULL);

  // reposition just the SECOND child, by hand, to prove the pairing survives an actual ApplyPositioning() pass
  UI_LAYOUTBOX* secondbox = tree->GetChildren().Get(1);
  ASSERT_TRUE(secondbox != NULL);
  secondbox->SetPosition(UI_POSITION_RELATIVE);
  UI_LAYOUTBOX_INSETS insets = UI_CSS_UnitTests_MakeInsets(true, 5.0, false, 0.0, false, 0.0, true, 2.0);
  secondbox->SetInsets(insets);

  UI_LAYOUTENGINE::ApplyPositioning(tree);
  UI_LAYOUTENGINE::WriteBackTree(parent, tree);

  UI_CSSBOX firstcssbox  = UI_CSSBox_Get(first);
  UI_CSSBOX secondcssbox = UI_CSSBox_Get(second);

  EXPECT_EQ(firstcssbox.left, 10.0);    // untouched (STATIC): round-trips unchanged
  EXPECT_EQ(firstcssbox.top, 10.0);
  EXPECT_EQ(secondcssbox.left, 12.0);   // 10 + left(2)
  EXPECT_EQ(secondcssbox.top, 75.0);    // 70 + top(5)

  GEN_DELETE tree;
  GEN_DELETE parent;
}


TEST(UI_LayoutEngine, RunLayoutIgnoresANullRoot)
{
  UI_LAYOUTENGINE::RunLayout(NULL, UI_LAYOUTSTRATEGY_CSS);      // must not crash
  UI_LAYOUTENGINE::RunLayout(NULL, UI_LAYOUTSTRATEGY_LEGACY);   // must not crash
}


TEST(UI_LayoutEngine, RunLayoutWithLegacyStrategyLeavesTheElementsGeometryCompletelyUntouched)
{
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(element, 15.0, 25.0, 80.0, 40.0);

  UI_LAYOUTENGINE::RunLayout(element, UI_LAYOUTSTRATEGY_LEGACY);

  UI_CSSBOX cssbox = UI_CSSBox_Get(element);
  EXPECT_EQ(cssbox.left, 15.0);
  EXPECT_EQ(cssbox.top, 25.0);
  EXPECT_EQ(cssbox.width, 80.0);
  EXPECT_EQ(cssbox.height, 40.0);

  GEN_DELETE element;
}


TEST(UI_LayoutEngine, RunLayoutWithCssStrategyRoundTripsAPurelyStaticTreeUnchangedThroughUiCssBoxSet)
{
  UI_ELEMENT* parent = GEN_NEW UI_ELEMENT();
  UI_ELEMENT* child   = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(parent, 0.0, 0.0, 300.0, 200.0);
  UI_CSS_UnitTests_SetElementBox(child, 20.0, 30.0, 100.0, 50.0);
  child->SetFather(parent);
  parent->GetComposeElements()->Add(child);

  UI_LAYOUTENGINE::RunLayout(parent, UI_LAYOUTSTRATEGY_CSS);

  // every box BuildTree() produces defaults to UI_POSITION_STATIC, so ApplyPositioning() is a no-op here --
  // this proves the full BuildTree()->ApplyPositioning()->WriteBackTree() pipeline round-trips identity,
  // i.e. UI_CSSBox_Set() really is being driven from the new engine's output now (not skipped/bypassed).
  UI_CSSBOX parentcssbox = UI_CSSBox_Get(parent);
  UI_CSSBOX childcssbox  = UI_CSSBox_Get(child);
  EXPECT_EQ(parentcssbox.left, 0.0);
  EXPECT_EQ(parentcssbox.width, 300.0);
  EXPECT_EQ(childcssbox.left, 20.0);
  EXPECT_EQ(childcssbox.top, 30.0);

  GEN_DELETE parent;
}


// -- UI_LAYOUTBOX: flex container properties (Phase 4, first increment, first sub-step) -----------------------------

TEST(UI_LayoutBox, DefaultsToNotAFlexContainerWithRowDirectionFlexStartJustifyAndZeroGap)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  EXPECT_FALSE(box->IsFlexContainer());
  EXPECT_EQ(box->GetFlexDirection(), UI_FLEX_DIRECTION_ROW);
  EXPECT_EQ(box->GetJustifyContent(), UI_JUSTIFY_CONTENT_FLEX_START);
  EXPECT_EQ(box->GetRowGap(), 0.0);
  EXPECT_EQ(box->GetColumnGap(), 0.0);

  GEN_DELETE box;
}


TEST(UI_LayoutBox, SetFlexContainerRoundTrips)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  box->SetFlexContainer(true);
  EXPECT_TRUE(box->IsFlexContainer());

  box->SetFlexContainer(false);
  EXPECT_FALSE(box->IsFlexContainer());

  GEN_DELETE box;
}


TEST(UI_LayoutBox, SetFlexDirectionRoundTrips)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  box->SetFlexDirection(UI_FLEX_DIRECTION_COLUMN_REVERSE);
  EXPECT_EQ(box->GetFlexDirection(), UI_FLEX_DIRECTION_COLUMN_REVERSE);

  GEN_DELETE box;
}


TEST(UI_LayoutBox, SetJustifyContentRoundTrips)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  box->SetJustifyContent(UI_JUSTIFY_CONTENT_SPACE_EVENLY);
  EXPECT_EQ(box->GetJustifyContent(), UI_JUSTIFY_CONTENT_SPACE_EVENLY);

  GEN_DELETE box;
}


TEST(UI_LayoutBox, SetGapRoundTripsRowAndColumnIndependently)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  box->SetGap(7.0, 3.0);
  EXPECT_EQ(box->GetRowGap(), 7.0);
  EXPECT_EQ(box->GetColumnGap(), 3.0);

  GEN_DELETE box;
}


// -- UI_LAYOUTENGINE::ApplyFlexLayout (Phase 4, first increment, first sub-step) -------------------------------------

TEST(UI_LayoutEngine, ApplyFlexLayoutIgnoresANullRoot)
{
  UI_LAYOUTENGINE::ApplyFlexLayout(NULL);   // must not crash
}


TEST(UI_LayoutEngine, ApplyFlexLayoutLeavesANonFlexContainersChildrenUntouched)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);   // NOT a flex container (default)

  UI_LAYOUTBOX* child = GEN_NEW UI_LAYOUTBOX();
  child->SetContentBox(123.0, 45.0, 50.0, 40.0);
  container->AddChild(child);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  EXPECT_EQ(child->GetContentLeft(), 123.0);
  EXPECT_EQ(child->GetContentTop(), 45.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutPacksRowChildrenLeftToRightWithFlexStartAndColumnGap)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetFlexContainer(true);
  container->SetGap(0.0, 10.0);   // column-gap = 10, is the MAIN-axis gap for a ROW container

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* third  = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(999.0, 999.0, 50.0, 40.0);
  second->SetContentBox(999.0, 999.0, 50.0, 40.0);
  third->SetContentBox(999.0, 999.0, 50.0, 40.0);
  container->AddChild(first);
  container->AddChild(second);
  container->AddChild(third);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  EXPECT_EQ(first->GetContentLeft(), 0.0);
  EXPECT_EQ(second->GetContentLeft(), 60.0);    // 0 + 50 + gap(10)
  EXPECT_EQ(third->GetContentLeft(), 120.0);    // 60 + 50 + gap(10)
  EXPECT_EQ(first->GetContentTop(), 0.0);       // cross axis: container's content top (flex-start)
  EXPECT_EQ(first->GetContentWidth(), 50.0);    // sizes never touched
  EXPECT_EQ(first->GetContentHeight(), 40.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutPacksRowChildrenRightAlignedWithFlexEnd)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetFlexContainer(true);
  container->SetJustifyContent(UI_JUSTIFY_CONTENT_FLEX_END);
  container->SetGap(0.0, 10.0);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* third  = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 50.0, 40.0);
  second->SetContentBox(0.0, 0.0, 50.0, 40.0);
  third->SetContentBox(0.0, 0.0, 50.0, 40.0);
  container->AddChild(first);
  container->AddChild(second);
  container->AddChild(third);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // total children main = 150 + 2*10 = 170; free = 130 -> starts at 130
  EXPECT_EQ(first->GetContentLeft(), 130.0);
  EXPECT_EQ(second->GetContentLeft(), 190.0);
  EXPECT_EQ(third->GetContentLeft(), 250.0);
  EXPECT_EQ(third->GetContentLeft() + third->GetContentWidth(), 300.0);   // last child flush with container's right edge

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutCentersRowChildrenWithCenterJustify)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetFlexContainer(true);
  container->SetJustifyContent(UI_JUSTIFY_CONTENT_CENTER);
  container->SetGap(0.0, 10.0);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* third  = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 50.0, 40.0);
  second->SetContentBox(0.0, 0.0, 50.0, 40.0);
  third->SetContentBox(0.0, 0.0, 50.0, 40.0);
  container->AddChild(first);
  container->AddChild(second);
  container->AddChild(third);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // free = 130 -> half = 65
  EXPECT_EQ(first->GetContentLeft(), 65.0);
  EXPECT_EQ(second->GetContentLeft(), 125.0);
  EXPECT_EQ(third->GetContentLeft(), 185.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutDistributesFreeSpaceBetweenChildrenWithSpaceBetween)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetFlexContainer(true);
  container->SetJustifyContent(UI_JUSTIFY_CONTENT_SPACE_BETWEEN);
  container->SetGap(0.0, 10.0);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* third  = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 50.0, 40.0);
  second->SetContentBox(0.0, 0.0, 50.0, 40.0);
  third->SetContentBox(0.0, 0.0, 50.0, 40.0);
  container->AddChild(first);
  container->AddChild(second);
  container->AddChild(third);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // free = 130, spacing = gap(10) + 130/2 = 75
  EXPECT_EQ(first->GetContentLeft(), 0.0);
  EXPECT_EQ(second->GetContentLeft(), 125.0);
  EXPECT_EQ(third->GetContentLeft(), 250.0);
  EXPECT_EQ(third->GetContentLeft() + third->GetContentWidth(), 300.0);   // last child flush with the far edge too

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutDistributesFreeSpaceAroundChildrenWithSpaceAround)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 200.0, 100.0);
  container->SetFlexContainer(true);
  container->SetJustifyContent(UI_JUSTIFY_CONTENT_SPACE_AROUND);
  container->SetGap(0.0, 10.0);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 50.0, 40.0);
  second->SetContentBox(0.0, 0.0, 50.0, 40.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // total = 50+50+10 = 110; free = 90; spacing = 10 + 90/2 = 55; leading = spacing/2 = 27.5
  EXPECT_EQ(first->GetContentLeft(), 27.5);
  EXPECT_EQ(second->GetContentLeft(), 132.5);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutDistributesFreeSpaceEvenlyWithSpaceEvenly)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 190.0, 100.0);
  container->SetFlexContainer(true);
  container->SetJustifyContent(UI_JUSTIFY_CONTENT_SPACE_EVENLY);
  // no gap -- isolates the SPACE_EVENLY distribution itself from gap addition

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* third  = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 50.0, 40.0);
  second->SetContentBox(0.0, 0.0, 50.0, 40.0);
  third->SetContentBox(0.0, 0.0, 50.0, 40.0);
  container->AddChild(first);
  container->AddChild(second);
  container->AddChild(third);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // total = 150; free = 40; spacing = 40/4 = 10 (four equal gaps: before/between/between/after)
  EXPECT_EQ(first->GetContentLeft(), 10.0);
  EXPECT_EQ(second->GetContentLeft(), 70.0);
  EXPECT_EQ(third->GetContentLeft(), 130.0);
  EXPECT_EQ(third->GetContentLeft() + third->GetContentWidth() + 10.0, 190.0);   // trailing gap too

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutPacksColumnChildrenTopToBottomUsingRowGap)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 50.0, 100.0);
  container->SetFlexContainer(true);
  container->SetFlexDirection(UI_FLEX_DIRECTION_COLUMN);
  container->SetGap(5.0, 0.0);   // row-gap = 5, is the MAIN-axis gap for a COLUMN container

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 50.0, 40.0);
  second->SetContentBox(0.0, 0.0, 50.0, 40.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  EXPECT_EQ(first->GetContentTop(), 0.0);
  EXPECT_EQ(second->GetContentTop(), 45.0);    // 0 + 40 + gap(5)
  EXPECT_EQ(first->GetContentLeft(), 0.0);     // cross axis: container's content left (flex-start)

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutReversesVisualOrderForRowReverse)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 200.0, 100.0);
  container->SetFlexContainer(true);
  container->SetFlexDirection(UI_FLEX_DIRECTION_ROW_REVERSE);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();   // compose-order first, width 50
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();   // compose-order second, width 70
  first->SetContentBox(0.0, 0.0, 50.0, 40.0);
  second->SetContentBox(0.0, 0.0, 70.0, 40.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // ROW_REVERSE packs from the main-axis start but visits children in REVERSE compose order: "second" (the
  // LAST compose child) is placed FIRST, flush with the container's left edge.
  EXPECT_EQ(second->GetContentLeft(), 0.0);
  EXPECT_EQ(first->GetContentLeft(), 70.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutAlignsChildrenToTheContainersCrossStartAccountingForTheirOwnMarginBorderPadding)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 200.0, 100.0);
  container->SetFlexContainer(true);

  UI_LAYOUTBOX* child = GEN_NEW UI_LAYOUTBOX();
  child->SetContentBox(0.0, 0.0, 50.0, 40.0);
  child->SetMargin(5.0, 0.0, 0.0, 0.0);
  child->SetBorder(2.0, 0.0, 0.0, 0.0);
  child->SetPadding(3.0, 0.0, 0.0, 0.0);
  container->AddChild(child);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // cross axis (top, ROW direction) = container content top (0) + child's own leading margin+border+padding
  EXPECT_EQ(child->GetContentTop(), 10.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutIncludesMarginBorderPaddingInEachChildsOuterMainSize)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetFlexContainer(true);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 50.0, 40.0);
  first->SetMargin(0.0, 3.0, 0.0, 3.0);     // +6 outer width
  first->SetBorder(0.0, 1.0, 0.0, 1.0);     // +2 outer width
  first->SetPadding(0.0, 2.0, 0.0, 2.0);    // +4 outer width -- outer width = 50+6+2+4 = 62

  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  second->SetContentBox(0.0, 0.0, 50.0, 40.0);

  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  EXPECT_EQ(first->GetContentLeft(), 6.0);      // leading margin(3)+border(1)+padding(2) on the left
  EXPECT_EQ(second->GetContentLeft(), 62.0);    // first's full outer width (62), no gap set

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutRecursesIntoNestedFlexContainers)
{
  UI_LAYOUTBOX* outer = GEN_NEW UI_LAYOUTBOX();
  outer->SetContentBox(0.0, 0.0, 300.0, 200.0);
  outer->SetFlexContainer(true);

  UI_LAYOUTBOX* inner = GEN_NEW UI_LAYOUTBOX();   // itself a flex item of "outer" AND a flex container for its own children
  inner->SetContentBox(0.0, 0.0, 100.0, 150.0);
  inner->SetFlexContainer(true);
  inner->SetFlexDirection(UI_FLEX_DIRECTION_COLUMN);
  outer->AddChild(inner);

  UI_LAYOUTBOX* grandchildfirst  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* grandchildsecond = GEN_NEW UI_LAYOUTBOX();
  grandchildfirst->SetContentBox(0.0, 0.0, 40.0, 30.0);
  grandchildsecond->SetContentBox(0.0, 0.0, 40.0, 30.0);
  inner->AddChild(grandchildfirst);
  inner->AddChild(grandchildsecond);

  UI_LAYOUTENGINE::ApplyFlexLayout(outer);

  // outer has a single flex item ("inner"): flex-start -> stays at outer's content origin
  EXPECT_EQ(inner->GetContentLeft(), 0.0);
  EXPECT_EQ(inner->GetContentTop(), 0.0);

  // inner is itself a COLUMN flex container: its two children stack top to bottom, no gap
  EXPECT_EQ(grandchildfirst->GetContentTop(), 0.0);
  EXPECT_EQ(grandchildsecond->GetContentTop(), 30.0);

  GEN_DELETE outer;
}


// -- UI_LAYOUTBOX: flex ITEM properties (Phase 4, first increment, second sub-step) -----------------------------------

TEST(UI_LayoutBox, DefaultsToZeroFlexGrowOneFlexShrinkAndAutoFlexBasis)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  EXPECT_EQ(box->GetFlexGrow(), 0.0);
  EXPECT_EQ(box->GetFlexShrink(), 1.0);
  EXPECT_FALSE(box->GetFlexBasis().specified);

  GEN_DELETE box;
}


TEST(UI_LayoutBox, SetFlexGrowRoundTrips)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  box->SetFlexGrow(2.5);
  EXPECT_EQ(box->GetFlexGrow(), 2.5);

  GEN_DELETE box;
}


TEST(UI_LayoutBox, SetFlexShrinkRoundTrips)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  box->SetFlexShrink(0.0);
  EXPECT_EQ(box->GetFlexShrink(), 0.0);

  GEN_DELETE box;
}


TEST(UI_LayoutBox, SetFlexBasisRoundTripsAndSetFlexBasisAutoClearsIt)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  box->SetFlexBasis(75.0);
  EXPECT_TRUE(box->GetFlexBasis().specified);
  EXPECT_EQ(box->GetFlexBasis().value, 75.0);

  box->SetFlexBasisAuto();
  EXPECT_FALSE(box->GetFlexBasis().specified);

  GEN_DELETE box;
}


// -- UI_LAYOUTENGINE::ApplyFlexLayout: flex-grow/flex-shrink/flex-basis (Phase 4, first increment, second sub-step) --

TEST(UI_LayoutEngine, ApplyFlexLayoutGrowsASingleChildToFillAllFreeSpaceWhenItIsTheOnlyGrowItem)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetFlexContainer(true);

  UI_LAYOUTBOX* child = GEN_NEW UI_LAYOUTBOX();
  child->SetContentBox(0.0, 0.0, 50.0, 40.0);
  child->SetFlexGrow(1.0);
  container->AddChild(child);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  EXPECT_EQ(child->GetContentWidth(), 300.0);   // grows to fill the whole container
  EXPECT_EQ(child->GetContentLeft(), 0.0);
  EXPECT_EQ(child->GetContentHeight(), 40.0);   // cross size untouched

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutDistributesGrowthProportionallyAmongMultipleGrowItems)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetFlexContainer(true);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 50.0, 40.0);
  first->SetFlexGrow(1.0);
  second->SetContentBox(0.0, 0.0, 50.0, 40.0);
  second->SetFlexGrow(3.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // total base = 100; free = 200; split 1:3 -> first gets 50, second gets 150
  EXPECT_EQ(first->GetContentWidth(), 100.0);
  EXPECT_EQ(second->GetContentWidth(), 200.0);
  EXPECT_EQ(first->GetContentLeft(), 0.0);
  EXPECT_EQ(second->GetContentLeft(), 100.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutLeavesSizesUnchangedWhenNoChildHasFlexGrow)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetFlexContainer(true);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 50.0, 40.0);    // flex-grow defaults to 0
  second->SetContentBox(0.0, 0.0, 50.0, 40.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  EXPECT_EQ(first->GetContentWidth(), 50.0);     // untouched: leftover space is justify-content's job, not growth
  EXPECT_EQ(second->GetContentWidth(), 50.0);
  EXPECT_EQ(first->GetContentLeft(), 0.0);       // flex-start default: packed at the start, free space trails
  EXPECT_EQ(second->GetContentLeft(), 50.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutShrinksChildrenByScaledShrinkFactorOnOverflow)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 150.0, 100.0);
  container->SetFlexContainer(true);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 100.0, 40.0);    // flex-shrink defaults to 1
  second->SetContentBox(0.0, 0.0, 100.0, 40.0);
  second->SetFlexShrink(3.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // base total = 200, overflow = 50; scaled factors: first = 1*100=100, second = 3*100=300, total = 400
  // first loses 50*(100/400)=12.5 -> 87.5; second loses 50*(300/400)=37.5 -> 62.5
  EXPECT_EQ(first->GetContentWidth(), 87.5);
  EXPECT_EQ(second->GetContentWidth(), 62.5);
  EXPECT_EQ(first->GetContentLeft() , 0.0);
  EXPECT_EQ(second->GetContentLeft(), 87.5);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutFlexShrinkZeroKeepsAnItemAtItsBaseSizeWhileSiblingAbsorbsAllOverflow)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 150.0, 100.0);
  container->SetFlexContainer(true);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 100.0, 40.0);
  first->SetFlexShrink(0.0);
  second->SetContentBox(0.0, 0.0, 100.0, 40.0);   // flex-shrink defaults to 1
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  EXPECT_EQ(first->GetContentWidth(), 100.0);     // shrink 0 -> completely unaffected by overflow
  EXPECT_EQ(second->GetContentWidth(), 50.0);     // absorbs the whole 50 of overflow alone

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutNeverShrinksAnItemsSizeBelowZero)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 0.0, 100.0);   // extreme overflow: no room at all
  container->SetFlexContainer(true);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 10.0, 40.0);
  first->SetFlexShrink(10.0);     // much bigger scaled-shrink share than "second"
  second->SetContentBox(0.0, 0.0, 100.0, 40.0);
  second->SetFlexShrink(1.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  EXPECT_EQ(first->GetContentWidth(), 0.0);    // its computed share would go negative -> clamped to 0
  EXPECT_GE(second->GetContentWidth(), 0.0);   // never negative either, whatever the (undersized) remaining total is

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutFlexBasisOverridesContentWidthAsTheStartingSize)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetFlexContainer(true);

  UI_LAYOUTBOX* child = GEN_NEW UI_LAYOUTBOX();
  child->SetContentBox(0.0, 0.0, 20.0, 40.0);   // content width 20, but...
  child->SetFlexBasis(80.0);                    // ...flex-basis says the starting size is 80, not 20
  container->AddChild(child);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // no flex-grow set (defaults to 0) -> final size is exactly the basis, proving 20 (content width) was ignored
  EXPECT_EQ(child->GetContentWidth(), 80.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutAppliesGrowAlongTheMainAxisWhenDirectionIsColumn)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 80.0, 200.0);
  container->SetFlexContainer(true);
  container->SetFlexDirection(UI_FLEX_DIRECTION_COLUMN);

  UI_LAYOUTBOX* child = GEN_NEW UI_LAYOUTBOX();
  child->SetContentBox(0.0, 0.0, 60.0, 50.0);
  child->SetFlexGrow(1.0);
  container->AddChild(child);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  EXPECT_EQ(child->GetContentHeight(), 200.0);   // grows along the COLUMN main axis (height)
  EXPECT_EQ(child->GetContentWidth(), 60.0);     // cross size (width) untouched

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutOnlyGrowsTheItemThatHasAPositiveFlexGrow)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetFlexContainer(true);

  UI_LAYOUTBOX* fixed  = GEN_NEW UI_LAYOUTBOX();
  fixed->SetFlexBasis(60.0);        // explicit, fixed size -- flex-grow stays 0 (default)

  UI_LAYOUTBOX* growing = GEN_NEW UI_LAYOUTBOX();
  growing->SetContentBox(0.0, 0.0, 40.0, 40.0);   // auto basis -> 40
  growing->SetFlexGrow(1.0);

  container->AddChild(fixed);
  container->AddChild(growing);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  EXPECT_EQ(fixed->GetContentWidth(), 60.0);        // exactly its basis, untouched
  EXPECT_EQ(growing->GetContentWidth(), 240.0);     // 40 base + the full 200 of free space
  EXPECT_EQ(fixed->GetContentLeft(), 0.0);
  EXPECT_EQ(growing->GetContentLeft(), 60.0);

  GEN_DELETE container;
}


TEST(UI_LayoutBox, FlexWrapAndAlignContentDefaultToNowrapAndFlexStart)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  EXPECT_EQ(box->GetFlexWrap(), UI_FLEX_WRAP_NOWRAP);
  EXPECT_EQ(box->GetAlignContent(), UI_ALIGN_CONTENT_FLEX_START);

  GEN_DELETE box;
}


TEST(UI_LayoutBox, FlexWrapAndAlignContentRoundTripThroughSettersAndGetters)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  box->SetFlexWrap(UI_FLEX_WRAP_WRAP_REVERSE);
  box->SetAlignContent(UI_ALIGN_CONTENT_SPACE_EVENLY);

  EXPECT_EQ(box->GetFlexWrap(), UI_FLEX_WRAP_WRAP_REVERSE);
  EXPECT_EQ(box->GetAlignContent(), UI_ALIGN_CONTENT_SPACE_EVENLY);

  GEN_DELETE box;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutWithNowrapNeverSplitsIntoMultipleLinesEvenOnOverflow)
{
  // Regression guard: the WRAP-branch line-splitting code must never run when flex-wrap is NOWRAP,
  // no matter how much the children overflow the container's main size.
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 100.0, 50.0);
  container->SetFlexContainer(true);   // flex-wrap defaults to NOWRAP

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 80.0, 20.0);
  first->SetFlexShrink(0.0);    // keep its outer main size fixed so the overflow is real, not shrunk away
  second->SetContentBox(0.0, 0.0, 80.0, 20.0);
  second->SetFlexShrink(0.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // Both children stay on the single line: same content-top (cross position), placed one after another on X.
  EXPECT_EQ(first->GetContentTop(), 0.0);
  EXPECT_EQ(second->GetContentTop(), 0.0);
  EXPECT_EQ(first->GetContentLeft(), 0.0);
  EXPECT_EQ(second->GetContentLeft(), 80.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutWithWrapSplitsChildrenOntoANewLineWhenTheyOverflowTheMainSize)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 100.0, 200.0);
  container->SetFlexContainer(true);
  container->SetFlexWrap(UI_FLEX_WRAP_WRAP);

  // Three 60-wide children in a 100-wide container: only one fits per line -> three lines.
  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* third  = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 60.0, 30.0);
  first->SetFlexShrink(0.0);    // keep outer main sizes fixed so 3x60=180 truly overflows the 100-wide container
  second->SetContentBox(0.0, 0.0, 60.0, 40.0);
  second->SetFlexShrink(0.0);
  third->SetContentBox(0.0, 0.0, 60.0, 20.0);
  third->SetFlexShrink(0.0);
  container->AddChild(first);
  container->AddChild(second);
  container->AddChild(third);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // Each on its own line, all starting at main-axis 0 (only one item per line fits).
  EXPECT_EQ(first->GetContentLeft(), 0.0);
  EXPECT_EQ(second->GetContentLeft(), 0.0);
  EXPECT_EQ(third->GetContentLeft(), 0.0);

  // Lines stack down the cross axis: line cross-size = tallest item on that line (each line has one item here).
  EXPECT_EQ(first->GetContentTop(), 0.0);
  EXPECT_EQ(second->GetContentTop(), 30.0);    // after first line's cross size (30)
  EXPECT_EQ(third->GetContentTop(), 70.0);     // after first (30) + second (40)

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutWithWrapAndAlignContentCenterCentersLinesOnTheCrossAxis)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 100.0, 300.0);   // plenty of extra cross space (height)
  container->SetFlexContainer(true);
  container->SetFlexWrap(UI_FLEX_WRAP_WRAP);
  container->SetAlignContent(UI_ALIGN_CONTENT_CENTER);

  // Two 60-wide children -> two lines, each 50 tall -> total lines cross size = 100, free space = 200.
  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 60.0, 50.0);
  first->SetFlexShrink(0.0);    // keep both items at outer main size 60 so 60+60=120 overflows the 100-wide line
  second->SetContentBox(0.0, 0.0, 60.0, 50.0);
  second->SetFlexShrink(0.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // Centered: leading offset = free space / 2 = 100.
  EXPECT_EQ(first->GetContentTop(), 100.0);
  EXPECT_EQ(second->GetContentTop(), 150.0);   // first line's top (100) + its cross size (50)

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutWithWrapAndAlignContentSpaceBetweenPushesFirstAndLastLinesToTheEdges)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 100.0, 300.0);
  container->SetFlexContainer(true);
  container->SetFlexWrap(UI_FLEX_WRAP_WRAP);
  container->SetAlignContent(UI_ALIGN_CONTENT_SPACE_BETWEEN);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 60.0, 50.0);
  first->SetFlexShrink(0.0);    // keep both items at outer main size 60 so 60+60=120 overflows the 100-wide line
  second->SetContentBox(0.0, 0.0, 60.0, 50.0);
  second->SetFlexShrink(0.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // First line at the cross-start, last line pushed all the way to the cross-end (300 - 50 = 250).
  EXPECT_EQ(first->GetContentTop(), 0.0);
  EXPECT_EQ(second->GetContentTop(), 250.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutWithWrapReverseStacksLinesInReverseCrossOrder)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 100.0, 200.0);
  container->SetFlexContainer(true);
  container->SetFlexWrap(UI_FLEX_WRAP_WRAP_REVERSE);

  // Three 60-wide children -> one per line, same as the earlier WRAP test, but stacking order reverses.
  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* third  = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 60.0, 30.0);
  first->SetFlexShrink(0.0);    // keep outer main sizes fixed so 3x60=180 truly overflows the 100-wide container
  second->SetContentBox(0.0, 0.0, 60.0, 40.0);
  second->SetFlexShrink(0.0);
  third->SetContentBox(0.0, 0.0, 60.0, 20.0);
  third->SetFlexShrink(0.0);
  container->AddChild(first);
  container->AddChild(second);
  container->AddChild(third);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // Compose order is unchanged (first/second/third stay in that order along the main axis within their own line),
  // but the THIRD line (per compose order) is packed first at the cross-start, then second, then first last.
  EXPECT_EQ(third->GetContentTop(), 0.0);
  EXPECT_EQ(second->GetContentTop(), 20.0);    // after third's line cross size (20)
  EXPECT_EQ(first->GetContentTop(), 60.0);     // after third (20) + second (40)

  GEN_DELETE container;
}


TEST(UI_LayoutBox, AlignItemsAndAlignSelfDefaultToFlexStartAndAuto)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  EXPECT_EQ(box->GetAlignItems(), UI_ALIGN_ITEMS_FLEX_START);
  EXPECT_EQ(box->GetAlignSelf(), UI_ALIGN_SELF_AUTO);

  GEN_DELETE box;
}


TEST(UI_LayoutBox, AlignItemsAndAlignSelfRoundTripThroughSettersAndGetters)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  box->SetAlignItems(UI_ALIGN_ITEMS_STRETCH);
  box->SetAlignSelf(UI_ALIGN_SELF_CENTER);

  EXPECT_EQ(box->GetAlignItems(), UI_ALIGN_ITEMS_STRETCH);
  EXPECT_EQ(box->GetAlignSelf(), UI_ALIGN_SELF_CENTER);

  GEN_DELETE box;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutAlignItemsFlexEndPushesTheChildToTheLinesCrossEnd)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 200.0, 100.0);
  container->SetFlexContainer(true);
  container->SetAlignItems(UI_ALIGN_ITEMS_FLEX_END);

  UI_LAYOUTBOX* child = GEN_NEW UI_LAYOUTBOX();
  child->SetContentBox(0.0, 0.0, 50.0, 40.0);
  container->AddChild(child);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // NOWRAP: the (only) line spans the container's full cross size (100) -- pushed to its end: 100 - 40 = 60.
  EXPECT_EQ(child->GetContentTop(), 60.0);
  EXPECT_EQ(child->GetContentHeight(), 40.0);   // FLEX_END only repositions, never resizes

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutAlignItemsCenterCentersTheChildOnTheCrossAxis)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 200.0, 100.0);
  container->SetFlexContainer(true);
  container->SetAlignItems(UI_ALIGN_ITEMS_CENTER);

  UI_LAYOUTBOX* child = GEN_NEW UI_LAYOUTBOX();
  child->SetContentBox(0.0, 0.0, 50.0, 40.0);
  container->AddChild(child);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  EXPECT_EQ(child->GetContentTop(), 30.0);   // (100 - 40) / 2

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutAlignItemsStretchGrowsTheChildsCrossSizeToFillTheLineMinusItsOwnMarginBorderPadding)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 200.0, 100.0);
  container->SetFlexContainer(true);
  container->SetAlignItems(UI_ALIGN_ITEMS_STRETCH);

  UI_LAYOUTBOX* child = GEN_NEW UI_LAYOUTBOX();
  child->SetContentBox(0.0, 0.0, 50.0, 40.0);    // content height 40, but STRETCH overrides it
  child->SetMargin(5.0, 0.0, 5.0, 0.0);          // top=5, bottom=5
  child->SetBorder(2.0, 0.0, 2.0, 0.0);          // top=2, bottom=2
  child->SetPadding(3.0, 0.0, 3.0, 0.0);         // top=3, bottom=3
  container->AddChild(child);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  // line cross size (100) - leading MBP (5+2+3=10) - trailing MBP (5+2+3=10) = 80.
  EXPECT_EQ(child->GetContentHeight(), 80.0);
  EXPECT_EQ(child->GetContentTop(), 10.0);       // leading MBP only -- stretch still starts at the cross-start
  EXPECT_EQ(child->GetContentWidth(), 50.0);     // main-axis (width) size is untouched by cross-axis stretch

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutAlignSelfOverridesTheContainersAlignItemsForOneChildOnly)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 200.0, 100.0);
  container->SetFlexContainer(true);
  container->SetAlignItems(UI_ALIGN_ITEMS_FLEX_START);   // container default: stay at the cross-start

  UI_LAYOUTBOX* deferring = GEN_NEW UI_LAYOUTBOX();      // align-self left at AUTO -> follows the container
  UI_LAYOUTBOX* overriding = GEN_NEW UI_LAYOUTBOX();     // explicit align-self -> ignores the container
  deferring->SetContentBox(0.0, 0.0, 50.0, 40.0);
  overriding->SetContentBox(0.0, 0.0, 50.0, 40.0);
  overriding->SetAlignSelf(UI_ALIGN_SELF_CENTER);
  container->AddChild(deferring);
  container->AddChild(overriding);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  EXPECT_EQ(deferring->GetContentTop(), 0.0);     // follows the container's FLEX_START
  EXPECT_EQ(overriding->GetContentTop(), 30.0);   // its own CENTER wins: (100 - 40) / 2

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyFlexLayoutWithWrapAlignItemsCentersAShorterItemWithinItsLinesCrossSize)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 200.0, 200.0);   // wide enough that both children share one line
  container->SetFlexContainer(true);
  container->SetFlexWrap(UI_FLEX_WRAP_WRAP);          // still goes through the line-based path, just as 1 line
  container->SetAlignItems(UI_ALIGN_ITEMS_CENTER);

  UI_LAYOUTBOX* tall   = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* shorter = GEN_NEW UI_LAYOUTBOX();
  tall->SetContentBox(0.0, 0.0, 60.0, 60.0);          // sets this line's cross size (tallest item) to 60
  shorter->SetContentBox(0.0, 0.0, 60.0, 20.0);
  container->AddChild(tall);
  container->AddChild(shorter);

  UI_LAYOUTENGINE::ApplyFlexLayout(container);

  EXPECT_EQ(tall->GetContentTop(), 0.0);       // fills the line's cross size exactly -- centering is a no-op
  EXPECT_EQ(shorter->GetContentTop(), 20.0);   // (60 - 20) / 2, centered within the SAME line's cross size

  GEN_DELETE container;
}


TEST(UI_LayoutBox, GridContainerAndTracksDefaultToNotAGridWithNoTracks)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  EXPECT_FALSE(box->IsGridContainer());
  EXPECT_EQ(box->GetGridColumnTracks().GetSize(), (XDWORD)0);
  EXPECT_EQ(box->GetGridRowTracks().GetSize(), (XDWORD)0);

  GEN_DELETE box;
}


TEST(UI_LayoutBox, GridColumnAndRowTracksRoundTripThroughAddAndClear)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();
  box->SetGridContainer(true);

  UI_GRIDTRACK pxtrack;
  pxtrack.unit  = UI_GRID_TRACK_UNIT_PX;
  pxtrack.value = 120.0;

  UI_GRIDTRACK percenttrack;
  percenttrack.unit  = UI_GRID_TRACK_UNIT_PERCENT;
  percenttrack.value = 50.0;

  box->AddGridColumnTrack(pxtrack);
  box->AddGridColumnTrack(percenttrack);
  box->AddGridRowTrack(pxtrack);

  EXPECT_TRUE(box->IsGridContainer());
  EXPECT_EQ(box->GetGridColumnTracks().GetSize(), (XDWORD)2);
  EXPECT_EQ(box->GetGridColumnTracks().Get(0).unit, UI_GRID_TRACK_UNIT_PX);
  EXPECT_EQ(box->GetGridColumnTracks().Get(1).unit, UI_GRID_TRACK_UNIT_PERCENT);
  EXPECT_EQ(box->GetGridRowTracks().GetSize(), (XDWORD)1);

  box->ClearGridColumnTracks();
  EXPECT_EQ(box->GetGridColumnTracks().GetSize(), (XDWORD)0);

  GEN_DELETE box;
}


TEST(UI_LayoutEngine, ApplyGridLayoutPlacesChildrenInRowMajorOrderAcrossFixedPxColumns)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK column;
  column.unit  = UI_GRID_TRACK_UNIT_PX;
  column.value = 100.0;
  container->AddGridColumnTrack(column);
  container->AddGridColumnTrack(column);
  container->AddGridColumnTrack(column);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* third  = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 50.0, 50.0);
  second->SetContentBox(0.0, 0.0, 50.0, 50.0);
  third->SetContentBox(0.0, 0.0, 50.0, 50.0);
  container->AddChild(first);
  container->AddChild(second);
  container->AddChild(third);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(first->GetContentLeft(), 0.0);
  EXPECT_EQ(second->GetContentLeft(), 100.0);
  EXPECT_EQ(third->GetContentLeft(), 200.0);
  EXPECT_EQ(first->GetContentTop(), 0.0);
  EXPECT_EQ(second->GetContentTop(), 0.0);
  EXPECT_EQ(third->GetContentTop(), 0.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutResolvesPercentTracksRelativeToTheContainersContentSize)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK column;
  column.unit  = UI_GRID_TRACK_UNIT_PERCENT;
  column.value = 50.0;
  container->AddGridColumnTrack(column);
  container->AddGridColumnTrack(column);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 10.0, 10.0);
  second->SetContentBox(0.0, 0.0, 10.0, 10.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(first->GetContentWidth(), 150.0);    // 50% of 300
  EXPECT_EQ(second->GetContentWidth(), 150.0);
  EXPECT_EQ(second->GetContentLeft(), 150.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutWrapsToANewRowWhenChildrenExceedTheColumnCount)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 200.0, 200.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK column;
  column.unit  = UI_GRID_TRACK_UNIT_PX;
  column.value = 100.0;
  container->AddGridColumnTrack(column);
  container->AddGridColumnTrack(column);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* third  = GEN_NEW UI_LAYOUTBOX();     // 3rd child -> wraps to row 1, column 0
  first->SetContentBox(0.0, 0.0, 50.0, 40.0);
  second->SetContentBox(0.0, 0.0, 50.0, 40.0);
  third->SetContentBox(0.0, 0.0, 50.0, 40.0);
  container->AddChild(first);
  container->AddChild(second);
  container->AddChild(third);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(third->GetContentLeft(), 0.0);
  EXPECT_EQ(third->GetContentTop(), 40.0);   // after row 0's implicit height (tallest item there: 40)

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutSizesImplicitRowsToTheTallestItemInThatRow)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 200.0, 300.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK column;
  column.unit  = UI_GRID_TRACK_UNIT_PX;
  column.value = 100.0;
  container->AddGridColumnTrack(column);
  container->AddGridColumnTrack(column);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();     // row 0
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();     // row 0 -- taller, sets the row's implicit height
  UI_LAYOUTBOX* third  = GEN_NEW UI_LAYOUTBOX();     // row 1, alone
  first->SetContentBox(0.0, 0.0, 50.0, 30.0);
  second->SetContentBox(0.0, 0.0, 50.0, 70.0);
  third->SetContentBox(0.0, 0.0, 50.0, 20.0);
  container->AddChild(first);
  container->AddChild(second);
  container->AddChild(third);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(third->GetContentTop(), 70.0);   // row 0's height is its TALLEST item (70), not the first child's (30)

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutUsesExplicitRowTracksInsteadOfContentSizingWhenProvided)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 200.0, 300.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK column;
  column.unit  = UI_GRID_TRACK_UNIT_PX;
  column.value = 100.0;
  container->AddGridColumnTrack(column);
  container->AddGridColumnTrack(column);

  UI_GRIDTRACK explicitrow;
  explicitrow.unit  = UI_GRID_TRACK_UNIT_PX;
  explicitrow.value = 40.0;
  container->AddGridRowTrack(explicitrow);           // row 0 is explicit: 40, regardless of its items' content

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* third  = GEN_NEW UI_LAYOUTBOX();     // row 1: no explicit track -> implicit, sized to content (20)
  first->SetContentBox(0.0, 0.0, 50.0, 999.0);       // huge content height -- must NOT affect the explicit row
  second->SetContentBox(0.0, 0.0, 50.0, 30.0);
  third->SetContentBox(0.0, 0.0, 50.0, 20.0);
  container->AddChild(first);
  container->AddChild(second);
  container->AddChild(third);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(third->GetContentTop(), 40.0);   // row 0's explicit 40, ignoring "first"'s 999-tall content

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutStretchesEachChildToFillItsCellMinusItsOwnMarginBorderPadding)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 100.0, 100.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK column;
  column.unit  = UI_GRID_TRACK_UNIT_PX;
  column.value = 100.0;
  container->AddGridColumnTrack(column);

  UI_GRIDTRACK row;
  row.unit  = UI_GRID_TRACK_UNIT_PX;
  row.value = 100.0;
  container->AddGridRowTrack(row);

  UI_LAYOUTBOX* child = GEN_NEW UI_LAYOUTBOX();
  child->SetContentBox(0.0, 0.0, 10.0, 10.0);    // small content -- STRETCH overrides it to fill the cell
  child->SetMargin(5.0, 0.0, 5.0, 0.0);
  child->SetBorder(2.0, 0.0, 2.0, 0.0);
  child->SetPadding(3.0, 0.0, 3.0, 0.0);
  container->AddChild(child);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  // cell is 100x100; only top/bottom MBP set (10 each side) -> content height = 80; left/right MBP all 0 -> width 100.
  EXPECT_EQ(child->GetContentWidth(), 100.0);
  EXPECT_EQ(child->GetContentHeight(), 80.0);
  EXPECT_EQ(child->GetContentTop(), 10.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutAppliesColumnAndRowGapBetweenTracks)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 300.0);
  container->SetGridContainer(true);
  container->SetGap(5.0, 10.0);    // row-gap 5, column-gap 10

  UI_GRIDTRACK column;
  column.unit  = UI_GRID_TRACK_UNIT_PX;
  column.value = 100.0;
  container->AddGridColumnTrack(column);
  container->AddGridColumnTrack(column);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* third  = GEN_NEW UI_LAYOUTBOX();     // wraps to row 1
  first->SetContentBox(0.0, 0.0, 50.0, 40.0);
  second->SetContentBox(0.0, 0.0, 50.0, 40.0);
  third->SetContentBox(0.0, 0.0, 50.0, 40.0);
  container->AddChild(first);
  container->AddChild(second);
  container->AddChild(third);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(second->GetContentLeft(), 110.0);   // 100 (column 0 width) + 10 (column-gap)
  EXPECT_EQ(third->GetContentTop(), 45.0);      // 40 (row 0 height) + 5 (row-gap)

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutDoesNothingWhenNoColumnTracksAreDefined)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetGridContainer(true);   // no AddGridColumnTrack() calls -- zero columns defined

  UI_LAYOUTBOX* child = GEN_NEW UI_LAYOUTBOX();
  child->SetContentBox(5.0, 5.0, 20.0, 20.0);
  container->AddChild(child);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(child->GetContentLeft(), 5.0);    // untouched
  EXPECT_EQ(child->GetContentTop(), 5.0);
  EXPECT_EQ(child->GetContentWidth(), 20.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutDistributesFreeSpaceAmongFrColumnsProportionally)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 400.0, 100.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK onefr;
  onefr.unit  = UI_GRID_TRACK_UNIT_FR;
  onefr.value = 1.0;

  UI_GRIDTRACK threefr;
  threefr.unit  = UI_GRID_TRACK_UNIT_FR;
  threefr.value = 3.0;

  container->AddGridColumnTrack(onefr);
  container->AddGridColumnTrack(threefr);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 10.0, 10.0);
  second->SetContentBox(0.0, 0.0, 10.0, 10.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(first->GetContentWidth(), 100.0);    // 1/4 of the 400 free space (no fixed tracks to subtract)
  EXPECT_EQ(second->GetContentWidth(), 300.0);   // 3/4
  EXPECT_EQ(second->GetContentLeft(), 100.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutFrColumnsShareLeftoverSpaceAfterFixedPxTracks)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK fixedcol;
  fixedcol.unit  = UI_GRID_TRACK_UNIT_PX;
  fixedcol.value = 100.0;

  UI_GRIDTRACK frcol;
  frcol.unit  = UI_GRID_TRACK_UNIT_FR;
  frcol.value = 1.0;

  container->AddGridColumnTrack(fixedcol);   // fixed: always 100
  container->AddGridColumnTrack(frcol);      // shares whatever's left: 300 - 100 = 200
  container->AddGridColumnTrack(frcol);      // ...split evenly between the two 1fr tracks: 100 each

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* third  = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 10.0, 10.0);
  second->SetContentBox(0.0, 0.0, 10.0, 10.0);
  third->SetContentBox(0.0, 0.0, 10.0, 10.0);
  container->AddChild(first);
  container->AddChild(second);
  container->AddChild(third);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(first->GetContentWidth(), 100.0);
  EXPECT_EQ(second->GetContentWidth(), 100.0);
  EXPECT_EQ(third->GetContentWidth(), 100.0);
  EXPECT_EQ(second->GetContentLeft(), 100.0);
  EXPECT_EQ(third->GetContentLeft(), 200.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutFrColumnsAccountForColumnGapWhenComputingFreeSpace)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetGridContainer(true);
  container->SetGap(0.0, 20.0);   // column-gap 20

  UI_GRIDTRACK frcol;
  frcol.unit  = UI_GRID_TRACK_UNIT_FR;
  frcol.value = 1.0;
  container->AddGridColumnTrack(frcol);
  container->AddGridColumnTrack(frcol);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 10.0, 10.0);
  second->SetContentBox(0.0, 0.0, 10.0, 10.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  // free space = 300 - 20 (the one gap between 2 columns) = 280, split evenly -> 140 each.
  EXPECT_EQ(first->GetContentWidth(), 140.0);
  EXPECT_EQ(second->GetContentWidth(), 140.0);
  EXPECT_EQ(second->GetContentLeft(), 160.0);   // 140 + the 20 gap

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutFrRowsShareLeftoverSpaceAfterExplicitFixedRowsAndImplicitRows)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 100.0, 200.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK column;
  column.unit  = UI_GRID_TRACK_UNIT_PX;
  column.value = 100.0;
  container->AddGridColumnTrack(column);   // 1 column -> each child gets its own row

  UI_GRIDTRACK fixedrow;
  fixedrow.unit  = UI_GRID_TRACK_UNIT_PX;
  fixedrow.value = 50.0;

  UI_GRIDTRACK frrow;
  frrow.unit  = UI_GRID_TRACK_UNIT_FR;
  frrow.value = 1.0;

  container->AddGridRowTrack(fixedrow);    // row 0: fixed 50
  container->AddGridRowTrack(frrow);       // row 1: fr -- shares whatever's left after row 0 AND row 2 (implicit)

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* third  = GEN_NEW UI_LAYOUTBOX();     // row 2: no explicit track -> implicit, sized to its content
  first->SetContentBox(0.0, 0.0, 50.0, 999.0);       // row 0 is fixed -- this huge content height must not matter
  second->SetContentBox(0.0, 0.0, 50.0, 999.0);      // row 1 is fr -- likewise unaffected by its own content
  third->SetContentBox(0.0, 0.0, 50.0, 30.0);        // row 2: implicit, sized to exactly this (30)
  container->AddChild(first);
  container->AddChild(second);
  container->AddChild(third);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  // container height 200; implicit row 2 = 30; row 0 fixed = 50; free space for row 1's 1fr = 200 - 30 - 50 = 120.
  EXPECT_EQ(second->GetContentHeight(), 120.0);
  EXPECT_EQ(second->GetContentTop(), 50.0);     // right after row 0's 50
  EXPECT_EQ(third->GetContentTop(), 170.0);     // 50 + 120
  EXPECT_EQ(third->GetContentHeight(), 30.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutGivesFrColumnsZeroWidthWhenNoFreeSpaceRemains)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 150.0, 100.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK fixedcol;
  fixedcol.unit  = UI_GRID_TRACK_UNIT_PX;
  fixedcol.value = 200.0;   // already overflows the 150-wide container on its own

  UI_GRIDTRACK frcol;
  frcol.unit  = UI_GRID_TRACK_UNIT_FR;
  frcol.value = 1.0;

  container->AddGridColumnTrack(fixedcol);
  container->AddGridColumnTrack(frcol);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 10.0, 10.0);
  second->SetContentBox(0.0, 0.0, 10.0, 10.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(first->GetContentWidth(), 200.0);    // fixed tracks are never clamped, same as before fr existed
  EXPECT_EQ(second->GetContentWidth(), 0.0);     // negative free space -> clamped to 0, never negative

  GEN_DELETE container;
}


TEST(UI_LayoutBox, GridTrackDefaultsToASimpleNonMinmaxPxZeroTrack)
{
  UI_GRIDTRACK track;

  EXPECT_FALSE(track.isminmax);
  EXPECT_EQ(track.unit, UI_GRID_TRACK_UNIT_PX);
  EXPECT_EQ(track.value, 0.0);
}


TEST(UI_LayoutEngine, ApplyGridLayoutMinmaxWithAFixedMaxResolvesToTheMaxWhenItExceedsTheMin)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK column;
  column.isminmax = true;
  column.minunit   = UI_GRID_TRACK_UNIT_PX;
  column.minvalue  = 50.0;
  column.maxunit   = UI_GRID_TRACK_UNIT_PX;
  column.maxvalue  = 150.0;
  container->AddGridColumnTrack(column);

  UI_LAYOUTBOX* child = GEN_NEW UI_LAYOUTBOX();
  child->SetContentBox(0.0, 0.0, 10.0, 10.0);
  container->AddChild(child);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(child->GetContentWidth(), 150.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutMinmaxFloorsTheTrackWhenTheDeclaredMinExceedsTheMax)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK column;
  column.isminmax = true;
  column.minunit   = UI_GRID_TRACK_UNIT_PX;
  column.minvalue  = 200.0;   // deliberately larger than max -- an edge case this engine tolerates (see SCOPE)
  column.maxunit   = UI_GRID_TRACK_UNIT_PX;
  column.maxvalue  = 100.0;
  container->AddGridColumnTrack(column);

  UI_LAYOUTBOX* child = GEN_NEW UI_LAYOUTBOX();
  child->SetContentBox(0.0, 0.0, 10.0, 10.0);
  container->AddChild(child);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(child->GetContentWidth(), 200.0);   // the larger of the two wins, whichever bound it came from

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutMinmaxWithAnFrMaxGrowsBeyondItsFloorToShareFreeSpace)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 400.0, 100.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK growing;
  growing.isminmax = true;
  growing.minunit   = UI_GRID_TRACK_UNIT_PX;
  growing.minvalue  = 100.0;
  growing.maxunit   = UI_GRID_TRACK_UNIT_FR;
  growing.maxvalue  = 1.0;

  UI_GRIDTRACK fixed;
  fixed.unit  = UI_GRID_TRACK_UNIT_PX;
  fixed.value = 100.0;

  container->AddGridColumnTrack(growing);
  container->AddGridColumnTrack(fixed);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 10.0, 10.0);
  second->SetContentBox(0.0, 0.0, 10.0, 10.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  // free space = 400 - 100 (this track's own floor, reserved) - 100 (the fixed column) = 200, all to this 1fr.
  EXPECT_EQ(first->GetContentWidth(), 300.0);    // 100 (floor) + 200 (its fr share)
  EXPECT_EQ(second->GetContentWidth(), 100.0);
  EXPECT_EQ(second->GetContentLeft(), 300.0);

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutMinmaxWithAnFrMaxNeverShrinksBelowItsFloorWhenNoFreeSpaceRemains)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 200.0, 100.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK growing;
  growing.isminmax = true;
  growing.minunit   = UI_GRID_TRACK_UNIT_PX;
  growing.minvalue  = 150.0;
  growing.maxunit   = UI_GRID_TRACK_UNIT_FR;
  growing.maxvalue  = 1.0;

  UI_GRIDTRACK fixed;
  fixed.unit  = UI_GRID_TRACK_UNIT_PX;
  fixed.value = 100.0;   // 150 (this track's floor) + 100 already overflows the 200-wide container

  container->AddGridColumnTrack(growing);
  container->AddGridColumnTrack(fixed);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 10.0, 10.0);
  second->SetContentBox(0.0, 0.0, 10.0, 10.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(first->GetContentWidth(), 150.0);    // its floor, exactly -- 0 free space to add on top
  EXPECT_EQ(second->GetContentWidth(), 100.0);   // fixed tracks are never clamped, same as always

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutTwoMinmaxFrColumnsShareFreeSpaceProportionallyByWeight)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 100.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK onefr;
  onefr.isminmax = true;
  onefr.minunit   = UI_GRID_TRACK_UNIT_PX;
  onefr.minvalue  = 0.0;
  onefr.maxunit   = UI_GRID_TRACK_UNIT_FR;
  onefr.maxvalue  = 1.0;

  UI_GRIDTRACK threefr;
  threefr.isminmax = true;
  threefr.minunit   = UI_GRID_TRACK_UNIT_PX;
  threefr.minvalue  = 0.0;
  threefr.maxunit   = UI_GRID_TRACK_UNIT_FR;
  threefr.maxvalue  = 3.0;

  container->AddGridColumnTrack(onefr);
  container->AddGridColumnTrack(threefr);

  UI_LAYOUTBOX* first  = GEN_NEW UI_LAYOUTBOX();
  UI_LAYOUTBOX* second = GEN_NEW UI_LAYOUTBOX();
  first->SetContentBox(0.0, 0.0, 10.0, 10.0);
  second->SetContentBox(0.0, 0.0, 10.0, 10.0);
  container->AddChild(first);
  container->AddChild(second);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(first->GetContentWidth(), 75.0);     // 1/4 of the 300 free space (both floors are 0)
  EXPECT_EQ(second->GetContentWidth(), 225.0);   // 3/4

  GEN_DELETE container;
}


TEST(UI_LayoutBox, GridColumnAndRowSpanDefaultToOne)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  EXPECT_EQ(box->GetGridColumnSpan(), (XDWORD)1);
  EXPECT_EQ(box->GetGridRowSpan(), (XDWORD)1);

  GEN_DELETE box;
}


TEST(UI_LayoutBox, GridColumnAndRowSpanRoundTripThroughSettersAndGetters)
{
  UI_LAYOUTBOX* box = GEN_NEW UI_LAYOUTBOX();

  box->SetGridColumnSpan(3);
  box->SetGridRowSpan(2);

  EXPECT_EQ(box->GetGridColumnSpan(), (XDWORD)3);
  EXPECT_EQ(box->GetGridRowSpan(), (XDWORD)2);

  GEN_DELETE box;
}


TEST(UI_LayoutEngine, ApplyGridLayoutColumnSpanMakesAnItemOccupyMultipleColumnsAndPushesLaterItemsPastIt)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 300.0, 200.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK column;
  column.unit  = UI_GRID_TRACK_UNIT_PX;
  column.value = 100.0;
  container->AddGridColumnTrack(column);
  container->AddGridColumnTrack(column);
  container->AddGridColumnTrack(column);

  UI_LAYOUTBOX* spanning = GEN_NEW UI_LAYOUTBOX();     // spans columns 0-1
  UI_LAYOUTBOX* sibling  = GEN_NEW UI_LAYOUTBOX();     // pushed to column 2, same row
  UI_LAYOUTBOX* wrapped  = GEN_NEW UI_LAYOUTBOX();     // no room left on row 0 -> wraps to row 1
  spanning->SetContentBox(0.0, 0.0, 10.0, 30.0);
  spanning->SetGridColumnSpan(2);
  sibling->SetContentBox(0.0, 0.0, 10.0, 50.0);
  wrapped->SetContentBox(0.0, 0.0, 10.0, 10.0);
  container->AddChild(spanning);
  container->AddChild(sibling);
  container->AddChild(wrapped);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(spanning->GetContentLeft(), 0.0);
  EXPECT_EQ(spanning->GetContentWidth(), 200.0);   // columns 0 AND 1, no gap between them
  EXPECT_EQ(sibling->GetContentLeft(), 200.0);      // pushed past the span, onto column 2
  EXPECT_EQ(wrapped->GetContentLeft(), 0.0);        // row 0 is full (span 2 + 1 == all 3 columns) -> wraps
  EXPECT_EQ(wrapped->GetContentTop(), 50.0);        // row 0's implicit height = tallest SINGLE-row item (50)

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutRowSpanMakesAnItemOccupyMultipleRowsAndLaterItemsSkipItsOccupiedCells)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 100.0, 300.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK column;
  column.unit  = UI_GRID_TRACK_UNIT_PX;
  column.value = 50.0;
  container->AddGridColumnTrack(column);
  container->AddGridColumnTrack(column);

  UI_LAYOUTBOX* tall   = GEN_NEW UI_LAYOUTBOX();   // (0,0), spans rows 0-1
  UI_LAYOUTBOX* topright = GEN_NEW UI_LAYOUTBOX(); // (0,1)
  UI_LAYOUTBOX* midright = GEN_NEW UI_LAYOUTBOX(); // wants (1,0) -- occupied by "tall" -> goes to (1,1) instead
  UI_LAYOUTBOX* bottom    = GEN_NEW UI_LAYOUTBOX(); // (2,0) -- "tall"'s span has ended by row 2
  tall->SetContentBox(0.0, 0.0, 10.0, 999.0);      // irrelevant -- rowspan items don't drive implicit row sizing
  tall->SetGridRowSpan(2);
  topright->SetContentBox(0.0, 0.0, 10.0, 20.0);
  midright->SetContentBox(0.0, 0.0, 10.0, 30.0);
  bottom->SetContentBox(0.0, 0.0, 10.0, 40.0);
  container->AddChild(tall);
  container->AddChild(topright);
  container->AddChild(midright);
  container->AddChild(bottom);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  // row 0 implicit height = "topright"'s 20 (the only rowspan==1 item on row 0 -- "tall" is excluded).
  // row 1 implicit height = "midright"'s 30 (the only rowspan==1 item on row 1).
  EXPECT_EQ(midright->GetContentLeft(), 50.0);     // pushed to column 1 -- column 0 row 1 is occupied by "tall"
  EXPECT_EQ(midright->GetContentTop(), 20.0);      // row 1 starts right after row 0's height (20)
  EXPECT_EQ(bottom->GetContentLeft(), 0.0);        // "tall"'s span is over -- column 0 is free again on row 2
  EXPECT_EQ(bottom->GetContentTop(), 50.0);        // row 0 (20) + row 1 (30)
  EXPECT_EQ(tall->GetContentHeight(), 50.0);       // spans rows 0+1: 20 + 30, stretched to fill both

  GEN_DELETE container;
}


TEST(UI_LayoutEngine, ApplyGridLayoutClampsAColumnSpanWiderThanTheGridToTheFullColumnCount)
{
  UI_LAYOUTBOX* container = GEN_NEW UI_LAYOUTBOX();
  container->SetContentBox(0.0, 0.0, 200.0, 100.0);
  container->SetGridContainer(true);

  UI_GRIDTRACK column;
  column.unit  = UI_GRID_TRACK_UNIT_PX;
  column.value = 100.0;
  container->AddGridColumnTrack(column);
  container->AddGridColumnTrack(column);

  UI_LAYOUTBOX* child = GEN_NEW UI_LAYOUTBOX();
  child->SetContentBox(0.0, 0.0, 10.0, 10.0);
  child->SetGridColumnSpan(5);   // wider than the 2 defined columns
  container->AddChild(child);

  UI_LAYOUTENGINE::ApplyGridLayout(container);

  EXPECT_EQ(child->GetContentWidth(), 200.0);   // clamped down to both columns, not left unplaced or crashing

  GEN_DELETE container;
}


// -- UI_ELEMENT: Flexbox CSS Lite wiring (Phase 4, "migración del ejemplo", first sub-step) --------------------------

TEST(UI_Element, DefaultsToNotAFlexContainerWithTheSameDefaultsAsAFreshUiLayoutBox)
{
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();

  EXPECT_FALSE(element->IsFlexContainer());
  EXPECT_EQ(element->GetFlexDirection(), UI_FLEX_DIRECTION_ROW);
  EXPECT_EQ(element->GetJustifyContent(), UI_JUSTIFY_CONTENT_FLEX_START);
  EXPECT_EQ(element->GetRowGap(), 0.0);
  EXPECT_EQ(element->GetColumnGap(), 0.0);
  EXPECT_EQ(element->GetFlexWrap(), UI_FLEX_WRAP_NOWRAP);
  EXPECT_EQ(element->GetAlignContent(), UI_ALIGN_CONTENT_FLEX_START);
  EXPECT_EQ(element->GetAlignItems(), UI_ALIGN_ITEMS_FLEX_START);

  GEN_DELETE element;
}

TEST(UI_Element, DefaultsToTheCssInitialFlexItemValues)
{
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();

  EXPECT_EQ(element->GetFlexGrow(), 0.0);
  EXPECT_EQ(element->GetFlexShrink(), 1.0);
  EXPECT_FALSE(element->GetFlexBasis().specified);   // "auto"
  EXPECT_EQ(element->GetAlignSelf(), UI_ALIGN_SELF_AUTO);

  GEN_DELETE element;
}

TEST(UI_Element, FlexContainerPropertiesRoundTripThroughSettersAndGetters)
{
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();

  element->SetFlexContainer(true);
  element->SetFlexDirection(UI_FLEX_DIRECTION_COLUMN_REVERSE);
  element->SetJustifyContent(UI_JUSTIFY_CONTENT_SPACE_EVENLY);
  element->SetGap(4.0, 8.0);
  element->SetFlexWrap(UI_FLEX_WRAP_WRAP_REVERSE);
  element->SetAlignContent(UI_ALIGN_CONTENT_SPACE_AROUND);
  element->SetAlignItems(UI_ALIGN_ITEMS_STRETCH);

  EXPECT_TRUE(element->IsFlexContainer());
  EXPECT_EQ(element->GetFlexDirection(), UI_FLEX_DIRECTION_COLUMN_REVERSE);
  EXPECT_EQ(element->GetJustifyContent(), UI_JUSTIFY_CONTENT_SPACE_EVENLY);
  EXPECT_EQ(element->GetRowGap(), 4.0);
  EXPECT_EQ(element->GetColumnGap(), 8.0);
  EXPECT_EQ(element->GetFlexWrap(), UI_FLEX_WRAP_WRAP_REVERSE);
  EXPECT_EQ(element->GetAlignContent(), UI_ALIGN_CONTENT_SPACE_AROUND);
  EXPECT_EQ(element->GetAlignItems(), UI_ALIGN_ITEMS_STRETCH);

  GEN_DELETE element;
}

TEST(UI_Element, FlexItemPropertiesRoundTripThroughSettersAndGetters)
{
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();

  element->SetFlexGrow(2.0);
  element->SetFlexShrink(0.5);
  element->SetFlexBasis(120.0);
  element->SetAlignSelf(UI_ALIGN_SELF_CENTER);

  EXPECT_EQ(element->GetFlexGrow(), 2.0);
  EXPECT_EQ(element->GetFlexShrink(), 0.5);
  EXPECT_TRUE(element->GetFlexBasis().specified);
  EXPECT_EQ(element->GetFlexBasis().value, 120.0);
  EXPECT_EQ(element->GetAlignSelf(), UI_ALIGN_SELF_CENTER);

  element->SetFlexBasisAuto();
  EXPECT_FALSE(element->GetFlexBasis().specified);

  GEN_DELETE element;
}


// -- UI_LAYOUTENGINE::BuildTree copies Flexbox properties (Phase 4, "migración del ejemplo", first sub-step) ---------

TEST(UI_LayoutEngine, BuildTreeCopiesAnElementsFlexContainerPropertiesOntoTheMirroredBox)
{
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();
  element->SetFlexContainer(true);
  element->SetFlexDirection(UI_FLEX_DIRECTION_COLUMN);
  element->SetJustifyContent(UI_JUSTIFY_CONTENT_CENTER);
  element->SetGap(3.0, 6.0);
  element->SetFlexWrap(UI_FLEX_WRAP_WRAP);
  element->SetAlignContent(UI_ALIGN_CONTENT_CENTER);
  element->SetAlignItems(UI_ALIGN_ITEMS_STRETCH);

  UI_LAYOUTBOX* box = UI_LAYOUTENGINE::BuildTree(element);
  ASSERT_TRUE(box != NULL);

  EXPECT_TRUE(box->IsFlexContainer());
  EXPECT_EQ(box->GetFlexDirection(), UI_FLEX_DIRECTION_COLUMN);
  EXPECT_EQ(box->GetJustifyContent(), UI_JUSTIFY_CONTENT_CENTER);
  EXPECT_EQ(box->GetRowGap(), 3.0);
  EXPECT_EQ(box->GetColumnGap(), 6.0);
  EXPECT_EQ(box->GetFlexWrap(), UI_FLEX_WRAP_WRAP);
  EXPECT_EQ(box->GetAlignContent(), UI_ALIGN_CONTENT_CENTER);
  EXPECT_EQ(box->GetAlignItems(), UI_ALIGN_ITEMS_STRETCH);

  GEN_DELETE box;
  GEN_DELETE element;
}

TEST(UI_LayoutEngine, BuildTreeCopiesAnElementsFlexItemPropertiesOntoTheMirroredBox)
{
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();
  element->SetFlexGrow(2.0);
  element->SetFlexShrink(0.0);
  element->SetFlexBasis(75.0);
  element->SetAlignSelf(UI_ALIGN_SELF_FLEX_END);

  UI_LAYOUTBOX* box = UI_LAYOUTENGINE::BuildTree(element);
  ASSERT_TRUE(box != NULL);

  EXPECT_EQ(box->GetFlexGrow(), 2.0);
  EXPECT_EQ(box->GetFlexShrink(), 0.0);
  EXPECT_TRUE(box->GetFlexBasis().specified);
  EXPECT_EQ(box->GetFlexBasis().value, 75.0);
  EXPECT_EQ(box->GetAlignSelf(), UI_ALIGN_SELF_FLEX_END);

  GEN_DELETE box;
  GEN_DELETE element;
}

TEST(UI_LayoutEngine, BuildTreeCopiesAnAutoFlexBasisAsUnspecified)
{
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();
  element->SetFlexBasis(50.0);    // explicit first...
  element->SetFlexBasisAuto();    // ...then back to "auto", must round-trip through BuildTree() as unspecified

  UI_LAYOUTBOX* box = UI_LAYOUTENGINE::BuildTree(element);
  ASSERT_TRUE(box != NULL);

  EXPECT_FALSE(box->GetFlexBasis().specified);

  GEN_DELETE box;
  GEN_DELETE element;
}

TEST(UI_LayoutEngine, BuildTreeUsesAnElementsIntrinsicSizeAsItsContentBoxWhenSetInsteadOfItsLiveBoundaryLine)
{
  // Footer icon/text gap regression, round 3 (see UI_SKIN::CalculateBoundaryLine()'s and dashboard.xml's own
  // SCOPE ADDENDUM comments for the full story): a flex item's flex-basis:auto "content size" must come from
  // GetIntrinsicWidth()/Height() when set -- NOT from the live BoundaryLine/UI_CSSBox_Get(), which
  // UI_LAYOUTENGINE::WriteBackTree() overwrites with whatever THIS SAME engine last resolved it to. Simulates
  // exactly that: an element whose live box holds a bogus, already-shrunk-looking width (5.0), left over from
  // a PRIOR flex pass this test never ran, while its intrinsic width still correctly says 40.0 (its true,
  // authored/measured size, stamped once and never touched by the flex engine itself).
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();
  UI_BOUNDARYLINE boundaryline;
  boundaryline.width  = 5.0;    // bogus leftover -- must be ignored
  boundaryline.height = 5.0;
  element->SetBoundaryLine(boundaryline);
  element->SetIntrinsicWidth(40.0);     // the element's true size
  element->SetIntrinsicHeight(40.0);

  UI_LAYOUTBOX* box = UI_LAYOUTENGINE::BuildTree(element);
  ASSERT_TRUE(box != NULL);

  EXPECT_EQ(box->GetContentWidth(), 40.0);
  EXPECT_EQ(box->GetContentHeight(), 40.0);

  GEN_DELETE box;
  GEN_DELETE element;
}

TEST(UI_LayoutEngine, BuildTreeFallsBackToTheLiveBoundaryLineWhenIntrinsicSizeWasNeverSet)
{
  // Zero behaviour change for every element that predates this fix (or is never routed through UI_SKIN::
  // CalculateBoundaryLine() -- e.g. every OTHER existing test in this file, which hand-builds elements via
  // UI_CSS_UnitTests_SetElementBox() alone): GetIntrinsicWidth()/Height() stay at their -1.0 "unset" sentinel,
  // so BuildTree() must keep reading the live BoundaryLine exactly as it always did.
  UI_ELEMENT* element = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(element, 0.0, 0.0, 40.0, 40.0);

  UI_LAYOUTBOX* box = UI_LAYOUTENGINE::BuildTree(element);
  ASSERT_TRUE(box != NULL);

  EXPECT_EQ(box->GetContentWidth(), 40.0);
  EXPECT_EQ(box->GetContentHeight(), 40.0);

  GEN_DELETE box;
  GEN_DELETE element;
}


// -- UI_LAYOUTENGINE::RunLayout wires Flexbox into the real pipeline (Phase 4, "migración del ejemplo", first
//    sub-step) --------------------------------------------------------------------------------------------------

TEST(UI_LayoutEngine, RunLayoutWithCssStrategyArrangesAFlexContainersChildrenWithJustifyContentSpaceBetween)
{
  UI_ELEMENT* parent = GEN_NEW UI_ELEMENT();
  UI_ELEMENT* first   = GEN_NEW UI_ELEMENT();
  UI_ELEMENT* second  = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(parent, 0.0, 0.0, 300.0, 100.0);
  UI_CSS_UnitTests_SetElementBox(first,  0.0, 0.0, 50.0, 40.0);    // starting position, about to be overwritten
  UI_CSS_UnitTests_SetElementBox(second, 0.0, 0.0, 50.0, 40.0);
  first->SetFather(parent);
  second->SetFather(parent);
  parent->GetComposeElements()->Add(first);
  parent->GetComposeElements()->Add(second);

  parent->SetFlexContainer(true);
  parent->SetJustifyContent(UI_JUSTIFY_CONTENT_SPACE_BETWEEN);

  UI_LAYOUTENGINE::RunLayout(parent, UI_LAYOUTSTRATEGY_CSS);

  UI_CSSBOX firstcssbox  = UI_CSSBox_Get(first);
  UI_CSSBOX secondcssbox = UI_CSSBox_Get(second);

  EXPECT_EQ(firstcssbox.left, 0.0);       // packed at the container's own start
  EXPECT_EQ(secondcssbox.left, 250.0);    // packed at the container's own end: 300 - 50

  GEN_DELETE parent;
}

TEST(UI_LayoutEngine, RunLayoutWithCssStrategyLeavesANonFlexContainersChildrenAtTheirOriginalPositions)
{
  UI_ELEMENT* parent = GEN_NEW UI_ELEMENT();
  UI_ELEMENT* child   = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(parent, 0.0, 0.0, 300.0, 100.0);
  UI_CSS_UnitTests_SetElementBox(child,  123.0, 45.0, 50.0, 40.0);
  child->SetFather(parent);
  parent->GetComposeElements()->Add(child);

  // "parent" is deliberately left as a plain (non-flex) container -- see this sub-step's SCOPE ADDENDUM in
  // UI_LayoutEngine.h: ApplyFlexLayout() must be a no-op here, so RunLayout(CSS) stays behaviour-preserving for
  // every layout that does not opt into "display: flex".
  UI_LAYOUTENGINE::RunLayout(parent, UI_LAYOUTSTRATEGY_CSS);

  UI_CSSBOX childcssbox = UI_CSSBox_Get(child);
  EXPECT_EQ(childcssbox.left, 123.0);
  EXPECT_EQ(childcssbox.top, 45.0);

  GEN_DELETE parent;
}

TEST(UI_LayoutEngine, RunLayoutWithCssStrategyAppliesFlexGrowThroughTheRealPipeline)
{
  UI_ELEMENT* parent = GEN_NEW UI_ELEMENT();
  UI_ELEMENT* fixed    = GEN_NEW UI_ELEMENT();
  UI_ELEMENT* growing  = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(parent, 0.0, 0.0, 300.0, 100.0);
  UI_CSS_UnitTests_SetElementBox(fixed,   0.0, 0.0, 50.0, 40.0);
  UI_CSS_UnitTests_SetElementBox(growing, 0.0, 0.0, 50.0, 40.0);
  fixed->SetFather(parent);
  growing->SetFather(parent);
  parent->GetComposeElements()->Add(fixed);
  parent->GetComposeElements()->Add(growing);

  parent->SetFlexContainer(true);
  growing->SetFlexGrow(1.0);   // "fixed" keeps flex-grow's CSS initial value (0): all free space goes to "growing"

  UI_LAYOUTENGINE::RunLayout(parent, UI_LAYOUTSTRATEGY_CSS);

  UI_CSSBOX growingcssbox = UI_CSSBox_Get(growing);
  EXPECT_EQ(growingcssbox.width, 250.0);   // 50 (base) + 200 (all the free space: 300 - 50 - 50)

  GEN_DELETE parent;
}

TEST(UI_LayoutEngine, RunLayoutReproducesDashboardXmlsFooterOriginalAbsolutePositionsThroughMarginAlone)
{
  // Mirrors dashboard.xml's "footer_bg" flex container exactly -- same 9 children (8 real ones plus the
  // invisible "footer_spacer"), same widths/heights, same "margin"/"flex-grow" values, same container-level
  // "align-items: center" (see that element's and dashboard.css's own comments for the full derivation, and
  // for the earlier per-item approaches this superseded: a 0-height assumption for text that clipped the
  // captions off-screen, then a per-item "align-self: flex-end" fix that put the captions on a different
  // cross-axis baseline than their own icons). A regression here means the shipped dashboard.xml no longer
  // reproduces its intended pixel positions.
  //
  // The 3 text items use a nonzero, representative font-metric height (18.0) ON PURPOSE, instead of 0.0:
  // UI_SKINCANVAS::CalculateBoundaryLine_Text() always auto-sizes a text element's real box to the font's own
  // height, it is never actually 0. Every child here (icons, dividers, AND the 3 texts alike) is centered by
  // the SAME container "align-items: center" -- none carries its own vertical margin or "align-self" any
  // more -- which is what makes every expectedtop below independent of that height, proving the centering is
  // robust regardless of the real font metrics, not just correct for one hardcoded value.
  //
  // "footer_spacer" (index 6) is the "margin: auto" substitute: "flex-grow: 1" makes it consume ALL of the
  // row's free space, so the last group ("Uptime") lands flush against the right edge minus its OWN
  // margin-right (24 -- the same number as "footer_equipo_icon"'s margin-LEFT, mirroring the left inset) --
  // regardless of how wide the preceding "SO: ..." caption's real, auto-measured string happens to be. That is
  // the point of expectedleft/expectedtop below being plain literals rather than a formula off the source
  // widths: this proves the mirrored 24px inset holds even with a placeholder width, so it will keep holding
  // whatever the real live caption widths turn out to be at runtime -- see UI_LAYOUTENGINE.cpp's flex-grow
  // resolution pass (already exercised by "RunLayoutWithCssStrategyAppliesFlexGrowThroughTheRealPipeline").
  UI_ELEMENT* footer = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(footer, 0.0, 856.0, 1440.0, 44.0);
  footer->SetFlexContainer(true);
  footer->SetAlignItems(UI_ALIGN_ITEMS_CENTER);

  struct { double width, height, marginleft, marginright, flexgrow, expectedleft, expectedtop; } items[9] =
  {
    {  36.0, 36.0,  24.0, 0.0, 0.0,    24.0, 860.0 },   // footer_equipo_icon   -- (44-36)/2=4  -> 856+4=860
    {   0.0, 18.0,  16.0, 0.0, 0.0,    76.0, 869.0 },   // footer_equipo_text   -- (44-18)/2=13 -> 856+13=869
    {   1.0, 24.0, 232.0, 0.0, 0.0,   308.0, 866.0 },   // footer_divider_1     -- (44-24)/2=10 -> 856+10=866
    {  36.0, 36.0, 219.0, 0.0, 0.0,   528.0, 860.0 },   // footer_so_icon
    {   0.0, 18.0,  16.0, 0.0, 0.0,   580.0, 869.0 },   // footer_so_text
    {   1.0, 24.0, 216.0, 0.0, 0.0,   796.0, 866.0 },   // footer_divider_2
    {   0.0,  0.0,   0.0, 0.0, 1.0,   797.0, 878.0 },   // footer_spacer        -- grows to consume ALL freespace
    {  36.0, 36.0, 219.0, 0.0, 0.0,  1364.0, 860.0 },   // footer_uptime_icon
    {   0.0, 18.0,  16.0, 24.0, 0.0, 1416.0, 869.0 },   // footer_uptime_text   -- right edge (1416+0=1416) is
  };                                                     // exactly 1440-24: the mirrored inset, via margin-right

  UI_ELEMENT* children[9];

  for(int i=0; i<9; i++)
    {
      children[i] = GEN_NEW UI_ELEMENT();
      UI_CSS_UnitTests_SetElementBox(children[i], 0.0, 0.0, items[i].width, items[i].height);
      children[i]->SetMargin(UI_ELEMENT_TYPE_ALIGN_LEFT , items[i].marginleft);
      children[i]->SetMargin(UI_ELEMENT_TYPE_ALIGN_RIGHT, items[i].marginright);
      children[i]->SetFlexGrow(items[i].flexgrow);
      children[i]->SetFather(footer);
      footer->GetComposeElements()->Add(children[i]);
    }

  UI_LAYOUTENGINE::RunLayout(footer, UI_LAYOUTSTRATEGY_CSS);

  for(int i=0; i<9; i++)
    {
      UI_CSSBOX box = UI_CSSBox_Get(children[i]);
      EXPECT_EQ(box.left, items[i].expectedleft)  << "item " << i;
      EXPECT_EQ(box.top, items[i].expectedtop)    << "item " << i;
    }

  GEN_DELETE footer;
}

TEST(UI_LayoutEngine, RunLayoutRepositionsEverySiblingAfterOneFlexChildsContentBoxChangesSize)
{
  // Reproduces the live-text-update regression the "footer" sub-step shipped: a flex row with three items,
  // the middle one's content box changes size AFTER the first RunLayout() (exactly what UI_MANAGER::
  // ChangeTextElementValue() does to a "#[FOOTER_SO]"/"#[FOOTER_UPTIME]"-style live text every time its
  // resolved value changes -- see that function's own comment on why it now calls RunLayout() again on the
  // flex father instead of leaving the element positioned by the old, flex-unaware CalculePosition() alone).
  // A single re-run of RunLayout() on the FATHER must reflow ALL of its children with their current sizes --
  // not just the one that changed -- proving the fix's mechanism: nothing here is UI_MANAGER-specific, this
  // is the exact "father->IsFlexContainer() -> RunLayout(father, CSS) again" step that function now performs.
  UI_ELEMENT* row = GEN_NEW UI_ELEMENT();
  UI_ELEMENT* left  = GEN_NEW UI_ELEMENT();
  UI_ELEMENT* middle = GEN_NEW UI_ELEMENT();
  UI_ELEMENT* right = GEN_NEW UI_ELEMENT();

  UI_CSS_UnitTests_SetElementBox(row,    0.0, 0.0, 400.0, 20.0);
  UI_CSS_UnitTests_SetElementBox(left,   0.0, 0.0,  10.0, 10.0);
  UI_CSS_UnitTests_SetElementBox(middle, 0.0, 0.0,  10.0, 10.0);   // starts narrow, like a live text before its first resolve
  UI_CSS_UnitTests_SetElementBox(right,  0.0, 0.0,  10.0, 10.0);

  row->SetFlexContainer(true);
  left->SetFather(row);    row->GetComposeElements()->Add(left);
  middle->SetFather(row);  row->GetComposeElements()->Add(middle);
  right->SetFather(row);   row->GetComposeElements()->Add(right);

  UI_LAYOUTENGINE::RunLayout(row, UI_LAYOUTSTRATEGY_CSS);

  UI_CSSBOX rightfirstpass = UI_CSSBox_Get(right);
  EXPECT_EQ(rightfirstpass.left, 20.0);   // packed right after "left" (10) + "middle" (10), flex-start, no gap

  // Simulate the live-update: "middle"'s auto-sized content box grows (e.g. its resolved text got longer),
  // exactly like UI_SKINCANVAS::CalculateBoundaryLine_Text() re-measuring it -- but nothing repositions its
  // siblings yet, matching CalculePosition()'s own single-element, flex-unaware reach.
  UI_CSSBOX middlebox = UI_CSSBox_Get(middle);
  middlebox.width = 100.0;
  UI_CSSBox_Set(middle, middlebox);

  // The fix: re-running RunLayout() on the FATHER (not the changed child alone) reflows every sibling.
  UI_LAYOUTENGINE::RunLayout(row, UI_LAYOUTSTRATEGY_CSS);

  UI_CSSBOX leftsecondpass   = UI_CSSBox_Get(left);
  UI_CSSBOX middlesecondpass = UI_CSSBox_Get(middle);
  UI_CSSBOX rightsecondpass  = UI_CSSBox_Get(right);

  EXPECT_EQ(leftsecondpass.left, 0.0);      // unmoved: still first in the row
  EXPECT_EQ(middlesecondpass.left, 10.0);   // unmoved: still packed right after "left"
  EXPECT_EQ(middlesecondpass.width, 100.0); // the grown size survives the re-run
  EXPECT_EQ(rightsecondpass.left, 110.0);   // pushed from 20 to 110: 10 ("left") + 100 (middle's NEW width)

  GEN_DELETE row;
}

TEST(UI_LayoutEngine, RunLayoutResolvesAFixedSizeSiblingsWidthIdenticallyNoMatterHowManyOverflowingPassesPrecededIt)
{
  // The real footer bug, reproduced at the engine level (see UI_SKIN::CalculateBoundaryLine()'s and
  // dashboard.xml's own SCOPE ADDENDUM comments for the full root-cause story, confirmed against a real
  // windowed build): UI_MANAGER::ChangeTextElementValue() re-runs RunLayout() on the flex father every time a
  // live "#[FOOTER_*]" caption's resolved text changes -- so a two-item row where "icon" is fixed-size and
  // "caption" is a live, auto-measured text whose width genuinely varies from refresh to refresh gets
  // RunLayout() called on it repeatedly, with "icon"'s intrinsic size stamped ONCE (up front, like a real
  // image's declared XML width) and "caption"'s intrinsic size RE-stamped before every pass (like
  // UI_SKINCANVAS::CalculateBoundaryLine_Text() re-measuring the live string on every refresh).
  //
  // Before this fix, "icon"'s flex-basis was read from its own live BoundaryLine -- which WriteBackTree() had
  // just overwritten with whatever the PREVIOUS pass shrank it to -- so a long "caption" pass left "icon"
  // permanently smaller, and even a LATER pass with the exact same short "caption" width as pass 1 could not
  // recover pass 1's result: "icon" only ever ratcheted down. With this fix, "icon"'s basis always comes from
  // its own (unchanged) intrinsic width, so passes 1 and 3 below -- same row width, same "icon" intrinsic
  // width, same "caption" width -- resolve "icon" to the EXACT same final width, regardless of pass 2's much
  // larger, intervening overflow.
  UI_ELEMENT* row     = GEN_NEW UI_ELEMENT();
  UI_ELEMENT* icon    = GEN_NEW UI_ELEMENT();
  UI_ELEMENT* caption = GEN_NEW UI_ELEMENT();

  UI_CSS_UnitTests_SetElementBox(row, 0.0, 0.0, 60.0, 20.0);

  UI_CSS_UnitTests_SetElementBox(icon, 0.0, 0.0, 40.0, 40.0);
  icon->SetIntrinsicWidth(40.0);    // stamped once, like a real image's declared XML width -- never changes
  icon->SetIntrinsicHeight(40.0);

  UI_CSS_UnitTests_SetElementBox(caption, 0.0, 0.0, 40.0, 10.0);
  caption->SetIntrinsicWidth(40.0);   // pass 1: "short" live caption, freshly (re)measured
  caption->SetIntrinsicHeight(10.0);

  row->SetFlexContainer(true);
  icon->SetFather(row);     row->GetComposeElements()->Add(icon);
  caption->SetFather(row);  row->GetComposeElements()->Add(caption);

  // Pass 1: row (60) overflows icon(40) + caption(40) = 80 by 20 -- default flex-shrink (1.0 on both) splits
  // the 20 deficit proportionally to each item's OWN basesize (40 vs 40 -- an even split): each loses 10.
  UI_LAYOUTENGINE::RunLayout(row, UI_LAYOUTSTRATEGY_CSS);
  double iconwidthpass1 = UI_CSSBox_Get(icon).width;
  EXPECT_EQ(iconwidthpass1, 30.0);
  EXPECT_EQ(UI_CSSBox_Get(caption).width, 30.0);

  // Pass 2: "caption" resolves a MUCH longer live string -- re-stamp ITS intrinsic size only, exactly like
  // UI_SKINCANVAS::CalculateBoundaryLine_Text() re-measuring it; "icon" is untouched, its intrinsic width is
  // still 40.0 from before. Row (60) now overflows icon(40) + caption(120) = 160 by 100.
  UI_CSSBOX captionbox = UI_CSSBox_Get(caption);
  captionbox.width = 120.0;
  UI_CSSBox_Set(caption, captionbox);
  caption->SetIntrinsicWidth(120.0);

  UI_LAYOUTENGINE::RunLayout(row, UI_LAYOUTSTRATEGY_CSS);
  EXPECT_EQ(UI_CSSBox_Get(icon).width, 15.0);      // 40 - (100 * 40/160) = 15 -- correctly shrunk THIS pass...

  // Pass 3: "caption" resolves back down to the SAME width as pass 1 (its intrinsic re-stamped to 40.0 again --
  // a shorter live string, exactly as plausible as the longer one from pass 2). "icon"'s own intrinsic width
  // was never touched by any of this.
  captionbox = UI_CSSBox_Get(caption);
  captionbox.width = 40.0;
  UI_CSSBox_Set(caption, captionbox);
  caption->SetIntrinsicWidth(40.0);

  UI_LAYOUTENGINE::RunLayout(row, UI_LAYOUTSTRATEGY_CSS);

  // ...and here is the actual regression: "icon" must resolve back to EXACTLY pass 1's result (30.0), not stay
  // dragged down by pass 2's deeper, merely transient overflow.
  EXPECT_EQ(UI_CSSBox_Get(icon).width, iconwidthpass1);
  EXPECT_EQ(UI_CSSBox_Get(icon).width, 30.0);
  EXPECT_EQ(UI_CSSBox_Get(caption).width, 30.0);

  GEN_DELETE row;
}

TEST(UI_LayoutEngine, RunLayoutReproducesDashboardXmlsSidebarNavOriginalAbsolutePositionsThroughMarginAndRowGap)
{
  // Mirrors dashboard.xml's "sidebar_nav" -- the FIRST genuinely TWO-LEVEL nested flex tree in this example
  // (the footer test above is a single flex container with 9 flat children; this one is a "flex-direction:
  // column" outer container of 8 "flex-direction: row" containers, each with its own 2 children). Proves
  // UI_LAYOUTENGINE::ApplyFlexLayoutRecursive()'s pre-order walk (arrange the OUTER container's children --
  // fixing each row's own width/position -- BEFORE recursing into any of them as ITS OWN flex container) so a
  // single UI_LAYOUTENGINE::RunLayout() call on "sidebar_nav" alone correctly resolves every icon/label pair
  // without a separate call per row -- see dashboard.xml's own comment on "sidebar_nav" for the full
  // derivation this reproduces.
  //
  // Like the footer test, each row's text item uses a nonzero, representative font-metric height (18.0) ON
  // PURPOSE instead of 0.0, to prove "align-items: center" (set on EACH row, not on "sidebar_nav" itself --
  // rows only need to center their OWN icon+label, not each other) is robust to whatever the real auto-
  // measured caption height turns out to be, exactly as already proven for the footer's 3 captions.
  UI_ELEMENT* sidebarnav = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(sidebarnav, 0.0, 46.0, 210.0, 810.0);
  sidebarnav->SetFlexContainer(true);
  sidebarnav->SetFlexDirection(UI_FLEX_DIRECTION_COLUMN);
  sidebarnav->SetGap(8.0, 0.0);   // row-gap: 8 -- the constant gap BETWEEN rows (none before the first/after the last)

  const char* labels[8] = { "Resumen", "CPU", "Memoria", "Red", "Disco", "Procesos", "Alertas", "Configuracion" };
  double expectedrowtops[8];
  expectedrowtops[0] = 70.0;   // 46 (container top) + 24 (first row's own margin-UP)
  for(int i=1; i<8; i++) expectedrowtops[i] = expectedrowtops[i-1] + 48.0 + 8.0;   // + row height + row-gap

  UI_ELEMENT* rows[8];
  UI_ELEMENT* icons[8];
  UI_ELEMENT* texts[8];

  for(int i=0; i<8; i++)
    {
      rows[i] = GEN_NEW UI_ELEMENT();
      UI_CSS_UnitTests_SetElementBox(rows[i], 0.0, 0.0, 210.0, 48.0);
      if(i == 0) rows[i]->SetMargin(UI_ELEMENT_TYPE_ALIGN_UP, 24.0);   // only the first row: offset from container top
      rows[i]->SetFlexContainer(true);
      rows[i]->SetAlignItems(UI_ALIGN_ITEMS_CENTER);
      rows[i]->SetFather(sidebarnav);
      sidebarnav->GetComposeElements()->Add(rows[i]);

      icons[i] = GEN_NEW UI_ELEMENT();
      UI_CSS_UnitTests_SetElementBox(icons[i], 0.0, 0.0, 44.0, 44.0);
      icons[i]->SetMargin(UI_ELEMENT_TYPE_ALIGN_LEFT, 26.0);
      icons[i]->SetFather(rows[i]);
      rows[i]->GetComposeElements()->Add(icons[i]);

      texts[i] = GEN_NEW UI_ELEMENT();
      UI_CSS_UnitTests_SetElementBox(texts[i], 0.0, 0.0, 0.0, 18.0);   // 0-width: auto-measured, like a real caption
      texts[i]->SetMargin(UI_ELEMENT_TYPE_ALIGN_LEFT, 14.0);
      texts[i]->SetFather(rows[i]);
      rows[i]->GetComposeElements()->Add(texts[i]);
    }

  UI_LAYOUTENGINE::RunLayout(sidebarnav, UI_LAYOUTSTRATEGY_CSS);

  for(int i=0; i<8; i++)
    {
      UI_CSSBOX rowbox  = UI_CSSBox_Get(rows[i]);
      UI_CSSBOX iconbox = UI_CSSBox_Get(icons[i]);
      UI_CSSBOX textbox = UI_CSSBox_Get(texts[i]);

      EXPECT_EQ(rowbox.left, 0.0)                << labels[i];
      EXPECT_EQ(rowbox.top, expectedrowtops[i])   << labels[i];
      EXPECT_EQ(rowbox.width, 210.0)              << labels[i];

      EXPECT_EQ(iconbox.left, 26.0)                          << labels[i];   // unchanged from the mockup's xpos=26
      EXPECT_EQ(iconbox.top, expectedrowtops[i] + 2.0)        << labels[i];   // centered: (48-44)/2 = 2

      EXPECT_EQ(textbox.left, 84.0)                           << labels[i];   // 26 (icon) + 44 (icon width) + 14 (gap)
      EXPECT_EQ(textbox.top, expectedrowtops[i] + 15.0)        << labels[i];   // centered: (48-18)/2 = 15
    }

  GEN_DELETE sidebarnav;
}

TEST(UI_LayoutEngine, RunLayoutReproducesDashboardXmlsCardHeaderOriginalAbsolutePositionsThroughFlexColumnCardAndChildMargin)
{
  // Mirrors dashboard.xml's card 1 ("card_cpu_temp" -> "card-header" -> "temp_hdr_icon"/"temp_hdr_text"), the
  // fourth sub-step of this same Phase 4 increment.
  //
  // CORRECTION (caught against Abraham's real Windows build, see dashboard.css's "form.card" comment for the
  // full story): the first version of this test made "card" a plain, non-flex father and pre-baked "header"'s
  // box at its expected FINAL absolute position, to match what was (wrongly) assumed UI_SKINCANVAS::
  // CalculePosition()'s legacy margin step would already have produced by the time RunLayout() runs. That
  // assumption was never actually exercised end-to-end -- and it was wrong: CalculePosition()'s Y math is
  // bottom-anchored (a child with no explicit "ypos" defaults to its father's BOTTOM edge, margin-UP offsetting
  // a little further from THERE, not from the top), so the real header landed at the BOTTOM of every card
  // instead of the top. The fix is "card" itself becoming a real (if functionally trivial, one-item) flex
  // column, exactly like "sidebar-nav" above, so "header" is a genuine flex ITEM positioned by the CSS-native,
  // top-anchored engine instead of falling through to the legacy one. This test now proves that end-to-end:
  // nothing is pre-resolved by hand, "header"'s box starts at (0,0) like every other flex item in this suite,
  // and RunLayout() alone must derive its correct on-screen position purely from "card"'s flex-column layout
  // plus "header"'s own margin.
  //
  // Numbers below are card 1's real ones: card at (238, 100), icon originally at absolute xpos=262 (238+24),
  // text at absolute xpos=320 (238+82, a 10px gap past the icon's right edge at 238+24+48=310).
  UI_ELEMENT* card = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(card, 238.0, 100.0, 376.0, 390.0);
  card->SetFlexContainer(true);
  card->SetFlexDirection(UI_FLEX_DIRECTION_COLUMN);
  // "card"'s padding is left at its default (0) -- SCOPE ADDENDUM in dashboard.css explains why it must stay 0
  // rather than the stylesheet's old "padding: 18", now that "card" has a real child for the first time.

  UI_ELEMENT* header = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(header, 0.0, 0.0, 328.0, 48.0);
  header->SetMargin(UI_ELEMENT_TYPE_ALIGN_LEFT, 24.0);
  header->SetMargin(UI_ELEMENT_TYPE_ALIGN_UP, 12.0);
  header->SetFlexContainer(true);
  header->SetAlignItems(UI_ALIGN_ITEMS_CENTER);
  header->SetFather(card);
  card->GetComposeElements()->Add(header);

  UI_ELEMENT* icon = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(icon, 0.0, 0.0, 48.0, 48.0);
  icon->SetFather(header);
  header->GetComposeElements()->Add(icon);

  UI_ELEMENT* text = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(text, 0.0, 0.0, 0.0, 19.0);   // 0-width: auto-measured, like a real title caption
  text->SetMargin(UI_ELEMENT_TYPE_ALIGN_LEFT, 10.0);
  text->SetFather(header);
  header->GetComposeElements()->Add(text);

  UI_LAYOUTENGINE::RunLayout(card, UI_LAYOUTSTRATEGY_CSS);

  UI_CSSBOX headerbox = UI_CSSBox_Get(header);
  UI_CSSBOX iconbox    = UI_CSSBox_Get(icon);
  UI_CSSBOX textbox    = UI_CSSBox_Get(text);

  EXPECT_EQ(headerbox.left, 262.0);                    // 238 (card) + 24 (header's own margin-LEFT)
  EXPECT_EQ(headerbox.top, 112.0);                     // 100 (card) + 12 (header's own margin-UP)

  EXPECT_EQ(iconbox.left, 262.0);                      // flush with the header, no margin of its own
  EXPECT_EQ(iconbox.top, 112.0);                       // centered: (48-48)/2 = 0

  EXPECT_EQ(textbox.left, 320.0);                      // 262 (icon) + 48 (icon width) + 10 (gap) = original xpos=320
  EXPECT_EQ(textbox.top, 126.5);                       // centered: 112 + (48-19)/2 = 126.5

  GEN_DELETE card;
}

TEST(UI_LayoutEngine, RunLayoutReproducesDashboardXmlsCard1BodyRowOfImageAndFlexColumnWithARowInsideIt)
{
  // Mirrors dashboard.xml's card 1 BODY ("card_cpu_temp" -> "card-body" -> [cpu_temp_bar] + [right-col ->
  // [value-row -> cpu_temp_value + cpu_temp_unit] + temp_cap_icon + temp_cap_text]) -- Phase 4's fifth sub-step
  // ("card bodies"), the first of the four cards nested this round. Proves a THREE-LEVEL flex tree (row
  // container -> column item that is ITSELF a flex column -> row item inside THAT column that is itself a flex
  // row) resolves correctly end-to-end through a single RunLayout() call, including "align-items: flex-end"
  // bottom-aligning two differently-sized siblings (the big value and its small degree-unit) on the same
  // baseline -- see dashboard.xml's own "card-body" comment on "card_cpu_temp" for the full pixel derivation
  // this test's numbers come from.
  //
  // "card-body"'s margin only resolves against a FATHER (see UI_LAYOUTENGINE::ArrangeFlexChildren()'s
  // mainleadingoffset/crossleadingoffset math): a margin set directly on RunLayout()'s own ROOT element is
  // never applied to that root (nothing calls ArrangeFlexChildren() for a root against ITS OWN father) --
  // the exact mistake "RunLayoutReproducesDashboardXmlsCardHeaderOriginalAbsolutePositionsThroughFlexColumnCard
  // AndChildMargin"'s own CORRECTION comment above already documents for "card-header". Matching that same
  // precedent, "body" (standing in for "card-body") is a flex-column CHILD of a "card" root (standing in for
  // "card_cpu_temp" itself) instead of being the RunLayout() root directly.
  UI_ELEMENT* card = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(card, 238.0, 100.0, 376.0, 390.0);
  card->SetFlexContainer(true);
  card->SetFlexDirection(UI_FLEX_DIRECTION_COLUMN);

  UI_ELEMENT* body = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(body, 0.0, 0.0, 346.0, 270.0);
  body->SetMargin(UI_ELEMENT_TYPE_ALIGN_LEFT, 24.0);
  body->SetMargin(UI_ELEMENT_TYPE_ALIGN_UP, 5.0);
  body->SetFlexContainer(true);
  body->SetGap(0.0, 24.0);   // column-gap: 24
  body->SetFather(card);
  card->GetComposeElements()->Add(body);

  UI_ELEMENT* bar = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(bar, 0.0, 0.0, 180.0, 270.0);
  bar->SetFather(body);
  body->GetComposeElements()->Add(bar);

  UI_ELEMENT* rightcol = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(rightcol, 0.0, 0.0, 142.0, 247.0);
  rightcol->SetFlexContainer(true);
  rightcol->SetFlexDirection(UI_FLEX_DIRECTION_COLUMN);
  rightcol->SetFather(body);
  body->GetComposeElements()->Add(rightcol);

  UI_ELEMENT* valuerow = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(valuerow, 0.0, 0.0, 142.0, 62.0);
  valuerow->SetFlexContainer(true);
  valuerow->SetAlignItems(UI_ALIGN_ITEMS_FLEX_END);
  valuerow->SetFather(rightcol);
  rightcol->GetComposeElements()->Add(valuerow);

  UI_ELEMENT* value = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(value, 0.0, 0.0, 0.0, 62.0);   // 0-width: auto-measured, like the real sizefont=50 value
  value->SetFather(valuerow);
  valuerow->GetComposeElements()->Add(value);

  UI_ELEMENT* unit = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(unit, 0.0, 0.0, 0.0, 28.0);    // 0-width: auto-measured, like the real sizefont=28 "°C"
  unit->SetMargin(UI_ELEMENT_TYPE_ALIGN_LEFT, 6.0);
  unit->SetFather(valuerow);
  valuerow->GetComposeElements()->Add(unit);

  UI_ELEMENT* capicon = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(capicon, 0.0, 0.0, 60.0, 60.0);
  capicon->SetMargin(UI_ELEMENT_TYPE_ALIGN_UP, 100.0);
  capicon->SetFather(rightcol);
  rightcol->GetComposeElements()->Add(capicon);

  UI_ELEMENT* captext = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(captext, 0.0, 0.0, 0.0, 18.0);   // 0-width: auto-measured, like the real sizefont=14 caption
  captext->SetMargin(UI_ELEMENT_TYPE_ALIGN_LEFT, 4.0);
  captext->SetMargin(UI_ELEMENT_TYPE_ALIGN_UP, 7.0);
  captext->SetFather(rightcol);
  rightcol->GetComposeElements()->Add(captext);

  UI_LAYOUTENGINE::RunLayout(card, UI_LAYOUTSTRATEGY_CSS);

  UI_CSSBOX barbox      = UI_CSSBox_Get(bar);
  UI_CSSBOX rightcolbox = UI_CSSBox_Get(rightcol);
  UI_CSSBOX valuerowbox = UI_CSSBox_Get(valuerow);
  UI_CSSBOX valuebox    = UI_CSSBox_Get(value);
  UI_CSSBOX unitbox     = UI_CSSBox_Get(unit);
  UI_CSSBOX capiconbox  = UI_CSSBox_Get(capicon);
  UI_CSSBOX captextbox  = UI_CSSBox_Get(captext);

  EXPECT_EQ(barbox.left, 262.0);                        // 238 (body) + 24 (body's own margin-LEFT)
  EXPECT_EQ(barbox.top, 105.0);                          // 100 (body) + 5 (body's own margin-UP)
  EXPECT_EQ(barbox.width, 180.0);
  EXPECT_EQ(barbox.height, 270.0);

  EXPECT_EQ(rightcolbox.left, 466.0);                    // 262 (bar) + 180 (bar width) + 24 (column-gap) = original xpos=466
  EXPECT_EQ(rightcolbox.top, 105.0);                     // same row as "bar": no cross-axis offset of its own

  EXPECT_EQ(valuerowbox.left, 466.0);
  EXPECT_EQ(valuerowbox.height, 62.0);                   // the row's own declared height, unaffected by its children

  EXPECT_EQ(valuebox.top, 105.0);                        // "align-items: flex-end", and value's own height (62) fills the row
  EXPECT_EQ(unitbox.left, 466.0 + 6.0);                  // value's 0-width auto-measured box adds nothing of its own, but
                                                          // "unit"'s own margin-LEFT (6) still applies on top of that
  EXPECT_EQ(unitbox.top, 105.0 + (62.0 - 28.0));          // flex-end: unit's SHORTER box sinks to the row's bottom edge

  EXPECT_EQ(capiconbox.left, 466.0);
  EXPECT_EQ(capiconbox.top, 105.0 + 62.0 + 100.0);        // value-row bottom + capicon's own margin-UP

  EXPECT_EQ(captextbox.left, 466.0 + 4.0);                // capicon's left edge + captext's own margin-LEFT
  EXPECT_EQ(captextbox.top, 105.0 + 62.0 + 100.0 + 60.0 + 7.0);   // capicon's bottom edge + captext's own margin-UP

  GEN_DELETE card;
}

TEST(UI_LayoutEngine, RunLayoutReproducesDashboardXmlsCard3BodyRingPlusColumnWithAJustifyContentSpaceBetweenRow)
{
  // Mirrors dashboard.xml's card 3 BODY ("card_ram" -> "card-body" -> [ram_usage_radial] + [right-col ->
  // ram_used_total + ram_caption + ram_linear_bar + scale-row (justify-content: space-between) ->
  // ram_scale_0/50/100]) -- Phase 4's fifth sub-step, the third of the four cards nested this round. Proves
  // "justify-content: space-between" correctly spreads THREE items across a flex row nested two levels deep
  // (row -> column -> row), reproducing the "0%"/"50%"/"100%" scale ladder the original flat layout hand-
  // positioned at fixed xpos values 0/57/113 apart -- see dashboard.xml's own "card-body" comment on "card_ram"
  // for the full pixel derivation this test's numbers come from.
  //
  // As in the card 1 body test above: "card-body"'s margin only resolves against a FATHER, so "body" (standing
  // in for "card-body") is a flex-column CHILD of a "card" root (standing in for "card_ram" itself), not the
  // RunLayout() root directly -- see that test's own comment for the full reasoning.
  UI_ELEMENT* card = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(card, 1034.0, 100.0, 400.0, 300.0);
  card->SetFlexContainer(true);
  card->SetFlexDirection(UI_FLEX_DIRECTION_COLUMN);

  UI_ELEMENT* body = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(body, 0.0, 0.0, 354.0, 186.0);
  body->SetMargin(UI_ELEMENT_TYPE_ALIGN_LEFT, 22.0);
  body->SetMargin(UI_ELEMENT_TYPE_ALIGN_UP, 58.0);
  body->SetFlexContainer(true);
  body->SetFather(card);
  card->GetComposeElements()->Add(body);

  UI_ELEMENT* ring = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(ring, 0.0, 0.0, 178.0, 178.0);
  ring->SetFather(body);
  body->GetComposeElements()->Add(ring);

  UI_ELEMENT* rightcol = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(rightcol, 0.0, 0.0, 160.0, 148.0);
  rightcol->SetMargin(UI_ELEMENT_TYPE_ALIGN_UP, 38.0);
  rightcol->SetFlexContainer(true);
  rightcol->SetFlexDirection(UI_FLEX_DIRECTION_COLUMN);
  rightcol->SetFather(body);
  body->GetComposeElements()->Add(rightcol);

  UI_ELEMENT* scalerow = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(scalerow, 0.0, 0.0, 140.0, 15.0);
  scalerow->SetMargin(UI_ELEMENT_TYPE_ALIGN_LEFT, 6.0);
  scalerow->SetMargin(UI_ELEMENT_TYPE_ALIGN_UP, 6.0 + 56.0);   // stands in for the used_total/caption/bar siblings
                                                                // above it in the real card (not needed here: this
                                                                // test targets "scale-row"'s OWN row layout, not
                                                                // "right-col"'s full vertical stack, already proven
                                                                // generically by the sidebar/card-header tests above)
  scalerow->SetFlexContainer(true);
  scalerow->SetJustifyContent(UI_JUSTIFY_CONTENT_SPACE_BETWEEN);
  scalerow->SetFather(rightcol);
  rightcol->GetComposeElements()->Add(scalerow);

  UI_ELEMENT* scale0 = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(scale0, 0.0, 0.0, 16.0, 15.0);   // "0%" -- narrow, representative auto-measured width
  scale0->SetFather(scalerow);
  scalerow->GetComposeElements()->Add(scale0);

  UI_ELEMENT* scale50 = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(scale50, 0.0, 0.0, 24.0, 15.0);   // "50%"
  scale50->SetFather(scalerow);
  scalerow->GetComposeElements()->Add(scale50);

  UI_ELEMENT* scale100 = GEN_NEW UI_ELEMENT();
  UI_CSS_UnitTests_SetElementBox(scale100, 0.0, 0.0, 32.0, 15.0);   // "100%"
  scale100->SetFather(scalerow);
  scalerow->GetComposeElements()->Add(scale100);

  UI_LAYOUTENGINE::RunLayout(card, UI_LAYOUTSTRATEGY_CSS);

  UI_CSSBOX ringbox     = UI_CSSBox_Get(ring);
  UI_CSSBOX scalerowbox = UI_CSSBox_Get(scalerow);
  UI_CSSBOX scale0box   = UI_CSSBox_Get(scale0);
  UI_CSSBOX scale50box  = UI_CSSBox_Get(scale50);
  UI_CSSBOX scale100box = UI_CSSBox_Get(scale100);

  EXPECT_EQ(ringbox.left, 1056.0);                       // 1034 (body) + 22 (body's own margin-LEFT)
  EXPECT_EQ(ringbox.top, 158.0);                          // 100 (body) + 58 (body's own margin-UP)

  EXPECT_EQ(scalerowbox.left, 1056.0 + 178.0 + 6.0);      // ring's right edge + scale-row's own margin-LEFT
  EXPECT_EQ(scalerowbox.width, 140.0);

  // "justify-content: space-between": first item flush left, last item flush right, the remaining free space
  // split evenly into the gaps between them -- exactly the "0%"/"50%"/"100%" ladder the flat layout hand-tuned.
  double freespace = scalerowbox.width - (scale0box.width + scale50box.width + scale100box.width);
  double gap       = freespace / 2.0;

  EXPECT_EQ(scale0box.left, scalerowbox.left);
  EXPECT_EQ(scale50box.left, scale0box.left + scale0box.width + gap);
  EXPECT_EQ(scale100box.left, scalerowbox.left + scalerowbox.width - scale100box.width);

  GEN_DELETE card;
}


#endif // GOOGLETEST_ACTIVE
