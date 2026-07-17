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

    bool                                  Set_Position                      (int x, int y);

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

    #ifdef GRP_OPENGL_ACTIVE
    GRPLINUXBLITGLESX11*                  blitgles;
    #endif

  private:

    bool                                  Create_Window                     (bool show);

    void                                  Chromes_ApplyStyle                ();
    void                                  Chromes_ApplyPostCreate           ();

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



