/**-------------------------------------------------------------------------------------------------------------------
*
* @file       SQLITE_Connection.h
*
* @class      SQLITE_CONNECTION
* @brief      DataBase SQLite Connection class
* @ingroup    DATABASESSQL
*
* @copyright  GEN Group. All right reserved.
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


#if defined(DB_SQL_ACTIVE) && defined(DB_SQLITE_ACTIVE)

#ifndef _SQLITE_CONNECTION_H_
#define _SQLITE_CONNECTION_H_

#ifndef SQLITE_CORE
#define SQLITE_CORE
#endif

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DB_SQL_Connection.h"
#include "DB_SQL_Database.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class SQLITE_CONNECTION : public DB_SQL_CONNECTION
{
  public:
                          SQLITE_CONNECTION         (DB_SQL_DATABASE* database);
    virtual              ~SQLITE_CONNECTION         ();

    virtual bool          Connect                   ();
    virtual bool          Disconnect                ();

  protected:

  private:

    void                  Clean                     ();

};


/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif

#endif


