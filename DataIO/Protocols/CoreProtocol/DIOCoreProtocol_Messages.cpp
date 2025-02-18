/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOCoreProtocol_Messages.cpp
* 
* @class      DIOCOREPROTOCOL_MESSAGES
* @brief      Data Input/Output Core Protocol Messages class
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

#include "DIOCoreProtocol_Messages.h"

#include "XBase.h"
#include "XFactory.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE


#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_DIOCOREPROTOCOL_MESSAGE


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_MESSAGE::DIOCOREPROTOCOL_MESSAGE()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_MESSAGE::DIOCOREPROTOCOL_MESSAGE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_MESSAGE::~DIOCOREPROTOCOL_MESSAGE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_MESSAGE::~DIOCOREPROTOCOL_MESSAGE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_MESSAGE_TYPE_ACQUISITION DIOCOREPROTOCOL_MESSAGE::GetAcquisitionType()
* @brief      Get acquisition type
* @ingroup    DATAIO
* 
* @return     DIOCOREPROTOCOL_MESSAGE_TYPE_ACQUISITION : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_MESSAGE_TYPE_ACQUISITION DIOCOREPROTOCOL_MESSAGE::GetAcquisitionType()
{
  return acquisitiontype;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_MESSAGE::SetAcquisitionType(DIOCOREPROTOCOL_MESSAGE_TYPE_ACQUISITION acquisitiontype)
* @brief      Set acquisition type
* @ingroup    DATAIO
* 
* @param[in]  acquisitiontype : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_MESSAGE::SetAcquisitionType(DIOCOREPROTOCOL_MESSAGE_TYPE_ACQUISITION acquisitiontype)
{
  this->acquisitiontype = acquisitiontype;
}  
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_HEADER* DIOCOREPROTOCOL_MESSAGE::GetHeader()
* @brief      Get header
* @ingroup    DATAIO
* 
* @return     DIOCOREPROTOCOL_HEADER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_HEADER* DIOCOREPROTOCOL_MESSAGE::GetHeader()
{
  return &header;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBUFFER* DIOCOREPROTOCOL_MESSAGE::GetContent()
* @brief      Get content
* @ingroup    DATAIO
* 
* @return     XBUFFER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* DIOCOREPROTOCOL_MESSAGE::GetContent()
{
  return &content;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL_MESSAGE::IsConsumed()
* @brief      Is consumed
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL_MESSAGE::IsConsumed()
{
  return isconsumed;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_MESSAGE::SetIsConsumed(bool isconsumed)
* @brief      Set is consumed
* @ingroup    DATAIO
* 
* @param[in]  isconsumed : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_MESSAGE::SetIsConsumed(bool isconsumed)
{
  this->isconsumed = isconsumed;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD DIOCOREPROTOCOL_MESSAGE::GetSizeAllMessage()
* @brief      Get size all message
* @ingroup    DATAIO
* 
* @return     XDWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOCOREPROTOCOL_MESSAGE::GetSizeAllMessage()
{
  return sizeallmessage;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_MESSAGE::SetSizeAllMessage(XDWORD sizeallmessage)
* @brief      Set size all message
* @ingroup    DATAIO
* 
* @param[in]  sizeallmessage : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_MESSAGE::SetSizeAllMessage(XDWORD sizeallmessage)
{
  this->sizeallmessage = sizeallmessage;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_MESSAGE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_MESSAGE::Clean()
{                                                            
  acquisitiontype = DIOCOREPROTOCOL_MESSAGE_TYPE_ACQUISITION_UNKNOWN;
  isconsumed      = false;
}


#pragma endregion


#pragma region CLASS_DIOCOREPROTOCOL_MESSAGES


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_MESSAGES::DIOCOREPROTOCOL_MESSAGES()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_MESSAGES::DIOCOREPROTOCOL_MESSAGES()
{
  Clean();

  xmutexmessages = GEN_XFACTORY.Create_Mutex();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_MESSAGES::~DIOCOREPROTOCOL_MESSAGES()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_MESSAGES::~DIOCOREPROTOCOL_MESSAGES()
{
  DeleteAll();

  if(xmutexmessages)
    {
      GEN_XFACTORY.Delete_Mutex(xmutexmessages);
    }

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XMAP<DIOCOREPROTOCOL_MESSAGE*, DIOCOREPROTOCOL_MESSAGE*>* DIOCOREPROTOCOL_MESSAGES::GetAll()
* @brief      Get all
* @ingroup    DATAIO
* 
* @return     XMAP<DIOCOREPROTOCOL_MESSAGE*, : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XMAP<DIOCOREPROTOCOL_MESSAGE*, DIOCOREPROTOCOL_MESSAGE*>*  DIOCOREPROTOCOL_MESSAGES::GetAll()
{
  return &allmessages;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL_MESSAGES::Delete(XUUID* IDmessage)
* @brief      Delete
* @ingroup    DATAIO
* 
* @param[in]  IDmessage : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL_MESSAGES::Delete(XUUID* IDmessage)
{
  if(!IDmessage) 
    {
      return false;
    }

  if(GetAll()->IsEmpty())
    {
      return false;
    }  

  if(xmutexmessages)
    {
      xmutexmessages->Lock();
    }
  

  bool    status  = false;
  XDWORD  c       = 0;

  while(c < GetAll()->GetSize())    
    {
      DIOCOREPROTOCOL_MESSAGE* request            = NULL;
      DIOCOREPROTOCOL_MESSAGE* response           = NULL;
      bool                     statusmessageID[2] = { false, false }; 

      request = (DIOCOREPROTOCOL_MESSAGE*)GetAll()->GetKey(c);    
      if(request)
        {
          if(request->GetHeader())
            {
              if(request->GetHeader()->GetIDMessage()->Compare((*IDmessage)))
                {
                  statusmessageID[0] = true;                  
                }
            }
        }

      response = (DIOCOREPROTOCOL_MESSAGE*)GetAll()->GetElement(c);
      if(response)
        {
          if(response->GetHeader())
            {
              if(response->GetHeader()->GetIDMessage()->Compare((*IDmessage)))
                {
                  statusmessageID[1] = true;                                   
                }
            }
        }
      
      if(statusmessageID[0] && statusmessageID[1])
        {     
          status = GetAll()->Delete(request);
          if(status)
            {                  
              delete request; 
              delete response;

              status  = true;
              break;
            }            
        }  
                
      c++;              
    }
 
  // ----------------------------------------------------------------------------------------------------------------           

  if(!status)
    {
      XSTRING UUID;
      IDmessage->GetToString(UUID);

      XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[DIO Core Protocol Messages] Delete message %s"), UUID.Get());
    }

  // ----------------------------------------------------------------------------------------------------------------       

  if(xmutexmessages)
    {
      xmutexmessages->UnLock();
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL_MESSAGES::DeleteAll()
* @brief      Delete all
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL_MESSAGES::DeleteAll()
{
  if(allmessages.IsEmpty())
    {
      return false;
    }
  
  if(xmutexmessages)
    {
      xmutexmessages->Lock();
    }

  allmessages.DeleteKeyContents();
  allmessages.DeleteElementContents();
 
  bool status = allmessages.DeleteAll();

  if(xmutexmessages)
    {
      xmutexmessages->UnLock();
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOCOREPROTOCOL_MESSAGES::FindRequest(XUUID* IDmessage)
* @brief      Find request
* @ingroup    DATAIO
* 
* @param[in]  IDmessage : 
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOCOREPROTOCOL_MESSAGES::FindRequest(XUUID* IDmessage)
{
  if(xmutexmessages)
    {
      xmutexmessages->Lock();
    }

  int result = NOTFOUND;

  for(XDWORD c=0; c<allmessages.GetSize(); c++)
    {
      DIOCOREPROTOCOL_MESSAGE* message_request = allmessages.GetKey(c);
      if(message_request)
        {
          if(message_request->GetHeader()->GetIDMessage()->Compare((*IDmessage)))
            {
              result = (int)c;
              break;  
            }
        }    
    }

  if(xmutexmessages)
    {
      xmutexmessages->UnLock();
    }

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOCOREPROTOCOL_MESSAGES::FindResponse(XUUID* IDmessage)
* @brief      Find response
* @ingroup    DATAIO
* 
* @param[in]  IDmessage : 
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOCOREPROTOCOL_MESSAGES::FindResponse(XUUID* IDmessage)
{
  if(xmutexmessages)
    {
      xmutexmessages->Lock();
    }

  int result = NOTFOUND;

  for(XDWORD c=0; c<allmessages.GetSize(); c++)
    {
      DIOCOREPROTOCOL_MESSAGE* message_response = allmessages.GetElement(c);
      if(message_response)
        {
          if(message_response->GetHeader()->GetIDMessage()->Compare((*IDmessage)))
            {
              result = (int)c;
              break;
            }
        }    
    }

  if(xmutexmessages)
    {
      xmutexmessages->UnLock();
    }

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOCOREPROTOCOL_MESSAGES::FindRequest(DIOCOREPROTOCOL_HEADER_OPERATION operation, XCHAR* operation_param)
* @brief      Find request
* @ingroup    DATAIO
* 
* @param[in]  operation : 
* @param[in]  operation_param : 
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOCOREPROTOCOL_MESSAGES::FindRequest(DIOCOREPROTOCOL_HEADER_OPERATION operation, XCHAR* operation_param)
{
  if(xmutexmessages)
    {
      xmutexmessages->Lock();
    }

  int result = NOTFOUND;

  for(XDWORD c=0; c<allmessages.GetSize(); c++)
    {
      DIOCOREPROTOCOL_MESSAGE* message_request = allmessages.GetKey(c);
      if(message_request)
        {
          if((message_request->GetHeader()->GetOperation() == operation) && !message_request->GetHeader()->GetOperationParam()->Compare(operation_param))
            {
              result = (int)c;
              break;
            }
        }    
    }

  if(xmutexmessages)
    {
      xmutexmessages->UnLock();
    }

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOCOREPROTOCOL_MESSAGES::FindResponse(DIOCOREPROTOCOL_HEADER_OPERATION operation, XCHAR* operation_param)
* @brief      Find response
* @ingroup    DATAIO
* 
* @param[in]  operation : 
* @param[in]  operation_param : 
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOCOREPROTOCOL_MESSAGES::FindResponse(DIOCOREPROTOCOL_HEADER_OPERATION operation, XCHAR* operation_param)
{
  if(xmutexmessages)
    {
      xmutexmessages->Lock();
    }

  int result = NOTFOUND;

  for(XDWORD c=0; c<allmessages.GetSize(); c++)
    {
      DIOCOREPROTOCOL_MESSAGE* message_response = allmessages.GetElement(c);
      if(message_response)
        {
          if((message_response->GetHeader()->GetOperation() == operation) && !message_response->GetHeader()->GetOperationParam()->Compare(operation_param))
            {
              result = (int)c;
              break;
            }
        }    
    }

  if(xmutexmessages)
    {
      xmutexmessages->UnLock();
    }

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL_MESSAGES::AddRequest(DIOCOREPROTOCOL_MESSAGE* message_request)
* @brief      Add request
* @ingroup    DATAIO
* 
* @param[in]  message_request : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL_MESSAGES::AddRequest(DIOCOREPROTOCOL_MESSAGE* message_request)
{
  if(!message_request)
    {
      return false;
    }

  int   index   = FindRequest(message_request->GetHeader()->GetIDMessage());
  bool  status  = false;

  if(index == NOTFOUND)
    {
      if(xmutexmessages)
        {
          xmutexmessages->Lock();
        }

      status = allmessages.Add(message_request, NULL); 

      if(xmutexmessages)
        {
          xmutexmessages->UnLock();
        }
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL_MESSAGES::AddResponse(DIOCOREPROTOCOL_MESSAGE* message_response)
* @brief      Add response
* @ingroup    DATAIO
* 
* @param[in]  message_response : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL_MESSAGES::AddResponse(DIOCOREPROTOCOL_MESSAGE* message_response)
{
  bool status = false;

  if(!message_response)
    {
      return false;
    }

  int index = FindRequest(message_response->GetHeader()->GetIDMessage());
  if(index != NOTFOUND)
    { 
      if(xmutexmessages)
        {
          xmutexmessages->Lock();
        }
    
      DIOCOREPROTOCOL_MESSAGE* message_request = allmessages.GetKey(index);
      if(message_request)
        {
          status = allmessages.Set(message_request, message_response);  
        }

      if(xmutexmessages)
        {
          xmutexmessages->UnLock();
        }
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL_MESSAGES::ShowDebug(bool isserver)
* @brief      Show debug
* @ingroup    DATAIO
* 
* @param[in]  isserver : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL_MESSAGES::ShowDebug(bool isserver)
{
  if(xmutexmessages)
    {
      xmutexmessages->Lock();
    }

  XBYTE color = XTRACE_COLOR_GREEN;  
  if(isserver)
    {
      color = XTRACE_COLOR_BLUE;  
    }

  XTRACE_PRINTCOLOR(color, __L("-------------------------------------------------------------------------------"));
  XTRACE_PRINTCOLOR(color, __L("Actual Messages: [%d]"), allmessages.GetSize());
  
  for(XDWORD c=0; c<allmessages.GetSize(); c++)
    {
      DIOCOREPROTOCOL_MESSAGE*  message_request  = allmessages.GetKey(c);  
      DIOCOREPROTOCOL_MESSAGE*  message_response = allmessages.GetElement(c);
      XSTRING                   line;

      line.AddFormat(__L(" [%d] "), c);
      XTRACE_PRINTCOLOR(color, __L("%s"), line.Get());

      line.Empty();
      line.AddFormat(__L("%-24s"), __L("request  : "));
      if(message_request)
        {
          XSTRING UUID;
          XSTRING  operation;

          switch(message_request->GetAcquisitionType())
            {
              case DIOCOREPROTOCOL_MESSAGE_TYPE_ACQUISITION_UNKNOWN  : operation = __L("unknown");    break;
              case DIOCOREPROTOCOL_MESSAGE_TYPE_ACQUISITION_READ     : operation = __L("Read");       break;
              case DIOCOREPROTOCOL_MESSAGE_TYPE_ACQUISITION_WRITE    : operation = __L("Write");      break;
            }

          message_request->GetHeader()->GetIDMessage()->GetToString(UUID);  
          line.AddFormat(__L("%-12s %08X %-42s consumed: %-10s"), operation.Get(), message_request, UUID.Get(), message_request->IsConsumed()?__L("Ok"):__L("none"));
        }    
       else
        {
          line.AddFormat(__L("%-12s %08X %-42s consumed: %-10s"), __L("unknown"),  0, __L("NULL"), __L("none"));
        } 

      XTRACE_PRINTCOLOR(color, __L("%s"), line.Get());
      if(message_request)
        {
          XTRACE_PRINTDATABLOCKCOLOR(color, (*message_request->GetContent()));
        }

      line.Empty();
      line.AddFormat(__L("%-24s"), __L("response : "));
      if(message_response)
        {
          XSTRING UUID;
          XSTRING operation;

          switch(message_response->GetAcquisitionType())
            {
              case DIOCOREPROTOCOL_MESSAGE_TYPE_ACQUISITION_UNKNOWN  : operation = __L("unknown");    break;
              case DIOCOREPROTOCOL_MESSAGE_TYPE_ACQUISITION_READ     : operation = __L("Read");       break;
              case DIOCOREPROTOCOL_MESSAGE_TYPE_ACQUISITION_WRITE    : operation = __L("Write");      break;
            }

          message_response->GetHeader()->GetIDMessage()->GetToString(UUID); 
          line.AddFormat(__L("%-12s %08X %-42s consumed: %-10s"), operation.Get(), message_response, UUID.Get(), message_response->IsConsumed()?__L("Ok"):__L("none"));          
        }    
       else
        {
          line.AddFormat(__L("%-12s %08X %-42s consumed: %-10s"), __L("unknown"),  0, __L("NULL"), __L("none"));
        } 
      
      XTRACE_PRINTCOLOR(color, __L("%s"), line.Get());
      if(message_response)
        {      
          XTRACE_PRINTDATABLOCKCOLOR(color, (*message_response->GetContent()));
        }
    }

  if(xmutexmessages)
    {
      xmutexmessages->UnLock();
    }
   
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_MESSAGES::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_MESSAGES::Clean()
{
  xmutexmessages  = NULL;
}


#pragma endregion

