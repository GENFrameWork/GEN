/**-------------------------------------------------------------------------------------------------------------------
*
* @file       INPFactoryDevices.h
*
* @class      INPFACTORYDEVICES
* @brief      INPUT factory Devices class
* @ingroup    INPUT
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

#ifndef _INPFACTORYDEVICES_H_
#define _INPFACTORYDEVICES_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "INPDevice.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class INPDEVICE;

class INPFACTORYDEVICES
{
  public:
                                    INPFACTORYDEVICES         ();
    virtual                        ~INPFACTORYDEVICES         ();

    static bool                     GetIsInstanced            ();
    static INPFACTORYDEVICES&       GetInstance               ();
    static bool                     SetInstance               (INPFACTORYDEVICES* instance);
    static bool                     DelInstance               ();

    virtual INPDEVICE*              CreateDevice              (INPDEVICE_TYPE type, void* param = NULL);
    virtual bool                    DeleteDevice              (INPDEVICE* device);

  private:

    void                            Clean                     ();

    static INPFACTORYDEVICES*       instance;
};



/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif

