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
* @ingroup    USERINTERFACE
*
* @param[in]  pathfile : Stylesheet file to load.
* @param[out] out : Target stylesheet; parsed rules are appended.
*
* @return     bool : true if the file was opened and read; false if it could not be opened.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_CSSPARSER::ParseFile(XPATH& pathfile, UI_STYLESHEET& out)
{
  XFILETXT* file = GEN_NEW XFILETXT();
  if(!file) return false;

  bool status = false;

  if(file->Open(pathfile, true))
    {
      if(file->ReadAllFile())
        {
          XSTRING alltext;
          file->GetAllInOneLine(alltext);

          status = ParseText(alltext, out);
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
  int pos = 0;
  int len = (int)text.GetSize();

  while(pos < len)
    {
      SkipWhitespaceAndComments(text, pos);
      if(pos >= len) break;

      UI_CSSRULE* rule = GEN_NEW UI_CSSRULE();
      if(!rule) return false;

      if(!ReadSelectorList(text, pos, rule) || !ReadDeclarationBlock(text, pos, rule))
        {
          // Malformed rule: throw it away and try to resync on the next '}'.
          GEN_DELETE rule;
          SkipToNextRule(text, pos);
          continue;
        }

      // A rule with zero valid selectors OR zero declarations is silently dropped: it cannot match anything
      // useful, and keeping it would just waste cycles during Resolve().
      if(rule->GetSelectors().GetSize() == 0 || rule->GetDeclarations().GetProperties()->GetSize() == 0)
        {
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
              // Pseudo-class. ":root" is the only one honoured in this step: the parser lifts its declarations
              // into the stylesheet's variable table when the rule is finalized. Every other pseudo-class is
              // stored on the selector so authored rules parse cleanly, contributes 10 to specificity, and is
              // rejected by Match() -- future state-aware cascade will be wired here.
              sel->AddPseudo(frag.Get());
            }
        }
    }

  sel->RecomputeSpecificity();
  return sel;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_CSSPARSER::ReadDeclarationBlock(XSTRING& text, int& pos, UI_CSSRULE* rule)
* @brief      Parse the "{ key : value ; ... }" block into rule->GetDeclarations().
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* @param[in]     text : Source text.
* @param[in,out] pos : Cursor; expected at '{' on entry, positioned right after '}' on success.
* @param[in,out] rule : Rule to populate.
*
* @return     bool : true on success; false on EOF before '}'.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_CSSPARSER::ReadDeclarationBlock(XSTRING& text, int& pos, UI_CSSRULE* rule)
{
  if(!rule) return false;

  int len = (int)text.GetSize();

  if(pos >= len || text[pos] != __C('{')) return false;
  pos++;

  UI_STYLE& decls = rule->GetDeclarations();

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
          // Malformed declaration: skip to next ';' or '}' and try to keep parsing this block.
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
* @fn         void UI_CSSPARSER::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_CSSPARSER::Clean()
{

}
