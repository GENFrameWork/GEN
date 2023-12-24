/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOLINUXStreamICMP.h
* 
* @class      DIOLINUXSTREAMICMP
* @brief      LINUX Data Input/Output Stream UDP class
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

#ifndef _DIOLINUXSTREAMICMP_H_
#define _DIOLINUXSTREAMICMP_H_

#if defined(DIO_ACTIVE) && defined(DIO_STREAMICMP_ACTIVE)

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include <stdio.h>

#include "XFSMachine.h"

#include "DIOStreamICMP.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

enum DIOLINUXICMPFSMEVENTS
{
  DIOLINUXICMPFSMEVENT_NONE               = 0 ,

  DIOLINUXICMPFSMEVENT_GETTINGCONNECTION       ,
  DIOLINUXICMPFSMEVENT_CONNECTED              ,
  DIOLINUXICMPFSMEVENT_WAITINGTOREAD          ,
  DIOLINUXICMPFSMEVENT_SENDINGDATA            ,
  DIOLINUXICMPFSMEVENT_DISCONNECTING          ,

  DIOLINUXICMP_LASTEVENT

};


enum DIOLINUXICMPFSMSTATES
{
  DIOLINUXICMPFSMSTATE_NONE               = 0 ,

  DIOLINUXICMPFSMSTATE_GETTINGCONNECTION       ,
  DIOLINUXICMPFSMSTATE_CONNECTED              ,
  DIOLINUXICMPFSMSTATE_WAITINGTOREAD          ,
  DIOLINUXICMPFSMSTATE_SENDINGDATA            ,
  DIOLINUXICMPFSMSTATE_DISCONNECTING          ,

  DIOLINUXICMP_LASTSTATE
};

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class XTHREADCOLLECTED;

class DIOLINUXSTREAMICMP : public DIOSTREAMICMP , public XFSMACHINE
{
  public:
                              DIOLINUXSTREAMICMP                    ();
    virtual                  ~DIOLINUXSTREAMICMP                    ();

    bool                      Open                                  ();
    bool                      Disconnect                            ();
    bool                      Close                                 ();

  protected:

    int                       IsReadyConnect                        (int sock);

  private:
    
    static void               ThreadRunFunction                     (void* thread);

    void                      Clean                                 ();

    XTHREADCOLLECTED*         threadconnection;
    int                       handle;
    XSTRING                   remoteaddress;
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif


#endif

