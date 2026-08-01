/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPScreen.h
* 
* @class      GRPSCREEN
* @brief      Graphics Screen Class
* @ingroup    GRAPHIC
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

#include <stdio.h>

#include "XBase.h"
#include "XFactory.h"
#include "XVector.h"
#include "XEvent.h"
#include "XSubject.h"
#include "XPublisher.h"

#include "GRPProperties.h"
#include "GRPRect.h"
#include "GRPScreenCFGChromes.h"



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


enum GRPSCREENTYPE
{
  GRPSCREENTYPE_UNKNOW                        =  0  ,

  GRPSCREENTYPE_WINDOWS                             ,

  GRPSCREENTYPE_LINUX_FRAMEBUFFER                   ,
  GRPSCREENTYPE_LINUX_X11                           ,
  GRPSCREENTYPE_LINUX_WAYLAND                       ,
  GRPSCREENTYPE_LINUX_DISPMAN                       ,

  GRPSCREENTYPE_ANDROID
};


enum GRPSCREENTYPE_DESKTOP
{
  GRPSCREENTYPE_DESKTOP_ALL                   = -1  ,
  GRPSCREENTYPE_DESKTOP_MAIN                  =  0  ,
  GRPSCREENTYPE_DESKTOP_SCREEN1               =  0  ,
  GRPSCREENTYPE_DESKTOP_SCREEN2               =  1  ,
  GRPSCREENTYPE_DESKTOP_SCREEN3               =  2  ,  
  GRPSCREENTYPE_DESKTOP_SCREEN4               =  3  ,
  GRPSCREENTYPE_DESKTOP_SCREEN5               =  4  ,
  GRPSCREENTYPE_DESKTOP_SCREEN6               =  5  ,
  GRPSCREENTYPE_DESKTOP_SCREEN7               =  6  ,
  GRPSCREENTYPE_DESKTOP_SCREEN8               =  7  ,  
};


enum GRPSCREENROTATION
{
  GRPSCREENROTATION_NONE                      =  0  ,
  GRPSCREENROTATION_90_CLOCKWISE                    ,                  
  GRPSCREENROTATION_90_ANTICLOCKWISE                ,                  
  GRPSCREENROTATION_180                             ,                  
  
};


#define GRPSCREENSTYLE_NONE                           0x00000000
#define GRPSCREENSTYLE_TITLE                          0x00000001
#define GRPSCREENSTYLE_FULLSCREEN                     0x00000002
#define GRPSCREENSTYLE_FULLSCREEN_ADJUSTRESOLUTION    0x00000004
#define GRPSCREENSTYLE_TRANSPARENT                    0x00000008
#define GRPSCREENSTYLE_ONTOP                          0x00000010
#define GRPSCREENSTYLE_NOWINDOWICONS                  0x00000020
#define GRPSCREENSTYLE_HEIGHTWITHOUTTASKBAR           0x00000040
#define GRPSCREENSTYLE_NOICONTASKBAR                  0x00000080
#define GRPSCREENSTYLE_ZORDER                         0x00000100

#define GRPSCREENSTYLE_DEFAULT                        GRPSCREENSTYLE_TITLE

#ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
// Dead zone, in desktop pixels, the cursor must travel with the button held down on the caption before the
// window actually starts moving. Keeps a plain click on the caption (or on a caption element that later grows
// a behaviour of its own) from nudging the window by one or two pixels.
#define GRPSCREEN_CFGCHROMES_DRAGDEADZONE             4

// Define this to force the in-process drag even where GRPSCREEN::SystemMove() is available.
// #define GRPSCREEN_CFGCHROMES_NOSYSTEMMOVE
#endif




/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class GRP2DCANVAS;
class GRPVIEWPORT;
class GRPFRAMERATE;
class GRPBITMAP;
class GRPDESKTOPMANAGER;

#ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
class UI_LAYOUT;
class XTIMER;
#endif


class GRPSCREEN : public GRPPROPERTIES, public XSUBJECT
{
  public:
                                          GRPSCREEN                     ();
    virtual                              ~GRPSCREEN                     ();

    GRPSCREENTYPE                         GetType                       ();
    bool                                  IsValid                       ();
    bool                                  IsActive                      ();

    bool                                  Style_Is                      (XDWORD style);              
    void                                  Style_Add                     (XDWORD style);         
    void                                  Style_Remove                  (XDWORD style); 

    XDWORD                                Styles_Get                    ();          
    void                                  Styles_Set                    (XDWORD styles);   

    bool                                  Styles_IsFullScreen           ();      

    bool                                  SetPropertys                  (int width, int height, float DPIs, int stride, GRPPROPERTYMODE mode);

    GRPSCREENROTATION                     GetRotation                   ();
    void                                  SetRotation                   (GRPSCREENROTATION screenrotation);

    bool                                  Rotate                        (int degrees);

    bool                                  IsRotationOrthogonal          ();
    XDWORD                                GetPresentationWidth          ();
    XDWORD                                GetPresentationHeight         ();
    
    virtual bool                          Create                        (bool show);

    virtual bool                          Update                        (GRP2DCANVAS* canvas);
    virtual bool                          UpdateTransparent             (GRP2DCANVAS* canvas);

    virtual bool                          Delete                        ();

    virtual bool                          Get_Position                  (int &x, int &y);
    virtual bool                          Set_Position                  (int x, int y);

    virtual bool                          GetClientSize                 (int& width, int& height);
    bool                                  IsClientSizeAtMaximum         ();

    virtual bool                          Resize                        (int width, int height);
    
    virtual bool                          IsShow                        ();
    virtual bool                          Show                          (bool active);

    virtual bool                          ShowCursor                    (bool active);

    virtual void*                         GetHandle                     ();    
    virtual void                          SetHandle                     (void* handle_window);
 
    virtual bool                          Set_Focus                     ();

    virtual bool                          Minimize                      (bool active);
    virtual bool                          Maximize                      (bool active);

    virtual GRPBITMAP*                    CaptureContent                (GRPRECTINT* rect = NULL, void* handle_window = NULL);
    
    virtual void*                         GetDesktopHandle              ();
    virtual void*                         GetShellHandle                ();

    bool                                  CanClose                      ();    
    void                                  SetCanClose                   (bool canclose);    

    XSTRING*                              GetTitle                      ();

    GRPSCREENCFGCHROMES*                  GetCFGChromes                 ();
    bool                                  SetCFGChromes                 (GRPSCREENCFGCHROMES& cfgchromes);
    bool                                  IsCFGChromesActive            ();

    #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
    bool                                  LoadCFGChromesLayout          ();
    UI_LAYOUT*                            GetCFGChromesLayout           ();
    bool                                  UpdateCFGChromesDrag          ();
    bool                                  UpdateCFGChromesAutoHide      ();
    bool                                  UpdateCFGChromesButtonsPosition ();

    // Cursor position in DESKTOP (screen/root) coordinates, independent of this window's own position.
    // UpdateCFGChromesDrag() needs this and ONLY this: expressing the drag delta in a reference frame the
    // drag itself does not move removes the feedback loop that the previous client-relative math had (see
    // the long note on UpdateCFGChromesDrag()). Platforms that cannot report it return false and the drag
    // simply does not start, exactly as if there were no caption element -- no other behaviour changes.
    virtual bool                          GetCursorDesktopPosition      (int& x, int& y);

    // Window position the drag delta is added to, in the same space Set_Position() consumes. Default: the
    // stored position, which is what the Windows backend keeps exact at all times (WM_MOVE updates it
    // synchronously). Backends whose stored position can go stale override it -- see GRPLINUXSCREENX11, where
    // it is only refreshed once per rendered frame and a window the user or the WM moved in between would
    // otherwise anchor the drag to an old value and teleport the window on the first tick.
    virtual bool                          GetCFGChromesDragAnchor       (int& x, int& y);

    // Hands the whole interactive move over to the window manager / compositor, the way a native title bar
    // does it. Tried FIRST when a caption drag starts: when it succeeds there is no in-process loop at all,
    // so tracking is exactly 1:1 and snapping/tiling/multi-monitor come for free. Backends return false when
    // the mechanism is not available and the in-process drag below takes over unchanged.
    virtual bool                          SystemMove                    ();

    // Pointer capture for the duration of a caption drag. Without it the pointer stops being reported to
    // this window the moment it leaves the client area, which is what used to leave the window frozen
    // mid-drag. Default implementation does nothing (and says so by returning false), so a platform that
    // does not implement it behaves exactly as before.
    virtual bool                          BeginCFGChromesDrag           ();
    virtual bool                          EndCFGChromesDrag             ();

    // True only while the user is actually dragging the window by its custom caption. Lets the platform
    // input devices keep reporting the pointer during the drag (see INPWINDOWSDEVICEMOUSE::Update()).
    bool                                  IsCFGChromesDragging          ();

    // Runs the caption-drag tick on every live screen. Meant to be called by the platform main loop right
    // after the input devices are updated (MAINPROCWINDOWS::Update(), MAINPROCLINUX::Update()), which is a
    // whole rendered frame earlier than UpdateViewports() -- that one runs AFTER the application's own
    // DrawFrame(), and that delay is what made the pointer visibly outrun the window. UpdateCFGChromesDrag()
    // is idempotent (it recomputes an ABSOLUTE target from the current cursor position, it does not
    // accumulate), so the call still left in UpdateViewports() for main loops that do not use MAINPROC
    // remains harmless.
    static bool                           UpdateAllCFGChromesDrag       ();
    #endif
    
    bool                                  UpdateSize                    (int width, int height);

    XVECTOR<GRPVIEWPORT*>*                GetViewports                  ();
    GRPVIEWPORT*                          GetViewport                   (int index);
    GRPVIEWPORT*                          GetViewport                   (XCHAR* ID);
    bool                                  CreateViewport                (XCHAR* ID, float posx, float posy, float width, float height, XDWORD canvasposx, XDWORD canvasposy, XDWORD canvaswidth, XDWORD canvasheight);
    bool                                  UpdateViewports               ();
    bool                                  DeleteViewport                (GRPVIEWPORT* viewport);
    bool                                  DeleteAllViewports            ();
    
    GRPFRAMERATE*                         GetFrameRate                  ();

    GRPDESKTOPMANAGER*                    GetDesktopManager             ();
    GRPSCREENTYPE_DESKTOP                 GetDesktopScreenSelected      ();
    void                                  SetDesktopScreenSelected      (GRPSCREENTYPE_DESKTOP desktopscreenselected);

    static XMAP<void*, GRPSCREEN*>*       GetListScreens                ();

    GRP2DCANVAS*                          GetScreenCanvas               ();   
    bool                                  SetScreenCanvas               (GRP2DCANVAS* screencanvas);   

  protected:    

    GRPSCREENTYPE                         type;
    bool                                  isvalid;
    bool                                  isactive;
    XDWORD                                styles;
    GRPSCREENROTATION                     rotation;
       
    GRP2DCANVAS*                          screencanvas;  
    XVECTOR<GRPVIEWPORT*>                 viewports;  
    
    GRPFRAMERATE*                         framerate;

    bool                                  isshow;

    bool                                  canclose;  

    XSTRING                               title; 

    GRPSCREENCFGCHROMES                   cfgchromes;
    bool                                  cfgchromesactive;

    #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
    UI_LAYOUT*                            cfgchromeslayout;              // weak reference (owned by UI_MANAGER)

    bool                                  cfgchromesdragging;             // button went down on the caption and has not been released yet
    bool                                  cfgchromesdragmoving;           // ...and it has already travelled past the dead zone, so the window is being moved
    bool                                  cfgchromesdragbuttonwaspressed; // left button state on the previous tick, for our OWN press-edge detection
    int                                   cfgchromesdragstartcursorx;     // DESKTOP coordinates of the cursor when the drag started
    int                                   cfgchromesdragstartcursory;
    int                                   cfgchromesdragstartscreenx;     // window position when the drag started
    int                                   cfgchromesdragstartscreeny;
    int                                   cfgchromesdraglastposx;         // last position actually requested through Set_Position() during this drag
    int                                   cfgchromesdraglastposy;

    bool                                  cfgchromesautohidevisible;      // current actual shown/hidden state
    bool                                  cfgchromesautohidedesired;      // last computed "cursor is over it" state
    XTIMER*                               cfgchromesautohidetimer;        // how long "desired" has stayed stable

    int                                   cfgchromesbuttonsrefwidth;      // window width the chrome layout was positioned for (captured at load)
    int                                   cfgchromesbuttonsshift;         // horizontal shift currently applied to min/max/close so they stay visible
    #endif

  private:

    void                                  Clean                         ();

    #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
    bool                                  GetCFGChromesCursorPosition   (int& uix, int& uiy);
    bool                                  IsOverCFGChromesButton        (int uix, int uiy);
    #endif
     
    GRPDESKTOPMANAGER*                    desktopmanager;
    GRPSCREENTYPE_DESKTOP                 desktopscreenselected;

    static XMAP<void*, GRPSCREEN*>        listscreens;

    #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
    // Every live screen, platform independent. listscreens above is keyed by native handle and is only ever
    // populated by the Windows backend (its window procedure looks screens up by HWND there), so it cannot be
    // used to reach the X11 screens -- which is why UpdateAllCFGChromesDrag() found nothing to do on Linux.
    static XVECTOR<GRPSCREEN*>            allscreens;
    #endif
};




/*---- INLINE FUNCTIONS + PROTOTYPES ----------------------------------------------------------------------------------------------------------*/




