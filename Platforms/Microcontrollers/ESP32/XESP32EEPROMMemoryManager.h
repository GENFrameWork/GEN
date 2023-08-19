/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XESP32FlashMemory.h
*
* @class      XESP32EEPROMMEMORYMANAGER
* @brief      eXtended utils ESP32 EEPROM Memory Manager class
* @ingroup    PLATFORM_ESP32    PLATFORM_ESP32
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

#ifndef _XESP32EEPROMMEMORYMANAGER_H_
#define _XESP32EEPROMMEMORYMANAGER_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XBase.h"

#include "XEEPROMMemoryManager.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define XESP32EEPROMMEMORYMANAGER_BASE      BKPSRAM_BASE
#define XESP32EEPROMMEMORYMANAGER_MAXSIZE   4096

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class XESP32EEPROMMEMORYMANAGER : public XEEPROMMEMORYMANAGER
{
  public:

                                  XESP32EEPROMMEMORYMANAGER         ();
    virtual                      ~XESP32EEPROMMEMORYMANAGER         ();

    bool                          Ini                               ();
    bool                          End                               ();

    bool                          Read                              (XDWORD offset, XBYTE* data, XDWORD size);
    bool                          Write                             (XDWORD offset, XBYTE* data, XDWORD size);

    bool                          EraseAll                          ();

};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif




