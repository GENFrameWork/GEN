/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStream_XEvent.cpp
* 
* @class      DIOSTREAM_XEVENT
* @brief      Data Input/Output eXtended Event class
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

#include "DIOStream_XEvent.h"

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
* @fn         DIOSTREAM_XEVENT::DIOSTREAM_XEVENT(XSUBJECT* subject, XDWORD type, XDWORD family)
* @brief      Constructor
* @ingroup    DATAIO
* 
* @param[in]  subject : 
* @param[in]  type : 
* @param[in]  family : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAM_XEVENT::DIOSTREAM_XEVENT(XSUBJECT* subject, XDWORD type, XDWORD family) : XEVENT(subject, type, family)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAM_XEVENT::~DIOSTREAM_XEVENT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAM_XEVENT::~DIOSTREAM_XEVENT()
{ 
  Clean();                          
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAM* DIOSTREAM_XEVENT::GetDIOStream()
* @brief      GetDIOStream
* @ingroup    DATAIO
* 
* @return     DIOSTREAM* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAM* DIOSTREAM_XEVENT::GetDIOStream()
{ 
  return diostream;                 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAM_XEVENT::SetDIOStream(DIOSTREAM* diostream)
* @brief      SetDIOStream
* @ingroup    DATAIO
* 
* @param[in]  diostream : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAM_XEVENT::SetDIOStream(DIOSTREAM* diostream)
{ 
  this->diostream = diostream;      
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMDEVICE* DIOSTREAM_XEVENT::GetDevice()
* @brief      GetDevice
* @ingroup    DATAIO
* 
* @return     DIOSTREAMDEVICE* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMDEVICE* DIOSTREAM_XEVENT::GetDevice()
{ 
  return &device;                   
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAM_XEVENT::SetDevice(DIOSTREAMDEVICE& device)
* @brief      SetDevice
* @ingroup    DATAIO
* 
* @param[in]  device : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAM_XEVENT::SetDevice(DIOSTREAMDEVICE& device)
{                                   

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAM_XEVENT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAM_XEVENT::Clean()
{
  diostream = NULL;
}

#pragma endregion


