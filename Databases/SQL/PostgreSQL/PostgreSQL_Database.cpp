/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       PostgreSQL_Database.cpp
* 
* @class      POSTGRESQL_DATABASE
* @brief      DataBase SQL PostgreSQL Database class
* @ingroup    DATABASE
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


#if defined(DB_SQL_ACTIVE) && defined(DB_POSTGRESQL_ACTIVE)


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "PostgreSQL_Database.h"

#include <libpq-fe.h>

#include "PostgreSQL_Connection.h"
#include "PostgreSQL_Query.h"

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
* @fn         POSTGRESQL_DATABASE::POSTGRESQL_DATABASE()
* @brief      Constructor of class
* @ingroup    DATABASE
*
* --------------------------------------------------------------------------------------------------------------------*/
POSTGRESQL_DATABASE::POSTGRESQL_DATABASE(): DB_SQL_DATABASE()
{
  Clean();

  //int version = PQlibVersion();

  type        = DB_SQL_DATABASE_FLAGS_SQL | DB_SQL_DATABASE_FLAGS_TRANSACTIONAL;
  defaultport = 5432;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         POSTGRESQL_DATABASE::~POSTGRESQL_DATABASE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATABASE
*
* --------------------------------------------------------------------------------------------------------------------*/
POSTGRESQL_DATABASE::~POSTGRESQL_DATABASE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_DATABASE_TYPE MYSQL_DATABASE::GetType()
* @brief      Get type
* @ingroup    DATABASE
*
* @return     DB_SQL_DATABASE_TYPE :
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_DATABASE_TYPE POSTGRESQL_DATABASE::GetType()
{
  return DB_SQL_DATABASE_TYPE_POSTGRESQL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* POSTGRESQL_DATABASE::GetTypeName()
* @brief      Get type name
* @ingroup    DATABASE
*
* @return     XCHAR* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* POSTGRESQL_DATABASE::GetTypeName()
{
  return __L("PostgreSQL");
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool POSTGRESQL_DATABASE::Transaction()
* @brief      Transaction
* @ingroup    DATABASE
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool POSTGRESQL_DATABASE::Transaction()
{
  PGconn*   conn = static_cast<POSTGRESQL_CONNECTION*>(this->connection)->connection;
  PGresult* res  = PQexec(conn, "BEGIN");

  if(PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Error(PQerrorMessage(conn));
      PQclear(res);

      return false;
    }

  PQclear(res);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool POSTGRESQL_DATABASE::Commit()
* @brief      Commit
* @ingroup    DATABASE
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool POSTGRESQL_DATABASE::Commit()
{
  PGconn* conn=static_cast<POSTGRESQL_CONNECTION*>(this->connection)->connection;
  PGresult* res = PQexec(conn, "END");

  if(PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Error(PQerrorMessage(conn));
      PQclear(res);

      return false;
    }

  PQclear(res);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool POSTGRESQL_DATABASE::Rollback(XCHAR* savepointname)
* @brief      Rollback
* @ingroup    DATABASE
*
* @param[in]  savepointname :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool POSTGRESQL_DATABASE::Rollback(XCHAR* savepointname)
{
  PGconn*   conn = static_cast<POSTGRESQL_CONNECTION*>(this->connection)->connection;
  PGresult* res  = PQexec(conn, "ABORT");

  if(PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Error(PQerrorMessage(conn));
      PQclear(res);

      return false;
    }

  PQclear(res);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool POSTGRESQL_DATABASE::Savepoint(XCHAR* savepoint)
* @brief      Savepoint
* @ingroup    DATABASE
*
* @param[in]  savepoint :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool POSTGRESQL_DATABASE::Savepoint(XCHAR* savepoint)
{
  PGconn* conn            = static_cast<POSTGRESQL_CONNECTION*>(this->connection)->connection;
  XSTRING savepointstring;
  bool    success;

  savepointstring.Format(__L("SAVEPOINT %s"),savepoint);

  XBUFFER oem;
  
  savepointstring.ConvertToASCII(oem); 
  PGresult* res = PQexec(conn, oem.GetPtrChar());
  if(PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Error(PQerrorMessage(conn));
      success=false;
    }
   else success=true;

  PQclear(res);

  return success;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool POSTGRESQL_DATABASE::ReleaseSavepoint(XCHAR* savepoint)
* @brief      Release savepoint
* @ingroup    DATABASE
*
* @param[in]  savepoint :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool POSTGRESQL_DATABASE::ReleaseSavepoint(XCHAR* savepoint)
{
  PGconn* conn              = static_cast<POSTGRESQL_CONNECTION*>(this->connection)->connection;
  XSTRING savepointstring;
  bool    success;

  savepointstring.Format(__L("RELEASE %s"),savepoint);

  XBUFFER oem;
  
  savepointstring.ConvertToASCII(oem);  
  PGresult* res = PQexec(conn, oem.GetPtrChar());

  if(PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Error(PQerrorMessage(conn));
      success=false;

    } else success=true;

  PQclear(res);

  return success;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_QUERY* POSTGRESQL_DATABASE::CreateQuery()
* @brief      Create query
* @ingroup    DATABASE
*
* @return     DB_SQL_QUERY* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_QUERY* POSTGRESQL_DATABASE::CreateQuery()
{
  POSTGRESQL_QUERY* query = new POSTGRESQL_QUERY(this);
  return query;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_CONNECTION* POSTGRESQL_DATABASE::CreateConnection()
* @brief      Create connection
* @ingroup    DATABASE
*
* @return     DB_SQL_CONNECTION* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_CONNECTION* POSTGRESQL_DATABASE::CreateConnection()
{
  POSTGRESQL_CONNECTION* pgconnection= new POSTGRESQL_CONNECTION(this);

  return pgconnection;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool POSTGRESQL_DATABASE::GetTables()
* @brief      Get tables
* @ingroup    DATABASE
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool POSTGRESQL_DATABASE::GetTables()
{
  DB_SQL_STRING querystring;
  querystring.Format(__L("SELECT relname FROM pg_class WHERE relkind='r';"));

  DB_SQL_QUERY*   query = CreateQuery();
  DB_SQL_VARIANT  variant;

  if (query!=NULL)
  {
    query->Set(querystring.Get());
    if (this->Execute(query))
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
                    variant=row->Get(e);
                    if(variant.GetType()==(XVARIANT_TYPE)DB_SQL_VARIANT_TYPE_STRING)
                      {
                        DB_SQL_STRING* tablename = new DB_SQL_STRING();
                        if(tablename)
                          {
                            tablename->Set((XCHAR*)variant);
                            tables.Add(tablename);
                          }
                      }
                  }
                result->Next();
              }
          }
          else return false;
    }
    delete(query);
  }
  else return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_DATABASE_ENCODING POSTGRESQL_DATABASE::GetDatabaseEncoding()
* @brief      Get database encoding
* @ingroup    DATABASE
*
* @return     DB_SQL_DATABASE_ENCODING :
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_DATABASE_ENCODING POSTGRESQL_DATABASE::GetDatabaseEncoding()
{
  int enc = PQclientEncoding(static_cast<POSTGRESQL_CONNECTION*>(this->connection)->connection);

  encodingtype = (DB_SQL_DATABASE_ENCODING)enc;

  return  encodingtype;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool POSTGRESQL_DATABASE::SetDatabaseEncoding(DB_SQL_DATABASE_ENCODING encodingtouse)
* @brief      Set database encoding
* @ingroup    DATABASE
*
* @param[in]  encodingtouse :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool POSTGRESQL_DATABASE::SetDatabaseEncoding(DB_SQL_DATABASE_ENCODING encodingtouse)
{
  PGconn* conn = static_cast<POSTGRESQL_CONNECTION*>(connection)->connection;

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

      case DB_SQL_DATABASE_ENCODING_UTF8              : { int rc = PQsetClientEncoding(conn,"UTF8");
                                                          if(rc==0) return true;
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
* @fn         bool POSTGRESQL_DATABASE::OnConnection()
* @brief      On connection
* @ingroup    DATABASE
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool POSTGRESQL_DATABASE::OnConnection()
{
  SetDatabaseEncoding(DB_SQL_DATABASE_ENCODING_UTF8);
  GetDatabaseEncoding();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool POSTGRESQL_DATABASE::OnDisconnection()
* @brief      On disconnection
* @ingroup    DATABASE
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool POSTGRESQL_DATABASE::OnDisconnection()
{
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool POSTGRESQL_DATABASE::IsThreadSafe()
* @brief      Is thread safe
* @ingroup    DATABASE
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool POSTGRESQL_DATABASE::IsThreadSafe()
{
  return PQisthreadsafe() == 1;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void POSTGRESQL_DATABASE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATABASE
*
* --------------------------------------------------------------------------------------------------------------------*/
void POSTGRESQL_DATABASE::Clean()
{
  type        = DB_SQL_DATABASE_FLAGS_NONE;
  connection  = NULL;
}


#pragma endregion


#endif

