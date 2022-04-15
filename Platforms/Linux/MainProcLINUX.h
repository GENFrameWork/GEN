/**-------------------------------------------------------------------------------------------------------------------
*
* @file       MainProcLINUX.h
*
* @class      MAINPROCLINUX
* @brief      Main procedure LINUX class
* @ingroup    PLATFORM_LINUX
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

#ifndef _MAINPROCLINUX_H_
#define _MAINPROCLINUX_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#ifdef APP_ACTIVE
#include "APPBase.h"
#include "APPMain.h"
#endif

#include "MainProc.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class MAINPROCLINUX : public MAINPROC
{
  public:
                                  MAINPROCLINUX         ();
    virtual                      ~MAINPROCLINUX         ();

    bool                          Ini                   (APPMAIN* appmain = NULL, APPBASE_APPLICATIONMODE_TYPE applicationmode = APPBASE_APPLICATIONMODE_TYPE_UNKNOWN);
    bool                          Update                ();
    bool                          End                   ();

    bool                          Factorys_Ini          ();
    bool                          Factorys_End          ();

  private:

    void                          Clean                 ();
};


/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/


#endif


