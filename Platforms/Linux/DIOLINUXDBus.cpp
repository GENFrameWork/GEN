/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOLINUXDBus.cpp
* 
* @class      DIOLINUXDBUS
* @brief      LINUX Data Input/Output DBus class
* @ingroup    PLATFORM_LINUX
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

/*---- PRECOMPILATION INCLUDES ----------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOLINUXDBus.h"

#include "XFactory.h"
#include "XTrace.h"
#include "XThreadCollected.h"
#include "XSleep.h"

#include "DIOLINUXDBus_XEvent.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


#pragma region CLASS_DIOLINUXDBUS_MESSAGE


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOLINUXDBUS_MESSAGE::DIOLINUXDBUS_MESSAGE()
* @brief      Constructor
* @ingroup    PLATFORM_LINUX
*
* ---------------------------------------------------------------------------------------------------------------------*/
DIOLINUXDBUS_MESSAGE::DIOLINUXDBUS_MESSAGE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOLINUXDBUS_MESSAGE::~DIOLINUXDBUS_MESSAGE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
*
* ---------------------------------------------------------------------------------------------------------------------*/
DIOLINUXDBUS_MESSAGE::~DIOLINUXDBUS_MESSAGE()
{
  Delete();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* DIOLINUXDBUS_MESSAGE::GetDestination()
* @brief      GetDestination
* @ingroup    PLATFORM_LINUX
*
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOLINUXDBUS_MESSAGE::GetDestination()
{
  return &destination;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* DIOLINUXDBUS_MESSAGE::GetPath()
* @brief      GetPath
* @ingroup    PLATFORM_LINUX
*
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOLINUXDBUS_MESSAGE::GetPath()
{
  return &path;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* DIOLINUXDBUS_MESSAGE::GetIFace()
* @brief      GetIFace
* @ingroup    PLATFORM_LINUX
*
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOLINUXDBUS_MESSAGE::GetIFace()
{
  return &iface;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* DIOLINUXDBUS_MESSAGE::GetMethod()
* @brief      GetMethod
* @ingroup    PLATFORM_LINUX
*
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOLINUXDBUS_MESSAGE::GetMethod()
{
  return &method;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS_MESSAGE::Create()
* @brief      Create
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS_MESSAGE::Create()
{
  bool status = false;

  XBUFFER _destination;
  XBUFFER _path;
  XBUFFER _iface;
  XBUFFER _method;
  
  destination.ConvertToASCII(_destination);
  path.ConvertToASCII(_path);
  iface.ConvertToASCII(_iface);
  method.ConvertToASCII(_method);
 
  handler = dbus_message_new_method_call(_destination.GetPtrChar(), _path.GetPtrChar(), _iface.GetPtrChar(), _method.GetPtrChar());
  if(handler) 
    {
      status = true;
    }
   else
    {
      XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[DBUS] ERROR: Unable to allocate memory for the message!"));
    }


  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DBusMessage* DIOLINUXDBUS_MESSAGE::GetHandler()
* @brief      GetHandler
* @ingroup    PLATFORM_LINUX
*
* @return     DBusMessage* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DBusMessage* DIOLINUXDBUS_MESSAGE::GetHandler()
{
  return handler;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS_MESSAGE::SetHandler(DBusMessage* handler)
* @brief      SetHandler
* @ingroup    PLATFORM_LINUX
*
* @param[in]  handler : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS_MESSAGE::SetHandler(DBusMessage* handler)
{  
  this->handler = handler;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS_MESSAGE::Delete()
* @brief      Delete
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS_MESSAGE::Delete()
{
  if(!handler) return false;

  dbus_message_unref(handler);
  handler = NULL;
  
  return true;  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS_MESSAGE::GetResult(XDWORD& result)
* @brief      GetResult
* @ingroup    PLATFORM_LINUX
*
* @param[in]  result : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS_MESSAGE::GetResult(XDWORD& result)
{
  DBusError   error;   
  bool        status  = false;

  result = 0;
  if(!handler) return false;

  dbus_error_init(&error);

  if(dbus_message_get_args(handler, &error, DBUS_TYPE_UINT32, &result, DBUS_TYPE_INVALID)) 
    {            
      status  = true;
    }
   else
    {
      DIOLINUXDBUS::TraceError(error);
      status = false; 
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS_MESSAGE::GetResult(XSTRING& result)
* @brief      GetResult
* @ingroup    PLATFORM_LINUX
*
* @param[in]  result : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS_MESSAGE::GetResult(XSTRING& result)
{
  const char* result_char  = NULL;
  DBusError   error;   
  bool        status       = false;

  result.Empty();
  if(!handler) return false;

  dbus_error_init(&error);

  if(dbus_message_get_args(handler, &error, DBUS_TYPE_STRING, &result_char, DBUS_TYPE_INVALID)) 
    {            
      result.Set(result_char);
      status  = true;
    }
   else
    {
      DIOLINUXDBUS::TraceError(error);
      status = false; 
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS_MESSAGE::GetResult(XVECTOR<XSTRING*>& result)
* @brief      GetResult
* @ingroup    PLATFORM_LINUX
*
* @param[in]  result : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS_MESSAGE::GetResult(XVECTOR<XSTRING*>& result)
{
  const char**  result_char  = NULL;
  int           lenght       = 0;
  DBusError     error;   
  bool          status        = false;

  if(!handler) return false;

  dbus_error_init(&error);

  if(dbus_message_get_args(handler, &error, DBUS_TYPE_ARRAY, DBUS_TYPE_OBJECT_PATH, &result_char, &lenght, DBUS_TYPE_INVALID)) 
    {              
      for(int c=0; c<lenght; c++)
        {
          XSTRING* string = new XSTRING();
          if(string)
            {
              string->Set(result_char[c]);          
              result.Add(string);
            }          
        }

      status  = true;
    }
   else
    {
      DIOLINUXDBUS::TraceError(error);
      status = false; 
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOLINUXDBUS_MESSAGE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
*
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOLINUXDBUS_MESSAGE::Clean()
{
  handler   = NULL;

  destination.Empty();
  path.Empty();
  iface.Empty();
  method.Empty(); 
}


#pragma endregion


#pragma region CLASS_DIOLINUXDBUS_ARGUMENTS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOLINUXDBUS_ARGUMENTS::DIOLINUXDBUS_ARGUMENTS()
* @brief      Constructor
* @ingroup    PLATFORM_LINUX
*
* ---------------------------------------------------------------------------------------------------------------------*/
DIOLINUXDBUS_ARGUMENTS::DIOLINUXDBUS_ARGUMENTS(bool in, DIOLINUXDBUS_MESSAGE* message)
{
  Clean();
  
  this->in      = in;
  this->message = message;

  if(message) 
    {
      if(in)
        {
          dbus_message_iter_init_append(message->GetHandler(), &iter);    
        }
       else
        {
          dbus_message_iter_init(message->GetHandler(), &iter);
        }

    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOLINUXDBUS_ARGUMENTS::~DIOLINUXDBUS_ARGUMENTS()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
*
* ---------------------------------------------------------------------------------------------------------------------*/
DIOLINUXDBUS_ARGUMENTS::~DIOLINUXDBUS_ARGUMENTS()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DBusMessageIter* DIOLINUXDBUS_ARGUMENTS::GetHandler()
* @brief      GetHandler
* @ingroup    PLATFORM_LINUX
*
* @return     DBusMessageIter* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DBusMessageIter* DIOLINUXDBUS_ARGUMENTS::GetHandler()
{
  return &iter;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS_ARGUMENTS::Append(bool value)
* @brief      Append
* @ingroup    PLATFORM_LINUX
*
* @param[in]  value : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS_ARGUMENTS::Append(bool value)
{
  dbus_bool_t _value  = false;  
  bool        status  = false;
  
  if(!in)                     return false; 
  if(!message->GetHandler())  return false;
  
  _value = value;

  status = dbus_message_append_args(message->GetHandler(), DBUS_TYPE_BOOLEAN, &_value, DBUS_TYPE_INVALID);
  
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS_ARGUMENTS::Append(XCHAR* value)
* @brief      Append
* @ingroup    PLATFORM_LINUX
*
* @param[in]  value : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS_ARGUMENTS::Append(XCHAR* value)
{
  XSTRING _value;     
  bool    status  = false;

  if(!in)                     return false; 
  if(!message->GetHandler())  return false;
 
  _value = value;

  XBUFFER charstr;
  
  _value.ConvertToASCII(charstr);

  char* charstr2 = charstr.GetPtrChar();
 
  return dbus_message_append_args(message->GetHandler(), DBUS_TYPE_STRING, &charstr2, DBUS_TYPE_INVALID);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS_ARGUMENTS::AppendIterator(bool value)
* @brief      AppendIterator
* @ingroup    PLATFORM_LINUX
*
* @param[in]  value : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS_ARGUMENTS::AppendIterator(bool value)
{
  dbus_bool_t _value  = false;  
  bool        status  = false;   

  if(!in)                     return false; 
  if(!message->GetHandler())  return false;
  
  _value = value;

  status = dbus_message_iter_append_basic(&iter, DBUS_TYPE_BOOLEAN, &_value);
  
  return status;
}
 

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS_ARGUMENTS::AppendIterator(XCHAR* value)
* @brief      AppendIterator
* @ingroup    PLATFORM_LINUX
*
* @param[in]  value : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS_ARGUMENTS::AppendIterator(XCHAR* value)
{
  XSTRING _value;     
  bool    status  = false;

  if(!in)                     return false; 
  if(!message->GetHandler())  return false;
   
  _value = value;
  
  XBUFFER charstr;
  
  _value.ConvertToASCII(charstr);

  char* charstr2 = charstr.GetPtrChar();

  return dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &charstr2);  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS_ARGUMENTS::GetResultInteractor(XVARIANT& result)
* @brief      GetResultInteractor
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  result : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS_ARGUMENTS::GetResultInteractor(XVARIANT& result)
{
  DBusMessageIter   iter_variant;
  bool              status = false;

  if(in)                      return false; 
  if(!message->GetHandler())  return false;  

  if(dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_VARIANT) return false;

  dbus_message_iter_recurse(&iter, &iter_variant);

  switch(dbus_message_iter_get_arg_type(&iter_variant))
    {   
      case DBUS_TYPE_UINT32         : { XDWORD value = 0;
                                        dbus_message_iter_get_basic(&iter_variant, &value);
                                        result = value;
                                      }
                                      break;    

      case DBUS_TYPE_STRING         : 
      case DBUS_TYPE_OBJECT_PATH    : { char* value_str_ptr = NULL;
                                        dbus_message_iter_get_basic(&iter_variant, &value_str_ptr);
                                        if(value_str_ptr)
                                          {
                                            result = value_str_ptr;
                                            status = true;
                                          }
                                      }
                                      break;


    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOLINUXDBUS_ARGUMENTS::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
*
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOLINUXDBUS_ARGUMENTS::Clean()
{
  in        = false;
  message   = NULL;
}


#pragma endregion


#pragma region CLASS_DIOLINUXDBUS_SIGNAL


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOLINUXDBUS_SIGNAL::DIOLINUXDBUS_SIGNAL()
* @brief      Constructor
* @ingroup    PLATFORM_LINUX
*
* ---------------------------------------------------------------------------------------------------------------------*/
DIOLINUXDBUS_SIGNAL::DIOLINUXDBUS_SIGNAL()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOLINUXDBUS_SIGNAL::~DIOLINUXDBUS_SIGNAL()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
*
* ---------------------------------------------------------------------------------------------------------------------*/
DIOLINUXDBUS_SIGNAL::~DIOLINUXDBUS_SIGNAL()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* DIOLINUXDBUS_SIGNAL::GetType()
* @brief      GetType
* @ingroup    PLATFORM_LINUX
*
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOLINUXDBUS_SIGNAL::GetType()
{
  return &type;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* DIOLINUXDBUS_SIGNAL::GetName()
* @brief      GetName
* @ingroup    PLATFORM_LINUX
*
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOLINUXDBUS_SIGNAL::GetName()
{
  return &name;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOLINUXDBUS_SIGNAL::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
*
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOLINUXDBUS_SIGNAL::Clean()
{
  type.Empty();
  name.Empty();
}


#pragma endregion


#pragma region CLASS_DIOLINUXDBUS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOLINUXDBUS::DIOLINUXDBUS()
* @brief      Constructor
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOLINUXDBUS::DIOLINUXDBUS()    
{ 
  Clean();  

  RegisterEvent(DIOLINUXDBUS_XEVENT_TYPE_SIGNAL);                          
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOLINUXDBUS::~DIOLINUXDBUS()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOLINUXDBUS::~DIOLINUXDBUS()    
{ 
  End();

  DeRegisterEvent(DIOLINUXDBUS_XEVENT_TYPE_SIGNAL);

  Clean();                            
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS::Ini(DBusBusType type)
* @brief      Ini
* @ingroup    PLATFORM_LINUX
*
* @param[in]  type : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS::Ini(DBusBusType type)
{
  DBusError error;

  // --------------------------------------------------
  // initialise the errors
  dbus_error_init(&error);

  connection = dbus_bus_get(type, &error);
  if(!connection)
    {
      DIOLINUXDBUS::TraceError(error);
      return false;
    }
 
  connection_name = dbus_bus_get_unique_name(connection);
  XTRACE_PRINTCOLOR(XTRACE_COLOR_GREEN, __L("[DBUS] Ini connection [%08X] %s ... "), connection, connection_name.Get());
    
  thread_signals = CREATEXTHREAD(XTHREADGROUPID_DIOLINUXDBUS, __L("DIOLINUXDBUS::Ini"), Thread_Signals, (void*)this);
  if(!thread_signals) return false;

  return thread_signals->Ini();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS::IsConnected()
* @brief      IsConnected
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS::IsConnected()
{
  if(!connection) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS::CallMethod(DIOLINUXDBUS_MESSAGE& message, DIOLINUXDBUS_MESSAGE& reply)
* @brief      CallMethod
* @ingroup    PLATFORM_LINUX
*
* @param[in]  message : 
* @param[in]  reply : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS::CallMethod(DIOLINUXDBUS_MESSAGE& message, DIOLINUXDBUS_MESSAGE& reply)
{  
  DBusMessage*  _reply   = NULL;
  DBusError     error;
  bool          status  = false;

  dbus_error_init(&error);
  
  if(!connection) 
    { 
      XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[DBUS] Connection invalid! "));
      return false;
    }
    
  _reply = dbus_connection_send_with_reply_and_block(connection, message.GetHandler(), DBUS_TIMEOUT_USE_DEFAULT, &error);
  if(_reply) 
    {      
      reply.SetHandler(_reply);      
      status = true;
    }
   else
    {
      DIOLINUXDBUS::TraceError(error);
      status = false; 
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS::Signal_Add(XCHAR* type, XCHAR* name)
* @brief      Signal_Add
* @ingroup    PLATFORM_LINUX
*
* @param[in]  type : 
* @param[in]  name : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS::Signal_Add(XCHAR* type, XCHAR* name)
{
  if(!connection) return false;

  XSTRING     typestr;
  DBusError   error;
  bool        status = true;

  if(!Signal_IsTypeAdd(type))
    {
      typestr.Format(__L("type='signal', interface='%s'"), type); 

      XBUFFER charstr;
      
      typestr.ConvertToASCII(charstr);      
      dbus_bus_add_match(connection, charstr.GetPtrChar(), &error);    // see signals from the given interface
      
      dbus_connection_flush(connection);

      if(dbus_error_is_set(&error)) 
        { 
          DIOLINUXDBUS::TraceError(error);
          status = false;   
        }

      //XTRACE_PRINTCOLOR((status?XTRACE_COLOR_BLUE:XTRACE_COLOR_RED), __L("[DBUS] Signal Add Type [%s] ..."), typestr.Get()); 
    }

  if(!status) return status;
  
  DIOLINUXDBUS_SIGNAL* signal = new DIOLINUXDBUS_SIGNAL();
  if(signal)
    {
      signal->GetType()->Set(type);
      signal->GetName()->Set(name);

      signals.Add(signal);    

    } else status = false;
    
  //XTRACE_PRINTCOLOR((status?XTRACE_COLOR_BLUE:XTRACE_COLOR_RED), __L("[DBUS] Signal Add Name [%s] ..."), name); 

  return status;
}
 

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS::Signal_Add(XSTRING& type, XSTRING& name)
* @brief      Signal_Add
* @ingroup    PLATFORM_LINUX
*
* @param[in]  type : 
* @param[in]  name : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS::Signal_Add(XSTRING& type, XSTRING& name)
{
  return Signal_Add(type.Get(), name.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS::Signal_IsTypeAdd(XCHAR* type)
* @brief      Signal_IsTypeAdd
* @ingroup    PLATFORM_LINUX
*
* @param[in]  type : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS::Signal_IsTypeAdd(XCHAR* type)
{
  if(signals.IsEmpty()) return false;

  for(XDWORD c=0; c<signals.GetSize(); c++)
    {
      DIOLINUXDBUS_SIGNAL* signal = signals.Get(c);
      if(signal)
        {
          if(!signal->GetType()->Compare(type, true)) return true;
        }
    }

  return false;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS::Signal_DeleteAll()
* @brief      Signal_DeleteAll
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS::Signal_DeleteAll()
{
  if(signals.IsEmpty()) return false;

  signals.DeleteContents();
  signals.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXDBUS::End()
* @brief      End
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS::End()
{
  if(connection) 
    {
      dbus_connection_unref(connection);
      connection = NULL;
    }

  if(thread_signals)
    {
      thread_signals->End();
      DELETEXTHREAD(XTHREADGROUPID_DIOLINUXDBUS, thread_signals);

      thread_signals = NULL;
    }

  Signal_DeleteAll();
    
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXDBUS::TraceError(DBusError error)
* @brief      TraceError
* @ingroup    PLATFORM_LINUX
*
* @param[in]  error : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXDBUS::TraceError(DBusError& error)
{
  XSTRING error_name;
  XSTRING error_message;

  error_name    = error.name;
  error_message = error.message;

  XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[DBUS] Error %s : [%s] "), error_name.Get(), error_message.Get());

  dbus_error_free(&error);  

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOLINUXDBUS::Thread_Signals(void* param)
* @brief      Thread_Signals
* @ingroup    PLATFORM_LINUX
*
* @param[in]  param : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOLINUXDBUS::Thread_Signals(void* param)
{
  DIOLINUXDBUS* dbus = (DIOLINUXDBUS*)param;
  if(!dbus) return;

  if(!dbus->connection)       return;
  if(dbus->signals.IsEmpty()) return;

  DBusMessage* handler_message = NULL;

  dbus_connection_read_write(dbus->connection, 0);
  handler_message = dbus_connection_pop_message(dbus->connection);

  if(!handler_message) 
    { 
      GEN_XSLEEP.Seconds(1);
      return;
    }

  //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[DBUS] Message ...")); 

  DIOLINUXDBUS_SIGNAL*  signal        = NULL;
  bool                  found_signal  = false;

  for(XDWORD c=0; c<dbus->signals.GetSize(); c++)
    {
      signal = dbus->signals.Get(c);
      if(signal)
        {
          // ---------------------------------------------------------------------------------------
          // check if the message is a signal from the correct interface and with the correct name

          XBUFFER typechar;
          XBUFFER namechar;
 
          (*signal->GetType()).ConvertToASCII(typechar);
          (*signal->GetName()).ConvertToASCII(namechar);

          if(dbus_message_is_signal(handler_message, typechar.GetPtrChar(), namechar.GetPtrChar()))  
            {
              found_signal = true;  
            }
          
          if(found_signal) 
            {
              break;
            }
           else 
            {
              signal = NULL;
            }
        }
    }


  if(found_signal && signal) 
    {
      DIOLINUXDBUS_MESSAGE message;

      message.SetHandler(handler_message);  

      DIOLINUXDBUS_XEVENT xevent(dbus, DIOLINUXDBUS_XEVENT_TYPE_SIGNAL);    
  
      xevent.SetMessage(&message);
      xevent.SetSignal(signal);

      dbus->PostEvent(&xevent);

      //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[DBUS] Signal produced [%s] [%s] ..."), signal->GetType()->Get(), signal->GetName()->Get());    
    }   
   else
    {
      dbus_message_unref(handler_message);
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOLINUXDBUS::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOLINUXDBUS::Clean()
{
  type            = DBUS_BUS_SYSTEM;
  connection      = NULL;

  thread_signals  = NULL;
}


#pragma endregion


#pragma endregion

