/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPWINDOWSScreen.h
* 
* @class      GRPWINDOWSSCREEN
* @brief      WINDOWS Graphics Screen class
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

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <Windows.h>
#include <winuser.h>

#include "XString.h"

#include "GRPScreen.h"

#ifdef GRP_OPENGL_ACTIVE
class GRPWINDOWSBLITGLES;
#endif


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define GRPWINDOWSSCREEN_NAMECLASS  __L("GEN_Windows_Screen")



/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class GRPWINDOWSSCREEN : public GRPSCREEN
{
  public:
                                          GRPWINDOWSSCREEN                    ();
    virtual                              ~GRPWINDOWSSCREEN                    ();

    bool                                  Create                              (bool show);

    bool                                  Update                              ();
    bool                                  Update                              (GRP2DCANVAS* canvas);
    bool                                  UpdateTransparent                   (GRP2DCANVAS* canvas);

    bool                                  Get_Position                        (int &x, int &y);
    bool                                  Set_Position                        (int x, int y);

    bool                                  GetClientSize                       (int& width, int& height);

    #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
    bool                                  GetCursorDesktopPosition            (int& x, int& y);

    bool                                  BeginCFGChromesDrag                 ();
    bool                                  EndCFGChromesDrag                   ();
    #endif

    bool                                  Delete                              ();

    bool                                  Resize                              (int width, int height);
    
    bool                                  Show                                (bool active);
    bool                                  ShowCursor                          (bool active);
    
    void*                                 GetHandle                           ();
    void                                  SetHandle                           (void* handle_window);

    bool                                  Set_Focus                           ();

    bool                                  Minimize                            (bool active);
    bool                                  Maximize                            (bool active);

    GRPBITMAP*                            CaptureContent                      (GRPRECTINT* rect = NULL, void* handle_window = NULL);
    
    void*                                 GetDesktopHandle                    ();
    void*                                 GetShellHandle                      ();
    
    void*                                 GetHDC                              ();
    void                                  SetHDC                              (HDC hdc);

    BITMAPINFO*                           GetHInfo                            ();

    #ifdef GRP_OPENGL_ACTIVE
    GRPWINDOWSBLITGLES*                   GetBlitGLES                         ();
    #endif
    
  private:

    int                                   GetTaskbarHeight                    ();

    bool                                  Create_Window                       (bool show);

    void                                  Chromes_ApplyStyle                  (DWORD& style, DWORD& exstyle);
    void                                  Chromes_ApplyPostCreate             ();

    // NOTE: resize/rescale behaviour (no bitmap stretching -- growth capped at the
    // GRPVIEWPORT_ID_MAIN viewport's max size, content hidden below its min size). See the
    // WM_GETMINMAXINFO handling in BaseWndProc, and GRPWINDOWSBLITGLES::ComputePresentationScale
    // for the OpenGL presentation path.
    void                                  ApplyResizeLimits                   (MINMAXINFO* minmaxinfo);
    bool                                  IsAboveViewportMinimumSize          ();
    void                                  ClientSizeToWindowSize              (int clientwidth, int clientheight, DWORD style, int& windowwidth, int& windowheight);

    // Clears DWMWA_CLOAK (see the NOTE in Create_Window()) the first time it is called after a real frame
    // has been presented. A no-op once windowcloaked is already false, so it is safe/cheap to call from
    // Update(GRP2DCANVAS*) every single frame.
    void                                  Uncloak                             ();

    static  LRESULT CALLBACK              BaseWndProc                         (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    void                                  Clean                               ();

    WNDCLASSEX                            wndclass;
    XSTRING                               classname;
    XSTRING                               title;

    HINSTANCE                             hinstance;
    HWND                                  hwnd;
    HDC                                   hdc;

    // true from the moment Create_Window() cloaks a freshly created Custom-Chromes window (DWMWA_CLOAK)
    // until Uncloak() (called from Update(GRP2DCANVAS*), on the first real frame presented) clears it
    // again. See the NOTE beside the DwmSetWindowAttribute() call in Create_Window() for why only that
    // window style needs this, and the NOTE in Show() for why uncloaking was moved out of it. Also
    // checked by BaseWndProc's WM_SIZE handler (see its NOTE) so a spurious, startup-time WM_SIZE
    // cannot call Update(GRP2DCANVAS*)/Uncloak() early through that unrelated resize-drag path.
    bool                                   windowcloaked;

    DEVMODE                               devmode;

    BITMAPINFO                            hinfo;

    #ifdef GRP_OPENGL_ACTIVE
    GRPWINDOWSBLITGLES*                   blitgles;
    #endif
};



/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/




