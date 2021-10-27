/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOLINUXGPIOPCPARALLEL.h
*
* @class      DIOLINUXGPIOPCPARALLEL
* @brief      Data Input/Output LINUX GPIO (General Purpose Input/Output) PC Parallel
* @ingroup    PLATFORM_LINUX
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

#ifdef HW_PC

#ifndef _DIOLINUXGPIOPCPARALLEL_H_
#define _DIOLINUXGPIOPCPARALLEL_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DIOGPIO.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


#define DIOLINUXGPIOPCPARALLEL_PORT1                0x378
#define DIOLINUXGPIOPCPARALLEL_PORT2                0x278

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class DIOLINUXGPIOPCPARALLEL : public DIOGPIO
{
  public:
                            DIOLINUXGPIOPCPARALLEL          ();
    virtual                ~DIOLINUXGPIOPCPARALLEL          ();

    XWORD                   GetParallelPort                 ();
    void                    SetParallelPort                 (XWORD port = DIOLINUXGPIOPCPARALLEL_PORT1);
    

    bool                    Ini                             ();

    bool                    SetMode                         (DIOGPIO_ENTRY* entry, XWORD mode);  

    bool                    GetValue                        (DIOGPIO_ENTRY* entry);
    bool                    SetValue                        (DIOGPIO_ENTRY* entry, bool value);   

    bool                    End                             ();

  private:

    bool                    PC_Get                          (XDWORD nport,XBYTE& data);
    bool                    PC_Set                          (XDWORD nport,XBYTE data);

    void                    Clean                           ();

    XWORD                   port;   
};

/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif


#endif