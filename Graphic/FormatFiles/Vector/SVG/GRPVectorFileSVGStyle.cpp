/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPVectorFileSVGStyle.cpp
* 
* @class      GRPVECTORFILESVGSTYLE
* @brief      Graphic Vector File SVG Style class (fill, stroke, stroke-width, opacity, fill-rule)
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

#include "GRPVectorFileSVGStyle.h"

#include "XFileXML.h"
#include "XVector.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPVECTORFILESVGSTYLE::GRPVECTORFILESVGSTYLE()
* @brief      Constructor of class
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILESVGSTYLE::GRPVECTORFILESVGSTYLE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPVECTORFILESVGSTYLE::~GRPVECTORFILESVGSTYLE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILESVGSTYLE::~GRPVECTORFILESVGSTYLE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPVECTORFILESVGSTYLE::ApplyData(XFILEXMLELEMENT* element)
* @brief      Apply data : read presentation attributes and the inline style attribute (style overrides)
* @ingroup    GRAPHIC
*
* @param[in]  element : xml element
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVGSTYLE::ApplyData(XFILEXMLELEMENT* element)
{
  if(!element) return false;

  static XCHAR* presentationattrs[] = { __L("fill")          ,
                                        __L("stroke")        ,
                                        __L("stroke-width")  ,
                                        __L("opacity")       ,
                                        __L("fill-opacity")  ,
                                        __L("stroke-opacity"),
                                        __L("fill-rule")     ,
                                        NULL                 };

  for(int c=0; presentationattrs[c]; c++)
    {
      XCHAR* value = element->GetValueAttribute(presentationattrs[c]);
      if(value)
        {
          XSTRING name(presentationattrs[c]);
          XSTRING strvalue(value);

          ApplyProperty(name, strvalue);
        }
    }

  // The inline style attribute overrides the presentation attributes.
  XCHAR* style = element->GetValueAttribute(__L("style"));
  if(style)
    {
      XSTRING strstyle(style);
      ParseStyleAttribute(strstyle);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRPVECTORFILESVGSTYLE::InheritFrom(GRPVECTORFILESVGSTYLE& parent)
* @brief      Inherit from : for every property not specified locally, take the parent value
* @ingroup    GRAPHIC
*
* @param[in]  parent : parent (effective) style
*
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILESVGSTYLE::InheritFrom(GRPVECTORFILESVGSTYLE& parent)
{
  if(!fillspecified)
    {
      fillspecified = parent.IsFillSpecified();
      fillnone      = !parent.HasFill();
      fillcolor     = parent.GetFillColor();
    }

  if(!strokespecified)
    {
      strokespecified = parent.IsStrokeSpecified();
      strokenone      = !parent.HasStroke();
      strokecolor     = parent.GetStrokeColor();
    }

  if(!strokewidthspecified)
    {
      strokewidthspecified = parent.IsStrokeWidthSpecified();
      strokewidth          = parent.GetStrokeWidth();
    }

  // opacity is not inherited in SVG, but fill-opacity / stroke-opacity / fill-rule are.
  fillopacity   = parent.GetFillOpacity()   * fillopacity;
  strokeopacity = parent.GetStrokeOpacity() * strokeopacity;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRPVECTORFILESVGSTYLE::HasFill()
* @brief      Has fill
* @ingroup    GRAPHIC
* @return     bool : true if there is a fill (not 'none')
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVGSTYLE::HasFill()                       { return !fillnone; }


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRPVECTORFILESVGSTYLE::IsFillSpecified()
* @brief      Is fill specified
* @ingroup    GRAPHIC
* @return     bool : true if fill was specified
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVGSTYLE::IsFillSpecified()               { return fillspecified; }


/**-------------------------------------------------------------------------------------------------------------------
* @fn         GRP2DCOLOR_RGBA8& GRPVECTORFILESVGSTYLE::GetFillColor()
* @brief      Get fill color
* @ingroup    GRAPHIC
* @return     GRP2DCOLOR_RGBA8& : fill color
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DCOLOR_RGBA8& GRPVECTORFILESVGSTYLE::GetFillColor()     { return fillcolor; }


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRPVECTORFILESVGSTYLE::HasStroke()
* @brief      Has stroke
* @ingroup    GRAPHIC
* @return     bool : true if there is a stroke (not 'none')
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVGSTYLE::HasStroke()                     { return (strokespecified && !strokenone); }


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRPVECTORFILESVGSTYLE::IsStrokeSpecified()
* @brief      Is stroke specified
* @ingroup    GRAPHIC
* @return     bool : true if stroke was specified
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVGSTYLE::IsStrokeSpecified()             { return strokespecified; }


/**-------------------------------------------------------------------------------------------------------------------
* @fn         GRP2DCOLOR_RGBA8& GRPVECTORFILESVGSTYLE::GetStrokeColor()
* @brief      Get stroke color
* @ingroup    GRAPHIC
* @return     GRP2DCOLOR_RGBA8& : stroke color
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DCOLOR_RGBA8& GRPVECTORFILESVGSTYLE::GetStrokeColor()   { return strokecolor; }


/**-------------------------------------------------------------------------------------------------------------------
* @fn         double GRPVECTORFILESVGSTYLE::GetStrokeWidth()
* @brief      Get stroke width
* @ingroup    GRAPHIC
* @return     double : stroke width
* --------------------------------------------------------------------------------------------------------------------*/
double GRPVECTORFILESVGSTYLE::GetStrokeWidth()              { return strokewidth; }


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRPVECTORFILESVGSTYLE::IsStrokeWidthSpecified()
* @brief      Is stroke width specified
* @ingroup    GRAPHIC
* @return     bool : true if stroke width was specified
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVGSTYLE::IsStrokeWidthSpecified()        { return strokewidthspecified; }


/**-------------------------------------------------------------------------------------------------------------------
* @fn         double GRPVECTORFILESVGSTYLE::GetOpacity()
* @brief      Get opacity
* @ingroup    GRAPHIC
* @return     double : opacity
* --------------------------------------------------------------------------------------------------------------------*/
double GRPVECTORFILESVGSTYLE::GetOpacity()                  { return opacity; }


/**-------------------------------------------------------------------------------------------------------------------
* @fn         double GRPVECTORFILESVGSTYLE::GetFillOpacity()
* @brief      Get fill opacity
* @ingroup    GRAPHIC
* @return     double : fill opacity
* --------------------------------------------------------------------------------------------------------------------*/
double GRPVECTORFILESVGSTYLE::GetFillOpacity()              { return fillopacity; }


/**-------------------------------------------------------------------------------------------------------------------
* @fn         double GRPVECTORFILESVGSTYLE::GetStrokeOpacity()
* @brief      Get stroke opacity
* @ingroup    GRAPHIC
* @return     double : stroke opacity
* --------------------------------------------------------------------------------------------------------------------*/
double GRPVECTORFILESVGSTYLE::GetStrokeOpacity()            { return strokeopacity; }


/**-------------------------------------------------------------------------------------------------------------------
* @fn         GRP2DPATHFILLRULE GRPVECTORFILESVGSTYLE::GetFillRule()
* @brief      Get fill rule
* @ingroup    GRAPHIC
* @return     GRP2DPATHFILLRULE : fill rule
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DPATHFILLRULE GRPVECTORFILESVGSTYLE::GetFillRule()      { return fillrule; }


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPVECTORFILESVGSTYLE::ApplyProperty(XSTRING& name, XSTRING& value)
* @brief      Apply property : set a single style property by name
* @note       INTERNAL
* @ingroup    GRAPHIC
*
* @param[in]  name : property name
* @param[in]  value : property value
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVGSTYLE::ApplyProperty(XSTRING& name, XSTRING& value)
{
  XSTRING propname(name);
  propname.DeleteCharacter(__L(' '));
  propname.ToLowerCase();

  XSTRING propvalue(value);
  propvalue.DeleteCharacter(__L(' '));

  if(propvalue.IsEmpty()) return false;

  if(!propname.Compare(__L("fill"), true))
    {
      bool isnone = false;
      if(ParseColor(propvalue, fillcolor, isnone))
        {
          fillnone      = isnone;
          fillspecified = true;
        }
    }
   else if(!propname.Compare(__L("stroke"), true))
    {
      bool isnone = false;
      if(ParseColor(propvalue, strokecolor, isnone))
        {
          strokenone      = isnone;
          strokespecified = true;
        }
    }
   else if(!propname.Compare(__L("stroke-width"), true))
    {
      strokewidth          = propvalue.ConvertToDouble(0, NULL, false);
      strokewidthspecified = true;
    }
   else if(!propname.Compare(__L("opacity"), true))
    {
      opacity = propvalue.ConvertToDouble(0, NULL, false);
    }
   else if(!propname.Compare(__L("fill-opacity"), true))
    {
      fillopacity = propvalue.ConvertToDouble(0, NULL, false);
    }
   else if(!propname.Compare(__L("stroke-opacity"), true))
    {
      strokeopacity = propvalue.ConvertToDouble(0, NULL, false);
    }
   else if(!propname.Compare(__L("fill-rule"), true))
    {
      if(!propvalue.Compare(__L("evenodd"), true))  fillrule = GRP2DPATHFILLRULE_EVENODD;
       else                                         fillrule = GRP2DPATHFILLRULE_NONZERO;
    }
   else
    {
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPVECTORFILESVGSTYLE::ParseStyleAttribute(XSTRING& style)
* @brief      Parse style attribute : "fill:#f00;stroke:black;stroke-width:2"
* @note       INTERNAL
* @ingroup    GRAPHIC
*
* @param[in]  style : style attribute value
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVGSTYLE::ParseStyleAttribute(XSTRING& style)
{
  XVECTOR<XSTRING*> declarations;
  style.Split(__L(';'), declarations, false);

  for(XDWORD c=0; c<declarations.GetSize(); c++)
    {
      XSTRING* declaration = declarations.Get(c);
      if(declaration && !declaration->IsEmpty())
        {
          int colon = declaration->Find(__L(":"), false, 0);
          if(colon > 0)
            {
              XSTRING name;
              XSTRING value;

              declaration->Copy(0, colon, name);
              declaration->Copy(colon+1, (int)declaration->GetSize(), value);

              ApplyProperty(name, value);
            }
        }
    }

  declarations.DeleteContents();
  declarations.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPVECTORFILESVGSTYLE::ParseColor(XSTRING& value, GRP2DCOLOR_RGBA8& color, bool& isnone)
* @brief      Parse color : #rgb, #rrggbb, rgb(r,g,b), none, basic named colors
* @note       INTERNAL
* @ingroup    GRAPHIC
*
* @param[in]  value : color string
* @param[in]  color : output color
* @param[out] isnone : true if the color is 'none'
*
* @return     bool : true if the value was recognized.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVGSTYLE::ParseColor(XSTRING& value, GRP2DCOLOR_RGBA8& color, bool& isnone)
{
  isnone = false;

  XSTRING str(value);
  str.DeleteCharacter(__L(' '));

  if(str.IsEmpty()) return false;

  if(!str.Compare(__L("none"), true))
    {
      isnone = true;
      return true;
    }

  if(str[0] == __L('#'))
    {
      int len = (int)str.GetSize() - 1;

      if(len == 3)
        {
          int r = HexValue(str[1]);
          int g = HexValue(str[2]);
          int b = HexValue(str[3]);

          color = GRP2DCOLOR_RGBA8((r*16)+r, (g*16)+g, (b*16)+b);
          return true;
        }
       else if(len >= 6)
        {
          int r = (HexValue(str[1])*16) + HexValue(str[2]);
          int g = (HexValue(str[3])*16) + HexValue(str[4]);
          int b = (HexValue(str[5])*16) + HexValue(str[6]);

          color = GRP2DCOLOR_RGBA8(r, g, b);
          return true;
        }

      return false;
    }

  if(str.Find(__L("rgb("), true, 0) == 0)
    {
      int open  = str.Find(__L("("), false, 0);
      int close = str.Find(__L(")"), false, 0);

      if((open >= 0) && (close > open))
        {
          XSTRING inside;
          str.Copy(open+1, close, inside);
          inside.Replace(__L(","), __L(" "));

          XVECTOR<XSTRING*> tokens;
          inside.Split(__L(' '), tokens, false);

          int comp[3] = { 0, 0, 0 };
          int n       = 0;

          for(XDWORD c=0; c<tokens.GetSize() && n<3; c++)
            {
              XSTRING* token = tokens.Get(c);
              if(token && !token->IsEmpty())
                {
                  comp[n] = token->ConvertToInt(0, NULL, false);
                  n++;
                }
            }

          tokens.DeleteContents();
          tokens.DeleteAll();

          if(n >= 3)
            {
              color = GRP2DCOLOR_RGBA8(comp[0], comp[1], comp[2]);
              return true;
            }
        }

      return false;
    }

  // Basic named colors.
  struct GRPVECTORFILESVGNAMEDCOLOR { XCHAR* name;  int r;  int g;  int b; };

  static GRPVECTORFILESVGNAMEDCOLOR namedcolors[] = { { __L("black")  ,   0,   0,   0 },
                                                      { __L("white")  , 255, 255, 255 },
                                                      { __L("red")    , 255,   0,   0 },
                                                      { __L("green")  ,   0, 128,   0 },
                                                      { __L("blue")   ,   0,   0, 255 },
                                                      { __L("yellow") , 255, 255,   0 },
                                                      { __L("cyan")   ,   0, 255, 255 },
                                                      { __L("aqua")   ,   0, 255, 255 },
                                                      { __L("magenta"), 255,   0, 255 },
                                                      { __L("fuchsia"), 255,   0, 255 },
                                                      { __L("gray")   , 128, 128, 128 },
                                                      { __L("grey")   , 128, 128, 128 },
                                                      { __L("silver") , 192, 192, 192 },
                                                      { __L("maroon") , 128,   0,   0 },
                                                      { __L("olive")  , 128, 128,   0 },
                                                      { __L("lime")   ,   0, 255,   0 },
                                                      { __L("teal")   ,   0, 128, 128 },
                                                      { __L("navy")   ,   0,   0, 128 },
                                                      { __L("purple") , 128,   0, 128 },
                                                      { __L("orange") , 255, 165,   0 },
                                                      { NULL          ,   0,   0,   0 } };

  for(int c=0; namedcolors[c].name; c++)
    {
      if(!str.Compare(namedcolors[c].name, true))
        {
          color = GRP2DCOLOR_RGBA8(namedcolors[c].r, namedcolors[c].g, namedcolors[c].b);
          return true;
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int GRPVECTORFILESVGSTYLE::HexValue(XCHAR character)
* @brief      Hex value : convert one hexadecimal character to its numeric value (0..15)
* @note       INTERNAL
* @ingroup    GRAPHIC
*
* @param[in]  character : hexadecimal character
*
* @return     int : numeric value (0..15) or 0 if invalid
*
* --------------------------------------------------------------------------------------------------------------------*/
int GRPVECTORFILESVGSTYLE::HexValue(XCHAR character)
{
  if((character >= __L('0')) && (character <= __L('9')))  return (int)(character - __L('0'));
  if((character >= __L('a')) && (character <= __L('f')))  return (int)(character - __L('a')) + 10;
  if((character >= __L('A')) && (character <= __L('F')))  return (int)(character - __L('A')) + 10;

  return 0;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRPVECTORFILESVGSTYLE::Clean()
* @brief      Clean the attributes of the class: Default initialize (SVG defaults)
* @note       INTERNAL
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILESVGSTYLE::Clean()
{
  fillspecified        = false;
  fillnone             = false;
  fillcolor            = GRP2DCOLOR_RGBA8(0, 0, 0);                            // SVG default fill is black

  strokespecified      = false;
  strokenone           = true;                                               // SVG default stroke is none
  strokecolor          = GRP2DCOLOR_RGBA8(0, 0, 0);

  strokewidthspecified = false;
  strokewidth          = 1.0;                                               // SVG default stroke-width is 1

  opacity              = 1.0;
  fillopacity          = 1.0;
  strokeopacity        = 1.0;

  fillrule             = GRP2DPATHFILLRULE_NONZERO;
}




