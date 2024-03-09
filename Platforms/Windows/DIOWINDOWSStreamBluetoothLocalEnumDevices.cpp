/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOWINDOWSStreamBluetoothLocalEnumDevices.cpp
* 
* @class      DIOWINDOWSSTREAMBLUETOOTHLOCALENUMDEVICES
* @brief      WINDOWS Data Input/Output Stream Bluetooth Local Enum Devices class
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

/*---- PRECOMPILATION INCLUDES ----------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


#if defined(DIO_ACTIVE) && (defined(DIO_STREAMBLUETOOTH_ACTIVE) || defined(DIO_STREAMBLUETOOTHLE_ACTIVE))


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOWINDOWSStreamBluetoothLocalEnumDevices.h"

#include <windows.h>
#include <iphlpapi.h>
#include <bluetoothapis.h>

#include "DIOStreamDeviceBluetooth.h"

#include "XMemory_Control.h"


#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOWINDOWSSTREAMBLUETOOTHLOCALENUMDEVICES::DIOWINDOWSSTREAMBLUETOOTHLOCALENUMDEVICES()
* @brief      Constructor
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOWINDOWSSTREAMBLUETOOTHLOCALENUMDEVICES::DIOWINDOWSSTREAMBLUETOOTHLOCALENUMDEVICES() : DIOSTREAMBLUETOOTHLOCALENUMDEVICES()
{

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOWINDOWSSTREAMBLUETOOTHLOCALENUMDEVICES::~DIOWINDOWSSTREAMBLUETOOTHLOCALENUMDEVICES()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOWINDOWSSTREAMBLUETOOTHLOCALENUMDEVICES::~DIOWINDOWSSTREAMBLUETOOTHLOCALENUMDEVICES()
{

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWINDOWSSTREAMBLUETOOTHLOCALENUMDEVICES::Search()
* @brief      Search
* @ingroup    PLATFORM_WINDOWS
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWINDOWSSTREAMBLUETOOTHLOCALENUMDEVICES::Search()
{
  DelAllDevices();

  BLUETOOTH_FIND_RADIO_PARAMS radioparams;
  HBLUETOOTH_RADIO_FIND       hfind;
  HANDLE                      hradio   = NULL;

  memset(&radioparams, 0, sizeof(radioparams));
  radioparams.dwSize = sizeof(radioparams);

  hfind = BluetoothFindFirstRadio((BLUETOOTH_FIND_RADIO_PARAMS*)&radioparams,(HANDLE*)&hradio);
  if(hfind)
    {
      BLUETOOTH_RADIO_INFO  radioinfo;
      BOOL                  foundnext;
      int                   index = 0;

      do{ memset(&radioinfo, 0, sizeof(radioinfo));
          radioinfo.dwSize =  sizeof(radioinfo);

          int wt     = 0;
          int status;

          do{ status = BluetoothGetRadioInfo(hradio, &radioinfo);
              if(status == ERROR_SUCCESS) break;

              wt++;
              if(wt>8) break;

            } while(1);

          if(status == ERROR_SUCCESS)
            {
              DIOSTREAMDEVICEBLUETOOTH* device = new DIOSTREAMDEVICEBLUETOOTH();
              if(device)
                {
                  device->SetIndex(index);

                  device->GetName()->Set((XCHAR*)radioinfo.szName);

                  XSTRING MAC;
                  MAC.Format(__L("%02X:%02X:%02X:%02X:%02X:%02X") , (XBYTE)radioinfo.address.rgBytes[5]
                                                                  , (XBYTE)radioinfo.address.rgBytes[4]
                                                                  , (XBYTE)radioinfo.address.rgBytes[3]
                                                                  , (XBYTE)radioinfo.address.rgBytes[2]
                                                                  , (XBYTE)radioinfo.address.rgBytes[1]
                                                                  , (XBYTE)radioinfo.address.rgBytes[0]);
                  device->GetMAC()->Set(MAC);

                  device->SetIsVisible(BluetoothIsDiscoverable(hradio)?true:false);

                  devices.Add(device);
                }
            }

          foundnext = BluetoothFindNextRadio(hfind,(HANDLE*)&hradio);

          if(foundnext) index++;


        } while(foundnext);

      BluetoothFindRadioClose(hfind);
    }

  return true;
}

#pragma endregion


#endif

