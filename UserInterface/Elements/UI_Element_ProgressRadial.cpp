/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_Element_ProgressRadial.cpp
* 
* @class      UI_ELEMENT_PROGRESS_RADIAL
* @brief      User Interface Element Radial Progress class (circular / ring progress indicator)
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

#include "UI_Element_ProgressRadial.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT_PROGRESS_RADIAL::UI_ELEMENT_PROGRESS_RADIAL()
* @brief      Constructor of class
* @ingroup    USERINTERFACE
*
* ---------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT_PROGRESS_RADIAL::UI_ELEMENT_PROGRESS_RADIAL()
{
  Clean();

  SetType(UI_ELEMENT_TYPE_PROGRESSRADIAL);
  GetTypeString()->Set(__L("progressradial"));
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT_PROGRESS_RADIAL::~UI_ELEMENT_PROGRESS_RADIAL()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    USERINTERFACE
*
* ---------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT_PROGRESS_RADIAL::~UI_ELEMENT_PROGRESS_RADIAL()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         float UI_ELEMENT_PROGRESS_RADIAL::GetLevel()
* @brief      Get level
* @ingroup    USERINTERFACE
*
* @return     float : current level in the range [0 .. 100]
* 
* ---------------------------------------------------------------------------------------------------------------------*/
float UI_ELEMENT_PROGRESS_RADIAL::GetLevel()
{
  return level;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT_PROGRESS_RADIAL::SetLevel(float level)
* @brief      Set level (clamped to [0 .. 100])
* @ingroup    USERINTERFACE
*
* @param[in]  level : new level
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT_PROGRESS_RADIAL::SetLevel(float level)
{
  if(level < 0)   level = 0;
  if(level > 100) level = 100;

  this->level = level;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT_PROGRESS_RADIAL::GetStartAngle()
* @brief      Get start angle (degrees)
* @ingroup    USERINTERFACE
*
* @return     double : start angle, in degrees (-90 = top / 12 o'clock)
* 
* ---------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT_PROGRESS_RADIAL::GetStartAngle()
{
  return startangle;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT_PROGRESS_RADIAL::SetStartAngle(double startangle)
* @brief      Set start angle (degrees)
* @ingroup    USERINTERFACE
*
* @param[in]  startangle : start angle in degrees
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT_PROGRESS_RADIAL::SetStartAngle(double startangle)
{
  this->startangle = startangle;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT_PROGRESS_RADIAL::GetSweepAngle()
* @brief      Get sweep angle (degrees)
* @ingroup    USERINTERFACE
*
* @return     double : sweep angle in degrees (positive = clockwise; 360 = full ring)
* 
* ---------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT_PROGRESS_RADIAL::GetSweepAngle()
{
  return sweepangle;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT_PROGRESS_RADIAL::SetSweepAngle(double sweepangle)
* @brief      Set sweep angle (degrees)
* @ingroup    USERINTERFACE
*
* @param[in]  sweepangle : sweep angle in degrees
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT_PROGRESS_RADIAL::SetSweepAngle(double sweepangle)
{
  this->sweepangle = sweepangle;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double UI_ELEMENT_PROGRESS_RADIAL::GetThickness()
* @brief      Get ring thickness (px). 0 means auto (a fraction of the radius)
* @ingroup    USERINTERFACE
*
* @return     double : ring thickness in pixels
* 
* ---------------------------------------------------------------------------------------------------------------------*/
double UI_ELEMENT_PROGRESS_RADIAL::GetThickness()
{
  return thickness;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT_PROGRESS_RADIAL::SetThickness(double thickness)
* @brief      Set ring thickness (px). 0 means auto
* @ingroup    USERINTERFACE
*
* @param[in]  thickness : ring thickness in pixels (0 = auto)
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT_PROGRESS_RADIAL::SetThickness(double thickness)
{
  if(thickness < 0) thickness = 0;

  this->thickness = thickness;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_ELEMENT_PROGRESS_RADIAL::GetRoundCap()
* @brief      get round cap
* @ingroup    USERINTERFACE
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_ELEMENT_PROGRESS_RADIAL::GetRoundCap()
{ 
  return roundcap; 
}


/**-------------------------------------------------------------------------------------------------------------------

@fn         void UI_ELEMENT_PROGRESS_RADIAL::SetRoundCap(bool roundcap)
@brief      set round cap
@ingroup    USERINTERFACE

@param[in]  roundcap : 

--------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT_PROGRESS_RADIAL::SetRoundCap(bool roundcap)    
{ 
  this->roundcap = roundcap; 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_COLOR* UI_ELEMENT_PROGRESS_RADIAL::GetLineColor()
* @brief      Get the value arc gradient END color (gradient START is GetColor())
* @ingroup    USERINTERFACE
*
* @return     UI_COLOR* : the line (gradient end) color
* 
* ---------------------------------------------------------------------------------------------------------------------*/
UI_COLOR* UI_ELEMENT_PROGRESS_RADIAL::GetLineColor()
{
  return &linecolor;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         UI_COLOR* UI_ELEMENT_PROGRESS_RADIAL::GetGradientColor()
* @brief      Get the value arc gradient END color (no gradient is drawn when its alpha is 0)
* @ingroup    USERINTERFACE
* @return     UI_COLOR* :
* ---------------------------------------------------------------------------------------------------------------------*/
UI_COLOR* UI_ELEMENT_PROGRESS_RADIAL::GetGradientColor()
{
  return &gradientcolor;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         int UI_ELEMENT_PROGRESS_RADIAL::GetGradientMode()
* @brief      Get the gradient mode (FILL / TRACK)
* @ingroup    USERINTERFACE
* @return     int :
* ---------------------------------------------------------------------------------------------------------------------*/
int UI_ELEMENT_PROGRESS_RADIAL::GetGradientMode()
{
  return gradientmode;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         void UI_ELEMENT_PROGRESS_RADIAL::SetGradientMode(int gradientmode)
* @brief      Set the gradient mode (FILL / TRACK)
* @ingroup    USERINTERFACE
* @param[in]  gradientmode :
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT_PROGRESS_RADIAL::SetGradientMode(int gradientmode)
{
  this->gradientmode = gradientmode;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_ELEMENT_PROGRESS_RADIAL::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_ELEMENT_PROGRESS_RADIAL::Clean()
{
  level       = 0.0f;

  startangle  = UI_ELEMENT_PROGRESS_RADIAL_DEFAULT_STARTANGLE;
  sweepangle  = UI_ELEMENT_PROGRESS_RADIAL_DEFAULT_SWEEPANGLE;
  thickness   = UI_ELEMENT_PROGRESS_RADIAL_DEFAULT_THICKNESS;

  roundcap    = false;

  gradientmode = UI_ELEMENT_PROGRESS_GRADIENTMODE_FILL;
}
