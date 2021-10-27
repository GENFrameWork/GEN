/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XWiNDOWSTrace.h
*
* @class      XWINDOWSTRACE
* @brief      eXtended WINDOWS debug trace class
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @copyright  Copyright(c) 2008 - 2016 GEN Group.
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

#ifndef _XWINDOWSDEBUGTRACE_H_
#define _XWINDOWSDEBUGTRACE_H_

#ifdef XTRACE_ACTIVE

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XBase.h"
#include "XString.h"
#include "DIOURL.h"

#include "XWINDOWSFactory.h"
#include "XWINDOWSDateTime.h"

#include "XTrace.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class XWINDOWSTRACE : public XTRACE
{
  public:
                        XWINDOWSTRACE      ();
    virtual            ~XWINDOWSTRACE      ();

    void                PrintSpecial            (XTRACE_TARGET* target, XBYTE level, XCHAR* string);
    void                PrintFile               (XTRACE_TARGET* target, XBYTE level, XCHAR* string);
    void                PrintNet                (XTRACE_TARGET* target, XBYTE level, XCHAR* string);

    bool                GetHandleNet            (XTRACE_TARGET* target);
    bool                CloseHandleNet          (XTRACE_TARGET* target);

  private:

    bool                Lock                    ();
    bool                UnLock                  ();

    void                Clean                   ();


    XWINDOWSDATETIME    xdatetime;
    XQWORD              mutexhandle;
};


/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/


#endif

#endif


