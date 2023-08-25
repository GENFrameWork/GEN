/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOWINDOWSPCap.h
* 
* @class      DIOWINDOWSPCAP
* @brief      WINDOWS Interface PCap Library (Capture Ethernet Packets) class
* @ingroup    PLATFORM_WINDOWS
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
* * --------------------------------------------------------------------------------------------------------------------*/

#ifdef DIO_PCAP_ACTIVE

#ifndef _DIOWINDOWSPCAP_H_
#define _DIOWINDOWSPCAP_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <pcap.h>

#include "DIOPCap.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XFACTORY;
class XTHREADCOLLECTED;
class XTHREADCOLLECTED;


class  DIOWINDOWSPCAP : public DIOPCAP
{
  public:
                                  DIOWINDOWSPCAP                      ();
    virtual                      ~DIOWINDOWSPCAP                      ();

    bool                          Capture_Start                       (DIOPCAPNETINTERFACE* netinterface, bool promiscuousmode = true, int timeout = 1000);
    bool                          Capture_End                         ();

  private:

    void                          Clean                               ();

    bool                          CreateListNetInterfaces             ();

    static void                   ThreadCapture                       (void* data);
    static void                   PacketHandler                       (u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data);

    pcap_t*                       handle;
    XTHREAD*                      threadcapture;
};

/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif

#endif

