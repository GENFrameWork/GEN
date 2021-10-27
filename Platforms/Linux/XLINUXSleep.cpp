/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XLINUXSleep.cpp
*
* @class      XLINUXSLEEP
* @brief      eXtended LINUX Sleep class
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>

#include "XTrace.h"
#include "XLINUXFile.h"

#include "XLINUXSleep.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XLINUXSLEEP::XLINUXSLEEP()
* @brief      Constructor
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XLINUXSLEEP::XLINUXSLEEP()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XLINUXSLEEP::~XLINUXSLEEP()
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
XLINUXSLEEP::~XLINUXSLEEP()

{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XLINUXSLEEP::Seconds(int seconds)
* @brief      Sleep n Seconds
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  seconds : n seconds to sleep
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XLINUXSLEEP::Seconds(int seconds)
{
  sleep(seconds);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XLINUXSLEEP::MilliSeconds(int milliseconds)
* @brief      Sleep n MilliSeconds
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  milliseconds : n milliseconds to sleep
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XLINUXSLEEP::MilliSeconds(int milliseconds)
{
  usleep(milliseconds*1000);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XLINUXSLEEP::MicroSeconds(int microseconds)
* @brief      Sleep n MicroSeconds
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  microseconds : n microseconds to sleep
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XLINUXSLEEP::MicroSeconds(int microseconds)
{
  usleep(microseconds);
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XLINUXSLEEP::NanoSeconds(int nanoseconds)
* @brief      Sleep n NanoSeconds
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  nanoseconds : n nanoseconds to sleep
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XLINUXSLEEP::NanoSeconds(int nanoseconds)
{
  struct timespec time = { 0, nanoseconds };
  struct timespec time2;

  nanosleep(&time , &time2);

  /*
  struct timespec delay = { nanoseconds / 1000000000, nanoseconds % 1000000000 };

  pselect(0, NULL, NULL, NULL, &delay, NULL);
  */
}





/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XLINUXSLEEP::Clean()
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
void XLINUXSLEEP::Clean()
{

}
