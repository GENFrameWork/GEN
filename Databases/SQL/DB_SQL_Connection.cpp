/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DB_SQL_Connection.cpp
*
* @class      DB_SQL_CONNECTION
* @brief      DataBase connection class
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

#include "XThread.h"
#include "XFactory.h"
#include "XThread.h"

#include "DB_SQL_Connection.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_CONNECTION::DB_SQL_CONNECTION(DB_SQL_DATABASE* database)
* @brief      Constructor
* @ingroup    DATABASESSQL
*
* @author     Diego Martinez Ruiz de Gaona
* @date       01/03/2016 12:00
*
* @param[in]  DB_SQL_DATABASE* :
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DB_SQL_CONNECTION::DB_SQL_CONNECTION(DB_SQL_DATABASE* database)
{
  Clean();

  this->database = database;

  database->SetConnection(this);

  options.SetIsMulti(false);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_CONNECTION::~DB_SQL_CONNECTION()
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
DB_SQL_CONNECTION::~DB_SQL_CONNECTION()
{
  options.DeleteKeyContents();
  options.DeleteElementContents();

  options.DeleteAll();

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DB_SQL_CONNECTION::SetOption(XCHAR* key, XCHAR* value)
* @brief      SetOption
* @ingroup    DATABASESSQL
*
* @author     Diego Martinez Ruiz de Gaona
* @date       01/03/2016 12:00
*
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DB_SQL_CONNECTION::SetOption(XCHAR* key, XCHAR* value)
{
  DB_SQL_STRING* newkey = new DB_SQL_STRING();
  if(newkey)
    {
      DB_SQL_STRING* newvalue = new DB_SQL_STRING();
      if(newvalue)
        {
          newkey->Set   (key);
          newvalue->Set (value);

          return options.Add(newkey,newvalue);
        }

      delete newkey;
    }

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DB_SQL_CONNECTION::FindOption(XCHAR* key, DB_SQL_STRING* result)
* @brief      FindOption
* @ingroup    DATABASESSQL
*
* @author     Diego Martinez Ruiz de Gaona
* @date       01/03/2016 12:00
*
* @param[in]  key :
* @param[in]  result :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DB_SQL_CONNECTION::FindOption(XCHAR* key, DB_SQL_STRING* result)
{
  for(XDWORD e=0; e<this->options.GetSize(); e++)
    {
      if(!options.GetKey(e)->Compare(key))
        {
          result->Set(options.GetElement(e)->Get());

          return true;
        }
    }

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DB_SQL_CONNECTION::DB_SQL_CONNECTION()
* @brief      Constructor
* @ingroup    DATABASESSQL
*
* @author     Diego Martinez Ruiz de Gaona
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DB_SQL_CONNECTION::DB_SQL_CONNECTION()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DB_SQL_CONNECTION::Clean()
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
void DB_SQL_CONNECTION::Clean()
{
  database = NULL;
}



#endif
