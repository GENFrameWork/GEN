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

  double margin = config.GetMargin();
  double left   = margin + (config.GetFontSize() * 3.0);                        // room for the Y value labels
  double right  = margin;
  double top    = margin;
  double bottom = margin + (config.GetFontSize() * 2.0);                        // room for the X category labels

  if(!config.GetTitle().IsEmpty())
    {
      DrawTitle(builder, width, top + config.GetTitleFontSize());

      top += config.GetTitleFontSize() + margin;
    }

  double plotx = left;
  double ploty = top;
  double plotw = width  - left - right;
  double ploth = height - top  - bottom;

  bool ok = false;

  if((plotw > 0.0) && (ploth > 0.0))
    {
      ok = DrawPlot(builder, plotx, ploty, plotw, ploth);
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


