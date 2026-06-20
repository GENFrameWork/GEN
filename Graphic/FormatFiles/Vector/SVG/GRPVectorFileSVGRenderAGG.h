/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPVectorFileSVGRenderAGG.h
* 
* @class      GRPVECTORFILESVGRENDERAGG
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

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XString.h"

#include "GRPVectorFileSVGStyle.h"
#include "GRPVectorFileSVGTransform.h"
#include "GRPVectorFileSVGObjText.h"



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/



/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class GRPCANVAS;
class GRPBITMAP;
class GRP2DPATH;
class GRPVECTORFILESVG;
class GRPVECTORFILESVGOBJ;



/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/


class GRPVECTORFILESVGRENDERAGG
{
  public:
                                    GRPVECTORFILESVGRENDERAGG   ();
    virtual                        ~GRPVECTORFILESVGRENDERAGG   ();

    bool                            Render                     (GRPVECTORFILESVG* svg, GRPCANVAS* canvas);
    bool                            Render                     (GRPVECTORFILESVG* svg, GRPCANVAS* canvas, double targetx, double targety, double targetwidth, double targetheight);

    bool                            RenderCached               (GRPVECTORFILESVG* svg, GRPCANVAS* canvas, double targetx, double targety, double targetwidth, double targetheight);
    void                            InvalidateCache            ();

  private:

    bool                            CalculateViewBoxTransform  (GRPVECTORFILESVG* svg, double targetx, double targety, double targetwidth, double targetheight, GRPVECTORFILESVGTRANSFORM& transform);

    bool                            RenderNode                 (GRPVECTORFILESVGOBJ* obj, GRPVECTORFILESVGTRANSFORM& parenttransform, GRPVECTORFILESVGSTYLE& parentstyle, GRPCANVAS* canvas);
    bool                            RenderShape                (GRPVECTORFILESVGOBJ* obj, GRPVECTORFILESVGTRANSFORM& transform, GRPVECTORFILESVGSTYLE& style, GRPCANVAS* canvas);
    bool                            RenderText                 (GRPVECTORFILESVGOBJ* obj, GRPVECTORFILESVGTRANSFORM& transform, GRPVECTORFILESVGSTYLE& style, GRPCANVAS* canvas);
    double                          DrawTextRun                (XSTRING* text, GRPVECTORFILESVGSTYLE& style, double sizeuser, double penx, double peny, GRPVECTORFILESVGTEXTANCHOR anchor, GRPVECTORFILESVGTRANSFORM& transform, double scale, GRPCANVAS* canvas);

    bool                            RenderGradient             (GRP2DPATH& devicepath, XSTRING* paintid, double globalalpha, bool isstroke, double linewidth, bool evenodd, GRPVECTORFILESVGTRANSFORM& transform, double bbx, double bby, double bbw, double bbh, GRPCANVAS* canvas);
    void                            ComputePathBBox            (GRP2DPATH& path, double& minx, double& miny, double& width, double& height);

    void                            TransformPath              (GRP2DPATH& path, GRPVECTORFILESVGTRANSFORM& transform);
    double                          GetScaleFactor             (GRPVECTORFILESVGTRANSFORM& transform);

    bool                            BuildCacheBitmap           (GRPVECTORFILESVG* svg, GRPCANVAS* referencecanvas, double width, double height);

    void                            Clean                      ();

    GRPBITMAP*                      cachebitmap;
    bool                            cachevalid;
    double                          cachex;
    double                          cachey;
    double                          cachewidth;
    double                          cacheheight;

    GRPVECTORFILESVG*               contextsvg;                                  // current SVG being rendered (to resolve <use> references)
    int                             usedepth;                                    // recursion guard for <use>
};




