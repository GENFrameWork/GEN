/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamUSBLocalEnumDevices.h
*
* @class      DIOSTREAMUSBLOCALENUMDEVICES
* @brief      Data Input/Output Stream USB local enum class
* @ingroup    DATAIO
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

#ifndef _DIOSTREAMUSBLOCALENUMDEVICES_H_
#define _DIOSTREAMUSBLOCALENUMDEVICES_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DIOStreamUSBConfig.h"
#include "DIOStreamDeviceUSB.h"

#include "DIOStreamEnumDevices.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define DIOSTREAMUSBLOCALENUMDEVICES_PORTKEY  __L("Port_")
#define DIOSTREAMUSBLOCALENUMDEVICES_PORTHUB  __L("Hub_")

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class DIOSTREAMUSBLOCALENUMDEVICES : public DIOSTREAMENUMDEVICES
{
  public:
                                DIOSTREAMUSBLOCALENUMDEVICES        ();
    virtual                    ~DIOSTREAMUSBLOCALENUMDEVICES        ();

    virtual bool                Search                              ();
    virtual bool                StopSearch                          (bool waitend);
    bool                        IsSearching                         ();

    virtual bool                IsDeviceAvailable                   (XCHAR* resource);

    DIOSTREAMDEVICEUSB*         FindDevice                          (XCHAR* location);
    DIOSTREAMDEVICEUSB*         FindDevice                          (XSTRING& location);

    DIOSTREAMDEVICEUSB*         FindDevice                          (XCHAR* location, DIOSTREAMDEVICEUSBCLASS classdev);
    DIOSTREAMDEVICEUSB*         FindDevice                          (XSTRING& location, DIOSTREAMDEVICEUSBCLASS classdev);

    DIOSTREAMDEVICEUSB*         FindDevice                          (XDWORD vendorID, XDWORD productID);
    DIOSTREAMDEVICEUSB*         FindDevice                          (XDWORD vendorID, XDWORD productID, DIOSTREAMDEVICEUSBCLASS classdev);
    DIOSTREAMDEVICEUSB*         FindDevice                          (DIOSTREAMUSBCONFIG& config);

  private:

    void                        Clean                               ();

    bool                        issearching;
};



/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif



