/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTLS.h
* 
* @class      DIOSTREAMTLS
* @brief      Data Input/Output Stream TLS (Transport Layer Security) class
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

#ifndef _DIOSTREAMTLS_H_
#define _DIOSTREAMTLS_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XBase.h"
#include "XFactory.h"
#include "XRand.h"
#include "XFSMachine.h"
#include "XBuffer.h"

#include "DIOFactory.h"
#include "DIOIP.h"
#include "DIOStreamConfig.h"
#include "DIOStream.h"
#include "DIOStreamTLSMessages.h"
#include "DIOStreamTLSMessagesExtension.h"
#include "DIOStreamTLSMessagesHandShakeClientHello.h"
#include "DIOStreamTLSMessagesHandShakeServerHello.h"


#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


enum DIOSTREAMTLSXFSMEVENTS
{
  DIOSTREAMTLS_XFSMEVENT_NONE              = 0 ,
  DIOSTREAMTLS_XFSMEVENT_INI                   ,
  DIOSTREAMTLS_XFSMEVENT_UPDATE                ,
  DIOSTREAMTLS_XFSMEVENT_END                   ,

  DIOSTREAMTLS_LASTEVENT
};


enum DIOSTREAMTLSXFSMSTATES
{
  DIOSTREAMTLS_XFSMSTATE_NONE              = 0 ,
  DIOSTREAMTLS_XFSMSTATE_INI                   ,
  DIOSTREAMTLS_XFSMSTATE_UPDATE                ,
  DIOSTREAMTLS_XFSMSTATE_END                   ,

  DIOSTREAMTLS_LASTSTATE
};


#define DIOSTREAMTLS_TIMEOUT  3       // in seconds

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


class DIOSTREAMTLSCONFIG;

template<typename T>
class DIOSTREAMTLS :  public T
{
  public:
                                            DIOSTREAMTLS                            () 
                                            {
                                              Clean();    
                                              
                                              timeout = DIOSTREAMTLS_TIMEOUT;  
                                            } 
                                       

    virtual                                ~DIOSTREAMTLS                            ()
                                            {                                             
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
                                                      DIOSTREAMTLS_MSG_RECORD<DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO>> clienthello_msg;

                                                      status = Send_HandShake_Client_ClientHello(clienthello_msg);
                                                      if(status)
                                                        {
                                                          DIOSTREAMTLS_MSG_RECORD<DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO>> serverhello_msg;
                                                          
                                                          status = Received_HandShake_Client_ServerHello(serverhello_msg);      
                                                          if(status)
                                                            {


                                                            }                                                        
                                                        }                                                 
                                                    } 
                                                }

                                              if(status) 
                                                {
                                                  thread = CREATEXTHREAD(XTHREADGROUPID_DIOSTREAMTLS, __L("DIOSTREAMTLS::Open"), ThreadRunFunction, (void*)this);
                                                  if(!thread) 
                                                    {
                                                      return false;    
                                                    }

                                                  thread->Ini();
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
                                              
                                              bool status = T::WaitToFlushOutXBuffer(timeout);  
                                              
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

    bool                                    Send_HandShake_Client_ClientHello       (DIOSTREAMTLS_MSG_RECORD<DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO>>& message)
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
                                                  extension_signaturealgorithms->List_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP256R1_SHA256);
                                                  extension_signaturealgorithms->List_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA256);
                                                  extension_signaturealgorithms->List_Add(DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PKCS1_SHA256);
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
                                              
                                              message.SetToBuffer(writebuffer, true);
  
                                              XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[DIO Stream TLS] ClientHello:"));
                                              XTRACE_PRINTDATABLOCKCOLOR(XTRACE_COLOR_BLUE, writebuffer);
          
                                              return Write(writebuffer.Get(), writebuffer.GetSize());                                                         
                                            }

    bool                                    Received_HandShake_Client_ServerHello   (DIOSTREAMTLS_MSG_RECORD<DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO>>& message)
                                            {
                                              XBUFFER readbuffer;
                                              XDWORD  sizeread;
                                              bool    status =  false;

                                              readbuffer.Resize(5);    
                                 
                                              sizeread = Read(readbuffer.Get(), readbuffer.GetSize()); 
                                              if(sizeread == readbuffer.GetSize())
                                                { 
                                                  XBYTE typemsg;
                                                  XWORD legacyversion;
                                                  XWORD sizemsg;  
                                                      
                                                  readbuffer.Get(typemsg);  
                                                  readbuffer.Get(legacyversion);  
                                                  readbuffer.Get(sizemsg);  

                                                  //message.SetLegacyVersion(legacyversion);

                                                  readbuffer.Resize(sizemsg);  

                                                  sizeread = Read(readbuffer.Get(), readbuffer.GetSize()); 
                                                  if(sizeread == readbuffer.GetSize())
                                                    { 

                                              
                                                        

                                                      status = true;   
                                                    }                                                    
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
                                              if(!T::AddState(DIOSTREAMTLS_XFSMSTATE_NONE            ,
                                                              DIOSTREAMTLS_XFSMEVENT_INI             , DIOSTREAMTLS_XFSMSTATE_INI           ,
                                                              DIOSTREAMTLS_XFSMEVENT_END             , DIOSTREAMTLS_XFSMSTATE_END           ,
                                                              XFSMACHINESTATE_EVENTDEFEND)) return false;


                                              if(!T::AddState(DIOSTREAMTLS_XFSMSTATE_INI             ,
                                                              DIOSTREAMTLS_XFSMEVENT_UPDATE          , DIOSTREAMTLS_XFSMSTATE_UPDATE        ,
                                                              DIOSTREAMTLS_XFSMEVENT_END             , DIOSTREAMTLS_XFSMSTATE_END           ,
                                                              XFSMACHINESTATE_EVENTDEFEND)) return false;


                                              if(!T::AddState(DIOSTREAMTLS_XFSMSTATE_UPDATE          ,
                                                              DIOSTREAMTLS_XFSMEVENT_NONE            , DIOSTREAMTLS_XFSMSTATE_NONE          ,                
                                                              DIOSTREAMTLS_XFSMEVENT_END             , DIOSTREAMTLS_XFSMSTATE_END           ,
                                                              XFSMACHINESTATE_EVENTDEFEND)) return false;


                                              if(!T::AddState(DIOSTREAMTLS_XFSMSTATE_END             ,
                                                              DIOSTREAMTLS_XFSMEVENT_NONE            , DIOSTREAMTLS_XFSMSTATE_NONE          ,
                                                              DIOSTREAMTLS_XFSMEVENT_INI             , DIOSTREAMTLS_XFSMSTATE_INI           ,
                                                              XFSMACHINESTATE_EVENTDEFEND)) return false;

                                              return true;  
                                            }


    static void                             ThreadRunFunction                       (void* param)
                                            {
                                              DIOSTREAMTLS* diostreamTLS = (DIOSTREAMTLS*)param;
                                              if(!diostreamTLS) 
                                                {
                                                  return;
                                                }
                                            }


    void                                    Clean                                   ()
                                            {
                                              timeout     = 0;
                                              thread      = NULL;
                                            }


    int                                     timeout;
    XTHREADCOLLECTED*                       thread;

    XBYTE                                   random[DIOSTREAMTLS_MSG_RANDOM_SIZE];
    XBYTE                                   sessionID[DIOSTREAMTLS_MSG_SESSIONID_SIZE];
};


#pragma endregion


#endif

