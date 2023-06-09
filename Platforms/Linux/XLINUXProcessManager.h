/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XLINUXProcessManager.h
*
* @class      XLINUXPROCESSMANAGER
* @brief      eXtended LINUX process management class
* @ingroup    PLATFORM_LINUX
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

#ifndef _XLINUXPROCESSMANAGER_H_
#define _XLINUXPROCESSMANAGER_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XProcessManager.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XLINUXPROCESSMANAGER : public XPROCESSMANAGER
{
  public:
                            XLINUXPROCESSMANAGER            ();
    virtual                ~XLINUXPROCESSMANAGER            ();


    bool                    MakeSystemCommand               (XCHAR* command);    
    bool                    MakeCommand                     (XCHAR* command, XSTRING* out = NULL, int* returncode = NULL);

    bool                    ExecuteApplication              (XCHAR* applicationpath, XCHAR* params = NULL, XSTRING* in = NULL, XSTRING* out = NULL, int* returncode = NULL);

    bool                    IsApplicationRunning            (XCHAR* command, XDWORD* ID = NULL);


  private:

    void                    Clean                       ();

};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif

