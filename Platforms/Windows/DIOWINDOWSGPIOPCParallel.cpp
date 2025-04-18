/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOWINDOWSGPIOPCParallel.cpp
* 
* @class      DIOWINDOWSGPIOPCPARALLEL
* @brief      WINDOWS Data Input/Output GPIO (General Purpose Input/Output) PC Parallel
* @ingroup    PLATFORM_WINDOWS
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

#include "DIOWINDOWSGPIOPCParallel.h"

#include "XPath.h"

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
* @fn         DIOWINDOWSGPIOPCPARALLEL::DIOWINDOWSGPIOPCPARALLEL()
* @brief      Constructor of class
* @ingroup    PLATFORM_WINDOWS
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOWINDOWSGPIOPCPARALLEL::DIOWINDOWSGPIOPCPARALLEL(): DIOGPIO()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOWINDOWSGPIOPCPARALLEL::~DIOWINDOWSGPIOPCPARALLEL()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_WINDOWS
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOWINDOWSGPIOPCPARALLEL::~DIOWINDOWSGPIOPCPARALLEL()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XPATH* DIOWINDOWSGPIOPCPARALLEL::GetPath()
* @brief      Get path
* @ingroup    PLATFORM_WINDOWS
* 
* @return     XPATH* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XPATH* DIOWINDOWSGPIOPCPARALLEL::GetPath()
{
  return xpath;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOWINDOWSGPIOPCPARALLEL::SetPath(XPATH* xpath)
* @brief      Set path
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  xpath :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWINDOWSGPIOPCPARALLEL::SetPath(XPATH* xpath)
{
  this->xpath = xpath;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOWINDOWSGPIOPCPARALLEL::GetParallelPort()
* @brief      Get parallel port
* @ingroup    PLATFORM_WINDOWS
*
* @return     XWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XWORD DIOWINDOWSGPIOPCPARALLEL::GetParallelPort()
{
  return port;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOWINDOWSGPIOPCPARALLEL::SetParallelPort(XWORD port)
* @brief      Set parallel port
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  port : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void DIOWINDOWSGPIOPCPARALLEL::SetParallelPort(XWORD port)
{
  this->port = port;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWINDOWSGPIOPCPARALLEL::Ini()
* @brief      Ini
* @ingroup    PLATFORM_WINDOWS
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWINDOWSGPIOPCPARALLEL::Ini()
{
  XPATH xpathDLL;

  for(int c=0; c<2; c++)
    {
      if(xpath)
        {
          xpathDLL.Add((*xpath));
          xpathDLL.Slash_Add();
        }

      switch(c)
        {
          case 0: xpathDLL.Add("inpout32.dll"); break;
          case 1: xpathDLL.Add("inpout64.dll"); break;
        }

      handleDLL = LoadLibrary(xpathDLL.Get());
      if(handleDLL) break;
    }

  if(!handleDLL) return false;

  outport       = (DIOWGPIOOUT)            GetProcAddress(handleDLL, "Out32");
  inpport       = (DIOWGPIOINP)            GetProcAddress(handleDLL, "Inp32");;
  isdriveropen  = (DIOWGPIOISDRIVEROPEN)   GetProcAddress(handleDLL, "IsInpOutDriverOpen");
  isdriver64bit = (DIOWGPIOISDRIVER64BITS) GetProcAddress(handleDLL, "IsXP64Bit");

  if((!outport) || (!inpport) || (!isdriveropen) || (!isdriver64bit))
    {
      FreeLibrary(handleDLL);
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD DIOWINDOWSGPIOPCPARALLEL::GetMode(DIOGPIO_ENTRY* entry)
* @brief      Get mode
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  entry : 
* 
* @return     XWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XWORD DIOWINDOWSGPIOPCPARALLEL::GetMode(DIOGPIO_ENTRY* entry)
{
  return (DIOGPIO_MODE_INPUT | DIOGPIO_MODE_OUTPUT);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWINDOWSGPIOPCPARALLEL::SetMode(DIOGPIO_ENTRY* entry, XWORD mode)
* @brief      Set mode
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  entry : 
* @param[in]  mode : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOWINDOWSGPIOPCPARALLEL::SetMode(DIOGPIO_ENTRY* entry, XWORD mode)
{
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWINDOWSGPIOPCPARALLEL::GetValue(DIOGPIO_ENTRY* entry)
* @brief      Get value
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  entry : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOWINDOWSGPIOPCPARALLEL::GetValue(DIOGPIO_ENTRY* entry)
{
  if(!entry) return false;

  PC_Get(port, dataport);

  return (dataport&~(0x01 << entry->GetPin()))?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWINDOWSGPIOPCPARALLEL::SetValue(DIOGPIO_ENTRY* entry, bool value)
* @brief      Set value
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  entry : 
* @param[in]  value : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOWINDOWSGPIOPCPARALLEL::SetValue(DIOGPIO_ENTRY* entry, bool value)
{
  if(!entry) return false;

  PC_Get(port, dataport);
  PC_Set(port ,value?(dataport|(0x01 << entry->GetPin())):(dataport & ~(0x01 << entry->GetPin())));

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWINDOWSGPIOPCPARALLEL::End()
* @brief      End
* @ingroup    PLATFORM_WINDOWS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWINDOWSGPIOPCPARALLEL::End()
{
  if(!handleDLL) return false;
  FreeLibrary(handleDLL);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWINDOWSGPIOPCPARALLEL::PC_Get(XDWORD nport, XBYTE& data)
* @brief      PC get
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  nport :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWINDOWSGPIOPCPARALLEL::PC_Get(XDWORD nport, XBYTE& data)
{
  if(!handleDLL)      return false;
  if(!isdriveropen()) return false;

  WORD _data = inpport(nport);
  data = (XBYTE)(_data & 0x00FF);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWINDOWSGPIOPCPARALLEL::PC_Set(XDWORD nport, XBYTE data)
* @brief      PC set
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  nport :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWINDOWSGPIOPCPARALLEL::PC_Set(XDWORD nport, XBYTE data)
{
  if(!handleDLL)      return false;
  if(!isdriveropen()) return false;

  WORD _data = data;
  outport(nport, _data);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOWINDOWSGPIOPCPARALLEL::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    PLATFORM_WINDOWS
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWINDOWSGPIOPCPARALLEL::Clean()
{ 
  xpath           = NULL;
  port            = DIOWINDOWSGPIOPCPARALLEL_PORT1;
  handleDLL       = NULL;

  outport         = NULL;
  inpport         = NULL;

  isdriveropen    = NULL;
  isdriver64bit   = NULL;
}


#pragma endregion








