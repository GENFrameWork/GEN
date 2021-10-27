/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOSTM32StreamI2C.h
*
* @class      DIOSTM32STREAMI2C
* @brief      Data Input/Output STM32 Stream I2C class
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

#ifndef _DIOSTM32STREAMI2C_H_
#define _DIOSTM32STREAMI2C_H_

#if defined(DIO_ACTIVE) && defined(DIOI2C_ACTIVE)

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XBuffer.h"
#include "XFSMachine.h"

#include "XSTM32_HAL.h"

#include "DIOStreamI2C.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define DIOSTM32STREAMI2C_MAXHANDLES   8
#define DIOSTM32STREAMI2C_MAXPORTS     4

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XFACTORY;
class XPUBLISHER;
class DIOFACTORY;
class DIOSTREAMCONFIG;
class XTHREADCOLLECTED;



class DIOSTM32STREAMI2CPORT
{
  public:
                                            DIOSTM32STREAMI2CPORT               ();
    virtual                                ~DIOSTM32STREAMI2CPORT               ();


    XDWORD                                  GetCounterRef                       ();
    void                                    SetCounterRef                       (XDWORD counterref);

    I2C_HandleTypeDef*                      GetHandleI2C                        ();
    void                                    SetHandleI2C                        (I2C_HandleTypeDef* hi2c);

  private:

    void                                    Clean                               ();

    XDWORD                                  counterref;
    I2C_HandleTypeDef*                      hi2c;
};




class DIOSTM32STREAMI2C : public DIOSTREAMI2C
{
  public:
                                            DIOSTM32STREAMI2C                   ();
    virtual                                ~DIOSTM32STREAMI2C                   ();

    DIOSTREAMSTATUS                         GetConnectStatus                    ();

    virtual bool                            Open                                ();

    bool                                    WaitToFilledReadingBuffer           (int filledto = DIOSTREAM_SOMETHINGTOREAD, int timeout = XTIMER_INFINITE);

    XDWORD                                  ReadDirect                          (XBYTE* buffer, XDWORD size);
    XDWORD                                  WriteDirect                         (XBYTE* buffer, XDWORD size);

    XDWORD                                  Write                               (XBYTE* buffer, XDWORD size);

    bool                                    Disconnect                          ();
    bool                                    Close                               ();

    bool                                    CleanBuffers                        ();

    I2C_HandleTypeDef*                      hi2c;
    XWORD                                   localaddress;
    XWORD                                   remoteaddress;
    XBYTE                                   bufferdata[DIOSTREAM_MAXBUFFER];
    XWORD                                   sizebufferdata;
    XBUFFER                                 readcache[DIOSTM32STREAMI2C_MAXPORTS];

    static DIOSTM32STREAMI2CPORT*           ports[DIOSTM32STREAMI2C_MAXPORTS];
    static DIOSTM32STREAMI2C*               handles[DIOSTM32STREAMI2C_MAXHANDLES];

    int                                     indexhandle;
    int                                     indexport;

  protected:

    static void                             ThreadConnection                    (void* param);

    void                                    Clean                               ();

    XTHREADCOLLECTED*                       threadconnection;
    XBYTE                                   writebuffer[DIOSTREAM_MAXBUFFER];

};


/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif

#endif


