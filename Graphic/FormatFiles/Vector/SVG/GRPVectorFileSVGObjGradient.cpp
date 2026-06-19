/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPVectorFileSVGObjGradient.cpp
* 
* @class      GRPVECTORFILESVGOBJGRADIENT
* @brief      Graphic Vector File SVG Object Gradient class (<linearGradient> / <radialGradient> paint server)
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

#include "GRPVectorFileSVGObjGradient.h"

#include "XFileXML.h"

#include "GRPVectorFileSVGStyle.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/




/**-------------------------------------------------------------------------------------------------------------------
* @fn         GRPVECTORFILESVGOBJGRADIENT::GRPVECTORFILESVGOBJGRADIENT()
* @brief      Constructor of class
* @ingroup    GRAPHIC
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILESVGOBJGRADIENT::GRPVECTORFILESVGOBJGRADIENT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         GRPVECTORFILESVGOBJGRADIENT::~GRPVECTORFILESVGOBJGRADIENT()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    GRAPHIC
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILESVGOBJGRADIENT::~GRPVECTORFILESVGOBJGRADIENT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRPVECTORFILESVGOBJGRADIENT::ApplyData(XFILEXMLELEMENT* element)
* @brief      Apply data : read the common attributes plus the gradient definition and its <stop> children
* @ingroup    GRAPHIC
* @param[in]  element : xml element
* @return     bool : true if is succesful.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVGOBJGRADIENT::ApplyData(XFILEXMLELEMENT* element)
{
  if(!element) return false;

  GRPVECTORFILESVGOBJ::ApplyData(element);                                     // id, etc.

  XCHAR* valueunits = element->GetValueAttribute(__L("gradientUnits"));
  if(valueunits)
    {
      XSTRING unitstr(valueunits);
      if(!unitstr.Compare(__L("userSpaceOnUse"), true))  units = GRPVECTORFILESVGGRADIENTUNITS_USERSPACEONUSE;
    }

  XCHAR* valuetransform = element->GetValueAttribute(__L("gradientTransform"));
  if(valuetransform)
    {
      XSTRING transformstr(valuetransform);
      gradienttransform.ParseFromString(transformstr.Get());
    }

  if(GetObjType() == GRPVECTORFILESVGOBJTYPE_RADIALGRADIENT)
    {
      cx = ParseCoordinate(element->GetValueAttribute(__L("cx")), 0.5);
      cy = ParseCoordinate(element->GetValueAttribute(__L("cy")), 0.5);
      r  = ParseCoordinate(element->GetValueAttribute(__L("r")) , 0.5);
      fx = ParseCoordinate(element->GetValueAttribute(__L("fx")), cx);
      fy = ParseCoordinate(element->GetValueAttribute(__L("fy")), cy);
    }
   else
    {
      x1 = ParseCoordinate(element->GetValueAttribute(__L("x1")), 0.0);
      y1 = ParseCoordinate(element->GetValueAttribute(__L("y1")), 0.0);
      x2 = ParseCoordinate(element->GetValueAttribute(__L("x2")), 1.0);
      y2 = ParseCoordinate(element->GetValueAttribute(__L("y2")), 0.0);
    }

  ParseStops(element);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRPVECTORFILESVGOBJGRADIENT::ParseStops(XFILEXMLELEMENT* element)
* @brief      Parse stops : read the <stop> children (offset, stop-color, stop-opacity)
* @note       INTERNAL
* @ingroup    GRAPHIC
* @param[in]  element : gradient xml element
* @return     bool : true if at least one stop was read.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVGOBJGRADIENT::ParseStops(XFILEXMLELEMENT* element)
{
  nstops = 0;

  for(XDWORD c=0; c<element->GetNElements(); c++)
    {
      XFILEXMLELEMENT* child = element->GetElement(c);
      if(!child) continue;
      if(child->GetType() != XFILEXMLELEMENTTYPE_NORMAL) continue;
      if(child->GetName().Compare(__L("stop"), true)) continue;                // not a <stop>

      if(nstops >= GRP2DGRADIENT_MAXSTOPS) break;

      double offset = ParseCoordinate(child->GetValueAttribute(__L("offset")), 0.0);
      if(offset < 0.0) offset = 0.0;
      if(offset > 1.0) offset = 1.0;

      GRP2DCOLOR_RGBA8 color(0, 0, 0);
      XSTRING          colorstr;
      if(GetStopProperty(child, __L("stop-color"), colorstr))
        {
          bool isnone = false;
          GRPVECTORFILESVGSTYLE::ParseColor(colorstr, color, isnone);
        }

      double  stopopacity = 1.0;
      XSTRING opacitystr;
      if(GetStopProperty(child, __L("stop-opacity"), opacitystr))
        {
          stopopacity = opacitystr.ConvertToDouble(0, NULL, false);
        }

      if(stopopacity < 0.0) stopopacity = 0.0;
      if(stopopacity > 1.0) stopopacity = 1.0;

      color.a = (XBYTE)(255.0 * stopopacity);

      stops[nstops].offset = offset;
      stops[nstops].color  = color;
      nstops++;
    }

  return (nstops > 0);
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         double GRPVECTORFILESVGOBJGRADIENT::ParseCoordinate(XCHAR* value, double defaultvalue)
* @brief      Parse coordinate : parse a gradient coordinate, supporting the trailing '%' (divided by 100)
* @note       INTERNAL
* @ingroup    GRAPHIC
* @param[in]  value : attribute value (may be NULL)
* @param[in]  defaultvalue : value to return when the attribute is missing
* @return     double : parsed coordinate.
* --------------------------------------------------------------------------------------------------------------------*/
double GRPVECTORFILESVGOBJGRADIENT::ParseCoordinate(XCHAR* value, double defaultvalue)
{
  if(!value) return defaultvalue;

  XSTRING valuestr(value);
  valuestr.DeleteCharacter(__C(' '));

  if(valuestr.IsEmpty()) return defaultvalue;

  bool percent = false;
  if(valuestr[(int)valuestr.GetSize() - 1] == __C('%'))
    {
      percent = true;
      valuestr.DeleteCharacter(__C('%'));
    }

  double result = valuestr.ConvertToDouble(0, NULL, false);

  if(percent)  result /= 100.0;

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRPVECTORFILESVGOBJGRADIENT::GetStopProperty(XFILEXMLELEMENT* element, XCHAR* name, XSTRING& outvalue)
* @brief      Get stop property : read a stop property from its attribute or from the inline style="" string
* @note       INTERNAL
* @ingroup    GRAPHIC
* @param[in]  element : <stop> element
* @param[in]  name : property name
* @param[out] outvalue : property value
* @return     bool : true if the property was found.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVGOBJGRADIENT::GetStopProperty(XFILEXMLELEMENT* element, XCHAR* name, XSTRING& outvalue)
{
  XCHAR* attribute = element->GetValueAttribute(name);
  if(attribute)
    {
      outvalue = attribute;
      return true;
    }

  XCHAR* style = element->GetValueAttribute(__L("style"));
  if(!style) return false;

  XSTRING stylestr(style);

  XSTRING key(name);
  key += __L(":");

  int index = stylestr.Find(key.Get(), true, 0);
  if(index < 0) return false;

  int start = index + (int)key.GetSize();
  int end   = stylestr.Find(__L(";"), false, start);
  if(end < 0)  end = (int)stylestr.GetSize();

  stylestr.Copy(start, end, outvalue);
  outvalue.DeleteCharacter(__C(' '));

  return (!outvalue.IsEmpty());
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRPVECTORFILESVGOBJGRADIENT::IsLinear()
* @brief      Is linear
* @ingroup    GRAPHIC
* @return     bool : true if it is a linear gradient.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVGOBJGRADIENT::IsLinear()   { return (GetObjType() == GRPVECTORFILESVGOBJTYPE_LINEARGRADIENT); }


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRPVECTORFILESVGOBJGRADIENT::IsRadial()
* @brief      Is radial
* @ingroup    GRAPHIC
* @return     bool : true if it is a radial gradient.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVGOBJGRADIENT::IsRadial()   { return (GetObjType() == GRPVECTORFILESVGOBJTYPE_RADIALGRADIENT); }


GRPVECTORFILESVGGRADIENTUNITS GRPVECTORFILESVGOBJGRADIENT::GetUnits()              { return units;              }
GRPVECTORFILESVGTRANSFORM*    GRPVECTORFILESVGOBJGRADIENT::GetGradientTransform()  { return &gradienttransform; }

double GRPVECTORFILESVGOBJGRADIENT::GetX1()   { return x1; }
double GRPVECTORFILESVGOBJGRADIENT::GetY1()   { return y1; }
double GRPVECTORFILESVGOBJGRADIENT::GetX2()   { return x2; }
double GRPVECTORFILESVGOBJGRADIENT::GetY2()   { return y2; }

double GRPVECTORFILESVGOBJGRADIENT::GetCX()   { return cx; }
double GRPVECTORFILESVGOBJGRADIENT::GetCY()   { return cy; }
double GRPVECTORFILESVGOBJGRADIENT::GetR()    { return r;  }
double GRPVECTORFILESVGOBJGRADIENT::GetFX()   { return fx; }
double GRPVECTORFILESVGOBJGRADIENT::GetFY()   { return fy; }

GRP2DGRADIENTSTOP* GRPVECTORFILESVGOBJGRADIENT::GetStops()   { return stops;  }
int                GRPVECTORFILESVGOBJGRADIENT::GetNStops()  { return nstops; }


/**-------------------------------------------------------------------------------------------------------------------
* @fn         void GRPVECTORFILESVGOBJGRADIENT::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    GRAPHIC
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILESVGOBJGRADIENT::Clean()
{
  units  = GRPVECTORFILESVGGRADIENTUNITS_OBJECTBOUNDINGBOX;

  gradienttransform.SetIdentity();

  x1 = 0.0;   y1 = 0.0;   x2 = 1.0;   y2 = 0.0;
  cx = 0.5;   cy = 0.5;   r  = 0.5;   fx = 0.5;   fy = 0.5;

  nstops = 0;
}
