/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       APPCheckResourcesHardware_XEvent.h
* 
* @class      APPCHECKRESOURCESHARDWARE_XEVENT
* @brief      Application Check Resources Hardware eXtended Event class
* @ingroup    APPLICATION
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

#ifndef _APPCHECKRESOURCESHARDWARE_XEVENT_H_
#define _APPCHECKRESOURCESHARDWARE_XEVENT_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XEvent.h"
#include "XSubject.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

enum APPCHECKRESOURCESHARDWARE_XEVENT_TYPE
{
  APPCHECKRESOURCESHARDWARE_XEVENT_TYPE_UNKNOWN              = XEVENT_TYPE_APPLICATIONCHECKRESOURCESHW  ,
  APPCHECKRESOURCESHARDWARE_XEVENT_TYPE_MEMFREELIMIT                                                    ,
  APPCHECKRESOURCESHARDWARE_XEVENT_TYPE_TOTALCPUUSAGELIMIT                                              ,
  APPCHECKRESOURCESHARDWARE_XEVENT_TYPE_APPCPUUSAGELIMIT                                       
};

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class XTIMER;
class XPUBLISHER;

class APPCHECKRESOURCESHARDWARE_XEVENT : public XEVENT
{
  public:
                                APPCHECKRESOURCESHARDWARE_XEVENT        (XSUBJECT* subject, XDWORD type = APPCHECKRESOURCESHARDWARE_XEVENT_TYPE_UNKNOWN, XDWORD family = XEVENT_TYPE_APPLICATIONCHECKRESOURCESHW);
    virtual                    ~APPCHECKRESOURCESHARDWARE_XEVENT        ();

    bool                        GetActualMemFree                        (XDWORD& memfree_inbytes, XBYTE& memfree_percent);
    void                        SetActualMemFree                        (XDWORD memfree_inbytes, XBYTE memfree_percent);

    int                         GetActualTotalCPUUsage                  ();
    void                        SetActualTotalCPUUsage                  (int& CPUusage);

    int                         GetActualAppCPUUsage                    ();
    void                        SetActualAppCPUUsage                    (int& CPUusage);

  private:

    void                        Clean                                   ();

    XDWORD                      memfree_inbytes;
    XBYTE                       memfree_percent;

    int                         totalCPUusage;    
    int                         appCPUusage;    
};

#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif
