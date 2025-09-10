/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamConfig.cpp
* 
* @class      DIOSTREAMCONFIG
* @brief      Data Input/Output Stream Config class
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

#include "DIOStreamConfig.h"

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
* @fn         DIOSTREAMCONFIG::DIOSTREAMCONFIG()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMCONFIG::DIOSTREAMCONFIG()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMCONFIG::~DIOSTREAMCONFIG()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMCONFIG::~DIOSTREAMCONFIG()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTYPE DIOSTREAMCONFIG::GetType()
* @brief      Get type
* @ingroup    DATAIO
*
* @return     DIOSTREAMTYPE :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTYPE DIOSTREAMCONFIG::GetType()
{
  return type;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMCONFIG::SetType(DIOSTREAMTYPE type)
* @brief      Set type
* @ingroup    DATAIO
*
* @param[in]  type :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMCONFIG::SetType(DIOSTREAMTYPE type)
{
  this->type = type;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMMODE DIOSTREAMCONFIG::GetMode()
* @brief      Get mode
* @ingroup    DATAIO
*
* @return     DIOSTREAMMODE :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMMODE DIOSTREAMCONFIG::GetMode()
{
  return mode;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMCONFIG::SetMode(DIOSTREAMMODE mode)
* @brief      Set mode
* @ingroup    DATAIO
*
* @param[in]  mode :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMCONFIG::SetMode(DIOSTREAMMODE mode)
{
  this->mode = mode;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMCONFIG::IsTLS()
* @brief      is TLs
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMCONFIG::IsTLS()
{
  return isTLS;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMCONFIG::SetIsTLS(bool isTLS)
* @brief      set is TLs
* @ingroup    DATAIO
* 
* @param[in]  isTLS : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMCONFIG::SetIsTLS(bool isTLS)
{
  this->isTLS = isTLS;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMCONFIG::IsServer()
* @brief      Is server
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMCONFIG::IsServer()
{
  if(mode == DIOSTREAMMODE_SERVER) return true;
  
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSTREAMCONFIG::GetSizeBufferSO()
* @brief      Get size buffer SO
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOSTREAMCONFIG::GetSizeBufferSO()
{
  return sizebufferSO;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMCONFIG::SetSizeBufferSO(XDWORD sizebufferSO)
* @brief      Set size buffer SO
* @ingroup    DATAIO
*
* @param[in]  sizebufferSO :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMCONFIG::SetSizeBufferSO(XDWORD sizebufferSO)
{
  this->sizebufferSO = sizebufferSO;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XTHREADPRIORITY DIOSTREAMCONFIG::GetThreadPriority()
* @brief      Get thread priority
* @ingroup    DATAIO
*
* @return     XTHREADPRIORITY :
*
* --------------------------------------------------------------------------------------------------------------------*/
XTHREADPRIORITY DIOSTREAMCONFIG::GetThreadPriority()
{
  return threadpriority;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMCONFIG::SetThreadPriority(XTHREADPRIORITY threadpriority)
* @brief      Set thread priority
* @ingroup    DATAIO
*
* @param[in]  threadpriority :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMCONFIG::SetThreadPriority(XTHREADPRIORITY threadpriority)
{
  this->threadpriority =  threadpriority;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSTREAMCONFIG::GetThreadWaitYield()
* @brief      Get thread wait yield
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOSTREAMCONFIG::GetThreadWaitYield()
{
  return threadwaityield;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMCONFIG::SetThreadWaitYield(XDWORD threadwaityield)
* @brief      Set thread wait yield
* @ingroup    DATAIO
*
* @param[in]  threadwaityield :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMCONFIG::SetThreadWaitYield(XDWORD threadwaityield)
{
  this->threadwaityield =  threadwaityield;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMCONFIG::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMCONFIG::Clean()
{
  type                    = DIOSTREAMTYPE_UNKNOWN;
  mode                    = DIOSTREAMMODE_NONE;
  isTLS                   = false;

  sizebufferSO            = 0;
  threadpriority          = XTHREADPRIORITY_NORMAL;
  threadwaityield         = XTHREAD_DEFAULTWAITYIELD;
}


#pragma endregion

