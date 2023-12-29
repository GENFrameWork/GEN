/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOFactory.cpp
*
* @class      DIOFACTORY
* @brief      Data Input/Output factory class
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


#ifdef DIO_ACTIVE

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DIOGPIO.h"
#include "DIOLedNeoPixelWS2812B.h"

#include "DIOFactory.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

DIOFACTORY* DIOFACTORY::instance = NULL;

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOFACTORY::DIOFACTORY()
* @brief      Constructor
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOFACTORY::DIOFACTORY()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOFACTORY::~DIOFACTORY()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOFACTORY::~DIOFACTORY()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOFACTORY::GetIsInstanced()
* @brief      Get if Is Instanced
* @note       STATIC
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOFACTORY::GetIsInstanced()
{
  return instance!=NULL;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOFACTORY& DIOFACTORY::GetInstance()
* @brief      Get Instance of DIOFACTORY
* @note       STATIC
* @ingroup    XUTILS
*
* @return     DIOFACTORY& :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOFACTORY& DIOFACTORY::GetInstance()
{
  if(!instance) instance = new DIOFACTORY();

  return (*instance);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOFACTORY::SetInstance(DIOFACTORY* instance)
* @brief      Set Instance of DIOFACTORY
* @note       STATIC
* @ingroup    XUTILS
*
* @param[in]  _instance : new instance
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOFACTORY::SetInstance(DIOFACTORY* _instance)
{
  if(!_instance) return false;

  instance = _instance;

  return (instance)?true:false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOFACTORY::DelInstance()
* @brief      Delete Instance of DIOFACTORY
* @note       STATIC
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOFACTORY::DelInstance()
{
  if(!instance) return false;

  delete instance;
  instance = NULL;

  return true;
}



#ifdef ANYTYPEOFDIOSTREAMIO

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMENUMDEVICES* DIOFACTORY::CreateStreamEnumDevices(DIOSTREAMENUMTYPE type)
* @brief      Create Stream Enum Devices
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @param[in]  type : type of enum devices (DIOSTREAMENUMTYPE)
*
* @return     DIOSTREAMENUMDEVICES* : enum created
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMENUMDEVICES* DIOFACTORY::CreateStreamEnumDevices(DIOSTREAMENUMTYPE type)
{
  return NULL;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOFACTORY::DeleteStreamEnumDevices(DIOSTREAMENUMDEVICES* enumdevices)
* @brief      Delete Stream Enum Devices
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @param[in]  enumdevices : enum to delete
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOFACTORY::DeleteStreamEnumDevices(DIOSTREAMENUMDEVICES* enumdevices)
{
  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAM* DIOFACTORY::CreateStreamIO(DIOSTREAMCONFIG* config)
* @brief      Create Stream IO
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @param[in]  config : config to create stream IO
*
* @return     DIOSTREAM* : stream IO created
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAM* DIOFACTORY::CreateStreamIO(DIOSTREAMCONFIG* config)
{
  return NULL;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOFACTORY::DeleteStreamIO(DIOSTREAM* streamio)
* @brief      Delete Stream IO
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @param[in]  streamio : stream IO to delete
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOFACTORY::DeleteStreamIO(DIOSTREAM* streamio)
{
  return false;
}


#endif




#ifdef DIO_WIFIMANAGERMODE_ACTIVE

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOWIFIMANAGERMODE* DIOFACTORY::CreateWifiManagerMode()
* @brief      CreateWifiManagerMode
* @ingroup    DATAIO
*
* @return     DIOWIFIMANAGERMODE* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOWIFIMANAGERMODE* DIOFACTORY::CreateWifiManagerMode()
{
  return NULL;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOFACTORY::DeleteWifiManagerMode(DIOWIFIMANAGERMODE* wifimanagermode)
* @brief      DeleteWifiManagerMode
* @ingroup    DATAIO
*
* @param[in]  wifimanagermode : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOFACTORY::DeleteWifiManagerMode(DIOWIFIMANAGERMODE* wifimanagermode)
{
  return false;
}


#endif



#ifdef DIO_PCAP_ACTIVE

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPCAP* DIOFACTORY::CreatePCap()
* @brief      Create PCap
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @return     DIOPCAP* : PCap created
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPCAP* DIOFACTORY::CreatePCap()
{
  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOFACTORY::DeletePCap(DIOPCAP* pcap)
* @brief      DeletePCap
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @param[in]  pcap : pcap to delete
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOFACTORY::DeletePCap(DIOPCAP* pcap)
{
  return false;
}

#endif



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIONOTIFICATIONSMANAGER* DIOFACTORY::CreateNotificationsManager()
* @brief      CreateNotificationsManager
* @ingroup    DATAIO
*
* @return     DIONOTIFICATIONSMANAGER* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIONOTIFICATIONSMANAGER* DIOFACTORY::CreateNotificationsManager()
{
  return NULL;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOFACTORY::DeleteNotificationsManager(DIONOTIFICATIONSMANAGER* notificationsmanager)
* @brief      DeleteNotificationsManager
* @ingroup    DATAIO
*
* @param[in]  notificationsmanager :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOFACTORY::DeleteNotificationsManager(DIONOTIFICATIONSMANAGER* notificationsmanager)
{
  return false;
}



#ifdef DIO_LEDNEOPIXELWS2812B_ACTIVE 

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOLEDNEOPIXELWS2812B* DIOFACTORY::CreateLedNeopixelWS2812B()
* @brief      CreateLedNeopixelWS2812B
* @ingroup    DATAIO
*
* @return     DIOLEDNEOPIXELWS2812B* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOLEDNEOPIXELWS2812B* DIOFACTORY::CreateLedNeopixelWS2812B()
{
  DIOLEDNEOPIXELWS2812B* ledneopixelws2812b = new DIOLEDNEOPIXELWS2812B();

  return ledneopixelws2812b;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOFACTORY::DeleteLedNeopixelWS2812B(DIOLEDNEOPIXELWS2812B* ledneopixelws2812b)
* @brief      DeleteLedNeopixelWS2812B
* @ingroup    DATAIO
*
* @param[in]  ledneopixelws2812b : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOFACTORY::DeleteLedNeopixelWS2812B(DIOLEDNEOPIXELWS2812B* ledneopixelws2812b)
{
  if(!ledneopixelws2812b) return false;
  
  delete ledneopixelws2812b;
  ledneopixelws2812b = NULL;
  
  return true;
}


#endif



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOFACTORY::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOFACTORY::Clean()
{


}




#endif


