/**-------------------------------------------------------------------------------------------------------------------
*
* @file       SQLITE_Database.cpp
*
* @class      SQLITE_DATABASE
* @brief      DataBase SQLite Database class
* @ingroup    DATABASESSQL
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"

#if defined(DB_SQL_ACTIVE) && defined(DB_SQLITE_ACTIVE)

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XTrace.h"

#include "SQLite_Connection.h"
#include "SQLite_Query.h"

#include "DB_SQL_Query.h"

#include "SQLite_Database.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         SQLITE_DATABASE::SQLITE_DATABASE()
* @brief      Constructor
* @ingroup    DATABASESSQL
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
SQLITE_DATABASE::SQLITE_DATABASE() : DB_SQL_DATABASE()
{
  Clean();

  type  = DB_SQL_DATABASE_FLAGS_SQL | DB_SQL_DATABASE_FLAGS_TRANSACTIONAL;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         SQLITE_DATABASE::~SQLITE_DATABASE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATABASESSQL
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
SQLITE_DATABASE::~SQLITE_DATABASE()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_DATABASE_TYPE SQLITE_DATABASE::GetType()
* @brief      GetType
* @ingroup    DATABASESSQL
*
* @return     DB_SQL_DATABASE_TYPE :
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_DATABASE_TYPE SQLITE_DATABASE::GetType()
{
  return DB_SQL_DATABASE_TYPE_SQLITE;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* SQLITE_DATABASE::GetTypeName()
* @brief      GetTypeName
* @ingroup    DATABASESSQL
*
* @return     XCHAR* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* SQLITE_DATABASE::GetTypeName()
{
  return __L("SQLite");
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool SQLITE_DATABASE::Transaction()
* @brief      Transaction
* @ingroup    DATABASESSQL
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool SQLITE_DATABASE::Transaction()
{
  transactionstarted = true;

  int rc=sqlite3_exec(sqlite3database, "BEGIN IMMEDIATE", 0, 0, 0);

  return (rc==SQLITE_OK || rc==SQLITE_BUSY);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool SQLITE_DATABASE::Commit()
* @brief      Commit
* @ingroup    DATABASESSQL
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool SQLITE_DATABASE::Commit()
{
  if(IsTransactionStarted())
    {
      int rc=sqlite3_exec(sqlite3database, "COMMIT", 0, 0, 0);
      transactionstarted = false;

      if(rc == SQLITE_OK)   return true;
      if(rc == SQLITE_BUSY) return true;
    }

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool SQLITE_DATABASE::Rollback(XCHAR* savepoint)
* @brief      Rollback
* @ingroup    DATABASESSQL
*
* @param[in]  savepoint :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool SQLITE_DATABASE::Rollback(XCHAR* savepoint)
{
  if(IsTransactionStarted())
    {
      if(savepoint == NULL)
        {
          int rc = sqlite3_exec(sqlite3database, "ROLLBACK", 0, 0, 0);
          transactionstarted = false;

          return (rc==SQLITE_OK);
        }
       else
        {
          DB_SQL_STRING text;
          text.Format(__L("ROLLBACK TO SAVEPOINT %s"), savepoint);

          XSTRING_CREATEOEM(text,rollbacktext);
          int rc = sqlite3_exec(sqlite3database, rollbacktext, 0, 0, 0);
          transactionstarted = false;
          XSTRING_DELETEOEM(text, rollbacktext);

          return (rc==SQLITE_OK);
        }
    }

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool SQLITE_DATABASE::Savepoint(XCHAR* savepoint)
* @brief      Savepoint
* @ingroup    DATABASESSQL
*
* @param[in]  savepoint :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool SQLITE_DATABASE::Savepoint(XCHAR* savepoint)
{
  DB_SQL_STRING text;
  text.Format(__L("SAVEPOINT %s"),savepoint);

  XSTRING_CREATEOEM(text,rollbacktext);
  int rc=sqlite3_exec(sqlite3database, rollbacktext, 0, 0, 0);
  XSTRING_DELETEOEM(text, rollbacktext);

  return (rc==SQLITE_OK);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool SQLITE_DATABASE::ReleaseSavepoint(XCHAR* savepoint)
* @brief      ReleaseSavepoint
* @ingroup    DATABASESSQL
*
* @param[in]  savepoint :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool SQLITE_DATABASE::ReleaseSavepoint(XCHAR* savepoint)
{
  DB_SQL_STRING text;
  text.Format(__L("RELEASE %s"),savepoint);

  XSTRING_CREATEOEM(text,rollbacktext);
  int rc=sqlite3_exec(sqlite3database, rollbacktext, 0, 0, 0);
  XSTRING_DELETEOEM(text, rollbacktext);

  return (rc==SQLITE_OK);
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_QUERY* SQLITE_DATABASE::CreateQuery()
* @brief      CreateQuery
* @ingroup    DATABASESSQL
*
* @return     DB_SQL_QUERY* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_QUERY* SQLITE_DATABASE::CreateQuery()
{
  DB_SQL_QUERY* query = new SQLITE_QUERY(this);
  if(!query)
    {
      //XTRACE_PRINTCOLOR(4,__L("SQLITE_DATABASE::CreateQuery  unable to create SQLITE_QUERY"));
      return NULL;
    }

  return query;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_CONNECTION* SQLITE_DATABASE::CreateConnection()
* @brief      CreateConnection
* @ingroup    DATABASESSQL
*
* @return     DB_SQL_CONNECTION* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_CONNECTION* SQLITE_DATABASE::CreateConnection()
{
  DB_SQL_CONNECTION* conn = new SQLITE_CONNECTION(this);
  return conn;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool SQLITE_DATABASE::GetTables()
* @brief      GetTables
* @ingroup    DATABASESSQL
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool SQLITE_DATABASE::GetTables()
{
  DB_SQL_QUERY* query = CreateQuery();
  if(!query) return false;

  bool status = true;

  tables.DeleteContents();
  tables.DeleteAll();

  query->Set(__L("SELECT name FROM sqlite_master WHERE type='table'"));
  if(query->Execute())
    {
      DB_SQL_RESULT* result=query->GetResult();
      if(result!=NULL) //hay un result
        {
          result->GetNumRows();
          while(result->HasNext())
            {
              result->ProcessRow();

              DB_SQL_ROW* row = result->GetRow();
              for(XDWORD e=0; e<result->GetNumColumns(); e++)
                {
                  if(row->Get(e).GetType()==(XVARIANT_TYPE)DB_SQL_VARIANT_TYPE_STRING)
                    {
                      DB_SQL_STRING* tablename = new DB_SQL_STRING();
                      if(tablename)
                        {
                          tablename->Set((XCHAR*)row->Get(0));
                          tables.Add(tablename);
                        }
                    }
                }

              result->Next();
            }
        }
    }
   else
    {
      Error(__L("Unable to obtain Tables"));
      status = false;
    }

  delete(query);

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_DATABASE_ENCODING SQLITE_DATABASE::GetDatabaseEncoding()
* @brief      GetDatabaseEncoding
* @ingroup    DATABASESSQL
*
* @return     DB_SQL_DATABASE_ENCODING :
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_DATABASE_ENCODING SQLITE_DATABASE::GetDatabaseEncoding()
{
  return DB_SQL_DATABASE_ENCODING_UNKNOWN;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool SQLITE_DATABASE::SetDatabaseEncoding(DB_SQL_DATABASE_ENCODING encodingtouse)
* @brief      SetDatabaseEncoding
* @ingroup    DATABASESSQL
*
* @param[in]  encodingtouse :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool SQLITE_DATABASE::SetDatabaseEncoding(DB_SQL_DATABASE_ENCODING encodingtouse)
{
  switch(encodingtouse)
    {
      case DB_SQL_DATABASE_ENCODING_BIG5              :
      case DB_SQL_DATABASE_ENCODING_EUC_CN            :
      case DB_SQL_DATABASE_ENCODING_EUC_JP            :
      case DB_SQL_DATABASE_ENCODING_EUC_JIS_2004      :
      case DB_SQL_DATABASE_ENCODING_EUC_KR            :
      case DB_SQL_DATABASE_ENCODING_EUC_TW            :
      case DB_SQL_DATABASE_ENCODING_GB18030           :
      case DB_SQL_DATABASE_ENCODING_GBK               :
      case DB_SQL_DATABASE_ENCODING_ISO_8859_5        :
      case DB_SQL_DATABASE_ENCODING_ISO_8859_6        :
      case DB_SQL_DATABASE_ENCODING_ISO_8859_7        :
      case DB_SQL_DATABASE_ENCODING_ISO_8859_8        :
      case DB_SQL_DATABASE_ENCODING_JOHAB             :
      case DB_SQL_DATABASE_ENCODING_KOI8R             :
      case DB_SQL_DATABASE_ENCODING_KOI8U             :
      case DB_SQL_DATABASE_ENCODING_LATIN1            :
      case DB_SQL_DATABASE_ENCODING_LATIN2            :
      case DB_SQL_DATABASE_ENCODING_LATIN3            :
      case DB_SQL_DATABASE_ENCODING_LATIN4            :
      case DB_SQL_DATABASE_ENCODING_LATIN5            :
      case DB_SQL_DATABASE_ENCODING_LATIN6            :
      case DB_SQL_DATABASE_ENCODING_LATIN7            :
      case DB_SQL_DATABASE_ENCODING_LATIN8            :
      case DB_SQL_DATABASE_ENCODING_LATIN9            :
      case DB_SQL_DATABASE_ENCODING_LATIN10           :
      case DB_SQL_DATABASE_ENCODING_MULE_INTERNAL     :
      case DB_SQL_DATABASE_ENCODING_SJIS              :
      case DB_SQL_DATABASE_ENCODING_SHIFT_JIS_2004    :
      case DB_SQL_DATABASE_ENCODING_SQL_ASCII         :
      case DB_SQL_DATABASE_ENCODING_UHC               : break;

      case DB_SQL_DATABASE_ENCODING_UTF8              : { DB_SQL_QUERY* query = CreateQuery();
                                                        if(!query) break;

                                                        query->Set(__L("PRAGMA encoding=\"UTF-8\""));
                                                        int rc = query->Execute();

                                                        delete query;

                                                        return rc==0;
                                                      }
                                                      break;
      case DB_SQL_DATABASE_ENCODING_WIN866            :
      case DB_SQL_DATABASE_ENCODING_WIN874            :
      case DB_SQL_DATABASE_ENCODING_WIN1250           :
      case DB_SQL_DATABASE_ENCODING_WIN1251           :
      case DB_SQL_DATABASE_ENCODING_WIN1252           :
      case DB_SQL_DATABASE_ENCODING_WIN1253           :
      case DB_SQL_DATABASE_ENCODING_WIN1254           :
      case DB_SQL_DATABASE_ENCODING_WIN1255           :
      case DB_SQL_DATABASE_ENCODING_WIN1256           :
      case DB_SQL_DATABASE_ENCODING_WIN1257           :
      case DB_SQL_DATABASE_ENCODING_WIN1258           : break;

    }

  Error (__L("Unable to set the charset encoding"));

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void SQLITE_DATABASE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATABASESSQL
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void SQLITE_DATABASE::Clean()
{
  type            = DB_SQL_DATABASE_FLAGS_NONE;
  connection      = NULL;
  sqlite3database = NULL;
}


#endif
