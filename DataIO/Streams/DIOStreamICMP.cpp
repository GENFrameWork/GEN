/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamICMP.cpp
* 
* @class      DIOSTREAMICMP
* @brief      Data Input/Output Stream ICMP class
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

#include "DIOStreamICMP.h"

#include "XFactory.h"
#include "XBuffer.h"
#include "XThread.h"

#include "DIOFactory.h"

#include "DIOStreamUDPConfig.h"

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

#pragma region CLASS_DIOSTREAMICMPDATAGRAM


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMICMPDATAGRAM::DIOSTREAMICMPDATAGRAM()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMICMPDATAGRAM::DIOSTREAMICMPDATAGRAM()
{
  Clean();

  data = new XBUFFER();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMICMPDATAGRAM::~DIOSTREAMICMPDATAGRAM()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMICMPDATAGRAM::~DIOSTREAMICMPDATAGRAM()
{
  if(data) delete data;

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMICMPDATAGRAM::IsToSend()
* @brief      Is to send
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMICMPDATAGRAM::IsToSend()
{
  return istosend;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMICMPDATAGRAM::SetIsToSend(bool istosend)
* @brief      Set is to send
* @ingroup    DATAIO
*
* @param[in]  istosend :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMICMPDATAGRAM::SetIsToSend(bool istosend)
{
  this->istosend = istosend;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOSTREAMICMPDATAGRAM::GetAddress()
* @brief      Get address
* @ingroup    DATAIO
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOSTREAMICMPDATAGRAM::GetAddress()
{
  return &address;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMICMPDATAGRAM::SetAddress(XCHAR* address)
* @brief      Set address
* @ingroup    DATAIO
*
* @param[in]  address :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMICMPDATAGRAM::SetAddress(XCHAR* address)
{
  if(!address) return false;
  this->address = address;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMICMPDATAGRAM::SetAddress(XSTRING& address)
* @brief      Set address
* @ingroup    DATAIO
*
* @param[in]  address :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMICMPDATAGRAM::SetAddress(XSTRING& address)
{
  return SetAddress(address.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* DIOSTREAMICMPDATAGRAM::GetData()
* @brief      Get data
* @ingroup    DATAIO
*
* @return     XBUFFER* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* DIOSTREAMICMPDATAGRAM::GetData()
{
  return data;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMICMPDATAGRAM::SetData(XBYTE* data,XDWORD size)
* @brief      Set data
* @ingroup    DATAIO
*
* @param[in]  data :
* @param[in]  size :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMICMPDATAGRAM::SetData(XBYTE* data,XDWORD size)
{
  if(!this->data) return false;
  this->data->Delete();
  this->data->Add(data,size);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMICMPDATAGRAM::SetData(XBUFFER& data)
* @brief      Set data
* @ingroup    DATAIO
*
* @param[in]  data :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMICMPDATAGRAM::SetData(XBUFFER& data)
{
  SetData(data.Get(), data.GetSize());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMICMPDATAGRAM::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMICMPDATAGRAM::Clean()
{
  istosend = false;
  address.Empty();
  data     = NULL;
}


#pragma endregion


#pragma region CLASS_DIOSTREAMICMP


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMICMP::DIOSTREAMICMP()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMICMP::DIOSTREAMICMP() : DIOSTREAM()
{
  Clean();

  GEN_XFACTORY_CREATE(datagramsmutex, Create_Mutex())
  host           = new DIOURL();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMICMP::~DIOSTREAMICMP()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMICMP::~DIOSTREAMICMP()
{
  DeleteAllDatagrams();

  delete host;

  GEN_XFACTORY.Delete_Mutex(datagramsmutex);

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMCONFIG* DIOSTREAMICMP::GetConfig()
* @brief      Get config
* @ingroup    DATAIO
*
* @return     DIOSTREAMCONFIG* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMCONFIG* DIOSTREAMICMP::GetConfig()
{
  return (DIOSTREAMCONFIG*)config;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMICMP::SetConfig(DIOSTREAMCONFIG* config)
* @brief      Set config
* @ingroup    DATAIO
*
* @param[in]  config :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMICMP::SetConfig(DIOSTREAMCONFIG* config)
{
  if(!config) return false;
  this->config = (DIOSTREAMICMPCONFIG *)config;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSTREAMICMP::Read(XBYTE* buffer, XDWORD size)
* @brief      Read
* @ingroup    DATAIO
* @note       NO VALID FUNCTION FOR THIS STREAM: USE WriteDatagram
*
* @param[in]  buffer :
* @param[in]  size :
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOSTREAMICMP::Read(XBYTE* buffer, XDWORD size)
{
  XDWORD esize = 0;

  int index = GetFirstDatagram(false);
  if(index != DIOSTREAMICMP_NOTFOUND)
    {
      DIOSTREAMICMPDATAGRAM* datagram = GetDatagram(index);
      if(datagram)
        {
          esize = datagram->GetData()->GetSize();
          if(esize >= size)
            {
              esize = size;

              memcpy(buffer,(XBYTE*)datagram->GetData()->Get(), size);

              nbytesread+= size;
              if(xtimernotactivity) xtimernotactivity->Reset();

              DeleteDatagram(index);

              inbuffer->Extract(buffer, 0, size);
            }
        }
    }

  return esize;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSTREAMICMP::Write(XBYTE* buffer, XDWORD size)
* @brief      Write
* @ingroup    DATAIO
* @note       NO VALID FUNCTION FOR THIS STREAM: USE WriteDatagram
*
* @param[in]  buffer :
* @param[in]  size :
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOSTREAMICMP::Write(XBYTE* buffer, XDWORD size)
{
  if(!AddDatagram(true, __L(""), buffer, size)) return 0;

  if(!outbuffer->Add(buffer,size))  return 0;

  nbyteswrite+=size;
  if(xtimernotactivity) xtimernotactivity->Reset();

  return size;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMICMP::ReadDatagram(XSTRING& address, XBUFFER& xbuffer)
* @brief      Read datagram
* @ingroup    DATAIO
*
* @param[in]  address :
* @param[in]  xbuffer :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMICMP::ReadDatagram(XSTRING& address, XBUFFER& xbuffer)
{
  int index = GetFirstDatagram(false);
  if(index != DIOSTREAMICMP_NOTFOUND)
    {
      DIOSTREAMICMPDATAGRAM* datagram = GetDatagram(index);
      if(datagram)
        {
          address = datagram->GetAddress()->Get();

          xbuffer.Delete();
          xbuffer.Add(datagram->GetData());

          nbytesread+= datagram->GetData()->GetSize();
          if(xtimernotactivity) xtimernotactivity->Reset();

          DeleteDatagram(index);

          return true;
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMICMP::WriteDatagram(XSTRING& address, XBYTE* buffer, XDWORD size)
* @brief      Write datagram
* @ingroup    DATAIO
*
* @param[in]  address :
* @param[in]  buffer :
* @param[in]  size :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMICMP::WriteDatagram(XSTRING& address, XBYTE* buffer, XDWORD size)
{
  if(address.IsEmpty()) return false;
  if(!AddDatagram(true, address, buffer, size)) return false;
  if(!outbuffer->Add(buffer,size)) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMICMP::WriteDatagram(XSTRING& address, XBUFFER& xbuffer)
* @brief      Write datagram
* @ingroup    DATAIO
*
* @param[in]  address :
* @param[in]  xbuffer :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMICMP::WriteDatagram(XSTRING& address, XBUFFER& xbuffer)
{
  return WriteDatagram(address, xbuffer.Get(), xbuffer.GetSize());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMICMP::ResetXBuffers()
* @brief      Reset X buffers
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMICMP::ResetXBuffers()
{
  if(!DIOSTREAM::ResetXBuffers()) return false;

  return DeleteAllDatagrams();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMICMP::ResetInXBuffer()
* @brief      Reset in X buffer
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMICMP::ResetInXBuffer()
{
  if(!DIOSTREAM::ResetInXBuffer()) return false;

  int index = 0;

  while(1)
    {
      DIOSTREAMICMPDATAGRAM* datagram = (DIOSTREAMICMPDATAGRAM*)datagrams.Get(index);
      if(!datagram) break;

      if(datagram->GetAddress()->GetSize())
        {
          DeleteDatagram(index);

        } else index++;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMICMP::ResetOutXBuffer()
* @brief      Reset out X buffer
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMICMP::ResetOutXBuffer()
{
  if(!DIOSTREAM::ResetOutXBuffer()) return false;

  int index = 0;

  while(1)
    {
      DIOSTREAMICMPDATAGRAM* datagram = (DIOSTREAMICMPDATAGRAM*)datagrams.Get(index);
      if(!datagram) break;

      if(!datagram->GetAddress()->GetSize())
        {
          DeleteDatagram(index);

        } else index++;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOURL* DIOSTREAMICMP::GetHost()
* @brief      Get host
* @ingroup    DATAIO
*
* @return     DIOURL* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOURL* DIOSTREAMICMP::GetHost()
{
  return host;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<DIOSTREAMICMPDATAGRAM*>* DIOSTREAMICMP::GetDatagramsVector()
* @brief      Get datagrams vector
* @ingroup    DATAIO
*
* @return     XVECTOR<DIOSTREAMICMPDATAGRAM*>* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<DIOSTREAMICMPDATAGRAM*>* DIOSTREAMICMP::GetDatagramsVector()
{
  return &datagrams;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMICMP::DeleteAllDatagrams()
* @brief      Delete all datagrams
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMICMP::DeleteAllDatagrams()
{
  if(datagrams.IsEmpty()) return false;

  if(datagramsmutex)  datagramsmutex->Lock();

  datagrams.DeleteContents();

  datagrams.DeleteAll();

  if(datagramsmutex)  datagramsmutex->UnLock();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMICMP::AddDatagram(bool istosend, XCHAR* address, XBYTE* data, XDWORD size)
* @brief      Add datagram
* @ingroup    DATAIO
*
* @param[in]  istosend :
* @param[in]  address :
* @param[in]  data :
* @param[in]  size :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMICMP::AddDatagram(bool istosend, XCHAR* address, XBYTE* data, XDWORD size)
{
  if(!address) return false;
  if(!data)    return false;

  if(datagramsmutex)  datagramsmutex->Lock();

  DIOSTREAMICMPDATAGRAM* datagram = new DIOSTREAMICMPDATAGRAM();
  if(datagram)
    {
      datagram->SetIsToSend(istosend);
      datagram->SetAddress(address);
      datagram->SetData(data,size);

      datagrams.Add(datagram);
    }

  if(datagramsmutex)  datagramsmutex->UnLock();

  return datagram?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMICMP::AddDatagram(bool istosend, XSTRING& address, XBYTE* data,XDWORD size)
* @brief      Add datagram
* @ingroup    DATAIO
*
* @param[in]  istosend :
* @param[in]  address :
* @param[in]  data :
* @param[in]  size :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMICMP::AddDatagram(bool istosend, XSTRING& address, XBYTE* data,XDWORD size)
{
  return AddDatagram(istosend, address.Get(), data, size);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMICMPDATAGRAM* DIOSTREAMICMP::GetDatagram(int index)
* @brief      Get datagram
* @ingroup    DATAIO
*
* @param[in]  index :
*
* @return     DIOSTREAMICMPDATAGRAM* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMICMPDATAGRAM* DIOSTREAMICMP::GetDatagram(int index)
{
  return (DIOSTREAMICMPDATAGRAM*)datagrams.Get(index);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOSTREAMICMP::GetFirstDatagram(bool tosend)
* @brief      Get first datagram
* @ingroup    DATAIO
*
* @param[in]  tosend :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMICMP::GetFirstDatagram(bool tosend)
{
  int index = DIOSTREAMICMP_NOTFOUND;

  if(datagramsmutex)  datagramsmutex->Lock();

  for(int c=0;c<(int)datagrams.GetSize();c++)
    {
      DIOSTREAMICMPDATAGRAM* datagram = (DIOSTREAMICMPDATAGRAM*)datagrams.Get(c);
      if(datagram)
        {
          if(datagram->IsToSend() == tosend)
            {
              index = c;
              break;
            }
        }
    }

  if(datagramsmutex)  datagramsmutex->UnLock();

  return index;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMICMP::DeleteDatagram(int index)
* @brief      Delete datagram
* @ingroup    DATAIO
*
* @param[in]  index :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMICMP::DeleteDatagram(int index)
{
  if(datagramsmutex)  datagramsmutex->Lock();

  DIOSTREAMICMPDATAGRAM* datagram = (DIOSTREAMICMPDATAGRAM*)GetDatagram(index);
  if(datagram)
    {
      datagrams.DeleteIndex(index);
      delete datagram;
    }

  if(datagramsmutex)  datagramsmutex->UnLock();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMICMP::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMICMP::Clean()
{
  config          = NULL;
  host            = NULL;
  datagramsmutex  = NULL;
}


#pragma endregion


#pragma endregion

