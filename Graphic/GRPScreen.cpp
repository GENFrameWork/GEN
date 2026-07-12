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

#ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
#include "XPathsManager.h"
#include "UI_Manager.h"
#include "UI_Layout.h"
#include "UI_Element_Text.h"
#include "INPManager.h"
#include "XTimer.h"
#endif


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

  #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
  if(cfgchromesautohidetimer)
    {
      GEN_XFACTORY.DeleteTimer(cfgchromesautohidetimer);
      cfgchromesautohidetimer = NULL;
    }
  #endif

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
* @fn         GRPSCREENCFGCHROMES* GRPSCREEN::GetCFGChromes()
* @brief      Get CFG chromes
* @ingroup    GRAPHIC
* 
* @return     GRPSCREENCFGCHROMES* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPSCREENCFGCHROMES* GRPSCREEN::GetCFGChromes()
{
  return &cfgchromes;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::SetCFGChromes(GRPSCREENCFGCHROMES& cfgchromes)
* @brief      Set CFG chromes
* @ingroup    GRAPHIC
* 
* @param[in]  cfgchromes : Chromes configuration value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::SetCFGChromes(GRPSCREENCFGCHROMES& cfgchromes)
{
  if(!this->cfgchromes.CopyFrom(cfgchromes)) return false;

  cfgchromesactive = true;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::IsCFGChromesActive()
* @brief      Is CFG chromes active
* @ingroup    GRAPHIC
* 
* @return     bool : true if a chromes configuration has been assigned to the screen; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::IsCFGChromesActive()
{
  return cfgchromesactive;
}


#ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::LoadCFGChromesLayout()
* @brief      Load CFG chromes layout
* @note       Resolves and loads the .xml (or .zip bundle) + layout named in GetCFGChromes(), and keeps a weak
*             reference to it (see GetCFGChromesLayout()). A no-op (returns false) when there is no active
*             Chromes configuration, when native chromes are requested (nothing to load, by design), when the
*             screen has no viewport/canvas yet (call this once the native window is created), or when the
*             file/layout name have not been set.
* @ingroup    GRAPHIC
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::LoadCFGChromesLayout()
{
  if(!IsCFGChromesActive())               return false;
  if(cfgchromes.GetUseNativeChromes())    return false;

  GRPVIEWPORT* viewport = GetViewport(0);
  if(!viewport)                           return false;
  if(!viewport->GetCanvas())              return false;

  XSTRING* namelayoutfile = cfgchromes.GetCustomLayoutFile();
  XSTRING* namelayout     = cfgchromes.GetCustomLayoutName();

  if(!namelayoutfile || namelayoutfile->IsEmpty())  return false;
  if(!namelayout      || namelayout->IsEmpty())     return false;

  // Already loaded (e.g. shared with other layouts/layers that loaded this same file earlier): just reuse it.
  cfgchromeslayout = GEN_USERINTERFACE.Layouts_Get(namelayout->Get());

  if(!cfgchromeslayout)
    {
      // namelayoutfile is already a full, resolved path (either the class' own default -- see
      // GRPSCREENCFGCHROMES::Clean() -- or whatever the app itself set via SetCustomLayoutFile()); no section
      // path gets prepended here. UI_MANAGER::Load() already tells a plain .xml apart from a compressed .zip
      // bundle by extension.
      XPATH pathfile;

      pathfile = namelayoutfile->Get();

      if(!GEN_USERINTERFACE.Load(pathfile, this, 0)) return false;

      cfgchromeslayout = GEN_USERINTERFACE.Layouts_Get(namelayout->Get());
    }

  if(!cfgchromeslayout) return false;

  // The layout's own role="title" element (if any) keeps whatever static text it was authored with in the
  // .xml; once custom chromes are active, this screen's own title (GetTitle(), the same string a native
  // caption would show) is what must actually be displayed, always -- so it is pushed into that element here,
  // every time this function runs (fresh load or an already-loaded layout shared with something else, e.g. a
  // different screen with a different title of its own).
  UI_ELEMENT* titleelement = cfgchromeslayout->Elements_Get(UI_ELEMENT_CHROMEROLE_TITLE);
  if(titleelement && (titleelement->GetType() == UI_ELEMENT_TYPE_TEXT))
    {
      ((UI_ELEMENT_TEXT*)titleelement)->GetText()->Set(GetTitle()->Get());
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_LAYOUT* GRPSCREEN::GetCFGChromesLayout()
* @brief      Get CFG chromes layout
* @ingroup    GRAPHIC
* 
* @return     UI_LAYOUT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUT* GRPSCREEN::GetCFGChromesLayout()
{
  return cfgchromeslayout;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::UpdateCFGChromesDrag()
* @brief      Update CFG chromes drag
* @note       Reads GEN_INPMANAGER's mouse device directly (raw cursor position + raw button held state) -- not
*             the UserInterface event pipeline, and not whatever input polling the running application does for
*             its own purposes; those stay entirely separate. On the frame the button goes down, hit-tests the
*             layout's role="caption" element (if any); while still held, moves this screen by exactly how much
*             the mouse has moved in real desktop coordinates since the drag started (rebuilt every frame from
*             the window's CURRENT position, so the window's own movement never feeds back into the next
*             frame's delta). Releasing the button ends the drag.
* @ingroup    GRAPHIC
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::UpdateCFGChromesDrag()
{
  if(!IsCFGChromesActive())            return false;
  if(cfgchromes.GetUseNativeChromes()) return false;
  if(!cfgchromeslayout)                return false;

  INPDEVICE* mousedevice = GEN_INPMANAGER.GetDevice(INPDEVICE_TYPE_MOUSE);
  if(!mousedevice) return false;

  INPBUTTON* inpbutton = mousedevice->GetButton(INPBUTTON_ID_MOUSE_LEFT);
  if(!inpbutton) return false;

  if(!inpbutton->IsPressed())
    {
      cfgchromesdragging = false;
      return true;
    }

  int uix = 0;
  int uiy = 0;

  // GetCFGChromesCursorPosition() returns false whenever the cursor is currently outside this window's client
  // bounds -- easy to hit for a frame or two during a fast drag, since the window is still catching up to the
  // cursor. Treat it as "no new reading this frame" rather than a real coordinate: feeding a bogus one into the
  // position math below was exactly what sent the window flying off when dragging quickly. Skipping it just
  // holds the window in place for a frame; the very next valid reading picks the drag back up seamlessly.
  if(!GetCFGChromesCursorPosition(uix, uiy)) return true;

  if(!cfgchromesdragging)
    {
      UI_ELEMENT* captionelement = cfgchromeslayout->Elements_Get(UI_ELEMENT_CHROMEROLE_CAPTION);
      if(!captionelement)                                                return false;
      if(!captionelement->IsActive() || !captionelement->IsVisible())    return false;

      UI_BOUNDARYLINE bline;

      bline.x      = captionelement->GetXPosition();
      bline.y      = captionelement->GetYPosition();
      bline.width  = captionelement->GetBoundaryLine()->width;
      bline.height = captionelement->GetBoundaryLine()->height;

      if(!bline.IsWithin(uix, uiy)) return false;

      cfgchromesdragging         = true;
      cfgchromesdragstartscreenx = GetPositionX();
      cfgchromesdragstartscreeny = GetPositionY();
      cfgchromesdragstartcursorx = uix + cfgchromesdragstartscreenx;
      cfgchromesdragstartcursory = uiy + cfgchromesdragstartscreeny;

      return true;
    }

  int desktopcursorx = uix + GetPositionX();
  int desktopcursory = uiy + GetPositionY();

  Set_Position(cfgchromesdragstartscreenx + (desktopcursorx - cfgchromesdragstartcursorx),
               cfgchromesdragstartscreeny + (desktopcursory - cfgchromesdragstartcursory));

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::GetCFGChromesCursorPosition(int& uix, int& uiy)
* @brief      Get CFG chromes cursor position
* @note       INTERNAL: reads GEN_INPMANAGER's mouse device directly (raw, client-relative, bottom-up -- 0 at the
*             bottom of the client area, increasing upward, confirmed by tracing INPWINDOWSDEVICEMOUSE::Update())
*             and converts it into the same top-down UI-space coordinates UI_ELEMENT's own
*             GetXPosition()/GetYPosition() use. Returns false (uix/uiy left untouched) when the cursor is
*             currently outside this window's client area (the device reports (-1,-1) in that case) or the
*             mouse device itself is unavailable. Shared by UpdateCFGChromesDrag() and UpdateCFGChromesAutoHide().
* @ingroup    GRAPHIC
* 
* @param[out] uix : X coordinate, UI-space (top-down, client-relative).
* @param[out] uiy : Y coordinate, UI-space (top-down, client-relative).
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::GetCFGChromesCursorPosition(int& uix, int& uiy)
{
  INPDEVICE* mousedevice = GEN_INPMANAGER.GetDevice(INPDEVICE_TYPE_MOUSE);
  if(!mousedevice) return false;

  INPCURSOR* inpcursor = mousedevice->GetCursor(INPCURSOR_ID_MOUSE);
  if(!inpcursor) return false;

  int rawx = (int)inpcursor->GetX();
  int rawy = (int)inpcursor->GetY();

  if((rawx == -1) && (rawy == -1)) return false;

  uix = rawx;
  uiy = GetHeight() - rawy;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         static void GRPSCREEN_SetElementVisibleRecursive(UI_ELEMENT* element, bool visible)
* @brief      INTERNAL: sets an element and every one of its compose (child) elements visible/invisible.
*             UI_ELEMENT::SetVisible() only ever affects the element itself, so hiding just the caption's own
*             "form" element would leave its icon/title/buttons drawn on their own.
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
static void GRPSCREEN_SetElementVisibleRecursive(UI_ELEMENT* element, bool visible)
{
  if(!element) return;

  element->SetVisible(visible);

  for(XDWORD c=0; c<element->GetComposeElements()->GetSize(); c++)
    {
      GRPSCREEN_SetElementVisibleRecursive(element->GetComposeElements()->Get(c), visible);
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREEN::UpdateCFGChromesAutoHide()
* @brief      Update CFG chromes auto hide
* @note       Hides the layout's role="caption" element (and everything inside it: icon, title, min/max/close)
*             when the cursor has stayed steadily outside its own area for GetCFGChromes()->GetCustomAutoHide()
*             milliseconds, and shows it again once the cursor has stayed steadily inside that same area for the
*             same amount of time. The debounce timer resets every time the cursor moves in or out, so a quick
*             pass through the caption's edge never triggers a transition by itself. A no-op when auto-hide is 0
*             (disabled, the default): the caption is left exactly as it already is.
* @ingroup    GRAPHIC
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREEN::UpdateCFGChromesAutoHide()
{
  if(!IsCFGChromesActive())            return false;
  if(cfgchromes.GetUseNativeChromes()) return false;
  if(!cfgchromeslayout)                return false;

  XDWORD timehidden = cfgchromes.GetCustomAutoHide();
  if(!timehidden) return false;

  UI_ELEMENT* captionelement = cfgchromeslayout->Elements_Get(UI_ELEMENT_CHROMEROLE_CAPTION);
  if(!captionelement) return false;

  if(!cfgchromesautohidetimer)
    {
      cfgchromesautohidetimer = GEN_XFACTORY.CreateTimer();
      if(!cfgchromesautohidetimer) return false;

      cfgchromesautohidetimer->Reset();
    }

  int  uix             = 0;
  int  uiy             = 0;
  bool cursoravailable = GetCFGChromesCursorPosition(uix, uiy);

  UI_BOUNDARYLINE bline;

  bline.x      = captionelement->GetXPosition();
  bline.y      = captionelement->GetYPosition();
  bline.width  = captionelement->GetBoundaryLine()->width;
  bline.height = captionelement->GetBoundaryLine()->height;

  // A cursor reading unavailable (e.g. the cursor is outside this window entirely) definitely counts as "not
  // over the caption".
  bool cursorover = cursoravailable && bline.IsWithin(uix, uiy);

  if(cursorover != cfgchromesautohidedesired)
    {
      cfgchromesautohidedesired = cursorover;
      cfgchromesautohidetimer->Reset();
    }

  if((cfgchromesautohidedesired != cfgchromesautohidevisible) &&
     (cfgchromesautohidetimer->GetMeasureMilliSeconds() >= timehidden))
    {
      cfgchromesautohidevisible = cfgchromesautohidedesired;

      GRPSCREEN_SetElementVisibleRecursive(captionelement, cfgchromesautohidevisible);
    }

  return true;
}

#endif


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

      #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
      
      if(viewports.GetSize() == 1) 
        {
          LoadCFGChromesLayout();
        }
      #endif

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

  #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
  UpdateCFGChromesDrag();
  UpdateCFGChromesAutoHide();
  #endif

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

  cfgchromesactive        = false;

  #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
  cfgchromeslayout        = NULL;

  cfgchromesdragging         = false;
  cfgchromesdragstartcursorx = 0;
  cfgchromesdragstartcursory = 0;
  cfgchromesdragstartscreenx = 0;
  cfgchromesdragstartscreeny = 0;

  cfgchromesautohidevisible  = true;      // starts shown, matching a freshly loaded/unaffected layout
  cfgchromesautohidedesired  = true;
  cfgchromesautohidetimer    = NULL;      // created lazily, on first actual use (see UpdateCFGChromesAutoHide())
  #endif
}


