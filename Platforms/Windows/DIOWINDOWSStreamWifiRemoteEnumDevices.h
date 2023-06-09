/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOWINDOWSStreamWifiRemoteEnumDevices.h
*
* @class      DIOWINDOWSSTREAMWIFIREMOTEENUMDEVICES
* @brief      Data Input/Output Stream WINDOWS Wi-Fi remote Enum Devices class
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

#ifndef _DIOWINDOWSSTREAMWIFIREMOTEENUMDEVICES_H_
#define _DIOWINDOWSSTREAMWIFIREMOTEENUMDEVICES_H_

#if defined(DIO_ACTIVE) && defined(DIOWIFI_ACTIVE)


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <windows.h>
#include <VersionHelpers.h>     //  Restricting to Vista+ to use API version 2
#include <Wlanapi.h>            //  Wlan functions
#include <wtypes.h>             //  Wlan types
//#include <io.h>                 //  This and fcntl.h are needed for _setmode call to allow outputting in unicode
//#include <fcntl.h>
//#include <objbase.h>


#include "DIOStream.h"
#include "DIOStreamWifiRemoteEnumDevices.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


typedef struct
{
  GUID    interfaceGUID;
  HANDLE  scanevent;
  DWORD   callbackreason;

} WLAN_CALLBACK_INFO;



/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class DIOWINDOWSSTREAMWIFIREMOTEENUMDEVICES :  public DIOSTREAMWIFIREMOTEENUMDEVICES
{
  public:
                              DIOWINDOWSSTREAMWIFIREMOTEENUMDEVICES           ();
    virtual                  ~DIOWINDOWSSTREAMWIFIREMOTEENUMDEVICES           ();

    bool                      Search                                          ();
    bool                      StopSearch                                      (bool waitend);
    bool                      IsSearching                                     ();

    bool                      SetAllConnectionInAutomaticMode                 (bool automaticmodeactive);
    bool                      SetAllConnectionStatus                          (bool activate);

  private:

    void                      TriggerScan                                     (HANDLE wlanhandle, WLAN_INTERFACE_INFO_LIST* interfaces);

    void                      Clean                                           ();

    bool                      issearching;
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif


#endif
