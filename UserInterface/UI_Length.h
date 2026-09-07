/**-------------------------------------------------------------------------------------------------------------------
*
* @file       UI_Length.h
*
* @class      UI_LENGTH
* @brief      User Interface : a typed CSS-style length value (number / percentage / bare keyword).
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
* SCOPE (Phase 1 -- "estilo calculado tipado", second increment)
*   Today UI_MANAGER::GetLayoutElement_Base() sniffs "xpos"/"ypos"/"width"/"height" by hand, four near-identical
*   times, with a chain of XSTRING::Compare() against keywords ("left"/"right"/"center"/"up"/"down"/"max"/"auto")
*   before falling back to ResolvePercentValue() or a plain number. UI_LENGTH packages that classification (is
*   this raw string a number, a "N%" percentage, or some other bare keyword?) and the number/percentage ->
*   pixel resolution into one typed, tested value, so a caller reads the RESULT of parsing once instead of
*   re-deriving it from string comparisons. The KEYWORD case is deliberately left uninterpreted here: which
*   keywords are legal, and what each one means, is still property-specific (xpos's "left" is not width's
*   "auto"), so that mapping stays where it already lives, in GetLayoutElement_Base() itself.
*
* @author     Abraham J. Velez / EndoraSoft
*
* ---------------------------------------------------------------------------------------------------------------------*/

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XString.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


enum UI_LENGTH_TYPE
{
  UI_LENGTH_TYPE_UNDEFINED = 0,                 // empty / absent source string: Parse() was never given anything usable
  UI_LENGTH_TYPE_NUMBER,                        // plain number, e.g. "42"
  UI_LENGTH_TYPE_PERCENT,                       // "N%", e.g. "50%" (GetValue() returns 50.0, not 0.5)
  UI_LENGTH_TYPE_KEYWORD,                       // anything else non-numeric, e.g. "left", "auto", "max"
};


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class UI_LENGTH
{
  public:
                                    UI_LENGTH                   ();
    virtual                        ~UI_LENGTH                  ();

    // Classifies "raw" into UNDEFINED / NUMBER / PERCENT / KEYWORD. Always succeeds (an unparseable numeric
    // string is treated the same way XSTRING::ConvertToDouble() already treats it elsewhere in this subsystem);
    // returns false only for an empty "raw", exactly like UI_STYLE::Get() returning false for an absent key.
    bool                            Parse                       (XSTRING& raw);

    UI_LENGTH_TYPE                  GetType                     ();

    // Raw magnitude for NUMBER (pixels, GEN's only unit today) and PERCENT (0..100, NOT 0..1). Meaningless for
    // KEYWORD/UNDEFINED -- always 0.0 there.
    double                          GetValue                    ();

    // Case-insensitive match against the keyword text. False (never a crash) when GetType() is not KEYWORD.
    bool                            IsKeyword                   (XCHAR* keyword);

    // Resolves NUMBER (basis ignored) or PERCENT (value/100 * basis) into "out". False for KEYWORD/UNDEFINED --
    // the caller is expected to have already handled its own keyword set via IsKeyword() before falling back
    // to Resolve(), exactly the order GetLayoutElement_Base() already used before this type existed.
    bool                            Resolve                     (double basis, double& out);

  private:

    void                            Clean                       ();

    UI_LENGTH_TYPE                   type;
    double                           value;
    XSTRING                          keyword;
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
