/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTLSMessagesHandShakeServerHello.cpp
* 
* @class      DIOSTREAMTLSMESSAGESHANDSHAKESERVERHELLO
* @brief      Data Input/Output Stream TLS (Transport Layer Security) Handshake Server Hello Message class
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#pragma region PRECOMPILATION_DEFINE_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOStreamTLSMessagesHandShakeServerHello.h"

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


#pragma region CLASS_DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::~DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO()
* @brief      Destructor of class
* @ingroup    DATAIO
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::~DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO()
{ 
  Extensions_DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetLegacyVersion()
* @brief      get legacy version
* @ingroup    DATAIO
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetLegacyVersion()
{
  return legacy_version;    
} 
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::SetLegacyVersion(XWORD legacy_version)
* @brief      set legacy version
* @ingroup    DATAIO
* 
* @param[in]  legacy_version : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::SetLegacyVersion(XWORD legacy_version)
{
  this->legacy_version = legacy_version;            
} 


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE* DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetRandom()
* @brief      get random
* @ingroup    DATAIO
* 
* @return     XBYTE* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE* DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetRandom()
{
  return random;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetSessionIDLength()
* @brief      get session IDlength
* @ingroup    DATAIO
* 
* @return     XBYTE : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetSessionIDLength()
{
  return sessionID_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::SetSessionIDLength(XBYTE sessionID_length)
* @brief      set session IDlength
* @ingroup    DATAIO
* 
* @param[in]  sessionID_length : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::SetSessionIDLength(XBYTE sessionID_length)
{
  this->sessionID_length = sessionID_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE* DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetSessionID()
* @brief      get session Id
* @ingroup    DATAIO
* 
* @return     XBYTE* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE* DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetSessionID()
{
  return sessionID;
}

                                          
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetCiphersuitesLength()
* @brief      get ciphersuites length
* @ingroup    DATAIO
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetCiphersuitesLength()
{
  return ciphersuites_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::SetCiphersuitesLength(XWORD ciphersuites_length)
* @brief      set ciphersuites length
* @ingroup    DATAIO
* 
* @param[in]  ciphersuites_length : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::SetCiphersuitesLength(XWORD ciphersuites_length)
{
  this->ciphersuites_length = ciphersuites_length;
}
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XWORD>* DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetCipherSuites()
* @brief      get cipher suites
* @ingroup    DATAIO
* 
* @return     XVECTOR<XWORD>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XWORD>* DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetCipherSuites()
{
  return &ciphersuites;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetCompressionLength()
* @brief      get compression length
* @ingroup    DATAIO
* 
* @return     XBYTE : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetCompressionLength()
{
  return compress_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::SetCompressionLength(XBYTE compress_length)
* @brief      set compression length
* @ingroup    DATAIO
* 
* @param[in]  compress_length : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::SetCompressionLength(XBYTE compress_length)
{
  this->compress_length = compress_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetCompressionMethod()
* @brief      get compression method
* @ingroup    DATAIO
* 
* @return     XBYTE : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetCompressionMethod()
{
  return compress_method;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::SetCompressionMethod(XBYTE compress_method)
* @brief      set compression method
* @ingroup    DATAIO
* 
* @param[in]  compress_method : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::SetCompressionMethod(XBYTE compress_method)
{
  this->compress_method = compress_method;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::Extensions_GetLenght()
* @brief      extensions  get lenght
* @ingroup    DATAIO
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::Extensions_GetLenght()
{
  return extensions_lenght;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::Extensions_SetLenght(XWORD extensions_lenght)
* @brief      extensions  set lenght
* @ingroup    DATAIO
* 
* @param[in]  extensions_lenght : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::Extensions_SetLenght(XWORD extensions_lenght)
{
  this->extensions_lenght = extensions_lenght; 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<DIOSTREAMTLS_MSG_EXTENSION*>* DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::Extensions_GetAll()
* @brief      extensions  get all
* @ingroup    DATAIO
* 
* @return     XVECTOR<DIOSTREAMTLS_MSG_EXTENSION*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<DIOSTREAMTLS_MSG_EXTENSION*>* DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::Extensions_GetAll()
{
  return &extensions;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::Extensions_Add(DIOSTREAMTLS_MSG_EXTENSION* extension)
* @brief      extensions  add
* @ingroup    DATAIO
* 
* @param[in]  extension : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::Extensions_Add(DIOSTREAMTLS_MSG_EXTENSION* extension)
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
* @fn         bool DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::Extensions_DeleteAll()
* @brief      extensions  delete all
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::Extensions_DeleteAll()
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
* @fn         bool DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::SetToBuffer(XBUFFER& buffer, bool showdebug)
* @brief      set to buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::SetToBuffer(XBUFFER& buffer, bool showdebug)
{
  buffer.Add(legacy_version);  

  buffer.Add((XBYTE*)random, DIOSTREAMTLS_MSG_RANDOM_SIZE);  

  buffer.Add(sessionID_length);  
  if(sessionID_length)
    {
      buffer.Add((XBYTE*)sessionID, sessionID_length);  
    }

  buffer.Add(ciphersuites_length);  

  for(XDWORD c=0; c<ciphersuites.GetSize(); c++)
    {
      XWORD ciphersuite = ciphersuites.Get(c);

      buffer.Add(ciphersuite);
    }

  buffer.Add(compress_length);  
  buffer.Add(compress_method);  

  buffer.Add(extensions_lenght);

  for(XDWORD c=0; c<extensions.GetSize(); c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION* extension = extensions.Get(c);
      if(extension)
        {
          extension->SetToBuffer(buffer, showdebug);  
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetFromBuffer(XBUFFER& buffer, bool showdebug)
* @brief      get from buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::GetFromBuffer(XBUFFER& buffer, bool showdebug)
{
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @ingroup    DATAIO
* @note       INTERNAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_HANDSHAKE_SERVERHELLO::Clean()
{
  legacy_version        = 0;

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


