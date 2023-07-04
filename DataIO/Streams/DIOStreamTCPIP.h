/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamTCPIP.h
*
* @class      DIOSTREAMTCPIP
* @brief      Data Input/Output Stream TCP/IP class
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

#ifndef _DIOSTREAMTCPIP_H_
#define _DIOSTREAMTCPIP_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DIOIP.h"
#include "DIOStream.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define DIOSTREAMTCPIP_MAXLISTEN                        1024
#define DIOSTREAMTCPIP_TIMEOUTENUMSERVERSIGNAL          3

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XBUFFER;
class XFACTORY;
class XTIMER;
class XTHREADCOLLECTED;
class DIOSTREAMTCPIPCONFIG;
class DIOSTREAMENUMSERVERS;
class DIOSTREAMUDP;


class DIOSTREAMTCPIP : public DIOSTREAM
{
  public:
                                            DIOSTREAMTCPIP            ();
    virtual                                ~DIOSTREAMTCPIP            ();

    virtual DIOSTREAMCONFIG*                GetConfig                 ();
    virtual bool                            SetConfig                 (DIOSTREAMCONFIG* config);

    virtual bool                            Open                      ()                                = 0;
    virtual bool                            Disconnect                ()                                = 0;
    virtual bool                            Close                     ()                                = 0;

    DIOIP*                                  GetClientIP               ();

    DIOSTREAMENUMSERVERS*                   GetEnumServers            ();
    bool                                    SetEnumServers            (DIOSTREAMENUMSERVERS* localenumservers);

  protected:

    DIOSTREAMTCPIPCONFIG*                   config;
    DIOIP                                   clientIP;

    DIOSTREAMENUMSERVERS*                   enumservers;

  private:

    void                                    Clean                     ();
};



/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif


