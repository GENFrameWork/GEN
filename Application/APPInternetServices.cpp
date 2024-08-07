/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       APPInternetServices.cpp
* 
* @class      APPINTERNETSERVICES
* @brief      Application Internet Connection Services class
* @ingroup    APPLICATION
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
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "APPInternetServices.h"

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
#include "APPLog.h"

#include "APPCFG.h"

#include "APPInternetServices_XEvent.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         APPINTERNETSERVICES::APPINTERNETSERVICES()
* @brief      Constructor
* @ingroup    APPLICATION
*
* --------------------------------------------------------------------------------------------------------------------*/
APPINTERNETSERVICES::APPINTERNETSERVICES()
{
  Clean();

  RegisterEvent(APPINTERNETSERVICES_XEVENT_TYPE_CHECKINTERNETCONNEXION);
  RegisterEvent(APPINTERNETSERVICES_XEVENT_TYPE_CHANGEIP);  
  RegisterEvent(APPINTERNETSERVICES_XEVENT_TYPE_ADJUSTDATETIME);
  
  GEN_XFACTORY_CREATE(xmutex_datetime_local, Create_Mutex())
  if(!xmutex_datetime_local) return;

  GEN_XFACTORY_CREATE(xmutex_datetime_UTC, Create_Mutex())
  if(!xmutex_datetime_UTC) return;

  GEN_XFACTORY_CREATE(xdatetime_local, CreateDateTime())
  if(xdatetime_local) xdatetime_local->Read();

  GEN_XFACTORY_CREATE(xdatetime_UTC, CreateDateTime())
  if(xdatetime_UTC) xdatetime_UTC->Read();

  #ifdef APP_CFG_DYNDNSMANAGER_ACTIVE
  dyndnsmanager = new DIODYNDNS_MANAGER();
  #endif  
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         APPINTERNETSERVICES::~APPINTERNETSERVICES()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    APPLICATION
*
* --------------------------------------------------------------------------------------------------------------------*/
APPINTERNETSERVICES::~APPINTERNETSERVICES()
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

  DeRegisterEvent(APPINTERNETSERVICES_XEVENT_TYPE_CHECKINTERNETCONNEXION);
  DeRegisterEvent(APPINTERNETSERVICES_XEVENT_TYPE_CHANGEIP);
  DeRegisterEvent(APPINTERNETSERVICES_XEVENT_TYPE_ADJUSTDATETIME);


  #ifdef APP_CFG_DYNDNSMANAGER_ACTIVE
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
* @fn         bool APPINTERNETSERVICES::Ini(APPCFG* cfg, XDWORD timeoutgetpublicip = 0)
* @brief      Ini
* @ingroup    APPLICATION
*
* @param[in]  cfg : 
* @param[in]  timeoutgetpublicip : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool APPINTERNETSERVICES::Ini(APPCFG* cfg, XDWORD timeoutgetpublicip)
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
      xtask->SetID(APPINTERNETSERVICES_TASKID_CHECKCONNECTIONINTERNET);
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
      xtask->SetID(APPINTERNETSERVICES_TASKID_GETIPS);
      xtask->SetIsStartImmediatelyCycles(false);
      xtask->SetIsActive(true);

      xscheduler->Task_Add(xtask);      
           
      #ifdef APP_CFG_DYNDNSMANAGER_ACTIVE
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
      xtask->SetID(APPINTERNETSERVICES_TASKID_CHECKNTPDATETIME);
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
* @fn         bool APPINTERNETSERVICES::HaveInternetConnection()
* @brief      HaveInternetConnection
* @ingroup    APPLICATION
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPINTERNETSERVICES::HaveInternetConnection()
{
  return haveinternetconnection;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPINTERNETSERVICES::GetAutomaticLocalIP()
* @brief      GetAutomaticLocalIP
* @ingroup    APPLICATION
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPINTERNETSERVICES::GetAutomaticLocalIP()
{
  return &automaticlocalIP;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* APPINTERNETSERVICES::GetAllLocalIP()
* @brief      GetAllLocalIP
* @ingroup    APPLICATION
* 
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPINTERNETSERVICES::GetAllLocalIP()
{
  return &alllocalIP;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPINTERNETSERVICES::GetPublicIP()
* @brief      GetPublicIP
* @ingroup    APPLICATION
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPINTERNETSERVICES::GetPublicIP()
{
  return &publicIP;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool APPINTERNETSERVICES::ChangeCadenceCheck(APPINTERNETSERVICES_TASKID taskID, int timecadenceseconds, bool startimmediatelycycles)
* @brief      ChangeCadenceCheck
* @ingroup    APPLICATION
* 
* @param[in]  taskID : 
* @param[in]  timecadenceseconds : 
* @param[in]  startimmediatelycycles : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPINTERNETSERVICES::ChangeCadenceCheck(APPINTERNETSERVICES_TASKID taskID, int timecadenceseconds, bool startimmediatelycycles)
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
* @fn         XDATETIME* APPINTERNETSERVICES::DateTime_GetLocal(bool active_daylightsave, bool active_meridian)
* @brief      DateTime_GetLocal
* @ingroup    APPLICATION
*
* @param[in]  active_daylightsave : 
* @param[in]  active_meridian : 
*
* @return     XDATETIME* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XDATETIME* APPINTERNETSERVICES::DateTime_GetLocal(bool active_daylightsave, bool active_meridian)
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
* @fn         XDATETIME* APPINTERNETSERVICES::DateTime_GetUTC()
* @brief      DateTime_GetUTC
* @ingroup    APPLICATION
*
* @return     XDATETIME* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XDATETIME* APPINTERNETSERVICES::DateTime_GetUTC()
{
  if(!xdatetime_UTC) return NULL;

  if(xmutex_datetime_UTC)  xmutex_datetime_UTC->Lock();

  xdatetime_UTC->Read(false);

  if(xmutex_datetime_UTC)  xmutex_datetime_UTC->UnLock();

 return xdatetime_UTC;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPINTERNETSERVICES::DateTime_GetMeridian()
* @brief      DateTime_GetMeridian
* @ingroup    APPLICATION
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPINTERNETSERVICES::DateTime_GetMeridian()
{
  int meridian = 0;
  
  if(cfg->InternetServices_GetUpdateTimeNTPMeridianDifference() == APP_CFG_INTERNETSERVICES_UPDATETIMENTPMERIDIANDIFFERENCE_AUTO)
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
* @fn         DIOCHECKTCPIPCONNECTIONS* APPINTERNETSERVICES::GetCheckTCPIPConnections()
* @brief      GetCheckTCPIPConnections
* @ingroup    APPLICATION
*
* @return     DIOCHECKTCPIPCONNECTIONS* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOCHECKTCPIPCONNECTIONS* APPINTERNETSERVICES::GetCheckTCPIPConnections()
{
  return  checkinternetconnection->GetCheckConnections();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOCHECKINTERNETCONNECTION* APPINTERNETSERVICES::GetCheckInternetConnection()
* @brief      GetCheckInternetConnection
* @ingroup    APPLICATION
*
* @return     DIOCHECKINTERNETCONNECTION* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOCHECKINTERNETCONNECTION* APPINTERNETSERVICES::GetCheckInternetConnection()
{
  return checkinternetconnection;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIODYNDNS_MANAGER* APPINTERNETSERVICES::GetDynDNSManager()
* @brief      GetDynDNSManager
* @ingroup    APPLICATION
*
* @return     DIODYNDNS_MANAGER* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIODYNDNS_MANAGER* APPINTERNETSERVICES::GetDynDNSManager()
{
  return dyndnsmanager;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSCHEDULER* APPINTERNETSERVICES::GetXScheduler()
* @brief      GetXScheduler
* @ingroup    APPLICATION
* 
* @return     XSCHEDULER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSCHEDULER* APPINTERNETSERVICES::GetXScheduler()
{
  return xscheduler;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool APPINTERNETSERVICES::IsActivedExit()
* @brief      IsActivedExit
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPINTERNETSERVICES::IsActivedExit()
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
* @fn         bool APPINTERNETSERVICES::SetActivedExit(bool actived)
* @brief      SetActivedExit
* @ingroup    DATAIO
* 
* @param[in]  actived : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPINTERNETSERVICES::SetActivedExit(bool actived)
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
* @fn         bool APPINTERNETSERVICES::End()
* @brief      End
* @ingroup    APPLICATION
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPINTERNETSERVICES::End()
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
* @fn         bool APPINTERNETSERVICES::CheckInternetConnection()
* @brief      CheckInternetConnection
* @ingroup    APPLICATION
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPINTERNETSERVICES::CheckInternetConnection()
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
* @fn         bool APPINTERNETSERVICES::CheckInternetStatus()
* @brief      CheckInternetStatus
* @ingroup    APPLICATION
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPINTERNETSERVICES::CheckInternetStatus()
{
  APPINTERNETSERVICES_XEVENT xevent(this, APPINTERNETSERVICES_XEVENT_TYPE_CHECKINTERNETCONNEXION);      

  xevent.SetInternetConnexionState(APPINTERNETSERVICES_CHECKINTERNETCONNEXION_STATE_CHECKING);  
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
              
              xevent.SetInternetConnexionState(APPINTERNETSERVICES_CHECKINTERNETCONNEXION_STATE_RESTORE);
              xevent.SetInternetConnextionCut(connectioncut);

              if(cfg->InternetServices_GetCheckInternetStatusCadence())
                {
                  // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[APP Internet Services] Get Internet Services cadence %d]"), cfg->InternetServices_GetCheckInternetStatusCadence());

                  ChangeCadenceCheck(APPINTERNETSERVICES_TASKID_CHECKCONNECTIONINTERNET, cfg->InternetServices_GetCheckInternetStatusCadence());     
                }

              if(cfg->InternetServices_GetCheckIPsChangeCadence())
                {
                  // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[APP Internet Services] Get IPs ajust to 30 seconds]"));

                  ChangeCadenceCheck(APPINTERNETSERVICES_TASKID_GETIPS, 30, false);                        
                }
            }
           else
            {
              xevent.SetInternetConnexionState(APPINTERNETSERVICES_CHECKINTERNETCONNEXION_STATE_CHECKED); 
            }
        }
       else
        {
          xevent.SetInternetConnexionState(APPINTERNETSERVICES_CHECKINTERNETCONNEXION_STATE_CUT);

          if(cfg->InternetServices_GetCheckInternetStatusCadence())
            {
              // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[APP Internet Services] Get Internet Services cadence 10]"));    

              ChangeCadenceCheck(APPINTERNETSERVICES_TASKID_CHECKCONNECTIONINTERNET, 5, false);                       
            }
        }              
    }
   else
    {
      xevent.SetInternetConnexionState(APPINTERNETSERVICES_CHECKINTERNETCONNEXION_STATE_CHECKED); 
    }

  PostEvent(&xevent);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool APPINTERNETSERVICES::UpdateIPs()
* @brief      UpdateIPs
* @ingroup    APPLICATION
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPINTERNETSERVICES::UpdateIPs(XSTRING& actualpublicIP)
{
  XSTRING                       actualautomaticlocalIP;
  DIOSTREAMIPLOCALENUMDEVICES*  enumdevices     = (DIOSTREAMIPLOCALENUMDEVICES*)GEN_DIOFACTORY.CreateStreamEnumDevices(DIOSTREAMENUMTYPE_IP_LOCAL);  
  bool                          sendchangeevent = false;
  bool                          status          = false;
      
  APPINTERNETSERVICES_XEVENT    xevent(this, APPINTERNETSERVICES_XEVENT_TYPE_CHANGEIP);  

  // APP_LOG_ENTRY(XLOGLEVEL_INFO, APP_CFG_LOG_SECTIONID_CONNEXIONS, false, __L("[Update IPs] Ini Get Local IP... "));       
                                                                
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

  // APP_LOG_ENTRY(status?XLOGLEVEL_INFO:XLOGLEVEL_ERROR, APP_CFG_LOG_SECTIONID_CONNEXIONS, false, __L("[Update IPs] End Get Local IP : [%s] "), actualautomaticlocalIP.Get());       

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
      
      // APP_LOG_ENTRY(XLOGLEVEL_INFO, APP_CFG_LOG_SECTIONID_CONNEXIONS, false, __L("[Update IPs] Ini Get Public IP... "));                                                                    
      
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
          APP_LOG_ENTRY(XLOGLEVEL_ERROR, APP_CFG_LOG_SECTIONID_CONNEXIONS, false, __L("[Update IPs] End Get Public IP: Error not IP!"));                                                                                                                      
        }
       else
        {   
          APP_LOG_ENTRY((status?XLOGLEVEL_INFO:XLOGLEVEL_ERROR), APP_CFG_LOG_SECTIONID_CONNEXIONS, false, __L("[Update IPs] End Get Public IP: [%s] (%s)"), actualpublicIP.Get(), sendchangeevent?__L("has changed"):__L("has not changed"));                                                                                                                    
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


 #ifdef APP_CFG_DYNDNSMANAGER_ACTIVE
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool APPINTERNETSERVICES::UpdateDynDNSURLs()
* @brief      UpdateDynDNSURLs
* @ingroup    APPLICATION
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPINTERNETSERVICES::UpdateDynDNSURLs(XSTRING& actualpublicIP)
{
  if(!dyndnsmanager) 
    {
      return false;
    }
                
  // APP_LOG_ENTRY(XLOGLEVEL_INFO, APP_CFG_LOG_SECTIONID_CONNEXIONS, false, __L("[Update IPs] Ini Update Dyndns URLs: [%s] "), actualpublicIP.Get());  

  bool status = dyndnsmanager->AssingAll();

  // APP_LOG_ENTRY((status?XLOGLEVEL_INFO:XLOGLEVEL_ERROR), APP_CFG_LOG_SECTIONID_CONNEXIONS, false, __L("[Update IPs] End Update Dyndns URLs: [%s]"), actualpublicIP.Get());    

  return status;
}
#endif               


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPINTERNETSERVICES::AdjustTimerByNTP(XVECTOR<XSTRING*>* servers)
* @brief      AdjustTimerByNTP
* @ingroup    APPLICATION
*
* @param[in]  servers :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPINTERNETSERVICES::AdjustTimerByNTP(XVECTOR<XSTRING*>* servers)
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
                  if(cfg->InternetServices_GetUpdateTimeNTPMeridianDifference() == APP_CFG_INTERNETSERVICES_UPDATETIMENTPMERIDIANDIFFERENCE_AUTO)
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
      APPINTERNETSERVICES_XEVENT xevent(this, APPINTERNETSERVICES_XEVENT_TYPE_ADJUSTDATETIME);            
      PostEvent(&xevent);
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void APPINTERNETSERVICES::HandleEvent_Scheduler(XSCHEDULER_XEVENT* event)
* @brief      Handle Event for the observer manager of this class
* @note       INTERNAL
* @ingroup    APPLICATION
*
* @param[in]  event : 
*
* --------------------------------------------------------------------------------------------------------------------*/
void APPINTERNETSERVICES::HandleEvent_Scheduler(XSCHEDULER_XEVENT* event)
{
  if(checkinternetconnection)
    {
      checkinternetconnection->GetCheckConnections()->Connections_GetMutex()->Lock();
    }

  switch(event->GetTask()->GetID())
    {
      case APPINTERNETSERVICES_TASKID_CHECKCONNECTIONINTERNET : CheckInternetStatus();                                                                  
                                                                break;

      case APPINTERNETSERVICES_TASKID_GETIPS                  : // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[APP Internet Services] Check Get IPs..."));    

                                                                if(CheckInternetConnection()) 
                                                                  { 
                                                                    XSTRING actualpublicIP;                                                                                                                             

                                                                    if(UpdateIPs(actualpublicIP))
                                                                      {  
                                                                        // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[APP Internet Services] Update public IP"));    
  
                                                                        #ifdef XTRACE_ACTIVE                                                                   
                                                                        XTRACE_RESOLVEALLRESOURCES; 
                                                                        #endif  

                                                                        #ifdef APP_CFG_DYNDNSMANAGER_ACTIVE
                                                                        UpdateDynDNSURLs(actualpublicIP);                                                                        
                                                                        #endif
                                                                      }   

                                                                    if(!actualpublicIP.IsEmpty())
                                                                      {
                                                                        #ifdef APP_CFG_DYNDNSMANAGER_ACTIVE                                                                  
                                                                        // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[APP Internet Services] Get IPs ajust to %d seconds]"), cfg->InternetServices_GetCheckIPsChangeCadence());
                                                                        ChangeCadenceCheck(APPINTERNETSERVICES_TASKID_GETIPS, cfg->InternetServices_GetCheckIPsChangeCadence());   
                                                                        #endif                                                                                                                                           
                                                                      }
                                                                  }
                                                                break;

      case APPINTERNETSERVICES_TASKID_CHECKNTPDATETIME        : if(CheckInternetConnection()) 
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
* @fn         void APPINTERNETSERVICES::HandleEvent(XEVENT* xevent)
* @brief      Handle Event for the observer manager of this class
* @note       INTERNAL
* @ingroup    APPLICATION
*
* @param[in]  xevent :
*
* --------------------------------------------------------------------------------------------------------------------*/
void APPINTERNETSERVICES::HandleEvent(XEVENT* xevent)
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
* @fn         void APPINTERNETSERVICES::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    APPLICATION
*
* --------------------------------------------------------------------------------------------------------------------*/
void APPINTERNETSERVICES::Clean()
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