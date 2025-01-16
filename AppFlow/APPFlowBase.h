/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       APPFlowBase.h
* 
* @class      APPFLOWBASE
* @brief      Application Flow Base class
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

#ifndef _APPFLOWBASE_H_
#define _APPFLOWBASE_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include <stdio.h>

#include "XString.h"
#include "XTrace.h"
#include "XSystem.h"
#include "XObserver.h"
#include "XSubject.h"
#include "XVector.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

enum APPFLOWBASE_EXITTYPE
{
  APPFLOWBASE_EXITTYPE_UNKNOWN                          = 0 ,
  APPFLOWBASE_EXITTYPE_BY_SERIOUSERROR                      ,
  APPFLOWBASE_EXITTYPE_BY_UPDATE                            ,
  APPFLOWBASE_EXITTYPE_BY_APPLICATION                       ,
  APPFLOWBASE_EXITTYPE_BY_USER                              ,
  APPFLOWBASE_EXITTYPE_BY_SHUTDOWN                          ,
  APPFLOWBASE_EXITTYPE_BY_INVALIDLICENSE                    ,
  APPFLOWBASE_EXITTYPE_BY_EXPIREDLICENSE                    ,
  APPFLOWBASE_EXITTYPE_BY_SERVICERELOAD             
};


enum APPFLOWBASE_MODE_TYPE
{
  APPFLOWBASE_MODE_TYPE_UNKNOWN                         = 0 ,
  APPFLOWBASE_MODE_TYPE_APPLICATION                         ,
  APPFLOWBASE_MODE_TYPE_SERVICE                             ,
  APPFLOWBASE_MODE_TYPE_DINAMICLIBRARY                      ,
};


#define APPFLOW_DEFAULT_DIRECTORY_ROOT            __L("assets")
#define APPFLOW_DEFAULT_DIRECTORY_WEB             __L("web")
#define APPFLOW_DEFAULT_DIRECTORY_DATABASES       __L("databases")
#define APPFLOW_DEFAULT_DIRECTORY_GRAPHICS        __L("graphics")
#define APPFLOW_DEFAULT_DIRECTORY_UI_LAYOUTS      __L("ui_layouts")
#define APPFLOW_DEFAULT_DIRECTORY_SOUNDS          __L("sounds")
#define APPFLOW_DEFAULT_DIRECTORY_FONTS           __L("fonts")
#define APPFLOW_DEFAULT_DIRECTORY_SCRIPTS         __L("scripts")
#define APPFLOW_DEFAULT_DIRECTORY_FIRMWARE        __L("firmware")
#define APPFLOW_DEFAULT_DIRECTORY_CERTIFICATES    __L("certificates")

#define APPLICATIONCREATEINSTANCE(CLASSNAME, VARIABLE)    CLASSNAME* VARIABLE = NULL;                     \
                                                          APPFLOWBASE* APPFLOWBASE::Create()              \
                                                          {                                               \
                                                            VARIABLE = new CLASSNAME();                   \
                                                            return VARIABLE;                              \
                                                          }

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class XTIMER;

class APPFLOWBASE :  public XSUBJECT
{
  public:

                                        APPFLOWBASE                   ();
    virtual                            ~APPFLOWBASE                   ();
      
    static APPFLOWBASE*                 Create                        ();
      
    virtual bool                        Ini                           (XVECTOR<XSTRING*>* execparams = NULL);
    virtual bool                        UpdateStep                    ();
    virtual bool                        End                           ();
      
    XVECTOR<XSTRING*>*                  GetExecParams                 ();

    APPFLOWBASE_MODE_TYPE               Application_GetMode           ();
    void                                Application_SetMode           (APPFLOWBASE_MODE_TYPE applicationmode);    
    void*                               Application_GetHandle         ();
    void                                Application_SetHandle         (void* handle);    
    XSTRING*                            Application_GetName           ();
    void                                Application_SetName           (XCHAR* name);    
    XSTRING*                            Application_GetExecutable     ();
    void                                Application_SetExecutable     (XCHAR* name);    
    XSTRING*                            Application_GetID             ();
    
    XTIMER*                             GetTimerGlobal                ();
    
    APPFLOWBASE_EXITTYPE                GetExitType                   ();
    void                                SetExitType                   (APPFLOWBASE_EXITTYPE exittype);
    bool                                GetExitTypeString             (XSTRING& exittypestring);
    
    XSYSTEM_CHANGESTATUSTYPE            GetSystemChangeStatus         ();
    void                                SetSystemChangeStatus         (XSYSTEM_CHANGESTATUSTYPE systemchangestatustype);
    
    virtual bool                        AppProc_PlatformIni           ();
    virtual bool                        AppProc_Ini                   ();
    virtual bool                        AppProc_FirstUpdate           ();
    virtual bool                        AppProc_Update                ();
    virtual bool                        AppProc_LastUpdate            ();
    virtual bool                        AppProc_EndBlockedResources   ();
    virtual bool                        AppProc_End                   ();
    virtual bool                        AppProc_PlatformEnd           ();


    

  protected:

    XVECTOR<XSTRING*>*                  execparams;

    APPFLOWBASE_MODE_TYPE               application_mode;
    void*                               application_handle;
    XSTRING                             application_name;
    XSTRING                             application_executable;
    XSTRING                             application_ID;
    
    XTIMER*                             xtimerglobal;
    
    bool                                firstupdate;
    
    APPFLOWBASE_EXITTYPE                exittype;
    XSYSTEM_CHANGESTATUSTYPE            systemchangestatustype;
    
  private:    
    
    void                                Clean                         ();
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif
