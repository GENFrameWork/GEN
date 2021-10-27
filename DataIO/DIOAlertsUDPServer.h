/**-------------------------------------------------------------------------------------------------------------------
*
* @file        DIOAlertsUDPServer.h
*
* @class       DIOALERTSUDPSERVER
* @brief       Data IO Alerts UDP servers class
* @ingroup     DATAIO
*
* @author      Abraham J. Velez
* @date       01/03/2016 12:00
*
* @copyright   Copyright(c) 2008 - 2016 GEN Group.
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

#ifndef _DIOALERTSUDPSERVER_H_
#define _DIOALERTSUDPSERVER_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XVector.h"
#include "XString.h"
#include "XDateTime.h"
#include "XFactory.h"

#include "DIOFactory.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class DIOSTREAMUDPCONFIG;
class DIOSTREAMUDPACKNOWLEDGE;
class DIOALERT;

class DIOALERTSUDPSERVER
{
  public:
                                DIOALERTSUDPSERVER      ();
    virtual                    ~DIOALERTSUDPSERVER      ();

    bool                        Ini                     (XCHAR* URL, int port);
    bool                        End                     ();

    int                         GetNAlerts              ();

    DIOALERT*                   GetAlertByIndex         (int index);
    DIOALERT*                   GetAlertByID            (XDWORD ID);

    bool                        DeleteAlertByIndex      (int index);
    bool                        DeleteAlertByID         (XDWORD ID);

    bool                        DeleteAllAlerts         ();

  private:



    static void                 ThreadReadFunction      (void* param);
    bool                        ReceivedEvents          ();

    void                        Clean                   ();


    DIOSTREAMUDPCONFIG*         UDPdiostreamcfg;
    DIOSTREAMUDPACKNOWLEDGE*    UDPdiostream;

    XMUTEX*                     xmutexalert;
    XVECTOR<DIOALERT*>          alerts;

    XTHREADCOLLECTED*           threadread;
};



/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif


