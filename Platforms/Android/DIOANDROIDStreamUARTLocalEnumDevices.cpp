/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOANDROIDStreamUARTLocalEnumDevices.cpp
* 
* @class      DIOANDROIDSTREAMUARTLOCALENUMDEVICES
* @brief      ANDROID Data Input/Output UART Local Enum Devices class
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


#if defined(DIO_ACTIVE) && defined(DIO_STREAMUART_ACTIVE)


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOANDROIDStreamUARTLocalEnumDevices.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

#include "XTrace.h"
#include "XPath.h"
#include "DIOStreamDevice.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOANDROIDSTREAMUARTLOCALENUMDEVICES::DIOANDROIDSTREAMUARTLOCALENUMDEVICES()
* @brief      Constructor of class
* @ingroup    PLATFORM_ANDROID
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOANDROIDSTREAMUARTLOCALENUMDEVICES::DIOANDROIDSTREAMUARTLOCALENUMDEVICES() : DIOSTREAMUARTLOCALENUMDEVICES()
{

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOANDROIDSTREAMUARTLOCALENUMDEVICES::~DIOANDROIDSTREAMUARTLOCALENUMDEVICES()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_ANDROID
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOANDROIDSTREAMUARTLOCALENUMDEVICES::~DIOANDROIDSTREAMUARTLOCALENUMDEVICES()
{

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOANDROIDSTREAMUARTLOCALENUMDEVICES::Search()
* @brief      Search
* @ingroup    PLATFORM_ANDROID
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOANDROIDSTREAMUARTLOCALENUMDEVICES::Search()
{
  DelAllDevices();

  struct dirent** namelist;
  XSTRING         sysdir;
  int             ndevices;

  sysdir = __L("/sys/class/tty/");

  XBUFFER charstr;
  
  sysdir.ConvertToASCII(charstr);  
  ndevices = scandir(charstr.GetPtrChar(), &namelist, NULL, NULL);
  
  if(ndevices >= 0)
    {
      while(ndevices--)
        {
          if(strcmp(namelist[ndevices]->d_name,"..") && strcmp(namelist[ndevices]->d_name,"."))
            {
              XSTRING     devicedir;
              struct stat st;

              devicedir  = sysdir;
              devicedir += namelist[ndevices]->d_name;

              devicedir += __L("/device");

              XBUFFER charstr;
              
              devicedir.ConvertToASCII(charstr); 
              bool result = (lstat(charstr.GetPtrChar(), &st)==0) && (S_ISLNK(st.st_mode));              s              
              if(result)
                {
                  char buffer[1024];

                  memset(buffer, 0, sizeof(buffer));

                  devicedir += __L("/driver");

                  XBUFFER charstr;
                  
                  devicedir.ConvertToASCII(charstr);
                  result = (readlink(charstr.GetPtrStr(), buffer, sizeof(buffer)) > 0);
                  
                  if(result)
                    {
                      XSTRING tty;
                      XPATH   devicepath;
                      XPATH   devicefile;

                      tty = buffer;

                      devicepath  = sysdir;
                      devicepath += namelist[ndevices]->d_name;

                      devicepath.GetNamefile(devicefile);

                      devicepath  = __L("/dev/");
                      devicepath += devicefile;

                      // XTRACE_PRINTCOLOR(4,__L(" %s %s %s"),devicedir.Get(), devicepath.Get(), tty.Get());

                      bool isvalid = false;

                      if(tty.Find(__L("serial8250"), true) != XSTRING_NOTFOUND)
                        {
                           struct serial_struct serinfo;
                           XBUFFER              charstr;
 
                           devicepath.ConvertToASCII(charstr);
                           int fd = open(charstr.GetPtrStr(), O_RDWR | O_NONBLOCK | O_NOCTTY);                           
                           if(fd >= 0)
                            {
                              // Get serial_info
                              if(ioctl(fd, TIOCGSERIAL, &serinfo)==0)
                                {
                                  // If device type is no PORT_UNKNOWN we accept the port
                                  if(serinfo.type != PORT_UNKNOWN) isvalid = true;
                                }

                              close(fd);
                            }
                        } else isvalid = true;

                      if(isvalid)
                        {
                          DIOSTREAMDEVICE* device = new DIOSTREAMDEVICE();
                          if(device)
                            {
                              //device->SetIndex(index);
                              device->GetResource()->Set(devicepath);

                              devices.Add(device);
                            }
                        }
                    }
                }
            }

          free(namelist[ndevices]);
        }

      free(namelist);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOANDROIDSTREAMUARTLOCALENUMDEVICES::IsDeviceAvailable(XCHAR* resource)
* @brief      Is device available
* @ingroup    PLATFORM_ANDROID
* 
* @param[in]  resource : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOANDROIDSTREAMUARTLOCALENUMDEVICES::IsDeviceAvailable(XCHAR* resource)
{
  /*
  XSTRING strdevice;

  strdevice = resource;

  HANDLE hcomt = CreateFile(strdevice.Get(),
                            GENERIC_READ|GENERIC_WRITE,
                            0,0,
                            OPEN_EXISTING,
                            0,0);

  if(hcomt==INVALID_HANDLE_VALUE) return false;

  CloseHandle(hcomt);
  */

  return true;
}


#pragma endregion


#endif

