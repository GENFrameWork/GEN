/**-------------------------------------------------------------------------------------------------------------------
*
* @file       GRPWINDOWSBlitGLES.cpp
*
* @class      GRPWINDOWSBLITGLES
* @brief      Windows specialisation. ANGLE accepts HDC as EGLNativeDisplayType and HWND as
*             EGLNativeWindowType. The HDC is obtained from the screen at this point.
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"

#ifdef GRP_OPENGL_ACTIVE


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "GRPWINDOWSBlitGLES.h"

#include "GRPWINDOWSScreen.h"
#include "GRPViewPort.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPWINDOWSBLITGLES::GRPWINDOWSBLITGLES()
* @brief      Constructor of class
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPWINDOWSBLITGLES::GRPWINDOWSBLITGLES() : GRPBLITGLES() 
{ 

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPWINDOWSBLITGLES::~GRPWINDOWSBLITGLES()
* @brief      Destructor of class
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPWINDOWSBLITGLES::~GRPWINDOWSBLITGLES()               
{ 

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         EGLNativeDisplayType GRPWINDOWSBLITGLES::GetNativeDisplay()
* @brief      Get native display
* @ingroup    PLATFORM_WINDOWS
* 
* @return     EGLNativeDisplayType : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
EGLNativeDisplayType GRPWINDOWSBLITGLES::GetNativeDisplay()
{
  GRPWINDOWSSCREEN* ws = (GRPWINDOWSSCREEN*)screen;
  if(!ws) 
    {
      return (EGLNativeDisplayType)EGL_DEFAULT_DISPLAY;
    }

  HDC hdc = (HDC)ws->GetHDC();
  return hdc ? (EGLNativeDisplayType)hdc : (EGLNativeDisplayType)EGL_DEFAULT_DISPLAY;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         EGLNativeWindowType GRPWINDOWSBLITGLES::GetNativeWindow()
* @brief      Get native window
* @ingroup    PLATFORM_WINDOWS
* 
* @return     EGLNativeWindowType : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
EGLNativeWindowType GRPWINDOWSBLITGLES::GetNativeWindow()
{
  GRPWINDOWSSCREEN* ws = (GRPWINDOWSSCREEN*)screen;
  return ws ? (EGLNativeWindowType)ws->GetHandle() : (EGLNativeWindowType)NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRPWINDOWSBLITGLES::ComputePresentationScale(GLsizei surfacewidth, GLsizei surfaceheight, float& scalex, float& scaley, float& translatex, float& translatey, bool& visible)
* @brief      Windows presentation policy: NO bitmap rescaling. The canvas is drawn at its native pixel size
*             (1 texel = 1 pixel) regardless of the drawable surface size, so growing the window shows more
*             background around the (unscaled) canvas, and shrinking it crops the canvas -- it is never
*             stretched. Width and height are resolved independently, matching the same independence at the
*             window-resize level (see GRPWINDOWSSCREEN::BaseWndProc, WM_GETMINMAXINFO).
* @note       Anchored to the window's TOP-LEFT corner, not centered: the canvas' own top-left corner always
*             stays pinned to the window's top-left corner. Growing the window only reveals more background
*             at the bottom/right; shrinking it only crops the canvas' bottom/right edges -- regardless of
*             which edge or corner the user actually dragged to resize (Windows always reports the same
*             final client width/height either way, which is all this method sees).
*             The window itself is already prevented from growing past the GRPVIEWPORT_ID_MAIN viewport's
*             max size (WM_GETMINMAXINFO caps the resize border and the native Maximize there), so the
*             "beyond max" half of the requirement is enforced natively and needs no handling here. This
*             method only needs to cover the "hide below min" half: whenever the surface (i.e. the window's
*             client area) is smaller than that viewport's declared minimum, on either axis, the canvas is
*             not drawn at all (the surface is left cleared/blank) instead of being shrunk to fit.
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  surfacewidth : Width, in pixels, of the drawable surface (matches the native window's client area).
* @param[in]  surfaceheight : Height, in pixels, of the drawable surface.
* @param[out] scalex : Resulting NDC X scale to apply to the fullscreen quad (native size, never a stretch).
* @param[out] scaley : Resulting NDC Y scale to apply to the fullscreen quad (native size, never a stretch).
* @param[out] translatex : Resulting NDC X shift so the LEFT edge of the quad stays pinned to the window's left edge.
* @param[out] translatey : Resulting NDC Y shift so the TOP edge of the quad stays pinned to the window's top edge.
* @param[out] visible : true if the canvas should be drawn at all; false to leave the surface cleared (blank).
*
* --------------------------------------------------------------------------------------------------------------------*/
void GRPWINDOWSBLITGLES::ComputePresentationScale(GLsizei surfacewidth, GLsizei surfaceheight, float& scalex, float& scaley, float& translatex, float& translatey, bool& visible)
{
  scalex     = 1.0f;
  scaley     = 1.0f;
  translatex = 0.0f;
  translatey = 0.0f;
  visible    = true;

  if(surfacewidth <= 0 || surfaceheight <= 0 || texw <= 0 || texh <= 0)
    {
      visible = false;
      return;
    }

  // Native 1:1 mapping: the quad's NDC half-extent (1.0) must cover exactly texw/texh pixels out of
  // a surfacewidth/surfaceheight-sized viewport. texw/texh are swapped for a 90/270 rotated
  // presentation, same convention as the base (shared) letterbox implementation.
  int effectivetexw = texw;
  int effectivetexh = texh;

  if(rotation == GRPSCREENROTATION_90_CLOCKWISE || rotation == GRPSCREENROTATION_90_ANTICLOCKWISE)
    {
      effectivetexw = texh;
      effectivetexh = texw;
    }

  scalex = (float)effectivetexw / (float)surfacewidth;
  scaley = (float)effectivetexh / (float)surfaceheight;

  // Top-left anchor: in NDC, x=-1 is the window's LEFT edge and y=+1 is its TOP edge (canonical
  // convention used throughout this pipeline -- see BuildQuad's UV mapping). After the scale above,
  // the quad spans [-scalex,+scalex] x [-scaley,+scaley], CENTERED at the origin. Shifting it by
  // (scalex-1, 1-scaley) moves its left edge to -1 and its top edge to +1 -- pinning that corner --
  // while the OPPOSITE (bottom-right) edge/corner is the one that now grows or gets clipped away as
  // the window is resized. This is a pure screen-space shift (see BuildModelMatrix), so it holds
  // regardless of any rotation/flip also baked into the matrix.
  translatex = scalex - 1.0f;
  translatey = 1.0f  - scaley;

  GRPWINDOWSSCREEN* ws = (GRPWINDOWSSCREEN*)screen;
  if(!ws)
    {
      return;
    }

  GRPVIEWPORT* mainviewport = ws->GetViewport(GRPVIEWPORT_ID_MAIN);
  if(!mainviewport)
    {
      return; // no viewport configured yet: no min/max restriction to apply
    }

  float minwidth  = mainviewport->GetMinWidth();
  float minheight = mainviewport->GetMinHeight();

  if((minwidth  > 0.0f && surfacewidth  < (GLsizei)minwidth) ||
     (minheight > 0.0f && surfaceheight < (GLsizei)minheight))
    {
      visible = false;
    }
}


#endif // GRP_OPENGL_ACTIVE
