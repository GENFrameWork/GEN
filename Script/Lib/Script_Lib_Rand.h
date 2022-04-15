/**-------------------------------------------------------------------------------------------------------------------
*
* @file       Script_Lib_Rand.h
*
* @class      SCRIPT_LIB_RAND
* @brief      Script Library Random
* @ingroup    SCRIPT
*
* @copyright  GEN Group. All right reserved.
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
*---------------------------------------------------------------------------------------------------------------------*/

#ifndef _SCRIPT_LIB_RAND_H_
#define _SCRIPT_LIB_RAND_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "Script_Lib.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define SCRIPT_LIB_NAME_RANDOM  __L("Random")

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class SCRIPT;
class XVARIANT;
class XRAND;

class SCRIPT_LIB_RAND : public SCRIPT_LIB
{
  public:
                          SCRIPT_LIB_RAND         ();
    virtual              ~SCRIPT_LIB_RAND         ();

    XRAND*                GetXRand                ();

    bool                  AddLibraryFunctions     (SCRIPT* script);

  private:

    void                  Clean                   ();

    XRAND*                xrand;
};


/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

void    Call_RandMax         (SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue);
void    Call_RandBetween     (SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue);

#endif

