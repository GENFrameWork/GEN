/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPWINDOWSScreen.cpp
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/


#include "GRPWINDOWSScreen.h"

#include "GRPFactory.h"
#include "GRPDesktopManager.h"

#include "XTrace.h"
#include "XSystem.h"

#include "MainProcWINDOWS.h"
#include "APPFlowGraphics.h"

#include "GRPXEvent.h"
#include "GRP2DCanvas.h"
#include "GRPBitmap.h"


#ifdef GRP_OPENGL_ACTIVE
#include "GRPWINDOWSBlitGLES.h"
#endif


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPWINDOWSSCREEN::GRPWINDOWSSCREEN(): GRPSCREEN()
* @brief      Constructor of class
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPWINDOWSSCREEN::GRPWINDOWSSCREEN(): GRPSCREEN()
{
  Clean();

  type = GRPSCREENTYPE_WINDOWS;

  DEVMODE devmode;
  DWORD   currentsettings = ENUM_CURRENT_SETTINGS;
  BOOL    result          = TRUE;

  memset(&devmode, 0, sizeof(DEVMODE));
  devmode.dmSize  = sizeof(DEVMODE);

  result = EnumDisplaySettings(NULL, currentsettings, &devmode);
  if(!result) return;

  SetSize(devmode.dmPelsWidth, devmode.dmPelsHeight);
  SetMaxSize(devmode.dmPelsWidth, devmode.dmPelsHeight);


  GRPRECTINT* rectworkarea = GetWorkRegion();
  if(rectworkarea)
    {
      RECT workarea;

      SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);

      rectworkarea->x1 = workarea.left;
      rectworkarea->y1 = workarea.top;

      rectworkarea->x2 = workarea.right;
      rectworkarea->y2 = workarea.bottom;
    }

  SetMode(GRPPROPERTYMODE_32_BGRA_8888);

  SetIsBufferInverse(false);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPWINDOWSSCREEN::~GRPWINDOWSSCREEN()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPWINDOWSSCREEN::~GRPWINDOWSSCREEN()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPWINDOWSSCREEN::Create(bool show)
* @brief      Create resource
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  show : Show value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPWINDOWSSCREEN::Create(bool show)
{
  if(!Create_Window(show)) 
    {
      return false;
    }

  GRPSCREEN::GetListScreens()->Add((void*)hwnd, this);

  return GRPSCREEN::Create(show);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPWINDOWSSCREEN::Update()
* @brief      Update
* @ingroup    PLATFORM_WINDOWS
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPWINDOWSSCREEN::Update()
{
  #ifndef GRP_OPENGL_ACTIVE

  if(!hdc) 
    {
      return false;
    }

  SwapBuffers(hdc);

  #else

  if(!blitgles) return false;
  return blitgles->SwapBuffers();

  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPWINDOWSSCREEN::Update(GRP2DCANVAS* canvas)
* @brief      Update
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  canvas : handle of canvas
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPWINDOWSSCREEN::Update(GRP2DCANVAS* canvas)
{
  #ifndef GRP_OPENGL_ACTIVE

  if(!hdc)    
    {
      return false;
    }

  if(!canvas) 
    {
      return false;
    }

  
  SetDIBitsToDevice(hdc, 0, 0, width  ,
                               height ,
                               0,0,0  ,
                               height ,
                               canvas->Buffer_Get() ,
                               &hinfo ,
                               DIB_RGB_COLORS);     
  
  return true;

  #else

  if(!canvas)   return false;
  if(!blitgles) return false;
  return blitgles->Update(canvas);

  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPWINDOWSSCREEN::UpdateTransparent(GRP2DCANVAS* canvas)
* @brief      Update transparent
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  canvas : Canvas object to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPWINDOWSSCREEN::UpdateTransparent(GRP2DCANVAS* canvas)
{
  if(!hdc) 
    {
      return false;
    }

  if(!canvas) 
    {
      return false;
    }

  HDC     hdcscreen = GetDC(NULL);
  HDC     hdcmem    = CreateCompatibleDC(hdcscreen);
  HBITMAP hbmmem    = CreateCompatibleBitmap(hdcscreen, (LONG)width, (LONG)height);
  HBITMAP hbmold    = (HBITMAP)SelectObject(hdcmem, hbmmem);

  RECT    rect      = { 0, 0, (LONG)width, (LONG)height };
  HBRUSH  hbrush    = CreateSolidBrush(RGB(0, 0, 0));

  FillRect(hdcmem, &rect, hbrush);
  DeleteObject(hbrush);

  SetBkMode(hdcmem, TRANSPARENT);
 
  // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen Windows] Update Transparent (Equal)  (%04d,%04d)  Bitxpixel (%d)"), width, height, GetBitsperPixel());
     
  SetDIBitsToDevice(hdcmem, 0, 0, width  ,
                                  height ,
                                  0,0,0  ,
                                  height ,
                                  canvas->Buffer_Get() ,
                                  &hinfo ,
                                  DIB_RGB_COLORS);     
 
  POINT         ptsrc   = { 0, 0 };
  SIZE          sizewnd = { (LONG)width, (LONG)height };
  BLENDFUNCTION blend   = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
  POINT         ptdst   = { (LONG)positionx, (LONG)positiony };

  UpdateLayeredWindow(hwnd, hdcscreen, &ptdst, &sizewnd, hdcmem, &ptsrc, 0, &blend, ULW_ALPHA);
 
  SelectObject(hdcmem, hbmold);
  DeleteObject(hbmmem);
  DeleteDC(hdcmem);
  ReleaseDC(NULL, hdcscreen);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPWINDOWSSCREEN::Delete()
* @brief      Delete resource
* @ingroup    PLATFORM_WINDOWS
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPWINDOWSSCREEN::Delete()
{
  #ifdef GRP_OPENGL_ACTIVE
  if(blitgles)
    {
      blitgles->Destroy();
      GEN_DELETE blitgles;
      blitgles = NULL;
    }
  #endif

  if(Style_Is(GRPSCREENSTYLE_FULLSCREEN)) 
    {
      ChangeDisplaySettings(NULL,0);
    }  

  if(hdc)
    {
      ReleaseDC(hwnd, hdc);
      hdc  = NULL;
    }

  if(hwnd) 
    {       
      SendMessage(hwnd, WM_CLOSE, 0, 0);
      //CloseWindow(hwnd);

      DestroyWindow(hwnd);

      hwnd = NULL;
    }

  UnregisterClassA((LPCSTR)GRPWINDOWSSCREEN_NAMECLASS, hinstance);

  return GRPSCREEN::Delete();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPWINDOWSSCREEN::Get_Position(int& x, int& y)
* @brief      Get position
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  x : X coordinate.
* @param[in]  y : Y coordinate.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPWINDOWSSCREEN::Get_Position(int& x, int& y)
{
  RECT    rect;

  GetWindowRect(hwnd, &rect);

  x = rect.left;  
  y = rect.top;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPWINDOWSSCREEN::Set_Position(int x, int y)
* @brief      Set position
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  x : X coordinate.
* @param[in]  y : Y coordinate.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPWINDOWSSCREEN::Set_Position(int x, int y)
{
  RECT    rect;
  XDWORD  style;

  GetClientRect(hwnd, &rect);

  int width   = rect.right  - rect.left; 
  int height  = rect.bottom - rect.top;  

  rect.right  = x;  
  rect.left   = x + width;

  rect.top    = y;  
  rect.bottom = y + height;

  style = GetWindowLong(hwnd, GWL_STYLE);

  SetPosition(x, y);

  AdjustWindowRect(&rect, style, false);

  SetWindowPos(hwnd,NULL, positionx, positiony, (rect.right-rect.left), (rect.bottom-rect.top)  , SWP_NOSIZE | SWP_NOZORDER);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPWINDOWSSCREEN::Resize(int width, int height)
* @brief      Resize
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  width : GEN_NEW width of the screen
* @param[in]  height : GEN_NEW height of the screen
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPWINDOWSSCREEN::Resize(int width, int height)
{
  RECT    rect;
  XDWORD  style;

  GetClientRect(hwnd, &rect);

  rect.right  = rect.left + width;    //+ 16;
  rect.bottom = rect.top  + height;   //+ 16;

  style = GetWindowLong(hwnd, GWL_STYLE);

  AdjustWindowRect(&rect,style,false);

  SetWindowPos(hwnd,NULL, positionx, positiony, (rect.right-rect.left), (rect.bottom-rect.top) , SWP_NOMOVE | SWP_NOZORDER);

  return UpdateSize(width, height);
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPWINDOWSSCREEN::Show(bool active)
* @brief      Show
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  active : Active value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPWINDOWSSCREEN::Show(bool active)
{
  if(!hwnd) return false;

  ::ShowWindow(hwnd, (active?SW_SHOW:SW_HIDE));

  isshow  = active;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPWINDOWSSCREEN::ShowCursor(bool active)
* @brief      Show cursor
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  active : Active value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPWINDOWSSCREEN::ShowCursor(bool active)
{
  if(!hwnd) return false;

  return ::ShowCursor(active?TRUE:FALSE)?true:false;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void* GRPWINDOWSSCREEN::GetHandle()
* @brief      Get handle
* @ingroup    PLATFORM_WINDOWS
* 
* @return     void* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void* GRPWINDOWSSCREEN::GetHandle()
{
  return (void*)hwnd;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPWINDOWSSCREEN::SetHandle(void* handle_window)
* @brief      Set handle
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  handle_window : Handle window pointer to use.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPWINDOWSSCREEN::SetHandle(void* handle_window)
{
  this->hwnd = (HWND)handle_window;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPWINDOWSSCREEN::Set_Focus()
* @brief      Set focus
* @ingroup    PLATFORM_WINDOWS
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPWINDOWSSCREEN::Set_Focus()
{
  if(!hwnd)
    {
      return false;
    }

  if(!SetForegroundWindow(hwnd))
    {
      return false;
    }

  AttachThreadInput(GetCurrentThreadId(), GetWindowThreadProcessId(GetAncestor(hwnd, GA_ROOT), NULL), TRUE);

  SetFocus(hwnd);
    
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPWINDOWSSCREEN::Minimize(bool active)
* @brief      Minimize
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  active : Active value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPWINDOWSSCREEN::Minimize(bool active)
{
  if(!hwnd)
    {
      return false;
    }

  return ShowWindow(hwnd, active?SW_SHOWMINIMIZED:SW_NORMAL)?true:false; 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPWINDOWSSCREEN::Maximize(bool active)
* @brief      Maximize
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  active : Active value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPWINDOWSSCREEN::Maximize(bool active)
{
  if(!hwnd)
    {
      return false;
    }

  return ShowWindow(hwnd, active?SW_SHOWMAXIMIZED:SW_NORMAL)?true:false; 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPBITMAP* GRPWINDOWSSCREEN::CaptureContent(GRPRECTINT* rect, void* handle_window)
* @brief      Capture content
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  rect : Rectangle to use.
* @param[in]  handle_window : Handle window pointer to use.
* 
* @return     GRPBITMAP* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPBITMAP* GRPWINDOWSSCREEN::CaptureContent(GRPRECTINT* rect, void* handle_window)
{
  XBUFFER xbuffer;
  HDC     hdcsource = GetDC(handle_window?(HWND)handle_window:hwnd);
  HDC     hdcmemory = CreateCompatibleDC(hdcsource);
  int     x         = 0;
  int     y         = 0;
  int     cx        = GetWidth();
  int     cy        = GetHeight();  
  bool    status    = false;

  if(!hdcsource)
    {
      return NULL;  
    }

  if(!hdcmemory)
    {
      DeleteDC(hdcsource);
      return NULL;        
    }

  if(rect)
    {
      x         = rect->x1;
      y         = rect->y1;
      cx        = (rect->x2 - rect->x1);
      cy        = (rect->y2 - rect->y1);
    }  

  HBITMAP hbitmap     = CreateCompatibleBitmap(hdcsource, cx, cy);
  HBITMAP hbitmapold  = (HBITMAP)SelectObject(hdcmemory, hbitmap);
  BITMAP  bitmap;

  BitBlt(hdcmemory, 0, 0, cx, cy, hdcsource, x, y, SRCCOPY);
  hbitmap = (HBITMAP)SelectObject(hdcmemory, hbitmapold);

  DeleteDC(hdcsource);
  DeleteDC(hdcmemory);

  GetObject(hbitmap, sizeof(bitmap), (LPSTR)&bitmap);

  HDC dcbitmap = CreateCompatibleDC(NULL);
  SelectObject(dcbitmap, hbitmap);

  BITMAPINFO bmpinfo;

  bmpinfo.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
  bmpinfo.bmiHeader.biWidth        = bitmap.bmWidth;
  bmpinfo.bmiHeader.biHeight       = bitmap.bmHeight;
  bmpinfo.bmiHeader.biPlanes       = bitmap.bmPlanes;
  bmpinfo.bmiHeader.biBitCount     = bitmap.bmBitsPixel;
  bmpinfo.bmiHeader.biCompression  = BI_RGB;
  bmpinfo.bmiHeader.biSizeImage    = 0;
  bmpinfo.bmiHeader.biClrImportant = 0;
  bmpinfo.bmiHeader.biClrUsed      = 0;

  GRPBITMAP* grpbitmap = GRPFACTORY::GetInstance().CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, GRPPROPERTYMODE_32_BGRA_8888);
  if(grpbitmap)
    {
      status = GetDIBits(dcbitmap, hbitmap, 0, bitmap.bmHeight, grpbitmap->GetBuffer(), &bmpinfo, DIB_RGB_COLORS)?true:false;

      XDWORD* buffer = (XDWORD*)grpbitmap->GetBuffer();
      for(XDWORD c=0; c<(grpbitmap->GetBufferSize()/sizeof(XDWORD)); c++)
        {
          (*buffer) |= 0xFF000000;
          buffer++;
        }
    }

  if(!status)
    {
      GEN_DELETE grpbitmap;
      grpbitmap = NULL;
    }

  return grpbitmap;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void* GRPWINDOWSSCREEN::GetDesktopHandle()
* @brief      Get desktop handle
* @ingroup    PLATFORM_WINDOWS
* 
* @return     void* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void* GRPWINDOWSSCREEN::GetDesktopHandle()
{
  HANDLE hwnd = GetDesktopWindow();

  return (void*)hwnd;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void* GRPWINDOWSSCREEN::GetShellHandle()
* @brief      Get shell handle
* @ingroup    PLATFORM_WINDOWS
* 
* @return     void* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void* GRPWINDOWSSCREEN::GetShellHandle()
{
  HANDLE hwnd = GetShellWindow();

  return (void*)hwnd;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void* GRPWINDOWSSCREEN::GetHDC()
* @brief      Get HDC
* @ingroup    PLATFORM_WINDOWS
* 
* @return     void* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void* GRPWINDOWSSCREEN::GetHDC()
{
  return (void*)hdc;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPWINDOWSSCREEN::SetHDC(HDC hdc)
* @brief      Set HDC
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  hdc : Hdc value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPWINDOWSSCREEN::SetHDC(HDC hdc)
{
  this->hdc = hdc;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         BITMAPINFO* GRPWINDOWSSCREEN::GetHInfo()
* @brief      Get H info
* @ingroup    PLATFORM_WINDOWS
* 
* @return     BITMAPINFO* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
BITMAPINFO* GRPWINDOWSSCREEN::GetHInfo()
{
  return &hinfo;
}


#ifdef GRP_OPENGL_ACTIVE
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPWINDOWSBLITGLES* GRPWINDOWSSCREEN::GetBlitGLES()
* @brief      Get OpenGL ES blitter (only present when GRP_OPENGL_ACTIVE is defined)
* @ingroup    PLATFORM_WINDOWS
* 
* @return     GRPWINDOWSBLITGLES* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPWINDOWSBLITGLES* GRPWINDOWSSCREEN::GetBlitGLES()
{
  return blitgles;
}
#endif


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int GRPWINDOWSSCREEN::GetTaskbarHeight()
* @brief      Get taskbar height
* @ingroup    PLATFORM_WINDOWS
* 
* @return     int : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int GRPWINDOWSSCREEN::GetTaskbarHeight()
{
  int screenWidth = GetSystemMetrics(SM_CXSCREEN);
  int screenHeight = GetSystemMetrics(SM_CYSCREEN);

  RECT workArea;
  SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

  int taskbarHeight = screenHeight - (workArea.bottom - workArea.top);

  return taskbarHeight;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPWINDOWSSCREEN::Chromes_ApplyStyle(DWORD& style, DWORD& exstyle)
* @brief      Chromes apply style
* @note       INTERNAL: resolves the current GRPSCREENCFGCHROMES configuration (native Windows chromes only, for now)
*             into the WS_* / WS_EX_* bits used when creating the window. Only called when a Chromes
*             configuration is active, native chromes are requested, and the screen is not transparent
*             (Fullscreen and Transparent styles are prioritary over the Chromes configuration).
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  style : Window style (WS_*) bits to complete.
* @param[in]  exstyle : Window extended style (WS_EX_*) bits to complete.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPWINDOWSSCREEN::Chromes_ApplyStyle(DWORD& style, DWORD& exstyle)
{
  GRPSCREENCFGCHROMES* cfgchromes = GetCFGChromes();
  if(!cfgchromes) return;

  if(cfgchromes->GetNativeCaptionActive())
    {
      style |= WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED;

      if(cfgchromes->GetNativeMinimizeActive()) style |= WS_MINIMIZEBOX;
      if(cfgchromes->GetNativeMaximizeActive()) style |= WS_MAXIMIZEBOX;
    }
   else
    {
      style |= WS_POPUP;
    }

  if(cfgchromes->GetResizeActive())
    {
      style |= WS_THICKFRAME;
    }

  (void)exstyle; // Reserved for future (custom GEN-drawn chromes) use.
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPWINDOWSSCREEN::Chromes_ApplyPostCreate()
* @brief      Chromes apply post create
* @note       INTERNAL: applies the parts of the GRPSCREENCFGCHROMES configuration that require the window
*             handle to already exist (application icon and close button), for native Windows chromes.
*             The close configuration also reuses the existing CanClose() mechanism, so WM_CLOSE keeps
*             being blocked consistently regardless of how the window is asked to close.
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPWINDOWSSCREEN::Chromes_ApplyPostCreate()
{
  if(!hwnd) return;

  GRPSCREENCFGCHROMES* cfgchromes = GetCFGChromes();
  if(!cfgchromes) return;

  if(cfgchromes->GetNativeCaptionActive() && !cfgchromes->GetNativeIconActive())
    {
      SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)NULL);
      SendMessage(hwnd, WM_SETICON, ICON_BIG  , (LPARAM)NULL);

      // NOTE: WM_SETICON alone only affects the taskbar/Alt+Tab icon; the caption itself still reserves
      // and paints an icon slot. WS_EX_DLGMODALFRAME is the standard Win32 technique to make the caption
      // stop reserving that icon slot; SWP_FRAMECHANGED forces the non-client area to be recalculated and
      // repainted so the change is actually visible (it has a minor side effect: the window border is
      // drawn with the thin dialog-style double line instead of the usual raised border).
      LONG_PTR exstylebits = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
      SetWindowLongPtr(hwnd, GWL_EXSTYLE, exstylebits | WS_EX_DLGMODALFRAME);

      SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }

  SetCanClose(cfgchromes->GetNativeCloseActive());

  if(!cfgchromes->GetNativeCloseActive())
    {
      HMENU hsysmenu = GetSystemMenu(hwnd, FALSE);
      if(hsysmenu)
        {
          EnableMenuItem(hsysmenu, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
        }
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPWINDOWSSCREEN::Create_Window(bool show)
* @brief      Create window
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  show : Show value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPWINDOWSSCREEN::Create_Window(bool show)
{
  //-----------------------------------------------------------------------------------------------------

  if(hwnd) 
    {
      return false;
    }

  hinstance = (HINSTANCE)mainprocwindows.GetHandle();

  //-----------------------------------------------------------------------------------------------------

  classname               = GRPWINDOWSSCREEN_NAMECLASS;
  title                   = __L(" ");

  wndclass.cbSize         = sizeof(wndclass);
  wndclass.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wndclass.lpfnWndProc    = BaseWndProc;
  wndclass.cbClsExtra     = 0;
  wndclass.cbWndExtra     = 0;
  wndclass.hInstance      = NULL;
  wndclass.hIcon          = NULL;
  wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wndclass.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wndclass.lpszMenuName   = NULL;
  wndclass.hIconSm        = NULL;

  wndclass.lpszClassName  = classname.Get();
  wndclass.hInstance      = hinstance;

  RegisterClassEx(&wndclass);

  //-----------------------------------------------------------------------------------------------------

  GRPRECTINT* alldesktoprect  = NULL;
  int         nscreen         = GetDesktopScreenSelected(); 
  DWORD       _exstyle        = 0;
  DWORD       _style          = 0;
  int         posx            = 0;
  int         posy            = 0;  

  if(nscreen == GRPSCREENTYPE_DESKTOP_ALL)
    {
      alldesktoprect = GetDesktopManager()->GetDesktopMonitors()->GetCombinedRect(); 
    }
   else 
    {
      alldesktoprect = GetDesktopManager()->GetDesktopMonitors()->GetMonitorsRects()->Get(nscreen);
    }
  
  if(!alldesktoprect)
    {
      return false;          
    }            

  if(Styles_IsFullScreen())          
    {
      if(Style_Is(GRPSCREENSTYLE_FULLSCREEN_ADJUSTRESOLUTION))
        {
          DEVMODE devmode;

          memset(&devmode, 0, sizeof(DEVMODE));

          devmode.dmSize       = sizeof(DEVMODE);
          devmode.dmPelsWidth  = width;
          devmode.dmPelsHeight = height;
          devmode.dmBitsPerPel = GetBitsperPixel();
          devmode.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

          LONG status = ChangeDisplaySettings(&devmode, CDS_FULLSCREEN);

          if(status != DISP_CHANGE_SUCCESSFUL) 
            {
              return NULL;
            }
        }
       else 
        {                         
          _exstyle  = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
          _style    = WS_POPUP;

          width  = alldesktoprect->GetWidth();    
          height = alldesktoprect->GetHeight(); 
   
          posx   = alldesktoprect->x1;
          posy   = alldesktoprect->y1;   
        }

      if(Style_Is(GRPSCREENSTYLE_HEIGHTWITHOUTTASKBAR))
        {
          height -= GetTaskbarHeight();
        }  

      if(Style_Is(GRPSCREENSTYLE_TRANSPARENT))
        {
          _exstyle |= WS_EX_LAYERED;
        }

      if(Style_Is(GRPSCREENSTYLE_NOICONTASKBAR))
        {
          _exstyle |= WS_EX_TOOLWINDOW;
        }

      hwnd = CreateWindowEx(_exstyle          ,
                            classname.Get()   ,
                            NULL              ,
                            _style            ,
                            posx, posy        ,
                            width             ,
                            height            ,
                            NULL              ,
                            NULL              ,
                            hinstance         ,
                            (void*)this);

      if(!hwnd) 
        {
          return false;
        }

      SetPosition(posx, posy);  
      SetSize(width ,height);
      SetMaxSize(width ,height);              
    }
   else
    {          
      int winw = (int)GetPresentationWidth();
      int winh = (int)GetPresentationHeight();

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
   
      // NOTE: Fullscreen and Transparent styles are prioritary over any Chromes configuration (see below).
      bool usecfgchromesnative = (IsCFGChromesActive()                       &&
                                   GetCFGChromes()->GetUseNativeChromes()    &&
                                   !Style_Is(GRPSCREENSTYLE_TRANSPARENT));

      // Custom (non native) Chromes: GEN's own UI layout draws the whole caption (icon, title, min/max/close),
      // so the native window must show NO decoration of its own at all -- no caption, no system menu, no
      // native icon/buttons. WS_THICKFRAME (native resize, if requested) is the only native bit kept; the
      // leftover sizing-frame bar that combination would otherwise show is already stripped generically by the
      // WM_NCCALCSIZE/WM_NCHITTEST handling in BaseWndProc (it keys off the style bits themselves, not off
      // which Chromes mode set them), so nothing else is needed here for that.
      bool usecfgchromescustom = (IsCFGChromesActive()                       &&
                                   !GetCFGChromes()->GetUseNativeChromes()   &&
                                   !Style_Is(GRPSCREENSTYLE_TRANSPARENT));

      if(usecfgchromesnative)
        {
          Chromes_ApplyStyle(_style, _exstyle);
        }
       else if(usecfgchromescustom)
        {
          _style |= WS_POPUP;

          if(GetCFGChromes()->GetResizeActive()) _style |= WS_THICKFRAME;
        }
       else
        {
          if(Style_Is(GRPSCREENSTYLE_NOWINDOWICONS))
            {
              _style |= WS_OVERLAPPED;  // _exstyle |= WS_EX_TOOLWINDOW;
            }
           else _style |= WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
        }


      if(Style_Is(GRPSCREENSTYLE_ONTOP))
        {
          _exstyle |= WS_EX_TOPMOST;
        }

      if(Style_Is(GRPSCREENSTYLE_TRANSPARENT))
        {
          _exstyle |= WS_EX_LAYERED;             
          _style   |= WS_POPUP;
        }
       else
        {
          if(!usecfgchromesnative && !usecfgchromescustom)
            {
              if(Style_Is(GRPSCREENSTYLE_TITLE))
                {
                  _style |= WS_OVERLAPPED;
                }
               else
                {
                  _style |= WS_POPUP;
                }
            }
        }

      if(Style_Is(GRPSCREENSTYLE_NOICONTASKBAR))
        {
          _exstyle |= WS_EX_TOOLWINDOW;
        }

      hwnd = CreateWindowEx(_exstyle          ,
                            classname.Get()   ,
                            (usecfgchromesnative && !GetCFGChromes()->GetNativeTitleActive()) ? __L("") : GetTitle()->Get() ,
                            _style            ,
                            posx              , 
                            posy              ,
                            winw ,winh        ,
                            NULL              ,
                            NULL              ,
                            hinstance         ,
                            (void*)this);

      if(!hwnd) 
        {
          return false;
        }

      if(usecfgchromesnative)
        {
          Chromes_ApplyPostCreate();
        }

      // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen Windows] Part 1 Ini: x=%04d, y=%04d (%04d,%04d)  Bitxpixel (%d)"), posx, posy, width, height, GetBitsperPixel());
       
      RECT rect;

      GetClientRect(hwnd, &rect);

      rect.right  = rect.left + winw;    //+ 16;
      rect.bottom = rect.top  + winh;   //+ 16;

      AdjustWindowRect(&rect, GetWindowLong(hwnd, GWL_STYLE), false);
    
      SetWindowPos(hwnd, NULL, posx , posy, (rect.right-rect.left), (rect.bottom-rect.top) , SWP_NOMOVE | SWP_NOZORDER);
      
      POINT point = { 0, 0}; 

      ClientToScreen(hwnd, (LPPOINT)&point);  

      if(Style_Is(GRPSCREENSTYLE_HEIGHTWITHOUTTASKBAR))
        {
          point.y -= GetTaskbarHeight();
        }  

      point.x  += alldesktoprect->x1;
      point.y  += alldesktoprect->y1;   
    
      SetPosition(point.x,  point.y);
      Set_Position(point.x,  point.y);

      // NOTE: the screen size members stay at the CONTENT size (width/height), not the window size.
      SetSize(width ,height);
      SetMaxSize(width ,height);
    }

  // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Screen Windows] Part 2 Ini: x=%04d, y=%04d (%04d,%04d)  Bitxpixel (%d)"), posx, posy, width, height, GetBitsperPixel());

  if(show)
    {
      if(!Show(show)) return false;
    }

  if(!SetFocus(hwnd))
    {
      return false;
    } 
    

  hdc = GetDC(hwnd);
  if(!hdc)
    {
      return false;
    }


  #ifndef OPENGL_INFO
  memset(&hinfo,0,sizeof(BITMAPINFO));

  hinfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
  hinfo.bmiHeader.biWidth         = width;
  hinfo.bmiHeader.biHeight        = height;
  hinfo.bmiHeader.biPlanes        = 1;
  hinfo.bmiHeader.biBitCount      = GetBitsperPixel();
  hinfo.bmiHeader.biCompression   = BI_RGB;
  hinfo.bmiHeader.biSizeImage     = (XDWORD)(hinfo.bmiHeader.biWidth*hinfo.bmiHeader.biHeight*hinfo.bmiHeader.biBitCount)/8;
  #endif

  #ifdef GRP_OPENGL_ACTIVE  
  if(!Style_Is(GRPSCREENSTYLE_TRANSPARENT))
    {
      if(!blitgles)
        {
          blitgles = GEN_NEW GRPWINDOWSBLITGLES();
          if(!blitgles)
            {
              XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[Screen Windows] Could not allocate GRPWINDOWSBLITGLES"));
              return false;
            }

          if(!blitgles->Create(this))
            {
              XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[Screen Windows] BlitGLES create failed"));
              GEN_DELETE blitgles;
              blitgles = NULL;
              return false;
            }
        }
    }
  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         LRESULT CALLBACK GRPWINDOWSSCREEN::BaseWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
* @brief      Base wnd proc
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  hwnd : Hwnd value.
* @param[in]  msg : Msg value.
* @param[in]  wparam : Wparam value.
* @param[in]  lparam : Lparam value.
* 
* @return     LRESULT CALLBACK : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
LRESULT CALLBACK GRPWINDOWSSCREEN::BaseWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch(msg)
    {
      case WM_SYSCOMMAND            :  switch(wparam)
                                        {
                                          case SC_CLOSE         : break;
                                          case SC_SCREENSAVE    :
                                          case SC_MONITORPOWER  : return 0;
                                        }
                                      break;

      case WM_ENDSESSION            : break;

      // NOTE: only taken for windows created with Chromes caption OFF and resize ON (WS_THICKFRAME without
      // WS_CAPTION). In that combination Windows still reserves/paints the standard sizing-frame non-client
      // area (visible as a thin bar/border), even though there is no caption; these two messages remove it
      // while keeping the edges/corners resizable by hand-testing the cursor position ourselves.
      case WM_NCCALCSIZE            : { LONG stylebits = GetWindowLong(hwnd, GWL_STYLE);

                                        if(wparam && (stylebits & WS_THICKFRAME) && !(stylebits & WS_CAPTION))
                                          {
                                            return 0; // Client area = whole window (no non-client border).
                                          }
                                      }
                                      break;

      case WM_NCHITTEST              : { LONG stylebits = GetWindowLong(hwnd, GWL_STYLE);

                                        if((stylebits & WS_THICKFRAME) && !(stylebits & WS_CAPTION))
                                          {
                                            RECT rect;
                                            GetWindowRect(hwnd, &rect);

                                            int x       = (int)(short)LOWORD(lparam);
                                            int y       = (int)(short)HIWORD(lparam);
                                            int border  = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);

                                            bool onleft   = (x >= rect.left)   && (x <  rect.left   + border);
                                            bool onright  = (x <  rect.right)  && (x >= rect.right  - border);
                                            bool ontop    = (y >= rect.top)    && (y <  rect.top    + border);
                                            bool onbottom = (y <  rect.bottom) && (y >= rect.bottom - border);

                                            if(ontop    && onleft)  return HTTOPLEFT;
                                            if(ontop    && onright) return HTTOPRIGHT;
                                            if(onbottom && onleft)  return HTBOTTOMLEFT;
                                            if(onbottom && onright) return HTBOTTOMRIGHT;
                                            if(onleft)              return HTLEFT;
                                            if(onright)             return HTRIGHT;
                                            if(ontop)               return HTTOP;
                                            if(onbottom)            return HTBOTTOM;

                                            return HTCLIENT;
                                          }
                                      }
                                      break;

      case WM_MOVE                  : { GRPWINDOWSSCREEN* screen =  (GRPWINDOWSSCREEN*)GRPSCREEN::GetListScreens()->Get((void*)hwnd);
                                        if(screen)
                                          {
                                            screen->SetPosition((int)(short) LOWORD(lparam),(int)(short) HIWORD(lparam));

                                            //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Windows] move: x=%04d, y=%04d (%04d,%04d)"), screen->GetPositionX(), screen->GetPositionY(), screen->GetWidth(), screen->GetHeight());
                                          }
                                      }
                                      break;

      case WM_CLOSE                 : {  GRPWINDOWSSCREEN* screen = (GRPWINDOWSSCREEN*)GRPSCREEN::GetListScreens()->Get((void*)hwnd);
                                         if(screen)
                                           {
                                            if(!screen->CanClose())
                                              {
                                                return 0;
                                              }
                                           }                                                                                                                             
                                      }  
                                      break;    

      case WM_DESTROY               : { GRPWINDOWSSCREEN* screen = (GRPWINDOWSSCREEN*)GRPSCREEN::GetListScreens()->Get((void*)hwnd);

                                        if(screen && mainprocwindows.GetAppMain())
                                          {
                                            APPFLOWGRAPHICS* appgraphics = dynamic_cast<APPFLOWGRAPHICS*>(mainprocwindows.GetAppMain()->GetApplication());
                                            if(appgraphics)
                                              {
                                                if(screen == appgraphics->GetMainScreen())
                                                  {
                                                    PostQuitMessage(0);
                                                  }
                                              }
                                          }                                        
                                      }
                                      break;

      case WM_WINDOWPOSCHANGING     : { GRPWINDOWSSCREEN* screen = (GRPWINDOWSSCREEN*)GRPSCREEN::GetListScreens()->Get((void*)hwnd);                                       
                                        if(screen)
                                          {   
                                            WINDOWPOS* pwp= (WINDOWPOS*)lparam;
                                            if(!pwp) 
                                              {
                                                break;
                                              }
  
                                            if(screen->Style_Is(GRPSCREENSTYLE_ZORDER))
                                              {
                                                pwp->flags |= SWP_NOZORDER;                                        
                                              }
                                          }
                                      }
                                      break;  

      case WM_PAINT                 : break;

      case WM_POWERBROADCAST        : //Computer is suspending
                                      if(wparam == PBT_APMSUSPEND)
                                        {
                                          if(mainprocwindows.GetAppMain())
                                            {
                                              if(mainprocwindows.GetAppMain()->GetApplication()) mainprocwindows.GetAppMain()->GetApplication()->SetSystemChangeStatus(XSYSTEM_CHANGESTATUSTYPE_SUSPEND);
                                            }                                          
                                        }

                                      break;

      case WM_QUERYENDSESSION       : //Computer is shutting down
                                      if(lparam == 0)
                                        {
                                          if(mainprocwindows.GetAppMain())
                                            {
                                              if(mainprocwindows.GetAppMain()->GetApplication()) mainprocwindows.GetAppMain()->GetApplication()->SetSystemChangeStatus(XSYSTEM_CHANGESTATUSTYPE_REBOOT);
                                            }                                          
                                        }

                                      //User is logging off
                                      if((lparam & ENDSESSION_LOGOFF) == ENDSESSION_LOGOFF)
                                        {                                          
                                          if(mainprocwindows.GetAppMain())
                                            {
                                              if(mainprocwindows.GetAppMain()->GetApplication()) mainprocwindows.GetAppMain()->GetApplication()->SetSystemChangeStatus(XSYSTEM_CHANGESTATUSTYPE_SESSION_LOGOFF);
                                            }                                         
                                        }

                                      break;
    }
 
  return DefWindowProc(hwnd, msg, wparam, lparam);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPWINDOWSSCREEN::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPWINDOWSSCREEN::Clean()
{
  hinstance       = NULL;
  hwnd            = NULL;
  hdc             = NULL;

  #ifdef GRP_OPENGL_ACTIVE
  blitgles        = NULL;
  #endif
}


