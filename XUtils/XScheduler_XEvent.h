/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XScheduler_XEvent.h
* 
* @class      XSCHEDULER_XEVENT
* @brief      eXtended Utils Scheduler Event class
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

#ifndef _XSCHEDULER_XEVENT_H_
#define _XSCHEDULER_XEVENT_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XEvent.h"
#include "XSubject.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

enum XSCHEDULER_XEVENT_TYPE
{
  XSCHEDULER_XEVENT_TYPE_UNKNOWN          = XEVENT_TYPE_SCHEDULER ,
};

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class XTIMER;
class XDATETIME;
class XPUBLISHER;
class XSCHEDULERTASK;
class XSCHEDULER;


class XSCHEDULER_XEVENT : public XEVENT
{
  public:
                                XSCHEDULER_XEVENT               (XSUBJECT* subject, XDWORD type = XSCHEDULER_XEVENT_TYPE_UNKNOWN, XDWORD family = XEVENT_TYPE_SCHEDULER);
    virtual                    ~XSCHEDULER_XEVENT               ();

    XSCHEDULER*                 GetScheduler                    ();
    void                        SetScheduler                    (XSCHEDULER* xscheduler);

    XSCHEDULERTASK*             GetTask                         ();
    void                        SetTask                         (XSCHEDULERTASK* xtask);

    XDATETIME*                  GetDateTime                     ();
    void                        SetDateTime                     (XDATETIME* xdatetime);

  private:

    void                        Clean                           ();

    XSCHEDULER*                 xscheduler;
    XSCHEDULERTASK*             xtask;
    XDATETIME*                  xdatetime;
    XTIMER*                     xtimer;
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

