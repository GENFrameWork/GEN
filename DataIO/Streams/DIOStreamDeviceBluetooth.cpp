/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamDeviceBluetooth.cpp
* 
* @class      DIOSTREAMDEVICEBLUETOOTH
* @brief      Data Input/Output Stream Device Bluetooth class
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

#include "DIOStreamDeviceBluetooth.h"

#include <string.h>

#include "XFactory.h"
#include "XString.h"
#include "XTimer.h"

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


#pragma region CLASS_DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL::DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL::DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL::~DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL::~DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL::GetType()
* @brief      Get type
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL::GetType()
{
  return type;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL::SetType(XDWORD type)
* @brief      Set type
* @ingroup    DATAIO
*
* @param[in]  type :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL::SetType(XDWORD type)
{
  this->type = type;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL::GetChannel()
* @brief      Get channel
* @ingroup    DATAIO
*
* @return     XWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL::GetChannel()
{
  return channel;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL::SetChannel(XWORD channel)
* @brief      Set channel
* @ingroup    DATAIO
*
* @param[in]  channel :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL::SetChannel(XWORD channel)
{
  this->channel = channel;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL::Clean()
{
  type    = 0;
  channel = 0;
}


#pragma endregion


#pragma region CLASS_DIOSTREAMDEVICEBLUETOOTHSDPSERVICE


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::DIOSTREAMDEVICEBLUETOOTHSDPSERVICE()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::DIOSTREAMDEVICEBLUETOOTHSDPSERVICE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::~DIOSTREAMDEVICEBLUETOOTHSDPSERVICE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::~DIOSTREAMDEVICEBLUETOOTHSDPSERVICE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::GetID()
* @brief      Get ID
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::GetID()
{
  return ID;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::SetID(XDWORD ID)
* @brief      Set ID
* @ingroup    DATAIO
*
* @param[in]  ID :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::SetID(XDWORD ID)
{
  this->ID = ID;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::GetRecHandle()
* @brief      Get rec handle
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::GetRecHandle()
{
  return rechandle;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::SetRecHandle(XDWORD rechandle)
* @brief      Set rec handle
* @ingroup    DATAIO
*
* @param[in]  rechandle :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::SetRecHandle(XDWORD rechandle)
{
  this->rechandle = rechandle;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::GetName()
* @brief      Get name
* @ingroup    DATAIO
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::GetName()
{
  return &name;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::GetDescription()
* @brief      Get description
* @ingroup    DATAIO
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::GetDescription()
{
  return &description;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::GetProvider()
* @brief      Get provider
* @ingroup    DATAIO
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::GetProvider()
{
  return &provider;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::GetNProtocols()
* @brief      Get N protocols
* @ingroup    DATAIO
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::GetNProtocols()
{
  return nprotocols;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::SetNProtocols(int nprotocols)
* @brief      Set N protocols
* @ingroup    DATAIO
*
* @param[in]  nprotocols :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::SetNProtocols(int nprotocols)
{
  this->nprotocols = nprotocols;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL* DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::GetProtocol(int index)
* @brief      Get protocol
* @ingroup    DATAIO
*
* @param[in]  index :
*
* @return     DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL* DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::GetProtocol(int index)
{
  if(index>=DIOSTREAMDEVICEBLUETOOTHSDPSERVICE_MAXPROTOCOLS) return NULL;
  if(index<0)                                                return NULL;

  return &protocols[index];
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::SetProtocol(int index, DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL* protocol)
* @brief      Set protocol
* @ingroup    DATAIO
*
* @param[in]  index :
* @param[in]  protocol :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::SetProtocol(int index, DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL* protocol)
{
  if(!protocol)         return false;
  if(nprotocols >= DIOSTREAMDEVICEBLUETOOTHSDPSERVICE_MAXPROTOCOLS) return false;
  if(nprotocols <= 0)   return false;

  protocols[index].SetType(protocol->GetType());
  protocols[index].SetChannel(protocol->GetChannel());

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEBLUETOOTHSDPSERVICE::Clean()
{
  ID          = 0;
  rechandle   = 0;
  nprotocols  = 0;
}


#pragma endregion



#pragma region CLASS_DIOSTREAMDEVICEBLUETOOTH 


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMDEVICEBLUETOOTH::DIOSTREAMDEVICEBLUETOOTH()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEBLUETOOTH::DIOSTREAMDEVICEBLUETOOTH()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMDEVICEBLUETOOTH::~DIOSTREAMDEVICEBLUETOOTH()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEBLUETOOTH::~DIOSTREAMDEVICEBLUETOOTH()
{
  Service_DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOMAC* DIOSTREAMDEVICEBLUETOOTH::GetMAC()
* @brief      Get MAC
* @ingroup    DATAIO
*
* @return     DIOMAC* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOMAC* DIOSTREAMDEVICEBLUETOOTH::GetMAC()
{
  return &MAC;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOSTREAMDEVICEBLUETOOTH::GetRSSI()
* @brief      Get RSSI
* @ingroup    DATAIO
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMDEVICEBLUETOOTH::GetRSSI()
{
  return rssi;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEBLUETOOTH::SetRSSI(int rssi)
* @brief      Set RSSI
* @ingroup    DATAIO
*
* @param[in]  rssi : 
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEBLUETOOTH::SetRSSI(int rssi)
{
  this->rssi = rssi;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMDEVICEBLUETOOTH::IsVisible()
* @brief      Is visible
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMDEVICEBLUETOOTH::IsVisible()
{
  return isvisible;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMDEVICEBLUETOOTH::SetIsVisible(bool isvisible)
* @brief      Set is visible
* @ingroup    DATAIO
*
* @param[in]  isvisible :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMDEVICEBLUETOOTH::SetIsVisible(bool isvisible)
{
  this->isvisible = isvisible;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOGUID* DIOSTREAMDEVICEBLUETOOTH::GetServiceClass()
* @brief      Get service class
* @ingroup    DATAIO
*
* @return     DIOGUID* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOGUID* DIOSTREAMDEVICEBLUETOOTH::GetServiceClass()
{
  return &serviceclass;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMDEVICEBLUETOOTH::Service_Add(DIOSTREAMDEVICEBLUETOOTHSDPSERVICE* service)
* @brief      Service add
* @ingroup    DATAIO
*
* @param[in]  service :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMDEVICEBLUETOOTH::Service_Add(DIOSTREAMDEVICEBLUETOOTHSDPSERVICE* service)
{
  if(!service) return false;

  services.Add(service);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<DIOSTREAMDEVICEBLUETOOTHSDPSERVICE*>* DIOSTREAMDEVICEBLUETOOTH::Service_GetAll()
* @brief      Service get all
* @ingroup    DATAIO
*
* @return     XVECTOR<DIOSTREAMDEVICEBLUETOOTHSDPSERVICE*>* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<DIOSTREAMDEVICEBLUETOOTHSDPSERVICE*>* DIOSTREAMDEVICEBLUETOOTH::Service_GetAll()
{
  return &services;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMDEVICEBLUETOOTH::Service_DeleteAll()
* @brief      Service delete all
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMDEVICEBLUETOOTH::Service_DeleteAll()
{
  if(services.IsEmpty()) return false;

  services.DeleteContents();

  services.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOSTREAMDEVICEBLUETOOTH::GetChannel(XWORD servicetype)
* @brief      Get channel
* @ingroup    DATAIO
*
* @param[in]  servicetype :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMDEVICEBLUETOOTH::GetChannel(XWORD servicetype)
{
  for(int c=0; c<(int)services.GetSize(); c++)
    {
       DIOSTREAMDEVICEBLUETOOTHSDPSERVICE* service =  (DIOSTREAMDEVICEBLUETOOTHSDPSERVICE*)services.Get(c);
       if(service)
         {
           if(servicetype == service->GetID())
             {
               for(int d=0; d<(int)service->GetNProtocols(); d++)
                 {
                   DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL* protocol = (DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL*)service->GetProtocol(d);
                   if(protocol)
                     {
                       int channel = protocol->GetChannel();
                       if(channel) return channel;
                     }
                 }
            }
         }
    }

  return DIOSTREAMDEVICEBLUETOOTH_NOCHANNEL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOSTREAMDEVICEBLUETOOTH::GetChannelSerialPort()
* @brief      Get channel serial port
* @ingroup    DATAIO
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMDEVICEBLUETOOTH::GetChannelSerialPort()
{
  return GetChannel(DIOSTREAMDEVICEBLUETOOTHSERVICETYPE_SERIALPORT);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOSTREAMDEVICEBLUETOOTH::GetChannelObexPush()
* @brief      Get channel obex push
* @ingroup    DATAIO
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMDEVICEBLUETOOTH::GetChannelObexPush()
{
  return GetChannel(DIOSTREAMDEVICEBLUETOOTHSERVICETYPE_OBEXOBJECTPUSH);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOSTREAMDEVICEBLUETOOTH::GetChannelDialUpNetworking()
* @brief      Get channel dial up networking
* @ingroup    DATAIO
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMDEVICEBLUETOOTH::GetChannelDialUpNetworking()
{
  return GetChannel(DIOSTREAMDEVICEBLUETOOTHSERVICETYPE_DIALUPNETWORKING);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOSTREAMDEVICEBLUETOOTH::GetChannelHID()
* @brief      Get channel HID
* @ingroup    DATAIO
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMDEVICEBLUETOOTH::GetChannelHID()
{
  return GetChannel(DIOSTREAMDEVICEBLUETOOTHSERVICETYPE_HUMANINTERFACEDEVICESERVICE);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMDEVICEBLUETOOTH::DebugPrintInfo()
* @brief      Debug print info
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMDEVICEBLUETOOTH::DebugPrintInfo()
{
  DIOSTREAMDEVICE::DebugPrintInfo();

  XSTRING line;
  XSTRING data;

  GetMAC()->GetXString(data);
  
  line.Format(__L("MAC              : %s") , data.Get());                          XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, line.Get());
  line.Format(__L("Service Class    : %08X"), GetServiceClass()->GetData1());      XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, line.Get());

  XSTRING ID;
  XSTRING channel;
  int     nservices = services.GetSize();

  for(int c=0;c<nservices;c++)
    {
      DIOSTREAMDEVICEBLUETOOTHSDPSERVICE* service = (DIOSTREAMDEVICEBLUETOOTHSDPSERVICE*)services.Get(c);
      if(!service) continue;

      ID.Format(__L("%08X"),service->GetID());

      line  = ID;
      line += __L(" - ");

      line += __L(" \"");
      line += service->GetName()->Get();
      line += __L("\" ");

      line += __L(" \"");
      line += service->GetDescription()->Get();
      line += __L("\" ");

      line += __L(" \"");
      line += service->GetProvider()->Get();
      line += __L("\" ");

      for(int d=0;d<service->GetNProtocols();d++)
        {
          DIOSTREAMDEVICEBLUETOOTHSDPSERVICEPROTOCOL* protocol = service->GetProtocol(d);
          if(protocol)
            {
              ID.Format(__L("%08X"),protocol->GetType());

              line += ID;
              line += " ";

              channel.Format(__L("[%d]"),protocol->GetChannel());

              line += channel;
              line += " ";
            }
        }

      XTRACE_PRINTCOLOR(2, line.Get());
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEBLUETOOTH::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEBLUETOOTH::Clean()
{
  type            = DIOSTREAMDEVICE_TYPE_BLUETOOTH;

  rssi            = 0;  
  isvisible       = false;
}


#pragma endregion


#pragma endregion

