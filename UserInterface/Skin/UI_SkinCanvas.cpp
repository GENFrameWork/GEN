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
#include "XDiagLog.h"                        // TEMPORARY diagnostic-only, see XDiagLog.h -- remove with it

#include "GRP2DCanvas.h"
#include "GRP2DColor.h"
#include "GRPViewPort.h"
#include "GRPScreen.h"
#include "GRPBitmap.h"
#include "GRPBitmapFile.h"
#include "GRPFactory.h"

// Needed only by UI_SkinCanvas_DrawSoftShadow's agg::stack_blur_rgba32 call further down: this file otherwise
// stays behind the GRP2DCANVAS/GRPBITMAP abstraction and never touches AGG types directly. agg_rendering_buffer.h
// and agg_pixfmt_rgba.h are already pulled in transitively (GRPBitmap.h, GRP2DCanvasAGG.h) on every platform
// that ships the AGG canvas backend; agg_blur.h is the one header this file is the first to require, so if a
// given platform's vendored AGG drop is a trimmed subset without it, this is the file that will fail to build --
// add agg_blur.h to that platform's AGG sources (it is dependency-free beyond agg_array.h) to fix it.
#include "agg_rendering_buffer.h"
#include "agg_pixfmt_rgba.h"
#include "agg_blur.h"

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
* @brief      Append a circular arc to a path as a short-segment polyline (~2 deg per step). Uses only MoveTo/LineTo, which are fully exercised by the canvas stroke pipeline; this deliberately avoids GRP2DPATH::ArcTo (the SVG elliptical-arc command), whose AGG arc_to conversion is not used anywhere else and produces no geometry here. Angles are degrees; a positive span advances clockwise in this y-down canvas.
* @note       INTERNAL / FILE LOCAL
* @ingroup    USERINTERFACE
* 
* @param[in]  path : destination path.
* @param[in]  cx : Cx value.
* @param[in]  cy : Cy value.
* @param[in]  r : ring radius (to the stroke centerline).
* @param[in]  startdeg : start angle in degrees.
* @param[in]  spandeg : signed span in degrees.
* @param[in]  firstpoint : true on the first append (emits a MoveTo); set to false afterwards.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         static void DrawScrollPill(GRP2DCANVAS* canvas, double a, double b, double c, double d, double formradius)
* @brief      Draw Scroll Pill
* @ingroup    USERINTERFACE
* 
* @param[in]  canvas : Canvas object to use.
* @param[in]  a : A value.
* @param[in]  b : B value.
* @param[in]  c : C value.
* @param[in]  d : D value.
* @param[in]  formradius : Formradius value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @brief      Draw a filled rect, rounded when radius > 0. The radius is clamped to half the smaller side so a capsule (roundcap) stays valid even when the progress fill is narrower than the bar thickness (agg::rounded_rect does not self-normalize the radius).
* @note       INTERNAL / FILE LOCAL
* @ingroup    USERINTERFACE
* 
* @param[in]  canvas : Canvas object to use.
* @param[in]  x1 : X1 value.
* @param[in]  y1 : Y1 value.
* @param[in]  x2 : X2 value.
* @param[in]  y2 : Y2 value.
* @param[in]  radius : Radius value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* 
* @fn         static void UI_SkinCanvas_AppendRoundRectPath(GRP2DPATH& path, double minx, double miny, double maxx, double maxy, double r)
* @brief      Builds a (optionally rounded) rectangle outline into a path with MoveTo/LineTo so it can be gradient-filled.
* @note       INTERNAL / FILE LOCAL. Inputs are already normalized (minx<=maxx, miny<=maxy) and r clamped by the caller.
* @ingroup    USERINTERFACE
* 
* @param[in]  path : Path to use.
* @param[in]  minx : Minx value.
* @param[in]  miny : Miny value.
* @param[in]  maxx : Maxx value.
* @param[in]  maxy : Maxy value.
* @param[in]  r : R value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
*
* @fn         void UI_SKINCANVAS::AppendRoundRectPathPerCorner(GRP2DPATH& path, double minx, double miny, double maxx, double maxy, double rTL, double rTR, double rBR, double rBL)
* @brief      Build a rounded-rectangle outline with a possibly-different radius per corner. Corners with radius
*             <= 0 are drawn square. Each non-zero radius is clamped to half the shorter side of the rect so
*             two adjacent large radii never overlap into an invalid shape. Uses only MoveTo/LineTo, matching the
*             stroke-friendly polyline convention of UI_SkinCanvas_AppendRoundRectPath so both fill and stroke
*             paint through the same AGG code path.
* @note       Shared box-model helper (built against base UI_ELEMENT accessors only, no GRP2DPATH-specific state
*             on UI_SKINCANVAS itself): any subclass' Draw_X can call it. Inputs are already normalized
*             (minx <= maxx, miny <= maxy).
* @ingroup    USERINTERFACE
*
* @param[in]  path : Path to append into.
* @param[in]  minx : Left edge.
* @param[in]  miny : Top edge.
* @param[in]  maxx : Right edge.
* @param[in]  maxy : Bottom edge.
* @param[in]  rTL : Top-left corner radius.
* @param[in]  rTR : Top-right corner radius.
* @param[in]  rBR : Bottom-right corner radius.
* @param[in]  rBL : Bottom-left corner radius.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_SKINCANVAS::AppendRoundRectPathPerCorner(GRP2DPATH& path, double minx, double miny, double maxx, double maxy,
                                                 double rTL, double rTR, double rBR, double rBL)
{
  double w    = maxx - minx;
  double h    = maxy - miny;
  double half = ((w < h) ? w : h) / 2.0;

  // Clamp each corner independently to keep the outline valid on narrow rectangles.
  if(rTL < 0.0) rTL = 0.0;   if(rTL > half) rTL = half;
  if(rTR < 0.0) rTR = 0.0;   if(rTR > half) rTR = half;
  if(rBR < 0.0) rBR = 0.0;   if(rBR > half) rBR = half;
  if(rBL < 0.0) rBL = 0.0;   if(rBL > half) rBL = half;

  // If nothing is rounded at all, emit the plain rectangle (matches the shortcut in AppendRoundRectPath).
  if(rTL <= 0.0 && rTR <= 0.0 && rBR <= 0.0 && rBL <= 0.0)
    {
      path.MoveTo(minx, miny);  path.LineTo(maxx, miny);  path.LineTo(maxx, maxy);  path.LineTo(minx, maxy);  path.Close();
      return;
    }

  const int STEPS = 6;
  bool      first = true;

  // Traversal order (clockwise, starting at the top edge between the two top corners):
  //   TL corner arc -> top edge -> TR arc -> right edge -> BR arc -> bottom edge -> BL arc -> left edge -> Close.
  double cx[4] = { minx + rTL, maxx - rTR, maxx - rBR, minx + rBL };                  // corner centers: TL, TR, BR, BL
  double cy[4] = { miny + rTL, miny + rTR, maxy - rBR, maxy - rBL };
  double r [4] = { rTL,        rTR,        rBR,        rBL        };
  double a0[4] = { 180.0,      270.0,        0.0,       90.0      };                  // start angle of each quarter arc

  for(int c = 0; c < 4; c++)
    {
      if(r[c] <= 0.0)
        {
          // Square corner: emit the sharp corner vertex itself.
          double vx = (c == 0) ? minx : (c == 1) ? maxx : (c == 2) ? maxx : minx;
          double vy = (c == 0) ? miny : (c == 1) ? miny : (c == 2) ? maxy : maxy;
          if(first) { path.MoveTo(vx, vy); first = false; }
          else        path.LineTo(vx, vy);
          continue;
        }

      for(int s = 0; s <= STEPS; s++)
        {
          double ang = (a0[c] + (90.0 * (double)s / (double)STEPS)) * (PI / 180.0);
          double px  = cx[c] + (r[c] * cos(ang));
          double py  = cy[c] + (r[c] * sin(ang));

          if(first) { path.MoveTo(px, py); first = false; }
          else        path.LineTo(px, py);
        }
    }

  path.Close();
}


// Padding, in pixels, added around a shadow silhouette so the blur fade has room to reach zero before the
// off-screen bitmap ends. SINGLE SOURCE OF TRUTH: both the renderer (UI_SkinCanvas_DrawSoftShadow, which sizes
// and positions the bitmap) and the invalidation logic (UI_SKINCANVAS::PreDrawFunction, which expands the
// element's rebuild-area) must agree on this number. If they drift apart, the outer ring of the blurred shadow
// falls outside the saved/restored background rectangle and leaves ghost pixels that accumulate -- getting
// progressively darker -- on every repaint.
// Still valid with agg::stack_blur_rgba32 (see UI_SkinCanvas_DrawSoftShadow): its single-pass triangular kernel
// has non-zero support out to exactly the blur radius on each side, i.e. NARROWER than the old hand-rolled
// 3-pass box blur this padding was originally sized for (whose support reached roughly 3x the radius). 2x
// stays comfortably generous for the new algorithm -- no need to shrink it, and shrinking would risk exactly
// the ghost-pixel bug described above for no benefit.
#define UI_SKINCANVAS_SHADOW_BLURPADDING(blur)      ((blur) * 2)


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         static bool UI_SkinCanvas_RoundedRectInside(int px, int py, int w, int h, double rTL, double rTR, double rBR, double rBL)
* @brief      Point-in-rounded-rect test for the four per-corner radius variant.
* @note       INTERNAL / FILE LOCAL. Rectangle spans [0, w) x [0, h). Corners with radius 0 draw square.
* @ingroup    USERINTERFACE
*
* @param[in]  px : Pixel X (relative to rectangle top-left).
* @param[in]  py : Pixel Y (relative to rectangle top-left).
* @param[in]  w : Rectangle width.
* @param[in]  h : Rectangle height.
* @param[in]  rTL : Top-left radius.
* @param[in]  rTR : Top-right radius.
* @param[in]  rBR : Bottom-right radius.
* @param[in]  rBL : Bottom-left radius.
*
* @return     bool : true iff the pixel is inside the shape.
*
* --------------------------------------------------------------------------------------------------------------------*/
static bool UI_SkinCanvas_RoundedRectInside(int px, int py, int w, int h, double rTL, double rTR, double rBR, double rBL)
{
  if(px < 0 || px >= w || py < 0 || py >= h) return false;

  // Which corner region does (px, py) fall into? A pixel not inside any corner square is always inside the shape.
  bool in_TL = (px <  (int)rTL) && (py <  (int)rTL);
  bool in_TR = (px >= (w - (int)rTR)) && (py <  (int)rTR);
  bool in_BR = (px >= (w - (int)rBR)) && (py >= (h - (int)rBR));
  bool in_BL = (px <  (int)rBL) && (py >= (h - (int)rBL));

  if(!in_TL && !in_TR && !in_BR && !in_BL) return true;

  double cx = 0.0, cy = 0.0, r = 0.0;

  if     (in_TL) { cx = rTL;      cy = rTL;      r = rTL; }
  else if(in_TR) { cx = w - rTR;  cy = rTR;      r = rTR; }
  else if(in_BR) { cx = w - rBR;  cy = h - rBR;  r = rBR; }
  else           { cx = rBL;      cy = h - rBL;  r = rBL; }

  if(r <= 0.0) return true;    // corner declared square: inside

  double dx = ((double)px + 0.5) - cx;
  double dy = ((double)py + 0.5) - cy;

  return (dx * dx + dy * dy) <= (r * r);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         static void UI_SkinCanvas_DrawSoftShadow(GRP2DCANVAS* canvas, double minx, double miny, double maxx, double maxy, double rTL, double rTR, double rBR, double rBL, UI_COLOR* shadow_color, int blur_radius)
* @brief      Draw a soft-edged rounded-rect drop shadow. Rasterises the silhouette into an off-screen RGBA
*             bitmap padded to hold the blur fade, runs agg::stack_blur_rgba32 on it, then composites via
*             the canvas's PutBitmapAlpha. Falls back cleanly on unsupported canvas modes (returns false so
*             the caller can draw a hard shadow instead).
* @note       INTERNAL / FILE LOCAL.
* @ingroup    USERINTERFACE
*
* @param[in]  canvas : Target canvas.
* @param[in]  minx : Shadow rect left edge (screen coords).
* @param[in]  miny : Shadow rect top edge.
* @param[in]  maxx : Shadow rect right edge.
* @param[in]  maxy : Shadow rect bottom edge.
* @param[in]  rTL : Top-left corner radius.
* @param[in]  rTR : Top-right corner radius.
* @param[in]  rBR : Bottom-right corner radius.
* @param[in]  rBL : Bottom-left corner radius.
* @param[in]  shadow_color : Shadow tint (RGB used everywhere; A used inside the silhouette).
* @param[in]  blur_radius : Blur radius in pixels; must be > 0.
*
* @return     bool : true on success; false when the canvas mode is not 32-bit RGBA/BGRA (caller draws hard).
*
* --------------------------------------------------------------------------------------------------------------------*/
// TEMPORARY diagnostics -- see XDiagLog.h. Accumulated microseconds spent building/compositing soft shadows and
// how many calls happened THIS FRAME, split into cache hits/misses (see UI_SkinCanvas_DrawSoftShadow_FormCached
// below and UI_ELEMENT_FORM::ShadowCache_* in UI_Element_Form.cpp/.h) so the effect of the bitmap cache shows up
// directly in the next uidiag.log. Read and reset once per frame from UI_SYSTEM::DrawFrame() (extern-declared
// there).
XQWORD diagskin_shadowus         = 0;
XDWORD diagskin_shadowcalls      = 0;
XDWORD diagskin_shadowcachehits  = 0;
XDWORD diagskin_shadowcachemiss  = 0;


// Rasterises the shadow silhouette into a NEW off-screen RGBA bitmap, padded to hold the blur fade, and runs
// agg::stack_blur_rgba32 on it. Does NOT composite it onto the canvas and does NOT delete it -- the caller owns
// the returned bitmap (either compositing-then-deleting it immediately, as UI_SkinCanvas_DrawSoftShadow_Impl
// does for the two non-Form call sites, or compositing-then-CACHING it on the owning UI_ELEMENT_FORM, as
// UI_SkinCanvas_DrawSoftShadow_FormCached does, so the SAME bitmap can be reused on a later frame instead of
// rebuilt from scratch). Returns NULL on failure (caller falls back to a hard shadow).
static GRPBITMAP* UI_SkinCanvas_BuildSoftShadowBitmap(int shape_w, int shape_h, double rTL, double rTR, double rBR, double rBL,
                                                        UI_COLOR* shadow_color, int blur_radius)
{
  if(!shadow_color || blur_radius <= 0)   return NULL;
  if(shape_w <= 0 || shape_h <= 0)        return NULL;

  // Pad the bitmap on each side so the fade at the shape's border has room to fully dissipate before the
  // buffer ends. Padding tighter than the blur radius would leave a visible hard cut. The very same formula
  // drives the rebuild-area expansion in PreDrawFunction -- keep them in sync via the shared macro.
  int pad  = UI_SKINCANVAS_SHADOW_BLURPADDING(blur_radius);
  int bw   = shape_w + (2 * pad);
  int bh   = shape_h + (2 * pad);

  // Always create the off-screen bitmap in RGBA_8888. This decouples the shadow pipeline from the canvas
  // native pixel format: our per-pixel writes below use fixed R=0/G=1/B=2/A=3 offsets, and the composite
  // step (UI_SkinCanvas_CompositeSoftShadowBitmap) goes through canvas->PutBlendPixel with a GRP2DCOLOR_RGBA8
  // -- the canvas layer handles whatever mode conversion is needed to write into 24-bit RGB, 16-bit RGB565 or
  // another 32-bit backend.
  GRPBITMAP* bitmap = GRPFACTORY::GetInstance().CreateBitmap(bw, bh, GRPPROPERTYMODE_32_RGBA_8888);
  if(!bitmap)         return NULL;
  if(!bitmap->IsValid())
    {
      GRPFACTORY::GetInstance().DeleteBitmap(bitmap);
      return NULL;
    }

  XBYTE* buf = bitmap->GetBuffer();
  if(!buf)
    {
      GRPFACTORY::GetInstance().DeleteBitmap(bitmap);
      return NULL;
    }

  // Fixed RGBA_8888 layout: R, G, B, A per pixel.
  const int r_off = 0;
  const int g_off = 1;
  const int b_off = 2;
  const int a_off = 3;

  XBYTE sr = (XBYTE)shadow_color->GetRed();
  XBYTE sg = (XBYTE)shadow_color->GetGreen();
  XBYTE sb = (XBYTE)shadow_color->GetBlue();
  XBYTE sa = (XBYTE)shadow_color->GetAlpha();

  // First pass: fill every pixel with the shadow RGB but alpha 0 (transparent shadow colour). This way the
  // subsequent blur averages RGB against itself (no drift) and only alpha fades from full-shadow to 0 at the
  // silhouette boundary -- the standard trick for correct soft-shadow colour at the edges.
  for(int y = 0; y < bh; y++)
    {
      XBYTE* row = buf + (y * bw * 4);
      for(int x = 0; x < bw; x++)
        {
          int off = x * 4;
          row[off + r_off] = sr;
          row[off + g_off] = sg;
          row[off + b_off] = sb;
          row[off + a_off] = 0;
        }
    }

  // Second pass: set alpha to sa inside the rounded rectangle silhouette (positioned at (pad, pad) relative
  // to the bitmap origin). Everything outside stays transparent.
  for(int y = 0; y < shape_h; y++)
    {
      XBYTE* row = buf + ((y + pad) * bw * 4);
      for(int x = 0; x < shape_w; x++)
        {
          if(UI_SkinCanvas_RoundedRectInside(x, y, shape_w, shape_h, rTL, rTR, rBR, rBL))
            {
              row[((x + pad) * 4) + a_off] = sa;
            }
        }
    }

  // Literal AGG stack blur (agg_blur.h), applied in place on the same off-screen RGBA_8888 buffer built above.
  // agg::stack_blur_rgba32 is templated on an "Img" concept (width()/height()/stride()/pix_ptr()/order_type),
  // not on a raw agg::rendering_buffer -- agg::pixfmt_rgba32 (order_rgba: R=0,G=1,B=2,A=3, agg_pixfmt_rgba.h)
  // is exactly that wrapper, and its byte order matches r_off/g_off/b_off/a_off above pixel for pixel, so no
  // channel shuffling is needed. rx/ry both use blur_radius: a single isotropic blur, same as the box-blur
  // version this replaces. See the NOTE on UI_SKINCANVAS_SHADOW_BLURPADDING for why the existing 2x padding
  // is still (more than) enough for this algorithm's narrower kernel support.
  {
    agg::rendering_buffer rbuf;
    rbuf.attach(buf, (unsigned)bw, (unsigned)bh, bw * 4);

    agg::pixfmt_rgba32 pixf(rbuf);

    agg::stack_blur_rgba32(pixf, (unsigned)blur_radius, (unsigned)blur_radius);
  }

  // One-shot self-check: the first time a soft shadow renders anywhere in the process, log the parameters
  // so the console confirms the soft path was reached and shows the effective bitmap / blur / colour. Any
  // subsequent invocation is silent so the log stays clean.
  static bool once = true;
  if(once)
    {
      XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[UI Draw] soft shadow: bitmap=%dx%d blur=%d shadow_alpha=%d"),
                        bw, bh, blur_radius, (int)shadow_color->GetAlpha());
      once = false;
    }

  return bitmap;
}


// Composites an already-built (fresh or cached) blurred shadow bitmap onto the canvas, anchored so its
// silhouette lands back at (minx, miny) exactly as when it was rasterised (the padding added by
// UI_SkinCanvas_BuildSoftShadowBitmap is recovered here from blur_radius via the same shared macro, so the
// caller does not need to remember it). Safe to call every frame even when reusing a cached bitmap: this is
// the part of the work that genuinely must happen on every redraw (the canvas pixels underneath were erased).
static void UI_SkinCanvas_CompositeSoftShadowBitmap(GRP2DCANVAS* canvas, GRPBITMAP* bitmap, double minx, double miny, int blur_radius)
{
  if(!canvas || !bitmap) return;

  int pad = UI_SKINCANVAS_SHADOW_BLURPADDING(blur_radius);
  int bw  = (int)bitmap->GetWidth();
  int bh  = (int)bitmap->GetHeight();

  XBYTE* buf = bitmap->GetBuffer();
  if(!buf) return;

  const int r_off = 0;
  const int g_off = 1;
  const int b_off = 2;
  const int a_off = 3;

  // Composite the blurred bitmap onto the canvas. We deliberately do NOT use canvas->PutBitmapAlpha because
  // that helper passes the source pixel's alpha as agg's cover_type, which then multiplies the source alpha
  // AGAIN internally -- effectively squaring the alpha and crushing every semi-transparent edge to zero.
  // The blurred soft edges of a shadow are almost entirely in that low-alpha range, so PutBitmapAlpha would
  // reduce the shadow to only its most opaque central band, producing the same hard-edged silhouette we are
  // trying to escape. Instead we iterate and call PutBlendPixel with cover=255 (full coverage), so the
  // compositing weight is exactly the pixel's own alpha and the soft gradient survives intact.
  double dest_x = minx - (double)pad;
  double dest_y = miny - (double)pad;

  for(int y = 0; y < bh; y++)
    {
      XBYTE* row = buf + (y * bw * 4);
      for(int x = 0; x < bw; x++)
        {
          int off = x * 4;
          XBYTE a = row[off + a_off];
          if(a == 0) continue;

          GRP2DCOLOR_RGBA8 c(row[off + r_off], row[off + g_off], row[off + b_off], a);
          canvas->PutBlendPixel(dest_x + (double)x, dest_y + (double)y, &c, 255.0);
        }
    }
}


// Behaviour-preserving recombination of UI_SkinCanvas_BuildSoftShadowBitmap + UI_SkinCanvas_CompositeSoftShadowBitmap
// for the two call sites that do not (yet) cache: build, composite, delete, in one shot -- exactly what the
// original single-piece function used to do.
static bool UI_SkinCanvas_DrawSoftShadow_Impl(GRP2DCANVAS* canvas, double minx, double miny, double maxx, double maxy,
                                          double rTL, double rTR, double rBR, double rBL,
                                          UI_COLOR* shadow_color, int blur_radius)
{
  if(!canvas) return false;

  int shape_w = (int)(maxx - minx);
  int shape_h = (int)(maxy - miny);

  GRPBITMAP* bitmap = UI_SkinCanvas_BuildSoftShadowBitmap(shape_w, shape_h, rTL, rTR, rBR, rBL, shadow_color, blur_radius);
  if(!bitmap) return false;

  UI_SkinCanvas_CompositeSoftShadowBitmap(canvas, bitmap, minx, miny, blur_radius);

  GRPFACTORY::GetInstance().DeleteBitmap(bitmap);
  return true;
}


// TEMPORARY diagnostics -- thin timing wrapper around UI_SkinCanvas_DrawSoftShadow_Impl (renamed above), kept
// under the ORIGINAL name so the two non-Form call sites are untouched. Measures wall-clock time for the whole
// call and tallies it into diagskin_shadowus/diagskin_shadowcalls, read once per frame from UI_SYSTEM::DrawFrame().
// See XDiagLog.h.
static bool UI_SkinCanvas_DrawSoftShadow(GRP2DCANVAS* canvas, double minx, double miny, double maxx, double maxy,
                                          double rTL, double rTR, double rBR, double rBL,
                                          UI_COLOR* shadow_color, int blur_radius)
{
  XQWORD diagskin_shadow_t0 = XDIAGLOG_NOWUS();

  bool result = UI_SkinCanvas_DrawSoftShadow_Impl(canvas, minx, miny, maxx, maxy, rTL, rTR, rBR, rBL, shadow_color, blur_radius);

  diagskin_shadowus += (XDIAGLOG_NOWUS() - diagskin_shadow_t0);
  diagskin_shadowcalls++;

  return result;
}


// Box-shadow entry point used ONLY by Draw_Form (the one path dashboard.css actually exercises -- "form.card"
// is the sole "box-shadow" rule in the stylesheet, per the 2026-09 investigation). Reuses the already-blurred
// bitmap cached on "element_form" when the shadow's appearance (size/corner-radii/blur/colour) has not changed
// since it was last rendered, instead of rebuilding it from scratch on every redraw -- measured ~6.5ms per
// call before this cache, almost entirely in the build step (fill passes + per-pixel rounded-rect test + AGG
// blur), not the composite step, which still runs every time (see UI_SkinCanvas_CompositeSoftShadowBitmap).
// The other 2 call sites (the generic DrawElementBoxShadow helper, used by images/animations; and the
// ProgressBar track shadow) keep using the uncached UI_SkinCanvas_DrawSoftShadow above -- dashboard.css does
// not author box-shadow on those element types today, so they were left as-is rather than widening this
// change beyond what was actually measured.
static bool UI_SkinCanvas_DrawSoftShadow_FormCached(GRP2DCANVAS* canvas, UI_ELEMENT_FORM* element_form, double minx, double miny, double maxx, double maxy,
                                                      double rTL, double rTR, double rBR, double rBL,
                                                      UI_COLOR* shadow_color, int blur_radius)
{
  if(!canvas || !element_form) return false;

  XQWORD diagskin_shadow_t0 = XDIAGLOG_NOWUS();       // TEMPORARY diagnostics -- see XDiagLog.h

  int shape_w = (int)(maxx - minx);
  int shape_h = (int)(maxy - miny);

  GRPBITMAP* bitmap = element_form->ShadowCache_GetIfMatches(shape_w, shape_h, rTL, rTR, rBR, rBL, blur_radius, shadow_color);

  if(bitmap)
    {
      diagskin_shadowcachehits++;                     // TEMPORARY diagnostics
    }
   else
    {
      diagskin_shadowcachemiss++;                      // TEMPORARY diagnostics

      bitmap = UI_SkinCanvas_BuildSoftShadowBitmap(shape_w, shape_h, rTL, rTR, rBR, rBL, shadow_color, blur_radius);
      if(!bitmap)
        {
          diagskin_shadowus    += (XDIAGLOG_NOWUS() - diagskin_shadow_t0);   // TEMPORARY diagnostics
          diagskin_shadowcalls++;                                             // TEMPORARY diagnostics
          return false;
        }

      // Ownership transfers to the element's cache -- NOT deleted here. It is freed either the next time this
      // element's shadow key changes (ShadowCache_Set releases the old one first) or when the element itself
      // is destroyed (UI_ELEMENT_FORM::~UI_ELEMENT_FORM -> ShadowCache_Release).
      element_form->ShadowCache_Set(bitmap, shape_w, shape_h, rTL, rTR, rBR, rBL, blur_radius, shadow_color);
    }

  UI_SkinCanvas_CompositeSoftShadowBitmap(canvas, bitmap, minx, miny, blur_radius);

  diagskin_shadowus    += (XDIAGLOG_NOWUS() - diagskin_shadow_t0);   // TEMPORARY diagnostics
  diagskin_shadowcalls++;                                             // TEMPORARY diagnostics

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_SKINCANVAS::DrawElementBoxShadow(GRP2DCANVAS* canvas, UI_ELEMENT* element, double x_position, double y_position)
* @brief      Draws an element's box-shadow layer, generic across widget types.
* @note       Shared box-model helper. Same box-shadow behaviour Draw_Form has had since steps 7-8 (soft shadow via
*             UI_SkinCanvas_DrawSoftShadow/agg::stack_blur_rgba32 when blur > 0, hard-edged silhouette otherwise),
*             but written against base UI_ELEMENT accessors only (IsBoxShadowSet/GetShadowOffsetX/GetShadowOffsetY/
*             GetShadowBlur/GetShadowColor/HasAnyPerCornerRadius/GetEffectiveBorderRadius/GetRoundRect are all
*             declared on UI_ELEMENT itself, not UI_ELEMENT_FORM) so any Draw_X, in any UI_SKINCANVAS subclass,
*             can call it. Draw_Form's own inline copy of this logic is deliberately left untouched rather than
*             rewired to call this helper: it anchors the shadow on GetVisibleRect(), a UI_ELEMENT_FORM-specific
*             rect (a distinct stored member, not simply recomputed from the base boundary line) that this
*             function cannot reproduce without risking a behaviour change on the one widget that already shipped
*             and was verified. This helper instead anchors the shadow on (x_position, y_position) plus
*             element->GetBoundaryLine()->width/height -- the exact same geometry UI_SKINCANVAS::PreDrawFunction
*             already uses to expand the rebuild-area for a shadow on ANY element type (see its own "Step 7"
*             block), and the same (x_position, y_position) the calling Draw_X already uses to place its own
*             content -- so the shadow, the widget's own drawing, and the invalidation rectangle all agree on
*             where the element is, for every type this is wired into.
* @ingroup    USERINTERFACE
*
* @param[in]  canvas : Target canvas.
* @param[in]  element : Element whose box-shadow (if any) should be drawn. A no-op if IsBoxShadowSet() is false.
* @param[in]  x_position : Element's resolved left edge (screen coords), as already computed by PreDrawFunction.
* @param[in]  y_position : Element's resolved bottom edge (screen coords), as already computed by PreDrawFunction.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_SKINCANVAS::DrawElementBoxShadow(GRP2DCANVAS* canvas, UI_ELEMENT* element, double x_position, double y_position)
{
  if(!canvas)                        return;
  if(!element)                       return;
  if(!element->IsBoxShadowSet())     return;

  double width  = element->GetBoundaryLine()->width;
  double height = element->GetBoundaryLine()->height;

  double minx = UI_BOUNDARYLINE_EdgeLeft  (x_position, width);
  double maxx = UI_BOUNDARYLINE_EdgeRight (x_position, width);
  double miny = UI_BOUNDARYLINE_EdgeTop   (y_position, height);
  double maxy = UI_BOUNDARYLINE_EdgeBottom(y_position, height);

  double sh_x    = element->GetShadowOffsetX();
  double sh_y    = element->GetShadowOffsetY();
  double sh_blur = element->GetShadowBlur();

  double sh_minx = minx + sh_x;
  double sh_miny = miny + sh_y;
  double sh_maxx = maxx + sh_x;
  double sh_maxy = maxy + sh_y;

  // Resolve corner radii once: per-corner if any is authored, else uniform roundrect on all four -- identical
  // precedence to Draw_Form's own copy of this logic.
  double rTL, rTR, rBR, rBL;
  if(element->HasAnyPerCornerRadius())
    {
      rTL = element->GetEffectiveBorderRadius(UI_ELEMENT_BORDER_CORNER_TL);
      rTR = element->GetEffectiveBorderRadius(UI_ELEMENT_BORDER_CORNER_TR);
      rBR = element->GetEffectiveBorderRadius(UI_ELEMENT_BORDER_CORNER_BR);
      rBL = element->GetEffectiveBorderRadius(UI_ELEMENT_BORDER_CORNER_BL);
    }
   else
    {
      double r = (double)element->GetRoundRect();
      rTL = rTR = rBR = rBL = r;
    }

  bool soft_ok = false;
  if(sh_blur > 0.0)
    {
      soft_ok = UI_SkinCanvas_DrawSoftShadow(canvas, sh_minx, sh_miny, sh_maxx, sh_maxy,
                                            rTL, rTR, rBR, rBL,
                                            element->GetShadowColor(), (int)sh_blur);
    }

  if(!soft_ok)
    {
      // Hard shadow fallback (no blur, or canvas mode not 32-bit) -- same construction as Draw_Form's.
      GRP2DCOLOR_RGBA8  shadow_col(element->GetShadowColor()->GetRed(),
                                   element->GetShadowColor()->GetGreen(),
                                   element->GetShadowColor()->GetBlue(),
                                   element->GetShadowColor()->GetAlpha());

      GRP2DCOLOR_RGBA8  shadow_line_none(0, 0, 0, 0);

      canvas->SetFillColor(&shadow_col);
      canvas->SetLineColor(&shadow_line_none);
      canvas->SetLineWidth(1.0f);

      if(element->HasAnyPerCornerRadius())
        {
          GRP2DPATH shpath;
          AppendRoundRectPathPerCorner(shpath, sh_minx, sh_miny, sh_maxx, sh_maxy, rTL, rTR, rBR, rBL);
          canvas->Path(shpath, true);
        }
       else if(element->GetRoundRect())
        {
          canvas->RoundRect(sh_minx, sh_maxy, sh_maxx, sh_miny, element->GetRoundRect(), true);
        }
       else
        {
          canvas->Rectangle(sh_minx, sh_maxy, sh_maxx, sh_miny, true);
        }
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         static void UI_SkinCanvas_ProgressBar_DrawGradientRect(GRP2DCANVAS* canvas, double x1, double y1, double x2, double y2, double radius, GRP2DGRADIENTSTOP* stops, double gx1, double gy1, double gx2, double gy2)
* @brief      Fills a (optionally rounded) rect with a linear gradient along (gx1,gy1)->(gx2,gy2).
* @note       INTERNAL / FILE LOCAL. Used only when gradientcolor is set; the solid DrawRect path is left untouched.
* @ingroup    USERINTERFACE
* 
* @param[in]  canvas : Canvas object to use.
* @param[in]  x1 : X1 value.
* @param[in]  y1 : Y1 value.
* @param[in]  x2 : X2 value.
* @param[in]  y2 : Y2 value.
* @param[in]  radius : Radius value.
* @param[in]  stops : Stops pointer to use.
* @param[in]  gx1 : Gx1 value.
* @param[in]  gy1 : Gy1 value.
* @param[in]  gx2 : Gx2 value.
* @param[in]  gy2 : Gy2 value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  screen : Screen object to use.
* @param[in]  viewportindex : Viewportindex value.
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
* --------------------------------------------------------------------------------------------------------------------*/
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
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS_REBUILDAREAS::RebuildAllAreas()
{
  XDWORD max_z_level = 0;
  XDWORD nareas      = areas.GetSize();

  // TEMPORARY diagnostic-only (see XDiagLog.h): wall-clock the whole function, and count how many areas this
  // pass restores (z-level loop) vs. drops as orphaned (cleanup loop below) -- part of root-causing the
  // multi-second blank-freeze under investigation. diag_t0 shared across every return path below.
  XQWORD diag_t0             = XDIAGLOG_NOWUS();
  XDWORD diag_nareas_initial = nareas;
  XDWORD diag_restored       = 0;
  XDWORD diag_orphaned       = 0;

  if(!nareas) return true;

  // Invalidating one saved area can expose pixels owned by another area that overlaps it. Resolve that
  // dependency transitively BEFORE restoring a single bitmap: every affected element is then dirty when the
  // restore pass starts, so all layers are peeled in one transaction and no neighbour is deferred to the next
  // frame. Deferring even one overlap is observable as a one-frame empty card/menu flash.
  XDWORD ndirty_before = 0;
  XDWORD ndirty_after  = 0;

  do
    {
      ndirty_before = 0;

      for(XDWORD c=0; c<nareas; c++)
        {
          GRP2DREBUILDAREA* area = areas.Get(c);
          if(!area) continue;

          UI_ELEMENT* element = (UI_ELEMENT*)area->GetExtraData();
          if(element && (element->MustReDraw() || (!element->IsVisible()))) ndirty_before++;
        }

      for(XDWORD c=0; c<nareas; c++)
        {
          GRP2DREBUILDAREA* area = areas.Get(c);
          if(!area) continue;

          UI_ELEMENT* element = (UI_ELEMENT*)area->GetExtraData();
          if(!element || ((!element->MustReDraw()) && element->IsVisible())) continue;

          GRPBITMAP* bitmap = area->GetBitmap();
          if(bitmap) MarkOverlappingAreasDirty(area, bitmap, (int)c);
        }

      ndirty_after = 0;

      for(XDWORD c=0; c<nareas; c++)
        {
          GRP2DREBUILDAREA* area = areas.Get(c);
          if(!area) continue;

          UI_ELEMENT* element = (UI_ELEMENT*)area->GetExtraData();
          if(element && (element->MustReDraw() || (!element->IsVisible()))) ndirty_after++;
        }
    }
  while(ndirty_after > ndirty_before);

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

  #ifdef XTRACE_ACTIVE
  //XTRACE_PRINTCOLOR(XTRACE_COLOR_INFO, __L("[DEBUGCAPTION] UI_SKINCANVAS_REBUILDAREAS::RebuildAllAreas: nareas=%d max_z_level=%d"), (int)nareas, (int)max_z_level);
  #endif

  // Level zero is a real paint layer, not a sentinel. Every fatherless element keeps the default z_level=0,
  // including the cards and menu hit targets in UI_System. The old `level>0` loop never restored or deleted
  // those areas, so each redraw composited transparency and shadows over stale pixels indefinitely.
  for(int level=(int)max_z_level; level>=0; level--)
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
                      if(element->GetZLevel() == (XDWORD)level)
                        {
                          // XTRACE_PRINTCOLOR(XTRACE_COLOR_PURPLE, __L("Del area level [%d] [%s] "), element->GetZLevel(), element->GetName()->Get());
                          GRPBITMAP* bitmap = area->GetBitmap();
                          if(bitmap) PutBitmapNoAlpha(area->GetXPos(), area->GetYPos(), bitmap);

                          areas.Delete(area);
                          GEN_DELETE area;

                          diag_restored++;                    // TEMPORARY diagnostic-only, see XDiagLog.h
                        }
                    }
                }
            }
        }
    }

  // Discard orphaned areas -- one whose owning element is neither dirty nor invisible was skipped by EVERY
  // z-level pass above (the "if" they are gated on never matched at any level), meaning nothing is scheduled
  // to redraw over it right now. That area's saved bitmap is the background from BEFORE the element's most
  // recent (successful, already-completed) draw; the element's CURRENT on-screen content is already correct,
  // so there is nothing to "undo" -- restoring that old snapshot would erase perfectly good pixels for no
  // reason. Left registered, it would instead sit inertly for an unbounded number of frames and eventually get
  // consumed whenever the element happens to become dirty again for a LATER, unrelated reason (a different
  // section selected, a future global Elements_SetToRedraw()...); at that point its bitmap is no longer a
  // snapshot of "just before this redraw" but of some arbitrary earlier moment, possibly missing content that
  // other elements have since drawn over the same rectangle. Restoring it then paints a stale picture over the
  // current correct one, and nothing is left dirty to draw it back -- exactly the multi-frame "blank flash"
  // seen after clicking a sidebar navigation entry (UI_SYSTEM::UserInterface_SelectSection() marks the WHOLE
  // tree dirty at once, so every element redraws and clears its own flag the same frame it was set; any area
  // whose owning element does not need a SECOND redraw this same frame is, by construction, exactly one frame
  // old and already served its purpose). Dropping it now (without restoring) costs nothing: if the element
  // becomes dirty again later, PreDrawFunction() finds no existing area and creates a fresh one from whatever
  // is on screen at that moment, which is always at least as correct as an old, possibly-invalidated snapshot.
  nareas = areas.GetSize();

  for(int index = (int)nareas-1; index>=0; index--)
    {
      GRP2DREBUILDAREA* area = areas.Get(index);
      if(!area) continue;

      UI_ELEMENT* element = (UI_ELEMENT*)area->GetExtraData();
      if(!element) continue;

      if(element->MustReDraw() || (!element->IsVisible())) continue;  // still due a restore -- leave it alone

      areas.Delete(area);
      GEN_DELETE area;

      diag_orphaned++;                                        // TEMPORARY diagnostic-only, see XDiagLog.h
    }

  // TEMPORARY diagnostic-only (see XDiagLog.h): only written when this pass took an abnormal amount of wall-clock
  // time (>5ms) OR touched an unusually large number of areas, to keep the log small during normal operation
  // while still catching the pass whenever it is the slow one inside a stalled frame.
  { XQWORD diag_elapsed = XDIAGLOG_NOWUS() - diag_t0;
    if(diag_elapsed > 5000 || diag_nareas_initial > 20)
      {
        XDIAGLOG_WRITE("REBUILDAREAS", "nareas=%u restored=%u orphaned=%u elapsed=%lluus",
                        (unsigned)diag_nareas_initial, (unsigned)diag_restored, (unsigned)diag_orphaned, (unsigned long long)diag_elapsed);
      }
  }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_SKINCANVAS_REBUILDAREAS::MarkElementSubtreeDirty(UI_ELEMENT* element)
* @brief      Marks "element" and every element in its compose-children subtree as needing a redraw.
* @note       Root-cause fix for the "empty card" blank-flash defect: a rebuild area's neighbour is very often
*             an opaque container (a "form.card"/"form.inner-box"), not a leaf. Draw_Form() always visits every
*             child during its own redraw pass regardless of dirty state, but each child's own painter
*             (Draw_Text/Draw_Image/...) only emits pixels when THAT child's own MustReDraw() is true. Marking
*             only the container therefore repaints its background/border on top of whatever was already on
*             screen while its still-clean children never redraw to replace what just got painted over them --
*             a fully-formed but content-less card shell is exactly what gets presented that frame. Mirrors
*             UI_LAYOUT::Elements_SetToRedraw(element, true) (UI_Layout.cpp), repeated locally here because this
*             class only ever sees a bare UI_ELEMENT* (via a rebuild area's ExtraData) and has no UI_LAYOUT* to
*             call through -- see MarkOverlappingAreasDirty()'s own note on why this must stay inside the element
*             tree instead of reaching for a manager/layout singleton from inside the skin.
* @ingroup    USERINTERFACE
*
* @param[in]  element : Root of the subtree to mark; NULL is a no-op.
*
* @return     void.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_SKINCANVAS_REBUILDAREAS::MarkElementSubtreeDirty(UI_ELEMENT* element)
{
  if(!element) return;

  element->SetMustReDraw(true);

  for(XDWORD c=0; c<element->GetComposeElements()->GetSize(); c++)
    {
      UI_ELEMENT* subelement = (UI_ELEMENT*)element->GetComposeElements()->Get(c);
      if(subelement) MarkElementSubtreeDirty(subelement);
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_SKINCANVAS_REBUILDAREAS::MarkOverlappingAreasDirty(GRP2DREBUILDAREA* area, GRPBITMAP* bitmap, int excludeindex)
* @brief      Marks dirty every other still-registered rebuild area whose saved rectangle overlaps the rectangle
*             of "area", so it gets its OWN proper restore+redraw cycle instead of being skipped as "clean" while
*             its shadow silently keeps stacking on top of a neighbor's. RebuildAllAreas() calls this in a
*             fixed-point pass before restoring any bitmap, so transitive overlaps are part of the same frame.
* @note       This function deliberately changes flags only. Two independently-owned, partially-overlapping
*             snapshots cannot both be replayed immediately here without one re-introducing stale data the other
*             has already cleared. The subsequent Z-ordered restore pass writes each affected snapshot once.
* @ingroup    USERINTERFACE
*
* @param[in]  area         : Area about to be restored/deleted by the caller; its saved rectangle is the overlap
*                             reference (xpos/ypos plus "bitmap"'s width/height).
* @param[in]  bitmap       : Bitmap of "area", already fetched by the caller (avoids fetching it twice).
* @param[in]  excludeindex : Index of "area" inside "areas", skipped so it is never compared against itself.
*
* @return     void.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_SKINCANVAS_REBUILDAREAS::MarkOverlappingAreasDirty(GRP2DREBUILDAREA* area, GRPBITMAP* bitmap, int excludeindex)
{
  if((!area) || (!bitmap)) return;

  double x1 = area->GetXPos();
  double y1 = area->GetYPos();
  double w1 = (double)bitmap->GetWidth();
  double h1 = (double)bitmap->GetHeight();

  XDWORD nareas = areas.GetSize();

  for(int index = (int)nareas-1; index>=0; index--)
    {
      if(index == excludeindex) continue;

      GRP2DREBUILDAREA* neighborarea = areas.Get((XDWORD)index);
      if(!neighborarea) continue;

      GRPBITMAP* neighborbitmap = neighborarea->GetBitmap();
      if(!neighborbitmap) continue;

      double x2 = neighborarea->GetXPos();
      double y2 = neighborarea->GetYPos();
      double w2 = (double)neighborbitmap->GetWidth();
      double h2 = (double)neighborbitmap->GetHeight();

      bool overlaps = ((x1 < (x2+w2)) && (x2 < (x1+w1)) && (y1 < (y2+h2)) && (y2 < (y1+h1)));
      if(!overlaps) continue;

      // Just flag it dirty -- do NOT touch its bitmap or delete its area here. The restore phase starts only
      // after the complete overlap closure has been found, then restores it exactly once from its own snapshot.
      //
      // The flag must cover the neighbour's WHOLE subtree, not just the neighbour itself: see
      // MarkElementSubtreeDirty()'s note just above for why marking a single opaque container without its
      // children is exactly the "empty card" blank-flash defect (the container's own redraw repaints over its
      // children, and those children never redraw back because nothing marked THEM dirty).
      UI_ELEMENT* neighborelement = (UI_ELEMENT*)neighborarea->GetExtraData();
      if(neighborelement) MarkElementSubtreeDirty(neighborelement);
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_SKINCANVAS_REBUILDAREAS::RebuildAllAreas(UI_LAYOUT* layout)
* @brief      Rebuild all areas
* @ingroup    USERINTERFACE
* 
* @param[in]  layout : Layout pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  x : X coordinate.
* @param[in]  y : Y coordinate.
* @param[in]  width : Width value.
* @param[in]  height : Height value.
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  x : X coordinate.
* @param[in]  y : Y coordinate.
* @param[in]  width : Width value.
* @param[in]  height : Height value.
* 
* @return     GRPBITMAP* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  x : X coordinate.
* @param[in]  y : Y coordinate.
* @param[in]  bitmap : Bitmap pointer to use.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  element : Element to process.
* 
* @return     GRP2DREBUILDAREA* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @fn         UI_SKINCANVAS::UI_SKINCANVAS(GRPSCREEN* screen, int viewportindex) : UI_SKIN(), UI_SKINCANVAS_REBUILDAREAS(screen)
* @brief      Constructor of class
* @ingroup    USERINTERFACE
* 
* @param[in]  screen : Screen object to use.
* @param[in]  viewportindex : Viewport index (default 0).
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
* --------------------------------------------------------------------------------------------------------------------*/
UI_SKINCANVAS::~UI_SKINCANVAS()
{
  if(!fontpathfile.IsEmpty())
    {
      GEN_USERINTERFACE.DeleteTemporalUnZipFile(fontpathfile);
    }

  DeleteAllRebuildAreas();

  // Mirrors DeleteAllRebuildAreas() above but for our own, separate progressbackdrops cache (see UI_SkinCanvas.h
  // and Draw_ProgressBar()): GRP2DREBUILDAREA's destructor already frees its own captured GRPBITMAP, so
  // DeleteContents() correctly releases every cached backdrop bitmap here too.
  progressbackdrops.DeleteContents();
  progressbackdrops.DeleteAll();

  // Companion cache to progressbackdrops above (see UI_SkinCanvas.h and Draw_ProgressBar()): plain records,
  // no bitmaps to free, but still owned by this object and must be deleted here.
  progressbarlastbounds.DeleteContents();
  progressbarlastbounds.DeleteAll();

  // Mirrors progressbackdrops above but for the ALPHA-DARKENING FIX's own cache (see UI_SkinCanvas.h and
  // Draw_Form()): same reasoning, same cleanup.
  formbackdrops.DeleteContents();
  formbackdrops.DeleteAll();

  // Mirrors progressbackdrops above but for the RADIAL CAPTION GHOSTING FIX's own cache (see UI_SkinCanvas.h
  // and Draw_ProgressRadial()): same reasoning, same cleanup.
  radialbackdrops.DeleteContents();
  radialbackdrops.DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPSCREEN* UI_SKINCANVAS::GetScreen()
* @brief      Get screen
* @ingroup    USERINTERFACE
* 
* @return     GRPSCREEN* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @return     GRP2DCANVAS* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @fn         static double UI_SKINCANVAS_ResolveMaxSize(UI_ELEMENT* element, double fathersize)
* @brief      INTERNAL: corrects the one-pixel-short father size GetFatherSize() reports for a ROOT (fatherless)
*             element, but ONLY at the point a width="max" / height="max" resolution consumes it -- never for
*             GetFatherSize()'s other consumer, CalculePosition()'s xpos="right"/"center" and
*             ypos="up"/"down"/"center" resolution, which is left untouched on purpose.
* @note       GetFatherSize() reports "canvas size - 1" as a root element's father size, a "last valid pixel
*             index" convention (the same one SetClipBox(0,0,width-1,height-1) uses for the canvas' own clip
*             box). That is a defensible convention for a COORDINATE, but every CalculateBoundaryLine_X()
*             function also feeds the very same number into a "max" SIZE resolution, where it means something
*             different: "fill the father edge-to-edge". A root form with width="max" on a 1440 px canvas was
*             resolving to 1439 -- one column short of the window's own right edge -- while the exact same
*             form with an explicit width="1440" filled it completely. That mismatch is what left a sliver of
*             unpainted canvas behind a custom window chrome's caption bar when its width was left to default.
*             This helper closes that ONE pixel gap exactly where it is created (the "max" case), and only for
*             root elements (a child's father size never carries GetFatherSize()'s -1: it comes straight from
*             the father's own already-resolved UI_BOUNDARYLINE). Every other consumer of GetFatherSize()'s
*             output -- most notably CalculePosition(), called right after with the SAME fatherwidth/fatherheight
*             local variables this function does not touch -- keeps its pre-existing arithmetic byte for byte,
*             so no existing layout's positioning shifts because of this fix.
* @ingroup    USERINTERFACE
*
* @param[in]  element : the element being resolved (only element->GetFather() is inspected).
* @param[in]  fathersize : the father width or height as returned by GetFatherSize().
*
* @return     double : fathersize unchanged for a child element; fathersize + 1 for a root element.
*
* --------------------------------------------------------------------------------------------------------------------*/
static double UI_SKINCANVAS_ResolveMaxSize(UI_ELEMENT* element, double fathersize)
{
  return element->GetFather() ? fathersize : (fathersize + 1.0);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_SKINCANVAS::GetFatherSize(UI_ELEMENT* element, double& width, double& height)
* @brief      Get father size
* @ingroup    USERINTERFACE
*
* @param[in]  element : Element to process.
* @param[in]  width : Width value.
* @param[in]  height : Height value.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  position : Position value.
* 
* @return     double : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  element : Element to process.
* @param[in]  fatherwidth : Fatherwidth value.
* @param[in]  fatherheight : Fatherheight value.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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

  // --- Step 4: father padding delta -----------------------------------------------------------------------------
  // Apply the father's padding as a delta on top of the alignment already resolved above. The delta depends on
  // WHICH alignment / numeric mode the child uses, because padding shifts the reference edge differently for
  // each case (LEFT-aligned or numeric-x shifts right by pad_L; RIGHT-aligned shifts left by pad_R; CENTER by
  // half the padding differential). Zero padding leaves this a full no-op, preserving the pre-step-4 geometry
  // of every layout that does not set the property.
  if(element->GetFather())
    {
      UI_ELEMENT* father = element->GetFather();

      double pad_L = father->GetPadding(UI_ELEMENT_TYPE_ALIGN_LEFT);
      double pad_R = father->GetPadding(UI_ELEMENT_TYPE_ALIGN_RIGHT);
      double pad_T = father->GetPadding(UI_ELEMENT_TYPE_ALIGN_UP);
      double pad_B = father->GetPadding(UI_ELEMENT_TYPE_ALIGN_DOWN);

      if(pad_L != 0.0 || pad_R != 0.0 || pad_T != 0.0 || pad_B != 0.0)
        {
          switch((int)element->GetBoundaryLine()->x)
            {
              case UI_ELEMENT_TYPE_ALIGN_LEFT   : x_position += pad_L;                                     break;
              case UI_ELEMENT_TYPE_ALIGN_RIGHT  : x_position -= pad_R;                                     break;
              case UI_ELEMENT_TYPE_ALIGN_CENTER : x_position += (int)round((pad_L - pad_R) / 2.0);         break;
                                        default : x_position += pad_L;                                     break;
            }

          // Y is bottom-anchored in GEN's internal storage: increasing y moves DOWN on screen. Padding shifts
          // the interior origin the same way it does in CSS, but the sign flips for numeric / DOWN cases
          // because they measure "distance from the bottom edge".
          switch((int)element->GetBoundaryLine()->y)
            {
              case UI_ELEMENT_TYPE_ALIGN_UP     : y_position += pad_T;                                     break;
              case UI_ELEMENT_TYPE_ALIGN_DOWN   : y_position -= pad_B;                                     break;
              case UI_ELEMENT_TYPE_ALIGN_CENTER : y_position += (int)round((pad_T - pad_B) / 2.0);         break;
                                        default : y_position -= pad_B;                                     break;
            }
        }
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
* @param[in]  string : String to use.
* @param[in]  sizefont : Sizefont value.
* 
* @return     double : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  string : String to use.
* @param[in]  sizefont : Sizefont value.
* 
* @return     double : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
      case UI_ELEMENT_TYPE_ALIGN_MAX   : element->GetBoundaryLine()->width = UI_SKINCANVAS_ResolveMaxSize(element, fatherwidth);                                               break;	    
    }
   
  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO	 :		 		
      case                           0 : element->GetBoundaryLine()->height  = (double)canvas->VectorFont_GetHeight(string_max.Get());  break;
      case UI_ELEMENT_TYPE_ALIGN_MAX   : element->GetBoundaryLine()->height  = UI_SKINCANVAS_ResolveMaxSize(element, fatherheight);                                            break;	    
    }

  return CalculePosition(element, fatherwidth, fatherheight, adjustsizemargin);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_TextBox(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line text box
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->width = UI_SKINCANVAS_ResolveMaxSize(element, fatherwidth);                                          break;	    
    }
   
  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO	  :		 		
      case                            0 : element->GetBoundaryLine()->height  = (double)element_image->GetImage()->GetHeight();     break;
      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->height  = UI_SKINCANVAS_ResolveMaxSize(element, fatherheight);                                       break;	    
    }

  return CalculePosition(element, fatherwidth, fatherheight, adjustsizemargin);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_Animation(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line animation
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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

      case UI_ELEMENT_TYPE_ALIGN_MAX      : element->GetBoundaryLine()->width = UI_SKINCANVAS_ResolveMaxSize(element, fatherwidth);                                          
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

      case UI_ELEMENT_TYPE_ALIGN_MAX      : element->GetBoundaryLine()->height  = UI_SKINCANVAS_ResolveMaxSize(element, fatherheight);                                       
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
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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

          case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->width = UI_SKINCANVAS_ResolveMaxSize(element, fatherwidth);                                          
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

          case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->height  = UI_SKINCANVAS_ResolveMaxSize(element, fatherheight);                                       
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
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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

      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->width = UI_SKINCANVAS_ResolveMaxSize(element, fatherwidth);                                          
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

      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->height  = UI_SKINCANVAS_ResolveMaxSize(element, fatherheight);                                       
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
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
      case UI_ELEMENT_TYPE_ALIGN_MAX  : element->GetBoundaryLine()->width = UI_SKINCANVAS_ResolveMaxSize(element, fatherwidth);                                                break;	    
    }
   
  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO :		 		
      case                          0 : element->GetBoundaryLine()->height  = (double)canvas->VectorFont_GetHeight(string_max.Get());   break;
      case UI_ELEMENT_TYPE_ALIGN_MAX  : element->GetBoundaryLine()->height  = UI_SKINCANVAS_ResolveMaxSize(element, fatherheight);                                             break;	    
    }

  return CalculePosition(element, fatherwidth, fatherheight, adjustsizemargin);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_Form(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line form
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::CalculateBoundaryLine_Form(UI_ELEMENT* element, bool adjustsizemargin)
{
  UI_ELEMENT_FORM* element_form = (UI_ELEMENT_FORM*)element;
  if(!element_form) return false;   

  double fatherwidth  = 0.0f;
  double fatherheight = 0.0f;

  GetFatherSize(element, fatherwidth, fatherheight);

  // --- Custom window chrome: the caption bar spans the whole window, always ---------------------------------------
  //
  // A form carrying role="caption" is the background bar of a GEN custom window chrome (see GRPSCREEN's
  // CFG Chromes). Its width is not a design decision of the layout: it IS the width of the window the chrome
  // is applied to. Resolving it here -- instead of making every chrome .xml hardcode a number -- is what lets
  // the very same chrome bundle be reused by applications of different sizes.
  //
  // Without this, a caption authored with no "width" attribute falls into the generic AUTO/0 case below, which
  // shrink-wraps the form around its own children. The bar then collapses to roughly the width of the title
  // text, and every child positioned with xpos="right" (minimize / maximize / close) resolves against that
  // collapsed width and lands on top of the icon -- the "only the caption icon shows" symptom.
  //
  // The value used is the FULL canvas width, not GetFatherSize()'s canvas width - 1: that -1 is the generic
  // "maximum valid coordinate" convention for width="max", and one pixel of bare canvas at the right end of
  // the caption bar is visible. A caption nested inside another element (unusual, but legal) simply takes its
  // father's width. An explicitly authored numeric width still wins: it is not touched here.
  if(element->GetChromeRole() == UI_ELEMENT_CHROMEROLE_CAPTION)
    {
      switch((int)element->GetBoundaryLine()->width)
        {
          case UI_ELEMENT_TYPE_ALIGN_AUTO :
          case UI_ELEMENT_TYPE_ALIGN_MAX  :
          case                          0 : { double captionwidth = fatherwidth;

                                              if(!element->GetFather())
                                                {
                                                  GRP2DCANVAS* canvas = GetCanvas();
                                                  if(canvas) captionwidth = (double)canvas->GetWidth();
                                                }

                                              element->GetBoundaryLine()->width = captionwidth;
                                            }
                                            break;
        }
    }

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

      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->width = UI_SKINCANVAS_ResolveMaxSize(element, fatherwidth);
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

      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->height  = UI_SKINCANVAS_ResolveMaxSize(element, fatherheight);                                       
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
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
      case UI_ELEMENT_TYPE_ALIGN_MAX  : element->GetBoundaryLine()->width = UI_SKINCANVAS_ResolveMaxSize(element, fatherwidth);                                                break;	    
    }
   
  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO	:		 		
      case                          0 : element->GetBoundaryLine()->height  = (double)canvas->VectorFont_GetHeight(string_max.Get());   break;
      case UI_ELEMENT_TYPE_ALIGN_MAX  : element->GetBoundaryLine()->height  = UI_SKINCANVAS_ResolveMaxSize(element, fatherheight);                                             break;	    
    }

  return CalculePosition(element, fatherwidth, fatherheight, adjustsizemargin);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_SKINCANVAS::ReapplyProgressBarAllocationLayout(UI_ELEMENT_PROGRESSBAR* element_progressbar, bool adjustsizemargin)
* @brief      Positions element_progressrect/element_animation/element_text against element_progressbar's OWN
*             (already resolved) box and applies the allocationtext-driven shift (DOWN/UP/LEFT move the rect/
*             animation to free up room for the caption).
* @note       ROOT-CAUSE FIX (2026-09, fourth pass): extracted out of CalculateBoundaryLine_ProgressBar() (which
*             still calls this immediately after resolving element_progressbar's own position/size) so that
*             UI_MANAGER::RefreshFlexProgressBarTracks() (UI_Manager.cpp) can call the EXACT same logic.
*
*             That function is a post-load hook that runs, unconditionally, for every ProgressBar right after
*             UI_LAYOUTENGINE::RunLayout() -- so that a ProgressBar which is itself a flex item gets its track
*             re-anchored to its flex-resolved position (see that function's own banner: without this, a flex
*             ProgressBar's track stayed stuck near the origin and never rendered at all). Its OLD implementation
*             called only the bare CalculePosition(element_progressrect, ...) -- the same first step
*             CalculateBoundaryLine_ProgressBar() takes -- and stopped there, never reapplying the allocationtext
*             switch below. For allocationtext="none"/"center" that is harmless (neither shifts the rect), which
*             is exactly why this went unnoticed; but for "down"/"up"/"left" -- e.g. progressbar3 in UI_Options'
*             example.xml, allocationtext="down" -- CalculateBoundaryLine_ProgressBar() (called moments earlier,
*             from CreatePartialLayout()) had ALREADY shifted element_progressrect to free up room for the
*             caption underneath it, and this post-load hook's bare CalculePosition() call silently RESET it back
*             to its natural, unshifted position, undoing that shift for the entire lifetime of the very first
*             frame. Confirmed live (see the GHOST-FILL FIX comment in Draw_ProgressBar()): the rect renders
*             unshifted (overlapping the caption's own zone) from the very first frame, and only jumps to its
*             correct shifted position the first time something ELSE re-runs CalculateBoundaryLine_ProgressBar()
*             (a real value change) -- a full-fledged, one-time geometry jump between "load" and "first value
*             change" that every downstream restore/redraw mechanism (including the widget-level backdrop cache
*             added alongside this fix) has to treat as a real, on-screen move, when the truly correct behaviour
*             is for the rect to already be at its final, shifted position from the very first frame and never
*             move again for this reason.
* @ingroup    USERINTERFACE
*
* @param[in]  element_progressbar : The progress-bar element itself; NULL is a no-op. Its own position/size are
*                                    read here, never written -- the caller (either call site) is responsible for
*                                    those being correct before calling this.
* @param[in]  adjustsizemargin : Forwarded to every CalculePosition() call below, exactly as CalculateBoundaryLine_ProgressBar() does.
*
* @return     void.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_SKINCANVAS::ReapplyProgressBarAllocationLayout(UI_ELEMENT_PROGRESSBAR* element_progressbar, bool adjustsizemargin)
{
  if(!element_progressbar) return;

  UI_ELEMENT*           element_progressrect  = element_progressbar->GetProgressRect();
  UI_ELEMENT_ANIMATION* element_animation     = (UI_ELEMENT_ANIMATION*)element_progressbar->Get_UIAnimation();
  UI_ELEMENT_TEXT*      element_text          = (UI_ELEMENT_TEXT*)element_progressbar->Get_UIText();

  if(element_progressrect)  CalculePosition(element_progressrect  , element_progressbar->GetBoundaryLine()->width, element_progressbar->GetBoundaryLine()->height, adjustsizemargin);
  if(element_animation)     CalculePosition(element_animation     , element_progressbar->GetBoundaryLine()->width, element_progressbar->GetBoundaryLine()->height, adjustsizemargin);
  if(element_text)          CalculePosition(element_text          , element_progressbar->GetBoundaryLine()->width, element_progressbar->GetBoundaryLine()->height, adjustsizemargin);

  // DEEP ROOT-CAUSE FIX (2026-09, third pass -- see the GHOST-FILL FIX comment in Draw_ProgressBar() for the
  // visual symptom this was ultimately traced back to): CalculePosition() just above gives element_progressrect/
  // element_animation/element_text their NATURAL, unshifted positions -- captured here, ONCE, before the
  // allocationtext switch below moves anything. Every branch of that switch MUST compute its shifted
  // position(s) from these frozen natural values, never from GetXPosition()/GetYPosition() read again after a
  // shift has already been applied -- otherwise a position that looks like a one-time "make room for the
  // caption" adjustment is actually INCREMENTAL: calling this function a second time (this function is re-run
  // on every real value change, and, it turns out, on every dirty tick while an element stays marked dirty --
  // see below) reads back the ALREADY-shifted coordinate and shifts it again by the same amount, silently
  // drifting the rect/animation further and further off its correct position on every call, with nothing to
  // ever bring it back. That drift was confirmed live: element_progressrect's own Y position for progressbar3
  // (allocationtext="down") changed on successive calls instead of converging, which in turn kept the widget's
  // on-screen boundary line "changing" every tick and thereby kept it (and its caption) marked dirty forever --
  // a second-order perpetual-redraw bug on top of the original one-shot mispositioning, and the true reason a
  // caption that DOES get redrawn every single tick can still end up invisible: whatever it draws one tick, a
  // still-drifting rect (or a backdrop-restore keyed to the widget's nominal box) can legitimately no longer
  // agree with the very next tick. A previous pass here already made element_text's OWN position absolute for
  // the DOWN and LEFT cases (see the two comments retained below); this pass finishes the job for
  // element_progressrect, element_animation, and the UP case's element_text, using the same principle
  // throughout: every SetXPosition()/SetYPosition() below is computed from a NATURAL baseline captured once,
  // so calling this function any number of times with the same inputs always produces the same output.
  double rect_natural_x = element_progressrect ? element_progressrect->GetXPosition() : 0.0;
  double rect_natural_y = element_progressrect ? element_progressrect->GetYPosition() : 0.0;
  double anim_natural_x = element_animation    ? element_animation->GetXPosition()    : 0.0;
  double anim_natural_y = element_animation    ? element_animation->GetYPosition()    : 0.0;
  double text_natural_y = element_text         ? element_text->GetYPosition()         : 0.0;

  switch(element_progressbar->GetAllocationTextType())
    {
      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_NONE    : break;

      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_DOWN    : if(element_animation && element_text)     element_animation->SetYPosition(anim_natural_y - element_text->GetBoundaryLine()->height);
                                                            if(element_progressrect && element_text)  element_progressrect->SetYPosition(rect_natural_y - (element_text->GetBoundaryLine()->height + 6));
                                                            if(element_progressrect && element_text)  element_text->SetXPosition(element_progressrect->GetXPosition() + ((element_progressrect->GetBoundaryLine()->width - element_text->GetBoundaryLine()->width) / 2.0));   // horizontal center on the bar
                                                            // ROOT-CAUSE FIX (2026-09, confirmed live against progressbar3 in UI_Options' example.xml,
                                                            // allocationtext="down"): the line above moves element_progressrect UP by
                                                            // (text height + 6) to free up room for the caption underneath it, but this function
                                                            // never used to reposition element_text itself along that same axis -- element_text
                                                            // kept whatever Y CalculePosition(element_text, ...) gave it moments earlier (the SAME
                                                            // "natural" baseline element_progressrect ALSO started from, before its own shift).
                                                            // The very first time this function runs for a given element (layout parse), that
                                                            // stale Y happens to still read correctly on screen because nothing has repainted the
                                                            // gap yet; the first REAL value change afterwards re-enters this function (see
                                                            // ChangeTextElementValue's progress-type-father special case) and reapplies the SAME
                                                            // rect shift, but Draw_ProgressBar's own track/fill painting only ever covers
                                                            // element_progressrect's (now shifted) rect -- never the stale, unshifted caption
                                                            // position -- so that row of the canvas is left with whatever raw, uninitialized
                                                            // (fully transparent) pixels were there, and the freshly-drawn glyph blends against
                                                            // that instead of an opaque background: exactly the "letters overlap / caption
                                                            // vanishes" defect reported on progress widgets after their first value change.
                                                            // Fix: reposition element_text into the space element_progressrect's shift just
                                                            // freed up, the same way every OTHER branch below positions text relative to the
                                                            // rect's post-shift position (absolute, not incremental -- safe to call this
                                                            // function any number of times with an identical result each time).
                                                            if(element_progressrect && element_text)  element_text->SetYPosition(element_progressrect->GetYPosition() + element_text->GetBoundaryLine()->height + 6);
                                                            break;

      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_UP      : { double shifted_text_y = text_natural_y;
                                                            if(element_animation && element_text)     shifted_text_y -= element_animation->GetBoundaryLine()->height;
                                                            if(element_progressrect && element_text)  shifted_text_y -= (element_progressrect->GetBoundaryLine()->height + 6);
                                                            if(element_text)                          element_text->SetYPosition(shifted_text_y);
                                                            if(element_progressrect && element_text)  element_text->SetXPosition(element_progressrect->GetXPosition() + ((element_progressrect->GetBoundaryLine()->width - element_text->GetBoundaryLine()->width)/2));   // horizontal center on the bar
                                                            break; }

      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_RIGHT   : if(element_progressrect && element_text)
                                                              {
                                                                element_text->SetXPosition(element_progressrect->GetXPosition() + element_progressrect->GetBoundaryLine()->width + 6);                                              // right of the bar (+gap), absolute not incremental
                                                                element_text->SetYPosition(element_progressrect->GetYPosition() - ((element_progressrect->GetBoundaryLine()->height - element_text->GetBoundaryLine()->height)/2));   // vertical center on the bar
                                                              }
                                                            break;

      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_LEFT    : if(element_animation && element_text)     element_animation->SetXPosition(anim_natural_x + element_text->GetBoundaryLine()->width);
                                                            if(element_progressrect && element_text)  element_progressrect->SetXPosition(rect_natural_x + element_text->GetBoundaryLine()->width);
                                                            if(element_progressrect && element_text)  element_text->SetYPosition(element_progressrect->GetYPosition() - ((element_progressrect->GetBoundaryLine()->height - element_text->GetBoundaryLine()->height)/2));   // vertical center on the bar
                                                            // ROOT-CAUSE FIX (2026-09): same class of bug as ALLOCATION_TEXT_TYPE_DOWN above --
                                                            // element_progressrect is shifted RIGHT by element_text's width to free up room for
                                                            // the caption on its left, but element_text's own X position was never set here at
                                                            // all, so it kept the stale X CalculePosition(element_text, ...) gave it (the same
                                                            // pre-shift baseline the rect started from). Absolute, not incremental -- safe to
                                                            // call this function any number of times with an identical result each time.
                                                            if(element_progressrect && element_text)  element_text->SetXPosition(element_progressrect->GetXPosition() - element_text->GetBoundaryLine()->width);
                                                            break;

      case UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_CENTER  : if(element_progressrect && element_text)
                                                              {
                                                                element_text->SetXPosition(element_progressrect->GetXPosition() + ((element_progressrect->GetBoundaryLine()->width  - element_text->GetBoundaryLine()->width)/2));
                                                                element_text->SetYPosition(element_progressrect->GetYPosition() - ((element_progressrect->GetBoundaryLine()->height - element_text->GetBoundaryLine()->height)/2));
                                                              }
                                                            break;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_ProgressBar(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate boundary line progress bar
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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

          case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->width = UI_SKINCANVAS_ResolveMaxSize(element, fatherwidth);                                          
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

          case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->height  = UI_SKINCANVAS_ResolveMaxSize(element, fatherheight);                                       
                                              break;	    
        }
    }
  
  CalculePosition(element_progressbar, fatherwidth, fatherheight, adjustsizemargin);

  // Sub-element positioning + the allocationtext-driven shift (element_progressrect/element_animation/
  // element_text against element_progressbar's OWN box, just resolved above) is shared with
  // UI_MANAGER::RefreshFlexProgressBarTracks() (UI_Manager.cpp) -- see ReapplyProgressBarAllocationLayout()'s
  // own banner and .cpp comment for why that function needs the EXACT same logic, not a hand-rolled subset.
  ReapplyProgressBarAllocationLayout(element_progressbar, adjustsizemargin);

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
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->width  = UI_SKINCANVAS_ResolveMaxSize(element, fatherwidth);                                                                                    break;
    }

  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO   :
      case                           0  : element->GetBoundaryLine()->height = (element->GetBoundaryLine()->width > 0) ? element->GetBoundaryLine()->width : fatherheight;     break;
      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->height = UI_SKINCANVAS_ResolveMaxSize(element, fatherheight);                                                                                   break;
    }

  CalculePosition(element_progress, fatherwidth, fatherheight, adjustsizemargin);

  // Center the caption inside the progress box (the child <text> is expected to use xpos="center" ypos="center").
  if(element_text) CalculePosition(element_text, element_progress->GetBoundaryLine()->width, element_progress->GetBoundaryLine()->height, adjustsizemargin);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         static void UI_SkinCanvas_ProgressImage_Layout(int allocation, double imgw, double imgh, double textw, double texth, double gap, double& bw, double& bh, double& ox, double& oy, double& tx, double& ty)
* @brief      Calculate boundary line progress image
* @ingroup    USERINTERFACE
* 
* @param[in]  allocation : Allocation value.
* @param[in]  imgw : Imgw value.
* @param[in]  imgh : Imgh value.
* @param[in]  textw : Textw value.
* @param[in]  texth : Texth value.
* @param[in]  gap : Gap value.
* @param[in]  bw : Bw value.
* @param[in]  bh : Bh value.
* @param[in]  ox : Ox value.
* @param[in]  oy : Oy value.
* @param[in]  tx : Tx value.
* @param[in]  ty : Ty value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         static void UI_SkinCanvas_ProgressImage_Layout(int allocation, double imgw, double imgh, double textw, double texth, double gap, double& bw, double& bh, double& ox, double& oy, double& tx, double& ty)
* @brief      Computes the ProgressImage caption layout for the current allocationtext.
* @ingroup    USERINTERFACE
* 
* @param[in]  allocation : Allocation value.
* @param[in]  imgw : Imgw value.
* @param[in]  imgh : Imgh value.
* @param[in]  textw : Textw value.
* @param[in]  texth : Texth value.
* @param[in]  gap : Gap value.
* @param[in]  bw : Bw value.
* @param[in]  bh : Bh value.
* @param[in]  ox : Ox value.
* @param[in]  oy : Oy value.
* @param[in]  tx : Tx value.
* @param[in]  ty : Ty value.
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


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::CalculateBoundaryLine_ProgressImage(UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Calculate Boundary Line Progress Image
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->width  = UI_SKINCANVAS_ResolveMaxSize(element, fatherwidth);  break;
    }

  switch((int)element->GetBoundaryLine()->height)
    {
      case UI_ELEMENT_TYPE_ALIGN_AUTO   :
      case                           0  : element->GetBoundaryLine()->height = bh;           break;
      case UI_ELEMENT_TYPE_ALIGN_MAX    : element->GetBoundaryLine()->height = UI_SKINCANVAS_ResolveMaxSize(element, fatherheight); break;
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
* @param[in]  element : Element to process.
* @param[in]  x_position : X position value.
* @param[in]  y_position : Y position value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
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

      // "text-align" (Step 10, see UI_ELEMENT::GetTextAlign() / UI_MANAGER::GetLayoutElement_Base). GEN's
      // VectorFont_Print(x, y, ...) always takes the LEFT edge of the string as its origin; CENTER/RIGHT are
      // implemented here, locally, as a one-off x offset computed from the string's own measured width against
      // the element's box width -- nothing in the canvas or layout layer needs to know alignment exists. LEFT
      // (the default for every element created before this) reduces to offset 0.0: pixel-identical to before.
      double alignoffset = 0.0f;

      switch(element_text->GetTextAlign())
        {
          case UI_ELEMENT_TYPE_ALIGN_CENTER : alignoffset = (element_text->GetBoundaryLine()->width - canvas->VectorFont_GetWidth(element_text->GetText()->Get())) / 2.0;
                                               break;

          case UI_ELEMENT_TYPE_ALIGN_RIGHT  : alignoffset = element_text->GetBoundaryLine()->width - canvas->VectorFont_GetWidth(element_text->GetText()->Get());
                                               break;

          default                           : break; // LEFT (and any non-horizontal enum value): no offset.
        }

      canvas->VectorFont_Print(x_position + alignoffset, y_position, element_text->GetText()->Get());
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
      // Box-shadow (see UI_SKINCANVAS::DrawElementBoxShadow): drawn before the bitmap, same "shadow first, own
      // content on top" ordering Draw_Form uses. A no-op when the element does not declare box-shadow, so this
      // is zero behaviour change for every existing image layout.
      DrawElementBoxShadow(canvas, element, x_position, y_position);

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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
      // ALPHA-DARKENING FIX (2026-09): steps 4-8 below (box-shadow, then fill+stroke) always paint by
      // ALPHA-BLENDING onto whatever is already on the canvas (AGG "over" compositing) -- never a plain
      // overwrite -- because that is what lets a translucent color (e.g. color="black,50", used by both
      // "form" and "menu" elements for a frosted-glass panel look) show the real background through it. A
      // form/menu that gets marked MustReDraw() again later WITHOUT ever moving or becoming invisible (the
      // common case: some unrelated interaction elsewhere in the layout calls a global
      // Elements_SetToRedraw(), which recursively re-dirties this element and its children too) simply
      // repaints the SAME nominal translucent color on top of its own earlier repaint of itself -- and
      // standard "over" compositing of a color onto itself compounds toward full opacity every time (50%
      // over 50% = 75%, then 87.5%, ...), NOT a no-op. Confirmed live via canvas->GetBitmap() buffer reads on
      // "ListBoxMenuID" (color="black,50"): stored alpha measured 127 -> 191 -> 242 across three consecutive
      // real redraws, each one triggered by selecting a different row (an unrelated, recursive
      // Elements_SetToRedraw() call, not a change to this element's own geometry or color) -- visually, the
      // translucent panel darkens toward solid black a little more on every such redraw, exactly as reported.
      //
      // Fix: cache the TRUE backdrop (the real pixels behind this element, e.g. the seamless background
      // pattern) once, the very first time this element is ever drawn -- before any of its own shadow, fill,
      // border or children have painted anything -- and restore it (PutBitmapNoAlpha(), GEN's own real
      // restore primitive, never a synthetic fill colour) immediately before repainting on every later real
      // redraw, so the translucent blend below always starts fresh instead of compounding on top of itself.
      //
      // Only done when this element actually owns its own rebuild-area THIS tick (GetRebuildAreaByElement(),
      // set a few lines above by PreDrawFunction()): that area's geometry is already correctly expanded for
      // the box-shadow footprint (see PreDrawFunction()'s own step-7 padding), so reusing it here keeps the
      // captured/restored box perfectly in sync with the shadow without duplicating that padding logic. When
      // this element is instead a descendant of an already-dirty ancestor (no area of its own), it is left
      // alone: the ancestor's own redraw -- running first, higher up this same recursive Draw() walk -- is
      // trusted to already have cleared/reset the canvas beneath before this descendant ever paints, exactly
      // the same "ancestor covers descendant" assumption the rest of this rebuild-area system relies on.
      GRP2DREBUILDAREA* ownarea = GetRebuildAreaByElement(element);

      if(ownarea)
        {
          GRP2DREBUILDAREA* formbackdrop = FormBackdrop_Find(element);

          if(!formbackdrop)
            {
              // First time this element is ever drawn: nothing has painted shadow/fill/border/children ink
              // here yet, so this is the one guaranteed-pristine moment to capture the true backdrop. Reuses
              // ownarea's already-computed box (position + size, shadow footprint included) rather than
              // recomputing it here.
              FormBackdrop_Capture(element, ownarea->GetXPos(), ownarea->GetYPos(),
                                    (double)ownarea->GetBitmap()->GetWidth(), (double)ownarea->GetBitmap()->GetHeight());
            }
           else
            {
              // Not the first draw: restore the true backdrop now, before repainting below.
              PutBitmapNoAlpha(formbackdrop->GetXPos(), formbackdrop->GetYPos(), formbackdrop->GetBitmap());

              // Restoring just wiped any ink our own children (an edit field, listbox rows, a scrollbar...)
              // already painted on earlier ticks. The children loop further down in this function already
              // calls Draw() on every child unconditionally, but each Draw_X() only actually repaints when
              // THAT child's own MustReDraw() is set -- so a child not ALSO marked dirty this exact tick
              // would be erased by the restore above and never repainted. Force every direct child dirty here
              // so the loop below always repaints it on top of the freshly restored backdrop. Harmless when a
              // child WAS already going to redraw anyway (repainting unchanged content is idempotent, the
              // same reasoning already relied on elsewhere in this file -- see the GHOST-FILL FIX comment in
              // Draw_ProgressBar()).
              for(XDWORD c=0; c<element_form->GetComposeElements()->GetSize(); c++)
                {
                  UI_ELEMENT* formchild = (UI_ELEMENT*)element_form->GetComposeElements()->Get(c);
                  if(formchild) formchild->SetMustReDraw(true);
                }
            }
        }

      // Element geometry (screen coords, y-down). Shared by the shadow layer (step 7) and the fill+stroke
      // layer (steps 4-5-6) below.
      double  vr_minx = element_form->GetVisibleRect()->x;
      double  vr_miny = element_form->GetVisibleRect()->GetTop();
      double  vr_maxx = element_form->GetVisibleRect()->x + element_form->GetVisibleRect()->width;
      double  vr_maxy = element_form->GetVisibleRect()->y;

      // Step 7-8: box-shadow. Drawn BEFORE the fill so the form paints on top of it. When blur > 0 the skin
      // rasterises the silhouette into an off-screen RGBA bitmap, applies a 3-pass box blur (Gaussian
      // approximation, visually equivalent to agg::stack_blur_rgba32 for typical UI radii) and composites
      // via PutBitmapAlpha. Falls back to the hard-edged variant when the canvas mode is not 32-bit
      // RGBA/BGRA, when blur is 0, or when the off-screen allocation fails. The rebuild-area was already
      // expanded in PreDrawFunction to include the blurred footprint so save/restore is safe.
      if(element_form->IsBoxShadowSet())
        {
          double sh_x    = element_form->GetShadowOffsetX();
          double sh_y    = element_form->GetShadowOffsetY();
          double sh_blur = element_form->GetShadowBlur();

          double sh_minx = vr_minx + sh_x;
          double sh_miny = vr_miny + sh_y;
          double sh_maxx = vr_maxx + sh_x;
          double sh_maxy = vr_maxy + sh_y;

          // Resolve corner radii once: per-corner if any is authored, else uniform roundrect on all four.
          double rTL, rTR, rBR, rBL;
          if(element_form->HasAnyPerCornerRadius())
            {
              rTL = element_form->GetEffectiveBorderRadius(UI_ELEMENT_BORDER_CORNER_TL);
              rTR = element_form->GetEffectiveBorderRadius(UI_ELEMENT_BORDER_CORNER_TR);
              rBR = element_form->GetEffectiveBorderRadius(UI_ELEMENT_BORDER_CORNER_BR);
              rBL = element_form->GetEffectiveBorderRadius(UI_ELEMENT_BORDER_CORNER_BL);
            }
           else
            {
              double r = (double)element_form->GetRoundRect();
              rTL = rTR = rBR = rBL = r;
            }

          bool soft_ok = false;
          if(sh_blur > 0.0)
            {
              // Cached path (see UI_SkinCanvas_DrawSoftShadow_FormCached): reuses the already-blurred bitmap
              // on "element_form" across frames instead of rebuilding it every redraw.
              soft_ok = UI_SkinCanvas_DrawSoftShadow_FormCached(canvas, element_form, sh_minx, sh_miny, sh_maxx, sh_maxy,
                                                    rTL, rTR, rBR, rBL,
                                                    element_form->GetShadowColor(), (int)sh_blur);
            }

          if(!soft_ok)
            {
              // Hard shadow fallback (no blur, or canvas mode not 32-bit).
              GRP2DCOLOR_RGBA8  shadow_col(element_form->GetShadowColor()->GetRed(),
                                           element_form->GetShadowColor()->GetGreen(),
                                           element_form->GetShadowColor()->GetBlue(),
                                           element_form->GetShadowColor()->GetAlpha());

              GRP2DCOLOR_RGBA8  shadow_line_none(0, 0, 0, 0);

              canvas->SetFillColor(&shadow_col);
              canvas->SetLineColor(&shadow_line_none);
              canvas->SetLineWidth(1.0f);

              if(element_form->HasAnyPerCornerRadius())
                {
                  GRP2DPATH shpath;
                  AppendRoundRectPathPerCorner(shpath, sh_minx, sh_miny, sh_maxx, sh_maxy, rTL, rTR, rBR, rBL);
                  canvas->Path(shpath, true);
                }
               else if(element_form->GetRoundRect())
                {
                  canvas->RoundRect(sh_minx, sh_maxy, sh_maxx, sh_miny, element_form->GetRoundRect(), true);
                }
               else
                {
                  canvas->Rectangle(sh_minx, sh_maxy, sh_maxx, sh_miny, true);
                }
            }
        }

      // Step 6: CSS-natural fill. Prefer background_color when authored (via "bckgrdcolor" or the CSS-standard
      // alias "background-color"); fall back to the historical "color" property for retro-compat with layouts
      // that predate this unification. A one-time XTRACE warning per fallback element helps authors migrate.
      UI_COLOR* fillsrc = element_form->IsBackgroundColorSet() ? element_form->GetBackgroundColor()
                                                               : element_form->GetColor();

      if(!element_form->IsBackgroundColorSet() && !element_form->GetLegacyFillWarningEmitted())
        {
          XTRACE_PRINTCOLOR(XTRACE_COLOR_WARNING, __L("[UI Draw] Form [%s]: fill via legacy \"color\" property; migrate to \"bckgrdcolor\" (or CSS \"background-color\") for CSS-natural semantics"),
                            element_form->GetName() ? element_form->GetName()->Get() : __L("(unnamed)"));
          element_form->SetLegacyFillWarningEmitted(true);
        }

      GRP2DCOLOR_RGBA8  color(fillsrc->GetRed(),
                              fillsrc->GetGreen(),
                              fillsrc->GetBlue(),
                              fillsrc->GetAlpha());

      // Step 5: prefer base-level border-color if authored, otherwise fall back to the historical per-type
      // linecolor member so pre-step-5 layouts render unchanged.
      UI_COLOR* bcsrc = element_form->IsBorderColorSet() ? element_form->GetBorderColor()
                                                         : element_form->GetLineColor();

      GRP2DCOLOR_RGBA8  linecolor(bcsrc->GetRed(),
                                  bcsrc->GetGreen(),
                                  bcsrc->GetBlue(),
                                  bcsrc->GetAlpha());

      canvas->SetLineColor(&linecolor);
      canvas->SetFillColor(&color);

      // Step 4: honour element-configured border-width. -1 = default (historical 1.0f for containers), 0 = no
      // stroke drawn at all (fill-only), >0 = author-specified thickness. AGG's RoundRect / Rectangle draw a
      // stroke along with the fill even when linewidth is 0, so the "no stroke" case is handled by pushing a
      // fully transparent line color instead of relying on width 0.
      double bw = element_form->GetBorderWidth();

      if(bw == 0.0)
        {
          GRP2DCOLOR_RGBA8  linecolor_none(0, 0, 0, 0);
          canvas->SetLineColor(&linecolor_none);
          canvas->SetLineWidth(1.0f);
        }
       else
        {
          canvas->SetLineWidth((bw < 0.0) ? 1.0f : bw);
        }

      // Step 5: per-corner radius. When any corner carries an authored radius, build a custom path and draw
      // via canvas->Path so each corner arcs with its own radius. Otherwise keep the classic single-radius
      // canvas->RoundRect call, which is the fastest path and unchanged from step 4.
      //
      // NOTE: canvas->Path fills OR strokes depending on the boolean argument (this is the same convention
      // used by the ProgressBar gradient-fill path at line 405 of this file). RoundRect, in contrast, does
      // both in a single call. To match RoundRect's behaviour we issue two Path calls, and honour the same
      // "border-width == 0 -> suppress stroke by pushing a transparent line colour" contract established
      // above for the RoundRect branch.
      if(element_form->HasAnyPerCornerRadius())
        {
          double rTL = element_form->GetEffectiveBorderRadius(UI_ELEMENT_BORDER_CORNER_TL);
          double rTR = element_form->GetEffectiveBorderRadius(UI_ELEMENT_BORDER_CORNER_TR);
          double rBR = element_form->GetEffectiveBorderRadius(UI_ELEMENT_BORDER_CORNER_BR);
          double rBL = element_form->GetEffectiveBorderRadius(UI_ELEMENT_BORDER_CORNER_BL);

          GRP2DPATH path;
          AppendRoundRectPathPerCorner(path, vr_minx, vr_miny, vr_maxx, vr_maxy, rTL, rTR, rBR, rBL);

          canvas->Path(path, true);       // fill
          canvas->Path(path, false);      // stroke (transparent line colour when border-width == 0)
        }
       else if(element_form->GetRoundRect())
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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

  // Our OWN rebuild-area (created by PreDrawFunction() right below, sized from GetXPosition()/GetYPosition()/
  // GetBoundaryLine()) must cover only what OUR OWN painting further down in this function actually repaints --
  // element_progressrect's track/fill (and element_animation's frame, if present). For an AUTO-sized progress
  // bar whose caption sits OUTSIDE the rect (allocationtext DOWN/UP/LEFT/RIGHT), GetBoundaryLine() is
  // deliberately bigger than that: it spans rect+gap+caption too, so SIBLINGS lay out around the whole widget,
  // caption included (see the width/height AUTO block in CalculateBoundaryLine_ProgressBar()). Reusing that
  // same box to size OUR rebuild-area conflates "how big I am for layout" with "what my own redraw repaints" --
  // element_text (below) now owns its own independent rebuild-area, so if OUR area still also covers the
  // caption strip, restoring it later pastes OUR stale "before" caption snapshot back over whatever
  // element_text's own, more current area just correctly settled: the caption vanishes/overlaps after a real
  // value change (root cause of the long-standing progressbar caption defect). Fix: temporarily shrink our own
  // position/size to the exact union of element_progressrect + element_animation's CURRENT absolute bounds --
  // already correctly placed by CalculateBoundaryLine_ProgressBar() -- for this one PreDrawFunction() call,
  // then restore the real (caption-inclusive) box immediately after so every other use of GetBoundaryLine()/
  // GetXPosition()/GetYPosition() this tick (siblings' layout, hit-testing, etc.) is unaffected.
  double savedx      = element->GetXPosition();
  double savedy      = element->GetYPosition();
  double savedwidth  = element->GetBoundaryLine()->width;
  double savedheight = element->GetBoundaryLine()->height;
  bool   shrunkarea  = false;

  if(element_text && element_progressrect &&
     (element_progressbar->GetAllocationTextType() != UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_NONE) &&
     (element_progressbar->GetAllocationTextType() != UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_CENTER))
    {
      double left   = element_progressrect->GetXPosition();
      double right  = element_progressrect->GetXPosition() + element_progressrect->GetBoundaryLine()->width;
      double bottom = element_progressrect->GetYPosition();
      double top    = element_progressrect->GetYPosition() - element_progressrect->GetBoundaryLine()->height;

      if(element_animation)
        {
          left   = __MIN(left,   element_animation->GetXPosition());
          right  = __MAX(right,  element_animation->GetXPosition() + element_animation->GetBoundaryLine()->width);
          bottom = __MAX(bottom, element_animation->GetYPosition());
          top    = __MIN(top,    element_animation->GetYPosition() - element_animation->GetBoundaryLine()->height);
        }

      // Pull every edge in by a small, fixed safety margin (well under the >=6px gap every allocationtext case
      // in CalculateBoundaryLine_ProgressBar() leaves between the rect/animation and the caption -- see the
      // "+ 6" in each branch there). Without this, PreDrawFunction()'s own "edge" padding on this area and on
      // element_text's OWN independent area (added a few lines below) can make the two areas touch or overlap
      // by a pixel or two even though the real content does not -- and if they do, MarkOverlappingAreasDirty()
      // (called from RebuildAllAreas() before every restore) perpetually re-marks each dirty because of the
      // other, which defeats element_text's own one-shot capture/redraw/settle cycle exactly as badly as the
      // original bug this whole block exists to avoid. The margin only shrinks what OUR OWN rebuild-area
      // covers, never the actual drawing below, so it cannot clip anything on screen.
      const double SAFETY_MARGIN = 3.0;

      left   += SAFETY_MARGIN;
      right  -= SAFETY_MARGIN;
      top    += SAFETY_MARGIN;
      bottom -= SAFETY_MARGIN;

      element->SetXPosition(left);
      element->SetYPosition(bottom);
      element->GetBoundaryLine()->width  = __MAX(0.0, right - left);
      element->GetBoundaryLine()->height = __MAX(0.0, bottom - top);

      shrunkarea = true;
    }

  PreDrawFunction(element, canvas, clip_rect, x_position, y_position, 1);

  if(shrunkarea)
    {
      element->SetXPosition(savedx);
      element->SetYPosition(savedy);
      element->GetBoundaryLine()->width  = savedwidth;
      element->GetBoundaryLine()->height = savedheight;
    }

  if(element->MustReDraw())
    {
      // GHOST-FILL FIX (2026-09, follow-up to the two fixes above): CalculateBoundaryLine_ProgressBar() can
      // MOVE element_progressrect (and element_animation) to make room for the caption -- for DOWN it shifts
      // the rect's bottom edge inward (up), for LEFT its right edge inward, freeing a strip that used to be
      // part of the rect's own painted area (border AND fill). The very first time this widget is laid out,
      // nothing has painted anything there yet, so the shift is invisible; but from the first REAL value
      // change onward, this function re-runs with the rect already sitting at its shifted position, paints
      // only that CURRENT (already-shifted) rect -- and the strip the rect used to occupy, now vacated, is
      // never explicitly repainted by anything: the shrink-fix above intentionally narrows OUR OWN
      // rebuild-area to the rect/animation's CURRENT bounds (so it cannot fight with element_text's own
      // area), so the vacated strip falls outside both this tick's capture and every later one, and the OLD
      // rect/border/fill ink painted there the previous frame is left behind for good -- confirmed live as a
      // persistent smear of the fill colour (plus a stray border line) sitting behind/under the caption from
      // the second real value change onward, distinct from the text-ghosting defect fixed below.
      //
      // Fix: the same real-backdrop-cache technique as the (now superseded) caption-only fix below, but sized
      // to the WIDGET'S FULL natural box (rect + gap + caption -- savedx/savedy/savedwidth/savedheight above,
      // captured BEFORE the temporary shrink further up in this function) instead of just the caption's own
      // box. Captured once, the very first time this widget is ever drawn -- before rect, animation or
      // caption have painted anything, so it is guaranteed the true backdrop -- and painted back
      // (PutBitmapNoAlpha(), GEN's own restore primitive, never a synthetic fill) whenever the rect/animation
      // actually MOVE, so everything below repaints onto a clean canvas exactly at the moment the vacated
      // strip would otherwise start collecting stale ink.
      //
      // Restoring this backdrop is only safe to do on a tick where it is actually NEEDED, never unconditionally
      // on every tick this widget happens to be marked dirty (see the perpetual-redraw note below). Two,
      // INDEPENDENT conditions each mean a vacated/stale strip can exist right now and must be wiped before the
      // rect/animation/caption painting below repaints:
      //
      //   (a) element_progressrect/element_animation's OWN bounds changed since the last tick we looked (the
      //       original GHOST-FILL FIX case: a shift/resize vacates the strip the rect used to occupy).
      //
      //   (b) element_text is about to actually repaint new glyph content (element_text->MustReDraw()). This
      //       second condition was MISSING from the original GHOST-FILL FIX and is the root cause of a
      //       regression reported live by a user: CAPTION GHOSTING RETURNED from the second real value change
      //       onward, even though the rect never moves for a same-digit-count change (e.g. "15%" -> "16%" for
      //       allocationtext="down"/"up"/"left"/"right" -- only the fill AMOUNT changes, drawn from
      //       element_progressrect's own, unchanged, stored bounds; see widthpercent/heightpercent below).
      //       Condition (a) alone therefore NEVER re-fires after the very first capture for an ordinary value
      //       change, which was the (incorrect) assumption behind the "GHOSTING FIX" comment a few dozen lines
      //       below this one ("it is now handled once, up front, by the GHOST-FILL FIX's whole-widget
      //       restore") -- it is not, because that whole-widget restore was gated ONLY on (a). Proven live via
      //       direct GetBitmap() buffer reads (not a screen capture): three consecutive real value changes on
      //       "progressbar3" (15% -> 16% -> 17% -> 18%) render cleanly on the FIRST change, then show a solid
      //       black blob merging the old and new second digit from the SECOND change onward -- exactly the
      //       "letters overlap" defect this whole investigation started from, because UI_SKINCANVAS::Draw_Text()
      //       never clears its own background, it only ever blends new glyph ink on top of whatever is already
      //       there (see the GHOSTING FIX comment below), and nothing else was clearing the caption's zone for
      //       an ordinary value-only change. Fix: also restore on condition (b), reusing the SAME already-
      //       captured widget backdrop (its box already spans rect + gap + caption, see WIDGETEDGE below) --
      //       no new capture, no synthetic fill color, just GEN's own real PutBitmapNoAlpha() restore primitive
      //       applied more often.
      //
      // element_progressrect and (independently) element_text can both stay marked dirty for many consecutive
      // ticks after a single value change (confirmed live: dozens to hundreds of extra redraw ticks, with the
      // rect's own resolved bounds and the text's own resolved string identical to the previous tick's the
      // whole time) -- an unrelated, pre-existing perpetual-redraw condition on this widget that remains out of
      // scope for this fix. Restoring the backdrop on every one of those extra ticks is still safe with
      // condition (b) added: the restore is immediately followed, in every case, by this same function's own
      // rect/animation repaint and then element_text's own repaint further down, so an extra restore+repaint
      // cycle on a tick where nothing actually changed just reproduces the same pixels a second time -- visibly
      // idempotent, unlike a bare/unconditional restore with nothing scheduled to redraw after it (which is
      // what caused the caption to vanish outright in an earlier, naive attempt at this same fix).
      if(shrunkarea)
        {
          double rx = element_progressrect ? element_progressrect->GetXPosition() : 0.0;
          double ry = element_progressrect ? element_progressrect->GetYPosition() : 0.0;
          double rw = element_progressrect ? element_progressrect->GetBoundaryLine()->width  : 0.0;
          double rh = element_progressrect ? element_progressrect->GetBoundaryLine()->height : 0.0;
          double ax = element_animation ? element_animation->GetXPosition() : 0.0;
          double ay = element_animation ? element_animation->GetYPosition() : 0.0;
          double aw = element_animation ? element_animation->GetBoundaryLine()->width  : 0.0;
          double ah = element_animation ? element_animation->GetBoundaryLine()->height : 0.0;

          GRP2DREBUILDAREA* widgetbackdrop = ProgressBackdrop_Find(element);

          if(!widgetbackdrop)
            {
              // First time this widget is ever drawn: nothing has painted rect/animation/caption ink yet, so
              // this is the one guaranteed-pristine moment to capture the true backdrop.
              const double WIDGETEDGE = 5.0;

              double wb_left = savedx - WIDGETEDGE;
              double wb_top  = (savedy - savedheight) - WIDGETEDGE;
              double wb_w    = savedwidth  + (WIDGETEDGE * 2.0);
              double wb_h    = savedheight + (WIDGETEDGE * 2.0);

              if(wb_left < 0) wb_left = 0;
              if(wb_top  < 0) wb_top  = 0;

              ProgressBackdrop_Capture(element, wb_left, wb_top, wb_w, wb_h);
              ProgressBounds_Remember(element, rx, ry, rw, rh, ax, ay, aw, ah);
            }
           else if(ProgressBounds_HasChanged(element, rx, ry, rw, rh, ax, ay, aw, ah) ||
                   (element_text && element_text->MustReDraw()))
            {
              // Either the rect/animation moved (condition (a): a vacated strip of stale ink can appear), or
              // the caption is about to repaint new text (condition (b): its old glyph ink would otherwise
              // never be cleared -- see the long comment above). Either way, restore the true backdrop now,
              // then let the rect/animation/caption painting below repaint the whole box fresh.
              PutBitmapNoAlpha(widgetbackdrop->GetXPos(), widgetbackdrop->GetYPos(), widgetbackdrop->GetBitmap());
              ProgressBounds_Remember(element, rx, ry, rw, rh, ax, ay, aw, ah);
            }
          // else: neither the geometry nor the caption text is about to change -- leave the canvas alone here;
          // the normal per-element redraw just below (and element_text's own redraw further down) is already
          // correct and idempotent for this case, exactly as it was before this whole GHOST-FILL FIX existed.
        }

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

          // Box-shadow, read from the PROGRESSBAR element (element/element_progressbar) but drawn behind the
          // TRACK rect (element_progressrect) -- that sub-element is what UI_SkinCanvas_ProgressBar_DrawRect
          // below actually paints, so the shadow has to match its rect and its radius (including the roundcap
          // override just above) to line up. This is deliberately NOT a call to the generic
          // DrawElementBoxShadow helper: that helper anchors on the ELEMENT's own boundary and
          // supports per-corner radii, neither of which is what gets drawn here -- the visible track is a
          // different rect (a sub-element) with a single uniform radius (roundradius above), never per-corner.
          // Reuses UI_SkinCanvas_ProgressBar_DrawRect itself for the hard-shadow (no blur) fallback so the
          // shadow's capsule-clamping (short bars where the frame is narrower than 2*radius) is pixel-identical
          // to the track's own, instead of a second hand-written copy of that edge case.
          if(element->IsBoxShadowSet())
            {
              double sh_x    = element->GetShadowOffsetX();
              double sh_y    = element->GetShadowOffsetY();
              double sh_blur = element->GetShadowBlur();

              double rect_minx = element_progressrect->GetXPosition();
              double rect_maxx = element_progressrect->GetXPosition() + element_progressrect->GetBoundaryLine()->width;
              double rect_miny = element_progressrect->GetTopY();
              double rect_maxy = element_progressrect->GetYPosition();

              double sh_minx = rect_minx + sh_x;
              double sh_miny = rect_miny + sh_y;
              double sh_maxx = rect_maxx + sh_x;
              double sh_maxy = rect_maxy + sh_y;

              bool soft_ok = false;
              if(sh_blur > 0.0)
                {
                  soft_ok = UI_SkinCanvas_DrawSoftShadow(canvas, sh_minx, sh_miny, sh_maxx, sh_maxy,
                                                        roundradius, roundradius, roundradius, roundradius,
                                                        element->GetShadowColor(), (int)sh_blur);
                }

              if(!soft_ok)
                {
                  GRP2DCOLOR_RGBA8  shadow_col(element->GetShadowColor()->GetRed(),
                                               element->GetShadowColor()->GetGreen(),
                                               element->GetShadowColor()->GetBlue(),
                                               element->GetShadowColor()->GetAlpha());
                  GRP2DCOLOR_RGBA8  shadow_line_none(0, 0, 0, 0);

                  canvas->SetFillColor(&shadow_col);
                  canvas->SetLineColor(&shadow_line_none);
                  canvas->SetLineWidth(1.0f);

                  UI_SkinCanvas_ProgressBar_DrawRect(canvas, sh_minx, sh_maxy, sh_maxx, sh_miny, roundradius);
                }

              // Restore the track's own colours/width: both branches above may have changed canvas state
              // (the hard-shadow fallback always does; the soft path never touches it, but restoring
              // unconditionally is one cheap, always-correct line instead of two conditional ones).
              canvas->SetLineColor(&linecolor);
              canvas->SetFillColor(&bkgcolor);
              canvas->SetLineWidth(1.0);
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

          // ROOT-CAUSE FIX (2026-09, confirmed live pixel-by-pixel against progressbar3/progressbar0/
          // progressbar4 in UI_Options' example.xml -- see PreDrawFunction() above in this same file): for
          // every allocationtext mode except NONE/CENTER, element_text's own box sits OUTSIDE
          // element_progressrect's box (CalculateBoundaryLine_ProgressBar offsets one or the other to make
          // room). PreDrawFunction()'s ancestor-walk assumes a dirty ancestor's own redraw geometrically
          // covers every descendant, so it never lets element_text own its own rebuild-area while its father
          // (this progressbar) is dirty -- which normally IS every frame here. But this progressbar's own
          // redraw (the track/fill painting above) only ever covers element_progressrect's bounds, never
          // element_text's -- so that assumption is false for exactly this child, and element_text is left
          // with no rebuild-area of its own to correctly capture/restore its own backdrop. In practice the
          // caption's zone still LOOKS fine while element_progressrect's bounds happen to overlap it (the
          // father's restore+repaint cycle papers over it by accident), but the moment a real value change
          // shifts the rect away from the caption (see CalculateBoundaryLine_ProgressBar), nothing is left to
          // restore/repaint that zone at all, and the freshly-drawn glyph blends against whatever was there
          // one frame too early in the cycle (proven via direct framebuffer reads: a transient, wrong
          // snapshot that the rect's own repaint happened to hide every prior frame) -- exactly the "letters
          // overlap / caption vanishes after the value changes" defect reported on progress widgets.
          // Fix: momentarily tell PreDrawFunction() this element is NOT dirty while element_text draws, so
          // its own ancestor-walk stops at OUR still-real dirty state and lets element_text own a normal,
          // self-restoring rebuild-area of its own -- precisely as if it were a standalone dirty element, the
          // same protection every other on-screen element already gets. Restored immediately after (Draw()
          // clears element_text's OWN flag via its own PostDrawFunction(), never touches ours), so
          // element_animation right below and our own PostDrawFunction() call at the end of this function see
          // this element's real dirty state, unchanged.
          if(element_text)
            {
              // GHOSTING FIX (2026-09, follow-up to the ROOT-CAUSE FIX above; the actual ghosting protection now
              // lives in the GHOST-FILL FIX at the top of this function -- see that comment's condition (b),
              // added as a correction to a regression this same toggle used to (incompletely) guard against):
              // the toggle below gives element_text its own one-shot rebuild-area, which correctly protects its
              // FIRST-ever draw -- but that area is captured fresh and thrown away every time (see
              // RebuildAllAreas()'s orphan-discard path: an area whose element has already settled clean is
              // dropped WITHOUT ever being restored, because normally "nothing to undo" is exactly right). For
              // an ordinary element that is correct: some ancestor's own redraw already re-covers its zone from
              // scratch every time it repaints. A progress-bar caption has no such ancestor -- our own painting
              // above only ever touches element_progressrect/element_animation, never the caption strip -- and
              // Draw_Text() itself (shared by every text element) never erases anything either, it only ever
              // blends new glyph ink on top of whatever is already there (canvas->VectorFont_Print(), no clear
              // step). This toggle ALONE cannot prevent that: it only changes which rebuild-area element_text
              // gets, never what is already on the canvas the moment Draw_Text() paints onto it. The actual
              // clearing has to happen BEFORE this point in the tick, and is now condition (b) of the GHOST-FILL
              // FIX's restore above (element_text->MustReDraw()) -- confirmed live via GetBitmap() buffer reads
              // to fully resolve the "letters overlap" defect across multiple consecutive value changes, not
              // just the first one.

              // Momentarily hide OUR OWN dirty state from PreDrawFunction()'s ancestor-walk (see the ROOT-CAUSE
              // FIX comment above) so element_text's own PreDrawFunction() call, made from inside Draw() below,
              // is free to give it an independent rebuild-area instead of being suppressed as "a descendant of
              // an already-dirty ancestor". This does NOT affect OUR OWN rebuild-area: that one was already
              // created by the PreDrawFunction(element, ...) call at the top of this function, before this
              // toggle ever runs. Restored immediately after so element_animation below and our own
              // PostDrawFunction() call at the end of this function see our real dirty state, unchanged.
              element->SetMustReDraw(false);
              Draw(element_text);
              element->SetMustReDraw(true);
            }

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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
      // RADIAL CAPTION GHOSTING FIX (2026-09): this widget's track ring, value arc, round caps and centered
      // caption are ALL painted by alpha-blending onto whatever is already on the canvas -- the ring/arc never
      // fill the whole box (only the ring geometry itself), and the caption is anti-aliased glyph ink, painted
      // via VectorFont blending, never a solid fill. Nothing here ever clears the box first.
      //
      // That is normally safe because the generic per-tick rebuild-area system (see GetRebuildAreaByElement()
      // and UI_SKINCANVAS_REBUILDAREAS::RebuildAllAreas()) is expected to restore the true backdrop before each
      // real repaint. But a progressradial is typically dirty for exactly ONE frame per value change and idle
      // in between, so its rebuild area is ORPHAN-DISCARDED (deleted WITHOUT restoring -- see RebuildAllAreas()'s
      // orphan-discard comment: "the element's current on-screen content is already correct", true only for a
      // widget that clears its own background) the very next frame. The NEXT real value change then creates a
      // brand-new area that captures whatever is CURRENTLY on screen -- already showing the PREVIOUS caption's
      // ink -- and blends the new caption straight on top of it. Confirmed live via canvas->GetBitmap() reads
      // (alpha/RGB of the caption area) and a temporary trace of PreDrawFunction/RebuildAllAreas: progressbar1's
      // area is created fresh on every real tick and is ALWAYS orphan-discarded (never restored) afterwards.
      //
      // Fix: cache the TRUE backdrop (the real pixels behind this whole widget) once, the very first time it is
      // ever drawn -- before any track/arc/cap/caption ink exists -- and restore it (PutBitmapNoAlpha(), never a
      // synthetic fill colour) immediately before repainting on every later real redraw, exactly mirroring the
      // already-verified ProgressBackdrop (linear progress bar) and FormBackdrop (form/menu) fixes above. Unlike
      // the linear bar's EXTERNAL caption, the radial caption is centered INSIDE the widget's own box, so one
      // capture covering the widget's own rebuild-area geometry is enough for ring+arc+caption together.
      GRP2DREBUILDAREA* ownarea = GetRebuildAreaByElement(element);

      if(ownarea)
        {
          GRP2DREBUILDAREA* radialbackdrop = RadialBackdrop_Find(element);

          if(!radialbackdrop)
            {
              // First time this widget is ever drawn: nothing has painted track/arc/caption ink here yet, so
              // this is the one guaranteed-pristine moment to capture the true backdrop.
              RadialBackdrop_Capture(element, ownarea->GetXPos(), ownarea->GetYPos(),
                                      (double)ownarea->GetBitmap()->GetWidth(), (double)ownarea->GetBitmap()->GetHeight());
            }
           else
            {
              // Not the first draw: restore the true backdrop now, before repainting below.
              PutBitmapNoAlpha(radialbackdrop->GetXPos(), radialbackdrop->GetYPos(), radialbackdrop->GetBitmap());

              // Restoring just wiped the previous caption's glyph ink. Draw(element_text) below only actually
              // repaints when the caption's OWN MustReDraw() is set -- force it here so the caption always
              // repaints on top of the freshly restored backdrop instead of vanishing (same reasoning as the
              // force-children-dirty step in the Draw_Form() ALPHA-DARKENING FIX above).
              if(element_text) element_text->SetMustReDraw(true);
            }
        }

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
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  text : Text to use.
* @param[in]  width : Width value.
* @param[in]  height : Height value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  size : Size value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  element : Element to process.
* @param[in]  canvas : Canvas object to use.
* @param[in]  x_position : X position value.
* @param[in]  y_position : Y position value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @fn         static void UI_SkinCanvas_ContentExtent(UI_ELEMENT* element, bool vx, bool vy, double margin, double& minx, double& maxx, double& miny, double& maxy)
* @brief      Pre draw function
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  vx : Vx value.
* @param[in]  vy : Vy value.
* @param[in]  margin : Margin value.
* @param[in]  minx : Minx value.
* @param[in]  maxx : Maxx value.
* @param[in]  miny : Miny value.
* @param[in]  maxy : Maxy value.
* 
* --------------------------------------------------------------------------------------------------------------------*/

// Accumulate the screen bounding box of an element's descendants, but only on the axes flagged (vx/vy). Used to size a
// scrollable container's rebuild area so it covers content that overflows on a non-clipped (VISIBLE) axis. Descendants
// no longer own rebuild areas, so without this their overflowing pixels (e.g. an animated icon sticking out of the bar)
// would never be erased and would pile up. Positions are absolute (scroll is applied at draw time, not stored), and a
// VISIBLE axis is never scrolled, so the stored positions are the right ones to measure on that axis.
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         static void UI_SkinCanvas_ContentExtent(UI_ELEMENT* element, bool vx, bool vy, double margin, double& minx, double& maxx, double& miny, double& maxy)
* @brief      Skin canvas content extent
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Pointer to element.
* @param[in]  vx : Vx value.
* @param[in]  vy : Vy value.
* @param[in]  margin : Margin value.
* @param[in]  minx : Minx value.
* @param[in]  maxx : Maxx value.
* @param[in]  miny : Miny value.
* @param[in]  maxy : Maxy value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_SKINCANVAS::PreDrawFunction(UI_ELEMENT* element, GRP2DCANVAS* canvas, XRECT& clip_rect, double& x_position, double& y_position, XDWORD edge)
* @brief      Pre Draw Function
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  canvas : Canvas object to use.
* @param[in]  clip_rect : Clip rect value.
* @param[in]  x_position : X position value.
* @param[in]  y_position : Y position value.
* @param[in]  edge : Edge value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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

      #ifdef XTRACE_ACTIVE
      /*
      if(element->GetName() && (element->GetName()->Find(__L("chrome"), true) != XSTRING_NOTFOUND || !element->GetName()->Compare(__L("caption"), true)))
        {
          XTRACE_PRINTCOLOR(XTRACE_COLOR_INFO, __L("[DEBUGCAPTION] PreDrawFunction: element=[%s] zlevel=%d createarea=%d hadexistingarea=%d"),
                                                                                element->GetName()->Get(),
                                                                                (int)element->GetZLevel(),
                                                                                createarea?1:0,
                                                                                GetRebuildAreaByElement(element)?1:0);
        }
      */
      #endif
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

      // Step 7: expand the rebuild-area to include the box-shadow footprint, so save/restore cycles at
      // repaint time do not leave ghost pixels where the shadow was drawn OUTSIDE the element's own rect.
      // Positive offset extends the corresponding far side; negative extends the near side. Blur, when
      // rendered in a follow-up rebanada, will add further symmetric expansion on all sides -- we already
      // account for it below so the rebuild-area is right on the day blur ships.
      double  shadow_L = 0.0, shadow_R = 0.0, shadow_T = 0.0, shadow_B = 0.0;
      if(element->IsBoxShadowSet())
        {
          double sx = element->GetShadowOffsetX();
          double sy = element->GetShadowOffsetY();
          double sb = element->GetShadowBlur();

          if(sx >= 0.0) shadow_R = sx; else shadow_L = -sx;
          if(sy >= 0.0) shadow_B = sy; else shadow_T = -sy;

          if(sb > 0.0)
            {
              // Must match the off-screen bitmap padding used by UI_SkinCanvas_DrawSoftShadow exactly: the
              // blurred bitmap is composited from (shape - pad) to (shape + pad), so an expansion of only
              // `blur` would leave the outer ring of the fade outside the rebuild-area. Those pixels would
              // never be restored, so each repaint would blend a new shadow on top of the previous one and
              // the halo would darken frame after frame.
              double pad = (double)UI_SKINCANVAS_SHADOW_BLURPADDING(sb);

              shadow_L += pad; shadow_R += pad;
              shadow_T += pad; shadow_B += pad;
            }
        }

      CreateRebuildArea(area_left - edge - shadow_L,
                        area_top  - edge - shadow_T,
                        (area_right  - area_left) + (edge * 2) + shadow_L + shadow_R,
                        (area_bottom - area_top ) + (edge * 2) + shadow_T + shadow_B, element);
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
* @param[in]  element : Element to process.
* @param[in]  canvas : Canvas object to use.
* @param[in]  clip_rect : Clip rect value.
* @param[in]  x_position : X position value.
* @param[in]  y_position : Y position value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @brief      Returns the scroll viewport SIZE for an element. For a Form (or Menu) with a defined visiblerect this is the visible window; otherwise (and for non-Form scrolleables) it is the element boundary line. The box POSITION is left to each call site (it already tracks scroll), so only the size needs to change.
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  width : Width value.
* @param[in]  height : Height value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  scrolleable : Scrolleable pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  scrolleable : Scrolleable pointer to use.
* @param[in]  canvas : Canvas object to use.
* @param[in]  x_position : X position value.
* @param[in]  y_position : Y position value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  element_textbox : Element textbox pointer to use.
* @param[in]  canvas : Canvas object to use.
* @param[in]  x_position : X position value.
* @param[in]  y_position : Y position value.
* @param[in]  nline : Nline value.
* @param[in]  characterstr : Characterstr value.
* @param[in]  index_char : Index char value.
* @param[in]  parts : Parts pointer to use.
* 
* @return     double : Requested value.
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

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         static void UI_SkinCanvas_TextObstacle(double x, double ltop, double lbot, const double* ol, const double* orr, const double* ot, const double* ob, int n, double& jumpx, double& aheadleft, double& aheadright)
* @brief      UI Skin Canvas Text Obstacle
* @ingroup    USERINTERFACE
* 
* @param[in]  x : X coordinate.
* @param[in]  ltop : Ltop value.
* @param[in]  lbot : Lbot value.
* @param[in]  ol : Ol value.
* @param[in]  orr : Orr value.
* @param[in]  ot : Ot value.
* @param[in]  ob : Ob value.
* @param[in]  n : N value.
* @param[in]  jumpx : Jumpx value.
* @param[in]  aheadleft : Aheadleft value.
* @param[in]  aheadright : Aheadright value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
// Inline-image obstacle resolver for the text box. Active images are rectangles the running text must flow around.
// For a line whose vertical band [ltop,lbot] overlaps an image, this advances the cursor past any image it currently
// sits inside (jumpx), and reports the nearest image edge still ahead on that line (aheadleft/aheadright) so a word is
// never laid down on top of the picture. With no active images the outputs leave the cursor untouched (aheadleft < 0),
// so the layout is byte-for-byte the original behaviour.
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         static void UI_SkinCanvas_TextObstacle(double x, double ltop, double lbot, const double* ol, const double* orr, const double* ot, const double* ob, int n, double& jumpx, double& aheadleft, double& aheadright)
* @brief      Skin canvas text obstacle
* @ingroup    USERINTERFACE
* 
* @param[in]  x : X value.
* @param[in]  ltop : Ltop value.
* @param[in]  lbot : Lbot value.
* @param[in]  ol : Pointer to ol.
* @param[in]  orr : Pointer to orr.
* @param[in]  ot : Pointer to ot.
* @param[in]  ob : Pointer to ob.
* @param[in]  n : N value.
* @param[in]  jumpx : Jumpx value.
* @param[in]  aheadleft : Aheadleft value.
* @param[in]  aheadright : Aheadright value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  element_textbox : Element textbox pointer to use.
* @param[in]  canvas : Canvas object to use.
* @param[in]  x_position : X position value.
* @param[in]  y_position : Y position value.
* @param[in]  parts : Parts pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* @param[in]  element : Element to process.
* @param[in]  x_position : X position value.
* @param[in]  y_position : Y position value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
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
* --------------------------------------------------------------------------------------------------------------------*/
void UI_SKINCANVAS::Clean()
{
  fontsize        = 0;
  screen          = NULL;
  viewportindex   = 0;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRP2DREBUILDAREA* UI_SKINCANVAS::ProgressBackdrop_Find(UI_ELEMENT* element)
* @brief      Look up the persistent "true backdrop" snapshot previously captured for a progress-bar widget's
*             FULL natural box (rect + gap + caption together).
* @note       See the GHOST-FILL FIX comment in Draw_ProgressBar() and the progressbackdrops member comment in
*             UI_SkinCanvas.h. Linear scan is deliberate: this cache only ever holds one entry per progress-bar
*             widget in a layout (a handful at most), so there is nothing to gain from a hash lookup here.
* @ingroup    USERINTERFACE
*
* @param[in]  element : Progress-bar element to look up (used only as an opaque identity key, never dereferenced).
*
* @return     GRP2DREBUILDAREA* : The cached entry (xpos/ypos/bitmap already positioned for PutBitmapNoAlpha);
*                                  NULL if this widget has never been captured yet.
*
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DREBUILDAREA* UI_SKINCANVAS::ProgressBackdrop_Find(UI_ELEMENT* element)
{
  if(!element) return NULL;

  for(XDWORD c=0; c<progressbackdrops.GetSize(); c++)
    {
      GRP2DREBUILDAREA* entry = progressbackdrops.Get(c);
      if(entry && (entry->GetExtraData() == (void*)element)) return entry;
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_SKINCANVAS::ProgressBackdrop_Capture(UI_ELEMENT* element, double x, double y, double width, double height)
* @brief      Captures the CURRENT on-screen pixels under a progress-bar widget's full natural box and keeps them
*             indefinitely as that widget's "true backdrop" reference.
* @note       Only ever correct to call the FIRST time a given widget is about to be drawn (see the caller in
*             Draw_ProgressBar(), gated on ProgressBackdrop_Find() returning NULL): at that point nothing has
*             painted a rect, animation or caption glyph in this zone yet, so whatever is on screen right now
*             genuinely IS the backdrop -- the same real pixels (seamless background pattern, gradient,
*             translucent panel, whatever this widget actually sits on) that must show through on every later
*             redraw. Reuses the inherited GetBitmap() -- the exact same capture primitive
*             UI_SKINCANVAS_REBUILDAREAS::CreateRebuildArea() itself uses -- so this is GEN's own real capture
*             machinery, not a new one.
* @ingroup    USERINTERFACE
*
* @param[in]  element : Progress-bar element this capture belongs to (stored only as an opaque identity key).
* @param[in]  x       : Left edge of the region to capture, in canvas coordinates.
* @param[in]  y       : Top edge of the region to capture, in canvas coordinates.
* @param[in]  width   : Width of the region to capture.
* @param[in]  height  : Height of the region to capture.
*
* @return     bool : true if the capture was stored; false if the bitmap grab or allocation failed (caller simply
*                     has no cached backdrop this tick and behaves as before this fix -- never worse).
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::ProgressBackdrop_Capture(UI_ELEMENT* element, double x, double y, double width, double height)
{
  if(!element) return false;

  GRPBITMAP* bitmap = GetBitmap(x, y, width, height);
  if(!bitmap) return false;

  GRP2DREBUILDAREA* entry = GEN_NEW GRP2DREBUILDAREA();
  if(!entry)
    {
      GEN_DELETE bitmap;
      return false;
    }

  entry->SetXPos(x);
  entry->SetYPos(y);
  entry->SetBitmap(bitmap);
  entry->SetExtraData((void*)element);

  return progressbackdrops.Add(entry);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_PROGRESSBAR_LASTBOUNDS* UI_SKINCANVAS::ProgressBounds_Find(UI_ELEMENT* element)
* @brief      Look up the last-known element_progressrect/element_animation bounds recorded for a progress bar.
* @note       See the ProgressBounds_Remember/HasChanged declarations and the GHOST-FILL FIX comment in
*             Draw_ProgressBar(). Linear scan is deliberate: same small-cardinality reasoning as
*             ProgressBackdrop_Find() above (one entry per progress-bar widget in a layout).
* @ingroup    USERINTERFACE
*
* @param[in]  element : Progress-bar element to look up (used only as an opaque identity key, never dereferenced).
*
* @return     UI_PROGRESSBAR_LASTBOUNDS* : The cached entry; NULL if this widget's bounds have never been
*                                          recorded yet.
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_PROGRESSBAR_LASTBOUNDS* UI_SKINCANVAS::ProgressBounds_Find(UI_ELEMENT* element)
{
  if(!element) return NULL;

  for(XDWORD c=0; c<progressbarlastbounds.GetSize(); c++)
    {
      UI_PROGRESSBAR_LASTBOUNDS* entry = progressbarlastbounds.Get(c);
      if(entry && (entry->element == element)) return entry;
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_SKINCANVAS::ProgressBounds_Remember(UI_ELEMENT* element, double rectx, double recty, double rectwidth, double rectheight, double animx, double animy, double animwidth, double animheight)
* @brief      Records (creating the entry the first time) the CURRENT element_progressrect/element_animation
*             bounds for a progress bar, so the next tick can tell whether they moved.
* @ingroup    USERINTERFACE
*
* @param[in]  element     : Progress-bar element these bounds belong to (stored only as an opaque identity key).
* @param[in]  rectx       : element_progressrect's current X position (0 if this widget has no rect sub-element).
* @param[in]  recty       : element_progressrect's current Y position.
* @param[in]  rectwidth   : element_progressrect's current width.
* @param[in]  rectheight  : element_progressrect's current height.
* @param[in]  animx       : element_animation's current X position (0 if this widget has no animation sub-element).
* @param[in]  animy       : element_animation's current Y position.
* @param[in]  animwidth   : element_animation's current width.
* @param[in]  animheight  : element_animation's current height.
*
* @return     void.
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_SKINCANVAS::ProgressBounds_Remember(UI_ELEMENT* element, double rectx, double recty, double rectwidth, double rectheight, double animx, double animy, double animwidth, double animheight)
{
  if(!element) return;

  UI_PROGRESSBAR_LASTBOUNDS* entry = ProgressBounds_Find(element);

  if(!entry)
    {
      entry = GEN_NEW UI_PROGRESSBAR_LASTBOUNDS();
      if(!entry) return;

      entry->element = element;

      progressbarlastbounds.Add(entry);
    }

  entry->rectx      = rectx;
  entry->recty      = recty;
  entry->rectwidth  = rectwidth;
  entry->rectheight = rectheight;
  entry->animx      = animx;
  entry->animy      = animy;
  entry->animwidth  = animwidth;
  entry->animheight = animheight;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_SKINCANVAS::ProgressBounds_HasChanged(UI_ELEMENT* element, double rectx, double recty, double rectwidth, double rectheight, double animx, double animy, double animwidth, double animheight)
* @brief      Compares the CURRENT element_progressrect/element_animation bounds against the last-recorded ones.
* @note       An element with no recorded entry yet counts as "changed" (the caller in Draw_ProgressBar() only
*             calls this after already handling the "never captured" case via ProgressBackdrop_Find() returning
*             NULL, so in practice this is always called with an existing entry -- treating a missing one as
*             changed is simply the safe default were that ever not true). Exact double comparison is
*             deliberate and safe here: both the current values and the remembered ones come from the same
*             deterministic, absolute (never incremental -- see CalculateBoundaryLine_ProgressBar()) layout
*             math, so two ticks with nothing genuinely different between them always compare bit-for-bit equal.
* @ingroup    USERINTERFACE
*
* @param[in]  element     : Progress-bar element to check.
* @param[in]  rectx       : element_progressrect's CURRENT X position to compare.
* @param[in]  recty       : element_progressrect's CURRENT Y position to compare.
* @param[in]  rectwidth   : element_progressrect's CURRENT width to compare.
* @param[in]  rectheight  : element_progressrect's CURRENT height to compare.
* @param[in]  animx       : element_animation's CURRENT X position to compare.
* @param[in]  animy       : element_animation's CURRENT Y position to compare.
* @param[in]  animwidth   : element_animation's CURRENT width to compare.
* @param[in]  animheight  : element_animation's CURRENT height to compare.
*
* @return     bool : true if these bounds differ from the last-recorded ones (or none were recorded yet);
*                    false if they are identical, meaning nothing moved since the last tick we looked.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::ProgressBounds_HasChanged(UI_ELEMENT* element, double rectx, double recty, double rectwidth, double rectheight, double animx, double animy, double animwidth, double animheight)
{
  UI_PROGRESSBAR_LASTBOUNDS* entry = ProgressBounds_Find(element);

  if(!entry) return true;

  return (entry->rectx      != rectx)      ||
         (entry->recty      != recty)      ||
         (entry->rectwidth  != rectwidth)  ||
         (entry->rectheight != rectheight) ||
         (entry->animx      != animx)      ||
         (entry->animy      != animy)      ||
         (entry->animwidth  != animwidth)  ||
         (entry->animheight != animheight);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRP2DREBUILDAREA* UI_SKINCANVAS::FormBackdrop_Find(UI_ELEMENT* element)
* @brief      Look up the persistent "true backdrop" snapshot previously captured for a "form"/"menu" element's
*             own box (see the ALPHA-DARKENING FIX comment in Draw_Form() and the formbackdrops member comment
*             in UI_SkinCanvas.h).
* @note       Linear scan is deliberate: same small-cardinality reasoning as ProgressBackdrop_Find() above -- a
*             typical layout has only a handful of translucent forms/menus.
* @ingroup    USERINTERFACE
*
* @param[in]  element : Form/menu element to look up (used only as an opaque identity key, never dereferenced).
*
* @return     GRP2DREBUILDAREA* : The cached entry (xpos/ypos/bitmap already positioned for PutBitmapNoAlpha);
*                                  NULL if this element has never been captured yet.
*
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DREBUILDAREA* UI_SKINCANVAS::FormBackdrop_Find(UI_ELEMENT* element)
{
  if(!element) return NULL;

  for(XDWORD c=0; c<formbackdrops.GetSize(); c++)
    {
      GRP2DREBUILDAREA* entry = formbackdrops.Get(c);
      if(entry && (entry->GetExtraData() == (void*)element)) return entry;
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_SKINCANVAS::FormBackdrop_Capture(UI_ELEMENT* element, double x, double y, double width, double height)
* @brief      Captures the CURRENT on-screen pixels under a form/menu element's own box and keeps them indefinitely
*             as that element's "true backdrop" reference.
* @note       Only ever correct to call the FIRST time a given element is about to be drawn (see the caller in
*             Draw_Form(), gated on FormBackdrop_Find() returning NULL): at that point nothing has painted this
*             element's own shadow, fill, border or children in this zone yet, so whatever is on screen right now
*             genuinely IS the backdrop (the real background image/pattern, or whatever sits behind this element).
*             Reuses the inherited GetBitmap() -- the exact same capture primitive
*             UI_SKINCANVAS_REBUILDAREAS::CreateRebuildArea() and ProgressBackdrop_Capture() themselves use -- so
*             this is GEN's own real capture machinery, not a new one, and never a synthetic/flat fill colour.
* @ingroup    USERINTERFACE
*
* @param[in]  element : Form/menu element this capture belongs to (stored only as an opaque identity key).
* @param[in]  x       : Left edge of the region to capture, in canvas coordinates.
* @param[in]  y       : Top edge of the region to capture, in canvas coordinates.
* @param[in]  width   : Width of the region to capture.
* @param[in]  height  : Height of the region to capture.
*
* @return     bool : true if the capture was stored; false if the bitmap grab or allocation failed (caller simply
*                     has no cached backdrop this tick and behaves as before this fix -- never worse).
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::FormBackdrop_Capture(UI_ELEMENT* element, double x, double y, double width, double height)
{
  if(!element) return false;

  GRPBITMAP* bitmap = GetBitmap(x, y, width, height);
  if(!bitmap) return false;

  GRP2DREBUILDAREA* entry = GEN_NEW GRP2DREBUILDAREA();
  if(!entry)
    {
      GEN_DELETE bitmap;
      return false;
    }

  entry->SetXPos(x);
  entry->SetYPos(y);
  entry->SetBitmap(bitmap);
  entry->SetExtraData((void*)element);

  return formbackdrops.Add(entry);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRP2DREBUILDAREA* UI_SKINCANVAS::RadialBackdrop_Find(UI_ELEMENT* element)
* @brief      Look up the persistent "true backdrop" snapshot previously captured for a progressradial widget's
*             own box (see the RADIAL CAPTION GHOSTING FIX comment in Draw_ProgressRadial() and the
*             radialbackdrops member comment in UI_SkinCanvas.h).
* @note       Linear scan is deliberate: same small-cardinality reasoning as ProgressBackdrop_Find()/
*             FormBackdrop_Find() above -- a typical layout has only a handful of radial progress widgets.
* @ingroup    USERINTERFACE
*
* @param[in]  element : Progressradial element to look up (used only as an opaque identity key, never dereferenced).
*
* @return     GRP2DREBUILDAREA* : The cached entry (xpos/ypos/bitmap already positioned for PutBitmapNoAlpha);
*                                  NULL if this element has never been captured yet.
*
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DREBUILDAREA* UI_SKINCANVAS::RadialBackdrop_Find(UI_ELEMENT* element)
{
  if(!element) return NULL;

  for(XDWORD c=0; c<radialbackdrops.GetSize(); c++)
    {
      GRP2DREBUILDAREA* entry = radialbackdrops.Get(c);
      if(entry && (entry->GetExtraData() == (void*)element)) return entry;
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_SKINCANVAS::RadialBackdrop_Capture(UI_ELEMENT* element, double x, double y, double width, double height)
* @brief      Captures the CURRENT on-screen pixels under a progressradial element's own box and keeps them
*             indefinitely as that element's "true backdrop" reference.
* @note       Only ever correct to call the FIRST time a given element is about to be drawn (see the caller in
*             Draw_ProgressRadial(), gated on RadialBackdrop_Find() returning NULL): at that point nothing has
*             painted this widget's own track/arc/caps/caption in this zone yet, so whatever is on screen right
*             now genuinely IS the backdrop. Reuses the inherited GetBitmap() -- the exact same capture primitive
*             UI_SKINCANVAS_REBUILDAREAS::CreateRebuildArea(), ProgressBackdrop_Capture() and FormBackdrop_Capture()
*             themselves use -- so this is GEN's own real capture machinery, not a new one, and never a
*             synthetic/flat fill colour.
* @ingroup    USERINTERFACE
*
* @param[in]  element : Progressradial element this capture belongs to (stored only as an opaque identity key).
* @param[in]  x       : Left edge of the region to capture, in canvas coordinates.
* @param[in]  y       : Top edge of the region to capture, in canvas coordinates.
* @param[in]  width   : Width of the region to capture.
* @param[in]  height  : Height of the region to capture.
*
* @return     bool : true if the capture was stored; false if the bitmap grab or allocation failed (caller simply
*                     has no cached backdrop this tick and behaves as before this fix -- never worse).
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_SKINCANVAS::RadialBackdrop_Capture(UI_ELEMENT* element, double x, double y, double width, double height)
{
  if(!element) return false;

  GRPBITMAP* bitmap = GetBitmap(x, y, width, height);
  if(!bitmap) return false;

  GRP2DREBUILDAREA* entry = GEN_NEW GRP2DREBUILDAREA();
  if(!entry)
    {
      GEN_DELETE bitmap;
      return false;
    }

  entry->SetXPos(x);
  entry->SetYPos(y);
  entry->SetBitmap(bitmap);
  entry->SetExtraData((void*)element);

  return radialbackdrops.Add(entry);
}


