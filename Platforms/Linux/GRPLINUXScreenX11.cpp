/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPLINUXScreenX11.cpp
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


#ifdef LINUX_X11_ACTIVE


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/


#include "GRPLINUXScreenX11.h"

#include "XTrace.h"
#include "XBuffer.h"

#include "GRP2DCanvas.h"
#include "GRPDesktopManager.h"

#include "MainProcLINUX.h"
#include "APPFlowGraphics.h"


#ifdef GRP_OPENGL_ACTIVE
#include "GRPLINUXBlitGLES.h"
#include <stdio.h>
#include <string.h>
#endif


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


#ifdef GRP_OPENGL_ACTIVE
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         static bool IsRunningOnWSL()
* @brief      Detect WSL/WSLg at runtime by reading /proc/version.
* @ingroup    PLATFORM_LINUX
* 
* @return     static bool : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
static bool IsRunningOnWSL()
{
  FILE* f = fopen("/proc/version", "r");
  if(!f) return false;
  char buf[512];
  size_t n = fread(buf, 1, sizeof(buf)-1, f);
  fclose(f);
  buf[n] = '\0';
  return (strstr(buf, "microsoft") != NULL || strstr(buf, "Microsoft") != NULL || strstr(buf, "WSL") != NULL);
}

#endif


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPLINUXSCREENX11::GRPLINUXSCREENX11(): GRPSCREEN()
* @brief      Constructor of class
* @ingroup    PLATFORM_LINUX
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPLINUXSCREENX11::GRPLINUXSCREENX11(): GRPSCREEN()
{
  Clean();

  type = GRPSCREENTYPE_LINUX_X11;

  //display = XOpenDisplay(":0.0");  
  display = XOpenDisplay(NULL);  
  if(!display)
    {
      return;
    }

  XSetErrorHandler(ErrorHandler);
    
  int width  = DisplayWidth  (display, DefaultScreen (display));
  int height = DisplayHeight (display, DefaultScreen (display));
  int depth  = DefaultDepth  (display, DefaultScreen (display));

  SetSize(width, height);
  SetMaxSize(width, height);

  /*
  switch(depth)
    {
      case   8 : SetMode(GRPPROPERTYMODE_08_INDEX);     break;
      case  16 : SetMode(GRPPROPERTYMODE_16_RGB_565);   break;
      case  24 : SetMode(GRPPROPERTYMODE_24_BGR_888);   break;
      case  32 : SetMode(GRPPROPERTYMODE_32_BGRA_8888); break;
    }
  */

  SetMode(GRPPROPERTYMODE_32_BGRA_8888);

  SetIsBufferInverse(true);
    
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPLINUXSCREENX11::~GRPLINUXSCREENX11()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPLINUXSCREENX11::~GRPLINUXSCREENX11()
{
  if(!display)
    {
      return;
    }
      
  //ScreenResolution(display, root, originalwidth, originalheight, 60, false);

  Delete();

  //XCloseDisplay(display);
 
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPLINUXSCREENX11::Create(bool show)
* @brief      Create resource
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  show : Show value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPLINUXSCREENX11::Create(bool show)
{
  if(!Create_Window(show)) 
    {
      return false;
    }

  XFlush(display);

  return GRPSCREEN::Create(show);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPLINUXSCREENX11::Update(GRP2DCANVAS* canvas)
* @brief      Update
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  canvas : canvas handle
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPLINUXSCREENX11::Update(GRP2DCANVAS* canvas)
{
  if(!display)
    { 
      return false;
    }

  if(!window)  
    {
      return false;
    }

  if(wmdeletewindow != None)
    {
      XEvent event;

      if(XCheckTypedWindowEvent(display, window, ClientMessage, &event))
        {
          if((Atom)event.xclient.data.l[0] == wmdeletewindow)
            {
              if(CanClose())
                {
                  if(mainproclinux.GetAppMain())
                    {
                      APPFLOWGRAPHICS* appgraphics = dynamic_cast<APPFLOWGRAPHICS*>(mainproclinux.GetAppMain()->GetApplication());
                      if(appgraphics)
                        {
                          if((GRPSCREEN*)this == appgraphics->GetMainScreen())
                            {
                              appgraphics->SetExitType(APPFLOWBASE_EXITTYPE_BY_USER);
                            }
                           else
                            {
                              Delete();
                            }
                        }
                    }                  
                }
            }
        }
    }


  if(!canvas)  
    {
      return false;
    }
  
  #ifdef GRP_OPENGL_ACTIVE
  if(blitgles) 
    {
      return blitgles->Update(canvas);
    }
  #endif


  GC gc = XCreateGC(display, window, 0, NULL);
  if(gc)
    {
      XSync(display, false);

      int neww = (int)width;
      int newh = (int)height;

      XWindowAttributes wattr;
      if(XGetWindowAttributes(display, window, &wattr))
        {
          neww = wattr.width;
          newh = wattr.height;
        }

      XBYTE* srcbuffer  = (XBYTE*)canvas->Buffer_Get();
      XBYTE* blitbuffer = srcbuffer;
      bool   scaled     = false;

      if(srcbuffer && ((neww != (int)width) || (newh != (int)height)) && (neww > 0) && (newh > 0))
        {
          XBYTE* scaledbuffer = ScaleBufferNearestLetterbox(srcbuffer, (int)width, (int)height, neww, newh);
          if(scaledbuffer)
            {
              blitbuffer = scaledbuffer;
              scaled     = true;
            }
        }

      int blitw = scaled ? neww   : (int)width;
      int blith = scaled ? newh   : (int)height;

      XImage* image = CreateXImageFromBuffer(display, DefaultScreen(display), blitbuffer, blitw, blith);
      if(image)
        {
          XPutImage(display, window, gc, image, 0, 0, 0, 0, blitw, blith);
          XDestroyImage(image);
        }

      if(scaled) free(blitbuffer);

      XFreeGC(display, gc);
    }

  XFlush(display);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPLINUXSCREENX11::UpdateTransparent(GRP2DCANVAS* canvas)
* @brief      Update transparent
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  canvas : Canvas object to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPLINUXSCREENX11::UpdateTransparent(GRP2DCANVAS* canvas)
{
  return Update(canvas);  
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPLINUXSCREENX11::Delete()
* @brief      Delete resource
* @ingroup    PLATFORM_LINUX
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPLINUXSCREENX11::Delete()
{
  #ifdef GRP_OPENGL_ACTIVE
  if(blitgles)
    {
      blitgles->Destroy();
      GEN_DELETE blitgles;
      blitgles = NULL;
    }
  #endif

  if(!display) return false;
  if(!window)  return false;

  XDestroyWindow(display, window);

  return GRPSCREEN::Delete();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPLINUXSCREENX11::Resize(int width, int height)
* @brief      Resize
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  width : Width value.
* @param[in]  height : Height value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPLINUXSCREENX11::Resize(int width, int height)
{
  XResizeWindow(display, window, width, height);

  return UpdateSize(width, height);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPLINUXSCREENX11::Show(bool active)
* @brief      Show
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  active : Active value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPLINUXSCREENX11::Show(bool active)
{
  isshow  = active;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPLINUXSCREENX11::ShowCursor(bool active)
* @brief      Show cursor
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  active : Active value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPLINUXSCREENX11::ShowCursor(bool active)
{
  if(!active)
    {
      // vars to make blank cursor
      Pixmap  blank;
      XColor  dummy;
      char    data[1] = { 0 };
      Cursor  cursor;

      // make a blank cursor
      blank = XCreateBitmapFromData(display, window, data, 1, 1);
      if(blank != None)
        {
          cursor = XCreatePixmapCursor(display, blank, blank, &dummy, &dummy, 0, 0);
          XFreePixmap(display, blank);
          XDefineCursor(display, window,cursor);

        } else return false;
    }
   else
    {
      XUndefineCursor(display, window);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void* GRPLINUXSCREENX11::GetHandle()
* @brief      Get handle
* @ingroup    PLATFORM_LINUX
* 
* @return     void* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void* GRPLINUXSCREENX11::GetHandle()
{
  return (void*)window;

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPLINUXSCREENX11::IsDesktop()
* @brief      Is desktop
* @ingroup    PLATFORM_LINUX
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool  GRPLINUXSCREENX11::IsDesktop()
{
  return isdesktop;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPLINUXSCREENX11::SetIsDesktop(bool isdesktop)
* @brief      Set is desktop
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  isdesktop : set is run in desktop
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPLINUXSCREENX11::SetIsDesktop(bool isdesktop)
{
  this->isdesktop = isdesktop;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPLINUXSCREENX11::HasFocus()
* @brief      Has focus
* @ingroup    PLATFORM_LINUX
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPLINUXSCREENX11::HasFocus()
{
  Window focus;
  int    status = 0;

  XGetInputFocus(display, &focus, &status);

  if(focus ==  window) return true;

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         Display* GRPLINUXSCREENX11::GetDisplay()
* @brief      Get display
* @ingroup    PLATFORM_LINUX
* 
* @return     Display* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
Display* GRPLINUXSCREENX11::GetDisplay()
{
  return display;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         Window* GRPLINUXSCREENX11::GetWindow()
* @brief      Get window
* @ingroup    PLATFORM_LINUX
* 
* @return     Window* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
Window* GRPLINUXSCREENX11::GetWindow()
{
  return &window;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         Window* GRPLINUXSCREENX11::GetWindowRoot()
* @brief      Get window root
* @ingroup    PLATFORM_LINUX
* 
* @return     Window* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
Window* GRPLINUXSCREENX11::GetWindowRoot()
{
  return &root;
}
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVisualInfo* GRPLINUXSCREENX11::GetVisualInfo()
* @brief      Get visual info
* @ingroup    PLATFORM_LINUX
* 
* @return     XVisualInfo* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVisualInfo* GRPLINUXSCREENX11::GetVisualInfo()
{
  return &vinfo;
}


#ifdef GRP_OPENGL_ACTIVE
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPLINUXBLITGLES* GRPLINUXSCREENX11::GetBlitGLES()
* @brief      Get OpenGL ES blitter (only present when GRP_OPENGL_ACTIVE is defined)
* @ingroup    PLATFORM_LINUX
* 
* @return     GRPLINUXBLITGLES* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPLINUXBLITGLES* GRPLINUXSCREENX11::GetBlitGLES()
{
  return blitgles;
}
#endif


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPLINUXSCREENX11::Create_Window(bool show)
* @brief      Create window
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  show : Show value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPLINUXSCREENX11::Create_Window(bool show)
{
  typedef struct  
  {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long          inputMode;
    unsigned long status;

  } MOTIFWMHINTS;

  if(!display) 
    {
      return false;
    }

  GRPRECTINT*           alldesktoprect  = NULL;
  int                   screenselected  = GetDesktopScreenSelected();   
  XSetWindowAttributes  attr;  
  int                   posx            = 0;
  int                   posy            = 0;
  int                   status          = false;   


  root = DefaultRootWindow(display);    

  if(screenselected == GRPSCREENTYPE_DESKTOP_ALL)
    {
      alldesktoprect = GetDesktopManager()->GetDesktopMonitors()->GetCombinedRect(); 
    }
   else 
    {
      alldesktoprect = GetDesktopManager()->GetDesktopMonitors()->GetMonitorsRects()->Get(screenselected);       
    }
  
  if(!alldesktoprect)
    {
      return false;          
    }            
   
   int winw = (int)GetPresentationWidth();
   int winh = (int)GetPresentationHeight();

   if(Styles_IsFullScreen())          
    {
      posx   = alldesktoprect->x1;
      posy   = alldesktoprect->y1; 

      if(!alldesktoprect->x1)
        {
          posx++;              
        }

      if(!alldesktoprect->y1)
        {
          posy++;              
        }

      if(Style_Is(GRPSCREENSTYLE_FULLSCREEN_ADJUSTRESOLUTION))
        {          
          ChangeScreenResolution(width, height);          
        }
       else 
        {
          width  = alldesktoprect->GetWidth();    
          height = alldesktoprect->GetHeight(); 

          if(Style_Is(GRPSCREENSTYLE_HEIGHTWITHOUTTASKBAR))
            {
              height -= GetTaskBarHeight();
            }
        }                      

      winw = (int)width;
      winh = (int)height;
    }
   else
    {         
      if(positionx == GRPPROPERTYMODE_SCREEN_CENTER) 
        {
          posx = (alldesktoprect->GetWidth() - winw)/2;
        }
       else 
        {
          posx = positionx; 
        }

      if(positiony == GRPPROPERTYMODE_SCREEN_CENTER) 
        {
          posy = (alldesktoprect->GetHeight() - winh)/2;
        }
       else 
        {
          posy = positiony; 
        }

      posx  += alldesktoprect->x1;
      posy  += alldesktoprect->y1;       
    } 
  
  
  #ifndef GRP_OPENGL_ACTIVE

  XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo);

  #else

  bool opengl_visual_ok = false;

  if(IsRunningOnWSL())
    {
      XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen X11] WSL detected; using X11 software path (no GPU / EGL->X11 SHM unsupported)"));
    }
   else
    {
      if(!blitgles)
        {
          blitgles = GEN_NEW GRPLINUXBLITGLES();
        }

      if(blitgles)
        {
          EGLint visid = 0;
          if(blitgles->ChooseVisualID((EGLNativeDisplayType)display, visid))
            {
              XVisualInfo vtempl;
              memset(&vtempl, 0, sizeof(vtempl));
              vtempl.visualid = (VisualID)visid;
              int nvis = 0;
              XVisualInfo* vis = XGetVisualInfo(display, VisualIDMask, &vtempl, &nvis);
              if(vis && nvis > 0)
                {
                  vinfo = vis[0];
                  XFree(vis);
                  opengl_visual_ok = true;
                }
            }

          if(!opengl_visual_ok)
            {
              XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen X11] EGL visual failed; falling back to X11 software path"));
              GEN_DELETE blitgles;
              blitgles = NULL;
            }
        }
    }

  if(!opengl_visual_ok)
    {
      XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo);
    }

  #endif

  attr.colormap         = XCreateColormap(display, root, vinfo.visual, AllocNone);
  attr.border_pixel     = 0;

  #ifdef GRP_OPENGL_ACTIVE
  if(blitgles)
    {
      attr.background_pixmap = None;
      window = XCreateWindow(display, root, posx, posy, winw, winh, 0, vinfo.depth, InputOutput, vinfo.visual, CWColormap | CWBorderPixel | CWBackPixmap, &attr);
    }
   else
  #endif
    {
      attr.background_pixel = Style_Is(GRPSCREENSTYLE_TRANSPARENT)?0x00000000:0xFFFFFFFF;
      window = XCreateWindow(display, root, posx, posy, winw, winh, 0, vinfo.depth, InputOutput, vinfo.visual, CWColormap | CWBorderPixel | CWBackPixel, &attr);        
    }
 
  if(!window) 
    {
      return false;
    }      

  if(Style_Is(GRPSCREENSTYLE_TRANSPARENT))
    {
      Atom comptonshadow = XInternAtom(display, "_COMPTON_SHADOW", False);  // create if absent so we can set it
      if(comptonshadow != None)
        {
          long shadowoff = 0;
          XChangeProperty(display, window, comptonshadow, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&shadowoff, 1);
          
          //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen X11] _COMPTON_SHADOW=0 set (no compositor shadow on transparent window)"));
        }
     
        XSetWindowAttributes swa;
        swa.override_redirect = True;
        XChangeWindowAttributes(display, window, CWOverrideRedirect, &swa);
        
        //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen X11] override_redirect set on transparent window (no WM/WSLg frame shadow)"));
      
    }

  /*
  if(Style_Is(GRPSCREENSTYLE_FULLSCREEN))
    {
      Atom wm_state            = XInternAtom (display, "_NET_WM_STATE", true );
      Atom wm_state_fullscreen = XInternAtom (display, "_NET_WM_STATE_FULLSCREEN", true);  

      if(wm_state == None || wm_state_fullscreen == None)        
        {
          XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[Screen X11] Error Atom FULLSCREEN  (fullscreen)"));
        }
       else
        {           
          XChangeProperty(display, window, wm_state, XA_ATOM, 32, PropModeReplace, (unsigned char *)&wm_state_fullscreen, 1);          
        }
    }
  */
 

  if(!Style_Is(GRPSCREENSTYLE_TITLE))
    {
      Atom          wmhintsatom = XInternAtom(display , "_MOTIF_WM_HINTS", True);
      MOTIFWMHINTS  hints; 

      XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen X11] Atom HINTS Decorations (no title) active"));

      if(wmhintsatom  == None)
        {
          XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[Screen X11] Error Atom HINTS Decorations (no title)"));
        }
       else
        {
          memset(&hints, 0, sizeof(hints));

          hints.flags        = GRPLINUXSCREENX11_MWM_HINTS_DECORATIONS;   
          hints.decorations  = 0;  

          XChangeProperty(display, window, wmhintsatom, wmhintsatom, 32, PropModeReplace, (unsigned char *)&hints, 5); 
        }
    }
   else
    {
      if(!title.IsEmpty())
        {
          XBUFFER xbuffer;

          title.ConvertToASCII(xbuffer);

          XStoreName(display, window, (char*)xbuffer.Get());
        }

      if(Style_Is(GRPSCREENSTYLE_NOWINDOWICONS))
        {
          Atom          wmhintsatom = XInternAtom(display , "_MOTIF_WM_HINTS", True);
          MOTIFWMHINTS  hints;

          XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen X11] Atom HINTS Decorations (no icons) active"));

          if(wmhintsatom  == None)
            {
              XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[Screen X11] Error Atom HINTS Decorations (no icons)"));
            }
           else
            { 
              memset(&hints, 0, sizeof(hints));

              hints.flags       = GRPLINUXSCREENX11_MWM_HINTS_DECORATIONS;
              hints.decorations = GRPLINUXSCREENX11_MWM_FUNC_CLOSE; 

              XChangeProperty(display, window, wmhintsatom, wmhintsatom, 32, PropModeReplace, (unsigned char*)&hints, 5);
            }
        }
    } 

  if(Style_Is(GRPSCREENSTYLE_NOICONTASKBAR))
    {
      Atom wm_state             = XInternAtom(display , "_NET_WM_STATE"             , True);
      Atom wm_state_skiptaskbar = XInternAtom(display , "_NET_WM_STATE_SKIP_TASKBAR", True);

      XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen X11] Atom SKIP_TASKBAR active"));
        
      if(wm_state == None || wm_state_skiptaskbar == None)
        {
          XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[Screen X11] Error Atom SKIP_TASKBAR (No Icon Taskbar)"));
        }
       else
        {
          XChangeProperty(display, window, wm_state, XA_ATOM, 32,PropModeReplace, (unsigned char *)&wm_state_skiptaskbar, 1);          
        }
    }

  if(Style_Is(GRPSCREENSTYLE_ONTOP))
    {
      Atom wm_state       = XInternAtom(display , "_NET_WM_STATE"       , True);
      Atom wm_state_above = XInternAtom(display , "_NET_WM_STATE_ABOVE" , True);

      XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen X11] Atom ABOVE active"));

      if(wm_state == None || wm_state_above == None)
        {
          XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[Screen X11] Error Atom ABOVE (On top)"));
        }
       else
        {
          XChangeProperty(display, window, wm_state, XA_ATOM, 32, PropModeReplace, (unsigned char *)&wm_state_above, 1);              
        }
    }


  if(Style_Is(GRPSCREENSTYLE_TRANSPARENT))
    {       
      /*
      Atom wm_opacity = XInternAtom(display, "_NET_WM_WINDOW_OPACITY", True);
      unsigned long opacity = (unsigned long)(0x01010101);

      XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen X11] Atom OPACITY active"));

      if(wm_opacity == None)
        {
          XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[Screen X11] Error Atom OPACITY (Transparent)"));
        }
       else
        {
          XChangeProperty(display, window, wm_opacity, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&opacity, 1L);
        }
      */

      /*
      Atom           wm_opacity = XInternAtom(display, "_NET_WM_WINDOW_OPACITY", True);      
      int            alpha = 0;
          
      XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen X11] Atom OPACITY active"));

      if(wm_opacity == None)
        {
          XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[Screen X11] Error Atom OPACITY (Transparent)"));
        }
       else
        {
          XChangeProperty (display, window, wm_opacity, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&alpha, 1);          
          XFlush(display);
        }
      */
    }
  
  
  wmdeletewindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
  if(wmdeletewindow != None)
    {
      XSetWMProtocols(display, window, &wmdeletewindow, 1);
    }

  XMapWindow(display , window);
  XMoveWindow(display, window, posx, posy);    

  #ifdef GRP_OPENGL_ACTIVE
 
  if(blitgles && !blitgles->Create(this))
    {
      XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen X11] BlitGLES create failed; falling back to X11 software path"));
      GEN_DELETE blitgles;
      blitgles = NULL;
    }
    
  if(blitgles)
    {
      blitgles->SetFlipY(true);
      XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen X11] BlitGLES SetFlipY(true) — correcting Mesa EGL-to-X11 vertical flip"));
    }
    
  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPLINUXSCREENX11::ChangeScreenResolution(int width, int height)
* @brief      Change screen resolution
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  width : Width value.
* @param[in]  height : Height value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPLINUXSCREENX11::ChangeScreenResolution(int width, int height)
{   
  int                     num_sizes;
  XRRScreenSize*          xrrs;
  XRRScreenConfiguration* conf;
  short                   original_rate;
  Rotation                original_rotation;
  SizeID                  original_size_id;
 
  root  = RootWindow(display, 0);
  xrrs  = XRRSizes(display, 0, &num_sizes);
 
  for(int c=0; c<num_sizes; c++) 
    {
      if((xrrs[c].width  == width) && (xrrs[c].height == height))
        {
          short*  rates;
          int     num_rates;

          rates = XRRRates(display, 0, c, &num_rates);
                      
          conf             = XRRGetScreenInfo(display, root);
          original_rate    = XRRConfigCurrentRate(conf);
          original_size_id = XRRConfigCurrentConfiguration(conf, &original_rotation);
              
          int status = XRRSetScreenConfigAndRate(display, conf, root, c, original_rotation, rates[0], CurrentTime);

          return true;        
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XImage* GRPLINUXSCREENX11::CreateXImageFromBuffer(Display* display, int screen, XBYTE* buffer, int width, int height)
* @brief      Create X image from buffer
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  display : Display pointer to use.
* @param[in]  screen : Screen object to use.
* @param[in]  buffer : Buffer to use.
* @param[in]  width : Width value.
* @param[in]  height : Height value.
* 
* @return     XImage* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XImage* GRPLINUXSCREENX11::CreateXImageFromBuffer(Display* display, int screen, XBYTE* buffer, int width, int height)
{
  XImage* image = NULL; 
  double  rratio;
  double  gratio;
  double  bratio;
  int     outindex       = 0;
  int     numbufferbytes = (4 * (width * height));

  rratio = vinfo.visual->red_mask   / 255.0;
  gratio = vinfo.visual->green_mask / 255.0;
  bratio = vinfo.visual->blue_mask  / 255.0;

  if(vinfo.depth >= 24)
    {
      int    numnewbufferbytes = (4 * (width * height));
      XDWORD* newbuffer        = (XDWORD*)malloc(numnewbufferbytes);

      for(int index=0;index<numbufferbytes;)
        {
          XDWORD b = (buffer[index++] * bratio);
          XDWORD g = (buffer[index++] * gratio);
          XDWORD r = (buffer[index++] * rratio);         
          XDWORD a;

          if(Style_Is(GRPSCREENSTYLE_TRANSPARENT))
            {
              a = buffer[index++] << 24;         
            }
           else
            {
              a = 0xFF000000;
              index++;
            } 

          r &= vinfo.visual->red_mask;
          g &= vinfo.visual->green_mask;
          b &= vinfo.visual->blue_mask;            

          newbuffer[outindex] = r | g | b | a;

          ++outindex;
        }

      image = XCreateImage (display, vinfo.visual, vinfo.depth,  ZPixmap, 0, (char *)newbuffer,width, height, 32, 0);
    }
    else
    {
      if(vinfo.depth >= 15)
        {
          int   numnewbufferbytes = (2 * (width * height));
          XWORD* newbuffer          = (XWORD*)malloc(numnewbufferbytes);

          for(int index=0;index<numbufferbytes;)
            {
              XDWORD b = (buffer[index++] * bratio);
              XDWORD g = (buffer[index++] * gratio);
              XDWORD r = (buffer[index++] * rratio);

              r &= vinfo.visual->red_mask;
              g &= vinfo.visual->green_mask;
              b &= vinfo.visual->blue_mask;

              newbuffer[outindex] = r | g | b;

              ++outindex;
            }

          image = XCreateImage(display, vinfo.visual, vinfo.depth, ZPixmap, 0, (char*)newbuffer, width, height, 16, 0);

        }  
       else 
        {
          return NULL;
        }
    }

  XInitImage(image);

  if(LSBFirst == GetByteOrder())
         image->byte_order = LSBFirst;
    else image->byte_order = MSBFirst;

  image->bitmap_bit_order = MSBFirst;

  return image;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE* GRPLINUXSCREENX11::ScaleBufferNearestLetterbox(XBYTE* src, int srcw, int srch, int dstw, int dsth)
* @brief      Nearest-neighbor scale of a 4-bytes-per-pixel buffer to a new size, letterboxed
*             (black bars) to preserve the source aspect ratio. Used by the X11 software blit
*             path (no GPU) so a resize/maximize scales the image the same way the GL path does,
*             instead of pasting it 1:1 in a corner.
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  src : Source buffer, srcw x srch, 4 bytes per pixel. Channel order does not matter,
*                   pixels are copied whole.
* @param[in]  srcw : Source width.
* @param[in]  srch : Source height.
* @param[in]  dstw : Destination width.
* @param[in]  dsth : Destination height.
* 
* @return     XBYTE* : Newly malloc'd dstw x dsth x 4 buffer (caller must free()); NULL on error.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE* GRPLINUXSCREENX11::ScaleBufferNearestLetterbox(XBYTE* src, int srcw, int srch, int dstw, int dsth)
{
  if(!src || (srcw <= 0) || (srch <= 0) || (dstw <= 0) || (dsth <= 0))
    {
      return NULL;
    }

  size_t dstbytes = (size_t)dstw * (size_t)dsth * 4;

  XBYTE* dst = (XBYTE*)malloc(dstbytes);
  if(!dst)
    {
      return NULL;
    }

  memset(dst, 0, dstbytes);   // black letterbox bars

  float scalex = (float)dstw / (float)srcw;
  float scaley = (float)dsth / (float)srch;
  float scale  = (scalex < scaley) ? scalex : scaley;

  int draww = (int)((float)srcw * scale);
  int drawh = (int)((float)srch * scale);
  if(draww < 1) draww = 1;
  if(drawh < 1) drawh = 1;

  int offsetx = (dstw - draww) / 2;
  int offsety = (dsth - drawh) / 2;

  for(int y=0; y<drawh; y++)
    {
      int srcy = (int)((float)y / scale);
      if(srcy >= srch) srcy = srch - 1;

      XBYTE* dstrow    = dst + ((size_t)(y + offsety) * (size_t)dstw + (size_t)offsetx) * 4;
      XBYTE* srcrowbase = src + (size_t)srcy * (size_t)srcw * 4;

      for(int x=0; x<draww; x++)
        {
          int srcx = (int)((float)x / scale);
          if(srcx >= srcw) srcx = srcw - 1;

          memcpy(dstrow + (size_t)x * 4, srcrowbase + (size_t)srcx * 4, 4);
        }
    }

  return dst;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int GRPLINUXSCREENX11::GetTaskBarHeight()
* @brief      Get task bar height
* @ingroup    PLATFORM_LINUX
* 
* @return     int : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int GRPLINUXSCREENX11::GetTaskBarHeight()
{
  int screen              = DefaultScreen(display);    
  int screen_width        = DisplayWidth(display, screen);
  int screen_height       = DisplayHeight(display, screen);

    
  Atom atomworkarea = XInternAtom(display, "_NET_WORKAREA", True);
  if(atomworkarea == None) 
    {
      //XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[Screen X11] Error Atom WORKAREA (Get Taskbar height)"));      
      return 0;
    }

  Atom           actual_type;
  int            actual_format;
  unsigned long  nitems;
  unsigned long  bytes_after;
  unsigned char* data     = nullptr;

  int status = XGetWindowProperty(display, root, atomworkarea, 0, 4 * sizeof(long), False, XA_CARDINAL, &actual_type, &actual_format, &nitems, &bytes_after, &data);
  if(status != Success || !data) 
    {
      //XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[Screen X11] Error XGetWindowProperty (Get Taskbar height)"));     
      return 0;
    }
    
  long*   workarea        = reinterpret_cast<long*>(data);
  int     work_width      = workarea[2];
  int     work_height     = workarea[3];
  int     taskbar_height  = screen_height - work_height;

  XFree(data);  

  return taskbar_height;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPLINUXSCREENX11::ShowDebugNetSupportedPropertys()
* @brief      Show debug net supported propertys
* @ingroup    PLATFORM_LINUX
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPLINUXSCREENX11::ShowDebugNetSupportedPropertys()
{
  Atom netsupported = XInternAtom(display, "_NET_SUPPORTED", True);
  if(netsupported == None) 
    {
      XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[Screen X11] Error NET_SUPPORTED"));          
      return false;
    }

  Atom            type;
  int             format;
  unsigned long   nitems;
  unsigned long   bytesafter;
  Atom*           atoms = nullptr;

  int status = XGetWindowProperty(display, root, netsupported, 0, 1024, False, XA_ATOM, &type, &format, &nitems, &bytesafter, reinterpret_cast<unsigned char **>(&atoms));
  if(status != Success || type != XA_ATOM || format != 32) 
    {
      XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[Screen X11] Failed to retrieve _NET_SUPPORTED property"));   

      if(atoms) 
        {
          XFree(atoms);
        }
        
      return 1;
    }


  //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen X11] _NET_SUPPORTED Atoms [%d]: "), nitems);

  for(unsigned long c=0; c<nitems; ++c) 
    {
      char* atomname = XGetAtomName(display, atoms[c]);
      if(atomname) 
        {
          XSTRING _atomname;

          _atomname = atomname;

          XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("%s"), _atomname.Get());
          
          XFree(atomname);
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int GRPLINUXSCREENX11::GetByteOrder()
* @brief      Get byte order
* @ingroup    PLATFORM_LINUX
* 
* @return     int : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int GRPLINUXSCREENX11::GetByteOrder()
{
  union
  {
    char c[sizeof(short)];
    short s;

  } order;

  order.s = 1;
  if((1 == order.c[0]))
    {
      return LSBFirst;
    }
   else
    {
      return MSBFirst;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int GRPLINUXSCREENX11::ErrorHandler(Display* display, XErrorEvent* errorevent)
* @brief      Error handler
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  display : Display pointer to use.
* @param[in]  errorevent : Errorevent pointer to use.
* 
* @return     int : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int GRPLINUXSCREENX11::ErrorHandler(Display* display, XErrorEvent* errorevent)
{
  int a=0;
  a++;

  return 0; 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPLINUXSCREENX11::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPLINUXSCREENX11::Clean()
{
  display        = NULL;
  window         = 0;

  wmdeletewindow = None;

  isdesktop      = false;

  #ifdef GRP_OPENGL_ACTIVE
  blitgles  = NULL;
  #endif
}



#endif

