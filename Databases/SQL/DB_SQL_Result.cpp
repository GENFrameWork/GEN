/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DB_SQL_Result.cpp
*
* @class      DB_SQL_RESULT
* @brief      Data Base SQL Holds Result class
* @ingroup    DATABASESSQL
*
* @author     Diego Martinez Ruiz de Gaona
* @date       01/03/2016 12:00
*
* @copyright  Copyright(c) 2008 - 2016 GEN Group.
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
*---------------------------------------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


#ifdef DB_SQL_ACTIVE

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DB_SQL_Result.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_RESULT::DB_SQL_RESULT()
* @brief      Constructor
* @ingroup    DATABASESSQL
*
* @author     Diego Martinez Ruiz de Gaona
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DB_SQL_RESULT::DB_SQL_RESULT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_RESULT::~DB_SQL_RESULT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATABASESSQL
*
* @author     Diego Martinez Ruiz de Gaona
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
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
* @ingroup    DATABASESSQL
*
* @author     Diego Martinez Ruiz de Gaona
* @date       01/03/2016 12:00
*
* @return     DB_SQL_ROW* :
*
*---------------------------------------------------------------------------------------------------------------------*/
DB_SQL_ROW* DB_SQL_RESULT::GetRow()
{
  return &row;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_ROWHEADER* DB_SQL_RESULT::GetHeader()
* @brief      GetHeader
* @ingroup    DATABASESSQL
*
* @author     Diego Martinez Ruiz de Gaona
* @date       01/03/2016 12:00
*
* @return     DB_SQL_ROWHEADER* :
*
*---------------------------------------------------------------------------------------------------------------------*/
DB_SQL_ROWHEADER* DB_SQL_RESULT::GetHeader()
{
  return &header;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DB_SQL_RESULT::GetCurrentRowIndex()
* @brief      GetCurrentRowIndex
* @ingroup    DATABASESSQL
*
* @author     Diego Martinez Ruiz de Gaona
* @date       01/03/2016 12:00
*
* @return     XDWORD :
*
*---------------------------------------------------------------------------------------------------------------------*/
XDWORD DB_SQL_RESULT::GetCurrentRowIndex()
{
  return currentrow;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DB_SQL_RESULT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATABASESSQL
*
* @author     Diego Martinez Ruiz de Gaona
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void DB_SQL_RESULT::Clean()
{
  currentrow      = 0;
  initialized     = false;
}


#endif


