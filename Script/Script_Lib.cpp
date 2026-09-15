/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       Script_Lib.cpp
* 
* @class      SCRIPT_LIB
* @brief      Script library class
* @ingroup    SCRIPT
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

#include "Script_Lib.h"

#include <string.h>

#include "XVariant.h"

#include "Script.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/




/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SCRIPT_LIB::SCRIPT_LIB(XCHAR* ID)
* @brief      Constructor of class
* @ingroup    SCRIPT
* 
* @param[in]  ID : Identifier to use.
* 
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_LIB::SCRIPT_LIB(XCHAR* ID)
{
  Clean();

  this->ID = ID;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SCRIPT_LIB::~SCRIPT_LIB()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    SCRIPT
* 
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_LIB::~SCRIPT_LIB()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* SCRIPT_LIB::GetID()
* @brief      Get ID
* @ingroup    SCRIPT
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* SCRIPT_LIB::GetID()
{
  return &ID;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_LIB::AddLibraryFunctions(SCRIPT* script)
* @brief      Add library functions
* @ingroup    SCRIPT
* 
* @param[in]  script : Script pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_LIB::AddLibraryFunctions(SCRIPT* script)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_LIB::GetParamConverted(XVARIANT* variant, bool& value)
* @brief      Get param converted
* @ingroup    SCRIPT
* 
* @param[in]  variant : Variant pointer to use.
* @param[in]  value : Value value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_LIB::GetParamConverted(XVARIANT* variant, bool& value)
{
  if(!variant) return false;

  value = 0;

  switch(variant->GetType())
    {
      case XVARIANT_TYPE_NULL           : break;

      case XVARIANT_TYPE_BOOLEAN        : value = (bool)(*variant)?true:false;
                                          break;
      
      case XVARIANT_TYPE_INTEGER        : value = (int)(*variant)?true:false;
                                          break;

      case XVARIANT_TYPE_FLOAT          : value = (float)(*variant)?true:false;;                                          
                                          break;

      case XVARIANT_TYPE_DOUBLE         : value = (double)(*variant)?true:false;;       
                                          break;

      case XVARIANT_TYPE_CHAR           :
      case XVARIANT_TYPE_XCHAR          :
      case XVARIANT_TYPE_STRING         :
      case XVARIANT_TYPE_DATE           :
      case XVARIANT_TYPE_TIME           :
      case XVARIANT_TYPE_DATETIME       : 
                            default     : return false;
                      

    }

  return true;
}

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_LIB::GetParamConverted(XVARIANT* variant, int& value)
* @brief      Get param converted
* @ingroup    SCRIPT
* 
* @param[in]  variant : Variant pointer to use.
* @param[in]  value : Value value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_LIB::GetParamConverted(XVARIANT* variant, int& value)
{
  if(!variant) return false;

  value = 0;

  switch(variant->GetType())
    {
      case XVARIANT_TYPE_NULL           : break;

      case XVARIANT_TYPE_BOOLEAN        : value = (bool)(*variant)?1:0;
      
      case XVARIANT_TYPE_INTEGER        : value = (*variant);
                                          break;

      case XVARIANT_TYPE_FLOAT          : { float fvalue = (*variant);
                                            value = (int)fvalue;
                                          }
                                          break;

      case XVARIANT_TYPE_DOUBLE         : { double dvalue = (*variant);
                                            value = (int)dvalue;
                                          }
                                          break;

      case XVARIANT_TYPE_CHAR           :
      case XVARIANT_TYPE_XCHAR          :
      case XVARIANT_TYPE_STRING         :
      case XVARIANT_TYPE_DATE           :
      case XVARIANT_TYPE_TIME           :
      case XVARIANT_TYPE_DATETIME       : 
                            default     : return false;

    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_LIB::GetParamConverted(XVARIANT* variant, XDWORD& value)
* @brief      Get param converted
* @ingroup    SCRIPT
* 
* @param[in]  variant : Variant pointer to use.
* @param[in]  value : Value value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_LIB::GetParamConverted(XVARIANT* variant, XDWORD& value)
{
  int _value = 0;

  bool status = GetParamConverted(variant, _value);
  value = (XDWORD)_value;

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_LIB::GetParamConverted(XVARIANT* variant, float& value)
* @brief      Get param converted
* @ingroup    SCRIPT
* 
* @param[in]  variant : Variant pointer to use.
* @param[in]  value : Value value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_LIB::GetParamConverted(XVARIANT* variant, float& value)
{
  if(!variant) return false;

  value = 0.0f;

  switch(variant->GetType())
    {
      case XVARIANT_TYPE_NULL           : break;

      case XVARIANT_TYPE_INTEGER        : { int ivalue = (*variant);
                                            value = (float)ivalue;
                                          }
                                          break;

      case XVARIANT_TYPE_FLOAT          : value = (*variant);
                                          break;

      case XVARIANT_TYPE_DOUBLE         : { double dvalue = (*variant);
                                            value = (float)dvalue;
                                          }
                                          break;

      case XVARIANT_TYPE_CHAR           :
      case XVARIANT_TYPE_XCHAR          :
      case XVARIANT_TYPE_STRING         :
      case XVARIANT_TYPE_DATE           :
      case XVARIANT_TYPE_TIME           :
      case XVARIANT_TYPE_DATETIME       : 
                              default   : return false;

    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_LIB::GetParamConverted(XVARIANT* variant, double& value)
* @brief      Get param converted
* @ingroup    SCRIPT
* 
* @param[in]  variant : Variant pointer to use.
* @param[in]  value : Value value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_LIB::GetParamConverted(XVARIANT* variant, double& value)
{
  if(!variant) return false;

  value = 0.0f;

  switch(variant->GetType())
    {
      case XVARIANT_TYPE_NULL           :
      
      case XVARIANT_TYPE_INTEGER        : { int ivalue = (*variant);
                                            value = (double)ivalue;
                                          }
                                          break;

      case XVARIANT_TYPE_FLOAT          : { float fvalue = (*variant);
                                            value = (double)fvalue;
                                          }
                                          break;

      case XVARIANT_TYPE_DOUBLE         : value = (*variant);
                                          break;

      case XVARIANT_TYPE_CHAR           :
      case XVARIANT_TYPE_XCHAR          :
      case XVARIANT_TYPE_STRING         :
      case XVARIANT_TYPE_DATE           :
      case XVARIANT_TYPE_TIME           :
      case XVARIANT_TYPE_DATETIME       : 
                            default     : return false;

    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_LIB::GetParamConverted(XVARIANT* variant, XSTRING& value)
* @brief      Get param converted
* @ingroup    SCRIPT
* 
* @param[in]  variant : Variant pointer to use.
* @param[in]  value : Value value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_LIB::GetParamConverted(XVARIANT* variant, XSTRING& value)
{
  if(!variant) return false;

  value = __L("");

  switch(variant->GetType())
    {
      case XVARIANT_TYPE_NULL           :      
      case XVARIANT_TYPE_INTEGER        :                                           
      case XVARIANT_TYPE_FLOAT          : 
      case XVARIANT_TYPE_DOUBLE         : 
      case XVARIANT_TYPE_CHAR           :
      case XVARIANT_TYPE_XCHAR          : return false;

      case XVARIANT_TYPE_STRING         : { XSTRING string = (*variant);
                                            value = string.Get();
                                          }
                                          break; 

      case XVARIANT_TYPE_DATE           :
      case XVARIANT_TYPE_TIME           :
      case XVARIANT_TYPE_DATETIME       : 
                            default     : return false;

    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool SCRIPT_LIB::CheckParams(SCRIPT* script, XVECTOR<XVARIANT*>* params, XDWORD minimum)
* @brief      Validate the minimum number of non-null parameters required by a native script function
* @ingroup    SCRIPT
*
* @param[in]  script : Script receiving the validation error.
* @param[in]  params : Script parameters.
* @param[in]  minimum : Minimum number of required parameters.
*
* @return     bool : true if all required parameters are available; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_LIB::CheckParams(SCRIPT* script, XVECTOR<XVARIANT*>* params, XDWORD minimum)
{
  if(!script || !params) return false;

  if(params->GetSize() < minimum)
    {
      script->HaveError(SCRIPT_ERRORCODE_INSUF_PARAMS);
      return false;
    }

  for(XDWORD c=0; c<minimum; c++)
    {
      if(!params->Get(c))
        {
          script->HaveError(SCRIPT_ERRORCODE_INSUF_PARAMS);
          return false;
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         SCRIPT_LIB_FORMATSTATUS SCRIPT_LIB::FormatParams(XVECTOR<XVARIANT*>* params, XDWORD firstparamindex, XCHAR* mask, XSTRING& output)
* @brief      Safely format a script parameter list
* @ingroup    SCRIPT
*
* @param[in]  params : Script parameters.
* @param[in]  firstparamindex : First value used by the format mask.
* @param[in]  mask : Format mask.
* @param[out] output : Formatted output.
*
* @return     SCRIPT_LIB_FORMATSTATUS : Result of the operation.
*
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_LIB_FORMATSTATUS SCRIPT_LIB::FormatParams(XVECTOR<XVARIANT*>* params, XDWORD firstparamindex, XCHAR* mask, XSTRING& output)
{
  output.Empty();

  if(!params || !mask) return SCRIPT_LIB_FORMATSTATUS_INVALID_PARAM;
  if(firstparamindex > params->GetSize()) return SCRIPT_LIB_FORMATSTATUS_INSUFFICIENT_PARAMS;

  XDWORD paramindex = firstparamindex;
  int    index      = 0;

  while(mask[index])
    {
      if(mask[index] != __C('%'))
        {
          XSTRING character;

          character.Set(mask[index]);
          output += character;
          index++;

          if(output.GetSize() > SCRIPT_LIB_FORMAT_MAXOUTPUT) return SCRIPT_LIB_FORMATSTATUS_OUTPUT_TOO_LONG;
          continue;
        }

      index++;

      if(mask[index] == __C('%'))
        {
          output += __L("%");
          index++;

          if(output.GetSize() > SCRIPT_LIB_FORMAT_MAXOUTPUT) return SCRIPT_LIB_FORMATSTATUS_OUTPUT_TOO_LONG;
          continue;
        }

      XCHAR formattoken[SCRIPT_LIB_FORMAT_MAXTOKEN];
      int   ntoken      = 1;
      int   fieldvalue  = 0;

      memset(formattoken, 0, sizeof(formattoken));
      formattoken[0] = __C('%');

      while(mask[index])
        {
          XCHAR character = mask[index];
          bool  isdigit   = (character >= __C('0')) && (character <= __C('9'));
          bool  isflag    = (character == __C('-')) || (character == __C('+')) ||
                            (character == __C(' ')) || (character == __C('#')) ||
                            (character == __C('.'));

          if(!isdigit && !isflag) break;
          if(ntoken >= (SCRIPT_LIB_FORMAT_MAXTOKEN-2)) return SCRIPT_LIB_FORMATSTATUS_INVALID_FORMAT;

          if(isdigit)
            {
              fieldvalue = (fieldvalue * 10) + (character - __C('0'));
              if(fieldvalue > SCRIPT_LIB_FORMAT_MAXFIELD) return SCRIPT_LIB_FORMATSTATUS_INVALID_FORMAT;
            }
           else if(character == __C('.'))
            {
              fieldvalue = 0;
            }

          formattoken[ntoken++] = character;
          index++;
        }

      XCHAR conversion = mask[index];
      if(!conversion) return SCRIPT_LIB_FORMATSTATUS_INVALID_FORMAT;

      bool isinteger  = (conversion == __C('c')) || (conversion == __C('C')) ||
                        (conversion == __C('d')) || (conversion == __C('i'));
      bool isunsigned = (conversion == __C('o')) || (conversion == __C('u')) ||
                        (conversion == __C('x')) || (conversion == __C('X'));
      bool isreal     = (conversion == __C('f')) || (conversion == __C('F')) ||
                        (conversion == __C('g')) || (conversion == __C('G')) ||
                        (conversion == __C('e')) || (conversion == __C('E'));
      bool isstring   = (conversion == __C('s')) || (conversion == __C('S'));

      if(!isinteger && !isunsigned && !isreal && !isstring) return SCRIPT_LIB_FORMATSTATUS_INVALID_FORMAT;
      if(paramindex >= params->GetSize()) return SCRIPT_LIB_FORMATSTATUS_INSUFFICIENT_PARAMS;

      XVARIANT* variant = params->Get(paramindex);
      if(!variant) return SCRIPT_LIB_FORMATSTATUS_INVALID_PARAM;

      if(conversion == __C('C')) conversion = __C('c');
      if(conversion == __C('S')) conversion = __C('s');

      formattoken[ntoken++] = conversion;
      formattoken[ntoken]   = __C('\0');
      index++;
      paramindex++;

      XSTRING formatted;
      bool    status = false;

      if(isinteger)
        {
          int value = 0;

          if(variant->GetType() == XVARIANT_TYPE_CHAR)
            {
              value  = (char)(*variant);
              status = true;
            }
           else if(variant->GetType() == XVARIANT_TYPE_XCHAR)
            {
              value  = (XCHAR)(*variant);
              status = true;
            }
           else
            {
              status = GetParamConverted(variant, value);
            }

          if(status) formatted.Format(formattoken, value);
        }
       else if(isunsigned)
        {
          XDWORD value = 0;

          status = GetParamConverted(variant, value);
          if(status) formatted.Format(formattoken, value);
        }
       else if(isreal)
        {
          double value = 0.0;

          status = GetParamConverted(variant, value);
          if(status) formatted.Format(formattoken, value);
        }
       else
        {
          XSTRING value;

          status = GetParamConverted(variant, value);
          if(status) formatted.Format(formattoken, value.Get());
        }

      if(!status) return SCRIPT_LIB_FORMATSTATUS_INVALID_PARAM;

      output += formatted;
      if(output.GetSize() > SCRIPT_LIB_FORMAT_MAXOUTPUT) return SCRIPT_LIB_FORMATSTATUS_OUTPUT_TOO_LONG;
    }

  return SCRIPT_LIB_FORMATSTATUS_OK;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void SCRIPT_LIB::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    SCRIPT
* 
* --------------------------------------------------------------------------------------------------------------------*/
void SCRIPT_LIB::Clean()
{
  script = NULL;
}
