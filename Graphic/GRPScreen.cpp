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
* @return     GRPSCREENTYPE : Requested value.
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
* @return     bool : true if the condition is met; otherwise false.
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
* @return     bool : true if the condition is met; otherwise false.
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
* @param[in]  style : Style value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  style : Style value.
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
* @param[in]  style : Style value.
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
* @return     XDWORD : Requested value.
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
* @param[in]  styles : Styles value.
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
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  width : Width value.
* @param[in]  height : Height value.
* @param[in]  DPIs : DP Is value.
* @param[in]  stride : Stride value.
* @param[in]  mode : Mode value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @return     GRPSCREENROTATION : Requested value.
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
* @param[in]  rotation : Rotation value.
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
* @ingroup    GRAPHIC
* 
* @param[in]  degrees : clockwise rotation in degrees (snapped to 0, 90, 180 or 270).
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::IsRotationOrthogonal()
{
  return (rotation == GRPSCREENROTATION_90_CLOCKWISE || rotation == GRPSCREENROTATION_90_ANTICLOCKWISE);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD GRPSCREEN::GetPresentationWidth()
* @brief      Width of the NATIVE WINDOW (presentation surface). On a 90/270 rotation the content is presented with its axes swapped, so the window width is the content HEIGHT. The content itself (canvas / viewport / UI, sized by GetWidth()/GetHeight()) is never changed.
* @ingroup    GRAPHIC
* 
* @return     XDWORD : Requested value.
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
* @return     XDWORD : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD GRPSCREEN::GetPresentationHeight()
{
  return IsRotationOrthogonal() ? GetWidth() : GetHeight();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::Create(bool show)
* @brief      Create resource
* @ingroup    GRAPHIC
* 
* @param[in]  show : Show value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  canvas : Canvas object to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  canvas : Canvas object to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::UpdateTransparent(GRP2DCANVAS* canvas)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::Delete()
* @brief      Delete resource
* @ingroup    GRAPHIC
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  x : X coordinate.
* @param[in]  y : Y coordinate.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  x : X coordinate.
* @param[in]  y : Y coordinate.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::Set_Position(int x, int y)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::Resize(int width, int height)
* @brief      Resize
* @ingroup    GRAPHIC
* 
* @param[in]  width : Width value.
* @param[in]  height : Height value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @return     bool : true if the condition is met; otherwise false.
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
* @param[in]  active : Active value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  active : Active value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::ShowCursor(bool active)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void* GRPSCREEN::GetHandle()
* @brief      Get handle
* @ingroup    GRAPHIC
* 
* @return     void* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void* GRPSCREEN::GetHandle()
{
  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREEN::SetHandle(void* handle_window)
* @brief      Set handle
* @ingroup    GRAPHIC
* 
* @param[in]  handle_window : Handle window pointer to use.
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
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  active : Active value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  active : Active value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  rect : Rectangle to use.
* @param[in]  handle_window : Handle window pointer to use.
* 
* @return     GRPBITMAP* : Pointer to the requested object; NULL if it is not available.
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
* @return     void* : Pointer to the requested object; NULL if it is not available.
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
* @return     void* : Pointer to the requested object; NULL if it is not available.
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
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  canclose : Canclose value.
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
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* GRPSCREEN::GetTitle()
{
  return &title;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::UpdateSize(int width, int height)
* @brief      Update Size
* @ingroup    GRAPHIC
* 
* @param[in]  width : Width value.
* @param[in]  height : Height value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @brief      Get viewports
* @ingroup    GRAPHIC
* 
* @return     XVECTOR<GRPVIEWPORT*>* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<GRPVIEWPORT*>* GRPSCREEN::GetViewports()
{
  return &viewports;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVIEWPORT* GRPSCREEN::GetViewport(int index)
* @brief      Get viewport
* @ingroup    GRAPHIC
* 
* @param[in]  index : Index value.
* 
* @return     GRPVIEWPORT* : Pointer to the requested object; NULL if it is not available.
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
* @param[in]  ID : Identifier to use.
* 
* @return     GRPVIEWPORT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  ID : Identifier to use.
* @param[in]  posx : Posx value.
* @param[in]  posy : Posy value.
* @param[in]  width : Width value.
* @param[in]  height : Height value.
* @param[in]  canvasposx : Canvasposx value.
* @param[in]  canvasposy : Canvasposy value.
* @param[in]  canvaswidth : Canvaswidth value.
* @param[in]  canvasheight : Canvasheight value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @fn         bool GRPSCREEN::UpdateViewports()
* @brief      Update Viewports
* @ingroup    GRAPHIC
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::UpdateViewports()
{
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
* @param[in]  viewport : Viewport pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @return     bool : true if the operation is successful; otherwise false.
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
* @return     GRPFRAMERATE* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @return     GRPDESKTOPMANAGER* : Pointer to the requested object; NULL if it is not available.
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
* @return     GRPSCREENTYPE_DESKTOP : Requested value.
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
* @param[in]  desktopscreenselected : Desktopscreenselected value.
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
* @return     XMAP<void*, GRPSCREEN*>* : Pointer to the requested object; NULL if it is not available.
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
* @return     GRP2DCANVAS* : Pointer to the requested object; NULL if it is not available.
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
* @param[in]  screencanvas : Screencanvas pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
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

  canclose                = true;
}


