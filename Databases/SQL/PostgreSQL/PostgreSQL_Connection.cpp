/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       PostgreSQL_Connection.cpp
* 
* @class      POSTGRESQL_CONNECTION
* @brief      DataBase SQL PostgreSQL Connection class
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


#if defined(DB_SQL_ACTIVE) && defined(DB_POSTGRESQL_ACTIVE)


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "PostgreSQL_Connection.h"

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
* @fn         POSTGRESQL_CONNECTION::POSTGRESQL_CONNECTION(DB_SQL_DATABASE* database) : DB_SQL_CONNECTION(database)
* @brief      Constructor
* @ingroup    DATABASES
*
* @param[in]  DB_SQL_DATABASE* :
*
* --------------------------------------------------------------------------------------------------------------------*/
POSTGRESQL_CONNECTION::POSTGRESQL_CONNECTION(DB_SQL_DATABASE* database) : DB_SQL_CONNECTION(database)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         POSTGRESQL_CONNECTION::~POSTGRESQL_CONNECTION()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATABASES
*
* --------------------------------------------------------------------------------------------------------------------*/
POSTGRESQL_CONNECTION::~POSTGRESQL_CONNECTION()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool POSTGRESQL_CONNECTION::Connect()
* @brief      Connect
* @ingroup    DATABASES
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool POSTGRESQL_CONNECTION::Connect()
{
  DB_SQL_STRING connectionstring;
  DB_SQL_STRING databaseUrl;
  DB_SQL_STRING databaseUser;
  DB_SQL_STRING databasePassword;
  DB_SQL_STRING databaseName;
  DB_SQL_STRING databasePort;
  DB_SQL_STRING databaseTimeOut;

  if(!FindOption(__L("URL"), &databaseUrl))
    {
      database->Error(__L("DATABASE URL argument not supplied"));
      return false;
    }

  if(!FindOption(__L("USER"), &databaseUser))
    {
      database->Error(__L("DATABASE User Name argument not supplied"));
      return false;
    }

  if(!FindOption(__L("PASSWORD"), &databasePassword))
    {
      database->Error(__L("DATABASE User Password argument not supplied"));
      return false;
    }

  if(!FindOption(__L("DATABASE"), &databaseName))
    {
      database->Error(__L("DATABASE Name argument not supplied"));
      return false;
    }

  if(!FindOption(__L("PORT"), &databasePort))
    {
      database->Error(__L("DATABASE Port argument not supplied"));
      return false;
    }

  FindOption(__L("TIMEOUT"), &databaseTimeOut);

  if(!databaseUrl.IsEmpty())
    {
      connectionstring.Add(__L("host = "));
      connectionstring.Add(databaseUrl);
      connectionstring.Add(__L(" "));
    }

  if(!databaseUser.IsEmpty())
    {
      connectionstring.Add(__L("user = "));
      connectionstring.Add(databaseUser);
      connectionstring.Add(__L(" "));
    }

  if(!databasePassword.IsEmpty())
    {
      connectionstring.Add(__L("password = "));
      connectionstring.Add(databasePassword);
      connectionstring.Add(__L(" "));
    }

  if(!databaseName.IsEmpty())
    {
      connectionstring.Add(__L("dbname = "));
      connectionstring.Add(databaseName);
      connectionstring.Add(__L(" "));
    }

  if(!databasePort.IsEmpty())
    {
      connectionstring.Add(__L("port = "));
      connectionstring.Add(databasePort);
      connectionstring.Add(__L(" "));
    }

  if(!databaseTimeOut.IsEmpty())
    {
      connectionstring.Add(__L("connect_timeout = "));
      connectionstring.Add(databaseTimeOut);
      connectionstring.Add(__L(" "));
    }

  XBUFFER conninfo;
  
  connectionstring.ConvertToASCII(conninfo);  
  connection = PQconnectdb(conninfo.GetPtrChar());

  bool status = true;

  if(PQstatus(connection) != CONNECTION_OK)
    {
      XSTRING error;

      error.Set(PQerrorMessage(connection));
      database->Error(error.Get());

      status = false;
    }
  
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool POSTGRESQL_CONNECTION::Disconnect()
* @brief      Disconnect
* @ingroup    DATABASES
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool POSTGRESQL_CONNECTION::Disconnect()
{
  PQfinish(connection);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void POSTGRESQL_CONNECTION::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATABASES
*
* --------------------------------------------------------------------------------------------------------------------*/
void POSTGRESQL_CONNECTION::Clean()
{

}


#pragma endregion


#endif

