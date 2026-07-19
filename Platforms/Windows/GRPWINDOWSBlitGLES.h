/**-------------------------------------------------------------------------------------------------------------------
*
* @file       GRPWINDOWSBlitGLES.h
*
* @class      GRPWINDOWSBLITGLES
* @brief      Windows specialisation of GRPBLITGLES. Uses ANGLE to translate GLES3 -> D3D11.
* @ingroup    PLATFORM_WINDOWS
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

#pragma once

#ifdef GRP_OPENGL_ACTIVE

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <Windows.h>

#include "GRPBlitGLES.h"


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class GRPWINDOWSBLITGLES : public GRPBLITGLES
{
  public:
                                          GRPWINDOWSBLITGLES                ();
    virtual                              ~GRPWINDOWSBLITGLES                ();

    EGLNativeDisplayType                  GetNativeDisplay                  ();
    EGLNativeWindowType                   GetNativeWindow                   ();

    // Windows: no bitmap rescaling. The canvas is presented at its native pixel size (1 texel = 1
    // pixel), growing/shrinking independently on each axis with the window instead of being
    // stretched/letterboxed to fill it. It is anchored to the window's TOP-LEFT corner (not
    // centered): growing the window reveals more background at the bottom/right, and shrinking it
    // crops the canvas' bottom/right edges first, regardless of which edge/corner was dragged. See
    // GRPWINDOWSSCREEN::BaseWndProc (WM_GETMINMAXINFO) for the native-window-side growth cap at the
    // viewport's max size; this method is what makes the content itself hide, rather than shrink,
    // below the viewport's min size.
    void                                   ComputePresentationScale          (GLsizei surfacewidth, GLsizei surfaceheight, float& scalex, float& scaley, float& translatex, float& translatey, bool& visible);
};


#endif // GRP_OPENGL_ACTIVE
