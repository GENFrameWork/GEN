/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPVectorFileSVGObjText.cpp
* 
* @class      GRPVECTORFILESVGOBJTEXT
* @brief      Graphic Vector File SVG Object Text class (<text> / <tspan>)
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

#include "GRPVectorFileSVGObjText.h"

#include "XFileXML.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/




/**-------------------------------------------------------------------------------------------------------------------
* @fn         GRPVECTORFILESVGOBJTEXT::GRPVECTORFILESVGOBJTEXT()
* @brief      Constructor of class
* @ingroup    GRAPHIC
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILESVGOBJTEXT::GRPVECTORFILESVGOBJTEXT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         GRPVECTORFILESVGOBJTEXT::~GRPVECTORFILESVGOBJTEXT()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    GRAPHIC
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILESVGOBJTEXT::~GRPVECTORFILESVGOBJTEXT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         bool GRPVECTORFILESVGOBJTEXT::ApplyData(XFILEXMLELEMENT* element)
* @brief      Apply data : read common attributes plus x, y, font-size, text-anchor and the text content
* @ingroup    GRAPHIC
* @param[in]  element : xml element
* @return     bool : true if is succesful.
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVGOBJTEXT::ApplyData(XFILEXMLELEMENT* element)
{
  if(!element) return false;

  GRPVECTORFILESVGOBJ::ApplyData(element);

  x = element->GetFloatValueAttribute(__L("x"));
  y = element->GetFloatValueAttribute(__L("y"));

  XCHAR* valuefontsize = element->GetValueAttribute(__L("font-size"));
  if(valuefontsize)
    {
      XSTRING fontsizestr(valuefontsize);
      double  parsed = fontsizestr.ConvertToDouble(0, NULL, false);            // tolerates "16px" -> 16
      if(parsed > 0.0)  fontsize = parsed;
    }

  XCHAR* valueanchor = element->GetValueAttribute(__L("text-anchor"));
  if(valueanchor)
    {
      XSTRING anchorstr(valueanchor);
      if(!anchorstr.Compare(__L("middle"), true))     textanchor = GRPVECTORFILESVGTEXTANCHOR_MIDDLE;
       else if(!anchorstr.Compare(__L("end"), true))  textanchor = GRPVECTORFILESVGTEXTANCHOR_END;
       else                                           textanchor = GRPVECTORFILESVGTEXTANCHOR_START;
    }

  //  Text content : trim leading / trailing whitespace.
  XSTRING rawtext(element->GetValue());

  int len = (int)rawtext.GetSize();
  int ini = 0;
  int end = len;

  while((ini < len) && ((rawtext[ini] == __C(' ')) || (rawtext[ini] == __C('\t')) || (rawtext[ini] == __C('\n')) || (rawtext[ini] == __C('\r'))))  ini++;
  while((end > ini) && ((rawtext[end-1] == __C(' ')) || (rawtext[end-1] == __C('\t')) || (rawtext[end-1] == __C('\n')) || (rawtext[end-1] == __C('\r'))))  end--;

  if(end > ini)  rawtext.Copy(ini, end, text);
   else          text.Empty();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* @fn         XSTRING* GRPVECTORFILESVGOBJTEXT::GetText()
* @brief      Get text
* @ingroup    GRAPHIC
* @return     XSTRING* : text content
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* GRPVECTORFILESVGOBJTEXT::GetText()   { return &text; }


double                     GRPVECTORFILESVGOBJTEXT::GetX()           { return x;          }
double                     GRPVECTORFILESVGOBJTEXT::GetY()           { return y;          }
double                     GRPVECTORFILESVGOBJTEXT::GetFontSize()    { return fontsize;   }
GRPVECTORFILESVGTEXTANCHOR GRPVECTORFILESVGOBJTEXT::GetTextAnchor()  { return textanchor; }


/**-------------------------------------------------------------------------------------------------------------------
* @fn         void GRPVECTORFILESVGOBJTEXT::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    GRAPHIC
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILESVGOBJTEXT::Clean()
{
  x          = 0.0;
  y          = 0.0;
  fontsize   = 16.0;                                                            // SVG medium default
  textanchor = GRPVECTORFILESVGTEXTANCHOR_START;
}
