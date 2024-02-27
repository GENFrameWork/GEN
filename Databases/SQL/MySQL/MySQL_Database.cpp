/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       MySQL_Database.cpp
* 
* @class      MYSQL_DATABASE
* @brief      DataBase SQL MySQL Database class
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

/*---- PRECOMPILATION INCLUDES ----------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


#if defined(DB_SQL_ACTIVE) && defined(DB_MYSQL_ACTIVE)


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "MySQL_Database.h"

#include "MySQL_Connection.h"
#include "MySQL_Query.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         MYSQL_DATABASE::MYSQL_DATABASE()
* @brief      Constructor
* @ingroup    DATABASES
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
MYSQL_DATABASE::MYSQL_DATABASE(): DB_SQL_DATABASE()
{
  Clean();

  type        = DB_SQL_DATABASE_FLAGS_SQL | DB_SQL_DATABASE_FLAGS_TRANSACTIONAL;
  defaultport = 3306;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         MYSQL_DATABASE::~MYSQL_DATABASE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATABASES
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
MYSQL_DATABASE::~MYSQL_DATABASE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_DATABASE_TYPE MYSQL_DATABASE::GetType()
* @brief      GetType
* @ingroup    DATABASES
*
* @return     DB_SQL_DATABASE_TYPE :
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_DATABASE_TYPE MYSQL_DATABASE::GetType()
{
  return DB_SQL_DATABASE_TYPE_MYSQL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* MYSQL_DATABASE::GetTypeName()
* @brief      GetTypeName
* @ingroup    DATABASES
*
* @return     XCHAR* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* MYSQL_DATABASE::GetTypeName()
{
  return __L("MySQL");
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_VARIANT* MYSQL_DATABASE::CreateVariant()
* @brief      CreateVariant
* @ingroup    DATABASES
*
* @return     DB_SQL_VARIANT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_VARIANT* MYSQL_DATABASE::CreateVariant()
{
  return new DB_SQL_VARIANT();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MYSQL_DATABASE::Transaction()
* @brief      Transaction
* @ingroup    DATABASES
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MYSQL_DATABASE::Transaction()
{
  MYSQL* conn=static_cast<MYSQL_CONNECTION*>(this->connection)->connection;
  if(!conn)  return false;

  mysql_autocommit(conn,false);

  int RC = mysql_query(conn,"BEGIN");

  if(RC == DB_SQL_MYSQL_OK)
    {
      transactionstarted=true;
      return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MYSQL_DATABASE::Commit()
* @brief      Commit
* @ingroup    DATABASES
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MYSQL_DATABASE::Commit()
{
  if(transactionstarted)
    {
      MYSQL* conn=static_cast<MYSQL_CONNECTION*>(this->connection)->connection;
      int RC = mysql_commit(conn);

      if(RC == DB_SQL_MYSQL_OK)
        {
          transactionstarted = false;
          mysql_autocommit(conn,true);

          return true;
        }
       else
        {
          Error((char*)mysql_error(conn));
          return false;
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MYSQL_DATABASE::Rollback(XCHAR* savepointname)
* @brief      Rollback
* @ingroup    DATABASES
*
* @param[in]  savepointname :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MYSQL_DATABASE::Rollback(XCHAR* savepointname)
{
  if(transactionstarted)
    {
      MYSQL* conn=static_cast<MYSQL_CONNECTION*>(this->connection)->connection;
      if(!savepointname)
        {
          int RC= mysql_rollback(conn);
          if (RC == DB_SQL_MYSQL_OK)
            {
              transactionstarted=false;
              return true;
            }
        }
       else
        {
          DB_SQL_STRING text;
          text.Format(__L("ROLLBACK TO SAVEPOINT %s"),savepointname);
          
          XBUFFER rollbacktext;
          
          text.ConvertToASCII(rollbacktext);
          int rc = mysql_query(conn, rollbacktext.GetPtrChar());

          if(rc != DB_SQL_MYSQL_OK)
            {
              Error((char*)mysql_error(conn));
              return false;
            }

          this->transactionstarted=false;

          return true;
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MYSQL_DATABASE::Savepoint(XCHAR* savepoint)
* @brief      Savepoint
* @ingroup    DATABASES
*
* @param[in]  savepoint :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MYSQL_DATABASE::Savepoint(XCHAR* savepoint)
{
  MYSQL* conn = static_cast<MYSQL_CONNECTION*>(this->connection)->connection;
  if(!conn) return false;

  DB_SQL_STRING text;

  text.Format(__L("SAVEPOINT %s"),savepoint);

  XBUFFER savetext;
  
  text.ConvertToASCII(savetext); 
  int rc = mysql_query(conn, savetext.GetPtrChar());
  
  if(rc != DB_SQL_MYSQL_OK)
    {
      Error((char*)mysql_error(conn));
      return false;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MYSQL_DATABASE::ReleaseSavepoint(XCHAR* savepoint)
* @brief      ReleaseSavepoint
* @ingroup    DATABASES
*
* @param[in]  savepoint :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MYSQL_DATABASE::ReleaseSavepoint(XCHAR* savepoint)
{
  MYSQL* conn=static_cast<MYSQL_CONNECTION*>(this->connection)->connection;
  if(!conn) return false;

  DB_SQL_STRING text;
  text.Format(__L("RELEASE %s"),savepoint);

  XBUFFER savetext;
  
  text.ConvertToASCII(savetext); 
  int rc = mysql_query(conn, savetext.GetPtrChar());
  
  if(rc != DB_SQL_MYSQL_OK)
    {
      Error((char*)mysql_error(conn));
      return false;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_QUERY* MYSQL_DATABASE::CreateQuery()
* @brief      CreateQuery
* @ingroup    DATABASES
*
* @return     DB_SQL_QUERY* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_QUERY* MYSQL_DATABASE::CreateQuery()
{
  DB_SQL_QUERY* query = new MYSQL_QUERY(this);
  return query;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_CONNECTION* MYSQL_DATABASE::CreateConnection()
* @brief      CreateConnection
* @ingroup    DATABASES
*
* @return     DB_SQL_CONNECTION* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_CONNECTION* MYSQL_DATABASE::CreateConnection()
{
  DB_SQL_CONNECTION* conn = new MYSQL_CONNECTION(this);
  return conn;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MYSQL_DATABASE::GetTables()
* @brief      GetTables
* @ingroup    DATABASES
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MYSQL_DATABASE::GetTables()
{
  MYSQL*      conn  = static_cast<MYSQL_CONNECTION*>(this->connection)->connection;
  DB_SQL_QUERY* query = CreateQuery();

  if(!conn)   return false;
  if(!query)  return false;

  tables.DeleteContents();
  tables.DeleteAll();

  query->Set(__L("SHOW TABLES"));
  if(query->Execute())
    {
      DB_SQL_RESULT* result=query->GetResult();
      if(result) //hay un result
        {
          result->GetNumRows();
          while(result->HasNext())
            {
              result->ProcessRow();

              DB_SQL_ROW* row=result->GetRow();
              for(XDWORD e=0; e<result->GetNumColumns(); e++)
                {
                  if(row->Get(e).GetType()==(XVARIANT_TYPE)DB_SQL_VARIANT_TYPE_STRING)
                    {
                      DB_SQL_STRING* tablename=new DB_SQL_STRING();
                      if(tablename)
                        {
                          tablename->Set((XCHAR*)row->Get(0));
                          this->tables.Add(tablename);
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
      delete(query);

      return false;
    }

  delete(query);

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_DATABASE_ENCODING MYSQL_DATABASE::GetDatabaseEncoding()
* @brief      GetDatabaseEncoding
* @ingroup    DATABASES
*
* @return     DB_SQL_DATABASE_ENCODING :
*
* --------------------------------------------------------------------------------------------------------------------*/
DB_SQL_DATABASE_ENCODING MYSQL_DATABASE::GetDatabaseEncoding()
{
  MYSQL* conn = static_cast<MYSQL_CONNECTION*>(this->connection)->connection;
  if(!conn) return DB_SQL_DATABASE_ENCODING_UNKNOWN;

  const char* name = mysql_character_set_name(conn);

  XSTRING s;
  s.Set(name);

  if (s.Compare(__L("ucs2")) == 0)  return DB_SQL_DATABASE_ENCODING_UNKNOWN;
  if (s.Compare(__L("utf8")) == 0)  return DB_SQL_DATABASE_ENCODING_UTF8;

  return DB_SQL_DATABASE_ENCODING_UNKNOWN;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MYSQL_DATABASE::SetDatabaseEncoding(DB_SQL_DATABASE_ENCODING encodingtouse)
* @brief      SetDatabaseEncoding
* @ingroup    DATABASES
*
* @param[in]  encodingtouse :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MYSQL_DATABASE::SetDatabaseEncoding(DB_SQL_DATABASE_ENCODING encodingtouse)
{
  MYSQL* conn = static_cast<MYSQL_CONNECTION*>(this->connection)->connection;

  switch(encodingtouse)
    {
      case DB_SQL_DATABASE_ENCODING_BIG5              :
      case DB_SQL_DATABASE_ENCODING_EUC_CN              :
      case DB_SQL_DATABASE_ENCODING_EUC_JP              :
      case DB_SQL_DATABASE_ENCODING_ISO_8859_5          :
      case DB_SQL_DATABASE_ENCODING_ISO_8859_6          :
      case DB_SQL_DATABASE_ENCODING_ISO_8859_7          :
      case DB_SQL_DATABASE_ENCODING_ISO_8859_8          :
      case DB_SQL_DATABASE_ENCODING_JOHAB               :
      case DB_SQL_DATABASE_ENCODING_EUC_JIS_2004        :
      case DB_SQL_DATABASE_ENCODING_EUC_KR              :
      case DB_SQL_DATABASE_ENCODING_EUC_TW              :
      case DB_SQL_DATABASE_ENCODING_GB18030             :
      case DB_SQL_DATABASE_ENCODING_GBK                 :
      case DB_SQL_DATABASE_ENCODING_KOI8R               :
      case DB_SQL_DATABASE_ENCODING_KOI8U               :
      case DB_SQL_DATABASE_ENCODING_LATIN1              :
      case DB_SQL_DATABASE_ENCODING_LATIN2              :
      case DB_SQL_DATABASE_ENCODING_LATIN3              :
      case DB_SQL_DATABASE_ENCODING_LATIN4              :
      case DB_SQL_DATABASE_ENCODING_LATIN5              :
      case DB_SQL_DATABASE_ENCODING_LATIN6              :
      case DB_SQL_DATABASE_ENCODING_LATIN7              :
      case DB_SQL_DATABASE_ENCODING_LATIN8              :
      case DB_SQL_DATABASE_ENCODING_LATIN9              :
      case DB_SQL_DATABASE_ENCODING_LATIN10             :
      case DB_SQL_DATABASE_ENCODING_MULE_INTERNAL       :
      case DB_SQL_DATABASE_ENCODING_SJIS                :
      case DB_SQL_DATABASE_ENCODING_SHIFT_JIS_2004      :
      case DB_SQL_DATABASE_ENCODING_SQL_ASCII           :
      case DB_SQL_DATABASE_ENCODING_UHC                 : break;

      case DB_SQL_DATABASE_ENCODING_UTF8                : { int rc = mysql_set_character_set(conn,"utf8");
                                                            if (rc == 0) return true;
                                                          }
                                                          break;
      case DB_SQL_DATABASE_ENCODING_WIN866              :
      case DB_SQL_DATABASE_ENCODING_WIN874              :
      case DB_SQL_DATABASE_ENCODING_WIN1250             :
      case DB_SQL_DATABASE_ENCODING_WIN1251             :
      case DB_SQL_DATABASE_ENCODING_WIN1252             :
      case DB_SQL_DATABASE_ENCODING_WIN1253             :
      case DB_SQL_DATABASE_ENCODING_WIN1254             :
      case DB_SQL_DATABASE_ENCODING_WIN1255             :
      case DB_SQL_DATABASE_ENCODING_WIN1256             :
      case DB_SQL_DATABASE_ENCODING_WIN1257             :
      case DB_SQL_DATABASE_ENCODING_WIN1258             : break;
    }

  Error (__L("Unable to set the charset encoding"));

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void MYSQL_DATABASE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATABASES
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void MYSQL_DATABASE::Clean()
{
  connection  = NULL;
  isnull      = false;
}


#pragma endregion


#endif

