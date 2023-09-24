//------------------------------------------------------------------------------------------
//  DIOLINUXSTREAMICMP.H
//
/**
// \class
//
//  LINUX Data Input/Output Stream UDP class
//
//  ""
//  @version 02/01/2002
*/
//  GEN  Copyright (C).  All right reserved.
//------------------------------------------------------------------------------------------

#ifndef _DIOLINUXSTREAMICMP_H_
#define _DIOLINUXSTREAMICMP_H_

#if defined(DIO_ACTIVE) && defined(DIO_STREAMICMP_ACTIVE)

//---- INCLUDES ----------------------------------------------------------------------------

#include <stdio.h>

#include "XFSMachine.h"

#include "DIOStreamICMP.h"

//---- DEFINES & ENUMS  --------------------------------------------------------------------


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


//---- CLASS -------------------------------------------------------------------------------

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

    void                      Clean                                 ();
    static void               ThreadRunFunction                     (void* thread);

    XTHREADCOLLECTED*         threadconnection;
    int                       handle;
    XSTRING                   remoteaddress;
};



//---- INLINE FUNCTIONS --------------------------------------------------------------------

#endif

#endif

