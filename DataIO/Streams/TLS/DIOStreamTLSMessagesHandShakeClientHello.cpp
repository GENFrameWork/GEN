/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTLSMessagesHandShakeClientHello.cpp
* 
* @class      DIOSTREAMTLSMESSAGESHANDSHAKECLIENTHELLO
* @brief      Data Input/Output Stream TLS (Transport Layer Security) Handshake Client Hello Message class
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

#include "GEN_Defines.h"



/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XTrace.h"

#include "DIOStreamTLSMessagesHandShakeClientHello.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/




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
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::~DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO()
{ 
  Extensions_DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetClientVersion()
* @brief      get client version
* @ingroup    DATAIO
* 
* @return     XWORD : Requested value.
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
* @param[in]  clientversion : Clientversion value.
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
* @return     XBYTE* : Pointer to the requested object; NULL if it is not available.
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
* @return     XBYTE : Requested value.
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
* @param[in]  sessionID_length : Session ID length value.
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
* @return     XBYTE* : Pointer to the requested object; NULL if it is not available.
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
* @return     XWORD : Requested value.
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
* @param[in]  ciphersuites_length : Ciphersuites length value.
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
* @return     XVECTOR<XWORD>* : Pointer to the requested object; NULL if it is not available.
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
* @return     XBYTE : Requested value.
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
* @param[in]  compress_length : Compress length value.
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
* @return     XBYTE : Requested value.
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
* @param[in]  compress_method : Compress method value.
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
* @return     XWORD : Requested value.
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
* @param[in]  extensions_lenght : Extensions lenght value.
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
* @return     XVECTOR<DIOSTREAMTLS_MSG_EXTENSION*>* : Pointer to the requested object; NULL if it is not available.
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
* @param[in]  extension : Extension pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::Extensions_Add(DIOSTREAMTLS_MSG_EXTENSION* extension)
{
  XDWORD extensionslength = 0;

  if(!extension)
    {
      return false;
    }

  for(XDWORD c=0; c<extensions.GetSize(); c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION* currentextension = extensions.Get(c);
      if(currentextension && (currentextension->GetType() == extension->GetType()))
        {
          return false;
        }

      if(currentextension)
        {
          XDWORD extensionlength = currentextension->GetLengthBuffer();
          if(!extensionlength || ((extensionslength + extensionlength) > 0xFFFF))
            {
              return false;
            }

          extensionslength += extensionlength;
        }
    }

  XDWORD extensionlength = extension->GetLengthBuffer();
  if(!extensionlength || ((extensionslength + extensionlength) > 0xFFFF))
    {
      return false;
    }

  extensionslength += extensionlength;

  if(!extensions.Add(extension))
    {
      return false;
    }

  Extensions_SetLenght((XWORD)extensionslength);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::Extensions_DeleteAll()
* @brief      extensions  GEN_DELETE all
* @ingroup    DATAIO
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @fn         bool DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::SetToBuffer(XBUFFER& buffer, bool showdebug)
* @brief      set to buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : Buffer to use.
* @param[in]  showdebug : Showdebug value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::SetToBuffer(XBUFFER& buffer, bool showdebug)
{
  if(sessionID_length > DIOSTREAMTLS_MSG_SESSIONID_SIZE)
    {
      return false;
    }

  if(ciphersuites.IsEmpty() || (ciphersuites.GetSize() > (0xFFFF / sizeof(XWORD))))
    {
      return false;
    }

  XBUFFER extensionbuffer;

  for(XDWORD c=0; c<extensions.GetSize(); c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION* extension = extensions.Get(c);
      if(!extension || !extension->SetToBuffer(extensionbuffer, showdebug))
        {
          return false;
        }
    }

  if(extensionbuffer.GetSize() > 0xFFFF)
    {
      return false;
    }

  ciphersuites_length = (XWORD)(ciphersuites.GetSize() * sizeof(XWORD));
  compress_length      = 1;
  extensions_lenght    = (XWORD)extensionbuffer.GetSize();

  if(!buffer.Add(clientversion))                                      return false;
  if(!buffer.Add((XBYTE*)random, DIOSTREAMTLS_MSG_RANDOM_SIZE))        return false;
  if(!buffer.Add(sessionID_length))                                   return false;
  if(sessionID_length && !buffer.Add((XBYTE*)sessionID, sessionID_length)) return false;
  if(!buffer.Add(ciphersuites_length))                                return false;

  for(XDWORD c=0; c<ciphersuites.GetSize(); c++)
    {
      if(!buffer.Add(ciphersuites.Get(c)))
        {
          return false;
        }
    }

  if(!buffer.Add(compress_length))                                    return false;
  if(!buffer.Add(compress_method))                                    return false;
  if(!buffer.Add(extensions_lenght))                                  return false;
  if(extensions_lenght && !buffer.Add(extensionbuffer))               return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetFromBuffer(XBUFFER& buffer, bool showdebug)
* @brief      get from buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : Buffer to use.
* @param[in]  showdebug : Showdebug value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::GetFromBuffer(XBUFFER& buffer, bool showdebug)
{
  XBUFFER workbuffer;
  XDWORD  sizeconsumed = buffer.GetSize();

  workbuffer.Add(buffer);

  if(workbuffer.GetSize() < (sizeof(XWORD) + DIOSTREAMTLS_MSG_RANDOM_SIZE + sizeof(XBYTE)))
    {
      return false;
    }

  Extensions_DeleteAll();
  ciphersuites.DeleteAll();

  if(!workbuffer.Extract(clientversion)) return false;

  if(workbuffer.Extract(random, 0, DIOSTREAMTLS_MSG_RANDOM_SIZE) != DIOSTREAMTLS_MSG_RANDOM_SIZE)
    {
      return false;
    }

  if(!workbuffer.Extract(sessionID_length)) return false;
  if((sessionID_length > DIOSTREAMTLS_MSG_SESSIONID_SIZE) || (workbuffer.GetSize() < sessionID_length)) return false;

  memset(sessionID, 0, DIOSTREAMTLS_MSG_SESSIONID_SIZE);

  if(sessionID_length && (workbuffer.Extract(sessionID, 0, sessionID_length) != sessionID_length))
    {
      return false;
    }

  if(!workbuffer.Extract(ciphersuites_length)) return false;
  if(!ciphersuites_length || (ciphersuites_length % sizeof(XWORD)) || (workbuffer.GetSize() < ciphersuites_length)) return false;

  for(XDWORD c=0; c<(ciphersuites_length / sizeof(XWORD)); c++)
    {
      XWORD ciphersuite = 0;

      if(!workbuffer.Extract(ciphersuite) || !ciphersuites.Add(ciphersuite))
        {
          ciphersuites.DeleteAll();
          return false;
        }
    }

  if(!workbuffer.Extract(compress_length)) return false;
  if((compress_length != 1) || (workbuffer.GetSize() < compress_length)) return false;
  if(!workbuffer.Extract(compress_method)) return false;

  if(workbuffer.IsEmpty())
    {
      extensions_lenght = 0;
      return (buffer.Extract(NULL, 0, sizeconsumed) == sizeconsumed);
    }

  if(!workbuffer.Extract(extensions_lenght)) return false;
  if(workbuffer.GetSize() != extensions_lenght) return false;

  while(!workbuffer.IsEmpty())
    {
      DIOSTREAMTLS_MSG_EXTENSION* extension = NULL;

      if(!DIOSTREAMTLS_MSG_EXTENSION_Extract(workbuffer, DIOSTREAMTLS_MSG_EXTENSION_CONTEXT_CLIENTHELLO, extension))
        {
          Extensions_DeleteAll();
          return false;
        }

      if(!Extensions_Add(extension))
        {
          GEN_DELETE extension;
          Extensions_DeleteAll();
          return false;
        }
    }

  Extensions_SetLenght(extensions_lenght);

  return (buffer.Extract(NULL, 0, sizeconsumed) == sizeconsumed);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_HANDSHAKE_CLIENTHELLO::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
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



