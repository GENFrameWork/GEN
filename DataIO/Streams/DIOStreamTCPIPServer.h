/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTCPIPServer.h
* 
* @class      DIOSTREAMTCPIPSERVER
* @brief      Data Input/Output Stream TCP/IP Server class (Multi-Socket)
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

#ifndef _DIOSTREAMTCPIPSERVER_H_
#define _DIOSTREAMTCPIPSERVER_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XThreadCollected.h"

#include "DIOIP.h"
#include "DIOStream.h"
#include "DIOStreamTCPIP.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

#define DIOSTREAMTCPIPSERVER_MAXLISTEN                        1024
#define DIOSTREAMTCPIPSERVER_TIMEOUTENUMSERVERSIGNAL          3

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class DIOSTREAMTCPIPCONFIG;
class DIOSTREAMENUMSERVERS;


class DIOSTREAMTCPIPSERVER : public DIOSTREAM
{
  public:
                                            DIOSTREAMTCPIPSERVER      ();
    virtual                                ~DIOSTREAMTCPIPSERVER      ();

    virtual DIOSTREAMCONFIG*                GetConfig                 ();
    virtual bool                            SetConfig                 (DIOSTREAMCONFIG* config);

    virtual bool                            Open                      ()                                = 0;
    virtual bool                            Disconnect                ()                                = 0;
    virtual bool                            Close                     ()                                = 0;

    DIOIP*                                  GetClientIP               ();

    DIOSTREAMENUMSERVERS*                   GetEnumServers            ();
    bool                                    SetEnumServers            (DIOSTREAMENUMSERVERS* localenumservers);

    XVECTOR<DIOSTREAMTCPIP*>*               GetMultiSocketStreams     ();    

  protected:

    DIOSTREAMTCPIPCONFIG*                   config;
    DIOIP                                   clientIP;

    DIOSTREAMENUMSERVERS*                   enumservers;

    XVECTOR<DIOSTREAMTCPIP*>                multisocketstreams;    

  private:

    void                                    Clean                     ();       
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

