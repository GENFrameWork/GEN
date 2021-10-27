//------------------------------------------------------------------------------------------
//  DIOANDROIDSTREAMTCPIP.CPP
//
//  ANDROID Data IO Stream IP Local Enum Devices class
//
//  Author            : Abraham J. Velez
//  Date Of Creation  : 02/01/2002
//  Last Mofificacion :
//
//  GEN  Copyright (C).  All right reserved.
//------------------------------------------------------------------------------------------


/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"

//---- INCLUDES ----------------------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>


#include "XTrace.h"
#include "DIOStreamDeviceIP.h"

#include "DIOANDROIDStreamIPLocalEnumDevices.h"

#include "XMemory_Control.h"


//---- GENERAL VARIABLE --------------------------------------------------------------------


//---- CLASS MEMBERS -----------------------------------------------------------------------




/*-------------------------------------------------------------------
//  DIOANDROIDSTREAMTCPIPLOCALENUMDEVICES::DIOANDROIDSTREAMTCPIPLOCALENUMDEVICES
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      28/04/2013 19:00:01
//
//  @return


*/
/*-----------------------------------------------------------------*/
DIOANDROIDSTREAMIPLOCALENUMDEVICES::DIOANDROIDSTREAMIPLOCALENUMDEVICES() : DIOSTREAMIPLOCALENUMDEVICES()
{

}



/*-------------------------------------------------------------------
//  DIOANDROIDSTREAMTCPIPLOCALENUMDEVICES::~DIOANDROIDSTREAMTCPIPLOCALENUMDEVICES
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      28/04/2013 19:00:12
//
//  @return
//  */
/*-----------------------------------------------------------------*/
DIOANDROIDSTREAMIPLOCALENUMDEVICES::~DIOANDROIDSTREAMIPLOCALENUMDEVICES()
{

}



/*-------------------------------------------------------------------
//  DIOANDROIDSTREAMTCPIPLOCALENUMDEVICES::Search
*/
/**
//
//
//  @author      Abraham J. Velez
//  @version      28/04/2013 19:00:27
//
//  @return       bool :
//  */
/*-----------------------------------------------------------------*/
bool DIOANDROIDSTREAMIPLOCALENUMDEVICES::Search()
{
  DelAllDevices();

  int           sock;
  struct ifconf ifconf;
  struct ifreq  ifreq[DIOANDROIDSTREAMIPLOCALENUMDEVICES_MAXIFS];
  int           ninterfaces;
  int           c;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) return false;

  ifconf.ifc_buf = (char *) ifreq;
  ifconf.ifc_len = sizeof ifreq;

  if(ioctl(sock, SIOCGIFCONF, &ifconf) == -1)
    {
      close(sock);
      return false;
    }

  ninterfaces = ifconf.ifc_len / sizeof(ifreq[0]);

  for(c=0; c<ninterfaces; c++)
    {
       char                 ip[INET_ADDRSTRLEN];
       struct sockaddr_in*  address = (struct sockaddr_in *) &ifreq[c].ifr_addr;

        if(!inet_ntop(AF_INET, &address->sin_addr, ip, sizeof(ip)))
          {
            close(sock);
            return false;
          }

        DIOSTREAMDEVICEIP* device = new DIOSTREAMDEVICEIP();
        if(device)
          {
            device->SetIndex(c);

            device->GetName()->Set(ifreq[c].ifr_name);
            device->GetDescription()->Set(__L(""));
            device->GetMAC()->Set((XBYTE*)ifreq[c].ifr_hwaddr.sa_data);

            device->SetIPType(DIOSTREAMIPDEVICE_TYPE_UNKNOWN);

            if(ifreq[c].ifr_flags & IFF_LOOPBACK)
              {
                device->SetIPType(DIOSTREAMIPDEVICE_TYPE_LOOPBACK);
              }
             else
              {
                if(device->GetName()->Find(__L("eth"), true)!=XSTRING_NOTFOUND)  device->SetIPType(DIOSTREAMIPDEVICE_TYPE_ETHERNET);
                if(device->GetName()->Find(__L("wlan"), true)!=XSTRING_NOTFOUND) device->SetIPType(DIOSTREAMIPDEVICE_TYPE_WIFI);
              }

            XSTRING IP;
            IP.Set(ip);
            device->GetIP()->Set(IP);

            devices.Add(device);
          }
    }

  close(sock);

  return true;
}



