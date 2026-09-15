/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       Script_Lib.h
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

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XString.h"
#include "XVector.h"



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define SCRIPT_LIB_FORMAT_MAXTOKEN              32
#define SCRIPT_LIB_FORMAT_MAXFIELD              1024
#define SCRIPT_LIB_FORMAT_MAXOUTPUT             65536

enum SCRIPT_LIB_FORMATSTATUS
{
  SCRIPT_LIB_FORMATSTATUS_OK                 ,
  SCRIPT_LIB_FORMATSTATUS_INVALID_FORMAT     ,
  SCRIPT_LIB_FORMATSTATUS_INSUFFICIENT_PARAMS,
  SCRIPT_LIB_FORMATSTATUS_INVALID_PARAM      ,
  SCRIPT_LIB_FORMATSTATUS_OUTPUT_TOO_LONG
};



/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XVARIANT;
class SCRIPT;

class SCRIPT_LIB
{
  public:
                          SCRIPT_LIB              (XCHAR* ID);
    virtual              ~SCRIPT_LIB              ();

    XSTRING*              GetID                   ();

    virtual bool          AddLibraryFunctions     (SCRIPT* script);

    bool                  GetParamConverted       (XVARIANT* variant, bool& value);
    bool                  GetParamConverted       (XVARIANT* variant, int& value);
    bool                  GetParamConverted       (XVARIANT* variant, XDWORD& value);
    bool                  GetParamConverted       (XVARIANT* variant, float& value);
    bool                  GetParamConverted       (XVARIANT* variant, double& value);
    bool                  GetParamConverted       (XVARIANT* variant, XSTRING& value);
    bool                  CheckParams             (SCRIPT* script, XVECTOR<XVARIANT*>* params, XDWORD minimum);

    SCRIPT_LIB_FORMATSTATUS FormatParams          (XVECTOR<XVARIANT*>* params, XDWORD firstparamindex, XCHAR* mask, XSTRING& output);

  protected:

    SCRIPT*               script;

  private:

    void                  Clean                   ();

    XSTRING               ID;
};



/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/


