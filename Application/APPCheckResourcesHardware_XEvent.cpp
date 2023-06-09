/**-------------------------------------------------------------------------------------------------------------------
*
* @file       APPCheckResourcesHardware_XEvent.cpp
*
* @class      APPCHECKRESOURCESHARDWARE_XEVENT
* @brief      Application Check Resources Hardware eXtended Event class
* @ingroup    APPLICATION
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

#include "APPCheckResourcesHardware_XEvent.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         APPCHECKRESOURCESHARDWARE_XEVENT::APPCHECKRESOURCESHARDWARE_XEVENT(XSUBJECT* subject, XDWORD type, XDWORD family)
* @brief      Constructor
* @ingroup    XUTILS
*
* @param[in]  subject :
* @param[in]  type :
* @param[in]  family :
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
APPCHECKRESOURCESHARDWARE_XEVENT::APPCHECKRESOURCESHARDWARE_XEVENT(XSUBJECT* subject, XDWORD type, XDWORD family) : XEVENT(subject, type, family)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         APPCHECKRESOURCESHARDWARE_XEVENT::~APPCHECKRESOURCESHARDWARE_XEVENT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
APPCHECKRESOURCESHARDWARE_XEVENT::~APPCHECKRESOURCESHARDWARE_XEVENT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPCHECKRESOURCESHARDWARE_XEVENT::GetActualMemFree(XDWORD& memfree_inbytes, XBYTE& memfree_percent)
* @brief      GetActualMemFree
* @ingroup    APPLICATION
*
* @param[in]  memfree_inbytes : 
* @param[in]  memfree_percent : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool APPCHECKRESOURCESHARDWARE_XEVENT::GetActualMemFree(XDWORD& memfree_inbytes, XBYTE& memfree_percent)
{
  memfree_inbytes = this->memfree_inbytes; 
  memfree_percent = this->memfree_percent;

  return true;
}
    

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void APPCHECKRESOURCESHARDWARE_XEVENT::SetActualMemFree(XDWORD memfree_inbytes, XBYTE memfree_percent)
* @brief      SetActualMemFree
* @ingroup    APPLICATION
*
* @param[in]  memfree_inbytes : 
* @param[in]  memfree_percent : 
*
* @return     void : does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
void APPCHECKRESOURCESHARDWARE_XEVENT::SetActualMemFree(XDWORD memfree_inbytes, XBYTE memfree_percent)
{
  this->memfree_inbytes = memfree_inbytes; 
  this->memfree_percent = memfree_percent;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int APPCHECKRESOURCESHARDWARE_XEVENT::GetActualCPUUsage()
* @brief      GetActualCPUUsage
* @ingroup    APPLICATION
*
* @return     int : 
*
* --------------------------------------------------------------------------------------------------------------------*/
int APPCHECKRESOURCESHARDWARE_XEVENT::GetActualCPUUsage()
{
  return CPUusage;
}
 

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void APPCHECKRESOURCESHARDWARE_XEVENT::SetActualCPUUsage(int& CPUusage)
* @brief      SetActualCPUUsage
* @ingroup    APPLICATION
*
* @param[in]  CPUusage : 
*
* @return     void : does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
void APPCHECKRESOURCESHARDWARE_XEVENT::SetActualCPUUsage(int& CPUusage)
{
  this->CPUusage = CPUusage;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void APPCHECKRESOURCESHARDWARE_XEVENT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void APPCHECKRESOURCESHARDWARE_XEVENT::Clean()
{
  memfree_inbytes   = 0;
  memfree_percent   = 0; 
  CPUusage          = 0; 
}
