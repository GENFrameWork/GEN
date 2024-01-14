/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XTimer.h
* 
* @class      XTIMER
* @brief      eXtended Utils Timer class
* @ingroup    XUTILS
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

#ifndef _XTIMER_H_
#define _XTIMER_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XBase.h"

#include "XVector.h"
#include "XString.h"
#include "XPath.h"

#include "FactoryBase.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

#define XTIMER_INFINITE                   -1

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class XDATETIME;
class XFACTORY;
class XMUTEX;
class XTHREADCOLLECTED;


#ifdef XTRACE_ACTIVE_VIRTUALCLOCKTICK

class XTIMERCLOCK
{
  public:

                            XTIMERCLOCK                       ();
    virtual                ~XTIMERCLOCK                       ();

    XQWORD                  GetClockTicks                     ();

  private:

    void                    Clean                             ();

    static void             ThreadClockFunction               (void* data);

    XQWORD                  clockticks;

    XMUTEX*                 xmutexclock;
    XTHREAD*                threadclock;
};

#endif


class XTIMER : public FACTORYBASE
{
  public:

                            XTIMER                            ();
    virtual                ~XTIMER                            ();    

    void                    AddMilliSeconds                   (XQWORD milliseconds);
    void                    AddSeconds                        (XQWORD seconds);

    void                    SetMilliSeconds                   (XQWORD millisecods);

    void                    Reset                             ();

    XDWORD                  GetMeasureHours                   ();
    XDWORD                  GetMeasureMinutes                 ();
    XDWORD                  GetMeasureSeconds                 ();
    XQWORD                  GetMeasureMilliSeconds            ();
    XQWORD                  GetMeasureMicroSeconds            ();

    bool                    GetMeasureToDate                  (XDATETIME* xdatetime);

    bool                    GetMeasureString                  (XSTRING& measure, bool large = false);

    virtual XQWORD          GetMicroSecondsTickCounter        ();

  protected:

    XQWORD                  last;
    XQWORD                  more;

  private:

    void                    Clean                             ();
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES

#ifdef XTRACE_ACTIVE_VIRTUALCLOCKTICK
  extern XTIMERCLOCK*  xtimerclock;
#endif

#pragma endregion


#endif

