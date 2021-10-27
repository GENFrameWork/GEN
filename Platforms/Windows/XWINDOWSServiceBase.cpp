/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XWINDOWSServiceBase.cpp
*
* @class      XWINDOWSSERVICEBASE
* @brief      eXtended WINDOWS Service Base class
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @copyright  Copyright(c) 2008 - 2016 GEN Group.
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
*---------------------------------------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <assert.h>


#include "XWINDOWSServiceBase.h"

#include "XTrace.h"
#include "XLog.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

XWINDOWSSERVICEBASE*        XWINDOWSSERVICEBASE::service                  = NULL;

LPQUERY_SERVICE_CONFIG      XWINDOWSSERVICEBASE::service_config           = NULL; 
LPSERVICE_DESCRIPTION       XWINDOWSSERVICEBASE::service_description      = NULL;
LPSERVICE_FAILURE_ACTIONS   XWINDOWSSERVICEBASE::service_failureactions   = NULL;


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWINDOWSSERVICEBASE::XWINDOWSSERVICEBASE(XCHAR* servicename, bool can_stop, bool can_shutdown, bool can_pause, bool can_powerevent)
* @brief      Constructor
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  XCHAR* :
* @param[in]   bool can_stop :
* @param[in]   bool can_shutdown :
* @param[in]   bool can_pause :
* @param[in]   bool can_powerevent :
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XWINDOWSSERVICEBASE::XWINDOWSSERVICEBASE(XCHAR* servicename, bool can_stop, bool can_shutdown, bool can_pause, bool can_powerevent, bool can_sessionchange)
{
  Clean();

  if(servicename)  this->servicename = servicename;

  Setup(can_stop, can_shutdown, can_pause, can_powerevent, can_sessionchange);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWINDOWSSERVICEBASE::~XWINDOWSSERVICEBASE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XWINDOWSSERVICEBASE::~XWINDOWSSERVICEBASE()
{  
  if(service_config)        LocalFree(service_config);
  if(service_description)   LocalFree(service_description);

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::Setup(bool can_stop, bool can_shutdown, bool can_pause, bool can_powerevent, bool can_sessionchange)
* @brief      Setup
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
*
* @param[in]  can_stop : 
* @param[in]  can_shutdown : 
* @param[in]  can_pause : 
* @param[in]  can_powerevent : 
* @param[in]  can_sessionchange : 
*
* @return     void : does not return anything. 
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::Setup(bool can_stop, bool can_shutdown, bool can_pause, bool can_powerevent, bool can_sessionchange)
{
  status.dwServiceType               = SERVICE_WIN32_OWN_PROCESS;
  status.dwCurrentState              = SERVICE_START_PENDING;

  DWORD controlsaccepted = 0;
  if(can_stop)          controlsaccepted |= SERVICE_ACCEPT_STOP;
  if(can_shutdown)      controlsaccepted |= SERVICE_ACCEPT_SHUTDOWN;
  if(can_pause)         controlsaccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
  if(can_powerevent)    controlsaccepted |= SERVICE_ACCEPT_POWEREVENT;
  if(can_sessionchange) controlsaccepted |= SERVICE_ACCEPT_SESSIONCHANGE;

  status.dwControlsAccepted          = controlsaccepted;
  status.dwWin32ExitCode             = NO_ERROR;
  status.dwServiceSpecificExitCode   = 0;
  status.dwCheckPoint                = 0;
  status.dwWaitHint                  = 0;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSSERVICEBASE::Run(XWINDOWSSERVICEBASE& service)
* @brief      Run
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  service :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSSERVICEBASE::Run(XWINDOWSSERVICEBASE& t_service)
{
  service = &t_service;

  SERVICE_TABLE_ENTRY servicetable[] =  { { t_service.servicename , ServiceMain   },
                                          { NULL                  , NULL          }
                                        };

  return StartServiceCtrlDispatcher(servicetable)?true:false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::Stop()
* @brief      Stop
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::Stop()
{
  DWORD originalstate = status.dwCurrentState;

  try {
        SetServiceStatus(SERVICE_STOP_PENDING);
        On_Stop();
        SetServiceStatus(SERVICE_STOPPED);
      }
    catch(DWORD error)
      {
        WriteErrorLogEntry(__L("Service Stop"), error);
        SetServiceStatus(originalstate);
      }
    catch(...)
      {
        WriteEventLogEntry(__L("Service failed to stop."), EVENTLOG_ERROR_TYPE);
        SetServiceStatus(originalstate);
      }
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::Install(XCHAR* servicename, XCHAR* displayname, XDWORD starttype, XCHAR* dependencies, XCHAR* account, XCHAR* password)
* @brief      Install
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  servicename :
* @param[in]  displayname :
* @param[in]  starttype :
* @param[in]  dependencies :
* @param[in]  account :
* @param[in]  password :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSSERVICEBASE::Install(XCHAR* servicename, XCHAR* displayname, XDWORD starttype, XCHAR* dependencies, XCHAR* account, XCHAR* password)
{
  XCHAR       path[MAX_PATH];
  SC_HANDLE   scmanager = NULL;
  SC_HANDLE   service = NULL;
  bool        status  = false;

  if(GetModuleFileName(NULL, path, ARRAYSIZE(path)) == 0)
    {
      return status;
    }

  scmanager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
  if(scmanager)
    {

      service = CreateService(scmanager                 ,      // SCManager database
                              servicename               ,      // Name of service
                              displayname               ,      // Name to display
                              SERVICE_QUERY_STATUS      ,      // Desired access
                              SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,      // Service type
                              starttype                 ,      // Service start type
                              SERVICE_ERROR_NORMAL      ,      // Error control type
                              path                      ,      // Service's binary
                              NULL                      ,      // No load ordering group
                              NULL                      ,      // No tag identifier
                              dependencies              ,      // Dependencies
                              account                   ,      // Service running account
                              password);                       // Password of the account

      if(service)
        {
          status = true;
        }
    }


  if(scmanager)
    {
      CloseServiceHandle(scmanager);
      scmanager = NULL;
    }

  if(service)
    {
      CloseServiceHandle(service);
      service = NULL;
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSSERVICEBASE::Uninstall(XCHAR* servicename)
* @brief      Uninstall
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  servicename :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSSERVICEBASE::Uninstall(XCHAR* servicename)
{
  SC_HANDLE      scmanager = NULL;
  SC_HANDLE      service   = NULL;
  SERVICE_STATUS svcstatus = {};
  bool           status    = false;

  scmanager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
  if(scmanager)
    {
      service = OpenService(scmanager, servicename, SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
      if(service)
        {

          if(ControlService(service, SERVICE_CONTROL_STOP, &svcstatus))
            {
              Sleep(1000);

              while(QueryServiceStatus(service, &svcstatus))
                {
                  if(svcstatus.dwCurrentState == SERVICE_STOP_PENDING)
                    {
                      Sleep(1000);
                    }
                   else break;
                }

              if(svcstatus.dwCurrentState == SERVICE_STOPPED)
                {
                  //wprintf(L"\n%s is stopped.\n", servicename);
                }
               else
                {
                  //wprintf(L"\n%s failed to stop.\n", servicename);
                }
            }


          if(DeleteService(service))
            {
               status = true;
            }
        }
    }


  if(scmanager)
    {
      CloseServiceHandle(scmanager);
      scmanager = NULL;
    }

  if(service)
    {
      CloseServiceHandle(service);
      service = NULL;
    }

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSSERVICEBASE::Start(XCHAR* servicename)
* @brief      StartService
* @ingroup    PLATFORM_WINDOWS
*
* @author
* @date       01/03/2016 12:00
*
* @param[in]  servicename :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSSERVICEBASE::Start(XCHAR* servicename)
{
  bool status = false;

  SC_HANDLE scmanager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
  if(scmanager == NULL) return false;

  SC_HANDLE service = OpenService(scmanager, servicename, SERVICE_ALL_ACCESS);
  if(service == NULL) return false;

  if(StartService(service, 0, NULL))  
    {
      status = true;
    } 
   else
    {
      DWORD error = GetLastError();
      if(error)
        {
          status = false;
        }
    }

  CloseServiceHandle(scmanager);

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSSERVICEBASE::Stopped(XCHAR* servicename)
* @brief      Stopped
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  servicename :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSSERVICEBASE::Stopped(XCHAR* servicename)
{
  SC_HANDLE scmanager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
  if(scmanager == NULL) return false;

  SC_HANDLE service = OpenService(scmanager, servicename, SERVICE_ALL_ACCESS);
  if(service == NULL) return false;

  SERVICE_STATUS ss;

  if(ControlService(service, SERVICE_CONTROL_STOP, &ss) == 0)
    {
      DWORD error = GetLastError();
      return false;
    }

  CloseServiceHandle(scmanager);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         LPQUERY_SERVICE_CONFIG XWINDOWSSERVICEBASE::GetConfig(XCHAR* servicename)
* @brief      GetConfig
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
*
* @param[in]  servicename : 
*
* @return     LPQUERY_SERVICE_CONFIG : 
*
*---------------------------------------------------------------------------------------------------------------------*/
LPQUERY_SERVICE_CONFIG  XWINDOWSSERVICEBASE::GetConfig(XCHAR* servicename)
{
  if(!GetServiceConfig(servicename)) return NULL;

  return service_config;

}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         LPSERVICE_DESCRIPTION XWINDOWSSERVICEBASE::GetDescription(XCHAR* servicename)
* @brief      GetDescription
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
*
* @param[in]  servicename : 
*
* @return     LPSERVICE_DESCRIPTION : 
*
*---------------------------------------------------------------------------------------------------------------------*/
LPSERVICE_DESCRIPTION  XWINDOWSSERVICEBASE::GetDescription(XCHAR* servicename)
{
  if(!GetServiceDescription(servicename)) return NULL;

  return service_description;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         LPSERVICE_FAILURE_ACTIONS XWINDOWSSERVICEBASE::GetFailureActions(XCHAR* servicename)
* @brief      GetFailureActions
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
*
* @param[in]  servicename : 
*
* @return     LPSERVICE_FAILURE_ACTIONS : 
*
*---------------------------------------------------------------------------------------------------------------------*/
LPSERVICE_FAILURE_ACTIONS XWINDOWSSERVICEBASE::GetFailureActions(XCHAR* servicename)
{
  if(!GetServiceFaiilureActions(servicename)) return NULL;

  return service_failureactions;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* XWINDOWSSERVICEBASE::GetServiceName()
* @brief      GetServiceName
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
*
* @return     XCHAR* : 
*
*---------------------------------------------------------------------------------------------------------------------*/
XCHAR* XWINDOWSSERVICEBASE::GetServiceName()
{
  return servicename;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::On_Start(DWORD argc, XCHAR** args)
* @brief      On_Start
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  argc :
* @param[in]  args :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::On_Start(DWORD argc, XCHAR** args)
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::On_Stop()
* @brief      On_Stop
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::On_Stop()
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::On_Pause()
* @brief      On_Pause
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::On_Pause()
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::On_Continue()
* @brief      On_Continue
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::On_Continue()
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::On_Shutdown()
* @brief      On_Shutdown
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::On_Shutdown()
{

}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::On_PowerEvent(DWORD eventtype)
* @brief      On_PowerEvent
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  eventtype :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::On_PowerEvent(DWORD eventtype)
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::On_SessionChange(DWORD eventtype)
* @brief      On_SessionChange
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  eventtype :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::On_SessionChange(DWORD eventtype)
{

}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::SetServiceStatus(XDWORD currentstate, XDWORD win32exitcode, XDWORD waithint)
* @brief      SetServiceStatus
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  currentstate :
* @param[in]  win32exitcode :
* @param[in]  waithint :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::SetServiceStatus(XDWORD currentstate, XDWORD win32exitcode, XDWORD waithint)
{
 static DWORD checkpoint = 1;

 status.dwCurrentState  = currentstate;
 status.dwWin32ExitCode = win32exitcode;
 status.dwWaitHint      = waithint;

 status.dwCheckPoint    = ((currentstate == SERVICE_RUNNING) || (currentstate == SERVICE_STOPPED)) ? 0 : checkpoint++;


 ::SetServiceStatus(statushandle, &status);
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::WriteEventLogEntry(XCHAR* message, XWORD type)
* @brief      WriteEventLogEntry
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  message :
* @param[in]  type :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::WriteEventLogEntry(XCHAR* message, XWORD type)
{
  HANDLE  heventsource = NULL;
  LPCWSTR strings[2]   = { NULL, NULL };

  heventsource = RegisterEventSource(NULL, servicename);
  if(!heventsource) return;

  strings[0] = servicename;
  strings[1] = message;

  ReportEvent(heventsource, type    ,  // Event type
                            0       ,  // Event category
                            0       ,  // Event identifier
                            NULL    ,  // No security identifier
                            2       ,  // Size of lpszStrings array
                            0       ,  // No binary data
                            strings ,  // Array of strings
                            NULL);     // No binary data


  DeregisterEventSource(heventsource);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::WriteErrorLogEntry(XCHAR* function, XDWORD error)
* @brief      WriteErrorLogEntry
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  function :
* @param[in]  error :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::WriteErrorLogEntry(XCHAR* function, XDWORD error)
{
  XSTRING message;

  message.Format( __L("%s failed w/err 0x%08lx"), function, error);

  WriteEventLogEntry(message.Get(), EVENTLOG_ERROR_TYPE);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSSERVICEBASE::GetServiceConfig(XCHAR* servicename)
* @brief      GetServiceConfig
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
*
* @param[in]  servicename : 
*
* @return     bool : true if is succesful. 
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSSERVICEBASE::GetServiceConfig(XCHAR* servicename)
{
  SC_HANDLE scmanager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
  if(scmanager == NULL) return false;

  SC_HANDLE service = OpenService(scmanager, servicename, SERVICE_ALL_ACCESS);
  if(service == NULL) return false;

  DWORD                   sizeneeded;
  DWORD                   buffersize;
  DWORD                   error; 
  bool                    status = false;

  if(service_config)  LocalFree(service_config);

  if(!QueryServiceConfig(service, NULL, 0, &sizeneeded))
    {
      error = GetLastError();
      if(error == ERROR_INSUFFICIENT_BUFFER)
        {
          buffersize = sizeneeded;
          service_config = (LPQUERY_SERVICE_CONFIG) LocalAlloc(LMEM_FIXED, buffersize);

          status = true;
        }       
    }

  if(status)
    {
      status = false;

      if(QueryServiceConfig(service, service_config, buffersize, &sizeneeded))      
        {        
          status = true;      
        }
    }

  CloseServiceHandle(scmanager);

  return status;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSSERVICEBASE::SetConfig(XCHAR* servicename, XDWORD type, XDWORD starttype, XDWORD errorcontrol)
* @brief      SetConfig
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
*
* @param[in]  servicename : 
* @param[in]  type : 
* @param[in]  starttype : 
* @param[in]  errorcontrol : 
*
* @return     bool : true if is succesful. 
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSSERVICEBASE::SetConfig(XCHAR* servicename, XDWORD type, XDWORD starttype, XDWORD errorcontrol)
{
  SC_HANDLE scmanager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
  if(scmanager == NULL) return false;

  SC_HANDLE service = OpenService(scmanager, servicename, SERVICE_ALL_ACCESS);
  if(service == NULL) return false;

  bool status = false;

  status = ChangeServiceConfig( service       ,                 // handle of service 
                                type          ,                 // service type: no change 
                                starttype     ,                 // service start type 
                                errorcontrol  ,                 // error control: no change 
                                NULL          ,                 // binary path: no change 
                                NULL          ,                 // load order group: no change 
                                NULL          ,                 // tag ID: no change 
                                NULL          ,                 // dependencies: no change 
                                NULL          ,                 // account name: no change 
                                NULL          ,                 // password: no change 
                                NULL);                          // display name: no change
    
  CloseServiceHandle(scmanager);

  return status;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSSERVICEBASE::GetServiceDescription(XCHAR* servicename)
* @brief      GetServiceDescription
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
*
* @param[in]  servicename : 
*
* @return     bool : true if is succesful. 
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSSERVICEBASE::GetServiceDescription(XCHAR* servicename)
{
  SC_HANDLE scmanager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
  if(scmanager == NULL) return false;

  SC_HANDLE service = OpenService(scmanager, servicename, SERVICE_ALL_ACCESS);
  if(service == NULL) return false;

  if(service_description)  LocalFree(service_description);

  DWORD                   sizeneeded;
  DWORD                   buffersize;
  DWORD                   error; 
  bool                    status = false;

  if(!QueryServiceConfig2(service, SERVICE_CONFIG_DESCRIPTION, NULL, 0, &sizeneeded))
    {
      error = GetLastError();
      if(error == ERROR_INSUFFICIENT_BUFFER)
        {
          buffersize = sizeneeded;
          service_description = (LPSERVICE_DESCRIPTION) LocalAlloc(LMEM_FIXED, buffersize);

          status = true;
        }
    }  

  if(status)
    {
      status = false;

      if(QueryServiceConfig2(service, SERVICE_CONFIG_DESCRIPTION, (LPBYTE)service_description, buffersize, &sizeneeded)) 
        {
          status = true;      
        }
    }

  CloseServiceHandle(scmanager);

  return status;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSSERVICEBASE::SetDescription(XCHAR* servicename, LPSERVICE_DESCRIPTION description)
* @brief      SetDescription
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
*
* @param[in]  servicename : 
* @param[in]  description : 
*
* @return     bool : true if is succesful. 
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSSERVICEBASE::SetDescription(XCHAR* servicename, LPSERVICE_DESCRIPTION description)
{
  bool status = false;

  SC_HANDLE scmanager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
  if(scmanager == NULL) return false;

  SC_HANDLE service = OpenService(scmanager, servicename, SERVICE_ALL_ACCESS);
  if(service == NULL) return false;
  
  status = ChangeServiceConfig2(service,  SERVICE_CONFIG_DESCRIPTION, description); 

  CloseServiceHandle(scmanager);

  return status;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSSERVICEBASE::GetServiceFaiilureActions(XCHAR* servicename)
* @brief      GetServiceFaiilureActions
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
*
* @param[in]  servicename : 
*
* @return     bool : true if is succesful. 
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSSERVICEBASE::GetServiceFaiilureActions(XCHAR* servicename)
{
  SC_HANDLE scmanager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
  if(scmanager == NULL) return false;

  SC_HANDLE service = OpenService(scmanager, servicename, SERVICE_ALL_ACCESS);
  if(service == NULL) return false;

  if(service_failureactions)  LocalFree(service_failureactions);

  DWORD                   sizeneeded;
  DWORD                   buffersize;
  DWORD                   error; 
  bool                    status = false;

  if(!QueryServiceConfig2(service, SERVICE_CONFIG_FAILURE_ACTIONS, NULL, 0, &sizeneeded))
    {
      error = GetLastError();
      if(error == ERROR_INSUFFICIENT_BUFFER)
        {
          buffersize = sizeneeded;
          service_failureactions = (LPSERVICE_FAILURE_ACTIONS) LocalAlloc(LMEM_FIXED, buffersize);

          status = true;
        }
    }  

  if(status)
    {
      status = false;

      if(QueryServiceConfig2(service, SERVICE_CONFIG_FAILURE_ACTIONS, (LPBYTE)service_failureactions, buffersize, &sizeneeded)) 
        {
          status = true;      
        }
    }

  CloseServiceHandle(scmanager);

  return status;
}





/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSSERVICEBASE::SetFailureActions(XCHAR* servicename, LPSERVICE_FAILURE_ACTIONS failureactions)
* @brief      SetFailureActions
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
*
* @param[in]  servicename : 
* @param[in]  failureactions : 
*
* @return     bool : true if is succesful. 
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSSERVICEBASE::SetFailureActions(XCHAR* servicename, LPSERVICE_FAILURE_ACTIONS failureactions)
{
  bool status = false;

  SC_HANDLE scmanager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
  if(scmanager == NULL) return false;

  SC_HANDLE service = OpenService(scmanager, servicename, SERVICE_ALL_ACCESS);
  if(service == NULL) return false;
  
  status = ChangeServiceConfig2(service, SERVICE_CONFIG_FAILURE_ACTIONS, failureactions); 

  CloseServiceHandle(scmanager);

  return status;
}





/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void WINAPI XWINDOWSSERVICEBASE::ServiceMain(XDWORD argc, XCHAR** args)
* @brief      ServiceMain
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  argc :
* @param[in]  args :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void WINAPI XWINDOWSSERVICEBASE::ServiceMain(DWORD argc, LPWSTR* argv)
{
  assert(service != NULL);

  service->statushandle = RegisterServiceCtrlHandlerEx(service->servicename, (LPHANDLER_FUNCTION_EX)ServiceCtrlHandler, (void*)XWINDOWSSERVICEBASE::service);

  if(service->statushandle == NULL)
    {
      throw GetLastError();
    }

  service->Start(argc, argv);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void WINAPI XWINDOWSSERVICEBASE::ServiceCtrlHandler(XDWORD ctrl)
* @brief      ServiceCtrlHandler
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  ctrl :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void WINAPI XWINDOWSSERVICEBASE::ServiceCtrlHandler(DWORD control, DWORD eventtype, LPVOID eventdata, LPVOID context)
{
  switch(control)
    {
      case SERVICE_CONTROL_STOP           : service->Stop();                     break;
      case SERVICE_CONTROL_PAUSE          : service->Pause();                    break;
      case SERVICE_CONTROL_CONTINUE       : service->Continue();                 break;
      case SERVICE_CONTROL_SHUTDOWN       : service->Shutdown();                 break;
      case SERVICE_CONTROL_POWEREVENT     : service->PowerEvent(eventtype);      break;
      case SERVICE_CONTROL_SESSIONCHANGE  : service->SessionChange(eventtype);   break;
      case SERVICE_CONTROL_INTERROGATE    : break;
                          default         : break;
    }
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::Start(XDWORD argc, XCHAR** args)
* @brief      Start
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  argc :
* @param[in]  args :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::Start(XDWORD argc,  XCHAR** args)
{
  try { SetServiceStatus(SERVICE_START_PENDING);

        On_Start(argc, args);

        SetServiceStatus(SERVICE_RUNNING);
      }
    catch(DWORD error)
      {
        #ifdef XLOG_ACTIVE
        GEN_XLOG.AddEntry(XLOGLEVEL_ERROR, XWINDOWSSERVICEBASE_LOG_SECTIONID_SERVICE, false, __L("%s: Service failed to start Error[%08X]."), servicename, error);
        #endif
        SetServiceStatus(SERVICE_STOPPED, error);
      }
    catch(...)
      {
        #ifdef XLOG_ACTIVE
        GEN_XLOG.AddEntry(XLOGLEVEL_ERROR, XWINDOWSSERVICEBASE_LOG_SECTIONID_SERVICE, false, __L("%s: Service failed to start."), servicename);
        #endif
        SetServiceStatus(SERVICE_STOPPED);
      }
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::Pause()
* @brief      Pause
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::Pause()
{
  try { SetServiceStatus(SERVICE_PAUSE_PENDING);

        On_Pause();

        SetServiceStatus(SERVICE_PAUSED);
      }
    catch(DWORD error)
      {
        #ifdef XLOG_ACTIVE
        GEN_XLOG.AddEntry(XLOGLEVEL_ERROR, XWINDOWSSERVICEBASE_LOG_SECTIONID_SERVICE, false, __L("%s: Service failed to pause Error[%08X]."), servicename, error);
        #else
        error++;
        #endif
        SetServiceStatus(SERVICE_RUNNING);
      }
    catch(...)
      {
        #ifdef XLOG_ACTIVE
        GEN_XLOG.AddEntry(XLOGLEVEL_ERROR, XWINDOWSSERVICEBASE_LOG_SECTIONID_SERVICE, false, __L("%s: Service failed to pause."), servicename);        
        #endif        
        SetServiceStatus(SERVICE_RUNNING);
      }
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::Continue()
* @brief      Continue
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::Continue()
{
  try { SetServiceStatus(SERVICE_CONTINUE_PENDING);

        On_Continue();
        SetServiceStatus(SERVICE_RUNNING);
      }
    catch(DWORD error)
      {
        #ifdef XLOG_ACTIVE
        GEN_XLOG.AddEntry(XLOGLEVEL_ERROR, XWINDOWSSERVICEBASE_LOG_SECTIONID_SERVICE, false, __L("%s: Service failed to continue Error[%08X]."), servicename, error);
        #else
        error++;
        #endif
        SetServiceStatus(SERVICE_PAUSED);
      }
    catch(...)
      {
        #ifdef XLOG_ACTIVE
        GEN_XLOG.AddEntry(XLOGLEVEL_ERROR, XWINDOWSSERVICEBASE_LOG_SECTIONID_SERVICE, false, __L("%s: Service failed to continue Error[%08X]."), servicename);        
        #endif
        SetServiceStatus(SERVICE_PAUSED);
     }
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::Shutdown()
* @brief      Shutdown
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::Shutdown()
{
  try { On_Shutdown();

        SetServiceStatus(SERVICE_STOPPED);
      }
     catch (DWORD error)
      {
        #ifdef XLOG_ACTIVE
        GEN_XLOG.AddEntry(XLOGLEVEL_ERROR, XWINDOWSSERVICEBASE_LOG_SECTIONID_SERVICE, false, __L("%s: Service failed to shutdown Error[%08X]."), servicename, error);
        #else
        error++;
        #endif
      }
     catch (...)
      {
        #ifdef XLOG_ACTIVE
        GEN_XLOG.AddEntry(XLOGLEVEL_ERROR, XWINDOWSSERVICEBASE_LOG_SECTIONID_SERVICE, false, __L("%s: Service failed to shutdown."), servicename);        
        #endif
      }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::PowerEvent(DWORD eventtype)
* @brief      PowerEvent
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  eventtype :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::PowerEvent(DWORD eventtype)
{
  On_PowerEvent(eventtype);
  #ifdef XLOG_ACTIVE
  GEN_XLOG.AddEntry(XLOGLEVEL_INFO, XWINDOWSSERVICEBASE_LOG_SECTIONID_SERVICE, false, __L("%s: Service Power Event : Event [%08X]."), servicename, eventtype);
  #endif
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::SessionChange(DWORD eventtype)
* @brief      SessionChange
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  eventtype :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::SessionChange(DWORD eventtype)
{
  On_SessionChange(eventtype);
  #ifdef XLOG_ACTIVE
  GEN_XLOG.AddEntry(XLOGLEVEL_INFO, XWINDOWSSERVICEBASE_LOG_SECTIONID_SERVICE, false, __L("%s: Service Session Change: Event [%08X]."), servicename, eventtype);
  #endif
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSSERVICEBASE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSSERVICEBASE::Clean()
{
  servicename           = __L("");
  statushandle          = NULL;

  service_config        = NULL; 
  service_description   = NULL;


}



