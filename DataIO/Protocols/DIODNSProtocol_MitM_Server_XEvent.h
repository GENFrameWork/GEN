/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOPing_XEvent.h
*
* @class      DIODNSPROTOCOL_MITM_SERVER_XEVENT
* @brief      Data Input/Output Domain Network System (DNS) protocol Man-in-the-Middle (MitM) Server eXtended Event class
* @ingroup    DATAIO
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

#ifndef _DIODNSPROTOCOL_MITM_SERVER_XEVENT_H_
#define _DIODNSPROTOCOL_MITM_SERVER_XEVENT_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XEvent.h"
#include "XSubject.h"
#include "XBuffer.h"

#include "DIOURL.h"
#include "DIOIP.h"
#include "DIODNSProtocol.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

enum DIODNSPROTOCOL_MITM_SERVER_XEVENT_TYPE
{
  DIODNSPROTOCOL_MITM_SERVER_XEVENT_TYPE_UNKNOWN                 = XEVENT_TYPE_DIODNS ,
  DIODNSPROTOCOL_MITM_SERVER_XEVENT_TYPE_ASKDNS                                       ,
  DIODNSPROTOCOL_MITM_SERVER_XEVENT_TYPE_ANSWERDNS                                       
};


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class DIOPINGREPLY;

class DIODNSPROTOCOL_MITM_SERVER_XEVENT : public XEVENT
{
  public:
                                  DIODNSPROTOCOL_MITM_SERVER_XEVENT             (XSUBJECT* subject, XDWORD type = DIODNSPROTOCOL_MITM_SERVER_XEVENT_TYPE_UNKNOWN, XDWORD family = XEVENT_TYPE_DIODNS);
    virtual                      ~DIODNSPROTOCOL_MITM_SERVER_XEVENT             ();

    DIOIP*                        GetOriginIP                                   ();
    DIOURL*                       GetAskedURL                                   ();
    XBUFFER*                      GetAskedBuffer                                ();
    
    XBUFFER*                      GetAnsweredBuffer                             ();
    DIOIP*                        GetAnsweredIP                                 ();

  private:

    void                          Clean                                         ();

    DIOIP                         origin_IP;
    DIOURL                        asked_URL;
    XBUFFER                       asked_buffer;
    XBUFFER                       answered_buffer;
    DIOIP                         answered_IP;

};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif

