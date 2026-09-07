/**-------------------------------------------------------------------------------------------------------------------
*
* @file       UI_Length.cpp
*
* @class      UI_LENGTH
* @brief      User Interface : a typed CSS-style length value (number / percentage / bare keyword).
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

#include "UI_Length.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_LENGTH::UI_LENGTH()
* @brief      Constructor of class
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_LENGTH::UI_LENGTH()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_LENGTH::~UI_LENGTH()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_LENGTH::~UI_LENGTH()
{

}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_LENGTH::Parse(XSTRING& raw)
* @brief      Parse
* @ingroup    USERINTERFACE
*
* @param[in]  raw : Text to use.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_LENGTH::Parse(XSTRING& raw)
{
  Clean();

  if(raw.IsEmpty()) return false;

  XDWORD len   = raw.GetSize();
  XCHAR  first = raw[0];

  bool starts_numeric = (first == __C('-')) || (first == __C('+')) || (first == __C('.')) ||
                         ((first >= __C('0')) && (first <= __C('9')));

  if(!starts_numeric)
    {
      type    = UI_LENGTH_TYPE_KEYWORD;
      keyword = raw;

      return true;
    }

  XCHAR last = raw[(int)(len - 1)];
  if(last == __C('%'))
    {
      XSTRING numeric;
      raw.Copy(0, (int)(len - 1), numeric);

      type  = UI_LENGTH_TYPE_PERCENT;
      value = numeric.ConvertToDouble();
    }
   else
    {
      type  = UI_LENGTH_TYPE_NUMBER;
      value = raw.ConvertToDouble();
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         UI_LENGTH_TYPE UI_LENGTH::GetType()
* @brief      Get type
* @ingroup    USERINTERFACE
*
* @return     UI_LENGTH_TYPE : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
UI_LENGTH_TYPE UI_LENGTH::GetType()
{
  return type;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double UI_LENGTH::GetValue()
* @brief      Get value
* @ingroup    USERINTERFACE
*
* @return     double : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
double UI_LENGTH::GetValue()
{
  return value;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_LENGTH::IsKeyword(XCHAR* keyword)
* @brief      Is keyword
* @ingroup    USERINTERFACE
*
* @param[in]  keyword : Keyword value.
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_LENGTH::IsKeyword(XCHAR* keyword)
{
  if(type != UI_LENGTH_TYPE_KEYWORD) return false;

  return !this->keyword.Compare(keyword, true);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_LENGTH::Resolve(double basis, double& out)
* @brief      Resolve
* @ingroup    USERINTERFACE
*
* @param[in]  basis : Basis value used to resolve a PERCENT length; ignored for NUMBER.
* @param[out] out : Receives the resolved value.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_LENGTH::Resolve(double basis, double& out)
{
  switch(type)
    {
      case UI_LENGTH_TYPE_NUMBER  : out = value;               return true;
      case UI_LENGTH_TYPE_PERCENT : out = (value / 100.0) * basis; return true;
      default                     :                            return false;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void UI_LENGTH::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* --------------------------------------------------------------------------------------------------------------------*/
void UI_LENGTH::Clean()
{
  type  = UI_LENGTH_TYPE_UNDEFINED;
  value = 0.0;

  keyword.Empty();
}
