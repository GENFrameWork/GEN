/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       APPFlowInternetServices.cpp
* 
* @class      APPFLOWINTERNETSERVICES
* @brief      Application Flow Internet Connection Services class
* @ingroup    APPFLOW
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

#include "APPFlowInternetServices.h"

#include "XLog.h"
#include "XScheduler.h"
#include "XScheduler_XEvent.h"
#include "XSystem.h"
#include "XTranslation.h"
#include "XTranslation_GEN.h"
#include "XTranslation.h"
#include "XLanguage_ISO_639_3.h"

#include "DIOFactory.h"
#include "DIOStreamDeviceIP.h"
#include "DIOStreamIPLocalEnumDevices.h"
#include "DIOCheckTCPIPConnections.h"
#include "DIOCheckInternetConnection.h"
#include "DIOPublicInternetIP.h"
#include "DIONTP.h"
#include "DIODynDNS_Manager.h"

#include "APPFlowLog.h"
#include "APPFlowCFG.h"
#include "APPFlowInternetServices_XEvent.h"

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


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         APPFLOWINTERNETSERVICES::APPFLOWINTERNETSERVICES()
* @brief      Constructor of class
* @ingroup    APPFLOW
*
* --------------------------------------------------------------------------------------------------------------------*/
APPFLOWINTERNETSERVICES::APPFLOWINTERNETSERVICES()
{
  Clean();

  RegisterEvent(APPFLOWINTERNETSERVICES_XEVENT_TYPE_CHECKINTERNETCONNEXION);
  RegisterEvent(APPFLOWINTERNETSERVICES_XEVENT_TYPE_CHANGEIP);  
  RegisterEvent(APPFLOWINTERNETSERVICES_XEVENT_TYPE_ADJUSTDATETIME);
  
  GEN_XFACTORY_CREATE(xmutex_datetime_local, Create_Mutex())
  if(!xmutex_datetime_local) return;

  GEN_XFACTORY_CREATE(xmutex_datetime_UTC, Create_Mutex())
  if(!xmutex_datetime_UTC) return;

  GEN_XFACTORY_CREATE(xdatetime_local, CreateDateTime())
  if(xdatetime_local) xdatetime_local->Read();

  GEN_XFACTORY_CREATE(xdatetime_UTC, CreateDateTime())
  if(xdatetime_UTC) xdatetime_UTC->Read();

  #ifdef APPFLOW_CFG_DYNDNSMANAGER_ACTIVE
  dyndnsmanager = new DIODYNDNS_MANAGER();
  #endif  
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         APPFLOWINTERNETSERVICES::~APPFLOWINTERNETSERVICES()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    APPFLOW
*
* --------------------------------------------------------------------------------------------------------------------*/
APPFLOWINTERNETSERVICES::~APPFLOWINTERNETSERVICES()
{
  End();

  NTPservers.DeleteContents();
  NTPservers.DeleteAll();

  if(xdatetime_local)
    {
      GEN_XFACTORY.DeleteDateTime(xdatetime_local);
      xdatetime_local = NULL;
    }

  if(xdatetime_UTC)
    {
      GEN_XFACTORY.DeleteDateTime(xdatetime_UTC);
      xdatetime_UTC = NULL;
    }

  if(xmutex_datetime_local) 
    {
      GEN_XFACTORY.Delete_Mutex(xmutex_datetime_local);
      xmutex_datetime_local = NULL;
    }

  if(xmutex_datetime_UTC) 
    {
      GEN_XFACTORY.Delete_Mutex(xmutex_datetime_UTC);
      xmutex_datetime_UTC = NULL;
    }

  DeRegisterEvent(APPFLOWINTERNETSERVICES_XEVENT_TYPE_CHECKINTERNETCONNEXION);
  DeRegisterEvent(APPFLOWINTERNETSERVICES_XEVENT_TYPE_CHANGEIP);
  DeRegisterEvent(APPFLOWINTERNETSERVICES_XEVENT_TYPE_ADJUSTDATETIME);


  #ifdef APPFLOW_CFG_DYNDNSMANAGER_ACTIVE
  if(dyndnsmanager)
    {
      delete dyndnsmanager;
      dyndnsmanager = NULL;
    }
  #endif  

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool APPFLOWINTERNETSERVICES::Ini(APPFLOWCFG* cfg, XDWORD timeoutgetpublicip = 0)
* @brief      Ini
* @ingroup    APPFLOW
*
* @param[in]  cfg : 
* @param[in]  timeoutgetpublicip : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWINTERNETSERVICES::Ini(APPFLOWCFG* cfg, XDWORD timeoutgetpublicip)
{
  bool status = false;

  this->cfg = cfg;

  xscheduler = new XSCHEDULER();
  if(!xscheduler) return false;

  XSCHEDULERTASK* xtask;
  
  //-------------------------------------------------------------------

  if(cfg->InternetServices_GetCheckInternetStatusCadence())
    {
      xtask = new XSCHEDULERTASK( xscheduler);
      if(!xtask) return false;
      
      xtask->SetNCycles(XSCHEDULER_CYCLEFOREVER, cfg->InternetServices_GetCheckInternetStatusCadence());
      xtask->SetID(APPFLOWINTERNETSERVICES_TASKID_CHECKCONNECTIONINTERNET);
      xtask->SetIsStartImmediatelyCycles(true);
      xtask->SetIsActive(true);

      xscheduler->Task_Add(xtask);

      checkinternetconnection = new DIOCHECKINTERNETCONNECTION(5);
      status =(checkinternetconnection)?true:false;
    }

  //-------------------------------------------------------------------

  if(cfg->InternetServices_GetCheckIPsChangeCadence())
    {
      xtask = new XSCHEDULERTASK(xscheduler);
      if(!xtask) return false;

      xtask->SetNCycles(XSCHEDULER_CYCLEFOREVER, 5);  // Get IP first time
      xtask->SetID(APPFLOWINTERNETSERVICES_TASKID_GETIPS);
      xtask->SetIsStartImmediatelyCycles(false);
      xtask->SetIsActive(true);

      xscheduler->Task_Add(xtask);      
           
      #ifdef APPFLOW_CFG_DYNDNSMANAGER_ACTIVE
      if(dyndnsmanager)
        {
          for(XDWORD c=0; c<cfg->DNSManager_GetURLs()->GetSize(); c++)
            {
              dyndnsmanager->AddDNS((*cfg->DNSManager_GetURL(c)));
            }
        }
        else return true;
      #endif        
    }

  //-------------------------------------------------------------------
  
  if(cfg->InternetServices_GetUpdateTimeByNTPCadence())
    {       
      for(XDWORD c=0; c<cfg->InternetServices_GetUpdateTimeNTPServers()->GetSize(); c++)
        {
          XSTRING* server = new XSTRING();
          if(server)
            {               
              server->Set(cfg->InternetServices_GetUpdateTimeNTPServer(c)->Get());
              NTPservers.Add(server);
              
              server = NULL;
            }
        }
     
      xtask = new XSCHEDULERTASK( xscheduler);
      if(!xtask) return false;

      xtask->SetNCycles(XSCHEDULER_CYCLEFOREVER, cfg->InternetServices_GetUpdateTimeByNTPCadence());
      xtask->SetID(APPFLOWINTERNETSERVICES_TASKID_CHECKNTPDATETIME);
      xtask->SetIsStartImmediatelyCycles(true);
      xtask->SetIsActive(true);

      xscheduler->Task_Add(xtask);
    }
  
  //-------------------------------------------------------------------

  SubscribeEvent(XEVENT_TYPE_SCHEDULER, xscheduler);
  if(!xscheduler->Ini()) return false;

  //-------------------------------------------------------------------

  if(timeoutgetpublicip)  
    {
      bool haveinternet = true;

      if(checkinternetconnection)
        {
          haveinternet = checkinternetconnection->Check();
        }

      if(haveinternet)
        {
          if(cfg->InternetServices_GetCheckIPsChangeCadence() > 0)
            {
              XTIMER* GEN_XFACTORY_CREATE(timeout, CreateTimer())
              if(timeout)
                {              
                  do{ if(!publicIP.IsEmpty()) break;

                      GEN_XSLEEP.MilliSeconds(50);    

                    } while(timeout->GetMeasureSeconds() <= timeoutgetpublicip);
                } 

              GEN_XFACTORY.DeleteTimer(timeout);    
            }         
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWINTERNETSERVICES::HaveInternetConnection()
* @brief      Have internet connection
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWINTERNETSERVICES::HaveInternetConnection()
{
  return haveinternetconnection;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWINTERNETSERVICES::GetAutomaticLocalIP()
* @brief      Get automatic local IP
* @ingroup    APPFLOW
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWINTERNETSERVICES::GetAutomaticLocalIP()
{
  return &automaticlocalIP;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* APPFLOWINTERNETSERVICES::GetAllLocalIP()
* @brief      Get all local IP
* @ingroup    APPFLOW
* 
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWINTERNETSERVICES::GetAllLocalIP()
{
  return &alllocalIP;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWINTERNETSERVICES::GetPublicIP()
* @brief      Get public IP
* @ingroup    APPFLOW
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWINTERNETSERVICES::GetPublicIP()
{
  return &publicIP;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool APPFLOWINTERNETSERVICES::ChangeCadenceCheck(APPFLOWINTERNETSERVICES_TASKID taskID, int timecadenceseconds, bool startimmediatelycycles)
* @brief      Change cadence check
* @ingroup    APPFLOW
* 
* @param[in]  taskID : 
* @param[in]  timecadenceseconds : 
* @param[in]  startimmediatelycycles : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWINTERNETSERVICES::ChangeCadenceCheck(APPFLOWINTERNETSERVICES_TASKID taskID, int timecadenceseconds, bool startimmediatelycycles)
{
  if(!xscheduler)
    {
      return false;
    } 

  if(!timecadenceseconds)    
    {
      return false;
    }

  XSCHEDULERTASK* task = NULL; 

  xscheduler->GetMutexScheduler()->Lock();

  task = xscheduler->Task_GetForID(taskID);        
  if(task) 
    {      
      task->SetNCycles(XSCHEDULER_CYCLEFOREVER, timecadenceseconds);
      task->SetIsStartImmediatelyCycles(startimmediatelycycles);
      task->SetIsActive(true);      

      task->ResetCondition();
    }

  xscheduler->GetMutexScheduler()->UnLock();       

  return task?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDATETIME* APPFLOWINTERNETSERVICES::DateTime_GetLocal(bool active_daylightsave, bool active_meridian)
* @brief      Date time get local
* @ingroup    APPFLOW
*
* @param[in]  active_daylightsave : 
* @param[in]  active_meridian : 
*
* @return     XDATETIME* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XDATETIME* APPFLOWINTERNETSERVICES::DateTime_GetLocal(bool active_daylightsave, bool active_meridian)
{
  if(!xdatetime_local) return NULL;

  if(xmutex_datetime_local)  xmutex_datetime_local->Lock();

  xdatetime_local->Read();

  XQWORD seconds = xdatetime_local->GetSeconsFromDate(false);
  if(seconds)
    {
      if(!active_meridian)
        {
          seconds -= (DateTime_GetMeridian() * 3600);           
        }

      if(!active_daylightsave)
        {
          if(cfg->InternetServices_GetUpdateTimeNTPUseDayLightSaving())
            {
               if(xdatetime_local->IsDayLigthSavingTime()) 
                 {
                   seconds -= 3600;
                 }
            }
        }

       xdatetime_local->SetDateToZero();
       xdatetime_local->SetDateFromSeconds(seconds, false);                  
    }

  if(xmutex_datetime_local)  xmutex_datetime_local->UnLock();

  return xdatetime_local;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDATETIME* APPFLOWINTERNETSERVICES::DateTime_GetUTC()
* @brief      Date time get UTC
* @ingroup    APPFLOW
*
* @return     XDATETIME* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XDATETIME* APPFLOWINTERNETSERVICES::DateTime_GetUTC()
{
  if(!xdatetime_UTC) return NULL;

  if(xmutex_datetime_UTC)  xmutex_datetime_UTC->Lock();

  xdatetime_UTC->Read(false);

  if(xmutex_datetime_UTC)  xmutex_datetime_UTC->UnLock();

 return xdatetime_UTC;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWINTERNETSERVICES::DateTime_GetMeridian()
* @brief      Date time get meridian
* @ingroup    APPFLOW
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWINTERNETSERVICES::DateTime_GetMeridian()
{
  int meridian = 0;
  
  if(cfg->InternetServices_GetUpdateTimeNTPMeridianDifference() == APPFLOW_CFG_INTERNETSERVICES_UPDATETIMENTPMERIDIANDIFFERENCE_AUTO)
    {
      meridian = xdatetime_local->GetMeridianDifference();  
    }
   else 
    {
      meridian = cfg->InternetServices_GetUpdateTimeNTPMeridianDifference();
    }
  
  return meridian;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOCHECKTCPIPCONNECTIONS* APPFLOWINTERNETSERVICES::GetCheckTCPIPConnections()
* @brief      Get check TCPIP connections
* @ingroup    APPFLOW
*
* @return     DIOCHECKTCPIPCONNECTIONS* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOCHECKTCPIPCONNECTIONS* APPFLOWINTERNETSERVICES::GetCheckTCPIPConnections()
{
  return  checkinternetconnection->GetCheckConnections();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOCHECKINTERNETCONNECTION* APPFLOWINTERNETSERVICES::GetCheckInternetConnection()
* @brief      Get check internet connection
* @ingroup    APPFLOW
*
* @return     DIOCHECKINTERNETCONNECTION* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOCHECKINTERNETCONNECTION* APPFLOWINTERNETSERVICES::GetCheckInternetConnection()
{
  return checkinternetconnection;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIODYNDNS_MANAGER* APPFLOWINTERNETSERVICES::GetDynDNSManager()
* @brief      Get dyn DNS manager
* @ingroup    APPFLOW
*
* @return     DIODYNDNS_MANAGER* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIODYNDNS_MANAGER* APPFLOWINTERNETSERVICES::GetDynDNSManager()
{
  return dyndnsmanager;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSCHEDULER* APPFLOWINTERNETSERVICES::GetXScheduler()
* @brief      Get X scheduler
* @ingroup    APPFLOW
* 
* @return     XSCHEDULER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSCHEDULER* APPFLOWINTERNETSERVICES::GetXScheduler()
{
  return xscheduler;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool APPFLOWINTERNETSERVICES::IsActivedExit()
* @brief      Is actived exit
* @ingroup    APPFLOW
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWINTERNETSERVICES::IsActivedExit()
{
  if(checkinternetconnection)
    {
      if(checkinternetconnection->GetCheckConnections())
        {
          return checkinternetconnection->GetCheckConnections()->IsActivedExit();
        }
    }
    
  return false;
}
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool APPFLOWINTERNETSERVICES::SetActivedExit(bool actived)
* @brief      Set actived exit
* @ingroup    APPFLOW
* 
* @param[in]  actived : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWINTERNETSERVICES::SetActivedExit(bool actived)
{  
  if(checkinternetconnection)
    {
      if(checkinternetconnection->GetCheckConnections())
        {
          return checkinternetconnection->GetCheckConnections()->SetActivedExit(actived);
        }
    }
    
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWINTERNETSERVICES::End()
* @brief      End
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWINTERNETSERVICES::End()
{
  SetActivedExit(true); 

  endservices =  true;

  if(xscheduler)
    {
      delete xscheduler;
      xscheduler = NULL;
    }

  if(checkinternetconnection)
    {
      delete checkinternetconnection;
      checkinternetconnection = NULL;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool APPFLOWINTERNETSERVICES::CheckInternetConnection()
* @brief      Check internet connection
* @ingroup    APPFLOW
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWINTERNETSERVICES::CheckInternetConnection()
{
  bool donotletinternetconnectionmatter = true;

  if(cfg)
    {
      donotletinternetconnectionmatter = cfg->InternetServices_DoNotLetInternetConnectionMatter();
    }

  if(donotletinternetconnectionmatter)
    {
      return true;
    }
   else
    {
      return haveinternetconnection;
    }
    
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWINTERNETSERVICES::CheckInternetStatus()
* @brief      Check internet status
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWINTERNETSERVICES::CheckInternetStatus()
{
  APPFLOWINTERNETSERVICES_XEVENT xevent(this, APPFLOWINTERNETSERVICES_XEVENT_TYPE_CHECKINTERNETCONNEXION);      

  xevent.SetInternetConnexionState(APPFLOWINTERNETSERVICES_CHECKINTERNETCONNEXION_STATE_CHECKING);  
  PostEvent(&xevent);
 
  haveinternetconnection = checkinternetconnection->Check();

  if(checkinternetconnection->IsChangeConnectionStatus())
    {
      if(haveinternetconnection)
        {
          DIOCHECKTCPIPCONNECTION_CUT*  connectioncut =  checkinternetconnection->GetLastConnectionsCut();
          XSTRING                       measuretime;

          if(connectioncut)
            {
              connectioncut->GetMeasureTimeString(measuretime, true);
              
              xevent.SetInternetConnexionState(APPFLOWINTERNETSERVICES_CHECKINTERNETCONNEXION_STATE_RESTORE);
              xevent.SetInternetConnextionCut(connectioncut);

              if(cfg->InternetServices_GetCheckInternetStatusCadence())
                {
                  // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[APP Internet Services] Get Internet Services cadence %d]"), cfg->InternetServices_GetCheckInternetStatusCadence());

                  ChangeCadenceCheck(APPFLOWINTERNETSERVICES_TASKID_CHECKCONNECTIONINTERNET, cfg->InternetServices_GetCheckInternetStatusCadence());     
                }

              if(cfg->InternetServices_GetCheckIPsChangeCadence())
                {
                  // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[APP Internet Services] Get IPs ajust to 30 seconds]"));

                  ChangeCadenceCheck(APPFLOWINTERNETSERVICES_TASKID_GETIPS, 30, false);                        
                }
            }
           else
            {
              xevent.SetInternetConnexionState(APPFLOWINTERNETSERVICES_CHECKINTERNETCONNEXION_STATE_CHECKED); 
            }
        }
       else
        {
          xevent.SetInternetConnexionState(APPFLOWINTERNETSERVICES_CHECKINTERNETCONNEXION_STATE_CUT);

          if(cfg->InternetServices_GetCheckInternetStatusCadence())
            {
              // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[APP Internet Services] Get Internet Services cadence 10]"));    

              ChangeCadenceCheck(APPFLOWINTERNETSERVICES_TASKID_CHECKCONNECTIONINTERNET, 5, false);                       
            }
        }              
    }
   else
    {
      xevent.SetInternetConnexionState(APPFLOWINTERNETSERVICES_CHECKINTERNETCONNEXION_STATE_CHECKED); 
    }

  PostEvent(&xevent);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool APPFLOWINTERNETSERVICES::UpdateIPs()
* @brief      Update I ps
* @ingroup    APPFLOW
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWINTERNETSERVICES::UpdateIPs(XSTRING& actualpublicIP)
{
  XSTRING                       actualautomaticlocalIP;
  DIOSTREAMIPLOCALENUMDEVICES*  enumdevices     = (DIOSTREAMIPLOCALENUMDEVICES*)GEN_DIOFACTORY.CreateStreamEnumDevices(DIOSTREAMENUMTYPE_IP_LOCAL);  
  bool                          sendchangeevent = false;
  bool                          status          = false;
      
  APPFLOWINTERNETSERVICES_XEVENT    xevent(this, APPFLOWINTERNETSERVICES_XEVENT_TYPE_CHANGEIP);  

  // APPFLOW_LOG_ENTRY(XLOGLEVEL_INFO, APPFLOW_CFG_LOG_SECTIONID_CONNEXIONS, false, __L("[Update IPs] Ini Get Local IP... "));       
                                                                
  if(enumdevices)
    {
      DIOSTREAMDEVICEIP* device = (DIOSTREAMDEVICEIP*)enumdevices->GetFirstActiveDevice();
      if(device)  device->GetIP()->GetXString(actualautomaticlocalIP);

      alllocalIP.Empty();

      for(XDWORD c=0; c<enumdevices->GetDevices()->GetSize(); c++)
        {
          device = (DIOSTREAMDEVICEIP*)enumdevices->GetDevices()->Get(c);
          if(device)
            {
              if((!device->GetIP()->IsEmpty()) && 
                  (!device->GetIP()->IsAPIPA()) &&
                  ((device->GetIPType() == DIOSTREAMIPDEVICE_TYPE_ETHERNET) ||
                   (device->GetIPType() == DIOSTREAMIPDEVICE_TYPE_WIFI)     ||
                   (device->GetIPType() == DIOSTREAMIPDEVICE_TYPE_PPP)))
                  {
                    XSTRING locaIPstring;

                    if(!alllocalIP.IsEmpty()) alllocalIP.Add(",");                                                                                      

                    device->GetIP()->GetXString(locaIPstring); 
                    alllocalIP.Add(locaIPstring.Get());

                    status = true;                                                                                      
                  }
            }  
        }    

      GEN_DIOFACTORY.DeleteStreamEnumDevices(enumdevices);
    }

  // APPFLOW_LOG_ENTRY(status?XLOGLEVEL_INFO:XLOGLEVEL_ERROR, APPFLOW_CFG_LOG_SECTIONID_CONNEXIONS, false, __L("[Update IPs] End Get Local IP : [%s] "), actualautomaticlocalIP.Get());       

  if(status)
    {
      DIOIP ip;
      
      status = false;

      // Check Local IP changed
      if(automaticlocalIP.Compare(actualautomaticlocalIP))
        {                                                                      
          xevent.SetIsChangeLocalIP(true);          
          xevent.GetChangeLocalIP()->Set(actualautomaticlocalIP);

          nchangeslocalIP++;

          automaticlocalIP  = actualautomaticlocalIP;
          sendchangeevent   = true;
        }                                                                         
      
      // APPFLOW_LOG_ENTRY(XLOGLEVEL_INFO, APPFLOW_CFG_LOG_SECTIONID_CONNEXIONS, false, __L("[Update IPs] Ini Get Public IP... "));                                                                    
      
      GEN_DIOPUBLICINTERNETIP.Get(actualpublicIP);

      if(!actualpublicIP.IsEmpty())
        {
          // Check Public IP changed
          if(publicIP.Compare(actualpublicIP))
            {
              xevent.SetIsChangePublicIP(true);          
              xevent.GetChangePublicIP()->Set(actualpublicIP);

              nchangespublicIP++;

              publicIP        = actualpublicIP;                                                                          
              sendchangeevent = true;                          
            }

          status = true;    
        }

      /*
      if(actualpublicIP.IsEmpty())
        {
          APPFLOW_LOG_ENTRY(XLOGLEVEL_ERROR, APPFLOW_CFG_LOG_SECTIONID_CONNEXIONS, false, __L("[Update IPs] End Get Public IP: Error not IP!"));                                                                                                                      
        }
       else
        {   
          APPFLOW_LOG_ENTRY((status?XLOGLEVEL_INFO:XLOGLEVEL_ERROR), APPFLOW_CFG_LOG_SECTIONID_CONNEXIONS, false, __L("[Update IPs] End Get Public IP: [%s] (%s)"), actualpublicIP.Get(), sendchangeevent?__L("has changed"):__L("has not changed"));                                                                                                                    
        }
      */
    }

  if(sendchangeevent)
    {
      nchangesIP++;

      xevent.SetNChangesIP(nchangesIP);
      xevent.SetNChangesLocalIP(nchangeslocalIP);
      xevent.SetNChangesPublicIP(nchangespublicIP);
 
      PostEvent(&xevent);                                                                      
    }

  return sendchangeevent;  
}


 #ifdef APPFLOW_CFG_DYNDNSMANAGER_ACTIVE
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool APPFLOWINTERNETSERVICES::UpdateDynDNSURLs()
* @brief      Update dyn DNSUR ls
* @ingroup    APPFLOW
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWINTERNETSERVICES::UpdateDynDNSURLs(XSTRING& actualpublicIP)
{
  if(!dyndnsmanager) 
    {
      return false;
    }
                
  // APPFLOW_LOG_ENTRY(XLOGLEVEL_INFO, APPFLOW_CFG_LOG_SECTIONID_CONNEXIONS, false, __L("[Update IPs] Ini Update Dyndns URLs: [%s] "), actualpublicIP.Get());  

  bool status = dyndnsmanager->AssingAll();

  // APPFLOW_LOG_ENTRY((status?XLOGLEVEL_INFO:XLOGLEVEL_ERROR), APPFLOW_CFG_LOG_SECTIONID_CONNEXIONS, false, __L("[Update IPs] End Update Dyndns URLs: [%s]"), actualpublicIP.Get());    

  return status;
}
#endif               


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWINTERNETSERVICES::AdjustTimerByNTP(XVECTOR<XSTRING*>* servers)
* @brief      Adjust timer by NTP
* @ingroup    APPFLOW
*
* @param[in]  servers :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWINTERNETSERVICES::AdjustTimerByNTP(XVECTOR<XSTRING*>* servers)
{
  if(!servers) return false;

  bool    status = false;

  DIONTP* ntp = new DIONTP();
  if(ntp)
    {
      DIOURL* url = new DIOURL();
      if(url)
        {

          for(XDWORD c=0; c<servers->GetSize(); c++)
            {
              (*url) = servers->Get(c)->Get();

              xdatetime_local->Read();

              if(endservices)
                {
                  break;
                }

              status = ntp->GetTime((*url), DIONTP_DEFAULTTIMEOUT, GEN_XSYSTEM.HardwareUseLittleEndian(), (*xdatetime_local));
              if(status)
                {                        
                  if(cfg->InternetServices_GetUpdateTimeNTPMeridianDifference() == APPFLOW_CFG_INTERNETSERVICES_UPDATETIMENTPMERIDIANDIFFERENCE_AUTO)
                    {                      
                      xdatetime_local->AddSeconds(xdatetime_local->GetMeridianDifference() * 60);  
                    }
                   else 
                    {
                      xdatetime_local->AddSeconds(cfg->InternetServices_GetUpdateTimeNTPMeridianDifference() * 60);  
                    }
                    
                  if(cfg->InternetServices_GetUpdateTimeNTPUseDayLightSaving())
                    {
                      if(xdatetime_local->IsDayLigthSavingTime()) 
                        {                      
                          xdatetime_local->AddSeconds(3600);
                        }
                    }
                 
                  status = xdatetime_local->Write();

                  break;
                } 
            }

          delete url;
        }

      delete ntp;
    } 

  if(status)
    {
      APPFLOWINTERNETSERVICES_XEVENT xevent(this, APPFLOWINTERNETSERVICES_XEVENT_TYPE_ADJUSTDATETIME);            
      PostEvent(&xevent);
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void APPFLOWINTERNETSERVICES::HandleEvent_Scheduler(XSCHEDULER_XEVENT* event)
* @brief      Handle event scheduler
* @note       INTERNAL
* @ingroup    APPFLOW
*
* @param[in]  event : 
*
* --------------------------------------------------------------------------------------------------------------------*/
void APPFLOWINTERNETSERVICES::HandleEvent_Scheduler(XSCHEDULER_XEVENT* event)
{
  if(checkinternetconnection)
    {
      checkinternetconnection->GetCheckConnections()->Connections_GetMutex()->Lock();
    }

  switch(event->GetTask()->GetID())
    {
      case APPFLOWINTERNETSERVICES_TASKID_CHECKCONNECTIONINTERNET : CheckInternetStatus();                                                                  
                                                                break;

      case APPFLOWINTERNETSERVICES_TASKID_GETIPS                  : // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[APP Internet Services] Check Get IPs..."));    

                                                                if(CheckInternetConnection()) 
                                                                  { 
                                                                    XSTRING actualpublicIP;                                                                                                                             

                                                                    if(UpdateIPs(actualpublicIP))
                                                                      {  
                                                                        // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[APP Internet Services] Update public IP"));    
  
                                                                        #ifdef XTRACE_ACTIVE                                                                   
                                                                        XTRACE_RESOLVEALLRESOURCES; 
                                                                        #endif  

                                                                        #ifdef APPFLOW_CFG_DYNDNSMANAGER_ACTIVE
                                                                        UpdateDynDNSURLs(actualpublicIP);                                                                        
                                                                        #endif
                                                                      }   

                                                                    if(!actualpublicIP.IsEmpty())
                                                                      {
                                                                        #ifdef APPFLOW_CFG_DYNDNSMANAGER_ACTIVE                                                                  
                                                                        // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[APP Internet Services] Get IPs ajust to %d seconds]"), cfg->InternetServices_GetCheckIPsChangeCadence());
                                                                        ChangeCadenceCheck(APPFLOWINTERNETSERVICES_TASKID_GETIPS, cfg->InternetServices_GetCheckIPsChangeCadence());   
                                                                        #endif                                                                                                                                           
                                                                      }
                                                                  }
                                                                break;

      case APPFLOWINTERNETSERVICES_TASKID_CHECKNTPDATETIME        : if(CheckInternetConnection()) 
                                                                 {
                                                                   AdjustTimerByNTP(&NTPservers);                                                                  
                                                                 }
                                                                break;
    }

  
  if(checkinternetconnection)
    {
      checkinternetconnection->GetCheckConnections()->Connections_GetMutex()->UnLock();
    }

}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void APPFLOWINTERNETSERVICES::HandleEvent(XEVENT* xevent)
* @brief      Handle event
* @note       INTERNAL
* @ingroup    APPFLOW
*
* @param[in]  xevent :
*
* --------------------------------------------------------------------------------------------------------------------*/
void APPFLOWINTERNETSERVICES::HandleEvent(XEVENT* xevent)
{
  if(!xevent) return;

  switch(xevent->GetEventFamily())
    {
      case XEVENT_TYPE_SCHEDULER       : {  XSCHEDULER_XEVENT* event = (XSCHEDULER_XEVENT*)xevent;
                                            if(!event) return;

                                            HandleEvent_Scheduler(event);
                                         }
                                         break;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void APPFLOWINTERNETSERVICES::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    APPFLOW
*
* --------------------------------------------------------------------------------------------------------------------*/
void APPFLOWINTERNETSERVICES::Clean()
{
  xscheduler              = NULL;

  automaticlocalIP.Empty();
  alllocalIP.Empty();

  nchangesIP              = 0;
  nchangeslocalIP         = 0;
  nchangespublicIP        = 0;

  checkinternetconnection = NULL;

  haveinternetconnection  = false;

  endservices             = false;

//scraperwebpublicIP      = NULL;

  xdatetime_local         = NULL;
  xdatetime_UTC           = NULL;

  dyndnsmanager           = NULL;
}


#pragma endregion
