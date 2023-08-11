/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       Script_Lib_System.h
* 
* @class      SCRIPT_LIB_SYSTEM
* @brief      Script Library System
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

#ifndef _SCRIPT_LIB_SYSTEM_H_
#define _SCRIPT_LIB_SYSTEM_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "Script_Lib.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

#define SCRIPT_LIB_SYSTEM_NAME  __L("System")

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class XVARIANT;
class SCRIPT;

class SCRIPT_LIB_SYSTEM : public SCRIPT_LIB
{
  public:
                          SCRIPT_LIB_SYSTEM         ();
    virtual              ~SCRIPT_LIB_SYSTEM         ();

    bool                  AddLibraryFunctions       (SCRIPT* script);

  private:

    void                  Clean                     ();
};

#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES

void    Call_SystemGetType                  (SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue);
void    Call_SystemReboot                   (SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue);
void    Call_SystemPowerOff                 (SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue);
void    Call_SystemLogout                   (SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue);

void    Call_GetEnviromentVar               (SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue);

#pragma endregion


#endif
