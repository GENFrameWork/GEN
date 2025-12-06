/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTLSMessages.cpp
* 
* @class      DIOSTREAMTLSMESSAGES
* @brief      Data Input/Output Stream TLS (Transport Layer Security) Messages class
* @ingroup    DATAIO
* 
* @copyright  EndoraSoft. All rights reserved.
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
#pragma region PRECOMPILATION_DEFINE_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOStreamTLSMessages.h"

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


#pragma region CLASS_DIOSTREAMTLS_MSG_INTERFACE


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_INTERFACE::DIOSTREAMTLS_MSG_INTERFACE()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_INTERFACE::DIOSTREAMTLS_MSG_INTERFACE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLS_MSG_INTERFACE::~DIOSTREAMTLS_MSG_INTERFACE()
* @brief      Destructor of class
* @ingroup    DATAIO
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_MSG_INTERFACE::~DIOSTREAMTLS_MSG_INTERFACE()
{ 
  Clean();
}
       
    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD DIOSTREAMTLS_MSG_INTERFACE::GetLengthBuffer()
* @brief      get length buffer
* @ingroup    DATAIO
* 
* @return     XDWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOSTREAMTLS_MSG_INTERFACE::GetLengthBuffer()
{ 
  XBUFFER data;
                                                 
  SetToBuffer(data, false);

  return data.GetSize();
}

  
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTLS_MSG_INTERFACE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @ingroup    DATAIO
* @note       INTERNAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS_MSG_INTERFACE::Clean()
{

}


#pragma endregion


#pragma endregion


