/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XObserver.cpp
*
* @class      XOBSERVER
* @brief      eXtended Observerd class
* @ingroup    UTILS
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

#include "XObserver.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



 /**-------------------------------------------------------------------------------------------------------------------
 *
 *  @fn         XOBSERVER::XOBSERVER()
 *  @brief      Constructor
 *  @ingroup    UTILS
 *
 *  @author     Abraham J. Velez
 *  @date       01/03/2016 12:00
 *
 *  @return     Does not return anything.
 *
 *---------------------------------------------------------------------------------------------------------------------*/
XOBSERVER::XOBSERVER()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XOBSERVER::~XOBSERVER()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XOBSERVER::~XOBSERVER()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XOBSERVER::SubscribeEvent(XDWORD type, XSUBJECT* subject, XOBSERVER* observer)
* @brief      SubscribeEvent
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  type :
* @param[in]  subject :
* @param[in]  observer :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XOBSERVER::SubscribeEvent(XDWORD type, XSUBJECT* subject, XOBSERVER* observer)
{
  return GEN_XPUBLISHER.SubscribeEvent(type, subject, (observer?observer:this));
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XOBSERVER::UnSubscribeEvent(XDWORD type, XSUBJECT* subject, XOBSERVER* observer)
* @brief      UnSubscribeEvent
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  type :
* @param[in]  subject :
* @param[in]  observer :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XOBSERVER::UnSubscribeEvent(XDWORD type, XSUBJECT* subject, XOBSERVER* observer)
{
  return GEN_XPUBLISHER.UnSubscribeEvent(type, subject, (observer?observer:this));
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XOBSERVER::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XOBSERVER::Clean()
{

}
