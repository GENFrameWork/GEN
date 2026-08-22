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
* @author     Abraham J. Velez / EndoraSoft
*
* ---------------------------------------------------------------------------------------------------------------------*/

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XPath.h"
#include "XString.h"

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


  private:


    void                            SkipWhitespaceAndComments   (XSTRING& text, int& pos);
    bool                            ReadSelectorList            (XSTRING& text, int& pos, UI_CSSRULE* rule);
    bool                            ReadDeclarationBlock        (XSTRING& text, int& pos, UI_CSSRULE* rule);
    void                            SkipToNextRule              (XSTRING& text, int& pos);



    UI_CSSSELECTOR*                 ParseCompoundSelector       (XSTRING& text, int start, int end);

    void                            Clean                       ();
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
