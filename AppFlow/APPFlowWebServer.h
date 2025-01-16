/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       APPFlowWebServer.h
* 
* @class      APPFLOWWEBSERVER
* @brief      Application Flow Web Server class
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

#ifndef _APPFLOWWEBSERVER_H_
#define _APPFLOWWEBSERVER_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XString.h"
#include "XPath.h"
#include "XObserver.h"

#include "DIOWebServer_QueryStrings.h"
#include "DIOWebPageHTMLCreator.h"
#include "DIOWebServer.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class XEVENT;
class DIOWEBSERVER_PLUGIN_PHP;
class DIOWEBSERVER_XEVENT;
class DIOWEBSERVER;
class APPFLOWCFG;


class APPFLOWWEBSERVER : public XOBSERVER, public XSUBJECT
{
  public:
                                APPFLOWWEBSERVER                  ();
    virtual                    ~APPFLOWWEBSERVER                  ();

    bool                        Ini                               (APPFLOWCFG* cfg, bool doinitialconnectitivitytest, bool isapirestonly, bool chekuseragentid);
    bool                        Ini                               (XDWORD port, bool doinitialconnectitivitytest, int timeoutserverpage, XSTRING* addrlocal);

    DIOWEBSERVER*               GetWebServer                      ();

    bool                        GetIsAuthenticatedAccess          ();
    bool                        GetIsApiRestOnly                  ();

    virtual bool                SendRequest                       (DIOWEBSERVER_CONNECTION* connection, DIOWEBHEADER_RESULT headerresult, XBUFFER* data , int timeout, XSTRING* addhead = NULL);
    bool                        SendRequest                       (DIOWEBSERVER_CONNECTION* connection, DIOWEBHEADER_RESULT headerresult, XSTRING& data , int timeout, XSTRING* addhead = NULL);
    bool                        SendRequest                       (DIOWEBSERVER_CONNECTION* connection, DIOWEBHEADER_RESULT headerresult, XPATH& file   , int timeout, XSTRING* addhead = NULL);

    virtual bool                ResolveRequest                    (DIOWEBSERVER* server, DIOWEBSERVER_CONNECTION* connection, DIOWEBSERVER_REQUEST*  request);

    virtual bool                GenerateMessagePage               (XCHAR* leyend, DIOWEBPAGEHTMLCREATOR& webHTMLpage);
    bool                        GenerateMessagePage               (XSTRING& leyend, DIOWEBPAGEHTMLCREATOR& webHTMLpage);
    bool                        GenerateResponse_Error            (DIOWEBSERVER_CONNECTION* connection, DIOWEBHEADER_RESULT result, XCHAR* leyend);
    bool                        GenerateResponse_Error            (DIOWEBSERVER_CONNECTION* connection, DIOWEBHEADER_RESULT result, XSTRING& leyend);

    bool                        End                               ();

  private:

    void                        HandleEvent_WebServer             (DIOWEBSERVER_XEVENT* event);
    void                        HandleEvent                       (XEVENT* xevent);

    void                        Clean                             ();

    APPFLOWCFG*                 cfg;

    DIOWEBSERVER*               webserver;

    bool                        isauthenticatedaccess;
    bool                        isapirestonly;

    XSTRING                     page_login;
    XSTRING                     page_password;

    DIOWEBSERVER_PLUGIN_PHP*    pluginPHP;
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

