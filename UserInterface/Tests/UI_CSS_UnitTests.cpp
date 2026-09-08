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


#endif // GOOGLETEST_ACTIVE
