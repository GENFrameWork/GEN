/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XEEPROMMemoryManager.h
*
* @class      XEEPROMMEMORYMANAGER
* @brief      eXtended EEPROM Memory Manager class
* @ingroup    XUTILS
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

#ifndef _XEEPROMMEMORYMANAGER_H_
#define _XEEPROMMEMORYMANAGER_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XBase.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class XEEPROMMEMORYMANAGER 
{
  public:

                                      XEEPROMMEMORYMANAGER        ();
    virtual                          ~XEEPROMMEMORYMANAGER        ();

    static bool                       GetIsInstanced              ();
    static XEEPROMMEMORYMANAGER&      GetInstance                 ();
    static bool                       SetInstance                 (XEEPROMMEMORYMANAGER* instance);
    static bool                       DelInstance                 ();

    virtual bool                      Ini                         ();
    virtual bool                      End                         ();

    bool                              Read                        (XDWORD offset, XBYTE& data);
    bool                              Read                        (XDWORD offset, XWORD& data);
    bool                              Read                        (XDWORD offset, XDWORD& data);
    virtual bool                      Read                        (XDWORD offset, XBYTE* data, XDWORD size);


    bool                              Write                       (XDWORD offset, XBYTE data);
    bool                              Write                       (XDWORD offset, XWORD data);
    bool                              Write                       (XDWORD offset, XDWORD data);
    virtual bool                      Write                       (XDWORD offset, XBYTE* data, XDWORD size);

    virtual bool                      EraseAll                    ();

  private:

    static XEEPROMMEMORYMANAGER*      instance;
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif




