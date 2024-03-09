/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       HashCRC16.cpp
* 
* @class      HASHCRC16
* @brief      Hash CRC16 class
* @ingroup    CIPHER
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

#include "HashCRC16.h"

#include "XBuffer.h"
#include "XFactory.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         HASHCRC16::HASHCRC16()
* @brief      Constructor
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
HASHCRC16::HASHCRC16() : HASH()
{
  Clean();

  type = HASHTYPE_CRC16;

  resulttmp = new XBUFFER();

  //XWORD value = 0xffff;
  XWORD value = 0x0000;
  resulttmp->Add((XWORD)value);

  Ini();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         HASHCRC16::~HASHCRC16()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
HASHCRC16::~HASHCRC16()
{
  if(resulttmp)
    {
      delete resulttmp;
      resulttmp = NULL;
    }

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool HASHCRC16::Do(XBYTE* input, XQWORD size)
* @brief      Do
* @ingroup    CIPHER
* 
* @param[in]  input : 
* @param[in]  size : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool HASHCRC16::Do(XBYTE* input, XQWORD size)
{
  if(!size) return false;

  XWORD value = 0;

  resulttmp->Extract(value);

  for(XDWORD c=0; c<size; c++)
    {
      value = Update(value, (*input));
      input++;
    }

  resulttmp->Add((XWORD)value);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool HASHCRC16::ResetResult()
* @brief      ResetResult
* @ingroup    CIPHER
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool HASHCRC16::ResetResult()
{
  HASH::ResetResult();

  resulttmp->Delete();
  //XWORD value = 0xffff; // CRC16-ANSI has an initial value of 0
  XWORD value = 0x0000;
  resulttmp->Add((XWORD)value);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int HASHCRC16::GetDefaultSize()
* @brief      GetDefaultSize
* @ingroup    CIPHER
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int HASHCRC16::GetDefaultSize()
{
  return sizeof(XWORD);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBUFFER* HASHCRC16::GetResult()
* @brief      GetResult
* @ingroup    CIPHER
* 
* @return     XBUFFER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* HASHCRC16::GetResult()
{
  GetResultCRC16();

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE* HASHCRC16::GetResult(XDWORD& resultsize)
* @brief      GetResult
* @ingroup    CIPHER
* 
* @param[in]  resultsize : 
* 
* @return     XBYTE* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE* HASHCRC16::GetResult(XDWORD& resultsize)
{
  GetResultCRC16();

  resultsize = result->GetSize();

  return result->Get();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD HASHCRC16::GetResultCRC16()
* @brief      GetResultCRC16
* @ingroup    CIPHER
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD HASHCRC16::GetResultCRC16()
{
  XWORD value = 0;

  result->Delete();

  resulttmp->Extract(value);

  //value ^= 0xffff; // CRC16-ANSI has an xor value of 0
  value ^= 0x0000;

  result->Add((XWORD)value);

  return value;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void HASHCRC16::Ini(void)
* @brief      Ini
* @ingroup    CIPHER
* 
* @param[in]  void : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void HASHCRC16::Ini(void)
{
  // precomputed CRC16
  // source: https://github.com/torvalds/linux/blob/master/lib/crc16.c
  XWORD tabletmp[256] =  {
                          0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
                          0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
                          0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
                          0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
                          0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
                          0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
                          0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
                          0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
                          0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
                          0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
                          0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
                          0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
                          0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
                          0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
                          0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
                          0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
                          0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
                          0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
                          0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
                          0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
                          0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
                          0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
                          0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
                          0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
                          0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
                          0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
                          0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
                          0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
                          0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
                          0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
                          0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
                          0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
                         };

  memcpy(table, tabletmp, 256*sizeof(XWORD));
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD HASHCRC16::Update(XWORD CRC16, XBYTE c)
* @brief      Update
* @ingroup    CIPHER
* 
* @param[in]  CRC16 : 
* @param[in]  c : 
* 
* @return     XWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD HASHCRC16::Update(XWORD CRC16, XBYTE c)
{
  return (CRC16 >> 8) ^ table[(CRC16 ^ c) & 0xff];
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void HASHCRC16::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
void HASHCRC16::Clean()
{
  resulttmp = NULL;
}


#pragma endregion