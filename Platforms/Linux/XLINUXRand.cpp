/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XLINUXRand.cpp
* 
* @class      XLINUXRAND
* @brief      LINUX eXtended Utils random class
* @ingroup    PLATFORM_LINUX
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

#include "XLINUXRand.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XLINUXRAND::XLINUXRAND()
* @brief      Constructor of class
* @ingroup    PLATFORM_LINUX
* 
* --------------------------------------------------------------------------------------------------------------------*/
XLINUXRAND::XLINUXRAND()
{

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XLINUXRAND::~XLINUXRAND()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
* 
* --------------------------------------------------------------------------------------------------------------------*/
XLINUXRAND::~XLINUXRAND()
{

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XLINUXRAND::Ini()
* @brief      Initialize the object
* @ingroup    PLATFORM_LINUX
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXRAND::Ini()
{
  XBYTE data = 0;

  return Generate(&data, sizeof(data));
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XLINUXRAND::Generate(XBYTE* buffer, XDWORD size)
* @brief      Generate random bytes
* @ingroup    PLATFORM_LINUX
* 
* @param[out] buffer : Buffer where the random bytes are stored.
* @param[in]  size : Size of the buffer in bytes.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXRAND::Generate(XBYTE* buffer, XDWORD size)
{
  if(!size)
    {
      return true;
    }

  if(!buffer)
    {
      return false;
    }

  int file = open("/dev/urandom", O_RDONLY);
  if(file < 0)
    {
      memset(buffer, 0, size);
      return false;
    }

  XDWORD generated = 0;

  while(generated < size)
    {
      ssize_t result = read(file, &buffer[generated], size-generated);

      if(result < 0)
        {
          if(errno == EINTR)
            {
              continue;
            }

          close(file);
          memset(buffer, 0, size);
          return false;
        }

      if(!result)
        {
          close(file);
          memset(buffer, 0, size);
          return false;
        }

      generated += (XDWORD)result;
    }

  close(file);

  return true;
}





