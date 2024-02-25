/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamBluetoothLERemoteEnumDevices_XEvent.cpp
* 
* @class      DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_XEVENT
* @brief      Data Input/Output Stream Bluetooth LE remote Enum Devices eXtension event class
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

/*---- PRECOMPILATION INCLUDES ----------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOStreamBluetoothLERemoteEnumDevices_XEvent.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_XEVENT::DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_XEVENT(XSUBJECT* subject, XDWORD family, XDWORD type)
* @brief      Constructor
* @ingroup    DATAIO
*
* @param[in]  XSUBJECT* : 
* @param[in]   XDWORD family : 
* @param[in]   XDWORD type : 
*
* @return     Does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_XEVENT::DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_XEVENT(XSUBJECT* subject, XDWORD family, XDWORD type) : XEVENT(subject, family, type)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_XEVENT::~DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_XEVENT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @return     Does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_XEVENT::~DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_XEVENT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEBLUETOOTHLE* DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_XEVENT::GetDeviceSearch()
* @brief      GetDeviceSearch
* @ingroup    DATAIO
*
* @return     DIOSTREAMDEVICEBLUETOOTHLE* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEBLUETOOTHLE* DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_XEVENT::GetDeviceSearch()
{
  return devicesearch;
}

    
/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_XEVENT::SetDeviceSearch(DIOSTREAMDEVICEBLUETOOTHLE* devicesearch)
* @brief      SetDeviceSearch
* @ingroup    DATAIO
*
* @param[in]  devicesearch : 
*
* @return     void : does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_XEVENT::SetDeviceSearch(DIOSTREAMDEVICEBLUETOOTHLE* devicesearch)
{
  this->devicesearch = devicesearch;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_XEVENT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @return     void : does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_XEVENT::Clean()
{
  devicesearch = NULL;
}


#pragma endregion

