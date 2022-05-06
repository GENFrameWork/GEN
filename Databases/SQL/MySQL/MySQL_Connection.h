/**-------------------------------------------------------------------------------------------------------------------
*
* @file       MYSQL_Connection.h
*
* @class      MYSQL_CONNECTION
* @brief      DataBase MySQL Connection class
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

#if defined(DB_SQL_ACTIVE) && defined(DB_MYSQL_ACTIVE)

#ifndef _MYSQL_CONNECTION_H_
#define _MYSQL_CONNECTION_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <mysql.h>

#include "DB_SQL_Connection.h"
#include "DB_SQL_Database.h"

#include "MySQL_Database.h"
#include "MySQL_Query.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class MYSQL_CONNECTION : public DB_SQL_CONNECTION
{
  public:

    friend class                  MYSQL_QUERY;
    friend class                  MYSQL_DATABASE;

                                  MYSQL_CONNECTION        (DB_SQL_DATABASE* database);
    virtual                      ~MYSQL_CONNECTION        ();

    virtual   bool                Connect                 ();
    virtual   bool                Disconnect              ();

  protected:

    MYSQL*                        connection;

  private:

    void                          Clean                   ();
};



/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif

#endif


