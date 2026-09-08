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


#endif // GOOGLETEST_ACTIVE
