/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       APPFlowLog.h
* 
* @class      APPFLOWLOG
* @brief      Application Flow Log class
* @ingroup    APPFLOW
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

#ifndef _APPFLOWLOG_H_
#define _APPFLOWLOG_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XPath.h"
#include "XString.h"
#include "XLog.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

#define APPFLOW_LOG               APPFLOWLOG::GetInstance()
#define APPFLOW_LOG_ENTRY         GEN_XLOG.AddEntry

#define APPFLOW_LOG_SEPARATOR     __L("-----------------------------------------------------------------------------------------------")

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class APPFLOWCFG;

class APPFLOWLOG
{
  public:

    static bool                         GetIsInstanced                  ();
    static APPFLOWLOG&                  GetInstance                     ();
    static bool                         DelInstance                     ();

    bool                                Ini                             (APPFLOWCFG* cfg, XCHAR* applicationname);

    bool                                End                             ();

  private:
                                        APPFLOWLOG                      ();
                                        APPFLOWLOG                      (APPFLOWLOG const&);        // Don't implement
    virtual                            ~APPFLOWLOG                      ();

    void                                operator =                      (APPFLOWLOG const&);        // Don't implement

    void                                Clean                           ();

    static APPFLOWLOG*                  instance;

    APPFLOWCFG*                         cfg;
};

#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

