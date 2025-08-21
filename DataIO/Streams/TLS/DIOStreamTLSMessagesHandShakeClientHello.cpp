/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTLSMessages.cpp
* 
* @class      DIOSTREAMTLSMESSAGES
* @brief      Data Input/Output Stream TLS (Transport Layer Security) Messages class
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
#pragma region PRECOMPILATION_DEFINE_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

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


#pragma region CLASS_DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::~DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO()
* @brief      Destructor of class
* @ingroup    DATAIO
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::~DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO()
{ 
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetClientVersion()
* @brief      get client version
* @ingroup    DATAIO
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetClientVersion()
{
  return clientversion;    
} 
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::SetClientVersion(XWORD clientversion)
* @brief      set client version
* @ingroup    DATAIO
* 
* @param[in]  clientversion : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::SetClientVersion(XWORD clientversion)
{
  this->clientversion = clientversion;            
} 


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE* DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetRandom()
* @brief      get random
* @ingroup    DATAIO
* 
* @return     XBYTE* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE* DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetRandom()
{
  return random;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetSessionIDLength()
* @brief      get session IDlength
* @ingroup    DATAIO
* 
* @return     XBYTE : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetSessionIDLength()
{
  return sessionID_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::SetSessionIDLength(XBYTE sessionID_length)
* @brief      set session IDlength
* @ingroup    DATAIO
* 
* @param[in]  sessionID_length : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::SetSessionIDLength(XBYTE sessionID_length)
{
  this->sessionID_length = sessionID_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE* DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetSessionID()
* @brief      get session Id
* @ingroup    DATAIO
* 
* @return     XBYTE* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE* DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetSessionID()
{
  return sessionID;
}

                                          
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetCiphersuitesLength()
* @brief      get ciphersuites length
* @ingroup    DATAIO
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetCiphersuitesLength()
{
  return ciphersuites_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::SetCiphersuitesLength(XWORD ciphersuites_length)
* @brief      set ciphersuites length
* @ingroup    DATAIO
* 
* @param[in]  ciphersuites_length : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::SetCiphersuitesLength(XWORD ciphersuites_length)
{
  this->ciphersuites_length = ciphersuites_length;
}
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XWORD>* DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetCipherSuites()
* @brief      get cipher suites
* @ingroup    DATAIO
* 
* @return     XVECTOR<XWORD>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XWORD>* DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetCipherSuites()
{
  return &ciphersuites;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetCompressionLength()
* @brief      get compression length
* @ingroup    DATAIO
* 
* @return     XBYTE : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetCompressionLength()
{
  return compress_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::SetCompressionLength(XBYTE compress_length)
* @brief      set compression length
* @ingroup    DATAIO
* 
* @param[in]  compress_length : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::SetCompressionLength(XBYTE compress_length)
{
  this->compress_length = compress_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetCompressionMethod()
* @brief      get compression method
* @ingroup    DATAIO
* 
* @return     XBYTE : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetCompressionMethod()
{
  return compress_method;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::SetCompressionMethod(XBYTE compress_method)
* @brief      set compression method
* @ingroup    DATAIO
* 
* @param[in]  compress_method : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::SetCompressionMethod(XBYTE compress_method)
{
  this->compress_method = compress_method;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::Extensions_GetLenght()
* @brief      extensions  get lenght
* @ingroup    DATAIO
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::Extensions_GetLenght()
{
  return extensions_lenght;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::Extensions_SetLenght(XWORD extensions_lenght)
* @brief      extensions  set lenght
* @ingroup    DATAIO
* 
* @param[in]  extensions_lenght : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::Extensions_SetLenght(XWORD extensions_lenght)
{
  this->extensions_lenght = extensions_lenght; 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<DIOSTREAMTLS_MSG_EXTENSION*>* DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::Extensions_GetAll()
* @brief      extensions  get all
* @ingroup    DATAIO
* 
* @return     XVECTOR<DIOSTREAMTLS_MSG_EXTENSION*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<DIOSTREAMTLS_MSG_EXTENSION*>* DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::Extensions_GetAll()
{
  return &extensions;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::Extensions_Add(DIOSTREAMTLS_MSG_EXTENSION* extension)
* @brief      extensions  add
* @ingroup    DATAIO
* 
* @param[in]  extension : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::Extensions_Add(DIOSTREAMTLS_MSG_EXTENSION* extension)
{
  if(!extension)
    {
      return false;
    }

  extensions.Add(extension);

  Extensions_SetLenght(0);

  for(XDWORD c=0; c<extensions.GetSize(); c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION* extension = extensions.Get(c);
      if(extension)
        {
          Extensions_SetLenght(Extensions_GetLenght() + extension->GetLengthBuffer());          
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::Extensions_DeleteAll()
* @brief      extensions  delete all
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::Extensions_DeleteAll()
{ 
  if(extensions.IsEmpty())
    {
      return true;
    }

  extensions.DeleteContents();
  extensions.DeleteAll();

  Extensions_SetLenght(0);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::SetToBuffer(XBUFFER& buffer)
* @brief      set to buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::SetToBuffer(XBUFFER& buffer)
{
  buffer.Add((XWORD)clientversion);  

  buffer.Add((XBYTE*)random, DIOSTREAMTLS_MSG_RANDOM_SIZE);  

  buffer.Add((XBYTE)sessionID_length);  
  if(sessionID_length)
    {
      buffer.Add((XBYTE*)sessionID, sessionID_length);  
    }

  buffer.Add((XWORD)ciphersuites_length);  

  for(XDWORD c=0; c<ciphersuites.GetSize(); c++)
    {
      XDWORD ciphersuite = ciphersuites.Get(c);

      buffer.Add((XWORD)ciphersuite);
    }

  buffer.Add((XBYTE)compress_length);  
  buffer.Add((XBYTE)compress_method);  

  buffer.Add((XWORD)extensions_lenght);

  for(XDWORD c=0; c<extensions.GetSize(); c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION* extension = extensions.Get(c);
      if(extension)
        {
          extension->SetToBuffer(buffer);  
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetFromBuffer(XBUFFER& buffer)
* @brief      get from buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetFromBuffer(XBUFFER& buffer)
{
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @ingroup    DATAIO
* @note       INTERNAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::Clean()
{
  clientversion         = 0;

  memset(random, 0, DIOSTREAMTLS_MSG_RANDOM_SIZE);

  sessionID_length      = 0;
  memset(sessionID, 0, DIOSTREAMTLS_MSG_SESSIONID_SIZE);  

  ciphersuites_length   = 0;

  compress_length       = 0;
  compress_method       = 0;

  extensions_lenght     = 0;
}


#pragma endregion


#pragma endregion


