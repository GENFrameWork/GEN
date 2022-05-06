/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamUDPLocalEnumServers.cpp
*
* @class      DIOSTREAMUDPLOCALENUMSERVERS
* @brief      Data Input/Output Stream UDP Local Enum Servers class
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XFactory.h"
#include "XBuffer.h"
#include "XThreadCollected.h"

#include "HashCRC32.h"

#include "DIOFactory.h"
#include "DIOStreamDeviceIP.h"
#include "DIOStreamUDPConfig.h"
#include "DIOStreamUDP.h"

#include "DIOStreamUDPLocalEnumServers.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/






/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMUDPLOCALENUMSERVERS::DIOSTREAMUDPLOCALENUMSERVERS()
* @brief      Constructor
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMUDPLOCALENUMSERVERS::DIOSTREAMUDPLOCALENUMSERVERS(): DIOSTREAMENUMSERVERS()
{
  Clean();

  xthread  = CREATEXTHREAD(XTHREADGROUPID_DIOSTREAMUDP, __L("DIOSTREAMUDPLOCALENUMSERVERS::DIOSTREAMUDPLOCALENUMSERVERS"), ThreadDiscoveryLocalServer, this);
  if(xthread) xthread->Ini(false);

  diostreamudpcfg = new DIOSTREAMUDPCONFIG();
  diostreamudp    = (DIOSTREAMUDP*)GEN_DIOFACTORY.CreateStreamIO(diostreamudpcfg);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMUDPLOCALENUMSERVERS::~DIOSTREAMUDPLOCALENUMSERVERS()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMUDPLOCALENUMSERVERS::~DIOSTREAMUDPLOCALENUMSERVERS()
{
  GEN_DIOFACTORY.DeleteStreamIO(diostreamudp);
  delete diostreamudpcfg;

  if(xthread)
    {
      xthread->End();
      DELETEXTHREAD(XTHREADGROUPID_DIOSTREAMUDP, xthread);
    }

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMUDPLOCALENUMSERVERS::Search()
* @brief      Search
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMUDPLOCALENUMSERVERS::Search()
{
  if(!xthread)    return false;
  if(issearching) return false;

  DelAllDevices();

  issearching = true;

  return xthread->Run(true);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMUDPLOCALENUMSERVERS::StopSearch(bool waitend)
* @brief      StopSearch
* @ingroup    DATAIO
*
* @param[in]  waitend :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMUDPLOCALENUMSERVERS::StopSearch(bool waitend)
{
  if(!xthread)       return false;
  if(!issearching)   return false;

  issearching = false;

  xthread->Run(false);

  xthread->Wait();

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMUDPLOCALENUMSERVERS::IsDeviceAvailable(XCHAR* resource)
* @brief      IsDeviceAvailable
* @ingroup    DATAIO
*
* @param[in]  resource :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMUDPLOCALENUMSERVERS::IsDeviceAvailable(XCHAR* resource)
{
  for(int c=0;c<(int)GetDevices()->GetSize();c++)
    {
      DIOSTREAMDEVICEIP* device = (DIOSTREAMDEVICEIP*)GetDevices()->Get(c);
      if(device)
        {
          XSTRING IPstring;

          device->GetIP()->GetXString(IPstring);

          if(!IPstring.Compare(resource))
            {
              return true;
            }
        }
    }

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMUDPCONFIG* DIOSTREAMUDPLOCALENUMSERVERS::GetDIOStreamUDPCFG()
* @brief      GetDIOStreamUDPCFG
* @ingroup    DATAIO
*
* @return     DIOSTREAMUDPCONFIG* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMUDPCONFIG* DIOSTREAMUDPLOCALENUMSERVERS::GetDIOStreamUDPCFG()
{
  return diostreamudpcfg;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMUDP* DIOSTREAMUDPLOCALENUMSERVERS::GetDIOStreamUDP()
* @brief      GetDIOStreamUDP
* @ingroup    DATAIO
*
* @return     DIOSTREAMUDP* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMUDP* DIOSTREAMUDPLOCALENUMSERVERS::GetDIOStreamUDP()
{
  return diostreamudp;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMUDPLOCALENUMSERVERS::SendSignal()
* @brief      SendSignal
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMUDPLOCALENUMSERVERS::SendSignal()
{
  if(!diostreamudp)     return false;
  if(!diostreamudpcfg)  return false;

  diostreamudpcfg->SetMode(DIOSTREAMMODE_CLIENT);
  diostreamudpcfg->SetBroadcastModeActive(true);
  diostreamudpcfg->SetRemotePort(port);
  diostreamudpcfg->SetIsUsedDatagrams(false);

  if(diostreamudp->Open())
    {
      XSTRING     string;
      XBUFFER     xbuffer;
      HASHCRC32   crc32;

      string.Format(DIOSTREAMUDPLOCALENUMSERVERSID,ID);

      XSTRING_CREATENORMALIZE(string, buffnormalize, false)

      crc32.Do((XBYTE*)buffnormalize,  string.GetSize()*sizeof(XWORD));

      xbuffer.Add((XDWORD)crc32.GetResultCRC32());
      xbuffer.Add((XBYTE*)buffnormalize, string.GetSize()*sizeof(XWORD));
      xbuffer.Add((XWORD)0);

      XSTRING_DELETENORMALIZE(string, buffnormalize)

      diostreamudp->Write(xbuffer.Get(),xbuffer.GetSize());

      diostreamudp->WaitToFlushXBuffers(5);

      diostreamudp->Close();
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOSTREAMUDPLOCALENUMSERVERS::GetPort()
* @brief      GetPort
* @ingroup    DATAIO
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMUDPLOCALENUMSERVERS::GetPort()
{
  return port;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMUDPLOCALENUMSERVERS::SetPort(int port)
* @brief      SetPort
* @ingroup    DATAIO
*
* @param[in]  port :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMUDPLOCALENUMSERVERS::SetPort(int port)
{
  this->port = port;

  return true;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMUDPLOCALENUMSERVERS::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMUDPLOCALENUMSERVERS::Clean()
{
  type             = DIOSTREAMENUMTYPE_TCPIP_LOCAL_SERVERS;

  diostreamudpcfg  = NULL;
  diostreamudp     = NULL;

  port             = 0;

  xthread          = NULL;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMUDPLOCALENUMSERVERS::ThreadDiscoveryLocalServer(void* data)
* @brief      ThreadDiscoveryLocalServer
* @ingroup    DATAIO
*
* @param[in]  data :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMUDPLOCALENUMSERVERS::ThreadDiscoveryLocalServer(void* data)
{
  DIOSTREAMUDPLOCALENUMSERVERS* discoveryenum = (DIOSTREAMUDPLOCALENUMSERVERS*)data;
  if(!discoveryenum) return;

  if(!discoveryenum->GetDIOStreamUDP())    return;
  if(!discoveryenum->GetDIOStreamUDPCFG()) return;

  discoveryenum->issearching = true;

  DIOSTREAMUDPCONFIG* diostreamudpcfg = discoveryenum->GetDIOStreamUDPCFG();
  DIOSTREAMUDP*       diostreamudp    = discoveryenum->GetDIOStreamUDP();

  diostreamudpcfg->SetMode(DIOSTREAMMODE_SERVER);
  diostreamudpcfg->SetBroadcastModeActive(true);
  diostreamudpcfg->SetRemotePort(discoveryenum->GetPort());
  diostreamudpcfg->SetIsUsedDatagrams(true);

  discoveryenum->xtimer->Reset();

  if(diostreamudp->Open())
    {
        while(discoveryenum->issearching)
          {
            if(discoveryenum->searchtimeout)
              {
                if(discoveryenum->xtimer->GetMeasureSeconds()>=discoveryenum->searchtimeout) break;
              }

            XBUFFER  data;
            XSTRING  address;
            XWORD    port;

            if(diostreamudp->ReadDatagram(address, port, data))
              {
                XSTRING     string;
                XSTRING     string2(256);
                XDWORD      ID = discoveryenum->GetID();
                HASHCRC32   crc32;
                XDWORD      crc32result[2];

                data.Extract(crc32result[0]);

                string.Format(DIOSTREAMUDPLOCALENUMSERVERSID,ID);

                string2.Set((XWORD*)data.Get());
                string2.AdjustSize();

                if(!string.Compare(string2))
                  {
                    XSTRING_CREATENORMALIZE(string2, buffnormalize, false)
                    crc32.Do((XBYTE*)buffnormalize, string2.GetSize()*sizeof(XWORD));
                    XSTRING_DELETENORMALIZE(string2, buffnormalize)

                    crc32result[1] = crc32.GetResultCRC32();

                    if(crc32result[0] == crc32result[1])
                      {
                        if(!discoveryenum->IsDeviceAvailable(address.Get()))
                          {
                            DIOSTREAMDEVICEIP* device = new DIOSTREAMDEVICEIP();
                            if(device)
                              {
                                device->GetIP()->Set(address);
                                discoveryenum->devices.Add(device);

                                if(!discoveryenum->searchtimeout) break;
                              }
                          }
                      }
                  }
              }
          }

      diostreamudp->WaitToFlushXBuffers(5);

      diostreamudp->Close();
    }

  discoveryenum->xthread->Run(false);

  discoveryenum->issearching = false;
}






