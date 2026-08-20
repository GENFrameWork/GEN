/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOWebClient.cpp
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"



/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DIOWebClient.h"

#include "XFactory.h"
#include "XSleep.h"
#include "XTimer.h"
#include "XPath.h"
#include "XFile.h"
#include "XTrace.h"

#include "HashMD5.h"

#include "DIOFactory.h"
#include "DIOURL.h"
#include "DIOStreamTCPIPConfig.h"
#include "DIOStreamTCPIP.h"

#ifdef DIO_STREAMTLS_ACTIVE
#include "DIOStreamTLSConfig.h"
#endif

#include "DIOWebClient_XEvent.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/




/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOWEBCLIENT_HEADER::DIOWEBCLIENT_HEADER()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOWEBCLIENT_HEADER::DIOWEBCLIENT_HEADER()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOWEBCLIENT_HEADER::~DIOWEBCLIENT_HEADER()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOWEBCLIENT_HEADER::~DIOWEBCLIENT_HEADER()
{
  DeleteAllLines();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT_HEADER::GetMethod(XSTRING& method)
* @brief      Get method
* @ingroup    DATAIO
* 
* @param[in]  method : Method descriptor to call.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT_HEADER::GetMethod(XSTRING& method)
{
  if(!GetFormat(false, 0, &method)) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT_HEADER::GetResource(XSTRING& resource)
* @brief      Get resource
* @ingroup    DATAIO
* 
* @param[in]  resource : Resource value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT_HEADER::GetResource(XSTRING& resource)
{
  if(!GetFormat(false, 1, &resource)) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOWEBCLIENT_HEADER::GetVersion()
* @brief      Get version
* @ingroup    DATAIO
* 
* @return     int : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOWEBCLIENT_HEADER::GetVersion()
{
  int data = 0;

  if(!GetFormat(false, 2, &data)) return false;

  return data;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOWEBCLIENT_HEADER::GetSubVersion()
* @brief      Get sub version
* @ingroup    DATAIO
* 
* @return     int : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOWEBCLIENT_HEADER::GetSubVersion()
{
  int data = 0;

  if(!GetFormat(false, 3, &data)) return false;

  return data;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOWEBCLIENT_HEADER::GetResultServer()
* @brief      Get result server
* @ingroup    DATAIO
* 
* @return     int : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOWEBCLIENT_HEADER::GetResultServer()
{
  int data = 0;

  if(!GetFormat(true, 2, &data)) return false;

  return data;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XQWORD DIOWEBCLIENT_HEADER::GetContentLength()
* @brief      Get content length
* @ingroup    DATAIO
* 
* @return     XQWORD : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XQWORD DIOWEBCLIENT_HEADER::GetContentLength()
{
  XQWORD contentlength = 0;

  GetContentLength(contentlength);

  return contentlength;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT_HEADER::HasContentLength()
* @brief      Check whether a Content-Length response field is present
* @ingroup    DATAIO
* 
* @return     bool : true when the field is present; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT_HEADER::HasContentLength()
{
  for(XDWORD c=0; c<lines.GetSize(); c++)
    {
      XSTRING* line = lines.Get(c);
      if(line)
        {
          int index = line->Find(__L(":"), true);
          if(index != XSTRING_NOTFOUND)
            {
              XSTRING field;

              line->Copy(0, index, field);
              if(!field.Compare(DIOWEBHEADER_CONTENT_LENGTH, true)) return true;
            }
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT_HEADER::GetContentLength(XQWORD& contentlength)
* @brief      Get and validate the Content-Length response field
* @ingroup    DATAIO
* 
* @param[out] contentlength : Parsed content length.
* 
* @return     bool : true when a valid Content-Length field is present; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT_HEADER::GetContentLength(XQWORD& contentlength)
{
  bool found = false;

  contentlength = 0;

  for(XDWORD c=0; c<lines.GetSize(); c++)
    {
      XSTRING* line = lines.Get(c);
      if(line)
        {
          int index = line->Find(__L(":"), true);
          if(index != XSTRING_NOTFOUND)
            {
              XSTRING field;

              line->Copy(0, index, field);
              if(field.Compare(DIOWEBHEADER_CONTENT_LENGTH, true)) continue;

              XDWORD  position = index + 1;
              XQWORD  value    = 0;
              bool    hasdigit = false;

              while(position < line->GetSize())
                {
                  XCHAR character = line->Get()[position];

                  if((character != __C(' ')) && (character != __C('\t'))) break;

                  position++;
                }

              while(position < line->GetSize())
                {
                  XCHAR character = line->Get()[position];

                  if((character < __C('0')) || (character > __C('9'))) break;

                  XQWORD digit   = character - __C('0');
                  XQWORD maximum = (XQWORD)-1;

                  if(value > ((maximum - digit) / 10)) return false;

                  value     = (value * 10) + digit;
                  hasdigit  = true;
                  position++;
                }

              if(!hasdigit) return false;

              while(position < line->GetSize())
                {
                  XCHAR character = line->Get()[position];

                  if((character != __C(' ')) && (character != __C('\t'))) return false;

                  position++;
                }

              if(found && (contentlength != value)) return false;

              contentlength = value;
              found         = true;
            }
        }
    }

  return found;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT_HEADER::GetTransferEncoding(XSTRING& transferencoding)
* @brief      Get the combined Transfer-Encoding response field
* @ingroup    DATAIO
* 
* @param[out] transferencoding : Transfer coding list.
* 
* @return     bool : true when the field is present; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT_HEADER::GetTransferEncoding(XSTRING& transferencoding)
{
  bool found = false;

  transferencoding.Empty();

  for(XDWORD c=0; c<lines.GetSize(); c++)
    {
      XSTRING* line = lines.Get(c);
      if(line)
        {
          int index = line->Find(__L(":"), true);
          if(index != XSTRING_NOTFOUND)
            {
              XSTRING field;
              XSTRING value;

              line->Copy(0, index, field);
              if(field.Compare(DIOWEBHEADER_TRANSFER_ENCODING, true)) continue;

              line->Copy(index+1, value);

              while(value.GetSize() && ((value.Get()[0] == __C(' ')) || (value.Get()[0] == __C('\t'))))
                {
                  value.DeleteCharacters(0, 1);
                }

              while(value.GetSize() && ((value.Get()[value.GetSize()-1] == __C(' ')) ||
                                        (value.Get()[value.GetSize()-1] == __C('\t'))))
                {
                  value.DeleteCharacters(value.GetSize()-1, 1);
                }

              if(found) transferencoding += __L(",");

              transferencoding += value;
              found              = true;
            }
        }
    }

  return found;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT_HEADER::GetETag(XSTRING& etag)
* @brief      Get E tag
* @ingroup    DATAIO
* 
* @param[in]  etag : Etag value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT_HEADER::GetETag(XSTRING& etag)
{
  etag.Empty();

  XCHAR* string = GetFieldValue(DIOWEBHEADER_ETAG);
  if(string)  etag = string;

  return etag.GetSize()?true:false;

  /*
  for(XDWORD c=0; c<lines.GetSize(); c++)
    {
      XSTRING* line = lines.Get(c);
      if(line)
        {
          if(line.Find(DIOWEBHEADER_ETAG, true) != XSTRING_NOTFOUND)
            {
              int index = line.Find(__L(":"), true);
              if(index != XSTRING_NOTFOUND)  line.Copy(index+1, etag);
            }
        }
    }

  return etag.GetSize()?true:false;
  */
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT_HEADER::GetWWWAuthenticate(XSTRING& authenticate)
* @brief      Get WWW authenticate
* @ingroup    DATAIO
* 
* @param[in]  authenticate : Authenticate value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT_HEADER::GetWWWAuthenticate(XSTRING& authenticate)
{
  authenticate.Empty();

  XCHAR* string = GetFieldValue(DIOWEBHEADER_WWW_AUTHENTICATE);
  if(string)  authenticate = string;

  return authenticate.GetSize()?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOWEBCLIENT_HEADER::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBCLIENT_HEADER::Clean()
{

}






/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOWEBCLIENT::DIOWEBCLIENT(XDWORD maxsizebuffer)
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* @param[in]  maxsizebuffer : Maxsizebuffer value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOWEBCLIENT::DIOWEBCLIENT(XDWORD maxsizebuffer)
{
  Clean();

  this->maxsizebuffer = maxsizebuffer;

  RegisterEvent(DIOWEBCLIENT_XEVENT_TYPE_OPENWEB);
  RegisterEvent(DIOWEBCLIENT_XEVENT_TYPE_WRITEHEADER);
  RegisterEvent(DIOWEBCLIENT_XEVENT_TYPE_SENDPOSTDATA);
  RegisterEvent(DIOWEBCLIENT_XEVENT_TYPE_HEADERERROR);
  RegisterEvent(DIOWEBCLIENT_XEVENT_TYPE_READHEADER);
  RegisterEvent(DIOWEBCLIENT_XEVENT_TYPE_READBODYBLOCK);
  RegisterEvent(DIOWEBCLIENT_XEVENT_TYPE_CLOSEWEB);

  timerout=GEN_XFACTORY.CreateTimer();
  
  #ifdef DIO_STREAMTLS_ACTIVE

  diostreamcfg = GEN_NEW DIOSTREAMTLSCONFIG();

  #else

  diostreamcfg = GEN_NEW DIOSTREAMTCPIPCONFIG();

  #endif

  if(diostreamcfg)
    {
      diostreamcfg->SetIsTLS(false);
      Stream_Create(false);
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOWEBCLIENT::~DIOWEBCLIENT()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOWEBCLIENT::~DIOWEBCLIENT()
{
  DeRegisterEvent(DIOWEBCLIENT_XEVENT_TYPE_OPENWEB);
  DeRegisterEvent(DIOWEBCLIENT_XEVENT_TYPE_WRITEHEADER);
  DeRegisterEvent(DIOWEBCLIENT_XEVENT_TYPE_SENDPOSTDATA);
  DeRegisterEvent(DIOWEBCLIENT_XEVENT_TYPE_HEADERERROR);
  DeRegisterEvent(DIOWEBCLIENT_XEVENT_TYPE_READHEADER);
  DeRegisterEvent(DIOWEBCLIENT_XEVENT_TYPE_READBODYBLOCK);
  DeRegisterEvent(DIOWEBCLIENT_XEVENT_TYPE_CLOSEWEB);

  if(diostream)
    {
      GEN_DIOFACTORY.DeleteStreamIO(diostream);
      diostream = NULL;
    }

  if(diostreamcfg)
    {
      GEN_DELETE diostreamcfg;
      diostreamcfg = NULL;
    }

  if(timerout)
    {
      GEN_XFACTORY.DeleteTimer(timerout);
      timerout = NULL;
    }

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOWEBCLIENT::Get_Port()
* @brief      Get port
* @ingroup    DATAIO
* 
* @return     int : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOWEBCLIENT::Get_Port()
{
  return port;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOWEBCLIENT::Set_Port(int port)
* @brief      Set port
* @ingroup    DATAIO
* 
* @param[in]  port : Port number to use.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBCLIENT::Set_Port(int port)
{
  this->port       = port;
  isportconfigured = true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOURL* DIOWEBCLIENT::GetProxyURL()
* @brief      Get proxy URL
* @ingroup    DATAIO
* 
* @return     DIOURL* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOURL* DIOWEBCLIENT::GetProxyURL()
{
  return &proxyurl;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOWEBCLIENT::GetProxyPort()
* @brief      Get proxy port
* @ingroup    DATAIO
* 
* @return     int : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOWEBCLIENT::GetProxyPort()
{
  return proxyport;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOWEBCLIENT::SetProxyPort(int port)
* @brief      Set proxy port
* @ingroup    DATAIO
* 
* @param[in]  port : Port number to use.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBCLIENT::SetProxyPort(int port)
{
  proxyport = port;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOWEBCLIENT_AUTHENTICATION_METHOD DIOWEBCLIENT::GetAuthenticationMethod()
* @brief      Get authentication method
* @ingroup    DATAIO
* 
* @return     DIOWEBCLIENT_AUTHENTICATION_METHOD : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOWEBCLIENT_AUTHENTICATION_METHOD DIOWEBCLIENT::GetAuthenticationMethod()
{
  return authenticationmethod;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOWEBCLIENT::SetAuthenticationMethod(DIOWEBCLIENT_AUTHENTICATION_METHOD authenticationmethod)
* @brief      Set authentication method
* @ingroup    DATAIO
* 
* @param[in]  authenticationmethod : Authenticationmethod value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBCLIENT::SetAuthenticationMethod(DIOWEBCLIENT_AUTHENTICATION_METHOD authenticationmethod)
{
  this->authenticationmethod = authenticationmethod;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* DIOWEBCLIENT::GetLogin()
* @brief      Get login
* @ingroup    DATAIO
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING*  DIOWEBCLIENT::GetLogin()
{
  return &login;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* DIOWEBCLIENT::GetPassword()
* @brief      Get password
* @ingroup    DATAIO
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING*  DIOWEBCLIENT::GetPassword()
{
  return &password;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTCPIPCONFIG* DIOWEBCLIENT::GetStreamCFG()
* @brief      Get stream CFG
* @ingroup    DATAIO
* 
* @return     DIOSTREAMTCPIPCONFIG* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTCPIPCONFIG* DIOWEBCLIENT::GetStreamCFG()
{
  return diostreamcfg;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTLSCONFIG* DIOWEBCLIENT::GetStreamTLSCFG()
* @brief      Get TLS stream CFG
* @ingroup    DATAIO
* 
* @return     DIOSTREAMTLSCONFIG* : Pointer to the TLS configuration; NULL if TLS is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSCONFIG* DIOWEBCLIENT::GetStreamTLSCFG()
{
  #ifdef DIO_STREAMTLS_ACTIVE

  return (DIOSTREAMTLSCONFIG*)diostreamcfg;

  #else

  return NULL;

  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOWEBCLIENT_HEADER* DIOWEBCLIENT::GetHeader()
* @brief      Get header
* @ingroup    DATAIO
* 
* @return     DIOWEBCLIENT_HEADER* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOWEBCLIENT_HEADER* DIOWEBCLIENT::GetHeader()
{
  return &header;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::IsActiveDoStopHTTPError()
* @brief      Is active do stop HTTP error
* @ingroup    DATAIO
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::IsActiveDoStopHTTPError()
{
  return dostophttperror;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOWEBCLIENT::DoStopHTTPError(bool activate)
* @brief      Do stop HTTP error
* @ingroup    DATAIO
* 
* @param[in]  activate : Activate value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBCLIENT::DoStopHTTPError(bool activate)
{
  dostophttperror = activate;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::Get(DIOURL& url, XBUFFER& tobuffer, XCHAR* addheader, int timeout, XSTRING* localIP)
* @brief      Get value
* @ingroup    DATAIO
* 
* @param[in]  url : URL to use.
* @param[in]  tobuffer : Tobuffer value.
* @param[in]  addheader : Addheader pointer to use.
* @param[in]  timeout : Timeout value.
* @param[in]  localIP : Local IP pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::Get(DIOURL& url, XBUFFER& tobuffer, XCHAR* addheader, int timeout, XSTRING* localIP)
{
  return MakeOperation(DIOWEBHEADER_METHOD_GET, url, NULL, addheader, timeout, localIP, true, (void*)&tobuffer);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::Get(XCHAR* url, XBUFFER& tobuffer, XCHAR* addheader, int timeout, XSTRING* localIP)
* @brief      Get value
* @ingroup    DATAIO
* 
* @param[in]  url : URL to use.
* @param[in]  tobuffer : Tobuffer value.
* @param[in]  addheader : Addheader pointer to use.
* @param[in]  timeout : Timeout value.
* @param[in]  localIP : Local IP pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::Get(XCHAR* url, XBUFFER& tobuffer, XCHAR* addheader, int timeout, XSTRING* localIP)
{
  DIOURL _url;
  _url = url;

  return MakeOperation(DIOWEBHEADER_METHOD_GET, _url, NULL, addheader, timeout, localIP, true, (void*)&tobuffer);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::Get(DIOURL& url, XPATH& pathfile, XCHAR* addheader, int timeout, XSTRING* localIP)
* @brief      Get value
* @ingroup    DATAIO
* 
* @param[in]  url : URL to use.
* @param[in]  pathfile : Pathfile value.
* @param[in]  addheader : Addheader pointer to use.
* @param[in]  timeout : Timeout value.
* @param[in]  localIP : Local IP pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::Get(DIOURL& url, XPATH& pathfile, XCHAR* addheader, int timeout, XSTRING* localIP)
{
  XFILE* file=GEN_XFACTORY.Create_File();
  if(!file) return false;
  
  bool status  = false;

  if(!file->Open(pathfile, false))
    {
      if(file->Create(pathfile))
        {
          status = true;
        }

    } else status = true;

  if(status)
    {      
      status = MakeOperation(DIOWEBHEADER_METHOD_GET, url, NULL, addheader, timeout, localIP, false, (void*)file);

      file->Close();

      if(!status)  file->Erase(pathfile);
    }

  GEN_XFACTORY.Delete_File(file);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::Get(XCHAR* url, XPATH& pathfile, XCHAR* addheader, int timeout, XSTRING* localIP)
* @brief      Get value
* @ingroup    DATAIO
* 
* @param[in]  url : URL to use.
* @param[in]  pathfile : Pathfile value.
* @param[in]  addheader : Addheader pointer to use.
* @param[in]  timeout : Timeout value.
* @param[in]  localIP : Local IP pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::Get(XCHAR* url, XPATH& pathfile, XCHAR* addheader, int timeout, XSTRING* localIP)
{
  DIOURL _url;
  _url = url;

  return Get(_url, pathfile, addheader, timeout, localIP);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::Put(DIOURL& url, XBUFFER& tobuffer, XCHAR* addheader, int timeout, XSTRING* localIP)
* @brief      Put
* @ingroup    DATAIO
* 
* @param[in]  url : URL to use.
* @param[in]  tobuffer : Tobuffer value.
* @param[in]  addheader : Addheader pointer to use.
* @param[in]  timeout : Timeout value.
* @param[in]  localIP : Local IP pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::Put(DIOURL& url, XBUFFER& tobuffer, XCHAR* addheader, int timeout, XSTRING* localIP)
{
  return MakeOperation(DIOWEBHEADER_METHOD_PUT, url, NULL, addheader, timeout, localIP, true, (void*)&tobuffer);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::Put(XCHAR* url, XBUFFER& tobuffer, XCHAR* addheader, int timeout, XSTRING* localIP)
* @brief      Put
* @ingroup    DATAIO
* 
* @param[in]  url : URL to use.
* @param[in]  tobuffer : Tobuffer value.
* @param[in]  addheader : Addheader pointer to use.
* @param[in]  timeout : Timeout value.
* @param[in]  localIP : Local IP pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::Put(XCHAR* url, XBUFFER& tobuffer, XCHAR* addheader, int timeout, XSTRING* localIP)
{
  DIOURL _url;
  _url = url;

  return MakeOperation(DIOWEBHEADER_METHOD_PUT, _url, NULL, addheader, timeout, localIP, true, (void*)&tobuffer);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::Put(DIOURL& url, XPATH& pathfile, XCHAR* addheader, int timeout, XSTRING* localIP)
* @brief      Put
* @ingroup    DATAIO
* 
* @param[in]  url : URL to use.
* @param[in]  pathfile : Pathfile value.
* @param[in]  addheader : Addheader pointer to use.
* @param[in]  timeout : Timeout value.
* @param[in]  localIP : Local IP pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::Put(DIOURL& url, XPATH& pathfile, XCHAR* addheader, int timeout, XSTRING* localIP)
{
  XFILE* file=GEN_XFACTORY.Create_File();
  if(!file) return false;

  XBUFFER buffer;
  bool    status  = false;

  if(!file->Open(pathfile, false))
    {
      if(file->Create(pathfile))
        {
          status = true;
        }

    } else status = true;

  if(status)
    {
      status = MakeOperation(DIOWEBHEADER_METHOD_PUT, url, &buffer, addheader, timeout, localIP, false, (void*)file);
      if(status) file->Write(buffer);

      file->Close();

      if(!status)  file->Erase(pathfile);
    }

  GEN_XFACTORY.Delete_File(file);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::Put(XCHAR* url, XPATH& pathfile, XCHAR* addheader, int timeout, XSTRING* localIP)
* @brief      Put
* @ingroup    DATAIO
* 
* @param[in]  url : URL to use.
* @param[in]  pathfile : Pathfile value.
* @param[in]  addheader : Addheader pointer to use.
* @param[in]  timeout : Timeout value.
* @param[in]  localIP : Local IP pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::Put(XCHAR* url, XPATH& pathfile, XCHAR* addheader, int timeout, XSTRING* localIP)
{
  DIOURL _url;
  _url = url;

  return Put(_url, pathfile, addheader, timeout, localIP);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::Post(DIOURL& url, XBUFFER& tobuffer, XBUFFER* postdata, XCHAR* addheader, int timeout, XSTRING* localIP)
* @brief      Post
* @ingroup    DATAIO
* 
* @param[in]  url : URL to use.
* @param[in]  tobuffer : Tobuffer value.
* @param[in]  postdata : Postdata pointer to use.
* @param[in]  addheader : Addheader pointer to use.
* @param[in]  timeout : Timeout value.
* @param[in]  localIP : Local IP pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::Post(DIOURL& url, XBUFFER& tobuffer, XBUFFER* postdata, XCHAR* addheader, int timeout, XSTRING* localIP)
{
  return MakeOperation(DIOWEBHEADER_METHOD_POST, url, postdata, addheader, timeout, localIP, true, (void*)&tobuffer);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::Post(XCHAR* url, XBUFFER& tobuffer, XBUFFER* postdata, XCHAR* addheader, int timeout, XSTRING* localIP)
* @brief      Post
* @ingroup    DATAIO
* 
* @param[in]  url : URL to use.
* @param[in]  tobuffer : Tobuffer value.
* @param[in]  postdata : Postdata pointer to use.
* @param[in]  addheader : Addheader pointer to use.
* @param[in]  timeout : Timeout value.
* @param[in]  localIP : Local IP pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::Post(XCHAR* url, XBUFFER& tobuffer, XBUFFER* postdata, XCHAR* addheader, int timeout, XSTRING* localIP)
{
  DIOURL _url;
  _url = url;

  return MakeOperation(DIOWEBHEADER_METHOD_POST, _url, postdata, addheader, timeout, localIP, true, (void*)&tobuffer);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::Post(DIOURL& url, XPATH& pathfile, XBUFFER* postdata, XCHAR* addheader, int timeout, XSTRING* localIP)
* @brief      Post
* @ingroup    DATAIO
* 
* @param[in]  url : URL to use.
* @param[in]  pathfile : Pathfile value.
* @param[in]  postdata : Postdata pointer to use.
* @param[in]  addheader : Addheader pointer to use.
* @param[in]  timeout : Timeout value.
* @param[in]  localIP : Local IP pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::Post(DIOURL& url, XPATH& pathfile, XBUFFER* postdata, XCHAR* addheader, int timeout, XSTRING* localIP)
{
  XFILE* file=GEN_XFACTORY.Create_File();
  if(!file) return false;

  XBUFFER buffer;
  bool    status  = false;

  if(file->Create(pathfile))
    {
      status = Post(url, buffer, postdata, addheader, timeout, localIP);

      if(status) file->Write(buffer.Get(), buffer.GetSize());

      file->Close();

      if(!status) file->Erase(pathfile);
    }

  GEN_XFACTORY.Delete_File(file);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::Post(XCHAR* url, XPATH& pathfile, XBUFFER* postdata, XCHAR* addheader, int timeout, XSTRING* localIP)
* @brief      Post
* @ingroup    DATAIO
* 
* @param[in]  url : URL to use.
* @param[in]  pathfile : Pathfile value.
* @param[in]  postdata : Postdata pointer to use.
* @param[in]  addheader : Addheader pointer to use.
* @param[in]  timeout : Timeout value.
* @param[in]  localIP : Local IP pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::Post(XCHAR* url, XPATH& pathfile, XBUFFER* postdata, XCHAR* addheader, int timeout, XSTRING* localIP)
{
  DIOURL _url;
  _url = url;

  return Post(_url, pathfile, postdata, addheader, timeout, localIP);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::Header_Read(int timeout)
* @brief      Read the final HTTP response header
* @ingroup    DATAIO
* 
* @param[in]  timeout : Timeout value.
* 
* @return     bool : true if a final response header was read; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::Header_Read(int timeout)
{
  for(int c=0; c<8; c++)
    {
      if(!header.Read(diostream, timeout)) return false;

      int result = header.GetResultServer();
      if(!result) return false;

      if((result < 100) || (result >= 200)) return true;
      if(result == DIOWEBHEADER_RESULT_SWITCHINGPROTOCOLS) return false;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::BodyBlock_Write(XBYTE* data, XDWORD size, bool istobuffer, void* to, XQWORD& totalsizeread, XQWORD contentlength, XTIMER* timerdownload, DIOWEBCLIENT_XEVENT& xevent)
* @brief      Write one decoded HTTP body block
* @ingroup    DATAIO
* 
* @param[in]  data : Body data.
* @param[in]  size : Body data size.
* @param[in]  istobuffer : true to write to an XBUFFER; false to write to an XFILE.
* @param[in]  to : Destination object.
* @param[in,out] totalsizeread : Total decoded body size.
* @param[in]  contentlength : Expected content length, or zero when it is not known.
* @param[in]  timerdownload : Download timer.
* @param[in]  xevent : Web client event.
* 
* @return     bool : true if the block was written; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::BodyBlock_Write(XBYTE* data, XDWORD size, bool istobuffer, void* to, XQWORD& totalsizeread, XQWORD contentlength, XTIMER* timerdownload, DIOWEBCLIENT_XEVENT& xevent)
{
  if(!data || !size || !to || !timerdownload) return false;

  XQWORD maximum = (XQWORD)-1;
  if(totalsizeread > (maximum - size)) return false;

  bool status;

  if(istobuffer)
    {
      status = ((XBUFFER*)to)->Add(data, size);
    }
   else
    {
      status = ((XFILE*)to)->Write(data, size);
    }

  if(!status) return false;

  totalsizeread += size;

  xevent.SetEventType(DIOWEBCLIENT_XEVENT_TYPE_READBODYBLOCK);
  xevent.SetDownloadSize(totalsizeread);
  xevent.SetContentLenght(contentlength);

  if(contentlength) xevent.SetOperationPercent((float)((totalsizeread * 100.0f) / contentlength));

  if(timerdownload->GetMeasureSeconds()) xevent.SetMBPSSpeed((XDWORD)(((totalsizeread * 8) / timerdownload->GetMeasureSeconds()) / 1000));

  PostEvent(&xevent);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOWEBCLIENT_CHUNKEDRESULT DIOWEBCLIENT::ChunkSize_Get(XBUFFER& input, XQWORD& chunksize)
* @brief      Extract and validate one HTTP chunk-size line
* @ingroup    DATAIO
* 
* @param[in,out] input : Pending transfer-coded bytes.
* @param[out] chunksize : Parsed chunk size.
* 
* @return     DIOWEBCLIENT_CHUNKEDRESULT : Complete, incomplete or error.
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOWEBCLIENT_CHUNKEDRESULT DIOWEBCLIENT::ChunkSize_Get(XBUFFER& input, XQWORD& chunksize)
{
  int lineend = XBUFFER_INVALIDPOSITION;

  chunksize = 0;

  for(XDWORD c=0; (c+1)<input.GetSize(); c++)
    {
      if((input.GetByte(c) == 0x0D) && (input.GetByte(c+1) == 0x0A))
        {
          lineend = c;
          break;
        }
    }

  if(lineend == XBUFFER_INVALIDPOSITION)
    {
      if(input.GetSize() > DIOWEBHEADER_MAXLINE) return DIOWEBCLIENT_CHUNKEDRESULT_ERROR;

      return DIOWEBCLIENT_CHUNKEDRESULT_INCOMPLETE;
    }

  bool   hasdigit    = false;
  bool   afterdigits = false;
  XQWORD maximum     = (XQWORD)-1;

  for(int c=0; c<lineend; c++)
    {
      XBYTE character = input.GetByte(c);

      if(character == __C(';')) break;

      if((character == __C(' ')) || (character == __C('\t')))
        {
          if(hasdigit) afterdigits = true;
          continue;
        }

      if(afterdigits) return DIOWEBCLIENT_CHUNKEDRESULT_ERROR;

      XBYTE digit;

      if((character >= __C('0')) && (character <= __C('9'))) digit = character - __C('0');
       else if((character >= __C('a')) && (character <= __C('f'))) digit = character - __C('a') + 10;
       else if((character >= __C('A')) && (character <= __C('F'))) digit = character - __C('A') + 10;
       else return DIOWEBCLIENT_CHUNKEDRESULT_ERROR;

      if(chunksize > ((maximum - digit) >> 4)) return DIOWEBCLIENT_CHUNKEDRESULT_ERROR;

      chunksize = (chunksize << 4) + digit;
      hasdigit  = true;
    }

  if(!hasdigit) return DIOWEBCLIENT_CHUNKEDRESULT_ERROR;

  input.Extract(NULL, 0, lineend+2);

  return DIOWEBCLIENT_CHUNKEDRESULT_COMPLETE;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::Body_Read(DIOWEBCLIENT_BODYMODE bodymode, bool isTLS, XQWORD contentlength, int timeout, bool istobuffer, void* to, DIOWEBCLIENT_XEVENT& xevent)
* @brief      Read one framed HTTP response body
* @ingroup    DATAIO
* 
* @param[in]  bodymode : Response body framing mode.
* @param[in]  isTLS : true when the transport is TLS.
* @param[in]  contentlength : Expected body size for Content-Length framing.
* @param[in]  timeout : Timeout value.
* @param[in]  istobuffer : true to write to an XBUFFER; false to write to an XFILE.
* @param[in]  to : Destination object.
* @param[in]  xevent : Web client event.
* 
* @return     bool : true if the complete body was read; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::Body_Read(DIOWEBCLIENT_BODYMODE bodymode, bool isTLS, XQWORD contentlength, int timeout, bool istobuffer, void* to, DIOWEBCLIENT_XEVENT& xevent)
{
  if(!to || !maxsizebuffer || (bodymode == DIOWEBCLIENT_BODYMODE_UNKNOWN)) return false;

  if(istobuffer) ((XBUFFER*)to)->Delete();

  if(bodymode == DIOWEBCLIENT_BODYMODE_NONE) return true;

  XTIMER* timerdownload = GEN_XFACTORY.CreateTimer();
  if(!timerdownload) return false;

  XBYTE* buffer = GEN_NEW XBYTE[maxsizebuffer];
  if(!buffer)
    {
      GEN_XFACTORY.DeleteTimer(timerdownload);
      return false;
    }

  XBUFFER                   chunkinput;
  DIOWEBCLIENT_CHUNKEDSTATE chunkstate       = DIOWEBCLIENT_CHUNKEDSTATE_SIZE;
  XQWORD                    chunkremaining   = 0;
  XQWORD                    contentremaining = contentlength;
  XQWORD                    totalsizeread    = 0;
  bool                      completed        = false;
  bool                      status           = true;

  if((bodymode == DIOWEBCLIENT_BODYMODE_CONTENTLENGTH) && !contentremaining) completed = true;

  timerout->Reset();

  while(status && !completed)
    {
      XDWORD sizetoread = maxsizebuffer;

      if((bodymode == DIOWEBCLIENT_BODYMODE_CONTENTLENGTH) && (contentremaining < sizetoread))
        {
          sizetoread = (XDWORD)contentremaining;
        }

      if(!sizetoread)
        {
          completed = true;
          break;
        }

      memset(buffer, 0, sizetoread);

      XDWORD sizeread = diostream->Read(buffer, sizetoread);
      if(sizeread)
        {
          timerout->Reset();

          switch(bodymode)
            {
              case DIOWEBCLIENT_BODYMODE_CONTENTLENGTH     : if(!BodyBlock_Write(buffer, sizeread, istobuffer, to, totalsizeread, contentlength, timerdownload, xevent))
                                                               {
                                                                 status = false;
                                                                 break;
                                                               }

                                                             if(sizeread > contentremaining)
                                                               {
                                                                 status = false;
                                                                 break;
                                                               }

                                                             contentremaining -= sizeread;
                                                             if(!contentremaining) completed = true;
                                                             break;

              case DIOWEBCLIENT_BODYMODE_CHUNKED          : if(!chunkinput.Add(buffer, sizeread))
                                                               {
                                                                 status = false;
                                                                 break;
                                                               }

                                                             while(status && !completed)
                                                               {
                                                                 XDWORD                    inputsizebefore      = chunkinput.GetSize();
                                                                 XQWORD                    chunkremainingbefore = chunkremaining;
                                                                 DIOWEBCLIENT_CHUNKEDSTATE chunkstatebefore     = chunkstate;

                                                                 switch(chunkstate)
                                                                   {
                                                                     case DIOWEBCLIENT_CHUNKEDSTATE_SIZE     : { DIOWEBCLIENT_CHUNKEDRESULT result = ChunkSize_Get(chunkinput, chunkremaining);

                                                                                                                   if(result == DIOWEBCLIENT_CHUNKEDRESULT_ERROR)
                                                                                                                     {
                                                                                                                       status = false;
                                                                                                                       break;
                                                                                                                     }

                                                                                                                   if(result == DIOWEBCLIENT_CHUNKEDRESULT_INCOMPLETE) break;

                                                                                                                   chunkstate = chunkremaining?DIOWEBCLIENT_CHUNKEDSTATE_DATA:DIOWEBCLIENT_CHUNKEDSTATE_TRAILERS;
                                                                                                                 }
                                                                                                                 break;

                                                                     case DIOWEBCLIENT_CHUNKEDSTATE_DATA     : { if(chunkinput.IsEmpty()) break;

                                                                                                                   XDWORD blocksize = chunkinput.GetSize();
                                                                                                                   if(chunkremaining < blocksize) blocksize = (XDWORD)chunkremaining;

                                                                                                                   if(!BodyBlock_Write(chunkinput.Get(), blocksize, istobuffer, to, totalsizeread, 0, timerdownload, xevent))
                                                                                                                     {
                                                                                                                       status = false;
                                                                                                                       break;
                                                                                                                     }

                                                                                                                   chunkinput.Extract(NULL, 0, blocksize);
                                                                                                                   chunkremaining -= blocksize;

                                                                                                                   if(!chunkremaining) chunkstate = DIOWEBCLIENT_CHUNKEDSTATE_DATAEND;
                                                                                                                 }
                                                                                                                 break;

                                                                     case DIOWEBCLIENT_CHUNKEDSTATE_DATAEND  : if(chunkinput.GetSize() < 2) break;

                                                                                                                 if((chunkinput.GetByte(0) != 0x0D) || (chunkinput.GetByte(1) != 0x0A))
                                                                                                                   {
                                                                                                                     status = false;
                                                                                                                     break;
                                                                                                                   }

                                                                                                                 chunkinput.Extract(NULL, 0, 2);
                                                                                                                 chunkstate = DIOWEBCLIENT_CHUNKEDSTATE_SIZE;
                                                                                                                 break;

                                                                     case DIOWEBCLIENT_CHUNKEDSTATE_TRAILERS : { int lineend = XBUFFER_INVALIDPOSITION;

                                                                                                                   for(XDWORD c=0; (c+1)<chunkinput.GetSize(); c++)
                                                                                                                     {
                                                                                                                       if((chunkinput.GetByte(c) == 0x0D) && (chunkinput.GetByte(c+1) == 0x0A))
                                                                                                                         {
                                                                                                                           lineend = c;
                                                                                                                           break;
                                                                                                                         }
                                                                                                                     }

                                                                                                                   if(lineend == XBUFFER_INVALIDPOSITION)
                                                                                                                     {
                                                                                                                       if(chunkinput.GetSize() > DIOWEBHEADER_MAXLINE) status = false;
                                                                                                                       break;
                                                                                                                     }

                                                                                                                   if(!lineend)
                                                                                                                     {
                                                                                                                       chunkinput.Extract(NULL, 0, 2);

                                                                                                                       if(!chunkinput.IsEmpty())
                                                                                                                         {
                                                                                                                           status = false;
                                                                                                                           break;
                                                                                                                         }

                                                                                                                       chunkstate = DIOWEBCLIENT_CHUNKEDSTATE_END;
                                                                                                                       completed  = true;
                                                                                                                       break;
                                                                                                                     }

                                                                                                                   bool hasseparator = false;

                                                                                                                   for(int c=1; c<lineend; c++)
                                                                                                                     {
                                                                                                                       if(chunkinput.GetByte(c) == __C(':'))
                                                                                                                         {
                                                                                                                           hasseparator = true;
                                                                                                                           break;
                                                                                                                         }
                                                                                                                     }

                                                                                                                   if(!hasseparator)
                                                                                                                     {
                                                                                                                       status = false;
                                                                                                                       break;
                                                                                                                     }

                                                                                                                   chunkinput.Extract(NULL, 0, lineend+2);
                                                                                                                 }
                                                                                                                 break;

                                                                     case DIOWEBCLIENT_CHUNKEDSTATE_END      : completed = true;
                                                                                                                 break;
                                                                   }

                                                                 if(!status || completed) break;

                                                                 if((chunkinput.GetSize() == inputsizebefore) &&
                                                                    (chunkremaining == chunkremainingbefore) &&
                                                                    (chunkstate == chunkstatebefore)) break;

                                                                 if((chunkstate == DIOWEBCLIENT_CHUNKEDSTATE_SIZE) && chunkinput.IsEmpty()) break;
                                                                 if((chunkstate == DIOWEBCLIENT_CHUNKEDSTATE_DATA) && chunkinput.IsEmpty()) break;
                                                                 if((chunkstate == DIOWEBCLIENT_CHUNKEDSTATE_DATAEND) && (chunkinput.GetSize() < 2)) break;
                                                                 if((chunkstate == DIOWEBCLIENT_CHUNKEDSTATE_TRAILERS) && (chunkinput.GetSize() < 2)) break;
                                                               }
                                                             break;

              case DIOWEBCLIENT_BODYMODE_CONNECTIONCLOSE : if(!BodyBlock_Write(buffer, sizeread, istobuffer, to, totalsizeread, 0, timerdownload, xevent)) status = false;
                                                             break;

                                                  default : status = false;
                                                             break;
            }
        }
       else
        {
          if(timeout && (timerout->GetMeasureSeconds() > (XDWORD)timeout))
            {
              status = false;
              break;
            }

          if(diostream->GetStatus() != DIOSTREAMSTATUS_CONNECTED)
            {
              if((bodymode == DIOWEBCLIENT_BODYMODE_CONNECTIONCLOSE) && !isTLS) completed = true;
               else status = false;

              break;
            }

          GEN_XSLEEP.MilliSeconds(1);
        }
    }

  GEN_DELETE_ARRAY buffer;
  GEN_XFACTORY.DeleteTimer(timerdownload);

  return status && completed;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::MakeOperation(DIOWEBHEADER_METHOD method, DIOURL& url, XBUFFER* postdata, XCHAR* addhead, int timeout, XSTRING* localIP, bool istobuffer, void* to)
* @brief      Make operation
* @ingroup    DATAIO
* 
* @param[in]  method : Method descriptor to call.
* @param[in]  url : URL to use.
* @param[in]  postdata : Postdata pointer to use.
* @param[in]  addhead : Addhead pointer to use.
* @param[in]  timeout : Timeout value.
* @param[in]  localIP : Local IP pointer to use.
* @param[in]  istobuffer : Istobuffer value.
* @param[in]  to : To pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::MakeOperation(DIOWEBHEADER_METHOD method, DIOURL& url, XBUFFER* postdata, XCHAR* addhead, int timeout, XSTRING* localIP, bool istobuffer, void* to)
{
  if(!diostreamcfg)     return false;
  if(!timerout)         return false;
  if(!to)               return false;

  if(istobuffer) ((XBUFFER*)to)->Delete();

  bool  status          = true;
  bool  isTLS           = IsSecureURL(url);
  int   defaultport     = isTLS?DIOWEBCLIENT_DEFAULTSECUREPORT:DIOWEBCLIENT_DEFAULTPORT;
  int   operationport;

  if(!url.HaveHTTPID()) url.AddHTTPID();

  XSTRING               server; 
  DIOURL                resource;
  XSTRING               methodstring;
  DIOWEBCLIENT_XEVENT   xevent(this, XEVENT_TYPE_NONE);

  xevent.GetURL()->Set(url.Get());
  xevent.SetMethod(method);

  diostreamcfg->SetRemotePort(0);
  if(!diostreamcfg->SetFromString(url.Get())) return false;

  url  = diostreamcfg->GetRemoteURL()->Get();
  if(diostreamcfg->GetRemotePort()) operationport = diostreamcfg->GetRemotePort();
                              else operationport = isportconfigured?port:defaultport;

  if(!isportconfigured) port = operationport;

  url.GetHTTPServer(server, login, password);
  url.GetHTTPResource(resource);

  if(resource.IsEmpty()) resource.Slash_Add();

  diostreamcfg->SetMode(DIOSTREAMMODE_CLIENT);
  if(localIP)
    {
      if(!localIP->IsEmpty()) diostreamcfg->GetLocalIP()->Set(localIP->Get());
    }

  if(proxyurl.IsEmpty())
    {
      diostreamcfg->GetRemoteURL()->Set(server.Get());
      diostreamcfg->SetRemotePort(operationport);
    }
   else
    {
      if(isTLS) return false;

      diostreamcfg->GetRemoteURL()->Set(proxyurl.Get());
      diostreamcfg->SetRemotePort(proxyport);
    }

  #ifdef DIO_STREAMTLS_ACTIVE

  if(isTLS)
    {
      DIOSTREAMTLSCONFIG* tlsconfig = GetStreamTLSCFG();

      if(!tlsconfig) return false;
      tlsconfig->GetServerName()->Set(server.Get());
    }

  #endif

  if(!Stream_Create(isTLS)) return false;

  //--- Connection WEB server -------------------

  if(!diostream->Open())
    {
      diostream->Close();
      return false;
    }

  if(!diostream->WaitToConnected(timeout))
    {
      diostream->Close();
      return false;
    }

  xevent.SetEventType(DIOWEBCLIENT_XEVENT_TYPE_OPENWEB);
  PostEvent(&xevent);

  //--- Send Header ----------------------------

  XSTRING sendheader;

  //resource.EncodeUnsafeChars();

  switch(method)
    {
      case DIOWEBHEADER_METHOD_GET  : methodstring = __L("GET");     break;
      case DIOWEBHEADER_METHOD_PUT  : methodstring = __L("PUT");     break;
      case DIOWEBHEADER_METHOD_POST : methodstring = __L("POST");    break;

                            default : { xevent.SetEventType(DIOWEBCLIENT_XEVENT_TYPE_HEADERERROR);
                                        PostEvent(&xevent);

                                        diostream->Close();
                                      }
                                      return false;
    }


  sendheader.AddFormat(__L("%s %s HTTP/1.1\r\n"), methodstring.Get(), resource.Get());

  sendheader += __L("Host: ");
  sendheader += server.Get();

  if(operationport!=defaultport)
    {
      XSTRING stringport;

      stringport.ConvertFromInt(operationport);

      sendheader += __L(":");
      sendheader += stringport.Get();
    }
  sendheader += __L("\r\n");

  if(addhead) sendheader += addhead;


  switch(authenticationmethod)
    {
      case DIOWEBCLIENT_AUTHENTICATION_METHOD_UNKNOWN : if(password.IsEmpty()) break;
                                                        authenticationmethod = DIOWEBCLIENT_AUTHENTICATION_METHOD_BASIC;
      
      case DIOWEBCLIENT_AUTHENTICATION_METHOD_BASIC   : { XSTRING loginpassword;
                                                          XSTRING loginpasswordbase64;

                                                          loginpassword.Format(__L("%s:%s"), login.Get(), password.Get());
                                                          loginpassword.ConvertToBase64(loginpasswordbase64);   

                                                          sendheader += __L("Authorization: Basic ");
                                                          sendheader += loginpasswordbase64;
                                                          sendheader += __L("\r\n");
                                                        }
                                                        break;

      case DIOWEBCLIENT_AUTHENTICATION_METHOD_DIGEST  : break;
    }
  

  if(postdata)
    {
      XSTRING stringlenght;
      stringlenght.Format(__L("%s: %d\r\n"), DIOWEBHEADER_CONTENT_LENGTH, postdata->GetSize());

      sendheader += stringlenght;
    }

  sendheader += __L("Connection: close\r\n");
  sendheader += __L("\r\n");

  diostream->WriteStr(sendheader);

  //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("%s"), sendheader.Get());

  xevent.SetEventType(DIOWEBCLIENT_XEVENT_TYPE_WRITEHEADER);
  PostEvent(&xevent);

  //--- Send Post data -------------------------

  if(postdata)
    {
      diostream->Write(postdata->Get(), postdata->GetSize());

      xevent.SetEventType(DIOWEBCLIENT_XEVENT_TYPE_SENDPOSTDATA);
      PostEvent(&xevent);
    }

  diostream->WaitToFlushOutXBuffer(timeout);

  //--- Read Header ----------------------------

  if(!Header_Read(timeout))
    {
      diostream->Close();
      return false;
    }

  //--- Read Content ---------------------------

  if(header.GetResultServer() == 301)
    {

    
    }

  if((header.GetResultServer() == 401) && (authenticationmethod == DIOWEBCLIENT_AUTHENTICATION_METHOD_DIGEST))
    {
      XSTRING   www_authenticate;
      HASHMD5   md5;
      XSTRING   HA1;
      XSTRING   HA2;
      XSTRING   calculatehash;
      XSTRING   realm_str;
      XSTRING   nonce_str;
      XSTRING   auth_str;
      XDWORD    nc;
      XDWORD    cnonce;
      XSTRING   nc_str;
      XSTRING   cnonce_str;
      XSTRING   opaque_str;
      XSTRING   algorithm_str;
      XSTRING   response;
      
      sendheader.DeleteCharacters(sendheader.GetSize()-2, 2);

      diostream->Close();

      if(!diostream->Open())
        {
          diostream->Close();
          return false;
        }

      if(!diostream->WaitToConnected(timeout))
        {
          diostream->Close();
          return false;
        }

      xevent.SetEventType(DIOWEBCLIENT_XEVENT_TYPE_OPENWEB);
      PostEvent(&xevent);


      auth_str    = __L("auth");
      nc          = 0x0000001;
      cnonce      = 0x0AF113B;

      nc_str.Format(__L("%08x"), nc);
      cnonce_str.Format(__L("%08x"), cnonce);
      
       
      header.GetWWWAuthenticate(www_authenticate);
      
      //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("%s"), www_authenticate.Get());

      GetSubStringWWWWAuthenticate(www_authenticate, __L("realm")     , realm_str);
      GetSubStringWWWWAuthenticate(www_authenticate, __L("nonce")     , nonce_str);
      GetSubStringWWWWAuthenticate(www_authenticate, __L("opaque")    , opaque_str);
      GetSubStringWWWWAuthenticate(www_authenticate, __L("algorithm") , algorithm_str, false);

      /*
      if(algorithm_str.Compare(__L("MD5"), true))
        {
          diostream->Close();
          return false;
        }    
      */
     
      { calculatehash.Format(__L("%s:%s:%s"), login.Get(), realm_str.Get(), password.Get());
       
        XBUFFER charhash;
        
        calculatehash.ConvertToASCII(charhash);       
        md5.Do((XBYTE*)charhash.Get(), calculatehash.GetSize());
        
        md5.GetResultString(HA1);
      }

      md5.ResetResult();

      { calculatehash.Format(__L("%s:%s"), methodstring.Get(), resource.Get());
  
        XBUFFER charhash;
        
        calculatehash.ConvertToASCII(charhash);       
        md5.Do((XBYTE*)charhash.Get(), calculatehash.GetSize());
        
        md5.GetResultString(HA2);
      }

      md5.ResetResult();  

      { calculatehash.Format(__L("%s:%s:%s:%s:%s:%s"), HA1.Get(), nonce_str.Get(), nc_str.Get(), cnonce_str.Get(), auth_str.Get(), HA2.Get());
  
        XBUFFER charhash;
        
        calculatehash.ConvertToASCII(charhash);       
        md5.Do((XBYTE*)charhash.Get(), calculatehash.GetSize());
        
        md5.GetResultString(response);
      }
     
   
      sendheader.AddFormat(__L("Authorization: Digest "));     
      sendheader.AddFormat(__L("username=\"%s\", "), login.Get());           
      sendheader.AddFormat(__L("realm=\"%s\", "), realm_str.Get());           
      sendheader.AddFormat(__L("nonce=\"%s\", "), nonce_str.Get());           
      sendheader.AddFormat(__L("uri=\"%s\", "), resource.Get());     
      sendheader.AddFormat(__L("algorithm=%s, "), algorithm_str.Get());
      sendheader.AddFormat(__L("response=\"%s\", "), response.Get());   
      sendheader.AddFormat(__L("qop=%s, "), auth_str.Get());     
      sendheader.AddFormat(__L("nc=%s, "), nc_str.Get());     
      sendheader.AddFormat(__L("cnonce=\"%s\" "), cnonce_str.Get());   
      if(opaque_str.GetSize()) sendheader.AddFormat(__L(", opaque=\"%s\""), opaque_str.Get());           
      sendheader += __L("\r\n");

      sendheader += __L("\r\n");
      
      //XTRACE_PRINTCOLOR(XTRACE_COLOR_PURPLE, __L("%s"), sendheader.Get());

      diostream->WriteStr(sendheader);

      xevent.SetEventType(DIOWEBCLIENT_XEVENT_TYPE_WRITEHEADER);
      PostEvent(&xevent);

      //--- Send Post data -------------------------

      if(postdata)
        {
          diostream->Write(postdata->Get(), postdata->GetSize());

          xevent.SetEventType(DIOWEBCLIENT_XEVENT_TYPE_SENDPOSTDATA);
          PostEvent(&xevent);
        }

      diostream->WaitToFlushOutXBuffer(timeout);

      //--- Read Header ----------------------------

      if(!Header_Read(timeout))
        {
          diostream->Close();
          return false;
        }
    }


  if((header.GetResultServer()>=400)||(!header.GetResultServer()))
    {     
      xevent.SetEventType(DIOWEBCLIENT_XEVENT_TYPE_HEADERERROR);
      PostEvent(&xevent);

      if(IsActiveDoStopHTTPError())
        {
          diostream->Close();
          return false;
        }
    }

  XQWORD                    contentlength        = 0;
  XSTRING                   transferencoding;
  XSTRING                   etag;
  DIOWEBCLIENT_BODYMODE     bodymode             = DIOWEBCLIENT_BODYMODE_UNKNOWN;
  int                       resultserver         = header.GetResultServer();
  bool                      hascontentlength     = header.HasContentLength();
  bool                      validcontentlength   = header.GetContentLength(contentlength);
  bool                      hastransferencoding  = header.GetTransferEncoding(transferencoding);
  bool                      hasnobody            = ((method == DIOWEBHEADER_METHOD_HEAD) ||
                                                    ((resultserver >= 100) && (resultserver < 200)) ||
                                                    (resultserver == DIOWEBHEADER_RESULT_NOCONTENT) ||
                                                    (resultserver == DIOWEBHEADER_RESULT_RESETCONTENT) ||
                                                    (resultserver == DIOWEBHEADER_RESULT_NOTMODIFIED));

  if(hasnobody)
    {
      bodymode     = DIOWEBCLIENT_BODYMODE_NONE;
      contentlength = 0;
    }
   else if(hastransferencoding)
    {
      transferencoding.DeleteCharacter(__C(' '));
      transferencoding.DeleteCharacter(__C('\t'));

      if(hascontentlength || transferencoding.Compare(__L("chunked"), true))
        {
          xevent.SetEventType(DIOWEBCLIENT_XEVENT_TYPE_HEADERERROR);
          PostEvent(&xevent);

          diostream->Close();
          return false;
        }

      bodymode      = DIOWEBCLIENT_BODYMODE_CHUNKED;
      contentlength = 0;
    }
   else if(hascontentlength)
    {
      if(!validcontentlength)
        {
          xevent.SetEventType(DIOWEBCLIENT_XEVENT_TYPE_HEADERERROR);
          PostEvent(&xevent);

          diostream->Close();
          return false;
        }

      bodymode = DIOWEBCLIENT_BODYMODE_CONTENTLENGTH;
    }
   else
    {
      bodymode = DIOWEBCLIENT_BODYMODE_CONNECTIONCLOSE;

      if(isTLS)
        {
          xevent.SetEventType(DIOWEBCLIENT_XEVENT_TYPE_HEADERERROR);
          PostEvent(&xevent);

          diostream->Close();
          return false;
        }
    }

  header.GetETag(etag);

  xevent.SetContentLenght(contentlength);
  xevent.GetEtag()->Set(etag);

  xevent.SetEventType(DIOWEBCLIENT_XEVENT_TYPE_READHEADER);

  PostEvent(&xevent);

  //--- Read Content ---------------------------

  status = Body_Read(bodymode, isTLS, contentlength, timeout, istobuffer, to, xevent);

  //--- Close ----------------------------------

  diostream->Close();

  xevent.SetEventType(DIOWEBCLIENT_XEVENT_TYPE_CLOSEWEB);
  PostEvent(&xevent);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::Stream_Create(bool isTLS)
* @brief      Create the transport required by the URL scheme
* @ingroup    DATAIO
* 
* @param[in]  isTLS : true to create a TLS transport; false to create a TCP/IP transport.
* 
* @return     bool : true if the required transport is available; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::Stream_Create(bool isTLS)
{
  if(!diostreamcfg) return false;

  #ifndef DIO_STREAMTLS_ACTIVE

  if(isTLS) return false;

  #endif

  if(diostream && (isstreamTLS != isTLS))
    {
      diostream->Close();
      GEN_DIOFACTORY.DeleteStreamIO(diostream);
      diostream = NULL;
    }

  diostreamcfg->SetIsTLS(isTLS);

  #ifdef DIO_STREAMTLS_ACTIVE

  if(isTLS)
    {
      DIOSTREAMTLSCONFIG* tlsconfig = GetStreamTLSCFG();

      if(!tlsconfig) return false;
      if(tlsconfig->GetTrustedRoots()->IsEmpty())
        {
          if(!tlsconfig->TrustedRoots_AddDefaults()) return false;
        }
    }

  #endif

  if(!diostream)
    {
      diostream = (DIOSTREAMTCPIP*)GEN_DIOFACTORY.CreateStreamIO(diostreamcfg);
      if(!diostream) return false;
    }

  isstreamTLS = isTLS;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::IsSecureURL(DIOURL& url)
* @brief      Check whether the URL selects HTTPS
* @ingroup    DATAIO
* 
* @param[in]  url : URL to check.
* 
* @return     bool : true if the URL starts with the HTTPS identifier; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::IsSecureURL(DIOURL& url)
{
  return (url.Find(DIOURL_WEBURLID_SECURE, true) == 0)?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBCLIENT::GetSubStringWWWWAuthenticate(XSTRING& www_authenticate, XCHAR* field, XSTRING& value, bool betweenquotation)
* @brief      Get sub string WWWW authenticate
* @ingroup    DATAIO
* 
* @param[in]  www_authenticate : Www authenticate value.
* @param[in]  field : Field pointer to use.
* @param[in]  value : Value value.
* @param[in]  betweenquotation : Betweenquotation value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBCLIENT::GetSubStringWWWWAuthenticate(XSTRING& www_authenticate, XCHAR* field, XSTRING& value, bool betweenquotation)
{
  if(www_authenticate.IsEmpty()) return false;

  XSTRING sfield;

  sfield = field;

  int index = www_authenticate.Find(field, true);
  if(index == XSTRING_NOTFOUND) return false;
  index +=  sfield.GetSize();

  index = www_authenticate.Find(__L("="), true, index);
  if(index == XSTRING_NOTFOUND) return false;

  int start = 0;
  int end   = 0;

  if(betweenquotation)
    {
      start = www_authenticate.Find(__L("\""), true, index+1);
      if(start == XSTRING_NOTFOUND) return false;

      end = www_authenticate.Find(__L("\""), true, start+1);
      if(end == XSTRING_NOTFOUND) return false;
    }
   else
    { 
      start = index;

      end = www_authenticate.Find(__L(","), true, start+1);
      if(end == XSTRING_NOTFOUND) return false;
    }

  return www_authenticate.Copy(start+1, end, value)?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOWEBCLIENT::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBCLIENT::Clean()
{
  maxsizebuffer           = 0;

  timerout                = NULL;

  diostreamcfg            = NULL;
  diostream               = NULL;

  proxyport               = 0;

  port                    = DIOWEBCLIENT_DEFAULTPORT;
  isportconfigured        = false;
  isstreamTLS             = false;

  authenticationmethod    = DIOWEBCLIENT_AUTHENTICATION_METHOD_UNKNOWN;

  dostophttperror         = true;
}
