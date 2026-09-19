/**-------------------------------------------------------------------------------------------------------------------
*
* @file       UI_CSSParser.h
*
* @class      UI_CSSPARSER
* @brief      User Interface : minimal CSS parser (text -> UI_STYLESHEET), scoped to GEN's selector subset.
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
*   Parse a CSS-like text buffer into a UI_STYLESHEET. The accepted grammar is deliberately small (see
*   UI_StyleSheet.h for the selector subset). Everything past that subset is treated as a syntax error inside
*   the current rule: the parser recovers by skipping ahead to the next '}' and continuing, so a malformed
*   rule never poisons the rest of the stylesheet.
*
*   Value semantics: declaration values are stored as raw strings, in the same format that XML attributes use
*   today ("red", "#RRGGBBAA", "15", "center", "0,0,175,30"). The consumer (UI_MANAGER + UI_STYLE typed
*   getters) already knows how to interpret every string that a valid GEN XML layout can produce, so no value
*   normalization happens here.
*
* LEXICAL PRIMITIVES (Phase 2, "lexer con tokens/strings/escapes reales")
*   ReadIdentifier()/ReadStringLiteral()/SkipStringLiteral() are small, reusable token readers shared by every
*   caller that used to slice identifiers or quoted strings out of the source text with raw XSTRING::Copy()
*   over a hand-tracked byte range. Both understand a single-character backslash escape ("\X" decodes to the
*   literal character X, dropping the backslash) -- deliberately NOT the full CSS "\ + up to 6 hex digits [+
*   trailing whitespace]" Unicode escape, which none of GEN's own stylesheets have ever needed; a backslash
*   followed by hex digits is simply read back as those literal digit characters, one escaped character at a
*   time -- a harmless degenerate case rather than a rejection.
*
*   Routing every quoted-string scan through these primitives closes a real correctness gap the previous
*   byte-by-byte scan had: a declaration value or @import URL containing ';', '}', or the selector-list ',' /
*   '{' INSIDE a quoted string used to end the value/URL/selector list right there, because the old scan
*   tested every byte against those delimiters unconditionally, with no notion of "currently inside a string".
*   ReadDeclarationBlock(), ReadSelectorList() and SkipToNextRule() now treat a quoted span as one atomic unit
*   (via SkipStringLiteral()) before ever testing a byte against a delimiter, so e.g. a value like
*   'font-family: "A; B";' keeps its embedded semicolon. The existing rule-level recovery strategy -- skip to
*   the next unquoted '}' and keep parsing -- is unchanged, just made string-aware.
*
* DESCENDANT/CHILD COMBINATORS (Phase 2, "combinadores descendiente/hijo")
*   ParseCompoundSelectorSequence() is what ReadSelectorList() now calls instead of ParseCompoundSelector()
*   directly. It splits ONE comma-separated entry (still delimited by ReadSelectorList() exactly as before) on
*   top-level combinators -- a run of whitespace between two compounds is the descendant combinator, an
*   explicit '>' is the child combinator -- and feeds each resulting span through the unchanged
*   ParseCompoundSelector(). A single-compound entry (no combinator at all, i.e. everything parsed before this
*   phase existed) takes a one-span fast path that returns EXACTLY what ParseCompoundSelector() alone always
*   returned, so no existing selector's parse result changes. See UI_StyleSheet.h's class banner for the
*   resulting UI_CSSSELECTOR shape (subject + ordered ancestor steps) and how UI_CSSSELECTOR::Match() evaluates
*   it against a real element tree.
*
* @author     Abraham J. Velez / EndoraSoft
*
* ---------------------------------------------------------------------------------------------------------------------*/

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XPath.h"
#include "XString.h"
#include "XVector.h"

#include "UI_StyleSheet.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class UI_CSSPARSER
{
  public:
                                    UI_CSSPARSER                ();
    virtual                        ~UI_CSSPARSER                ();




    bool                            ParseFile                   (XPATH& pathfile, UI_STYLESHEET& out);


    bool                            ParseText                   (XSTRING& text, UI_STYLESHEET& out);


    // Step 6 ("sin overrides puntuales por elemento"): parse a BARE declaration list -- the body a stylesheet
    // rule would have between '{' and '}', with no selector of its own -- straight into a UI_STYLE bag. This is
    // exactly what an XML "style=" attribute's value looks like, and reusing ReadDeclarationBlock() means it
    // accepts the same grammar (comments, "prop: value; ...", trimming) as any class rule in a .css file.
    bool                            ParseInlineDeclarations     (XSTRING& text, UI_STYLE& out);


    // Phase 2 ("parser y cascada fiables"): every diagnostic below is reported as an offset into the FLATTENED
    // text ParseText() actually scans -- which, since ParseFile() now preserves real '\n' separators when
    // flattening the source file (see ParseFile()), still lets a raw offset be turned back into a real (line,
    // column-within-line) pair cheaply, without keeping a separate token-position table. Exposed as a public
    // static utility (no instance state, mirrors UI_PROPERTYREGISTRY's helpers) so it is directly unit-testable
    // and reusable by any future caller that wants to report "line N" instead of a meaningless flat offset.
    static void                     ResolveLineColumn           (XSTRING& text, int offset, int& outline, int& outcolumn);


  private:


    void                            SkipWhitespaceAndComments   (XSTRING& text, int& pos);
    bool                            ReadSelectorList            (XSTRING& text, int& pos, UI_CSSRULE* rule);
    bool                            ReadDeclarationBlock        (XSTRING& text, int& pos, UI_STYLE& decls);
    void                            SkipToNextRule              (XSTRING& text, int& pos);

    // Phase 2 ("lexer con tokens/strings/escapes reales"): see the class banner above for the full rationale.
    // ReadIdentifier() reads one identifier-like token (a selector's type/id/class/pseudo fragment) starting at
    // `pos`, stopping at end-of-text or at an unescaped '#'/'.'/':' , and returns false if it consumed nothing.
    // ReadStringLiteral() reads one quoted string token ('...' or "...") starting at `pos` (which MUST be
    // positioned on the opening quote), decodes backslash escapes, and returns false if the string is
    // unterminated (mirrors the previous behaviour: an unterminated quote is a hard failure for the caller to
    // recover from). SkipStringLiteral() is the same scan without building the decoded text, for callers that
    // only need to jump over a quoted span intact (SkipToNextRule(), the declaration key/value scans, the
    // selector-list top-level scan).
    bool                            ReadIdentifier              (XSTRING& text, int& pos, XSTRING& outident);
    bool                            ReadStringLiteral           (XSTRING& text, int& pos, XSTRING& outstring);
    void                            SkipStringLiteral           (XSTRING& text, int& pos);

    // Step 5 ("una sola hoja por layout"): minimal "@import "file.css";" support, so a theme (":root"
    // variables plus shared rules) can live in one file and be pulled into several stylesheets instead of
    // being copy-pasted into each. See UI_CSSParser.cpp for the full design note.
    bool                            ReadImportStatement         (XSTRING& text, int& pos, XSTRING& outurl);
    bool                            ResolveAndParseImport       (XSTRING& importurl, UI_STYLESHEET& out);

    UI_CSSSELECTOR*                 ParseCompoundSelector       (XSTRING& text, int start, int end);

    // Phase 2 ("combinadores descendiente/hijo"): see the class banner above. Splits [start, end) -- one
    // comma-separated selector-list entry -- on top-level combinators and builds the resulting subject +
    // ancestor-steps UI_CSSSELECTOR. This is what ReadSelectorList() now calls instead of ParseCompoundSelector()
    // directly.
    UI_CSSSELECTOR*                 ParseCompoundSelectorSequence(XSTRING& text, int start, int end);

    void                            Clean                       ();

    XPATH                           currentfiledir;   // directory of the file ParseText() is currently inside of (ParseFile()-driven parses only); base for resolving a relative @import URL.
    XPATH                           currentfilepath;  // full path of the file currently being parsed (ParseFile only); included in ERROR/WARNING diagnostics so a silent transparent UI is diagnosable.
    XVECTOR<XPATH*>                 importstack;      // files currently open along this ParseFile() call's @import chain; cycle guard, see ParseFile().

    // Phase 2 ("cerrar el caso residual de variables"): depth of @import recursion this parser is currently
    // inside of, maintained ONLY around the ParseFile() call in ResolveAndParseImport(). ParseText() consults
    // it to decide whether IT is the call that should run UI_STYLESHEET::ExpandVariables() -- see ParseText()
    // for the full rationale (the residual bug: an imported file's var() references used to be substituted
    // before the IMPORTING file had parsed its own later ":root" block, permanently losing any override).
    int                             importdepth;

    // Phase 0 hygiene: counts rules discarded during the current ParseText() (malformed / empty). Used to emit
    // a visible ERROR summary when a non-empty source produced zero kept rules -- the classic "comment closed
    // early by */ in prose, everything after is garbage" failure mode that used to leave the UI fully transparent.
    int                             discarded_rules;
    int                             unterminated_comments;
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
