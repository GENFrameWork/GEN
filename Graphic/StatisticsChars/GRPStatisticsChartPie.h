/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPStatisticsChartPie.h
* 
* @class      GRPSTATISTICSCHARTPIE
* @brief      Graphic Statistics Chart : pie / donut chart (one sector per category, from the first serie). Concrete chart type.
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

#include "GRPStatisticsChart.h"



/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class GRPSTATISTICSCHARTPIE : public GRPSTATISTICSCHART
{
  public:
                                    GRPSTATISTICSCHARTPIE                 ();
    virtual                         ~GRPSTATISTICSCHARTPIE                ();

    void                            SetInnerRadiusRatio                   (double ratio)                                  { innerradiusratio = ratio;                             }
    double                          GetInnerRadiusRatio                   ()                                              { return innerradiusratio;                              }

  protected:

    bool                            DrawPlot                              (GRPSTATISTICSCHARTBUILDER& builder, double x, double y, double width, double height);

    XDWORD                          GetLegendCount                        ();
    bool                            GetLegendEntry                        (XDWORD index, XSTRING** name, GRPSTATISTICSCHARTCOLOR& color);

  private:

    void                            DrawSector                            (GRPSTATISTICSCHARTBUILDER& builder, double cx, double cy, double router, double rinner, double startdeg, double enddeg, GRPSTATISTICSCHARTSTYLE& style);

    double                          innerradiusratio;
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/



