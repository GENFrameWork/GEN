/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPXEvent.h
* 
* @class      GRPXEVENT
* @brief      Graphics XEvent class
* @ingroup    GRAPHIC
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

#ifndef _GRPXEVENT_H_
#define _GRPXEVENT_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XEvent.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

enum GRPXEVENT_TYPE
{
  GRPXEVENT_TYPE_UNKNOWN                   = XEVENT_TYPE_GRAPHICS ,

  GRPXEVENT_TYPE_SCREEN_CREATING                                 ,
  GRPXEVENT_TYPE_SCREEN_CANVASCREATING                           ,
  GRPXEVENT_TYPE_SCREEN_CREATED                                  ,
  GRPXEVENT_TYPE_SCREEN_DELETING                                 ,
  GRPXEVENT_TYPE_SCREEN_DELETED                                  ,
  GRPXEVENT_TYPE_SCREEN_CHANGEPOSITION                           ,
  GRPXEVENT_TYPE_SCREEN_CHANGESIZE                               ,
  GRPXEVENT_TYPE_SCREEN_CHANGEFOCUS                              ,
};

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class GRPSCENE;
class GRPSCREEN;
class GRPOBJECT;
class GRPVIEWPORT;

class GRPXEVENT : public XEVENT
{
  public:
                                      GRPXEVENT                     (XSUBJECT* subject, XDWORD type, XDWORD family = XEVENT_TYPE_GRAPHICS);
                                     ~GRPXEVENT                     ();

    GRPSCREEN*                        GetScreen                     ();
    void                              SetScreen                     (GRPSCREEN* screen);

    GRPVIEWPORT*                      GetViewport                   ();
    void                              SetViewport                   (GRPVIEWPORT* viewport);

    bool                              GetError                      ();
    void                              SetError                      (bool error);

  protected:

    GRPSCREEN*                        screen;
    GRPVIEWPORT*                      viewport;
    bool                              error;


  private:

    void                              Clean                         ();
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif
