/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       Script.h
* 
* @class      SCRIPT
* @brief      Script base class
* @ingroup    SCRIPT
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

#ifndef _SCRIPT_H_
#define _SCRIPT_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XBase.h"
#include "XVector.h"
#include "XMap.h"
#include "XSubject.h"
#include "XString.h"
#include "XPath.h"
#include "XEvent.h"
#include "XVariant.h"
#include "XThread.h"
#include "XThreadCollected.h"

#include "Script_ErrorCode.h"
#include "Script_Lib_Function.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

enum SCRIPT_TYPE
{
  SCRIPT_TYPE_UNKNOWN   = 0 ,
  SCRIPT_TYPE_G             ,
  SCRIPT_TYPE_LUA           ,
  SCRIPT_TYPE_JAVASCRIPT    ,
};


class SCRIPT;
class SCRIPT_LIB;

typedef void (*SCRFUNCIONLIBRARY) (SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue);



#define SCRIPT_SET_LIB_CFG(config)        { SCRIPT_LIB_CFG* lib = (SCRIPT_LIB_CFG*)script->GetLibrary(__L("Config")); \
                                            if(lib) \
                                              { \
                                                lib->SetXFileCFG(&config); \
                                              } \
                                          }
#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class XTIMER;
class XPUBLISHER;
class XTHREADCOLLECTED;
class XFILETXT;
class SCRIPTVAR;

class SCRIPT : public XSUBJECT
{
  public:
                                        SCRIPT                        ();
    virtual                            ~SCRIPT                        ();                                       

    static SCRIPT*                      Create                        (XCHAR* namefilescript);

    virtual bool                        Load                          (XPATH& xpath);
    virtual bool                        Save                          (XPATH& xpath);

    static bool                         IsScript                      (XPATH& xpath, XCHAR* extension);

    XSTRING*                            GetNameScript                 ();
    XSTRING*                            GetScript                     ();
    XPATH*                              GetPath                       ();

    virtual int                         Run                           (int* returnval = NULL);

    bool                                RunWithThread                 ();
    bool                                IsRunWithThread               ();
    bool                                IsRunThread                   (int* error = NULL,int* returnvalue = NULL);

    bool                                CancelExecution               ();

    XTIMER*                             GetTimer                      ();
    XTHREADCOLLECTED*                   GetThread                     ();

    SCRIPT_LIB*                         GetLibrary                    (XCHAR* ID);
    bool                                AddLibrary                    (SCRIPT_LIB* scriptlib);
    bool                                DeleteAllLibrarys             ();

    SCRIPT_LIB_FUNCTION*                GetLibraryFunction            (XCHAR* name) ;
    SCRIPT_LIB_FUNCTION*                GetLibraryFunction            (void* ptrfunction);
    virtual bool                        AddLibraryFunction            (SCRIPT_LIB* library, XCHAR* name, SCRFUNCIONLIBRARY ptrfunction);
    bool                                DeleteLibraryFuncions         ();

    bool                                AddInternalLibraries          ();  

    int                                 GetErrorScript                ();
    bool                                SetErrorScript                (int errorcode);

    virtual bool                        HaveError                     (int errorcode);

  protected:

    SCRIPT_TYPE                         type;
    XPATH                               xpath;
    XFILETXT*                           xfiletxt;
    XSTRING                             namescript;
    XSTRING                             script;

    XTIMER*                             xtimer;
    int                                 errorcode;

    XTHREADCOLLECTED*                   thread;
    bool                                isrunwiththread;

    int                                 returnvaluescript;

    bool                                iscancelexec;

    XVECTOR<SCRIPT_LIB*>                librarys;
    XVECTOR<SCRIPT_LIB_FUNCTION*>       libraryfunctions;

  private:

    static void                         ThreadFunction          (void* data);
    void                                Clean                   ();
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif
