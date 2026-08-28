/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTCPIPProxy.cpp
* 
* @class      DIOSTREAMTCPIPPROXYCFG / DIOSTREAMTCPIPPROXY
* @brief      Data Input/Output proxy configuration and tunnel negotiation classes
* @ingroup    DATAIO
* 
* @copyright  EndoraSoft. All rights reserved.
* 
* --------------------------------------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DIOStreamTCPIPProxy.h"
#include "DIOStream.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/

DIOSTREAMTCPIPPROXYCFG::DIOSTREAMTCPIPPROXYCFG()
{
  Clean();
}


DIOSTREAMTCPIPPROXYCFG::~DIOSTREAMTCPIPPROXYCFG()
{
  Clean();
}


DIOSTREAMTCPIPPROXYTYPE DIOSTREAMTCPIPPROXYCFG::GetType()
{
  return type;
}


void DIOSTREAMTCPIPPROXYCFG::SetType(DIOSTREAMTCPIPPROXYTYPE type)
{
  this->type = type;
}


DIOSTREAMTCPIPPROXYMODE DIOSTREAMTCPIPPROXYCFG::GetMode()
{
  return mode;
}


void DIOSTREAMTCPIPPROXYCFG::SetMode(DIOSTREAMTCPIPPROXYMODE mode)
{
  this->mode = mode;
}


bool DIOSTREAMTCPIPPROXYCFG::IsActive()
{
  if(type == DIOSTREAMTCPIPPROXYTYPE_NONE) return false;
  if(mode == DIOSTREAMTCPIPPROXYMODE_NONE) return false;
  if(url.IsEmpty()) return false;
  if(port <= 0) return false;

  return true;
}


DIOURL* DIOSTREAMTCPIPPROXYCFG::GetURL()
{
  return &url;
}


int DIOSTREAMTCPIPPROXYCFG::GetPort()
{
  return port;
}


void DIOSTREAMTCPIPPROXYCFG::SetPort(int port)
{
  this->port = port;
}


XSTRING* DIOSTREAMTCPIPPROXYCFG::GetLogin()
{
  return &login;
}


XSTRING* DIOSTREAMTCPIPPROXYCFG::GetPassword()
{
  return &password;
}


void DIOSTREAMTCPIPPROXYCFG::Clean()
{
  type = DIOSTREAMTCPIPPROXYTYPE_NONE;
  mode = DIOSTREAMTCPIPPROXYMODE_NONE;
  port = 0;
}


DIOSTREAMTCPIPPROXY::DIOSTREAMTCPIPPROXY()
{
}


DIOSTREAMTCPIPPROXY::~DIOSTREAMTCPIPPROXY()
{
}


bool DIOSTREAMTCPIPPROXY::Connect(DIOSTREAM* stream, DIOSTREAMTCPIPPROXYCFG* cfg, XCHAR* target, int targetport, int timeout)
{
  if(!stream || !cfg || !target || !target[0] || (targetport <= 0)) return false;
  if(!cfg->IsActive()) return true;
  if(cfg->GetMode() != DIOSTREAMTCPIPPROXYMODE_TUNNEL) return true;

  switch(cfg->GetType())
    {
      case DIOSTREAMTCPIPPROXYTYPE_HTTP : return ConnectHTTP(stream, cfg, target, targetport, timeout);
                 default     : break;
    }

  return false;
}


bool DIOSTREAMTCPIPPROXY::ConnectHTTP(DIOSTREAM* stream, DIOSTREAMTCPIPPROXYCFG* cfg, XCHAR* target, int targetport, int timeout)
{
  XSTRING request;
  XSTRING line;

  request.Format(__L("CONNECT %s:%d HTTP/1.1\r\nHost: %s:%d\r\n"), target, targetport, target, targetport);

  if(!cfg->GetLogin()->IsEmpty() || !cfg->GetPassword()->IsEmpty())
    {
      XSTRING credentials;
      XSTRING credentialsbase64;

      credentials.Format(__L("%s:%s"), cfg->GetLogin()->Get(), cfg->GetPassword()->Get());
      credentials.ConvertToBase64(credentialsbase64);

      request += __L("Proxy-Authorization: Basic ");
      request += credentialsbase64;
      request += __L("\r\n");
    }

  request += __L("Proxy-Connection: Keep-Alive\r\n\r\n");

  if(!stream->WriteStr(request)) return false;
  if(!stream->WaitToFlushOutXBuffer(timeout)) return false;

  if(!stream->ReadStr(line, timeout)) return false;

  int statuscode = 0;
  XSTRING protocol;

  protocol.AdjustSize(32);
  if(!line.UnFormat(__L("%s %d"), protocol.Get(), &statuscode)) return false;
  protocol.AdjustSize();

  if((statuscode < 200) || (statuscode > 299)) return false;

  for(;;)
    {
      line.Empty();
      if(!stream->ReadStr(line, timeout)) return false;
      if(line.IsEmpty()) break;
    }

  return true;
}
