/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamBluetoothLERemoteEnumDevices.h
* 
* @class      DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES
* @brief      Data Input/Output Stream Bluetooth LE Eemote Enum Devices class
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

#ifndef _DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_H_
#define _DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XFSMachine.h"
#include "DIOStreamEnumBluetoothDevices.h"
#include "DIOStreamDeviceBluetoothLE.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

enum DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_SEARCHMODE
{
  DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_SEARCHMODE_NONE         = 0x00  ,
  DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_SEARCHMODE_ADDLIST      = 0x01  ,
  DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES_SEARCHMODE_EVENT        = 0x02  ,
};

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES :  public DIOSTREAMENUMBLUETOOTHDEVICES
{
  public:
                            DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES     ();
    virtual                ~DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES     ();

    virtual bool            Search                                    ();
    virtual bool            StopSearch                                (bool waitend);
    virtual bool            IsSearching                               ();

    virtual bool            IsDeviceAvailable                         (XCHAR* resource);

    XDWORD                  GetSearchMode                             ();
    void                    SetSearchMode                             (XDWORD searchmode);

  private:

    void                    Clean                                     ();

    XDWORD                  searchmode;
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

