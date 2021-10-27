/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOI2CTemHumSensorAM2315.h
*
* @class      DIOI2CTEMHUMSENSORAM2315
* @brief      Data Input/Output I2C Sensor AOSONG AM2315 class (Temperature and humidity module)
* @ingroup    DATAIO
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

#ifndef _DIOI2CTEMHUMSENSORAM2315_H_
#define _DIOI2CTEMHUMSENSORAM2315_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <stdio.h>

#include "XBase.h"

#include "DIODeviceI2C.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define DIOI2CTEMHUMSENSORAM2315_ADDRESS                     0x5c

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class XFACTORY;
class XPUBLISHER;
class XTIMER;
class DIOFACTORY;
class DIOSTREAMI2CCONFIG;
class DIOSTREAMI2C;


class DIOI2CTEMHUMSENSORAM2315: public DIODEVICEI2C
{
  public:

                              DIOI2CTEMHUMSENSORAM2315      ();
    virtual                  ~DIOI2CTEMHUMSENSORAM2315      ();

    bool                      Read                          (float& temperature, float& humidity);

    bool                      End                           ();

  private:

    bool                      IniDevice                     ();
    bool                      CalculeCRC                    (XBYTE* buffer, XWORD& CRC);

    void                      Clean                         ();
};



/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif




