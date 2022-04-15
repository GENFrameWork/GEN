/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOI2CEEprom24XXX.h
*
* @class      DIOI2CEEPROM24XXX
* @brief      Data Input/Output I2C EEPROM 24XXX class.
* @ingroup    DATAIO
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

#ifndef _DIOI2CEEPROM24XXX_H_
#define _DIOI2CEEPROM24XXX_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <stdio.h>

#include "XBase.h"

#include "DIODeviceI2C.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

enum DIOI2CEEPROM24XXXTYPE
{
  DIOI2CEEPROM24XXXTYPE_UNKNOWN       = 0 ,
  DIOI2CEEPROM24XXXTYPE_08                ,
  DIOI2CEEPROM24XXXTYPE_16                ,
  DIOI2CEEPROM24XXXTYPE_65                ,
  DIOI2CEEPROM24XXXTYPE_515               ,
};


#define DIOI2CEEPROM2408_ADDR                   0x50

#define DIOI2CEEPROM2408_FIXEDADDRESSBASE       0x50
#define DIOI2CEEPROM2416_FIXEDADDRESSBASE       0x50

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class XFACTORY;
class DIOFACTORY;
class XPUBLISHER;
class DIOSTREAMI2CCONFIG;
class DIOSTREAMI2C;


class DIOI2CEEPROM24XXX : public DIODEVICEI2C
{
  public:

                              DIOI2CEEPROM24XXX           ();
    virtual                  ~DIOI2CEEPROM24XXX           ();

    DIOI2CEEPROM24XXXTYPE     GetType                     ();
    void                      SetType                     (DIOI2CEEPROM24XXXTYPE type);

    XDWORD                    GetSize                     ();

    bool                      Read                        (XDWORD address, XBYTE* buffer, XDWORD size);
    bool                      Read                        (XDWORD address, XDWORD size, XBUFFER& xbuffer);

    bool                      Write                       (XDWORD address, XBYTE* buffer, XDWORD size);
    bool                      Write                       (XDWORD address, XBUFFER& xbuffer);

    bool                      End                         ();

  protected:  

    DIOI2CEEPROM24XXXTYPE     type;

    XDWORD                    maxsize;                        // All descriptors in Bytes.
    XWORD                     npages;
    XWORD                     maxsizepage;
    XWORD                     maxsizechunk;

  private:

    bool                      IniDevice                   ();


    bool                      ReadBYTE                    (XDWORD address, XBYTE& data);
    bool                      WriteBYTE                   (XDWORD address, XBYTE data);

    void                      Clean                       ();   
};


/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif

