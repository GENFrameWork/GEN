/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOWebClient.h
* 
* @class      DIOWEBCLIENT
* @brief      Data Input/Output Web Client class
* @ingroup    DATAIO
* 
* @copyright  EndoraSoft. All rights reserved.
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

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XBuffer.h"
#include "XPath.h"
#include "XSubject.h"

#include "DIOURL.h"
#include "DIOWebHeader.h"



/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


#define DIOWEBCLIENT_TIMEOUT            8
#define DIOWEBCLIENT_MAXSIZEBUFFER      (64*1024)
#define DIOWEBCLIENT_DEFAULTPORT        80
#define DIOWEBCLIENT_DEFAULTSECUREPORT  443

#define DIOWEBCLIENT_DEFAULTUSERAGENT   __L("User-Agent: Mozilla 5.0\r\n")

enum DIOWEBCLIENT_AUTHENTICATION_METHOD
{
  DIOWEBCLIENT_AUTHENTICATION_METHOD_UNKNOWN               = 0 ,
  DIOWEBCLIENT_AUTHENTICATION_METHOD_BASIC                     ,
  DIOWEBCLIENT_AUTHENTICATION_METHOD_DIGEST                    ,      
};


enum DIOWEBCLIENT_BODYMODE
{
  DIOWEBCLIENT_BODYMODE_UNKNOWN                     = 0 ,
  DIOWEBCLIENT_BODYMODE_NONE                            ,
  DIOWEBCLIENT_BODYMODE_CONTENTLENGTH                   ,
  DIOWEBCLIENT_BODYMODE_CHUNKED                         ,
  DIOWEBCLIENT_BODYMODE_CONNECTIONCLOSE                 ,
};


enum DIOWEBCLIENT_CHUNKEDSTATE
{
  DIOWEBCLIENT_CHUNKEDSTATE_SIZE                     = 0 ,
  DIOWEBCLIENT_CHUNKEDSTATE_DATA                         ,
  DIOWEBCLIENT_CHUNKEDSTATE_DATAEND                      ,
  DIOWEBCLIENT_CHUNKEDSTATE_TRAILERS                     ,
  DIOWEBCLIENT_CHUNKEDSTATE_END                          ,
};


enum DIOWEBCLIENT_CHUNKEDRESULT
{
  DIOWEBCLIENT_CHUNKEDRESULT_ERROR                   = -1 ,
  DIOWEBCLIENT_CHUNKEDRESULT_INCOMPLETE               = 0 ,
  DIOWEBCLIENT_CHUNKEDRESULT_COMPLETE                     ,
};




/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class XTIMER;
class XFILE;
class DIOSTREAMTCPIPCONFIG;
class DIOSTREAMTCPIP;
class DIOSTREAMTLSCONFIG;
class DIOWEBCLIENT_XEVENT;


class DIOWEBCLIENT_HEADER : public DIOWEBHEADER
{
  public:
                                              DIOWEBCLIENT_HEADER               ();
    virtual                                  ~DIOWEBCLIENT_HEADER               ();

    bool                                      GetMethod                         (XSTRING& method);
    bool                                      GetResource                       (XSTRING& resource);
    int                                       GetVersion                        ();
    int                                       GetSubVersion                     ();
    int                                       GetResultServer                   ();

    XQWORD                                    GetContentLength                  ();
    bool                                      HasContentLength                  ();
    bool                                      GetContentLength                  (XQWORD& contentlength);
    bool                                      GetTransferEncoding              (XSTRING& transferencoding);
    bool                                      GetETag                           (XSTRING& etag);
    bool                                      GetWWWAuthenticate                (XSTRING& authenticate);

  private:

    void                                      Clean                             ();
};


class DIOWEBCLIENT : public XSUBJECT
{
  public:
                                              DIOWEBCLIENT                      (XDWORD maxsizebuffer = DIOWEBCLIENT_MAXSIZEBUFFER);
    virtual                                  ~DIOWEBCLIENT                      ();

    int                                       Get_Port                          ();
    void                                      Set_Port                          (int port);



    DIOURL*                                   GetProxyURL                       ();
    int                                       GetProxyPort                      ();
    void                                      SetProxyPort                      (int port);


    DIOWEBCLIENT_AUTHENTICATION_METHOD        GetAuthenticationMethod           ();
    void                                      SetAuthenticationMethod           (DIOWEBCLIENT_AUTHENTICATION_METHOD authenticationmethod);

    XSTRING*                                  GetLogin                          ();
    XSTRING*                                  GetPassword                       ();

    DIOSTREAMTCPIPCONFIG*                     GetStreamCFG                      ();
    DIOSTREAMTLSCONFIG*                       GetStreamTLSCFG                   ();

    DIOWEBCLIENT_HEADER*                      GetHeader                         ();
    
    bool                                      IsActiveDoStopHTTPError           ();
    void                                      DoStopHTTPError                   (bool activate);

    bool                                      Get                               (DIOURL& url, XBUFFER& tobuffer , XCHAR* addheader = NULL, int timeout = DIOWEBCLIENT_TIMEOUT, XSTRING* localIP = NULL);
    bool                                      Get                               (XCHAR*  url, XBUFFER& tobuffer , XCHAR* addheader = NULL, int timeout = DIOWEBCLIENT_TIMEOUT, XSTRING* localIP = NULL);
    bool                                      Get                               (DIOURL& url, XPATH& pathfile   , XCHAR* addheader = NULL, int timeout = DIOWEBCLIENT_TIMEOUT, XSTRING* localIP = NULL);
    bool                                      Get                               (XCHAR*  url, XPATH& pathfile   , XCHAR* addheader = NULL, int timeout = DIOWEBCLIENT_TIMEOUT, XSTRING* localIP = NULL);

    bool                                      Put                               (DIOURL& url, XBUFFER& tobuffer , XCHAR* addheader = NULL, int timeout = DIOWEBCLIENT_TIMEOUT, XSTRING* localIP = NULL);
    bool                                      Put                               (XCHAR*  url, XBUFFER& tobuffer , XCHAR* addheader = NULL, int timeout = DIOWEBCLIENT_TIMEOUT, XSTRING* localIP = NULL);
    bool                                      Put                               (DIOURL& url, XPATH& pathfile   , XCHAR* addheader = NULL, int timeout = DIOWEBCLIENT_TIMEOUT, XSTRING* localIP = NULL);
    bool                                      Put                               (XCHAR*  url, XPATH& pathfile   , XCHAR* addheader = NULL, int timeout = DIOWEBCLIENT_TIMEOUT, XSTRING* localIP = NULL);

    bool                                      Post                              (DIOURL& url, XBUFFER& tobuffer , XBUFFER* postdata = NULL, XCHAR* addheader = NULL, int timeout = DIOWEBCLIENT_TIMEOUT, XSTRING* localIP = NULL);
    bool                                      Post                              (XCHAR*  url, XBUFFER& tobuffer , XBUFFER* postdata = NULL, XCHAR* addheader = NULL, int timeout = DIOWEBCLIENT_TIMEOUT, XSTRING* localIP = NULL);
    bool                                      Post                              (DIOURL& url, XPATH& pathfile   , XBUFFER* postdata = NULL, XCHAR* addheader = NULL, int timeout = DIOWEBCLIENT_TIMEOUT, XSTRING* localIP = NULL);
    bool                                      Post                              (XCHAR*  url, XPATH& pathfile   , XBUFFER* postdata = NULL, XCHAR* addheader = NULL, int timeout = DIOWEBCLIENT_TIMEOUT, XSTRING* localIP = NULL);
   
  private:

    bool                                      MakeOperation                     (DIOWEBHEADER_METHOD method, DIOURL& url, XBUFFER* postdata, XCHAR* addhead, int timeout, XSTRING* localIP, bool istobuffer, void* to);
    bool                                      Header_Read                       (int timeout);
    bool                                      Body_Read                         (DIOWEBCLIENT_BODYMODE bodymode, bool isTLS, XQWORD contentlength, int timeout, bool istobuffer, void* to, DIOWEBCLIENT_XEVENT& xevent);
    bool                                      BodyBlock_Write                   (XBYTE* data, XDWORD size, bool istobuffer, void* to, XQWORD& totalsizeread, XQWORD contentlength, XTIMER* timerdownload, DIOWEBCLIENT_XEVENT& xevent);
    DIOWEBCLIENT_CHUNKEDRESULT                ChunkSize_Get                     (XBUFFER& input, XQWORD& chunksize);
    bool                                      Stream_Create                     (bool isTLS);
    bool                                      IsSecureURL                       (DIOURL& url);

    bool                                      GetSubStringWWWWAuthenticate      (XSTRING& www_authenticate, XCHAR* field, XSTRING& value, bool betweenquotation = true);

    void                                      Clean                             ();

    XDWORD                                    maxsizebuffer;

    XTIMER*                                   timerout;

    DIOSTREAMTCPIPCONFIG*                     diostreamcfg;
    DIOSTREAMTCPIP*                           diostream;

    int                                       port;
    bool                                      isportconfigured;
    bool                                      isstreamTLS;

    DIOURL                                    proxyurl;
    int                                       proxyport;

    DIOWEBCLIENT_AUTHENTICATION_METHOD        authenticationmethod;

    XSTRING                                   login;
    XSTRING                                   password;

    DIOWEBCLIENT_HEADER                       header;
    bool                                      dostophttperror;
};




/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/


