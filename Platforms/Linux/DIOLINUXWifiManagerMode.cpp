/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOLINUXWifiManagerMode.cpp
*
* @class      DIOLINUXWIFIMANAGERMODE
* @brief      Data Input/Output LINUX Wifi Manager Mode
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>


#include "XFactory.h"
#include "XTimer.h"
#include "XTrace.h"
#include "XVector.h"
#include "XSleep.h"
#include "XString.h"
#include "XFileTXT.h"
#include "XProcessManager.h"

#include "DIOFactory.h"
#include "DIOStreamDeviceIP.h"
#include "DIOStreamEnumDevices.h"

#include "DIOLINUXWifiManagerMode.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOLINUXWIFIMANAGERMODE::DIOLINUXWIFIMANAGERMODE()
* @brief      Constructor
* @ingroup    DATAIO
*
* @return     Does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOLINUXWIFIMANAGERMODE::DIOLINUXWIFIMANAGERMODE()    
{ 
  Clean();                            
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOLINUXWIFIMANAGERMODE::~DIOLINUXWIFIMANAGERMODE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @return     Does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOLINUXWIFIMANAGERMODE::~DIOLINUXWIFIMANAGERMODE()    
{ 
  Clean();                            
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXWIFIMANAGERMODE::SetMode_AP(XCHAR* SSID, XCHAR* password, bool serialize)
* @brief      SetMode_AP
* @ingroup    PLATFORM_LINUX
*
* @param[in]  SSID : 
* @param[in]  password : 
* @param[in]  serialize : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::SetMode_AP(XCHAR* SSID, XCHAR* password, bool serialize)
{    
  WPA_RemoveAllNetworks();

  APList_Reload();
  
  return WPA_SetMode(2, SSID, password, -1, serialize);                                                                                                                      
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXWIFIMANAGERMODE::SetMode_Client(XCHAR* SSID, XCHAR* password, bool serialize)
* @brief      SetMode_Client
* @ingroup    PLATFORM_LINUX
*
* @param[in]  SSID : 
* @param[in]  password : 
* @param[in]  serialize : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::SetMode_Client(XCHAR* SSID, XCHAR* password, bool serialize)  
{     
  bool status = false;    

  switch(modetype)
    {
      case DIOWIFIMANAGER_MODETYPE_DEFAULT              : WPA_RemoveAllNetworks(); 
                                                          //APList_Reload();
                                                          status = WPA_SetMode(0, SSID, password, -1, serialize);                                                                                                                       
                                                          break;

      case DIOWIFIMANAGER_MODETYPE_LINUX_CLIENT_NM      : status = NM_SetMode(0, SSID, password, -1, serialize);    
                                                          break;           
    }

  return status;
}    



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXWIFIMANAGERMODE::IsDisconnected()
* @brief      IsDisconnected
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::IsDisconnected()
{
  bool status = false;    
  
  switch(modetype)
    {
      case DIOWIFIMANAGER_MODETYPE_DEFAULT              : status = WPA_IsDisconnected();    break;
      case DIOWIFIMANAGER_MODETYPE_LINUX_CLIENT_NM      : status = NM_IsDisconnected();     break;
    }

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXWIFIMANAGERMODE::Disconnect()
* @brief      Disconnect
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::Disconnect()
{
  bool status = false;    
  
  switch(modetype)
    {
      case DIOWIFIMANAGER_MODETYPE_DEFAULT            : status = WPA_Disconnect();    break;
      case DIOWIFIMANAGER_MODETYPE_LINUX_CLIENT_NM    : status = NM_Disconnect();     break;
    }
  
  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXWIFIMANAGERMODE::APList_Reload()
* @brief      APList_Reload
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::APList_Reload()
{
  return WPA_GetListRemoteAP(nameAPs);                                                                                                                          
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXWIFIMANAGERMODE::WPA_SetMode(int mode, XCHAR* SSID, XCHAR* password, int indexnetwork, bool serialize)
* @brief      WPA_SetMode
* @ingroup    PLATFORM_LINUX
*
* @param[in]  mode : 
* @param[in]  SSID : 
* @param[in]  password : 
* @param[in]  indexnetwork : 
* @param[in]  serialize : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::WPA_SetMode(int mode, XCHAR* SSID, XCHAR* password, int indexnetwork, bool serialize)  
{
  XSTRING   prefixcommand;
  XSTRING   command;
  XSTRING   modestr;
  XSTRING   output;
  int       indexnet;  
  bool      status;
     
  prefixcommand.Format(DIOLINUXWIFIMANAGERMODE_PREFIXCOMMAND, netinterface.Get());

  indexnet = indexnetwork;

  if(indexnet == -1)
    {
      command.Format(__L("%s add_network"), prefixcommand.Get());
      WPA_CLI_Command(command, output);

      output.DeleteCharacter(__C('\n'));
      output.DeleteCharacter(__C('\r'));

      indexnet = output.ConvertToInt();
    }
   
  status = WPA_SetValue(indexnet, __L("ssid"), SSID, true);
  if(!status) return false;

  if(password)
    {
      status = WPA_SetValue(indexnet, __L("psk"), password, false);
      if(!status) return false;
    }
   else
    {
      status = WPA_SetValue(indexnet, __L("key_mgmt"), __L("NONE"), false);
      if(!status) return false;
    }
      
  modestr.Format(__L("%d"), mode);

  status = WPA_SetValue(indexnet, __L("mode"), modestr.Get(), false);
  if(!status) return false;

  if(mode == 2)
    {
      command.Format(__L("%s ap_scan 1"), prefixcommand.Get());
      WPA_CLI_Command(command, output);
    }  
  
  command.Format(__L("%s enable_network %d"), prefixcommand.Get(), indexnet);
  status = WPA_CLI_Command(command, output);

  command.Format(__L("%s reconnect"), prefixcommand.Get(), indexnet);
  status = WPA_CLI_Command(command, output);

 
  if(mode == 0)
    {
      XSTRING   input;
      XSTRING   param;
      int       returncode = 0;


      for(int c=0; c<10; c++)
        {        
          param.Format(__L("-x"));   
          GEN_XPROCESSMANAGER.Application_Execute(__L("/sbin/dhcpcd"), param.Get(), &input, &output, &returncode);
        }

      for(int c=0; c<10; c++)
        {
          GEN_XPROCESSMANAGER.MakeSystemCommand(__L("killall -9 dhcpcd"));                
        }

      param.Format(__L("-4 -t 15 %s"), netinterface.Get());

      XTRACE_PRINTCOLOR(XTRACE_COLOR_PURPLE, __L("dhcpcd: %s"), param.Get());
 
      GEN_XPROCESSMANAGER.Application_Execute(__L("/sbin/dhcpcd"), param.Get(), &input, &output, &returncode);
      XTRACE_PRINTCOLOR(XTRACE_COLOR_PURPLE, __L("dhclient %s %d"), output.Get(), returncode);

      status = false;
      if(output.Find(__L("rebinding lease"), true) != XSTRING_NOTFOUND) status = true;
      

      for(int c=0; c<10; c++)
        {        
          param.Format(__L("-x"));   
          GEN_XPROCESSMANAGER.Application_Execute(__L("/sbin/dhcpcd"), param.Get(), &input, &output, &returncode);
        }


      for(int c=0; c<10; c++)
        {          
          GEN_XPROCESSMANAGER.MakeSystemCommand(__L("killall -9 dhcpcd"));                
        }
    }  
 

  /*
  if(mode == 0)
    {
      XSTRING   input;
      XSTRING   param;
      int       returncode = 0;

      //GEN_XSLEEP.Seconds(2);

      param = __L("-v wlan0");

      XTRACE_PRINTCOLOR(XTRACE_COLOR_PURPLE, __L("dhclient ... "));
 
      GEN_XPROCESSMANAGER.Application_Execute(__L("/sbin/dhclient"), param.Get(), &input, &output, &returncode);

      XTRACE_PRINTCOLOR(XTRACE_COLOR_PURPLE, __L("dhclient %s %d"), output.Get(), returncode);
    }  
  */

  /*
   if(mode == 0)
    {
      XSTRING   input;
      XSTRING   command;
      int       returncode = 0;
   
      command = __L("/sbin/dhclient -v wlan0");

      XTRACE_PRINTCOLOR(XTRACE_COLOR_PURPLE, __L("dhclient ... "));
 
      GEN_XPROCESSMANAGER.MakeCommand(command.Get(), &output, &returncode);

      XTRACE_PRINTCOLOR(XTRACE_COLOR_PURPLE, __L("dhclient %s %d"), output.Get(), returncode);
    }  
  */


  if(serialize)
    {
      if(status)
        {
          command.Format(__L("%s save_config"), prefixcommand.Get());
          status = WPA_CLI_Command(command, output);
      
          if(status)
            {
              command.Format(__L("%s reconfigure"), prefixcommand.Get());
              status = WPA_CLI_Command(command, output);
            }
        }
    }
  
  return status;
}    



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXWIFIMANAGERMODE::WPA_IsDisconnected()
* @brief      WPA_IsDisconnected
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::WPA_IsDisconnected()
{
  XSTRING   prefixcommand;
  XSTRING   command;
  XSTRING   output;
  bool      status = false;    
  
  prefixcommand.Format(DIOLINUXWIFIMANAGERMODE_PREFIXCOMMAND, netinterface.Get());

  command.Format(__L("%s status"), prefixcommand.Get());
  WPA_CLI_Command(command, output);

  if(output.Find(__L("wpa_state=INACTIVE"), true) != XSTRING_NOTFOUND) 
    { 
      status = true;
    }
   else 
    {
      if(output.Find(__L("wpa_state=DISCONNECTED"), true) != XSTRING_NOTFOUND) 
        {
          status = true;
        }
       else 
        {
          if(output.Find(__L("wpa_state=COMPLETED"), true) != XSTRING_NOTFOUND) 
            { 
              status = false; 
            }
        }
    }

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXWIFIMANAGERMODE::WPA_Disconnect()
* @brief      WPA_Disconnected
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::WPA_Disconnect()
{
  XSTRING   prefixcommand;
  XSTRING   command;
  XSTRING   output;
  bool      status = false;    
  
  prefixcommand.Format(DIOLINUXWIFIMANAGERMODE_PREFIXCOMMAND, netinterface.Get());

  command.Format(__L("%s disconnect"), prefixcommand.Get());
  status =  WPA_CLI_Command(command, output);

  WPA_RemoveAllNetworks();

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXWIFIMANAGERMODE::WPA_RemoveAllNetworks()
* @brief      WPA_RemoveAllNetworks
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::WPA_RemoveAllNetworks()
{
  XSTRING   prefixcommand;
  XSTRING   command;
  XSTRING   output;
  bool      status;

  prefixcommand.Format(DIOLINUXWIFIMANAGERMODE_PREFIXCOMMAND, netinterface.Get());

  for(int c=0; c<32; c++)
    {
      command.Format(__L("%s remove_network %d"), prefixcommand.Get(), c);    
      status = WPA_CLI_Command(command, output);     
    } 
      
  command.Format(__L("%s save_config"), prefixcommand.Get());
  status = WPA_CLI_Command(command, output);
  
  return status;

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXWIFIMANAGERMODE::WPA_SetValue(int indexnetwork, XCHAR* namevalue, XCHAR* value)
* @brief      WPA_SetValue
* @ingroup    PLATFORM_LINUX
*
* @param[in]  indexnetwork : 
* @param[in]  namevalue : 
* @param[in]  value : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::WPA_SetValue(int indexnetwork, XCHAR* namevalue, XCHAR* value, bool isstring)
{
  XSTRING   prefixcommand;
  XSTRING   command;
  XSTRING   output;
  bool      status;

  prefixcommand.Format(DIOLINUXWIFIMANAGERMODE_PREFIXCOMMAND, netinterface.Get());

  if(isstring)  
    {
      command.Format(__L("%s set_network %d %s \"%s\"") , prefixcommand.Get(), indexnetwork, namevalue, value);          
    }
   else  
    {
      command.Format(__L("%s set_network %d %s %s")       , prefixcommand.Get(), indexnetwork, namevalue, value);    
    }

  status = WPA_CLI_Command(command, output);

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXWIFIMANAGERMODE::WPA_SetValue(int indexnetwork, XSTRING& namevalue, XSTRING& value)
* @brief      WPA_SetValue
* @ingroup    PLATFORM_LINUX
*
* @param[in]  indexnetwork : 
* @param[in]  namevalue : 
* @param[in]  value : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::WPA_SetValue(int indexnetwork, XSTRING& namevalue, XSTRING& value)
{
  return WPA_SetValue(indexnetwork, namevalue.Get(), value.Get());
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXWIFIMANAGERMODE::WPA_GetListRemoteAP(XVECTOR<XSTRING*>& nameAPs)
* @brief      WPA_GetListRemoteAP
* @ingroup    PLATFORM_LINUX
*
* @param[in]  nameAPs : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::WPA_GetListRemoteAP(XVECTOR<XSTRING*>& nameAPs)
{ 
  XSTRING   prefixcommand;
  XSTRING   command;
  XSTRING   output;
  bool      status;


  nameAPs.DeleteContents();
  nameAPs.DeleteAll();

  prefixcommand.Format(DIOLINUXWIFIMANAGERMODE_PREFIXCOMMAND, netinterface.Get());

  command.Format(__L("%s scan") , prefixcommand.Get());     
  status = WPA_CLI_Command(command, output);
  if(!status) return false;

  GEN_XSLEEP.Seconds(3);

  command.Format(__L("%s scan_results") , prefixcommand.Get());     
  status = WPA_CLI_Command(command, output);

  int indexfr =  output.Find(__L("\n"), true);
  if(indexfr == XSTRING_NOTFOUND) return false;

  indexfr++;

  int index      = 0;
  int indexstart = 0;

  do{
      index =  output.Find(__L("\n"), true, indexfr);
      if(index == XSTRING_NOTFOUND ) break;

      for(int c=index; c>0 ; c--)
        {
          if(output.Get()[c] == __C(']'))  
            {
              indexstart = c;
              break;
            }               
        }

      indexfr = index+1;

      if(indexstart)
        {
          XSTRING* newstr = new XSTRING();
          if(newstr)
            {
              output.Copy(indexstart+1, index, (*newstr));
              newstr->DeleteCharacter(__C('\t'), XSTRINGCONTEXT_FROM_FIRST);
              
              nameAPs.Add(newstr);                 
            }
        }

    } while(index != XSTRING_NOTFOUND);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXWIFIMANAGERMODE::WPA_CLI_Command(XSTRING& command, XSTRING& output)
* @brief      WPA_CLI_Command
* @ingroup    PLATFORM_LINUX
*
* @param[in]  command : 
* @param[in]  output : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::WPA_CLI_Command(XSTRING& command, XSTRING& output)
{
  if(command.IsEmpty()) return false;

  output.Empty();

  XSTRING   input;
  int       returncode = 0;
  bool      status     = false;

  GEN_XPROCESSMANAGER.Application_Execute(__L("/sbin/wpa_cli"), command.Get(), &input, &output, &returncode);
  if(output.Find(__L("OK"), true) != XSTRING_NOTFOUND) status = true;
 
  output.DeleteCharacter(__C('\n'));
  output.DeleteCharacter(__C('\r'));

  XTRACE_PRINTCOLOR(XTRACE_COLOR_PURPLE, __L("[WIFI manager] WPA CLI command [%s]: %s"), command.Get(), output.Get()); 
    
  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXWIFIMANAGERMODE::NM_SetMode(int mode, XCHAR* SSID, XCHAR* password, int indexnetwork, bool serialize)
* @brief      NM_SetMode
* @ingroup    PLATFORM_LINUX
*
* @param[in]  mode : 
* @param[in]  SSID : 
* @param[in]  password : 
* @param[in]  indexnetwork : 
* @param[in]  serialize : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::NM_SetMode(int mode, XCHAR* SSID, XCHAR* password, int indexnetwork, bool serialize)
{
  XSTRING   command;
  XSTRING   output;
  bool      status = false;

  /*
  command = __L("radio wifi on");
  status = NM_CLI_Command(command, output);
  
  if(!output.IsEmpty()) return false;
 
  for(int c=0; c<5; c++)
    {
      command = __L("device wifi rescan");
      status = NM_CLI_Command(command, output);

      command = __L("device wifi list");
      status = NM_CLI_Command(command, output);

      if(status)
        {
          if(output.Find(SSID, true) != XSTRING_NOTFOUND) break;
        }
    }
  */

  command.Format(__L("device wifi connect %s"), SSID);  
  if(password) command.AddFormat(__L(" password %s"), password);
  status = NM_CLI_Command(command, output);  
  if(status)
    {
      if(output.Find(__L("successfully activated"), false) == XSTRING_NOTFOUND) status = false;
    }
  
  lastSSID.Empty();
  if(status) lastSSID = SSID;

  return status;
} 



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXWIFIMANAGERMODE::NM_IsDisconnected()
* @brief      NM_IsDisconnected
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::NM_IsDisconnected()
{
  XSTRING   command;
  XSTRING   output;
  bool      status = false;
  
  if(lastSSID.IsEmpty()) return true;

  command.Format(__L("con show --active"));  
  status = NM_CLI_Command(command, output); 
  if(status)
    {
      if(output.Find(lastSSID.Get(), false) == XSTRING_NOTFOUND) status = true; else status = false;
    }
    
  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXWIFIMANAGERMODE::NM_Disconnect()
* @brief      NM_Disconnected
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::NM_Disconnect()
{
  XSTRING   command;
  XSTRING   output;
  bool      status = false;

  if(lastSSID.IsEmpty()) return false;

  command.Format(__L("con down %s"), lastSSID.Get());  
  status = NM_CLI_Command(command, output);  
  if(status)
    {
      status = false; 
      if(output.Find(__L("successfully deactivated"), false) != XSTRING_NOTFOUND)  status = true;
    }

  command.Format(__L("con del %s"), lastSSID.Get());  
  status = NM_CLI_Command(command, output);  
  if(status)
    {
      status = false;
      if(output.Find(__L("successfully deleted"), false) != XSTRING_NOTFOUND) status = true;
    }
  
  if(status) 
    {
      // After disconnecting you have to wait 3 seconds for the WIFIs list to be available  �? 
      /*
      for(int c=0; c<12; c++)
        {
          command = __L("device wifi rescan");
          NM_CLI_Command(command, output);
          command = __L("device wifi list");
          NM_CLI_Command(command, output);


          GEN_XSLEEP.Seconds(1);
        }
      */
      lastSSID.Empty();
    }

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXWIFIMANAGERMODE::NM_CLI_Command(XSTRING& command, XSTRING& output)
* @brief      NM_CLI_Command
* @ingroup    PLATFORM_LINUX
*
* @param[in]  command : 
* @param[in]  output : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXWIFIMANAGERMODE::NM_CLI_Command(XSTRING& command, XSTRING& output)
{
  if(command.IsEmpty()) return false;

  output.Empty();

  XSTRING   input;
  int       returncode = 0;
  bool      status     = false;

  GEN_XPROCESSMANAGER.Application_Execute(__L("/usr/bin/nmcli"), command.Get(), &input, &output, &returncode);
  if(output.GetSize()) status = true;
   
  XTRACE_PRINTCOLOR(XTRACE_COLOR_PURPLE, __L("[WIFI manager] NM CLI command [%s]: %s"), command.Get(), output.Get()); 
    
  return status;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOLINUXWIFIMANAGERMODE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @return     void : does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOLINUXWIFIMANAGERMODE::Clean()
{


}
