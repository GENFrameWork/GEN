/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPScreen.cpp
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"
#if defined(__ANDROID__)
#include <android/log.h>
#endif



/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "GRPScreen.h"

#include "GRPFactory.h"
#include "GRPXEvent.h"
#include "GRP2DCanvas.h"
#include "GRPViewPort.h"
#include "GRPFrameRate.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

XMAP<void*, GRPSCREEN*>  GRPSCREEN::listscreens;



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPSCREEN::GRPSCREEN()
* @brief      Constructor of class
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPSCREEN::GRPSCREEN()
{
  Clean();

  RegisterEvent(GRPXEVENT_TYPE_SCREEN_CREATING);
  RegisterEvent(GRPXEVENT_TYPE_SCREEN_CREATED);
  RegisterEvent(GRPXEVENT_TYPE_SCREEN_CANVASCREATING);
  RegisterEvent(GRPXEVENT_TYPE_SCREEN_DELETING);
  RegisterEvent(GRPXEVENT_TYPE_SCREEN_DELETED);
  RegisterEvent(GRPXEVENT_TYPE_SCREEN_CHANGEPOSITION);
  RegisterEvent(GRPXEVENT_TYPE_SCREEN_CHANGESIZE);
  RegisterEvent(GRPXEVENT_TYPE_SCREEN_CHANGEFOCUS);

  framerate = GEN_NEW GRPFRAMERATE();

  desktopmanager = GEN_GRPFACTORY.CreateDesktopManager();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPSCREEN::~GRPSCREEN()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPSCREEN::~GRPSCREEN()
{
  if(framerate)
    {
      GEN_DELETE framerate;
      framerate = NULL;
    }

  if(desktopmanager)
    {
      GEN_GRPFACTORY.DeleteDesktopManager(desktopmanager);
      desktopmanager = NULL;
    }

  DeRegisterEvent(GRPXEVENT_TYPE_SCREEN_CREATING);
  DeRegisterEvent(GRPXEVENT_TYPE_SCREEN_CANVASCREATING);
  DeRegisterEvent(GRPXEVENT_TYPE_SCREEN_CREATED);
  DeRegisterEvent(GRPXEVENT_TYPE_SCREEN_DELETING);
  DeRegisterEvent(GRPXEVENT_TYPE_SCREEN_DELETED);
  DeRegisterEvent(GRPXEVENT_TYPE_SCREEN_CHANGEFOCUS);
  DeRegisterEvent(GRPXEVENT_TYPE_SCREEN_CHANGESIZE);
  DeRegisterEvent(GRPXEVENT_TYPE_SCREEN_CHANGEPOSITION);

  DeleteAllViewports();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPSCREENTYPE GRPSCREEN::GetType()
* @brief      Get type
* @ingroup    GRAPHIC
*
* @return     GRPSCREENTYPE : Screen type
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPSCREENTYPE GRPSCREEN::GetType()
{
  return type;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPSCREEN::IsValid()
* @brief      Is valid
* @ingroup    GRAPHIC
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::IsValid()
{
  return isvalid;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPSCREEN::IsActive()
* @brief      Is active
* @ingroup    GRAPHIC
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::IsActive()
{
  return isactive;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::Style_Is(XDWORD style)
* @brief      Style is
* @ingroup    GRAPHIC
* 
* @param[in]  style : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::Style_Is(XDWORD style)
{
  return (styles & style)?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREEN::Style_Add(XDWORD style)
* @brief      Style add
* @ingroup    GRAPHIC
* 
* @param[in]  style : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREEN::Style_Add(XDWORD style)
{
  Styles_Set(Styles_Get() | style);  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREEN::Style_Remove(XDWORD style)
* @brief      Style remove
* @ingroup    GRAPHIC
* 
* @param[in]  style : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREEN::Style_Remove(XDWORD style)
{
  Styles_Set(Styles_Get() & ~style);  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD GRPSCREEN::Styles_Get()
* @brief      Styles get
* @ingroup    GRAPHIC
* 
* @return     XDWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD GRPSCREEN::Styles_Get()
{
  return styles;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREEN::Styles_Set(XDWORD styles)
* @brief      Styles set
* @ingroup    GRAPHIC
* 
* @param[in]  styles : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREEN::Styles_Set(XDWORD styles)
{
  this->styles = styles;

  if(Styles_IsFullScreen())
    {
      this->styles &= ~GRPSCREENSTYLE_TITLE;
      this->styles &= ~GRPSCREENSTYLE_NOWINDOWICONS;
      this->styles |=  GRPSCREENSTYLE_ONTOP;
    }  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::Styles_IsFullScreen()
* @brief      Styles is full screen
* @ingroup    GRAPHIC
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::Styles_IsFullScreen()
{
  if(((this->styles & GRPSCREENSTYLE_FULLSCREEN)                   == GRPSCREENSTYLE_FULLSCREEN)                    ||
     ((this->styles & GRPSCREENSTYLE_FULLSCREEN_ADJUSTRESOLUTION)  == GRPSCREENSTYLE_FULLSCREEN_ADJUSTRESOLUTION))
    {
      return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::SetPropertys(int width, int height, float DPIs, int stride, GRPPROPERTYMODE mode)
* @brief      Set propertys
* @ingroup    GRAPHIC
* 
* @param[in]  width : 
* @param[in]  height : 
* @param[in]  DPIs : 
* @param[in]  stride : 
* @param[in]  mode : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::SetPropertys(int width, int height, float DPIs, int stride, GRPPROPERTYMODE mode)
{
  SetWidth(width);
  SetHeight(height);

  SetDPI(DPIs);

  SetStride(stride);

  SetMode(mode);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPSCREENROTATION GRPSCREEN::GetRotation()
* @brief      Get rotation
* @ingroup    GRAPHIC
* 
* @return     GRPSCREENROTATION : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPSCREENROTATION GRPSCREEN::GetRotation()
{
  return rotation;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREEN::SetRotation(GRPSCREENROTATION rotation)
* @brief      Set rotation
* @ingroup    GRAPHIC
* 
* @param[in]  rotation : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREEN::SetRotation(GRPSCREENROTATION rotation)
{
  this->rotation = rotation;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::Rotate(int degrees)
* @brief      Configures the screen presentation rotation, expressed in degrees, CLOCKWISE.
* 
*             This is a screen-level configuration (it may be set before Create(), exactly like the
*             resolution via SetWidth()/SetHeight()). The rotation is ONLY honoured by the OpenGL ES
*             presentation path (GRPBLITGLES), which applies it on the GPU as part of the fullscreen
*             quad model matrix — costing nothing per frame and working identically on Windows
*             (ANGLE/D3D11), Linux/X11 (Mesa) and Android (NDK).
* 
*             Only the four axis-aligned orientations are supported (0, 90, 180, 270). Any other angle
*             is snapped to the nearest one. Restricting to multiples of 90 keeps the transform a pure
*             axis-aligned remap of the quad (no bilinear resampling, no fractional letterbox), which is
*             both faster and pixel-exact.
* 
*             Width/height handling (this is the key point): the SCREEN width/height keep representing
*             the CONTENT the application draws (canvas / viewport / UI), so SetWidth()/SetHeight() and
*             every layout that relies on them are left untouched -> no resampling, no broken layouts.
*             What changes is the NATIVE WINDOW: it is created in the rotated orientation through
*             GetPresentationWidth()/GetPresentationHeight() (width and height swapped on 90/270), so the
*             rotated content fills the window edge to edge with no letterbox.
* 
* @ingroup    GRAPHIC
* 
* @param[in]  degrees : clockwise rotation in degrees (snapped to 0, 90, 180 or 270).
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::Rotate(int degrees)
{
  // Normalize to [0, 360)
  int normalized = degrees % 360;
  if(normalized < 0)
    {
      normalized += 360;
    }

  // Snap to the nearest 90 degree step: 0, 1, 2, 3  ->  0, 90, 180, 270
  int step = ((normalized + 45) / 90) % 4;

  switch(step)
    {
      case 1  : rotation = GRPSCREENROTATION_90_CLOCKWISE;     break;   //  90 clockwise
      case 2  : rotation = GRPSCREENROTATION_180;              break;   // 180
      case 3  : rotation = GRPSCREENROTATION_90_ANTICLOCKWISE; break;   // 270 clockwise == 90 anti-clockwise
      case 0  :
      default : rotation = GRPSCREENROTATION_NONE;             break;   //   0 (no rotation)
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::IsRotationOrthogonal()
* @brief      true when the current rotation swaps the screen axes (90 / 270 degrees)
* @ingroup    GRAPHIC
* 
* @return     bool : true if the rotation is 90 or 270 degrees. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::IsRotationOrthogonal()
{
  return (rotation == GRPSCREENROTATION_90_CLOCKWISE || rotation == GRPSCREENROTATION_90_ANTICLOCKWISE);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD GRPSCREEN::GetPresentationWidth()
* @brief      Width of the NATIVE WINDOW (presentation surface). On a 90/270 rotation the content is
*             presented with its axes swapped, so the window width is the content HEIGHT. The content
*             itself (canvas / viewport / UI, sized by GetWidth()/GetHeight()) is never changed.
* @ingroup    GRAPHIC
* 
* @return     XDWORD : window width in pixels. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD GRPSCREEN::GetPresentationWidth()
{
  return IsRotationOrthogonal() ? GetHeight() : GetWidth();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD GRPSCREEN::GetPresentationHeight()
* @brief      Height of the NATIVE WINDOW (presentation surface). See GetPresentationWidth().
* @ingroup    GRAPHIC
* 
* @return     XDWORD : window height in pixels. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD GRPSCREEN::GetPresentationHeight()
{
  return IsRotationOrthogonal() ? GetWidth() : GetHeight();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPSCREEN::Create(void* handle)
* @brief      Create
* @ingroup    GRAPHIC
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::Create(bool show)
{
  screencanvas = GRPFACTORY::GetInstance().CreateCanvas(this);
  if(!screencanvas)
    {
      return false;
    }

  // The screen width/height ARE the content size (the rotation only affects the native window,
  // see GRPSCREEN::Rotate / GetPresentationWidth/Height), so the canvas keeps the screen size.
  screencanvas->SetWidth(this->GetWidth());
  screencanvas->SetHeight(this->GetHeight());

  screencanvas->Buffer_Create();
      
  isactive = true;

  if(framerate) framerate->Reset();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::Update(GRP2DCANVAS* canvas)
* @brief      Update
* @ingroup    GRAPHIC
* 
* @param[in]  canvas : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::Update(GRP2DCANVAS* canvas)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::UpdateTransparent(GRP2DCANVAS* canvas)
* @brief      Update transparent
* @ingroup    GRAPHIC
* 
* @param[in]  canvas : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::UpdateTransparent(GRP2DCANVAS* canvas)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPSCREEN::Delete()
* @brief      Delete
* @ingroup    GRAPHIC
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::Delete()
{
  if(screencanvas)
    {
      GEN_DELETE screencanvas;
      screencanvas = NULL;
    }

  isactive = false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::Get_Position(int &x, int &y)
* @brief      Get position
* @ingroup    GRAPHIC
* 
* @param[in]  &x : 
* @param[in]  &y : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::Get_Position(int &x, int &y)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::Set_Position(int x, int y)
* @brief      Set position
* @ingroup    GRAPHIC
* 
* @param[in]  x : 
* @param[in]  y : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::Set_Position(int x, int y)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPSCREEN::Resize(int width, int height)
* @brief
* @ingroup    GRAPHIC
*
* @param[in]  width :
* @param[in]  height :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::Resize(int width, int height)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPSCREEN::IsShow()
* @brief      Is show
* @ingroup    GRAPHIC
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::IsShow()
{
  return isshow;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPSCREEN::Show(bool active)
* @brief      Show
* @ingroup    GRAPHIC
*
* @param[in]  active :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::Show(bool active)
{
  isshow  = active;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPSCREEN::ShowCursor(bool active)
* @brief      Show cursor
* @ingroup    GRAPHIC
*
* @param[in]  active :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::ShowCursor(bool active)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void* GRPSCREEN::GetHandle()
* @brief
* @ingroup    GRAPHIC
*
* @return     void* :
*
* --------------------------------------------------------------------------------------------------------------------*/
void* GRPSCREEN::GetHandle()
{
  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREEN::SetHandle(XDWORD handle_window)
* @brief      Set handle
* @ingroup    GRAPHIC
* 
* @param[in]  handle_window : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREEN::SetHandle(void* handle_window)
{
  return;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::Set_Focus()
* @brief      Set focus
* @ingroup    GRAPHIC
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::Set_Focus()
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::Minimize(bool active)
* @brief      Minimize
* @ingroup    GRAPHIC
* 
* @param[in]  active : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::Minimize(bool active)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::Maximize(bool active)
* @brief      Maximize
* @ingroup    GRAPHIC
* 
* @param[in]  active : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::Maximize(bool active)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPBITMAP* GRPSCREEN::CaptureContent(GRPRECTINT* rect, void* handle_window)
* @brief      Capture content
* @ingroup    GRAPHIC
* 
* @param[in]  rect : 
* @param[in]  handle_window : 
* 
* @return     GRPBITMAP* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPBITMAP* GRPSCREEN::CaptureContent(GRPRECTINT* rect, void* handle_window)
{
  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void* GRPSCREEN::GetDesktopHandle()
* @brief      Get desktop handle
* @ingroup    GRAPHIC
* 
* @return     void* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void* GRPSCREEN::GetDesktopHandle()
{
  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void* GRPSCREEN::GetShellHandle()
* @brief      Get shell handle
* @ingroup    GRAPHIC
* 
* @return     void* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void* GRPSCREEN::GetShellHandle()
{
  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::CanClose()
* @brief      can close
* @ingroup    GRAPHIC
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::CanClose()
{
  return canclose;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREEN::SetCanClose(bool canclose)
* @brief      set can close
* @ingroup    GRAPHIC
* 
* @param[in]  canclose : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREEN::SetCanClose(bool canclose)
{
  this->canclose = canclose;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* GRPSCREEN::GetTitle()
* @brief      Get title
* @ingroup    GRAPHIC
* 
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* GRPSCREEN::GetTitle()
{
  return &title;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPSCREEN::UpdateSize(int width, int height)
* @brief
* @ingroup    GRAPHIC
*
* @param[in]  width :
* @param[in]  height :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::UpdateSize(int width, int height)
{
  GRPXEVENT grpevent(this, GRPXEVENT_TYPE_SCREEN_CHANGESIZE);
  grpevent.SetScreen(this);

  PostEvent(&grpevent);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<GRPVIEWPORT*>* GRPSCREEN::GetViewports()
* @brief
* @ingroup    GRAPHIC
*
* @return     XVECTOR<GRPVIEWPORT*>* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<GRPVIEWPORT*>* GRPSCREEN::GetViewports()
{
  return &viewports;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPVIEWPORT* GRPSCREEN::GetViewport(int index)
* @brief
* @ingroup    GRAPHIC
*
* @param[in]  index :
*
* @return     GRPVIEWPORT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPVIEWPORT* GRPSCREEN::GetViewport(int index)
{
  return (viewports.Get(index));
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVIEWPORT* GRPSCREEN::GetViewport(XCHAR* ID)
* @brief      Get viewport
* @ingroup    GRAPHIC
*
* @param[in]  ID : 
* 
* @return     GRPVIEWPORT* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
GRPVIEWPORT* GRPSCREEN::GetViewport(XCHAR* ID)
{
  for(XDWORD c=0; c<viewports.GetSize(); c++)
    {
      GRPVIEWPORT* viewport = viewports.Get(c);
      if(viewport) 
        {
          if(!viewport->GetID()->Compare(ID, true)) return viewport;
        }                    
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPSCREEN::CreateViewport(XCHAR* ID, float posx, float posy, float width, float height, XDWORD canvasposx, XDWORD canvasposy, XDWORD canvaswidth, XDWORD canvasheight)
* @brief      Create viewport
* @ingroup    GRAPHIC
*
* @param[in]  ID :
* @param[in]  posx :
* @param[in]  posy :
* @param[in]  width :
* @param[in]  height :
* @param[in]  canvasposx :
* @param[in]  canvasposy :
* @param[in]  canvaswidth :
* @param[in]  canvasheight :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::CreateViewport(XCHAR* ID, float posx, float posy, float width, float height, XDWORD canvasposx, XDWORD canvasposy, XDWORD canvaswidth, XDWORD canvasheight)
{
  GRPVIEWPORT* viewport = GEN_NEW GRPVIEWPORT();
  if(!viewport)  return false;

  viewport->GetID()->Set(ID);
  viewport->SetPosition(posx, posy);
  viewport->SetSize(width, height);

  GRPPROPERTIES canvasproperties; 
  float         cwidth  = (float)canvaswidth;
  float         cheight = (float)canvasheight;

  if(width  > cwidth)  cwidth  = width;
  if(height > cheight) cheight = height;

  canvasproperties.CopyPropertysFrom(this);
  canvasproperties.SetPosition(canvasposx, canvasposy); 
  canvasproperties.SetSize((XDWORD)cwidth, (XDWORD)cheight);

  if(viewport->CreateCanvas(canvasproperties))
    {
      viewport->SetCanvasPosition(posx, posy);
      viewports.Add(viewport);
             
    } else return false;

  return true;
 }


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPSCREEN::UpdateViewports(int width, int height)
* @brief
* @ingroup    GRAPHIC
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::UpdateViewports()
{
  #if defined(__ANDROID__)
  __android_log_print(ANDROID_LOG_INFO, "GEN_BLIT", "PATH UpdateViewports ENTER screencanvas=%p transparent=%d nviewports=%d", (void*)screencanvas, (int)Style_Is(GRPSCREENSTYLE_TRANSPARENT), (int)viewports.GetSize());
  #endif
  if(!screencanvas)
    {   
      return false;
    }

  for(XDWORD c=0; c<viewports.GetSize(); c++)
    {      
      screencanvas->CopyBufferRenderFromViewport(viewports.Get(c));                               
    } 
       
  if(Style_Is(GRPSCREENSTYLE_TRANSPARENT))
    {
      UpdateTransparent(screencanvas);      
    }
    else
    {
      Update(screencanvas);      
    }    

  if(framerate) framerate->AddNFrames();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPSCREEN::DeleteViewport(GRPVIEWPORT* viewport)
* @brief      Delete viewport
* @ingroup    GRAPHIC
*
* @param[in]  viewport :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::DeleteViewport(GRPVIEWPORT* viewport)
{
  if(!viewport) 
    {
      return false;
    }

  viewports.Delete(viewport);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPSCREEN::DeleteAllViewports()
* @brief      Delete all viewports
* @ingroup    GRAPHIC
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::DeleteAllViewports()
{
  if(viewports.IsEmpty()) 
    {
      return false;
    }

  viewports.DeleteContents();
  viewports.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPFRAMERATE* GRPSCREEN::GetFrameRate()
* @brief      Get frame rate
* @ingroup    GRAPHIC
*
* @return     GRPFRAMERATE* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
GRPFRAMERATE* GRPSCREEN::GetFrameRate()
{
  return framerate;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPDESKTOPMANAGER* GRPSCREEN::GetDesktopManager()
* @brief      Get desktop manager
* @ingroup    GRAPHIC
* 
* @return     GRPDESKTOPMANAGER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPDESKTOPMANAGER* GRPSCREEN::GetDesktopManager()
{
  return desktopmanager;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPSCREENTYPE_DESKTOP GRPSCREEN::GetDesktopScreenSelected()
* @brief      Get desktop screen selected
* @ingroup    GRAPHIC
* 
* @return     GRPSCREENTYPE_DESKTOP : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPSCREENTYPE_DESKTOP GRPSCREEN::GetDesktopScreenSelected()
{
  return desktopscreenselected;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREEN::SetDesktopScreenSelected(GRPSCREENTYPE_DESKTOP desktopscreenselected)
* @brief      Set desktop screen selected
* @ingroup    GRAPHIC
* 
* @param[in]  desktopscreenselected : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREEN::SetDesktopScreenSelected(GRPSCREENTYPE_DESKTOP desktopscreenselected)
{
  this->desktopscreenselected = desktopscreenselected;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XMAP<void*, GRPSCREEN*>* GRPSCREEN::GetListScreens()
* @brief      Get list screens
* @ingroup    GRAPHIC
* 
* @return     XMAP<void*, : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XMAP<void*, GRPSCREEN*>* GRPSCREEN::GetListScreens()
{
  return &listscreens;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRP2DCANVAS* GRPSCREEN::GetScreenCanvas()
* @brief      Get screen canvas
* @ingroup    GRAPHIC
* 
* @return     GRP2DCANVAS* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DCANVAS* GRPSCREEN::GetScreenCanvas()
{
  return screencanvas;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::SetScreenCanvas(GRP2DCANVAS* screencanvas)
* @brief      Set screen canvas
* @ingroup    GRAPHIC
* 
* @param[in]  screencanvas : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::SetScreenCanvas(GRP2DCANVAS* screencanvas)
{
  this->screencanvas = screencanvas;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRPSCREEN::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREEN::Clean()
{
  type                    = GRPSCREENTYPE_UNKNOW;

  isvalid                 = false;
  isactive                = false;

  styles                  = GRPSCREENSTYLE_DEFAULT;

  rotation                = GRPSCREENROTATION_NONE;
     
  framerate               = NULL; 

  screencanvas            = NULL;

  desktopmanager          = NULL;
  desktopscreenselected   = GRPSCREENTYPE_DESKTOP_ALL;

  isshow                  = false;

  canclose                = false;
}


