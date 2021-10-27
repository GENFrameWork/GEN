/*------------------------------------------------------------------------------------------
//  DIOPCAP.CPP
//
//  Interface PCap Library class
//
//  Author            : Abraham J. Velez
//  Date Of Creation  : 22/10/2012 13:30:11
//  Last Mofificacion :
//
//  GEN  Copyright (C).  All right reserved.
//----------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


#ifdef DIOPCAP_ACTIVE

/*---- INCLUDES --------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "XBase.h"
#include "XFactory.h"
#include "XVector.h"
#include "XSystem.h"

#include "DIOPCap.h"

#include "XMemory_Control.h"


/*---- GENERAL VARIABLE ------------------------------------------------------------------*/


/*---- CLASS MEMBERS ---------------------------------------------------------------------*/


/*-------------------------------------------------------------------
//  DIOPCAPNETINTERFACE::DIOPCAPNETINTERFACE
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 16:37:37
//
//  @return
//  */
/*-----------------------------------------------------------------*/
DIOPCAPNETINTERFACE::DIOPCAPNETINTERFACE()
{
  Clean();
}



/*-------------------------------------------------------------------
//  DIOPCAPNETINTERFACE::~DIOPCAPNETINTERFACE
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 16:37:46
//
//  @return
//  */
/*-----------------------------------------------------------------*/
DIOPCAPNETINTERFACE::~DIOPCAPNETINTERFACE()
{
  Clean();
}



/*-------------------------------------------------------------------
//  DIOPCAPNETINTERFACE::GetName
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 16:43:16
//
//  @return       XSTRING* :
//  */
/*-----------------------------------------------------------------*/
XSTRING* DIOPCAPNETINTERFACE::GetName()
{
  return &name;
}



/*-------------------------------------------------------------------
//  DIOPCAPNETINTERFACE::GetDescription
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 16:44:21
//
//  @return       XSTRING* :
//  */
/*-----------------------------------------------------------------*/
XSTRING* DIOPCAPNETINTERFACE::GetDescription()
{
  return &description;
}



/*-------------------------------------------------------------------
//  DIOPCAPNETINTERFACE::SetName
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 16:45:03
//
//  @return       bool :
//  @param        name :
*/
/*-----------------------------------------------------------------*/
bool DIOPCAPNETINTERFACE::SetName(XCHAR* name)
{
  this->name = name;

  return true;
}



/*-------------------------------------------------------------------
//  DIOPCAPNETINTERFACE::SetName
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 16:45:28
//
//  @return       bool :
//  @param        name :
*/
/*-----------------------------------------------------------------*/
bool DIOPCAPNETINTERFACE::SetName(XSTRING& name)
{
  this->name = name.Get();

  return true;
}



/*-------------------------------------------------------------------
//  DIOPCAPNETINTERFACE::SetDescription
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 16:46:29
//
//  @return       bool :
//  @param        description :
*/
/*-----------------------------------------------------------------*/
bool DIOPCAPNETINTERFACE::SetDescription(XCHAR* description)
{
  this->description  = description;

  return true;
}



/*-------------------------------------------------------------------
//  DIOPCAPNETINTERFACE::SetDescription
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 16:47:00
//
//  @return       bool :
//  @param        description :
*/
/*-----------------------------------------------------------------*/
bool DIOPCAPNETINTERFACE::SetDescription(XSTRING& description)
{
  this->description  = description.Get();

  return true;
}




/*-------------------------------------------------------------------
//  DIOPCAPNETINTERFACE::Clean
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 16:38:12
//
//  @return       void :
//  */
/*-----------------------------------------------------------------*/
void DIOPCAPNETINTERFACE::Clean()
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPCAPFRAME::DIOPCAPFRAME
* @brief      Constructor
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  hardwareuselittleendian : true is hardware use "littleendian"
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOPCAPFRAME::DIOPCAPFRAME(bool hardwareuselittleendian)
{
  Clean();

  this->hardwareuselittleendian = hardwareuselittleendian;

  data = new XBUFFER();
}



/*-------------------------------------------------------------------
//  DIOPCAPFRAME::~DIOPCAPFRAME
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      23/10/2012 16:57:45
//
//  @return       virtual :
//  */
/*-----------------------------------------------------------------*/
DIOPCAPFRAME::~DIOPCAPFRAME()
{
  if(data)
    {
      delete data;
      data = NULL;
    }

  Clean();
}




/*-------------------------------------------------------------------
//  DIOPCAPFRAME::GetHeaderEthernet
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      26/10/2012 14:44:20
//
//  @return       bool :
//  @param        ethernetheader :
*/
/*-----------------------------------------------------------------*/
bool DIOPCAPFRAME::GetHeaderEthernet(DIOPCAPETHERNETHEADER& ethernetheader)
{
  if(!xfactory) return false;
  if(!data)     return false;

  if(data->GetSize() < (sizeof(DIOPCAPETHERNETHEADER))) return false;

  DIOPCAPETHERNETHEADER* _ethernetheader = (DIOPCAPETHERNETHEADER*)(data->Get());
  if(!_ethernetheader) return false;

  memcpy((XBYTE*)&ethernetheader,(XBYTE*)(_ethernetheader),sizeof(DIOPCAPETHERNETHEADER));

  if(hardwareuselittleendian)
    {
      SWAPWORD(ethernetheader.type);
    }

  return true;
}




/*-------------------------------------------------------------------
//  DIOPCAPFRAME::GetHeaderIP
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      26/10/2012 14:46:11
//
//  @return       bool :
//  @param        ipheader :
*/
/*-----------------------------------------------------------------*/
bool DIOPCAPFRAME::GetHeaderIP(DIOPCAPIPHEADER& ipheader)
{
  if(!xfactory) return false;
  if(!data) return false;

  if(data->GetSize() < (sizeof(DIOPCAPETHERNETHEADER) + sizeof(DIOPCAPIPHEADER))) return false;

  DIOPCAPIPHEADER* _ipheader = (DIOPCAPIPHEADER*)(data->Get() + sizeof(DIOPCAPETHERNETHEADER));
  if(!_ipheader) return false;

  memcpy((XBYTE*)&ipheader,(XBYTE*)(_ipheader),sizeof(DIOPCAPIPHEADER));

  if(hardwareuselittleendian)
    {
      SWAPWORD(ipheader.len);
      SWAPWORD(ipheader.identification);
      SWAPWORD(ipheader.flags_fo);
      SWAPWORD(ipheader.CRC);
    }

  return true; //length of ethernet header
}



/*-------------------------------------------------------------------
//  DIOPCAPFRAME::GetHeaderUDP
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      26/10/2012 14:51:38
//
//  @return       bool :
//  @param        udpheader :
*/
/*-----------------------------------------------------------------*/
bool DIOPCAPFRAME::GetHeaderUDP(DIOPCAPUDPHEADER& udpheader)
{
  DIOPCAPIPHEADER ipheader;
  if(!GetHeaderIP(ipheader)) return false;

  int iplen = (ipheader.ver_ihl & 0xf) * 4;

  DIOPCAPUDPHEADER* _udpheader = (DIOPCAPUDPHEADER*)((XBYTE*)data->Get() + sizeof(DIOPCAPETHERNETHEADER) + iplen);
  if(!_udpheader) return false;

  memcpy((XBYTE*)&udpheader,(XBYTE*)(_udpheader),sizeof(DIOPCAPUDPHEADER));

  if(hardwareuselittleendian)
    {
      SWAPWORD(udpheader.sourceport);        // Source port
      SWAPWORD(udpheader.targetport);        // Target port
      SWAPWORD(udpheader.len);               // Datagram length
      SWAPWORD(udpheader.CRC);               // Checksum
    }

  return true;
}




/*-------------------------------------------------------------------
//  DIOPCAPFRAME::GetHeaderTCP
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      26/10/2012 14:55:05
//
//  @return       bool :
//  @param        tcpheader :
*/
/*-----------------------------------------------------------------*/
bool DIOPCAPFRAME::GetHeaderTCP(DIOPCAPTCPHEADER& tcpheader)
{
  DIOPCAPTCPHEADER* _tcpheader = (DIOPCAPTCPHEADER*)(data->Get() + sizeof(DIOPCAPETHERNETHEADER) + sizeof(DIOPCAPIPHEADER));
  if(!_tcpheader) return false;

  memcpy((XBYTE*)&tcpheader,(XBYTE*)(_tcpheader),sizeof(DIOPCAPTCPHEADER));

  if(hardwareuselittleendian)
    {
      SWAPWORD (tcpheader.sourceport);
      SWAPWORD (tcpheader.targetport);
      SWAPDWORD(tcpheader.seq);
      SWAPDWORD(tcpheader.ack_seq);
      SWAPWORD (tcpheader.dataoffctrl);
      SWAPWORD (tcpheader.window);
      SWAPWORD (tcpheader.check);
      SWAPWORD (tcpheader.urg_ptr);
    }

  return true;
}




/*-------------------------------------------------------------------
//  DIOPCAPFRAME::UserData_Get
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      28/10/2012 20:29:35
//
//  @return       XBYTE* :
//  */
/*-----------------------------------------------------------------*/
XBYTE* DIOPCAPFRAME::UserData_Get()
{
  XBYTE* userdata = data->Get();

  DIOPCAPETHERNETHEADER ethernetheader;
  if(GetHeaderEthernet(ethernetheader))
    {
      userdata += sizeof(DIOPCAPETHERNETHEADER);

      if(ethernetheader.type == DIOPCAPETHERNETTYPE_IP)
        {
          DIOPCAPIPHEADER ipheader;
          if(GetHeaderIP(ipheader))
            {
              userdata += sizeof(DIOPCAPIPHEADER);

              switch(ipheader.protocol)
                {
                  case DIOPCAPIPPROTOCOLTYPE_TCP : userdata += sizeof(DIOPCAPTCPHEADER);
                                                   break;

                  case DIOPCAPIPPROTOCOLTYPE_UDP : userdata += sizeof(DIOPCAPUDPHEADER);
                                                   break;
                }
            }
        }
    }

  return userdata;
}


/*-------------------------------------------------------------------
//  DIOPCAPFRAME::UserData_GetSize
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      28/10/2012 20:29:45
//
//  @return       int :
//  */
/*-----------------------------------------------------------------*/
int DIOPCAPFRAME::UserData_GetSize()
{
  int userdatasize = data->GetSize();

  DIOPCAPETHERNETHEADER ethernetheader;
  if(GetHeaderEthernet(ethernetheader))
    {
      userdatasize -= sizeof(DIOPCAPETHERNETHEADER);

      if(ethernetheader.type == DIOPCAPETHERNETTYPE_IP)
        {
          DIOPCAPIPHEADER ipheader;
          if(GetHeaderIP(ipheader))
            {
              userdatasize -= sizeof(DIOPCAPIPHEADER);

              switch(ipheader.protocol)
                {
                  case DIOPCAPIPPROTOCOLTYPE_TCP : userdatasize -= sizeof(DIOPCAPTCPHEADER);
                                                   break;

                  case DIOPCAPIPPROTOCOLTYPE_UDP : userdatasize -= sizeof(DIOPCAPUDPHEADER);
                                                   break;
                }
            }
        }
    }

  return userdatasize;
}



/*-------------------------------------------------------------------
//  DIOPCAPFRAME::GetData
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      23/10/2012 17:01:53
//
//  @return       XBUFFER* :
//  */
/*-----------------------------------------------------------------*/
XBUFFER* DIOPCAPFRAME::GetData()
{
  return data;
}




/*-------------------------------------------------------------------
//  DIOPCAPFRAME::SetData
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      23/10/2012 17:01:58
//
//  @return       bool :
//  @param        data :
//  @param        size :
*/
/*-----------------------------------------------------------------*/
bool DIOPCAPFRAME::SetData(XBYTE* data, XDWORD size)
{
  if(!this->data) return false;
  if(!data)       return false;
  if(!size)       return false;

  this->data->Delete();
  this->data->Add(data,size);

  return true;
}




/*-------------------------------------------------------------------
//  DIOPCAPFRAME::Clean
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      23/10/2012 16:58:49
//
//  @return       void :
//  */
/*-----------------------------------------------------------------*/
void DIOPCAPFRAME::Clean()
{
  data                    = NULL;
  hardwareuselittleendian = false;
}



/*-------------------------------------------------------------------
//  DIOPCAP::DIOPCAP
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      26/10/2012 13:08:06
//
//  @return
//  @param        xfactory :
//  @param        uselittleendian :
*/
/*-----------------------------------------------------------------*/
DIOPCAP::DIOPCAP()
{
  Clean();

  xsystem = GEN_XFACTORY.CreateSystem();
}



/*-------------------------------------------------------------------
//  DIOPCAP::~DIOPCAP
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 15:18:36
//
//  @return
//  */
/*-----------------------------------------------------------------*/
DIOPCAP::~DIOPCAP()
{
  End();

  if(xsystem) GEN_XFACTORY.DeleteSystem(xsystem);

  Clean();
}



/*-------------------------------------------------------------------
//  DIOPCAP::GetNetInterfaces
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 16:49:52
//
//  @return       XVECTOR<DIOPCAPNETINTERFACE*>* :
//  */
/*-----------------------------------------------------------------*/
XVECTOR<DIOPCAPNETINTERFACE*>* DIOPCAP::GetNetInterfaces()
{
  return &netinterfaces;
}




/*-------------------------------------------------------------------
//  DIOPCAP::GetNetInterface
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 17:26:15
//
//  @return       DIOPCAPNETINTERFACE* :
//  @param        index :
*/
/*-----------------------------------------------------------------*/
DIOPCAPNETINTERFACE* DIOPCAP::GetNetInterface(int index)
{
  return (DIOPCAPNETINTERFACE*)netinterfaces.Get(index);
}



/*-------------------------------------------------------------------
//  DIOPCAP::Ini
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 16:06:06
//
//  @return       bool :
//  */
/*-----------------------------------------------------------------*/
bool DIOPCAP::Ini()
{
  if(!CreateListNetInterfaces()) return false;

  xmutexframes = GEN_XFACTORY.Create_Mutex();

  return netinterfaces.GetSize()?true:false;
}



/*-------------------------------------------------------------------
//  DIOPCAP::Capture_Start
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 17:29:20
//
//  @return       bool :
//  @param        index :
*/
/*-----------------------------------------------------------------*/
bool DIOPCAP::Capture_Start(int index, bool promiscuousmode, int timeout)
{
  DIOPCAPNETINTERFACE* netinterface = GetNetInterface(index);
  if(!netinterface) return false;

  return Capture_Start(netinterface, promiscuousmode, timeout);
}




/*-------------------------------------------------------------------
//  DIOPCAP::Frames_Get
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      23/10/2012 17:40:47
//
//  @return       DIOPCAPFRAME* :
//  @param        index :
*/
/*-----------------------------------------------------------------*/
DIOPCAPFRAME* DIOPCAP::Frames_Get(int index)
{
  if(index<0)                      return NULL;
  if(index>=(int)frames.GetSize()) return NULL;

  if(xmutexframes) xmutexframes->Lock();

  DIOPCAPFRAME* frame = (DIOPCAPFRAME*)frames.Get(index);

  if(xmutexframes) xmutexframes->UnLock();

  return frame;
}




/*-------------------------------------------------------------------
//  DIOPCAP::Frames_Delete
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      23/10/2012 17:42:09
//
//  @return       bool :
//  @param        index :
*/
/*-----------------------------------------------------------------*/
bool DIOPCAP::Frames_Delete(int index)
{
  if(xmutexframes) xmutexframes->Lock();

  bool status = false;

  DIOPCAPFRAME* frame = (DIOPCAPFRAME*)frames.Get(index);
  if(frame)
    {
      frames.Delete(frame);
      delete frame;
      status = true;
    }

  if(xmutexframes) xmutexframes->UnLock();

  return status;
}



/*-------------------------------------------------------------------
//  DIOPCAP::Frames_Get
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      23/10/2012 17:57:20
//
//  @return       XVECTOR<DIOPCAPFRAME*>* :
//  */
/*-----------------------------------------------------------------*/
XVECTOR<DIOPCAPFRAME*>* DIOPCAP::Frames_Get()
{
  return &frames;
}



/*-------------------------------------------------------------------
//  DIOPCAP::Frames_DeleteAll
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      23/10/2012 17:44:37
//
//  @return       bool :
//  */
/*-----------------------------------------------------------------*/
bool DIOPCAP::Frames_DeleteAll()
{
  if(frames.IsEmpty())  return false;

  if(xmutexframes) xmutexframes->Lock();

  frames.DeleteContents();

  frames.DeleteAll();

  if(xmutexframes) xmutexframes->UnLock();

  return true;
}




/*-------------------------------------------------------------------
//  DIOPCAP::End
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 16:06:19
//
//  @return       bool :
//  */
/*-----------------------------------------------------------------*/
bool DIOPCAP::End()
{
  DeleteListNetInterfaces();

  Frames_DeleteAll();

  if(xmutexframes)
    {
      GEN_XFACTORY.Delete_Mutex(xmutexframes);
      xmutexframes = NULL;
    }

  return true;
}



/*-------------------------------------------------------------------
//  DIOPCAP::Frames_Add
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      23/10/2012 17:37:57
//
//  @return       bool :
//  @param        data :
//  @param        size :
*/
/*-----------------------------------------------------------------*/
bool DIOPCAP::Frames_Add(XBYTE* data,XDWORD size)
{
  if(!data) return false;
  if(!size) return false;

  bool status = false;

  if(xmutexframes) xmutexframes->Lock();

  bool hardwareuselittleendian = true;

  if(xsystem) hardwareuselittleendian = xsystem->HardwareUseLittleEndian();


  DIOPCAPFRAME* frame = new DIOPCAPFRAME(hardwareuselittleendian);
  if(frame)
    {
      if(frame->SetData(data,size))
        {
          frames.Add(frame);
          status = true;
        }
    }

  if(xmutexframes) xmutexframes->UnLock();

  return status;
}




/*-------------------------------------------------------------------
//  DIOPCAP::Clean
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 15:18:56
//
//  @return       void :
//  */
/*-----------------------------------------------------------------*/
void DIOPCAP::Clean()
{
  xmutexframes    = NULL;
  xsystem         = NULL;
}



/*-------------------------------------------------------------------
//  DIOPCAP::DeleteListNetInterfaces
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      22/10/2012 17:01:06
//
//  @return       bool :
//  */
/*-----------------------------------------------------------------*/
bool DIOPCAP::DeleteListNetInterfaces()
{
  if(netinterfaces.IsEmpty()) return false;

  netinterfaces.DeleteContents();

  netinterfaces.DeleteAll();

  return true;
}


#endif