/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOCoreProtocol.cpp
* 
* @class      DIOCOREPROTOCOL
* @brief      Data Input/Output Core Protocol class
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

#include "DIOCoreProtocol.h"

#include "XFactory.h"
#include "XString.h"
#include "XFileJSON.h"
#include "XDateTime.h"

#include "HashCRC32.h"

#include "CompressManager.h"

#include "DIOStream.h"

#include "DIOCoreProtocol_Header.h"
#include "DIOCoreProtocol_CFG.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_DIOCOREPROTOCOL_COMMAND


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_COMMAND::DIOCOREPROTOCOL_COMMAND()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_COMMAND::DIOCOREPROTOCOL_COMMAND()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_COMMAND::~DIOCOREPROTOCOL_COMMAND()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_COMMAND::~DIOCOREPROTOCOL_COMMAND()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOCOREPROTOCOL_COMMAND::GetType()
* @brief      Get type
* @ingroup    DATAIO
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOCOREPROTOCOL_COMMAND::GetType()
{
  return type;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_COMMAND::SetType(int type)
* @brief      Set type
* @ingroup    DATAIO
* 
* @param[in]  type : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_COMMAND::SetType(int type)
{
  this->type = type;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* DIOCOREPROTOCOL_COMMAND::GetTypeString()
* @brief      Get type string
* @ingroup    DATAIO
* 
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOCOREPROTOCOL_COMMAND::GetTypeString()
{
  return &typestr;
}
    
    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_BIDIRECTIONALITYMODE DIOCOREPROTOCOL_COMMAND::GetBidirectionalityMode()
* @brief      Get bidirectionality mode
* @ingroup    DATAIO
* 
* @return     DIOCOREPROTOCOL_BIDIRECTIONALITYMODE : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_BIDIRECTIONALITYMODE DIOCOREPROTOCOL_COMMAND::GetBidirectionalityMode()
{
  return bidirectionalitymode;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_COMMAND::SetBidirectionalityMode(DIOCOREPROTOCOL_BIDIRECTIONALITYMODE bidirectionalitymode)
* @brief      Set bidirectionality mode
* @ingroup    DATAIO
* 
* @param[in]  bidirectionalitymode : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_COMMAND::SetBidirectionalityMode(DIOCOREPROTOCOL_BIDIRECTIONALITYMODE bidirectionalitymode)
{
  this->bidirectionalitymode = bidirectionalitymode;
}

   
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_COMMAND::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_COMMAND::Clean()
{
  type                  = DIOCOREPROTOCOL_COMMAND_TYPE_UNKNOWN;
  typestr.Empty();   
  bidirectionalitymode  = DIOCOREPROTOCOL_BIDIRECTIONALITYMODE_NONE;
}


#pragma endregion


#pragma region CLASS_DIOCOREPROTOCOL_UPDATECLASS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_UPDATECLASS::DIOCOREPROTOCOL_UPDATECLASS()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_UPDATECLASS::DIOCOREPROTOCOL_UPDATECLASS()
{
  Clean();
  
  timerlastupdate = GEN_XFACTORY.CreateTimer();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_UPDATECLASS::~DIOCOREPROTOCOL_UPDATECLASS()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_UPDATECLASS::~DIOCOREPROTOCOL_UPDATECLASS()
{
  if(timerlastupdate)
    {
      GEN_XFACTORY.DeleteTimer(timerlastupdate);
    }

  Clean();
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* DIOCOREPROTOCOL_UPDATECLASS::GetClassName()
* @brief      Get class name
* @ingroup    DATAIO
* 
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOCOREPROTOCOL_UPDATECLASS::GetClassName()
{
  return &classname;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL_UPDATECLASS::InitCache()
* @brief      Init cache
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL_UPDATECLASS::InitCache()
{
  if(classptr)
    {
      return false;
    }

  classptr->HasBeenChanged();
  classptr->SetHasBeenChanged(false);  

  return false;

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSERIALIZABLE* DIOCOREPROTOCOL_UPDATECLASS::GetClassPtr()
* @brief      Get class ptr
* @ingroup    DATAIO
* 
* @return     XSERIALIZABLE* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSERIALIZABLE* DIOCOREPROTOCOL_UPDATECLASS::GetClassPtr()
{
  return classptr;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_UPDATECLASS::SetClassPtr(XSERIALIZABLE* classptr)
* @brief      Set class ptr
* @ingroup    DATAIO
* 
* @param[in]  classptr : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_UPDATECLASS::SetClassPtr(XSERIALIZABLE* classptr)
{
  this->classptr = classptr;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL_UPDATECLASS::IsAsk()
* @brief      Is ask
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL_UPDATECLASS::IsAsk()
{
  return isask;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_UPDATECLASS::SetIsAsk(bool isask)
* @brief      Set is ask
* @ingroup    DATAIO
* 
* @param[in]  isask : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_UPDATECLASS::SetIsAsk(bool isask)
{
  this->isask = isask;  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_BIDIRECTIONALITYMODE DIOCOREPROTOCOL_UPDATECLASS::GetBidirectionalityMode()
* @brief      Get bidirectionality mode
* @ingroup    DATAIO
* 
* @return     DIOCOREPROTOCOL_BIDIRECTIONALITYMODE : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_BIDIRECTIONALITYMODE DIOCOREPROTOCOL_UPDATECLASS::GetBidirectionalityMode()
{
  return bidirectionalitymode;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_UPDATECLASS::SetBidirectionalityMode(DIOCOREPROTOCOL_BIDIRECTIONALITYMODE bidirectionalitymode)
* @brief      Set bidirectionality mode
* @ingroup    DATAIO
* 
* @param[in]  bidirectionalitymode : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_UPDATECLASS::SetBidirectionalityMode(DIOCOREPROTOCOL_BIDIRECTIONALITYMODE bidirectionalitymode)
{
  this->bidirectionalitymode = bidirectionalitymode;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL_UPDATECLASS::IsInitialUpdate()
* @brief      Is initial update
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL_UPDATECLASS::IsInitialUpdate()
{
  return isinitialupdate;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_UPDATECLASS::SetIsInitialUpdate(bool isinitialupdate)
* @brief      Set is initial update
* @ingroup    DATAIO
* 
* @param[in]  isinitialupdate : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_UPDATECLASS::SetIsInitialUpdate(bool isinitialupdate)
{
  this->isinitialupdate = isinitialupdate;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD DIOCOREPROTOCOL_UPDATECLASS::GetTimeToUpdate()
* @brief      Get time to update
* @ingroup    DATAIO
* 
* @return     XDWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOCOREPROTOCOL_UPDATECLASS::GetTimeToUpdate()
{
  return timetoupdate;
}

 
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_UPDATECLASS::SetTimeToUpdate(XDWORD timetoupdate)
* @brief      Set time to update
* @ingroup    DATAIO
* 
* @param[in]  timetoupdate : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_UPDATECLASS::SetTimeToUpdate(XDWORD timetoupdate)
{
  this->timetoupdate = timetoupdate;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XTIMER* DIOCOREPROTOCOL_UPDATECLASS::GetTimerLastUpdate()
* @brief      Get timer last update
* @ingroup    DATAIO
* 
* @return     XTIMER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XTIMER* DIOCOREPROTOCOL_UPDATECLASS::GetTimerLastUpdate()
{
  return timerlastupdate;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_UPDATECLASS::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_UPDATECLASS::Clean()
{  
  classname.Empty();   
  classptr              = NULL;
  isask                 = false;  
  bidirectionalitymode  = DIOCOREPROTOCOL_BIDIRECTIONALITYMODE_NONE;

  isinitialupdate       = false;
  timetoupdate          = 0;
  timerlastupdate       = NULL;
 }


#pragma endregion


#pragma region CLASS_DIOCOREPROTOCOL

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL::DIOCOREPROTOCOL(DIOCOREPROTOCOL_CFG* protocolCFG, DIOSTREAM* diostream)
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* @param[in]  protocolCFG : 
* @param[in]  diostream : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL::DIOCOREPROTOCOL(DIOCOREPROTOCOL_CFG* protocolCFG, DIOSTREAM* diostream)
{
  Clean();

  compressmanager = new COMPRESSMANAGER();
  if(compressmanager)
    {
	    compressor = compressmanager->Create(COMPRESSBASE_TYPE_GZ);
      if(!compressor)
        {
          delete compressmanager;          
        }
    } 

  this->protocolCFG = protocolCFG;
  this->diostream   = diostream;  

  Commands_Add(DIOCOREPROTOCOL_COMMAND_TYPE_HEARTBEAT             , DIOCOREPROTOCOL_COMMAND_TYPE_STRING_HEARTBEAT             , DIOCOREPROTOCOL_BIDIRECTIONALITYMODE_BOTH);
  Commands_Add(DIOCOREPROTOCOL_COMMAND_TYPE_UPDATECLASSINITIALIZED, DIOCOREPROTOCOL_COMMAND_TYPE_STRING_UPDATECLASSINITIALIZED, DIOCOREPROTOCOL_BIDIRECTIONALITYMODE_BOTH);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL::~DIOCOREPROTOCOL()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL::~DIOCOREPROTOCOL()
{
  Commands_DeleteAll();

  UpdateClass_DeleteAll();

  if(compressmanager)
    {
      delete compressmanager;
    }

  if(compressor)
    {
      delete compressor;
    }

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::Ini()
* @brief      Ini
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL::Ini()
{  
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::End()
* @brief      End
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL::End()
{  
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_CFG* DIOCOREPROTOCOL::GetProtocolCFG()
* @brief      Get protocol CFG
* @ingroup    DATAIO
* 
* @return     DIOCOREPROTOCOL_CFG* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_CFG* DIOCOREPROTOCOL::GetProtocolCFG()
{
  return protocolCFG;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAM* DIOCOREPROTOCOL::GetDIOStream()
* @brief      Get DIO stream
* @ingroup    DATAIO
* 
* @return     DIOSTREAM* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAM* DIOCOREPROTOCOL::GetDIOStream()
{
  return diostream;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL::SetDIOStream(DIOSTREAM* diostream)
* @brief      Set DIO stream
* @ingroup    DATAIO
* 
* @param[in]  diostream : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL::SetDIOStream(DIOSTREAM* diostream)
{
  this->diostream = diostream;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::SendMsg(DIOCOREPROTOCOL_HEADER* header, XBUFFER& contentresult)
* @brief      Send msg
* @ingroup    DATAIO
* 
* @param[in]  header : 
* @param[in]  contentresult : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOCOREPROTOCOL::SendMsg(DIOCOREPROTOCOL_HEADER* header, XBUFFER& contentresult)
{
  XBUFFER senddata; 
  XWORD   headersize;
  bool    status   = false;

  if(!header)
    {
      return 0;
    }
    
  if(!GenerateHeaderToSend(header, senddata, &headersize))
    {
      return 0;
    }
           
  senddata.Add(contentresult);

  XDWORD size = senddata.GetSize();
  if(size)
    {
      if(!SendData(senddata))
        {
          return 0;
        }
    }

  return size;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::ReceivedMsg(DIOCOREPROTOCOL_HEADER& header, XBUFFER& content)
* @brief      Received msg
* @ingroup    DATAIO
* 
* @param[in]  header : 
* @param[in]  content : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOCOREPROTOCOL::ReceivedMsg(DIOCOREPROTOCOL_HEADER& header, XBUFFER& content)
{
  if(!protocolCFG)
    {
      return 0;
    }

  if(!diostream)
    {
      return 0;
    }

  XBUFFER* readbuffer = diostream->GetInXBuffer();
  if(!readbuffer)
    {
      return 0;
    }
  
  readbuffer->ResetPosition();

  if(readbuffer->GetSize() < DIOCOREPROTOCOL_HEADER_SIZE_ID)
    {
      return 0;
    }

  XDWORD start    = 0;
  XDWORD sizeread = 0;
  XDWORD index    = 0;
  bool   status   = false;
 
  for(index=0; index<readbuffer->GetSize()-sizeof(XDWORD); index++)
    {
      if(readbuffer->Get((XDWORD&)start, index))
        {
           if(start == DIOCOREPROTOCOL_HEADER_MAGIC_ID)
             {
               break;
             }
        }
    }

  if(readbuffer->Get((XDWORD&)start, index))
    {
      sizeread += sizeof(XDWORD);

      // Compress Data
      if(start == DIOCOREPROTOCOL_HEADER_MAGIC_ID)
        {
          XBUFFER headerbuffer;
          XWORD   sizeheader    = 0;
          XWORD   sizeheadercmp = 0;
          XDWORD  crc32header   = 0;

          if(readbuffer->Get((XWORD&)sizeheader))
            {
              sizeread += sizeof(XWORD);

              if(readbuffer->Get((XWORD&)sizeheadercmp))
                {
                  sizeread += sizeof(XWORD);

                  if(readbuffer->Get((XDWORD&)crc32header))
                    {
                      sizeread += sizeof(XDWORD);
  
                      if(sizeheader)
                        {       
                          XBYTE*    ptrheader       = &readbuffer->Get()[DIOCOREPROTOCOL_HEADER_SIZE_ID];
                          XBYTE*    ptrcontent      = ptrheader;                           
                          HASHCRC32 hashCRC32;
                          XDWORD    crc32headercalc = 0;
  
                          if(sizeheadercmp)
                            {     
                              hashCRC32.Do(ptrheader, sizeheadercmp); 
                            }
                            else
                            {
                              hashCRC32.Do(ptrheader, sizeheader); 
                            } 

                          crc32headercalc = hashCRC32.GetResultCRC32();
   
                          if(crc32headercalc == crc32header)
                            {
                              if(sizeheadercmp)
                                {
                                  headerbuffer.Resize(sizeheader);                                        
                                  status      = compressor->Decompress(ptrheader, sizeheadercmp, &headerbuffer);
                                  ptrcontent += sizeheadercmp;
                                  sizeread   += sizeheadercmp;

                                  if(!status)
                                    {
                                      XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[DIO Core Protocol Header] Error to read msg: to decompress header"));
                                    }
                                }
                               else
                                {
                                  status      = headerbuffer.Add(ptrheader, sizeheader);
                                  ptrcontent += sizeheader;
                                  sizeread   += sizeheader;

                                  if(!status)
                                    {
                                      XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[DIO Core Protocol Header] Error to read msg: to get header"));
                                    }
                                }

                              XFILEJSON* headerxfileJSON = header.GetSerializationXFileJSON();
                              if(headerxfileJSON)
                                {                                                                                                  
                                  header.GetSerializationXFileJSON()->AddBufferLines(XFILETXTFORMATCHAR_UTF8, headerbuffer);
                                  header.GetSerializationXFileJSON()->DecodeAllLines();
                                  header.DoDeserialize();
                                } 

                              if(header.GetContentCompressSize() || header.GetContentSize())
                                {
                                  HASHCRC32 hashCRC32;
                                  XDWORD    crc32contentcalc = 0;

                                  hashCRC32.Do(ptrcontent, header.GetContentCompressSize()?header.GetContentCompressSize():header.GetContentSize());  

                                  crc32contentcalc = hashCRC32.GetResultCRC32();
                              
                                  if(header.GetContentCRC32() == crc32contentcalc)
                                    {        
                                      if(header.GetContentCompressSize())
                                        {
                                          content.Resize(header.GetContentSize());
                                          status    = compressor->Decompress(ptrcontent, header.GetContentCompressSize(), &content);
                                          sizeread += header.GetContentCompressSize();

                                          if(!status)
                                            {
                                              XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[DIO Core Protocol Header] Error to read msg: to decompress content"));
                                            }
                                        }
                                       else
                                        {
                                          if(header.GetContentSize())
                                            {
                                              status = content.Add(ptrcontent, header.GetContentSize());
                                              sizeread += header.GetContentSize();

                                              if(!status)
                                                {
                                                  XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[DIO Core Protocol Header] Error to read msg: to get content"));
                                                }
                                            }
                                        }
                                    }
                                }
                               else
                                {
                                  if(!header.GetContentCRC32())
                                    {
                                      status =  true;
                                    }
                                } 
                            }

                          // ------------------------------------------------------------------------------------------------------------------------
                          /*
                          XBYTE color = XTRACE_COLOR_GREEN;
                          if(GetProtocolCFG()->GetIsServer())
                            {
                              color = XTRACE_COLOR_BLUE;      
                            }

                          XTRACE_PRINTCOLOR(color, __L("READ -----------------------------------------------------------------------------------------------"));   
                          XTRACE_PRINTDATABLOCKCOLOR(color, readbuffer->Get(), sizeread +  index); 
                          */
                          // ------------------------------------------------------------------------------------------------------------------------
                          
                          status = readbuffer->Extract(NULL, 0, sizeread +  index);

                          if(!status)
                            {
                              XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[DIO Core Protocol Header] Error to read msg: eliminate buffer used: %d + %d"), sizeread, index);
                            }

                          return sizeread;  
                        }
                       else
                        {
                          XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[DIO Core Protocol Header] Error to read msg: not read size header")); 
                        }           
                    }
                   else
                    {
                      XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[DIO Core Protocol Header] Error to read msg: not read header CRC")); 
                    }           
                }
               else
                {
                  XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[DIO Core Protocol Header] Error to read msg: not read size header cmp")); 
                }           
            }
           else
            {
              XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[DIO Core Protocol Header] Error to read msg: not read size header")); 
            }           
        }
       else
        {
          XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[DIO Core Protocol Header] Error to read msg: not found magic")); 
        }           
    }
   else
    {
      XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[DIO Core Protocol Header] Error to read msg: read magic")); 
    }
   
  return sizeread;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_HEADER* DIOCOREPROTOCOL::CreateHeader(XUUID* ID_message, DIOCOREPROTOCOL_HEADER_OPERATION operation, XCHAR* operation_param)
* @brief      Create header
* @ingroup    DATAIO
* 
* @param[in]  ID_message : 
* @param[in]  operation : 
* @param[in]  operation_param : 
* 
* @return     DIOCOREPROTOCOL_HEADER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_HEADER* DIOCOREPROTOCOL::CreateHeader(XUUID* ID_message, DIOCOREPROTOCOL_HEADER_OPERATION operation, XCHAR* operation_param)
{
  DIOCOREPROTOCOL_HEADER* header = new DIOCOREPROTOCOL_HEADER();
  if(!header)
    {
      return NULL;
    }

  bool IDmsgempty = false;

  if(ID_message)
    {    
      if(ID_message->IsEmpty())
        {
          IDmsgempty = true;
        }
    }
   else
    {
      return NULL;
    }

  if(IDmsgempty)
    {
      header->SetMessageType(DIOCOREPROTOCOL_HEADER_MESSAGETYPE_REQUEST);
      header->GetIDMessage()->GenerateRandom();

      ID_message->CopyFrom((*header->GetIDMessage()));      
    }
   else
    {      
      header->SetMessageType(DIOCOREPROTOCOL_HEADER_MESSAGETYPE_RESPONSE);
      header->GetIDMessage()->CopyFrom((*ID_message));
    }

  header->SetOperation(operation);
  header->GetOperationParam()->Set(operation_param);
  header->GetDateTimeSend()->Read();

  header->SetContentType(DIOCOREPROTOCOL_HEADER_CONTENTTYPE_NONE);
  header->SetContentSize(0); 
  header->SetContentCRC32(0);

  return header;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_HEADER* DIOCOREPROTOCOL::CreateHeader(XUUID* ID_message, DIOCOREPROTOCOL_HEADER_OPERATION operation, XCHAR* operation_param, XBUFFER* content, XBUFFER* contentresult)
* @brief      Create header
* @ingroup    DATAIO
* 
* @param[in]  ID_message : 
* @param[in]  operation : 
* @param[in]  operation_param : 
* @param[in]  content : 
* @param[in]  contentresult : 
* 
* @return     DIOCOREPROTOCOL_HEADER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_HEADER* DIOCOREPROTOCOL::CreateHeader(XUUID* ID_message, DIOCOREPROTOCOL_HEADER_OPERATION operation, XCHAR* operation_param, XBUFFER* content, XBUFFER* contentresult)
{
  DIOCOREPROTOCOL_HEADER* header = CreateHeader(ID_message, operation, operation_param);
  if(!header)
    {
      return NULL;
    }

  bool havecontent = false;

  if(content)
    {
      if(!content->IsEmpty())
        {  
          header->SetContentType(DIOCOREPROTOCOL_HEADER_CONTENTTYPE_BINARY);
          header->SetContentSize(content->GetSize()); 

          if(!CompressContent(header, (*content), (*contentresult)))
            {                  
              contentresult->CopyFrom((*content));
            }   
  
          HASHCRC32 hashCRC32;

          hashCRC32.Do((*contentresult));  
          header->SetContentCRC32(hashCRC32.GetResultCRC32());

          havecontent = true;
        }
    }

  if(!havecontent)
    {
      header->SetContentType(DIOCOREPROTOCOL_HEADER_CONTENTTYPE_BINARY);
      header->SetContentSize(0); 
      header->SetContentCRC32(0);
    }

  return header;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_HEADER* DIOCOREPROTOCOL::CreateHeader(XUUID* ID_message, DIOCOREPROTOCOL_HEADER_OPERATION operation, XCHAR* operation_param, XSTRING* content, XBUFFER* contentresult)
* @brief      Create header
* @ingroup    DATAIO
* 
* @param[in]  ID_message :  
* @param[in]  operation : 
* @param[in]  operation_param : 
* @param[in]  content : 
* @param[in]  contentresult : 
* 
* @return     DIOCOREPROTOCOL_HEADER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_HEADER* DIOCOREPROTOCOL::CreateHeader(XUUID* ID_message, DIOCOREPROTOCOL_HEADER_OPERATION operation, XCHAR* operation_param, XSTRING* content, XBUFFER* contentresult)
{
  DIOCOREPROTOCOL_HEADER* header = CreateHeader(ID_message, operation, operation_param);
  if(!header)
    {
      return NULL;
    }  

  bool havecontent = false;

  if(content)
    {
      if(!content->IsEmpty())
        {     
          XBUFFER contentbinary;
          content->ConvertToUTF8(contentbinary, false);

          header->SetContentType(DIOCOREPROTOCOL_HEADER_CONTENTTYPE_TEXT);
          header->SetContentSize(contentbinary.GetSize());   

          if(!CompressContent(header, contentbinary, (*contentresult)))
            {                  
              contentresult->CopyFrom(contentbinary);
            }   

          HASHCRC32 hashCRC32;

          hashCRC32.Do((*contentresult));  
          header->SetContentCRC32(hashCRC32.GetResultCRC32());

          havecontent = true;
        }
    }
   
  if(!havecontent)
    {
      header->SetContentType(DIOCOREPROTOCOL_HEADER_CONTENTTYPE_TEXT);
      header->SetContentSize(0);   
      header->SetContentCRC32(0);
    }

  return header;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_HEADER* DIOCOREPROTOCOL::CreateHeader(XUUID* ID_message, DIOCOREPROTOCOL_HEADER_OPERATION operation, XCHAR* operation_param, XFILEJSON* content, XBUFFER* contentresult)
* @brief      Create header
* @ingroup    DATAIO
* 
* @param[in]  ID_message : 
* @param[in]  operation : 
* @param[in]  operation_param : 
* @param[in]  content : 
* @param[in]  contentresult : 
* 
* @return     DIOCOREPROTOCOL_HEADER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_HEADER* DIOCOREPROTOCOL::CreateHeader(XUUID* ID_message, DIOCOREPROTOCOL_HEADER_OPERATION operation, XCHAR* operation_param, XFILEJSON* content, XBUFFER* contentresult)
{
  DIOCOREPROTOCOL_HEADER* header = CreateHeader(ID_message, operation, operation_param);
  if(!header)
    {
      return NULL;
    }  

  bool havecontent = false;

  if(content)
    {
      content->EncodeAllLines(false);  

      if(!content->GetLines()->IsEmpty())
        {  
          XSTRING contentstring;
          XBUFFER contentbinary;   

          content->GetAllInOneLine(contentstring);
          contentstring.ConvertToUTF8(contentbinary);

          header->SetContentType(DIOCOREPROTOCOL_HEADER_CONTENTTYPE_JSON);
          header->SetContentSize(contentbinary.GetSize());   

          if(!CompressContent(header, contentbinary, (*contentresult)))
            {                  
              contentresult->CopyFrom(contentbinary);
            }   

          HASHCRC32 hashCRC32;

          hashCRC32.Do((*contentresult));  
          header->SetContentCRC32(hashCRC32.GetResultCRC32());

          havecontent = true;
        }
    }

  if(!havecontent)
    {
      header->SetContentType(DIOCOREPROTOCOL_HEADER_CONTENTTYPE_JSON);
      header->SetContentSize(0);   
      header->SetContentCRC32(0);
    }

  return header;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::GenerateAuthenticationChallenge(XBUFFER& autentication_challange)
* @brief      Generate authentication challenge
* @ingroup    DATAIO
* 
* @param[in]  autentication_challange : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL::GenerateAuthenticationChallenge(XBUFFER& autentication_challange)
{
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::GenerateAuthenticationResponse(XBUFFER& autentication_challange, XBUFFER& autentication_response)
* @brief      Generate authentication response
* @ingroup    DATAIO
* 
* @param[in]  autentication_challange : 
* @param[in]  autentication_response : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL::GenerateAuthenticationResponse(XBUFFER& autentication_challange, XBUFFER& autentication_response)
{
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::MaskKey(XBYTE* key, int size, XBYTE mask)
* @brief      Mask key
* @ingroup    DATAIO
* 
* @param[in]  key : 
* @param[in]  size : 
* @param[in]  mask : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL::MaskKey(XBYTE* key, int size, XBYTE mask)
{
  if(!key)
    {
      return false;
    }

  for(XDWORD c=0; c<size; c+=2)
    {    
      key[c]   ^= (0xAA + mask);
      key[c+1] ^= (0x55 + mask);
    }
      
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<DIOCOREPROTOCOL_COMMAND*>* DIOCOREPROTOCOL::Commands_GetAll()
* @brief      Commands get all
* @ingroup    DATAIO
* 
* @return     XVECTOR<DIOCOREPROTOCOL_COMMAND*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<DIOCOREPROTOCOL_COMMAND*>* DIOCOREPROTOCOL::Commands_GetAll()
{
  return &commands;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::Commands_Add(XDWORD type, XCHAR* command, DIOCOREPROTOCOL_BIDIRECTIONALITYMODE bidirectionalitymode)
* @brief      Commands add
* @ingroup    DATAIO
* 
* @param[in]  type : 
* @param[in]  command : 
* @param[in]  bidirectionalitymode : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL::Commands_Add(XDWORD type, XCHAR* command, DIOCOREPROTOCOL_BIDIRECTIONALITYMODE bidirectionalitymode)
{
  if(!command)
    {
      return false;
    }

  if(Commands_Get(command))
    {
      return false;
    }
  
  if(Commands_Get(type))
    {
      return false;
    }


  DIOCOREPROTOCOL_COMMAND* coreprotocolcommand = new DIOCOREPROTOCOL_COMMAND();
  if(!coreprotocolcommand)
    {
      return false;
    }
  
  coreprotocolcommand->SetType(type);
  coreprotocolcommand->GetTypeString()->Set(command);
  coreprotocolcommand->SetBidirectionalityMode(bidirectionalitymode);

  return commands.Add(coreprotocolcommand);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD DIOCOREPROTOCOL::Commands_Get(XCHAR* command)
* @brief      Commands get
* @ingroup    DATAIO
* 
* @param[in]  command : 
* 
* @return     XDWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOCOREPROTOCOL::Commands_Get(XCHAR* command)
{
  if(!command)
    {
      return false;
    }

  for(XDWORD c=0; c<commands.GetSize(); c++)
    {
      DIOCOREPROTOCOL_COMMAND* coreprotocolcommand = commands.Get(c);
      if(coreprotocolcommand)
        {
          if(!coreprotocolcommand->GetTypeString()->Compare(command, true))
            {
               return c; 
            }
        }
    }

  return 0;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XCHAR* DIOCOREPROTOCOL::Commands_Get(XDWORD type)
* @brief      Commands get
* @ingroup    DATAIO
* 
* @param[in]  type : 
* 
* @return     XCHAR* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* DIOCOREPROTOCOL::Commands_Get(XDWORD type)
{
  if(!type)
    {
      return NULL;
    }

  for(XDWORD c=0; c<commands.GetSize(); c++)
    {
      DIOCOREPROTOCOL_COMMAND* coreprotocolcommand = commands.Get(c);
      if(coreprotocolcommand)
        {
          if(coreprotocolcommand->GetType() == type)
            {
               return coreprotocolcommand->GetTypeString()->Get(); 
            }
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_COMMAND* DIOCOREPROTOCOL::Commands_GetCoreProtocol(XDWORD type)
* @brief      Commands get core protocol
* @ingroup    DATAIO
* 
* @param[in]  type : 
* 
* @return     DIOCOREPROTOCOL_COMMAND* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_COMMAND* DIOCOREPROTOCOL::Commands_GetCoreProtocol(XDWORD type)
{
  if(!type)
    {
      return NULL;
    }

  for(XDWORD c=0; c<commands.GetSize(); c++)
    {
      DIOCOREPROTOCOL_COMMAND* coreprotocolcommand = commands.Get(c);
      if(coreprotocolcommand)
        {
          if(coreprotocolcommand->GetType() == type)
            {
               return coreprotocolcommand; 
            }
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::Commands_DeleteAll()
* @brief      Commands delete all
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL::Commands_DeleteAll()
{
  if(commands.IsEmpty())
    {
      return false;
    }
    
  commands.DeleteContents();
  commands.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<DIOCOREPROTOCOL_UPDATECLASS*>* DIOCOREPROTOCOL::UpdateClass_GetAll()
* @brief      Update class get all
* @ingroup    DATAIO
* 
* @return     XVECTOR<DIOCOREPROTOCOL_UPDATECLASS*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<DIOCOREPROTOCOL_UPDATECLASS*>*  DIOCOREPROTOCOL::UpdateClass_GetAll()
{
  return &updateclasses;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::UpdateClass_Add(bool isask, XCHAR* classname, XSERIALIZABLE* classptr, bool initupdate, XDWORD timetoupdate, DIOCOREPROTOCOL_BIDIRECTIONALITYMODE bidirectionalitymode)
* @brief      Update class add
* @ingroup    DATAIO
* 
* @param[in]  isask : 
* @param[in]  classname : 
* @param[in]  classptr : 
* @param[in]  initupdate : 
* @param[in]  timetoupdate : 
* @param[in]  bidirectionalitymode : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL::UpdateClass_Add(bool isask, XCHAR* classname, XSERIALIZABLE* classptr, bool initupdate, XDWORD timetoupdate, DIOCOREPROTOCOL_BIDIRECTIONALITYMODE bidirectionalitymode)
{
  if(!classname)
    {
      return false;
    }

  if(!classptr)
    {
      return false;
    }

  if(UpdateClass_Get(classname))
    {
      return false;
    }

  DIOCOREPROTOCOL_UPDATECLASS* updateclass =  new DIOCOREPROTOCOL_UPDATECLASS();
  if(!updateclass)
    {
      return false;
    }

  updateclass->SetIsAsk(isask);
  updateclass->GetClassName()->Set(classname);
  if(updateclass->GetClassName()->IsEmpty())
    {
      delete updateclass;
      return false;
    }

  updateclass->SetClassPtr(classptr);  
  updateclass->SetIsInitialUpdate(initupdate);

  if(initupdate)
    {
      updateclass_nforinitialization++;
    }

  updateclass->SetTimeToUpdate(timetoupdate);
  updateclass->SetBidirectionalityMode(bidirectionalitymode);

  updateclass->InitCache();

  return updateclasses.Add(updateclass);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_UPDATECLASS* DIOCOREPROTOCOL::UpdateClass_Get(XCHAR* classname)
* @brief      Update class get
* @ingroup    DATAIO
* 
* @param[in]  classname : 
* 
* @return     DIOCOREPROTOCOL_UPDATECLASS* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_UPDATECLASS* DIOCOREPROTOCOL::UpdateClass_Get(XCHAR* classname)
{
  if(!classname)
    {
      return NULL;
    }

  for(XDWORD c=0; c<updateclasses.GetSize(); c++)
    {
      DIOCOREPROTOCOL_UPDATECLASS* coreprotocolupdateclass = updateclasses.Get(c);
      if(coreprotocolupdateclass)
        {
          if(!coreprotocolupdateclass->GetClassName()->Compare(classname, true))
            {
              return coreprotocolupdateclass; 
            }
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::UpdateClass_DeleteAll()
* @brief      Update class delete all
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL::UpdateClass_DeleteAll()
{
  if(updateclasses.IsEmpty())
    {
      return false;
    }

  updateclasses.DeleteContents();
  updateclasses.DeleteAll();  

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD DIOCOREPROTOCOL::UpdateClass_GetNForInitialization()
* @brief      Update class get N for initialization
* @ingroup    DATAIO
* 
* @return     XDWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOCOREPROTOCOL::UpdateClass_GetNForInitialization()
{
  return updateclass_nforinitialization;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL::UpdateClass_SetNForInitialization(XDWORD updateclass_nforinitialization)
* @brief      Update class set N for initialization
* @ingroup    DATAIO
* 
* @param[in]  updateclass_nforinitialization : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL::UpdateClass_SetNForInitialization(XDWORD updateclass_nforinitialization)
{
  this->updateclass_nforinitialization = updateclass_nforinitialization;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD DIOCOREPROTOCOL::UpdateClass_GetNInitialized()
* @brief      Update class get N initialized
* @ingroup    DATAIO
* 
* @return     XDWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOCOREPROTOCOL::UpdateClass_GetNInitialized()
{
  return updateclass_ninitialized;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL::UpdateClass_SetNInitialized(XDWORD updateclass_ninitialized)
* @brief      Update class set N initialized
* @ingroup    DATAIO
* 
* @param[in]  updateclass_ninitialized : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL::UpdateClass_SetNInitialized(XDWORD updateclass_ninitialized)
{
  this->updateclass_ninitialized = updateclass_ninitialized;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL::UpdateClass_SetRemoteAllInitialized(bool updateclass_remoteallinitialized)
* @brief      Update class set remote all initialized
* @ingroup    DATAIO
* 
* @param[in]  updateclass_remoteallinitialized : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL::UpdateClass_SetRemoteAllInitialized(bool updateclass_remoteallinitialized)
{
  this->updateclass_remoteallinitialized = updateclass_remoteallinitialized;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::UpdateClass_GetSendAllClassInitializated()
* @brief      Update class get send all class initializated
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL::UpdateClass_GetSendAllClassInitializated()
{
  return updateclass_sendallclassinitializated;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL::UpdateClass_SetSendAllClassInitializated(bool updateclass_sendallclassinitializated)
* @brief      Update class set send all class initializated
* @ingroup    DATAIO
* 
* @param[in]  updateclass_sendallclassinitializated : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL::UpdateClass_SetSendAllClassInitializated(bool updateclass_sendallclassinitializated)
{
  this->updateclass_sendallclassinitializated = updateclass_sendallclassinitializated;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::UpdateClass_IsAllInitialized()
* @brief      Update class is all initialized
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL::UpdateClass_IsAllInitialized()
{
  if((updateclass_nforinitialization == updateclass_ninitialized) && updateclass_remoteallinitialized)
    {
      return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::ShowDebug(bool send, DIOCOREPROTOCOL_HEADER* header, XBUFFER& content, XDWORD sizeallmessage, bool showlongformat)
* @brief      Show debug
* @ingroup    DATAIO
* 
* @param[in]  send : 
* @param[in]  header : 
* @param[in]  content : 
* @param[in]  sizeallmessage : 
* @param[in]  showlongformat : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL::ShowDebug(bool send, DIOCOREPROTOCOL_HEADER* header, XBUFFER& content, XDWORD sizeallmessage, bool showlongformat)
{
  XBUFFER   contentresult;  
  int       colormsg = XTRACE_COLOR_PURPLE;
  XSTRING   title; 
 
  if(!header)
    {
      return false;
    }
 
  if(!send)
    {
      colormsg = XTRACE_COLOR_GREEN;
  
      title.Format(__L("[Net Conn] Read  message [%5d bytes] : "), sizeallmessage); 
    }  
   else 
    {
      title.Format(__L("[Net Conn] Write message [%5d bytes] : "), sizeallmessage);  
    }

  if(!showlongformat)
    {
      XSTRING ID_message;
      XSTRING operationstring;

      header->GetIDMessage()->GetToString(ID_message);
      header->GetOperationToString(operationstring);

      title.AddFormat(__L("%-20s %-15s [%s] "), ID_message.Get(), operationstring.Get(), header->GetOperationParam()->Get()); 

      XTRACE_PRINTCOLOR(colormsg, title.Get()); 
    }
   else
    {
      XTRACE_PRINTCOLOR(colormsg, title.Get()); 

      if(header->GetSerializationXFileJSON()->GetRoot())
        {  
          if(!header->GetSerializationXFileJSON()->GetRoot()->GetValues()->GetSize())
            {
              header->Serialize();
            }
        }

      header->GetSerializationXFileJSON()->ShowTraceJSON(colormsg);

      if(header->GetContentCompressSize())
        {
          contentresult.Resize(header->GetContentSize());
          compressor->Decompress(content.Get(), header->GetContentCompressSize(), &contentresult);
        }
       else  
        {
          contentresult.CopyFrom(content);
        }  
  
      switch(header->GetContentType())
        {
          case DIOCOREPROTOCOL_HEADER_CONTENTTYPE_NONE      : break;
  
          case DIOCOREPROTOCOL_HEADER_CONTENTTYPE_BINARY    : XTRACE_PRINTDATABLOCKCOLOR(colormsg, contentresult);
                                                              break; 
  
          case DIOCOREPROTOCOL_HEADER_CONTENTTYPE_TEXT      : { XSTRING string;
  
                                                                string.ConvertFromUTF8(contentresult);
                                                                XTRACE_PRINTCOLOR(colormsg, string.Get());
                                                              }
                                                              break;
  
          case DIOCOREPROTOCOL_HEADER_CONTENTTYPE_JSON      : { XFILEJSON xfileJSON;   
                                                            
                                                                xfileJSON.AddBufferLines(XFILETXTFORMATCHAR_UTF8, contentresult);
                                                                xfileJSON.DecodeAllLines();
                                                                xfileJSON.ShowTraceJSON(colormsg);                                                                  
                                                              }
                                                              break;
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::GenerateHeaderToSend(DIOCOREPROTOCOL_HEADER* header, XBUFFER& headersenddata, XWORD* headersize)
* @brief      Generate header to send
* @ingroup    DATAIO
* 
* @param[in]  header : 
* @param[in]  headersenddata : 
* @param[in]  headersize : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL::GenerateHeaderToSend(DIOCOREPROTOCOL_HEADER* header, XBUFFER& headersenddata, XWORD* headersize)
{
  XFILEJSON*  headerxfileJSON = NULL;
  XSTRING     headerstring;
  XBUFFER     headerbuffer;  
  XBUFFER     headercmp;
  bool        isheadercmp     = false;  
  bool        status          = false;
  
  if(!header)
    {
      return status;
    }

  headersenddata.Empty();

  headerxfileJSON = header->GetSerializationXFileJSON();
  if(!headerxfileJSON)
    {
      return false;
    }

  status = header->DoSerialize();
  if(!status)
    {    
      return status;
    }
 
  headerxfileJSON->EncodeAllLines(false);  
  headerxfileJSON->GetAllInOneLine(headerstring);

  headerstring.ConvertToUTF8(headerbuffer);

  if(protocolCFG)
    {
      if(protocolCFG->GetCompressHeader())
        {
          if(!compressor->Compress(headerbuffer.Get(), headerbuffer.GetSize(), &headercmp))
            {
              return status;        
            }

          if(headercmp.GetSize() < headerbuffer.GetSize())
            {
              isheadercmp = true;
            }
           else 
            {
              headercmp.Empty();    
            } 
        }
    }
      
  headersenddata.Add((XDWORD)DIOCOREPROTOCOL_HEADER_MAGIC_ID);          
  headersenddata.Add((XWORD)headerbuffer.GetSize()); 
  headersenddata.Add((XWORD)headercmp.GetSize()); 

  HASHCRC32 hashCRC32;
  
  if(isheadercmp)
    {     
      hashCRC32.Do(headercmp); 
    }
   else
    {
      hashCRC32.Do(headerbuffer); 
    } 
   
  headersenddata.Add((XDWORD)hashCRC32.GetResultCRC32());
              
  if(isheadercmp)
    {
      if(headersize)
        {
          (*headersize) = headercmp.GetSize();  
        }

      headersenddata.Add(headercmp); 
    }
   else
    {
      if(headersize)
        {
          (*headersize) = headerbuffer.GetSize();  
        }

      headersenddata.Add(headerbuffer); 
    } 

  return status;  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::SendData(XBUFFER& senddata)
* @brief      Send data
* @ingroup    DATAIO
* 
* @param[in]  senddata : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL::SendData(XBUFFER& senddata)
{
  bool status = false;

  if(!protocolCFG)
    {
      return false;
    }

  if(!diostream)
    {
      return false;
    } 


  // ------------------------------------------------------------------------------------------------------------------------
  /*
  XBYTE color = XTRACE_COLOR_GREEN;
  if(GetProtocolCFG()->GetIsServer())
    {
      color = XTRACE_COLOR_BLUE;      
    }

  XTRACE_PRINTCOLOR(color, __L("WRITE ----------------------------------------------------------------------------------------------"));   
  XTRACE_PRINTDATABLOCKCOLOR(color, senddata); 
  */
  // ------------------------------------------------------------------------------------------------------------------------

     
  status = diostream->Write(senddata);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL::CompressContent(DIOCOREPROTOCOL_HEADER* header, XBUFFER& content, XBUFFER& contentresult)
* @brief      Compress content
* @ingroup    DATAIO
* 
* @param[in]  header : 
* @param[in]  content : 
* @param[in]  contentresult : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL::CompressContent(DIOCOREPROTOCOL_HEADER* header, XBUFFER& content,  XBUFFER& contentresult)
{
  bool status = false;

  if(protocolCFG)
    {    
      if(protocolCFG->GetCompressContent())
        {
          if(content.GetSize() >= protocolCFG->GetMinSizeCompressContent())
            {
              if(compressor->Compress(content.Get(), content.GetSize(), &contentresult))
                {
                  if(content.GetSize() > contentresult.GetSize()) 
                    {
                      header->SetContentCompressSize(contentresult.GetSize());   
                      status = true; 
                    }
                }    
            }
        }
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL::Clean()
{
  protocolCFG                           = NULL;
  diostream                             = NULL;
  
  compressmanager                       = NULL;
  compressor                            = NULL;    	
  
  initialization                        = false;

  updateclass_nforinitialization        = 0;
  updateclass_ninitialized              = 0;
  updateclass_remoteallinitialized      = false;
  updateclass_sendallclassinitializated = false;
}


#pragma endregion

