/**-------------------------------------------------------------------------------------------------------------------
*
* @file       INPLINUXDeviceTouchScreenDirect.cpp
*
* @class      INPLINUXDEVICETOUCHSCREENDIRECT
* @brief      LINUX direct input device mouse class
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


/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <unistd.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <cerrno>

#include "XTimer.h"
#include "XFileTXT.h"
#include "XTrace.h"
#include "XSleep.h"

#include "GRPScreen.h"

#include "INPLINUXFactory.h"

#include "INPLINUXDeviceTouchScreenDirect.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         INPLINUXDEVICETOUCHSCREENDIRECT::INPLINUXDEVICETOUCHSCREENDIRECT()
* @brief      Constructor
* @ingroup    PLATFORM_LINUX
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
INPLINUXDEVICETOUCHSCREENDIRECT::INPLINUXDEVICETOUCHSCREENDIRECT(): INPDEVICE()
{
  Clean();

  SetType(INPDEVICE_TYPE_TOUCHSCREEN);

  created = true;

  if(CreateAllButtons())
    {
      if(CreateAllCursors())
        {
          SetEnabled(true);
        }
    }

  UpdateTouchScreenData();

  OpenAllDevicesID();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         INPLINUXDEVICETOUCHSCREENDIRECT::~INPLINUXDEVICETOUCHSCREENDIRECT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
INPLINUXDEVICETOUCHSCREENDIRECT::~INPLINUXDEVICETOUCHSCREENDIRECT()
{
  CloseAllDevicesID();

  touchscr.DeleteContents();
  touchscr.DeleteAll();

  DeleteAllButtons();
  DeleteAllCursors();

  SetEnabled(false);
  created = false;

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool INPLINUXDEVICETOUCHSCREENDIRECT::Update()
* @brief      Update
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool INPLINUXDEVICETOUCHSCREENDIRECT::Update()
{
  if((!created)||(!enabled)) return false;
 
  struct input_event  event;

  int                 eventsize = sizeof(struct input_event);
  int                 bytesread = 0;

  //--------------- clean up old states

  /*
  for(XDWORD c=0; c<buttons.GetSize(); c++)
    {
      INPBUTTON* button=buttons.Get(c);
      if(button)
        {
          if(button->GetState() == INPBUTTON_STATE_RELEASED)
            {
              button->SetState(INPBUTTON_STATE_UP);
            }
           else
            {
              if(button->GetState() == INPBUTTON_STATE_PRESSED) button->SetState(INPBUTTON_STATE_HOLD);
           }
        }
    }
    */


  INPBUTTON* button = GetButton(INPBUTTON_ID_TOUCHSCREEN);  
  if(button) 
    {
      button->SetPressed(false);

      touchscr_x = -1;
      touchscr_y = -1;

      UpdateTouchScreenData();
    }

  //--------------- process

  if(touchscr.GetSize() == 0)
    {
      OpenAllDevicesID();
    }

  for(XDWORD e=0; e<touchscr.GetSize(); e++)
    {
      while(true)
        {
          bytesread=0;

          memset(&event, 0, sizeof(event));

          errno = 0;
          bytesread = read(touchscr.Get(e)->GetFileDescriptor(), &event, sizeof(event)/*eventsize*/);

          // if device is diconnected we receive a ENODEV message (happens for example every time a touchscreen is poweredoff)

          if(bytesread == -1 && errno == ENODEV)
            {
              // we lost the handle to the event file, need to reclaim it
              touchscr.DeleteContents();
              OpenAllDevicesID();
            }

          if(bytesread==-1 && errno!=EAGAIN) break;

          if((bytesread < sizeof(struct input_event)) || (bytesread == -1))  break;

          if(bytesread == sizeof(event)/*eventsize*/)
            {
              switch(event.type)
                {
                  case EV_ABS : switch(event.code)
                                  {
                                    case ABS_X      : { touchscr_x = event.value;                                                                                                              
                                                        //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Touch] x: %d"), touchscr_x);

                                                      }
                                                      break;

                                    case ABS_Y      : { touchscr_y = grpscreen->GetHeight() - event.value; 
                                                        //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Touch] y: %d"), touchscr_y);                                                                                                             

                                                        INPBUTTON* button = GetButton(INPBUTTON_ID_TOUCHSCREEN);
                                                        if(button) button->SetPressed(true);                                                        
                                                      } 
                                                      break;                               
                                   }
                                break;
             
                      default : continue;
                }

            } else break;
        }
    }

  UpdateTouchScreenData();

  return true;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool INPLINUXDEVICETOUCHSCREENDIRECT::CreateAllButtons()
* @brief      Create All Buttons
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool INPLINUXDEVICETOUCHSCREENDIRECT::CreateAllButtons()
{

//INPBUTTON::CreateButton( &buttons, BTN_RIGHT    , INPBUTTON_ID_TOUCHSCREEN_RIGHT        , __C('\x0'));
//INPBUTTON::CreateButton( &buttons, BTN_LEFT     , INPBUTTON_ID_TOUCHSCREEN_LEFT         , __C('\x0'));
//INPBUTTON::CreateButton( &buttons, BTN_MIDDLE   , INPBUTTON_ID_TOUCHSCREEN_MIDDLE       , __C('\x0'));
  INPBUTTON::CreateButton( &buttons, BTN_TOUCH    , INPBUTTON_ID_TOUCHSCREEN              , __C('\x0'));

  return true;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool INPLINUXDEVICETOUCHSCREENDIRECT::CreateAllCursors()
* @brief      Create All Cursors
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool INPLINUXDEVICETOUCHSCREENDIRECT::CreateAllCursors()
{
  INPCURSOR* cursor = new INPCURSOR();
  if(!cursor) return false;

  cursor->SetID(INPCURSOR_ID_TOUCHSCREEN1);

  cursor->SetHavePreSelect(true);

  cursors.Add(cursor);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool INPLINUXDEVICETOUCHSCREENDIRECT::OpenAllDevicesID()
* @brief      Open All Devices ID
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool INPLINUXDEVICETOUCHSCREENDIRECT::OpenAllDevicesID()
{
  INPLINUXFACTORY*     inpdevices = (INPLINUXFACTORY*)(&INPFACTORY::GetInstance());
  XVECTOR<INPLINUXDEVICEID*>  devicesID;
      
  if(inpdevices) inpdevices->GetDeviceHandlers(INPDEVICE_TYPE_TOUCHSCREEN, devicesID);

  for(XDWORD c=0; c<devicesID.GetSize(); c++)
    {
      INPLINUXDEVICEID* deviceID = devicesID.Get(c);
      if(deviceID)
        {
          XSTRING event;
          int     filedescriptor = INPLINUXDEVICEID_INVALID;
     
          event.Format(__L("/dev/input/event%d"), deviceID->GetEventIndex());

          XBUFFER charstr;
          
          event.ConvertToASCII(charstr);          
          filedescriptor = open(charstr.GetPtrChar(), O_RDONLY | O_NONBLOCK);
          
          if(filedescriptor != -1)
            {
              char    mouseOEMname[_MAXSTR];
              XSTRING namehardware;

              // we get the keyboard name
              memset(mouseOEMname, 0, _MAXSTR);
  
              ioctl(filedescriptor, EVIOCGNAME(_MAXSTR), mouseOEMname);
              namehardware.Set(mouseOEMname);
                      
              deviceID->SetFileDescriptor(filedescriptor);
              //deviceID->GetName()->Set(namehardware);

              touchscr.Add(deviceID);
           }
        }
    }

  devicesID.DeleteAll();

  return touchscr.GetSize()?true:false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool INPLINUXDEVICETOUCHSCREENDIRECT::CloseAllDevicesID()
* @brief      Close All Devices ID
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool INPLINUXDEVICETOUCHSCREENDIRECT::CloseAllDevicesID()
{
  if(touchscr.IsEmpty()) return false;

  for(XDWORD c=0; c<touchscr.GetSize(); c++)
    {
      INPLINUXDEVICEID* deviceID = touchscr.Get(c);
      if(deviceID)
        {
          close(deviceID->GetFileDescriptor());
        }
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool INPLINUXDEVICETOUCHSCREENDIRECT::UpdateTouchScreenData()
* @brief      Update TouchScreen Data
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool INPLINUXDEVICETOUCHSCREENDIRECT::UpdateTouchScreenData()
{
  if(!grpscreen) return false;

  for(int e=0;e<GetCursors()->GetSize();e++)
    {
      INPCURSOR* cursor = GetCursors()->Get(e);
      if(!cursor) continue;

      cursor->SetIsChanged(false);

      if((cursor->GetX() != touchscr_x) || (cursor->GetY() != touchscr_y))
        {
          cursor->Set(touchscr_x, touchscr_y);
          cursor->SetIsChanged(true);
        }
    }

  /*
  INPBUTTON* button = GetButton(INPBUTTON_ID_TOUCHSCREEN_LEFT);
  if(button)
    {
      if(GetCursors()->Get(0)) GetCursors()->Get(0)->AddPointToMotion(button->IsPressed());
    }
  */

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void INPLINUXDEVICETOUCHSCREENDIRECT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void INPLINUXDEVICETOUCHSCREENDIRECT::Clean()
{
  touchscr_x  = -1;
  touchscr_y  = -1;
}
