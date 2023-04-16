/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIONode_XEvent.h
*
* @class      DIONODE_XEVENT
* @brief      Data Input/Output Node eXtended Event class
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

#ifndef _DIONODE_XEVENT_H_
#define _DIONODE_XEVENT_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XEvent.h"
#include "XSubject.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

enum DIONODE_XEVENT_TYPE
{
  DIONODE_XEVENT_TYPE_UNKNOWN                 = XEVENT_TYPE_DIONODE ,
  DIONODE_XEVENT_TYPE_UPDATE                                        ,
};


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class DIONODE_XEVENT : public XEVENT
{
  public:
                                  DIONODE_XEVENT              (XSUBJECT* subject, XDWORD type = DIONODE_XEVENT_TYPE_UNKNOWN, XDWORD family = XEVENT_TYPE_DIONODE);
    virtual                      ~DIONODE_XEVENT              ();
    
  private:

    void                          Clean                       ();

};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif

