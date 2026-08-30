/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XWINDOWSRand.cpp
* 
* @class      XWINDOWSRAND
* @brief      WINDOWS eXtended Utils Rand class
* @ingroup    PLATFORM_WINDOWS
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

#include "XWINDOWSRand.h"

#include <windows.h>
#include <bcrypt.h>
#include <string.h>



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWINDOWSRAND::XWINDOWSRAND()
* @brief      Constructor of class
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWINDOWSRAND::XWINDOWSRAND()
{

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWINDOWSRAND::~XWINDOWSRAND()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWINDOWSRAND::~XWINDOWSRAND()
{

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XWINDOWSRAND::Ini()
* @brief      Initialize the object
* @ingroup    PLATFORM_WINDOWS
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSRAND::Ini()
{
  XBYTE data = 0;

  return Generate(&data, sizeof(data));
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XWINDOWSRAND::IsCryptographicallySecure()
* @brief      Report whether the platform provider is cryptographically secure
* @ingroup    PLATFORM_WINDOWS
* 
* @return     bool : true.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSRAND::IsCryptographicallySecure()
{
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XWINDOWSRAND::Generate(XBYTE* buffer, XDWORD size)
* @brief      Generate random bytes
* @ingroup    PLATFORM_WINDOWS
* 
* @param[out] buffer : Buffer where the random bytes are stored.
* @param[in]  size : Size of the buffer in bytes.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSRAND::Generate(XBYTE* buffer, XDWORD size)
{
  if(!size)
    {
      return true;
    }

  if(!buffer)
    {
      return false;
    }

  if(BCryptGenRandom(NULL, (PUCHAR)buffer, (ULONG)size, BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0)
    {
      memset(buffer, 0, size);
      return false;
    }

  return true;
}

