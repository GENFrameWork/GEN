/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRP2DVectorFileRenderAGG.cpp
* 
* @class      GRP2DVECTORFILERENDERAGG
* @brief      Graphic Vector File Render (AGG) class : common front end that inspects the type of a GRPVECTORFILE and
*             dispatches the rendering to the matching backend (GRP2DVECTORFILEDXFRENDERAGG or GRP2DVECTORFILESVGRENDERAGG)
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "GRP2DVectorFileRenderAGG.h"

#include "GRP2DCanvas.h"
#include "GRPProperties.h"
#include "GRPFactory.h"
#include "GRPBitmap.h"

#include "GRPVectorFile.h"

#ifdef GRP_VECTOR_FILE_DXF_ACTIVE
#include "GRPVectorFileDXF.h"
#endif

#ifdef GRP_VECTOR_FILE_SVG_ACTIVE
#include "GRPVectorFileSVG.h"
#endif

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/




/**-------------------------------------------------------------------------------------------------------------------
* @fn         GRP2DVECTORFILERENDERAGG::GRP2DVECTORFILERENDERAGG()
* @brief      Constructor of class
* @ingroup    GRAPHIC
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DVECTORFILERENDERAGG::GRP2DVECTORFILERENDERAGG()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         GRP2DVECTORFILERENDERAGG::~GRP2DVECTORFILERENDERAGG()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    GRAPHIC
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DVECTORFILERENDERAGG::~GRP2DVECTORFILERENDERAGG()
{
  InvalidateCache();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILERENDERAGG::Render(GRPVECTORFILE* file, GRP2DCANVAS* canvas)
* @brief      Render : paint the vector file fitted into the whole canvas, choosing the backend by file type
* @ingroup    GRAPHIC
* @param[in]  file : loaded vector file (any supported type)
* @param[in]  canvas : target canvas
* @return     bool : true if a backend handled the file.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILERENDERAGG::Render(GRPVECTORFILE* file, GRP2DCANVAS* canvas)
{
  if(!file || !canvas) return false;

  switch(file->GetType())
    {
      case GRPVECTORFILETYPE_UNKNOWN  :
                            default    : break;

      #ifdef GRP_VECTOR_FILE_DXF_ACTIVE
      case GRPVECTORFILETYPE_DXF       : return dxfrender.Render((GRPVECTORFILEDXF*)file, canvas);
      #endif

      #ifdef GRP_VECTOR_FILE_SVG_ACTIVE
      case GRPVECTORFILETYPE_SVG       : return svgrender.Render((GRPVECTORFILESVG*)file, canvas);
      #endif
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILERENDERAGG::Render(GRPVECTORFILE* file, GRP2DCANVAS* canvas, double targetx, double targety, double targetwidth, double targetheight)
* @brief      Render : paint the vector file fitted into the target rectangle, choosing the backend by file type
* @ingroup    GRAPHIC
* @param[in]  file : loaded vector file (any supported type)
* @param[in]  canvas : target canvas
* @param[in]  targetx : target rectangle x
* @param[in]  targety : target rectangle y
* @param[in]  targetwidth : target rectangle width
* @param[in]  targetheight : target rectangle height
* @return     bool : true if a backend handled the file.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILERENDERAGG::Render(GRPVECTORFILE* file, GRP2DCANVAS* canvas, double targetx, double targety, double targetwidth, double targetheight)
{
  if(!file || !canvas) return false;

  switch(file->GetType())
    {
      case GRPVECTORFILETYPE_UNKNOWN  :
                            default    : break;

      #ifdef GRP_VECTOR_FILE_DXF_ACTIVE
      case GRPVECTORFILETYPE_DXF       : return dxfrender.Render((GRPVECTORFILEDXF*)file, canvas, targetx, targety, targetwidth, targetheight);
      #endif

      #ifdef GRP_VECTOR_FILE_SVG_ACTIVE
      case GRPVECTORFILETYPE_SVG       : return svgrender.Render((GRPVECTORFILESVG*)file, canvas, targetx, targety, targetwidth, targetheight);
      #endif
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILERENDERAGG::RenderCached(GRPVECTORFILE* file, GRP2DCANVAS* canvas, double targetx, double targety, double targetwidth, double targetheight)
* @brief      Render cached : rasterize any supported vector file once into a transparent offscreen bitmap and blit it
*             on every frame. The cache is owned by this common front end; SVG and DXF backends only render vectors.
* @ingroup    GRAPHIC
* @param[in]  file : loaded vector file (any supported type)
* @param[in]  canvas : target canvas
* @param[in]  targetx : target rectangle x
* @param[in]  targety : target rectangle y
* @param[in]  targetwidth : target rectangle width
* @param[in]  targetheight : target rectangle height
* @return     bool : true if a backend handled the file.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILERENDERAGG::RenderCached(GRPVECTORFILE* file, GRP2DCANVAS* canvas, double targetx, double targety, double targetwidth, double targetheight)
{
  if(!file || !canvas) return false;
  if((targetwidth <= 0.0) || (targetheight <= 0.0)) return false;

  bool supported = false;

  switch(file->GetType())
    {
      case GRPVECTORFILETYPE_UNKNOWN  :
                            default    : break;

      #ifdef GRP_VECTOR_FILE_DXF_ACTIVE
      case GRPVECTORFILETYPE_DXF       : supported = true; break;
      #endif

      #ifdef GRP_VECTOR_FILE_SVG_ACTIVE
      case GRPVECTORFILETYPE_SVG       : supported = true; break;
      #endif
    }

  if(!supported) return false;

  XPATH*                         vectorfontpathfile = canvas->VectorFont_GetPathFile();
  GRP2DCANVAS_VECTORFONT_CONFIG* vectorfontconfig   = canvas->Vectorfont_GetConfig();
  bool                           vectorkerning      = vectorfontconfig ? vectorfontconfig->IsKerning() : false;

  bool samefontpath = vectorfontpathfile ? (!cachevectorfontpathfile.Compare((*vectorfontpathfile))) : cachevectorfontpathfile.IsEmpty();

  bool samecache = cachevalid && cachebitmap &&
                   (cachefile     == file) &&
                   (cachefiletype == (int)file->GetType()) &&
                   (cachewidth    == targetwidth) &&
                   (cacheheight   == targetheight) &&
                   samefontpath &&
                   (cachevectorkerning == vectorkerning);

  if(!samecache)
    {
      InvalidateCache();

      if(!BuildCacheBitmap(file, canvas, targetwidth, targetheight)) return false;

      cachefile     = file;
      cachefiletype = (int)file->GetType();
      cachewidth    = targetwidth;
      cacheheight   = targetheight;

      if(vectorfontpathfile) cachevectorfontpathfile = (*vectorfontpathfile);
        else                 cachevectorfontpathfile.Empty();

      cachevectorkerning = vectorkerning;
      cachevalid         = (cachebitmap != NULL);
    }

  if(cachebitmap) canvas->PutBitmap(targetx, targety, cachebitmap);           // blit the cached sprite (per pixel alpha)

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILERENDERAGG::BuildCacheBitmap(GRPVECTORFILE* file, GRP2DCANVAS* referencecanvas, double width, double height)
* @brief      Build cache bitmap : rasterize the vector file into a transparent offscreen canvas and capture it
* @note       INTERNAL. Common CANVAS cache path shared by SVG and DXF.
* @ingroup    GRAPHIC
* @param[in]  file : loaded vector file
* @param[in]  referencecanvas : destination canvas (its pixel format is reused for the offscreen)
* @param[in]  width : sprite width
* @param[in]  height : sprite height
* @return     bool : true if the cache bitmap was built.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILERENDERAGG::BuildCacheBitmap(GRPVECTORFILE* file, GRP2DCANVAS* referencecanvas, double width, double height)
{
  if(!file || !referencecanvas) return false;
  if((width <= 0.0) || (height <= 0.0)) return false;

  GRPPROPERTIES properties;
  properties.CopyPropertysFrom(referencecanvas);                              // same pixel format as the destination
  properties.SetPosition(0, 0);
  properties.SetSize((XDWORD)width, (XDWORD)height);

  GRP2DCANVAS* offscreen = GEN_GRPFACTORY.CreateCanvas(&properties);
  if(!offscreen) return false;

  offscreen->SetWidth((XDWORD)width);
  offscreen->SetHeight((XDWORD)height);

  if(!offscreen->Buffer_Create())
    {
      GEN_GRPFACTORY.DeleteCanvas(offscreen);
      return false;
    }

  offscreen->VectorFont_CopyFrom(referencecanvas);                            // SVG text needs the same loaded vector font as the real canvas

  GRP2DCOLOR_RGBA8 transparent(0, 0, 0, 0);
  offscreen->Clear(&transparent);                                             // transparent background

  bool status = Render(file, offscreen, 0.0, 0.0, width, height);             // rasterize into the offscreen

  if(status) cachebitmap = offscreen->GetBitmap(0.0, 0.0, width, height);     // capture with its own alpha

  GEN_GRPFACTORY.DeleteCanvas(offscreen);

  return (status && (cachebitmap != NULL));
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         void GRP2DVECTORFILERENDERAGG::InvalidateCache()
* @brief      Invalidate cache : free the common cached bitmap and force a new rasterization on the next RenderCached
* @ingroup    GRAPHIC
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DVECTORFILERENDERAGG::InvalidateCache()
{
  if(cachebitmap)
    {
      GEN_GRPFACTORY.DeleteBitmap(cachebitmap);
      cachebitmap = NULL;
    }

  cachevalid    = false;
  cachefile     = NULL;
  cachefiletype = GRPVECTORFILETYPE_UNKNOWN;
  cachewidth    = 0.0;
  cacheheight   = 0.0;
  cachevectorfontpathfile.Empty();
  cachevectorkerning = false;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILERENDERAGG::GetBackgroundIsDark()
* @brief      Get background is dark (DXF backend)
* @ingroup    GRAPHIC
* @return     bool : true if the DXF backend treats the background as dark.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILERENDERAGG::GetBackgroundIsDark()
{
  #ifdef GRP_VECTOR_FILE_DXF_ACTIVE
  return dxfrender.GetBackgroundIsDark();
  #else
  return false;
  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         void GRP2DVECTORFILERENDERAGG::SetBackgroundIsDark(bool isdark)
* @brief      Set background is dark (DXF backend; ignored for SVG)
* @ingroup    GRAPHIC
* @param[in]  isdark : true if the destination background is dark
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DVECTORFILERENDERAGG::SetBackgroundIsDark(bool isdark)
{
  #ifdef GRP_VECTOR_FILE_DXF_ACTIVE
  bool oldisdark = dxfrender.GetBackgroundIsDark();

  dxfrender.SetBackgroundIsDark(isdark);

  if(oldisdark != dxfrender.GetBackgroundIsDark()) InvalidateCache();
  #else
  (void)isdark;
  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILERENDERAGG::GetForceColorActive()
* @brief      Get force color active (DXF backend)
* @ingroup    GRAPHIC
* @return     bool : true if the DXF backend is overriding every color.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILERENDERAGG::GetForceColorActive()
{
  #ifdef GRP_VECTOR_FILE_DXF_ACTIVE
  return dxfrender.GetForceColorActive();
  #else
  return false;
  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         void GRP2DVECTORFILERENDERAGG::SetForceColor(bool active, GRP2DCOLOR_RGBA8 color)
* @brief      Set force color (DXF backend; ignored for SVG) : paint the whole drawing with a single color
* @ingroup    GRAPHIC
* @param[in]  active : true to override the file colors
* @param[in]  color : the override color
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DVECTORFILERENDERAGG::SetForceColor(bool active, GRP2DCOLOR_RGBA8 color)
{
  #ifdef GRP_VECTOR_FILE_DXF_ACTIVE
  bool oldactive = dxfrender.GetForceColorActive();

  dxfrender.SetForceColor(active, color);

  if((oldactive != dxfrender.GetForceColorActive()) || active) InvalidateCache();
  #else
  (void)active;
  (void)color;
  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         double GRP2DVECTORFILERENDERAGG::GetLineWidth()
* @brief      Get line width (DXF backend)
* @ingroup    GRAPHIC
* @return     double : the DXF backend stroke width.
* --------------------------------------------------------------------------------------------------------------------*/
double GRP2DVECTORFILERENDERAGG::GetLineWidth()
{
  #ifdef GRP_VECTOR_FILE_DXF_ACTIVE
  return dxfrender.GetLineWidth();
  #else
  return 0.0;
  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         void GRP2DVECTORFILERENDERAGG::SetLineWidth(double linewidth)
* @brief      Set line width (DXF backend; ignored for SVG)
* @ingroup    GRAPHIC
* @param[in]  linewidth : device stroke width (pixels)
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DVECTORFILERENDERAGG::SetLineWidth(double linewidth)
{
  #ifdef GRP_VECTOR_FILE_DXF_ACTIVE
  double oldlinewidth = dxfrender.GetLineWidth();

  dxfrender.SetLineWidth(linewidth);

  if(oldlinewidth != dxfrender.GetLineWidth()) InvalidateCache();
  #else
  (void)linewidth;
  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILERENDERAGG::GetDrawText()
* @brief      Get draw text (DXF backend)
* @ingroup    GRAPHIC
* @return     bool : true if the DXF backend paints TEXT / MTEXT.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILERENDERAGG::GetDrawText()
{
  #ifdef GRP_VECTOR_FILE_DXF_ACTIVE
  return dxfrender.GetDrawText();
  #else
  return false;
  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         void GRP2DVECTORFILERENDERAGG::SetDrawText(bool drawtext)
* @brief      Set draw text (DXF backend; ignored for SVG)
* @ingroup    GRAPHIC
* @param[in]  drawtext : true to paint TEXT / MTEXT (needs a vector font on the canvas)
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DVECTORFILERENDERAGG::SetDrawText(bool drawtext)
{
  #ifdef GRP_VECTOR_FILE_DXF_ACTIVE
  bool olddrawtext = dxfrender.GetDrawText();

  dxfrender.SetDrawText(drawtext);

  if(olddrawtext != dxfrender.GetDrawText()) InvalidateCache();
  #else
  (void)drawtext;
  #endif
}


#ifdef GRP_VECTOR_FILE_SVG_ACTIVE
/**-------------------------------------------------------------------------------------------------------------------
* @fn         GRP2DVECTORFILESVGRENDERAGG* GRP2DVECTORFILERENDERAGG::GetSVGRender()
* @brief      Get SVG render : direct access to the SVG backend
* @ingroup    GRAPHIC
* @return     GRP2DVECTORFILESVGRENDERAGG* : the SVG backend.
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DVECTORFILESVGRENDERAGG* GRP2DVECTORFILERENDERAGG::GetSVGRender()
{
  return &svgrender;
}
#endif


#ifdef GRP_VECTOR_FILE_DXF_ACTIVE
/**-------------------------------------------------------------------------------------------------------------------
* @fn         GRP2DVECTORFILEDXFRENDERAGG* GRP2DVECTORFILERENDERAGG::GetDXFRender()
* @brief      Get DXF render : direct access to the DXF backend
* @ingroup    GRAPHIC
* @return     GRP2DVECTORFILEDXFRENDERAGG* : the DXF backend.
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DVECTORFILEDXFRENDERAGG* GRP2DVECTORFILERENDERAGG::GetDXFRender()
{
  return &dxfrender;
}
#endif


/**-------------------------------------------------------------------------------------------------------------------
* @fn         void GRP2DVECTORFILERENDERAGG::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL. The backends initialize themselves through their own constructors.
* @ingroup    GRAPHIC
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DVECTORFILERENDERAGG::Clean()
{
  cachebitmap  = NULL;
  cachevalid   = false;
  cachefile    = NULL;
  cachefiletype = GRPVECTORFILETYPE_UNKNOWN;
  cachewidth   = 0.0;
  cacheheight  = 0.0;
  cachevectorfontpathfile.Empty();
  cachevectorkerning = false;
}
