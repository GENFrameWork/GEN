/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOSTM32GPIO.cpp
*
* @class      DIOSTM32GPIO
* @brief      Data Input/Output STM32 General Port Input/Ouput (GPIO)
* @ingroup    PLATFORM_STM32
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

#include "XSTM32_HAL.h"
#include "XTrace.h"

#include "DIOSTM32GPIO.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTM32GPIO::DIOSTM32GPIO()
* @brief      Constructor of class
* @ingroup    PLATFORM_STM32
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTM32GPIO::DIOSTM32GPIO(): DIOGPIO()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTM32GPIO::~DIOSTM32GPIO()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_STM32
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTM32GPIO::~DIOSTM32GPIO()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTM32GPIO::Ini()
* @brief      Ini
* @ingroup    PLATFORM_STM32
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTM32GPIO::Ini()
{
  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTM32GPIO::SetMode(DIOGPIO_ENTRY* entry, XWORD mode)
* @brief      Set mode
* @ingroup    PLATFORM_STM32
*
* @param[in]  entry : 
* @param[in]  mode : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOSTM32GPIO::SetMode(DIOGPIO_ENTRY* entry, XWORD mode)
{
  if(!entry) return false;

  entry->SetMode(mode);  
  
  GPIO_InitTypeDef  GPIO_initstruct = {0}; 
  GPIO_TypeDef*     GPIOgroup       = NULL;
 
  GPIO_initstruct.Pin  = entry->GetPin();
  
  switch(entry->GetGroup())
    {
      case DIOGPIO_PINSGROUP_NONE : break;
      case DIOGPIO_PINSGROUP_A    : GPIOgroup = GPIOA;   break;
      case DIOGPIO_PINSGROUP_B    : GPIOgroup = GPIOB;   break;
      case DIOGPIO_PINSGROUP_C    : GPIOgroup = GPIOC;   break;
      case DIOGPIO_PINSGROUP_D    : GPIOgroup = GPIOD;   break;
      case DIOGPIO_PINSGROUP_E    : GPIOgroup = GPIOE;   break;
      #if defined(HW_STM32F4XX)
      case DIOGPIO_PINSGROUP_F    : GPIOgroup = GPIOF;   break;
      #else    
      case DIOGPIO_PINSGROUP_F    : break;
      #endif                
    }
  
  if((mode & DIOGPIO_MODE_INTERRUPT) == DIOGPIO_MODE_INTERRUPT)
    {                    
      GPIO_initstruct.Mode = GPIO_MODE_IT_RISING_FALLING;
      GPIO_initstruct.Pull = GPIO_NOPULL;		          
    }
   else
    {           
      GPIO_initstruct.Mode       = GPIO_MODE_INPUT;      
      if((mode & DIOGPIO_MODE_INPUT)  == DIOGPIO_MODE_INPUT)   GPIO_initstruct.Mode = GPIO_MODE_INPUT;
      if((mode & DIOGPIO_MODE_OUTPUT) == DIOGPIO_MODE_OUTPUT)  GPIO_initstruct.Mode = GPIO_MODE_OUTPUT_PP;
      
      switch(entry->GetPullMode())
        {
          case DIOGPIO_PULLMODE_OFF   : GPIO_initstruct.Pull = GPIO_NOPULL;    break;
          case DIOGPIO_PULLMODE_UP    : GPIO_initstruct.Pull = GPIO_PULLUP;    break;
          case DIOGPIO_PULLMODE_DOWN  : GPIO_initstruct.Pull = GPIO_PULLDOWN;  break;    
        }   
      
      GPIO_initstruct.Speed      = GPIO_SPEED_FREQ_LOW;  
    }
    
  HAL_GPIO_Init(GPIOgroup, &GPIO_initstruct);
  
  if((mode & DIOGPIO_MODE_INTERRUPT) == DIOGPIO_MODE_INTERRUPT)
    {
      // EXTI interrupt init
      HAL_NVIC_SetPriority((IRQn_Type)entry->GetIntIRQ(), entry->GetIntPriority(), 0);
      HAL_NVIC_EnableIRQ((IRQn_Type)entry->GetIntIRQ());          
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTM32GPIO::GetValue(DIOGPIO_ENTRY* entry)
* @brief      Get value
* @ingroup    PLATFORM_STM32
*
* @param[in]  entry : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOSTM32GPIO::GetValue(DIOGPIO_ENTRY* entry)
{   
  if(!entry) return false;
  
  GPIO_TypeDef* GPIOgroup = NULL;

  switch(entry->GetGroup())
    {
      case DIOGPIO_PINSGROUP_NONE : break;
      case DIOGPIO_PINSGROUP_A    : GPIOgroup = GPIOA;   break;
      case DIOGPIO_PINSGROUP_B    : GPIOgroup = GPIOB;   break;
      case DIOGPIO_PINSGROUP_C    : GPIOgroup = GPIOC;   break;
      case DIOGPIO_PINSGROUP_D    : GPIOgroup = GPIOD;   break;
      case DIOGPIO_PINSGROUP_E    : GPIOgroup = GPIOE;   break;
      #if defined(HW_STM32F4XX)
      case DIOGPIO_PINSGROUP_F    : GPIOgroup = GPIOF;   break;
      #else
      case DIOGPIO_PINSGROUP_F    : break;
      #endif
    }

  GPIO_PinState state = HAL_GPIO_ReadPin(GPIOgroup, entry->GetPin());
  if(state == GPIO_PIN_RESET) return false;
  
  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTM32GPIO::SetValue(DIOGPIO_ENTRY* entry, bool value)
* @brief      Set value
* @ingroup    PLATFORM_STM32
*
* @param[in]  entry : 
* @param[in]  value : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOSTM32GPIO::SetValue(DIOGPIO_ENTRY* entry, bool value)
{
  if(!entry) return false;
  
  GPIO_TypeDef* GPIOgroup = NULL;

  switch(entry->GetGroup())
    {
      case DIOGPIO_PINSGROUP_NONE : break;
      case DIOGPIO_PINSGROUP_A    : GPIOgroup = GPIOA;   break;
      case DIOGPIO_PINSGROUP_B    : GPIOgroup = GPIOB;   break;
      case DIOGPIO_PINSGROUP_C    : GPIOgroup = GPIOC;   break;
      case DIOGPIO_PINSGROUP_D    : GPIOgroup = GPIOD;   break;
      case DIOGPIO_PINSGROUP_E    : GPIOgroup = GPIOE;   break;
      #if defined(HW_STM32F4XX)
      case DIOGPIO_PINSGROUP_F    : GPIOgroup = GPIOF;   break;
      #else
      case DIOGPIO_PINSGROUP_F    : break;
      #endif
    }

  HAL_GPIO_WritePin(GPIOgroup, entry->GetPin(), value?GPIO_PIN_SET:GPIO_PIN_RESET);
  
  return false;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTM32GPIO::End()
* @brief      End
* @ingroup    PLATFORM_STM32
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTM32GPIO::End()
{
  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTM32GPIO::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    PLATFORM_STM32
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTM32GPIO::Clean()
{


}
