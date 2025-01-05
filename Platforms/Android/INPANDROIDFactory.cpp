/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       INPANDROIDFactory.cpp
* 
* @class      INPANDROIDFACTORY
* @brief      ANDROID input factory class
* @ingroup    PLATFORM_ANDROID
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "INPANDROIDFactory.h"

#include "XTrace.h"

#include "GRPScreen.h"

#include "INPANDROIDDeviceKeyboard.h"
#include "INPANDROIDDeviceMouse.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         INPDEVICE* INPANDROIDFACTORY::CreateDevice(XCHAR* devicename, void* param)
* @brief      CreateDevice
* @ingroup    PLATFORM_ANDROID
*
* @param[in]  type : device type of the input device
* @param[in]  param : param to create device (in windows need handle to windows)
*
* @return     INPDEVICE* : device input created
*
* --------------------------------------------------------------------------------------------------------------------*/
INPDEVICE* INPANDROIDFACTORY::CreateDevice(INPDEVICE_TYPE type, void* param)
{
  INPDEVICE* inpdevice    = NULL;

  switch(type)
    {
      case INPDEVICE_TYPE_KEYBOARD :  { INPANDROIDDEVICEKEYBOARD* keyboard = new INPANDROIDDEVICEKEYBOARD();
                                        if(keyboard)
                                          {
                                            keyboard->SetScreen(param);
                                            inpdevice = keyboard;
                                          }
                                      }
                                      break;

      case INPDEVICE_TYPE_MOUSE    :  { INPANDROIDDEVICEMOUSE* mouse = new INPANDROIDDEVICEMOUSE();
                                        if(mouse)
                                          {
                                            mouse->SetScreen(param);
                                            inpdevice = mouse;
                                          }
                                      }
                                      break;

    }

  return inpdevice;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool INPANDROIDFACTORY::DeleteDevice(INPDEVICE* device)
* @brief      Delete Device
* @ingroup    PLATFORM_ANDROID
*
* @param[in]  device : device to delete
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool INPANDROIDFACTORY::DeleteDevice(INPDEVICE* device)
{
  if(!device)  return false;

  INPDEVICE* inpdevice    = NULL;

  switch(device->GetType())
    {
      case INPDEVICE_TYPE_KEYBOARD :  { INPANDROIDDEVICEKEYBOARD* keyboard = new INPANDROIDDEVICEKEYBOARD();
                                        if(keyboard) delete keyboard;                                          
                                      }
                                      break;

      case INPDEVICE_TYPE_MOUSE    :  { INPANDROIDDEVICEMOUSE* mouse = new INPANDROIDDEVICEMOUSE();
                                        if(mouse) delete mouse;                                        
                                      }
                                      break;
    }


  return true;
}


#pragma endregion

