/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOLINUXDBus_XEvent.cpp
*
* @class      DIOLINUXDBUS_XEVENT
* @brief      Data Input/Output Linux DBus eXtended Event class
* @ingroup    DATAIO
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

#include "XString.h"
#include "XPath.h"
#include "XEvent.h"

#include "DIOLINUXDBus_XEvent.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOLINUXDBUS_XEVENT::DIOLINUXDBUS_XEVENT(XSUBJECT* subject, XDWORD type)
* @brief      Constructor
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  subject :
* @param[in]  type :
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOLINUXDBUS_XEVENT::DIOLINUXDBUS_XEVENT(XSUBJECT* subject, XDWORD type)  : XEVENT(subject, type, XEVENT_TYPE_LINUX_DBUS)
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOLINUXDBUS_XEVENT::~DIOLINUXDBUS_XEVENT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOLINUXDBUS_XEVENT::~DIOLINUXDBUS_XEVENT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOLINUXDBUS_SIGNAL* DIOLINUXDBUS_XEVENT::GetSignal()
* @brief      GetSignal
* @ingroup    PLATFORM_LINUX
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @return     DIOLINUXDBUS_SIGNAL* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOLINUXDBUS_SIGNAL* DIOLINUXDBUS_XEVENT::GetSignal()
{
  return signal;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOLINUXDBUS_XEVENT::SetSignal(DIOLINUXDBUS_SIGNAL* signal)
* @brief      SetSignal
* @ingroup    PLATFORM_LINUX
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @param[in]  signal : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOLINUXDBUS_XEVENT::SetSignal(DIOLINUXDBUS_SIGNAL* signal)
{
  this->signal = signal;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOLINUXDBUS_MESSAGE* DIOLINUXDBUS_XEVENT::GetMessage()
* @brief      GetMessage
* @ingroup    PLATFORM_LINUX
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @return     DIOLINUXDBUS_MESSAGE* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOLINUXDBUS_MESSAGE* DIOLINUXDBUS_XEVENT::GetMessage()
{
  return message;
}
    


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOLINUXDBUS_XEVENT::SetMessage(DIOLINUXDBUS_MESSAGE* message)
* @brief      SetMessage
* @ingroup    PLATFORM_LINUX
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @param[in]  message : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOLINUXDBUS_XEVENT::SetMessage(DIOLINUXDBUS_MESSAGE* message)
{
  this->message = message;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOLINUXDBUS_XEVENT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void DIOLINUXDBUS_XEVENT::Clean()
{  
  signal  = NULL;
  message = NULL;
}



