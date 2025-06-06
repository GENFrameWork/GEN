/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       MainProcLINUX.cpp
* 
* @class      MAINPROCLINUX
* @brief      LINUX Main procedure class
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <utmp.h>
#include <time.h>
#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <wchar.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <execinfo.h>
#include <errno.h>
#include <cxxabi.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fenv.h>

#ifdef GOOGLETEST_ACTIVE      
#include "gtest/gtest.h"
#endif

#include "MainProcLINUX.h"

#include "XLINUXFactory.h"
#include "XLINUXRand.h"
#include "XLINUXSleep.h"
#include "XLINUXSystem.h"
#include "XLINUXTrace.h"

#ifdef XPROCESSMANAGER_ACTIVE
  #include "XLINUXProcessManager.h"
#endif

#ifdef XSHAREDMEMORYMANAGER_ACTIVE
  #include "XLINUXSharedMemoryManager.h"
#endif

#ifdef XDRIVEIMAGE_ACTIVE
  #include "XLINUXDriveImage.h"
#endif

#ifdef XEEPROMMEMORYMANAGER_ACTIVE
  #include "XLINUXEEPROMMemoryManager.h"
#endif


#ifdef GRP_ACTIVE
#include "GRPLINUXFactory.h"
#endif

#ifdef INP_ACTIVE
#include "INPManager.h"
#include "INPFactory.h"
#include "INPLINUXFactory.h"
#endif

#ifdef SND_ACTIVE
#include "SNDLINUXFactory.h"
#endif

#ifdef DIO_ACTIVE
#include "DIODNSResolver.h"
#include "DIOLINUXFactory.h"

  #ifdef DIO_GPIO_ACTIVE

    #include "DIOLINUXGPIO.h"

    #ifdef HW_PC
      #ifdef DIO_GPIO_PCPARALLEL_ACTIVE
        #include "DIOLINUXGPIOPCParallel.h"
      #endif
    #endif
    
    #ifdef HW_RASPBERRYPI
    #include "DIOLINUXGPIORPi.h"
    #endif
    
    #if defined(HW_ARM) || defined(HW_ARM64)
    #include "DIOLINUXGPIOARM.h"
    #endif

  #endif

  #ifdef DIO_PING_ACTIVE
  #include "DIOLINUXPing.h"
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

#include "APPFlowBase.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#ifdef XTRACE_ACTIVE
XLINUXTRACE           linuxdebugtrace;
#endif

MAINPROCLINUX         mainproclinux;
bool                  libmainproclinux      = false;
XSTRING               allexceptiontext;

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS

void                  Signal_Ini                (void);
static void           Signal_Handler            (int sig);
bool                  Signal_Printf             (bool iserror, XCHAR* title, XCHAR* mask, ...);
static inline void    Signal_PrintfStackTrace   (FILE *out = stderr, unsigned int max_frames = 63);
bool                  Signal_RunLevel           (XCHAR& previous, XCHAR& actual);


#pragma region CLASS_MAINPROCLINUX


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         MAINPROCLINUX::MAINPROCLINUX()
* @brief      Constructor of class
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
MAINPROCLINUX::MAINPROCLINUX()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         MAINPROCLINUX::~MAINPROCLINUX()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
MAINPROCLINUX::~MAINPROCLINUX()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MAINPROCLINUX::Ini(APPFLOWMAIN* appmain, APPFLOWBASE_MODE_TYPE applicationmode)
* @brief      Ini
* @ingroup    PLATFORM_LINUX
*
* @param[in]  appmain :
* @param[in]  applicationmode :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MAINPROCLINUX::Ini(APPFLOWMAIN* appmain, APPFLOWBASE_MODE_TYPE applicationmode)
{
  this->appmain = appmain;

  //---------------------------------------------------------------------------
  #ifdef FPE_ACTIVE
  feenableexcept(FE_DIVBYZERO | FE_OVERFLOW /*| FE_UNDERFLOW | FE_INEXACT*/);
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

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MAINPROCLINUX::Update()
* @brief      Update
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MAINPROCLINUX::Update()
{
  #ifdef INP_ACTIVE
  GEN_INPMANAGER.Update();
  #endif

  #ifdef APPFLOW_ACTIVE

  if(appmain)
    {
      if(!appmain->Update()) return false;
    }

  #else

   if(Main_Proc_Update)
    {
      if(!Main_Proc_Update(this)) return false;
    }

  #endif

  #ifndef GRP_ACTIVE
  usleep(5000);
  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MAINPROCLINUX::End()
* @brief      End
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MAINPROCLINUX::End()
{
  #ifdef APPFLOW_ACTIVE

  if(appmain) appmain->End();

  #else

  if(Main_Proc_End)
    {
      if(!Main_Proc_End(this)) return false;
    }

  #endif

  #ifdef APPFLOW_ACTIVE

  if(appmain) appmain->Delete();

  #else

  if(Main_Proc_PlatformEnd)
    {
      if(!Main_Proc_PlatformEnd(this)) return false;
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

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MAINPROCLINUX::Factorys_Ini()
* @brief      Factorys ini
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MAINPROCLINUX::Factorys_Ini()
{
  if(!XFACTORY::SetInstance(new XLINUXFACTORY())) 
    {
      return false;
    }

  XFEEDBACK_CONTROL_CREATE
  
  #ifdef XSYSTEM_ACTIVE  
  if(!XSYSTEM::SetInstance(new XLINUXSYSTEM())) 
    {
      return false;
    }
  XBUFFER::SetGlobalHardwareUseLittleEndian(GEN_XSYSTEM.HardwareUseLittleEndian());
  #endif  
  
  if(!XRAND::SetInstance(new XLINUXRAND())) 
    {
      return false;
    }

  #ifdef XSLEEP_ACTIVE
  if(!XSLEEP::SetInstance(new XLINUXSLEEP())) 
    {
      return false;
    }
  #endif
    
  #ifdef XPROCESSMANAGER_ACTIVE
  if(!XPROCESSMANAGER::SetInstance(new XLINUXPROCESSMANAGER())) 
    {
      return false;
    }
  #endif
  
  #ifdef XSHAREDMEMORYMANAGER_ACTIVE
  if(!XSHAREDMEMORYMANAGER::SetInstance(new XLINUXSHAREDMEMORYMANAGER())) 
    {
      return false;  
    }
  #endif
    
  #ifdef XDRIVEIMAGE_ACTIVE
  if(!XDRIVEIMAGE::SetInstance(new XLINUXDRIVEIMAGE())) 
    {
      return false;
    }
  #endif

  #ifdef XEEPROMMEMORYMANAGER_ACTIVE
  if(!XEEPROMMEMORYMANAGER::SetInstance(new XLINUXEEPROMMEMORYMANAGER())) 
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
  if(!INPFACTORY::SetInstance(new INPLINUXFACTORY())) 
    {
      return false;  
    }
  #endif

  #ifdef DIO_ACTIVE
  if(!DIOFACTORY::SetInstance(new DIOLINUXFACTORY())) 
    {
      return false;
    }
  
    #ifdef DIO_GPIO_ACTIVE

      #ifdef HW_PC
        #ifdef DIO_GPIO_PCPARALLEL_ACTIVE
        if(!DIOGPIO::SetInstance(new DIOLINUXGPIOPCPARALLEL())) 
          {
            return false;
          }
        #endif
      #endif

      #ifdef HW_RASPBERRYPI
      if(!DIOGPIO::SetInstance(new DIOLINUXGPIORPI())) 
        {
          return false;
        }
      #endif

      #if defined(HW_ARM) || defined(HW_ARM64)
      if(!DIOGPIO::SetInstance(new DIOLINUXGPIOARM())) 
        {
          return false;
        }
      #endif
    
    if(!DIOGPIO::GetInstance().Ini()) 
      {
        return false;
      }

    #endif

    #ifdef DIO_PING_ACTIVE    
    if(!DIOPING::SetInstance(new DIOLINUXPING())) 
      {
        return false;
      }    
    #endif
	
  #endif

  #ifdef SND_ACTIVE
  if(!SNDFACTORY::SetInstance(new SNDLINUXFACTORY())) 
    {    
      return false;
    }

  if(!SNDFACTORY::GetInstance().Ini()) 
    {
      // return false;
    }
  #endif

  #ifdef GRP_ACTIVE
  if(!GRPFACTORY::SetInstance(new GRPLINUXFACTORY())) 
    {
      return false;
    }
  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MAINPROCLINUX::Factorys_End()
* @brief      Factorys end
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MAINPROCLINUX::Factorys_End()
{
  #ifdef GRP_ACTIVE
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

  XTRANSLATION::DelInstance();
  XTRANSLATION_GEN::DelInstance();

  #ifdef XSLEEP_ACTIVE
  XSLEEP::DelInstance();
  #endif

  XRAND::DelInstance();

  #ifdef XSYSTEM_ACTIVE
  XSYSTEM::DelInstance();
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
* @fn         void MAINPROCLINUX::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
* 
* --------------------------------------------------------------------------------------------------------------------*/
void MAINPROCLINUX::Clean()
{

}


#pragma endregion


#pragma region MAINENTRYS

/*--------------------------------------------------------------------------------------------------------------------*/
/*  main        : Linux normal entry                                                                                  */
/*  LIBRARY_Ini :                                                                                                     */
/*  LIBRARY_End : Linux Library entry (Construct / Destruct)                                                          */
/*                                                                                                                    */
/*--------------------------------------------------------------------------------------------------------------------*/


#if !defined(APPMODE_LIBRARY_STATIC) && !defined(APPMODE_LIBRARY_DINAMIC)

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int main(int argc, char* argv[])
* @brief      ain
* @ingroup    PLATFORM_LINUX
*
* @param[in]  argc :
* @param[in]  argv[] :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
  XMEMORY_CONTROL_ACTIVATED

  #ifdef GOOGLETEST_ACTIVE
  testing::InitGoogleTest(&argc, argv);
  #endif

  Signal_Ini();

  char  sztmp[64];
  int   pathsize = 512;
  char  path[pathsize];

  memset(path, 0, pathsize);

  sprintf(sztmp, "/proc/%d/exe", getpid());
  int rb = readlink(sztmp, path, pathsize);
  if(rb >= 0)
    {
      path[rb] = '\0';
      mainproclinux.GetXPathExec()->Set(path);
    }
   else mainproclinux.GetXPathExec()->Set(argv[0]);

  mainproclinux.CreateParams(argc, argv);

  int status = 0;
  
  #ifdef APPFLOW_ACTIVE
  if(!mainproclinux.Ini(&GEN_appmain, APPFLOWBASE_MODE_TYPE_APPLICATION))
  #else
  if(!mainproclinux.Ini())
  #endif
    {
      status=1;
    }

  if(!status)
    {
      #ifdef GOOGLETEST_ACTIVE      
      int code = RUN_ALL_TESTS();
      if(code)
        {
          status = 1; 
        }
      #else
      while(mainproclinux.Update());
      #endif      
    }

  int returncode = 0;

  #ifdef APPFLOW_ACTIVE
  if(mainproclinux.GetAppMain())
    {
      if(mainproclinux.GetAppMain()->GetApplication())
        {
          returncode = mainproclinux.GetAppMain()->GetApplication()->GetExitType();
        }
    }
  #endif

  mainproclinux.GetXPathExec()->Empty();

  mainproclinux.End();
      
  return returncode;
}

#endif


#ifdef APPMODE_LIBRARY_DINAMIC


extern "C"
{

__attribute__((constructor))
/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void LIBRARY_Ini(void)
* @brief      LIB Ini
* @ingroup    PLATFORM_LINUX
*
* @param[in]  void :
*
* --------------------------------------------------------------------------------------------------------------------*/
static void LIBRARY_Ini(void)
{
  XMEMORY_CONTROL_ACTIVATED 

  char  xpathexecutable[_MAXPATH];
  int   status = 0;
  char  sztmp[64];

  memset(xpathexecutable, 0, _MAXPATH);

  sprintf(sztmp, "/proc/%d/exe", getpid());
  int rb = readlink(sztmp, xpathexecutable, _MAXPATH);
  if(rb >= 0) xpathexecutable[rb] = '\0';

  mainproclinux.GetXPathExec()->Set(xpathexecutable);
  #ifdef APPFLOW_ACTIVE
  mainproclinux.Ini(&GEN_appmain, APPFLOWBASE_MODE_TYPE_DINAMICLIBRARY);
  #else
  mainproclinux.Ini();
  #endif

  libmainproclinux = true;
}



__attribute__((destructor))
/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void LIBRARY_End(void)
* @brief      LIB End
* @ingroup    PLATFORM_LINUX
*
* @param[in]  void :
*
* --------------------------------------------------------------------------------------------------------------------*/
static void LIBRARY_End(void)
{
  mainproclinux.GetXPathExec()->Empty();
  mainproclinux.End();
}


}

#endif


#pragma endregion


#pragma region SIGNAL_FUNCTIONS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void Signal_Ini(void)
* @brief      signal ini
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
void Signal_Ini(void)
{
  /*
  signalaction.sa_handler = Signal_Handler;
  sigemptyset(&signalaction.sa_mask);
  signalaction.sa_flags   = SA_SIGINFO | SA_UNSUPPORTED | SA_EXPOSE_TAGBITS;
  signalaction.sa_mask    = signalaction.sa_mask;

  sigaddset(&signalaction.sa_mask, SIGHUP);       sigaction(SIGHUP    , &signalaction , (struct sigaction*)NULL);
  sigaddset(&signalaction.sa_mask, SIGINT);       sigaction(SIGINT    , &signalaction , (struct sigaction*)NULL);
  sigaddset(&signalaction.sa_mask, SIGQUIT);      sigaction(SIGQUIT   , &signalaction , (struct sigaction*)NULL);
  sigaddset(&signalaction.sa_mask, SIGILL);       sigaction(SIGILL    , &signalaction , (struct sigaction*)NULL);
  sigaddset(&signalaction.sa_mask, SIGBUS);       sigaction(SIGBUS    , &signalaction , (struct sigaction*)NULL);
  sigaddset(&signalaction.sa_mask, SIGFPE);       sigaction(SIGFPE    , &signalaction , (struct sigaction*)NULL);
//sigaddset(&signalaction.sa_mask, SIGKILL);      sigaction(SIGKILL   , &signalaction , (struct sigaction*)NULL);   // Not Available
  sigaddset(&signalaction.sa_mask, SIGSEGV);      sigaction(SIGSEGV   , &signalaction , (struct sigaction*)NULL);
  sigaddset(&signalaction.sa_mask, SIGTERM);      sigaction(SIGTERM   , &signalaction , (struct sigaction*)NULL);
  sigaddset(&signalaction.sa_mask, SIGSTKFLT);    sigaction(SIGSTKFLT , &signalaction , (struct sigaction*)NULL);
//sigaddset(&signalaction.sa_mask, SIGSTOP);      sigaction(SIGSTOP   , &signalaction , (struct sigaction*)NULL);   // Not Available
  sigaddset(&signalaction.sa_mask, SIGTSTP);      sigaction(SIGTSTP   , &signalaction , (struct sigaction*)NULL);
  sigaddset(&signalaction.sa_mask, SIGABRT);      sigaction(SIGABRT   , &signalaction , (struct sigaction*)NULL);

  sigaddset(&signalaction.sa_mask, SIGUSR1);      sigaction(SIGUSR1   , &signalaction , (struct sigaction*)NULL);
  sigaddset(&signalaction.sa_mask, SIGUSR2);      sigaction(SIGUSR2   , &signalaction , (struct sigaction*)NULL);
  */


  struct sigaction  act;
  int               i = 0;

  memset(&act, 0, sizeof act);

  sigemptyset(&act.sa_mask);
  act.sa_handler = Signal_Handler;
  act.sa_flags = 0;

  for(i = SIGRTMIN; i <= SIGRTMAX; i++)
    {
      if(sigaction(i, &act, NULL)) 
        {
          fprintf(stderr, "Cannot install realtime signal %d handler: %s.\n", i, strerror(errno));
          exit(EXIT_FAILURE);
        }
    }
  
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         static void Signal_Handler(int sig)
* @brief      oid Signal_Handler
* @ingroup    PLATFORM_LINUX
*
* @param[in]  sig :
*
* @return     static :
*
* --------------------------------------------------------------------------------------------------------------------*/
static void Signal_Handler(int sig)
{
  XSTRING signalstr;
  XSTRING description;
  XSTRING string;
  bool    iserror = false;

  allexceptiontext.Empty();

  #ifdef APPFLOW_ACTIVE
  APPFLOWBASE* app = NULL;
  if(mainproclinux.GetAppMain()) app = mainproclinux.GetAppMain()->GetApplication();

  if(app)
    {
      if(app->GetExitType()!=APPFLOWBASE_EXITTYPE_UNKNOWN) return;
    }
  #endif

  switch(sig)
    {
      case SIGSEGV    : iserror     = true;
                        signalstr   = __L("SEGV");
                        description = __L("Segmentation fault.");
                        break;

      case SIGBUS     : iserror     = true;
                        signalstr   = __L("BUS");
                        description = __L("Bus fault.");
                        break;

      case SIGFPE     : iserror     = true;
                        signalstr   = __L("FPE");
                        description = __L("Erroneous arithmetic operation (such as divide by zero).");
                        break;

      case SIGABRT    : iserror     = true;
                        signalstr   = __L("ABRT");
                        description = __L("Error: Abnormal termination condition.");
                        break;

      case SIGSTKFLT  : iserror     = true;
                        signalstr   = __L("STKFLT");
                        description = __L("Error: Stack fault.");
                        break;

      case SIGILL     : iserror     = true;
                        signalstr   = __L("ILL");
                        description = __L("Error: Illegal instruction.");
                        break;


      case SIGHUP     : signalstr   = __L("HUP");
                        description = __L("Program hanged up.");
                        break;

      case SIGTERM    : signalstr   = __L("TERM");
                        description = __L("Termination request, sent to the program.");
                        break;

      case SIGINT     : signalstr   = __L("INT");
                        description = __L("External interrupt, usually initiated by the user.");
                        break;

      case SIGQUIT    : signalstr   = __L("QUIT");
                        description = __L("Terminal quit.");
                        break;

      case SIGTSTP    : signalstr   = __L("TSTP");
                        description = __L("Terminal stop signalstr.");
                        break;

      case SIGKILL    : // Not Available -> Kill (can't be caught or ignored).
                        break;

      case SIGSTOP    : // Not Available -> Stop executing(can't be caught or ignored).
                        break;
      case SIGUSR1    :
      case SIGUSR2    :   signalstr = __L("USR");
                          description = __L("User signal received.");
                          break;
    }

  Signal_Printf(iserror, __L("SIGNAL"), __L("[SIG%s] Error: %s"), signalstr.Get(), description.Get());

  #ifdef APPFLOW_ACTIVE
  if(app)
    {
      if(app->GetTimerGlobal())
        {
          XSTRING string2;

          app->GetTimerGlobal()->GetMeasureString(string2, true);
          Signal_Printf(false, NULL,__L("Time working: %s."), string2.Get());
        }
    }
  #endif

  switch(sig)
    {
      case SIGSEGV    :
      case SIGBUS     :
      case SIGFPE     :
      case SIGABRT    :
      case SIGSTKFLT  :
      case SIGILL     : { Signal_PrintfStackTrace();

                          string.Format(__L("SIGNAL %s: %s"), signalstr.Get(), description.Get());

                          #ifdef DIO_ALERTS_ACTIVE
                          DIOALERT* alert = GEN_DIOALERTS.CreateAlert(DIOALERTLEVEL_DANGER, string.Get(), allexceptiontext.Get());
                          if(alert)
                            {
                              GEN_DIOALERTS.Send(DIOALERTSSENDER_ALL, DIOALERTS_CONDITIONS_ID_GENINTERN_EXCEPTION, alert);
                              delete alert;
                            }
                          #endif

                          allexceptiontext.Empty();

                          fprintf(stderr," \n");

                          #ifdef APPFLOW_ACTIVE
                          if(app)
                            {
                              app->SetExitType(APPFLOWBASE_EXITTYPE_BY_SERIOUSERROR);
                              app->AppProc_End();
                            }
                          #endif

                          signal(sig, SIG_DFL);
                          kill(getpid(), sig);

                          //exit(sig);
                        }
                        break;


      case SIGINT     :
                        #ifdef APPFLOW_ACTIVE
                        if(app) app->SetExitType(APPFLOWBASE_EXITTYPE_BY_USER);
                        #endif
                        allexceptiontext.Empty();
                        break;

      case SIGQUIT    :
                        #ifdef APPFLOW_ACTIVE
                        if(app) app->SetExitType(APPFLOWBASE_EXITTYPE_BY_SHUTDOWN);
                        #endif
                        allexceptiontext.Empty();
                        break;

      case SIGHUP     :
      case SIGTERM    : { XCHAR previous;
                          XCHAR actual;

                          #ifdef APPFLOW_ACTIVE
                          if(app)
                            {
                              if(app->GetExitType()!=APPFLOWBASE_EXITTYPE_UNKNOWN) return;
                            }
                          #endif

                          if(Signal_RunLevel(previous, actual))
                            {
                              if(actual == __C('0'))
                                {
                                  #ifdef APPFLOW_ACTIVE
                                  if(app) app->SetExitType(APPFLOWBASE_EXITTYPE_BY_SHUTDOWN);
                                  #endif
                                  allexceptiontext.Empty();
                                  break;
                                }
                            }

                          #ifdef APPFLOW_ACTIVE
                          if(app) app->SetExitType(APPFLOWBASE_EXITTYPE_BY_USER);
                          #endif
                          allexceptiontext.Empty();
                        }
                        break;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool Signal_Printf(bool iserror, XCHAR* title, XCHAR* mask, ...)
* @brief      ignal_Printf
* @ingroup    PLATFORM_LINUX
*
* @param[in]  iserror :
* @param[in]  title :
* @param[in]  mask :
* @param[in]  ... :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool Signal_Printf(bool iserror, XCHAR* title, XCHAR* mask, ...)
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

  allexceptiontext += outstring;
  allexceptiontext += __L("\r\n");

  #ifdef XTRACE_ACTIVE
  if(!do_log)
    {
      int level = iserror?4:0;

     XTRACE_PRINTCOLOR(level, outstring.Get());
    }
  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         static inline void Signal_PrintfStackTrace(FILE*out, unsigned int max_frames)
* @brief      nline void Signal_PrintfStackTrace
* @ingroup    PLATFORM_LINUX
*
* @param[in]  FILE*out :
* @param[in]  int max_frames :
*
* @return     static :
*
* --------------------------------------------------------------------------------------------------------------------*/
static inline void Signal_PrintfStackTrace(FILE *out, unsigned int max_frames)
{
  void*  addrlist[max_frames+1];        // storage array for stack trace address data
  XDWORD addrlen = 0;

  memset(addrlist,0, sizeof(addrlist));

  addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));  // retrieve current stack addresses

  if(addrlen == 0)
    {
      Signal_Printf(true, NULL, __L("Stack trace: Not available."));
      return;

    } else Signal_Printf(true, NULL, __L("Stack trace: (%d) calls"),addrlen);

  // resolve addresses into strings containing "filename(function+address)",
  // Actually it will be ## program address function + offset
  // this array must be free()-ed
  char** symbollist = backtrace_symbols( addrlist, addrlen );

  size_t funcnamesize = 1204;
  char   funcname[1024];

  // iterate over the returned symbol lines. skip the first, it is the
  // address of this function.
  for(XDWORD i = 0; i < addrlen; i++)
    {
      char* begin_name   = NULL;
      char* begin_offset = NULL;
      char* end_offset   = NULL;

      XSTRING   beginname;
      XSTRING   beginoffset;
      XSTRING   endoffset;
      XSTRING   symbol;

      // find parentheses and +address offset surrounding the mangled name

      for(char *p = symbollist[i]; *p; ++p)
        {
         if(*p == '(')
           {
             begin_name = p;
           }
          else
           {
             if(*p == '+')
               {
                 begin_offset = p;
               }
              else
               {
                 if ( *p == ')' && ( begin_offset || begin_name ))  end_offset = p;
               }
           }
        }

      if(begin_name && end_offset && (begin_name < end_offset))
        {
          *begin_name++   = '\0';
          *end_offset++   = '\0';

          if(begin_offset) *begin_offset++ = '\0';

          int   status = -1;
          char* ret    = NULL;
          // mangled name is now in [begin_name, begin_offset) and caller
          // offset in [begin_offset, end_offset). now apply
          // __cxa_demangle():

          //#ifndef HW_PC_OLD
          abi::__cxa_demangle(begin_name, funcname, &funcnamesize, &status);

          char* fname = begin_name;
          if(status == 0)  fname = ret;
          beginname   = fname;
          //#endif

          endoffset   = end_offset;
          symbol      = symbollist[i];

          if(begin_offset)
            {
              beginoffset = begin_offset;
              Signal_Printf(true, NULL, __L("%-40s (%-40s  + %-6s) %s"), symbol.Get(), beginname.Get(), beginoffset.Get(), endoffset.Get());
            }
           else
            {
              Signal_Printf(true, NULL, __L("%-40s (%-40s    %-6s) %s"), symbol.Get(), beginname.Get(), __L(""), endoffset.Get());
            }
        }
       else
        {
          symbol      = symbollist[i];
          // couldn't parse the line? print the whole line.
          Signal_Printf(true, NULL, __L("%-40s"), symbol.Get());
        }
    }

  free(symbollist);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool Signal_RunLevel(XCHAR& previous, XCHAR& actual)
* @brief      ignal_RunLevel
* @ingroup    PLATFORM_LINUX
*
* @param[in]  previous :
* @param[in]  actual :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool Signal_RunLevel(XCHAR& previous, XCHAR& actual)
{
  struct utmp* ut;
  bool         status = false;

  previous = 0;
  actual   = 0;

  setutent();

  while((ut = getutent()) != NULL)
    {
      if(ut->ut_type == RUN_LVL)
        {
          previous = (XCHAR)(ut->ut_pid / 256);
          if(!previous) previous = __C('N');

          actual   = (XCHAR)(ut->ut_pid % 256);

          status = true;
        }
    }

  endutent();

  return status;
}


#pragma endregion


#pragma endregion


