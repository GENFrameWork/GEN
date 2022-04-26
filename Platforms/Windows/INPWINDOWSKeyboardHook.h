/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       INPWINDOWSKeyboardHook.h
* 
* @class      INPWINDOWSKEYBOARDHOOK
* @brief      Input WINDOWS Keyboard Hook Class
* @ingroup    PLATFORM_WINDOWS
* 
* @copyright  Copyright(c) 2005 - 2022 GEN Group.
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
* *-------------------------------------------------------------------------------------------------------------------*/

#ifndef _INPWINDOWSKEYBOARDHOOK_H_
#define _INPWINDOWSKEYBOARDHOOK_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>

#include "XBase.h"
#include "XString.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class INPWINDOWSKEYBOARDHOOK
{
  public:
                               INPWINDOWSKEYBOARDHOOK      (void* applicationhandle);
    virtual                   ~INPWINDOWSKEYBOARDHOOK      ();

    bool                       Activate                    ();

    bool                       IsKBDStopped                ();
    bool                       SetKBDStopped               (bool stopped);

    bool                       Deactivate                  ();

  protected:

  private:

    void                       Clean                       ();

    static LRESULT CALLBACK    LowLevelKeyboardProc        (int ncode, WPARAM wparam, LPARAM lparam);

    void*                      applicationhandle;
    HHOOK                      keyhook;
    static bool                isstopkbd;
};

/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif