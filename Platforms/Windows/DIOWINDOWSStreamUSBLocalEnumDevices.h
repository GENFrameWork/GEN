/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOWINDOWSStreamUSBLocalEnumDevices.h
*
* @class      DIOWINDOWSSTREAMUSBLOCALENUMDEVICES
* @brief      Data Input/Output Stream WINDOWS USB Local Enum Devices class
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

#ifndef _DIOWINDOWSSTREAMUSBLOCALENUMDEVICES_H_
#define _DIOWINDOWSSTREAMUSBLOCALENUMDEVICES_H_

#if defined(DIO_ACTIVE) && defined(DIOUSB_ACTIVE)

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <SetupAPI.h>

#include "XString.h"

#include "DIOStreamDeviceUSB.h"
#include "DIOStreamUSBLocalEnumDevices.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class DIOWINDOWSSTREAMUSBLOCALENUMDEVICES :  public DIOSTREAMUSBLOCALENUMDEVICES
{
  public:
                                                    DIOWINDOWSSTREAMUSBLOCALENUMDEVICES         ();
    virtual                                        ~DIOWINDOWSSTREAMUSBLOCALENUMDEVICES         ();

    bool                                            Search                                      ();

  private:


    bool                                            SearchGUID                                  (const GUID* interfaceclassguid);

    bool                                            GetInfoFromRegistry                         (DIOSTREAMDEVICEUSB* device);
    bool                                            GetUSBDescriptor                            (HDEVINFO& hdevinfo, int menberindex, DIOSTREAMDEVICEUSB* device);

    bool                                            SearchAndDeleteEqualDevices                 ();

    void                                            Clean                                       ();
};

/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif

#endif




