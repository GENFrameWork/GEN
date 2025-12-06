/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOLINUXLedNeoPixelWS2812BRPi.cpp
* 
* @class      DIOLINUXLEDNEOPIXELWS2812BRPI
* @brief      LINUX Data Input/Output Led Neopixel WS2812B Raspberry Pi class
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
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOLINUXLedNeoPixelWS2812BRPi.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdarg.h>
#include <getopt.h>

#include "clk.h"
#include "gpio.h"
#include "dma.h"
#include "pwm.h"
#include "version.h"

#include "XTrace.h"
#include "XSleep.h"
#include "XBuffer.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

//#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE


#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOLINUXLEDNEOPIXELWS2812BRPI::DIOLINUXLEDNEOPIXELWS2812BRPI()
* @brief      Constructor of class
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOLINUXLEDNEOPIXELWS2812BRPI::DIOLINUXLEDNEOPIXELWS2812BRPI()
{
  Clean();

  //XLINUXIDRPI IDpi;  
  //model = IDpi.GetModel();

  memset(&config, 0, sizeof(ws2811_t));

  config.freq                  = NEOPIXELWS2812BRPI_DEFAULT_TARGET_FREQ;
  config.dmanum                = NEOPIXELWS2812BRPI_DEFAULT_DMA;
  
  config.channel[0].gpionum    = NEOPIXELWS2812BRPI_DEFAULT_GPIO_PIN;
  config.channel[0].count      = NEOPIXELWS2812BRPI_DEFAULT_LED_COUNT;
  config.channel[0].invert     = 0;
  config.channel[0].brightness = brightnesslevel;
  config.channel[0].strip_type = NEOPIXELWS2812BRPI_DEFAULT_STRIP_TYPE;
  
  config.channel[1].gpionum    = 0;
  config.channel[1].count      = 0;
  config.channel[1].invert     = 0;
  config.channel[1].brightness = 0;  
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOLINUXLEDNEOPIXELWS2812BRPI::~DIOLINUXLEDNEOPIXELWS2812BRPI()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOLINUXLEDNEOPIXELWS2812BRPI::~DIOLINUXLEDNEOPIXELWS2812BRPI()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXLEDNEOPIXELWS2812BRPI::Ini(XDWORD nled)
* @brief      Ini
* @ingroup    PLATFORM_LINUX
* 
* @param[in]  nled : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXLEDNEOPIXELWS2812BRPI::Ini(XDWORD nled)
{  
  config.channel[0].count = nled;

  databuffer.Resize(nled * 3);

  ws2811_return_t returnvalue = ws2811_init(&config);
  if(returnvalue != WS2811_SUCCESS) 
    {
      return false;
    }

  return true;  
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXLEDNEOPIXELWS2812BRPI::Send()
* @brief      Send
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXLEDNEOPIXELWS2812BRPI::Send()
{
  XBYTE* ptrdata  = databuffer.Get();
  XDWORD data     = 0;
  int    index    = 0;

  for(int c=0; c<config.channel[0].count; c++)
    {
      config.channel[0].leds[c] = 0;

      data = ptrdata[index];      
      config.channel[0].leds[c] |= data;           
      index++;

      data = ptrdata[index];      
      data <<= 8;
      config.channel[0].leds[c] |= data;           
      index++;

      data = ptrdata[index];      
      data <<= 16;
      config.channel[0].leds[c] |= data;           
      index++;

      data = 0xFF;      
      data <<= 24;
      config.channel[0].leds[c] |= data;                
    }

  config.channel[0].brightness = brightnesslevel;

  ws2811_return_t returnvalue = ws2811_render(&config);
  if(returnvalue != WS2811_SUCCESS) 
    { 
      return false;
    }

  return true;
   
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXLEDNEOPIXELWS2812BRPI::End()
* @brief      End
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXLEDNEOPIXELWS2812BRPI::End()
{  
  //databuffer.FillBuffer();
  SetBrightnessLevel(0);
  Send();

  ws2811_fini(&config);   
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOLINUXLEDNEOPIXELWS2812BRPI::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOLINUXLEDNEOPIXELWS2812BRPI::Clean()
{
  
}


#pragma endregion

