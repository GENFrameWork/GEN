/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XANDROIDTrace.cpp
*
* @class      XANDROIDTRACE
* @brief      ANDROID debug trace class
* @ingroup    PLATFORM_ANDROID
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


#ifdef XTRACE_ACTIVE

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <android/log.h>

#include "XANDROIDTrace.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XANDROIDTRACE::XANDROIDTRACE()
* @brief      Constructor
* @ingroup    PLATFORM_ANDROID
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XANDROIDTRACE::XANDROIDTRACE()
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XANDROIDTRACE::~XANDROIDTRACE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_ANDROID
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XANDROIDTRACE::~XANDROIDTRACE()
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XANDROIDTRACE::PrintSpecial(XDEBUGCTRLTARGET* target, XBYTE level, XCHAR* string)
* @brief      PrintSpecial
* @ingroup    PLATFORM_ANDROID
*
* @param[in]  target : target to print the debug
* @param[in]  level : level of debug
* @param[in]  string : string with the line
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XANDROIDTRACE::PrintSpecial(XTRACE_TARGET* target, XBYTE level, XCHAR* string)
{
  if(!target)              return;
  if(!target->GetAim()[0]) return;

  Lock();

  XSTRING line;
  line = string;
  
  XBUFFER charstr;
  
  line.ConvertToASCII(charstr);
  
  switch(level)
    {
      case  0 : XANDROIDDEBUG_LOG_INFO((const char*)charstr.GetPtrChar());      break;
      case  1 : XANDROIDDEBUG_LOG_ERROR((const char*)charstr.GetPtrChar());     break;
      case  2 : XANDROIDDEBUG_LOG_WARNING((const char*)charstr.GetPtrChar());   break;
      case  3 :
      default : XANDROIDDEBUG_LOG_DEBUG((char*)charstr.GetPtrChar());           break;
    }
  
  UnLock();
}




#endif


