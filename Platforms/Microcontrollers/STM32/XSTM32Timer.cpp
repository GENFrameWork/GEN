/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XSTM32Timer.cpp
*
* @class      XSTM32TIMER
* @brief      eXtended utils STM32 timer class
* @ingroup    PLATFORM_STM32
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

#include "XSTM32_HAL.h"

#include "XSTM32Timer.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTM32TIMER::XSTM32TIMER()
* @brief      Constructor
* @ingroup    PLATFORM_STM32
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTM32TIMER::XSTM32TIMER()
{
  Reset();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTM32TIMER::~XSTM32TIMER()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_STM32
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTM32TIMER::~XSTM32TIMER()
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XQWORD XSTM32TIMER::GetMicroSecondsTickCounter()
* @brief      GetMicroSecondsTickCounter
* @ingroup    PLATFORM_STM32
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     XQWORD :
*
*---------------------------------------------------------------------------------------------------------------------*/
XQWORD XSTM32TIMER::GetMicroSecondsTickCounter()
{
  XQWORD elapsedtime = HAL_GetTick();

  elapsedtime *= 1000;  // Pass to MicroSeconds

  return elapsedtime;
}

