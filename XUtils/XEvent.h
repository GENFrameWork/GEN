/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XEvent.h
*
* @class      XEVENT
* @brief      eXtended Event base class
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

#ifndef _XEVENT_H_
#define _XEVENT_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XBase.h"
#include "XVector.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

enum XEVENT_TYPE
{
  XEVENT_TYPE_NONE                              = 0x00000000 ,
  XEVENT_TYPE_USERINTERFACE                     = 0x00001000 ,
  XEVENT_TYPE_GRAPHICS                          = 0x00002000 ,
  XEVENT_TYPE_DIOSTREAM                         = 0x00003000 ,
  XEVENT_TYPE_DIOIEC60870_5                     = 0x00004000 ,
  XEVENT_TYPE_DIOMODBUSELECTRICMETER            = 0x00005000 ,
  XEVENT_TYPE_PING                              = 0x00006000 ,
  XEVENT_TYPE_WEBCLIENT                         = 0x00007000 ,
  XEVENT_TYPE_WEBSERVER                         = 0x00008000 ,
  XEVENT_TYPE_WEBPAGEHTMLCREATOR                = 0x00009000 ,
  XEVENT_TYPE_INTERNET                          = 0x0000A000 ,
  XEVENT_TYPE_PROTOCOL                          = 0x0000B000 ,
  XEVENT_TYPE_PROTOCOLCONNECTIONS               = 0x0000C000 ,
  XEVENT_TYPE_INPUT                             = 0x0000D000 ,
  XEVENT_TYPE_SCRIPT                            = 0x0000E000 ,
  XEVENT_TYPE_SOUND                             = 0x0000F000 ,
  XEVENT_TYPE_SCHEDULER                         = 0x00010000 ,
  XEVENT_TYPE_LICENSE                           = 0x00011000 ,
  XEVENT_TYPE_APPLICATIONCHECKRESOURCESHW       = 0x00012000 ,
  XEVENT_TYPE_APPLICATIONINTERNETSERVICES       = 0x00013000 ,
  XEVENT_TYPE_APPLICATIONUPDATE                 = 0x00014000 ,
  XEVENT_TYPE_APPLICATION                       = 0x00015000 ,
  XEVENT_TYPE_LOADER                            = 0x00016000 ,
  XEVENT_TYPE_DRIVEIMAGEMANAGER                 = 0x00017000 ,
  XEVENT_TYPE_FILEZIP                           = 0x00018000 ,
  XEVENT_TYPE_FILECFG                           = 0x00019000 ,  
  XEVENT_TYPE_BLUETOOTH                         = 0x00020000 ,
  XEVENT_TYPE_WINDOWS_KBDHOOK                   = 0x00021000 ,
  XEVENT_TYPE_LINUX_DBUS                        = 0x00022000 ,

  XEVENT_TYPE_OWNAPPLICATION                    = 0x80000000 
};


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class  XSUBJECT;

class XEVENT
{
  public:
                      XEVENT            (XSUBJECT* subject, XDWORD type, XDWORD family);
                      XEVENT            (XSUBJECT* subject, XDWORD type);

    virtual          ~XEVENT            ();

    XSUBJECT*         GetSubject        ();
    void              SetSubject        (XSUBJECT* subject);

    XDWORD            GetEventFamily    ();
    void              SetEventFamily    (XDWORD family);

    XDWORD            GetEventType      ();
    void              SetEventType      (XDWORD type);        

  protected:

    void              Clean             ();

    XDWORD            family;
    XDWORD            type;
    XSUBJECT*         subject;
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif





