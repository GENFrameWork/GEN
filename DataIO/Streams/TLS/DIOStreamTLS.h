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
#include "XRand.h"
#include "XFSMachine.h"
#include "XBuffer.h"
#include "XTrace.h"

#include "DIOFactory.h"
#include "DIOIP.h"
#include "DIOStreamConfig.h"
#include "DIOStream.h"
#include "DIOStreamTLSMessages.h"
#include "DIOStreamTLSMessagesExtension.h"
#include "DIOStreamTLSMessagesHandShakeClientHello.h"
#include "DIOStreamTLSMessagesHandShakeServerHello.h"




/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


enum DIOSTREAMTLSXFSMEVENTS
{
  DIOSTREAMTLS_XFSMEVENT_NONE              = 0 ,
  DIOSTREAMTLS_XFSMEVENT_OPEN                  ,
  DIOSTREAMTLS_XFSMEVENT_HANDSHAKE             ,
  DIOSTREAMTLS_XFSMEVENT_CONNECTED             ,
  DIOSTREAMTLS_XFSMEVENT_DISCONNECTED          ,
  DIOSTREAMTLS_XFSMEVENT_CLOSE                 ,

  DIOSTREAMTLS_LASTEVENT
};


enum DIOSTREAMTLSXFSMSTATES
{
  DIOSTREAMTLS_XFSMSTATE_NONE              = 0 ,
  DIOSTREAMTLS_XFSMSTATE_OPEN                  ,
  DIOSTREAMTLS_XFSMSTATE_HANDSHAKE             ,
  DIOSTREAMTLS_XFSMSTATE_CONNECTED             ,
  DIOSTREAMTLS_XFSMSTATE_DISCONNECTED          ,
  DIOSTREAMTLS_XFSMSTATE_CLOSE                 ,

  DIOSTREAMTLS_LASTSTATE
};


#define DIOSTREAMTLS_TIMEOUT                3      // in seconds



/*---- CLASS ---------------------------------------------------------------------------------------------------------*/



/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/


class DIOSTREAMTLSCONFIG;

template<typename T>
class DIOSTREAMTLS :  public T
{
  public:
                                            DIOSTREAMTLS                            () 
                                            {
                                              Clean();    
                                              
                                              timeout = DIOSTREAMTLS_TIMEOUT;  

                                              fsmachine = new XFSMACHINE(0);  

                                              IniFSMachine();                                               
                                            } 
                                       

    virtual                                ~DIOSTREAMTLS                            ()
                                            {          
                                              if(fsmachine)
                                                {
                                                  delete fsmachine;
                                                  fsmachine = NULL;  
                                                }
                                   
                                              Clean();
                                            }


    bool                                    Open                                    ()
                                            {                                              
                                              bool status = T::Open();
                                              if(!status) 
                                                {
                                                  return false;
                                                }

                                              if(T::GetConfig()->IsServer())
                                                {



                                                }
                                               else
                                                {   
                                                  status = T::WaitToConnected(timeout);
                                                  if(status) 
                                                    {                                                     
                                                      thread = CREATEXTHREAD(XTHREADGROUPID_DIOSTREAMTLS, __L("DIOSTREAMTLS::Open"), ThreadRunFunction, (void*)this);
                                                      if(!thread) 
                                                        {
                                                          return false;    
                                                        }

                                                      if(thread->Ini())
                                                        {
                                                          fsmachine->SetEvent(DIOSTREAMTLS_XFSMEVENT_OPEN);
                                                        }
                                                    }
                                                }
                                             
                                              return status;
                                            }
    

    XDWORD                                  Read                                    (XBYTE* buffer, XDWORD size)
                                            {
                                              bool status = T::WaitToFilledReadingBuffer(size, timeout);
                                              if(!status)    
                                                {
                                                  return 0;    
                                                }

                                              return T::Read(buffer, size);
                                            }


    XDWORD                                  Write                                   (XBYTE* buffer, XDWORD size)
                                            {
                                              if(!size)
                                                {
                                                  return 0;    
                                                }

                                              XDWORD _size = T::Write(buffer, size);
                                              if(!_size)
                                                {
                                                  return 0;    
                                                }
                                              
                                              bool status = T::WaitToFlushOutXBuffer(timeout, false);  
                                              
                                              return status?size:0;
                                            }


    bool                                    Disconnect                              ()
                                            {                                              
                                              return T::Disconnect();
                                            }


    bool                                    Close                                   ()
                                            {                                              
                                              DELETEXTHREAD(XTHREADGROUPID_DIOSTREAMTLS, thread); 
                                              thread = NULL;

                                              return T::Close();
                                            } 

    DIOSTREAMSTATUS                         GetStatus                               ()
                                            {                                              
                                              return T::GetStatus();
                                            }


    int                                     GetTimeout                              ()
                                            {
                                              return timeout;
                                            }


    void                                    SetTimeout                              (int timeout = DIOSTREAMTLS_TIMEOUT)
                                            {
                                              this->timeout = timeout;
                                            }

  private:

     bool                                   HandShake_Client_ClientHello                      ()
                                            {
                                              DIOSTREAMTLS_MSG_RECORD<DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO>>  clienthello_msg;
                                              bool                                                                                        status = false;

                                              status = HandShake_Client_Encode_ClientHello(clienthello_msg);
                                              if(status)
                                                {
                                                  XBUFFER writebuffer;    
                                                  XDWORD  writesize;  

                                                  clienthello_msg.SetToBuffer(writebuffer, true);
  
                                                  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[DIO Stream TLS] ClientHello:"));
                                                  XTRACE_PRINTDATABLOCKCOLOR(XTRACE_COLOR_BLUE, writebuffer);
          
                                                  writesize = Write(writebuffer.Get(), writebuffer.GetSize());      
                                                  if(writesize == writebuffer.GetSize())     
                                                    {
                                                      DIOSTREAMTLS_MSG_RECORD<DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO>>  serverhello_msg;
                                                      XBUFFER                                                                                     readbuffer; 
                                                      XBYTE                                                                                       typemsg       = 0;
                                                      XWORD                                                                                       legacyversion = 0;
                                                      XWORD                                                                                       sizemsg       = 0;

                                                      status = Received_Message(readbuffer, typemsg, legacyversion, sizemsg);                                                              
                                                      if(status)
                                                        {    
                                                          if(typemsg != DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE)
                                                            {
                                                              status = false;          
                                                            }
                                                           else
                                                            { 
                                                              status = HandShake_Client_Decode_ServerHello(readbuffer, serverhello_msg);      
                                                              if(status)
                                                                {
                                                                  

                                                                }                                                         
                                                            }
                                                        }
                                                    }
                                                }

                                              return status;    
                                            }

    bool                                    HandShake_Client_Encode_ClientHello     (DIOSTREAMTLS_MSG_RECORD<DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO>>& message)
                                            {
                                              DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO>*  fragment  = NULL;
                                              DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO*                             body      = NULL;
                                              XBUFFER                                                             writebuffer;
  
                                              fragment  = message.GetFragment();
                                              body      = message.GetFragment()->GetBody();

                                              if(!fragment || !body)
                                                {
                                                  return false;
                                                }

                                              // -----------------------------------------------------------------------------------

                                              message.SetContenType(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE);
                                              message.SetProtocolVersion(DIOSTREAMTLS_MSG_VERSION_TLS_1_2);

                                              fragment->SetMsgType(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_CLIENT_HELLO);

                                              fragment->GetBody()->SetClientVersion(DIOSTREAMTLS_MSG_VERSION_TLS_1_2);

                                              // -----------------------------------------------------------------------------------

                                              if(!GenerateRandom(random))
                                                {
                                                  return false;
                                                }

                                              memcpy(body->GetRandom(), random, DIOSTREAMTLS_MSG_RANDOM_SIZE);  

                                              // -----------------------------------------------------------------------------------
  
                                              GenerateSessionID(sessionID, DIOSTREAMTLS_MSG_SESSIONID_SIZE);
                                              memcpy(body->GetSessionID(), sessionID, DIOSTREAMTLS_MSG_SESSIONID_SIZE);
                                              body->SetSessionIDLength(DIOSTREAMTLS_MSG_SESSIONID_SIZE);
                                              // body->SetSessionIDLength(0);

                                              // -----------------------------------------------------------------------------------

                                              body->GetCipherSuites()->Add(DIOSTREAMTLS_MSG_CIPHER_ECDHE_RSA_WITH_AES_128_GCM_SHA256 /*DIOSTREAMTLS_MSG_CIPHER_RSA_WITH_AES_128_GCM_SHA256*/);
                                              body->GetCipherSuites()->Add(DIOSTREAMTLS_MSG_CIPHER_ECDHE_RSA_WITH_AES_256_GCM_SHA384 /*DIOSTREAMTLS_MSG_CIPHER_RSA_WITH_AES_256_CBC_SHA256*/);
                                              body->GetCipherSuites()->Add(DIOSTREAMTLS_MSG_CIPHER_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 /*DIOSTREAMTLS_MSG_CIPHER_RSA_WITH_AES_128_CBC_SHA*/);
                                              body->GetCipherSuites()->Add(DIOSTREAMTLS_MSG_CIPHER_AES_128_GCM_SHA256);
                                              body->GetCipherSuites()->Add(DIOSTREAMTLS_MSG_CIPHER_AES_256_GCM_SHA384);
                                            
                                              body->SetCiphersuitesLength((XWORD)body->GetCipherSuites()->GetSize() * sizeof(XWORD));

                                              body->SetCompressionLength(0x01);
                                              body->SetCompressionMethod(DIOSTREAMTLS_MSG_COMPRESS_METHOD_NULL);

                                              // -----------------------------------------------------------------------------------

                                              DIOSTREAMTLS_MSG_EXTENSION_SNI* extension_SNI = new DIOSTREAMTLS_MSG_EXTENSION_SNI();
                                              if(extension_SNI)
                                                {
                                                  DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME extension_SNI_servername;
      
                                                  extension_SNI_servername.Name_SetType(0); 
                                                  extension_SNI_servername.Name_GetHost()->Set(__L("www.google.es")); 
                                                  extension_SNI_servername.Name_SetLength(extension_SNI_servername.Name_GetHost()->GetSize());

                                                  extension_SNI->List_Add(&extension_SNI_servername);           
                                                }

                                              body->Extensions_Add((DIOSTREAMTLS_MSG_EXTENSION*)extension_SNI); 

                                              // ---------------------------------------

                                              DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDGROUPS* extension_supportedgroups = new DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDGROUPS();
                                              if(extension_supportedgroups)
                                                { 
                                                  extension_supportedgroups->List_Add(DIOSTREAMTLS_MSG_CURVEID_X25519);
                                                  extension_supportedgroups->List_Add(DIOSTREAMTLS_MSG_CURVEID_SECP256R1);
                                                  extension_supportedgroups->List_Add(DIOSTREAMTLS_MSG_CURVEID_SECP384R1);
                                                }
  
                                              body->Extensions_Add((DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDGROUPS*)extension_supportedgroups); 

                                              // ---------------------------------------

                                              DIOSTREAMTLS_MSG_EXTENSION_ECPOINTFORMATS* extension_ECpointformats = new DIOSTREAMTLS_MSG_EXTENSION_ECPOINTFORMATS();
                                              if(extension_ECpointformats)
                                                { 
                                                  extension_ECpointformats->List_Add(DIOSTREAMTLS_MSG_COMPRESS_METHOD_NULL);
                                                }
  
                                              body->Extensions_Add((DIOSTREAMTLS_MSG_EXTENSION_ECPOINTFORMATS*)extension_ECpointformats); 

                                              // ---------------------------------------

                                              DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS* extension_signaturealgorithms = new DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS();
                                              if(extension_signaturealgorithms)
                                                {  
                                                  extension_signaturealgorithms->List_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PKCS1_SHA256);
                                                  extension_signaturealgorithms->List_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PKCS1_SHA384);
                                                  extension_signaturealgorithms->List_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PKCS1_SHA512);
                                                  extension_signaturealgorithms->List_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP256R1_SHA256);
                                                  extension_signaturealgorithms->List_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA256);
                                                
                                                }
  
                                              body->Extensions_Add((DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS*)extension_signaturealgorithms); 

                                              // ---------------------------------------

                                              DIOSTREAMTLS_MSG_EXTENSION_ALPN* extension_ALPN = new DIOSTREAMTLS_MSG_EXTENSION_ALPN();
                                              if(extension_ALPN)
                                                { 
                                                  extension_ALPN->List_Add(DIOSTREAMTLS_ALPN_TYPE_HTTP_2);
                                                  extension_ALPN->List_Add(DIOSTREAMTLS_ALPN_TYPE_HTTP_1_1);     
                                                }
 
                                              body->Extensions_Add((DIOSTREAMTLS_MSG_EXTENSION*)extension_ALPN); 

                                              // ---------------------------------------

                                              DIOSTREAMTLS_MSG_EXTENSION_EMS* extension_EMS = new DIOSTREAMTLS_MSG_EXTENSION_EMS();
                                              if(extension_EMS)
                                                {

                                                }
 
                                              body->Extensions_Add((DIOSTREAMTLS_MSG_EXTENSION*)extension_EMS); 

                                              // ---------------------------------------

                                              DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS* extension_supportedversions = new DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS();
                                              if(extension_supportedversions)
                                                { 
                                                  extension_supportedversions->List_Add(DIOSTREAMTLS_MSG_VERSION_TLS_1_3);
                                                  extension_supportedversions->List_Add(DIOSTREAMTLS_MSG_VERSION_TLS_1_2);     
                                                } 
  
                                              body->Extensions_Add((DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS*)extension_supportedversions); 

                                              // -----------------------------------------------------------------------------------
  
                                              DIOSTREAMTLS_MSG_EXTENSION_PSKKEYEXCHANGEMODES* extension_PSKkeyexchangemodes = new DIOSTREAMTLS_MSG_EXTENSION_PSKKEYEXCHANGEMODES();
                                              if(extension_PSKkeyexchangemodes)
                                                { 
                                                  extension_PSKkeyexchangemodes->List_Add(DIOSTREAMTLS_MSG_PSKKEYEXCHANGEMODE_PSK);
                                                  extension_PSKkeyexchangemodes->List_Add(DIOSTREAMTLS_MSG_PSKKEYEXCHANGEMODE_PSK_DHE);
                                                }
  
                                              body->Extensions_Add((DIOSTREAMTLS_MSG_EXTENSION_PSKKEYEXCHANGEMODES*)extension_PSKkeyexchangemodes); 

                                              // -----------------------------------------------------------------------------------
  
                                              DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE* extension_keyshare = new DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE();
                                              if(extension_keyshare)
                                                { 
                                                  DIOSTREAMTLS_MSG_EXTENSION_KEY key;
                                                  XBYTE                          datakey[32];

                                                  GenerateRandom(datakey);

                                                  key.SetKeyType(DIOSTREAMTLS_MSG_CURVEID_X25519);
                                                  key.SetLengthKeyData(sizeof(datakey));
                                                  key.GetKeyData()->Add(datakey, sizeof(datakey));

                                                  extension_keyshare->List_Add(&key);
                                                }
  
                                              body->Extensions_Add((DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE*)extension_keyshare); 

                                              message.CalculateLength();                                        
                                              
                                              return true;                                         
                                            }

    bool                                    HandShake_Client_Decode_ServerHello   (XBUFFER& readbuffer, DIOSTREAMTLS_MSG_RECORD<DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO>>& message)
                                            {                                                                                           
                                              XBYTE typemsg;
                                              XWORD legacyversion;
                                              XWORD sizemsg;  
                                              bool  status =  false;
                                                      
                                              readbuffer.Get(typemsg);  
                                              readbuffer.Get(legacyversion);  
                                              readbuffer.Get(sizemsg); 

                                              if(typemsg != DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE)
                                                {
                                                  return false;          
                                                }

                                              if(!sizemsg)
                                                {
                                                  return false;          
                                                }  
                                                      

 
                                              status = true;
      
                                              return status;
                                            }

    bool                                    Received_Message                        (XBUFFER& readbuffer, XBYTE& typemsg, XWORD& legacyversion, XWORD& sizemsg)
                                            {
                                              XBUFFER message;
                                              XDWORD  sizeread;
                                              bool    status =  false;

                                              readbuffer.Resize(5);                                     
                                              sizeread = Read(readbuffer.Get(), readbuffer.GetSize()); 
                                              if(sizeread == readbuffer.GetSize())
                                                { 
                                                  readbuffer.Get(typemsg);  
                                                  readbuffer.Get(legacyversion);  
                                                  readbuffer.Get(sizemsg);                                                    

                                                  message.Resize(sizemsg);  

                                                  sizeread = Read(message.Get(), message.GetSize()); 
                                                  if(sizeread == message.GetSize())
                                                    { 
                                                      readbuffer.Add(message);                                                                                                  
                                                      status = true;   
                                                    }                                                    
                                                } 

                                              if(!status)                         
                                                {
                                                  readbuffer.Empty();
                                                }

                                              return status;
                                            }

    
    bool                                    GenerateRandom                          (XBYTE* random)
                                            {
                                              if(!random)
                                                {
                                                  return false;
                                                }

                                              memset(random, 0, DIOSTREAMTLS_MSG_RANDOM_SIZE);

                                              XDATETIME*  timestamp = GEN_XFACTORY.CreateDateTime();  
                                              bool        status    = false;

                                              if(!timestamp)
                                                {
                                                  return false;
                                                }

                                              if(timestamp->Read())
                                                {
                                                  XDWORD timerstampdata = (XDWORD)timestamp->GetEPOCHFormat();

                                                  memcpy(random, &timerstampdata, sizeof(timerstampdata));

                                                  XRAND* xrand = GEN_XFACTORY.CreateRand();
                                                  if(xrand)
                                                    {
                                                      xrand->Ini();      

                                                      for(XDWORD c=sizeof(timerstampdata); c<DIOSTREAMTLS_MSG_RANDOM_SIZE; c++)
                                                        {
                                                          random[c] = xrand->Max(255);
                                                        }

                                                      status = true;  
                                                    }

                                                  GEN_XFACTORY.DeleteRand(xrand);
                                               }

                                              GEN_XFACTORY.DeleteDateTime(timestamp);

                                              return status;
                                            }


    bool                                    GenerateSessionID                       (XBYTE* sessionID, XBYTE sessionIDlength)
                                            {
                                              if(!sessionID)
                                                {
                                                  return false;
                                                }

                                              memset(sessionID, 0, sessionIDlength);

                                              XDATETIME*  timestamp = GEN_XFACTORY.CreateDateTime();  
                                              bool        status    = false;

                                              if(!timestamp)
                                                {
                                                  return false;
                                                }

                                              if(timestamp->Read())
                                                {
                                                  XDWORD timerstampdata = (XDWORD)timestamp->GetEPOCHFormat();

                                                  memcpy(sessionID, &timerstampdata, sizeof(timerstampdata));

                                                  XRAND* xrand = GEN_XFACTORY.CreateRand();
                                                  if(xrand)
                                                    {
                                                      xrand->Ini();      

                                                      for(XDWORD c=sizeof(timerstampdata); c<sessionIDlength; c++)
                                                        {
                                                          sessionID[c] = xrand->Max(255);
                                                        }

                                                      status = true;  
                                                    }

                                                  GEN_XFACTORY.DeleteRand(xrand);
                                               }

                                              GEN_XFACTORY.DeleteDateTime(timestamp);

                                              return status;
                                            }
    

    bool                                    IniFSMachine                            ()                                                                                         
                                            {
                                              if(!fsmachine)
                                                {
                                                  return false;              
                                                }

                                              if(!fsmachine->AddState(DIOSTREAMTLS_XFSMSTATE_NONE            ,
                                                                      DIOSTREAMTLS_XFSMEVENT_OPEN            ,  DIOSTREAMTLS_XFSMSTATE_OPEN         ,
                                                                      DIOSTREAMTLS_XFSMEVENT_CLOSE           ,  DIOSTREAMTLS_XFSMSTATE_CLOSE        ,
                                                                      XFSMACHINESTATE_EVENTDEFEND)) return false;


                                              if(!fsmachine->AddState(DIOSTREAMTLS_XFSMSTATE_OPEN            ,
                                                                      DIOSTREAMTLS_XFSMEVENT_HANDSHAKE       ,  DIOSTREAMTLS_XFSMSTATE_HANDSHAKE    ,
                                                                      DIOSTREAMTLS_XFSMEVENT_CLOSE           ,  DIOSTREAMTLS_XFSMSTATE_CLOSE        ,
                                                                      XFSMACHINESTATE_EVENTDEFEND)) return false;


                                              if(!fsmachine->AddState(DIOSTREAMTLS_XFSMSTATE_HANDSHAKE       ,
                                                                      DIOSTREAMTLS_XFSMEVENT_CONNECTED       ,  DIOSTREAMTLS_XFSMSTATE_CONNECTED    ,
                                                                      DIOSTREAMTLS_XFSMEVENT_DISCONNECTED    ,  DIOSTREAMTLS_XFSMSTATE_DISCONNECTED ,                                                                      
                                                                      XFSMACHINESTATE_EVENTDEFEND)) return false;


                                              if(!fsmachine->AddState(DIOSTREAMTLS_XFSMSTATE_CONNECTED       ,                                                                      
                                                                      DIOSTREAMTLS_XFSMEVENT_DISCONNECTED    ,  DIOSTREAMTLS_XFSMSTATE_DISCONNECTED ,                                                                      
                                                                      XFSMACHINESTATE_EVENTDEFEND)) return false;


                                              if(!fsmachine->AddState(DIOSTREAMTLS_XFSMSTATE_DISCONNECTED    ,                                                                                                                                    
                                                                      DIOSTREAMTLS_XFSMEVENT_CLOSE           ,  DIOSTREAMTLS_XFSMSTATE_CLOSE        ,
                                                                      XFSMACHINESTATE_EVENTDEFEND)) return false;


                                              if(!fsmachine->AddState(DIOSTREAMTLS_XFSMSTATE_CLOSE           ,
                                                                      DIOSTREAMTLS_XFSMEVENT_NONE            ,  DIOSTREAMTLS_XFSMSTATE_NONE         ,                                                                   
                                                                      XFSMACHINESTATE_EVENTDEFEND)) return false;

                                              return true;  
                                            }


    bool                                    Update                                  ()
                                            {
                                              if(!fsmachine)
                                                {
                                                  return false;              
                                                }

                                              if(fsmachine->GetEvent() == DIOSTREAMTLS_XFSMEVENT_NONE) // Not new event
                                                {
                                                  switch(fsmachine->GetCurrentState())
                                                    {
                                                      case DIOSTREAMTLS_XFSMSTATE_NONE                :  break;
                                                      case DIOSTREAMTLS_XFSMSTATE_OPEN                :  break;
                                                      case DIOSTREAMTLS_XFSMSTATE_HANDSHAKE           :  break;
                                                      case DIOSTREAMTLS_XFSMSTATE_CONNECTED           :  break;
                                                      case DIOSTREAMTLS_XFSMSTATE_DISCONNECTED        :  break;

                                                      case DIOSTREAMTLS_XFSMSTATE_CLOSE               :  fsmachine->SetEvent(DIOSTREAMTLS_XFSMEVENT_NONE);
                                                                                                         break;                                                       
                                                    }
                                                }
                                               else //  New event
                                                {
                                                  if(fsmachine->GetEvent() < DIOSTREAMTLS_LASTEVENT)
                                                    {
                                                      fsmachine->CheckTransition();

                                                      switch(fsmachine->GetCurrentState())
                                                        {
                                                          case DIOSTREAMTLS_XFSMSTATE_NONE            : break;

                                                          case DIOSTREAMTLS_XFSMSTATE_OPEN            : fsmachine->SetEvent(DIOSTREAMTLS_XFSMEVENT_HANDSHAKE);
                                                                                                        break;
                                                           
                                                          case DIOSTREAMTLS_XFSMSTATE_HANDSHAKE       : if(HandShake_Client_ClientHello())
                                                                                                          {
                                                                                                            fsmachine->SetEvent(DIOSTREAMTLS_XFSMEVENT_CONNECTED);
                                                                                                          }
                                                                                                         else
                                                                                                          {
                                                                                                            fsmachine->SetEvent(DIOSTREAMTLS_XFSMEVENT_DISCONNECTED);
                                                                                                          }  
                                                                                                        break;
                                                               
                                                          case DIOSTREAMTLS_XFSMSTATE_CONNECTED       : break;
                                                          
                                                          case DIOSTREAMTLS_XFSMSTATE_DISCONNECTED    : fsmachine->SetEvent(DIOSTREAMTLS_XFSMEVENT_CLOSE);
                                                                                                        break;
                                                           
                                                          case DIOSTREAMTLS_XFSMSTATE_CLOSE           : break;
                                                        }
                                                    }
                                                }

                                              return true;
                                            }


    static void                             ThreadRunFunction                       (void* param)
                                            {
                                              DIOSTREAMTLS* diostreamTLS = (DIOSTREAMTLS*)param;
                                              if(!diostreamTLS) 
                                                {
                                                  return;
                                                }

                                              diostreamTLS->Update();
                                            }


    void                                    Clean                                   ()
                                            {
                                              timeout     = 0;

                                              fsmachine   = NULL;
                                              thread      = NULL;
                                            }


    int                                     timeout;
  
    XFSMACHINE*                             fsmachine;        
    XTHREADCOLLECTED*                       thread;

    XBYTE                                   random[DIOSTREAMTLS_MSG_RANDOM_SIZE];
    XBYTE                                   sessionID[DIOSTREAMTLS_MSG_SESSIONID_SIZE];
};





