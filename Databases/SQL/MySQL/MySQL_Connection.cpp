/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       MySQL_Connection.cpp
* 
* @class      MYSQL_CONNECTION
* @brief      DataBase SQL MySQL Connection class
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

#include "MySQL_Connection.h"

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
* @fn         MYSQL_CONNECTION::MYSQL_CONNECTION(DB_SQL_DATABASE* database)
* @brief      Constructor
* @ingroup    DATABASES
*
* @param[in]  DB_SQL_DATABASE* :
*
* --------------------------------------------------------------------------------------------------------------------*/
MYSQL_CONNECTION::MYSQL_CONNECTION(DB_SQL_DATABASE* database): DB_SQL_CONNECTION(database)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         MYSQL_CONNECTION::~MYSQL_CONNECTION()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATABASES
*
* --------------------------------------------------------------------------------------------------------------------*/
MYSQL_CONNECTION::~MYSQL_CONNECTION()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MYSQL_CONNECTION::Connect()
* @brief      Connect
* @ingroup    DATABASES
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MYSQL_CONNECTION::Connect()
{
  connection = mysql_init(NULL);
  if(!connection)
    {
      XSTRING error;
      error.Set(mysql_error(connection));

      database->Error(error.Get());

      return false;
    }

  DB_SQL_STRING databaseurl;
  DB_SQL_STRING databaseuser;
  DB_SQL_STRING databasepassword;
  DB_SQL_STRING databasename;
  DB_SQL_STRING databaseport;
  DB_SQL_STRING databasetimeout;
  DB_SQL_STRING reconnect;

  if(!FindOption(__L("URL"),  &databaseurl))
    {
      database->Error(__L("DATABASE URL argument not supplied"));
      return false;
    }

  if(!FindOption(__L("USER"), &databaseuser))
    {
      database->Error(__L("DATABASE User Name argument not supplied"));
      return false;
    }

  if(!FindOption(__L("PASSWORD"), &databasepassword))
    {
      database->Error(__L("DATABASE User Name argument not supplied"));
      return false;
    }

  FindOption(__L("DATABASE")  , &databasename);
  FindOption(__L("PORT")      , &databaseport);
  
  XBUFFER url;
  XBUFFER user;
  XBUFFER pswd;
  XBUFFER name;
  
  databaseurl.ConvertToASCII(url);
  databaseuser.ConvertToASCII(user);
  databasepassword.ConvertToASCII(pswd);
  databasename.ConvertToASCII(name);

  unsigned int port = databaseport.ConvertToDWord();

  bool success=true;

  if (FindOption(__L("RECONNECT"), &reconnect))
  {
    int reconnect_opt = reconnect.ConvertToInt();
    mysql_options(connection, MYSQL_OPT_RECONNECT, &reconnect_opt);
  }
  else
  {
    int reconnect_opt = 1;
    mysql_options(connection, MYSQL_OPT_RECONNECT, &reconnect_opt);
  }

  if (FindOption(__L("TIMEOUT"), &databasetimeout))
  {
      int timeoutseconds=databasetimeout.ConvertToInt();
      mysql_options(connection,MYSQL_OPT_CONNECT_TIMEOUT ,&timeoutseconds);
  }

  if(mysql_real_connect(connection, url.GetPtrChar(), user.GetPtrChar(), pswd.GetPtrChar(), name.GetPtrChar(), port, NULL, 0) == NULL)
    {
      DB_SQL_STRING error;

      error.Set(mysql_error(connection));
      database->Error(error.Get());
      mysql_close(connection);

      success = false;
    }
  
  return success;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool MYSQL_CONNECTION::Disconnect()
* @brief      Disconnect
* @ingroup    DATABASES
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool MYSQL_CONNECTION::Disconnect()
{
  mysql_close(connection);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void MYSQL_CONNECTION::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATABASES
*
* --------------------------------------------------------------------------------------------------------------------*/
void MYSQL_CONNECTION::Clean()
{

}


#pragma endregion


#endif

