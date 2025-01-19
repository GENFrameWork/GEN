/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOGPIO.cpp
* 
* @class      DIOGPIO
* @brief      Data Input/Output GPIO (General Purpose Input/Output)
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOGPIO.h"

#include "XTrace.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

DIOGPIO* DIOGPIO::instance = NULL;

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


#pragma region CLASS_DIOGPIO_ENTRY


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOGPIO_ENTRY::DIOGPIO_ENTRY()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* ---------------------------------------------------------------------------------------------------------------------*/
DIOGPIO_ENTRY::DIOGPIO_ENTRY()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOGPIO_ENTRY::~DIOGPIO_ENTRY()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* ---------------------------------------------------------------------------------------------------------------------*/
DIOGPIO_ENTRY::~DIOGPIO_ENTRY()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD DIOGPIO_ENTRY::GetID()
* @brief      Get ID
* @ingroup    DATAIO
*
* @return     XDWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOGPIO_ENTRY::GetID()
{
  return ID;
}
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOGPIO_ENTRY::SetID(XDWORD ID)
* @brief      Set ID
* @ingroup    DATAIO
*
* @param[in]  ID : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOGPIO_ENTRY::SetID(XDWORD ID)
{
  this->ID = ID;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XQWORD DIOGPIO_ENTRY::GetGPIO()
* @brief      Get GPIO
* @ingroup    DATAIO
*
* @return     XQWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XQWORD DIOGPIO_ENTRY::GetGPIO()
{
  return GPIO;
}
  

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOGPIO_ENTRY::SetGPIO(XQWORD GPIO)
* @brief      Set GPIO
* @ingroup    DATAIO
*
* @param[in]  GPIO : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOGPIO_ENTRY::SetGPIO(XQWORD GPIO)
{
  this->GPIO = GPIO;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOGPIO_PINSGROUP DIOGPIO_ENTRY::GetGroup()
* @brief      Get group
* @ingroup    DATAIO
*
* @return     DIOGPIO_PINSGROUP : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOGPIO_PINSGROUP DIOGPIO_ENTRY::GetGroup()
{
  return group;
}
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOGPIO_ENTRY::SetGroup(DIOGPIO_PINSGROUP group)
* @brief      Set group
* @ingroup    DATAIO
*
* @param[in]  group : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOGPIO_ENTRY::SetGroup(DIOGPIO_PINSGROUP group)
{
  this->group = group;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOGPIO_ENTRY::GetPin()
* @brief      Get pin
* @ingroup    DATAIO
*
* @return     XWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XWORD DIOGPIO_ENTRY::GetPin()
{
  return pin;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOGPIO_ENTRY::SetPin(XWORD pin)
* @brief      Set pin
* @ingroup    DATAIO
*
* @param[in]  pin : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOGPIO_ENTRY::SetPin(XWORD pin)
{
  this->pin = pin;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOGPIO_ENTRY::GetMode()
* @brief      Get mode
* @ingroup    DATAIO
*
* @return     XWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XWORD DIOGPIO_ENTRY::GetMode()
{
  return mode;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOGPIO_ENTRY::SetMode(XWORD mode)
* @brief      Set mode
* @ingroup    DATAIO
*
* @param[in]  mode : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOGPIO_ENTRY::SetMode(XWORD mode)
{
  this->mode = mode;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOGPIO_PULLMODE DIOGPIO_ENTRY::GetPullMode()
* @brief      Get pull mode
* @ingroup    DATAIO
*
* @return     DIOGPIO_PULLMODE : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOGPIO_PULLMODE DIOGPIO_ENTRY::GetPullMode()
{
  return pullmode;
}
  

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOGPIO_ENTRY::SetPullMode(DIOGPIO_PULLMODE pullmode)
* @brief      Set pull mode
* @ingroup    DATAIO
*
* @param[in]  pullmode : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOGPIO_ENTRY::SetPullMode(DIOGPIO_PULLMODE pullmode)
{
  this->pullmode = pullmode;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOGPIO_INT_FUNCPTR DIOGPIO_ENTRY::GetIntFunctionPointer()
* @brief      Get int function pointer
* @ingroup    DATAIO
*
* @return     DIOGPIO_INT_FUNCPTR : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOGPIO_INT_FUNCPTR DIOGPIO_ENTRY::GetIntFunctionPointer()
{
  return int_funcptr;
}

   
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOGPIO_ENTRY::SetIntFunctionPointer(DIOGPIO_INT_FUNCPTR int_funcptr)
* @brief      Set int function pointer
* @ingroup    DATAIO
*
* @param[in]  int_funcptr : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOGPIO_ENTRY::SetIntFunctionPointer(DIOGPIO_INT_FUNCPTR int_funcptr)
{
  this->int_funcptr = int_funcptr;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void* DIOGPIO_ENTRY::GetIntParamPointer()
* @brief      Get int param pointer
* @ingroup    DATAIO
*
* @return     void* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void* DIOGPIO_ENTRY::GetIntParamPointer()
{
  return int_paramptr;    
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOGPIO_ENTRY::SetIntParamPointer(void* int_paramptr)
* @brief      Set int param pointer
* @ingroup    DATAIO
*
* @param[in]  int_paramptr : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOGPIO_ENTRY::SetIntParamPointer(void* int_paramptr)
{
  this->int_paramptr = int_paramptr;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD DIOGPIO_ENTRY::GetIntIRQ()
* @brief      Get int IRQ
* @ingroup    DATAIO
*
* @return     XDWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOGPIO_ENTRY::GetIntIRQ()
{
  return int_IRQ;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOGPIO_ENTRY::SetIntIRQ(XDWORD int_IRQ)
* @brief      Set int IRQ
* @ingroup    DATAIO
*
* @param[in]  int_IRQ : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOGPIO_ENTRY::SetIntIRQ(XDWORD int_IRQ)
{
  this->int_IRQ = int_IRQ;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD DIOGPIO_ENTRY::GetIntPriority()
* @brief      Get int priority
* @ingroup    DATAIO
*
* @return     XDWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOGPIO_ENTRY::GetIntPriority()
{
  return int_priority;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOGPIO_ENTRY::SetIntPriority(XDWORD int_priority)
* @brief      Set int priority
* @ingroup    DATAIO
*
* @param[in]  int_priority : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOGPIO_ENTRY::SetIntPriority(XDWORD int_priority)
{
  this->int_priority = int_priority;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOGPIO_ENTRY::DebugTrace()
* @brief      Debug trace
* @ingroup    DATAIO
*
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOGPIO_ENTRY::DebugTrace()
{
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("-- GPIO Entry ---------------------------------"));
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("ID            : [%d]")    , ID);
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("GPIO          : [%lld]")  , GPIO);
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Pin           : [%d]")    , pin);
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Group         : [%d]")    , group);    
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Mode          : [%08X]")  , mode);
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Pull Mode     : [%d]")    , pullmode); 
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Int Func      : [%X08]")  , int_funcptr);                                                          
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Int IRQ       : [%X08]")  , int_IRQ);                                                          
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Int Priority  : [%X08]")  , int_priority);                                                          
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOGPIO_ENTRY::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOGPIO_ENTRY::Clean()
{
  ID               = 0;
  GPIO             = DIOGPIO_INVALID;   
  group            = DIOGPIO_PINSGROUP_NONE;
  pin              = 0;    
  mode             = DIOGPIO_MODE_NONE;   
  pullmode         = DIOGPIO_PULLMODE_OFF;
  
  int_funcptr      = NULL;
  int_paramptr    =  NULL;
  int_IRQ          = 0;
  int_priority     = 0;                                                              
}


#pragma endregion


#pragma region CLASS_DIOGPIO


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOGPIO::DIOGPIO()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOGPIO::DIOGPIO()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOGPIO::~DIOGPIO()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOGPIO::~DIOGPIO()
{ 
  End();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOGPIO::GetIsInstanced()
* @brief      Get is instanced
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOGPIO::GetIsInstanced()
{
  return instance!=NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOGPIO& DIOGPIO::GetInstance()
* @brief      Get instance
* @ingroup    DATAIO
*
* @return     DIOGPIO& :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOGPIO& DIOGPIO::GetInstance()
{
  if(!instance) instance = new DIOGPIO();

  return (*instance);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOGPIO::SetInstance(DIOGPIO* _instance)
* @brief      Set instance
* @ingroup    DATAIO
*
* @param[in]  _instance :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOGPIO::SetInstance(DIOGPIO* _instance)
{
  if(!_instance) return false;

  instance = _instance;

  return (instance)?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOGPIO::DelInstance()
* @brief      Del instance
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOGPIO::DelInstance()
{
  if(instance)
    {
      delete instance;
      instance = NULL;

      return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<DIOGPIO_ENTRY*>* DIOGPIO::GPIOEntry_GetAll()
* @brief      GPIO entry get all
* @ingroup    DATAIO
* 
* @return     XVECTOR<DIOGPIO_ENTRY*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<DIOGPIO_ENTRY*>* DIOGPIO::GPIOEntry_GetAll()
{
  return &GPIOentrys;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOGPIO_ENTRY* DIOGPIO::GPIOEntry_CreateByPin(XDWORD ID, XWORD pin, DIOGPIO_PINSGROUP group)
* @brief      GPIO entry create by pin
* @ingroup    DATAIO
*
* @param[in]  ID : 
* @param[in]  pin : 
* @param[in]  group : 
* 
* @return     DIOGPIO_ENTRY* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOGPIO_ENTRY* DIOGPIO::GPIOEntry_CreateByPin(XDWORD ID, XWORD pin, DIOGPIO_PINSGROUP group)
{
  DIOGPIO_ENTRY* entry = new DIOGPIO_ENTRY();
  if(!entry) return NULL;

  entry->SetID(ID);
  entry->SetPin(pin);  
  entry->SetGroup(group);
  entry->SetGPIO(0); 

  GPIOentrys.Add(entry);

  return entry;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOGPIO_ENTRY* DIOGPIO::GPIOEntry_CreateByGPIO(XDWORD ID, XQWORD GPIO)
* @brief      GPIO entry create by GPIO
* @ingroup    DATAIO
*
* @param[in]  ID : 
* @param[in]  GPIO : 
* 
* @return     DIOGPIO_ENTRY* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOGPIO_ENTRY* DIOGPIO::GPIOEntry_CreateByGPIO(XDWORD ID, XQWORD GPIO)
{
  DIOGPIO_ENTRY* entry = new DIOGPIO_ENTRY();
  if(!entry) return NULL;

  entry->SetID(ID);  
  entry->SetPin(0); 
  entry->SetGroup(DIOGPIO_PINSGROUP_NONE);
  entry->SetGPIO(GPIO);  
 
  GPIOentrys.Add(entry);

  return entry;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOGPIO_ENTRY* DIOGPIO::GPIOEntry_Create(XDWORD ID, XWORD pin, XQWORD GPIO)
* @brief      GPIO entry create
* @ingroup    DATAIO
*
* @param[in]  ID : 
* @param[in]  pin : 
* @param[in]  GPIO : 
* 
* @return     DIOGPIO_ENTRY* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOGPIO_ENTRY* DIOGPIO::GPIOEntry_Create(XDWORD ID, XWORD pin, XQWORD GPIO)
{
  DIOGPIO_ENTRY* entry = new DIOGPIO_ENTRY();
  if(!entry) return NULL;
  
  entry->SetID(ID);
  entry->SetPin(pin); 
  entry->SetGroup(DIOGPIO_PINSGROUP_NONE); 
  entry->SetGPIO(GPIO);

  GPIOentrys.Add(entry);

  return entry;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOGPIO_ENTRY* DIOGPIO::GPIOEntry_GetByID(XDWORD ID)
* @brief      GPIO entry get by ID
* @ingroup    DATAIO
*
* @param[in]  ID : 
* 
* @return     DIOGPIO_ENTRY* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOGPIO_ENTRY* DIOGPIO::GPIOEntry_GetByID(XDWORD ID)
{
  if(GPIOentrys.IsEmpty()) return NULL;

  for(XDWORD c=0; c<GPIOentrys.GetSize(); c++)
    {
      DIOGPIO_ENTRY* entry = GPIOentrys.Get(c);
      if(entry)
        {
          if(entry->GetID() == ID) return entry;
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOGPIO_ENTRY* DIOGPIO::GPIOEntry_GetByPin(XWORD pin, DIOGPIO_PINSGROUP group)
* @brief      GPIO entry get by pin
* @ingroup    DATAIO
*
* @param[in]  pin : 
* @param[in]  group : 
* 
* @return     DIOGPIO_ENTRY* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOGPIO_ENTRY* DIOGPIO::GPIOEntry_GetByPin(XWORD pin, DIOGPIO_PINSGROUP group)    
{
  if(GPIOentrys.IsEmpty()) return NULL;

  for(XDWORD c=0; c<GPIOentrys.GetSize(); c++)
    {
      DIOGPIO_ENTRY* entry = GPIOentrys.Get(c);
      if(entry)
        {
          if((entry->GetPin() == pin) && (entry->GetGroup() == group)) return entry;
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOGPIO_ENTRY* DIOGPIO::GPIOEntry_GetInterruptByPin(XWORD pin)
* @brief      GPIO entry get interrupt by pin
* @ingroup    DATAIO
*
* @param[in]  pin : 
* 
* @return     DIOGPIO_ENTRY* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOGPIO_ENTRY* DIOGPIO::GPIOEntry_GetInterruptByPin(XWORD pin)
{
  if(GPIOentrys.IsEmpty()) return NULL;

  for(XDWORD c=0; c<GPIOentrys.GetSize(); c++)
    {
      DIOGPIO_ENTRY* entry = GPIOentrys.Get(c);
      if(entry)
        {
          if((entry->GetPin() == pin) && ((entry->GetMode() & DIOGPIO_MODE_INTERRUPT) == DIOGPIO_MODE_INTERRUPT)) return entry;
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOGPIO_ENTRY* DIOGPIO::GPIOEntry_GetByGPIO(XQWORD GPIO)
* @brief      GPIO entry get by GPIO
* @ingroup    DATAIO
*
* @param[in]  GPIO : 
* 
* @return     DIOGPIO_ENTRY* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOGPIO_ENTRY* DIOGPIO::GPIOEntry_GetByGPIO(XQWORD GPIO)    
{
  if(GPIOentrys.IsEmpty()) return NULL;

  for(XDWORD c=0; c<GPIOentrys.GetSize(); c++)
    {
      DIOGPIO_ENTRY* entry = GPIOentrys.Get(c);
      if(entry)
        {
          if(entry->GetGPIO() == GPIO) return entry;
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOGPIO::GPIOEntry_DeleteAll()
* @brief      GPIO entry delete all
* @ingroup    DATAIO
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOGPIO::GPIOEntry_DeleteAll()
{
  if(GPIOentrys.IsEmpty()) return false;

  GPIOentrys.DeleteContents();
  GPIOentrys.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOGPIO::GPIOEntry_SetIDByPin(XDWORD ID, XWORD pin, DIOGPIO_PINSGROUP group)
* @brief      GPIO entry set ID by pin
* @ingroup    DATAIO
*
* @param[in]  ID : 
* @param[in]  pin : 
* @param[in]  group : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOGPIO::GPIOEntry_SetIDByPin(XDWORD ID, XWORD pin, DIOGPIO_PINSGROUP group)
{
  DIOGPIO_ENTRY* entry = GPIOEntry_GetByPin(pin, group);
  if(!entry) return false;

  entry->SetID(ID);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOGPIO::GPIOEntry_SetIDByGPIO(XDWORD ID, XQWORD GPIO)
* @brief      GPIO entry set ID by GPIO
* @ingroup    DATAIO
*
* @param[in]  ID : 
* @param[in]  GPIO : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOGPIO::GPIOEntry_SetIDByGPIO(XDWORD ID, XQWORD GPIO)
{
  DIOGPIO_ENTRY* entry = GPIOEntry_GetByGPIO(GPIO);
  if(!entry) return false;

  entry->SetID(ID);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOGPIO::Ini(XPATH* xpath)
* @brief      Ini
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOGPIO::Ini()
{
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOGPIO::GetMode(XDWORD ID)
* @brief      Get mode
* @ingroup    DATAIO
*
* @param[in]  ID : 
* 
* @return     XWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XWORD DIOGPIO::GetMode(XDWORD ID)
{
  DIOGPIO_ENTRY* entry = GPIOEntry_GetByID(ID);
  if(!entry) return 0;

  return GetMode(entry);  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOGPIO::SetMode(XDWORD ID, XWORD mode)
* @brief      Set mode
* @ingroup    DATAIO
*
* @param[in]  ID : 
* @param[in]  mode : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOGPIO::SetMode(XDWORD ID, XWORD mode)
{
  DIOGPIO_ENTRY* entry = GPIOEntry_GetByID(ID);
  if(!entry) return false;
  
  SetMode(entry, mode);  

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOGPIO::GetValue(XDWORD ID)
* @brief      Get value
* @ingroup    DATAIO
*
* @param[in]  ID : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOGPIO::GetValue(XDWORD ID)
{
  DIOGPIO_ENTRY* entry = GPIOEntry_GetByID(ID);
  if(!entry) return false;

  return GetValue(entry);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOGPIO::SetValue(XDWORD ID, bool value)
* @brief      Set value
* @ingroup    DATAIO
*
* @param[in]  ID : 
* @param[in]  value : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOGPIO::SetValue(XDWORD ID, bool value)
{
  DIOGPIO_ENTRY* entry = GPIOEntry_GetByID(ID);
  if(!entry) return false;

  return SetValue(entry, value);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOGPIO::GetMode(DIOGPIO_ENTRY* entry)
* @brief      Get mode
* @ingroup    DATAIO
*
* @param[in]  entry : 
* 
* @return     XWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XWORD DIOGPIO::GetMode(DIOGPIO_ENTRY* entry)
{
  if(!entry) return 0;

  return entry->GetMode();  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOGPIO::SetMode(DIOGPIO_ENTRY* entry, XWORD mode)
* @brief      Set mode
* @ingroup    DATAIO
*
* @param[in]  entry : 
* @param[in]  mode : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOGPIO::SetMode(DIOGPIO_ENTRY* entry, XWORD mode)
{
  if(!entry) return false;

  entry->SetMode(mode);  

  return true;
}  


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOGPIO::GetValue(DIOGPIO_ENTRY* entry)
* @brief      Get value
* @ingroup    DATAIO
*
* @param[in]  entry : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOGPIO::GetValue(DIOGPIO_ENTRY* entry)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOGPIO::SetValue(DIOGPIO_ENTRY* entry, bool value)
* @brief      Set value
* @ingroup    DATAIO
*
* @param[in]  entry : 
* @param[in]  value : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOGPIO::SetValue(DIOGPIO_ENTRY* entry, bool value)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOGPIO::End()
* @brief      End
* @ingroup    DATAIO
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOGPIO::End()
{
  GPIOEntry_DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOGPIO::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOGPIO::Clean()
{
  instance            = NULL;
}


#pragma endregion


#pragma endregion

