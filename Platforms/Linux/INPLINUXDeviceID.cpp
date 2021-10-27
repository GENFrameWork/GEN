/**-------------------------------------------------------------------------------------------------------------------
*
* @file       INPLINUXDeviceID.cpp
*
* @class      INPLINUXDEVICEID
* @brief      Linux Input device ID
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @copyright  Copyright(c) 2008 - 2016 GEN Group.
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


/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "INPLINUXDeviceID.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         INPLINUXDEVICEID::INPLINUXDEVICEID()
* @brief      Constructor
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
INPLINUXDEVICEID::INPLINUXDEVICEID()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         INPLINUXDEVICEID::~INPLINUXDEVICEID()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
INPLINUXDEVICEID::~INPLINUXDEVICEID()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         INPDEVICE_TYPE INPLINUXDEVICEID::GetType()
* @brief      GetType
* @ingroup    PLATFORM_LINUX
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @return     INPDEVICE_TYPE : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
INPDEVICE_TYPE INPLINUXDEVICEID::GetType()
{
 return type;
}

    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void INPLINUXDEVICEID::INPLINUXDEVICEID::SetType(INPDEVICE_TYPE type)
* @brief      INPLINUXDEVICEID::SetType
* @ingroup    PLATFORM_LINUX
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @param[in]  type : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void INPLINUXDEVICEID::INPLINUXDEVICEID::SetType(INPDEVICE_TYPE type)
{
  this->type = type;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* INPLINUXDEVICEID::GetName()
* @brief      Get Name
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     XSTRING* : name of device ID
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTRING* INPLINUXDEVICEID::GetName()
{
  return &name;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int INPLINUXDEVICEID::GetEventIndex()
* @brief      GetEventIndex
* @ingroup    APPLICATION
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @return     int : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
int INPLINUXDEVICEID::GetEventIndex()
{
  return eventindex;
}
    


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void INPLINUXDEVICEID::SetEventIndex(int eventindex)
* @brief      SetEventIndex
* @ingroup    APPLICATION
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @param[in]  eventindex : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void INPLINUXDEVICEID::SetEventIndex(int eventindex)
{
  this->eventindex = eventindex;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int INPLINUXDEVICEID::GetFileDescriptor()
* @brief      GetFileDescriptor
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     int : file descriptor
*
*---------------------------------------------------------------------------------------------------------------------*/
int INPLINUXDEVICEID::GetFileDescriptor()
{
  return filedescriptor;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void INPLINUXDEVICEID::SetFileDescriptor(int filedescriptor)
* @brief      SetFileDescriptor
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  filedescriptor : new file descriptor
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void INPLINUXDEVICEID::SetFileDescriptor(int filedescriptor)
{
  this->filedescriptor = filedescriptor;
}





/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void INPLINUXDEVICEID::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void INPLINUXDEVICEID::Clean()
{
  type            = INPDEVICE_TYPE_NONE;  
  eventindex      = INPLINUXDEVICEID_INVALID;
  filedescriptor  = INPLINUXDEVICEID_INVALID;
}

