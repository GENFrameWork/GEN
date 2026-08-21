/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamTLS.h
*
* @class      DIOSTREAMTLS
* @brief      Data Input/Output Stream TLS (Transport Layer Security) class
* @ingroup    DATAIO
*
* @copyright  EndoraSoft. All rights reserved.
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

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XBase.h"
#include "XFactory.h"
#include "XBuffer.h"
#include "XTimer.h"
#include "XTrace.h"

#include "DIOStream.h"
#include "DIOStreamTLSConfig.h"
#include "DIOStreamTLSSession.h"
#include "DIOStreamTLSHandshakeClient.h"
#include "DIOStreamTLSMessagesHandShakeClientHello.h"
#include "DIOStreamTLSMessagesHandShakeServerHello.h"



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


#define DIOSTREAMTLS_TIMEOUT                3      // in seconds


enum DIOSTREAMTLS_ERROR
{
  DIOSTREAMTLS_ERROR_NONE              = 0 ,
  DIOSTREAMTLS_ERROR_CONFIGURATION         ,
  DIOSTREAMTLS_ERROR_TRANSPORT             ,
  DIOSTREAMTLS_ERROR_HANDSHAKE             ,
  DIOSTREAMTLS_ERROR_RECORD                ,
  DIOSTREAMTLS_ERROR_TRUNCATED             ,
};




/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


template<typename T>
class DIOSTREAMTLS : public T
{
  public:
                                            DIOSTREAMTLS                            ()
                                            {
                                              Clean();

                                              timeout = DIOSTREAMTLS_TIMEOUT;
                                            }


    virtual                                ~DIOSTREAMTLS                            ()
                                            {
                                              handshakeclient.End();
                                              session.End();

                                              Clean();
                                            }


    bool                                    Open                                    ()
                                            {
                                              DIOSTREAMTLSCONFIG* config;
                                              XCHAR*                servername;

                                              if(!isclosed)
                                                {
                                                  if(!Close()) return false;
                                                }

                                              tlserror  = DIOSTREAMTLS_ERROR_NONE;
                                              isclosing = false;

                                              config = dynamic_cast<DIOSTREAMTLSCONFIG*>(T::GetConfig());
                                              if(!config || config->IsServer())
                                                {
                                                  tlserror = DIOSTREAMTLS_ERROR_CONFIGURATION;
                                                  return false;
                                                }

                                              servername = config->GetServerName()->GetSize()?config->GetServerName()->Get():config->GetRemoteURL()->Get();
                                              if(!servername || !servername[0])
                                                {
                                                  tlserror = DIOSTREAMTLS_ERROR_CONFIGURATION;
                                                  return false;
                                                }

                                              handshakeclient.End();
                                              session.End();

                                              if(!session.Ini(config->GetCipherSuite(), DIOSTREAMTLSKEYSCHEDULE_ROLE_CLIENT) ||
                                                 !handshakeclient.Ini(&session, config->IsAllowUnauthenticatedServer()) ||
                                                 !handshakeclient.Capabilities_Set(config))
                                                {
                                                  tlserror = DIOSTREAMTLS_ERROR_CONFIGURATION;
                                                  return false;
                                                }

                                              if(!config->IsAllowUnauthenticatedServer() &&
                                                 !handshakeclient.Authentication_Set(servername, config->GetTrustedRoots()))
                                                {
                                                  tlserror = DIOSTREAMTLS_ERROR_CONFIGURATION;
                                                  return false;
                                                }

                                              if(!T::Open())
                                                {
                                                  tlserror = DIOSTREAMTLS_ERROR_TRANSPORT;
                                                  return false;
                                                }

                                              isclosed = false;

                                              if(!T::WaitToConnected(timeout))
                                                {
                                                  tlserror = DIOSTREAMTLS_ERROR_TRANSPORT;
                                                  Close_OnError();
                                                  return false;
                                                }

                                              T::SetStatus(DIOSTREAMSTATUS_GETTINGCONNECTION);

                                              if(!Handshake_Client(servername))
                                                {
                                                  if(tlserror == DIOSTREAMTLS_ERROR_NONE) tlserror = DIOSTREAMTLS_ERROR_HANDSHAKE;

                                                  if(session.GetEpoch(DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL) != DIOSTREAMTLSSESSION_EPOCH_CLEAR)
                                                    {
                                                      Alert_Send(DIOSTREAMTLS_ALERT_LEVEL_FATAL, DIOSTREAMTLS_ALERT_DESCRIPTION_HANDSHAKE_FAILURE);
                                                    }

                                                  Close_OnError();
                                                  return false;
                                                }

                                              T::SetStatus(DIOSTREAMSTATUS_CONNECTED);

                                              return true;
                                            }


    XDWORD                                  Read                                    (XBYTE* buffer, XDWORD size)
                                            {
                                              if(!buffer || !size) return 0;

                                              XDWORD sizeread = session.ApplicationData_Read(buffer, size);
                                              if(!sizeread)
                                                {
                                                  if(!ApplicationInput_Process()) return 0;

                                                  sizeread = session.ApplicationData_Read(buffer, size);
                                                }

                                              if(sizeread)
                                                {
                                                  T::AddNBytesRead(sizeread);
                                                  if(this->xtimernotactivity) this->xtimernotactivity->Reset();
                                                }

                                              return sizeread;
                                            }


    XDWORD                                  Read                                    (XBUFFER& buffer)
                                            {
                                              if(!buffer.GetSize()) return 0;

                                              if(session.GetApplicationInput()->IsEmpty())
                                                {
                                                  if(!ApplicationInput_Process() || session.GetApplicationInput()->IsEmpty()) return 0;
                                                }

                                              XDWORD sizeread = Read(buffer.Get(), buffer.GetSize());
                                              if(sizeread)
                                                {
                                                  if(sizeread != buffer.GetSize()) buffer.Resize(sizeread);
                                                }
                                               else
                                                {
                                                  buffer.Delete();
                                                }

                                              return buffer.GetSize();
                                            }


    XDWORD                                  Write                                   (XBYTE* buffer, XDWORD size)
                                            {
                                              XBUFFER records;

                                              if(!buffer || !size || isclosing || isclosed ||
                                                 (T::GetStatus() != DIOSTREAMSTATUS_CONNECTED) ||
                                                 !handshakeclient.IsHandshakeCompleted())
                                                {
                                                  return 0;
                                                }

                                              if(!session.ApplicationData_Protect(buffer, size, records) ||
                                                 !Transport_Write(records))
                                                {
                                                  tlserror = DIOSTREAMTLS_ERROR_TRANSPORT;
                                                  return 0;
                                                }

                                              T::AddNBytesWrite(size);
                                              if(this->xtimernotactivity) this->xtimernotactivity->Reset();

                                              return size;
                                            }


    bool                                    KeyUpdate                               (bool requestpeer = false)
                                            {
                                              XBUFFER records;

                                              if(isclosing || isclosed ||
                                                 (T::GetStatus() != DIOSTREAMSTATUS_CONNECTED) ||
                                                 !handshakeclient.IsHandshakeCompleted() ||
                                                 !session.KeyUpdate_Create(requestpeer, records))
                                                {
                                                  return false;
                                                }

                                              if(!Transport_Write(records))
                                                {
                                                  tlserror = DIOSTREAMTLS_ERROR_TRANSPORT;
                                                  Close_OnError();
                                                  return false;
                                                }

                                              return true;
                                            }


    bool                                    WaitToFilledReadingBuffer               (int filledto = DIOSTREAM_SOMETHINGTOREAD, int timeout = XTIMER_INFINITE)
                                            {
                                              XTIMER* xtimer;
                                              bool    status = false;

                                              xtimer = GEN_XFACTORY.CreateTimer();
                                              if(!xtimer) return false;

                                              xtimer->Reset();

                                              while(true)
                                                {
                                                  XDWORD size = session.GetApplicationInput()->GetSize();

                                                  if(((filledto == DIOSTREAM_SOMETHINGTOREAD) && size) ||
                                                     ((filledto != DIOSTREAM_SOMETHINGTOREAD) && ((int)size >= filledto)))
                                                    {
                                                      status = true;
                                                      break;
                                                    }

                                                  if(!ApplicationInput_Process()) break;

                                                  size = session.GetApplicationInput()->GetSize();

                                                  if(((filledto == DIOSTREAM_SOMETHINGTOREAD) && size) ||
                                                     ((filledto != DIOSTREAM_SOMETHINGTOREAD) && ((int)size >= filledto)))
                                                    {
                                                      status = true;
                                                      break;
                                                    }

                                                  if(session.IsCloseNotifyReceived() || session.IsError() ||
                                                     (T::GetStatus() == DIOSTREAMSTATUS_DISCONNECTED))
                                                    {
                                                      break;
                                                    }

                                                  if((timeout != XTIMER_INFINITE) && (xtimer->GetMeasureSeconds() >= (XDWORD)timeout)) break;

                                                  DIOSTREAMCONFIG* streamconfig = T::GetConfig();
                                                  T::Wait(streamconfig?streamconfig->GetPollInterval():DIOSTREAM_TIMEINWAITFUNCTIONS);
                                                }

                                              GEN_XFACTORY.DeleteTimer(xtimer);

                                              return status;
                                            }


    XBUFFER*                                GetInXBuffer                            ()
                                            {
                                              ApplicationInput_Process();

                                              return session.GetApplicationInput();
                                            }


    bool                                    Disconnect                              ()
                                            {
                                              if(isclosed) return true;

                                              bool status = true;

                                              if(!isclosing && handshakeclient.IsHandshakeCompleted() &&
                                                 (T::GetStatus() != DIOSTREAMSTATUS_DISCONNECTED))
                                                {
                                                  status = CloseNotify_Send();
                                                }

                                              isclosing = true;

                                              if(T::GetStatus() != DIOSTREAMSTATUS_DISCONNECTED)
                                                {
                                                  status = T::Disconnect() && status;
                                                }

                                              T::SetStatus(DIOSTREAMSTATUS_DISCONNECTED);

                                              return status;
                                            }


    bool                                    Close                                   ()
                                            {
                                              if(isclosed) return true;

                                              bool status = true;

                                              if(!isclosing && handshakeclient.IsHandshakeCompleted() &&
                                                 (T::GetStatus() != DIOSTREAMSTATUS_DISCONNECTED))
                                                {
                                                  status = CloseNotify_Send();
                                                }

                                              isclosing = true;

                                              status = T::Close() && status;

                                              handshakeclient.End();
                                              session.End();

                                              T::SetStatus(DIOSTREAMSTATUS_DISCONNECTED);

                                              isclosed  = true;
                                              isclosing = false;

                                              return status;
                                            }


    int                                     GetTimeout                              ()
                                            {
                                              return timeout;
                                            }


    void                                    SetTimeout                              (int timeout = DIOSTREAMTLS_TIMEOUT)
                                            {
                                              this->timeout = timeout;
                                            }


    DIOSTREAMTLS_ERROR                      GetLastTLSError                         ()
                                            {
                                              return tlserror;
                                            }


    DIOSTREAMTLSSESSION*                    GetTLSSession                           ()
                                            {
                                              return &session;
                                            }


    DIOSTREAMTLSHANDSHAKECLIENT*            GetTLSHandshakeClient                   ()
                                            {
                                              return &handshakeclient;
                                            }

  private:

    bool                                    Handshake_Client                        (XCHAR* servername)
                                            {
                                              XBUFFER clienthello;
                                              XBUFFER records;
                                              XTIMER* xtimer;
                                              bool    serverhelloprocessed = false;
                                              bool    status               = false;

                                              if(!handshakeclient.ClientHello_Create(servername, clienthello, records) ||
                                                 !Transport_Write(records))
                                                {
                                                  return false;
                                                }

                                              xtimer = GEN_XFACTORY.CreateTimer();
                                              if(!xtimer) return false;

                                              xtimer->Reset();

                                              while(xtimer->GetMeasureSeconds() < (XDWORD)timeout)
                                                {
                                                  XBUFFER input;

                                                  if(!Transport_Read(input))
                                                    {
                                                      if(T::GetStatus() == DIOSTREAMSTATUS_DISCONNECTED) break;

                                                      DIOSTREAMCONFIG* streamconfig = T::GetConfig();
                                                      T::Wait(streamconfig?streamconfig->GetPollInterval():DIOSTREAM_TIMEINWAITFUNCTIONS);
                                                      continue;
                                                    }

                                                  if(!handshakeclient.RecordInput_Add(input)) break;

                                                  if(!serverhelloprocessed &&
                                                     !ServerHello_Process(serverhelloprocessed)) break;

                                                  if(serverhelloprocessed && !handshakeclient.Process()) break;

                                                  if(handshakeclient.IsServerFinishedVerified())
                                                    {
                                                      XBUFFER clientfinished;
                                                      XBUFFER clientfinishedrecords;

                                                      if(handshakeclient.ClientFinished_Create(clientfinished, clientfinishedrecords) &&
                                                         Transport_Write(clientfinishedrecords))
                                                        {
                                                          status = true;
                                                        }

                                                      break;
                                                    }
                                                }

                                              GEN_XFACTORY.DeleteTimer(xtimer);

                                              return status;
                                            }


    bool                                    ServerHello_Process                     (bool& serverhelloprocessed)
                                            {
                                              while(!serverhelloprocessed)
                                                {
                                                  DIOSTREAMTLS_CONTENTTYPE   contenttype = (DIOSTREAMTLS_CONTENTTYPE)0;
                                                  DIOSTREAMTLSSESSION_RESULT result;
                                                  XBUFFER                    plain;

                                                  result = session.Record_Extract(contenttype, plain);

                                                  if(result == DIOSTREAMTLSSESSION_RESULT_INCOMPLETE) return true;
                                                  if(result == DIOSTREAMTLSSESSION_RESULT_ERROR)      return false;

                                                  if(contenttype == DIOSTREAMTLS_MSG_CONTENTTYPE_CHANGE_CIPHER_SPEC)
                                                    {
                                                      if((plain.GetSize() != 1) || (plain.GetByte(0) != 1)) return false;
                                                      continue;
                                                    }

                                                  if(contenttype != DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE ||
                                                     !session.HandshakeInput_Add(plain))
                                                    {
                                                      return false;
                                                    }

                                                  XBUFFER serverhello;
                                                  result = session.Handshake_Extract(serverhello);

                                                  if(result == DIOSTREAMTLSSESSION_RESULT_INCOMPLETE) continue;
                                                  if(result == DIOSTREAMTLSSESSION_RESULT_ERROR)      return false;

                                                  DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO> serverhellomessage;
                                                  XBUFFER                                                          workbuffer;

                                                  workbuffer.Add(serverhello);
                                                  if(!serverhellomessage.GetFromBuffer(workbuffer, false) || !workbuffer.IsEmpty()) return false;

                                                  if(serverhellomessage.GetBody()->IsHelloRetryRequest())
                                                    {
                                                      XBUFFER secondclienthello;
                                                      XBUFFER retryrecords;

                                                      if(!session.GetHandshakeInput()->IsEmpty() ||
                                                         !handshakeclient.HelloRetryRequest_Process(serverhello, secondclienthello, retryrecords) ||
                                                         !Transport_Write(retryrecords))
                                                        {
                                                          return false;
                                                        }

                                                      continue;
                                                    }

                                                  if(!handshakeclient.ServerHello_Process(serverhello)) return false;
                                                  if(!session.GetHandshakeInput()->IsEmpty())            return false;

                                                  serverhelloprocessed = true;
                                                }

                                              return true;
                                            }


    bool                                    ApplicationInput_Process                ()
                                            {
                                              XBUFFER input;

                                              if(!session.IsIni()) return false;
                                              if(session.IsTransportClosedWithoutNotify()) return false;

                                              if(Transport_Read(input) && !session.RecordInput_Add(input))
                                                {
                                                  tlserror = DIOSTREAMTLS_ERROR_RECORD;
                                                  return false;
                                                }

                                              DIOSTREAMTLSSESSION_RESULT result = session.ApplicationData_Process();

                                              if(result == DIOSTREAMTLSSESSION_RESULT_ERROR)
                                                {
                                                  tlserror = DIOSTREAMTLS_ERROR_RECORD;
                                                  Alert_Send(DIOSTREAMTLS_ALERT_LEVEL_FATAL, DIOSTREAMTLS_ALERT_DESCRIPTION_DECODE_ERROR);
                                                  T::SetStatus(DIOSTREAMSTATUS_DISCONNECTED);
                                                  return false;
                                                }

                                              XBUFFER posthandshakeoutput;

                                              if(!session.PostHandshakeOutput_Extract(posthandshakeoutput) ||
                                                 (!posthandshakeoutput.IsEmpty() && !Transport_Write(posthandshakeoutput)))
                                                {
                                                  tlserror = DIOSTREAMTLS_ERROR_TRANSPORT;
                                                  T::SetStatus(DIOSTREAMSTATUS_DISCONNECTED);
                                                  return false;
                                                }

                                              if(session.IsCloseNotifyReceived())
                                                {
                                                  if(!session.IsCloseNotifySent()) CloseNotify_Send();

                                                  T::SetStatus(DIOSTREAMSTATUS_DISCONNECTED);
                                                }
                                               else
                                                {
                                                  if(!isclosed && (T::GetStatus() == DIOSTREAMSTATUS_DISCONNECTED))
                                                    {
                                                      if(!session.TransportClosed())
                                                        {
                                                          tlserror = DIOSTREAMTLS_ERROR_TRUNCATED;
                                                          return !session.GetApplicationInput()->IsEmpty();
                                                        }
                                                    }
                                                }

                                              return true;
                                            }


    bool                                    Transport_Read                          (XBUFFER& input)
                                            {
                                              XBUFFER* transportinput = T::GetInXBuffer();
                                              XDWORD   size;

                                              input.Delete();

                                              if(!transportinput) return false;

                                              size = transportinput->GetSize();
                                              if(!size) return false;

                                              if(!input.Resize(size)) return false;

                                              if(transportinput->Extract(input.Get(), 0, size) != size)
                                                {
                                                  input.Delete();
                                                  return false;
                                                }

                                              return true;
                                            }


    bool                                    Transport_Write                         (XBUFFER& output)
                                            {
                                              XBUFFER* transportoutput;
                                              XTIMER*  xtimer;
                                              bool     status = true;

                                              if(output.IsEmpty()) return true;

                                              transportoutput = T::GetOutXBuffer();
                                              if(!transportoutput || !transportoutput->Add(output)) return false;

                                              xtimer = GEN_XFACTORY.CreateTimer();
                                              if(!xtimer) return false;

                                              xtimer->Reset();

                                              while(!T::IsFlushOutXBuffer())
                                                {
                                                  if(T::GetStatus() == DIOSTREAMSTATUS_DISCONNECTED)
                                                    {
                                                      T::ResetOutXBuffer();
                                                      status = false;
                                                      break;
                                                    }

                                                  if(timeout != XTIMER_INFINITE)
                                                    {
                                                      if(xtimer->GetMeasureSeconds() >= (XDWORD)timeout)
                                                        {
                                                          T::ResetOutXBuffer();
                                                          status = false;
                                                          break;
                                                        }
                                                    }

                                                  DIOSTREAMCONFIG* streamconfig = T::GetConfig();
                                                  T::Wait(streamconfig?streamconfig->GetPollInterval():DIOSTREAM_TIMEINWAITFUNCTIONS);
                                                }

                                              GEN_XFACTORY.DeleteTimer(xtimer);

                                              return status;
                                            }


    bool                                    Alert_Send                              (DIOSTREAMTLS_ALERT_LEVEL level, DIOSTREAMTLS_ALERT_DESCRIPTION description)
                                            {
                                              XBUFFER records;

                                              if(!session.Alert_Create(level, description, records)) return false;

                                              return Transport_Write(records);
                                            }


    bool                                    CloseNotify_Send                        ()
                                            {
                                              XBUFFER records;

                                              if(!session.CloseNotify_Create(records)) return false;
                                              if(records.IsEmpty())                  return true;

                                              return Transport_Write(records);
                                            }


    void                                    Close_OnError                           ()
                                            {
                                              isclosing = true;

                                              T::Close();
                                              T::SetStatus(DIOSTREAMSTATUS_DISCONNECTED);

                                              isclosed  = true;
                                              isclosing = false;
                                            }


    void                                    Clean                                   ()
                                            {
                                              timeout     = 0;
                                              tlserror    = DIOSTREAMTLS_ERROR_NONE;
                                              isclosed    = true;
                                              isclosing   = false;
                                            }


    int                                     timeout;
    DIOSTREAMTLS_ERROR                      tlserror;
    bool                                    isclosed;
    bool                                    isclosing;

    DIOSTREAMTLSSESSION                     session;
    DIOSTREAMTLSHANDSHAKECLIENT             handshakeclient;
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
