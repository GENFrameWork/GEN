/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamUDPConfig.cpp
*
* @class      DIOSTREAMUDPCONFIG
* @brief      Data Input/Output Stream UDP Config class
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @copyright  Copyright(c) 2008 - 2016 GEN Group.
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
*---------------------------------------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DIOStreamUDPConfig.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMUDPCONFIG::DIOSTREAMUDPCONFIG()
* @brief      Constructor
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMUDPCONFIG::DIOSTREAMUDPCONFIG() : DIOSTREAMCONFIG(), DIOSTREAMCONFIGSTRING()
{
  Clean();

  type = DIOSTREAMTYPE_UDP;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMUDPCONFIG::~DIOSTREAMUDPCONFIG()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMUDPCONFIG::~DIOSTREAMUDPCONFIG()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOIP* DIOSTREAMUDPCONFIG ::GetLocalIP()
* @brief      GetLocalIP
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     DIOIP* :
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOIP* DIOSTREAMUDPCONFIG ::GetLocalIP()
{
  return &localIP;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOURL* DIOSTREAMUDPCONFIG ::GetRemoteURL()
* @brief      GetRemoteURL
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     DIOURL* :
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOURL* DIOSTREAMUDPCONFIG ::GetRemoteURL()
{
  return &remoteURL;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOSTREAMUDPCONFIG ::GetRemotePort()
* @brief      GetRemotePort
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     int :
*
*---------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMUDPCONFIG ::GetRemotePort()
{
  return remoteport;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMUDPCONFIG::SetRemotePort(int port)
* @brief      SetRemotePort
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  port :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMUDPCONFIG::SetRemotePort(int port)
{
  this->remoteport = port;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMUDPCONFIG::IsUsedDatagrams()
* @brief      IsUsedDatagrams
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMUDPCONFIG::IsUsedDatagrams()
{
  return isuseddatagrams;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMUDPCONFIG::SetIsUsedDatagrams(bool active)
* @brief      SetIsUsedDatagrams
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  active :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMUDPCONFIG::SetIsUsedDatagrams(bool active)
{
  isuseddatagrams = active;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMUDPCONFIG::IsBroadcastModeActive()
* @brief      IsBroadcastModeActive
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMUDPCONFIG::IsBroadcastModeActive()
{
  return isbroadcastmodeactive;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMUDPCONFIG::SetBroadcastModeActive(bool active)
* @brief      SetBroadcastModeActive
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  active :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMUDPCONFIG::SetBroadcastModeActive(bool active)
{
  isbroadcastmodeactive = active;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMUDPCONFIG::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMUDPCONFIG::Clean()
{
  type                    = DIOSTREAMTYPE_UDP;

  remoteoffsetport        = 0;

  isuseddatagrams         = false;
  isbroadcastmodeactive   = false;
}



