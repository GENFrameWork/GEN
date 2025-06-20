/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       APPFLOWCFG.cpp
* 
* @class      APPFLOWCFG
* @brief      Application Flow Config class
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

#include "APPFlowCFG.h"

#include "Common_DefaultXTraceCFG.h"

#ifdef XFEEDBACK_CONTROL_ACTIVE
#include "XFeedback_Control.h"
#endif
#include "XTrace.h"
#include "XLog.h"

#ifdef DIO_WEBSERVER_ACTIVE
#include "DIOWebServer.h"
#endif
#ifdef DIO_SSHREVERSE_ACTIVE
#include "DIOSSHReverse.h"
#endif
#include "DIODNSResolver.h"
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
* @fn         APPFLOWCFG::APPFLOWCFG(XCHAR* namefile)
* @brief      Constructor of class
* @ingroup    APPFLOW
* 
* @param[in]  namefile : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
APPFLOWCFG::APPFLOWCFG(XCHAR* namefile)
#ifdef APPFLOW_CFG_REMOTEFILE_ACTIVE
 : DIOREMOTEFILECFG(namefile)
#else
 : XFILECFG(namefile)
#endif
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         APPFLOWCFG::~APPFLOWCFG()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    APPFLOW
* 
* --------------------------------------------------------------------------------------------------------------------*/
APPFLOWCFG::~APPFLOWCFG()
{
  End();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool APPFLOWCFG::DoVariableMapping()
* @brief      Do variable mapping
* @ingroup    APPFLOW
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::DoVariableMapping()
{
  if(!XFILECFG::DoVariableMapping())
    {
      return false;
    }

  //---------------------------------------------------------------------------------------------------------------------------------------------------------------

  #ifdef APPFLOW_CFG_GENERAL_ACTIVE  
  AddRemark(APPFLOW_CFG_SECTION_GENERAL, __L("--------------------------------------------------------------------------------------------------------------------------------------------"), 0, 1);
  AddRemark(APPFLOW_CFG_SECTION_GENERAL, __L(" General section of configuration"), 0, 2);

  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_GENERAL                   , APPFLOW_CFG_SCRAPERWEBSCRIPTURLDOWNLOAD                         , &scraperwebscripturldownload                                        , __L("Scrapper WEB Script URL download")                                   , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_GENERAL                   , APPFLOW_CFG_SHOWDETAILINFO                                      , &showdetailinfo                                                     , __L("Show Detail info")                                                   , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);

  #ifdef XTRACE_ACTIVE
  XFILECFGVALUE* CFGvalue = AddValueSecuence<XSTRING>(XFILECFG_VALUETYPE_STRING, APPFLOW_CFG_SECTION_GENERAL, APPFLOW_CFG_TRACETARGET, __L("%02d"), XTRACE_MINNTARGETS
                                                                                                            , XTRACE_MAXNTARGETS
                                                                                                            , tracetargets
                                                                                                            , ntracetargets                                                                                               , __L("eXtended Trace Aim ")                                                , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  if(CFGvalue) 
    { 
      CFGvalue->SetModeRemoteMix((XFILECFG_MODEREMOTEMIX)(CFGvalue->GetModeRemoteMix() | XFILECFG_MODEREMOTEMIX_NOTDELADDKEYS));
    }
  #endif  
  #endif

  //---------------------------------------------------------------------------------------------------------------------------------------------------------------

  #ifdef APPFLOW_CFG_CHECKRESOURCESHARDWARE_ACTIVE
  AddRemark(APPFLOW_CFG_SECTION_CHECKRESOURCESHARDWARE, __L("--------------------------------------------------------------------------------------------------------------------------------------------"), 0, 1);
  AddRemark(APPFLOW_CFG_SECTION_CHECKRESOURCESHARDWARE, __L(" Check resources section of configuration"), 0, 2);

  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_CHECKRESOURCESHARDWARE    , APPFLOW_CFG_CHECKRESOURCESHARDWARE_MEMSTATUSCHECKCADENCE              , &checkresourceshardware_memstatuscheckcadence                 , __L("System Memory status check cadence")                                 , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_CHECKRESOURCESHARDWARE    , APPFLOW_CFG_CHECKRESOURCESHARDWARE_MEMSTATUSLIMITPERCENT              , &checkresourceshardware_memstatuslimitpercent                 , __L("System Memory free Limit percent")                                   , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_CHECKRESOURCESHARDWARE    , APPFLOW_CFG_CHECKRESOURCESHARDWARE_TOTALCPUUSAGECHECKCADENCE          , &checkresourceshardware_totalcpuusagecheckcadence             , __L("System Total CPU usage cadence")                                     , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_CHECKRESOURCESHARDWARE    , APPFLOW_CFG_CHECKRESOURCESHARDWARE_TOTALCPUUSAGELIMITPERCENT          , &checkresourceshardware_totalcpuusagelimitpercent             , __L("System Total CPU limit percent")                                     , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);  
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_CHECKRESOURCESHARDWARE    , APPFLOW_CFG_CHECKRESOURCESHARDWARE_APPCPUUSAGEPROCESSNAME             , &checkresourceshardware_appcpuusageprocessname                , __L("System App CPU usage process name")                                  , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_CHECKRESOURCESHARDWARE    , APPFLOW_CFG_CHECKRESOURCESHARDWARE_APPCPUUSAGECHECKCADENCE            , &checkresourceshardware_appcpuusagecheckcadence               , __L("System App CPU usage cadence")                                       , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_CHECKRESOURCESHARDWARE    , APPFLOW_CFG_CHECKRESOURCESHARDWARE_APPCPUUSAGELIMITPERCENT            , &checkresourceshardware_appcpuusagelimitpercent               , __L("System App CPU limit percent")                                       , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);    
  #endif

  //---------------------------------------------------------------------------------------------------------------------------------------------------------------

  #ifdef APPFLOW_CFG_INTERNETSERVICES_ACTIVE
  AddRemark(APPFLOW_CFG_SECTION_INTERNETSERVICES, __L("--------------------------------------------------------------------------------------------------------------------------------------------"), 0, 1);
  AddRemark(APPFLOW_CFG_SECTION_INTERNETSERVICES, __L(" Internet services section of configuration"), 0, 2);

  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_INTERNETSERVICES          , APPFLOW_CFG_INTERNETSERVICES_CHECKINTERNETSTATUSCADENCE               , &internetservices_checkinternetstatuscadence                  , __L("Internet connection status cadence")                                 , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_BOOLEAN , APPFLOW_CFG_SECTION_INTERNETSERVICES          , APPFLOW_CFG_INTERNETSERVICES_DONOTLETINTERNETCONNECTIONMATTER         , &internetservices_donotletinternetconnectionmatter            , __L("Do not let Internet connection matter")                              , APPFLOW_CFG_DEFAULT_REMARK_COLUMN); 
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_INTERNETSERVICES          , APPFLOW_CFG_INTERNETSERVICES_CHECKIPSCHANGECADENCE                    , &internetservices_checkipschangecadence                       , __L("Internet IP Change Cadence")                                         , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_INTERNETSERVICES          , APPFLOW_CFG_INTERNETSERVICES_UPDATETIMEBYNTPCADENCE                   , &internetservices_updatetimebyntpcadence                      , __L("Internet update time by NTP cadence")                                , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);

  AddValueSecuence<XSTRING>(XFILECFG_VALUETYPE_STRING, APPFLOW_CFG_SECTION_INTERNETSERVICES,  APPFLOW_CFG_INTERNETSERVICES_UPDATETIMENTPSERVER, __L("%02d") , 3, XFILECFG_DEFAULTMAXSECUENCEENTRYS
                                                                                                                                                            , internetservices_updatetimentpservers
                                                                                                                                                            , internetservices_nupdatetimentpservers                          , __L("Internet update NTP Server")                                         , APPFLOW_CFG_DEFAULT_REMARK_COLUMN); 

  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_INTERNETSERVICES          , APPFLOW_CFG_INTERNETSERVICES_UPDATETIMENTPMERIDIANDIFFERENCE          , &internetservices_updatetimentpmeridiandifference             , __L("Internet update meridian difference")                                , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_BOOLEAN , APPFLOW_CFG_SECTION_INTERNETSERVICES          , APPFLOW_CFG_INTERNETSERVICES_UPDATETIMENTPUSEDAYLIGHTSAVING           , &internetservices_updatetimentpusedaylightsaving              , __L("Internet update time day light saving")                              , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);

  #ifdef APPFLOW_CFG_DNSRESOLVER_ACTIVE
  AddRemark(APPFLOW_CFG_SECTION_DNSRESOLVER, __L("--------------------------------------------------------------------------------------------------------------------------------------------"), 0, 1);
  AddRemark(APPFLOW_CFG_SECTION_DNSRESOLVER, __L(" DNS resolved section of configuration"), 0, 2); 

  AddValueSecuence<XSTRING>(XFILECFG_VALUETYPE_STRING, APPFLOW_CFG_SECTION_DNSRESOLVER, APPFLOW_CFG_DNSRESOLVER_HOSTRESOLVED, __L("%02d") , 3, XFILECFG_DEFAULTMAXSECUENCEENTRYS
                                                                                                                                          , hostsresolved
                                                                                                                                          , nhostsresolved                                                                    , __L("Host resolved for DNS ")                                     , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValueSecuence<XSTRING>(XFILECFG_VALUETYPE_STRING, APPFLOW_CFG_SECTION_DNSRESOLVER, APPFLOW_CFG_DNSRESOLVER_DNSSERVER   , __L("%02d") , 3, XFILECFG_DEFAULTMAXSECUENCEENTRYS
                                                                                                                                          , DNSservers, nDNSservers                                                           , __L("Server for DNS ")                                            , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  #endif

  #ifdef APPFLOW_CFG_DYNDNSMANAGER_ACTIVE
  AddRemark(APPFLOW_CFG_DYNDNSMANAGER_URL, __L("--------------------------------------------------------------------------------------------------------------------------------------------"), 0, 1);
  AddRemark(APPFLOW_CFG_DYNDNSMANAGER_URL, __L(" DynDNS Manager section of iguration"), 0, 2);

  AddValueSecuence<XSTRING>(XFILECFG_VALUETYPE_STRING, APPFLOW_CFG_SECTION_DYNDNSMANAGER,  APPFLOW_CFG_DYNDNSMANAGER_URL, __L("%02d")     , 3, XFILECFG_DEFAULTMAXSECUENCEENTRYS
                                                                                                                                          , dnsmanager_urls
                                                                                                                                          , dnsmanager_nurls                                                                  , __L("DynDNS Manager URL to assign")                                       , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  #endif
  #endif

  //---------------------------------------------------------------------------------------------------------------------------------------------------------------

  #ifdef APPFLOW_CFG_DIOLOCATION_ACTIVE
  AddRemark(APPFLOW_CFG_SECTION_LOCATION, __L("--------------------------------------------------------------------------------------------------------------------------------------------"), 0, 1);
  AddRemark(APPFLOW_CFG_SECTION_LOCATION, __L(" Location info section of configuration"), 0, 2);
  
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_LOCATION                  , APPFLOW_CFG_LOCATION_STREET                                     , &location_street                                                    , __L("Location street")                                                    , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_LOCATION                  , APPFLOW_CFG_LOCATION_TOWN                                       , &location_city                                                      , __L("Location city")                                                      , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_LOCATION                  , APPFLOW_CFG_LOCATION_STATE                                      , &location_state                                                     , __L("Location state")                                                     , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_LOCATION                  , APPFLOW_CFG_LOCATION_COUNTRY                                    , &location_country                                                   , __L("Location country")                                                   , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_LOCATION                  , APPFLOW_CFG_LOCATION_POSTALCODE                                 , &location_postalcode                                                , __L("Location postal code")                                               , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  #endif

  //---------------------------------------------------------------------------------------------------------------------------------------------------------------

  #ifdef APPFLOW_CFG_APPUPDATE_ACTIVE
  AddRemark(APPFLOW_CFG_SECTION_APPLICATIONUPDATE, __L("--------------------------------------------------------------------------------------------------------------------------------------------"), 0, 1);
  AddRemark(APPFLOW_CFG_SECTION_APPLICATIONUPDATE, __L(" Application Update section of configuration"), 0, 2);

  AddValue(XFILECFG_VALUETYPE_BOOLEAN , APPFLOW_CFG_SECTION_APPLICATIONUPDATE         , APPFLOW_CFG_APPLICATIONUPDATE_ISACTIVE                          , &applicationupdate_isactive                                         , __L("Application Update is active")                                       , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_APPLICATIONUPDATE         , APPFLOW_CFG_APPLICATIONUPDATE_URL                               , &applicationupdate_URL                                              , __L("Application Update URL")                                             , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);      
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_APPLICATIONUPDATE         , APPFLOW_CFG_APPLICATIONUPDATE_PORT                              , &applicationupdate_port                                             , __L("Application Update port")                                            , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_APPLICATIONUPDATE         , APPFLOW_CFG_APPLICATIONUPDATE_CHECKCADENCE                      , &applicationupdate_checkcadence                                     , __L("Application Update check cadence")                                   , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_APPLICATIONUPDATE         , APPFLOW_CFG_APPLICATIONUPDATE_CHECKTIME                         , &applicationupdate_checktime                                        , __L("Application Update check time")                                      , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_APPLICATIONUPDATE         , APPFLOW_CFG_APPLICATIONUPDATE_MAXRESTORATIONS                   , &applicationupdate_maxrestorations                                  , __L("Application Update maximum number of restorations")                  , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  #endif


  #ifdef APPFLOW_CFG_WEBSERVER_ACTIVE
  AddRemark(APPFLOW_CFG_SECTION_WEBSERVER, __L("--------------------------------------------------------------------------------------------------------------------------------------------"), 0, 1);
  AddRemark(APPFLOW_CFG_SECTION_WEBSERVER, __L(" Web server section of configuration"), 0, 2);

  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_WEBSERVER                 , APPFLOW_CFG_WEBSERVER_LOCALADDR                                 , &webserver_localaddr                                                , __L("Local IP for the WEB server")                                        , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_WEBSERVER                 , APPFLOW_CFG_WEBSERVER_PORT                                      , &webserver_port                                                     , __L("Port for the WEB server")                                            , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_WEBSERVER                 , APPFLOW_CFG_WEBSERVER_TIMEOUTTOSERVERPAGE                       , &webserver_timeouttoserverpage                                      , __L("Timeout for the WEB server")                                         , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_BOOLEAN , APPFLOW_CFG_SECTION_WEBSERVER                 , APPFLOW_CFG_WEBSERVER_AUTHENTICATEDACCESS                       , &webserver_isauthenticatedaccess                                    , __L("Authenticate access for the WEB server")                             , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_WEBSERVER                 , APPFLOW_CFG_WEBSERVER_PASSWORD                                  , &webserver_password                                                 , __L("Password for the WEB server")                                        , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_WEBSERVER                 , APPFLOW_CFG_WEBSERVER_PATH_RESOURCES                            , &webserver_path_resources                                           , __L("Path resources for the WEB server")                                  , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_WEBSERVER                 , APPFLOW_CFG_WEBSERVER_PATH_PHP                                  , &webserver_path_PHP                                                 , __L("Path instalation PHP for the WEB server")                            , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  #endif


  #ifdef APPFLOW_CFG_ALERTS_ACTIVE
  AddRemark(APPFLOW_CFG_SECTION_ALERTS, __L("--------------------------------------------------------------------------------------------------------------------------------------------"), 0, 1);
  AddRemark(APPFLOW_CFG_SECTION_ALERTS, __L(" Alerts section of configuration"), 0, 2);
  
  AddValue(XFILECFG_VALUETYPE_BOOLEAN , APPFLOW_CFG_SECTION_ALERTS                    , APPFLOW_CFG_ALERTS_ISACTIVE                                     , &alerts_isactive                                                    , __L("De/Activate all alerts")                                             , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValueSecuence<XSTRING>(XFILECFG_VALUETYPE_STRING, APPFLOW_CFG_SECTION_ALERTS,  APPFLOW_CFG_ALERTS_CONDITION, __L("%02d")     , 3, XFILECFG_DEFAULTMAXSECUENCEENTRYS
                                                                                                                                  , alerts_conditions
                                                                                                                                  , alerts_nconditions                                                                        , __L("Conditions for alerts")                                              , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);

  AddValue(XFILECFG_VALUETYPE_BOOLEAN , APPFLOW_CFG_SECTION_ALERTS                    , APPFLOW_CFG_ALERTS_SMTP_ISACTIVE                                , &alerts_SMTP_isactive                                               , __L("De/Activate alerts by SMTP")                                         , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_ALERTS                    , APPFLOW_CFG_ALERTS_SMTP_URL                                     , &alerts_SMTP_URL                                                    , __L("URL for the SMTP server")                                            , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_ALERTS                    , APPFLOW_CFG_ALERTS_SMTP_PORT                                    , &alerts_SMTP_port                                                   , __L("Port for the SMTP server")                                           , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_ALERTS                    , APPFLOW_CFG_ALERTS_SMTP_LOGIN                                   , &alerts_SMTP_login                                                  , __L("Login for the SMTP server")                                          , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_ALERTS                    , APPFLOW_CFG_ALERTS_SMTP_PASSWORD                                , &alerts_SMTP_password                                               , __L("Password for the SMTP server")                                       , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_ALERTS                    , APPFLOW_CFG_ALERTS_SMTP_SENDER                                  , &alerts_SMTP_sender                                                 , __L("Sender for the SMTP server")                                         , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValueSecuence<XSTRING>(XFILECFG_VALUETYPE_STRING, APPFLOW_CFG_SECTION_ALERTS, APPFLOW_CFG_ALERTS_SMTP_RECIPIENT, __L("%02d") , 3, XFILECFG_DEFAULTMAXSECUENCEENTRYS
                                                                                                                                  , alerts_SMTP_recipients
                                                                                                                                  , alerts_SMTP_nrecipients                                                                   , __L("Recipient for sending by SMTP")                                      , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  
  AddValue(XFILECFG_VALUETYPE_BOOLEAN , APPFLOW_CFG_SECTION_ALERTS                    , APPFLOW_CFG_ALERTS_SMS_ISACTIVE                                 , &alerts_SMS_isactive                                                , __L("De/Activate alerts by SMS")                                          , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValueSecuence<XSTRING>(XFILECFG_VALUETYPE_STRING, APPFLOW_CFG_SECTION_ALERTS, APPFLOW_CFG_ALERTS_SMS_RECIPIENT, __L("%02d")  , 3, XFILECFG_DEFAULTMAXSECUENCEENTRYS
                                                                                                                                  , alerts_SMS_recipients
                                                                                                                                  , alerts_SMS_nrecipients                                                                    , __L("Recipient for sending by SMS")                                       , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);

  AddValue(XFILECFG_VALUETYPE_BOOLEAN , APPFLOW_CFG_SECTION_ALERTS                    , APPFLOW_CFG_ALERTS_WEB_ISACTIVE                                 , &alerts_WEB_isactive                                                , __L("De/Activate alerts by WEB")                                          , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_BOOLEAN , APPFLOW_CFG_SECTION_ALERTS                    , APPFLOW_CFG_ALERTS_WEB_UISUSEGET                                , &alerts_WEB_isuseget                                                , __L("Use Get for alerts by WEB")                                          , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValueSecuence<XSTRING>(XFILECFG_VALUETYPE_STRING, APPFLOW_CFG_SECTION_ALERTS, APPFLOW_CFG_ALERTS_WEB_RECIPIENT, __L("%02d")  , 3, XFILECFG_DEFAULTMAXSECUENCEENTRYS
                                                                                                                                  , alerts_WEB_recipients
                                                                                                                                  , alerts_WEB_nrecipients                                                                    , __L("Recipient for sending by WEB")                                       , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);

  AddValue(XFILECFG_VALUETYPE_BOOLEAN , APPFLOW_CFG_SECTION_ALERTS                    , APPFLOW_CFG_ALERTS_UDP_ISACTIVE                                 , &alerts_UDP_isactive                                                , __L("De/Activate alerts by UDP")                                          , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_ALERTS                    , APPFLOW_CFG_ALERTS_UDP_PORT                                     , &alerts_UDP_port                                                    , __L("Port for the UDP server")                                            , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValueSecuence<XSTRING>(XFILECFG_VALUETYPE_STRING, APPFLOW_CFG_SECTION_ALERTS, APPFLOW_CFG_ALERTS_UDP_RECIPIENT, __L("%02d")  , 3, XFILECFG_DEFAULTMAXSECUENCEENTRYS
                                                                                                                                  , alerts_UDP_recipients
                                                                                                                                  , alerts_UDP_nrecipients                                                                    , __L("Recipient for sending by UDP")                                       , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  #endif


  #ifdef APPFLOW_CFG_SCRIPTS_ACTIVE

  AddRemark(APPFLOW_CFG_SECTION_LOG, __L("--------------------------------------------------------------------------------------------------------------------------------------------"), 0, 1);
  AddRemark(APPFLOW_CFG_SECTION_LOG, __L(" Scripts list"), 0, 2);

  AddValueSecuence<XSTRING>(XFILECFG_VALUETYPE_STRING, APPFLOW_CFG_SECTION_SCRIPTS    , APPFLOW_CFG_SCRIPTS_SCRIPT                , __L("%03d"), 3, 999 
                                                                                                                                  , scripts_list
                                                                                                                                  , scripts_nscripts                                                                          , __L("Scripts")                                                            , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);



  #endif


  #ifdef APPFLOW_CFG_LOG_ACTIVE

  AddRemark(APPFLOW_CFG_SECTION_LOG, __L("--------------------------------------------------------------------------------------------------------------------------------------------"), 0, 1);
  AddRemark(APPFLOW_CFG_SECTION_LOG, __L(" Log section of configuration"), 0, 2);
  
  AddValue(XFILECFG_VALUETYPE_BOOLEAN , APPFLOW_CFG_SECTION_LOG                     , APPFLOW_CFG_LOG_ISACTIVE                                          , &log_isactive                                                       , __L("De/Activate log generation")                                         , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_BOOLEAN , APPFLOW_CFG_SECTION_LOG                     , APPFLOW_CFG_LOG_BACKUPISACTIVE                                    , &log_backupisactive                                                 , __L("De/Activate backup for the log")                                     , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_LOG                     , APPFLOW_CFG_LOG_BACKUPMAXFILES                                    , &log_backupmaxfiles                                                 , __L("Maximum number of backup files (ZIP)")                               , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_BOOLEAN , APPFLOW_CFG_SECTION_LOG                     , APPFLOW_CFG_LOG_BACKUPISCOMPRESS                                  , &log_backupiscompress                                               , __L("De/Activate compression in log backup")                              , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);   
  AddValue(XFILECFG_VALUETYPE_STRING  , APPFLOW_CFG_SECTION_LOG                     , APPFLOW_CFG_LOG_ACTIVESECTIONSID                                  , &log_activesectionsID                                               , __L("Section filter by ID in the log")                                    , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_MASK    , APPFLOW_CFG_SECTION_LOG                     , APPFLOW_CFG_LOG_LEVELMASK                                         , &log_levelmask                                                      , __L("Filter by level mask")                                               , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_LOG                     , APPFLOW_CFG_LOG_MAXSIZE                                           , &log_maxsize                                                        , __L("Limit of the main file to perform the backup (in Kb)")               , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  AddValue(XFILECFG_VALUETYPE_INT     , APPFLOW_CFG_SECTION_LOG                     , APPFLOW_CFG_LOG_REDUCTIONPERCENT                                  , &log_reductionpercent                                               , __L("Reduction (percentage) of the main file when performing backup")     , APPFLOW_CFG_DEFAULT_REMARK_COLUMN);
  #endif

  return true;
}
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool APPFLOWCFG::DoDefault()
* @brief      Do default
* @ingroup    APPFLOW
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::DoDefault()
{
  if(!XFILECFG::DoDefault())
    {
      return false;
    }
  

  #ifdef APPFLOW_CFG_GENERAL_ACTIVE
  showdetailinfo = __L("0000");
  #ifdef XTRACE_ACTIVE
  SetDefaultTraceTargets();
  #endif
  #endif


  #ifdef APPFLOW_CFG_CHECKRESOURCESHARDWARE_ACTIVE
  checkresourceshardware_memstatuscheckcadence      = 30;
  checkresourceshardware_memstatuslimitpercent      = 5;
  checkresourceshardware_totalcpuusagecheckcadence  = 20;
  checkresourceshardware_totalcpuusagelimitpercent  = 70;  
  checkresourceshardware_appcpuusageprocessname.Empty();
  checkresourceshardware_appcpuusagecheckcadence    = 20;
  checkresourceshardware_appcpuusagelimitpercent    = 70;  
  #endif


  #ifdef APPFLOW_CFG_INTERNETSERVICES_ACTIVE
  internetservices_checkinternetstatuscadence       = 30;
  internetservices_donotletinternetconnectionmatter = false;
  internetservices_checkipschangecadence            = 3600;
  internetservices_updatetimebyntpcadence           = 4;
  internetservices_updatetimentpservers.Get(0)->Set(__L("1.es.pool.ntp.org")); 
  internetservices_updatetimentpservers.Get(1)->Set(__L("1.europe.pool.ntp.org")); 
  internetservices_updatetimentpservers.Get(2)->Set(__L("3.europe.pool.ntp.org")); 
  internetservices_updatetimentpmeridiandifference  = APPFLOW_CFG_INTERNETSERVICES_UPDATETIMENTPMERIDIANDIFFERENCE_AUTO;
  internetservices_updatetimentpusedaylightsaving   = true;
  #endif


  #ifdef APPFLOW_CFG_WEBSERVER_ACTIVE
  webserver_timeouttoserverpage                     = 30;
  #endif


  #ifdef APPFLOW_CFG_APPUPDATE_ACTIVE
  applicationupdate_maxrestorations                 = 12;
  #endif


  #ifdef APPFLOW_CFG_LOG_ACTIVE
  log_isactive                                      = true;
  log_backupisactive                                = true;
  log_backupmaxfiles                                = 10;
  log_backupiscompress                              = true;
  log_activesectionsID.Empty();
  log_activesectionsID                             += APPFLOW_CFG_LOG_SECTIONID_INITIATION;
  log_activesectionsID                             += __L(",");
  log_activesectionsID                             += APPFLOW_CFG_LOG_SECTIONID_GENERIC;
  log_activesectionsID                             += __L(",");
  log_activesectionsID                             += APPFLOW_CFG_LOG_SECTIONID_STATUSAPP;
  log_activesectionsID                             += __L(",");

  #ifdef XFEEDBACK_CONTROL_ACTIVE  
  log_activesectionsID                             += XFEEDBACK_CONTROL_LOG_SECTIONID; 
  log_activesectionsID                             += __L(","); 
  #endif

  #ifdef DIO_WEBSERVER_ACTIVE
  log_activesectionsID                             += DIOWEBSERVER_LOGSECTIONID;
  log_activesectionsID                             += __L(",");
  #endif
  
  #ifdef DIO_SSHREVERSE_ACTIVE
  log_activesectionsID                             += DIOSSHREVERSE_LOGSECTIONID;
  log_activesectionsID                             += __L(",");
  #endif

  log_activesectionsID                             += APPFLOW_CFG_LOG_SECTIONID_ENDING;
  log_levelmask                                     = XLOGLEVEL_ALL;
  log_maxsize                                       = 3000;
  log_reductionpercent                              = 10;
  #endif 

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool APPFLOWCFG::End()
* @brief      End
* @ingroup    APPFLOW
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::End()
{ 
  #ifdef APPFLOW_CFG_GENERAL_ACTIVE
  showdetailinfo.Empty();
  #ifdef XTRACE_ACTIVE
  tracetargets.DeleteContents();
  tracetargets.DeleteAll();
  #endif
  #endif

  #ifdef APPFLOW_CFG_INTERNETSERVICES_ACTIVE
  internetservices_updatetimentpservers.DeleteContents();
  internetservices_updatetimentpservers.DeleteAll();

  #ifdef APPFLOW_CFG_DNSRESOLVER_ACTIVE
  hostsresolved.DeleteContents();
  hostsresolved.DeleteAll();
  DNSservers.DeleteContents();
  DNSservers.DeleteAll();
  #endif

  #ifdef APPFLOW_CFG_DYNDNSMANAGER_ACTIVE
  dnsmanager_urls.DeleteContents();  
  dnsmanager_urls.DeleteAll();
  #endif
  #endif

  #ifdef APPFLOW_CFG_ALERTS_ACTIVE
  alerts_conditions.DeleteContents();
  alerts_conditions.DeleteAll();
  alerts_SMTP_recipients.DeleteContents();
  alerts_SMTP_recipients.DeleteAll();
  alerts_SMS_recipients.DeleteContents();
  alerts_SMS_recipients.DeleteAll();
  alerts_WEB_recipients.DeleteContents();
  alerts_WEB_recipients.DeleteAll();
  alerts_UDP_recipients.DeleteContents();
  alerts_UDP_recipients.DeleteAll();
  #endif  

  #ifdef APPFLOW_CFG_SCRIPTS_ACTIVE
  scripts_list.DeleteContents();
  scripts_list.DeleteAll();
  #endif

  #ifdef APPFLOW_CFG_REMOTEFILE_ACTIVE
  return DIOREMOTEFILECFG::End();
  #else
  return XFILECFG::End();
  #endif
}



#ifdef APPFLOW_CFG_GENERAL_ACTIVE
/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::GetScraperWebScriptURLDownload()
* @brief      Get scraper web script URL download
* @ingroup    APPFLOW
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::GetScraperWebScriptURLDownload()
{
  return &scraperwebscripturldownload;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD APPFLOWCFG::GetShowDetailInfo()
* @brief      Get show detail info
* @ingroup    APPFLOW
*
* @return     XWORD : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XWORD APPFLOWCFG::GetShowDetailInfo()
{
  XDWORD detail = 0x0000;

  showdetailinfo.ToUpperCase();
  showdetailinfo.UnFormat(__L("%04X"), &detail);

  return detail;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void APPFLOWCFG::SetShowDetailInfo(XWORD detail)
* @brief      Set show detail info
* @ingroup    APPFLOW
*
* @param[in]  detail : 
*
* --------------------------------------------------------------------------------------------------------------------*/
void APPFLOWCFG::SetShowDetailInfo(XWORD detail)
{
  showdetailinfo.Format(__L("%04X"), detail);
}


#ifdef XTRACE_ACTIVE
/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::GetTraceTarget(int index)
* @brief      Get trace target
* @ingroup    APPFLOW
*
* @param[in]  index : 
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::GetTraceTarget(int index)
{
  if(index <  0)                   return NULL;
  if(index >= XTRACE_MAXNTARGETS)  return NULL;

  return tracetargets.Get(index);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::SetTraceTarget(int index, XSTRING& target)
* @brief      Set trace target
* @ingroup    APPFLOW
*
* @param[in]  index :
* @param[in]  target :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::SetTraceTarget(int index, XSTRING& target)
{
  if(index <  0)                   return false;
  if(index >= XTRACE_MAXNTARGETS)  return false;

  tracetargets.Get(index)->Set(target.Get());

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::SetAutomaticTraceTargets()
* @brief      Set automatic trace targets
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::SetAutomaticTraceTargets()
{
  bool status = false;

  for(int c=0;c<XTRACE_MAXNTARGETS; c++)
    {
      XSTRING* debugcfg = GetTraceTarget(c);
      if(debugcfg)
        {
          if(!debugcfg->IsEmpty())
            {
              int      type;
              XSTRING  aim;

              aim.AdjustSize(_MAXSTR);
              debugcfg->UnFormat(__L("%d,%s"), &type, aim.Get());
              aim.AdjustSize();

              XTRACE_SETTARGET(c, (XTRACE_TYPE)type, aim.Get());

              status = true;
            }
        }
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::SetDefaultTraceTargets()
* @brief      Set default trace targets
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::SetDefaultTraceTargets()
{
  GEN_XTRACE_NET_CFG_DEFAULT_01

  return true;
}

#endif

#endif


#ifdef APPFLOW_CFG_CHECKRESOURCESHARDWARE_ACTIVE

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::CheckResourcesHardware_GetMemStatusCheckCadence()
* @brief      Check resources hardware get mem status check cadence
* @ingroup    APPFLOW
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::CheckResourcesHardware_GetMemStatusCheckCadence()
{
  return checkresourceshardware_memstatuscheckcadence;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::CheckResourcesHardware_GetMemStatusLimitPercent()
* @brief      Check resources hardware get mem status limit percent
* @ingroup    APPFLOW
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::CheckResourcesHardware_GetMemStatusLimitPercent()
{
  return checkresourceshardware_memstatuslimitpercent;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::CheckResourcesHardware_GetTotalCPUUsageCheckCadence()
* @brief      Check resources hardware get total CPU usage check cadence
* @ingroup    APPFLOW
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::CheckResourcesHardware_GetTotalCPUUsageCheckCadence()
{
  return checkresourceshardware_totalcpuusagecheckcadence;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::CheckResourcesHardware_GetTotalCPUUsageLimitPercent()
* @brief      Check resources hardware get total CPU usage limit percent
* @ingroup    APPFLOW
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::CheckResourcesHardware_GetTotalCPUUsageLimitPercent()
{
  return checkresourceshardware_totalcpuusagelimitpercent;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::CheckResourcesHardware_GetAppCPUUsageProcessName()
* @brief      Check resources hardware get app CPU usage process name
* @ingroup    APPFLOW
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::CheckResourcesHardware_GetAppCPUUsageProcessName()
{
  return &checkresourceshardware_appcpuusageprocessname;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::CheckResourcesHardware_GetAppCPUUsageCheckCadence()
* @brief      Check resources hardware get app CPU usage check cadence
* @ingroup    APPFLOW
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::CheckResourcesHardware_GetAppCPUUsageCheckCadence()
{
  return checkresourceshardware_appcpuusagecheckcadence;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::CheckResourcesHardware_GetAppCPUUsageLimitPercent()
* @brief      Check resources hardware get app CPU usage limit percent
* @ingroup    APPFLOW
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::CheckResourcesHardware_GetAppCPUUsageLimitPercent()
{
  return checkresourceshardware_appcpuusagelimitpercent;
}


#endif


#ifdef APPFLOW_CFG_INTERNETSERVICES_ACTIVE

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::InternetServices_GetCheckInternetStatusCadence()
* @brief      Internet services get check internet status cadence
* @ingroup    APPFLOW
*
* @return     int : check cadence in seconds
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::InternetServices_GetCheckInternetStatusCadence()
{
  return internetservices_checkinternetstatuscadence;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool APPFLOWCFG::InternetServices_DoNotLetInternetConnectionMatter()
* @brief      Internet services do not let internet connection matter
* @ingroup    APPFLOW
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::InternetServices_DoNotLetInternetConnectionMatter()
{
  return internetservices_donotletinternetconnectionmatter;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int APPFLOWCFG::InternetServices_GetCheckIPsChangeCadence()
* @brief      Internet services get check I ps change cadence
* @ingroup    APPFLOW
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::InternetServices_GetCheckIPsChangeCadence()
{
  return internetservices_checkipschangecadence;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::InternetServices_GetUpdateTimeByNTPCadence()
* @brief      Internet services get update time by NTP cadence
* @ingroup    APPFLOW
*
* @return     int : check cadence in seconds
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::InternetServices_GetUpdateTimeByNTPCadence()
{
  return (internetservices_updatetimebyntpcadence*3600);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XSTRING*>* APPFLOWCFG::InternetServices_GetUpdateTimeNTPServers()
* @brief      Internet services get update time NTP servers
* @ingroup    APPFLOW
* 
* @return     XVECTOR<XSTRING*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XSTRING*>* APPFLOWCFG::InternetServices_GetUpdateTimeNTPServers()
{
  return &internetservices_updatetimentpservers;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::InternetServices_GetUpdateTimeNTPServer(int index)
* @brief      Internet services get update time NTP server
* @ingroup    APPFLOW
*
* @param[in]  index :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::InternetServices_GetUpdateTimeNTPServer(int index)
{  
  if(index < 0)                                                      return NULL;
  if(index >= (int)internetservices_updatetimentpservers.GetSize())  return NULL;

  return internetservices_updatetimentpservers.Get(index);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::InternetServices_GetUpdateTimeNTPMeridianDifference()
* @brief      Internet services get update time NTP meridian difference
* @ingroup    APPFLOW
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::InternetServices_GetUpdateTimeNTPMeridianDifference()
{
  return internetservices_updatetimentpmeridiandifference;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::InternetServices_GetUpdateTimeNTPUseDayLightSaving()
* @brief      Internet services get update time NTP use day light saving
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::InternetServices_GetUpdateTimeNTPUseDayLightSaving()
{
  return internetservices_updatetimentpusedaylightsaving;
}


#ifdef APPFLOW_CFG_DNSRESOLVER_ACTIVE

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::DNSResolver_GetHost(int index, XSTRING& host, XSTRING& IPresolved)
* @brief      DNS resolver get host
* @ingroup    APPFLOW
*
* @param[in]  index :
* @param[in]  host :
* @param[in]  IPresolved :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::DNSResolver_GetHost(int index, XSTRING& host, XSTRING& IPresolved)
{
  if(index < 0)                       return false;
  if(index >= hostsresolved.GetSize()) return false;

  host.AdjustSize(_MAXSTR);
  IPresolved.AdjustSize(_MAXSTR);

  hostsresolved.Get(index)->UnFormat(__L("%s,%s"), host.Get(), IPresolved.Get());

  host.AdjustSize();
  IPresolved.AdjustSize();

  return (host.GetSize() && IPresolved.GetSize())?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::DNSResolver_GetDNSserver(int index)
* @brief      DNS resolver get DN sserver
* @ingroup    APPFLOW
*
* @param[in]  index :
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::DNSResolver_GetDNSserver(int index)
{
  if(index < 0)                       return NULL;
  if(index >= DNSservers.GetSize())   return NULL;

  if(!DNSservers.GetSize()) return NULL;

  return DNSservers.Get(index);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::SetAutomaticDNSResolver()
* @brief      Set automatic DNS resolver
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::SetAutomaticDNSResolver()
{
  for(XDWORD c=0; c<nhostsresolved; c++)
    {
      XSTRING  host;
      XSTRING  IPresolved;

      if(DNSResolver_GetHost(c, host, IPresolved))
        {
          GEN_DIODNSRESOLVER.Host_Add(host, IPresolved);
        }
    }

  for(XDWORD c=0; c<nDNSservers; c++)
    {
      XSTRING* dnsserver = DNSResolver_GetDNSserver(c);
      if(dnsserver)
        {
          GEN_DIODNSRESOLVER.DNSServer_AddDNSServer(dnsserver->Get());
        }
    }

  return true;
}

#endif


#ifdef APPFLOW_CFG_DYNDNSMANAGER_ACTIVE


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XSTRING*>* APPFLOWCFG::DNSManager_GetURLs()
* @brief      DNS manager get UR ls
* @ingroup    APPFLOW
* 
* @return     XVECTOR<XSTRING*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XSTRING*>* APPFLOWCFG::DNSManager_GetURLs()
{
  return &dnsmanager_urls;

}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::DNSManager_GetURL(int index)
* @brief
* @ingroup    APPFLOW
*
* @param[in]  index :
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::DNSManager_GetURL(int index)
{
  if(index < 0)                          return NULL;
  if(index >= dnsmanager_urls.GetSize()) return NULL;

  return dnsmanager_urls.Get(index);
}

#endif

#endif


#ifdef APPFLOW_CFG_DIOLOCATION_ACTIVE

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::Location_GetStreet()
* @brief      Location get street
* @ingroup    APPFLOW
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::Location_GetStreet()
{
  return &location_street;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::Location_GetCity()
* @brief      Location get city
* @ingroup    APPFLOW
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::Location_GetCity()
{
  return &location_city;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::Location_GetState()
* @brief      Location get state
* @ingroup    APPFLOW
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::Location_GetState()
{
  return &location_state;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::Location_GetCountry()
* @brief      Location get country
* @ingroup    APPFLOW
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::Location_GetCountry()
{
  return &location_country;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::Location_GetPostalCode()
* @brief      Location get postal code
* @ingroup    APPFLOW
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::Location_GetPostalCode()
{
  return location_postalcode;
}

#endif


#ifdef APPFLOW_CFG_APPUPDATE_ACTIVE

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::ApplicationUpdate_IsActive()
* @brief      Application update is active
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::ApplicationUpdate_IsActive()
{
  return applicationupdate_isactive;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::ApplicationUpdate_GetPort()
* @brief      Application update get port
* @ingroup    APPFLOW
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::ApplicationUpdate_GetPort()
{
  return applicationupdate_port;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::ApplicationUpdate_GetURL()
* @brief      Application update get URL
* @ingroup    APPFLOW
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::ApplicationUpdate_GetURL()
{
  return &applicationupdate_URL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::ApplicationUpdate_GetCheckCadence()
* @brief      Application update get check cadence
* @ingroup    APPFLOW
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::ApplicationUpdate_GetCheckCadence()
{
  return applicationupdate_checkcadence;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::ApplicationUpdate_GetCheckTime()
* @brief      Application update get check time
* @ingroup    APPFLOW
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::ApplicationUpdate_GetCheckTime()
{
  return &applicationupdate_checktime;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::ApplicationUpdate_GetMaxRestorations()
* @brief      Application update get max restorations
* @ingroup    APPFLOW
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::ApplicationUpdate_GetMaxRestorations()
{
  return applicationupdate_maxrestorations;
}

#endif


#ifdef APPFLOW_CFG_ALERTS_ACTIVE

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::Alerts_IsActive()
* @brief      Alerts is active
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::Alerts_IsActive()
{
  return alerts_isactive;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XSTRING*>* APPFLOWCFG::Alerts_GetConditions()
* @brief      Alerts get conditions
* @ingroup    APPFLOW
* 
* @return     XVECTOR<XSTRING*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XSTRING*>* APPFLOWCFG::Alerts_GetConditions()
{
  return &alerts_conditions;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::Alerts_GetCondition(int index)
* @brief      Alerts get condition
* @ingroup    APPFLOW
*
* @param[in]  index : 
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::Alerts_GetCondition(int index)
{
  if(index < 0)                              return NULL;
  if(index >= alerts_conditions.GetSize())   return NULL;

  return alerts_conditions.Get(index);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::Alerts_GetCondition(int index, XDWORD& conditionID, int& timelimitforrepeat, int& everynumberoftimes)
* @brief      Alerts get condition
* @ingroup    APPFLOW
*
* @param[in]  index : 
* @param[in]  conditionID : 
* @param[in]  timelimitforrepeat : 
* @param[in]  everynumberoftimes : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::Alerts_GetCondition(int index, XDWORD& conditionID, int& timelimitforrepeat, int& everynumberoftimes)
{
  XSTRING* string = Alerts_GetCondition(index);
  if(!string)             return false;
  if(string->IsEmpty())   return false;

  conditionID          = 0; 
  timelimitforrepeat   = 0;
  everynumberoftimes   = 0;

  string->UnFormat(__L("%d,%d,%d"), &conditionID, &timelimitforrepeat, &everynumberoftimes);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::Alerts_IsActiveSMTP()
* @brief      Alerts is active SMTP
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::Alerts_IsActiveSMTP()
{
  return alerts_SMTP_isactive;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::Alerts_GetSMTPURL()
* @brief      Alerts get SMTPURL
* @ingroup    APPFLOW
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::Alerts_GetSMTPURL()
{
  return &alerts_SMTP_URL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::Alerts_GetSMTPPort()
* @brief      Alerts get SMTP port
* @ingroup    APPFLOW
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::Alerts_GetSMTPPort()
{
  return alerts_SMTP_port;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::Alerts_GetSMTPLogin()
* @brief      Alerts get SMTP login
* @ingroup    APPFLOW
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::Alerts_GetSMTPLogin()
{
  return &alerts_SMTP_login;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::Alerts_GetSMTPPassword()
* @brief      Alerts get SMTP password
* @ingroup    APPFLOW
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::Alerts_GetSMTPPassword()
{
  return &alerts_SMTP_password;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::Alerts_GetSMTPSender()
* @brief      Alerts get SMTP sender
* @ingroup    APPFLOW
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::Alerts_GetSMTPSender()
{
  return &alerts_SMTP_sender;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XSTRING*>* APPFLOWCFG::Alerts_GetSMTPRecipients()
* @brief      Alerts get SMTP recipients
* @ingroup    APPFLOW
* 
* @return     XVECTOR<XSTRING*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XSTRING*>* APPFLOWCFG::Alerts_GetSMTPRecipients()
{
  return &alerts_SMTP_recipients;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::Alerts_GetSMTPRecipient(int index)
* @brief      Alerts get SMTP recipient
* @ingroup    APPFLOW
*
* @param[in]  index : 
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::Alerts_GetSMTPRecipient(int index)
{
  if(index < 0)                                  return NULL;
  if(index >= alerts_SMTP_recipients.GetSize())  return NULL;

  return alerts_SMTP_recipients.Get(index);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::Alerts_IsActiveSMS()
* @brief      Alerts is active SMS
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::Alerts_IsActiveSMS()
{
  return alerts_SMS_isactive;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XSTRING*>* APPFLOWCFG::Alerts_GetSMSRecipients()
* @brief      Alerts get SMS recipients
* @ingroup    APPFLOW
* 
* @return     XVECTOR<XSTRING*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XSTRING*>* APPFLOWCFG::Alerts_GetSMSRecipients()
{
  return &alerts_SMS_recipients;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::Alerts_GetSMSRecipient(int index)
* @brief      Alerts get SMS recipient
* @ingroup    APPFLOW
*
* @param[in]  index : 
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::Alerts_GetSMSRecipient(int index)
{
  if(index < 0)                                 return NULL;
  if(index >= alerts_SMS_recipients.GetSize())  return NULL;

  return alerts_SMS_recipients.Get(index);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::Alerts_IsActiveWEB()
* @brief      Alerts is active WEB
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::Alerts_IsActiveWEB()
{
  return alerts_WEB_isactive;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::Alerts_GetWEBIsUseGet()
* @brief      Alerts get WEB is use get
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::Alerts_GetWEBIsUseGet()
{
  return alerts_WEB_isuseget;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XSTRING*>* APPFLOWCFG::Alerts_GetWEBRecipients()
* @brief      Alerts get WEB recipients
* @ingroup    APPFLOW
* 
* @return     XVECTOR<XSTRING*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XSTRING*>* APPFLOWCFG::Alerts_GetWEBRecipients()
{
  return &alerts_WEB_recipients;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::Alerts_GetWEBRecipient(int index)
* @brief      Alerts get WEB recipient
* @ingroup    APPFLOW
*
* @param[in]  index : 
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::Alerts_GetWEBRecipient(int index)
{
  if(index <  0)                               return NULL;
  if(index >= alerts_WEB_recipients.GetSize()) return NULL;

  return alerts_WEB_recipients.Get(index);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::Alerts_IsActiveUDP()
* @brief      Alerts is active UDP
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::Alerts_IsActiveUDP()
{
  return alerts_UDP_isactive;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::Alerts_GetUDPPort()
* @brief      Alerts get UDP port
* @ingroup    APPFLOW
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::Alerts_GetUDPPort()
{
  return alerts_UDP_port;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XSTRING*>* APPFLOWCFG::Alerts_GetUDPRecipients()
* @brief      Alerts get UDP recipients
* @ingroup    APPFLOW
* 
* @return     XVECTOR<XSTRING*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XSTRING*>* APPFLOWCFG::Alerts_GetUDPRecipients()
{
  return &alerts_UDP_recipients;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::Alerts_GetUDPRecipient(int index)
* @brief      Alerts get UDP recipient
* @ingroup    APPFLOW
*
* @param[in]  index : 
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::Alerts_GetUDPRecipient(int index)
{
  if(index <  0)                                return NULL;
  if(index >= alerts_UDP_recipients.GetSize())  return NULL;

  return alerts_UDP_recipients.Get(index);
}

#endif


#ifdef APPFLOW_CFG_WEBSERVER_ACTIVE

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::WebServer_GetLocalAddress()
* @brief      Web server get local address
* @ingroup    APPFLOW
*
* @return     XSTRING* : local address
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::WebServer_GetLocalAddress()
{
  return &webserver_localaddr;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::WebServer_GetPort()
* @brief      Web server get port
* @ingroup    APPFLOW
*
* @return     int : port number
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::WebServer_GetPort()
{
  return webserver_port;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::WebServer_GetTimeoutToServerPage()
* @brief      Web server get timeout to server page
* @ingroup    APPFLOW
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::WebServer_GetTimeoutToServerPage()
{
  return webserver_timeouttoserverpage;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::WebServer_AuthenticatedAccess()
* @brief      Web server authenticated access
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::WebServer_IsAuthenticatedAccess()
{
  return webserver_isauthenticatedaccess;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::WebServer_GetPassword()
* @brief      Web server get password
* @ingroup    APPFLOW
*
* @return     XSTRING* : Password web server
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::WebServer_GetPassword()
{
  return &webserver_password;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XPATH* APPFLOWCFG::WebServer_PathResources()
* @brief      Web server path resources
* @ingroup    APPFLOW
*
* @return     XPATH* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XPATH* APPFLOWCFG::WebServer_PathResources()
{
  return &webserver_path_resources;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XPATH* APPFLOWCFG::WebServer_PathPHP()
* @brief      Web server path PHP
* @ingroup    APPFLOW
*
* @return     XPATH* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XPATH* APPFLOWCFG::WebServer_PathPHP()
{
  return &webserver_path_PHP;
}

#endif


#ifdef APPFLOW_CFG_SCRIPTS_ACTIVE

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XSTRING*>* CBUILDER_CFG::Scripts_GetAll()
* @brief      Scripts get all
* @ingroup    APPFLOW
* 
* @return     XVECTOR<XSTRING*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XSTRING*>* APPFLOWCFG::Scripts_GetAll()
{
  return &scripts_list;
}
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* CBUILDER_CFG::Scripts_GetScript(int index)
* @brief      Scripts get script
* @ingroup    APPFLOW
* 
* @param[in]  index : 
* 
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::Scripts_GetScript(int index)
{
  if(index <  0)                        return NULL;
  if(index >= scripts_list.GetSize())   return NULL;

  return scripts_list.Get(index);
}

#endif


#ifdef APPFLOW_CFG_LOG_ACTIVE

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::Log_IsActive()
* @brief      Log is active
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::Log_IsActive()
{
  return log_isactive;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::Log_Backup_IsActive()
* @brief      Log backup is active
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::Log_Backup_IsActive()
{
  return log_backupisactive;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::Log_Backup_GetMaxFiles()
* @brief      Log backup get max files
* @ingroup    APPFLOW
*
* @return     int : max files number
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::Log_Backup_GetMaxFiles()
{
  return log_backupmaxfiles;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPFLOWCFG::Log_Backup_IsCompress()
* @brief      Log backup is compress
* @ingroup    APPFLOW
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPFLOWCFG::Log_Backup_IsCompress()
{
  return log_backupiscompress;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* APPFLOWCFG::Log_ActiveSectionsID()
* @brief      Log active sections ID
* @ingroup    APPFLOW
*
* @return     XSTRING* : string with the secctions active (separated by comma ',')
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* APPFLOWCFG::Log_ActiveSectionsID()
{
  return &log_activesectionsID;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE APPFLOWCFG::Log_LevelMask()
* @brief      Log level mask
* @ingroup    APPFLOW
*
* @return     XBYTE : Mask of log
*
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE APPFLOWCFG::Log_LevelMask()
{
  return (XBYTE)log_levelmask;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::Log_MaxSize()
* @brief      Log max size
* @ingroup    APPFLOW
*
* @return     int : size in bytes
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::Log_MaxSize()
{
  return log_maxsize;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPFLOWCFG::Log_ReductionPercent()
* @brief      Log reduction percent
* @ingroup    APPFLOW
*
* @return     int : percent reduction (what remains)
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPFLOWCFG::Log_ReductionPercent()
{
  return log_reductionpercent;
}

#endif


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void APPFLOWCFG::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    APPFLOW
* 
* --------------------------------------------------------------------------------------------------------------------*/
void APPFLOWCFG::Clean()
{  
  //-----------------------------------------------------------------------------------------------------

  #ifdef APPFLOW_CFG_GENERAL_ACTIVE
  scraperwebscripturldownload.Empty();
  showdetailinfo.Empty();
  #ifdef XTRACE_ACTIVE
  ntracetargets = 0;
  #endif
  #endif

  //-----------------------------------------------------------------------------------------------------

  #ifdef APPFLOW_CFG_CHECKRESOURCESHARDWARE_ACTIVE
  checkresourceshardware_memstatuscheckcadence      = 0;
  checkresourceshardware_memstatuslimitpercent      = 0;
  checkresourceshardware_totalcpuusagecheckcadence  = 0;                      
  checkresourceshardware_totalcpuusagelimitpercent  = 0;  
  checkresourceshardware_appcpuusageprocessname.Empty();
  checkresourceshardware_appcpuusagecheckcadence    = 0;                      
  checkresourceshardware_appcpuusagelimitpercent    = 0;  
  #endif

  //-----------------------------------------------------------------------------------------------------

  #ifdef APPFLOW_CFG_INTERNETSERVICES_ACTIVE
  internetservices_checkinternetstatuscadence       = 0;
  internetservices_donotletinternetconnectionmatter = false;
  internetservices_checkipschangecadence            = 0;
  internetservices_updatetimebyntpcadence           = 0;
  internetservices_nupdatetimentpservers            = 0;
  internetservices_updatetimentpmeridiandifference  = 0;
  internetservices_updatetimentpusedaylightsaving   = false;
  #ifdef APPFLOW_CFG_DNSRESOLVER_ACTIVE
  nhostsresolved = 0; 
  nDNSservers    = 0;  
  #endif
  #ifdef APPFLOW_CFG_DYNDNSMANAGER_ACTIVE
  dnsmanager_nurls = 0;   
  #endif
  #endif

  //-----------------------------------------------------------------------------------------------------

  #ifdef APPFLOW_CFG_DIOLOCATION_ACTIVE
  location_street.Empty();
  location_city.Empty();
  location_state.Empty();
  location_country.Empty();
  location_postalcode                               = 0;
  #endif

  //-----------------------------------------------------------------------------------------------------

  #ifdef APPFLOW_CFG_APPUPDATE_ACTIVE
  applicationupdate_isactive                        = false;
  applicationupdate_port                            = 0;
  applicationupdate_checkcadence                    = 0;
  applicationupdate_checktime.Empty();
  applicationupdate_maxrestorations                 = 0;
  #endif

  //-----------------------------------------------------------------------------------------------------

  #ifdef APPFLOW_CFG_WEBSERVER_ACTIVE
  webserver_localaddr.Empty();
  webserver_port                                    = 0;
  webserver_timeouttoserverpage                     = 0;
  webserver_isauthenticatedaccess                   = false;
  #endif

  //-----------------------------------------------------------------------------------------------------

  #ifdef APPFLOW_CFG_ALERTS_ACTIVE
  alerts_isactive                                   = false;
  alerts_nconditions                                = 0;
  alerts_SMTP_isactive                              = false;
  alerts_SMTP_port                                  = 0; 
  alerts_SMTP_nrecipients                           = 0;
  alerts_SMS_isactive                               = false;
  alerts_SMS_nrecipients                            = 0;  
  alerts_WEB_isactive                               = false;
  alerts_WEB_isuseget                               = false;
  alerts_WEB_nrecipients                            = 0;
  alerts_UDP_isactive                               = false;
  alerts_UDP_port                                   = 0;
  alerts_UDP_nrecipients                            = 0;
  #endif

  //-----------------------------------------------------------------------------------------------------

  #ifdef APPFLOW_CFG_SCRIPTS_ACTIVE
  scripts_nscripts                                  = 0;  
  #endif

  //-----------------------------------------------------------------------------------------------------

  #ifdef APPFLOW_CFG_LOG_ACTIVE
  log_isactive                                      = false;
  log_backupisactive                                = false;
  log_backupmaxfiles                                = 0;
  log_backupiscompress                              = false;
  log_activesectionsID.Empty();
  log_levelmask                                     = 0;
  log_maxsize                                       = 0;
  log_reductionpercent                              = 0;
  #endif

  //-----------------------------------------------------------------------------------------------------
}


#pragma endregion
