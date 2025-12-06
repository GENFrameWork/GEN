/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTLSMessages.cpp
* 
* @class      DIOSTREAMTLSMESSAGES
* @brief      Data Input/Output Stream TLS (Transport Layer Security) Messages class
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

#include "DIOStreamTLSMessagesExtension.h"

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


#pragma region CLASS_DIOSTREAMTLS_MSG_EXTENSION


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION::DIOSTREAMTLS_MSG_EXTENSION()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION::DIOSTREAMTLS_MSG_EXTENSION()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION::~DIOSTREAMTLS_MSG_EXTENSION()
* @brief      Destructor of class
* @ingroup    DATAIO
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION::~DIOSTREAMTLS_MSG_EXTENSION()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_EXTENSION::GetType()
* @brief      get type
* @ingroup    DATAIO
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS_MSG_EXTENSION::GetType()
{
  return type;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION::SetType(XWORD type)
* @brief      set type
* @ingroup    DATAIO
* 
* @param[in]  type : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION::SetType(XWORD type)
{
  this->type = type;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_EXTENSION::GetLength()
* @brief      get length
* @ingroup    DATAIO
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS_MSG_EXTENSION::GetLength()
{
  return length;    
} 
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION::SetLength(XWORD length)
* @brief      set length
* @ingroup    DATAIO
* 
* @param[in]  length : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION::SetLength(XWORD length)
{
  this->length = length; 
} 


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION::SetToBuffer(XBUFFER& buffer, bool showdebug)
* @brief      set to buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION::SetToBuffer(XBUFFER& buffer, bool showdebug)
{
  buffer.Add((XWORD)type);
  buffer.Add((XWORD)length);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION::GetFromBuffer(XBUFFER& buffer, bool showdebug)
* @brief      get from buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION::GetFromBuffer(XBUFFER& buffer, bool showdebug)
{
  buffer.Extract(type);
  buffer.Extract(length);
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @ingroup    DATAIO
* @note       INTERNAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION::Clean()
{
  type    = 0;
  length  = 0;      
}


#pragma endregion


#pragma region CLASS_DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::~DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME()
* @brief      Destructor of class
* @ingroup    DATAIO
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::~DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::Name_GetType()
* @brief      name  get type
* @ingroup    DATAIO
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::Name_GetType()
{
  return name_type;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::Name_SetType(XWORD name_type)
* @brief      name  set type
* @ingroup    DATAIO
* 
* @param[in]  name_type : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::Name_SetType(XWORD name_type)
{
  this->name_type = name_type;
}

            
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::Name_GetLength()
* @brief      name  get length
* @ingroup    DATAIO
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::Name_GetLength()
{
  return name_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::Name_SetLength(XWORD name_length)
* @brief      name  set length
* @ingroup    DATAIO
* 
* @param[in]  name_length : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::Name_SetLength(XWORD name_length)
{
  this->name_length = name_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::Name_GetHost()
* @brief      name  get host
* @ingroup    DATAIO
* 
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::Name_GetHost()
{
  return &hostname;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::CopyTo(DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* servername)
* @brief      copy to
* @ingroup    DATAIO
* 
* @param[in]  servername : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::CopyTo(DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* servername)
{
  if(!servername)
    {
      return false;
    }

  servername->Name_SetType(Name_GetType());
  servername->Name_SetLength(Name_GetLength());
  servername->Name_GetHost()->Set(Name_GetHost()->Get());

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::CopyFrom(DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* servername)
* @brief      copy from
* @ingroup    DATAIO
* 
* @param[in]  servername : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::CopyFrom(DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* servername)
{
  if(!servername)
    {
      return false;
    }

  Name_SetType(servername->Name_GetType());
  Name_SetLength(servername->Name_GetLength());
  Name_GetHost()->Set(servername->Name_GetHost()->Get());

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::SetToBuffer(XBUFFER& buffer, bool showdebug)
* @brief      set to buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::SetToBuffer(XBUFFER& buffer, bool showdebug)
{ 
  XBUFFER bufferstring;
  
  hostname.ConvertToASCII(bufferstring);

  if(bufferstring.GetSize() > 1)
    {
      bufferstring.Resize(bufferstring.GetSize()-1);
    }
   
  name_length = bufferstring.GetSize();

  buffer.Add((XBYTE)name_type);
  buffer.Add((XWORD)name_length);

  return buffer.Add(bufferstring);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::GetFromBuffer(XBUFFER& buffer, bool showdebug)
* @brief      get from buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::GetFromBuffer(XBUFFER& buffer, bool showdebug)
{
  XBYTE _hostname[_MAXSTR];

  memset((XBYTE*)_hostname, 0, _MAXSTR);

  buffer.Extract(name_type);
  buffer.Extract(name_length);

  buffer.Extract((XBYTE*)_hostname, 0, name_length);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @ingroup    DATAIO
* @note       INTERNAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME::Clean()
{
  name_type   = 0;
  name_length = 0;
}


#pragma endregion


#pragma region CLASS_DIOSTREAMTLS_MSG_EXTENSION_SNI


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_SNI::DIOSTREAMTLS_MSG_EXTENSION_SNI()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_SNI::DIOSTREAMTLS_MSG_EXTENSION_SNI()
{
  Clean();

  SetType(DIOSTREAMTLS_MSG_EXTENSION_TYPE_SNI);   
}

                        
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_SNI::~DIOSTREAMTLS_MSG_EXTENSION_SNI()
* @brief      Destructor of class
* @ingroup    DATAIO
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_SNI::~DIOSTREAMTLS_MSG_EXTENSION_SNI()
{
  List_DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_EXTENSION_SNI::List_GetLength()
* @brief      list  get length
* @ingroup    DATAIO
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS_MSG_EXTENSION_SNI::List_GetLength()
{
  return list_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION_SNI::List_SetLength(XWORD list_length)
* @brief      list  set length
* @ingroup    DATAIO
* 
* @param[in]  list_length : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION_SNI::List_SetLength(XWORD list_length)
{
  this->list_length = list_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME*>* DIOSTREAMTLS_MSG_EXTENSION_SNI::List_Get()
* @brief      list  get
* @ingroup    DATAIO
* 
* @return     XVECTOR<DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME*>* DIOSTREAMTLS_MSG_EXTENSION_SNI::List_Get()
{
  return &list;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_SNI::List_Add(DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* servername)
* @brief      list  add
* @ingroup    DATAIO
* 
* @param[in]  servername : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_SNI::List_Add(DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* servername)
{
  DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* _servername =  new DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME();
  if(!_servername)
    {
      return false;
    }
        
  _servername->CopyFrom(servername);

  if(!list.Add(_servername))
    {
      return false;
    }

  SetLength(0);
  List_SetLength(0);

  for(XDWORD c=0; c<list.GetSize(); c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* servername = list.Get(c);
      if(servername)
        {
          List_SetLength(List_GetLength() + servername->Name_GetHost()->GetSize());          
        }
    }

  List_SetLength(List_GetLength() + sizeof(XBYTE) + sizeof(XWORD));

  SetLength(List_GetLength() + sizeof(XWORD));

  return true;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_SNI::List_DeleteAll()
* @brief      list  delete all
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_SNI::List_DeleteAll()
{
  if(list.IsEmpty())
    {
      return false;
    }

  list.DeleteContents();
  list.DeleteAll();

  List_SetLength(0);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_SNI::CopyTo(DIOSTREAMTLS_MSG_EXTENSION_SNI* extension)
* @brief      copy to
* @ingroup    DATAIO
* 
* @param[in]  extension : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_SNI::CopyTo(DIOSTREAMTLS_MSG_EXTENSION_SNI* extension)
{
  if(!extension)
    {
      return false;
    }

  extension->SetType(GetType());  
  extension->SetLength(GetLength());

  extension->List_DeleteAll();

  extension->List_SetLength(List_GetLength());

  for(XDWORD c=0; c<list.GetSize(); c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* servername = list.Get(c);
      if(servername)
        {
          DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* _servername = new DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME();
          if(_servername)
            { 
              servername->CopyTo(_servername);

              extension->List_Add(_servername);
            }
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_SNI::CopyFrom(DIOSTREAMTLS_MSG_EXTENSION_SNI* extension)
* @brief      copy from
* @ingroup    DATAIO
* 
* @param[in]  extension : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_SNI::CopyFrom(DIOSTREAMTLS_MSG_EXTENSION_SNI* extension)
{
  if(!extension)
    {
      return false;
    }

  SetType(extension->GetType());  
  SetLength(extension->GetLength());

  List_DeleteAll();

  List_SetLength(extension->List_GetLength());

  for(XDWORD c=0; c<extension->List_Get()->GetSize(); c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* servername = extension->List_Get()->Get(c);
      if(servername)
        {
          DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* _servername = new DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME();
          if(_servername)
            { 
              _servername->CopyFrom(servername);

              List_Add(_servername);
            }
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_SNI::SetToBuffer(XBUFFER& buffer, bool showdebug)
* @brief      set to buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_SNI::SetToBuffer(XBUFFER& buffer, bool showdebug)
{
  DIOSTREAMTLS_MSG_EXTENSION::SetToBuffer(buffer, showdebug);

  buffer.Add((XWORD)list_length);

  for(XDWORD c=0; c<list.GetSize(); c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* servername = list.Get(c);
      if(servername)
        {
          servername->SetToBuffer(buffer, showdebug);
        }
    }

  return true;
}
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_SNI::GetFromBuffer(XBUFFER& buffer, bool showdebug)
* @brief      get from buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_SNI::GetFromBuffer(XBUFFER& buffer, bool showdebug)
{
  DIOSTREAMTLS_MSG_EXTENSION::GetFromBuffer(buffer, showdebug);

  buffer.Extract(list_length);

  for(XDWORD c=0; c<list_length; c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* servername = new DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME();
      if(servername)
        {
          servername->GetFromBuffer(buffer, showdebug);
        }

      if(!List_Add(servername))
        {
          return false;
        }
    }

  return true;
}

                                               
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION_SNI::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @ingroup    DATAIO
* @note       INTERNAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION_SNI::Clean()
{
  list_length = 0;
}


#pragma endregion


#pragma region CLASS_DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDGROUPS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDGROUPS::DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDGROUPS()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDGROUPS::DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDGROUPS()
{
  SetType(DIOSTREAMTLS_MSG_EXTENSION_TYPE_SUPPORTEDGROUPS); 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDGROUPS::~DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDGROUPS()
* @brief      Destructor of class
* @ingroup    DATAIO
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDGROUPS::~DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDGROUPS()
{
 
}


#pragma endregion


#pragma region CLASS_DIOSTREAMTLS_MSG_EXTENSION_ECPOINTFORMATS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_ECPOINTFORMATS::DIOSTREAMTLS_MSG_EXTENSION_ECPOINTFORMATS()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_ECPOINTFORMATS::DIOSTREAMTLS_MSG_EXTENSION_ECPOINTFORMATS()
{
  SetType(DIOSTREAMTLS_MSG_EXTENSION_TYPE_ECPOINTFORMATS); 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_ECPOINTFORMATS::~DIOSTREAMTLS_MSG_EXTENSION_ECPOINTFORMATS()
* @brief      Destructor of class
* @ingroup    DATAIO
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_ECPOINTFORMATS::~DIOSTREAMTLS_MSG_EXTENSION_ECPOINTFORMATS()
{
 
}


#pragma endregion


#pragma region CLASS_DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS::DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS::DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS()
{
  SetType(DIOSTREAMTLS_MSG_EXTENSION_TYPE_SIGNATUREALGORITHMS); 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS::~DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS()
* @brief      Destructor of class
* @ingroup    DATAIO
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS::~DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS()
{
 
}


#pragma endregion


#pragma region CLASS_DIOSTREAMTLS_MSG_EXTENSION_ALPN


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_ALPN::DIOSTREAMTLS_MSG_EXTENSION_ALPN()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_ALPN::DIOSTREAMTLS_MSG_EXTENSION_ALPN()
{
  Clean();

  SetType(DIOSTREAMTLS_MSG_EXTENSION_TYPE_ALPN);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_ALPN::~DIOSTREAMTLS_MSG_EXTENSION_ALPN()
* @brief      Destructor of class
* @ingroup    DATAIO
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_ALPN::~DIOSTREAMTLS_MSG_EXTENSION_ALPN()
{
  List_DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_EXTENSION_ALPN::List_GetLength()
* @brief      list  get length
* @ingroup    DATAIO
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS_MSG_EXTENSION_ALPN::List_GetLength()
{
  return list_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION_ALPN::List_SetLength(XWORD list_length)
* @brief      list  set length
* @ingroup    DATAIO
* 
* @param[in]  list_length : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION_ALPN::List_SetLength(XWORD list_length)
{
  this->list_length = list_length;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBUFFER* DIOSTREAMTLS_MSG_EXTENSION_ALPN::List_GetBuffer()
* @brief      list  get buffer
* @ingroup    DATAIO
* 
* @return     XBUFFER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* DIOSTREAMTLS_MSG_EXTENSION_ALPN::List_GetBuffer()
{
  return &list_buffer;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_ALPN::List_Add(DIOSTREAMTLS_ALPN_TYPE alpn_type)
* @brief      list  add
* @ingroup    DATAIO
* 
* @param[in]  alpn_type : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_ALPN::List_Add(DIOSTREAMTLS_ALPN_TYPE alpn_type)
{
  switch(alpn_type)
    {  
      case DIOSTREAMTLS_ALPN_TYPE_HTTP_1_1 : 
                             default       :  { XBYTE value[] = { 0x08, 0x68, 0x74, 0x74, 0x70, 0x2F, 0x31, 0x2E, 0x31 };
                                                list_buffer.Add(value, sizeof(value));  
                                              } 
                                              break;

      case DIOSTREAMTLS_ALPN_TYPE_HTTP_2   :  { XBYTE value[] = { 0x02, 0x68, 0x32 };
                                                list_buffer.Add(value, sizeof(value));  
                                              } 
                                              break;

      case DIOSTREAMTLS_ALPN_TYPE_HTTP_3   :  { XBYTE value[] = { 0x02, 0x68, 0x33 };
                                                list_buffer.Add(value, sizeof(value));  
                                              } 
                                              break;      
    }

  if(list_buffer.IsEmpty())
    {
      return false;
    }

  list_length = list_buffer.GetSize();

  SetLength(list_length  + sizeof(XWORD));

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_ALPN::List_DeleteAll()
* @brief      list  delete all
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_ALPN::List_DeleteAll()
{
  return list_buffer.Empty();  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_ALPN::CopyTo(DIOSTREAMTLS_MSG_EXTENSION_ALPN* extension)
* @brief      copy to
* @ingroup    DATAIO
* 
* @param[in]  extension : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_ALPN::CopyTo(DIOSTREAMTLS_MSG_EXTENSION_ALPN* extension)
{
  if(!extension)
    {
      return false;
    }

  extension->SetType(GetType());  
  extension->SetLength(GetLength());

  extension->List_SetLength(List_GetLength());

  extension->List_GetBuffer()->CopyTo(list_buffer);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_ALPN::CopyFrom(DIOSTREAMTLS_MSG_EXTENSION_ALPN* extension)
* @brief      copy from
* @ingroup    DATAIO
* 
* @param[in]  extension : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_ALPN::CopyFrom(DIOSTREAMTLS_MSG_EXTENSION_ALPN* extension)
{
  if(!extension)
    {
      return false;
    }

  SetType(extension->GetType());  
  SetLength(extension->GetLength());

  List_SetLength(extension->List_GetLength());
  
  list_buffer.CopyFrom((*extension->List_GetBuffer()));

  return true;
}    


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_ALPN::SetToBuffer(XBUFFER& buffer, bool showdebug)
* @brief      set to buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_ALPN::SetToBuffer(XBUFFER& buffer, bool showdebug)
{
  DIOSTREAMTLS_MSG_EXTENSION::SetToBuffer(buffer, showdebug);

  buffer.Add((XWORD)list_length);

  buffer.Add(list_buffer);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_ALPN::GetFromBuffer(XBUFFER& buffer, bool showdebug)
* @brief      get from buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_ALPN::GetFromBuffer(XBUFFER& buffer, bool showdebug)
{
  DIOSTREAMTLS_MSG_EXTENSION::GetFromBuffer(buffer, showdebug);

  buffer.Extract(list_length);

  list_buffer.Resize(buffer.GetSize());

  buffer.Extract(list_buffer.Get(), 0, list_buffer.GetSize());

  return true;
}
     

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION_ALPN::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @ingroup    DATAIO
* @note       INTERNAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION_ALPN::Clean()
{
  list_length = 0;

  list_buffer.Empty();   
}


#pragma endregion


#pragma region CLASS_DIOSTREAMTLS_MSG_EXTENSION_EMS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_EMS::DIOSTREAMTLS_MSG_EXTENSION_EMS()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_EMS::DIOSTREAMTLS_MSG_EXTENSION_EMS()
{
  Clean();

  SetType(DIOSTREAMTLS_MSG_EXTENSION_TYPE_EMS); 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_EMS::~DIOSTREAMTLS_MSG_EXTENSION_EMS()
* @brief      Destructor of class
* @ingroup    DATAIO
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_EMS::~DIOSTREAMTLS_MSG_EXTENSION_EMS()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_EMS::CopyTo(DIOSTREAMTLS_MSG_EXTENSION_EMS* extension)
* @brief      copy to
* @ingroup    DATAIO
* 
* @param[in]  extension : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_EMS::CopyTo(DIOSTREAMTLS_MSG_EXTENSION_EMS* extension)
{
  if(!extension)
    {
      return false;
    }

  extension->SetType(GetType());  
  extension->SetLength(GetLength());

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_EMS::CopyFrom(DIOSTREAMTLS_MSG_EXTENSION_EMS* extension)
* @brief      copy from
* @ingroup    DATAIO
* 
* @param[in]  extension : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_EMS::CopyFrom(DIOSTREAMTLS_MSG_EXTENSION_EMS* extension)
{
  if(!extension)
    {
      return false;
    }

  SetType(extension->GetType());  
  SetLength(extension->GetLength());

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_EMS::SetToBuffer(XBUFFER& buffer, bool showdebug)
* @brief      set to buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_EMS::SetToBuffer(XBUFFER& buffer, bool showdebug)
{
  DIOSTREAMTLS_MSG_EXTENSION::SetToBuffer(buffer, showdebug);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_EMS::GetFromBuffer(XBUFFER& buffer, bool showdebug)
* @brief      get from buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_EMS::GetFromBuffer(XBUFFER& buffer, bool showdebug)
{
  DIOSTREAMTLS_MSG_EXTENSION::GetFromBuffer(buffer, showdebug);

  return true;
}
     

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION_EMS::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @ingroup    DATAIO
* @note       INTERNAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION_EMS::Clean()
{
  
}


#pragma endregion


#pragma region CLASS_DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS::DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS::DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS()
{
  SetType(DIOSTREAMTLS_MSG_EXTENSION_TYPE_SUPPORTEDVERSIONS); 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS::~DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS()
* @brief      Destructor of class
* @ingroup    DATAIO
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS::~DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS()
{
 
}


#pragma endregion


#pragma region CLASS_DIOSTREAMTLS_MSG_EXTENSION_PSKKEYEXCHANGEMODES


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_PSKKEYEXCHANGEMODES::DIOSTREAMTLS_MSG_EXTENSION_PSKKEYEXCHANGEMODES()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_PSKKEYEXCHANGEMODES::DIOSTREAMTLS_MSG_EXTENSION_PSKKEYEXCHANGEMODES()
{
  SetType(DIOSTREAMTLS_MSG_EXTENSION_TYPE_PSKKEYEXCHANGEMODES); 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_PSKKEYEXCHANGEMODES::~DIOSTREAMTLS_MSG_EXTENSION_PSKKEYEXCHANGEMODES()
* @brief      Destructor of class
* @ingroup    DATAIO
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_PSKKEYEXCHANGEMODES::~DIOSTREAMTLS_MSG_EXTENSION_PSKKEYEXCHANGEMODES()
{
 
}


#pragma endregion


#pragma region CLASS_DIOSTREAMTLS_MSG_EXTENSION_KEY


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_KEY::DIOSTREAMTLS_MSG_EXTENSION_KEY()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_KEY::DIOSTREAMTLS_MSG_EXTENSION_KEY()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_KEY::~DIOSTREAMTLS_MSG_EXTENSION_KEY()
* @brief      Destructor of class
* @ingroup    DATAIO
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_KEY::~DIOSTREAMTLS_MSG_EXTENSION_KEY()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_EXTENSION_KEY::GetKeyType()
* @brief      get key type
* @ingroup    DATAIO
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS_MSG_EXTENSION_KEY::GetKeyType()
{
  return keytype;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION_KEY::SetKeyType(XWORD keytype)
* @brief      set key type
* @ingroup    DATAIO
* 
* @param[in]  keytype : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION_KEY::SetKeyType(XWORD keytype)
{
  this->keytype = keytype;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_EXTENSION_KEY::GetLength()
* @brief      get length
* @ingroup    DATAIO
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS_MSG_EXTENSION_KEY::GetLengthKeyData()
{
  return lengthkeydata;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION_KEY::SetLengthKeyData(XWORD lengthkeydata)
* @brief      set length key data
* @ingroup    DATAIO
* 
* @param[in]  lengthkeydata : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION_KEY::SetLengthKeyData(XWORD lengthkeydata)
{
  this->lengthkeydata = lengthkeydata;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBUFFER* DIOSTREAMTLS_MSG_EXTENSION_KEY::GetKeyData()
* @brief      get key data
* @ingroup    DATAIO
* 
* @return     XBUFFER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* DIOSTREAMTLS_MSG_EXTENSION_KEY::GetKeyData()
{
  return &keydata;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_KEY::CopyTo(DIOSTREAMTLS_MSG_EXTENSION_KEY* key)
* @brief      copy to
* @ingroup    DATAIO
* 
* @param[in]  key : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_KEY::CopyTo(DIOSTREAMTLS_MSG_EXTENSION_KEY* key)
{ 
  key->SetKeyType(keytype);
  
  key->SetLengthKeyData(lengthkeydata);

  key->GetKeyData()->Empty();
  key->GetKeyData()->Add(keydata);

  return true;
}
  
  
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_KEY::CopyFrom(DIOSTREAMTLS_MSG_EXTENSION_KEY* key)
* @brief      copy from
* @ingroup    DATAIO
* 
* @param[in]  key : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_KEY::CopyFrom(DIOSTREAMTLS_MSG_EXTENSION_KEY* key)
{ 
  SetKeyType(key->GetKeyType());
  
  SetLengthKeyData(key->GetLengthKeyData());
  
  GetKeyData()->Empty();
  GetKeyData()->Add(key->GetKeyData());

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_KEY::SetToBuffer(XBUFFER& buffer, bool showdebug)
* @brief      set to buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_KEY::SetToBuffer(XBUFFER& buffer, bool showdebug)
{ 
  buffer.Add(keytype);
  
  buffer.Add(lengthkeydata); 
  buffer.Add(keydata);

  return true;
}                                           


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_KEY::GetFromBuffer(XBUFFER& buffer, bool showdebug)
* @brief      get from buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_KEY::GetFromBuffer(XBUFFER& buffer, bool showdebug)
{  
  buffer.Extract(keytype);
  
  buffer.Extract(lengthkeydata);

  keydata.Resize(buffer.GetSize());
  buffer.Extract(keydata.Get(), 0, keydata.GetSize());

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION_KEY::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @ingroup    DATAIO
* @note       INTERNAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION_KEY::Clean()
{
  keytype = 0;
  lengthkeydata  = 0;
  keydata.Empty();
}


#pragma endregion


#pragma region CLASS_DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE()
{
  Clean();

  SetType(DIOSTREAMTLS_MSG_EXTENSION_TYPE_KEYSHARE); 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::~DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE()
* @brief      Destructor of class
* @ingroup    DATAIO
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::~DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::List_GetLength()
* @brief      list  get length
* @ingroup    DATAIO
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::List_GetLength()
{
  return list_length;
}                                          


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::List_SetLength(XWORD list_length)
* @brief      list  set length
* @ingroup    DATAIO
* 
* @param[in]  list_length : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::List_SetLength(XWORD list_length)
{
  this->list_length = list_length;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<DIOSTREAMTLS_MSG_EXTENSION_KEY*>* DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::List_Get()
* @brief      list  get
* @ingroup    DATAIO
* 
* @return     XVECTOR<DIOSTREAMTLS_MSG_EXTENSION_KEY*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<DIOSTREAMTLS_MSG_EXTENSION_KEY*>* DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::List_Get()
{
  return &list;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::List_Add(DIOSTREAMTLS_MSG_EXTENSION_KEY* key)
* @brief      list  add
* @ingroup    DATAIO
* 
* @param[in]  key : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::List_Add(DIOSTREAMTLS_MSG_EXTENSION_KEY* key)
{
  DIOSTREAMTLS_MSG_EXTENSION_KEY* _key =  new DIOSTREAMTLS_MSG_EXTENSION_KEY();
  if(!_key)
    {
      return false;
    }
        
  _key->CopyFrom(key);

  if(!list.Add(_key))
    {
      return false;
    }

  SetLength(0);
  List_SetLength(0);

  for(XDWORD c=0; c<list.GetSize(); c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION_KEY* key = list.Get(c);
      if(key)
        {
          List_SetLength(List_GetLength() + key->GetKeyData()->GetSize());          
        }
    }

  List_SetLength(List_GetLength() + sizeof(XWORD) + sizeof(XWORD));

  SetLength(List_GetLength() + sizeof(XWORD));

  return true;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::List_DeleteAll()
* @brief      list  delete all
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::List_DeleteAll()
{
  if(list.IsEmpty())
    {
      return false;
    }

  list.DeleteContents();
  list.DeleteAll();

  List_SetLength(0);

  return true;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::CopyTo(DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE* extension)
* @brief      copy to
* @ingroup    DATAIO
* 
* @param[in]  extension : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::CopyTo(DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE* extension)
{
  if(!extension)
    {
      return false;
    }

  extension->SetType(GetType());  
  extension->SetLength(GetLength());

  extension->List_DeleteAll();

  extension->List_SetLength(List_GetLength());

  for(XDWORD c=0; c<list.GetSize(); c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION_KEY* key = list.Get(c);
      if(key)
        {
          DIOSTREAMTLS_MSG_EXTENSION_KEY* _key = new DIOSTREAMTLS_MSG_EXTENSION_KEY();
          if(_key)
            { 
              key->CopyTo(_key);
              extension->List_Add(_key);
            }
        }
    }

  return true;
}
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::CopyFrom(DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE* extension)
* @brief      copy from
* @ingroup    DATAIO
* 
* @param[in]  extension : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::CopyFrom(DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE* extension)
{
  if(!extension)
    {
      return false;
    }

  SetType(extension->GetType());  
  SetLength(extension->GetLength());

  List_DeleteAll();

  List_SetLength(extension->List_GetLength());

  for(XDWORD c=0; c<extension->List_Get()->GetSize(); c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION_KEY* key = extension->List_Get()->Get(c);
      if(key)
        {
          DIOSTREAMTLS_MSG_EXTENSION_KEY* _key = new DIOSTREAMTLS_MSG_EXTENSION_KEY();
          if(_key)
            { 
              _key->CopyFrom(key);

              List_Add(_key);
            }
        }
    }
  
  return true;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::SetToBuffer(XBUFFER& buffer, bool showdebug)
* @brief      set to buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::SetToBuffer(XBUFFER& buffer, bool showdebug)
{
  DIOSTREAMTLS_MSG_EXTENSION::SetToBuffer(buffer, showdebug);

  buffer.Add((XWORD)list_length);

  for(XDWORD c=0; c<list.GetSize(); c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION_KEY* key = list.Get(c);
      if(key)
        {
          key->SetToBuffer(buffer, showdebug);
        }
    }

  return true;
}

  
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::GetFromBuffer(XBUFFER& buffer, bool showdebug)
* @brief      get from buffer
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* @param[in]  showdebug : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::GetFromBuffer(XBUFFER& buffer, bool showdebug)
{
  DIOSTREAMTLS_MSG_EXTENSION::GetFromBuffer(buffer, showdebug);

  buffer.Extract(list_length);

  for(XDWORD c=0; c<list_length; c++)
    {
      DIOSTREAMTLS_MSG_EXTENSION_KEY* key = new DIOSTREAMTLS_MSG_EXTENSION_KEY();
      if(key)
        {
          key->GetFromBuffer(buffer, showdebug);
        }

      if(!List_Add(key))
        {
          return false;
        }
    }

  return true;
}
                                             
      
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @ingroup    DATAIO
* @note       INTERNAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE::Clean()
{
  list_length = 0;
}
    

#pragma endregion


#pragma endregion

