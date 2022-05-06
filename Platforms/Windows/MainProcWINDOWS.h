/**-------------------------------------------------------------------------------------------------------------------
*
* @file       MainProcWINDOWS.h
*
* @class      MAINPROCWINDOWS
* @brief      Main Procedure WINDOWS class
* @ingroup    PLATFORM_WINDOWS
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

#ifndef _MAINPROCWINDOWS_H_
#define _MAINPROCWINDOWS_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <StackWalker.h>

#include "XWINDOWSServiceBase.h"

#include "XString.h"
#include "XSystem.h"

#include "APPBase.h"
#include "APPMain.h"

#include "MainProc.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


#ifndef APPMODE_SERVICE_NAME
  #define APPMODE_SERVICE_NAME              __L("GEN Service")
#endif

#ifndef APPMODE_SERVICE_DISPLAYNAME
  #define APPMODE_SERVICE_DISPLAYNAME       __L("GEN Service")
#endif


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class MAINPROCWINDOWS : public MAINPROC
{
  public:
                                    MAINPROCWINDOWS                   ();
    virtual                        ~MAINPROCWINDOWS                   ();

    bool                            Ini                               (APPMAIN* appmain = NULL, APPBASE_APPLICATIONMODE_TYPE applicationmode = APPBASE_APPLICATIONMODE_TYPE_UNKNOWN);
    bool                            Update                            ();
    bool                            End                               ();

    bool                            Factorys_Ini                      ();
    bool                            Factorys_End                      ();

    int                             MainLoop                          ();

    bool                            IsRunningAsService                ();   

  private:

    void                            Clean                             ();

    bool                            isrunningasservice;    
};


#if !defined(APPMODE_LIBRARY_DINAMIC) && !defined(APPMODE_LIBRARY_DINAMIC)

class WINDOWSSERVICE : public XWINDOWSSERVICEBASE
{
  public:

                                    WINDOWSSERVICE                    (XCHAR* servicename, bool can_stop = true, bool can_shutdown = true, bool can_pause = false, bool can_powerevent = true, bool can_sessionchange = true);
    virtual                        ~WINDOWSSERVICE                    ();

    HANDLE                          GetHandleStoppedEvent             (); 

    bool                            MustRestartService                ();
    void                            SetMustRestartService             (bool reloadservice);  

  protected:

    virtual void                    On_Start                          (DWORD argc, XCHAR** args);
    virtual void                    On_Stop                           ();
    virtual void                    On_Shutdown                       ();
    virtual void                    On_PowerEvent                     (DWORD eventtype);
    virtual void                    On_SessionChange                  (DWORD eventtype);

  private:

    void                            Clean                             ();

    HANDLE                          event_stopped;
    bool                            mustrestartservice;
};

#endif


class MAINPROCWINDOWSSTACKWALKER : public StackWalker
{
  public:

    virtual void                    OnSymInit                         (LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName);
    virtual void                    OnLoadModule                      (LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion)   {    }
    virtual void                    OnCallstackEntry                  (CallstackEntryType eType, CallstackEntry &entry);
    virtual void                    OnDbgHelpErr                      (LPCSTR szFuncName, DWORD gle, DWORD64 addr)                                                                                  {    }
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

extern int                          Exception_Filter                  (XDWORD code, struct _EXCEPTION_POINTERS *ep);


extern MAINPROCWINDOWS              mainprocwindows;

#endif

