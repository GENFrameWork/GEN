/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOWebServer_Plugin_CGI.cpp
* 
* @class      DIOWEBSERVER_PLUGIN_CGI
* @brief      Data Input/Output Web Server CGI (Common Gateway Interface) Plugin class
* @ingroup    DATAIO
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOWebServer_Plugin_CGI.h"

#include "XFileTXT.h"
#include "XSystem.h"
#include "XProcessManager.h"
#include "XTrace.h"

#include "DIOWebServer_QueryStrings.h"
#include "DIOWebServer.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOWEBSERVER_PLUGIN_CGI::DIOWEBSERVER_PLUGIN_CGI()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOWEBSERVER_PLUGIN_CGI::DIOWEBSERVER_PLUGIN_CGI()
{
  Clean();

  GetName()->Set(__L("CGI Pluging"));
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOWEBSERVER_PLUGIN_CGI::~DIOWEBSERVER_PLUGIN_CGI()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOWEBSERVER_PLUGIN_CGI::~DIOWEBSERVER_PLUGIN_CGI()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBSERVER_PLUGIN_CGI::Config(XPATH* pathCGIinterpreter, XCHAR* nameexec)
* @brief      Config
* @ingroup    DATAIO
* 
* @param[in]  pathCGIinterpreter : 
* @param[in]  nameexec : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBSERVER_PLUGIN_CGI::Config(XPATH* pathCGIinterpreter, XCHAR* nameexec)
{
  if(!pathCGIinterpreter) return false;

  this->pathCGIinterpreter  = pathCGIinterpreter->Get();
  if(nameexec) this->nameexec = nameexec;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBSERVER_PLUGIN_CGI::PageExtension(XPATH& pathfile, DIOWEBSERVER_REQUEST* request, DIOWEBSERVER_QUERYSTRINGS* querystrings, DIOWEBHEADER_RESULT& headerresult, XSTRING& result)
* @brief      Page extension
* @ingroup    DATAIO
*
* @param[in]  pathfile :
* @param[in]  request :
* @param[in]  querystrings :
* @param[in]  headerresult :
* @param[in]  result :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBSERVER_PLUGIN_CGI::PageExtension(XPATH& pathfile, DIOWEBSERVER_REQUEST* request, DIOWEBSERVER_QUERYSTRINGS* querystrings, DIOWEBHEADER_RESULT& headerresult, XSTRING& result)
{
  if(!request)      return false;
  if(!querystrings) return false;

  XSTRING allparam;

  headerresult = DIOWEBHEADER_RESULT_OK;

  querystrings->GetAllParam(allparam);

  XPATH pathexec;

  pathexec  = pathCGIinterpreter.Get();
  pathexec.Slash_Add();
  pathexec += nameexec;

  XSTRING methodstring;
  int     returnerror = 0;
  XBUFFER in;
  XBUFFER out;
  int     sizein = 0;
  XSTRING sizeinstr;
  bool    status;

  if(request->GetData())
    {
      sizein = request->GetData()->GetSize();
      if(sizein)
        {
          in.Add((*request->GetData()));

          sizeinstr.Format(__L("%d"), sizein);
        }
    }

  switch(request->GetMethod())
    {
      case DIOWEBHEADER_METHOD_GET     : methodstring = __L("\"GET\"");       break;
      case DIOWEBHEADER_METHOD_POST    : methodstring = __L("\"POST\"");      break;
      case DIOWEBHEADER_METHOD_PUT     : methodstring = __L("\"PUT\"");       break;
      case DIOWEBHEADER_METHOD_DELETE  : methodstring = __L("\"DELETE\"");    break;
                          default      : break;
    }


  GEN_XSYSTEM.SetEnviromentVariable(__L("GATEWAY_INTERFACE") , __L("CGI/1.1"));
  GEN_XSYSTEM.SetEnviromentVariable(__L("REQUEST_METHOD")    , __L("POST"));
  GEN_XSYSTEM.SetEnviromentVariable(__L("SCRIPT_FILENAME")   , pathfile.Get());
  GEN_XSYSTEM.SetEnviromentVariable(__L("CONTENT_LENGTH")    , sizeinstr.Get());
  GEN_XSYSTEM.SetEnviromentVariable(__L("REDIRECT_STATUS")   , __L("true"));
  GEN_XSYSTEM.SetEnviromentVariable(__L("CONTENT_TYPE")      , __L("application/x-www-form-urlencoded"));

  if(request->GetMethod() == DIOWEBHEADER_METHOD_GET) GEN_XSYSTEM.SetEnviromentVariable(__L("QUERY_STRING"), allparam.GetSize()?allparam.Get():__L("\"\""));

  status = GEN_XPROCESSMANAGER.Application_Execute(pathexec.Get(), NULL, &in, &out, &returnerror);

  if(!out.IsEmpty())
    {
      result.Add(out);
    }

  GEN_XSYSTEM.DelEnviromentVariable(__L("GATEWAY_INTERFACE"));
  GEN_XSYSTEM.DelEnviromentVariable(__L("REQUEST_METHOD"));
  GEN_XSYSTEM.DelEnviromentVariable(__L("SCRIPT_FILENAME"));
  GEN_XSYSTEM.DelEnviromentVariable(__L("CONTENT_LENGTH"));
  GEN_XSYSTEM.DelEnviromentVariable(__L("REDIRECT_STATUS"));
  GEN_XSYSTEM.DelEnviromentVariable(__L("CONTENT_TYPE"));
  GEN_XSYSTEM.DelEnviromentVariable(__L("QUERY_STRING"));

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOWEBSERVER_PLUGIN_CGI::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBSERVER_PLUGIN_CGI::Clean()
{
  pathCGIinterpreter.Empty();
}


#pragma endregion

