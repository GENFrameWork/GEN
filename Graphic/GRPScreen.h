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

    // NOTE: only meaningful when a Chromes configuration is active AND GetCFGChromes()->GetUseNativeChromes()
    // is false; a native-chromes screen (the default, and every screen that never calls SetCFGChromes()) never
    // needs any of this, and this whole feature can be compiled out entirely with GRP_SCREEN_CUSTOMCHROMES_FEATURE.
    // Call once the screen already has a viewport/canvas (native window created); resolves and loads
    // GetCFGChromes()->GetCustomLayoutFile()/GetCustomLayoutName() (a plain .xml, a .xml shared with other
    // layouts, or a whole .xml+resources bundle compressed as .zip -- UI_MANAGER::Load() already tells these
    // apart by extension) and keeps a (weak, UI_MANAGER-owned) reference to the resulting layout.
    bool                                  LoadCFGChromesLayout          ();
    UI_LAYOUT*                            GetCFGChromesLayout           ();

    // Window drag over the layout's role="caption" element (see UI_ELEMENT_CHROMEROLE). Reads GEN's own INPUT
    // module directly (INPMANAGER/INPDEVICE/INPCURSOR/INPBUTTON) -- entirely independent of whatever input
    // handling the running application does for its own purposes; call once per frame (e.g. alongside
    // UpdateViewports()). A no-op for a native-chromes screen, or one with no role="caption" element at all.
    bool                                  UpdateCFGChromesDrag          ();

    // Auto-hide/show the layout's role="caption" element based on GetCFGChromes()->GetCustomAutoHide() and
    // whether the cursor is currently over its own area; see GRPSCREENCFGCHROMES::GetCustomAutoHide(). Same
    // INPUT-module-direct approach as UpdateCFGChromesDrag(); call once per frame alongside it. A no-op when
    // auto-hide is 0 (disabled, the default), for a native-chromes screen, or one with no role="caption".
    bool                                  UpdateCFGChromesAutoHide      ();

    // The chrome layout and the app's own content layout are two entirely separate UI_LAYOUT objects, each
    // with its own independent rebuild-area bookkeeping, even though both share the same physical canvas;
    // GEN_USERINTERFACE.Update() draws every loaded layout in load order (chrome first, content afterwards --
    // see LoadCFGChromesLayout()), so whenever the content redraws a region overlapping the caption, it paints
    // right over it. Call this once, from within the app's OWN frame-drawing function, immediately after its
    // own GEN_USERINTERFACE.Update() call for that frame (NOT from UpdateViewports(), which runs later and
    // outside that frame's rebuild-area cycle): it redraws the chrome layout's own elements again, so the
    // caption ends up on top regardless of what the content just did, correctly participating in this frame's
    // capture/restore cycle so a future frame can still undo it properly.
    bool                                  DrawCFGChromesOnTop           ();

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

    bool                                  cfgchromesdragging;
    int                                   cfgchromesdragstartcursorx;
    int                                   cfgchromesdragstartcursory;
    int                                   cfgchromesdragstartscreenx;
    int                                   cfgchromesdragstartscreeny;

    bool                                  cfgchromesautohidevisible;      // current actual shown/hidden state
    bool                                  cfgchromesautohidedesired;      // last computed "cursor is over it" state
    XTIMER*                               cfgchromesautohidetimer;        // how long "desired" has stayed stable
    #endif

  private:

    void                                  Clean                         ();

    #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
    bool                                  GetCFGChromesCursorPosition   (int& uix, int& uiy);
    #endif
     
    GRPDESKTOPMANAGER*                    desktopmanager;
    GRPSCREENTYPE_DESKTOP                 desktopscreenselected;

    static XMAP<void*, GRPSCREEN*>        listscreens;
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/




