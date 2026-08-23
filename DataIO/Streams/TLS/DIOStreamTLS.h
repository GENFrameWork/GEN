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
#include "DIOStreamTLS13Session.h"
#include "DIOStreamTLS13HandshakeClient.h"
#include "DIOStreamTLSMessagesHandShakeClientHello.h"
#include "DIOStreamTLSMessagesHandShakeServerHello.h"

// Phase 5: version negotiation. PARALLEL TLS 1.2 objects (handshakeclient12 below, owning its own session) live
// alongside the TLS 1.3 ones untouched; DIOSTREAMTLS<T> itself is the dispatch point, decided per
// DIOSTREAMTLSCONFIG's min/max version window (see DIOStreamTLSConfig.h).
#include "DIOStreamTLS12Session.h"
#include "DIOStreamTLS12HandshakeClient.h"



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
                                              handshakeclient12.End();

                                              Clean();
                                            }


    bool                                    Open                                    ()
                                            {
                                              DIOSTREAMTLSCONFIG* config;
                                              XCHAR*                servername;
                                              XWORD                 minversion;
                                              XWORD                 maxversion;
                                              bool                  starttls12;

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

                                              minversion = config->GetMinVersion();
                                              maxversion = config->GetMaxVersion();

                                              if(((minversion != DIOSTREAMTLS_MSG_VERSION_TLS_1_2) && (minversion != DIOSTREAMTLS_MSG_VERSION_TLS_1_3)) ||
                                                 ((maxversion != DIOSTREAMTLS_MSG_VERSION_TLS_1_2) && (maxversion != DIOSTREAMTLS_MSG_VERSION_TLS_1_3)) ||
                                                 (minversion > maxversion))
                                                {
                                                  tlserror = DIOSTREAMTLS_ERROR_CONFIGURATION;
                                                  return false;
                                                }

                                              // dualversionmode == both versions allowed: TLS 1.3 is attempted first (Handshake_Attempt() below,
                                              // completely unchanged from every earlier phase), and Open() closes the transport and retries the
                                              // WHOLE connection from scratch, this time offering TLS 1.2 only via the parallel objects, ONLY
                                              // when that first attempt was explicitly turned away by a fatal protocol_version/handshake_failure
                                              // alert (versionrejected -- see its declaration and ServerHello_Process()). A bare timeout, a reset
                                              // connection, or anything else that isn't a real "I don't speak TLS 1.3" answer from the peer never
                                              // triggers the retry, so an on-path attacker cannot force the weaker path just by breaking the 1.3
                                              // attempt in some generic way. This costs an extra round trip on a real downgrade, but it means the
                                              // TLS 1.3 path (handshakeclient/session, Capabilities_Set() included) never has to tolerate anything
                                              // beyond the two suites it already validates, so it needed no changes at all.
                                              dualversionmode = (minversion == DIOSTREAMTLS_MSG_VERSION_TLS_1_2) && (maxversion == DIOSTREAMTLS_MSG_VERSION_TLS_1_3);
                                              starttls12       = (maxversion == DIOSTREAMTLS_MSG_VERSION_TLS_1_2);

                                              if(Handshake_Attempt(config, servername, starttls12))
                                                {
                                                  T::SetStatus(DIOSTREAMSTATUS_CONNECTED);
                                                  return true;
                                                }

                                              if(dualversionmode && !starttls12 && (tlserror == DIOSTREAMTLS_ERROR_HANDSHAKE) && versionrejected)
                                                {
                                                  tlserror = DIOSTREAMTLS_ERROR_NONE;

                                                  if(Handshake_Attempt(config, servername, true))
                                                    {
                                                      T::SetStatus(DIOSTREAMSTATUS_CONNECTED);
                                                      return true;
                                                    }
                                                }

                                              return false;
                                            }


    // One full connect-and-handshake attempt, either version. Open() calls this once, and a second time (over
    // a fresh transport) only for a dual-version TLS 1.3-to-1.2 fallback (see Open() above).
    bool                                    Handshake_Attempt                       (DIOSTREAMTLSCONFIG* config, XCHAR* servername, bool astls12)
                                            {
                                              handshakeclient.End();
                                              session.End();
                                              handshakeclient12.End();

                                              usingtls12      = astls12;
                                              versionrejected = false;

                                              if(usingtls12)
                                                {
                                                  // dualversionmode can only be true here on the fallback (second) attempt -- see Open(): it is
                                                  // mutually exclusive with starttls12, and only the fallback path ever calls Handshake_Attempt()
                                                  // with astls12==true while dualversionmode==true. That is exactly when the RFC 8446 downgrade
                                                  // sentinel must be checked (see DIOSTREAMTLS12HANDSHAKECLIENT::Ini()).
                                                  if(!handshakeclient12.Ini(config->IsAllowUnauthenticatedServer(), dualversionmode) ||
                                                     (!config->IsAllowUnauthenticatedServer() &&
                                                      !handshakeclient12.Authentication_Set(servername, config->GetTrustedRoots())))
                                                    {
                                                      tlserror = DIOSTREAMTLS_ERROR_CONFIGURATION;
                                                      return false;
                                                    }
                                                }
                                               else
                                                {
                                                  if(!session.Ini(config->GetCipherSuite(), DIOSTREAMTLSKEYSCHEDULE_ROLE_CLIENT) ||
                                                     !handshakeclient.Ini(&session, config->IsAllowUnauthenticatedServer()) ||
                                                     !handshakeclient.Capabilities_Set(config) ||
                                                     (!config->IsAllowUnauthenticatedServer() &&
                                                      !handshakeclient.Authentication_Set(servername, config->GetTrustedRoots())))
                                                    {
                                                      tlserror = DIOSTREAMTLS_ERROR_CONFIGURATION;
                                                      return false;
                                                    }
                                                }

                                              // A dual-version retry reuses the same underlying T after Close_OnError() from the first (failed)
                                              // attempt: that only closes the socket, it does not clear whatever was left queued in T's own
                                              // in/out buffers (a transport class like SYNCSTREAM that bypasses the base Open()'s bookkeeping
                                              // never gets a chance to). Any leftover bytes from the aborted attempt would otherwise get sent
                                              // to (or misread from) the freshly reconnected socket, corrupting this second attempt.
                                              T::ResetOutXBuffer();
                                              T::ResetInXBuffer();

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

                                                  if((!usingtls12 && (session.GetEpoch(DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL) != DIOSTREAMTLS13SESSION_EPOCH_CLEAR)) ||
                                                     (usingtls12 && handshakeclient12.GetSession()->IsIni()))
                                                    {
                                                      Alert_Send(DIOSTREAMTLS_ALERT_LEVEL_FATAL, DIOSTREAMTLS_ALERT_DESCRIPTION_HANDSHAKE_FAILURE);
                                                    }

                                                  Close_OnError();
                                                  return false;
                                                }

                                              return true;
                                            }


    XDWORD                                  Read                                    (XBYTE* buffer, XDWORD size)
                                            {
                                              if(!buffer || !size) return 0;

                                              XDWORD sizeread = usingtls12?handshakeclient12.GetSession()->ApplicationData_Read(buffer, size):session.ApplicationData_Read(buffer, size);
                                              if(!sizeread)
                                                {
                                                  if(!ApplicationInput_Process()) return 0;

                                                  sizeread = usingtls12?handshakeclient12.GetSession()->ApplicationData_Read(buffer, size):session.ApplicationData_Read(buffer, size);
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

                                              XBUFFER* applicationinput = usingtls12?handshakeclient12.GetSession()->GetApplicationInput():session.GetApplicationInput();

                                              if(applicationinput->IsEmpty())
                                                {
                                                  if(!ApplicationInput_Process() || applicationinput->IsEmpty()) return 0;
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
                                                 !IsHandshakeCompleted())
                                                {
                                                  return 0;
                                                }

                                              bool protectstatus = usingtls12?handshakeclient12.GetSession()->ApplicationData_Protect(buffer, size, records):
                                                                              session.ApplicationData_Protect(buffer, size, records);

                                              if(!protectstatus || !Transport_Write(records))
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

                                              // RFC 5246 has no KeyUpdate message: unsupported once negotiated TLS 1.2.
                                              if(usingtls12) return false;

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
                                                  XBUFFER* applicationinput = usingtls12?handshakeclient12.GetSession()->GetApplicationInput():session.GetApplicationInput();
                                                  XDWORD   size             = applicationinput->GetSize();

                                                  if(((filledto == DIOSTREAM_SOMETHINGTOREAD) && size) ||
                                                     ((filledto != DIOSTREAM_SOMETHINGTOREAD) && ((int)size >= filledto)))
                                                    {
                                                      status = true;
                                                      break;
                                                    }

                                                  if(!ApplicationInput_Process()) break;

                                                  size = applicationinput->GetSize();

                                                  if(((filledto == DIOSTREAM_SOMETHINGTOREAD) && size) ||
                                                     ((filledto != DIOSTREAM_SOMETHINGTOREAD) && ((int)size >= filledto)))
                                                    {
                                                      status = true;
                                                      break;
                                                    }

                                                  if((usingtls12?handshakeclient12.GetSession()->IsCloseNotifyReceived():session.IsCloseNotifyReceived()) ||
                                                     (usingtls12?handshakeclient12.GetSession()->IsError():session.IsError()) ||
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

                                              return usingtls12?handshakeclient12.GetSession()->GetApplicationInput():session.GetApplicationInput();
                                            }


    bool                                    Disconnect                              ()
                                            {
                                              if(isclosed) return true;

                                              bool status = true;

                                              if(!isclosing && IsHandshakeCompleted() &&
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

                                              if(!isclosing && IsHandshakeCompleted() &&
                                                 (T::GetStatus() != DIOSTREAMSTATUS_DISCONNECTED))
                                                {
                                                  status = CloseNotify_Send();
                                                }

                                              isclosing = true;

                                              status = T::Close() && status;

                                              handshakeclient.End();
                                              session.End();
                                              handshakeclient12.End();

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


    DIOSTREAMTLS13SESSION*                    GetTLSSession                           ()
                                            {
                                              return &session;
                                            }


    DIOSTREAMTLS13HANDSHAKECLIENT*            GetTLSHandshakeClient                   ()
                                            {
                                              return &handshakeclient;
                                            }


    // Phase 5: introspection of the parallel TLS 1.2 objects and of which version actually got negotiated.
    // GetTLSSession()/GetTLSHandshakeClient() above keep returning the TLS 1.3 objects unconditionally, exactly
    // as before, for source compatibility; check IsUsingTLS12() first if the negotiated version matters.
    bool                                    IsUsingTLS12                            ()
                                            {
                                              return usingtls12;
                                            }


    DIOSTREAMTLS12SESSION*                  GetTLSSession12                         ()
                                            {
                                              // Unlike DIOSTREAMTLS13SESSION (TLS 1.3, injected as a pointer into its handshake client),
                                              // DIOSTREAMTLS12SESSION is owned directly BY handshakeclient12 (see DIOStreamTLS12HandshakeClient.h)
                                              // — there is no separate session12 member here to return a pointer to.
                                              return handshakeclient12.GetSession();
                                            }


    DIOSTREAMTLS12HANDSHAKECLIENT*          GetTLSHandshakeClient12                 ()
                                            {
                                              return &handshakeclient12;
                                            }


    bool                                    IsHandshakeCompleted                    ()
                                            {
                                              return usingtls12?handshakeclient12.IsHandshakeCompleted():handshakeclient.IsHandshakeCompleted();
                                            }

  private:

    bool                                    Handshake_Client                        (XCHAR* servername)
                                            {
                                              XBUFFER clienthello;
                                              XBUFFER records;
                                              XTIMER* xtimer;
                                              bool    serverhelloprocessed = false;
                                              bool    status               = false;

                                              if(usingtls12)
                                                {
                                                  // Pure TLS 1.2 mode, decided in Open(): native ClientHello, nothing borrowed from the 1.3 path.
                                                  if(!handshakeclient12.ClientHello_Create(servername, clienthello, records) ||
                                                     !Transport_Write(records))
                                                    {
                                                      return false;
                                                    }
                                                }
                                               else
                                                {
                                                  if(!handshakeclient.ClientHello_Create(servername, clienthello, records) ||
                                                     !Transport_Write(records))
                                                    {
                                                      return false;
                                                    }

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

                                                  if(usingtls12)
                                                    {
                                                      if(!handshakeclient12.RecordInput_Add(input) || !handshakeclient12.Process()) break;

                                                      if(handshakeclient12.GetState() == DIOSTREAMTLS12HANDSHAKECLIENT_STATE_READY_CLIENTFLIGHT)
                                                        {
                                                          XBUFFER flightrecords;

                                                          if(!handshakeclient12.ClientFlight_Create(flightrecords) || !Transport_Write(flightrecords)) break;

                                                          continue;
                                                        }

                                                      if(handshakeclient12.IsHandshakeCompleted())
                                                        {
                                                          status = true;
                                                          break;
                                                        }

                                                      if(handshakeclient12.GetState() == DIOSTREAMTLS12HANDSHAKECLIENT_STATE_ERROR) break;

                                                      continue;
                                                    }

                                                  if(!handshakeclient.RecordInput_Add(input)) break;

                                                  if(!serverhelloprocessed &&
                                                     !ServerHello_Process(serverhelloprocessed, servername)) break;

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

                                              if(!status)
                                                {
                                                  XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[TLS Handshake] \"%s\": FAILED (usingtls12: %s, serverhello processed: %s, server finished verified: %s, transport: %s)"),
                                                                    servername?servername:__L("?"),
                                                                    usingtls12?__L("yes"):__L("no"),
                                                                    serverhelloprocessed?__L("yes"):__L("NO"),
                                                                    (!usingtls12 && handshakeclient.IsServerFinishedVerified())?__L("yes"):__L("NO"),
                                                                    (T::GetStatus() == DIOSTREAMSTATUS_DISCONNECTED)?__L("DISCONNECTED"):__L("connected"));
                                                }

                                              return status;
                                            }


    bool                                    ServerHello_Process                     (bool& serverhelloprocessed, XCHAR* servername = NULL)
                                            {
                                              while(!serverhelloprocessed)
                                                {
                                                  DIOSTREAMTLS_CONTENTTYPE   contenttype = (DIOSTREAMTLS_CONTENTTYPE)0;
                                                  DIOSTREAMTLS13SESSION_RESULT result;
                                                  XBUFFER                    plain;

                                                  result = session.Record_Extract(contenttype, plain);

                                                  if(result == DIOSTREAMTLS13SESSION_RESULT_INCOMPLETE) return true;
                                                  if(result == DIOSTREAMTLS13SESSION_RESULT_ERROR)      return false;

                                                  if(contenttype == DIOSTREAMTLS_MSG_CONTENTTYPE_CHANGE_CIPHER_SPEC)
                                                    {
                                                      if((plain.GetSize() != 1) || (plain.GetByte(0) != 1)) return false;
                                                      continue;
                                                    }

                                                  // A server that refuses the ClientHello answers with an ALERT record BEFORE any ServerHello.
                                                  // That arrives here, not in DIOSTREAMTLS13HANDSHAKECLIENT::Process(), so without this the most
                                                  // common rejection of all (description 40, handshake_failure: nothing we offered is usable,
                                                  // typically an ECDSA-only certificate against an RSA-only signature_algorithms) is silent.
                                                  if(contenttype == DIOSTREAMTLS_MSG_CONTENTTYPE_ALERT)
                                                    {
                                                      DIOSTREAMTLS_MSG_ALERT alert;

                                                      if(alert.GetFromBuffer(plain, false))
                                                        {
                                                          XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[TLS Handshake] \"%s\": ALERT before ServerHello: level %d, description %d [%s]"),
                                                                            servername?servername:__L("?"), (int)alert.GetLevel(), (int)alert.GetDescription(),
                                                                            DIOSTREAMTLS_MSG_ALERT::GetDescriptionString(alert.GetDescription()));

                                                          // Phase 6 hardening: only a well-formed FATAL alert, and only these two descriptions --
                                                          // the ones a real TLS-1.2-only server actually sends when it cannot make sense of a
                                                          // TLS-1.3-shaped ClientHello -- is treated as an explicit "this server does not speak
                                                          // TLS 1.3" signal. This is the ONLY place versionrejected is set; see its declaration.
                                                          if((alert.GetLevel() == DIOSTREAMTLS_ALERT_LEVEL_FATAL) &&
                                                             ((alert.GetDescription() == DIOSTREAMTLS_ALERT_DESCRIPTION_PROTOCOL_VERSION) ||
                                                              (alert.GetDescription() == DIOSTREAMTLS_ALERT_DESCRIPTION_HANDSHAKE_FAILURE)))
                                                            {
                                                              versionrejected = true;
                                                            }
                                                        }
                                                       else
                                                        {
                                                          XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[TLS Handshake] \"%s\": malformed ALERT before ServerHello (%d bytes)"),
                                                                            servername?servername:__L("?"), (int)plain.GetSize());
                                                        }

                                                      return false;
                                                    }

                                                  if(contenttype != DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE ||
                                                     !session.HandshakeInput_Add(plain))
                                                    {
                                                      XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[TLS Handshake] \"%s\": unexpected content type %d before ServerHello"),
                                                                        servername?servername:__L("?"), (int)contenttype);
                                                      return false;
                                                    }

                                                  XBUFFER serverhello;
                                                  result = session.Handshake_Extract(serverhello);

                                                  if(result == DIOSTREAMTLS13SESSION_RESULT_INCOMPLETE) continue;
                                                  if(result == DIOSTREAMTLS13SESSION_RESULT_ERROR)      return false;

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

                                              if(usingtls12)
                                                {
                                                  if(!handshakeclient12.GetSession()->IsIni()) return false;
                                                  if(handshakeclient12.GetSession()->IsTransportClosedWithoutNotify()) return false;

                                                  if(Transport_Read(input) && !handshakeclient12.GetSession()->RecordInput_Add(input))
                                                    {
                                                      tlserror = DIOSTREAMTLS_ERROR_RECORD;
                                                      return false;
                                                    }

                                                  DIOSTREAMTLS12SESSION_RESULT result = handshakeclient12.GetSession()->ApplicationData_Process();

                                                  if(result == DIOSTREAMTLS12SESSION_RESULT_ERROR)
                                                    {
                                                      tlserror = DIOSTREAMTLS_ERROR_RECORD;
                                                      Alert_Send(DIOSTREAMTLS_ALERT_LEVEL_FATAL, DIOSTREAMTLS_ALERT_DESCRIPTION_DECODE_ERROR);
                                                      T::SetStatus(DIOSTREAMSTATUS_DISCONNECTED);
                                                      return false;
                                                    }

                                                  // RFC 5246 has no post-handshake output queue (no KeyUpdate, no NewSessionTicket).

                                                  if(handshakeclient12.GetSession()->IsCloseNotifyReceived())
                                                    {
                                                      if(!handshakeclient12.GetSession()->IsCloseNotifySent()) CloseNotify_Send();

                                                      T::SetStatus(DIOSTREAMSTATUS_DISCONNECTED);
                                                    }
                                                   else
                                                    {
                                                      if(!isclosed && (T::GetStatus() == DIOSTREAMSTATUS_DISCONNECTED))
                                                        {
                                                          if(!handshakeclient12.GetSession()->TransportClosed())
                                                            {
                                                              tlserror = DIOSTREAMTLS_ERROR_TRUNCATED;
                                                              return !handshakeclient12.GetSession()->GetApplicationInput()->IsEmpty();
                                                            }
                                                        }
                                                    }

                                                  return true;
                                                }

                                              if(!session.IsIni()) return false;
                                              if(session.IsTransportClosedWithoutNotify()) return false;

                                              if(Transport_Read(input) && !session.RecordInput_Add(input))
                                                {
                                                  tlserror = DIOSTREAMTLS_ERROR_RECORD;
                                                  return false;
                                                }

                                              DIOSTREAMTLS13SESSION_RESULT result = session.ApplicationData_Process();

                                              if(result == DIOSTREAMTLS13SESSION_RESULT_ERROR)
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

                                              bool status = usingtls12?handshakeclient12.GetSession()->Alert_Create(level, description, records):
                                                                        session.Alert_Create(level, description, records);

                                              if(!status) return false;

                                              return Transport_Write(records);
                                            }


    bool                                    CloseNotify_Send                        ()
                                            {
                                              XBUFFER records;

                                              bool status = usingtls12?handshakeclient12.GetSession()->CloseNotify_Create(records):
                                                                        session.CloseNotify_Create(records);

                                              if(!status)          return false;
                                              if(records.IsEmpty()) return true;

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
                                              timeout         = 0;
                                              tlserror        = DIOSTREAMTLS_ERROR_NONE;
                                              isclosed        = true;
                                              isclosing       = false;
                                              usingtls12      = false;
                                              dualversionmode = false;
                                              versionrejected = false;
                                            }


    int                                     timeout;
    DIOSTREAMTLS_ERROR                      tlserror;
    bool                                    isclosed;
    bool                                    isclosing;

    // Phase 6 hardening: the dual-version fallback (Open(), below) must only fire when the TLS 1.3 attempt was
    // actually, explicitly turned away by the peer -- a fatal protocol_version or handshake_failure alert
    // received before ServerHello, the two descriptions a real TLS-1.2-only server sends (see ServerHello_Process()).
    // Any OTHER handshake-stage failure (a dead/blackholed connection, a timeout, injected garbage that isn't a
    // well-formed alert at all) must NOT authorize retrying over pure TLS 1.2: an active on-path attacker who
    // simply breaks the 1.3 attempt, without ever answering as a legitimate 1.2 server, must not be able to
    // force this client onto the weaker path. Reset once per Handshake_Attempt(), set only in that one place.
    bool                                    versionrejected;

    DIOSTREAMTLS13SESSION                     session;
    DIOSTREAMTLS13HANDSHAKECLIENT             handshakeclient;

    // Phase 5: parallel TLS 1.2 objects. Unlike DIOSTREAMTLS13SESSION (TLS 1.3), which is owned here and injected
    // into handshakeclient as a pointer, DIOSTREAMTLS12SESSION is owned directly BY handshakeclient12 (see
    // DIOStreamTLS12HandshakeClient.h) — reached via handshakeclient12.GetSession(), never a separate member.
    // usingtls12/dualversionmode are decided once per Handshake_Attempt() call from Open() (see there): pure
    // TLS 1.2 config starts with usingtls12 true; dual-version mode tries TLS 1.3 first and retries as pure TLS
    // 1.2 (a second, independent Handshake_Attempt over a fresh transport) only if that first attempt fails at
    // the handshake stage.
    DIOSTREAMTLS12HANDSHAKECLIENT            handshakeclient12;
    bool                                    usingtls12;
    bool                                    dualversionmode;
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
