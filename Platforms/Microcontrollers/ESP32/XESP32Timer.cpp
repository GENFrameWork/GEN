/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XESP32Timer.cpp
* 
* @class      XESP32TIMER
* @brief      eXtended utils ESP32 timer class
* @ingroup    PLATFORM_ESP32
* 
* @copyright  EndoraSoft. All rights reserved.
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

#include "GEN_Defines.h"



/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XESP32Timer.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XESP32TIMER::XESP32TIMER()
* @brief      Constructor of class
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
XESP32TIMER::XESP32TIMER()
{
  Reset();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XESP32TIMER::~XESP32TIMER()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_ESP32
*
* --------------------------------------------------------------------------------------------------------------------*/
XESP32TIMER::~XESP32TIMER()
{

}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XQWORD XESP32TIMER::GetMicroSecondsTickCounter()
* @brief      Get micro seconds tick counter
* @ingroup    PLATFORM_ESP32
*
* @return     XQWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XQWORD XESP32TIMER::GetMicroSecondsTickCounter()
{

  return 0;
}










