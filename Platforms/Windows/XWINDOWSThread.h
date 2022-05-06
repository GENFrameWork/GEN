/**-------------------------------------------------------------------------------------------------------------------
*
* @file        XWINDOWSThread.h
*
* @class       XWINDOWSTHREAD
* @brief       eXtended WINDOWS Thread class
* @ingroup     PLATFORM_WINDOWS
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

#ifndef _XWINDOWSTHREAD_H_
#define _XWINDOWSTHREAD_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <windows.h>

#include "XThread.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class XWINDOWSMUTEX : public XMUTEX
{
  public:
                            XWINDOWSMUTEX         ();
    virtual                ~XWINDOWSMUTEX         ();

    bool                    Lock                  ();
    bool                    UnLock                ();

  private:

    void                    Clean                 ();

    HANDLE                  mxhandle;
};



class XWINDOWSTHREAD : public XTHREAD
{
  public:
                            XWINDOWSTHREAD        (XTHREADGROUPID groupID, XCHAR* ID,XTHREADFUNCTION function = NULL,void* data = NULL);
     virtual               ~XWINDOWSTHREAD        ();

     bool                   Ini                   (bool run = true);
     bool                   Wait                  (int miliseconds =  XTHREAD_DEFAULTWAITYIELD);
     bool                   End                   ();

  private:

     static DWORD WINAPI    Callback              (LPVOID thread);

     void                   Clean                 ();

     HANDLE                 thhandle;
};



/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif


