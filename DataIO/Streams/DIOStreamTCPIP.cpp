/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTCPIP.cpp
* 
* @class      DIOSTREAMTCPIP
* @brief      Data Input/Output Stream TCP/IP class
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

#include "DIOStreamTCPIP.h"

#include "DIOFactory.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTCPIP::DIOSTREAMTCPIP()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTCPIP::DIOSTREAMTCPIP(): DIOSTREAM()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTCPIP::~DIOSTREAMTCPIP()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTCPIP::~DIOSTREAMTCPIP()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMCONFIG* DIOSTREAMTCPIP::GetConfig()
* @brief      Get config
* @ingroup    DATAIO
*
* @return     DIOSTREAMCONFIG* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMCONFIG* DIOSTREAMTCPIP::GetConfig()
{
  return (DIOSTREAMCONFIG*)config;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTCPIP::SetConfig(DIOSTREAMCONFIG* config)
* @brief      Set config
* @ingroup    DATAIO
*
* @param[in]  config :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTCPIP::SetConfig(DIOSTREAMCONFIG* config)
{
  if(!config) return false;
  this->config = (DIOSTREAMTCPIPCONFIG  *)config;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOIP* DIOSTREAMTCPIP::GetClientIP()
* @brief      Get client IP
* @ingroup    DATAIO
*
* @return     DIOIP* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOIP* DIOSTREAMTCPIP::GetClientIP()
{
  return &clientIP;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMENUMSERVERS* DIOSTREAMTCPIP::GetEnumServers()
* @brief      Get enum servers
* @ingroup    DATAIO
*
* @return     DIOSTREAMENUMSERVERS* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMENUMSERVERS* DIOSTREAMTCPIP::GetEnumServers()
{
  return enumservers;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTCPIP::SetEnumServers(DIOSTREAMENUMSERVERS* enumservers)
* @brief      Set enum servers
* @ingroup    DATAIO
*
* @param[in]  enumservers :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTCPIP::SetEnumServers(DIOSTREAMENUMSERVERS* enumservers)
{
  this->enumservers = enumservers;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTCPIP::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTCPIP::Clean()
{
  config      = NULL;
  enumservers = NULL;
}



