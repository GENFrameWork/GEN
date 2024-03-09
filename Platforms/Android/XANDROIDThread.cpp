/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XANDROIDThread.cpp
* 
* @class      XANDROIDTHREAD
* @brief      ANDROID eXtended Thread and Mutex class
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

/*---- PRECOMPILATION INCLUDES ----------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XANDROIDThread.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


#pragma region CLASS_XANDROIDMUTEX


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XANDROIDMUTEX::XANDROIDMUTEX()
* @brief      Constructor
* @ingroup    PLATFORM_ANDROID
*
* --------------------------------------------------------------------------------------------------------------------*/
XANDROIDMUTEX::XANDROIDMUTEX()
{

}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XANDROIDMUTEX::~XANDROIDMUTEX()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_ANDROID
*
* --------------------------------------------------------------------------------------------------------------------*/
XANDROIDMUTEX::~XANDROIDMUTEX()
{

}


#pragma endregion


#pragma region CLASS_XANDROIDTHREAD


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XANDROIDTHREAD::XANDROIDTHREAD(XTHREADGROUPID groupID, XCHAR* ID, XTHREADFUNCTION function, void* param)
* @brief      Constructor
* @ingroup    PLATFORM_ANDROID
*
* @param[in]  groupID : group ID of threads
* @param[in]  ID : ID for thread
* @param[in]  function : callback to thread
* @param[in]  param : param to callback
**
* --------------------------------------------------------------------------------------------------------------------*/
XANDROIDTHREAD::XANDROIDTHREAD(XTHREADGROUPID groupID, XCHAR* ID, XTHREADFUNCTION function, void* param) : XLINUXTHREAD(groupID, ID, function, param)
{

}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XANDROIDTHREAD::~XANDROIDTHREAD()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_ANDROID
*
* --------------------------------------------------------------------------------------------------------------------*/
XANDROIDTHREAD::~XANDROIDTHREAD()
{

}


#pragma endregion


#pragma endregion

