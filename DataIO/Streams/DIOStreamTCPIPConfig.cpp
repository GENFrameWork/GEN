/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTCPIPConfig.cpp
* 
* @class      DIOSTREAMTCPIPCONFIG
* @brief      Data Input/Output Stream TCP/IP Config class
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

#include "DIOStreamTCPIPConfig.h"

#include "XFactory.h"

#include "DIOFactory.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTCPIPCONFIG::DIOSTREAMTCPIPCONFIG() : DIOSTREAMCONFIG(), DIOSTREAMCONFIGSTRING()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTCPIPCONFIG::DIOSTREAMTCPIPCONFIG() : DIOSTREAMCONFIG(), DIOSTREAMCONFIGSTRING()
{
  Clean();

  type  = DIOSTREAMTYPE_TCPIP;

  xmutexmultiserver=GEN_XFACTORY.Create_Mutex();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTCPIPCONFIG::~DIOSTREAMTCPIPCONFIG()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTCPIPCONFIG::~DIOSTREAMTCPIPCONFIG()
{
  if(xmutexmultiserver) GEN_XFACTORY.Delete_Mutex(xmutexmultiserver);

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOIP* DIOSTREAMTCPIPCONFIG ::GetLocalIP()
* @brief      Get local IP
* @ingroup    DATAIO
* 
* @return     DIOIP* DIOSTREAMTCPIPCONFIG :: : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOIP* DIOSTREAMTCPIPCONFIG ::GetLocalIP()
{
  return &localIP;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOURL* DIOSTREAMTCPIPCONFIG ::GetRemoteURL()
* @brief      Get remote URL
* @ingroup    DATAIO
* 
* @return     DIOURL* DIOSTREAMTCPIPCONFIG :: : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOURL* DIOSTREAMTCPIPCONFIG ::GetRemoteURL()
{
  return &remoteURL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOSTREAMTCPIPCONFIG ::GetRemotePort()
* @brief      Get remote port
* @ingroup    DATAIO
* 
* @return     int DIOSTREAMTCPIPCONFIG :: : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMTCPIPCONFIG ::GetRemotePort()
{
  return remoteport;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOSTREAMTCPIPCONFIG ::SetRemotePort(int port)
* @brief      Set remote port
* @ingroup    DATAIO
* 
* @param[in]  port : Port number to use.
* 
* @return     bool DIOSTREAMTCPIPCONFIG :: : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTCPIPCONFIG ::SetRemotePort(int port)
{
  this->remoteport = port;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMTCPIPPROXYCFG* DIOSTREAMTCPIPCONFIG::GetProxyCFG()
* @brief      Get proxy configuration
* @ingroup    DATAIO
* 
* @return     DIOSTREAMTCPIPPROXYCFG* : Proxy configuration.
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTCPIPPROXYCFG* DIOSTREAMTCPIPCONFIG::GetProxyCFG()
{
  return &proxycfg;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOURL* DIOSTREAMTCPIPCONFIG::GetConnectionURL()
* @brief      Get the physical endpoint used to establish the TCP connection
* @ingroup    DATAIO
* 
* @return     DIOURL* : Proxy URL when active; otherwise the configured remote URL.
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOURL* DIOSTREAMTCPIPCONFIG::GetConnectionURL()
{
  if(proxycfg.IsActive()) return proxycfg.GetURL();

  return GetRemoteURL();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOSTREAMTCPIPCONFIG::GetConnectionPort()
* @brief      Get the physical endpoint port used to establish the TCP connection
* @ingroup    DATAIO
* 
* @return     int : Proxy port when active; otherwise the configured remote port.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMTCPIPCONFIG::GetConnectionPort()
{
  if(proxycfg.IsActive()) return proxycfg.GetPort();

  return GetRemotePort();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOSTREAMTCPIPCONFIG::GetCounterMultiServer()
* @brief      Get counter multi server
* @ingroup    DATAIO
* 
* @return     int : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMTCPIPCONFIG::GetCounterMultiServer()
{
  return countermultiserver;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTCPIPCONFIG::SetCounterMultiServer(int countermultiserver)
* @brief      Set counter multi server
* @ingroup    DATAIO
* 
* @param[in]  countermultiserver : Countermultiserver value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTCPIPCONFIG::SetCounterMultiServer(int countermultiserver)
{
  this->countermultiserver = countermultiserver;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XMUTEX* DIOSTREAMTCPIPCONFIG::GetXMutexMultiServer()
* @brief      Get X mutex multi server
* @ingroup    DATAIO
* 
* @return     XMUTEX* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XMUTEX* DIOSTREAMTCPIPCONFIG::GetXMutexMultiServer()
{
  return xmutexmultiserver;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int DIOSTREAMTCPIPCONFIG::GetHandleMultiServer()
* @brief      Get handle multi server
* @ingroup    DATAIO
* 
* @return     int : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMTCPIPCONFIG::GetHandleMultiServer()
{
  return handlemultiserver;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTCPIPCONFIG::SetHandleMultiServer(int handlemultiserver)
* @brief      Set handle multi server
* @ingroup    DATAIO
* 
* @param[in]  handlemultiserver : Handlemultiserver value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTCPIPCONFIG::SetHandleMultiServer(int handlemultiserver)
{
  this->handlemultiserver = handlemultiserver;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOSTREAMTCPIPCONFIG::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTCPIPCONFIG::Clean()
{
  type                    = DIOSTREAMTYPE_TCPIP;

  xmutexmultiserver       = NULL;

  countermultiserver      = 0;
  handlemultiserver       = -1;
}



