/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamDeviceUSB.cpp
* 
* @class      DIOSTREAMDEVICEUSB
* @brief      Data Input/Output Stream Device USB class
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

#include "DIOStreamDeviceUSB.h"

#include <string.h>

#include "XString.h"
#include "XTrace.h"

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


#pragma region CLASS_DIOSTREAMDEVICEUSB_DESCRIPTOR


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEUSB_DESCRIPTOR::DIOSTREAMDEVICEUSB_DESCRIPTOR()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEUSB_DESCRIPTOR::DIOSTREAMDEVICEUSB_DESCRIPTOR()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEUSB_DESCRIPTOR::~DIOSTREAMDEVICEUSB_DESCRIPTOR()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEUSB_DESCRIPTOR::~DIOSTREAMDEVICEUSB_DESCRIPTOR()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD DIOSTREAMDEVICEUSB_DESCRIPTOR::GetBCDUSB()
* @brief      Get BCDUSB
* @ingroup    DATAIO
*
* @return     XWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMDEVICEUSB_DESCRIPTOR::GetBCDUSB()
{
  return BCDUSB;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetBCDUSB(XWORD BCDUSB)
* @brief      Set BCDUSB
* @ingroup    DATAIO
*
* @param[in]  BCDUSB :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetBCDUSB(XWORD BCDUSB)
{
  this->BCDUSB = BCDUSB;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetDeviceClass()
* @brief      Get device class
* @ingroup    DATAIO
*
* @return     XBYTE :
*
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetDeviceClass()
{
  return deviceclass;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetDeviceClass(XBYTE deviceclass)
* @brief      Set device class
* @ingroup    DATAIO
*
* @param[in]  deviceclass :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetDeviceClass(XBYTE deviceclass)
{
  this->deviceclass = deviceclass;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetDeviceSubClass()
* @brief      Get device sub class
* @ingroup    DATAIO
*
* @return     XBYTE :
*
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetDeviceSubClass()
{
  return devicesubclass;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetDeviceSubClass(XBYTE devicesubclass)
* @brief      Set device sub class
* @ingroup    DATAIO
*
* @param[in]  devicesubclass :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetDeviceSubClass(XBYTE devicesubclass)
{
  this->devicesubclass = devicesubclass;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetDeviceProtocol()
* @brief      Get device protocol
* @ingroup    DATAIO
*
* @return     XBYTE :
*
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetDeviceProtocol()
{
  return deviceprotocol;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetDeviceProtocol(XBYTE deviceprotocol)
* @brief      Set device protocol
* @ingroup    DATAIO
*
* @param[in]  deviceprotocol :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetDeviceProtocol(XBYTE deviceprotocol)
{
  this->deviceprotocol = deviceprotocol;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetMaxPacketSize()
* @brief      Get max packet size
* @ingroup    DATAIO
*
* @return     XBYTE :
*
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetMaxPacketSize()
{
  return maxpacketsize;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetMaxPacketSize(XBYTE maxpacketsize)
* @brief      Set max packet size
* @ingroup    DATAIO
*
* @param[in]  maxpacketsize :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetMaxPacketSize(XBYTE maxpacketsize)
{
  this->maxpacketsize = maxpacketsize;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD DIOSTREAMDEVICEUSB_DESCRIPTOR::GetIDVendor()
* @brief      Get ID vendor
* @ingroup    DATAIO
*
* @return     XWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMDEVICEUSB_DESCRIPTOR::GetIDVendor()
{
  return IDvendor;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetIDVendor(XWORD IDvendor)
* @brief      Set ID vendor
* @ingroup    DATAIO
*
* @param[in]  IDvendor :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetIDVendor(XWORD IDvendor)
{
  this->IDvendor = IDvendor;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD DIOSTREAMDEVICEUSB_DESCRIPTOR::GetIDProduct()
* @brief      Get ID product
* @ingroup    DATAIO
*
* @return     XWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMDEVICEUSB_DESCRIPTOR::GetIDProduct()
{
  return IDproduct;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetIDProduct(XWORD IDproduct)
* @brief      Set ID product
* @ingroup    DATAIO
*
* @param[in]  IDproduct :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetIDProduct(XWORD IDproduct)
{
  this->IDproduct = IDproduct;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD DIOSTREAMDEVICEUSB_DESCRIPTOR::GetBCDDevice()
* @brief      Get BCD device
* @ingroup    DATAIO
*
* @return     XWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMDEVICEUSB_DESCRIPTOR::GetBCDDevice()
{
  return BCDdevice;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetBCDDevice(XWORD BCDdevice)
* @brief      Set BCD device
* @ingroup    DATAIO
*
* @param[in]  BCDdevice :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetBCDDevice(XWORD BCDdevice)
{
  this->BCDdevice = BCDdevice;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetManufacturer()
* @brief      Get manufacturer
* @ingroup    DATAIO
*
* @return     XBYTE :
*
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetManufacturer()
{
  return manufacturer;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetManufacturer(XBYTE manufacturer)
* @brief      Set manufacturer
* @ingroup    DATAIO
*
* @param[in]  manufacturer :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetManufacturer(XBYTE manufacturer)
{
  this->manufacturer = manufacturer;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetProduct()
* @brief      Get product
* @ingroup    DATAIO
*
* @return     XBYTE :
*
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetProduct()
{
  return product;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetProduct(XBYTE product)
* @brief      Set product
* @ingroup    DATAIO
*
* @param[in]  product :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetProduct(XBYTE product)
{
  this->product = product;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetSerialNumber()
* @brief      Get serial number
* @ingroup    DATAIO
*
* @return     XBYTE :
*
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetSerialNumber()
{
  return serialnumber;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetSerialNumber(XBYTE serialnumber)
* @brief      Set serial number
* @ingroup    DATAIO
*
* @param[in]  serialnumber :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetSerialNumber(XBYTE serialnumber)
{
  this->serialnumber = serialnumber;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetNumConfigurations()
* @brief      Get num configurations
* @ingroup    DATAIO
*
* @return     XBYTE :
*
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetNumConfigurations()
{
  return numconfigurations;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetNumConfigurations(XBYTE numconfigurations)
* @brief      Set num configurations
* @ingroup    DATAIO
*
* @param[in]  numconfigurations :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetNumConfigurations(XBYTE numconfigurations)
{
  this->numconfigurations = numconfigurations;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB_DESCRIPTOR::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::Clean()
{
  BCDUSB            = 0;
  deviceclass       = 0;
  devicesubclass    = 0;
  deviceprotocol    = 0;
  maxpacketsize     = 0;
  IDvendor          = 0;
  IDproduct         = 0;
  BCDdevice         = 0;
  manufacturer      = 0;
  product           = 0;
  serialnumber      = 0;
  numconfigurations = 0;
}


#pragma endregion


#pragma region CLASS_DIOSTREAMDEVICEUSB


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEUSB::DIOSTREAMDEVICEUSB()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEUSB::DIOSTREAMDEVICEUSB()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEUSB::~DIOSTREAMDEVICEUSB()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEUSB::~DIOSTREAMDEVICEUSB()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMDEVICEUSB::IsHUB()
* @brief      Is HUB
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMDEVICEUSB::IsHUB()
{
  return isHUB;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB::SetIsHUB(bool isHUB)
* @brief      Set is HUB
* @ingroup    DATAIO
*
* @param[in]  isHUB :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB::SetIsHUB(bool isHUB)
{
  this->isHUB = isHUB;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOSTREAMDEVICEUSB::GetLocation()
* @brief      Get location
* @ingroup    DATAIO
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOSTREAMDEVICEUSB::GetLocation()
{
  return &location;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOSTREAMDEVICEUSB::GetParentID()
* @brief      Get parent ID
* @ingroup    DATAIO
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOSTREAMDEVICEUSB::GetParentID()
{
  return &parentID;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD DIOSTREAMDEVICEUSB::GetVendorID()
* @brief      Get vendor ID
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMDEVICEUSB::GetVendorID()
{
  return vendorID;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB::SetVendorID(XDWORD vendorID)
* @brief      Set vendor ID
* @ingroup    DATAIO
*
* @param[in]  vendorID :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB::SetVendorID(XWORD vendorID)
{
  this->vendorID = vendorID;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSTREAMDEVICEUSB::GetProductID()
* @brief      Get product ID
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMDEVICEUSB::GetProductID()
{
  return productID;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB::SetProductID(XWORD productID)
* @brief      Set product ID
* @ingroup    DATAIO
*
* @param[in]  productID :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB::SetProductID(XWORD productID)
{
  this->productID = productID;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEUSBCLASS DIOSTREAMDEVICEUSB::GetClass()
* @brief      Get class
* @ingroup    DATAIO
*
* @return     DIOSTREAMDEVICEUSBCLASS :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEUSBCLASS DIOSTREAMDEVICEUSB::GetClass()
{
  return classdev;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB::SetClass(DIOSTREAMDEVICEUSBCLASS classdev)
* @brief      Set class
* @ingroup    DATAIO
*
* @param[in]  classdev :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB::SetClass(DIOSTREAMDEVICEUSBCLASS classdev)
{
  this->classdev = classdev;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOSTREAMDEVICEUSB::GetSerialString()
* @brief      Get serial string
* @ingroup    DATAIO
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOSTREAMDEVICEUSB::GetSerialString()
{
  return &serialstring;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEUSB_DESCRIPTOR* DIOSTREAMDEVICEUSB::GetDescriptor()
* @brief      Get descriptor
* @ingroup    DATAIO
*
* @return     DIOSTREAMDEVICEUSB_DESCRIPTOR* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEUSB_DESCRIPTOR* DIOSTREAMDEVICEUSB::GetDescriptor()
{
  return &descriptor;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMDEVICEUSB::DebugPrintInfo()
* @brief      Debug print info
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMDEVICEUSB::DebugPrintInfo()
{
  #ifdef XTRACE_ACTIVE

  DIOSTREAMDEVICE::DebugPrintInfo();

  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Location         : %s")    , location.Get());
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Vendor ID        : %08X")  , vendorID);
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Product ID       : %08X")  , productID);
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Class            : %02x")  , classdev);
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Serial           : %s")    , serialstring.Get());

  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("BCD USB          : %08X")  , descriptor.GetBCDUSB());
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Device class     : %d")    , descriptor.GetDeviceClass());
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Device subclass  : %d")    , descriptor.GetDeviceSubClass());
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Device protocol  : %d")    , descriptor.GetDeviceProtocol());
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Max Packet Size  : %d")    , descriptor.GetMaxPacketSize());
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("id Vendor        : %d")    , descriptor.GetIDVendor());
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("id Product       : %d")    , descriptor.GetIDProduct());
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("BCD device       : %04X")  , descriptor.GetBCDDevice());
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Manufacturer     : %d")    , descriptor.GetManufacturer());
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Product          : %d")    , descriptor.GetProduct());
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Serial Number    : %d")    , descriptor.GetSerialNumber());
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("NConfigurations  : %d")    , descriptor.GetNumConfigurations());

  return true;

  #else

  return false;

  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB::Clean()
{
  isHUB           = false;
  vendorID        = 0;
  productID       = 0;
  classdev        = DIOSTREAMDEVICEUSBCLASS_DEVICE_USECLASSINFORMATIONINTERFACEDESCRIPTORS;
}


#pragma endregion


#pragma endregion

