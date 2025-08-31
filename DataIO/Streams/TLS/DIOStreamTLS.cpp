/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTLS.cpp
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOStreamTLS.h"

#include "XBase.h"
#include "XFactory.h"
#include "XRand.h"

#include "DIOFactory.h"
#include "DIOStreamTCPIP.h"

#include "DIOStreamTLSMessages.h"
#include "DIOStreamTLSMessagesExtension.h"
#include "DIOStreamTLSMessagesHandShakeClientHello.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS::DIOSTREAMTLS(DIOSTREAMCONFIG* config)
* @brief      Constructor of class
* @ingroup    DATAIO
*
* @param[in]  config : 
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS::DIOSTREAMTLS(DIOSTREAMTLSCONFIG* config) : XFSMACHINE(0)
{
  Clean();

  SetConfig((DIOSTREAMCONFIG*)config);

  diostream = (DIOSTREAMTCPIP*)GEN_DIOFACTORY.CreateStreamIO((DIOSTREAMCONFIG*)config);

  if(diostream)
    {
      SetTimeout();
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS::~DIOSTREAMTLS()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS::~DIOSTREAMTLS()
{
  if(diostream) GEN_DIOFACTORY.DeleteStreamIO(diostream);

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMCONFIG* DIOSTREAMTLS::GetConfig()
* @brief      Get config
* @ingroup    DATAIO
*
* @return     DIOSTREAMCONFIG* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMCONFIG* DIOSTREAMTLS::GetConfig()
{
  return (DIOSTREAMCONFIG*)config;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS::SetConfig(DIOSTREAMCONFIG* config)
* @brief      Set config
* @ingroup    DATAIO
*
* @param[in]  config :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS::SetConfig(DIOSTREAMCONFIG* config)
{
  if(!config) return false;
  this->config = (DIOSTREAMTLSCONFIG *)config;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS::Open()
* @brief      Open
* @ingroup    DATAIO
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS::Open()
{
  if(!diostream) 
    {
      return false;
    }

  bool status = diostream->Open();
  if(!status) 
    {
      return false;
    }

  status = diostream->WaitToConnected(timeout);
  if(status)
    {

      thread = CREATEXTHREAD(XTHREADGROUPID_DIOSTREAMTLS, __L("DIOSTREAMTLS::Open"), ThreadRunFunction, (void*)this);
      if(!thread) 
        {
          return false;    
        }

      thread->Ini();

      status = HandShake_Send_Client_Hello();
      if(status)
        {
          status = HandShake_Receive_Server_Hello();
          if(status)
            {
        

            }
        }
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSTREAMTLS::Read(XBYTE* buffer, XDWORD size)
* @brief      Read
* @ingroup    DATAIO
*
* @param[in]  buffer : 
* @param[in]  size : 
*
* @return     XDWORD : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOSTREAMTLS::Read(XBYTE* buffer, XDWORD size)
{
  if(!diostream) return false;
  return diostream->Read(buffer, size);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSTREAMTLS::Write(XBYTE* buffer, XDWORD size)
* @brief      Write
* @ingroup    DATAIO
*
* @param[in]  buffer : 
* @param[in]  size : 
*
* @return     XDWORD : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOSTREAMTLS::Write(XBYTE* buffer, XDWORD size)
{
  if(!diostream) return false;
  return diostream->Write(buffer, size);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS::Disconnect()
* @brief      Disconnect
* @ingroup    DATAIO
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS::Disconnect()
{
  if(!diostream) 
    {
      return false;
    }

  return diostream->Disconnect();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS::Close()
* @brief      Close
* @ingroup    DATAIO
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS::Close()
{
  if(!diostream) 
    {
      return false;
    }

  DELETEXTHREAD(XTHREADGROUPID_DIOSTREAMTLS, thread); 

  thread = NULL;

  return diostream->Close();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMSTATUS DIOSTREAMTLS::GetStatus()
* @brief      Get status
* @ingroup    DATAIO
*
* @return     DIOSTREAMSTATUS : 
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMSTATUS DIOSTREAMTLS::GetStatus()
{
  if(!diostream) return DIOSTREAMSTATUS_DISCONNECTED;

  return diostream->GetStatus();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOSTREAMTLS::GetTimeout()
* @brief      get timeout
* @ingroup    DATAIO
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMTLS::GetTimeout()
{
  return timeout;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS::SetTimeout(int timeout)
* @brief      set timeout
* @ingroup    DATAIO
* 
* @param[in]  timeout : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS::SetTimeout(int timeout)
{
  this->timeout = timeout;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS::HandShake_Send_Client_Hello()
* @brief      hand shake  send  client  hello
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS::HandShake_Send_Client_Hello()
{
  DIOSTREAMTLS_MSG_RECORD<DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO>>  message;
  XBUFFER                                                                                     xbuffer;
  DIOSTREAMTLS_MSG_FRAGMENT<DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO>*                          fragment  = NULL;
  DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO*                                                     body      = NULL;
  bool                                                                                        status    = false;

  fragment  = message.GetFragment();
  body      = message.GetFragment()->GetBody();

  if(!fragment || !body)
    {
      return status;
    }

  // -----------------------------------------------------------------------------------

  message.SetContenType(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE);
  message.SetProtocolVersion(DIOSTREAMTLS_MSG_VERSION_TLS_1_0);

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

  // ---------------------------------------

  message.CalculateLength();

  message.SetToBuffer(xbuffer);

  // -----------------------------------------------------------------------------------

  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[DIO Stream TLS] ClientHello:"));
  XTRACE_PRINTDATABLOCKCOLOR(XTRACE_COLOR_BLUE, xbuffer);

  XDWORD size = Write(xbuffer.Get(), xbuffer.GetSize());
  if(size == xbuffer.GetSize()) 
    {
      status = WaitToFlushOutXBuffer(timeout);      
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS::HandShake_Receive_Server_Hello(int timeout)
* @brief      hand shake  receive  server  hello
* @ingroup    DATAIO
* 
* @param[in]  timeout : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS::HandShake_Receive_Server_Hello(int timeout)
{
  #define READBUFFERSIZE  10

  XBUFFER xbuffer;

  xbuffer.Resize(READBUFFERSIZE);

  if(!WaitToFilledReadingBuffer(READBUFFERSIZE, timeout))
    {
      return false;
    }

  XDWORD size = Read(xbuffer.Get(), xbuffer.GetSize());
  if(size != xbuffer.GetSize()) 
    {
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS::GenerateRandom(XBYTE* random)
* @brief      generate random
* @ingroup    DATAIO
* 
* @param[in]  random : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS::GenerateRandom(XBYTE* random)
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


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS::GenerateSessionID(XBYTE* sessionID, XBYTE sessionIDlength)
* @brief      generate session Id
* @ingroup    DATAIO
* 
* @param[in]  sessionID : 
* @param[in]  sessionIDlength : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS::GenerateSessionID(XBYTE* sessionID, XBYTE sessionIDlength)
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


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS::IniFSMachine()
* @brief      ini FSmachine
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS::IniFSMachine()
{
  if(!AddState( DIOSTREAMTLS_XFSMSTATE_NONE            ,
                DIOSTREAMTLS_XFSMEVENT_INI             , DIOSTREAMTLS_XFSMSTATE_INI           ,
                DIOSTREAMTLS_XFSMEVENT_END             , DIOSTREAMTLS_XFSMSTATE_END           ,
                XFSMACHINESTATE_EVENTDEFEND)) return false;


  if(!AddState( DIOSTREAMTLS_XFSMSTATE_INI             ,
                DIOSTREAMTLS_XFSMEVENT_UPDATE          , DIOSTREAMTLS_XFSMSTATE_UPDATE        ,
                DIOSTREAMTLS_XFSMEVENT_END             , DIOSTREAMTLS_XFSMSTATE_END           ,
                XFSMACHINESTATE_EVENTDEFEND)) return false;


  if(!AddState( DIOSTREAMTLS_XFSMSTATE_UPDATE          ,
                DIOSTREAMTLS_XFSMEVENT_NONE            , DIOSTREAMTLS_XFSMSTATE_NONE          ,                
                DIOSTREAMTLS_XFSMEVENT_END             , DIOSTREAMTLS_XFSMSTATE_END           ,
                XFSMACHINESTATE_EVENTDEFEND)) return false;


  if(!AddState( DIOSTREAMTLS_XFSMSTATE_END             ,
                DIOSTREAMTLS_XFSMEVENT_NONE            , DIOSTREAMTLS_XFSMSTATE_NONE          ,
                DIOSTREAMTLS_XFSMEVENT_INI             , DIOSTREAMTLS_XFSMSTATE_INI           ,
                XFSMACHINESTATE_EVENTDEFEND)) return false;

  return true;  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTCPIP* DIOSTREAMTLS::GetDIOStreamTCPIP()
* @brief      Get DIO stream TCPIP
* @ingroup    DATAIO
* 
* @return     DIOSTREAMTCPIP* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTCPIP* DIOSTREAMTLS::GetDIOStreamTCPIP()
{
  return diostream;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS::ThreadRunFunction(void* param)
* @brief      thread run function
* @ingroup    DATAIO
* 
* @param[in]  param : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS::ThreadRunFunction(void* param)
{
  DIOSTREAMTLS* diostreamTLS = (DIOSTREAMTLS*)param;
  if(!diostreamTLS) 
    {
      return;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLS::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS::Clean()
{
  config      = NULL;  
  diostream   = NULL;

  timeout     = 0;

  thread      = NULL;
}


#pragma endregion
















