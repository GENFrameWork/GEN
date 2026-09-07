/**-------------------------------------------------------------------------------------------------------------------
*
* @file       UI_CSSParser.cpp
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
* --------------------------------------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "UI_CSSParser.h"

#include "XFileTXT.h"
#include "XFactory.h"
#include "XTrace.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_CSSPARSER::UI_CSSPARSER()
* @brief      Constructor of class
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_CSSPARSER::UI_CSSPARSER()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_CSSPARSER::~UI_CSSPARSER()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_CSSPARSER::~UI_CSSPARSER()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_CSSPARSER::ParseFile(XPATH& pathfile, UI_STYLESHEET& out)
* @brief      Read a .css file from disk and parse it into `out`.
* @note       Step 5 ("una sola hoja por layout"): this is also the entry point @import recurses through, so
*             it (a) guards against cycles -- a file already open somewhere up this call's @import chain is
*             skipped instead of recursed into again -- and (b) tracks `currentfiledir` (this file's own
*             directory) around the ParseText() call, restoring the caller's value afterwards, so a NESTED
*             @import inside an imported file resolves against ITS OWN directory rather than the top-level
*             layout's. Both only matter to ResolveAndParseImport(); a normal, import-free stylesheet behaves
*             exactly as before.
* @ingroup    USERINTERFACE
*
* @param[in]  pathfile : Stylesheet file to load.
* @param[out] out : Target stylesheet; parsed rules are appended.
*
* @return     bool : true if the file was opened and read; false if it could not be opened, or if it is
*             already open along the current @import chain (cycle).
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_CSSPARSER::ParseFile(XPATH& pathfile, UI_STYLESHEET& out)
{
  for(XDWORD c=0; c<importstack.GetSize(); c++)
    {
      XPATH* openfile = importstack.Get(c);
      if(openfile && (*openfile) == pathfile)
        {
          XTRACE_PRINTCOLOR(XTRACE_COLOR_WARNING, __L("[CSS Parse] @import cycle detected: [%s] is already open, skipping"), pathfile.Get());
          return false;
        }
    }

  XFILETXT* file = GEN_NEW XFILETXT();
  if(!file) return false;

  bool status = false;

  if(file->Open(pathfile, true))
    {
      if(file->ReadAllFile())
        {
          XSTRING alltext;
          file->GetAllInOneLine(alltext);

          XPATH* marker = GEN_NEW XPATH(pathfile);
          if(marker) importstack.Add(marker);

          XPATH   savedcurrentfiledir = currentfiledir;
          XSTRING drive;
          XSTRING dir;

          pathfile.GetDrive(drive);
          pathfile.GetPath(dir);

          currentfiledir  = drive;
          currentfiledir += dir;

          status = ParseText(alltext, out);

          currentfiledir = savedcurrentfiledir;

          if(marker)
            {
              importstack.DeleteLast();
              GEN_DELETE marker;
            }
        }

      file->Close();
    }

  GEN_DELETE file;

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_CSSPARSER::ParseText(XSTRING& text, UI_STYLESHEET& out)
* @brief      Parse an in-memory CSS text buffer into `out`. Rules with syntax errors are skipped via
*             SkipToNextRule() and do not abort the parse of the remaining input.
* @note       Step 11: every discard (malformed rule, empty rule, malformed declaration -- see
*             ReadDeclarationBlock()) is reported through XTRACE_PRINTCOLOR(). This costs nothing when GEN is
*             built without XTRACE_ACTIVE (the macro compiles to a no-op, see XTrace.h) and is otherwise
*             filtered exactly like every other GEN trace, through the app's own ini ([log] activesectionsID /
*             levelmask) or target selection -- no separate on/off switch was added for this.
* @ingroup    USERINTERFACE
*
* @param[in]  text : CSS source text.
* @param[out] out : Target stylesheet; parsed rules are appended.
*
* @return     bool : true always (a valid empty text is a valid stylesheet); reserved for future hard errors.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_CSSPARSER::ParseText(XSTRING& text, UI_STYLESHEET& out)
{
  int pos        = 0;
  int len        = (int)text.GetSize();
  int ruleindex  = 0;   // 1-based, counts every rule ATTEMPT (kept or discarded) -- Step 11 debug trace only.

  while(pos < len)
    {
      SkipWhitespaceAndComments(text, pos);
      if(pos >= len) break;

      // Step 5: "@import "file.css";" (ParseFile()-driven parses only -- see ResolveAndParseImport()). Handled
      // BEFORE the generic rule path on purpose: an at-rule has no '{' of its own, so ReadSelectorList() would
      // otherwise happily scan straight through it looking for one, folding it and the NEXT real rule's
      // selector into a single corrupt token. Any other at-rule (@media, @font-face, ...) is outside GEN's CSS
      // subset and is discarded the same way a malformed rule is, rather than being fed to the selector parser.
      if(text[pos] == __C('@'))
        {
          int     atrulestart = pos;
          int     kwend       = pos + 7; if(kwend > len) kwend = len;
          XSTRING atkeyword;
          text.Copy(pos, kwend, atkeyword);

          if(!atkeyword.Compare(__L("@import"), true))
            {
              pos += 7;   // length of "@import"

              XSTRING importurl;
              if(ReadImportStatement(text, pos, importurl))
                {
                  ResolveAndParseImport(importurl, out);   // failures are traced inside; never fatal here
                }
               else
                {
                  XTRACE_PRINTCOLOR(XTRACE_COLOR_WARNING, __L("[CSS Parse] malformed @import discarded (started at column %d)"), atrulestart);
                  SkipToNextRule(text, pos);
                }
            }
           else
            {
              XTRACE_PRINTCOLOR(XTRACE_COLOR_WARNING, __L("[CSS Parse] unsupported at-rule ignored (started at column %d)"), atrulestart);
              SkipToNextRule(text, pos);
            }

          continue;
        }

      int rulestartcolumn = pos;
      ruleindex++;

      UI_CSSRULE* rule = GEN_NEW UI_CSSRULE();
      if(!rule) return false;

      if(!ReadSelectorList(text, pos, rule) || !ReadDeclarationBlock(text, pos, rule->GetDeclarations()))
        {
          // Malformed rule: throw it away and try to resync on the next '}'. Step 11: trace WHERE (rule
          // index + the column the parser was scanning when it gave up), since ParseFile flattens the whole
          // file into one line via GetAllInOneLine() -- a plain line number is not available here, so a
          // character column into the source text is the next best thing for locating the bad rule by eye.
          XTRACE_PRINTCOLOR(XTRACE_COLOR_WARNING, __L("[CSS Parse] rule #%d discarded: malformed selector or declaration block (started at column %d, gave up at column %d)"), ruleindex, rulestartcolumn, pos);

          GEN_DELETE rule;
          SkipToNextRule(text, pos);
          continue;
        }

      // A rule with zero valid selectors OR zero declarations is silently dropped: it cannot match anything
      // useful, and keeping it would just waste cycles during Resolve().
      if(rule->GetSelectors().GetSize() == 0 || rule->GetDeclarations().GetProperties()->GetSize() == 0)
        {
          XTRACE_PRINTCOLOR(XTRACE_COLOR_WARNING, __L("[CSS Parse] rule #%d dropped: parsed with %d selector(s) and %d declaration(s) (started at column %d)"), ruleindex, (int)rule->GetSelectors().GetSize(), (int)rule->GetDeclarations().GetProperties()->GetSize(), rulestartcolumn);

          GEN_DELETE rule;
          continue;
        }

      // ":root { ... }" interception: if ANY selector in this rule is exactly ":root", its declarations are
      // absorbed into the stylesheet's variable table (later declarations overwrite earlier ones, matching
      // CSS cascade order) and the rule itself is discarded so it never reaches the matcher.
      bool isroot = false;
      {
        XVECTOR<UI_CSSSELECTOR*>& sels = rule->GetSelectors();
        for(XDWORD s=0; s<sels.GetSize(); s++)
          {
            UI_CSSSELECTOR* sel = sels.Get(s);
            if(sel && sel->IsRootOnly()) { isroot = true; break; }
          }
      }

      if(isroot)
        {
          XVECTOR<UI_STYLEPROPERTY*>* declprops = rule->GetDeclarations().GetProperties();
          if(declprops)
            {
              for(XDWORD d=0; d<declprops->GetSize(); d++)
                {
                  UI_STYLEPROPERTY* prop = declprops->Get(d);
                  if(prop) out.Variables().Set(prop->GetKey().Get(), prop->GetValue());
                }
            }

          GEN_DELETE rule;
          continue;
        }

      out.Rules_Add(rule);
    }

  // Resolve variable references now, so cascade at load time sees fully-expanded values.
  out.ExpandVariables();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_CSSPARSER::ParseInlineDeclarations(XSTRING& text, UI_STYLE& out)
* @brief      Step 6 ("sin overrides puntuales por elemento"): parse a bare "prop: value; prop: value" list --
*             no selector, no surrounding "{ }" of its own -- into `out`. This is what an XML "style=" attribute
*             looks like; wrapping it in a synthetic "{ ... }" lets ReadDeclarationBlock() parse it unchanged,
*             so it accepts exactly the same grammar (comments, whitespace, trimming) as a real rule body.
* @ingroup    USERINTERFACE
*
* @param[in]  text : Raw declaration-list text, e.g. "color: red; padding: 4".
* @param[out] out : Style bag the declarations are written into.
*
* @return     bool : true if the block parsed to completion; false on malformed input (missing/mismatched
*             braces are impossible here since they are synthetic, so this mirrors ReadDeclarationBlock()'s own
*             EOF-before-'}' failure, which cannot happen with a well-formed wrapper -- kept for symmetry and
*             to surface a NULL member misuse defensively).
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_CSSPARSER::ParseInlineDeclarations(XSTRING& text, UI_STYLE& out)
{
  XSTRING wrapped = __L("{");
  wrapped += text;
  wrapped += __L("}");

  int pos = 0;
  return ReadDeclarationBlock(wrapped, pos, out);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_CSSPARSER::SkipWhitespaceAndComments(XSTRING& text, int& pos)
* @brief      Advance `pos` over runs of whitespace and C-style comments ("/ * ... * /"), including nested
*             adjacent runs. Unterminated comments consume up to end-of-text.
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* @param[in]     text : Source text.
* @param[in,out] pos : Cursor.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_CSSPARSER::SkipWhitespaceAndComments(XSTRING& text, int& pos)
{
  int len = (int)text.GetSize();

  while(pos < len)
    {
      XCHAR ch = text[pos];

      if(ch == __C(' ') || ch == __C('\t') || ch == __C('\r') || ch == __C('\n'))
        {
          pos++;
          continue;
        }

      if(ch == __C('/') && (pos + 1) < len && text[pos + 1] == __C('*'))
        {
          pos += 2;
          while(pos < len)
            {
              if(text[pos] == __C('*') && (pos + 1) < len && text[pos + 1] == __C('/'))
                {
                  pos += 2;
                  break;
                }
              pos++;
            }
          continue;
        }

      break;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_CSSPARSER::ReadSelectorList(XSTRING& text, int& pos, UI_CSSRULE* rule)
* @brief      Read the comma-separated selector list up to (but not including) '{'. Each compound selector is
*             parsed via ParseCompoundSelector() and added to the rule.
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* @param[in]     text : Source text.
* @param[in,out] pos : Cursor.
* @param[in,out] rule : Rule to populate with selectors.
*
* @return     bool : true if at least the '{' terminator was found in-range; false on EOF before '{'.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_CSSPARSER::ReadSelectorList(XSTRING& text, int& pos, UI_CSSRULE* rule)
{
  if(!rule) return false;

  int len   = (int)text.GetSize();
  int start = pos;

  while(pos < len)
    {
      XCHAR ch = text[pos];

      if(ch == __C(',') || ch == __C('{'))
        {
          UI_CSSSELECTOR* sel = ParseCompoundSelector(text, start, pos);
          if(sel) rule->GetSelectors().Add(sel);

          if(ch == __C('{')) return true;

          pos++;
          SkipWhitespaceAndComments(text, pos);
          start = pos;
          continue;
        }

      pos++;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_CSSSELECTOR* UI_CSSPARSER::ParseCompoundSelector(XSTRING& text, int start, int end)
* @brief      Parse one compound selector between [start, end): TypeSelector? ("#" Id)? ("." Class)*, or "*".
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* @param[in]  text : Source text.
* @param[in]  start : Start offset (inclusive).
* @param[in]  end : End offset (exclusive).
*
* @return     UI_CSSSELECTOR* : Owned pointer, or NULL on empty/invalid token.
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_CSSSELECTOR* UI_CSSPARSER::ParseCompoundSelector(XSTRING& text, int start, int end)
{
  // Extract the token substring and trim surrounding whitespace.
  XSTRING token;
  text.Copy(start, end, token);

  int tlen = (int)token.GetSize();
  int i    = 0;

  while(i < tlen)
    {
      XCHAR ch = token[i];
      if(ch != __C(' ') && ch != __C('\t') && ch != __C('\r') && ch != __C('\n')) break;
      i++;
    }

  int j = tlen;
  while(j > i)
    {
      XCHAR ch = token[j - 1];
      if(ch != __C(' ') && ch != __C('\t') && ch != __C('\r') && ch != __C('\n')) break;
      j--;
    }

  if(j <= i) return NULL;

  UI_CSSSELECTOR* sel = GEN_NEW UI_CSSSELECTOR();
  if(!sel) return NULL;

  // Universal selector "*": no restrictions at all.
  if((j - i) == 1 && token[i] == __C('*'))
    {
      sel->RecomputeSpecificity();
      return sel;
    }

  // Type prefix (optional): identifier characters up to '#', '.' or ':'.
  int k = i;
  while(k < j)
    {
      XCHAR ch = token[k];
      if(ch == __C('#') || ch == __C('.') || ch == __C(':')) break;
      k++;
    }

  if(k > i)
    {
      XSTRING typestr;
      token.Copy(i, k, typestr);
      sel->GetType().Set(typestr);
    }

  // Then a sequence of ("#" ident), ("." ident) and (":" ident) fragments, in any order.
  while(k < j)
    {
      XCHAR marker = token[k];
      if(marker != __C('#') && marker != __C('.') && marker != __C(':'))
        {
          // Unknown character in selector: bail out but keep whatever we already parsed.
          break;
        }

      k++;
      int fragstart = k;
      while(k < j)
        {
          XCHAR ch = token[k];
          if(ch == __C('#') || ch == __C('.') || ch == __C(':')) break;
          k++;
        }

      if(k > fragstart)
        {
          XSTRING frag;
          token.Copy(fragstart, k, frag);

          if(marker == __C('#'))
            {
              // If two ids are supplied on the same compound selector, the last one wins: no meaningful
              // semantics for two ids, and this matches how most CSS engines silently handle it.
              sel->GetID().Set(frag);
            }
           else if(marker == __C('.'))
            {
              sel->AddClass(frag.Get());
            }
           else /* marker == ':' */
            {
              // Pseudo-class. ":root" is special-cased here at parse time: the parser lifts its declarations
              // into the stylesheet's variable table when the rule is finalized instead of storing it as a
              // regular pseudo. Every other pseudo-class (including the live state ones -- ":preselect"/
              // ":hover", ":selected", ":active", ":disabled" -- and any structural one GEN does not support,
              // e.g. ":first-child") is stored on the selector as authored, so it parses cleanly and
              // contributes 10 to specificity; whether it ever MATCHES an element is entirely Match()'s call
              // at Resolve()/ReapplyStyleVisual() time, driven by UI_ELEMENT::GetActivePseudos().
              sel->AddPseudo(frag.Get());
            }
        }
    }

  sel->RecomputeSpecificity();
  return sel;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_CSSPARSER::ReadDeclarationBlock(XSTRING& text, int& pos, UI_STYLE& decls)
* @brief      Parse the "{ key : value ; ... }" block into `decls`.
* @note       INTERNAL. Takes the destination bag directly (rather than a UI_CSSRULE*) so it can populate either
*             a rule's own declarations (see ParseText()) or a standalone bag with no rule/selector around it
*             at all (see ParseInlineDeclarations(), Step 6).
* @ingroup    USERINTERFACE
*
* @param[in]     text : Source text.
* @param[in,out] pos : Cursor; expected at '{' on entry, positioned right after '}' on success.
* @param[in,out] decls : Style bag to populate.
*
* @return     bool : true on success; false on EOF before '}'.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_CSSPARSER::ReadDeclarationBlock(XSTRING& text, int& pos, UI_STYLE& decls)
{
  int len = (int)text.GetSize();

  if(pos >= len || text[pos] != __C('{')) return false;
  pos++;

  while(pos < len)
    {
      SkipWhitespaceAndComments(text, pos);
      if(pos >= len) return false;

      if(text[pos] == __C('}'))
        {
          pos++;
          return true;
        }

      // --- Key : identifier up to ':' -----------------------------------------------------------------------------
      int keystart = pos;
      while(pos < len)
        {
          XCHAR ch = text[pos];
          if(ch == __C(':') || ch == __C(';') || ch == __C('}')) break;
          pos++;
        }

      if(pos >= len || text[pos] != __C(':'))
        {
          // Malformed declaration: skip to next ';' or '}' and try to keep parsing this block. Step 11: trace
          // it -- this is the one discard SkipToNextRule() never sees, since the enclosing rule is otherwise
          // fine and keeps being parsed.
          XTRACE_PRINTCOLOR(XTRACE_COLOR_WARNING, __L("[CSS Parse] declaration discarded at column %d: no ':' found before ';'/'}'"), keystart);

          while(pos < len && text[pos] != __C(';') && text[pos] != __C('}')) pos++;
          if(pos < len && text[pos] == __C(';')) pos++;
          continue;
        }

      XSTRING key;
      text.Copy(keystart, pos, key);
      key.DeleteNoCharacters(__L(" \t\r\n"));  // trim in-place

      pos++;  // past ':'

      // --- Value : everything up to ';' or '}' --------------------------------------------------------------------
      SkipWhitespaceAndComments(text, pos);
      int valstart = pos;
      while(pos < len)
        {
          XCHAR ch = text[pos];
          if(ch == __C(';') || ch == __C('}')) break;
          pos++;
        }

      int valend = pos;

      // Trim trailing whitespace from value without touching interior spaces (colors like "160,195,255" and
      // multi-token values like "0,0,175,30" must survive intact).
      while(valend > valstart)
        {
          XCHAR ch = text[valend - 1];
          if(ch != __C(' ') && ch != __C('\t') && ch != __C('\r') && ch != __C('\n')) break;
          valend--;
        }

      if(valend > valstart && !key.IsEmpty())
        {
          XSTRING value;
          text.Copy(valstart, valend, value);
          decls.Set(key.Get(), value);
        }

      if(pos < len && text[pos] == __C(';')) pos++;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_CSSPARSER::SkipToNextRule(XSTRING& text, int& pos)
* @brief      Error-recovery: consume everything up to and including the next '}'. String literals and comments
*             are not tracked here; the parser doesn't accept either inside declarations, so this is safe.
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* @param[in]     text : Source text.
* @param[in,out] pos : Cursor.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_CSSPARSER::SkipToNextRule(XSTRING& text, int& pos)
{
  int len = (int)text.GetSize();

  while(pos < len)
    {
      XCHAR ch = text[pos++];
      if(ch == __C('}')) return;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_CSSPARSER::ReadImportStatement(XSTRING& text, int& pos, XSTRING& outurl)
* @brief      Step 5: parse the body of an "@import" at-rule -- everything AFTER the "@import" keyword itself
*             (the caller has already consumed that). Accepts a quoted string ("theme.css" / 'theme.css'),
*             optionally wrapped in CSS's "url(...)" form (quoted or bare), terminated by ';'. Nothing else in
*             real CSS's @import grammar (media queries and the like) is supported -- GEN's layouts don't need
*             them, and a rule this small doesn't need to sniff for them either.
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* @param[in]     text : Source text.
* @param[in,out] pos : Cursor; expected right after the "@import" keyword on entry, positioned right after the
*                terminating ';' (or at EOF, if none was found) on return.
* @param[out]    outurl : The quoted/bare URL text, verbatim (not yet resolved to a filesystem path).
*
* @return     bool : true if a non-empty URL was found; false otherwise (outurl is left empty).
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_CSSPARSER::ReadImportStatement(XSTRING& text, int& pos, XSTRING& outurl)
{
  int len = (int)text.GetSize();

  outurl.Empty();

  SkipWhitespaceAndComments(text, pos);

  bool sawurlwrapper = false;

  if((pos + 3) <= len)
    {
      XSTRING maybeurl;
      text.Copy(pos, pos + 3, maybeurl);
      if(!maybeurl.Compare(__L("url"), true))
        {
          sawurlwrapper = true;
          pos += 3;
          SkipWhitespaceAndComments(text, pos);
          if(pos < len && text[pos] == __C('(')) pos++;
          SkipWhitespaceAndComments(text, pos);
        }
    }

  if(pos >= len) return false;

  XCHAR quote = text[pos];

  if(quote == __C('"') || quote == __C('\''))
    {
      pos++;
      int start = pos;
      while(pos < len && text[pos] != quote) pos++;
      if(pos >= len) return false;   // unterminated string: bail out, let the caller resync

      text.Copy(start, pos, outurl);
      pos++;   // past the closing quote
    }
   else
    {
      // Bare token, only meaningful inside "url(...)" -- e.g. url(theme.css). Read up to ')' or ';'.
      int start = pos;
      while(pos < len && text[pos] != __C(')') && text[pos] != __C(';')) pos++;
      text.Copy(start, pos, outurl);
      outurl.DeleteNoCharacters(__L(" \t\r\n"));
    }

  SkipWhitespaceAndComments(text, pos);
  if(sawurlwrapper && pos < len && text[pos] == __C(')'))
    {
      pos++;
      SkipWhitespaceAndComments(text, pos);
    }

  while(pos < len && text[pos] != __C(';')) pos++;
  if(pos < len && text[pos] == __C(';')) pos++;

  return !outurl.IsEmpty();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_CSSPARSER::ResolveAndParseImport(XSTRING& importurl, UI_STYLESHEET& out)
* @brief      Step 5: resolve an @import URL against the directory of the file currently being parsed
*             (`currentfiledir`, maintained by ParseFile()) and merge the target file's rules and ":root"
*             variables into `out` by recursing into ParseFile() -- i.e. imported content behaves as if it had
*             been written in place of the "@import" line, matching real CSS semantics.
* @note       A no-op (traced, never fatal to the importING file) when: `currentfiledir` is empty (ParseText()
*             was called directly on an in-memory buffer rather than through ParseFile(), so there is no
*             directory to resolve a RELATIVE url against), the target file is missing or empty, or the target
*             is already open along this @import chain (cycle -- see ParseFile()).
* @ingroup    USERINTERFACE
*
* @param[in]  importurl : Raw URL text from ReadImportStatement(), e.g. "theme.css".
* @param[out] out : Target stylesheet; the imported file's rules/variables are appended into it.
*
* @return     bool : true if the imported file was found, parsed, and merged; false otherwise.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_CSSPARSER::ResolveAndParseImport(XSTRING& importurl, UI_STYLESHEET& out)
{
  if(importurl.IsEmpty()) return false;

  if(currentfiledir.IsEmpty())
    {
      XTRACE_PRINTCOLOR(XTRACE_COLOR_WARNING, __L("[CSS Parse] @import [%s] ignored: no file context (stylesheet text was parsed directly, not loaded from a file)"), importurl.Get());
      return false;
    }

  XPATH importpath;
  importpath  = currentfiledir;
  importpath += importurl;

  bool status = ParseFile(importpath, out);

  if(status)
    XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[CSS Parse] @import [%s] merged"), importpath.Get());
   else
    XTRACE_PRINTCOLOR(XTRACE_COLOR_WARNING, __L("[CSS Parse] @import [%s] not applied (missing, empty, or cyclic)"), importpath.Get());

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_CSSPARSER::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_CSSPARSER::Clean()
{
  // Defensive only: importstack is pushed/popped in matched pairs within ParseFile(), so it is already empty
  // by the time a top-level ParseFile()/ParseText() call returns. DeleteContents() covers the one path where
  // it would not be -- an early return out of a deeply recursed @import chain -- without which those XPATH
  // markers would leak.
  importstack.DeleteContents();
  importstack.DeleteAll();

  currentfiledir.Empty();
}
