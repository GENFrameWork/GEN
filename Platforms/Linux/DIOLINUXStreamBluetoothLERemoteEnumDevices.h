/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOLINUXStreamBluetoothLERemoteEnumDevices.h
*
* @class      DIOLINUXSTREAMBLUETOOTHLEREMOTEENUMDEVICES
* @brief      LINUX Data Input/Output Stream Bluetooth Remote Enum Devices class
* @ingroup    PLATFORM_LINUX
*
* @copyright  GEN Group. All right reserved.
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
*---------------------------------------------------------------------------------------------------------------------*/

#ifndef _DIOLINUXSTREAMBLUETOOTHLEREMOTEENUMDEVICES_H_
#define _DIOLINUXSTREAMBLUETOOTHLEREMOTEENUMDEVICES_H_


#if defined(DIO_ACTIVE) && defined(DIOBLUETOOTHLE_ACTIVE)


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>


#include "DIOStreamBluetoothLERemoteEnumDevices.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define DIOLINUXSTREAMBLUETOOTHLEREMOTEENUMDEVICES_MAXLIST  1024

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XTHREADCOLLECTED;
class XMUTEX;
class DIOSTREAMDEVICEBLUETOOTH;

class DIOLINUXSTREAMBLUETOOTHLEREMOTEENUMDEVICES :  public DIOSTREAMBLUETOOTHLEREMOTEENUMDEVICES
{
  public:
                                    DIOLINUXSTREAMBLUETOOTHLEREMOTEENUMDEVICES      ();
    virtual                        ~DIOLINUXSTREAMBLUETOOTHLEREMOTEENUMDEVICES      ();

    bool                            Search                                          ();
    bool                            StopSearch                                      (bool waitend);
    bool                            IsSearching                                     ();

  private:

    static void                     ThreadEnumDevices                               (void* thread);
    static void                     ThreadEnumDevicesEvent                          (void* thread);

    void                            Clean                                           ();
    
    XTHREADCOLLECTED*               threadenumdevices;  
    XTHREADCOLLECTED*               threadenumdevicesevent;  

    int                             hcideviceID;
    int                             hcisocket;
    struct hci_dev_info             hcidevinfo;
    struct hci_filter               old_hcifilter;
    struct hci_filter               new_hcifilter;

    XMUTEX*                         xmutexdevicesevent;                                    
    XVECTOR<DIOSTREAMDEVICE*>       devicesevent;         
};


/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif


#endif

