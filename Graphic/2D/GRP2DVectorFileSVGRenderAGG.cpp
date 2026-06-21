/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRP2DVectorFileSVGRenderAGG.cpp
* 
* @class      GRP2DVECTORFILESVGRENDERAGG
* @brief      Graphic Vector File SVG Render (AGG) class : walks the SVG object tree and paints it on a canvas
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

#include "GRP2DVectorFileSVGRenderAGG.h"

#include <math.h>

#include "GRP2DCanvas.h"
#include "GRP2DColor.h"
#include "GRP2DPath.h"
#include "GRP2DGradientStop.h"
#include "GRPProperties.h"
#include "GRPFactory.h"
#include "GRPBitmap.h"

#include "GRPVectorFileSVG.h"
#include "GRPVectorFileSVGConfig.h"
#include "GRPVectorFileSVGObj.h"
#include "GRPVectorFileSVGObjUse.h"
#include "GRPVectorFileSVGObjGradient.h"
#include "GRPVectorFileSVGObjText.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/




/**-------------------------------------------------------------------------------------------------------------------
* @fn         GRP2DVECTORFILESVGRENDERAGG::GRP2DVECTORFILESVGRENDERAGG()
* @brief      Constructor of class
* @ingroup    GRAPHIC
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DVECTORFILESVGRENDERAGG::GRP2DVECTORFILESVGRENDERAGG()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         GRP2DVECTORFILESVGRENDERAGG::~GRP2DVECTORFILESVGRENDERAGG()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    GRAPHIC
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DVECTORFILESVGRENDERAGG::~GRP2DVECTORFILESVGRENDERAGG()
{
  InvalidateCache();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILESVGRENDERAGG::Render(GRPVECTORFILESVG* svg, GRP2DCANVAS* canvas)
* @brief      Render : paint the SVG fitted into the whole canvas
* @ingroup    GRAPHIC
* @param[in]  svg : loaded SVG file
* @param[in]  canvas : target canvas
* @return     bool : true if is succesful.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILESVGRENDERAGG::Render(GRPVECTORFILESVG* svg, GRP2DCANVAS* canvas)
{
  if(!canvas) return false;

  return Render(svg, canvas, 0.0, 0.0, canvas->GetWidth(), canvas->GetHeight());
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILESVGRENDERAGG::Render(GRPVECTORFILESVG* svg, GRP2DCANVAS* canvas, double targetx, double targety, double targetwidth, double targetheight)
* @brief      Render : paint the SVG fitted into the target rectangle (uniform scale, centered)
* @ingroup    GRAPHIC
* @param[in]  svg : loaded SVG file
* @param[in]  canvas : target canvas
* @param[in]  targetx : target rectangle x
* @param[in]  targety : target rectangle y
* @param[in]  targetwidth : target rectangle width
* @param[in]  targetheight : target rectangle height
* @return     bool : true if is succesful.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILESVGRENDERAGG::Render(GRPVECTORFILESVG* svg, GRP2DCANVAS* canvas, double targetx, double targety, double targetwidth, double targetheight)
{
  if(!svg || !canvas) return false;

  GRPVECTORFILESVGOBJ* root = svg->GetRoot();
  if(!root) return false;

  double savedlinewidth = canvas->GetLineWidth();                              // save canvas state

  GRPVECTORFILESVGTRANSFORM basetransform;
  CalculateViewBoxTransform(svg, targetx, targety, targetwidth, targetheight, basetransform);

  GRPVECTORFILESVGSTYLE basestyle;                                             // SVG initial values (fill black, stroke none)

  contextsvg = svg;                                                            // resolve <use> against this SVG
  usedepth   = 0;

  RenderNode(root, basetransform, basestyle, canvas);

  contextsvg = NULL;

  canvas->SetLineWidth(savedlinewidth);                                        // restore canvas state

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILESVGRENDERAGG::RenderCached(GRPVECTORFILESVG* svg, GRP2DCANVAS* canvas, double targetx, double targety, double targetwidth, double targetheight)
* @brief      Render cached : rasterize the SVG once into a transparent offscreen bitmap and, on every frame, blit that
*             bitmap (with per pixel alpha) over the destination canvas. The cached sprite keeps its own transparency,
*             so it composes correctly over any (even changing) background. Call InvalidateCache() to force a rebuild.
* @ingroup    GRAPHIC
* @param[in]  svg : loaded SVG file
* @param[in]  canvas : target canvas
* @param[in]  targetx : target rectangle x
* @param[in]  targety : target rectangle y
* @param[in]  targetwidth : target rectangle width
* @param[in]  targetheight : target rectangle height
* @return     bool : true if is succesful.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILESVGRENDERAGG::RenderCached(GRPVECTORFILESVG* svg, GRP2DCANVAS* canvas, double targetx, double targety, double targetwidth, double targetheight)
{
  if(!svg || !canvas) return false;

  bool samerect = (cachex     == targetx)     && (cachey      == targety) &&
                  (cachewidth == targetwidth) && (cacheheight == targetheight);

  if(!cachevalid || !cachebitmap || !samerect)
    {
      InvalidateCache();

      if(!BuildCacheBitmap(svg, canvas, targetwidth, targetheight)) return false;

      cachex      = targetx;
      cachey      = targety;
      cachewidth  = targetwidth;
      cacheheight = targetheight;
      cachevalid  = (cachebitmap != NULL);
    }

  if(cachebitmap)  canvas->PutBitmap(cachex, cachey, cachebitmap);             // blit the cached sprite (per pixel alpha)

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILESVGRENDERAGG::BuildCacheBitmap(GRPVECTORFILESVG* svg, GRP2DCANVAS* referencecanvas, double width, double height)
* @brief      Build cache bitmap : rasterize the SVG into a transparent offscreen canvas and capture it (with alpha)
* @note       INTERNAL
* @ingroup    GRAPHIC
* @param[in]  svg : loaded SVG file
* @param[in]  referencecanvas : destination canvas (its pixel format is reused for the offscreen)
* @param[in]  width : sprite width
* @param[in]  height : sprite height
* @return     bool : true if the cache bitmap was built.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILESVGRENDERAGG::BuildCacheBitmap(GRPVECTORFILESVG* svg, GRP2DCANVAS* referencecanvas, double width, double height)
{
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

  GRP2DCOLOR_RGBA8 transparent(0, 0, 0, 0);
  offscreen->Clear(&transparent);                                             // transparent background

  Render(svg, offscreen, 0.0, 0.0, width, height);                           // rasterize the SVG into the offscreen

  cachebitmap = offscreen->GetBitmap(0.0, 0.0, width, height);               // capture with its own alpha

  GEN_GRPFACTORY.DeleteCanvas(offscreen);

  return (cachebitmap != NULL);
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         void GRP2DVECTORFILESVGRENDERAGG::InvalidateCache()
* @brief      Invalidate cache : free the cached bitmap and force a new rasterization on the next RenderCached
* @ingroup    GRAPHIC
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DVECTORFILESVGRENDERAGG::InvalidateCache()
{
  if(cachebitmap)
    {
      GEN_GRPFACTORY.DeleteBitmap(cachebitmap);
      cachebitmap = NULL;
    }

  cachevalid = false;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILESVGRENDERAGG::CalculateViewBoxTransform(GRPVECTORFILESVG* svg, double targetx, double targety, double targetwidth, double targetheight, GRPVECTORFILESVGTRANSFORM& transform)
* @brief      Calculate view box transform : map the SVG viewBox into the target rectangle (uniform scale, centered)
* @note       INTERNAL
* @ingroup    GRAPHIC
* @param[in]  svg : loaded SVG file
* @param[in]  targetx : target rectangle x
* @param[in]  targety : target rectangle y
* @param[in]  targetwidth : target rectangle width
* @param[in]  targetheight : target rectangle height
* @param[out] transform : resulting base transform
* @return     bool : true if is succesful.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILESVGRENDERAGG::CalculateViewBoxTransform(GRPVECTORFILESVG* svg, double targetx, double targety, double targetwidth, double targetheight, GRPVECTORFILESVGTRANSFORM& transform)
{
  transform.SetIdentity();

  GRPVECTORFILESVGCONFIG* config = (GRPVECTORFILESVGCONFIG*)svg->GetConfig();
  if(!config) return false;

  double vbminx  = 0.0;
  double vbminy  = 0.0;
  double vbwidth = config->GetWidth();
  double vbheight= config->GetHeight();

  if(config->HasViewBox())
    {
      vbminx   = config->GetViewBoxMinX();
      vbminy   = config->GetViewBoxMinY();
      vbwidth  = config->GetViewBoxWidth();
      vbheight = config->GetViewBoxHeight();
    }

  if((vbwidth <= 0.0) || (vbheight <= 0.0)) return false;

  double scalex = targetwidth  / vbwidth;
  double scaley = targetheight / vbheight;
  double scale  = (scalex < scaley) ? scalex : scaley;                         // uniform : preserve aspect ratio

  double offsetx = targetx + ((targetwidth  - (vbwidth  * scale)) / 2.0) - (vbminx * scale);
  double offsety = targety + ((targetheight - (vbheight * scale)) / 2.0) - (vbminy * scale);

  transform.Set(scale, 0.0, 0.0, scale, offsetx, offsety);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILESVGRENDERAGG::RenderNode(GRPVECTORFILESVGOBJ* obj, GRPVECTORFILESVGTRANSFORM& parenttransform, GRPVECTORFILESVGSTYLE& parentstyle, GRP2DCANVAS* canvas)
* @brief      Render node : accumulate transform / style and render this node (recursing into containers)
* @note       INTERNAL
* @ingroup    GRAPHIC
* @param[in]  obj : object node
* @param[in]  parenttransform : accumulated parent transform
* @param[in]  parentstyle : effective parent style
* @param[in]  canvas : target canvas
* @return     bool : true if is succesful.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILESVGRENDERAGG::RenderNode(GRPVECTORFILESVGOBJ* obj, GRPVECTORFILESVGTRANSFORM& parenttransform, GRPVECTORFILESVGSTYLE& parentstyle, GRP2DCANVAS* canvas)
{
  if(!obj) return false;

  //  <defs> is not painted directly.
  if(obj->GetObjType() == GRPVECTORFILESVGOBJTYPE_DEFS) return true;

  //  Accumulated transform : node = parent * local
  GRPVECTORFILESVGTRANSFORM nodetransform;
  nodetransform.CopyFrom(parenttransform);
  nodetransform.Multiply(*obj->GetTransform());

  //  Effective style : node specified values over the inherited parent ones
  GRPVECTORFILESVGSTYLE nodestyle = (*obj->GetStyle());
  nodestyle.InheritFrom(parentstyle);

  if(obj->GetObjType() == GRPVECTORFILESVGOBJTYPE_USE)
    {
      GRPVECTORFILESVGOBJUSE* useobj = (GRPVECTORFILESVGOBJUSE*)obj;

      if(contextsvg && (usedepth < 16))
        {
          GRPVECTORFILESVGOBJ* reference = contextsvg->FindObjByID(useobj->GetHRef()->Get());
          if(reference && (reference != obj))
            {
              //  The <use> establishes the referenced content at its (x,y) offset.
              GRPVECTORFILESVGTRANSFORM usetransform;
              usetransform.CopyFrom(nodetransform);

              GRPVECTORFILESVGTRANSFORM offset;
              offset.Set(1.0, 0.0, 0.0, 1.0, useobj->GetX(), useobj->GetY());
              usetransform.Multiply(offset);

              usedepth++;
              RenderNode(reference, usetransform, nodestyle, canvas);
              usedepth--;
            }
        }

      return true;
    }

  if(obj->GetObjType() == GRPVECTORFILESVGOBJTYPE_TEXT)
    {
      RenderText(obj, nodetransform, nodestyle, canvas);
      return true;
    }

  if(obj->IsContainer())
    {
      for(XDWORD c=0; c<obj->GetNChilds(); c++)
        {
          RenderNode(obj->GetChild(c), nodetransform, nodestyle, canvas);
        }
    }
   else
    {
      RenderShape(obj, nodetransform, nodestyle, canvas);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILESVGRENDERAGG::RenderShape(GRPVECTORFILESVGOBJ* obj, GRPVECTORFILESVGTRANSFORM& transform, GRPVECTORFILESVGSTYLE& style, GRP2DCANVAS* canvas)
* @brief      Render shape : build the geometry, transform it and paint fill and/or stroke
* @note       INTERNAL
* @ingroup    GRAPHIC
* @param[in]  obj : shape node
* @param[in]  transform : accumulated transform for this node
* @param[in]  style : effective style for this node
* @param[in]  canvas : target canvas
* @return     bool : true if is succesful.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILESVGRENDERAGG::RenderShape(GRPVECTORFILESVGOBJ* obj, GRPVECTORFILESVGTRANSFORM& transform, GRPVECTORFILESVGSTYLE& style, GRP2DCANVAS* canvas)
{
  GRP2DPATH path;

  if(!obj->BuildPath(path)) return false;                                      // no geometry (containers, text, image, use ...)

  path.SetFillRule(style.GetFillRule());

  double bbx = 0.0;  double bby = 0.0;  double bbw = 0.0;  double bbh = 0.0;
  ComputePathBBox(path, bbx, bby, bbw, bbh);                                   // user space bbox (for objectBoundingBox gradients)

  TransformPath(path, transform);

  double opacity = style.GetOpacity();

  bool evenodd = (style.GetFillRule() == GRP2DPATHFILLRULE_EVENODD);

  if(style.IsFillPaintServer())
    {
      RenderGradient(path, style.GetFillPaintID(), opacity * style.GetFillOpacity(), false, 0.0, evenodd, transform, bbx, bby, bbw, bbh, canvas);
    }
   else if(style.HasFill())
    {
      GRP2DCOLOR_RGBA8 fillcolor = style.GetFillColor();
      fillcolor.a = (XBYTE)(255.0 * opacity * style.GetFillOpacity());

      canvas->SetFillColor(&fillcolor);
      canvas->Path(path, true);
    }

  if(style.IsStrokePaintServer())
    {
      RenderGradient(path, style.GetStrokePaintID(), opacity * style.GetStrokeOpacity(), true, style.GetStrokeWidth() * GetScaleFactor(transform), evenodd, transform, bbx, bby, bbw, bbh, canvas);
    }
   else if(style.HasStroke())
    {
      GRP2DCOLOR_RGBA8 strokecolor = style.GetStrokeColor();
      strokecolor.a = (XBYTE)(255.0 * opacity * style.GetStrokeOpacity());

      canvas->SetLineColor(&strokecolor);
      canvas->SetLineWidth(style.GetStrokeWidth() * GetScaleFactor(transform));
      canvas->Path(path, false);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILESVGRENDERAGG::RenderText(GRPVECTORFILESVGOBJ* obj, GRPVECTORFILESVGTRANSFORM& transform, GRPVECTORFILESVGSTYLE& style, GRP2DCANVAS* canvas)
* @brief      Render text : draw the text using the canvas vector font (a vector font must be loaded on the canvas)
* @note       INTERNAL. The anchor (x,y) is transformed to device space and the font size is scaled by the transform.
*             Text rotation/skew from the transform is not applied (the glyphs stay upright).
* @ingroup    GRAPHIC
* @param[in]  obj : text node
* @param[in]  transform : accumulated transform (user -> device)
* @param[in]  style : effective style
* @param[in]  canvas : target canvas
* @return     bool : true if drawn.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILESVGRENDERAGG::RenderText(GRPVECTORFILESVGOBJ* obj, GRPVECTORFILESVGTRANSFORM& transform, GRPVECTORFILESVGSTYLE& style, GRP2DCANVAS* canvas)
{
  GRPVECTORFILESVGOBJTEXT* textobj = (GRPVECTORFILESVGOBJTEXT*)obj;

  if(!canvas->Vectorfont_GetConfig()) return false;

  double scale      = GetScaleFactor(transform);
  double parentsize = textobj->GetFontSize();

  double penx = textobj->GetX();
  double peny = textobj->GetY();

  bool haschildren = (textobj->GetNChilds() > 0);

  //  The <text> own text (the chunk the parser kept). With <tspan> runs the anchor is start (left to right).
  if(!textobj->GetText()->IsEmpty())
    {
      GRPVECTORFILESVGTEXTANCHOR anchor = haschildren ? GRPVECTORFILESVGTEXTANCHOR_START : textobj->GetTextAnchor();
      penx = DrawTextRun(textobj->GetText(), style, parentsize, penx, peny, anchor, transform, scale, canvas);
    }

  //  <tspan> runs.
  for(XDWORD c=0; c<textobj->GetNChilds(); c++)
    {
      GRPVECTORFILESVGOBJ* child = textobj->GetChild(c);
      if(!child) continue;
      if(child->GetObjType() != GRPVECTORFILESVGOBJTYPE_TEXT) continue;

      GRPVECTORFILESVGOBJTEXT* tspan = (GRPVECTORFILESVGOBJTEXT*)child;
      if(tspan->GetText()->IsEmpty()) continue;

      GRPVECTORFILESVGSTYLE tspanstyle = (*tspan->GetStyle());
      tspanstyle.InheritFrom(style);

      double tspansize = tspan->HasFontSize() ? tspan->GetFontSize() : parentsize;

      if(tspan->HasX())  penx = tspan->GetX();
      if(tspan->HasY())  peny = tspan->GetY();

      penx += tspan->GetDX();
      peny += tspan->GetDY();

      penx = DrawTextRun(tspan->GetText(), tspanstyle, tspansize, penx, peny, GRPVECTORFILESVGTEXTANCHOR_START, transform, scale, canvas);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         double GRP2DVECTORFILESVGRENDERAGG::DrawTextRun(XSTRING* text, GRPVECTORFILESVGSTYLE& style, double sizeuser, double penx, double peny, GRPVECTORFILESVGTEXTANCHOR anchor, GRPVECTORFILESVGTRANSFORM& transform, double scale, GRP2DCANVAS* canvas)
* @brief      Draw text run : draw one text run at the (user space) pen and return the advanced pen x (user space)
* @note       INTERNAL
* @ingroup    GRAPHIC
* @param[in]  text : run text
* @param[in]  style : effective style for the run
* @param[in]  sizeuser : font size in user units
* @param[in]  penx : pen x (user space)
* @param[in]  peny : pen y (user space, baseline)
* @param[in]  anchor : text anchor
* @param[in]  transform : accumulated transform (user -> device)
* @param[in]  scale : transform scale factor
* @param[in]  canvas : target canvas
* @return     double : advanced pen x (user space)
* --------------------------------------------------------------------------------------------------------------------*/
double GRP2DVECTORFILESVGRENDERAGG::DrawTextRun(XSTRING* text, GRPVECTORFILESVGSTYLE& style, double sizeuser, double penx, double peny, GRPVECTORFILESVGTEXTANCHOR anchor, GRPVECTORFILESVGTRANSFORM& transform, double scale, GRP2DCANVAS* canvas)
{
  GRP2DCANVAS_VECTORFONT_CONFIG* config = canvas->Vectorfont_GetConfig();

  double devicesize = sizeuser * scale;
  if(devicesize < 1.0)  devicesize = 1.0;

  config->SetSize(devicesize);

  double width = canvas->VectorFont_GetWidth(text->Get());

  if(style.HasFill())
    {
      GRP2DCOLOR_RGBA8 color = style.GetFillColor();
      color.a = (XBYTE)(255.0 * style.GetOpacity() * style.GetFillOpacity());
      config->SetColor(&color);

      double dx = penx;
      double dy = peny;
      transform.ApplyToPoint(dx, dy);

      if(anchor == GRPVECTORFILESVGTEXTANCHOR_MIDDLE)   dx -= (width / 2.0);
       else if(anchor == GRPVECTORFILESVGTEXTANCHOR_END)  dx -= width;

      canvas->VectorFont_Print(dx, dy, text->Get());
    }

  if(scale > 0.0)  penx += (width / scale);                                    // advance pen in user units

  return penx;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         void GRP2DVECTORFILESVGRENDERAGG::ComputePathBBox(GRP2DPATH& path, double& minx, double& miny, double& width, double& height)
* @brief      Compute path bbox : axis aligned bounding box of the path (approximated using endpoints + control points)
* @note       INTERNAL
* @ingroup    GRAPHIC
* @param[in]  path : path (user space)
* @param[out] minx : bounding box min x
* @param[out] miny : bounding box min y
* @param[out] width : bounding box width
* @param[out] height : bounding box height
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DVECTORFILESVGRENDERAGG::ComputePathBBox(GRP2DPATH& path, double& minx, double& miny, double& width, double& height)
{
  bool   first = true;
  double x0 = 0.0;  double y0 = 0.0;  double x1 = 0.0;  double y1 = 0.0;

  for(XDWORD c=0; c<path.GetSize(); c++)
    {
      GRP2DPATHSEGMENT* segment = path.Get((int)c);
      if(!segment) continue;
      if(segment->type == GRP2DPATHSEGMENTTYPE_CLOSE) continue;

      double px[3];   double py[3];   int n = 1;
      px[0] = segment->x;    py[0] = segment->y;

      if((segment->type == GRP2DPATHSEGMENTTYPE_CURVETO) || (segment->type == GRP2DPATHSEGMENTTYPE_QUADTO))
        {
          px[n] = segment->c1x;  py[n] = segment->c1y;  n++;
        }

      if(segment->type == GRP2DPATHSEGMENTTYPE_CURVETO)
        {
          px[n] = segment->c2x;  py[n] = segment->c2y;  n++;
        }

      for(int i=0; i<n; i++)
        {
          if(first)  { x0 = x1 = px[i];  y0 = y1 = py[i];  first = false; }
           else
            {
              if(px[i] < x0) x0 = px[i];
              if(px[i] > x1) x1 = px[i];
              if(py[i] < y0) y0 = py[i];
              if(py[i] > y1) y1 = py[i];
            }
        }
    }

  minx   = x0;
  miny   = y0;
  width  = (x1 - x0);
  height = (y1 - y0);
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRP2DVECTORFILESVGRENDERAGG::RenderGradient(GRP2DPATH& devicepath, XSTRING* paintid, double globalalpha, bool isstroke, double linewidth, bool evenodd, GRPVECTORFILESVGTRANSFORM& transform, double bbx, double bby, double bbw, double bbh, GRP2DCANVAS* canvas)
* @brief      Render gradient : resolve the paint server gradient and fill (or stroke) the path with it
* @note       INTERNAL
* @ingroup    GRAPHIC
* @param[in]  devicepath : path already transformed to device space
* @param[in]  paintid : referenced gradient id (fill or stroke paint server)
* @param[in]  globalalpha : opacity * (fill|stroke)-opacity to bake into the stops
* @param[in]  isstroke : true to paint the stroke outline, false to fill
* @param[in]  linewidth : device stroke width (only when isstroke)
* @param[in]  evenodd : even-odd fill rule (fill only)
* @param[in]  transform : accumulated node transform (user -> device)
* @param[in]  bbx : path user space bounding box x
* @param[in]  bby : path user space bounding box y
* @param[in]  bbw : path user space bounding box width
* @param[in]  bbh : path user space bounding box height
* @param[in]  canvas : target canvas
* @return     bool : true if painted with a gradient.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DVECTORFILESVGRENDERAGG::RenderGradient(GRP2DPATH& devicepath, XSTRING* paintid, double globalalpha, bool isstroke, double linewidth, bool evenodd, GRPVECTORFILESVGTRANSFORM& transform, double bbx, double bby, double bbw, double bbh, GRP2DCANVAS* canvas)
{
  if(!contextsvg) return false;

  GRPVECTORFILESVGOBJ* paintobj = contextsvg->FindObjByID(paintid->Get());
  if(!paintobj) return false;

  if((paintobj->GetObjType() != GRPVECTORFILESVGOBJTYPE_LINEARGRADIENT) &&
     (paintobj->GetObjType() != GRPVECTORFILESVGOBJTYPE_RADIALGRADIENT)) return false;

  GRPVECTORFILESVGOBJGRADIENT* gradient = (GRPVECTORFILESVGOBJGRADIENT*)paintobj;

  //  Stops may be inherited from another gradient via xlink:href.
  GRPVECTORFILESVGOBJGRADIENT* stopsource = gradient;
  int                         guard      = 0;

  while(stopsource && (stopsource->GetNStops() == 0) && (!stopsource->GetHRef()->IsEmpty()) && (guard < 8))
    {
      GRPVECTORFILESVGOBJ* ref = contextsvg->FindObjByID(stopsource->GetHRef()->Get());

      if(ref && ((ref->GetObjType() == GRPVECTORFILESVGOBJTYPE_LINEARGRADIENT) ||
                 (ref->GetObjType() == GRPVECTORFILESVGOBJTYPE_RADIALGRADIENT)))   stopsource = (GRPVECTORFILESVGOBJGRADIENT*)ref;
       else                                                                       stopsource = NULL;

      guard++;
    }

  int                nstops = stopsource ? stopsource->GetNStops() : 0;
  if(nstops < 1) return false;

  //  Bake the global / fill opacity into each stop alpha.
  GRP2DGRADIENTSTOP* src = stopsource->GetStops();
  GRP2DGRADIENTSTOP  stops[GRP2DGRADIENT_MAXSTOPS];

  for(int i=0; i<nstops; i++)
    {
      stops[i].offset = src[i].offset;
      stops[i].color  = src[i].color;
      stops[i].color.a = (XBYTE)((double)src[i].color.a * globalalpha);
    }

  bool                       objectbbox = (gradient->GetUnits() == GRPVECTORFILESVGGRADIENTUNITS_OBJECTBOUNDINGBOX);
  GRPVECTORFILESVGTRANSFORM* gtransform = gradient->GetGradientTransform();

  if(gradient->IsRadial())
    {
      double ucx = gradient->GetCX();
      double ucy = gradient->GetCY();
      double ur  = gradient->GetR();

      double centerx;   double centery;   double radius;

      if(objectbbox)
        {
          centerx = bbx + (ucx * bbw);
          centery = bby + (ucy * bbh);
          radius  = ur * sqrt(((bbw * bbw) + (bbh * bbh)) / 2.0);
        }
       else
        {
          centerx = ucx;
          centery = ucy;
          radius  = ur;
        }

      gtransform->ApplyToPoint(centerx, centery);                              // gradientTransform (user space)
      double gradientscale = GetScaleFactor(*gtransform);

      transform.ApplyToPoint(centerx, centery);                               // user -> device
      double devicescale = GetScaleFactor(transform);

      double deviceradius = radius * gradientscale * devicescale;

      if(isstroke)  canvas->PathGradientRadialStroke(devicepath, linewidth, centerx, centery, deviceradius, stops, nstops);
       else         canvas->PathGradientRadial(devicepath, evenodd, centerx, centery, deviceradius, stops, nstops);
    }
   else
    {
      double ux1 = gradient->GetX1();   double uy1 = gradient->GetY1();
      double ux2 = gradient->GetX2();   double uy2 = gradient->GetY2();

      double ax;   double ay;   double bx;   double by;

      if(objectbbox)
        {
          ax = bbx + (ux1 * bbw);   ay = bby + (uy1 * bbh);
          bx = bbx + (ux2 * bbw);   by = bby + (uy2 * bbh);
        }
       else
        {
          ax = ux1;   ay = uy1;
          bx = ux2;   by = uy2;
        }

      gtransform->ApplyToPoint(ax, ay);                                        // gradientTransform (user space)
      gtransform->ApplyToPoint(bx, by);

      transform.ApplyToPoint(ax, ay);                                          // user -> device
      transform.ApplyToPoint(bx, by);

      if(isstroke)  canvas->PathGradientLinearStroke(devicepath, linewidth, ax, ay, bx, by, stops, nstops);
       else         canvas->PathGradientLinear(devicepath, evenodd, ax, ay, bx, by, stops, nstops);
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
* @brief      Transform path : apply the transform to every point of the path
* @note       INTERNAL
* @ingroup    GRAPHIC
* @param[in]  path : path to transform (in place)
* @param[in]  transform : transform to apply
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DVECTORFILESVGRENDERAGG::TransformPath(GRP2DPATH& path, GRPVECTORFILESVGTRANSFORM& transform)
{
  for(XDWORD c=0; c<path.GetSize(); c++)
    {
      GRP2DPATHSEGMENT* segment = path.Get((int)c);
      if(!segment) continue;

      transform.ApplyToPoint(segment->x  , segment->y  );

      if((segment->type == GRP2DPATHSEGMENTTYPE_CURVETO) || (segment->type == GRP2DPATHSEGMENTTYPE_QUADTO))
        {
          transform.ApplyToPoint(segment->c1x, segment->c1y);
        }

      if(segment->type == GRP2DPATHSEGMENTTYPE_CURVETO)
        {
          transform.ApplyToPoint(segment->c2x, segment->c2y);
        }
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         double GRP2DVECTORFILESVGRENDERAGG::GetScaleFactor(GRPVECTORFILESVGTRANSFORM& transform)
* @brief      Get scale factor : uniform scale approximation from the transform (used to scale the stroke width)
* @note       INTERNAL
* @ingroup    GRAPHIC
* @param[in]  transform : transform
* @return     double : scale factor
* --------------------------------------------------------------------------------------------------------------------*/
double GRP2DVECTORFILESVGRENDERAGG::GetScaleFactor(GRPVECTORFILESVGTRANSFORM& transform)
{
  double determinant = (transform.GetA() * transform.GetD()) - (transform.GetB() * transform.GetC());

  double scale = sqrt((determinant < 0.0) ? -determinant : determinant);

  if(scale <= 0.0) scale = 1.0;

  return scale;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         void GRP2DVECTORFILESVGRENDERAGG::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    GRAPHIC
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DVECTORFILESVGRENDERAGG::Clean()
{
  cachebitmap = NULL;
  cachevalid  = false;
  cachex      = 0.0;
  cachey      = 0.0;
  cachewidth  = 0.0;
  cacheheight = 0.0;

  contextsvg  = NULL;
  usedepth    = 0;
}
