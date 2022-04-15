/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOSTM32Factory.cpp
*
* @class      DIOSTM32FACTORY
* @brief      Data Input/Output STM32 class
* @ingroup    PLATFORM_STM32
*
* @copyright  GEN Group. All right reserved.
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

#ifdef DIO_ACTIVE

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#ifdef DIOUART_ACTIVE
#include "DIOStreamUARTConfig.h"
#include "DIOSTM32StreamUART.h"
#endif

#ifdef DIOUSB_ACTIVE
#include "DIOStreamUSBConfig.h"
#include "DIOSTM32StreamUSB.h"
#endif

#ifdef DIOSPI_ACTIVE
#include "DIOStreamSPIConfig.h"
#include "DIOSTM32StreamSPI.h"
#endif

#ifdef DIOI2C_ACTIVE
#include "DIOStreamI2CConfig.h"
#include "DIOSTM32StreamI2C.h"
#endif


#ifdef DIOLEDNEOPIXELWS2812B_ACTIVE 
#include "DIOSTM32LedNeoPixelWS2812B.h"
#endif

#include "DIOSTM32Factory.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



#ifdef ANYTYPEOFDIOSTREAMIO

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAM* DIOSTM32FACTORY::CreateStreamIO(DIOSTREAMCONFIG* config)
* @brief      CreateStreamIO
* @ingroup    PLATFORM_STM32
*
* @param[in]  config :
*
* @return     DIOSTREAM* :
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOSTREAM* DIOSTM32FACTORY::CreateStreamIO(DIOSTREAMCONFIG* config)
{
  if(!config) return NULL;

  DIOSTREAM* _class = NULL;

  switch(config->GetType())
    {
      case DIOSTREAMTYPE_UNKNOWN    :
                          default   : return NULL;

      #ifdef DIOUART_ACTIVE
      case DIOSTREAMTYPE_UART       : _class = new DIOSTM32STREAMUART();        break;
      #endif

      #ifdef DIOUSB_ACTIVE
      case DIOSTREAMTYPE_USB        : _class = new DIOSTM32STREAMUSB();         break;
      #endif

      #ifdef DIOSPI_ACTIVE
      case DIOSTREAMTYPE_SPI        : _class = new DIOSTM32STREAMSPI();         break;
      #endif

      #ifdef DIOI2C_ACTIVE
      case DIOSTREAMTYPE_I2C        : _class = new DIOSTM32STREAMI2C();         break;
      #endif
    }

  if(_class)
    {
      _class->SetType(config->GetType());
      _class->SetConfig(config);
    }


  return _class;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTM32FACTORY::DeleteStreamIO(DIOSTREAM* diostream)
* @brief      DeleteStreamIO
* @ingroup    PLATFORM_STM32
*
* @param[in]  diostream :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOSTM32FACTORY::DeleteStreamIO(DIOSTREAM* diostream)
{
  if(!diostream) return false;
  delete(diostream);

  return true;
}



#ifdef DIOLEDNEOPIXELWS2812B_ACTIVE 

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOLEDNEOPIXELWS2812B* DIOSTM32FACTORY::CreateLedNeopixelWS2812B()
* @brief      CreateLedNeopixelWS2812B
* @ingroup    PLATFORM_STM32
*
* @return     DIOLEDNEOPIXELWS2812B* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOLEDNEOPIXELWS2812B* DIOSTM32FACTORY::CreateLedNeopixelWS2812B()
{
  DIOSTM32LEDNEOPIXELWS2812B* ledneopixelws2812b = new DIOSTM32LEDNEOPIXELWS2812B();

  return ledneopixelws2812b;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTM32FACTORY::DeleteLedNeopixelWS2812B(DIOLEDNEOPIXELWS2812B* ledneopixelws2812b)
* @brief      DeleteLedNeopixelWS2812B
* @ingroup    PLATFORM_STM32
*
* @param[in]  ledneopixelws2812b : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOSTM32FACTORY::DeleteLedNeopixelWS2812B(DIOLEDNEOPIXELWS2812B* ledneopixelws2812b)
{
  if(!ledneopixelws2812b) return false;
  
  delete ledneopixelws2812b;
  ledneopixelws2812b = NULL;
  
  return true;
}


#endif






#endif

#endif
