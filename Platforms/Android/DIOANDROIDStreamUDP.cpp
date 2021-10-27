//------------------------------------------------------------------------------------------
//  DIOANDROIDSTREAMUDP.CPP
//
//  ANDROID Data IO Stream UDP class
//
//  Author            : Abraham J. Velez
//  Date Of Creation  : 02/01/2002
//  Last Mofificacion :
//
//  GEN  Copyright (C).  All right reserved.
//------------------------------------------------------------------------------------------



/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"

//---- INCLUDES ----------------------------------------------------------------------------

#include "GEN_Defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <malloc.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <linux/if_ether.h>

#include "XFactory.h"
#include "XThreadCollected.h"
#include "XBuffer.h"
#include "XTrace.h"

#include "DIOIP.h"
#include "DIOURL.h"
#include "DIOStreamXEvent.h"
#include "DIOStreamUDP.h"
#include "DIOStreamUDPConfig.h"

#include "DIOANDROIDStreamUDP.h"

#include "XMemory_Control.h"


//---- GENERAL VARIABLE --------------------------------------------------------------------


//---- CLASS MEMBERS -----------------------------------------------------------------------


/*-------------------------------------------------------------------
//  DIOANDROIDSTREAMUDP::DIOANDROIDSTREAMUDP
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      18/02/2013 23:10:30
//
//  @return



*/
/*-----------------------------------------------------------------*/
DIOANDROIDSTREAMUDP::DIOANDROIDSTREAMUDP() : DIOSTREAMUDP() , XFSMACHINE(0)
{
  Clean();

  AddState( DIOANDROIDUDPFSMSTATE_NONE                ,
            DIOANDROIDUDPFSMEVENT_GETTINGCONNECTION    , DIOANDROIDUDPFSMSTATE_GETTINGCONNECTION,
            DIOANDROIDUDPFSMEVENT_CONNECTED           , DIOANDROIDUDPFSMSTATE_CONNECTED        ,
            DIOANDROIDUDPFSMEVENT_DISCONNECTING       , DIOANDROIDUDPFSMSTATE_DISCONNECTING    ,
            XFSMACHINESTATE_EVENTDEFEND);

  AddState( DIOANDROIDUDPFSMSTATE_GETTINGCONNECTION    ,
            DIOANDROIDUDPFSMEVENT_CONNECTED           , DIOANDROIDUDPFSMSTATE_CONNECTED        ,
            DIOANDROIDUDPFSMEVENT_WAITINGTOREAD       , DIOANDROIDUDPFSMSTATE_WAITINGTOREAD    ,
            DIOANDROIDUDPFSMEVENT_SENDINGDATA         , DIOANDROIDUDPFSMSTATE_SENDINGDATA      ,
            DIOANDROIDUDPFSMEVENT_DISCONNECTING       , DIOANDROIDUDPFSMSTATE_DISCONNECTING    ,
            XFSMACHINESTATE_EVENTDEFEND);

  AddState( DIOANDROIDUDPFSMSTATE_CONNECTED           ,
            DIOANDROIDUDPFSMEVENT_GETTINGCONNECTION    , DIOANDROIDUDPFSMSTATE_GETTINGCONNECTION,
            DIOANDROIDUDPFSMEVENT_WAITINGTOREAD       , DIOANDROIDUDPFSMSTATE_WAITINGTOREAD    ,
            DIOANDROIDUDPFSMEVENT_SENDINGDATA         , DIOANDROIDUDPFSMSTATE_SENDINGDATA      ,
            DIOANDROIDUDPFSMEVENT_DISCONNECTING       , DIOANDROIDUDPFSMSTATE_DISCONNECTING    ,
            XFSMACHINESTATE_EVENTDEFEND);

  AddState( DIOANDROIDUDPFSMSTATE_WAITINGTOREAD       ,
            DIOANDROIDUDPFSMEVENT_GETTINGCONNECTION    , DIOANDROIDUDPFSMSTATE_GETTINGCONNECTION,
            DIOANDROIDUDPFSMEVENT_CONNECTED           , DIOANDROIDUDPFSMSTATE_CONNECTED        ,
            DIOANDROIDUDPFSMEVENT_SENDINGDATA         , DIOANDROIDUDPFSMSTATE_SENDINGDATA      ,
            DIOANDROIDUDPFSMEVENT_DISCONNECTING       , DIOANDROIDUDPFSMSTATE_DISCONNECTING    ,
            XFSMACHINESTATE_EVENTDEFEND);

  AddState( DIOANDROIDUDPFSMSTATE_DISCONNECTING       ,
            DIOANDROIDUDPFSMEVENT_GETTINGCONNECTION    , DIOANDROIDUDPFSMSTATE_GETTINGCONNECTION,
            DIOANDROIDUDPFSMEVENT_CONNECTED           , DIOANDROIDUDPFSMSTATE_CONNECTED        ,
            DIOANDROIDUDPFSMEVENT_WAITINGTOREAD       , DIOANDROIDUDPFSMSTATE_WAITINGTOREAD    ,
            DIOANDROIDUDPFSMEVENT_SENDINGDATA         , DIOANDROIDUDPFSMSTATE_SENDINGDATA      ,
            XFSMACHINESTATE_EVENTDEFEND);

  threadconnection = CREATEXTHREAD(XTHREADGROUPID_DIOSTREAMUDP, __L("DIOANDROIDSTREAMUDP::DIOANDROIDSTREAMUDP"), ThreadRunFunction, (void*)this);
}


//-------------------------------------------------------------------
//  DIOANDROIDSTREAMUDP::~DIOANDROIDSTREAMUDP
/**
//
//
//  @author      Abraham J. Velez
//  @version      03/09/2001 16:58:17
//
//  @return
*/
//-------------------------------------------------------------------
DIOANDROIDSTREAMUDP::~DIOANDROIDSTREAMUDP()
{
  if(threadconnection)
    {
      DELETEXTHREAD(XTHREADGROUPID_DIOSTREAMUDP, threadconnection);
    }

  Clean();
}



//-------------------------------------------------------------------
//  DIOANDROIDSTREAMUDP::Open
/**
//
//
//  @author      Abraham J. Velez
//  @version      03/09/2001 16:58:17
//
//  @return       bool :
*/
//-------------------------------------------------------------------
bool DIOANDROIDSTREAMUDP::Open()
{
  if(!threadconnection)  return false;

  if(!inbuffer)         return false;
  if(!outbuffer)        return false;

  SetEvent(DIOANDROIDUDPFSMEVENT_GETTINGCONNECTION);

  status = DIOSTREAMSTATUS_GETTINGCONNECTION;

  ResetXBuffers();

  ResetConnectionStatistics();

  return threadconnection->Ini();
}


/*-------------------------------------------------------------------
//  DIOANDROIDSTREAMUDP::Disconnect
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      01/12/2010 23:10:56
//
//  @return       bool :
//  */
/*-----------------------------------------------------------------*/
bool DIOANDROIDSTREAMUDP::Disconnect()
{
  if((GetConnectStatus()==DIOSTREAMSTATUS_GETTINGCONNECTION)||
     (GetConnectStatus()==DIOSTREAMSTATUS_CONNECTED))
     {
       SetEvent(DIOANDROIDUDPFSMEVENT_DISCONNECTING);

       while(GetConnectStatus()!=DIOSTREAMSTATUS_DISCONNECTED)
        {
          usleep(10000);
        }
     }

  return true;
}


//-------------------------------------------------------------------
//  DIOANDROIDSTREAMUDP::Close
/**
//
//
//  @author      Abraham J. Velez
//  @version      03/09/2001 16:58:17
//
//  @return
*/
//-------------------------------------------------------------------
bool DIOANDROIDSTREAMUDP::Close()
{
  if(!threadconnection) return false;

  threadconnection->End();

  if(handle>=0)
    {
      shutdown(handle,SHUT_RDWR);
      close(handle);
      handle  = -1;
    }

  return true;
}



//-------------------------------------------------------------------
//  DIOANDROIDSTREAMUDP::IsReadyConnect
/**
//
//
//  @author      Abraham J. Velez
//  @version      08/03/2006 15:36:59
//
//  @return       int :
//  @param        sock :
*/
//-------------------------------------------------------------------
int DIOANDROIDSTREAMUDP::IsReadyConnect(int socket)
{
  struct timeval  tv;
  int             rc;
  fd_set          fdr;
  fd_set          fdw;
  fd_set          fds;

  if(socket==-1) return -1;

  FD_ZERO(&fdr);
  FD_ZERO(&fdw);
  FD_ZERO(&fds);

  FD_SET(socket,&fdr);
  FD_SET(socket,&fdw);
  FD_SET(socket,&fds);

  tv.tv_sec  = 0;
  tv.tv_usec = 100;

  rc = select(socket+1, &fdr, &fdw, &fds, &tv);
  if(rc==-1) return -1;

  int status1 = FD_ISSET(socket,&fdr) ? 1 : 0;
  int status2 = FD_ISSET(socket,&fdw) ? 1 : 0;
  int status3 = FD_ISSET(socket,&fds) ? 1 : 0;

  if(config->IsServer())
    {
      if(status1 || status2 ) return  1;
      if(status3)             return -1;
    }
   else
    {
      if((!status1) && status2) return  1;
      if(status3)               return -1;
    }

  return 0;
}



//-------------------------------------------------------------------
//  DIOANDROIDSTREAMUDP::Clean
/**
//
//
//  @author      Abraham J. Velez
//  @version      03/09/2001 16:58:17
//
//  @return
*/
//-------------------------------------------------------------------
void DIOANDROIDSTREAMUDP::Clean()
{
  threadconnection   = NULL;
  status            = DIOSTREAMSTATUS_DISCONNECTED;
  handle            = -1;
}



//-------------------------------------------------------------------
//  DIOANDROIDSTREAMUDP::ThreadRunFunction
/**
//
//
//  @author      Abraham J. Velez
//  @version      06/03/2006 15:44:00
//
//  @return       void :
//  @param        data :
*/
//-------------------------------------------------------------------
void DIOANDROIDSTREAMUDP::ThreadRunFunction(void* thread)
{
  DIOANDROIDSTREAMUDP* diostream = (DIOANDROIDSTREAMUDP*)thread;
  if(!diostream) return;

  if(diostream->GetEvent()==DIOANDROIDUDPFSMEVENT_NONE)
    {
      switch(diostream->GetCurrentState())
        {
          case DIOANDROIDUDPFSMSTATE_NONE               : break;

          case DIOANDROIDUDPFSMSTATE_GETTINGCONNECTION   : switch(diostream->IsReadyConnect(diostream->handle))
                                                          {
                                                            case -1:  diostream->SetEvent(DIOANDROIDUDPFSMEVENT_DISCONNECTING);
                                                                      break;

                                                            case  1:  diostream->SetEvent(DIOANDROIDUDPFSMEVENT_CONNECTED);
                                                                      diostream->status = DIOSTREAMSTATUS_CONNECTED;
                                                                      break;

                                                            default:  diostream->status = DIOSTREAMSTATUS_GETTINGCONNECTION;
                                                                      break;
                                                          }
                                                        break;

          case DIOANDROIDUDPFSMSTATE_CONNECTED          : break;

          case DIOANDROIDUDPFSMSTATE_WAITINGTOREAD        : { struct timeval  waitd;
                                                            fd_set          read_flags;
                                                            fd_set          write_flags;

                                                            if(diostream->handle == -1)
                                                              {
                                                                diostream->SetEvent(DIOANDROIDUDPFSMEVENT_DISCONNECTING);
                                                                break;
                                                              }

                                                            waitd.tv_sec  = 0;
                                                            waitd.tv_usec = 100;

                                                            FD_ZERO(&read_flags);
                                                            FD_ZERO(&write_flags);

                                                            if(!diostream->IsBlockRead()) FD_SET(diostream->handle, &read_flags);
                                                            if(!diostream->IsBlockWrite())
                                                              {
                                                                if(diostream->config->IsUsedDatagrams())
                                                                  {
                                                                    if(diostream->GetFirstDatagram(true) != DIOSTREAMUDP_NOTFOUND) FD_SET(diostream->handle, &write_flags);
                                                                  }
                                                                 else
                                                                  {
                                                                    if(diostream->outbuffer->GetSize()) FD_SET(diostream->handle, &write_flags);
                                                                  }
                                                              }

                                                            int error = select((int)(diostream->handle)+1, &read_flags, &write_flags, (fd_set*)0, &waitd);
                                                            if(error == -1)
                                                              {
                                                                diostream->SetEvent(DIOANDROIDUDPFSMEVENT_DISCONNECTING);
                                                                break;
                                                              }

                                                            if(FD_ISSET(diostream->handle, &read_flags))  //Socket ready for reading
                                                              {
                                                                XBYTE buffer[DIOSTREAM_MAXBUFFER];

                                                                FD_CLR(diostream->handle, &read_flags);

                                                                struct sockaddr_in  origin_addr;
                                                                socklen_t           size_addr = sizeof(sockaddr_in);

                                                                memset(&origin_addr, 0, size_addr);

                                                                int size = recvfrom(diostream->handle, (char*)buffer, DIOSTREAM_MAXBUFFER, 0,(sockaddr*)&origin_addr, &size_addr);

                                                                if(size < 0)
                                                                  {
                                                                    diostream->SetEvent(DIOANDROIDUDPFSMEVENT_DISCONNECTING);
                                                                    break;
                                                                  }

                                                                if(size)
                                                                  {
                                                                    XSTRING address;
                                                                    XWORD   port;

                                                                    int _address[4] = { 0, 0, 0, 0 };

                                                                    sscanf(inet_ntoa(origin_addr.sin_addr),"%d.%d.%d.%d"  ,&_address[0]
                                                                                                                          ,&_address[1]
                                                                                                                          ,&_address[2]
                                                                                                                          ,&_address[3]);

                                                                    address.Format(__L("%d.%d.%d.%d") , _address[0]
                                                                                                      , _address[1]
                                                                                                      , _address[2]
                                                                                                      , _address[3]);

                                                                    port =  ntohs(origin_addr.sin_port);

                                                                    //XTRACE_PRINTCOLOR(1, __L("Read UDP from [%s] (%d)"), address.Get(), size);

                                                                    if(diostream->config->IsUsedDatagrams())
                                                                            diostream->AddDatagram(false, address.Get(), port, (XBYTE*)buffer, size);
                                                                       else diostream->inbuffer->Add(buffer, size);
                                                                  }
                                                                 else
                                                                  {
                                                                    diostream->SetEvent(DIOANDROIDUDPFSMEVENT_DISCONNECTING);
                                                                    break;
                                                                  }
                                                              }


                                                            if(FD_ISSET(diostream->handle, &write_flags))  //Socket ready for writing
                                                              {
                                                                FD_CLR(diostream->handle, &write_flags);

                                                                if(diostream->config->IsUsedDatagrams())
                                                                  {
                                                                    int indexdatagram  = diostream->GetFirstDatagram(true);
                                                                    if(indexdatagram != DIOSTREAMUDP_NOTFOUND)
                                                                      {
                                                                        DIOSTREAMUDPDATAGRAM* datagram = (DIOSTREAMUDPDATAGRAM*)diostream->GetDatagram(indexdatagram);
                                                                        if(datagram)
                                                                          {
                                                                            struct sockaddr_in  target_addr;
                                                                            socklen_t           size_addr = sizeof(sockaddr_in);
                                                                            int                 size;
                                                                            XSTRING             tmpremoteaddress;

                                                                            memset(&target_addr, 0, size_addr);

                                                                            target_addr.sin_family = AF_INET;

                                                                            if(diostream->config->IsBroadcastModeActive())
                                                                              {
                                                                                target_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
                                                                              }
                                                                             else
                                                                              {
                                                                                if(datagram->GetAddress()->IsEmpty())
                                                                                        tmpremoteaddress = diostream->remoteaddress.Get();
                                                                                  else  tmpremoteaddress = datagram->GetAddress()->Get();

                                                                                XSTRING_CREATEOEM(tmpremoteaddress, charstr)
                                                                                target_addr.sin_addr.s_addr   = inet_addr(charstr);
                                                                                XSTRING_DELETEOEM(tmpremoteaddress, charstr)
                                                                              }

                                                                            target_addr.sin_port  = htons(datagram->GetPort()?datagram->GetPort():diostream->config->GetRemotePort());

                                                                            size = sendto(diostream->handle,(char*)datagram->GetData()->Get(), datagram->GetData()->GetSize(), 0, (sockaddr*)&target_addr, size_addr);

                                                                            //XTRACE_PRINTCOLOR(1, __L("Write UDP to [%s] (%d)"), tmpremoteaddress.Get(), size);

                                                                            if(size == -1)
                                                                              {
                                                                                diostream->SetEvent(DIOANDROIDUDPFSMEVENT_DISCONNECTING);
                                                                                break;
                                                                              }

                                                                            if(size)
                                                                              {
                                                                                if(diostream->config->IsUsedDatagrams())
                                                                                     diostream->DeleteDatagram(indexdatagram);
                                                                                else diostream->outbuffer->Extract(NULL, 0 , datagram->GetData()->GetSize());
                                                                              }
                                                                          }
                                                                      }
                                                                  }
                                                                 else
                                                                  {
                                                                    int esize = diostream->outbuffer->GetSize();
                                                                    if(esize>DIOSTREAM_MAXBUFFER) esize = DIOSTREAM_MAXBUFFER;

                                                                    if(esize)
                                                                      {
                                                                        XBYTE buffer[DIOSTREAM_MAXBUFFER];

                                                                        struct sockaddr_in  target_addr;
                                                                        socklen_t           size_addr = sizeof(sockaddr_in);
                                                                        int                 size;
                                                                        XSTRING             tmpremoteaddress;

                                                                        memset(&target_addr, 0, size_addr);

                                                                        target_addr.sin_family = AF_INET;

                                                                        if(diostream->config->IsBroadcastModeActive())
                                                                          {
                                                                            target_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
                                                                          }
                                                                         else
                                                                          {
                                                                            tmpremoteaddress = diostream->remoteaddress.Get();
                                                                            XSTRING_CREATEOEM(tmpremoteaddress, charstr)
                                                                            target_addr.sin_addr.s_addr   = inet_addr(charstr);
                                                                            XSTRING_DELETEOEM(tmpremoteaddress, charstr)
                                                                          }

                                                                        target_addr.sin_port  = htons(diostream->config->GetRemotePort());

                                                                        diostream->outbuffer->SetBlocked(true);
                                                                        size = sendto(diostream->handle,(char*)diostream->outbuffer->Get(), esize, 0, (sockaddr*)&target_addr, size_addr);
                                                                        diostream->outbuffer->SetBlocked(false);

                                                                        if(size == -1)
                                                                          {
                                                                            diostream->SetEvent(DIOANDROIDUDPFSMEVENT_DISCONNECTING);
                                                                            break;
                                                                          }

                                                                        if(size)
                                                                          {
                                                                            diostream->outbuffer->Extract(NULL, 0 , esize);
                                                                          }

                                                                      }
                                                                  }
                                                              }
                                                          }
                                                          break;

          case DIOANDROIDUDPFSMSTATE_SENDINGDATA        : break;

          case DIOANDROIDUDPFSMSTATE_DISCONNECTING      : break;
        }
    }
   else
    {
      if(diostream->GetEvent()<DIOANDROIDUDP_LASTEVENT)
        {
          diostream->CheckTransition();

          switch(diostream->GetCurrentState())
            {
              case DIOANDROIDUDPFSMSTATE_NONE               : break;

              case DIOANDROIDUDPFSMSTATE_GETTINGCONNECTION   : { struct sockaddr_in loc_addr = { 0 };

                                                              diostream->handle = socket(AF_INET, SOCK_DGRAM, 0);
                                                              if(diostream->handle < 0)
                                                                {
                                                                  diostream->SetEvent(DIOANDROIDUDPFSMEVENT_DISCONNECTING);
                                                                  break;
                                                                }

                                                              /*
                                                              int options = 1;
                                                              if(setsockopt(diostream->handle, SOL_SOCKET, SO_REUSEADDR, (const char *)&options, sizeof(options)) == -1)
                                                                {
                                                                  diostream->SetEvent(DIOANDROIDUDPFSMEVENT_DISCONNECTING);
                                                                  break;
                                                                }
                                                              */

                                                              if(diostream->config->IsServer() || (!diostream->config->GetLocalIP()->IsEmpty()))
                                                                {
                                                                  XSTRING IPstring;

                                                                  diostream->config->GetLocalIP()->GetXString(IPstring);

                                                                  loc_addr.sin_family = AF_INET;

                                                                  if(!diostream->config->GetLocalIP()->IsEmpty())
                                                                    {
                                                                      XSTRING_CREATEOEM(IPstring, charOEM)
                                                                      loc_addr.sin_addr.s_addr  = inet_addr(charOEM);
                                                                      XSTRING_DELETEOEM(IPstring, charOEM)

                                                                    } else loc_addr.sin_addr.s_addr = htonl(INADDR_ANY);

                                                                  loc_addr.sin_port = htons(diostream->config->GetRemotePort());

                                                                  if(bind(diostream->handle, (struct sockaddr *)&loc_addr, sizeof(loc_addr)) == -1)
                                                                    {
                                                                      diostream->SetEvent(DIOANDROIDUDPFSMEVENT_DISCONNECTING);
                                                                      break;
                                                                    }
                                                                }

                                                              if(!diostream->config->IsServer())
                                                                {
                                                                  diostream->config->GetRemoteURL()->ResolveURL(diostream->remoteaddress);

                                                                  if(diostream->config->IsBroadcastModeActive())
                                                                    {
                                                                      int yes = 1;
                                                                      setsockopt(diostream->handle, SOL_SOCKET, SO_BROADCAST, (char*)&yes, sizeof(yes));
                                                                    }
                                                                }

                                                              fcntl(diostream->handle, F_SETFL, fcntl(diostream->handle, F_GETFL,0) | O_NONBLOCK);

                                                            }
                                                            break;


              case DIOANDROIDUDPFSMSTATE_CONNECTED          : { DIOSTREAMXEVENT xevent(diostream,DIOSTREAMXEVENT_TYPE_CONNECTED);
                                                              xevent.SetDIOStream(diostream);
                                                              diostream->PostEvent(&xevent);

                                                              diostream->SetEvent(DIOANDROIDUDPFSMEVENT_WAITINGTOREAD);
                                                            }
                                                            break;

              case DIOANDROIDUDPFSMSTATE_WAITINGTOREAD      : break;

              case DIOANDROIDUDPFSMSTATE_SENDINGDATA        : break;

              case DIOANDROIDUDPFSMSTATE_DISCONNECTING      : { DIOSTREAMXEVENT xevent(diostream,DIOSTREAMXEVENT_TYPE_DISCONNECTED);
                                                              xevent.SetDIOStream(diostream);
                                                              diostream->PostEvent(&xevent);

                                                              diostream->threadconnection->Run(false);
                                                              diostream->status        = DIOSTREAMSTATUS_DISCONNECTED;
                                                            }
                                                            break;

            }
        }
    }
}


