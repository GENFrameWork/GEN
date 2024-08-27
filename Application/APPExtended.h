/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       APPExtended.h
* 
* @class      APPEXTENDED
* @brief      Application Extended class
* @ingroup    APPLICATION
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

#ifndef _APPEXTENDED_H_
#define _APPEXTENDED_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XPath.h"
#include "XString.h"
#include "XLog.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

#define APP_EXTENDED             APPEXTENDED::GetInstance()

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class XCONSOLE;
class APPCFG;
class APPEXTENDED_APPLICATIONSTATUS;

class APPEXTENDED
{
  public:

    static bool                         GetIsInstanced                  ();
    static APPEXTENDED&                 GetInstance                     ();
    static bool                         DelInstance                     ();

    bool                                APPStart                        (APPCFG* cfg, XCONSOLE* console = NULL);
    bool                                APPEnd                          (APPCFG* cfg, XCONSOLE* console = NULL);

    bool                                ShowLine                        (XCONSOLE* console, XSTRING& string, XSTRING& string2, int tab = 3, bool linefeed = true);
    
    #ifdef APP_EXTENDED_APPLICATIONHEADER_ACTIVE
    bool                                ShowHeader                      (XCONSOLE* console, bool separator);
    #endif
   
    #ifdef APP_EXTENDED_APPLICATIONSTATUS_ACTIVE
    APPEXTENDED_APPLICATIONSTATUS*      GetApplicationStatus            (); 

    XMUTEX*                             GetXMutexShowAll                (); 
    bool                                ShowAll                         (XCONSOLE* console);

    #endif
    
  private:
                                        APPEXTENDED                     ();
                                        APPEXTENDED                     (APPEXTENDED const&);        // Don't implement
    virtual                            ~APPEXTENDED                     ();

    void                                operator =                      (APPEXTENDED const&);        // Don't implement

    void                                Clean                           ();

    static APPEXTENDED*                 instance;

    APPCFG*                             cfg;
    XMUTEX*                             xmutexshowall; 

    #ifdef APP_EXTENDED_APPLICATIONSTATUS_ACTIVE
    APPEXTENDED_APPLICATIONSTATUS*      applicationstatus;
    #endif
};

#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

