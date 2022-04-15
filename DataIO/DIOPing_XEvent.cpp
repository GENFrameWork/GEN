/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOPing_XEvent.cpp
*
* @class      DIOPING_XEVENT
* @brief      Data Input/Output Ping eXtended Event class
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DIOPing_XEvent.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPING_XEVENT::DIOPING_XEVENT(XSUBJECT* subject, XDWORD type, XDWORD family)
* @brief      DIOPING_XEVENT
* @ingroup    DATAIO
*
* @param[in]  subject :
* @param[in]  type :
* @param[in]  family :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOPING_XEVENT::DIOPING_XEVENT(XSUBJECT* subject, XDWORD type, XDWORD family): XEVENT(subject, type, family)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPING_XEVENT::DIOPING_XEVENT::~DIOPING_XEVENT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOPING_XEVENT::~DIOPING_XEVENT()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPINGREPLY* DIOPING_XEVENT::GetPingReply()
* @brief      GetPingReply
* @ingroup    DATAIO
*
* @return     DIOPINGREPLY* :
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOPINGREPLY* DIOPING_XEVENT::GetPingReply()
{
  return pingreply;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPING_XEVENT::SetPingReply(DIOPINGREPLY* pingreply)
* @brief      SetPingReply
* @ingroup    DATAIO
*
* @param[in]  pingreply :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void DIOPING_XEVENT::SetPingReply(DIOPINGREPLY* pingreply)
{
  this->pingreply = pingreply;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPING_XEVENT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void DIOPING_XEVENT::Clean()
{
  pingreply = NULL;
}


