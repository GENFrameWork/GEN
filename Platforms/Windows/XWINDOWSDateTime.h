/**-------------------------------------------------------------------------------------------------------------------
*
* @file        XWINDOWSDateTime.h
*
* @class       XWINDOWSDATETIME
* @brief       eXtended WINDOWS Data Time class
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

#ifndef _XWINDOWSDATETIME_H_
#define _XWINDOWSDATETIME_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <windows.h>
#include <time.h>

#include "XBase.h"
#include "XDateTime.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XWINDOWSDATETIME : public XDATETIME
{
  public:
                                XWINDOWSDATETIME                  ();
    virtual                    ~XWINDOWSDATETIME                  ();

    bool                        Read                              (bool islocal = true);
    bool                        Write                             (bool islocal = true);

    bool                        GetFileDateTime                   (XPATH& xpath, void* tmzip, XDWORD* dt);
    bool                        GetFileDateTime                   (XPATH& xpath);

    int                         GetMeridianDifference             ();

    bool                        IsDayLigthSavingTime              (int* bias = NULL);

  protected:

    void                        GetActualDateTime                 (XWINDOWSDATETIME* time, bool islocal = true);
    void                        SetActualDateTime                 (XWINDOWSDATETIME* time, bool islocal = true);
};



/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif


