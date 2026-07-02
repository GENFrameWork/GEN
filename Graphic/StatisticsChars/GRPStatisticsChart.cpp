/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPStatisticsChart.cpp
* 
* @class      GRPSTATISTICSCHART
* @brief      Graphic Chart : abstract chart base implementation
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

#include <math.h>

#include "GRPStatisticsChart.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPSTATISTICSCHART::GRPSTATISTICSCHART()
* @brief      Constructor of class
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPSTATISTICSCHART::GRPSTATISTICSCHART()
{

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPSTATISTICSCHART::~GRPSTATISTICSCHART()
* @brief      Destructor of class
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPSTATISTICSCHART::~GRPSTATISTICSCHART()
{

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPSTATISTICSCHARTDATA* GRPSTATISTICSCHART::GetData()
* @brief      Get data
* @ingroup    GRAPHIC
* 
* @return     GRPSTATISTICSCHARTDATA* : chart data model
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPSTATISTICSCHARTDATA* GRPSTATISTICSCHART::GetData()
{
  return &data;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPSTATISTICSCHARTCONFIG* GRPSTATISTICSCHART::GetConfig()
* @brief      Get config
* @ingroup    GRAPHIC
* 
* @return     GRPSTATISTICSCHARTCONFIG* : chart visual config
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPSTATISTICSCHARTCONFIG* GRPSTATISTICSCHART::GetConfig()
{
  return &config;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILERESULT GRPSTATISTICSCHART::Generate(GRPSTATISTICSCHARTBUILDER& builder, double width, double height)
* @brief      Generate : lay out the document (background, title, plot area) and draw the chart into the builder
* @ingroup    GRAPHIC
* 
* @param[in]  builder : output builder (format backend)
* @param[in]  width   : output width
* @param[in]  height  : output height
* 
* @return     GRPVECTORFILERESULT : result
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILERESULT GRPSTATISTICSCHART::Generate(GRPSTATISTICSCHARTBUILDER& builder, double width, double height)
{
  if((width <= 0.0) || (height <= 0.0))
    {
      return GRPVECTORFILERESULT_ERRORUNKNOWN;
    }

  builder.BeginDocument(width, height);

  DrawBackground(builder, width, height);

  double margin    = config.GetMargin();
  double axislabel = config.GetShowAxisLabels() ? config.GetAxisFontSize() : 0.0;

  double left   = margin + (axislabel * 3.0);                                   // room for the Y value labels
  double right  = margin;
  double top    = margin;
  double bottom = margin + (axislabel * 2.0);                                   // room for the X category labels

  if(!config.GetTitle().IsEmpty())
    {
      DrawTitle(builder, width, top + config.GetTitleFontSize());

      top += config.GetTitleFontSize() + margin;
    }

  //  reserve room for the legend (before the plot rectangle is computed)

  GRPSTATISTICSCHARTLEGENDPOSITION legendpos   = config.GetLegendPosition();
  bool                             haslegend   = (config.GetShowLegend() && (data.GetNSeries() > 0));
  bool                             legendhoriz = ((legendpos == GRPSTATISTICSCHARTLEGENDPOSITION_TOP) || (legendpos == GRPSTATISTICSCHARTLEGENDPOSITION_BOTTOM));
  double                           legendthk   = haslegend ? MeasureLegendThickness(legendhoriz) : 0.0;

  double legendx = 0.0;
  double legendy = 0.0;

  if(haslegend)
    {
      if(legendpos == GRPSTATISTICSCHARTLEGENDPOSITION_TOP)    { legendy = top;   top  += legendthk; }
      if(legendpos == GRPSTATISTICSCHARTLEGENDPOSITION_LEFT)   { left += legendthk;                 }
      if(legendpos == GRPSTATISTICSCHARTLEGENDPOSITION_BOTTOM) { bottom += legendthk;                }
      if(legendpos == GRPSTATISTICSCHARTLEGENDPOSITION_RIGHT)  { right  += legendthk;                }
    }

  double plotx = left;
  double ploty = top;
  double plotw = width  - left - right;
  double ploth = height - top  - bottom;

  bool ok = false;

  if((plotw > 0.0) && (ploth > 0.0))
    {
      ok = DrawPlot(builder, plotx, ploty, plotw, ploth);

      if(haslegend)
        {
          double legendw = legendhoriz ? plotw     : legendthk;
          double legendh = legendhoriz ? legendthk : ploth;

          if(legendpos == GRPSTATISTICSCHARTLEGENDPOSITION_TOP)    {                                                        legendx = plotx;                    }
          if(legendpos == GRPSTATISTICSCHARTLEGENDPOSITION_LEFT)   { legendx = margin;                                      legendy = ploty;                    }
          if(legendpos == GRPSTATISTICSCHARTLEGENDPOSITION_BOTTOM) { legendx = plotx;                                       legendy = height - margin - legendthk; }
          if(legendpos == GRPSTATISTICSCHARTLEGENDPOSITION_RIGHT)  { legendx = width - margin - legendthk;                  legendy = ploty;                    }

          DrawLegend(builder, legendx, legendy, legendw, legendh, legendhoriz);
        }
    }

  builder.EndDocument();

  return ok ? GRPVECTORFILERESULT_OK : GRPVECTORFILERESULT_ERRORUNKNOWN;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSTATISTICSCHART::DrawBackground(GRPSTATISTICSCHARTBUILDER& builder, double width, double height)
* @brief      Draw background
* @ingroup    GRAPHIC
* 
* @param[in]  builder : output builder
* @param[in]  width   : output width
* @param[in]  height  : output height
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSTATISTICSCHART::DrawBackground(GRPSTATISTICSCHARTBUILDER& builder, double width, double height)
{
  if(!config.HasBackground()) return;

  GRPSTATISTICSCHARTSTYLE style;

  style.SetFill(config.GetBackgroundColor());

  builder.DrawRect(0.0, 0.0, width, height, style);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSTATISTICSCHART::DrawTitle(GRPSTATISTICSCHARTBUILDER& builder, double width, double baseline)
* @brief      Draw title (centered)
* @ingroup    GRAPHIC
* 
* @param[in]  builder  : output builder
* @param[in]  width    : output width
* @param[in]  baseline : text baseline Y
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSTATISTICSCHART::DrawTitle(GRPSTATISTICSCHARTBUILDER& builder, double width, double baseline)
{
  GRPSTATISTICSCHARTTEXTSTYLE style;

  style.color  = config.GetTextColor();
  style.size   = config.GetTitleFontSize();
  style.anchor = GRPSTATISTICSCHARTTEXTANCHOR_MIDDLE;
  style.bold   = true;

  builder.DrawText(width / 2.0, baseline, config.GetTitle().Get(), style);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double GRPSTATISTICSCHART::MeasureLegendThickness(bool horizontal)
* @brief      Measure legend thickness : reserved band size (height for a horizontal band, width for a vertical column)
* @ingroup    GRAPHIC
* 
* @param[in]  horizontal : true for a top/bottom band, false for a left/right column
* 
* @return     double : reserved thickness in output units
* 
* @note       Text width is estimated (no font metrics are available at build time)
* 
* --------------------------------------------------------------------------------------------------------------------*/
double GRPSTATISTICSCHART::MeasureLegendThickness(bool horizontal)
{
  double lf = config.GetLegendFontSize();

  if(horizontal)
    {
      return lf * 2.2;                                                          // one row : swatch + vertical padding
    }

  XDWORD nseries = data.GetNSeries();
  XDWORD maxlen  = 0;

  for(XDWORD s=0; s<nseries; s++)
    {
      GRPSTATISTICSCHARTSERIE* serie = data.GetSerie(s);
      if(!serie) continue;

      XDWORD len = serie->GetName().GetSize();
      if(len > maxlen) maxlen = len;
    }

  double swatch = lf;
  double gap    = lf * 0.4;
  double text   = (double)maxlen * lf * 0.6;                                    // estimated text width

  return swatch + gap + text + (lf * 0.8);                                      // + right padding
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSTATISTICSCHART::DrawLegend(GRPSTATISTICSCHARTBUILDER& builder, double x, double y, double width, double height, bool horizontal)
* @brief      Draw legend : one color swatch + series name per series, inside the reserved band
* @ingroup    GRAPHIC
* 
* @param[in]  builder    : output builder
* @param[in]  x          : legend band left
* @param[in]  y          : legend band top
* @param[in]  width      : legend band width
* @param[in]  height     : legend band height
* @param[in]  horizontal : true for a top/bottom row, false for a left/right column
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSTATISTICSCHART::DrawLegend(GRPSTATISTICSCHARTBUILDER& builder, double x, double y, double width, double height, bool horizontal)
{
  XDWORD nseries = data.GetNSeries();

  if(!nseries) return;

  double lf     = config.GetLegendFontSize();
  double swatch = lf;
  double gap    = lf * 0.4;

  GRPSTATISTICSCHARTTEXTSTYLE textstyle;
  textstyle.color  = config.GetTextColor();
  textstyle.size   = lf;
  textstyle.anchor = GRPSTATISTICSCHARTTEXTANCHOR_START;

  if(horizontal)
    {
      double total = 0.0;                                                       // total row width (to center it)

      for(XDWORD s=0; s<nseries; s++)
        {
          GRPSTATISTICSCHARTSERIE* serie = data.GetSerie(s);
          if(!serie) continue;

          double text = (double)serie->GetName().GetSize() * lf * 0.6;
          total += swatch + gap + text + lf;                                    // entry width + inter-entry gap
        }

      double curx = x + ((width - total) * 0.5);
      double cy   = y + (height * 0.5);

      if(curx < x) curx = x;

      for(XDWORD s=0; s<nseries; s++)
        {
          GRPSTATISTICSCHARTSERIE* serie = data.GetSerie(s);
          if(!serie) continue;

          GRPSTATISTICSCHARTCOLOR color = serie->HasColor() ? serie->GetColor() : config.GetPaletteColor(s);

          GRPSTATISTICSCHARTSTYLE box;
          box.SetFill(color);

          builder.DrawRect(curx, cy - (swatch * 0.5), swatch, swatch, box);
          builder.DrawText(curx + swatch + gap, cy + (lf * 0.35), serie->GetName().Get(), textstyle);

          double text = (double)serie->GetName().GetSize() * lf * 0.6;
          curx += swatch + gap + text + lf;
        }

      return;
    }

  //  vertical (left / right column)

  double rowh   = lf * 1.6;
  double totalh = (double)nseries * rowh;
  double cury   = y + ((height - totalh) * 0.5);

  if(cury < y) cury = y;

  for(XDWORD s=0; s<nseries; s++)
    {
      GRPSTATISTICSCHARTSERIE* serie = data.GetSerie(s);
      if(!serie) continue;

      GRPSTATISTICSCHARTCOLOR color = serie->HasColor() ? serie->GetColor() : config.GetPaletteColor(s);

      GRPSTATISTICSCHARTSTYLE box;
      box.SetFill(color);

      builder.DrawRect(x, cury, swatch, swatch, box);
      builder.DrawText(x + swatch + gap, cury + (swatch * 0.8), serie->GetName().Get(), textstyle);

      cury += rowh;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSTATISTICSCHART::ComputeNiceMax(double rawmax, double& nicemax, int& ndivisions)
* @brief      Compute a nice rounded maximum (1 / 2 / 5 x 10^n) and the number of axis divisions
* @ingroup    GRAPHIC
* 
* @param[in]  rawmax      : raw maximum value of the data
* @param[out] nicemax     : rounded maximum that covers rawmax
* @param[out] ndivisions  : number of axis divisions
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSTATISTICSCHART::ComputeNiceMax(double rawmax, double& nicemax, int& ndivisions)
{
  ndivisions = 5;

  if(rawmax <= 0.0)
    {
      nicemax = 1.0;
      return;
    }

  double rough = rawmax / (double)ndivisions;
  double mag   = pow(10.0, floor(log10(rough)));
  double norm  = rough / mag;
  double nice;

  if(norm      <= 1.0) nice = 1.0;
   else if(norm <= 2.0) nice = 2.0;
   else if(norm <= 5.0) nice = 5.0;
   else                 nice = 10.0;

  double step = nice * mag;

  nicemax = step * (double)ndivisions;

  while(nicemax < rawmax)
    {
      nicemax += step;
      ndivisions++;
    }
}


