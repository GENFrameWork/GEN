/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOLINUXStreamUDP.h
* 
* @class      DIOLINUXSTREAMUDP
* @brief      Data Input/Output Linux Stream UDP class
* @ingroup    PLATFORM_LINUX
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

#ifndef _DIOLINUXSTREAMUDP_H_
#define _DIOLINUXSTREAMUDP_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <stdio.h>

#include "XFSMachine.h"

#include "DIOStreamUDP.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

enum DIOLINUXUDPFSMEVENTS
{
  DIOLINUXUDPFSMEVENT_NONE              = 0 ,

  DIOLINUXUDPFSMEVENT_GETTINGCONNECTION      ,
  DIOLINUXUDPFSMEVENT_CONNECTED             ,
  DIOLINUXUDPFSMEVENT_WAITINGTOREAD         ,
  DIOLINUXUDPFSMEVENT_SENDINGDATA           ,
  DIOLINUXUDPFSMEVENT_DISCONNECTING         ,

  DIOLINUXUDP_LASTEVENT

};


enum DIOLINUXUDPFSMSTATES
{
  DIOLINUXUDPFSMSTATE_NONE              = 0 ,

  DIOLINUXUDPFSMSTATE_GETTINGCONNECTION      ,
  DIOLINUXUDPFSMSTATE_CONNECTED             ,
  DIOLINUXUDPFSMSTATE_WAITINGTOREAD         ,
  DIOLINUXUDPFSMSTATE_SENDINGDATA           ,
  DIOLINUXUDPFSMSTATE_DISCONNECTING         ,

  DIOLINUXUDP_LASTSTATE
};

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XTHREADCOLLECTED;

class DIOLINUXSTREAMUDP : public DIOSTREAMUDP , public XFSMACHINE
{
  public:
                              DIOLINUXSTREAMUDP                     ();
    virtual                  ~DIOLINUXSTREAMUDP                     ();

    bool                      Open                                  ();
    bool                      Disconnect                            ();
    bool                      Close                                 ();

  protected:

    int                       IsReadyConnect                        (int sock);

  private:

    void                      Clean                                 ();
    static void               ThreadRunFunction                     (void* thread);

    XTHREADCOLLECTED*         threadconnection;
    int                       handle;
    XSTRING                   remoteaddress;
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif



