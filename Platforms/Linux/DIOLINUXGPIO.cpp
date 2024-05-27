/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOLINUXGPIO.cpp
* 
* @class      DIOLINUXGPIO
* @brief      LINUX Data Input/Output GPIO (General Purpose Input/Output) 
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOLINUXGPIO.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "XFactory.h"
#include "XTrace.h"
#include "XFile.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOLINUXGPIO::DIOLINUXGPIO()
* @brief      Constructor
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOLINUXGPIO::DIOLINUXGPIO() : DIOGPIO()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOLINUXGPIO::~DIOLINUXGPIO()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOLINUXGPIO::~DIOLINUXGPIO()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXGPIO::Ini(XPATH* xpath)
* @brief      Ini
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXGPIO::Ini()
{
  return true;
}
   

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXGPIO::SetMode(DIOGPIO_ENTRY* entry, XWORD mode)
* @brief      SetMode
* @ingroup    PLATFORM_LINUX
*
* @param[in]  entry : 
* @param[in]  mode : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXGPIO::SetMode(DIOGPIO_ENTRY* entry, XWORD mode)
{
  if(!entry) return false;

	if(GPIOExport_Add(entry->GetGPIO()))
		{			
			GPIO_Export(entry->GetGPIO(), true);		
		}

	bool isinput = true;

  if((mode & DIOGPIO_MODE_INPUT)  == DIOGPIO_MODE_INPUT)   
		{
			isinput = true;
		}

  if((mode & DIOGPIO_MODE_OUTPUT) == DIOGPIO_MODE_OUTPUT)  
		{
			isinput = false;
		}
	
	return GPIO_SetDirection(entry->GetGPIO(), isinput); 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXGPIO::GetValue(DIOGPIO_ENTRY* entry)
* @brief      GetValue
* @ingroup    PLATFORM_LINUX
*
* @param[in]  entry : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXGPIO::GetValue(DIOGPIO_ENTRY* entry)
{
	if(!entry) return false;

	if(GPIOExport_Add(entry->GetGPIO())) 
		{
			GPIO_Export(entry->GetGPIO(), true);
		}

	return GPIO_GetData(entry->GetGPIO());	
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOLINUXGPIO::SetValue(DIOGPIO_ENTRY* entry, bool value)
* @brief      SetValue
* @ingroup    PLATFORM_LINUX
*
* @param[in]  entry : 
* @param[in]  value : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXGPIO::SetValue(DIOGPIO_ENTRY* entry, bool value)
{
	if(!entry) return false;

	if(GPIOExport_Add(entry->GetGPIO())) 
		{
			GPIO_Export(entry->GetGPIO(), true);
		}

  return GPIO_SetData(entry->GetGPIO(), value);	
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXGPIO::End()
* @brief      End
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXGPIO::End()
{
	XVECTOR<XDWORD>* GPIO_exports = GPIOExport_Get();   
	if(GPIO_exports)
		{
			for(XDWORD c=0; c<GPIO_exports->GetSize(); c++)
				{		
					XDWORD GPIO = GPIO_exports->Get(c);
					if(GPIO) 
						{
							GPIO_Export(GPIO, false);												
						}
				}

			GPIOExport_DeleteAll();
		}

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXGPIO::GPIOExport_Add(XDWORD GPIO)
* @brief      GPIOExport_Add
* @ingroup    PLATFORM_LINUX
*
* @param[in]  GPIO : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXGPIO::GPIOExport_Add(XDWORD GPIO)
{
  if(GPIOExport_IsExport(GPIO))  
		{
			return false;
		}

  return GPIOexports.Add(GPIO);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXGPIO::GPIOExport_IsExport(XDWORD GPIO)
* @brief      GPIOExport_IsExport
* @ingroup    PLATFORM_LINUX
*
* @param[in]  GPIO : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXGPIO::GPIOExport_IsExport(XDWORD GPIO)
{
  for(XDWORD c=0; c<GPIOexports.GetSize(); c++)
    {
      XDWORD _GPIO = GPIOexports.Get(c);    
      if(GPIO == _GPIO) 
				{
					return true;
				}
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<XDWORD>* DIOLINUXGPIO::GPIOExport_Get()
* @brief      GPIOexport_Get
* @ingroup    PLATFORM_LINUX
*
* @return     XVECTOR<XDWORD>* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XDWORD>* DIOLINUXGPIO::GPIOExport_Get()
{
  return &GPIOexports;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXGPIO::GPIOExport_DeleteAll()
* @brief      GPIOExport_DeleteAll
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXGPIO::GPIOExport_DeleteAll()    
{
  if(GPIOexports.IsEmpty()) 
		{
			return false;
		}

  GPIOexports.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXGPIO::GPIO_Export(XDWORD GPIO, bool isexport)
* @brief      GPIO_Export
* @ingroup    PLATFORM_LINUX
*
* @param[in]  GPIO : 
* @param[in]  isexport : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXGPIO::GPIO_Export(XDWORD GPIO, bool isexport)
{	
	XPATH			xpath;
	XSTRING		GPIOdatastr;
	bool			status = false;

	xpath.Format(__L("%s/%s"), DIOLINUXGPIO_PATH, (isexport?__L("export"):__L("unexport")));

  XBUFFER pathchar;
  
  xpath.ConvertToASCII(pathchar);	
	int fd = open(pathchar.GetPtrChar(), O_WRONLY);
	if(fd >= 0) 
		{					
			GPIOdatastr.Format(__L("%d"), GPIO);
			
      XBUFFER GPIOdatachar;
      
      GPIOdatastr.ConvertToASCII(GPIOdatachar);      
			write(fd, (char*)GPIOdatachar.Get(), GPIOdatastr.GetSize());
			
			close(fd);
			
			status = true;
		} 
	
	//XTRACE_PRINTCOLOR((status?XTRACE_COLOR_BLUE:XTRACE_COLOR_RED), __L("[Linux GPIO] Export %s %s"), xpath.Get(), GPIOdatastr.Get());

	return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXGPIO::GPIO_SetDirection(XDWORD GPIO, bool isinput)
* @brief      GPIO_SetDirection
* @ingroup    PLATFORM_LINUX
*
* @param[in]  GPIO : 
* @param[in]  isinput : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXGPIO::GPIO_SetDirection(XDWORD GPIO, bool isinput)
{
	XPATH			xpath;
	XSTRING		GPIOdatastr;
	bool			status = false;
		
	xpath.Format(__L("%s/gpio%d/direction"), DIOLINUXGPIO_PATH, GPIO);
	  
  XBUFFER pathchar;
  
  xpath.ConvertToASCII(pathchar);	
	int fd = open(pathchar.GetPtrChar(), O_WRONLY);
	if(fd >= 0) 
		{					
			GPIOdatastr.Format(__L("%s"), (isinput?__L("in"):__L("out")));

      XBUFFER GPIOdatachar;
      
      GPIOdatastr.ConvertToASCII(GPIOdatachar);			      
			write(fd, (char*)GPIOdatachar.Get(), GPIOdatastr.GetSize());
			
			close(fd);
	
			status = true;
		}
	
	//XTRACE_PRINTCOLOR((status?XTRACE_COLOR_BLUE:XTRACE_COLOR_RED), __L("[Linux GPIO] Set Direction %s %s"), xpath.Get(), GPIOdatastr.Get());

	return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXGPIO::GPIO_GetData(XDWORD GPIO)
* @brief      GPIO_GetData
* @ingroup    PLATFORM_LINUX
*
* @param[in]  GPIO : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXGPIO::GPIO_GetData(XDWORD GPIO)
{
	XPATH   xpath;
	bool	  status = false;		
	
	xpath.Format(__L("%s/gpio%d/value"), DIOLINUXGPIO_PATH, GPIO);
	
  XBUFFER pathchar;
  
  xpath.ConvertToASCII(pathchar);  
	int fd = open(pathchar.GetPtrChar(), O_RDONLY);
	if(fd >= 0) 
		{
			char ch;

			read(fd, &ch, 1);

			if(ch == '1') 
				{
					status = true;
				}
				
			close(fd);				
		}
	
	//XTRACE_PRINTCOLOR((status?XTRACE_COLOR_BLUE:XTRACE_COLOR_RED), __L("[Linux GPIO] Get Data %s %s"), xpath.Get(), status?__L("1"):__L("0"));

	return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOLINUXGPIO::GPIO_SetData(XDWORD GPIO, bool on)
* @brief      GPIO_SetData
* @ingroup    PLATFORM_LINUX
*
* @param[in]  GPIO : 
* @param[in]  on : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOLINUXGPIO::GPIO_SetData(XDWORD GPIO, bool on)
{
	XPATH			xpath;
	XSTRING		GPIOdatastr;	
	bool			status = false;
			
	xpath.Format(__L("%s/gpio%d/value"), DIOLINUXGPIO_PATH, GPIO);

  XBUFFER pathchar;
  
  xpath.ConvertToASCII(pathchar);
	int fd = open(pathchar.GetPtrChar(), O_WRONLY);
	if(fd >= 0) 
		{		
			GPIOdatastr.Format(__L("%s"), (on?__L("1"):__L("0")));

      XBUFFER GPIOdatachar;
      
      GPIOdatastr.ConvertToASCII(GPIOdatachar);			
			write(fd, (char*)GPIOdatachar.Get(), GPIOdatastr.GetSize());

			close(fd);
			
			status = true;
		}

	//XTRACE_PRINTCOLOR((status?XTRACE_COLOR_BLUE:XTRACE_COLOR_RED), __L("[Linux GPIO] Set Data %s %s"), xpath.Get(), GPIOdatastr.Get());		

	return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOLINUXGPIO::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOLINUXGPIO::Clean()
{

}


#pragma endregion






