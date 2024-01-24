/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOWINDOWSPCap.h
* 
* @class      DIOWINDOWSPCAP
* @brief      WINDOWS Data Input/Output PCap Library class
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
* --------------------------------------------------------------------------------------------------------------------*/

#ifndef _DIOWINDOWSPCAP_H_
#define _DIOWINDOWSPCAP_H_

#ifdef DIO_PCAP_ACTIVE

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include <winsock2.h>
#include <windows.h>

#include <pcap.h>

#include "XThreadCollected.h"

#include "DIOPCap.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class DIOWINDOWSPCAP : public DIOPCAP
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
    XTHREADCOLLECTED*             threadcapture;
};

#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

#endif

