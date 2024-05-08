/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XESP32Dir.cpp
* 
* @class      XESP32DIR
* @brief      eXtended ESP32 Directory class
* @ingroup    PLATFORM_ESP32
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

#include "XESP32Dir.h"

#include "XBase.h"
#include "XFactory.h"
#include "XFile.h"
#include "XString.h"
#include "XPath.h"
#include "XTrace.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE


#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XESP32DIR::XESP32DIR()
* @brief      Constructor
* @ingroup    PLATFORM_ESP32 
*
* --------------------------------------------------------------------------------------------------------------------*/
XESP32DIR::XESP32DIR(): XDIR()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XESP32DIR::~XESP32DIR()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_ESP32 
*
* --------------------------------------------------------------------------------------------------------------------*/
XESP32DIR::~XESP32DIR()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XESP32DIR::Exist(XCHAR* path)
* @brief      Exist
* @ingroup    PLATFORM_ESP32 
*
* @param[in]  path :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XESP32DIR::Exist(XCHAR* path)
{   
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XESP32DIR::Make(XCHAR* path, bool recursive)
* @brief      Make
* @ingroup    PLATFORM_ESP32 
*
* @param[in]  path : 
* @param[in]  recursive : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XESP32DIR::Make(XCHAR* path, bool recursive)
{
  return false;  
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XESP32DIR::ChangeTo(XCHAR* path)
* @brief      ChangeTo
* @ingroup    PLATFORM_ESP32 
*
* @param[in]  path :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XESP32DIR::ChangeTo(XCHAR* path)
{  
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XESP32DIR::Delete(XCHAR* path,bool all)
* @brief      Delete
* @ingroup    PLATFORM_ESP32 
*
* @param[in]  path :
* @param[in]  all :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XESP32DIR::Delete(XCHAR* path, bool all)
{
   return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XESP32DIR::GetActual(XPATH& xpath)
* @brief      GetActual
* @ingroup    PLATFORM_ESP32 
*
* @param[in]  xpath :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XESP32DIR::GetActual(XPATH& xpath)
{       
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XESP32DIR::FirstSearch(XCHAR* xpath, XCHAR* patternsearch, XDIRELEMENT* searchelement)
* @brief      FirstSearch
* @ingroup    PLATFORM_ESP32 
*
* @param[in]  xpath :
* @param[in]  patternsearch :
* @param[in]  searchelement :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XESP32DIR::FirstSearch(XCHAR* xpath, XCHAR* patternsearch, XDIRELEMENT* searchelement)
{    
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XESP32DIR::NextSearch(XDIRELEMENT* searchelement)
* @brief      NextSearch
* @ingroup    PLATFORM_ESP32
* 
* @param[in]  searchelement : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XESP32DIR::NextSearch(XDIRELEMENT* searchelement)
{ 
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XESP32DIR::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_ESP32 
*
* --------------------------------------------------------------------------------------------------------------------*/
void XESP32DIR::Clean()
{

}

#pragma endregion