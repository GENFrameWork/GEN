/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamUARTConfig.cpp
* 
* @class      DIOSTREAMUARTCONFIG
* @brief      Data Input/Output Stream UART config class
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

/*---- PRECOMPILATION INCLUDES ----------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOStreamUARTConfig.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMUARTCONFIG::DIOSTREAMUARTCONFIG()
* @brief      Constructor
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMUARTCONFIG::DIOSTREAMUARTCONFIG(): DIOSTREAMCONFIG()
{
  Clean();

  type = DIOSTREAMTYPE_UART;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMUARTCONFIG::~DIOSTREAMUARTCONFIG()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMUARTCONFIG::~DIOSTREAMUARTCONFIG()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOSTREAMUARTCONFIG::GetPort()
* @brief      GetPort
* @ingroup    DATAIO
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMUARTCONFIG::GetPort()
{
  return port;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMUARTCONFIG::SetPort(int port)
* @brief      SetPort
* @ingroup    DATAIO
*
* @param[in]  port :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMUARTCONFIG::SetPort(int port)
{
  this->port = port;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOSTREAMUARTCONFIG::GetLocalDeviceName()
* @brief      GetLocalDeviceName
* @ingroup    DATAIO
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOSTREAMUARTCONFIG::GetLocalDeviceName()
{
  return &devicename;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOSTREAMUARTCONFIG::GetBaudRate()
* @brief      GetBaudRate
* @ingroup    DATAIO
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOSTREAMUARTCONFIG::GetBaudRate()
{
  return baudrate;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMUARTCONFIG::SetBaudRate(int baudrate)
* @brief      SetBaudRate
* @ingroup    DATAIO
*
* @param[in]  baudrate :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMUARTCONFIG::SetBaudRate(int baudrate)
{
  this->baudrate = baudrate;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMUARTDATABITS DIOSTREAMUARTCONFIG::GetDataBits()
* @brief      GetDataBits
* @ingroup    DATAIO
*
* @return     DIOSTREAMUARTDATABITS :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMUARTDATABITS DIOSTREAMUARTCONFIG::GetDataBits()
{
  return databits;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMUARTCONFIG::SetDataBits(DIOSTREAMUARTDATABITS databits)
* @brief      SetDataBits
* @ingroup    DATAIO
*
* @param[in]  databits :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMUARTCONFIG::SetDataBits(DIOSTREAMUARTDATABITS databits)
{
  this->databits = databits;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMUARTPARITY DIOSTREAMUARTCONFIG::GetParity()
* @brief      GetParity
* @ingroup    DATAIO
*
* @return     DIOSTREAMUARTPARITY :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMUARTPARITY DIOSTREAMUARTCONFIG::GetParity()
{
  return parity;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMUARTCONFIG::SetParity(DIOSTREAMUARTPARITY parity)
* @brief      SetParity
* @ingroup    DATAIO
*
* @param[in]  parity :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMUARTCONFIG::SetParity(DIOSTREAMUARTPARITY parity)
{
  this->parity = parity;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMUARTSTOPBITS DIOSTREAMUARTCONFIG::GetStopBits()
* @brief      GetStopBits
* @ingroup    DATAIO
*
* @return     DIOSTREAMUARTSTOPBITS :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMUARTSTOPBITS DIOSTREAMUARTCONFIG::GetStopBits()
{
  return stopbits;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMUARTCONFIG::SetStopBits(DIOSTREAMUARTSTOPBITS stopbits)
* @brief      SetStopBits
* @ingroup    DATAIO
*
* @param[in]  stopbits :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMUARTCONFIG::SetStopBits(DIOSTREAMUARTSTOPBITS stopbits)
{
  this->stopbits = stopbits;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMUARTFLOWCONTROL DIOSTREAMUARTCONFIG::GetFlowControl()
* @brief      GetFlowControl
* @ingroup    DATAIO
*
* @return     DIOSTREAMUARTFLOWCONTROL :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMUARTFLOWCONTROL DIOSTREAMUARTCONFIG::GetFlowControl()
{
  return flowcontrol;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMUARTCONFIG::DIOSTREAMUARTCONFIG::SetFlowControl(DIOSTREAMUARTFLOWCONTROL flowcontrol)
* @brief      DIOSTREAMUARTCONFIG::SetFlowControl
* @ingroup    DATAIO
*
* @param[in]  flowcontrol :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMUARTCONFIG::SetFlowControl(DIOSTREAMUARTFLOWCONTROL flowcontrol)
{
  this->flowcontrol = flowcontrol;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMUARTCONFIG::GetToString(XSTRING* string)
* @brief      GetToString
* @ingroup    DATAIO
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMUARTCONFIG::GetToString(XSTRING* string)
{
  if(!string) return false;

  XSTRING xstringtmp;

  string->Empty();

  if(!GetLocalDeviceName()->IsEmpty())
    {
      string->Add(GetLocalDeviceName()->Get());
      string->Add(__L(","));
    }
   else
    {
      if(!GetPort()) return false;
      string->Format(__L("%d,"), GetPort());
    }

  if(!GetBaudRate()) return false;
  xstringtmp.Format(__L("%d,"), GetBaudRate());
  string->Add(xstringtmp);

  switch(GetDataBits())
    {
      case DIOSTREAMUARTDATABIT_5       : string->Add(__L("5,"));     break;
      case DIOSTREAMUARTDATABIT_6       : string->Add(__L("6,"));     break;
      case DIOSTREAMUARTDATABIT_7       : string->Add(__L("7,"));     break;
      case DIOSTREAMUARTDATABIT_8       : string->Add(__L("8,"));     break;
                        default         : return false;
    }

  switch(GetParity())
    {
      case DIOSTREAMUARTPARITY_NONE     : string->Add(__L("N,"));   break;
      case DIOSTREAMUARTPARITY_ODD      : string->Add(__L("O,"));   break;
      case DIOSTREAMUARTPARITY_EVEN     : string->Add(__L("E,"));   break;
      case DIOSTREAMUARTPARITY_MARK     : string->Add(__L("M,"));   break;
      case DIOSTREAMUARTPARITY_SPACE    : string->Add(__L("S,"));   break;
                             default    : return false;
    }

  switch(GetFlowControl())
    {
      case DIOSTREAMUARTFLOWCONTROL_NONE : string->Add(DIOSTREAMUART_FLOWCONTROL_STR_NONE);   break;
      case DIOSTREAMUARTFLOWCONTROL_HARD : string->Add(DIOSTREAMUART_FLOWCONTROL_STR_SOFT);   break;
      case DIOSTREAMUARTFLOWCONTROL_SOFT : string->Add(DIOSTREAMUART_FLOWCONTROL_STR_HARD);   break;
                                default  : return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMUARTCONFIG::GetToString(XSTRING& string)
* @brief      GetToString
* @ingroup    DATAIO
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMUARTCONFIG::GetToString(XSTRING& string)
{
  return GetToString(&string);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMUARTCONFIG::SetFromString(XCHAR* string)
* @brief      SetFromString
* @ingroup    DATAIO
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMUARTCONFIG::SetFromString(XCHAR* string)
{
  if(!string) return false;

  XSTRING   _string  = string;
  XSTRING   devicename;
  int       baudrate;
  int       databits;
  XCHAR     parity;
  XSTRING   stopbits;
  XSTRING   flowcontrol;

  devicename.AdjustSize(_MAXSTR);
  stopbits.AdjustSize(_MAXSTR);
  flowcontrol.AdjustSize(_MAXSTR);

  _string.UnFormat(__L("%s,%d,%d,%c,%s,%s"), devicename.Get(), &baudrate, &databits, &parity, stopbits.Get(), flowcontrol.Get());

  devicename.AdjustSize();
  stopbits.AdjustSize();
  flowcontrol.AdjustSize();


  if(devicename.HaveOnlyNumbers())
    {
      SetPort(devicename.ConvertToInt());
      if(!GetPort()) return false;
    }
   else GetLocalDeviceName()->Set(devicename);

  if(!baudrate) return false;
  SetBaudRate(baudrate);

  switch(databits)
    {
      case 5    : SetDataBits(DIOSTREAMUARTDATABIT_5);  break;
      case 6    : SetDataBits(DIOSTREAMUARTDATABIT_6);  break;
      case 7    : SetDataBits(DIOSTREAMUARTDATABIT_7);  break;
      case 8    : SetDataBits(DIOSTREAMUARTDATABIT_8);  break;
        default : return false;
    }

  switch(parity)
    {
      case 'N'  : SetParity(DIOSTREAMUARTPARITY_NONE);  break;
      case 'O'  : SetParity(DIOSTREAMUARTPARITY_ODD);   break;
      case 'E'  : SetParity(DIOSTREAMUARTPARITY_EVEN);  break;
      case 'M'  : SetParity(DIOSTREAMUARTPARITY_MARK);  break;
      case 'S'  : SetParity(DIOSTREAMUARTPARITY_SPACE); break;
       default  : return false;
    }

  if(!stopbits.Compare(__L("1")))
    {
      SetStopBits(DIOSTREAMUARTSTOPBITS_ONE);
    }
   else
    {
      if(!stopbits.Compare(__L("1.5")))
        {
          SetStopBits(DIOSTREAMUARTSTOPBITS_ONEANDAHALF);
        }
       else
        {
          if(!stopbits.Compare(__L("2")))
            {
              SetStopBits(DIOSTREAMUARTSTOPBITS_TWO);

            } else return false;
        }
    }

  if(!flowcontrol.Compare(DIOSTREAMUART_FLOWCONTROL_STR_NONE, true))
    {
      SetFlowControl(DIOSTREAMUARTFLOWCONTROL_NONE);
    }
   else
    {
      if(!stopbits.Compare(DIOSTREAMUART_FLOWCONTROL_STR_SOFT) || !stopbits.Compare(__L("SOFT")))
        {
          SetFlowControl(DIOSTREAMUARTFLOWCONTROL_SOFT);
        }
       else
        {
          if(!stopbits.Compare(DIOSTREAMUART_FLOWCONTROL_STR_HARD) || !stopbits.Compare(__L("HARD")))
            {
              SetFlowControl(DIOSTREAMUARTFLOWCONTROL_HARD);

            } else return false;
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMUARTCONFIG::SetFromString(XSTRING* string)
* @brief      SetFromString
* @ingroup    DATAIO
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMUARTCONFIG::SetFromString(XSTRING* string)
{
  if(!string) return false;
  return SetFromString(string->Get());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMUARTCONFIG::SetFromString(XSTRING& string)
* @brief      SetFromString
* @ingroup    DATAIO
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMUARTCONFIG::SetFromString(XSTRING& string)
{
  return SetFromString(&string);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMUARTCONFIG::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMUARTCONFIG::Clean()
{
  type          = DIOSTREAMTYPE_UART;

  port          = 0;
  baudrate      = 0;
  databits      = DIOSTREAMUARTDATABIT_8;
  parity        = DIOSTREAMUARTPARITY_NONE;
  stopbits      = DIOSTREAMUARTSTOPBITS_ONE;
  flowcontrol   = DIOSTREAMUARTFLOWCONTROL_NONE;
}


#pragma endregion

