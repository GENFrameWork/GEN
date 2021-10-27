/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamIPLocalEnumDevices.h
*
* @class      DIOSTREAMIPLOCALENUMDEVICES
* @brief      Data Input/Output Stream IP Local Enum Devices class
* @ingroup    DATAIO
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

#ifndef _DIOSTREAMIPLOCALENUMDEVICES_H_
#define _DIOSTREAMIPLOCALENUMDEVICES_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DIOStreamEnumDevices.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XFACTORY;
class XPUBLISHER;

class DIOSTREAMIPLOCALENUMDEVICES :  public DIOSTREAMENUMDEVICES
{
  public:
                            DIOSTREAMIPLOCALENUMDEVICES       ();
    virtual                ~DIOSTREAMIPLOCALENUMDEVICES       ();

    virtual bool            Search                            ();
    virtual bool            StopSearch                        (bool waitend);
    virtual bool            IsSearching                       ();

    virtual bool            IsDeviceAvailable                 (XCHAR* resource);

    DIOSTREAMDEVICE*        GetFirstActiveDevice              ();

  private:

    void                    Clean                             ();

    bool                    issearching;
};


/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif


