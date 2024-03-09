/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamIPLocalEnumDevices.cpp
* 
* @class      DIOSTREAMIPLOCALENUMDEVICES
* @brief      Data Input/Output Stream IP Local Enum Devices class
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

#include "DIOStreamIPLocalEnumDevices.h"

#include "DIOStreamEnumDevices.h"
#include "DIOStreamDeviceIP.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMIPLOCALENUMDEVICES::DIOSTREAMIPLOCALENUMDEVICES()
* @brief      Constructor
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMIPLOCALENUMDEVICES::DIOSTREAMIPLOCALENUMDEVICES(): DIOSTREAMENUMDEVICES()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMIPLOCALENUMDEVICES::~DIOSTREAMIPLOCALENUMDEVICES()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMIPLOCALENUMDEVICES::~DIOSTREAMIPLOCALENUMDEVICES()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMIPLOCALENUMDEVICES::Search()
* @brief      Search
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMIPLOCALENUMDEVICES::Search()
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMIPLOCALENUMDEVICES::StopSearch(bool waitend)
* @brief      StopSearch
* @ingroup    DATAIO
*
* @param[in]  waitend :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMIPLOCALENUMDEVICES::StopSearch(bool waitend)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMIPLOCALENUMDEVICES::IsSearching()
* @brief      IsSearching
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMIPLOCALENUMDEVICES::IsSearching()
{
  return issearching;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMIPLOCALENUMDEVICES::IsDeviceAvailable(XCHAR* resource)
* @brief      IsDeviceAvailable
* @ingroup    DATAIO
*
* @param[in]  resource :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMIPLOCALENUMDEVICES::IsDeviceAvailable(XCHAR* resource)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMDEVICE* DIOSTREAMIPLOCALENUMDEVICES::GetFirstActiveDevice()
* @brief      GetFirstActiveDevice
* @ingroup    DATAIO
*
* @return     DIOSTREAMDEVICE* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICE* DIOSTREAMIPLOCALENUMDEVICES::GetFirstActiveDevice()
{
  Search();

  for(int c=0;c<(int)GetDevices()->GetSize();c++)
    {
      DIOSTREAMDEVICEIP* device = (DIOSTREAMDEVICEIP*)GetDevices()->Get(c);
      if(device)
        {
          if((!device->GetIP()->IsEmpty()) && 
             (!device->GetIP()->IsAPIPA()) &&
             ((device->GetIPType() == DIOSTREAMIPDEVICE_TYPE_ETHERNET) ||
              (device->GetIPType() == DIOSTREAMIPDEVICE_TYPE_WIFI)     ||
              (device->GetIPType() == DIOSTREAMIPDEVICE_TYPE_PPP)))
            {
              return device;
            }
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMIPLOCALENUMDEVICES::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMIPLOCALENUMDEVICES::Clean()
{
  type        = DIOSTREAMENUMTYPE_IP_LOCAL;
  issearching = false;
}


#pragma endregion

