/**-------------------------------------------------------------------------------------------------------------------
*
* @file       GRPLINUXBlitGLESWayland.cpp
*
* @class      GRPLINUXBLITGLESWAYLAND
* @brief      Linux Wayland specialisation. Provides wl_display* and a struct wl_egl_window* to EGL.
* @ingroup    PLATFORM_LINUX
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

#if defined(GRP_OPENGL_ACTIVE) && defined(LINUX_WAYLAND_ACTIVE)


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "GRPLINUXBlitGLESWayland.h"

#include "GRPLINUXScreenWayland.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPLINUXBLITGLESWAYLAND::GRPLINUXBLITGLESWAYLAND()
* @brief      Constructor of class
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPLINUXBLITGLESWAYLAND::GRPLINUXBLITGLESWAYLAND() : GRPBLITGLES()
{
  eglwindow = NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPLINUXBLITGLESWAYLAND::~GRPLINUXBLITGLESWAYLAND()
* @brief      Destructor of class
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPLINUXBLITGLESWAYLAND::~GRPLINUXBLITGLESWAYLAND()
{
  // Unlike GRPLINUXBLITGLESX11 on X11 (which only ever borrows the raw Window XID it never
  // created), this class DOES own eglwindow -- it was allocated by GetNativeWindow() below via
  // wl_egl_window_create() -- so it must be released here. GRPBLITGLES::Destroy() (called by the
  // base destructor before this body runs) has already torn down the EGL surface/context that
  // was built on top of it.
  if(eglwindow)
    {
      wl_egl_window_destroy(eglwindow);
      eglwindow = NULL;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         EGLNativeDisplayType GRPLINUXBLITGLESWAYLAND::GetNativeDisplay()
* @brief      Get native display
* @ingroup    PLATFORM_LINUX
*
* @return     EGLNativeDisplayType : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
EGLNativeDisplayType GRPLINUXBLITGLESWAYLAND::GetNativeDisplay()
{
  GRPLINUXSCREENWAYLAND* ws = (GRPLINUXSCREENWAYLAND*)screen;
  if(!ws) return (EGLNativeDisplayType)EGL_DEFAULT_DISPLAY;

  struct wl_display* dpy = ws->GetWLDisplay();
  return dpy ? (EGLNativeDisplayType)dpy : (EGLNativeDisplayType)EGL_DEFAULT_DISPLAY;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         EGLNativeWindowType GRPLINUXBLITGLESWAYLAND::GetNativeWindow()
* @brief      Get native window. Wraps the screen's wl_surface in a struct wl_egl_window*
*             (created once, lazily) -- EGL cannot take a raw wl_surface* directly the way it
*             takes a raw X11 Window XID. Also keeps the wl_egl_window's internal size in sync
*             with the current GEN screen size on every call: unlike an X11 Window (which IS
*             already the right size the moment XResizeWindow() returns), a wl_egl_window has
*             its own separately-tracked buffer size that only wl_egl_window_resize() updates.
* @ingroup    PLATFORM_LINUX
*
* @return     EGLNativeWindowType : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
EGLNativeWindowType GRPLINUXBLITGLESWAYLAND::GetNativeWindow()
{
  GRPLINUXSCREENWAYLAND* ws = (GRPLINUXSCREENWAYLAND*)screen;
  if(!ws) return (EGLNativeWindowType)0;

  struct wl_surface* surface = ws->GetWLSurface();
  if(!surface) return (EGLNativeWindowType)0;

  int width  = (int)ws->GetWidth();
  int height = (int)ws->GetHeight();
  if(width  <= 0) width  = GRPLINUXSCREENWAYLAND_DEFAULT_WIDTH;
  if(height <= 0) height = GRPLINUXSCREENWAYLAND_DEFAULT_HEIGHT;

  if(!eglwindow)
    {
      eglwindow = wl_egl_window_create(surface, width, height);
    }
   else
    {
      wl_egl_window_resize(eglwindow, width, height, 0, 0);
    }

  return eglwindow ? (EGLNativeWindowType)eglwindow : (EGLNativeWindowType)0;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPLINUXBLITGLESWAYLAND::GetNativeWindowSize(int& width, int& height)
* @brief      Report the CURRENT screen size, the same role GRPLINUXBLITGLESX11::GetNativeWindowSize()
*             plays on X11 (there via XGetWindowAttributes()). Lets the base blitter detect a
*             resize (compositor-driven here, via xdg_toplevel::configure -- see
*             GRPLINUXSCREENWAYLAND::XDGToplevel_Configure()) and recreate the EGL surface to
*             match, the same way GRPANDROIDBLITGLES does for a rotation.
* @ingroup    PLATFORM_LINUX
*
* @param[in]  width : Width value.
* @param[in]  height : Height value.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPLINUXBLITGLESWAYLAND::GetNativeWindowSize(int& width, int& height)
{
  GRPLINUXSCREENWAYLAND* ws = (GRPLINUXSCREENWAYLAND*)screen;
  if(!ws) return false;

  width  = (int)ws->GetWidth();
  height = (int)ws->GetHeight();

  return (width > 0 && height > 0);
}


#endif // GRP_OPENGL_ACTIVE && LINUX_WAYLAND_ACTIVE
