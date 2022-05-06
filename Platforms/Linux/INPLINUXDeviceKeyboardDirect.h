/**-------------------------------------------------------------------------------------------------------------------
*
* @file       INPLINUXDeviceKeyboardDirect.h
*
* @class      INPLINUXDEVICEKEYBOARDDIRECT
* @brief      LINUX direct input device keyboard class
* @ingroup    PLATFORM_LINUX
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

#ifndef _INPLINUXDEVICEKEYBOARDDIRECT_H_
#define _INPLINUXDEVICEKEYBOARDDIRECT_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <termios.h>

#include "XVector.h"

#include "INPButton.h"
#include "INPDevice.h"

#include "INPLINUXDeviceID.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define INPLINUXDEVICEKEYBOARDDIRECT_OLDMAXBUFFER   16

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class GRPSCREEN;

class INPLINUXDEVICEKEYBOARDDIRECT : public INPDEVICE
{
  public:
                                INPLINUXDEVICEKEYBOARDDIRECT      ();
    virtual                    ~INPLINUXDEVICEKEYBOARDDIRECT      ();

    bool                        Update                            ();

  private:

    void                        Clean                             ();

    bool                        CreateAllButtons                  ();

    bool                        CreateDevices                     ();

    bool                        Old_CreateDevices                 ();
    bool                        Old_DeleteTerminalConfig          ();
    int                         Old_IsConsole                     (int fd);
    int                         Old_OpenConsole                   (const char *fnam);
    int                         Old_GetFD                         (const char *fnam);

    int                         old_fd;
    int                         old_keyboardmode;
    struct termios              old_termiosinitialsettings;

    XVECTOR<INPLINUXDEVICEID*>  keyboards;
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif


