/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOI2C6AxisTrackingLSM303DLHC.cpp
*
* @class      DIOI2C6AXISTRACKINGLSM303DLHC
* @brief      Data Input/Output I2C 6 axis tracking LSM303DLHC
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

#include <math.h>

#include "XFactory.h"
#include "XBuffer.h"
#include "XTrace.h"

#include "DIOFactory.h"
#include "DIOStreamI2CConfig.h"
#include "DIOStreamI2C.h"

#include "DIOI2C6AxisTrackingLSM303DLHC.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/
/*  DIOI2CLSM303DLHCVECTOR                                                                                            */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOI2CLSM303DLHCVECTOR::DIOI2CLSM303DLHCVECTOR()
* @brief      Constructor
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOI2CLSM303DLHCVECTOR::DIOI2CLSM303DLHCVECTOR()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOI2CLSM303DLHCVECTOR::~DIOI2CLSM303DLHCVECTOR()
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
DIOI2CLSM303DLHCVECTOR::~DIOI2CLSM303DLHCVECTOR()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOI2CLSM303DLHCVECTOR::Cross(const DIOI2CLSM303DLHCVECTOR& a, const DIOI2CLSM303DLHCVECTOR& b, DIOI2CLSM303DLHCVECTOR& out)
* @brief      Cross
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  DIOI2CLSM303DLHCVECTOR& a :
* @param[in]  DIOI2CLSM303DLHCVECTOR& b :
* @param[in]  out :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void DIOI2CLSM303DLHCVECTOR::Cross(const DIOI2CLSM303DLHCVECTOR& a, const DIOI2CLSM303DLHCVECTOR& b, DIOI2CLSM303DLHCVECTOR& out)
{
  out.x = a.y * b.z - a.z * b.y;
  out.y = a.z * b.x - a.x * b.z;
  out.z = a.x * b.y - a.y * b.x;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         float DIOI2CLSM303DLHCVECTOR::Dot(const DIOI2CLSM303DLHCVECTOR& a, const DIOI2CLSM303DLHCVECTOR& b)
* @brief      Dot
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  DIOI2CLSM303DLHCVECTOR& a :
* @param[in]  DIOI2CLSM303DLHCVECTOR& b :
*
* @return     float :
*
*---------------------------------------------------------------------------------------------------------------------*/
float DIOI2CLSM303DLHCVECTOR::Dot(const DIOI2CLSM303DLHCVECTOR& a, const DIOI2CLSM303DLHCVECTOR& b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOI2CLSM303DLHCVECTOR::Normalize(DIOI2CLSM303DLHCVECTOR& a)
* @brief      Normalize
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  a :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void DIOI2CLSM303DLHCVECTOR::Normalize(DIOI2CLSM303DLHCVECTOR& a)
{
  float mag = (float)sqrt(Dot(a,a));

  a.x /= float(mag);
  a.y /= float(mag);
  a.z /= float(mag);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOI2CLSM303DLHCVECTOR::Clean()
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
void DIOI2CLSM303DLHCVECTOR::Clean()
{
  x = 0;
  y = 0;
  z = 0;
}


/*--------------------------------------------------------------------------------------------------------------------*/
/*  DIOI2CLSM303DLHC                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOI2CLSM303DLHC::DIOI2CLSM303DLHC() : DIODEVICEI2C()
* @brief      Constructor
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOI2CLSM303DLHC::DIOI2CLSM303DLHC() : DIODEVICEI2C()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOI2CLSM303DLHC::~DIOI2CLSM303DLHC()
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
DIOI2CLSM303DLHC::~DIOI2CLSM303DLHC()
{
  End();

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOI2CLSM303DLHC::End()
* @brief      End
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOI2CLSM303DLHC::End()
{
  if(!DIODEVICEI2C::End()) return false;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOI2CLSM303DLHC::IniDevice()
* @brief      IniDevice
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOI2CLSM303DLHC::IniDevice()
{ 
  if(!DIODEVICEI2C::IniDevice()) return false;

  if(diostream->Open())
    {
      if(diostream->WaitToConnected(timeout))
        {
          return DIODEVICE::Ini();
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOI2CLSM303DLHC::ReadRegister(XBYTE reg, XBYTE& value)
* @brief      ReadRegister
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  reg :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOI2CLSM303DLHC::ReadRegister(XBYTE reg, XBYTE& value)
{
  if(!diostream)          return false;
  if(!IsInitialized())    return false;

  bool  status;

  status = diostream->Write(&reg, 1)?true:false;
  if(status) status = diostream->WaitToFlushOutXBuffer(timeout);

  if(!status) return false;

  status = diostream->WaitToFilledReadingBuffer(1, timeout);
  if(status) status = diostream->Read(&value, 1)?true:false;

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOI2CLSM303DLHC::ReadRegister(XBYTE reg, XWORD& value)
* @brief      ReadRegister
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  reg :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOI2CLSM303DLHC::ReadRegister(XBYTE reg, XWORD& value)
{
  if(!diostream)  return false;
  if(!IsInitialized())  return false;

  bool  status;

  status = diostream->Write(&reg, 1)?true:false;
  if(status) status = diostream->WaitToFlushOutXBuffer(timeout);

  if(!status) return false;

  status = diostream->WaitToFilledReadingBuffer(2, timeout);
  if(status) status = diostream->Read((XBYTE*)&value, 2)?true:false;

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOI2CLSM303DLHC::WriteRegister(XBYTE reg, XBYTE value)
* @brief      WriteRegister
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  reg :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOI2CLSM303DLHC::WriteRegister(XBYTE reg, XBYTE value)
{
  if(!diostream)  return false;
  if(!IsInitialized())  return false;

  XBUFFER xbuffer;

  xbuffer.Add((XBYTE)reg);
  xbuffer.Add((XBYTE)value);

  bool status = diostream->Write(xbuffer)?true:false;
  if(status) status = diostream->WaitToFlushOutXBuffer(timeout);

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOI2CLSM303DLHC::Clean()
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
void DIOI2CLSM303DLHC::Clean()
{
  
}




/*--------------------------------------------------------------------------------------------------------------------*/
/*  DIOI2C3DCOMPASSLSM303DLHC                                                                                         */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOI2C3DCOMPASSLSM303DLHC::DIOI2C3DCOMPASSLSM303DLHC()
* @brief      Constructor
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOI2C3DCOMPASSLSM303DLHC::DIOI2C3DCOMPASSLSM303DLHC()
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOI2C3DCOMPASSLSM303DLHC::~DIOI2C3DCOMPASSLSM303DLHC()
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
DIOI2C3DCOMPASSLSM303DLHC::~DIOI2C3DCOMPASSLSM303DLHC()

{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOI2C3DACCELEROMETERLSM303DLHC::Configure()
* @brief      Configure
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOI2C3DACCELEROMETERLSM303DLHC::Configure()
{
  if(!IsInitialized()) return false;

  XBYTE value;

  value  = 0x00;
  value |= 0x01 << 5;     // Normal mode
  value |= 0x07;          // X/Y/Z axis enable.
  if(!WriteRegister(DIOI2CLSM303DLHC_CTRL_REG1_A , value))    return false;
  value  = 0x00;
  ReadRegister(DIOI2CLSM303DLHC_CTRL_REG1_A, value);

  value  = 0x00;
  value |= 0x01 << 6;     // 1: data MSB @ lower address
  value |= 0x01 << 4;     // +/- 4g
  if(!WriteRegister(DIOI2CLSM303DLHC_CTRL_REG4_A, value))   return false;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOI2C3DACCELEROMETERLSM303DLHC::Read(XWORD& a_x, XWORD& a_y, XWORD& a_z)
* @brief      Read
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  a_x :
* @param[in]  a_y :
* @param[in]  a_z :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOI2C3DACCELEROMETERLSM303DLHC::Read(XWORD& a_x, XWORD& a_y, XWORD& a_z)
{
  if(!IsInitialized()) return false;

  a_x = 0;
  a_y = 0;
  a_z = 0;

  if(!ReadRegister(DIOI2CLSM303DLHC_OUT_X_A, a_x)) return false;
  if(!ReadRegister(DIOI2CLSM303DLHC_OUT_Y_A, a_y)) return false;
  if(!ReadRegister(DIOI2CLSM303DLHC_OUT_Z_A, a_z)) return false;

  return true;
}




/*--------------------------------------------------------------------------------------------------------------------*/
/* DIOI2C3DACCELEROMETERLSM303DLHC                                                                                    */
/*--------------------------------------------------------------------------------------------------------------------*/




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOI2C3DACCELEROMETERLSM303DLHC::DIOI2C3DACCELEROMETERLSM303DLHC()
* @brief      Constructor
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOI2C3DACCELEROMETERLSM303DLHC::DIOI2C3DACCELEROMETERLSM303DLHC()
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOI2C3DACCELEROMETERLSM303DLHC::~DIOI2C3DACCELEROMETERLSM303DLHC()
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
DIOI2C3DACCELEROMETERLSM303DLHC::~DIOI2C3DACCELEROMETERLSM303DLHC()
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOI2C3DCOMPASSLSM303DLHC::Configure()
* @brief      Configure
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOI2C3DCOMPASSLSM303DLHC::Configure()
{
  if(!IsInitialized()) return false;

  XBYTE value;

  value  = 0x00;
  value |= 0x04 << 2;     // Minimum data output rate = 15Hz
  if(!WriteRegister(DIOI2CLSM303DLHC_CRA_REG_M, value))           return false;

  value  = 0x00;
//value |= 0x01 << 5;     // +-1.3Gauss
  value |= 0x07 << 5;     // +-8.1Gauss
  if(!WriteRegister(DIOI2CLSM303DLHC_CRB_REG_M, value))           return false;

  value = 0x00;           // Continuous-conversion mode
  if(!WriteRegister(DIOI2CLSM303DLHC_MR_REG_M, value))            return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOI2C3DCOMPASSLSM303DLHC::Read(XWORD& c_x, XWORD& c_y, XWORD& c_z)
* @brief      Read
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  c_x :
* @param[in]  c_y :
* @param[in]  c_z :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOI2C3DCOMPASSLSM303DLHC::Read(XWORD& c_x, XWORD& c_y, XWORD& c_z)
{
  if(!IsInitialized()) return false;

  c_x = 0;
  c_y = 0;
  c_z = 0;

  if(!ReadRegister(DIOI2CLSM303DLHC_OUT_X_M, c_x)) return false;
  if(!ReadRegister(DIOI2CLSM303DLHC_OUT_Y_M, c_y)) return false;
  if(!ReadRegister(DIOI2CLSM303DLHC_OUT_Z_M, c_z)) return false;

  return true;
}



/*--------------------------------------------------------------------------------------------------------------------*/
/* DIOI2C3DACCELEROMETERLSM303DLHC                                                                                    */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOI2C6AXISTRACKINGLSM303DLHC::DIOI2C6AXISTRACKINGLSM303DLHC()
* @brief      Constructor
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOI2C6AXISTRACKINGLSM303DLHC::DIOI2C6AXISTRACKINGLSM303DLHC()
{
  Clean();

  accelerometer = new DIOI2C3DACCELEROMETERLSM303DLHC();
  compass       = new DIOI2C3DCOMPASSLSM303DLHC();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOI2C6AXISTRACKINGLSM303DLHC::~DIOI2C6AXISTRACKINGLSM303DLHC()
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
DIOI2C6AXISTRACKINGLSM303DLHC::~DIOI2C6AXISTRACKINGLSM303DLHC()
{
  delete accelerometer;
  delete compass;

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOI2C6AXISTRACKINGLSM303DLHC::Ini(XCHAR* localdevicename, int remotedeviceaddressaccelerometer, int remotedeviceaddresscompass, int timeout)
* @brief      Ini
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  localdevicename :
* @param[in]  remotedeviceaddressaccelerometer :
* @param[in]  remotedeviceaddresscompass :
* @param[in]  timeout :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOI2C6AXISTRACKINGLSM303DLHC::Ini(XCHAR* localdevicename, int remotedeviceaddressaccelerometer, int remotedeviceaddresscompass, int timeout)
{
  if(!accelerometer)  return false;
  if(!compass)        return false;

  if(!accelerometer->Ini(localdevicename, remotedeviceaddressaccelerometer, timeout)) return false;
  if(!compass->Ini(localdevicename, remotedeviceaddresscompass, timeout))             return false;

  if(!accelerometer->Configure()) return false;
  if(!compass->Configure())       return false;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOI2C6AXISTRACKINGLSM303DLHC::Ini(int port, int remotedeviceaddressaccelerometer, int remotedeviceaddresscompass, int timeout)
* @brief      Ini
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  port :
* @param[in]  remotedeviceaddressaccelerometer :
* @param[in]  remotedeviceaddresscompass :
* @param[in]  timeout :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOI2C6AXISTRACKINGLSM303DLHC::Ini(int port,  int remotedeviceaddressaccelerometer, int remotedeviceaddresscompass, int timeout)
{
  if(!accelerometer)  return false;
  if(!compass)        return false;


  filter_az       = 6000;

  if(!accelerometer->Ini(port, remotedeviceaddressaccelerometer, timeout)) return false;
  if(!compass->Ini(port, remotedeviceaddresscompass, timeout))             return false;

  if(!accelerometer->Configure()) return false;
  if(!compass->Configure())       return false;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOI2C6AXISTRACKINGLSM303DLHC::SetOffset(float x, float y, float z)
* @brief      SetOffset
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  x :
* @param[in]  y :
* @param[in]  z :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void DIOI2C6AXISTRACKINGLSM303DLHC::SetOffset(float x, float y, float z)
{
  offset_x = x;
  offset_y = y;
  offset_z = z;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOI2C6AXISTRACKINGLSM303DLHC::SetScale(float x, float y, float z)
* @brief      SetScale
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  x :
* @param[in]  y :
* @param[in]  z :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void DIOI2C6AXISTRACKINGLSM303DLHC::SetScale(float x, float y, float z)
{
  scale_x = x;
  scale_y = y;
  scale_z = z;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOI2C6AXISTRACKINGLSM303DLHC::Read(XWORD& a_x, XWORD& a_y, XWORD& a_z, XWORD& c_x, XWORD& c_y, XWORD& c_z)
* @brief      Read
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  a_x :
* @param[in]  a_y :
* @param[in]  a_z :
* @param[in]  c_x :
* @param[in]  c_y :
* @param[in]  c_z :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOI2C6AXISTRACKINGLSM303DLHC::Read(XWORD& a_x, XWORD& a_y, XWORD& a_z, XWORD& c_x, XWORD& c_y, XWORD& c_z)
{
  if(!accelerometer)  return false;
  if(!compass)        return false;

  bool status;

  status = accelerometer->Read(a_x, a_y, a_z);
  if(status) status = compass->Read(c_x, c_y, c_z);

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOI2C6AXISTRACKINGLSM303DLHC::Read(DIOI2CLSM303DLHCVECTOR& a, DIOI2CLSM303DLHCVECTOR& c)
* @brief      Read
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  a :
* @param[in]  c :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOI2C6AXISTRACKINGLSM303DLHC::Read(DIOI2CLSM303DLHCVECTOR& a, DIOI2CLSM303DLHCVECTOR& c)
{
  if(!accelerometer)  return false;
  if(!compass)        return false;

  XWORD ax = 0;
  XWORD ay = 0;
  XWORD az = 0;
  XWORD cx = 0;
  XWORD cy = 0;
  XWORD cz = 0;

  if(!Read(ax, ay, az, cx, cy, cz)) return false;

  // Perform simple lowpass filtering Intended to stabilize heading despite device vibration such as on a UGV
  filter_ax += ax - (filter_ax >> DIOI2CLSM303DLHC_FILTERSHIFT);
  filter_ay += ay - (filter_ay >> DIOI2CLSM303DLHC_FILTERSHIFT);
  filter_az += az - (filter_az >> DIOI2CLSM303DLHC_FILTERSHIFT);

  a.x = (float)(filter_ax >> DIOI2CLSM303DLHC_FILTERSHIFT);
  a.y = (float)(filter_ay >> DIOI2CLSM303DLHC_FILTERSHIFT);
  a.z = (float)(filter_az >> DIOI2CLSM303DLHC_FILTERSHIFT);

  // offset and scale
  c.x = (cx + offset_x) * scale_x;
  c.y = (cy + offset_y) * scale_y;
  c.z = (cz + offset_z) * scale_z;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         float DIOI2C6AXISTRACKINGLSM303DLHC::Heading()
* @brief      Heading
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     float :
*
*---------------------------------------------------------------------------------------------------------------------*/
float DIOI2C6AXISTRACKINGLSM303DLHC::Heading()
{
  DIOI2CLSM303DLHCVECTOR vector;

  vector.x =  0;
  vector.y = -1;
  vector.z =  0;

  return Heading(vector);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         float DIOI2C6AXISTRACKINGLSM303DLHC::Heading(DIOI2CLSM303DLHCVECTOR& from)
* @brief      Heading
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  from :
*
* @return     float :
*
*---------------------------------------------------------------------------------------------------------------------*/
float DIOI2C6AXISTRACKINGLSM303DLHC::Heading(DIOI2CLSM303DLHCVECTOR& from)
{
  DIOI2CLSM303DLHCVECTOR a;
  DIOI2CLSM303DLHCVECTOR c;

  if(!Read(a, c)) return 0.0f;

  DIOI2CLSM303DLHCVECTOR temp_a;

  temp_a.x = a.x;
  temp_a.y = a.y;
  temp_a.z = a.z;

  // normalize
  DIOI2CLSM303DLHCVECTOR::Normalize(temp_a);

  // compute E and N
  DIOI2CLSM303DLHCVECTOR e;
  DIOI2CLSM303DLHCVECTOR n;

  DIOI2CLSM303DLHCVECTOR::Cross(c ,temp_a, e);

  DIOI2CLSM303DLHCVECTOR::Normalize(e);

  DIOI2CLSM303DLHCVECTOR::Cross(temp_a, e , n);

  // compute heading
  float heading = (float)atan2( DIOI2CLSM303DLHCVECTOR::Dot(e , from), DIOI2CLSM303DLHCVECTOR::Dot(n , from)) * (180/PI);
  if (heading < 0) heading += 360;

  return float(heading);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOI2C6AXISTRACKINGLSM303DLHC::End()
* @brief      End
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOI2C6AXISTRACKINGLSM303DLHC::End()
{
  if(!accelerometer)  return false;
  if(!compass)        return false;

  accelerometer->End();
  compass->End();

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOI2C6AXISTRACKINGLSM303DLHC::Clean()
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
void DIOI2C6AXISTRACKINGLSM303DLHC::Clean()
{
  accelerometer   = NULL;
  compass         = NULL;

  offset_x        = 0.0f;
  offset_y        = 0.0f;
  offset_z        = 0.0f;

  scale_x         = 0.0f;
  scale_y         = 0.0f;
  scale_z         = 0.0f;

  filter_ax       = 0;
  filter_ay       = 0;
  filter_az       = 0;
}