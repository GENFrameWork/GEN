/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamUART.h
*
* @class      DIOSTREAMUART
* @brief      Data Input/Output Stream UART class
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

#ifndef _DIOSTREAMUART_H_
#define _DIOSTREAMUART_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DIOStream.h"
#include "DIOStreamUARTConfig.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XFACTORY;
class XMUTEX;
class XPUBLISHER;
class DIOFACTORY;

class DIOSTREAMUART : public DIOSTREAM
{
  public:
                                  DIOSTREAMUART                     ();
    virtual                      ~DIOSTREAMUART                     ();

    virtual DIOSTREAMCONFIG*      GetConfig                         ();
    virtual bool                  SetConfig                         (DIOSTREAMCONFIG* config);

    virtual DIOSTREAMSTATUS       GetConnectStatus                  ();

    virtual bool                  Open                              ();

    virtual bool                  Config                            (XWORD mask = DIOSTREAMUARTMASK_ALL);

    virtual XDWORD                ReadDirect                        (XBYTE* buffer, XDWORD size);
    virtual XDWORD                WriteDirect                       (XBYTE* buffer, XDWORD size);

    virtual bool                  Close                             ();

    virtual bool                  GetCTS                            ();
    virtual bool                  GetDSR                            ();
    virtual bool                  GetRing                           ();
    virtual bool                  GetRLSD                           ();

    virtual bool                  SetRTS                            (bool on = true);
    virtual bool                  SetDTR                            (bool on = true);

    virtual bool                  CleanBuffers                      ();

  protected:

    DIOSTREAMUARTCONFIG*          config;
    XTIMER*                       xtimer;

  private:

    void                          Clean                             ();
};

/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif


