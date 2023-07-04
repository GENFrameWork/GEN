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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <string.h>

#include "XString.h"
#include "XTrace.h"

#include "DIOStreamDeviceUSB.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/*  DIOSTREAMDEVICEUSB_DESCRIPTOR                                                                                     */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEUSB_DESCRIPTOR::DIOSTREAMDEVICEUSB_DESCRIPTOR()
* @brief      Constructor
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEUSB_DESCRIPTOR::DIOSTREAMDEVICEUSB_DESCRIPTOR()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEUSB_DESCRIPTOR::~DIOSTREAMDEVICEUSB_DESCRIPTOR()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEUSB_DESCRIPTOR::~DIOSTREAMDEVICEUSB_DESCRIPTOR()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD DIOSTREAMDEVICEUSB_DESCRIPTOR::GetBCDUSB()
* @brief      GetBCDUSB
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
* @brief      SetBCDUSB
* @ingroup    DATAIO
*
* @param[in]  BCDUSB :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetBCDUSB(XWORD BCDUSB)
{
  this->BCDUSB = BCDUSB;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetDeviceClass()
* @brief      GetDeviceClass
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
* @brief      SetDeviceClass
* @ingroup    DATAIO
*
* @param[in]  deviceclass :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetDeviceClass(XBYTE deviceclass)
{
  this->deviceclass = deviceclass;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetDeviceSubClass()
* @brief      GetDeviceSubClass
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
* @brief      SetDeviceSubClass
* @ingroup    DATAIO
*
* @param[in]  devicesubclass :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetDeviceSubClass(XBYTE devicesubclass)
{
  this->devicesubclass = devicesubclass;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetDeviceProtocol()
* @brief      GetDeviceProtocol
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
* @brief      SetDeviceProtocol
* @ingroup    DATAIO
*
* @param[in]  deviceprotocol :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetDeviceProtocol(XBYTE deviceprotocol)
{
  this->deviceprotocol = deviceprotocol;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetMaxPacketSize()
* @brief      GetMaxPacketSize
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
* @brief      SetMaxPacketSize
* @ingroup    DATAIO
*
* @param[in]  maxpacketsize :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetMaxPacketSize(XBYTE maxpacketsize)
{
  this->maxpacketsize = maxpacketsize;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD DIOSTREAMDEVICEUSB_DESCRIPTOR::GetIDVendor()
* @brief      GetIDVendor
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
* @brief      SetIDVendor
* @ingroup    DATAIO
*
* @param[in]  IDvendor :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetIDVendor(XWORD IDvendor)
{
  this->IDvendor = IDvendor;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD DIOSTREAMDEVICEUSB_DESCRIPTOR::GetIDProduct()
* @brief      GetIDProduct
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
* @brief      SetIDProduct
* @ingroup    DATAIO
*
* @param[in]  IDproduct :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetIDProduct(XWORD IDproduct)
{
  this->IDproduct = IDproduct;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD DIOSTREAMDEVICEUSB_DESCRIPTOR::GetBCDDevice()
* @brief      GetBCDDevice
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
* @brief      SetBCDDevice
* @ingroup    DATAIO
*
* @param[in]  BCDdevice :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetBCDDevice(XWORD BCDdevice)
{
  this->BCDdevice = BCDdevice;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetManufacturer()
* @brief      GetManufacturer
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
* @brief      SetManufacturer
* @ingroup    DATAIO
*
* @param[in]  manufacturer :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetManufacturer(XBYTE manufacturer)
{
  this->manufacturer = manufacturer;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetProduct()
* @brief      GetProduct
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
* @brief      SetProduct
* @ingroup    DATAIO
*
* @param[in]  product :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetProduct(XBYTE product)
{
  this->product = product;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetSerialNumber()
* @brief      GetSerialNumber
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
* @brief      SetSerialNumber
* @ingroup    DATAIO
*
* @param[in]  serialnumber :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetSerialNumber(XBYTE serialnumber)
{
  this->serialnumber = serialnumber;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMDEVICEUSB_DESCRIPTOR::GetNumConfigurations()
* @brief      GetNumConfigurations
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
* @brief      SetNumConfigurations
* @ingroup    DATAIO
*
* @param[in]  numconfigurations :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB_DESCRIPTOR::SetNumConfigurations(XBYTE numconfigurations)
{
  this->numconfigurations = numconfigurations;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMDEVICEUSB_DESCRIPTOR::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @return     void : does not return anything.
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




/*--------------------------------------------------------------------------------------------------------------------*/
/*  DIOSTREAMDEVICEUSB                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEUSB::DIOSTREAMDEVICEUSB()
* @brief      Constructor
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEUSB::DIOSTREAMDEVICEUSB()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEUSB::~DIOSTREAMDEVICEUSB()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICEUSB::~DIOSTREAMDEVICEUSB()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMDEVICEUSB::IsHUB()
* @brief      IsHUB
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
* @brief      SetIsHUB
* @ingroup    DATAIO
*
* @param[in]  isHUB :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB::SetIsHUB(bool isHUB)
{
  this->isHUB = isHUB;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOSTREAMDEVICEUSB::GetLocation()
* @brief      GetLocation
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
* @brief      GetParentID
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
* @brief      GetVendorID
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
* @brief      SetVendorID
* @ingroup    DATAIO
*
* @param[in]  vendorID :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB::SetVendorID(XWORD vendorID)
{
  this->vendorID = vendorID;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSTREAMDEVICEUSB::GetProductID()
* @brief      GetProductID
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
* @brief      SetProductID
* @ingroup    DATAIO
*
* @param[in]  productID :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB::SetProductID(XWORD productID)
{
  this->productID = productID;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICEUSBCLASS DIOSTREAMDEVICEUSB::GetClass()
* @brief      GetClass
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
* @brief      SetClass
* @ingroup    DATAIO
*
* @param[in]  classdev :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB::SetClass(DIOSTREAMDEVICEUSBCLASS classdev)
{
  this->classdev = classdev;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOSTREAMDEVICEUSB::GetSerialString()
* @brief      GetSerialString
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
* @brief      GetDescriptor
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
* @brief      DebugPrintInfo
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
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMDEVICEUSB::Clean()
{
  isHUB           = false;
  vendorID        = 0;
  productID       = 0;
  classdev        = DIOSTREAMDEVICEUSBCLASS_DEVICE_USECLASSINFORMATIONINTERFACEDESCRIPTORS;
}


