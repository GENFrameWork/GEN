/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTCPIPProxy.h
* 
* @class      DIOSTREAMTCPIPPROXYCFG / DIOSTREAMTCPIPPROXY
* @brief      Data Input/Output proxy configuration and tunnel negotiation classes
* @ingroup    DATAIO
* 
* @copyright  EndoraSoft. All rights reserved.
* 
* --------------------------------------------------------------------------------------------------------------------*/

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XString.h"
#include "DIOURL.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

enum DIOSTREAMTCPIPPROXYTYPE
{
  DIOSTREAMTCPIPPROXYTYPE_NONE                  = 0 ,
  DIOSTREAMTCPIPPROXYTYPE_HTTP                      ,
};


enum DIOSTREAMTCPIPPROXYMODE
{
  DIOSTREAMTCPIPPROXYMODE_NONE                  = 0 ,
  DIOSTREAMTCPIPPROXYMODE_FORWARD                   ,
  DIOSTREAMTCPIPPROXYMODE_TUNNEL                    ,
};


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class DIOSTREAM;


class DIOSTREAMTCPIPPROXYCFG
{
  public:
                              DIOSTREAMTCPIPPROXYCFG          ();
    virtual                  ~DIOSTREAMTCPIPPROXYCFG          ();

    DIOSTREAMTCPIPPROXYTYPE  GetType                         ();
    void                      SetType                         (DIOSTREAMTCPIPPROXYTYPE type);

    DIOSTREAMTCPIPPROXYMODE  GetMode                         ();
    void                      SetMode                         (DIOSTREAMTCPIPPROXYMODE mode);

    bool                      IsActive                        ();

    DIOURL*                   GetURL                          ();

    int                       GetPort                         ();
    void                      SetPort                         (int port);

    XSTRING*                  GetLogin                        ();
    XSTRING*                  GetPassword                     ();

  private:

    void                      Clean                           ();

    DIOSTREAMTCPIPPROXYTYPE  type;
    DIOSTREAMTCPIPPROXYMODE  mode;
    DIOURL                    url;
    int                       port;
    XSTRING                   login;
    XSTRING                   password;
};


class DIOSTREAMTCPIPPROXY
{
  public:
                              DIOSTREAMTCPIPPROXY             ();
    virtual                  ~DIOSTREAMTCPIPPROXY             ();

    bool                      Connect                         (DIOSTREAM* stream, DIOSTREAMTCPIPPROXYCFG* cfg, XCHAR* target, int targetport, int timeout);

  private:

    bool                      ConnectHTTP                     (DIOSTREAM* stream, DIOSTREAMTCPIPPROXYCFG* cfg, XCHAR* target, int targetport, int timeout);
};

