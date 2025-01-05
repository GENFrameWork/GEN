/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DB_SQL_Result.cpp
* 
* @class      DB_SQL_RESULT
* @brief      DataBase SQL Result class
* @ingroup    DATABASES
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


#ifdef DB_SQL_ACTIVE


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DB_SQL_Result.h"

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
* @fn         DB_SQL_RESULT::DB_SQL_RESULT()
* @brief      Constructor
* @ingroup    DATABASES
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_RESULT::DB_SQL_RESULT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_RESULT::~DB_SQL_RESULT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATABASES
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_RESULT::~DB_SQL_RESULT()
{
  row.Clear();

  header.DeleteContents();
  header.DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_ROW* DB_SQL_RESULT::GetRow()
* @brief      GetRow
* @ingroup    DATABASES
*
* @return     DB_SQL_ROW* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_ROW* DB_SQL_RESULT::GetRow()
{
  return &row;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_ROWHEADER* DB_SQL_RESULT::GetHeader()
* @brief      GetHeader
* @ingroup    DATABASES
*
* @return     DB_SQL_ROWHEADER* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_ROWHEADER* DB_SQL_RESULT::GetHeader()
{
  return &header;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DB_SQL_RESULT::GetCurrentRowIndex()
* @brief      GetCurrentRowIndex
* @ingroup    DATABASES
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DB_SQL_RESULT::GetCurrentRowIndex()
{
  return currentrow;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DB_SQL_RESULT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATABASES
*
* --------------------------------------------------------------------------------------------------------------------*/
void DB_SQL_RESULT::Clean()
{
  currentrow      = 0;
  initialized     = false;
}


#pragma endregion


#endif

