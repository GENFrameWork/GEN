/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       Script_Lib_CFG.h
* 
* @class      SCRIPT_LIB_CFG
* @brief      Script Library CFG
* @ingroup    SCRIPT
* 
* @copyright  GEN Group. All rights reserved.
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

#ifndef _SCRIPT_LIB_CFG_H_
#define _SCRIPT_LIB_CFG_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "Script_Lib.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

#define SCRIPT_LIB_NAME_CFG   __L("CFG")


#define SCRIPT_SET_LIB_CFG(script, config)        { SCRIPT_LIB_CFG* lib = (SCRIPT_LIB_CFG*)script->GetLibrary(SCRIPT_LIB_NAME_CFG); \
                                                    if(lib) \
                                                      { \
                                                        lib->SetXFileCFG(&config); \
                                                      } \
                                                  }

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class XVARIANT;
class XFILECFG;
class SCRIPT;

class SCRIPT_LIB_CFG : public SCRIPT_LIB
{
  public:
                          SCRIPT_LIB_CFG          ();
    virtual              ~SCRIPT_LIB_CFG          ();

    bool                  AddLibraryFunctions     (SCRIPT* script);

    XFILECFG*             GetXFileCFG             ();
    void                  SetXFileCFG             (XFILECFG* xfileCFG);

  private:

    void                  Clean                   ();

    XFILECFG*             xfileCFG;
};

#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES

void    Call_GetFileCFGValue          (SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue);

#pragma endregion


#endif
