/**-------------------------------------------------------------------------------------------------------------------
*
* @file       MYSQL_Database.h
*
* @class      MYSQL_DATABASE
* @brief      DataBase MySQL Database class
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

#if defined(DB_SQL_ACTIVE) && defined(DB_MYSQL_ACTIVE)

#ifndef _MYSQL_DATABASE_H_
#define _MYSQL_DATABASE_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <mysql.h>

#include "XTrace.h"

#include "DB_SQL_Base.h"
#include "DB_SQL_Database.h"

#include "MySQL_Variant.h"
#include "MySQL_Query.h"
#include "MySQL_Connection.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class MYSQL_DATABASE : public DB_SQL_DATABASE
{
  public:

    friend class                        MYSQL_CONNECTION;
    friend class                        MYSQL_QUERY;

                                        MYSQL_DATABASE          ();
    virtual                            ~MYSQL_DATABASE          ();

    virtual DB_SQL_DATABASE_TYPE        GetType                 ();
    virtual XCHAR*                      GetTypeName             ();

    virtual   DB_SQL_VARIANT*           CreateVariant           ();

    virtual   bool                      Transaction             ();
    virtual   bool                      Commit                  ();
    virtual   bool                      Rollback                (XCHAR* savepointname=NULL);
    virtual   bool                      Savepoint               (XCHAR* savepoint);
    virtual   bool                      ReleaseSavepoint        (XCHAR* savepoint);
    virtual   DB_SQL_QUERY*             CreateQuery             ();
    virtual   DB_SQL_CONNECTION*        CreateConnection        ();
    virtual   bool                      GetTables               ();
    virtual   DB_SQL_DATABASE_ENCODING  GetDatabaseEncoding     ();
    virtual   bool                      SetDatabaseEncoding     (DB_SQL_DATABASE_ENCODING encodingtouse);

  protected:

    my_bool                             isnull;

  private:

    void                                Clean                   ();
};

/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif


#endif
