/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPLINUXScreenX11.h
* 
* @class      GRPLINUXSCREENX11
* @brief      LINUX Graphics X11 Screen class
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

#pragma once


#ifdef LINUX_X11_ACTIVE


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/


#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/extensions/Xrandr.h>

#include "GRPScreen.h"


#ifdef GRP_OPENGL_ACTIVE
class GRPLINUXBLITGLESX11;
#endif




/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define GRPLINUXSCREENX11_MWM_HINTS_FUNCTIONS   (1L << 0)
#define GRPLINUXSCREENX11_MWM_HINTS_DECORATIONS (1L << 1)

#define GRPLINUXSCREENX11_MWM_FUNC_ALL          (1L << 0)
#define GRPLINUXSCREENX11_MWM_FUNC_RESIZE       (1L << 1)
#define GRPLINUXSCREENX11_MWM_FUNC_MOVE         (1L << 2)
#define GRPLINUXSCREENX11_MWM_FUNC_MINIMIZE     (1L << 3)
#define GRPLINUXSCREENX11_MWM_FUNC_MAXIMIZE     (1L << 4)
#define GRPLINUXSCREENX11_MWM_FUNC_CLOSE        (1L << 5)

#define GRPLINUXSCREENX11_MWM_DECOR_ALL         (1L << 0)
#define GRPLINUXSCREENX11_MWM_DECOR_BORDER      (1L << 1)
#define GRPLINUXSCREENX11_MWM_DECOR_RESIZEH     (1L << 2)
#define GRPLINUXSCREENX11_MWM_DECOR_TITLE       (1L << 3)
#define GRPLINUXSCREENX11_MWM_DECOR_MENU        (1L << 4)
#define GRPLINUXSCREENX11_MWM_DECOR_MINIMIZE    (1L << 5)
#define GRPLINUXSCREENX11_MWM_DECOR_MAXIMIZE    (1L << 6)

// Custom chromes' OWN resize grip (see GRPLINUXSCREENX11::UpdateCustomChromesResize()). In custom-chromes
// mode the window is created with _MOTIF_WM_HINTS decorations = 0 (NOTHING native at all, see
// Create_Window()), so there is no WM-drawn resize border to grab: GEN implements the grip itself, as a band
// of this many pixels along the INSIDE of the window's right and bottom edges (the same approach a
// client-side-decorated GTK/Qt application uses). Right/bottom/bottom-right only, never left/top: the canvas
// is anchored to the window's TOP-LEFT corner (see GRPLINUXBLITGLESX11::ComputePresentationScale) so those
// are the edges that actually change what is visible, and keeping the top edge out of it means this can
// never fight GRPSCREEN::UpdateCFGChromesDrag()'s caption drag.
#define GRPLINUXSCREENX11_CUSTOMRESIZE_BORDER       8
#define GRPLINUXSCREENX11_CUSTOMRESIZE_MINSIZE      64

#define GRPLINUXSCREENX11_CUSTOMRESIZE_EDGE_RIGHT   (1 << 0)
#define GRPLINUXSCREENX11_CUSTOMRESIZE_EDGE_BOTTOM  (1 << 1)



/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


typedef struct
{
   unsigned long   flags;
   unsigned long   functions;
   unsigned long   decorations;
   long            inputMode;
   unsigned long   status;

} HITNS;


class GRP2DCANVAS;

class GRPLINUXSCREENX11 : public GRPSCREEN
{
  public:
                                          GRPLINUXSCREENX11                 ();
    virtual                              ~GRPLINUXSCREENX11                 ();

    bool                                  Create                            (bool show);

    bool                                  Update                            (GRP2DCANVAS* canvas);
    bool                                  UpdateTransparent                 (GRP2DCANVAS* canvas);
    bool                                  Delete                            ();

    bool                                  Resize                            (int width, int height);

    bool                                  Get_Position                      (int& x, int& y);
    bool                                  Set_Position                      (int x, int y);

    bool                                  GetClientSize                     (int& width, int& height);

    #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
    bool                                  GetCursorDesktopPosition          (int& x, int& y);
    bool                                  GetCFGChromesDragAnchor           (int& x, int& y);

    bool                                  SystemMove                        ();

    bool                                  BeginCFGChromesDrag               ();
    bool                                  EndCFGChromesDrag                 ();
    #endif

    bool                                  Show                              (bool active);
    bool                                  ShowCursor                        (bool active);

    bool                                  Minimize                          (bool active);
    bool                                  Maximize                          (bool active);

    void*                                 GetHandle                         ();
    
    bool                                  IsDesktop                         ();
    void                                  SetIsDesktop                      (bool isdesktop);

    bool                                  HasFocus                          ();

    Display*                              GetDisplay                        ();
    Window*                               GetWindow                         ();
    Window*                               GetWindowRoot                     ();
    XVisualInfo*                          GetVisualInfo                     ();

    #ifdef GRP_OPENGL_ACTIVE
    GRPLINUXBLITGLESX11*                  GetBlitGLES                       ();
    #endif

  protected:

    Display*                              display;

    // A SEPARATE X11 connection, used only by Set_Position() to move the window. WSLg's compositor appears to
    // silently ignore geometry-change requests for a window's frame when they arrive on the SAME connection
    // that owns the underlying client window (confirmed empirically: xdotool, a completely separate X client,
    // moves it just fine; the exact same XMoveWindow() on our own "display" connection does nothing) -- opening
    // a second connection and issuing the request from that one instead reproduces what an external tool does.
    Display*                              movedisplay;

    Window                                window;
    Window                                root;
    XVisualInfo                           vinfo;

    Atom                                  wmdeletewindow;

    bool                                  isdesktop;

    // Set to true the first time ApplyWMNormalHints() is called successfully from Update() (lazy
    // path). ApplyWMNormalHints() is also called from Create_Window(), but at that point the
    // GRPVIEWPORT_ID_MAIN viewport may not be configured yet, causing it to exit early without
    // setting PMaxSize. The first Update() call runs after the viewport is always available, so
    // re-calling it once from there is the reliable way to ensure KWin's maximize respects the
    // viewport's max (KWin reads PMaxSize from WM_NORMAL_HINTS to cap its maximize target).
    bool                                  normalhintsapplied;

    // State of GEN's own resize grip for custom-chromes windows (see UpdateCustomChromesResize() and the
    // GRPLINUXSCREENX11_CUSTOMRESIZE_* defines above). Start size/pointer position are captured once, when the
    // grip is first grabbed, and every later frame resizes relative to THOSE, not to the previous frame -- so a
    // pointer that outruns the window (or leaves it entirely) mid-drag cannot make the size drift.
    bool                                  customresizing;
    int                                   customresizeedge;
    bool                                  customresizeprevpressed;
    int                                   customresizestartrootx;
    int                                   customresizestartrooty;
    int                                   customresizestartwidth;
    int                                   customresizestartheight;

    #ifdef GRP_OPENGL_ACTIVE
    GRPLINUXBLITGLESX11*                  blitgles;
    #endif

  private:

    #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
    bool                                  IsWMSupported                     (Atom atom);
    #endif


    bool                                  Create_Window                     (bool show);

    void                                  Chromes_ApplyStyle                ();
    void                                  Chromes_ApplyPostCreate           ();

    // Single source of truth for "is this screen using GEN's own custom-drawn chrome instead of
    // native/WM decoration" -- mirrors the usecfgchromescustom local in Create_Window() exactly
    // (IsCFGChromesActive() && !GetCFGChromes()->GetUseNativeChromes() && !TRANSPARENT). Selects
    // the no-native-decoration-at-all path in Create_Window() (_MOTIF_WM_HINTS decorations = 0) and
    // enables GEN's own resize grip (UpdateCustomChromesResize()) that replaces the WM-drawn resize
    // border that path gives up. Does NOT gate the growth cap: ApplyWMNormalHints() and Update()'s
    // reactive snap-back apply to custom-chromes windows exactly like any other, which is what keeps
    // "the window can never be bigger than the viewport" true in this mode too.
    bool                                  IsUsingCustomChromes              ();

    // GEN's own resize grip, for custom-chromes windows only -- the replacement for the WM-drawn
    // resize border that asking for _MOTIF_WM_HINTS decorations = 0 necessarily gives up (see
    // Create_Window(): on X11 there is no way to ask a window manager for "resize border but no
    // titlebar" that works across WMs -- a Motif decorations mask of MWM_DECOR_RESIZEH is a
    // NON-ZERO mask, and WSLg's compositor, like several others, treats any non-zero mask as
    // "decorate this window", drawing its FULL native frame -- titlebar included -- on top of
    // GEN's custom-drawn caption. Only decorations = 0 reliably means "no native decoration".)
    // Polls the pointer every frame from Update() and does the resize itself, in-process, via
    // XResizeWindow, clamped to the viewport's max exactly like every other resize path here.
    void                                  UpdateCustomChromesResize         ();

    // Resolves the GRPVIEWPORT_ID_MAIN viewport's max size, shared by ApplyWMNormalHints() (the
    // proactive WM hint) and Update()'s reactive snap-back (the WM-independent guarantee -- some
    // X11 window managers do not honour WM_NORMAL_HINTS' PMaxSize for interactive border-drag
    // resize, so the hint alone is not a reliable enforcement mechanism the way Win32's
    // WM_GETMINMAXINFO is). An EXPLICIT GRPVIEWPORT::SetMaxSize() on an axis wins; otherwise that
    // axis falls back to the viewport's own declared size (GetWidth()/GetHeight() -- the
    // design/content resolution); if that is ALSO unset, falls back to this screen's own
    // GRPPROPERTIES max (GetMaxWidth()/GetMaxHeight(), set in the constructor to the X11 display's
    // own resolution), so neither axis is ever left at a literal 0 when the other has a real value.
    // Leaves maxw/maxh untouched (caller's responsibility to have zeroed them) if no viewport is
    // configured yet.
    void                                  ResolveViewportMax                (float& maxw, float& maxh);

    // X11 equivalent of GRPWINDOWSSCREEN::ApplyResizeLimits's WM_GETMINMAXINFO handling: sets a
    // PMaxSize XSizeHints property (via XSetWMNormalHints) so a compliant EWMH window manager caps
    // this window's growth -- both interactive border-drag resize and the native Maximize request
    // (see Maximize() below, which sends the _NET_WM_STATE_MAXIMIZED_HORZ/VERT ClientMessage this
    // hint constrains) -- at the GRPVIEWPORT_ID_MAIN viewport's declared max size. Deliberately does
    // NOT set PMinSize (no OS-enforced minimum): see GRPLINUXBLITGLESX11::ComputePresentationScale
    // for how content below the viewport's min is hidden instead. Only called for native-chromes,
    // non-fullscreen windows -- see IsUsingCustomChromes()'s comment above for why.
    void                                  ApplyWMNormalHints                ();

    bool                                  ChangeScreenResolution            (int width, int height);

    XImage*                               CreateXImageFromBuffer            (Display* display, int screen, XBYTE* buffer, int width, int height);

    XBYTE*                                ScaleBufferNearestLetterbox       (XBYTE* src, int srcw, int srch, int dstw, int dsth);

    int                                   GetTaskBarHeight                  ();

    bool                                  ShowDebugNetSupportedPropertys    ();

    int                                   GetByteOrder                      ();

    static int                            ErrorHandler                      (Display* display, XErrorEvent* errorevent);

    void                                  Clean                             ();
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/




#endif



