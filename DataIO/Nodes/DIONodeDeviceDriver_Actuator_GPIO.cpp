/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIONodeDeviceDriver_Actuator_GPIO.cpp
* 
* @class      DIONODEDEVICEDRIVER_ACTUATOR_GPIO
* @brief      Data Input/Output Node Device Driver actuator GPIO
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DIOGPIO.h"
#include "DIONodeItemActuator.h"

#include "DIONodeDeviceDriver_Actuator_GPIO.h"

#include "XMemory_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIONODEDEVICEDRIVER_ACTUATOR_GPIO::DIONODEDEVICEDRIVER_ACTUATOR_GPIO()
* @brief      Constructor
* @ingroup    DATAIO
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIONODEDEVICEDRIVER_ACTUATOR_GPIO::DIONODEDEVICEDRIVER_ACTUATOR_GPIO(XDWORD entryID, int GPIO, int pin) : DIONODEDEVICEDRIVER_GPIO(entryID, GPIO, pin)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIONODEDEVICEDRIVER_ACTUATOR_GPIO::~DIONODEDEVICEDRIVER_ACTUATOR_GPIO()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIONODEDEVICEDRIVER_ACTUATOR_GPIO::~DIONODEDEVICEDRIVER_ACTUATOR_GPIO()
{
  Close();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIONODEDEVICEDRIVER_ACTUATOR_GPIO::Open()
* @brief      Open
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIONODEDEVICEDRIVER_ACTUATOR_GPIO::Open()
{
  isopen = true;
  isworking = true;

  GEN_DIOGPIO.SetMode(entryID, DIOGPIO_MODE_OUTPUT);  
   
  return isopen;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIONODEDEVICEDRIVER_ACTUATOR_GPIO::Update()
* @brief      Update
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIONODEDEVICEDRIVER_ACTUATOR_GPIO::Update()
{
  DIONODEITEMACTUATOR* nodeitemactuator = (DIONODEITEMACTUATOR*)GetNodeItem();
  if(!nodeitemactuator)
    {
      return false;
    }

  DIONODEITEMVALUE* itemvalue = nodeitemactuator->GetValues()->Get(0);
  if(itemvalue)
    {
      XVARIANT* value = nodeitemactuator->GetValues()->Get(0)->GetValue();
      if(value)
        {
          if(itemvalue->ValueHasChanged())
            {
              bool _value = (*value);
                    
              GEN_DIOGPIO.SetValue(entryID, _value);
          
              AdjustTimeInChange(_value);

              itemvalue->SetValueHasChanged(false);

              XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[DIO Node Item GPIO Actuator] value %s"), _value?__L("true"):__L("false"));
            }          
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIONODEDEVICEDRIVER_ACTUATOR_GPIO::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIONODEDEVICEDRIVER_ACTUATOR_GPIO::Clean()
{

}
