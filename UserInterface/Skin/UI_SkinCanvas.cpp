/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_SkinCanvas.cpp
* 
* @class      UI_SKINCANVAS
* @brief      User Interface Skin Canvas class
* @ingroup    USERINTERFACE
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

#include "UI_SkinCanvas.h"

#include "XTrace.h"
#include "XTimer.h"
#include "XSleep.h"

#include "GRP2DCanvas.h"
#include "GRPViewPort.h"
#include "GRPScreen.h"
#include "GRPBitmap.h"
#include "GRPBitmapFile.h"

#include "UI_Animation.h"
#include "UI_Element.h"
#include "UI_Property_Scrolleable.h"
#include "UI_Element_Scroll.h"
#include "UI_Element_Text.h"
#include "UI_Element_TextBox.h"
#include "UI_Element_Image.h"
#include "UI_Element_Animation.h"
#include "UI_Element_Option.h"
#include "UI_Element_MultiOption.h"
#include "UI_Element_Button.h"
#include "UI_Element_CheckBox.h"
#include "UI_Element_EditText.h"
#include "UI_Element_Form.h"
#include "UI_Element_Menu.h"
#include "UI_Element_ListBox.h"
#include "UI_Element_ProgressBar.h"
#include "UI_Element_ProgressRadial.h"
#include "UI_Element_ProgressImage.h"
#include "UI_Layout.h"
#include "UI_Manager.h"

#include "APPFlowBase.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

#define UI_SKINCANVAS_SCROLLBAR_WIDTH       UI_PROPERTY_SCROLLEABLE_BARWIDTH    // overlay scrollbar thickness (px)
#define UI_SKINCANVAS_SCROLLBAR_MINTHUMB    16       // minimum thumb length (px)

#define UI_SKINCANVAS_SCROLLBAR_TRACK_R     0        // track color (faint)
#define UI_SKINCANVAS_SCROLLBAR_TRACK_G     0
#define UI_SKINCANVAS_SCROLLBAR_TRACK_B     0
#define UI_SKINCANVAS_SCROLLBAR_TRACK_A     30

#define UI_SKINCANVAS_SCROLLBAR_THUMB_R     80       // thumb color (semi-transparent)
#define UI_SKINCANVAS_SCROLLBAR_THUMB_G     80
#define UI_SKINCANVAS_SCROLLBAR_THUMB_B     80
#define UI_SKINCANVAS_SCROLLBAR_THUMB_A     160



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         static void UI_SkinCanvas_ProgressRadial_AppendArc(GRP2DPATH& path, double cx, double cy, double r, double startdeg, double spandeg, bool& firstpoint)
* @brief      Append a circular arc to a path as a short-segment polyline (~2 deg per step). Uses only MoveTo/LineTo,
*             which are fully exercised by the canvas stroke pipeline; this deliberately avoids GRP2DPATH::ArcTo
*             (the SVG elliptical-arc command), whose AGG arc_to conversion is not used anywhere else and produces
*             no geometry here. Angles are degrees; a positive span advances clockwise in this y-down canvas.
* @note       INTERNAL / FILE LOCAL
* @ingroup    USERINTERFACE
*
* @param[in]      path       : destination path.
* @param[in]      cx, cy     : ring center.
* @param[in]      r          : ring radius (to the stroke centerline).
* @param[in]      startdeg   : start angle in degrees.
* @param[in]      spandeg    : signed span in degrees.
* @param[in,out]  firstpoint : true on the first append (emits a MoveTo); set to false afterwards.
*
* ---------------------------------------------------------------------------------------------------------------------*/
static void UI_SkinCanvas_ProgressRadial_AppendArc(GRP2DPATH& path, double cx, double cy, double r, double startdeg, double spandeg, bool& firstpoint)
{
  if(r <= 0.0) return;

  int nseg = (int)ceil(fabs(spandeg) / 2.0);                          // ~2 deg per segment
  if(nseg < 1) nseg = 1;

  double step = spandeg / nseg;

  for(int c=0; c<=nseg; c++)
    {
      double deg = startdeg + (step * c);
      double rad = deg * (PI / 180.0);

      double px  = cx + (r * cos(rad));
      double py  = cy + (r * sin(rad));

      if(firstpoint)
        {
          path.MoveTo(px, py);
          firstpoint = false;
        }
       else
        {
          path.LineTo(px, py);
        }
    }
}


static void DrawScrollPill(GRP2DCANVAS* canvas, double a, double b, double c, double d, double formradius)
{
  // Normalize coords (the edge/clip convention can give either order). The bar follows the form: it is rounded only
  // when the form has rounded corners, with the corner radius clamped to half the shorter side so a wide-thin or
  // tall-thin bar stays a valid pill (agg::rounded_rect can misround a wide reversed-Y rect).
  double x1   = __MIN(a, c);
  double x2   = __MAX(a, c);
  double y1   = __MIN(b, d);
  double y2   = __MAX(b, d);
  double half = __MIN((x2 - x1), (y2 - y1)) / 2.0f;
  double r    = (formradius > 0.0f) ? __MIN(formradius, half) : 0.0f;

  canvas->RoundRect(x1, y1, x2, y2, r, true);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         static void UI_SkinCanvas_ProgressBar_DrawRect(GRP2DCANVAS* canvas, double x1, double y1, double x2, double y2, double radius)
* @brief      Draw a filled rect, rounded when radius > 0. The radius is clamped to half the smaller side so a
*             capsule (roundcap) stays valid even when the progress fill is narrower than the bar thickness
*             (agg::rounded_rect does not self-normalize the radius).
* @note       INTERNAL / FILE LOCAL
* @ingroup    USERINTERFACE
*
* ---------------------------------------------------------------------------------------------------------------------*/
static void UI_SkinCanvas_ProgressBar_DrawRect(GRP2DCANVAS* canvas, double x1, double y1, double x2, double y2, double radius)
{
  if(!canvas) return;

  double w = (x2 > x1) ? (x2 - x1) : (x1 - x2);
  double h = (y1 > y2) ? (y1 - y2) : (y2 - y1);

  if((w <= 0.0) || (h <= 0.0)) return;                                                // nothing to draw (e.g. level 0)

  if(radius <= 0.0) { canvas->Rectangle(x1, y1, x2, y2, true); return; }              // square corners

  // If the rect is shorter than 2*radius on the progress axis, a smaller clamped radius would make the cap squarer
  // than the frame's cap and poke outside it. Instead keep the frame radius, extend the rect to a valid size on that
  // axis, and clip to the real band: the cap then follows the SAME circle as the frame and nests exactly inside it.
  double dx2     = x2;
  double dy2     = y2;
  bool   clipped = false;

  if(w < (2.0 * radius)) { dx2 = (x2 >= x1) ? (x1 + (2.0 * radius)) : (x1 - (2.0 * radius)); clipped = true; }
  if(h < (2.0 * radius)) { dy2 = (y2 <= y1) ? (y1 - (2.0 * radius)) : (y1 + (2.0 * radius)); clipped = true; }

  if(!clipped) { canvas->RoundRect(x1, y1, x2, y2, radius, true); return; }

  double ox1 = 0.0, oy1 = 0.0, ox2 = 0.0, oy2 = 0.0;
  canvas->GetClipBox(ox1, oy1, ox2, oy2);

  double cminx = __MIN(ox1, ox2), cmaxx = __MAX(ox1, ox2);
  double cminy = __MIN(oy1, oy2), cmaxy = __MAX(oy1, oy2);
  double bminx = __MIN(x1, x2),   bmaxx = __MAX(x1, x2);
  double bminy = __MIN(y1, y2),   bmaxy = __MAX(y1, y2);

  double iminx = __MAX(cminx, bminx), imaxx = __MIN(cmaxx, bmaxx);
  double iminy = __MAX(cminy, bminy), imaxy = __MIN(cmaxy, bmaxy);

  if((imaxx > iminx) && (imaxy > iminy))
    {
      canvas->SetClipBox(iminx, imaxy, imaxx, iminy);                                 // (left, bottom, right, top)
      canvas->RoundRect(x1, y1, dx2, dy2, radius, true);
      canvas->SetClipBox(ox1, oy1, ox2, oy2);                                         // restore
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         static void UI_SkinCanvas_AppendRoundRectPath(...)
* @brief      Builds a (optionally rounded) rectangle outline into a path with MoveTo/LineTo so it can be gradient-filled.
* @note       INTERNAL / FILE LOCAL. Inputs are already normalized (minx<=maxx, miny<=maxy) and r clamped by the caller.
* ---------------------------------------------------------------------------------------------------------------------*/
static void UI_SkinCanvas_AppendRoundRectPath(GRP2DPATH& path, double minx, double miny, double maxx, double maxy, double r)
{
  if(r <= 0.0)
    {
      path.MoveTo(minx, miny);  path.LineTo(maxx, miny);  path.LineTo(maxx, maxy);  path.LineTo(minx, maxy);  path.Close();
      return;
    }

  double cx[4] = { minx + r, maxx - r, maxx - r, minx + r };                          // corner centers: TL, TR, BR, BL
  double cy[4] = { miny + r, miny + r, maxy - r, maxy - r };
  double a0[4] = { 180.0,    270.0,      0.0,     90.0    };                          // start angle of each quarter arc (deg)

  const int STEPS = 6;
  bool      first = true;

  for(int c = 0; c < 4; c++)
    {
      for(int s = 0; s <= STEPS; s++)
        {
          double ang = (a0[c] + (90.0 * (double)s / (double)STEPS)) * (PI / 180.0);
          double px  = cx[c] + (r * cos(ang));
          double py  = cy[c] + (r * sin(ang));

          if(first) { path.MoveTo(px, py); first = false; }
          else        path.LineTo(px, py);
        }
    }
  path.Close();
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         static void UI_SkinCanvas_ProgressBar_DrawGradientRect(...)
* @brief      Fills a (optionally rounded) rect with a linear gradient along (gx1,gy1)->(gx2,gy2).
* @note       INTERNAL / FILE LOCAL. Used only when gradientcolor is set; the solid DrawRect path is left untouched.
*             At narrow widths the radius is clamped to w/2 (a smaller capsule nested inside the frame, never poking out).
* ---------------------------------------------------------------------------------------------------------------------*/
static void UI_SkinCanvas_ProgressBar_DrawGradientRect(GRP2DCANVAS* canvas, double x1, double y1, double x2, double y2, double radius,
                                                       GRP2DGRADIENTSTOP* stops, double gx1, double gy1, double gx2, double gy2)
{
  if(!canvas || !stops) return;

  double minx = __MIN(x1, x2), maxx = __MAX(x1, x2);
  double miny = __MIN(y1, y2), maxy = __MAX(y1, y2);
  double w    = maxx - minx,   h    = maxy - miny;

  if((w <= 0.0) || (h <= 0.0)) return;                                                // nothing to draw (e.g. level 0)

  double r    = radius;
  double maxr = ((w < h) ? w : h) / 2.0;
  if(r > maxr) r = maxr;

  GRP2DPATH path;
  UI_SkinCanvas_AppendRoundRectPath(path, minx, miny, maxx, maxy, r);

  canvas->PathGradientLinear(path, false, gx1, gy1, gx2, gy2, stops, 2);
  canvas->Path(path, false);                                                          // stroke the same outline with the caller's line color/width (the solid RoundRect did this; the gradient fill does not)
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_SKINCANVAS_REBUILDAREAS::UI_SKINCANVAS_REBUILDAREAS(GRPSCREEN* screen, int viewportindex)
* @brief      Constructor of class
* @ingroup    USERINTERFACE
* 
* @param[in]  screen : 
* @param[in]  viewportindex : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_SKINCANVAS_REBUILDAREAS::UI_SKINCANVAS_REBUILDAREAS(GRPSCREEN* screen, int viewportindex)
{
  Clean();

  SetExcessEdge(0);

  GRPVIEWPORT* viewport = NULL;  

  this->viewportindex = viewportindex;
 
  this->screen = screen;
  if(screen) 
    { 
      viewport = screen->GetViewport(viewportindex);
      if(viewport) 
        {
          canvas = viewport->GetCanvas();
        }
    }      
}

 
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_SKINCANVAS_REBUILDAREAS::~UI_SKINCANVAS_REBUILDAREAS()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    USERINTERFACE
*
* ---------------------------------------------------------------------------------------------------------------------*/
UI_SKINCANVAS_REBUILDAREAS::~UI_SKINCANVAS_REBUILDAREAS()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS_REBUILDAREAS::RebuildAllAreas()
* @brief      Rebuild all areas
* @ingroup    USERINTERFACE
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS_REBUILDAREAS::RebuildAllAreas()
{   
  XDWORD max_z_level = 0;
  XDWORD nareas      = areas.GetSize();
  bool   first       =  false;

  if(!nareas) return true;

  for(XDWORD c=0; c<nareas; c++)
    {
      GRP2DREBUILDAREA* area = areas.Get(c);
      if(area)
        {
          UI_ELEMENT* element = (UI_ELEMENT*)area->GetExtraData();
          if(element)
            {
              max_z_level =  __MAX(max_z_level, element->GetZLevel()); 
            }
        }
    }

  if(!max_z_level) return false;

  for(XDWORD level = max_z_level; level>0; level--)  
    {
      nareas = areas.GetSize();  

      for(int index = nareas-1; index>=0; index--)  
        {
          GRP2DREBUILDAREA* area = areas.Get(index);
          if(area)
            {
              UI_ELEMENT* element = (UI_ELEMENT*)area->GetExtraData();
              if(element)
                {
                  if(element->MustReDraw() || (!element->IsVisible())) 
                    { 
                      if(element->GetZLevel() == level)
                        {       
                          // XTRACE_PRINTCOLOR(XTRACE_COLOR_PURPLE, __L("Del area level [%d] [%s] "), element->GetZLevel(), element->GetName()->Get());
                          GRPBITMAP* bitmap = area->GetBitmap();              
                          if(bitmap) PutBitmapNoAlpha(area->GetXPos(), area->GetYPos(), bitmap);

                          areas.Delete(area);                
                          GEN_DELETE area;  
                        }              
                    }
                }
            }           
        }
    }  

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS_REBUILDAREAS::RebuildAllAreas(UI_LAYOUT* layout)
* @brief      Rebuild all areas
* @ingroup    USERINTERFACE
*
* @param[in]  layout : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS_REBUILDAREAS::RebuildAllAreas(UI_LAYOUT* layout)
{ 
  if(!layout) return false;

  // The stored rebuild areas belong to the canvas, not to the XML layout order.
  // Restoring them by walking the layout tree can restore overlapping translucent
  // elements in the wrong order.  Use the global Z-level aware restoration instead.
  return RebuildAllAreas();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS_REBUILDAREAS::RebuildAllAreas(UI_ELEMENT* element)
* @brief      Rebuild all areas
* @ingroup    USERINTERFACE
*
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS_REBUILDAREAS::RebuildAllAreas(UI_ELEMENT* element)
{ 
  XDWORD max_z_level = 0;
  XDWORD nareas      = areas.GetSize();
  bool   first       = false;

  if(!nareas) return true;

  for(XDWORD c=0; c<nareas; c++)
    {
      GRP2DREBUILDAREA* area = areas.Get(c);
      if(area)
        {
          UI_ELEMENT* element = (UI_ELEMENT*)area->GetExtraData();
          if(element)
            {
              max_z_level =  __MAX(max_z_level, element->GetZLevel()); 
            }
        }
    }

  if(!max_z_level) return false;
  
  for(XDWORD level = max_z_level; level>0; level--)  
    {
      nareas = areas.GetSize();  

      for(int index = (int)nareas - 1; index >= 0; index--)                
        {
          GRP2DREBUILDAREA* area = areas.Get((XDWORD)index);
          if(area)
            {
              UI_ELEMENT* _element = (UI_ELEMENT*)area->GetExtraData();
              if(_element)
                {
                  if(_element == element)
                    {
                      if(_element->GetZLevel() == level)
                        {  
                          if(_element->MustReDraw() || (!_element->IsVisible())) 
                            {                              
                              GRPBITMAP* bitmap = area->GetBitmap();
                              if(bitmap) PutBitmapNoAlpha(area->GetXPos(), area->GetYPos(), bitmap);

                              areas.Delete(area);                
                              GEN_DELETE area;  

                              area = NULL;
                            }              
                        }
                    }
                }
            } 
        }
    } 

  for(XDWORD c=0; c<element->GetComposeElements()->GetSize(); c++)
    {
      UI_ELEMENT* subelement = (UI_ELEMENT*)element->GetComposeElements()->Get(c);
      if(subelement) RebuildAllAreas(subelement);            
    }

  return true;  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS_REBUILDAREAS::CreateRebuildArea(double x, double y, double width, double height, UI_ELEMENT* element)
* @brief      Create rebuild area
* @ingroup    USERINTERFACE
*
* @param[in]  x : 
* @param[in]  y : 
* @param[in]  width : 
* @param[in]  height : 
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS_REBUILDAREAS::CreateRebuildArea(double x, double y, double width, double height, UI_ELEMENT* element)
{  
  return GRP2DREBUILDAREAS::CreateRebuildArea(x, y, width+1, height+1, (void*)element);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPBITMAP* UI_SKINCANVAS_REBUILDAREAS::GetBitmap(double x, double y, double width, double height)
* @brief      Get bitmap
* @ingroup    USERINTERFACE
*
* @param[in]  x : 
* @param[in]  y : 
* @param[in]  width : 
* @param[in]  height : 
* 
* @return     GRPBITMAP* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
GRPBITMAP* UI_SKINCANVAS_REBUILDAREAS::GetBitmap(double x, double y, double width, double height)
{
  if(!canvas) return NULL;

  return canvas->GetBitmap(x, y, width, height);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_SKINCANVAS_REBUILDAREAS::PutBitmapNoAlpha(double x, double y, GRPBITMAP* bitmap)
* @brief      Put bitmap no alpha
* @ingroup    USERINTERFACE
*
* @param[in]  x : 
* @param[in]  y : 
* @param[in]  bitmap : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_SKINCANVAS_REBUILDAREAS::PutBitmapNoAlpha(double x, double y, GRPBITMAP* bitmap)
{
  if(!canvas) return;

  canvas->PutBitmapNoAlpha(x, y, bitmap); 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRP2DREBUILDAREA* UI_SKINCANVAS_REBUILDAREAS::GetRebuildAreaByElement(UI_ELEMENT* element)
* @brief      Get rebuild area by element
* @ingroup    USERINTERFACE
*
* @param[in]  element : 
* 
* @return     GRP2DREBUILDAREA* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
GRP2DREBUILDAREA* UI_SKINCANVAS_REBUILDAREAS::GetRebuildAreaByElement(UI_ELEMENT* element)
{
  for(XDWORD c=0; c<GetRebuildAreas()->GetSize(); c++)
    {
      GRP2DREBUILDAREA* rebuild_area = GetRebuildAreas()->Get(c);
      if(rebuild_area)
        {
          UI_ELEMENT* _element = (UI_ELEMENT*)rebuild_area->GetExtraData();
          if(_element)
            {
              if(_element == element) return rebuild_area;
            }
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_SKINCANVAS_REBUILDAREAS::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_SKINCANVAS_REBUILDAREAS::Clean()
{
  screen          = NULL;
  viewportindex   = 0;
  canvas          = NULL;
}






/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_SKINCANVAS::UI_SKINCANVAS(GRPSCREEN* screen, int viewportindex)
* @brief      Constructor of class
* @ingroup    USERINTERFACE
* 
* @param[in]  screen : 
* @param[in]  viewportindex : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_SKINCANVAS::UI_SKINCANVAS(GRPSCREEN* screen,  int viewportindex) : UI_SKIN(),  UI_SKINCANVAS_REBUILDAREAS(screen)
{ 
  Clean();     

  this->screen        = screen;
  this->viewportindex = viewportindex;
  this->name          = UI_SKINCANVAS_NAME_UNKNOWN;
  this->drawmode      = UI_SKIN_DRAWMODE_CANVAS;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_SKINCANVAS::~UI_SKINCANVAS()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    USERINTERFACE
*
* ---------------------------------------------------------------------------------------------------------------------*/
UI_SKINCANVAS::~UI_SKINCANVAS()    
{ 
  if(!fontpathfile.IsEmpty())
    {
      GEN_USERINTERFACE.DeleteTemporalUnZipFile(fontpathfile);  
    }

  DeleteAllRebuildAreas();

  Clean();                            
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPSCREEN* UI_SKINCANVAS::GetScreen()
* @brief      Get screen
* @ingroup    USERINTERFACE
*
* @return     GRPSCREEN* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
GRPSCREEN* UI_SKINCANVAS::GetScreen()
{
  return screen;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRP2DCANVAS* UI_SKINCANVAS::GetCanvas()
* @brief      Get canvas
* @ingroup    USERINTERFACE
*
* @return     GRP2DCANVAS* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
GRP2DCANVAS* UI_SKINCANVAS::GetCanvas()
{
  if(!screen)                   
    {
      return NULL;
    }

  if(!screen->GetViewport(viewportindex))   
    {
      return NULL;
    }

  return screen->GetViewport(viewportindex)->GetCanvas();
}
	

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::LoadFonts()
* @brief      Load fonts
* @ingroup    USERINTERFACE
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::LoadFonts()
{
  XPATH  xpath;
  bool   status = false; 

  GRP2DCANVAS* canvas = GetCanvas();
  if(!canvas) return false;
  
  if(!rasterfontname.IsEmpty())
    {
      status = canvas->RasterFont_Select(rasterfontname.Get());

    } else status = true;

  if(status)
    {
      if(!vectorfontname.IsEmpty())
        {          
          GEN_XPATHSMANAGER.GetPathOfSection(XPATHSMANAGERSECTIONTYPE_FONTS, xpath);
          xpath.Slash_Add();
          xpath.Add(vectorfontname.Get());

          if(GEN_USERINTERFACE.IsZippedFile())
            {             
              XFILEUNZIP* unzipfile = GEN_USERINTERFACE.GetUnzipFile();
              if(unzipfile)
                {                  
                  XPATH pathnamefilecmp;

                  pathnamefilecmp = APPFLOW_DEFAULT_DIRECTORY_FONTS;
                  pathnamefilecmp.Slash_Add();
                  pathnamefilecmp += vectorfontname;

                  status = unzipfile->DecompressFile(pathnamefilecmp, (*GEN_USERINTERFACE.GetUnzipPathFile()), vectorfontname.Get());   
                  if(status)
                    {  
                      XPATH unzippathfile_tmp;

                      unzippathfile_tmp  = GEN_USERINTERFACE.GetUnzipPathFile()->Get();
                      unzippathfile_tmp += vectorfontname;

                      status = canvas->VectorFont_Load(unzippathfile_tmp);  

                      //GEN_USERINTERFACE.DeleteTemporalUnZipFile(unzippathfile_tmp);  

                      fontpathfile = unzippathfile_tmp;
                    }
                    
                }
            }
           else
            {                            
              status = canvas->VectorFont_Load(xpath);    
            }

        } else status = true;
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::GetFatherSize(UI_ELEMENT* element, double& width, double& height)
* @brief      Get father size
* @ingroup    USERINTERFACE
*
* @param[in]  element : 
* @param[in]  width : 
* @param[in]  height : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::GetFatherSize(UI_ELEMENT* element, double& width, double& height)
{
  GRP2DCANVAS* canvas = GetCanvas();
  if(!canvas) return false;

  width  = 0.0f;
  height = 0.0f;
  
  if(element->GetFather())  
    {
      width  = element->GetFather()->GetBoundaryLine()->width; 
      height = element->GetFather()->GetBoundaryLine()->height; 
    }
   else  
    { 
      width  = canvas->GetWidth()-1;   
      height = canvas->GetHeight()-1;   
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_SKINCANVAS::GetPositionWithoutDefine(double position)
* @brief      Get position without define
* @ingroup    USERINTERFACE
*
* @param[in]  position : 
* 
* @return     double : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
double UI_SKINCANVAS::GetPositionWithoutDefine(double position)
{ 
  switch((int)position)
    {
      case UI_ELEMENT_TYPE_ALIGN_LEFT     : break;
      case UI_ELEMENT_TYPE_ALIGN_RIGHT		: break;
      case UI_ELEMENT_TYPE_ALIGN_UP       : break;
      case UI_ELEMENT_TYPE_ALIGN_DOWN   	: break;
      case UI_ELEMENT_TYPE_ALIGN_CENTER		: break;
                        default       : return position;
    } 
    
  return 0.0f;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::SetAroundFromSubElements(UI_ELEMENT* element)
* @brief      Set around from sub elements
* @ingroup    USERINTERFACE
*
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::SetAroundFromSubElements(UI_ELEMENT* element)
{
  if(!element) return false;
  if(element->GetComposeElements()->IsEmpty()) return false;

  UI_BOUNDARYLINE bondaryline[2];

  bondaryline[1].x      = element->GetXPosition();
  bondaryline[1].y      = element->GetYPosition();
  bondaryline[1].width  = element->GetBoundaryLine()->width;
  bondaryline[1].height = element->GetBoundaryLine()->height;

  for(XDWORD c=0; c<element->GetComposeElements()->GetSize(); c++)
    {
      UI_ELEMENT* subelement = element->GetComposeElements()->Get(c);
      if(subelement)
        {
          bondaryline[0].x       = subelement->GetXPosition();
          bondaryline[0].y       = subelement->GetYPosition();
          bondaryline[0].width   = subelement->GetBoundaryLine()->width;
          bondaryline[0].height  = subelement->GetBoundaryLine()->height;

          bondaryline[1].SetAround(bondaryline[0]);
        }
    }


  for(int c=(int)element->GetComposeElements()->GetSize()-1; c>=0; c--)
    {
      UI_ELEMENT* subelement = element->GetComposeElements()->Get(c);
      if(subelement)
        {
          bondaryline[0].x       = subelement->GetXPosition();
          bondaryline[0].y       = subelement->GetYPosition();
          bondaryline[0].width   = subelement->GetBoundaryLine()->width;
          bondaryline[0].height  = subelement->GetBoundaryLine()->height;

          bondaryline[1].SetAround(bondaryline[0]);
        }
    }
 
  element->SetXPosition(bondaryline[1].x);
  element->SetYPosition(bondaryline[1].y);
  element->GetBoundaryLine()->width  = bondaryline[1].width;
  element->GetBoundaryLine()->height = bondaryline[1].height;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::AddPositionSubElements(UI_ELEMENT* element)
* @brief      Add position sub elements
* @ingroup    USERINTERFACE
*
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::AddPositionSubElements(UI_ELEMENT* element)
{
  if(!element) return false;   

  for(XDWORD d=0; d<element->GetComposeElements()->GetSize(); d++)   
    {
      UI_ELEMENT* subelement = element->GetComposeElements()->Get(d);
      if(subelement) 
        {
          subelement->SetXPosition(subelement->GetXPosition() + element->GetXPosition()); 
          subelement->SetYPosition(subelement->GetYPosition() + element->GetYPosition()); 
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculePosition(UI_ELEMENT* element, double fatherwidth, double fatherheight, bool adjustsizemargin)
* @brief      Calcule position
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* @param[in]  fatherwidth : 
* @param[in]  fatherheight : 
* @param[in]  adjustsizemargin : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculePosition(UI_ELEMENT* element, double fatherwidth, double fatherheight, bool adjustsizemargin)
{ 
  if(!element) 
    {
      return false;
    }

  double x_position = 0.0f;
  double y_position = 0.0f;

  if(element->GetFather())
    {
      x_position = GetPositionWithoutDefine(element->GetFather()->GetXPosition());
      y_position = GetPositionWithoutDefine(element->GetFather()->GetYPosition());     
    }

  if(adjustsizemargin)
    {
      element->GetBoundaryLine()->width  -= (element->GetMargin(UI_ELEMENT_TYPE_ALIGN_LEFT) + element->GetMargin(UI_ELEMENT_TYPE_ALIGN_RIGHT));
      element->GetBoundaryLine()->height -= (element->GetMargin(UI_ELEMENT_TYPE_ALIGN_UP)   + element->GetMargin(UI_ELEMENT_TYPE_ALIGN_DOWN));
    }


  switch((int)element->GetBoundaryLine()->x)
    {
      case UI_ELEMENT_TYPE_ALIGN_LEFT     : break;

      case UI_ELEMENT_TYPE_ALIGN_RIGHT		: x_position += (fatherwidth - element->GetBoundaryLine()->width);                  
                                            break;

      case UI_ELEMENT_TYPE_ALIGN_CENTER   : x_position += (int)round((fatherwidth - element->GetBoundaryLine()->width)/2);                                               
                                            break;

                                default   : x_position += GetPositionWithoutDefine(element->GetBoundaryLine()->x);            
                                            break;
    }

  if(element->GetFather())
    {
      switch((int)element->GetBoundaryLine()->y)
        {
          case UI_ELEMENT_TYPE_ALIGN_UP       : y_position -= fatherheight - element->GetBoundaryLine()->height;                 
                                                break;

          case UI_ELEMENT_TYPE_ALIGN_DOWN   	: break;

          case UI_ELEMENT_TYPE_ALIGN_CENTER		: y_position -= (int)round((fatherheight - element->GetBoundaryLine()->height)/2);  
                                                break;

                                    default   : y_position -= GetPositionWithoutDefine(element->GetBoundaryLine()->y);            
                                                break;
        }
     
    }
   else 
    {
      switch((int)element->GetBoundaryLine()->y)
        {
          case UI_ELEMENT_TYPE_ALIGN_UP       : y_position += element->GetBoundaryLine()->height;                                     
                                                break;

          case UI_ELEMENT_TYPE_ALIGN_DOWN   	: y_position += fatherheight;                                                        
                                                break;

          case UI_ELEMENT_TYPE_ALIGN_CENTER		: y_position += (int)round((fatherheight + element->GetBoundaryLine()->height)/2);  
                                                break;

                                    default   : y_position += GetPositionWithoutDefine(element->GetBoundaryLine()->y);            
                                                break;
        }
    } 

  x_position += element->GetMargin(UI_ELEMENT_TYPE_ALIGN_LEFT);  
  x_position -= element->GetMargin(UI_ELEMENT_TYPE_ALIGN_RIGHT);

  if(element->GetFather())
    {
      y_position += element->GetMargin(UI_ELEMENT_TYPE_ALIGN_UP);          
      y_position -= element->GetMargin(UI_ELEMENT_TYPE_ALIGN_DOWN);      
    }
   else
    {
      y_position -= element->GetMargin(UI_ELEMENT_TYPE_ALIGN_UP);          
      y_position += element->GetMargin(UI_ELEMENT_TYPE_ALIGN_DOWN);      
    }

  element->SetXPosition(x_position);
  element->SetYPosition(y_position);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_SKINCANVAS::GetWidthString(XCHAR* string, XDWORD sizefont)
* @brief      Get with string
* @ingroup    USERINTERFACE
*
* @param[in]  string : 
* @param[in]  sizefont : 
* 
* @return     double : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
double UI_SKINCANVAS::GetWidthString(XCHAR* string,  XDWORD sizefont)
{
  XDWORD width  = 0;
  XDWORD height = 0;

  SetFontSize(sizefont);
  GetFontSize(string, width, height);

  return (double)width;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_SKINCANVAS::GetHeightString(XCHAR* string, XDWORD sizefont)
* @brief      Get height string
* @ingroup    USERINTERFACE
*
* @param[in]  string : 
* @param[in]  sizefont : 
* 
* @return     double : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
double  UI_SKINCANVAS::GetHeightString(XCHAR* string, XDWORD sizefont)
{
  XDWORD width  = 0;
  XDWORD height = 0;

  SetFontSize(sizefont);
  GetFontSize(string, width, height);

  return (double)height;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_Scroll(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line scroll
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* @param[in]  adjustsizemargin : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_Scroll(UI_ELEMENT* element, bool adjustsizemargin)
{
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_Text(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line text
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* @param[in]  adjustsizemargin : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_Text(UI_ELEMENT* element, bool adjustsizemargin)
{
  if(!element) return false;

  UI_ELEMENT_TEXT* element_text = (UI_ELEMENT_TEXT*)element;
  GRP2DCANVAS*       canvas       = GetCanvas();

  if(!canvas) return false;
  
  canvas->Vectorfont_GetConfig()->SetSize(element_text->GetSizeFont());

  XSTRING text;
  if(!element_text->GetText()->IsEmpty()) 
    {
      text = element_text->GetText()->Get();    
    }
   else 
    {
      if(!element_text->GetMaskText()->IsEmpty()) 
        {
          text = element_text->GetMaskText()->Get();
        }
    }

  double fatherwidth  = 0.0f;
  double fatherheight = 0.0f;
  
  GetFatherSize(element, fatherwidth, fatherheight);

  XSTRING string_max = text;
  if(element_text->GetMaxSizeText())
    {
      XDWORD size_max = element_text->GetMaxSizeText();     
      
      if(!size_max) size_max = text.GetSize();
      string_max.Empty();
      for(XDWORD c=0; c<size_max; c++)
        {
          string_max += __L("A");
        }
    }

  switch((int)element->GetBoundaryLine()->width)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO	 :		 		
      case                           0 : element->GetBoundaryLine()->width  = (double)canvas->VectorFont_GetWidth(string_max.Get());    break;
      case UI_ELEMENT_TYPE_ALIGN_MAX   : element->GetBoundaryLine()->width = fatherwidth;                                               break;	    
    }
   
  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO	 :		 		
      case                           0 : element->GetBoundaryLine()->height  = (double)canvas->VectorFont_GetHeight(string_max.Get());  break;
      case UI_ELEMENT_TYPE_ALIGN_MAX   : element->GetBoundaryLine()->height  = fatherheight;                                            break;	    
    }

  return CalculePosition(element, fatherwidth, fatherheight, adjustsizemargin);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_TextBox(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line text box
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* @param[in]  adjustsizemargin : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_TextBox(UI_ELEMENT* element, bool adjustsizemargin)
{  
  UI_ELEMENT_TEXTBOX* element_textbox = (UI_ELEMENT_TEXTBOX*)element;
  if(!element_textbox) return false;

  double fatherwidth  = 0.0f;
  double fatherheight = 0.0f;
  
  GetFatherSize(element, fatherwidth, fatherheight);

  switch((int)element->GetBoundaryLine()->width)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO   :		 		
      case                            0 : 
      case UI_ELEMENT_TYPE_ALIGN_MAX    : return false; 
                                          break;
    }
   
  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO	  :		 		
      case                            0 : 
      case UI_ELEMENT_TYPE_ALIGN_MAX    : return false; 
                                          break;
    }

  return CalculePosition(element, fatherwidth, fatherheight, adjustsizemargin);  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_Image(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line image
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* @param[in]  adjustsizemargin : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_Image(UI_ELEMENT* element, bool adjustsizemargin)
{
  if(!element) return false;
  
  UI_ELEMENT_IMAGE* element_image = (UI_ELEMENT_IMAGE*)element;
  if(!element_image->GetImage()) return false;

  double fatherwidth  = 0.0f;
  double fatherheight = 0.0f;
  
  GetFatherSize(element, fatherwidth, fatherheight);

  switch((int)element->GetBoundaryLine()->width)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO   :		 		
      case                            0 : element->GetBoundaryLine()->width  = (double)element_image->GetImage()->GetWidth();       break;
      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->width = fatherwidth;                                          break;	    
    }
   
  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO	  :		 		
      case                            0 : element->GetBoundaryLine()->height  = (double)element_image->GetImage()->GetHeight();     break;
      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->height  = fatherheight;                                       break;	    
    }

  return CalculePosition(element, fatherwidth, fatherheight, adjustsizemargin);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_Animation(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line animation
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* @param[in]  adjustsizemargin : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_Animation(UI_ELEMENT* element, bool adjustsizemargin)
{
  UI_ELEMENT_ANIMATION* element_animation = (UI_ELEMENT_ANIMATION*)element;
  if(!element_animation) return false;  

  double fatherwidth  = 0.0f;
  double fatherheight = 0.0f;
  
  GetFatherSize(element, fatherwidth, fatherheight);

  switch((int)element->GetBoundaryLine()->width)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO     :		 		
      case                             0  : { double maximgwidth = 0.0f; 

                                              for(XDWORD c=0; c<element_animation->GetComposeElements()->GetSize(); c++)
                                                {
                                                 UI_ELEMENT_IMAGE* element_image = (UI_ELEMENT_IMAGE*)element_animation->GetComposeElements()->Get(c);
                                                 if(element_image) maximgwidth = __MAX((GetPositionWithoutDefine(element_image->GetBoundaryLine()->x) + element_image->GetBoundaryLine()->width), maximgwidth);                                                    
                                                }

                                              element->GetBoundaryLine()->width = maximgwidth;
                                            }
                                            break;

      case UI_ELEMENT_TYPE_ALIGN_MAX      : element->GetBoundaryLine()->width = fatherwidth;                                          
                                            break;	    
    }
   
  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO     :		 		
      case                             0  : { double maximgheight = 0.0f; 

                                              for(XDWORD c=0; c<element_animation->GetComposeElements()->GetSize(); c++)
                                                {
                                                 UI_ELEMENT_IMAGE* element_image = (UI_ELEMENT_IMAGE*)element_animation->GetComposeElements()->Get(c);
                                                 if(element_image) maximgheight = __MAX((GetPositionWithoutDefine(element_image->GetBoundaryLine()->y) + element_image->GetBoundaryLine()->height), maximgheight);                                                    
                                                }

                                              element->GetBoundaryLine()->height = maximgheight;
                                            }
                                            break;

      case UI_ELEMENT_TYPE_ALIGN_MAX      : element->GetBoundaryLine()->height  = fatherheight;                                       
                                            break;	    
    }

  CalculePosition(element, fatherwidth, fatherheight, adjustsizemargin);

  for(XDWORD c=0; c<element_animation->GetComposeElements()->GetSize(); c++)
    {
      UI_ELEMENT_IMAGE* element_image = (UI_ELEMENT_IMAGE*)element_animation->GetComposeElements()->Get(c);
      if(element_image)
        {
          CalculePosition(element_image, element_animation->GetBoundaryLine()->width, element_image->GetBoundaryLine()->height, adjustsizemargin);
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_Option(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line option
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* @param[in]  adjustsizemargin : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_Option(UI_ELEMENT* element, bool adjustsizemargin)
{
  UI_ELEMENT_OPTION* element_option = (UI_ELEMENT_OPTION*)element;
  if(!element_option) return false;  

  double fatherwidth  = 0.0f;
  double fatherheight = 0.0f;
  
  GetFatherSize(element, fatherwidth, fatherheight);

  UI_ELEMENT_ANIMATION* element_animation = (UI_ELEMENT_ANIMATION*)element_option->Get_UIAnimation();
  UI_ELEMENT_TEXT*      element_text      = (UI_ELEMENT_TEXT*)element_option->Get_UIText();

  double maximgwidth  = 0.0f; 
  double maximgheight = 0.0f; 

  if(element_animation || element_text)
    {
      switch((int)element->GetBoundaryLine()->width)
        {
          case UI_ELEMENT_TYPE_ALIGN_AUTO   :		 		
          case                           0  : { switch(element_option->GetAllocationTextType())
                                                  {                                            
                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_RIGHT   : 
                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_LEFT    : if(element_animation) maximgwidth += element_animation->GetBoundaryLine()->width;  
                                                                                                          if(element_text)      maximgwidth += element_text->GetBoundaryLine()->width;  
                                                                                                          break;

                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_NONE    :                                                                  
                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_CENTER  : 
                                                                                              default   : if(element_animation) maximgwidth = __MAX(maximgwidth, element_animation->GetBoundaryLine()->width);                                                                                                    
                                                                                                          if(element_text)      maximgwidth = __MAX(maximgwidth, element_text->GetBoundaryLine()->width);                                                                                                  
                                                                                                          break; 
                                                  } 
                                     
                                                element->GetBoundaryLine()->width = maximgwidth;
                                              }
                                              break;

          case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->width = fatherwidth;                                          
                                              break;	    
        }

      switch((int)element->GetBoundaryLine()->height)
        {
          case UI_ELEMENT_TYPE_ALIGN_AUTO   :		 		
          case                           0  : { switch(element_option->GetAllocationTextType())
                                                  {                                           
                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_UP      : 
                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_DOWN    : if(element_animation) maximgheight += element_animation->GetBoundaryLine()->height;  
                                                                                                          if(element_text)      maximgheight += element_text->GetBoundaryLine()->height;  
                                                                                                          break;

                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_NONE    : 
                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_CENTER  : 
                                                                                              default   : if(element_animation) maximgheight = __MAX(maximgheight, element_animation->GetBoundaryLine()->height);                                                                                                    
                                                                                                          if(element_text)      maximgheight = __MAX(maximgheight, element_text->GetBoundaryLine()->height);                                                                                                  
                                                                                                          break; 
                                                  }                                      
                                                element->GetBoundaryLine()->height = maximgheight;
                                              }
                                              break;

          case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->height  = fatherheight;                                       
                                              break;	    
        }
    }
  
  CalculePosition(element_option, fatherwidth, fatherheight, adjustsizemargin);

  if(element_animation) CalculePosition(element_animation, element_option->GetBoundaryLine()->width, element_option->GetBoundaryLine()->height,  adjustsizemargin);
  if(element_text)      CalculePosition(element_text     , element_option->GetBoundaryLine()->width, element_option->GetBoundaryLine()->height,  adjustsizemargin);  

  switch(element_option->GetAllocationTextType())
    {
      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_NONE    : break;

      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_DOWN    : if(element_animation && element_text) element_animation->SetYPosition(element_animation->GetYPosition() - element_text->GetBoundaryLine()->height);     break;
      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_UP      : if(element_animation && element_text) element_text->SetYPosition(element_text->GetYPosition() - element_animation->GetBoundaryLine()->height);          break;                                                           
      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_RIGHT   : if(element_animation && element_text) element_text->SetXPosition(element_text->GetXPosition() + element_animation->GetBoundaryLine()->width);           break;
      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_LEFT    : if(element_animation && element_text) element_animation->SetXPosition(element_animation->GetXPosition() + element_text->GetBoundaryLine()->width);      break;
      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_CENTER  : break; 
    } 

  if(element_animation)
    {
      for(XDWORD c=0; c<element_animation->GetComposeElements()->GetSize(); c++)
        {
          UI_ELEMENT* subelement = element_animation->GetComposeElements()->Get(c);
          if(subelement) CalculePosition(subelement, element_animation->GetBoundaryLine()->width, element_animation->GetBoundaryLine()->height, adjustsizemargin);         
        } 
    }

  SetAroundFromSubElements(element_option);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_MultiOption(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line multi option
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* @param[in]  adjustsizemargin : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_MultiOption(UI_ELEMENT* element, bool adjustsizemargin)
{
  UI_ELEMENT_MULTIOPTION* element_multioption = (UI_ELEMENT_MULTIOPTION*)element;
  if(!element_multioption) return false;   

  double fatherwidth  = 0.0f;
  double fatherheight = 0.0f;
  
  GetFatherSize(element, fatherwidth, fatherheight);

  switch((int)element->GetBoundaryLine()->width)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO   :		 		
      case                          0   : { double maximgwidth = 0.0f; 

                                            for(XDWORD c=0; c<element_multioption->GetComposeElements()->GetSize(); c++)
                                              {
                                               UI_ELEMENT* subelement = (UI_ELEMENT*)element->GetComposeElements()->Get(c);
                                               if(subelement) maximgwidth = __MAX((GetPositionWithoutDefine(subelement->GetBoundaryLine()->x) + subelement->GetBoundaryLine()->width), maximgwidth);                                                    
                                              }

                                            element->GetBoundaryLine()->width = maximgwidth;
                                          }
                                          break;

      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->width = fatherwidth;                                          
                                          break;	    
    }
   
  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO	  :		 		
      case                          0   : { double maximgheight = 0.0f; 

                                            for(XDWORD c=0; c<element->GetComposeElements()->GetSize(); c++)
                                              {
                                               UI_ELEMENT* subelement = (UI_ELEMENT*)element->GetComposeElements()->Get(c);
                                               if(subelement) maximgheight = __MAX((GetPositionWithoutDefine(subelement->GetBoundaryLine()->y) + subelement->GetBoundaryLine()->height), maximgheight);                                                    
                                              }

                                            element->GetBoundaryLine()->height = maximgheight;
                                          }
                                          break;

      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->height  = fatherheight;                                       
                                          break;	    
    }

  CalculePosition(element_multioption, fatherwidth, fatherheight, adjustsizemargin);
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_Button(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line button
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* @param[in]  adjustsizemargin : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_Button(UI_ELEMENT* element, bool adjustsizemargin)
{
  return CalculateBoundaryLine_Option(element, adjustsizemargin);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_CheckBox(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line check box
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* @param[in]  adjustsizemargin : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_CheckBox(UI_ELEMENT* element, bool adjustsizemargin)
{
  return CalculateBoundaryLine_Option(element, adjustsizemargin);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_EditText(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line edit text
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* @param[in]  adjustsizemargin : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_EditText(UI_ELEMENT* element, bool adjustsizemargin)
{  
  if(!element) return false;

  UI_ELEMENT_EDITTEXT* element_edittext = (UI_ELEMENT_EDITTEXT*)element;
  GRP2DCANVAS*           canvas           = GetCanvas();

  if(!canvas) return false;
  
  canvas->Vectorfont_GetConfig()->SetSize(element_edittext->GetSizeFont());

  XSTRING text;
  if(!element_edittext->GetText()->IsEmpty()) text = element_edittext->GetText()->Get();
    else if(!element_edittext->GetMaskText()->IsEmpty()) text = element_edittext->GetMaskText()->Get();

  double fatherwidth  = 0.0f;
  double fatherheight = 0.0f;
  
  GetFatherSize(element, fatherwidth, fatherheight);

  XSTRING string_max;
  XDWORD  size_max   =  element_edittext->GetMaxSizeText();
  
  if(!size_max) size_max = text.GetSize();
  for(XDWORD c=0; c<size_max; c++)
    {
      string_max += __L("A");
    }

  switch((int)element->GetBoundaryLine()->width)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO :		 		
      case                          0 : element->GetBoundaryLine()->width  = (double)canvas->VectorFont_GetWidth(string_max.Get());     break;
      case UI_ELEMENT_TYPE_ALIGN_MAX  : element->GetBoundaryLine()->width = fatherwidth;                                                break;	    
    }
   
  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO :		 		
      case                          0 : element->GetBoundaryLine()->height  = (double)canvas->VectorFont_GetHeight(string_max.Get());   break;
      case UI_ELEMENT_TYPE_ALIGN_MAX  : element->GetBoundaryLine()->height  = fatherheight;                                             break;	    
    }

  return CalculePosition(element, fatherwidth, fatherheight, adjustsizemargin);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_Form(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line form
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* @param[in]  adjustsizemargin : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_Form(UI_ELEMENT* element, bool adjustsizemargin)
{
  UI_ELEMENT_FORM* element_form = (UI_ELEMENT_FORM*)element;
  if(!element_form) return false;   

  double fatherwidth  = 0.0f;
  double fatherheight = 0.0f;
  
  GetFatherSize(element, fatherwidth, fatherheight);

  switch((int)element->GetBoundaryLine()->width)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO	  :		 		
      case                           0  : { double maximgwidth = 0.0f; 

                                            for(XDWORD c=0; c<element_form->GetComposeElements()->GetSize(); c++)
                                              {
                                               UI_ELEMENT* subelement = (UI_ELEMENT*)element->GetComposeElements()->Get(c);
                                               if(subelement) maximgwidth = __MAX((GetPositionWithoutDefine(subelement->GetBoundaryLine()->x) + subelement->GetBoundaryLine()->width), maximgwidth);                                                    
                                              }

                                            element->GetBoundaryLine()->width = maximgwidth;
                                          }
                                          break;

      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->width = fatherwidth;                                          
                                          break;	    
    }
   
  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO	  :		 		
      case                          0   : { double maximgheight = 0.0f; 

                                            for(XDWORD c=0; c<element->GetComposeElements()->GetSize(); c++)
                                              {
                                               UI_ELEMENT* subelement = (UI_ELEMENT*)element->GetComposeElements()->Get(c);
                                               if(subelement) maximgheight = __MAX((GetPositionWithoutDefine(subelement->GetBoundaryLine()->y) + subelement->GetBoundaryLine()->height), maximgheight);                                                    
                                              }

                                            element->GetBoundaryLine()->height = maximgheight;
                                          }
                                          break;

      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->height  = fatherheight;                                       
                                          break;	    
    }

  CalculePosition(element_form, fatherwidth, fatherheight, adjustsizemargin);
  
  AddPositionSubElements(element_form);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_Menu(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line menu
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* @param[in]  adjustsizemargin : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_Menu(UI_ELEMENT* element, bool adjustsizemargin)
{  
  return CalculateBoundaryLine_Form(element, adjustsizemargin);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_ListBox(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line list box
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* @param[in]  adjustsizemargin : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_ListBox(UI_ELEMENT* element, bool adjustsizemargin)
{  
  if(!element) return false;

  UI_ELEMENT_LISTBOX* element_listbox  = (UI_ELEMENT_LISTBOX*)element;
  GRP2DCANVAS*          canvas           = GetCanvas();

  if(!canvas) return false;
  
  canvas->Vectorfont_GetConfig()->SetSize(element_listbox->GetSizeFont());

  XSTRING text;
  if(!element_listbox->GetText()->IsEmpty()) text = element_listbox->GetText()->Get();
    else if(!element_listbox->GetMaskText()->IsEmpty()) text = element_listbox->GetMaskText()->Get();

  double fatherwidth  = 0.0f;
  double fatherheight = 0.0f;
  
  GetFatherSize(element, fatherwidth, fatherheight);

  XSTRING string_max;
  XDWORD  size_max   =  element_listbox->GetMaxSizeText();
  
  if(!size_max) size_max = text.GetSize();
  for(XDWORD c=0; c<size_max; c++)
    {
      string_max += __L("A");
    }

  switch((int)element->GetBoundaryLine()->width)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO :		 		
      case                          0 : element->GetBoundaryLine()->width  = (double)canvas->VectorFont_GetWidth(string_max.Get());     break;
      case UI_ELEMENT_TYPE_ALIGN_MAX  : element->GetBoundaryLine()->width = fatherwidth;                                                break;	    
    }
   
  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO	:		 		
      case                          0 : element->GetBoundaryLine()->height  = (double)canvas->VectorFont_GetHeight(string_max.Get());   break;
      case UI_ELEMENT_TYPE_ALIGN_MAX  : element->GetBoundaryLine()->height  = fatherheight;                                             break;	    
    }

  return CalculePosition(element, fatherwidth, fatherheight, adjustsizemargin);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_ProgressBar(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line progress bar
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* @param[in]  adjustsizemargin : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_ProgressBar(UI_ELEMENT* element, bool adjustsizemargin)
{
  UI_ELEMENT_PROGRESSBAR* element_progressbar = (UI_ELEMENT_PROGRESSBAR*)element;
  if(!element_progressbar) return false;  

  double fatherwidth  = 0.0f;
  double fatherheight = 0.0f;
  
  GetFatherSize(element, fatherwidth, fatherheight);

  UI_ELEMENT*           element_progressrect  = element_progressbar->GetProgressRect();
  UI_ELEMENT_ANIMATION* element_animation     = (UI_ELEMENT_ANIMATION*)element_progressbar->Get_UIAnimation();
  UI_ELEMENT_TEXT*      element_text          = (UI_ELEMENT_TEXT*)element_progressbar->Get_UIText();

  double maximgwidth  = 0.0f; 
  double maximgheight = 0.0f; 

  if(element_animation || element_text || element_progressrect)
    {
      switch((int)element->GetBoundaryLine()->width)
        {
          case UI_ELEMENT_TYPE_ALIGN_AUTO   :		 		
          case                           0  : { switch(element_progressbar->GetAllocationTextType())
                                                  {                                            
                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_RIGHT   : 
                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_LEFT    : if(element_progressrect)  maximgwidth += element_progressrect->GetBoundaryLine()->width;  
                                                                                                          if(element_animation)     maximgwidth += element_animation->GetBoundaryLine()->width;  
                                                                                                          if(element_text)          maximgwidth += element_text->GetBoundaryLine()->width;  
                                                                                                          break;

                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_NONE    :                                                                  
                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_CENTER  : 
                                                                                              default   : if(element_progressrect)  maximgwidth = __MAX(maximgwidth, element_progressrect->GetBoundaryLine()->width);  
                                                                                                          if(element_animation)     maximgwidth = __MAX(maximgwidth, element_animation->GetBoundaryLine()->width);                                                                                                    
                                                                                                          if(element_text)          maximgwidth = __MAX(maximgwidth, element_text->GetBoundaryLine()->width);                                                                                                  
                                                                                                          break; 
                                                  } 
                                     
                                                element->GetBoundaryLine()->width = maximgwidth;
                                              }
                                              break;

          case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->width = fatherwidth;                                          
                                              break;	    
        }

      switch((int)element->GetBoundaryLine()->height)
        {
          case UI_ELEMENT_TYPE_ALIGN_AUTO   :		 		
          case                           0  : { switch(element_progressbar->GetAllocationTextType())
                                                  {                                           
                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_UP      : 
                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_DOWN    : if(element_progressrect)  maximgheight += element_progressrect->GetBoundaryLine()->height; 
                                                                                                          if(element_animation)     maximgheight += element_animation->GetBoundaryLine()->height;  
                                                                                                          if(element_text)          maximgheight += element_text->GetBoundaryLine()->height;  
                                                                                                          break;

                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_NONE    : 
                                                    case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_CENTER  : 
                                                                                              default   : if(element_progressrect)  maximgheight = __MAX(maximgheight, element_progressrect->GetBoundaryLine()->height); 
                                                                                                          if(element_animation)     maximgheight = __MAX(maximgheight, element_animation->GetBoundaryLine()->height);                                                                                                    
                                                                                                          if(element_text)          maximgheight = __MAX(maximgheight, element_text->GetBoundaryLine()->height);                                                                                                  
                                                                                                          break; 
                                                  }                                      
                                                element->GetBoundaryLine()->height = maximgheight;
                                              }
                                              break;

          case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->height  = fatherheight;                                       
                                              break;	    
        }
    }
  
  CalculePosition(element_progressbar, fatherwidth, fatherheight, adjustsizemargin);

  if(element_progressrect)  CalculePosition(element_progressrect  , element_progressbar->GetBoundaryLine()->width, element_progressbar->GetBoundaryLine()->height, adjustsizemargin);
  if(element_animation)     CalculePosition(element_animation     , element_progressbar->GetBoundaryLine()->width, element_progressbar->GetBoundaryLine()->height, adjustsizemargin);
  if(element_text)          CalculePosition(element_text          , element_progressbar->GetBoundaryLine()->width, element_progressbar->GetBoundaryLine()->height, adjustsizemargin);  


  switch(element_progressbar->GetAllocationTextType())
    {
      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_NONE    : break;

      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_DOWN    : if(element_animation && element_text)     element_animation->SetYPosition(element_animation->GetYPosition() - element_text->GetBoundaryLine()->height);
                                                            if(element_progressrect && element_text)  element_progressrect->SetYPosition(element_progressrect->GetYPosition() - (element_text->GetBoundaryLine()->height + 6));
                                                            if(element_progressrect && element_text)  element_text->SetXPosition(element_progressrect->GetXPosition() + ((element_progressrect->GetBoundaryLine()->width - element_text->GetBoundaryLine()->width) / 2.0));   // horizontal center on the bar
                                                            break;

      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_UP      : if(element_animation && element_text)     element_text->SetYPosition(element_text->GetYPosition() - element_animation->GetBoundaryLine()->height);
                                                            if(element_progressrect && element_text)  element_text->SetYPosition(element_text->GetYPosition() - (element_progressrect->GetBoundaryLine()->height + 6));
                                                            if(element_progressrect && element_text)  element_text->SetXPosition(element_progressrect->GetXPosition() + ((element_progressrect->GetBoundaryLine()->width - element_text->GetBoundaryLine()->width)/2));   // horizontal center on the bar
                                                            break;

      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_RIGHT   : if(element_progressrect && element_text)
                                                              {
                                                                element_text->SetXPosition(element_progressrect->GetXPosition() + element_progressrect->GetBoundaryLine()->width + 6);                                              // right of the bar (+gap), absolute not incremental
                                                                element_text->SetYPosition(element_progressrect->GetYPosition() - ((element_progressrect->GetBoundaryLine()->height - element_text->GetBoundaryLine()->height)/2));   // vertical center on the bar
                                                              }
                                                            break;

      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_LEFT    : if(element_animation && element_text)     element_animation->SetXPosition(element_animation->GetXPosition() + element_text->GetBoundaryLine()->width);
                                                            if(element_progressrect && element_text)  element_progressrect->SetXPosition(element_progressrect->GetXPosition() + element_text->GetBoundaryLine()->width);
                                                            if(element_progressrect && element_text)  element_text->SetYPosition(element_progressrect->GetYPosition() - ((element_progressrect->GetBoundaryLine()->height - element_text->GetBoundaryLine()->height)/2));   // vertical center on the bar
                                                            break;

      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_CENTER  : if(element_progressrect && element_text)  
                                                              {
                                                                element_text->SetXPosition(element_progressrect->GetXPosition() + ((element_progressrect->GetBoundaryLine()->width  - element_text->GetBoundaryLine()->width)/2));    
                                                                element_text->SetYPosition(element_progressrect->GetYPosition() - ((element_progressrect->GetBoundaryLine()->height - element_text->GetBoundaryLine()->height)/2));             
                                                              }
                                                            break; 
    } 

  if(element_animation)
    {
      for(XDWORD c=0; c<element_animation->GetComposeElements()->GetSize(); c++)
        {
          UI_ELEMENT* subelement = element_animation->GetComposeElements()->Get(c);
          if(subelement) CalculePosition(subelement, element_animation->GetBoundaryLine()->width, element_animation->GetBoundaryLine()->height, adjustsizemargin);         
        } 
    }

  SetAroundFromSubElements(element_progressbar);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_ProgressRadial(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line radial progress
* @ingroup    USERINTERFACE
*
* @param[in]  element :
* @param[in]  adjustsizemargin :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_ProgressRadial(UI_ELEMENT* element, bool adjustsizemargin)
{
  UI_ELEMENT_PROGRESS_RADIAL* element_progress = (UI_ELEMENT_PROGRESS_RADIAL*)element;
  if(!element_progress) return false;

  double fatherwidth  = 0.0f;
  double fatherheight = 0.0f;

  GetFatherSize(element, fatherwidth, fatherheight);

  UI_ELEMENT_TEXT* element_text = (UI_ELEMENT_TEXT*)element_progress->Get_UIText();

  // A radial progress element has no intrinsic content size. AUTO/0 falls back to a square using the resolved opposite side
  // (or the father size when both are unset). MAX resolves to the father size on that axis.
  switch((int)element->GetBoundaryLine()->width)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO   :
      case                           0  : element->GetBoundaryLine()->width  = (element->GetBoundaryLine()->height > 0) ? element->GetBoundaryLine()->height : fatherwidth;    break;
      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->width  = fatherwidth;                                                                                    break;
    }

  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO   :
      case                           0  : element->GetBoundaryLine()->height = (element->GetBoundaryLine()->width > 0) ? element->GetBoundaryLine()->width : fatherheight;     break;
      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->height = fatherheight;                                                                                   break;
    }

  CalculePosition(element_progress, fatherwidth, fatherheight, adjustsizemargin);

  // Center the caption inside the progress box (the child <text> is expected to use xpos="center" ypos="center").
  if(element_text) CalculePosition(element_text, element_progress->GetBoundaryLine()->width, element_progress->GetBoundaryLine()->height, adjustsizemargin);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_ProgressImage(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line progress image
* @ingroup    USERINTERFACE
*
* @param[in]  element :
* @param[in]  adjustsizemargin :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         static void UI_SkinCanvas_ProgressImage_Layout(...)
* @brief      Computes the ProgressImage caption layout for the current allocationtext.
*
*             Outputs are in top-left relative coordinates (0,0 = boundary top-left, y down). No element members are
*             stored: both CalculateBoundaryLine_ProgressImage and Draw_ProgressImage call this helper so the boundary,
*             the image draw position and the caption position always stay in sync.
*
*             For RIGHT/UP and CENTER/NONE the image offset is (0,0), so the image keeps its current position and only
*             the caption (and boundary) move; LEFT/DOWN shift the image to make room for the caption.
*
* --------------------------------------------------------------------------------------------------------------------*/
static void UI_SkinCanvas_ProgressImage_Layout(int allocation, double imgw, double imgh, double textw, double texth, double gap,
                                               double& bw, double& bh, double& ox, double& oy, double& tx, double& ty)
{
  bw = imgw;  bh = imgh;                                                  // defaults: image only
  ox = 0.0;   oy = 0.0;
  tx = 0.0;   ty = 0.0;

  if(textw <= 0.0 && texth <= 0.0) return;                               // no caption

  switch(allocation)
    {
      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_RIGHT  : bw = imgw + gap + textw;  bh = __MAX(imgh, texth);
                                                           ox = 0.0;               oy = (bh - imgh) / 2.0;
                                                           tx = imgw + gap;        ty = (bh - texth) / 2.0;
                                                           break;

      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_LEFT   : bw = imgw + gap + textw;  bh = __MAX(imgh, texth);
                                                           tx = 0.0;               ty = (bh - texth) / 2.0;
                                                           ox = textw + gap;       oy = (bh - imgh) / 2.0;
                                                           break;

      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_DOWN   : bw = __MAX(imgw, textw);  bh = imgh + gap + texth;
                                                           ox = (bw - imgw) / 2.0; oy = 0.0;
                                                           tx = (bw - textw) / 2.0;ty = imgh + gap;
                                                           break;

      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_UP     : bw = __MAX(imgw, textw);  bh = imgh + gap + texth;
                                                           tx = (bw - textw) / 2.0;ty = 0.0;
                                                           ox = (bw - imgw) / 2.0; oy = texth + gap;
                                                           break;

      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_CENTER : bw = imgw;  bh = imgh;
                                                           ox = 0.0;               oy = 0.0;
                                                           tx = (imgw - textw) / 2.0;
                                                           ty = (imgh - texth) / 2.0;
                                                           break;

      default                                            : break;        // NONE: image only, caption keeps its own xpos/ypos
    }
}


bool UI_SKINCANVAS::CalculateBoundaryLine_ProgressImage(UI_ELEMENT* element, bool adjustsizemargin)
{
  UI_ELEMENT_PROGRESS_IMAGE* element_progressimage = (UI_ELEMENT_PROGRESS_IMAGE*)element;
  if(!element_progressimage) return false;

  double fatherwidth  = 0.0f;
  double fatherheight = 0.0f;

  GetFatherSize(element, fatherwidth, fatherheight);

  UI_ELEMENT_TEXT* element_text = (UI_ELEMENT_TEXT*)element_progressimage->Get_UIText();

  GRPBITMAP*       ref          = element_progressimage->GetImageFull() ? element_progressimage->GetImageFull() : element_progressimage->GetImageEmpty();
  double           imgwidth     = ref ? (double)ref->GetWidth()  : 0.0;
  double           imgheight    = ref ? (double)ref->GetHeight() : 0.0;

  int    allocation = (int)element_progressimage->GetAllocationTextType();
  double textw      = element_text ? element_text->GetBoundaryLine()->width  : 0.0;
  double texth      = element_text ? element_text->GetBoundaryLine()->height : 0.0;

  // Reserve the width of the widest percent value ("100%", 4 chars). The caption is sized from the CURRENT value, so a
  // boundary built at e.g. "91%" (3 chars) would clip the extra digit at level 100; reserving 4 chars avoids that.
  if(element_text && (textw > 0.0))
    {
      XDWORD nchars = element_text->GetText()->GetSize();
      if(nchars > 0)
        {
          double maxw = (textw / (double)nchars) * 4.0;
          if(maxw > textw) textw = maxw;
        }
    }

  double bw, bh, ox, oy, tx, ty;
  UI_SkinCanvas_ProgressImage_Layout(allocation, imgwidth, imgheight, textw, texth, 6.0, bw, bh, ox, oy, tx, ty);

  switch((int)element->GetBoundaryLine()->width)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO   :
      case                           0  : element->GetBoundaryLine()->width  = bw;           break;
      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->width  = fatherwidth;  break;
    }

  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO   :
      case                           0  : element->GetBoundaryLine()->height = bh;           break;
      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->height = fatherheight; break;
    }

  CalculePosition(element_progressimage, fatherwidth, fatherheight, adjustsizemargin);

  if(element_text)
    {
      if(allocation == UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_NONE)
        {
          CalculePosition(element_text, element_progressimage->GetBoundaryLine()->width, element_progressimage->GetBoundaryLine()->height, adjustsizemargin);   // no allocation: caption keeps its own xpos/ypos
        }
       else
        {
          double bheight = element_progressimage->GetBoundaryLine()->height;                                                   // place caption per allocationtext (y = BOTTOM edge of the text)
          element_text->SetXPosition(element_progressimage->GetXPosition() + tx);
          element_text->SetYPosition((element_progressimage->GetYPosition() - bheight) + ty + texth);
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::SetElementPosition(UI_ELEMENT* element, double x_position, double y_position)
* @brief      Set element position
* @ingroup    USERINTERFACE
*
* @param[in]  element : 
* @param[in]  x_position : 
* @param[in]  y_position : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::SetElementPosition(UI_ELEMENT* element, double x_position, double y_position)
{
  if(!element) return false;

  element->GetBoundaryLine()->x = x_position;
  element->GetBoundaryLine()->y = y_position;

  bool status = CalculeBoundaryLine_AllElements(element);

  if(dynamic_cast<UI_ELEMENT_FORM*>(element))                                                                                                                                                                                                                                          
    {                                                     
      UI_ELEMENT_FORM* element_form = (UI_ELEMENT_FORM*)element;

      element_form->GetVisibleRect()->x = element_form->GetXPosition();
      element_form->GetVisibleRect()->y = element_form->GetYPosition();
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::Draw_Scroll(UI_ELEMENT* element)
* @brief      Draw scroll
* @ingroup    USERINTERFACE
*
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_Scroll(UI_ELEMENT* element)  
{
  return true;
}
  

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::Draw_Text(UI_ELEMENT* element)
* @brief      Draw text
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_Text(UI_ELEMENT* element)  
{
  if(!element) return false;  

  UI_ELEMENT_TEXT*    element_text  = (UI_ELEMENT_TEXT*)element;
  GRP2DCANVAS*          canvas        = GetCanvas();  
  double              x_position    = 0.0f;
  double              y_position    = 0.0f;  
  XRECT               clip_rect;                       
  
  if(!canvas) return false;
 
  PreDrawFunction(element, canvas, clip_rect, x_position, y_position);
      
  if(element->MustReDraw()) 
    {
      GRP2DCOLOR_RGBA8  color(element->GetColor()->GetRed(),
                              element->GetColor()->GetGreen(),
                              element->GetColor()->GetBlue(),
                              element->GetColor()->GetAlpha());
                         
      
      canvas->Vectorfont_GetConfig()->SetColor(&color);
      canvas->Vectorfont_GetConfig()->SetSize(element_text->GetSizeFont());  
      canvas->VectorFont_Print(x_position, y_position, element_text->GetText()->Get());
    }

  PostDrawFunction(element, canvas, clip_rect, x_position, y_position);
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::Draw_TextBox(UI_ELEMENT* element)
* @brief      Draw text box
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_TextBox(UI_ELEMENT* element)  
{ 
  if(!element) return false;  

  UI_ELEMENT_TEXTBOX* element_textbox = (UI_ELEMENT_TEXTBOX*)element;
  GRP2DCANVAS*          canvas          = GetCanvas();  
  double              x_position      = 0.0f;
  double              y_position      = 0.0f;  
  XRECT               clip_rect;                       
  
  if(!canvas) return false;
 
  PreDrawFunction(element, canvas, clip_rect, x_position, y_position);

  if(element->MustReDraw()) 
    {
      GRP2DCOLOR_RGBA8                color(element->GetColor()->GetRed(),
                                            element->GetColor()->GetGreen(),
                                            element->GetColor()->GetBlue(),
                                            element->GetColor()->GetAlpha());
      XVECTOR<UI_SKIN_TEXTBOX_PART*>  parts;
                                     
      canvas->Vectorfont_GetConfig()->SetColor(&color);
      canvas->Vectorfont_GetConfig()->SetSize(element_textbox->GetSizeFont());  

      TextBox_GenerateLines(element_textbox, canvas, x_position, y_position, parts);

      for(XDWORD c=0; c<parts.GetSize(); c++)
        {
          UI_SKIN_TEXTBOX_PART* textbox_part = parts.Get(c);
          if(textbox_part)
            {
              if(textbox_part->GetImage())
                {
                  // inline image: bottom on the text baseline (drawn at ypos - height)
                  canvas->PutBitmapAlpha((double)textbox_part->GetXPos(),
                                         (double)textbox_part->GetYPos() - (double)textbox_part->GetHeight(),
                                         textbox_part->GetImage(), 100.0f);
                }
               else
                {
                  GRP2DCOLOR_RGBA8 color_part(textbox_part->GetColor()->GetRed(),
                                              textbox_part->GetColor()->GetGreen(),
                                              textbox_part->GetColor()->GetBlue(),
                                              textbox_part->GetColor()->GetAlpha());

                  canvas->Vectorfont_GetConfig()->SetColor(&color_part);

                  canvas->VectorFont_Printf(textbox_part->GetXPos(), textbox_part->GetYPos(), textbox_part->GetText()->Get());   


                  #ifdef USERINTERFACE_DEBUG 
                  GRP2DCOLOR_RGBA8  color_debug(255, 0, 255);

                  int width   = (int)canvas->VectorFont_GetWidth(textbox_part->GetText()->Get());
                  int height  = (int)canvas->VectorFont_GetHeight(__L("A"));

                  canvas->SetLineWidth(1.0f);
                  canvas->SetLineColor(&color_debug);
                  canvas->Rectangle(textbox_part->GetXPos() ,  
                                    textbox_part->GetYPos() ,
                                    textbox_part->GetXPos() + width , 
                                    textbox_part->GetYPos() - height);             
                  #endif  
                }
            }
        }     
   
      parts.DeleteContents();
      parts.DeleteAll();
    }

  PostDrawFunction(element, canvas, clip_rect, x_position, y_position);
  
  return true;
}
  

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::Draw_Image(UI_ELEMENT* element)
* @brief      Draw image
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_Image(UI_ELEMENT* element)  
{
  if(!element) return false;
 
  UI_ELEMENT_IMAGE*   element_image = (UI_ELEMENT_IMAGE*)element;
  GRP2DCANVAS*          canvas        = GetCanvas(); 
  double              x_position    = 0.0f;
  double              y_position    = 0.0f;
  XRECT               clip_rect;                       

  if(!element_image->GetImage()) return false; 
  if(!canvas) return false;

  PreDrawFunction(element, canvas, clip_rect, x_position, y_position);

  if(element->MustReDraw()) 
    {
      canvas->PutBitmapAlpha(x_position , 
                             y_position - element_image->GetImage()->GetHeight(), 
                             element_image->GetImage(), element_image->GetAlpha());
    }

  PostDrawFunction(element, canvas, clip_rect, x_position, y_position);
    
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::Draw_Animation(UI_ELEMENT* element)
* @brief      Draw animation
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_Animation(UI_ELEMENT* element)  
{
  if(!element) return false;

  UI_ELEMENT_ANIMATION* element_animation  = (UI_ELEMENT_ANIMATION*)element;  
  UI_ELEMENT_IMAGE*     element_image      = (UI_ELEMENT_IMAGE*)element_animation->GetComposeElements()->Get(element_animation->IndexImage_Get());
  double                x_position         = 0.0f;
  double                y_position         = 0.0f;  
  GRP2DCANVAS*            canvas             = GetCanvas();    
  XRECT                 clip_rect;          
  bool                  redraw             = false;

  if(!element_animation)  return false;
  if(!element_image)      return false;

  PreDrawFunction(element, canvas, clip_rect, x_position, y_position);

  if(element->MustReDraw()) 
    {
      if(element_animation)
        {
          XBYTE alpha = element_animation->GetAlpha();
          if(alpha < element_image->GetAlpha()) element_image->SetAlpha(alpha);
        }

      Draw(element_image);     
    }

  switch(element_animation->GetState())
    {
      case UI_ELEMENT_ANIMATION_STATE_UNKNOWN   : break;

      case UI_ELEMENT_ANIMATION_STATE_PLAY      : if(element_animation->GetXTimerPlay()->GetMeasureMilliSeconds() >= element_animation->GetMilliSecondsDelay())
                                                    {
                                                      element_animation->IndexImage_Add();
                                                      element_animation->GetXTimerPlay()->Reset();  
                                                      redraw = true;                                                          
                                                    }
                                                  
                                                  break;

      case UI_ELEMENT_ANIMATION_STATE_STOP      : element_animation->IndexImage_Reset();                                                  
                                                  redraw = true;
                                                  break;


      case UI_ELEMENT_ANIMATION_STATE_PAUSE     : break;
    }

  PostDrawFunction(element, canvas, clip_rect, x_position, y_position);
  
  if(redraw) 
    {
      Elements_SetToRedraw(element_animation);  

      // If this animation lives inside a clipping scrollable container, it no longer owns a rebuild area. The rebuild
      // area belongs to the OUTERMOST clipping scrollable ancestor (an inner one — e.g. a MultiOption — also skips its
      // area because IT is inside the outer container). So walk the whole chain and mark that outermost owner: its area
      // (stretched to cover the VISIBLE-axis overflow in PreDrawFunction) is what erases the previous sprite. Marking an
      // inner scrollable that owns no area would do nothing, which is why the sprite was piling up.
      UI_ELEMENT* ancestor = element_animation->GetFather();
      UI_ELEMENT* owner    = NULL;
      while(ancestor)
        {
          UI_PROPERTY_SCROLLEABLE* sc = dynamic_cast<UI_PROPERTY_SCROLLEABLE*>(ancestor);
          if(sc && sc->Scroll_NeedClip()) owner = ancestor;   // keep the outermost
          ancestor = ancestor->GetFather();
        }
      if(owner) Elements_SetToRedraw(owner, true);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::Draw_Option(UI_ELEMENT* element)
* @brief      Draw option
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_Option(UI_ELEMENT* element)  
{
  if(!element) return false;
 
  UI_ELEMENT_OPTION*    element_option    = (UI_ELEMENT_OPTION*)element;
  UI_ELEMENT_ANIMATION* element_animation = element_option->Get_UIAnimation();
  UI_ELEMENT_TEXT*      element_text      = element_option->Get_UIText();
  double                x_position        = 0.0f;
  double                y_position        = 0.0f;
  GRP2DCANVAS*            canvas            = GetCanvas();
  XRECT                 clip_rect;              

  if(!canvas) return false;

  PreDrawFunction(element, canvas, clip_rect, x_position, y_position, UI_SKINCANVAS_PRESELECT_MAXEDGE);

  if(element->MustReDraw()) 
    {
      bool isselected  = false;
      bool ispreselect = false;

      if(element_option->GetFather()) 
        {
          if(element_option->GetFather()->GetType() != UI_ELEMENT_TYPE_MULTIOPTION)
            {
              ispreselect = element_option->IsPreSelect();
              if(!ispreselect) ispreselect = element_option->GetFather()->IsPreSelect();    
            }

          if(element_option->GetFather()->GetType() != UI_ELEMENT_TYPE_MULTIOPTION)
            {
              isselected = element_option->IsSelected();
              if(!isselected) isselected = element_option->GetFather()->IsSelected();    
            }
  
        } 
       else 
        {
          isselected = element_option->IsSelected();
          ispreselect = element_option->IsPreSelect();
        }

      if(element->IsBlinking() && !element->GetStateBlink()) ispreselect = false;

      DrawBackgroundColor(element_option, canvas, x_position, y_position);
        
      if(isselected)
        { 
          if(element_option->GetVisibleLimitType() & UI_ELEMENT_OPTION_VISIBLE_LIMIT_SELECT) 
            {     
              GRP2DCOLOR_RGBA8  colorred(255, 0, 0);     
              GRP2DCOLOR_RGBA8  colorhover(0x32, 0x33, 0x36, 150);
  
              canvas->SetLineColor(&colorred);
              canvas->SetFillColor(&colorhover);

              canvas->SetLineWidth(1.0f);

              if(element_option->GetRoundRect())
                {
                  canvas->RoundRect(x_position  - UI_SKINCANVAS_PRESELECT_MAXEDGE                                      , 
                                    y_position  + UI_SKINCANVAS_PRESELECT_MAXEDGE                                      ,
                                    x_position  + element->GetBoundaryLine()->width  + UI_SKINCANVAS_PRESELECT_MAXEDGE , 
                                    UI_BOUNDARYLINE_EdgeTop(y_position, element->GetBoundaryLine()->height) - UI_SKINCANVAS_PRESELECT_MAXEDGE , element_option->GetRoundRect(), true);      
                }
               else
                {
                  canvas->Rectangle(x_position  - UI_SKINCANVAS_PRESELECT_MAXEDGE                                      , 
                                    y_position  + UI_SKINCANVAS_PRESELECT_MAXEDGE                                      ,
                                    x_position  + element->GetBoundaryLine()->width  + UI_SKINCANVAS_PRESELECT_MAXEDGE , 
                                    UI_BOUNDARYLINE_EdgeTop(y_position, element->GetBoundaryLine()->height) - UI_SKINCANVAS_PRESELECT_MAXEDGE , true);      
                } 
            }
        }
       else
        {
          if(ispreselect)
            {       
              if(element_option->GetVisibleLimitType() & UI_ELEMENT_OPTION_VISIBLE_LIMIT_PRESELECT) 
                {        
                  GRP2DCOLOR_RGBA8  colorwhite(255, 255, 255);
                  GRP2DCOLOR_RGBA8  colorhover(0x32, 0x33, 0x36, 150);
  
                  canvas->SetLineColor(&colorwhite);
                  canvas->SetFillColor(&colorhover);

                  canvas->SetLineWidth(1.0f);
      
                  if(element_option->GetRoundRect())
                    {
                      canvas->RoundRect(x_position  - UI_SKINCANVAS_PRESELECT_MAXEDGE                                      , 
                                        y_position  + UI_SKINCANVAS_PRESELECT_MAXEDGE                                      ,
                                        x_position  + element->GetBoundaryLine()->width  + UI_SKINCANVAS_PRESELECT_MAXEDGE , 
                                        UI_BOUNDARYLINE_EdgeTop(y_position, element->GetBoundaryLine()->height) - UI_SKINCANVAS_PRESELECT_MAXEDGE , element_option->GetRoundRect(), true);      
                    }
                   else
                    {
                      canvas->Rectangle(x_position  - UI_SKINCANVAS_PRESELECT_MAXEDGE                                      , 
                                        y_position  + UI_SKINCANVAS_PRESELECT_MAXEDGE                                      ,
                                        x_position  + element->GetBoundaryLine()->width  + UI_SKINCANVAS_PRESELECT_MAXEDGE , 
                                        UI_BOUNDARYLINE_EdgeTop(y_position, element->GetBoundaryLine()->height) - UI_SKINCANVAS_PRESELECT_MAXEDGE , true);      
                    } 
                }
            }
           else          
            { 
              if(element_option->GetVisibleLimitType() & UI_ELEMENT_OPTION_VISIBLE_LIMIT_ACTIVE)
                {
                  GRP2DCOLOR_RGBA8  colorwhite(255, 255, 255);
                  GRP2DCOLOR_RGBA8  colorhover(0x32, 0x33, 0x36, 0);
  
                  canvas->SetLineColor(&colorwhite);
                  canvas->SetFillColor(&colorhover);

                  canvas->SetLineWidth(1.0f);
      
                  if(element_option->GetRoundRect())
                    {
                      canvas->RoundRect(x_position  - UI_SKINCANVAS_PRESELECT_MAXEDGE                                      , 
                                        y_position  + UI_SKINCANVAS_PRESELECT_MAXEDGE                                      ,
                                        x_position  + element->GetBoundaryLine()->width  + UI_SKINCANVAS_PRESELECT_MAXEDGE , 
                                        UI_BOUNDARYLINE_EdgeTop(y_position, element->GetBoundaryLine()->height) - UI_SKINCANVAS_PRESELECT_MAXEDGE , element_option->GetRoundRect(), true);      
                    }
                   else
                    {
                      canvas->Rectangle(x_position  - UI_SKINCANVAS_PRESELECT_MAXEDGE                                      , 
                                        y_position  + UI_SKINCANVAS_PRESELECT_MAXEDGE                                      ,
                                        x_position  + element->GetBoundaryLine()->width  + UI_SKINCANVAS_PRESELECT_MAXEDGE , 
                                        UI_BOUNDARYLINE_EdgeTop(y_position, element->GetBoundaryLine()->height) - UI_SKINCANVAS_PRESELECT_MAXEDGE , true);      
                    } 
                }
            }
        }

      if(element_animation)  Draw(element_animation);    
      if(element_text)       Draw(element_text);  
    }

  
  PostDrawFunction(element, canvas, clip_rect, x_position, y_position);

  if(element_animation )
    {
      if(element_animation->GetState() == UI_ELEMENT_ANIMATION_STATE_PLAY)
        { 
          Elements_SetToRedraw(element_option);  
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::Draw_MultiOption(UI_ELEMENT* element)
* @brief      Draw multi option
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_MultiOption(UI_ELEMENT* element)
{    
  if(!screen)  return false;
  if(!element) return false;

  UI_ELEMENT_MULTIOPTION* element_multioption  = (UI_ELEMENT_MULTIOPTION*)element;
  double                  x_position           = 0.0f;
  double                  y_position           = 0.0f;
  GRP2DCANVAS*              canvas               = GetCanvas();
  XRECT                   clip_rect;  
  
  if(!canvas) return false;
  
  PreDrawFunction(element, canvas, clip_rect, x_position, y_position);

  UI_PROPERTY_SELECTABLE_STATE actual_state = UI_PROPERTY_SELECTABLE_STATE_DEACTIVE;
  
  if(element_multioption->IsSelected()) 
    { 
      actual_state = UI_PROPERTY_SELECTABLE_STATE_SELECTED;
    }
   else 
    {
      if(element_multioption->IsPreSelect()) 
        {
          actual_state = UI_PROPERTY_SELECTABLE_STATE_PRESELECT;        
        }
       else 
        {
          if(element_multioption->IsActive()) 
            { 
              actual_state = UI_PROPERTY_SELECTABLE_STATE_ACTIVE;              
            }
        }
    }

  //if(element->MustReDraw()) 
    {
      DrawBackgroundColor(element_multioption, canvas, x_position, y_position);
 
      for(XDWORD c=0; c<element_multioption->GetComposeElements()->GetSize(); c++)
        {
          UI_ELEMENT_OPTION* element_option = (UI_ELEMENT_OPTION*)element_multioption->GetComposeElements()->Get(c);
          if(element_option) 
            {
              if(element_option->GetSelectableState() == actual_state) 
                {
                  Draw(element_option);              
                  break;              
                }       
            }            
        }
    }
      
  PostDrawFunction(element, canvas, clip_rect, x_position, y_position);

  return true;
}
  

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::Draw_Button(UI_ELEMENT* element)
* @brief      Draw button
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_Button(UI_ELEMENT* element)  
{  
  return Draw_Option(element);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::Draw_CheckBox(UI_ELEMENT* element)
* @brief      Draw check box
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_CheckBox(UI_ELEMENT* element)  
{
  UI_ELEMENT_CHECKBOX* checkbox = (UI_ELEMENT_CHECKBOX*)element;

  if(checkbox)
    {
      UI_ELEMENT_ANIMATION* animation = checkbox->Get_UIAnimation();
      if(animation) animation->IndexImage_Set(checkbox->GetStatus()?1:0);
    }

  return Draw_Option(element);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::Draw_EditText(UI_ELEMENT* element)
* @brief      Draw edit text
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_EditText(UI_ELEMENT* element)  
{    
  if(!element) return false;  

  UI_ELEMENT_EDITTEXT*  element_edittext  = (UI_ELEMENT_EDITTEXT*)element;
  double                x_position        = 0.0f;
  double                y_position        = 0.0f;
  GRP2DCANVAS*            canvas            = GetCanvas();    
  static bool           visible           = false;
  XRECT                 clip_rect;                       
  
  if(!canvas) return false;

  PreDrawFunction(element, canvas, clip_rect, x_position, y_position, UI_SKINCANVAS_EDIT_MAXEDGE);
  
  GRP2DCOLOR_RGBA8  color(element->GetColor()->GetRed(),
                          element->GetColor()->GetGreen(),
                          element->GetColor()->GetBlue(),
                          element->GetColor()->GetAlpha());

  GRP2DCOLOR_RGBA8  bkgcolor(element->GetBackgroundColor()->GetRed(),
                             element->GetBackgroundColor()->GetGreen(),
                             element->GetBackgroundColor()->GetBlue(),
                             element_edittext->IsPreSelect()?element->GetBackgroundColor()->GetAlpha()/2:element->GetBackgroundColor()->GetAlpha());

  GRP2DCOLOR_RGBA8  linecolor;

  GRP2DCOLOR_RGBA8  colorwhite(255, 255, 255, 255);
 
  if(element->MustReDraw()) 
    {
      canvas->SetLineColor(&linecolor);
      canvas->SetFillColor(&bkgcolor);

      if(element->GetRoundRect())
        {
          canvas->RoundRect(element->GetXPosition()  - UI_SKINCANVAS_EDIT_MAXEDGE                                      ,
                            element->GetYPosition()  + UI_SKINCANVAS_EDIT_MAXEDGE                                      ,       
                            element->GetXPosition()  + element->GetBoundaryLine()->width  + UI_SKINCANVAS_EDIT_MAXEDGE , 
                            element->GetTopY() - UI_SKINCANVAS_EDIT_MAXEDGE , element->GetRoundRect(), true);        
        }
       else
        {
          canvas->Rectangle(element->GetXPosition()  - UI_SKINCANVAS_EDIT_MAXEDGE                                      ,
                            element->GetYPosition()  + UI_SKINCANVAS_EDIT_MAXEDGE                                      ,       
                            element->GetXPosition()  + element->GetBoundaryLine()->width  + UI_SKINCANVAS_EDIT_MAXEDGE , 
                            element->GetTopY() - UI_SKINCANVAS_EDIT_MAXEDGE , true);
        }
   
                                   
      canvas->Vectorfont_GetConfig()->SetColor(&color);
      canvas->Vectorfont_GetConfig()->SetSize(element_edittext->GetSizeFont());  
      canvas->VectorFont_Printf(element->GetXPosition(), element->GetYPosition(), element_edittext->GetText()->Get());

      if(element_edittext->Cursor_IsVisible())
        {
          XDWORD    cursor_xpos = 0;
          XSTRING   cursor_string;

          element_edittext->GetText()->Copy(0, element_edittext->Cursor_GetPosition(), cursor_string);

          cursor_xpos = (XDWORD)GetWidthString(cursor_string.Get(), element_edittext->GetSizeFont());
       
          if(element_edittext->Cursor_GetXTimerBlink()->GetMeasureMilliSeconds() >= element_edittext->Cursor_GetTimeBlink())
            {
              element_edittext->Cursor_ChangeStateBlink();   
              element_edittext->Cursor_GetXTimerBlink()->Reset();
            } 
 
          if(element_edittext->Cursor_GetStateBlink())
            {
              int cursor_size = 3;

              canvas->SetLineColor(&colorwhite);
              canvas->SetFillColor(&colorwhite);

              canvas->Rectangle( x_position + cursor_xpos , 
                                 y_position + UI_SKINCANVAS_EDIT_MAXEDGE - 4  ,
                                 x_position + cursor_xpos + cursor_size , 
                                 UI_BOUNDARYLINE_EdgeTop(y_position, element_edittext->GetBoundaryLine()->height) - UI_SKINCANVAS_EDIT_MAXEDGE + 4 , true);                          
            }
        }
    }

  PostDrawFunction(element, canvas, clip_rect, x_position, y_position);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::Draw_Form(UI_ELEMENT* element)
* @brief      Draw form
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_Form(UI_ELEMENT* element)
{    
  if(!screen)  return false;
  if(!element) return false;

  UI_ELEMENT_FORM*    element_form = (UI_ELEMENT_FORM*)element;
  double              x_position   = 0.0f;
  double              y_position   = 0.0f;
  GRP2DCANVAS*          canvas       = GetCanvas();
  XRECT               clip_rect;  
  
  if(!canvas) return false;
  
  PreDrawFunction(element, canvas, clip_rect, x_position, y_position);

  if(element->MustReDraw()) 
    {
      GRP2DCOLOR_RGBA8  color(element_form->GetColor()->GetRed(),
                              element_form->GetColor()->GetGreen(),
                              element_form->GetColor()->GetBlue(),
                              element_form->GetColor()->GetAlpha());

      GRP2DCOLOR_RGBA8  linecolor(element_form->GetLineColor()->GetRed(),
                                  element_form->GetLineColor()->GetGreen(),
                                  element_form->GetLineColor()->GetBlue(),
                                  element_form->GetLineColor()->GetAlpha());

      
      canvas->SetLineColor(&linecolor);
      canvas->SetFillColor(&color);

      canvas->SetLineWidth(1.0f);

      if(element_form->GetRoundRect())
        {          
          canvas->RoundRect(element_form->GetVisibleRect()->x, 
                            element_form->GetVisibleRect()->y,
                            element_form->GetVisibleRect()->x + element_form->GetVisibleRect()->width, 
                            element_form->GetVisibleRect()->GetTop(), element_form->GetRoundRect(), true);
        }
       else
        {        
          canvas->Rectangle(element_form->GetVisibleRect()->x,
                            element_form->GetVisibleRect()->y,
                            element_form->GetVisibleRect()->x + element_form->GetVisibleRect()->width, 
                            element_form->GetVisibleRect()->GetTop(),  true);
        }
    }

  for(XDWORD c=0; c<element_form->GetComposeElements()->GetSize(); c++)
    {
      UI_ELEMENT* subelement = (UI_ELEMENT*)element->GetComposeElements()->Get(c);
      if(subelement) 
        {
          Draw(subelement);
        }  
    }
  
  PostDrawFunction(element, canvas, clip_rect, x_position, y_position);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::Draw_Menu(UI_ELEMENT* element)
* @brief      Draw menu
* @ingroup    USERINTERFACE
* 
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_Menu(UI_ELEMENT* element)
{
  return Draw_Form(element);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::Draw_ListBox(UI_ELEMENT* element)
* @brief      Draw list box
* @ingroup    USERINTERFACE
*
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_ListBox(UI_ELEMENT* element)
{
  if(!element) return false;  

  UI_ELEMENT_LISTBOX*   element_listbox = (UI_ELEMENT_LISTBOX*)element;
  double                x_position      = 0.0f;
  double                y_position      = 0.0f;
  GRP2DCANVAS*            canvas          = GetCanvas();    
  static bool           visible         = false;
  XRECT                 clip_rect;                       
  
  if(!canvas) return false;

  PreDrawFunction(element, canvas, clip_rect, x_position, y_position, UI_SKINCANVAS_EDIT_MAXEDGE);

  if(element->MustReDraw()) 
    {
      GRP2DCOLOR_RGBA8  color(element->GetColor()->GetRed(),
                              element->GetColor()->GetGreen(),
                              element->GetColor()->GetBlue(),
                              element->GetColor()->GetAlpha());

      GRP2DCOLOR_RGBA8  bkgcolor(element->GetBackgroundColor()->GetRed(),
                                 element->GetBackgroundColor()->GetGreen(),
                                 element->GetBackgroundColor()->GetBlue(),
                                 element_listbox->IsPreSelect()?element->GetBackgroundColor()->GetAlpha()/2:element->GetBackgroundColor()->GetAlpha());

      GRP2DCOLOR_RGBA8  linecolor;

      GRP2DCOLOR_RGBA8  colorwhite(255, 255, 255, 255);
 
  
      canvas->SetLineColor(&linecolor);
      canvas->SetFillColor(&bkgcolor);

      if(element->GetRoundRect())
        {
          canvas->RoundRect(element->GetXPosition()  - UI_SKINCANVAS_EDIT_MAXEDGE                                      , 
                            element->GetYPosition()  + UI_SKINCANVAS_EDIT_MAXEDGE                                      ,
                            element->GetXPosition()  + element->GetBoundaryLine()->width  + UI_SKINCANVAS_EDIT_MAXEDGE , 
                            element->GetTopY() - UI_SKINCANVAS_EDIT_MAXEDGE , element->GetRoundRect(), true);
        }
       else
        {
          canvas->Rectangle(element->GetXPosition()  - UI_SKINCANVAS_EDIT_MAXEDGE                                      , 
                            element->GetYPosition()  + UI_SKINCANVAS_EDIT_MAXEDGE                                      ,
                            element->GetXPosition()  + element->GetBoundaryLine()->width  + UI_SKINCANVAS_EDIT_MAXEDGE , 
                            element->GetTopY() - UI_SKINCANVAS_EDIT_MAXEDGE , true);
        }
                                      
      canvas->Vectorfont_GetConfig()->SetColor(&color);
      canvas->Vectorfont_GetConfig()->SetSize(element_listbox->GetSizeFont());  
      canvas->VectorFont_Printf(element->GetXPosition(), element->GetYPosition(), element_listbox->GetText()->Get());
    }
  
  PostDrawFunction(element, canvas, clip_rect, x_position, y_position);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::Draw_ProgressBar(UI_ELEMENT* element)
* @brief      Draw progress bar
* @ingroup    USERINTERFACE
*
* @param[in]  element : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_ProgressBar(UI_ELEMENT* element)
{
  if(!element) return false;  

  UI_ELEMENT_PROGRESSBAR* element_progressbar   = (UI_ELEMENT_PROGRESSBAR*)element;
  UI_ELEMENT*             element_progressrect  = element_progressbar->GetProgressRect();
  UI_ELEMENT_ANIMATION*   element_animation     = element_progressbar->Get_UIAnimation();
  UI_ELEMENT_TEXT*        element_text          = element_progressbar->Get_UIText();
  double                  x_position            = 0.0f;
  double                  y_position            = 0.0f;
  GRP2DCANVAS*              canvas                = GetCanvas();    
  static bool             visible               = false;
  XRECT                   clip_rect;                       
  
  if(!canvas) return false;

  PreDrawFunction(element, canvas, clip_rect, x_position, y_position, 1);

  if(element->MustReDraw()) 
    {
      if(element_progressrect)
        {
          GRP2DCOLOR_RGBA8  linecolor(element_progressbar->GetLineColor()->GetRed(), element_progressbar->GetLineColor()->GetGreen(), element_progressbar->GetLineColor()->GetBlue(), element_progressbar->GetLineColor()->GetAlpha());
          GRP2DCOLOR_RGBA8  color    (element->GetColor()->GetRed(), element->GetColor()->GetGreen(), element->GetColor()->GetBlue(), element->GetColor()->GetAlpha());      
          GRP2DCOLOR_RGBA8  bkgcolor (element->GetBackgroundColor()->GetRed(), element->GetBackgroundColor()->GetGreen(), element->GetBackgroundColor()->GetBlue(), element->GetBackgroundColor()->GetAlpha());
          
          canvas->SetLineColor(&linecolor);
          canvas->SetFillColor(&bkgcolor);

          canvas->SetLineWidth(1.0);                                                  // thin, deterministic border (do not inherit another element's line width)

          double roundradius = element->GetRoundRect();                               // existing rounded-corner radius

          if(element_progressbar->GetRoundCap())                                      // capsule ends: radius = half the bar thickness
            {
              if(element_progressbar->GetDirection() == UI_ELEMENT_TYPE_DIRECTION_VERTICAL)
                   roundradius = element_progressrect->GetBoundaryLine()->width  / 2.0;
              else roundradius = element_progressrect->GetBoundaryLine()->height / 2.0;
            }

          UI_SkinCanvas_ProgressBar_DrawRect(canvas,
                                             element_progressrect->GetXPosition(),
                                             element_progressrect->GetYPosition(),
                                             element_progressrect->GetXPosition() + element_progressrect->GetBoundaryLine()->width,
                                             element_progressrect->GetTopY(),
                                             roundradius);

          double widthpercent   = element_progressrect->GetBoundaryLine()->width;
          double heightpercent  = element_progressrect->GetBoundaryLine()->height;  
                                    
          if(element_progressbar->ContinuousCycle_Is())
            {                
              XDWORD xpos_segment = 0;
              XDWORD ypos_segment = 0;
           
              switch(element_progressbar->GetDirection())
                {     
                  case UI_ELEMENT_TYPE_DIRECTION_UNKWOWN	  :
	                case UI_ELEMENT_TYPE_DIRECTION_HORIZONTAL : widthpercent   = (element_progressrect->GetBoundaryLine()->width  * element_progressbar->ContinuousCycle_GetSizeSegment()) / 100.0f;   
                                                              xpos_segment   = element_progressbar->ContinuousCycle_GetNSteps();
                                                              xpos_segment  += element_progressbar->ContinuousCycle_GetStepSegment();
                                                              element_progressbar->ContinuousCycle_SetNSteps(xpos_segment);        
                                                                                                                          
                                                              if(xpos_segment < widthpercent) 
                                                                {                                                                   
                                                                  widthpercent = xpos_segment;                                                                  
                                                                  xpos_segment = 0;                                                                                                                                                                                                                                                                                                                                           
                                                                }                                                                
                                                               else
                                                                {                                                                                                                                      
                                                                  if((xpos_segment + widthpercent) >= element_progressrect->GetBoundaryLine()->width) 
                                                                    {                                                                       
                                                                      widthpercent = (element_progressrect->GetBoundaryLine()->width - xpos_segment);                                                                       
                                                                      if(!widthpercent) 
                                                                        {
                                                                          xpos_segment = 0;
                                                                          element_progressbar->ContinuousCycle_SetNSteps(xpos_segment);                                                                          
                                                                        }

                                                                      xpos_segment -= (XDWORD)widthpercent;   
                                                                    }  
                                                                   else
                                                                    {
                                                                      xpos_segment -= (XDWORD)widthpercent;   
                                                                    }                                                                                                                                                                                                                       
                                                                }
                                                                 
                                                                                                                                                                                      
                                                              break;

	                case UI_ELEMENT_TYPE_DIRECTION_VERTICAL		: heightpercent  = (element_progressrect->GetBoundaryLine()->height * element_progressbar->ContinuousCycle_GetSizeSegment()) / 100.0f;   
                                                              ypos_segment   = element_progressbar->ContinuousCycle_GetNSteps();
                                                              ypos_segment  += element_progressbar->ContinuousCycle_GetStepSegment();
                                                              element_progressbar->ContinuousCycle_SetNSteps(ypos_segment);        
                                                              
                                                              if(ypos_segment < heightpercent) 
                                                                {                                                                   
                                                                  heightpercent = ypos_segment;                                                                  
                                                                  ypos_segment = 0;                                                                                                                                                                                                                                                                                                                                           
                                                                }                                                                
                                                               else
                                                                {                                                                                                                                      
                                                                  if((ypos_segment + heightpercent) >= element_progressrect->GetBoundaryLine()->height) 
                                                                    {                                                                       
                                                                      heightpercent = (element_progressrect->GetBoundaryLine()->height - ypos_segment);                                                                       
                                                                      if(!heightpercent) 
                                                                        {
                                                                          ypos_segment = 0;
                                                                          element_progressbar->ContinuousCycle_SetNSteps(ypos_segment);                                                                          
                                                                        }

                                                                      ypos_segment -= (XDWORD)heightpercent;   
                                                                    }  
                                                                   else
                                                                    {
                                                                      ypos_segment -= (XDWORD)heightpercent;   
                                                                    }                                                                                                                                                                                                                       
                                                                }
                                                                 
                                                              break;
                }

              canvas->SetFillColor(&color);

              UI_SkinCanvas_ProgressBar_DrawRect(canvas,
                                                 element_progressrect->GetXPosition() + xpos_segment,
                                                 element_progressrect->GetYPosition() + ypos_segment,
                                                 element_progressrect->GetXPosition() + xpos_segment + widthpercent,
                                                 element_progressrect->GetYPosition() - (ypos_segment + heightpercent),
                                                 roundradius);
              
            }
           else
            {                
              switch(element_progressbar->GetDirection())
                {     
                  case UI_ELEMENT_TYPE_DIRECTION_UNKWOWN	  :
	                case UI_ELEMENT_TYPE_DIRECTION_HORIZONTAL : widthpercent   = (element_progressrect->GetBoundaryLine()->width  * element_progressbar->GetLevel()) / 100.0f;   break;
	                case UI_ELEMENT_TYPE_DIRECTION_VERTICAL		: heightpercent  = (element_progressrect->GetBoundaryLine()->height * element_progressbar->GetLevel()) / 100.0f;   break;
                }
               
              if(element_progressbar->GetGradientColor()->GetAlpha())                 // gradient fill: color -> gradientcolor (else solid color below)
                {
                  GRP2DGRADIENTSTOP stops[2];

                  stops[0].offset = 0.0;
                  stops[0].color  = color;
                  stops[1].offset = 1.0;
                  stops[1].color  = GRP2DCOLOR_RGBA8(element_progressbar->GetGradientColor()->GetRed(),
                                                     element_progressbar->GetGradientColor()->GetGreen(),
                                                     element_progressbar->GetGradientColor()->GetBlue(),
                                                     element_progressbar->GetGradientColor()->GetAlpha());

                  double bx    = element_progressrect->GetXPosition();
                  double by    = element_progressrect->GetYPosition();
                  double bw    = element_progressrect->GetBoundaryLine()->width;
                  double bh    = element_progressrect->GetBoundaryLine()->height;
                  bool   track = (element_progressbar->GetGradientMode() == UI_ELEMENT_PROGRESS_GRADIENTMODE_TRACK);

                  double gx1, gy1, gx2, gy2;

                  if(element_progressbar->GetDirection() == UI_ELEMENT_TYPE_DIRECTION_VERTICAL)
                    {
                      gx1 = bx;  gx2 = bx;
                      gy1 = by;                                                        // bottom = start color
                      gy2 = track ? (by - bh) : (by - heightpercent);                  // track: full bar | fill: current fill top
                    }
                   else
                    {
                      gy1 = by;  gy2 = by;
                      gx1 = bx;                                                        // left = start color
                      gx2 = track ? (bx + bw) : (bx + widthpercent);                   // track: full bar | fill: current fill right
                    }

                  UI_SkinCanvas_ProgressBar_DrawGradientRect(canvas, bx, by, bx + widthpercent, by - heightpercent, roundradius, stops, gx1, gy1, gx2, gy2);
                }
               else
                {
                  canvas->SetFillColor(&color);

                  UI_SkinCanvas_ProgressBar_DrawRect(canvas,
                                                     element_progressrect->GetXPosition(),
                                                     element_progressrect->GetYPosition(),
                                                     element_progressrect->GetXPosition() + widthpercent,
                                                     element_progressrect->GetYPosition() - heightpercent,
                                                     roundradius);
                }
            } 

          if(element_text) Draw(element_text);  
            
        }  
    
      if(element_animation) Draw(element_animation);                    
    }

  PostDrawFunction(element, canvas, clip_rect, x_position, y_position); 
    
  if(element_progressbar->ContinuousCycle_Is()) 
    {
      if(element_progressbar->ContinuousCycle_GetXTimer()->GetMeasureMilliSeconds() > element_progressbar->ContinuousCycle_GetTimeStep())
        {
          element->SetMustReDraw(true);

          element_progressbar->ContinuousCycle_GetXTimer()->Reset();
        }
    }
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_SKINCANVAS::Draw_ProgressRadial(UI_ELEMENT* element)
* @brief      Draw radial progress (track ring + gradient value arc + centered caption)
* @ingroup    USERINTERFACE
*
* @param[in]  element :
*
* @return     bool : true if is succesful.
*
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_ProgressRadial(UI_ELEMENT* element)
{
  if(!element) return false;

  UI_ELEMENT_PROGRESS_RADIAL* element_progress = (UI_ELEMENT_PROGRESS_RADIAL*)element;
  UI_ELEMENT_TEXT*            element_text     = (UI_ELEMENT_TEXT*)element_progress->Get_UIText();
  GRP2DCANVAS*                canvas           = GetCanvas();
  double                      x_position       = 0.0f;
  double                      y_position       = 0.0f;
  XRECT                       clip_rect;

  if(!canvas) return false;

  PreDrawFunction(element, canvas, clip_rect, x_position, y_position, 1);

  if(element->MustReDraw())
    {
      double width  = element->GetBoundaryLine()->width;
      double height = element->GetBoundaryLine()->height;

      double cx     = x_position + (width  / 2.0);
      double cy     = y_position - (height / 2.0);                       // y_position is the BOTTOM edge

      double outer  = __MIN(width, height) / 2.0;

      double thick  = element_progress->GetThickness();
      if(thick <= 0.0)    thick = outer * UI_ELEMENT_PROGRESS_RADIAL_AUTOTHICKNESS_FACTOR;
      if(thick > outer)   thick = outer;

      double r      = outer - (thick / 2.0) - 1.0;                       // radius to the ring centerline
      if(r < 1.0) r = 1.0;

      double start  = element_progress->GetStartAngle();
      double sweep  = element_progress->GetSweepAngle();

      // ---- 1) track ring (full sweep, flat background color) ---------------------------------------------------------
      if(element->GetBackgroundColor()->GetAlpha())
        {
          GRP2DCOLOR_RGBA8 trackcolor(element->GetBackgroundColor()->GetRed(),
                                      element->GetBackgroundColor()->GetGreen(),
                                      element->GetBackgroundColor()->GetBlue(),
                                      element->GetBackgroundColor()->GetAlpha());

          canvas->SetLineWidth(thick);
          canvas->SetLineColor(&trackcolor);

          if(fabs(sweep) >= 359.999)
            {
              canvas->Circle(cx, cy, r, false);                                       // full ring: closed ellipse outline (no polyline seam or end caps)
            }
           else
            {
              GRP2DPATH trackpath;
              bool      firstpoint = true;

              UI_SkinCanvas_ProgressRadial_AppendArc(trackpath, cx, cy, r, start, sweep, firstpoint);
              canvas->Path(trackpath, false);                                         // partial sweep: open arc
            }
        }

      // ---- 2) value arc (gradient color -> linecolor, scaled by level) -----------------------------------------------
      double valuesweep = sweep * (element_progress->GetLevel() / 100.0);

      if(fabs(valuesweep) > 0.01)
        {
          GRP2DPATH valuepath;
          bool      firstpoint = true;

          UI_SkinCanvas_ProgressRadial_AppendArc(valuepath, cx, cy, r, start, valuesweep, firstpoint);

          if(element_progress->GetGradientColor()->GetAlpha())             // ANGULAR gradient along the arc: color -> gradientcolor
            {
              UI_COLOR* c0    = element->GetColor();
              UI_COLOR* c1    = element_progress->GetGradientColor();
              bool      track = (element_progress->GetGradientMode() == UI_ELEMENT_PROGRESS_GRADIENTMODE_TRACK);

              int    nseg = (int)(fabs(valuesweep) / 3.0);                  // ~3 degrees per segment
              if(nseg < 1) nseg = 1;

              double segsweep = valuesweep / (double)nseg;

              canvas->SetLineWidth(thick);

              for(int i = 0; i < nseg; i++)
                {
                  double a0 = start + (segsweep * (double)i);
                  double t  = track ? ((segsweep * ((double)i + 0.5)) / sweep)        // along the FULL sweep: strong only at 100%
                                    : (((double)i + 0.5) / (double)nseg);             // along the value arc: strong at the leading edge

                  if(t < 0.0) t = 0.0;
                  if(t > 1.0) t = 1.0;

                  int rr = (int)c0->GetRed()   + (int)(((int)c1->GetRed()   - (int)c0->GetRed())   * t);
                  int gg = (int)c0->GetGreen() + (int)(((int)c1->GetGreen() - (int)c0->GetGreen()) * t);
                  int bb = (int)c0->GetBlue()  + (int)(((int)c1->GetBlue()  - (int)c0->GetBlue())  * t);
                  int aa = (int)c0->GetAlpha() + (int)(((int)c1->GetAlpha() - (int)c0->GetAlpha()) * t);

                  GRP2DCOLOR_RGBA8 segcolor(rr, gg, bb, aa);
                  canvas->SetLineColor(&segcolor);

                  double s = segsweep;
                  if(i < (nseg - 1)) s += (segsweep * 0.6);                            // overlap forward so the round joins hide the seams (last segment not extended)

                  GRP2DPATH segpath;
                  bool      segfirst = true;

                  UI_SkinCanvas_ProgressRadial_AppendArc(segpath, cx, cy, r, a0, s, segfirst);
                  canvas->Path(segpath, false);
                }
            }
           else                                                          // flat stroke with the element color
            {
              GRP2DCOLOR_RGBA8 arccolor(element->GetColor()->GetRed(),
                                        element->GetColor()->GetGreen(),
                                        element->GetColor()->GetBlue(),
                                        element->GetColor()->GetAlpha());

              canvas->SetLineWidth(thick);
              canvas->SetLineColor(&arccolor);
              canvas->Path(valuepath, false);
            }
        }

      // ---- 2b) rounded caps (optional): filled circles sampling the gradient at each endpoint ------------------------
      if(element_progress->GetRoundCap() && (fabs(valuesweep) > 0.01))
        {
          double cap_r   = thick / 2.0;
          double a_start = start                 * (PI / 180.0);
          double a_end   = (start + valuesweep)  * (PI / 180.0);

          double sx = cx + (r * cos(a_start));
          double sy = cy + (r * sin(a_start));
          double ex = cx + (r * cos(a_end));
          double ey = cy + (r * sin(a_end));

          GRP2DCOLOR_RGBA8 capstart;
          GRP2DCOLOR_RGBA8 capend;

          if(element_progress->GetGradientColor()->GetAlpha())              // caps match the ANGULAR arc color at each end
            {
              UI_COLOR* c0    = element->GetColor();
              UI_COLOR* c1    = element_progress->GetGradientColor();
              bool      track = (element_progress->GetGradientMode() == UI_ELEMENT_PROGRESS_GRADIENTMODE_TRACK);
              double    tend  = track ? (element_progress->GetLevel() / 100.0) : 1.0;

              if(tend < 0.0) tend = 0.0;
              if(tend > 1.0) tend = 1.0;

              capstart = GRP2DCOLOR_RGBA8(c0->GetRed(), c0->GetGreen(), c0->GetBlue(), c0->GetAlpha());   // start cap = gradient at t 0

              int rr = (int)c0->GetRed()   + (int)(((int)c1->GetRed()   - (int)c0->GetRed())   * tend);
              int gg = (int)c0->GetGreen() + (int)(((int)c1->GetGreen() - (int)c0->GetGreen()) * tend);
              int bb = (int)c0->GetBlue()  + (int)(((int)c1->GetBlue()  - (int)c0->GetBlue())  * tend);
              int aa = (int)c0->GetAlpha() + (int)(((int)c1->GetAlpha() - (int)c0->GetAlpha()) * tend);

              capend = GRP2DCOLOR_RGBA8(rr, gg, bb, aa);                                                  // end cap = gradient at the leading-edge t
            }
           else                                                          // flat color
            {
              capstart = GRP2DCOLOR_RGBA8(element->GetColor()->GetRed(), element->GetColor()->GetGreen(), element->GetColor()->GetBlue(), element->GetColor()->GetAlpha());
              capend   = capstart;
            }

          canvas->SetLineWidth(0.0);                                       // fill only, no outline

          canvas->SetFillColor(&capstart);
          canvas->Circle(sx, sy, cap_r, true);

          canvas->SetFillColor(&capend);
          canvas->Circle(ex, ey, cap_r, true);
        }

      // ---- 3) centered caption (e.g. "37%") --------------------------------------------------------------------------
      if(element_text) Draw(element_text);

      canvas->SetLineWidth(1.0);                                                       // reset shared line width so it doesn't leak to the next element
    }

  PostDrawFunction(element, canvas, clip_rect, x_position, y_position);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_SKINCANVAS::Draw_ProgressImage(UI_ELEMENT* element)
* @brief      Draw progress image: empty (0%) graphic first, then full (100%) clipped to the level by direction.
* @ingroup    USERINTERFACE
*
* @param[in]  element :
*
* @return     bool : true if is succesful.
*
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Draw_ProgressImage(UI_ELEMENT* element)
{
  if(!element) return false;

  UI_ELEMENT_PROGRESS_IMAGE* element_progressimage = (UI_ELEMENT_PROGRESS_IMAGE*)element;
  UI_ELEMENT_TEXT*           element_text          = (UI_ELEMENT_TEXT*)element_progressimage->Get_UIText();
  GRP2DCANVAS*               canvas                = GetCanvas();
  double                     x_position            = 0.0f;
  double                     y_position            = 0.0f;
  XRECT                      clip_rect;

  GRPBITMAP*                 imgempty              = element_progressimage->GetImageEmpty();
  GRPBITMAP*                 imgfull               = element_progressimage->GetImageFull();

  if(!canvas) return false;
  if(!imgempty && !imgfull) return false;

  PreDrawFunction(element, canvas, clip_rect, x_position, y_position);

  if(element->MustReDraw())
    {
      GRPBITMAP* ref    = imgfull ? imgfull : imgempty;                  // both graphics share size
      double     imgw   = (double)ref->GetWidth();
      double     imgh   = (double)ref->GetHeight();
      double     alpha  = (double)element_progressimage->GetAlpha();

      // caption-aware image offset (mirrors the boundary computed in CalculateBoundaryLine_ProgressImage; (0,0) for none/right/up/center)
      int        allocation = (int)element_progressimage->GetAllocationTextType();
      double     textw      = element_text ? element_text->GetBoundaryLine()->width  : 0.0;
      double     texth      = element_text ? element_text->GetBoundaryLine()->height : 0.0;
      double     bw, bh, ox, oy, tx, ty;
      UI_SkinCanvas_ProgressImage_Layout(allocation, imgw, imgh, textw, texth, 6.0, bw, bh, ox, oy, tx, ty);
      (void)bw; (void)bh; (void)tx; (void)ty;                                       // Draw only needs the image offset (ox,oy)

      double     left   = x_position + ox;
      double     right  = left + imgw;
      double     top    = (y_position - element->GetBoundaryLine()->height) + oy;   // boundary top (y_position is BOTTOM) + image offset
      double     bottom = top + imgh;

      // ---- 1) empty graphic (0%) drawn fully -----------------------------------------------------------------------
      if(imgempty) canvas->PutBitmapAlpha(left, top, imgempty, alpha);

      // ---- 2) full graphic (100%) revealed up to level, clipped by direction ---------------------------------------
      if(imgfull && (element_progressimage->GetLevel() > 0.0f))
        {
          double level = (double)element_progressimage->GetLevel();
          if(level > 100.0) level = 100.0;

          double r_left   = left;
          double r_right  = right;
          double r_top    = top;
          double r_bottom = bottom;

          double offsetstart = element_progressimage->GetOffsetStart();
          double offsetend   = element_progressimage->GetOffsetEnd();

          if(element_progressimage->GetDirection() == UI_ELEMENT_TYPE_DIRECTION_VERTICAL)
            {
              double activelen  = imgh - offsetstart - offsetend;       // progression band along the height
              if(activelen < 0.0) activelen = 0.0;

              double bandbottom = bottom - offsetstart;                 // progression starts offsetstart up from the bottom
              r_bottom = bandbottom;                                    // revealed region bottom edge
              r_top    = bandbottom - (activelen * (level / 100.0));    // grows upward with level
            }
           else
            {
              double activelen = imgw - offsetstart - offsetend;        // progression band along the width
              if(activelen < 0.0) activelen = 0.0;

              double bandleft  = left + offsetstart;                    // progression starts offsetstart from the left
              r_left  = bandleft;                                       // revealed region left edge
              r_right = bandleft + (activelen * (level / 100.0));       // grows rightward with level
            }

          // intersect the reveal rect with the current clip, then draw only that part of the full graphic
          double ox1 = 0.0, oy1 = 0.0, ox2 = 0.0, oy2 = 0.0;
          canvas->GetClipBox(ox1, oy1, ox2, oy2);

          double cminx = __MIN(ox1, ox2), cmaxx = __MAX(ox1, ox2);
          double cminy = __MIN(oy1, oy2), cmaxy = __MAX(oy1, oy2);

          double rminx = __MIN(r_left, r_right),  rmaxx = __MAX(r_left, r_right);
          double rminy = __MIN(r_top,  r_bottom), rmaxy = __MAX(r_top,  r_bottom);

          double iminx = __MAX(cminx, rminx), imaxx = __MIN(cmaxx, rmaxx);
          double iminy = __MAX(cminy, rminy), imaxy = __MIN(cmaxy, rmaxy);

          if((imaxx > iminx) && (imaxy > iminy))
            {
              canvas->SetClipBox(iminx, imaxy, imaxx, iminy);            // (left, bottom, right, top)
              canvas->PutBitmapAlpha(left, top, imgfull, alpha);
              canvas->SetClipBox(ox1, oy1, ox2, oy2);                    // restore previous clip
            }
        }

      // ---- 3) optional caption -------------------------------------------------------------------------------------
      if(element_text) Draw(element_text);
    }

  PostDrawFunction(element, canvas, clip_rect, x_position, y_position);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::GetFontSize(XCHAR* text, XDWORD& width, XDWORD& height)
* @brief      Get font size
* @ingroup    USERINTERFACE
*
* @param[in]  text : 
* @param[in]  width : 
* @param[in]  height : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::GetFontSize(XCHAR* text, XDWORD& width, XDWORD& height)
{
  GRP2DCANVAS* canvas = GetCanvas();
  if(!canvas) return false;

  width   = (XDWORD)canvas->VectorFont_GetWidth(text);
  height  = (XDWORD)canvas->VectorFont_GetHeight(text);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::SetFontSize(XDWORD size)
* @brief      Set font size
* @ingroup    USERINTERFACE
*
* @param[in]  size : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::SetFontSize(XDWORD size)
{
  GRP2DCANVAS* canvas = GetCanvas();
  if(!canvas) return false;

  canvas->Vectorfont_GetConfig()->SetSize(size);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::DrawBackgroundColor(UI_ELEMENT* element, GRP2DCANVAS* canvas, double x_position, double y_position)
* @brief      Draw background color
* @ingroup    USERINTERFACE
*
* @param[in]  element : 
* @param[in]  canvas : 
* @param[in]  x_position : 
* @param[in]  y_position : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::DrawBackgroundColor(UI_ELEMENT* element, GRP2DCANVAS* canvas, double x_position, double y_position)
{
  if(!element->GetBackgroundColor()->GetAlpha()) return false;

  GRP2DCOLOR_RGBA8  bkgcolor(element->GetBackgroundColor()->GetRed(),
                              element->GetBackgroundColor()->GetGreen(),
                              element->GetBackgroundColor()->GetBlue(),
                              element->GetBackgroundColor()->GetAlpha());


  canvas->SetLineWidth(0.0f);
  canvas->SetLineColor(&bkgcolor);
  canvas->SetFillColor(&bkgcolor);

  canvas->Rectangle(x_position,
                    y_position,
                    x_position  + element->GetBoundaryLine()->width   , 
                    UI_BOUNDARYLINE_EdgeTop(y_position, element->GetBoundaryLine()->height)  , true);      

  return true;  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::PreDrawFunction(UI_ELEMENT* element, GRP2DCANVAS* canvas, XRECT& clip_rect, double& x_position, double& y_position, XDWORD edge)
* @brief      Pre draw function
* @ingroup    USERINTERFACE
*
* @param[in]  element : 
* @param[in]  canvas : 
* @param[in]  clip_rect : 
* @param[in]  x_position : 
* @param[in]  y_position : 
* @param[in]  edge : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/

// Accumulate the screen bounding box of an element's descendants, but only on the axes flagged (vx/vy). Used to size a
// scrollable container's rebuild area so it covers content that overflows on a non-clipped (VISIBLE) axis. Descendants
// no longer own rebuild areas, so without this their overflowing pixels (e.g. an animated icon sticking out of the bar)
// would never be erased and would pile up. Positions are absolute (scroll is applied at draw time, not stored), and a
// VISIBLE axis is never scrolled, so the stored positions are the right ones to measure on that axis.
static void UI_SkinCanvas_ContentExtent(UI_ELEMENT* element, bool vx, bool vy, double margin,
                                        double& minx, double& maxx, double& miny, double& maxy)
{
  if(!element) return;

  XVECTOR<UI_ELEMENT*>* childs = element->GetComposeElements();
  if(!childs) return;

  for(XDWORD c=0; c<childs->GetSize(); c++)
    {
      UI_ELEMENT* child = childs->Get(c);
      if(!child)                   continue;
      if(!child->IsVisible())      continue;
      if(!child->GetBoundaryLine()) continue;

      double cx = child->GetXPosition();
      double cy = child->GetYPosition();
      double cw = child->GetBoundaryLine()->width;
      double ch = child->GetBoundaryLine()->height;

      if(vx) { minx = __MIN(minx, UI_BOUNDARYLINE_EdgeLeft(cx, cw) - margin); maxx = __MAX(maxx, UI_BOUNDARYLINE_EdgeRight (cx, cw) + margin); }
      if(vy) { miny = __MIN(miny, UI_BOUNDARYLINE_EdgeTop (cy, ch) - margin); maxy = __MAX(maxy, UI_BOUNDARYLINE_EdgeBottom(cy, ch) + margin); }

      // Recurse to reach deeper content (e.g. an animated sprite inside a MultiOption). A nested clipping scrollable
      // bounds its content only on the axes IT clips; on a VISIBLE axis its content still overflows, so keep measuring
      // that axis and drop the ones it clips.
      bool child_vx = vx;
      bool child_vy = vy;
      UI_PROPERTY_SCROLLEABLE* csc = dynamic_cast<UI_PROPERTY_SCROLLEABLE*>(child);
      if(csc)
        {
          if(csc->Scroll_GetOverflow(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL) != UI_OVERFLOW_VISIBLE) child_vx = false;
          if(csc->Scroll_GetOverflow(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL)   != UI_OVERFLOW_VISIBLE) child_vy = false;
        }

      if(child_vx || child_vy) UI_SkinCanvas_ContentExtent(child, child_vx, child_vy, margin, minx, maxx, miny, maxy);
    }
}

bool UI_SKINCANVAS::PreDrawFunction(UI_ELEMENT* element, GRP2DCANVAS* canvas, XRECT& clip_rect, double& x_position, double& y_position, XDWORD edge)
{
  if(!canvas) return false;

  x_position = element->GetXPosition();
  y_position = element->GetYPosition();

  if(element->GetFather()) GetAddPositionScrollSteps(element->GetFather(), x_position, y_position);

  element->SetXPositionWithScroll(x_position);
  element->SetYPositionWithScroll(y_position);
 
  bool createarea = false;
  if(element->MustReDraw()) 
    {
      createarea = true;
      if(GetRebuildAreaByElement(element)) createarea = false;          

      // A descendant of an element that is already going to be redrawn must NOT own another rebuild area.
      // Otherwise nested transparent elements (button -> animation -> image) store overlapping copies of the same
      // background and those copies can be restored/repainted more than once, accumulating alpha.
      //
      // A descendant of a clipping scrollable container must NOT own a rebuild area either: it moves with the
      // container's scroll, and the area's save/restore (a fixed screen rectangle) would repaint ghosts at
      // stale/scrolled positions. The container's single rebuild area covers all its content and redraws it as a unit.
      UI_ELEMENT* ancestor = element->GetFather();
      while(ancestor)
        {
          if(ancestor->MustReDraw()) { createarea = false; break; }

          UI_PROPERTY_SCROLLEABLE* sc = dynamic_cast<UI_PROPERTY_SCROLLEABLE*>(ancestor);
          if(sc && sc->Scroll_NeedClip()) { createarea = false; break; }
          ancestor = ancestor->GetFather();
        }
    }

  if(createarea)
    {     
      //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Add area: level [%d] [%s] "), element->GetZLevel(), element->GetName()->Get());

      double area_left   = UI_BOUNDARYLINE_EdgeLeft  (x_position, element->GetBoundaryLine()->width);
      double area_right  = UI_BOUNDARYLINE_EdgeRight  (x_position, element->GetBoundaryLine()->width);
      double area_top    = UI_BOUNDARYLINE_EdgeTop    (y_position, element->GetBoundaryLine()->height);
      double area_bottom = UI_BOUNDARYLINE_EdgeBottom  (y_position, element->GetBoundaryLine()->height);

      // A scrollable container with a non-clipped (VISIBLE) axis lets its content overflow the viewport on that axis.
      // Those descendants no longer own rebuild areas, so the container's area must stretch to cover their overflow,
      // otherwise the overflowing pixels are never erased (animated icons pile up). The clipped axis stays at the
      // viewport (content there is clipped, so it never overflows).
      UI_PROPERTY_SCROLLEABLE* sc_self = dynamic_cast<UI_PROPERTY_SCROLLEABLE*>(element);
      if(sc_self)
        {
          bool vx = (sc_self->Scroll_GetOverflow(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL) == UI_OVERFLOW_VISIBLE);
          bool vy = (sc_self->Scroll_GetOverflow(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL)   == UI_OVERFLOW_VISIBLE);
          if(vx || vy) UI_SkinCanvas_ContentExtent(element, vx, vy, (double)UI_SKINCANVAS_PRESELECT_MAXEDGE,
                                                    area_left, area_right, area_top, area_bottom);
        }

      CreateRebuildArea(area_left - edge, 
                        area_top  - edge, 
                        (area_right  - area_left) + (edge * 2),
                        (area_bottom - area_top ) + (edge * 2), element);                    
    }

  UI_PROPERTY_SCROLLEABLE* scrolleable = dynamic_cast<UI_PROPERTY_SCROLLEABLE*>(element);
  if(scrolleable && scrolleable->Scroll_NeedClip())
    {
      double vp_width    = 0.0f;
      double vp_height   = 0.0f;
      GetScrollViewportSize(element, vp_width, vp_height);
      vp_width  = scrolleable->Scroll_GetContentViewport(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL, vp_width);
      vp_height = scrolleable->Scroll_GetContentViewport(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL  , vp_height);

      double clip_top    = UI_BOUNDARYLINE_EdgeTop    (y_position, vp_height);
      double clip_bottom = UI_BOUNDARYLINE_EdgeBottom (y_position, vp_height);
      double clip_right  = UI_BOUNDARYLINE_EdgeRight  (x_position, vp_width);
      double clip_left   = x_position - 1;

      canvas->GetClipBox(clip_rect);                                       // save the parent clip

      // Per-axis clipping (CSS overflow-x / overflow-y are independent): an axis whose overflow is VISIBLE is NOT
      // clipped (it inherits the parent bounds, so content may overflow on that axis). A clipped axis is INTERSECTED
      // with the parent clip, so a nested scrolleable is bounded by its ancestors instead of replacing their clip.
      bool clip_h = (scrolleable->Scroll_GetOverflow(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL) != UI_OVERFLOW_VISIBLE);
      bool clip_v = (scrolleable->Scroll_GetOverflow(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL)   != UI_OVERFLOW_VISIBLE);

      // element box and parent box, normalized to [min,max] (the stored clip can use either Y order)
      double e_minx = clip_left;
      double e_maxx = clip_right;
      double e_miny = clip_top - 1;
      double e_maxy = clip_bottom;

      double p_minx = __MIN(clip_rect.x1, clip_rect.x2);
      double p_maxx = __MAX(clip_rect.x1, clip_rect.x2);
      double p_miny = __MIN(clip_rect.y1, clip_rect.y2);
      double p_maxy = __MAX(clip_rect.y1, clip_rect.y2);

      double r_minx = clip_h ? __MAX(e_minx, p_minx) : p_minx;
      double r_maxx = clip_h ? __MIN(e_maxx, p_maxx) : p_maxx;
      double r_miny = clip_v ? __MAX(e_miny, p_miny) : p_miny;
      double r_maxy = clip_v ? __MIN(e_maxy, p_maxy) : p_maxy;

      canvas->SetClipBox(r_minx, r_maxy, r_maxx, r_miny);                  // (left, bottom, right, top): same order as before
    }
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::PostDrawFunction(UI_ELEMENT* element, GRP2DCANVAS* canvas, XRECT& clip_rect, double x_position, double y_position)
* @brief      Post draw function
* @ingroup    USERINTERFACE
*
* @param[in]  element : 
* @param[in]  canvas : 
* @param[in]  clip_rect : 
* @param[in]  x_position : 
* @param[in]  y_position : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::PostDrawFunction(UI_ELEMENT* element, GRP2DCANVAS* canvas, XRECT& clip_rect, double x_position, double y_position)
{  
  bool redrew = element->MustReDraw();           // capture before clearing: true only when the area was just repainted
  element->SetMustReDraw(false);

  UI_PROPERTY_SCROLLEABLE* scrolleable = dynamic_cast<UI_PROPERTY_SCROLLEABLE*>(element);
  if(scrolleable && scrolleable->Scroll_NeedClip())
    {
      canvas->SetClipBox(clip_rect);
    }

  if(scrolleable)
    {
      ResolveScrollPolicy(element, scrolleable);                                       // state only (no drawing): always
      if(redrew) DrawScrollBars(element, scrolleable, canvas, x_position, y_position); // draw only when the gutter was just
                                                                                       // cleared, else the translucent bar
                                                                                       // would be painted over itself every
                                                                                       // frame and the alpha would accumulate
    }
  
  #ifdef USERINTERFACE_DEBUG 
	Debug_Draw(element, x_position, y_position);			
  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_SKINCANVAS::GetScrollViewportSize(UI_ELEMENT* element, double& width, double& height)
* @brief      Returns the scroll viewport SIZE for an element. For a Form (or Menu) with a defined visiblerect this is
*             the visible window; otherwise (and for non-Form scrolleables) it is the element boundary line. The box
*             POSITION is left to each call site (it already tracks scroll), so only the size needs to change.
* @ingroup    USERINTERFACE
*
* @param[in]  element : 
* @param[out] width : 
* @param[out] height : 
*
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_SKINCANVAS::GetScrollViewportSize(UI_ELEMENT* element, double& width, double& height)
{
  width  = 0.0f;
  height = 0.0f;
  if(!element) return;

  UI_ELEMENT_FORM* element_form = dynamic_cast<UI_ELEMENT_FORM*>(element);
  if(element_form && element_form->GetVisibleRect()->width > 0 && element_form->GetVisibleRect()->height > 0)
    {
      width  = element_form->GetVisibleRect()->width;
      height = element_form->GetVisibleRect()->height;
    }
   else
    {
      width  = element->GetBoundaryLine()->width;
      height = element->GetBoundaryLine()->height;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_SKINCANVAS::ResolveScrollPolicy(UI_ELEMENT* element, UI_PROPERTY_SCROLLEABLE* scrolleable)
* @brief      Measures the content extent of a scrollable container and resolves its scroll policy (active/visible/limit).
* @note       Called at draw time (not only at layout) because children can be added after the layout pass (e.g. menu
*             options created from code). The content extent is the bounding SPAN of the children rectangles
*             (max far edge - min near edge), which is robust to the per-element y/x convention (a constant offset
*             cancels out) and to whether children stack upward or downward.
* @ingroup    USERINTERFACE
*
* @param[in]  element :
* @param[in]  scrolleable :
*
* @return     bool : true if is succesful.
*
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::ResolveScrollPolicy(UI_ELEMENT* element, UI_PROPERTY_SCROLLEABLE* scrolleable)
{
  if(!element || !scrolleable) return false;

  UI_ELEMENT_TEXTBOX* element_textbox = dynamic_cast<UI_ELEMENT_TEXTBOX*>(element);
  if(element_textbox)
    {
      // TextBox content is text, not compose elements: the scroll extent is the total laid-out text height (set by
      // TextBox_GenerateLines during draw). Text wraps to the box width, so there is no horizontal scroll.
      double tb_w = 0.0f;
      double tb_h = 0.0f;
      GetScrollViewportSize(element, tb_w, tb_h);

      double vp_w = scrolleable->Scroll_GetContentViewport(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL, tb_w);
      double vp_h = scrolleable->Scroll_GetContentViewport(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL,   tb_h);

      scrolleable->Scroll_ResolvePolicy(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL, vp_w, vp_w);
      scrolleable->Scroll_ResolvePolicy(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL,   vp_h, element_textbox->GetContentHeight());
      return true;
    }

  bool   has  = false;
  double minx = 0.0f;
  double maxx = 0.0f;
  double miny = 0.0f;
  double maxy = 0.0f;

  for(XDWORD c=0; c<element->GetComposeElements()->GetSize(); c++)
    {
      UI_ELEMENT* subelement = element->GetComposeElements()->Get(c);
      if(!subelement) continue;

      double sx = GetPositionWithoutDefine(subelement->GetBoundaryLine()->x);
      double sy = GetPositionWithoutDefine(subelement->GetBoundaryLine()->y);
      double sw = subelement->GetBoundaryLine()->width;
      double sh = subelement->GetBoundaryLine()->height;

      if(!has)
        {
          minx = sx;   maxx = sx + sw;
          miny = sy;   maxy = sy + sh;
          has  = true;
        }
       else
        {
          if(sx      < minx) minx = sx;
          if(sx + sw > maxx) maxx = sx + sw;
          if(sy      < miny) miny = sy;
          if(sy + sh > maxy) maxy = sy + sh;
        }
    }

  double box_w = 0.0f;
  double box_h = 0.0f;
  GetScrollViewportSize(element, box_w, box_h);

  double vp_w = scrolleable->Scroll_GetContentViewport(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL, box_w);
  double vp_h = scrolleable->Scroll_GetContentViewport(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL,   box_h);

  // Content extent vs viewport [0 .. viewport]. When the content overflows, the scroll range is the overflow PLUS the
  // content's natural leading margin mirrored at the trailing edge, so the first and last items are framed the same
  // way (symmetric margins). When the content fits, extent == viewport (no scroll). Convention-robust: a constant
  // offset cancels in min/max.
  double content_w = vp_w;
  double content_h = vp_h;

  if(has)
    {
      double over_left   = (minx < 0.0f) ? (-minx)        : 0.0f;   // content past the near edge (overflow)
      double over_right  = (maxx > vp_w) ? (maxx - vp_w)  : 0.0f;   // content past the far edge
      double lead_margin = (vp_w > maxx) ? (vp_w - maxx)  : 0.0f;   // natural gap before content, mirrored after it
      if(over_left > 0.0f || over_right > 0.0f) content_w = vp_w + over_left + over_right + lead_margin;

      double over_below  = (miny < 0.0f) ? (-miny)        : 0.0f;
      double over_above  = (maxy > vp_h) ? (maxy - vp_h)  : 0.0f;
      double top_margin  = (vp_h > maxy) ? (vp_h - maxy)  : 0.0f;
      if(over_below > 0.0f || over_above > 0.0f) content_h = vp_h + over_below + over_above + top_margin;
    }

  scrolleable->Scroll_ResolvePolicy(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL, vp_w, content_w);
  scrolleable->Scroll_ResolvePolicy(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL,   vp_h, content_h);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_SKINCANVAS::DrawScrollBars(UI_ELEMENT* element, UI_PROPERTY_SCROLLEABLE* scrolleable, GRP2DCANVAS* canvas, double x_position, double y_position)
* @brief      Draws the overlay scrollbars (track + proportional thumb) of a scrollable container.
* @note       Overlay model: drawn after the content clip is restored and pinned to the container viewport (it does
*             NOT receive the container's own scroll displacement). The thumb length is the visible fraction of the
*             content (viewport / contentextent) and its position is displacement / limit. Read-only for now;
*             interaction (drag/paging) is added in the next layer without changing this geometry.
* @ingroup    USERINTERFACE
*
* @param[in]  element :
* @param[in]  scrolleable :
* @param[in]  canvas :
* @param[in]  x_position :
* @param[in]  y_position :
*
* @return     bool : true if is succesful.
*
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::DrawScrollBars(UI_ELEMENT* element, UI_PROPERTY_SCROLLEABLE* scrolleable, GRP2DCANVAS* canvas, double x_position, double y_position)
{
  if(!element || !scrolleable || !canvas) return false;

  double box_w      = 0.0f;
  double box_h      = 0.0f;
  GetScrollViewportSize(element, box_w, box_h);
  double box_left   = x_position;
  double box_right  = x_position + box_w;
  double box_bottom = y_position;                                       // larger screen-y
  double box_top    = UI_BOUNDARYLINE_EdgeTop(y_position, box_h);       // smaller screen-y
  double radius     = (double)element->GetRoundRect();                  // shorten the bar to clear rounded corners

  double track_left;
  double track_top;
  double track_right;
  double track_bottom;
  int    tr_r;
  int    tr_g;
  int    tr_b;
  int    tr_a;
  int    th_r;
  int    th_g;
  int    th_b;
  int    th_a;

  // ---- vertical bar (right gutter) ----
  if(scrolleable->Scroll_GetTrackRect(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL, box_left, box_top, box_right, box_bottom, radius, track_left, track_top, track_right, track_bottom))
    {
      double tracklen  = track_bottom - track_top;
      double viewport  = scrolleable->Scroll_GetContentViewport(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL, box_h);
      double limit     = scrolleable->Scroll_GetLimit(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL);            // <= 0
      double disp      = scrolleable->Scroll_GetDisplacement(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL);
      double content   = viewport - limit;                                                              // >= viewport
      double fraction  = (limit != 0.0f) ? (disp / limit) : 0.0f;

      if(fraction < 0.0f) fraction = 0.0f;
      if(fraction > 1.0f) fraction = 1.0f;

      double thumb     = (content > 0.0f) ? (tracklen * (viewport / content)) : tracklen;
      if(thumb < UI_SKINCANVAS_SCROLLBAR_MINTHUMB) thumb = UI_SKINCANVAS_SCROLLBAR_MINTHUMB;
      if(thumb > tracklen)                         thumb = tracklen;

      double thumb_top = track_top + (fraction * (tracklen - thumb));

      scrolleable->Scroll_GetBarTrackColor(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL, tr_r, tr_g, tr_b, tr_a);
      scrolleable->Scroll_GetBarThumbColor(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL, th_r, th_g, th_b, th_a);

      GRP2DCOLOR_RGBA8 trackcolor(tr_r, tr_g, tr_b, tr_a);
      GRP2DCOLOR_RGBA8 thumbcolor(th_r, th_g, th_b, th_a);

      canvas->SetLineWidth(0.0f);
      canvas->SetLineColor(&trackcolor);
      canvas->SetFillColor(&trackcolor);
      DrawScrollPill(canvas, track_left, track_bottom, track_right, track_top, radius);

      canvas->SetLineColor(&thumbcolor);
      canvas->SetFillColor(&thumbcolor);
      DrawScrollPill(canvas, track_left, thumb_top + thumb, track_right, thumb_top, radius);
    }

  // ---- horizontal bar (bottom gutter) ----
  if(scrolleable->Scroll_GetTrackRect(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL, box_left, box_top, box_right, box_bottom, radius, track_left, track_top, track_right, track_bottom))
    {
      double tracklen  = track_right - track_left;
      double viewport  = scrolleable->Scroll_GetContentViewport(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL, box_w);
      double limit     = scrolleable->Scroll_GetLimit(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL);
      double disp      = scrolleable->Scroll_GetDisplacement(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL);
      double content   = viewport - limit;
      double fraction  = (limit != 0.0f) ? (disp / limit) : 0.0f;

      if(fraction < 0.0f) fraction = 0.0f;
      if(fraction > 1.0f) fraction = 1.0f;

      double thumb     = (content > 0.0f) ? (tracklen * (viewport / content)) : tracklen;
      if(thumb < UI_SKINCANVAS_SCROLLBAR_MINTHUMB) thumb = UI_SKINCANVAS_SCROLLBAR_MINTHUMB;
      if(thumb > tracklen)                         thumb = tracklen;

      double thumb_left = track_left + (fraction * (tracklen - thumb));

      scrolleable->Scroll_GetBarTrackColor(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL, tr_r, tr_g, tr_b, tr_a);
      scrolleable->Scroll_GetBarThumbColor(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL, th_r, th_g, th_b, th_a);

      GRP2DCOLOR_RGBA8 trackcolor(tr_r, tr_g, tr_b, tr_a);
      GRP2DCOLOR_RGBA8 thumbcolor(th_r, th_g, th_b, th_a);

      canvas->SetLineWidth(0.0f);
      canvas->SetLineColor(&trackcolor);
      canvas->SetFillColor(&trackcolor);
      DrawScrollPill(canvas, track_left, track_bottom, track_right, track_top, radius);

      canvas->SetLineColor(&thumbcolor);
      canvas->SetFillColor(&thumbcolor);
      DrawScrollPill(canvas, thumb_left, track_bottom, thumb_left + thumb, track_top, radius);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_SKINCANVAS::TextBox_SizeLine(UI_ELEMENT_TEXTBOX* element_textbox, GRP2DCANVAS* canvas, double x_position, double y_position, int nline, XSTRING& characterstr, XDWORD index_char, XVECTOR<UI_SKIN_TEXTBOX_PART*>& parts)
* @brief      Text box size line
* @ingroup    USERINTERFACE
* 
* @param[in]  element_textbox : 
* @param[in]  canvas : 
* @param[in]  x_position : 
* @param[in]  y_position : 
* @param[in]  nline : 
* @param[in]  characterstr : 
* @param[in]  index_char : 
* @param[in]  parts : 
* 
* @return     double : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
double UI_SKINCANVAS::TextBox_SizeLine(UI_ELEMENT_TEXTBOX* element_textbox, GRP2DCANVAS* canvas, double x_position, double y_position, int nline, XSTRING& characterstr, XDWORD index_char, XVECTOR<UI_SKIN_TEXTBOX_PART*>& parts)
{
  XSTRING _characterstr;
  XDWORD  y_pos    = 0;    
  double  sizeline = 0;

  _characterstr = characterstr;
                               
  for(XDWORD c=0; c<parts.GetSize(); c++)
    {
      UI_SKIN_TEXTBOX_PART* textbox_part = parts.Get(c);
      if(textbox_part->GetLineNumber() == nline)
        {
          sizeline += canvas->VectorFont_GetWidth(textbox_part->GetText()->Get());    
          y_pos     = textbox_part->GetYPos();    
        }                                      
    }

  _characterstr.Add(element_textbox->GetText()->Get()[index_char]); 
  sizeline += canvas->VectorFont_GetWidth(_characterstr.Get());  

  return sizeline;
}


#define UI_SKINCANVAS_TEXTBOX_MAXINLINEIMAGES   32

// Inline-image obstacle resolver for the text box. Active images are rectangles the running text must flow around.
// For a line whose vertical band [ltop,lbot] overlaps an image, this advances the cursor past any image it currently
// sits inside (jumpx), and reports the nearest image edge still ahead on that line (aheadleft/aheadright) so a word is
// never laid down on top of the picture. With no active images the outputs leave the cursor untouched (aheadleft < 0),
// so the layout is byte-for-byte the original behaviour.
static void UI_SkinCanvas_TextObstacle(double x, double ltop, double lbot,
                                       const double* ol, const double* orr, const double* ot, const double* ob, int n,
                                       double& jumpx, double& aheadleft, double& aheadright)
{
  jumpx = x;

  bool changed = true;
  while(changed)
    {
      changed = false;
      for(int i=0; i<n; i++)
        {
          if((lbot > ot[i]) && (ltop < ob[i]) && (jumpx >= ol[i]) && (jumpx < orr[i])) { jumpx = orr[i]; changed = true; }
        }
    }

  aheadleft  = -1.0f;
  aheadright = -1.0f;

  bool   has  = false;
  double best = 0.0f;
  for(int i=0; i<n; i++)
    {
      if((lbot > ot[i]) && (ltop < ob[i]) && (ol[i] > jumpx))
        {
          if(!has || (ol[i] < best)) { best = ol[i]; aheadleft = ol[i]; aheadright = orr[i]; has = true; }
        }
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::TextBox_GenerateLines(UI_ELEMENT_TEXTBOX* element_textbox, GRP2DCANVAS* canvas, double x_position, double y_position, XVECTOR<UI_SKIN_TEXTBOX_PART*>& parts)
* @brief      Text box generate lines
* @ingroup    USERINTERFACE
*
* @param[in]  element_textbox : 
* @param[in]  canvas : 
* @param[in]  x_position : 
* @param[in]  y_position : 
* @param[in]  parts : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::TextBox_GenerateLines(UI_ELEMENT_TEXTBOX* element_textbox, GRP2DCANVAS* canvas, double x_position, double y_position, XVECTOR<UI_SKIN_TEXTBOX_PART*>& parts)
{
  double                  x_text_position = x_position;
  double                  y_text_position = UI_BOUNDARYLINE_EdgeTop(y_position, element_textbox->GetBoundaryLine()->height) + (double)canvas->VectorFont_GetHeight(__L("A")) + element_textbox->Scroll_GetDisplacement(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL);  
  XDWORD                  index_char      = 0;  
  UI_COLOR                actual_color;
  XDWORD                  nline           = 1;
  double                  lineheight      = (canvas->VectorFont_GetHeight(__L("A")) + element_textbox->GetLineSpacing());
  XSTRING                 text;
  UI_SKIN_TEXTBOX_PART*   textbox_part    = NULL;
  bool                    outlimit        = false;
  bool                    newpart         = false;  

  double                  fontheight      = (double)canvas->VectorFont_GetHeight(__L("A"));
  double                  box_right       = (double)element_textbox->GetXPosition() + (double)element_textbox->GetBoundaryLine()->width;

  double                  obs_l[UI_SKINCANVAS_TEXTBOX_MAXINLINEIMAGES];   // active inline-image rectangles (text flows around them)
  double                  obs_r[UI_SKINCANVAS_TEXTBOX_MAXINLINEIMAGES];
  double                  obs_t[UI_SKINCANVAS_TEXTBOX_MAXINLINEIMAGES];
  double                  obs_b[UI_SKINCANVAS_TEXTBOX_MAXINLINEIMAGES];
  int                     n_obs           = 0;
  double                  max_obs_bottom  = 0.0f;
  double                  firstline_top   = y_text_position - fontheight;   // top of line 1 (scroll-independent reference for image content height)

  text = element_textbox->GetText()->Get();
  if(text.IsEmpty()) { element_textbox->SetContentHeight(0.0f); return false; }

  actual_color.CopyFrom(element_textbox->GetColor());

  textbox_part = GEN_NEW UI_SKIN_TEXTBOX_PART();
  if(!textbox_part) return false;
   
  textbox_part->SetLineNumber(nline);
  textbox_part->SetXPos((XDWORD)x_text_position);  
  textbox_part->SetYPos((XDWORD)y_text_position);  
  textbox_part->GetColor()->CopyFrom(&actual_color);
  
  while(index_char < text.GetSize() && (!outlimit))  
    {
      // flow the text around active inline images: if the cursor lands inside an image band on this line, jump past it
      if(n_obs)
        {
          double jumpx       = x_text_position;
          double aheadleft   = -1.0f;
          double aheadright  = -1.0f;
          UI_SkinCanvas_TextObstacle(x_text_position, y_text_position - fontheight, y_text_position,
                                     obs_l, obs_r, obs_t, obs_b, n_obs, jumpx, aheadleft, aheadright);
          if(jumpx > x_text_position)
            {
              if(!textbox_part->GetText()->IsEmpty())
                {
                  textbox_part->SetWidth((XDWORD)canvas->VectorFont_GetWidth(textbox_part->GetText()->Get()));
                  textbox_part->SetHeight((XDWORD)canvas->VectorFont_GetHeight(__L("A")));
                  parts.Add(textbox_part);
                  textbox_part = GEN_NEW UI_SKIN_TEXTBOX_PART();
                  if(!textbox_part) return false;
                }
              x_text_position = jumpx;
              textbox_part->SetLineNumber(nline);
              textbox_part->SetXPos((XDWORD)x_text_position);
              textbox_part->SetYPos((XDWORD)y_text_position);
              textbox_part->GetColor()->CopyFrom(&actual_color);
            }
        }

      XCHAR character = text.Get()[index_char];

      switch(character)
        {
          case __C('\n')  : x_text_position  = x_position;      
                            y_text_position += lineheight;                
                            nline++;
                            newpart = true;     

                            index_char++;                            
                            break;

          case __C('!')   : { int indexstart = index_char+1;
                              if(text.Get()[indexstart] == __C('['))
                                {                
                                  int type;
                
                                  int indexend = text.FindCharacter(__C(']'), indexstart);
                                  if(indexend != XSTRING_NOTFOUND)
                                    {                                                                      
                                      XCHAR*    keywords[] = {  __L("COLOR") ,                                                              
                                                                __L("END") ,
                                                                __L("IMAGE") ,
                                                             };
                                      XSTRING   keyword;
                                      bool      found_keyword = false;  
                                                                                    
                                      text.Copy(indexstart+1, indexend, keyword);    

                                      for(type=0; type<(sizeof(keywords)/sizeof(XCHAR*)); type++)
                                        {
                                          int keywordindex = keyword.Find(keywords[type], true);
                                          if(!keywordindex)
                                            {
                                              XSTRING keyword_params;
                                              XSTRING string;        
                                              
                                              found_keyword = true;  

                                              string = keywords[type];
                                              keywordindex += string.GetSize()+1;

                                              keyword.Copy(keywordindex, keyword_params);
                                              keyword_params.DeleteCharacter(__C(' '));

                                              switch(type)
                                                {
                                                  case  0 : actual_color.SetFromString(keyword_params);
                                                            newpart = true;
                                                            break;      

                                                  case  1 : for(int d=0; d<(sizeof(keywords)/sizeof(XCHAR*)); d++)
                                                              {
                                                                if(!keyword_params.Compare(keywords[d], true))
                                                                  {
                                                                    switch(d)
                                                                      {
                                                                        case  0 : actual_color.CopyFrom(element_textbox->GetColor());
                                                                                  newpart = true;
                                                                                  break;

                                                                        case  1 : break;  

                                                                        case  2 : break;   // "END IMAGE" -> no-op (images are single inline insertions)
                                                                      }                                                                    
                                                                  }
                                                              }
                                                            break;                                                          

                                                  case  2 : { // IMAGE <file> : insert an inline image; following text flows after it (wrapping to the next line)
                                                              GRPBITMAP*    bitmap = NULL;
                                                              UI_ANIMATION* anim   = GEN_USERINTERFACE.GetOrAddAnimationCache(UI_SKIN_DRAWMODE_CANVAS, canvas->GetMode(), __L(""), keyword_params.Get());
                                                              if(anim) bitmap = anim->GetBitmap();

                                                              if(bitmap)
                                                                {
                                                                  double imgwidth  = (double)bitmap->GetWidth();
                                                                  double imgheight = (double)bitmap->GetHeight();
                                                                  double sep       = (double)element_textbox->GetImageSeparation();   // gap kept around the graphic (all edges)

                                                                  // close the current text run (the text that precedes the image)
                                                                  if(!textbox_part->GetText()->IsEmpty())
                                                                    {
                                                                      textbox_part->SetWidth((XDWORD)canvas->VectorFont_GetWidth(textbox_part->GetText()->Get()));
                                                                      textbox_part->SetHeight((XDWORD)canvas->VectorFont_GetHeight(__L("A")));
                                                                      parts.Add(textbox_part);
                                                                      textbox_part = GEN_NEW UI_SKIN_TEXTBOX_PART();
                                                                    }

                                                                  // wrap to a new line if the image (plus its side margins) does not fit on the current one
                                                                  if((x_text_position > x_position) && ((x_text_position + sep + imgwidth + sep) > box_right))
                                                                    {
                                                                      x_text_position  = x_position;
                                                                      y_text_position += lineheight;
                                                                      nline++;
                                                                    }

                                                                  // The image grows DOWNWARD from the top of the current line (so it never paints over the line above).
                                                                  // It is drawn at its real rectangle, but the obstacle the text flows around is inflated by 'sep' on every
                                                                  // edge, so a configurable gap is kept between the graphic and the text on all four sides.
                                                                  double img_left = x_text_position + sep;
                                                                  double img_top  = y_text_position - fontheight;
                                                                  double img_bot  = img_top + imgheight;

                                                                  if(textbox_part)
                                                                    {
                                                                      UI_SKIN_TEXTBOX_PART* imagepart = GEN_NEW UI_SKIN_TEXTBOX_PART();
                                                                      if(imagepart)
                                                                        {
                                                                          imagepart->SetImage(bitmap);
                                                                          imagepart->SetLineNumber(nline);
                                                                          imagepart->SetXPos((XDWORD)img_left);
                                                                          imagepart->SetYPos((XDWORD)img_bot);     // drawn at ypos - height = img_top (downward from the line)
                                                                          imagepart->SetWidth((XDWORD)imgwidth);
                                                                          imagepart->SetHeight((XDWORD)imgheight);
                                                                          parts.Add(imagepart);
                                                                        }
                                                                    }

                                                                  if(n_obs < UI_SKINCANVAS_TEXTBOX_MAXINLINEIMAGES)
                                                                    {
                                                                      obs_l[n_obs] = img_left - sep;             // left margin   (= x_text_position)
                                                                      obs_r[n_obs] = img_left + imgwidth + sep;  // right margin
                                                                      obs_t[n_obs] = img_top;                   // top aligns with the line (image grows downward; the line above stays free)
                                                                      obs_b[n_obs] = img_bot + sep;             // bottom margin (keeps the line below clear)
                                                                      n_obs++;
                                                                    }
                                                                  if((img_bot + sep) > max_obs_bottom) max_obs_bottom = img_bot + sep;

                                                                  x_text_position = img_left + imgwidth;     // following text re-flows past the image (the right margin is applied by the obstacle jump)

                                                                  // re-anchor the (empty) current part for the text that follows the image on this line
                                                                  if(textbox_part)
                                                                    {
                                                                      textbox_part->SetLineNumber(nline);
                                                                      textbox_part->SetXPos((XDWORD)x_text_position);
                                                                      textbox_part->SetYPos((XDWORD)y_text_position);
                                                                      textbox_part->GetColor()->CopyFrom(&actual_color);
                                                                    }

                                                                  newpart = false;
                                                                }
                                                            }
                                                            break;
                                                }
                                              
                                              break;
                                            }
                                        }
                                                                  
                                      if(found_keyword) 
                                        {    
                                          text.DeleteCharacters(index_char, (keyword.GetSize() + 3));                                         
                                          
                                                        
                                          if(type == 1 && (indexstart > 1))
                                            {
                                              indexstart = -2;

                                              if(text.Get()[indexstart] != __C(' '))  
                                                {
                                                  text.Insert(__L(" "), indexstart);                                                                                                  
                                                }
                                            }
                                            
                                          break;
                                        }
                                    }
                                } 
                            }

                  default : { XSTRING characterstr;                              
                              characterstr.Add(character);
                              
                              double            charwidth = (double)canvas->VectorFont_GetWidth(characterstr.Get());
                              double            xbefore   = x_text_position;
                              x_text_position += charwidth; 

                              UI_BOUNDARYLINE   boundaryline;
                              bool              isinbox    = false;
                                
                              boundaryline.x      = element_textbox->GetXPosition();
                              boundaryline.y      = element_textbox->GetYPosition();
                              boundaryline.width  = element_textbox->GetBoundaryLine()->width;
                              boundaryline.height = element_textbox->GetBoundaryLine()->height;

                              isinbox = boundaryline.IsWithin((XDWORD)(x_text_position), (XDWORD)(y_text_position));                                                                                        

                              // would this character paint on top of an inline image ahead? jump past it (or wrap if the image reaches the margin)
                              bool jumpedimage = false;
                              if(isinbox && n_obs)
                                {
                                  double jx         = xbefore;
                                  double aheadleft  = -1.0f;
                                  double aheadright = -1.0f;
                                  UI_SkinCanvas_TextObstacle(xbefore, y_text_position - fontheight, y_text_position,
                                                             obs_l, obs_r, obs_t, obs_b, n_obs, jx, aheadleft, aheadright);
                                  if((aheadleft >= 0.0f) && (x_text_position > aheadleft))
                                    {
                                      if(aheadright < box_right)
                                        {
                                          if(!textbox_part->GetText()->IsEmpty())
                                            {
                                              textbox_part->SetWidth((XDWORD)canvas->VectorFont_GetWidth(textbox_part->GetText()->Get()));
                                              textbox_part->SetHeight((XDWORD)canvas->VectorFont_GetHeight(__L("A")));
                                              parts.Add(textbox_part);
                                              textbox_part = GEN_NEW UI_SKIN_TEXTBOX_PART();
                                              if(!textbox_part) return false;
                                            }
                                          x_text_position = aheadright;
                                          textbox_part->SetLineNumber(nline);
                                          textbox_part->SetXPos((XDWORD)x_text_position);
                                          textbox_part->SetYPos((XDWORD)y_text_position);
                                          textbox_part->GetColor()->CopyFrom(&actual_color);
                                          jumpedimage = true;       // reprocess this character after the image (do not consume it)
                                        }
                                       else
                                        {
                                          isinbox = false;          // image runs to the right margin -> wrap to the next line
                                        }
                                    }
                                }

                              if(jumpedimage)
                                {
                                  // character intentionally not consumed; it will be laid down after the image on the next pass
                                }
                               else if(isinbox)
                                {                                        
                                  textbox_part->GetText()->Add(characterstr);
                                  index_char++;
                                }
                               else
                                {  
                                  if(element_textbox->IsWordClipping())
                                    {      
                                      while(!textbox_part->GetText()->IsEmpty())
                                        {
                                          if(textbox_part->GetText()->Get()[textbox_part->GetText()->GetSize()-1] == __C(' ')) break;
                                          textbox_part->GetText()->DeleteLastCharacter();
                                          index_char--;
                                        }          
                                    }
                                    
                                  x_text_position  = x_position;      
                                  y_text_position += lineheight; 
                                  nline++;

                                  if(!textbox_part->GetText()->IsEmpty()) 
                                    {
                                      newpart = true;                                      
                                    }
                                   else
                                    {
                                      textbox_part->SetLineNumber(nline);
                                      textbox_part->SetXPos((XDWORD)x_text_position);  
                                      textbox_part->SetYPos((XDWORD)y_text_position);  
                                    }
                                }                               
                            }
                            break;
        }


      if(newpart)
        {
          if(!textbox_part->GetText()->IsEmpty())    
            {
              textbox_part->SetWidth((XDWORD)canvas->VectorFont_GetWidth(textbox_part->GetText()->Get()));
              textbox_part->SetHeight((XDWORD)canvas->VectorFont_GetHeight(__L("A")));
              parts.Add(textbox_part);  

              textbox_part = GEN_NEW UI_SKIN_TEXTBOX_PART();
              if(!textbox_part) return false;
            }
   
          textbox_part->SetLineNumber(nline);
          textbox_part->SetXPos((XDWORD)x_text_position);  
          textbox_part->SetYPos((XDWORD)y_text_position);  
          textbox_part->GetColor()->CopyFrom(&actual_color);

          newpart = false;
        }
    }

  if(outlimit)
    {
      GEN_DELETE textbox_part;
      textbox_part = NULL;
    }
   else
    {
      parts.Add(textbox_part);  
    }

  XVECTOR<UI_SKIN_TEXTBOX_PART*>    checkparts;
  XDWORD                            maxnlines   = 0;

  for(XDWORD c=0; c<parts.GetSize(); c++)
    {
      UI_SKIN_TEXTBOX_PART* textbox_part = parts.Get(c);
      if(textbox_part) maxnlines = __MAX(maxnlines, textbox_part->GetLineNumber());  
    }

  for(XDWORD nl=0; nl<maxnlines; nl++)
    {     
      for(XDWORD p=0; p<parts.GetSize(); p++)
        {          
          UI_SKIN_TEXTBOX_PART* textbox_part = parts.Get(p);        
          if(textbox_part) 
            {
              if(textbox_part->GetLineNumber() == nl+1) checkparts.Add(textbox_part);
            }
        }

      if(checkparts.GetSize())
        {  
          switch(element_textbox->GetTextAlignment())
            {             
                                      default   : break;

              case UI_ELEMENT_TYPE_ALIGN_LEFT	  : break;										

              case UI_ELEMENT_TYPE_ALIGN_RIGHT	:	break;																				                  

              case UI_ELEMENT_TYPE_ALIGN_CENTER	: { // a line that flows around an inline image is kept left-aligned, so the wrap stays aligned to the picture
                                                    bool constrained = false;
                                                    if(n_obs)
                                                      {
                                                        for(XDWORD p=0; p<checkparts.GetSize() && (!constrained); p++)
                                                          {
                                                            UI_SKIN_TEXTBOX_PART* cp = checkparts.Get(p);
                                                            if(!cp) continue;
                                                            double ly = (double)cp->GetYPos();
                                                            for(int i=0; i<n_obs; i++)
                                                              {
                                                                if((ly > obs_t[i]) && ((ly - fontheight) < obs_b[i])) { constrained = true; break; }
                                                              }
                                                          }
                                                      }

                                                    if(!constrained)
                                                      {
                                                        XDWORD sizealltext = 0;
                                                        for(XDWORD p=0; p<checkparts.GetSize(); p++)
                                                          {          
                                                            UI_SKIN_TEXTBOX_PART* textbox_part = checkparts.Get(p);        
                                                            if(textbox_part) sizealltext += textbox_part->GetWidth();
                                                          }
                                          
                                                        XDWORD shift = (XDWORD)(element_textbox->GetBoundaryLine()->width - sizealltext)/2;    

                                                        for(XDWORD p=0; p<checkparts.GetSize(); p++)
                                                          {          
                                                            UI_SKIN_TEXTBOX_PART* textbox_part = checkparts.Get(p);        
                                                            if(textbox_part) textbox_part->SetXPos(textbox_part->GetXPos()+shift); 
                                                          }
                                                      }
                                                  }       
                                                  break;										           
            }              
        }

      checkparts.DeleteAll();                 
    }

  {
    double textheight  = (double)maxnlines * lineheight;
    double imageextent = max_obs_bottom - firstline_top;                  // image bottom relative to the top of line 1 (scroll cancels out)
    element_textbox->SetContentHeight(__MAX(textheight, imageextent));    // make sure scrolling can reach the bottom of an image too
  }

  return true;
}


#ifdef USERINTERFACE_DEBUG 

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::Debug_Draw(UI_ELEMENT* element, double x_position, double y_position)
* @brief      Debug draw
* @ingroup    USERINTERFACE
*
* @param[in]  element : 
* @param[in]  x_position : 
* @param[in]  y_position : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::Debug_Draw(UI_ELEMENT* element, double x_position, double y_position)
{  
  GRP2DCANVAS* canvas = GetCanvas();
  if(!canvas) return false;
  
  GRP2DCOLOR_RGBA8  colorred(255, 0, 0);
  GRP2DCOLOR_RGBA8  colorblue(0, 0, 255);
 
  canvas->SetLineWidth(1.0f);
  canvas->SetLineColor(&colorblue);  
  canvas->Circle(x_position, y_position, 5);               

  canvas->SetLineWidth(1.0f);
  canvas->SetLineColor(&colorred);
  canvas->Rectangle(x_position ,  
                    y_position ,
                    x_position + (element->GetBoundaryLine()->width), 
                    UI_BOUNDARYLINE_EdgeTop(y_position, element->GetBoundaryLine()->height));
  return true;
}
#endif


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_SKINCANVAS::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_SKINCANVAS::Clean()
{
  fontsize        = 0;
  screen          = NULL;
  viewportindex   = 0;
}





