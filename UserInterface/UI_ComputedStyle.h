/**-------------------------------------------------------------------------------------------------------------------
*
* @file       UI_ComputedStyle.h
*
* @class      UI_COMPUTEDSTYLE
* @brief      User Interface : typed accessors layered on top of the neutral UI_STYLE property bag.
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
*   UI_COMPUTEDSTYLE IS-A UI_STYLE: it inherits the existing (key -> string) bag and every FillFrom*() front-end
*   untouched, and adds typed getters on top (GetLength(), GetColor()) plus alias-aware variants that go through
*   UI_PROPERTYREGISTRY::GetAliased() instead of a caller re-implementing the "try the legacy name, fall back to
*   the CSS name" pattern by hand.
*
*   This is the type GetLayoutElement_Base() now builds and hands back to its per-widget callers (see the new
*   GetLayoutElement_Base(..., UI_COMPUTEDSTYLE&, ...) overload in UI_Manager.h/.cpp), so a per-type builder like
*   GetLayoutElement_Text() can read "sizefont" from the SAME fully-resolved bag (XML attributes < CSS rules <
*   inline style) that GetLayoutElement_Base() already used for "color"/"width"/etc., instead of re-reading the
*   raw XFILEXMLELEMENT* and silently losing any CSS/inline override -- the exact P0 gap the UI/CSS analysis
*   report identified as 50 direct-read call sites across UI_Manager.cpp's per-widget builders.
*
* @author     Abraham J. Velez / EndoraSoft
*
* ---------------------------------------------------------------------------------------------------------------------*/

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "UI_Style.h"
#include "UI_Length.h"
#include "UI_Color.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class UI_COMPUTEDSTYLE : public UI_STYLE
{
  public:
                                    UI_COMPUTEDSTYLE            ();
    virtual                        ~UI_COMPUTEDSTYLE            ();

    // Alias-aware string lookup: thin wrapper over UI_PROPERTYREGISTRY::GetAliased(), so a builder does not
    // need its own copy of the "try primarykey, fall back to secondarykey" pattern.
    bool                            GetAliased                  (XCHAR* primarykey, XCHAR* secondarykey, XSTRING& value);

    // Parses the raw string at "key" into a UI_LENGTH (see UI_Length.h for NUMBER/PERCENT/KEYWORD). False if
    // the key is absent, exactly like UI_STYLE::Get().
    bool                            GetLength                   (XCHAR* key, UI_LENGTH& value);
    bool                            GetLength                   (XCHAR* primarykey, XCHAR* secondarykey, UI_LENGTH& value);

    // Parses the raw string at "key" through UI_COLOR::SetFromString() (accepts "R,G,B[,A]", "#RRGGBB[AA]" and
    // named colours -- whatever SetFromString() already accepts elsewhere in this subsystem). False if the key
    // is absent or the string failed to parse into a valid colour.
    bool                            GetColor                     (XCHAR* key, UI_COLOR& color);
    bool                            GetColor                     (XCHAR* primarykey, XCHAR* secondarykey, UI_COLOR& color);
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
