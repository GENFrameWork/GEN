/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       MainProcWINDOWS.cpp
* 
* @class      MAINPROCWINDOWS
* @brief      WINDOWS Main procedure class
* @ingroup    PLATFORM_WINDOWS
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

#include <winsock2.h>

#pragma warning(push)
#pragma warning (disable : 4091)
#include <dbghelp.h>
#pragma warning(pop)

#ifdef GOOGLETEST_ACTIVE      
#include "gtest/gtest.h"
#endif

#include "MainProcWINDOWS.h"

#include "XWINDOWSFactory.h"
#include "XWINDOWSTrace.h"
#include "XWINDOWSRand.h"
#include "XWINDOWSSleep.h"
#include "XWINDOWSTrace.h"

#ifdef XSYSTEM_ACTIVE
  #include "XWINDOWSSystem.h"
#endif

#ifdef DIO_OSPIPELINE_ACTIVE
  #include "DIOWINDOWSOSPipeline.h"
#endif

#ifdef XPROCESSMANAGER_ACTIVE
  #include "XWINDOWSProcessManager.h"
#endif

#ifdef XSHAREDMEMORYMANAGER_ACTIVE
  #include "XWINDOWSSharedMemoryManager.h"
#endif

#ifdef XDRIVEIMAGEMANAGER_ACTIVE
  #include "XWINDOWSDriveImageManager.h"
#endif

#ifdef XEEPROMMEMORYMANAGER_ACTIVE
  #include "XWINDOWSEEPROMMemoryManager.h"
#endif


#ifdef GRP_ACTIVE
#include "GRPWINDOWSScreen.h"
#include "GRPWINDOWSFactory.h"
#endif

#ifdef INP_ACTIVE
#include "INPWINDOWSDeviceKeyboard.h"
#include "INPWINDOWSDeviceMouse.h"
#include "INPWINDOWSFactory.h"
#include "INPManager.h"
#include "INPFactory.h"
#endif

#ifdef SND_ACTIVE
#include "SNDWINDOWSFactory.h"
#endif

#ifdef DIO_ACTIVE
#include "DIODNSResolver.h"
#include "DIOWINDOWSFactory.h"

  #ifdef DIO_GPIO_ACTIVE
  #include "DIOGPIO.h"
    #ifdef DIO_GPIO_PCPARALLEL_ACTIVE
    #include "DIOWINDOWSGPIOPCParallel.h"
    #endif   
  #endif

  #ifdef DIO_PING_ACTIVE
  #include "DIOWINDOWSPing.h"
  #include "DIOPing.h"
  #endif

  #ifdef DIO_PUBLICINTERNETIP_ACTIVE
  #include "DIOPublicInternetIP.h"
  #endif

  #ifdef DIO_ALERTS_ACTIVE
  #include "DIOAlerts.h"
  #endif

#endif

#include "VersionFrameWork.h"

#ifdef SCRIPT_CACHE_ACTIVE
#include "Script_Cache.h"
#endif

#include "XFeedback_Control.h"
#include "XRand.h"
#include "XSleep.h"
#include "XPath.h"
#include "XString.h"
#include "XLog.h"
#include "XFile.h"
#include "XTimer.h"
#include "XThreadCollected.h"
#include "XDateTime.h"
#include "XTranslation_GEN.h"
#include "XTranslation.h"
#include "XPublisher.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#ifdef XTRACE_ACTIVE
XWINDOWSTRACE           windowsdebugtrace;
#endif
MAINPROCWINDOWS         mainprocwindows;
XSTRING*                allexceptiontext  = NULL;

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS

DWORD WINAPI            Service_WorkerThread          (LPVOID lpParam);

bool                    Exception_Printf              (bool iserror, XCHAR* title, XCHAR* mask, ...);
BOOL                    Exception_ConsoleHandler      (DWORD fdwctrltype);


#pragma region CLASS_MAINPROCWINDOWS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         MAINPROCWINDOWS::MAINPROCWINDOWS()
* @brief      Constructor of class
* @ingroup    PLATFORM_WINDOWS
*
* --------------------------------------------------------------------------------------------------------------------*/
MAINPROCWINDOWS::MAINPROCWINDOWS()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         MAINPROCWINDOWS::~MAINPROCWINDOWS()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_WINDOWS
*
* --------------------------------------------------------------------------------------------------------------------*/
MAINPROCWINDOWS::~MAINPROCWINDOWS()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MAINPROCWINDOWS::Ini(APPFLOWMAIN* appmain, APPFLOWBASE_MODE_TYPE applicationmode)
* @brief      Ini
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  appmain :
* @param[in]  applicationmode :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MAINPROCWINDOWS::Ini(APPFLOWMAIN* appmain, APPFLOWBASE_MODE_TYPE applicationmode)
{
  this->appmain = appmain;

  //---------------------------------------------------------------------------

  #if  defined(XTRACE_ACTIVE) ||  defined(DIO_ACTIVE) || defined(DIO_STREAMUDP_ACTIVE) || defined(DIO_STREAMTCPIP_ACTIVE) || defined(DIO_STREAMBLUETOOTH_ACTIVE)
  XWORD   versionrequested = WINSOCK_VERSION;
  WSADATA data;
  int     error = false;

  error = WSAStartup(versionrequested, &data);
  if(!error)
    {
      if(data.wVersion != versionrequested) error = WSAVERNOTSUPPORTED;
    }

  if(error)
    {
      WSACleanup();
      return false;
    }
  #endif

  //---------------------------------------------------------------------------

  if(!Factorys_Ini()) return false;

  #ifdef APPFLOW_ACTIVE

  if(!appmain)                              return false;
  if(!appmain->Create())                    return false;
  if(!appmain->Ini(this, applicationmode))  return false;

  #else

   if(Main_Proc_PlatformIni)
    {
      if(!Main_Proc_PlatformIni(this)) return false;
    }

  GEN_XPATHSMANAGER.GetAppExecPath()->Set(GetXPathExec()->Get());
  GetXPathExec()->SetOnlyDriveAndPath();
  GEN_XPATHSMANAGER.AddPathSection(XPATHSMANAGERSECTIONTYPE_ROOT , (*GetXPathExec()));

   if(Main_Proc_Ini)
    {
      if(!Main_Proc_Ini(this)) return false;
    }

  #endif

  isrunningasservice = mainprocwindows.IsRunningAsService();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MAINPROCWINDOWS::Update()
* @brief      Update
* @ingroup    PLATFORM_WINDOWS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MAINPROCWINDOWS::Update()
{
  if(!isrunningasservice)
    {
      #ifdef GRP_ACTIVE
      MSG   msg;

      APPFLOWBASE* app = NULL;
      if(mainprocwindows.GetAppMain()) 
        {
          app = mainprocwindows.GetAppMain()->GetApplication();
        }

      while(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {          
          switch(msg.message)
            {
              case WM_QUIT        : if(app) 
                                      {
                                        app->SetExitType(APPFLOWBASE_EXITTYPE_BY_USER);
                                      }
                                    break;

              case WM_CLOSE       : //PostQuitMessage(APPFLOWBASE_EXITTYPE_BY_USER);
                                    break;
            }
          
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
      #endif

      #ifdef INP_ACTIVE
      GEN_INPMANAGER.Update();
      #endif
    }


  #ifdef APPFLOW_ACTIVE
  if(appmain)
    {
      if(!appmain->Update()) 
        {
          return false;
        }
    }
  #else
   if(Main_Proc_Update)
    {
      if(!Main_Proc_Update(this)) 
        {
          return false;
        }
    }
  #endif

  #ifndef GRP_ACTIVE
  Sleep(5);
  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MAINPROCWINDOWS::End()
* @brief      End
* @ingroup    PLATFORM_WINDOWS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MAINPROCWINDOWS::End()
{
  #ifdef APPFLOW_ACTIVE

  if(appmain) 
    {
      appmain->End();
    }

  #else

  if(Main_Proc_End)
    {
      if(!Main_Proc_End(this)) 
        {
          return false;
        }
    }

  #endif
  
  #ifdef APPFLOW_ACTIVE

  if(appmain) 
    {
      appmain->Delete();
    }

  #else

  if(Main_Proc_PlatformEnd)
    {
      if(!Main_Proc_PlatformEnd(this)) 
        {
          return false;
        }
    }

  #endif

  DeleteAllExecParams(); 

  #ifdef SCRIPT_CACHE_ACTIVE 
  GEN_SCRIPT_CACHE.DelInstance();
  #endif

  GEN_VERSION.DelInstance();

  XFILE_DISPLAYNOTCLOSEFILES
  XFEEDBACK_CONTROL_DISPLAYFEEDBACK  
  XFEEDBACK_CONTROL_DELETE 

  Factorys_End();

  XMEMORY_CONTROL_DEACTIVATED
  XMEMORY_CONTROL_DISPLAYMEMORYLEAKS

  #if defined(XTRACE_ACTIVE) || defined(DIO_ACTIVE) || defined(DIO_STREAMUDP_ACTIVE) || defined(DIO_STREAMTCPIP_ACTIVE) || defined(DIO_STREAMBLUETOOTH_ACTIVE)
  WSACleanup();
  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MAINPROCWINDOWS::Factorys_Ini()
* @brief      Factorys ini
* @ingroup    PLATFORM_WINDOWS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MAINPROCWINDOWS::Factorys_Ini()
{  
  if(!XFACTORY::SetInstance(new XWINDOWSFACTORY()))
    {
      return false;
    }

  XFEEDBACK_CONTROL_CREATE
  
  #ifdef XSYSTEM_ACTIVE  
  if(!XSYSTEM::SetInstance(new XWINDOWSSYSTEM()))  
    {
      return false;
    }
  XBUFFER::SetGlobalHardwareUseLittleEndian(GEN_XSYSTEM.HardwareUseLittleEndian());
  #endif

  #ifdef DIO_OSPIPELINE_ACTIVE  
  if(!DIOOSPIPELINE::SetInstance(new DIOWINDOWSOSPIPELINE()))  
    {
      return false;
    } 
  DIOOSPIPELINE::GetInstance().Open(); 
  #endif
  
  #ifdef XSLEEP_ACTIVE
  if(!XSLEEP::SetInstance(new XWINDOWSSLEEP())) 
    {
      return false;
    }
  #endif

  if(!XRAND::SetInstance(new XWINDOWSRAND())) 
    {
      return false;
    }
  
  #ifdef XPROCESSMANAGER_ACTIVE
  if(!XPROCESSMANAGER::SetInstance(new XWINDOWSPROCESSMANAGER())) 
    {
      return false;
    }
  #endif
  
  #ifdef XSHAREDMEMORYMANAGER_ACTIVE
  if(!XSHAREDMEMORYMANAGER::SetInstance(new XWINDOWSSHAREDMEMORYMANAGER())) 
    {
      return false;
    }
  #endif

  #ifdef XDRIVEIMAGE_ACTIVE
  if(!XDRIVEIMAGE::SetInstance(new XWINDOWSDRIVEIMAGE())) 
    {
      return false;
    }
  #endif

  #ifdef XEEPROMMEMORYMANAGER_ACTIVE
  if(!XEEPROMMEMORYMANAGER::SetInstance(new XWINDOWSEEPROMMEMORYMANAGER())) 
    {
      return false;
    }
  #endif

  #ifdef XTRACE_VIRTUALCLOCKTICK
  xtimerclock = new XTIMERCLOCK();
  if(!xtimerclock) 
    {
      return false;
    }
  #endif

  #ifdef INP_ACTIVE
  if(!INPFACTORY::SetInstance(new INPWINDOWSFACTORY())) 
    {
      return false;  
    }
  #endif

  #ifdef DIO_ACTIVE
  if(!DIOFACTORY::SetInstance(new DIOWINDOWSFACTORY())) 
    {
      return false;
    }
  
    #ifdef DIO_GPIO_ACTIVE

      #ifdef DIO_GPIO_PCPARALLEL_ACTIVE
      if(!DIOGPIO::SetInstance(new DIOWINDOWSGPIOPCPARALLEL())) 
        {
          return false;    
        }

      if(!DIOGPIO::GetInstance().Ini()) 
        {
          return false;
        }
      #endif

    #endif

    #ifdef DIO_PING_ACTIVE    
    if(!DIOPING::SetInstance(new DIOWINDOWSPING())) 
      {
        return false;
      }    
    #endif

  #endif

  #ifdef SND_ACTIVE
  if(!SNDFACTORY::SetInstance(new SNDWINDOWSFACTORY())) 
    {
      return false;
    }
  if(!SNDFACTORY::GetInstance().Ini()) 
    {
      //return false;
    }
  #endif

  #ifdef GRP_ACTIVE
  if(!GRPFACTORY::SetInstance(new GRPWINDOWSFACTORY())) 
    {
      return false;
    }
  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MAINPROCWINDOWS::Factorys_End()
* @brief      Factorys end
* @ingroup    PLATFORM_WINDOWS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MAINPROCWINDOWS::Factorys_End()
{
  #ifdef GRP_ACTIVE
  GRPSCREEN::GetListScreens()->DeleteAll();
  GRPFACTORY::DelInstance();
  #endif

  #ifdef SND_ACTIVE  
  if(SNDFACTORY::GetIsInstanced())
    {
      SNDFACTORY::GetInstance().End();
      SNDFACTORY::DelInstance();
    }
  #endif

  #ifdef DIO_ACTIVE

    #ifdef DIO_GPIO_ACTIVE
    if(DIOGPIO::GetIsInstanced())
      {
        DIOGPIO::GetInstance().End();
        DIOGPIO::DelInstance();
      }
    #endif

    #ifdef DIO_STREAMUDP_ACTIVE
    DIODNSRESOLVER::DelInstance();
    #endif

    #ifdef DIO_PING_ACTIVE    
    DIOPING::DelInstance();      
    #endif

    DIOFACTORY::DelInstance();

  #endif

  #ifdef INP_ACTIVE
  INPMANAGER::DelInstance();
  INPFACTORY::DelInstance();
  #endif

  #ifdef XTRACE_VIRTUALCLOCKTICK
  if(xtimerclock)
    {
      delete xtimerclock;
      xtimerclock = NULL;
    }
  #endif

  #ifdef DIO_PUBLICINTERNETIP_ACTIVE
  DIOPUBLICINTERNETIP::DelInstance();
  #endif

  #ifdef DIO_ALERTS_ACTIVE
  DIOALERTS::DelInstance();
  #endif 

  #ifdef XLOG_ACTIVE
  XLOG::DelInstance();
  #endif

  #ifdef XPROCESSMANAGER_ACTIVE
  XPROCESSMANAGER::DelInstance();
  #endif

  #ifdef XEEPROMMEMORYMANAGER_ACTIVE
  XEEPROMMEMORYMANAGER::DelInstance();
  #endif

  #ifdef XDRIVEIMAGE_ACTIVE
  XDRIVEIMAGE::DelInstance();
  #endif

  #ifdef XSHAREDMEMORYMANAGER_ACTIVE   
  XSHAREDMEMORYMANAGER::DelInstance();
  #endif  

  XPATHSMANAGER::DelInstance();

  #ifdef XTRANSLATION_ACTIVE
  XTRANSLATION::DelInstance();
  #endif  
  
  #ifdef XTRANSLATION_GEN_ACTIVE
  XTRANSLATION_GEN::DelInstance();
  #endif  

  XRAND::DelInstance();

  #ifdef XSLEEP_ACTIVE
  XSLEEP::DelInstance();
  #endif

  #ifdef XSYSTEM_ACTIVE  
  XSYSTEM::DelInstance();
  #endif

  #ifdef DIO_OSPIPELINE_ACTIVE  
  DIOOSPIPELINE::DelInstance();
  #endif
  
  #ifdef XPUBLISHER_ACTIVE
  XPUBLISHER::DelInstance();
  #endif
    
  #ifdef XTHREADCOLLECTED_ACTIVE
  XTHREADSCOLLECTEDMANAGER::DelInstance();
  #endif

  XFACTORY::DelInstance();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int MAINPROCWINDOWS::MainLoop()
* @brief      Main loop
* @ingroup    PLATFORM_WINDOWS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int MAINPROCWINDOWS::MainLoop()
{
  int status = 0;

  #ifndef BUILDER
  __try  {
  #endif
          #ifdef APPFLOW_ACTIVE
          if(!mainprocwindows.Ini(&GEN_appmain, APPFLOWBASE_MODE_TYPE_APPLICATION))
          #else
          if(!mainprocwindows.Ini())
          #endif
            {
              status=1;
            }

          if(!status)
            {
              while(mainprocwindows.Update())
                {

                }
            }

          mainprocwindows.GetXPathExec()->Empty();
          if(!mainprocwindows.End()) status = 1;

  #ifndef BUILDER
        }
        __except(Exception_Filter(GetExceptionCode(), GetExceptionInformation())) {  }
  #endif

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MAINPROCWINDOWS::IsRunningAsService()
* @brief      Is running as service
* @ingroup    PLATFORM_WINDOWS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MAINPROCWINDOWS::IsRunningAsService()
{
 // return IsRunningAsService(APPMODE_SERVICE_NAME);

  SERVICE_STATUS_PROCESS ssstatus;
  DWORD                  bytesneeded;
  SC_HANDLE              schscmanager;

  schscmanager = OpenSCManager( NULL,                              // local computer
                                NULL                  ,            // servicesActive database
                                SC_MANAGER_ALL_ACCESS  );          // full access rights
  if(!schscmanager) return false;

  // Get a handle to the service.
  SC_HANDLE schservice = OpenService( schscmanager          ,      // SCM database
                                      APPMODE_SERVICE_NAME  ,      // name of service
                                      SERVICE_ALL_ACCESS);         // full access

  if(!schservice)
    {
      CloseServiceHandle(schscmanager);
      return false;
    }

  // Check the status in case the service is not stopped.

  if(!QueryServiceStatusEx( schservice,                             // handle to service
                            SC_STATUS_PROCESS_INFO,                 // information level
                            (LPBYTE) &ssstatus,                     // address of structure
                            sizeof(SERVICE_STATUS_PROCESS),         // size of structure
                            &bytesneeded ) )                        // size needed if buffer is too small
    {
      CloseServiceHandle(schservice);
      CloseServiceHandle(schscmanager);
      return false;
    }

  DWORD processid = GetCurrentProcessId();
  if(processid == ssstatus.dwProcessId)
    {
      CloseServiceHandle(schservice);
      CloseServiceHandle(schscmanager);

      return true;
    }

  CloseServiceHandle(schservice);
  CloseServiceHandle(schscmanager);

  if((!ssstatus.dwProcessId) && (ssstatus.dwCurrentState == SERVICE_START_PENDING)) return true;

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void MAINPROCWINDOWS::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    PLATFORM_WINDOWS
*
* --------------------------------------------------------------------------------------------------------------------*/
void MAINPROCWINDOWS::Clean()
{
  isrunningasservice = false;  
}


#pragma endregion


#if !defined(APPMODE_LIBRARY_DINAMIC) 


#pragma region CLASS_WINDOWSSERVICE


/**-------------------------------------------------------------------------------------------------------------------

@fn         WINDOWSSERVICE::WINDOWSSERVICE(XCHAR* servicename, bool can_stop, bool can_shutdown, bool can_pause, bool can_powerevent, bool can_sessionchange)
@brief      Constructor
@ingroup    PLATFORM_WINDOWS

@param[in]  servicename : 
@param[in]  can_stop : 
@param[in]  can_shutdown : 
@param[in]  can_pause : 
@param[in]  can_powerevent : 
@param[in]  can_sessionchange : 

--------------------------------------------------------------------------------------------------------------------*/
WINDOWSSERVICE::WINDOWSSERVICE(XCHAR* servicename, bool can_stop, bool can_shutdown, bool can_pause, bool can_powerevent, bool can_sessionchange) : XWINDOWSSERVICEBASE(servicename, can_stop, can_shutdown, can_pause, can_powerevent, can_sessionchange)
{
  Clean();

  event_stopped = CreateEvent(NULL, TRUE, FALSE, NULL);
  if(event_stopped == NULL)
    {

    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         WINDOWSSERVICE::~WINDOWSSERVICE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_WINDOWS
*
* --------------------------------------------------------------------------------------------------------------------*/
WINDOWSSERVICE::~WINDOWSSERVICE()
{
  Clean();

  if(event_stopped)
    {
      CloseHandle(event_stopped);
      event_stopped = NULL;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         HANDLE WINDOWSSERVICE::GetHandleStoppedEvent()
* @brief      Get handle stopped event
* @ingroup    PLATFORM_WINDOWS
*
* @return     HANDLE :
*
* --------------------------------------------------------------------------------------------------------------------*/
HANDLE WINDOWSSERVICE::GetHandleStoppedEvent()
{
  return event_stopped;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool WINDOWSSERVICE::MustRestartService()
* @brief      Must restart service
* @ingroup    PLATFORM_WINDOWS
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool WINDOWSSERVICE::MustRestartService()
{
  return mustrestartservice;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void WINDOWSSERVICE::SetMustRestartService(bool mustrestartservice)
* @brief      Set must restart service
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  mustrestartservice : 
*
* --------------------------------------------------------------------------------------------------------------------*/
void WINDOWSSERVICE::SetMustRestartService(bool mustrestartservice)
{
  this->mustrestartservice = mustrestartservice;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void WINDOWSSERVICE::On_Start(DWORD argc, XCHAR** args)
* @brief      On start
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  argc :
* @param[in]  args :
*
* --------------------------------------------------------------------------------------------------------------------*/
void WINDOWSSERVICE::On_Start(DWORD argc, XCHAR** args)
{
  CreateThread(NULL, 0, Service_WorkerThread, this, 0, NULL);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void WINDOWSSERVICE::On_Stop()
* @brief      On stop
* @ingroup    PLATFORM_WINDOWS
*
* --------------------------------------------------------------------------------------------------------------------*/
void WINDOWSSERVICE::On_Stop()
{
  #ifdef APPFLOW_ACTIVE
  if(mainprocwindows.GetAppMain())
    {
      if(mainprocwindows.GetAppMain()->GetApplication())
        {
          if(mainprocwindows.GetAppMain()->GetApplication()->GetExitType() == APPFLOWBASE_EXITTYPE_UNKNOWN)  
            {
              mainprocwindows.GetAppMain()->GetApplication()->SetExitType(APPFLOWBASE_EXITTYPE_BY_USER);            
            }
        }
    }
  #endif

  if(event_stopped)
    {
      while(WaitForSingleObject(event_stopped, 0) != WAIT_OBJECT_0)
        {
          Sleep(5*1000);          
        }
    }
  
  if(MustRestartService())
    {
     
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void WINDOWSSERVICE::On_Shutdown()
* @brief      On shutdown
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
void WINDOWSSERVICE::On_Shutdown()
{
  #ifdef APPFLOW_ACTIVE
  if(mainprocwindows.GetAppMain())
    {
      if(mainprocwindows.GetAppMain()->GetApplication())
        {
           mainprocwindows.GetAppMain()->GetApplication()->SetSystemChangeStatus(XSYSTEM_CHANGESTATUSTYPE_REBOOT);
        }
    }
  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void WINDOWSSERVICE::On_PowerEvent(DWORD eventtype)
* @brief      On power event
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  eventtype :
*
* --------------------------------------------------------------------------------------------------------------------*/
void WINDOWSSERVICE::On_PowerEvent(DWORD eventtype)
{
  #ifdef APPFLOW_ACTIVE
  //XSYSTEM_CHANGESTATUSTYPE systemchangestatustype = XSYSTEM_CHANGESTATUSTYPE_UNKNOWN;

  if(mainprocwindows.GetAppMain())
    {
      if(mainprocwindows.GetAppMain()->GetApplication())
        {
          if (eventtype == PBT_APMSUSPEND)
            {
              mainprocwindows.GetAppMain()->GetApplication()->SetSystemChangeStatus(XSYSTEM_CHANGESTATUSTYPE_SUSPEND);
            }
        }
    }
  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void WINDOWSSERVICE::On_SessionChange(DWORD eventtype)
* @brief      On session change
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  eventtype :
*
* --------------------------------------------------------------------------------------------------------------------*/
void WINDOWSSERVICE::On_SessionChange(DWORD eventtype)
{
  #ifdef APPFLOW_ACTIVE
  //XSYSTEM_CHANGESTATUSTYPE systemchangestatustype = XSYSTEM_CHANGESTATUSTYPE_UNKNOWN;

  if(mainprocwindows.GetAppMain())
    {
      if(mainprocwindows.GetAppMain()->GetApplication())
        {
          switch(eventtype)
            {
              case WTS_CONSOLE_CONNECT      : mainprocwindows.GetAppMain()->GetApplication()->SetSystemChangeStatus(XSYSTEM_CHANGESTATUSTYPE_CONSOLE_CONNECT);        break;
              case WTS_CONSOLE_DISCONNECT   : mainprocwindows.GetAppMain()->GetApplication()->SetSystemChangeStatus(XSYSTEM_CHANGESTATUSTYPE_CONSOLE_DISCONNECT);     break;
              case WTS_SESSION_LOGON        : mainprocwindows.GetAppMain()->GetApplication()->SetSystemChangeStatus(XSYSTEM_CHANGESTATUSTYPE_SESSION_LOGON);          break;
              case WTS_SESSION_LOGOFF       : mainprocwindows.GetAppMain()->GetApplication()->SetSystemChangeStatus(XSYSTEM_CHANGESTATUSTYPE_SESSION_LOGOFF);         break;
              case WTS_SESSION_LOCK         : mainprocwindows.GetAppMain()->GetApplication()->SetSystemChangeStatus(XSYSTEM_CHANGESTATUSTYPE_SESSION_LOCK);           break;
              case WTS_SESSION_UNLOCK       : mainprocwindows.GetAppMain()->GetApplication()->SetSystemChangeStatus(XSYSTEM_CHANGESTATUSTYPE_SESSION_UNLOCK);         break;
            }
        }
    }
  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void WINDOWSSERVICE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    PLATFORM_WINDOWS
*
* --------------------------------------------------------------------------------------------------------------------*/
void WINDOWSSERVICE::Clean()
{
  event_stopped       = NULL;
  mustrestartservice  = false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DWORD WINAPI Service_WorkerThread(LPVOID lpparam)
* @brief      INAPI Service_WorkerThread
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  lpparam :
*
* @return     DWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI Service_WorkerThread(LPVOID lpparam)
{
  WINDOWSSERVICE* service = (WINDOWSSERVICE*)lpparam;
  bool            userstop = false;
  int             status   = 0;

  if(!service) return false;

  #ifdef APPFLOW_ACTIVE
  if(!mainprocwindows.Ini(&GEN_appmain, APPFLOWBASE_MODE_TYPE_SERVICE))
  #else
  if(!mainprocwindows.Ini())
  #endif
    {
      status = 1;
    }

  if(!status)
    {
      while(1)
        {
          if(!mainprocwindows.Update())
            {
              userstop = true;
              break;
            }
        }
    }

  
  APPFLOWBASE_EXITTYPE exittype = APPFLOWBASE_EXITTYPE_UNKNOWN;

  #ifdef APPFLOW_ACTIVE  
  exittype =  mainprocwindows.GetAppMain()->GetApplication()->GetExitType();
  #endif
  
  mainprocwindows.GetXPathExec()->Empty();
  mainprocwindows.End();


  if(exittype == APPFLOWBASE_EXITTYPE_BY_SERVICERELOAD)
    {
      service->SetMustRestartService(true);
    }

  if(service->GetHandleStoppedEvent()) 
    {
      SetEvent(service->GetHandleStoppedEvent());
    }

  if(userstop) 
    {
      service->Stop();
    }

  return ERROR_SUCCESS;
}


#pragma endregion


#pragma region MAINENTRYS

/*--------------------------------------------------------------------------------------------------------------------*/
/*  wmain    : Windows UNICODE entry  /CONSOLE mode                                                                   */
/*  WinMain  : Windows entry          /WINDOWS mode                                                                   */
/*  DllMain  : Windows DLL entry                                                                                      */
/*--------------------------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int wmain(int argc, wchar_t* argv[])
* @brief      main
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  argc :
* @param[in]  argv[] :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int wmain(int argc, wchar_t* argv[])
{ 
  XMEMORY_CONTROL_ACTIVATED

  #ifdef GOOGLETEST_ACTIVE
  testing::InitGoogleTest(&argc, argv);
  #endif

  mainprocwindows.SetHandle((void*)GetModuleHandle(NULL));

  mainprocwindows.GetXPathExec()->Set(argv[0]);
  mainprocwindows.CreateParams(argc, argv);  

  if(!mainprocwindows.IsRunningAsService())
    {
      // Need to enable FPE manually additionaly to the compiler flag
      // disabled right now as OpenAL has a FPE exception in windows, need to look into it
      // also windows timers and so on
      #ifdef FPE_ACTIVE
      _controlfp_s(NULL, 0, _EM_ZERODIVIDE); //  | _EM_OVERFLOW | _EM_UNDERFLOW | _EM_INVALID);
      #endif

      if(!SetConsoleCtrlHandler((PHANDLER_ROUTINE)Exception_ConsoleHandler, TRUE)) return -1;

      int status            = 0;
      int returngoogletest  = 0;
      int returncode        = 0;
      int ntotaltests       = 0;

      #ifdef APPFLOW_ACTIVE
      if(!mainprocwindows.Ini(&GEN_appmain, APPFLOWBASE_MODE_TYPE_APPLICATION))
      #else
      if(!mainprocwindows.Ini())
      #endif
        {
          status=1;
        }

      if(!status)
        {
          #ifdef GOOGLETEST_ACTIVE                
          returngoogletest = RUN_ALL_TESTS();
          ntotaltests = ::testing::UnitTest::GetInstance()->total_test_count();
          #else
          while(mainprocwindows.Update());
          #endif
        }

      #ifdef APPFLOW_ACTIVE
      if(mainprocwindows.GetAppMain())
        {
          if(mainprocwindows.GetAppMain()->GetApplication())
            {
              returncode = mainprocwindows.GetAppMain()->GetApplication()->GetExitType();
            }
        }
      #endif

      mainprocwindows.GetXPathExec()->Empty();

      mainprocwindows.End();

      if(!SetConsoleCtrlHandler((PHANDLER_ROUTINE)Exception_ConsoleHandler, FALSE)) 
        {
          return -1;
        }

      #ifdef GOOGLETEST_ACTIVE      
      returncode = returngoogletest;
      //if(!returngoogletest) returncode = ntotaltests;
      #endif

      return returncode;
    }
   else
    {
      WINDOWSSERVICE service(APPMODE_SERVICE_NAME);
      if(!WINDOWSSERVICE::Run(service))
        {

        }

      #ifdef APPFLOW_ACTIVE
      if(mainprocwindows.GetAppMain())
        {
          if(mainprocwindows.GetAppMain()->GetApplication())
            {
              return mainprocwindows.GetAppMain()->GetApplication()->GetExitType();
            }
        }
      #endif
    }

  return 0;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinst, LPSTR cmdline, int cmdshow)
* @brief      WinMain
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  hinstance :
* @param[in]  hprevinst :
* @param[in]  cmdline :
* @param[in]  cmdshow :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinst, LPSTR cmdline, int cmdshow)
{
  XMEMORY_CONTROL_ACTIVATED

  int      nargs        = 0;
  LPWSTR*  ptrpathexec  = CommandLineToArgvW(__L(""), &nargs);
  XSTRING  xpathexecutable;
  XSTRING  cmdlinestr   = cmdline;

  mainprocwindows.SetHandle((void*)hinstance);

  for(int c=0; c<nargs; c++)
    {
      xpathexecutable += ptrpathexec[c];
      if(c != nargs-1)  xpathexecutable += __L(" ");
    }

  mainprocwindows.GetXPathExec()->Set(xpathexecutable);

  mainprocwindows.CreateParams(cmdlinestr.Get());
  cmdlinestr.Empty();
  xpathexecutable.Empty(); 


  if(!mainprocwindows.IsRunningAsService())
    {   
      mainprocwindows.MainLoop();
    }
   else
    {
      WINDOWSSERVICE service(APPMODE_SERVICE_NAME);
      if(!WINDOWSSERVICE::Run(service))
        {


        }
    }

  #ifdef APPFLOW_ACTIVE
  if(mainprocwindows.GetAppMain())
    {
      if(mainprocwindows.GetAppMain()->GetApplication())
        {
          return mainprocwindows.GetAppMain()->GetApplication()->GetExitType();
        }
    }
  #endif

  return 0;
}


#else


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         BOOL WINAPI DllMain(HINSTANCE hinstDLL,XDWORD fdwReason,LPVOID lpvReserved)
* @brief      Dll Main
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  hinstDLL :
* @param[in]  fdwReason :
* @param[in]  lpvReserved :
*
* @return     BOOL :
*
* --------------------------------------------------------------------------------------------------------------------*/
BOOL WINAPI DllMain(HINSTANCE hinstDLL, XDWORD fdwReason,LPVOID lpvReserved)
{
  switch(fdwReason)
    {
      case DLL_PROCESS_ATTACH : { XMEMORY_CONTROL_ACTIVATED                                  
      
                                  XPATH xpathexecutable;

                                  xpathexecutable.AdjustSize(_MAXPATH);
                                  GetModuleFileName(hinstDLL, xpathexecutable.Get(), xpathexecutable.GetSize());
                                  xpathexecutable.AdjustSize();

                                  mainprocwindows.GetXPathExec()->Set(xpathexecutable);
                                  xpathexecutable.Empty();
                                  
                                  #ifdef APPFLOW_ACTIVE
                                  mainprocwindows.Ini(&GEN_appmain, APPFLOWBASE_MODE_TYPE_DINAMICLIBRARY);
                                  #else
                                  mainprocwindows.Ini();
                                  #endif
                                }
                                break;

      case DLL_THREAD_ATTACH  : break;

      case DLL_THREAD_DETACH  : break;

      case DLL_PROCESS_DETACH : mainprocwindows.GetXPathExec()->Empty();
                                mainprocwindows.End();
                                break;
    }

  return true;
}


#endif


#pragma endregion


#pragma region WINDOWS_STACKWALKER


#ifdef WINDOWS_STACKWALKER_ACTIVE


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void MAINPROCWINDOWSSTACKWALKER::OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName)
* @brief      On sym init
* @note       INTERNAL
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  szSearchPath :
* @param[in]  symOptions :
* @param[in]  szUserName :
*
* --------------------------------------------------------------------------------------------------------------------*/
void MAINPROCWINDOWSSTACKWALKER::OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName)
{
  Exception_Printf(true, NULL, __L("Stack trace: "));
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void MAINPROCWINDOWSSTACKWALKER::OnCallstackEntry(CallstackEntryType eType, CallstackEntry& entry)
* @brief      On callstack entry
* @note       INTERNAL
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  eType :
* @param[in]  entry :
*
* --------------------------------------------------------------------------------------------------------------------*/
void MAINPROCWINDOWSSTACKWALKER::OnCallstackEntry(CallstackEntryType eType, CallstackEntry& entry)
{
  XSTRING modulename;
  XSTRING linefilename;
  XSTRING name;

  if((eType != lastEntry) && (entry.offset != 0))
    {
      if(entry.name[0]         == 0)  strcpy_s(entry.name, "(function-name not available)");
      if(entry.undName[0]      != 0)  strcpy_s(entry.name, entry.undName);
      if(entry.undFullName[0]  != 0)  strcpy_s(entry.name, entry.undFullName);
      if(entry.lineFileName[0] == 0)
        {
          strcpy_s(entry.lineFileName, "(filename not available)");
          if (entry.moduleName[0] == 0) strcpy_s(entry.moduleName, "(module-name not available)");
        }

      modulename   = entry.moduleName;
      linefilename = entry.lineFileName;
      name         = entry.name;

      Exception_Printf(true, NULL, __L("%08X  %-48s (%6d) %-64s"), (LPVOID) entry.offset, name.Get(), entry.lineNumber, linefilename.Get());
    }
}


#endif


#pragma endregion


#pragma region EXECPTION_FUNTIONS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         BOOL Exception_ConsoleHandler(DWORD fdwctrltype)
* @brief      Exception_ConsoleHandler
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  fdwctrltype : 
* 
* @return     BOOL : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
BOOL Exception_ConsoleHandler(DWORD fdwctrltype)
{
  #ifdef APPFLOW_ACTIVE
  APPFLOWBASE* app = NULL;
  if(mainprocwindows.GetAppMain()) app = mainprocwindows.GetAppMain()->GetApplication();
  #endif

  switch(fdwctrltype)
    {
      case CTRL_BREAK_EVENT     : Exception_Printf(false, __L("EVENT"), __L("Ctrl-Break event."));
                                  #ifdef APPFLOW_ACTIVE
                                  if(app) app->SetExitType(APPFLOWBASE_EXITTYPE_BY_USER);
                                  #endif
                                  return FALSE;

      case CTRL_C_EVENT         : Exception_Printf(false, __L("EVENT"), __L("Ctrl-C event."));
                                  #ifdef APPFLOW_ACTIVE
                                  if(app) app->SetExitType(APPFLOWBASE_EXITTYPE_BY_USER);
                                  #endif
                                  return FALSE;

      case CTRL_CLOSE_EVENT     :
                                  #ifdef APPFLOW_ACTIVE
                                  if(app) app->SetExitType(APPFLOWBASE_EXITTYPE_BY_USER);
                                  #endif
                                  FreeConsole();         // Detachxconsole
                                  break;

      case CTRL_LOGOFF_EVENT    : Exception_Printf(false, __L("EVENT"), __L("Ctrl-Logoff event."));
                                  #ifdef APPFLOW_ACTIVE
                                  if(app) app->SetExitType(APPFLOWBASE_EXITTYPE_BY_SHUTDOWN);
                                  #endif
                                  FreeConsole();         // Detachxconsole
                                  ExitThread(0);         // Prevent closing.
                                  break;

      case CTRL_SHUTDOWN_EVENT  : Exception_Printf(false, __L("EVENT"), __L("Ctrl-Shutdown event."));
                                  #ifdef APPFLOW_ACTIVE
                                  if(app) app->SetExitType(APPFLOWBASE_EXITTYPE_BY_SHUTDOWN);
                                  #endif
                                  FreeConsole();         // Detachxconsole
                                  ExitThread(0);         // Prevent closing.
                                  break;

                      default   : return FALSE;
    }

  return FALSE;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int Exception_Filter(XDWORD code, struct _EXCEPTION_POINTERS* ep)
* @brief      Exception_Filter
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  code : 
* @param[in]  ep : 
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int Exception_Filter(XDWORD code, struct _EXCEPTION_POINTERS* ep)
{
  XSTRING exception;
  XSTRING description;
  XSTRING string;

  allexceptiontext = new XSTRING();
  if(!allexceptiontext) return 0;

  APPFLOWBASE* app = NULL;
  #ifdef APPFLOW_ACTIVE
  if(mainprocwindows.GetAppMain()) app = mainprocwindows.GetAppMain()->GetApplication();
  #endif


  switch(code)
    {
      case EXCEPTION_ACCESS_VIOLATION         : exception   = __L("ACCESS VIOLATION");
                                                description = __L("Read from or write to a virtual address for which it does not have access.");
                                                break;

      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED    : exception   = __L("ARRAY BOUNDS EXCEEDED");
                                                description = __L("Access an array element that is out of bounds");
                                                break;

      case EXCEPTION_DATATYPE_MISALIGNMENT    : exception   = __L("DATATYPE MISALIGNMENT");
                                                description = __L("Read or write data that is misaligned on hardware that does not provide alignment.");
                                                break;

      case EXCEPTION_FLT_DENORMAL_OPERAND     : exception   = __L("FLT DENORMAL OPERAND]");
                                                description = __L("One of the operands in a floating point operation is denormal.");
                                                break;

      case EXCEPTION_FLT_DIVIDE_BY_ZERO       : exception   = __L("FLT DIVIDE BY ZERO");
                                                description = __L("Divide a floating point value by a floating point divisor of 0 (zero).");
                                                break;

      case EXCEPTION_FLT_INEXACT_RESULT       : exception   = __L("FLT INEXACT RESULT");
                                                description = __L("A floating point operation cannot be represented exactly as a decimal fraction.");
                                                break;

      case EXCEPTION_FLT_INVALID_OPERATION    : exception   = __L("FLT INVALID OPERATION");
                                                description = __L("A floating point exception (not defined).");
                                                break;

      case EXCEPTION_FLT_OVERFLOW             : exception   = __L("FLT OVERFLOW");
                                                description = __L("The exponent of a floating point operation is greater than the magnitude allowed by the corresponding type.");
                                                break;

      case EXCEPTION_FLT_STACK_CHECK          : exception   = __L("FLT STACK CHECK");
                                                description = __L("The stack has overflowed or underflowed, because of a floating point operation.");
                                                break;

      case EXCEPTION_FLT_UNDERFLOW            : exception   = __L("FLT UNDERFLOW");
                                                description = __L("The exponent of a floating point operation is less than the magnitude allowed by the corresponding type.");
                                                break;

      case EXCEPTION_GUARD_PAGE               : exception   = __L("GUARD PAGE");
                                                description = __L("Accessed memory allocated with the PAGE GUARD modifier.");
                                                break;

      case EXCEPTION_ILLEGAL_INSTRUCTION      : exception   = __L("ILLEGAL INSTRUCTION");
                                                description = __L("Tries to execute an invalid instruction.");
                                                break;

      case EXCEPTION_IN_PAGE_ERROR            : exception   = __L("IN PAGE ERROR");
                                                description = __L("Tries to access a page that is not present, and the system is unable to load the page.");
                                                break;

      case EXCEPTION_INT_DIVIDE_BY_ZERO       : exception   = __L("INT DIVIDE BY ZERO");
                                                description = __L("Divide an integer value by an integer divisor of 0 (zero).");
                                                break;

      case EXCEPTION_INT_OVERFLOW             : exception   = __L("INT OVERFLOW");
                                                description = __L("The result of an integer operation creates a value that is too large to be held by the destination register.");
                                                break;

      case EXCEPTION_INVALID_DISPOSITION      : exception   = __L("INVALID DISPOSITION");
                                                description = __L("An exception handler returns an invalid disposition to the exception dispatcher.");
                                                break;

      case EXCEPTION_INVALID_HANDLE           : exception   = __L("INVALID HANDLE") ;
                                                description = __L("The thread used a handle to a kernel object that was invalid (probably because it had been closed).");
                                                break;

      case EXCEPTION_NONCONTINUABLE_EXCEPTION : exception   = __L("NONCONTINUABLE EXCEPTION");
                                                description = __L("The thread attempts to continue execution after a non-continuable exception occurs.");
                                                break;

      case EXCEPTION_PRIV_INSTRUCTION         : exception   = __L("PRIV INSTRUCTION");
                                                description = __L("Execute an instruction with an operation that is not allowed in the current computer mode.");
                                                break;

      case EXCEPTION_STACK_OVERFLOW           : exception   = __L("STACK OVERFLOW");
                                                description = __L("Stack overflow");
                                                break;

      case EXCEPTION_BREAKPOINT               : //Exception_Printf(__L("[EXCEPTION_BREAKPOINT]")              , __L("A breakpoint is encountered"));
      case EXCEPTION_SINGLE_STEP              : //Exception_Printf(__L("[EXCEPTION_SINGLE_STEP]")             , __L("A trace trap or other single instruction mechanism signals that one instruction is executed."));
      case STATUS_UNWIND_CONSOLIDATE          : return EXCEPTION_CONTINUE_SEARCH;
                                                break;

                                      default : return EXCEPTION_EXECUTE_HANDLER;
    }

  Exception_Printf(true, __L("EXCEPTION"), __L("%s: %s"), exception.Get(), description.Get());

  #ifdef APPFLOW_ACTIVE
  if(app)
    {
      if(app->GetTimerGlobal())
        {
          app->GetTimerGlobal()->GetMeasureString(string, true);
          Exception_Printf(false, NULL, __L("Time working: %s."), string.Get());
        }
    }
  #endif

  switch(code)
    {
      case EXCEPTION_ACCESS_VIOLATION         :
      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED    :
      case EXCEPTION_DATATYPE_MISALIGNMENT    :
      case EXCEPTION_FLT_DENORMAL_OPERAND     :
      case EXCEPTION_FLT_DIVIDE_BY_ZERO       :
      case EXCEPTION_FLT_INEXACT_RESULT       :
      case EXCEPTION_FLT_INVALID_OPERATION    :
      case EXCEPTION_FLT_OVERFLOW             :
      case EXCEPTION_FLT_STACK_CHECK          :
      case EXCEPTION_FLT_UNDERFLOW            :
      case EXCEPTION_GUARD_PAGE               :
      case EXCEPTION_ILLEGAL_INSTRUCTION      :
      case EXCEPTION_IN_PAGE_ERROR            :
      case EXCEPTION_INT_DIVIDE_BY_ZERO       :
      case EXCEPTION_INT_OVERFLOW             :
      case EXCEPTION_INVALID_DISPOSITION      :
      case EXCEPTION_INVALID_HANDLE           :
      case EXCEPTION_NONCONTINUABLE_EXCEPTION :
      case EXCEPTION_PRIV_INSTRUCTION         :
      case EXCEPTION_STACK_OVERFLOW           : { 
                                                  #ifdef WINDOWS_STACKWALKER_ACTIVE

                                                  MAINPROCWINDOWSSTACKWALKER stackwalker;
                                                  stackwalker.ShowCallstack(GetCurrentThread(), ep->ContextRecord);


                                                  string.Format(__L("EXCEPTION %s"), exception.Get());

                                                  #ifdef DIO_ALERTS_ACTIVE
                                                  DIOALERT* alert = GEN_DIOALERTS.CreateAlert(DIOALERTLEVEL_DANGER, string.Get(), allexceptiontext->Get());
                                                  if(alert)
                                                    {
                                                      GEN_DIOALERTS.Send(DIOALERTSSENDER_ALL, DIOALERTS_CONDITIONS_ID_GENINTERN_EXCEPTION, alert);
                                                      delete alert;
                                                    }
                                                  #endif

                                                  #ifdef APPFLOW_ACTIVE
                                                  if(app) exit(APPFLOWBASE_EXITTYPE_BY_SERIOUSERROR);
                                                  #endif

                                                  #endif 
                                                }
                                                break;
    }

  if(allexceptiontext)
    {
      delete allexceptiontext;
      allexceptiontext = NULL;
    }

  return EXCEPTION_EXECUTE_HANDLER;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool Exception_Printf(bool iserror, XCHAR* title, XCHAR* mask, ...)
* @brief      Exception_Printf
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  iserror : 
* @param[in]  title : 
* @param[in]  mask : 
* @param[in]  ... : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool Exception_Printf(bool iserror, XCHAR* title, XCHAR* mask, ...)
{
  XSTRING outstring;

  va_list arg;

  va_start(arg, mask);

  outstring.FormatArg(mask, &arg);

  va_end(arg);

  bool do_log = false;

  #ifdef XLOG_ACTIVE
  if(GEN_XLOG.IsActive())
    {
      GEN_XLOG.SetFilters(__L(""));

      if(GEN_XLOG.AddEntry(iserror?XLOGLEVEL_ERROR:XLOGLEVEL_WARNING, title, false, outstring.Get()))
        {
          do_log = true;
        }
    }
  #endif

  if(allexceptiontext)
    {
      (*allexceptiontext) += outstring;
      (*allexceptiontext) += __L("\r\n");
    }

  #ifdef XTRACE_ACTIVE
  if(!do_log)
    {
      int level = iserror?4:0;
      XTRACE_PRINTCOLOR(level, outstring.Get());
    }
  #endif

  return true;
}


#pragma endregion


#pragma endregion

