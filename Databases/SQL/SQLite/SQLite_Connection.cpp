/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       SQLite_Connection.cpp
* 
* @class      SQLITE_CONNECTION
* @brief      DataBase SQL SQLite Connection class
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


#if defined(DB_SQL_ACTIVE) && defined(DB_SQLITE_ACTIVE)


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "SQLite_Connection.h"

#include <sqlite3.h>

#include "SQLite_Database.h"

#include "DB_SQL_Error.h"
#include "DB_SQL_Database.h"
#include "DB_SQL_Error.h"

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
* @fn         SQLITE_CONNECTION::SQLITE_CONNECTION(DB_SQL_DATABASE* database)
* @brief      Constructor
* @ingroup    DATABASES
*
* @param[in]  DB_SQL_DATABASE* :
*
* --------------------------------------------------------------------------------------------------------------------*/
SQLITE_CONNECTION::SQLITE_CONNECTION(DB_SQL_DATABASE* database) : DB_SQL_CONNECTION(database)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         SQLITE_CONNECTION::~SQLITE_CONNECTION()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATABASES
*
* --------------------------------------------------------------------------------------------------------------------*/
SQLITE_CONNECTION::~SQLITE_CONNECTION()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool SQLITE_CONNECTION::Connect()
* @brief      Connect
* @ingroup    DATABASES
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool SQLITE_CONNECTION::Connect()
{
  if(!database)
    {
      database->Error(__L("DATABASE not initialized"));
      return false;
    }

  // First check if options provided a db name

  DB_SQL_STRING databasename;
  DB_SQL_STRING databasetimeout;

  if(!FindOption(__L("DATABASE"), &databasename))
    {
      database->Error(__L("DATABASE argument not supplied"));
      return false;
    }

  // Attempt connection
  XPATH xpathdatabase;

  if(!FindOption(__L("PATH"), &xpathdatabase))
    {
      database->Error(__L("DATABASE argument not supplied"));
      return false;
    }

  xpathdatabase.Add(databasename);
  
  XBUFFER dbname;
  
  xpathdatabase.ConvertToASCII(dbname);
  int returncode = sqlite3_open(dbname.GetPtrChar(), &static_cast<SQLITE_DATABASE*>(database)->sqlite3database);  
  if(returncode)
    {
      database->Error((char*)sqlite3_errmsg(static_cast<SQLITE_DATABASE*>(database)->sqlite3database));
      sqlite3_close(static_cast<SQLITE_DATABASE*>(database)->sqlite3database);

      return false;
    }

  if(FindOption(__L("TIMEOUT"), &databasetimeout))
    {
      int timeoutseconds = databasetimeout.ConvertToInt();

      returncode = sqlite3_busy_timeout(static_cast<SQLITE_DATABASE*>(database)->sqlite3database,timeoutseconds*1000);

       if(returncode)
        {
          database->Error((char*)sqlite3_errmsg(static_cast<SQLITE_DATABASE*>(database)->sqlite3database));
          sqlite3_close(static_cast<SQLITE_DATABASE*>(database)->sqlite3database);

          return false;
        }
  }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool SQLITE_CONNECTION::Disconnect()
* @brief      Disconnect
* @ingroup    DATABASES
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool SQLITE_CONNECTION::Disconnect()
{
  if(!database) return false;

  if(static_cast<SQLITE_DATABASE*>(database)->sqlite3database)
         sqlite3_close(static_cast<SQLITE_DATABASE*>(database)->sqlite3database);
    else return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void SQLITE_CONNECTION::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATABASES
*
* --------------------------------------------------------------------------------------------------------------------*/
void SQLITE_CONNECTION::Clean()
{

}


#pragma endregion


#endif

