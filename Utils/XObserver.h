/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XObserver.h
*
* @class      XOBSERVER
* @brief      eXtended Observerd class
* @ingroup    UTILS
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

#ifndef _XOBSERVER_H_
#define _XOBSERVER_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XPublisher.h"
#include "XEvent.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XSUBJECT;
class XPUBLISHER;

class XOBSERVER
{
  public:

                                    XOBSERVER                     ();
    virtual                        ~XOBSERVER                     ();

    bool                            SubscribeEvent                (XDWORD type, XSUBJECT* subject, XOBSERVER* observer = NULL);
    bool                            UnSubscribeEvent              (XDWORD type, XSUBJECT* subject, XOBSERVER* observer = NULL);

    virtual void                    HandleEvent                   (XEVENT* event) = 0;

  private:

    void                            Clean                         ();
};


/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif

