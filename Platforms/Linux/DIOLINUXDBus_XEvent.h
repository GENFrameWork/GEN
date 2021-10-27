/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOLINUXDBus_XEvent.h
*
* @class      DIOLINUXDBUS_XEVENT
* @brief      Data Input/Output Linux DBus eXtended Event class
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

#ifndef _DIOLINUXDBUS_XEVENT_H_
#define _DIOLINUXDBUS_XEVENT_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XString.h"
#include "XPath.h"
#include "XEvent.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

enum DIOLINUXDBUS_XEVENT_TYPE
{
  DIOLINUXDBUS_XEVENT_TYPE_UNKNOW                = XEVENT_TYPE_LINUX_DBUS  ,  
  DIOLINUXDBUS_XEVENT_TYPE_SIGNAL                                          ,  
};


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class DIOLINUXDBUS_MESSAGE;
class DIOLINUXDBUS_SIGNAL;
class DIOLINUXDBUS;

class DIOLINUXDBUS_XEVENT : public XEVENT
{
  public:
                                            DIOLINUXDBUS_XEVENT           (XSUBJECT* subject, XDWORD type = DIOLINUXDBUS_XEVENT_TYPE_UNKNOW);
    virtual                                ~DIOLINUXDBUS_XEVENT           ();

    DIOLINUXDBUS_SIGNAL*                    GetSignal                     ();                      
    void                                    SetSignal                     (DIOLINUXDBUS_SIGNAL* signal);

    DIOLINUXDBUS_MESSAGE*                   GetMessage                    ();                      
    void                                    SetMessage                    (DIOLINUXDBUS_MESSAGE* message);

    
  private:

    void                                    Clean                         ();    

    DIOLINUXDBUS_SIGNAL*                    signal;
    DIOLINUXDBUS_MESSAGE*                   message;
    
};


/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif



