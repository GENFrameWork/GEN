/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOLINUXGPIOARM.cpp
* 
* @class      DIOLINUXGPIOARM
* @brief      LINUX Data Input/Output GPIO (General Purpose Input/Output) ARM class
* @ingroup    PLATFORM_LINUX
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


#if defined(HW_ARM) || defined(HW_ARM64)


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOLINUXGPIOARM.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "XFactory.h"
#include "XFile.h"
#include "XTrace.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOLINUXGPIOARM::DIOLINUXGPIOARM()
* @brief      Constructor
* @ingroup    PLATFORM_LINUX
*
* @return     Does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOLINUXGPIOARM::DIOLINUXGPIOARM() : DIOLINUXGPIO()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOLINUXGPIOARM::~DIOLINUXGPIOARM()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
*
* @return     Does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOLINUXGPIOARM::~DIOLINUXGPIOARM()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOLINUXGPIOARM::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
*
* @return     void : does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOLINUXGPIOARM::Clean()
{

}


#pragma endregion


#endif

