/**-------------------------------------------------------------------------------------------------------------------
*
* @file       UI_StyleSheet.cpp
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
* --------------------------------------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "UI_StyleSheet.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS (UI_CSSSELECTOR) --------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_CSSSELECTOR::UI_CSSSELECTOR()
* @brief      Constructor of class
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_CSSSELECTOR::UI_CSSSELECTOR()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_CSSSELECTOR::~UI_CSSSELECTOR()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_CSSSELECTOR::~UI_CSSSELECTOR()
{
  for(XDWORD c=0; c<classes.GetSize(); c++)
    {
      XSTRING* classname = classes.Get(c);
      if(classname) GEN_DELETE classname;
    }

  classes.DeleteAll();

  for(XDWORD c=0; c<pseudos.GetSize(); c++)
    {
      XSTRING* pseudoname = pseudos.Get(c);
      if(pseudoname) GEN_DELETE pseudoname;
    }

  pseudos.DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_CSSSELECTOR::AddClass(XCHAR* classname)
* @brief      Add class name (owned copy).
* @ingroup    USERINTERFACE
*
* @param[in]  classname : Class name to add.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_CSSSELECTOR::AddClass(XCHAR* classname)
{
  if(!classname) return;

  XSTRING* copy = GEN_NEW XSTRING();
  if(!copy) return;

  copy->Set(classname);
  classes.Add(copy);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_CSSSELECTOR::AddPseudo(XCHAR* pseudoname)
* @brief      Add pseudo-class name (owned copy). Names are stored as authored, without the leading colon.
* @ingroup    USERINTERFACE
*
* @param[in]  pseudoname : Pseudo-class name to add ("root", "hover", "selected", ...).
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_CSSSELECTOR::AddPseudo(XCHAR* pseudoname)
{
  if(!pseudoname) return;

  XSTRING* copy = GEN_NEW XSTRING();
  if(!copy) return;

  copy->Set(pseudoname);
  pseudos.Add(copy);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_CSSSELECTOR::RecomputeSpecificity()
* @brief      Recompute cached CSS-style specificity. Pseudo-classes count the same as regular classes (=10).
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_CSSSELECTOR::RecomputeSpecificity()
{
  int spec = 0;

  if(!id.IsEmpty())    spec += 100;
  if(!type.IsEmpty())  spec += 1;

  spec += (int)classes.GetSize() * 10;
  spec += (int)pseudos.GetSize() * 10;

  specificity = spec;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_CSSSELECTOR::IsRootOnly()
* @brief      True iff this selector is exactly ":root" (parser hint to route the rule into variables).
* @ingroup    USERINTERFACE
*
* @return     bool : true when the selector has no type/id/class parts and exactly one pseudo named "root".
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_CSSSELECTOR::IsRootOnly()
{
  if(!type.IsEmpty())          return false;
  if(!id.IsEmpty())            return false;
  if(classes.GetSize() > 0)    return false;
  if(pseudos.GetSize() != 1)   return false;

  XSTRING* p = pseudos.Get(0);
  if(!p) return false;

  return p->Compare(__L("root"), true) == 0;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_CSSSELECTOR::Match(XSTRING& elementtype, XSTRING& elementid, XVECTOR<XSTRING*>& elementclasses, XVECTOR<XSTRING*>& activepseudos)
* @brief      Test if this selector matches an element. Empty selector components are treated as wildcards.
* @ingroup    USERINTERFACE
*
* @param[in]  elementtype : Element type string (as reported by UI_ELEMENT::GetTypeString()).
* @param[in]  elementid : Element id (name).
* @param[in]  elementclasses : Element class list (may be empty).
*
* @return     bool : true if the selector matches; false otherwise.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_CSSSELECTOR::Match(XSTRING& elementtype, XSTRING& elementid, XVECTOR<XSTRING*>& elementclasses, XVECTOR<XSTRING*>& activepseudos)
{
  // Type component: if set, must match element type string (case-insensitive).
  if(!type.IsEmpty())
    {
      if(elementtype.IsEmpty())                 return false;
      if(elementtype.Compare(type, true) != 0)  return false;
    }

  // Id component: if set, must match element id (case-insensitive).
  if(!id.IsEmpty())
    {
      if(elementid.IsEmpty())                 return false;
      if(elementid.Compare(id, true) != 0)    return false;
    }

  // Class components: every class listed by the selector must be present in the element's class list.
  for(XDWORD c=0; c<classes.GetSize(); c++)
    {
      XSTRING* wanted = classes.Get(c);
      if(!wanted) continue;

      bool found = false;

      for(XDWORD d=0; d<elementclasses.GetSize(); d++)
        {
          XSTRING* have = elementclasses.Get(d);
          if(!have) continue;

          if(have->Compare(*wanted, true) == 0)
            {
              found = true;
              break;
            }
        }

      if(!found) return false;
    }

  // Pseudo components: every pseudo required by the selector must be present in the element's active-pseudos
  // list. An empty active-pseudos list therefore makes any pseudo-carrying selector unmatchable (load-time /
  // stateless resolution), while an active-pseudos list like ["preselect"] enables ":preselect" rules.
  for(XDWORD c=0; c<pseudos.GetSize(); c++)
    {
      XSTRING* wanted = pseudos.Get(c);
      if(!wanted) continue;

      bool found = false;

      for(XDWORD d=0; d<activepseudos.GetSize(); d++)
        {
          XSTRING* have = activepseudos.Get(d);
          if(!have) continue;

          if(have->Compare(*wanted, true) == 0)
            {
              found = true;
              break;
            }
        }

      if(!found) return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_CSSSELECTOR::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_CSSSELECTOR::Clean()
{
  specificity = 0;
}




/*---- CLASS MEMBERS (UI_CSSRULE) ------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_CSSRULE::UI_CSSRULE()
* @brief      Constructor of class
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_CSSRULE::UI_CSSRULE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_CSSRULE::~UI_CSSRULE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_CSSRULE::~UI_CSSRULE()
{
  for(XDWORD c=0; c<selectors.GetSize(); c++)
    {
      UI_CSSSELECTOR* sel = selectors.Get(c);
      if(sel) GEN_DELETE sel;
    }

  selectors.DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_CSSRULE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_CSSRULE::Clean()
{
  sourceindex = 0;
}




/*---- CLASS MEMBERS (UI_STYLESHEET) ---------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_STYLESHEET::UI_STYLESHEET()
* @brief      Constructor of class
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_STYLESHEET::UI_STYLESHEET()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_STYLESHEET::~UI_STYLESHEET()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_STYLESHEET::~UI_STYLESHEET()
{
  Rules_DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_STYLESHEET::Rules_Add(UI_CSSRULE* rule)
* @brief      Append a rule at the end of the stylesheet (takes ownership).
* @ingroup    USERINTERFACE
*
* @param[in]  rule : Rule pointer to append; ownership is transferred to the stylesheet.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_STYLESHEET::Rules_Add(UI_CSSRULE* rule)
{
  if(!rule) return false;

  rule->SetSourceIndex((int)rules.GetSize());
  rules.Add(rule);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_STYLESHEET::Rules_DeleteAll()
* @brief      Delete all owned rules.
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_STYLESHEET::Rules_DeleteAll()
{
  for(XDWORD c=0; c<rules.GetSize(); c++)
    {
      UI_CSSRULE* rule = rules.Get(c);
      if(rule) GEN_DELETE rule;
    }

  rules.DeleteAll();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_STYLESHEET::Resolve(XSTRING& elementtype, XSTRING& elementid, XVECTOR<XSTRING*>& elementclasses, XVECTOR<XSTRING*>& activepseudos, UI_STYLE& out)
* @brief      Apply the cascade for one element into `out`. Existing keys in `out` are overwritten by matches
*             (CSS-wins semantics). Rules are applied in ascending (specificity, source-index) order, so on
*             return each key holds the value from the winning rule.
* @ingroup    USERINTERFACE
*
* @param[in]  elementtype : Element type string.
* @param[in]  elementid : Element id (name).
* @param[in]  elementclasses : Element class list.
* @param[in,out] out : Style bag to accumulate declarations into.
*
* @return     bool : true if at least one rule matched; false otherwise.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_STYLESHEET::Resolve(XSTRING& elementtype, XSTRING& elementid, XVECTOR<XSTRING*>& elementclasses, XVECTOR<XSTRING*>& activepseudos, UI_STYLE& out)
{
  // Two-pass to avoid depending on std::sort or an XVECTOR sort: collect matched (specificity, sourceindex,
  // rule*) tuples, then apply in ascending order. The tuple count is bounded by the number of rules in the
  // sheet, which is small in the target usage (one .css per layout).
  XVECTOR<int>          match_spec;
  XVECTOR<int>          match_index;
  XVECTOR<UI_CSSRULE*>  match_rule;

  for(XDWORD c=0; c<rules.GetSize(); c++)
    {
      UI_CSSRULE* rule = rules.Get(c);
      if(!rule) continue;

      XVECTOR<UI_CSSSELECTOR*>& sels = rule->GetSelectors();

      int best_spec = -1;

      for(XDWORD d=0; d<sels.GetSize(); d++)
        {
          UI_CSSSELECTOR* sel = sels.Get(d);
          if(!sel) continue;

          if(sel->Match(elementtype, elementid, elementclasses, activepseudos))
            {
              int s = sel->GetSpecificity();
              if(s > best_spec) best_spec = s;
            }
        }

      if(best_spec >= 0)
        {
          match_spec.Add(best_spec);
          match_index.Add(rule->GetSourceIndex());
          match_rule.Add(rule);
        }
    }

  if(match_rule.GetSize() == 0) return false;

  // Insertion-sort ascending by (specificity, sourceindex). O(n^2) is fine here: the number of matched rules
  // per element is small in practice, and this avoids introducing an STL sort dependency.
  for(XDWORD i=1; i<match_rule.GetSize(); i++)
    {
      int          s_i = match_spec.Get(i);
      int          x_i = match_index.Get(i);
      UI_CSSRULE*  r_i = match_rule.Get(i);

      XDWORD j = i;
      while(j > 0)
        {
          int s_p = match_spec.Get(j - 1);
          int x_p = match_index.Get(j - 1);

          bool greater_or_equal = (s_p < s_i) || (s_p == s_i && x_p <= x_i);
          if(greater_or_equal) break;

          match_spec .Set(j, match_spec .Get(j - 1));
          match_index.Set(j, match_index.Get(j - 1));
          match_rule .Set(j, match_rule .Get(j - 1));

          j--;
        }

      match_spec .Set(j, s_i);
      match_index.Set(j, x_i);
      match_rule .Set(j, r_i);
    }

  // Apply in ascending order: the winner (highest specificity, latest on ties) writes last and prevails.
  for(XDWORD c=0; c<match_rule.GetSize(); c++)
    {
      UI_CSSRULE* rule = match_rule.Get(c);
      if(!rule) continue;

      XVECTOR<UI_STYLEPROPERTY*>* declprops = rule->GetDeclarations().GetProperties();
      if(!declprops) continue;

      for(XDWORD d=0; d<declprops->GetSize(); d++)
        {
          UI_STYLEPROPERTY* prop = declprops->Get(d);
          if(prop) out.Set(prop->GetKey().Get(), prop->GetValue());
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_STYLESHEET::Variables_Get(XCHAR* name, XSTRING& out)
* @brief      Look up a theme variable by name ("--name"). Returns the stored value verbatim, without further
*             expansion (ExpandVariables() is expected to have run already if inter-variable references are used).
* @ingroup    USERINTERFACE
*
* @param[in]  name : Variable name including the leading "--".
* @param[out] out : Value string; emptied and left empty on miss.
*
* @return     bool : true if the variable exists; false otherwise.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_STYLESHEET::Variables_Get(XCHAR* name, XSTRING& out)
{
  return variables.Get(name, out);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_STYLESHEET::ExpandVariables()
* @brief      Fold every var(--name[, fallback]) reference embedded in the stylesheet. Runs two phases:
*               1. Self-expand variable values (up to 3 iterations) so a variable pointing at another variable
*                  ends up holding the transitive final value.
*               2. Walk every rule's declaration values and substitute in place.
*             Safe to call more than once. Cheap on stylesheets with no var() usage (linear scan of values).
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_STYLESHEET::ExpandVariables()
{
  // Phase 1: self-expand variable values. Up to 3 passes is enough to resolve one level of indirection while
  // remaining bounded even on pathological input (circular references stabilize at the last resolved form).
  for(int pass=0; pass<3; pass++)
    {
      bool changed = false;

      XVECTOR<UI_STYLEPROPERTY*>* props = variables.GetProperties();
      if(!props) break;

      for(XDWORD c=0; c<props->GetSize(); c++)
        {
          UI_STYLEPROPERTY* prop = props->Get(c);
          if(!prop) continue;

          XSTRING expanded;
          if(SubstituteVars(prop->GetValue(), expanded))
            {
              if(prop->GetValue().Compare(expanded, false) != 0)
                {
                  prop->GetValue().Set(expanded);
                  changed = true;
                }
            }
        }

      if(!changed) break;
    }

  // Phase 2: expand var() references inside regular rule declarations. Single pass is enough because Phase 1
  // has already resolved variable-to-variable indirection.
  for(XDWORD c=0; c<rules.GetSize(); c++)
    {
      UI_CSSRULE* rule = rules.Get(c);
      if(!rule) continue;

      XVECTOR<UI_STYLEPROPERTY*>* props = rule->GetDeclarations().GetProperties();
      if(!props) continue;

      for(XDWORD d=0; d<props->GetSize(); d++)
        {
          UI_STYLEPROPERTY* prop = props->Get(d);
          if(!prop) continue;

          XSTRING expanded;
          if(SubstituteVars(prop->GetValue(), expanded))
            {
              prop->GetValue().Set(expanded);
            }
        }
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_STYLESHEET::SubstituteVars(XSTRING& in, XSTRING& out)
* @brief      Substitute every "var(--name)" and "var(--name, fallback)" occurrence in `in` into `out`.
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* @param[in]  in : Source string.
* @param[out] out : Destination; emptied and rewritten. Left equal to `in` when no var() token is present.
*
* @return     bool : true if at least one substitution ran; false when `in` had no var() token.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_STYLESHEET::SubstituteVars(XSTRING& in, XSTRING& out)
{
  int   len       = (int)in.GetSize();
  int   pos       = 0;
  bool  didanyth  = false;

  out.Empty();

  while(pos < len)
    {
      int found = in.Find(__L("var("), false, pos);
      if(found < 0)
        {
          // No more var(): copy the rest verbatim and finish.
          XSTRING rest;
          in.Copy(pos, len, rest);
          out += rest;
          break;
        }

      // Copy prefix [pos, found) unchanged.
      if(found > pos)
        {
          XSTRING prefix;
          in.Copy(pos, found, prefix);
          out += prefix;
        }

      int argsstart = found + 4;                 // skip "var("
      int close     = in.FindCharacter(__C(')'), (XDWORD)argsstart);
      if(close < 0)
        {
          // Unterminated var(...): treat everything from "var(" onward as literal text.
          XSTRING rest;
          in.Copy(found, len, rest);
          out += rest;
          break;
        }

      // Parse inside the parens: "--name" or "--name, fallback".
      XSTRING args;
      in.Copy(argsstart, close, args);

      XSTRING varname;
      XSTRING fallback;

      int comma = args.FindCharacter(__C(','));
      if(comma >= 0)
        {
          args.Copy(0, comma, varname);
          args.Copy(comma + 1, (int)args.GetSize(), fallback);
        }
       else
        {
          varname = args;
        }

      // Trim whitespace around varname; trim only outer whitespace of fallback (interior commas / spaces are
      // legitimate parts of tuple values like "160,195,255").
      varname.DeleteNoCharacters(__L(" \t\r\n"));

      int  fb_len   = (int)fallback.GetSize();
      int  fb_start = 0;
      while(fb_start < fb_len)
        {
          XCHAR ch = fallback[fb_start];
          if(ch != __C(' ') && ch != __C('\t') && ch != __C('\r') && ch != __C('\n')) break;
          fb_start++;
        }

      int fb_end = fb_len;
      while(fb_end > fb_start)
        {
          XCHAR ch = fallback[fb_end - 1];
          if(ch != __C(' ') && ch != __C('\t') && ch != __C('\r') && ch != __C('\n')) break;
          fb_end--;
        }

      XSTRING fb_trimmed;
      if(fb_end > fb_start) fallback.Copy(fb_start, fb_end, fb_trimmed);

      // Substitute.
      XSTRING value;
      if(Variables_Get(varname.Get(), value))
        {
          out += value;
        }
       else if(!fb_trimmed.IsEmpty())
        {
          out += fb_trimmed;
        }
      // else: missing variable and no fallback -> empty substitution.

      didanyth = true;
      pos = close + 1;
    }

  return didanyth;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_STYLESHEET::HasPseudoRulesFor(XSTRING& elementtype, XSTRING& elementid, XVECTOR<XSTRING*>& elementclasses)
* @brief      Cheap probe: does the stylesheet contain any pseudo-carrying rule whose type/id/class part could
*             match this element? Used at load time to skip snapshotting and state hooks for elements that no
*             pseudo rule will ever restyle.
* @ingroup    USERINTERFACE
*
* @param[in]  elementtype : Element type string.
* @param[in]  elementid : Element id (name).
* @param[in]  elementclasses : Element class list.
*
* @return     bool : true if the element must subscribe to state re-resolution.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_STYLESHEET::HasPseudoRulesFor(XSTRING& elementtype, XSTRING& elementid, XVECTOR<XSTRING*>& elementclasses)
{
  // Simulate "every possible pseudo active" so any pseudo-carrying selector matches its structural constraints.
  // The heuristic accepts a false positive when the sheet has pseudo rules for a different element with the
  // same identity (rare and harmless: we just do a re-resolve that changes nothing), and avoids a false
  // negative: any rule that could ever match this element is caught.
  XVECTOR<XSTRING*> allpseudos;
  XSTRING           s_root       (__L("root"));
  XSTRING           s_preselect  (__L("preselect"));
  XSTRING           s_selected   (__L("selected"));
  XSTRING           s_active     (__L("active"));
  XSTRING           s_disabled   (__L("disabled"));
  XSTRING           s_hover      (__L("hover"));

  allpseudos.Add(&s_root);
  allpseudos.Add(&s_preselect);
  allpseudos.Add(&s_selected);
  allpseudos.Add(&s_active);
  allpseudos.Add(&s_disabled);
  allpseudos.Add(&s_hover);

  for(XDWORD c=0; c<rules.GetSize(); c++)
    {
      UI_CSSRULE* rule = rules.Get(c);
      if(!rule) continue;

      XVECTOR<UI_CSSSELECTOR*>& sels = rule->GetSelectors();

      for(XDWORD d=0; d<sels.GetSize(); d++)
        {
          UI_CSSSELECTOR* sel = sels.Get(d);
          if(!sel) continue;
          if(!sel->HasPseudos()) continue;

          if(sel->Match(elementtype, elementid, elementclasses, allpseudos))
            {
              // Detach borrowed pointers before returning: XVECTOR::DeleteAll would not free them (we didn't
              // allocate the XSTRINGs on the heap), but leaving them attached is harmless -- allpseudos is a
              // local variable and its destructor will not double-free non-owned entries.
              return true;
            }
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_STYLESHEET::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_STYLESHEET::Clean()
{

}
