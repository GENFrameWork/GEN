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
DIOSTREAMTLS::DIOSTREAMTLS(DIOSTREAMTLSCONFIG* config) 
{
  Clean();

  SetConfig((DIOSTREAMCONFIG*)config);

  diostream = (DIOSTREAMTCPIP*)GEN_DIOFACTORY.CreateStreamIO((DIOSTREAMCONFIG*)config);
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

  status = diostream->WaitToConnected(5);
  if(status)
    {
      status = HandShake_Client_Hello();
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
  if(!diostream) return false;
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
  if(!diostream) return false;
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
* @fn         bool DIOSTREAMTLS::HandShake_Client_Hello()
* @brief      Hand shake client hello
* @ingroup    DATAIO
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS::HandShake_Client_Hello()
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

  message.SetContenType(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE);
  message.SetProtocolVersion(DIOSTREAMTLS_MSG_VERSION_TLS_1_2);

  fragment->SetMsgType(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_CLIENT_HELLO);

  fragment->GetBody()->SetClientVersion(DIOSTREAMTLS_MSG_VERSION_TLS_1_2);

  if(!GenerateRandom(random))
    {
      return false;
    }

  memcpy(body->GetRandom(), random, DIOSTREAMTLS_MSG_RANDOM_SIZE);
  
  body->SetSessionIDLength(DIOSTREAMTLS_MSG_SESSIONID_SIZE);

  GenerateSessionID(sessionID, body->GetSessionIDLength());
  memcpy(body->GetSessionID(), sessionID, DIOSTREAMTLS_MSG_SESSIONID_SIZE);

  body->GetCipherSuites()->Add((XWORD)DIOSTREAMTLS_MSG_CIPHER_RSA_WITH_AES_128_GCM_SHA256);
  body->GetCipherSuites()->Add((XWORD)DIOSTREAMTLS_MSG_CIPHER_RSA_WITH_AES_256_CBC_SHA256);
  body->GetCipherSuites()->Add((XWORD)DIOSTREAMTLS_MSG_CIPHER_RSA_WITH_AES_128_CBC_SHA);
  body->SetCiphersuitesLength((XWORD)body->GetCipherSuites()->GetSize());

  body->SetCompressionLength(0x01);
  body->SetCompressionMethod(DIOSTREAMTLS_MSG_COMPRESS_METHOD_NULL);

  body->Extensions_SetLenght(0x0000);

  DIOSTREAMTLS_MSG_EXTENSION_SNI* extension_SNI = new DIOSTREAMTLS_MSG_EXTENSION_SNI();
  if(extension_SNI)
    {
      DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME extension_SNI_servername;

      extension_SNI->List_SetLength(1);

      extension_SNI_servername.Name_SetType(0); 
      extension_SNI_servername.Name_GetHost()->Set(__L("www.example.com")); 
      extension_SNI_servername.Name_SetLength(extension_SNI_servername.Name_GetHost()->GetSize());

      extension_SNI->List_Add(&extension_SNI_servername);

      body->Extensions_Add((DIOSTREAMTLS_MSG_EXTENSION*)extension_SNI);
    }


  message.CalculateLength();

  message.SetToBuffer(xbuffer);

  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[DIO Stream TLS] ClientHello:"));
  XTRACE_PRINTDATABLOCKCOLOR(XTRACE_COLOR_BLUE, xbuffer);

  XDWORD size = Write(xbuffer.Get(), xbuffer.GetSize());
  if(size == xbuffer.GetSize()) 
    {
      status = true;
    }

  return status;
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

  memset(sessionID, 0, DIOSTREAMTLS_MSG_SESSIONID_SIZE);

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

          for(XDWORD c=sizeof(timerstampdata); c<DIOSTREAMTLS_MSG_SESSIONID_SIZE; c++)
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
}


#pragma endregion
















