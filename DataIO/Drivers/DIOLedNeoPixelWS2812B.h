/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOLedNeoPixelWS2812B.h
* 
* @class      DIOLEDNEOPIXELWS2812B
* @brief      Data Input/Output Led Neopixel WS2812B class
* @ingroup    DATAIO
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

#ifndef _DIOLEDNEOPIXELWS2812B_H_
#define _DIOLEDNEOPIXELWS2812B_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XBuffer.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class DIOLEDNEOPIXELWS2812B
{
  public:
                              DIOLEDNEOPIXELWS2812B     ();
    virtual                  ~DIOLEDNEOPIXELWS2812B     ();


    virtual bool              Ini                       (XDWORD nled);
    
    XDWORD                    GetDataGPIOEntryID        ();
    void                      SetDataGPIOEntryID        (XDWORD GPIOentryID);

    XBYTE                     GetBrightnessLevel        ();
    void                      SetBrightnessLevel        (XBYTE brightnesslevel = 255);

    bool                      SendData                  (XBUFFER* data);
    bool                      SendData                  (XBYTE* data, XDWORD size); 

    virtual bool              Send                      ();
    
    virtual bool              End                       ();
    
  protected:     
    
    XDWORD                    nled;
    XDWORD                    GPIOentryID;

    XBYTE                     brightnesslevel;

    XBUFFER                   databuffer;  
    
  private:

    void                      Clean                     ();
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

