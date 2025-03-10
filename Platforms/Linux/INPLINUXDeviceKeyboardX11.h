/**-------------------------------------------------------------------------------------------------------------------
*
* @file       INPLINUXDeviceKeyboardX11.h
*
* @class      INPLINUXDEVICEKEYBOARDX11
* @brief      LINUX X11 input device keyboard class
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

#ifdef LINUX_X11_ACTIVE

#ifndef _INPLINUXDEVICEKEYBOARDX11_H_
#define _INPLINUXDEVICEKEYBOARDX11_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "INPButton.h"
#include "INPDevice.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define INPLINUXDEVICEKEYBOARDX11_EVRELEASED    0
#define INPLINUXDEVICEKEYBOARDX11_EVPRESSED     1
#define INPLINUXDEVICEKEYBOARDX11_EVREPEAT      2

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class GRPLINUXSCREENX11;

class INPLINUXDEVICEKEYBOARDX11 : public INPDEVICE
{
  public:
                            INPLINUXDEVICEKEYBOARDX11     ();
    virtual                ~INPLINUXDEVICEKEYBOARDX11     ();

    bool                    Update                        ();

    bool                    SetScreen                     (void* screenhandle);

  private:

    void                    Clean                         ();

    bool                    CreateAllButtons              ();

    GRPLINUXSCREENX11*      grpscreenx11;
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/


#endif


#endif
