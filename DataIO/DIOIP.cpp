/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOIP.cpp
* 
* @class      DIOIP
* @brief      Data Input/Output IP class
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

#include "DIOIP.h"

#include <stdio.h>
#include <string.h>

#include "DIOURL.h"

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


#pragma region CLASS_DIOIPADDRESS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOIPADDRESS::DIOIPADDRESS()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOIPADDRESS::DIOIPADDRESS()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOIPADDRESS::~DIOIPADDRESS()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOIPADDRESS::~DIOIPADDRESS()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE* DIOIPADDRESS::Get()
* @brief      Get
* @ingroup    DATAIO
* 
* @return     XBYTE* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE* DIOIPADDRESS::Get()
{
  return address;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOIPADDRESS::GetXString(XSTRING& addressstring)
* @brief      Get X string
* @ingroup    DATAIO
* 
* @param[in]  addressstring : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOIPADDRESS::GetXString(XSTRING& addressstring)
{
  addressstring.Empty();

  if(IsEmpty()) return false;

  int _address[DIOIP_MAXIPADDRESS];
  
  for(int c=0; c<DIOIP_MAXIPADDRESS; c++)
    {
      _address[c] = address[c];
    }

  addressstring.Format(__L("%d.%d.%d.%d") , _address[0], _address[1], _address[2], _address[3]);

  return addressstring.IsEmpty()?false:true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOIPADDRESS::GetDIOURL(DIOURL* url)
* @brief      Get DIOURL
* @ingroup    DATAIO
* 
* @param[in]  url : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOIPADDRESS::GetDIOURL(DIOURL* url)
{
  if(!url) return false;

  url->Empty();

  int _address[DIOIP_MAXIPADDRESS];
  
  for(int c=0; c<DIOIP_MAXIPADDRESS; c++)
    {
      _address[c] = address[c];
    }

  url->Format(__L("%d.%d.%d.%d") , _address[0], _address[1], _address[2], _address[3]);

  return url->IsEmpty()?false:true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOIPADDRESS::IsEmpty()
* @brief      Is empty
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOIPADDRESS::IsEmpty()
{
  if((!address[0]) && (!address[1]) && (!address[2]) && (!address[3])) return true;

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOIPADDRESS::IsLocal()
* @brief      Is local
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOIPADDRESS::IsLocal()
{
  if(address[0]==10)  return true;
  if(address[0]==172)
    {
      if((address[1]>=16) && (address[1]<=31)) return true;
    }

  if((address[0]==192) && (address[1]==168)) return true;
  if((address[0]==169) && (address[1]==254)) return true;    // APIPA (Automatic Private Internet Protocol Addressing)

  if((address[0]==127) && (address[1]==0) && (address[2]==0) && (address[3]==1)) return true;

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOIPADDRESS::IsAPIPA()
* @brief      Is APIPA
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOIPADDRESS::IsAPIPA()
{
  if((address[0]==169) && (address[1]==254)) return true;    // APIPA (Automatic Private Internet Protocol Addressing)

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOIPADDRESS::Set(XBYTE* address)
* @brief      Set
* @ingroup    DATAIO
* 
* @param[in]  address : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOIPADDRESS::Set(XBYTE* address)
{
  for(XDWORD c=0;c< DIOIP_MAXIPADDRESS;c++)
    {
      this->address[c] = address[c];
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOIPADDRESS::Set(XBYTE byte1, XBYTE byte2, XBYTE byte3, XBYTE byte4)
* @brief      Set
* @ingroup    DATAIO
* 
* @param[in]  byte1 : 
* @param[in]  byte2 : 
* @param[in]  byte3 : 
* @param[in]  byte4 : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOIPADDRESS::Set(XBYTE byte1, XBYTE byte2, XBYTE byte3, XBYTE byte4)
{
  this->address[0] = byte1;
  this->address[1] = byte2;
  this->address[2] = byte3;
  this->address[3] = byte4;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOIPADDRESS::Set(XCHAR* addresschar)
* @brief      Set
* @ingroup    DATAIO
* 
* @param[in]  addresschar : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOIPADDRESS::Set(XCHAR* addresschar)
{
  XSTRING _addresschar;

  _addresschar = addresschar;

  return Set(_addresschar);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOIPADDRESS::Set(XSTRING& addressstring)
* @brief      Set
* @ingroup    DATAIO
* 
* @param[in]  addressstring : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOIPADDRESS::Set(XSTRING& addressstring)
{
  if(addressstring.IsEmpty()) return false;

  int   iip[DIOIP_MAXIPADDRESS] = { 0, 0, 0, 0 };
  XBYTE bip[DIOIP_MAXIPADDRESS] = { 0, 0, 0, 0 };

  if(addressstring.GetNCharacters(__C('.')) != 3) return false;

  for(XDWORD c=0; c<addressstring.GetSize(); c++)
    {
      if((addressstring.Get()[c]<__C('0')) && (addressstring.Get()[c]>__C('9')) && (addressstring.Get()[c]!= __C('.'))) return false;
    }

  addressstring.UnFormat(__L("%d.%d.%d.%d"), &iip[0], &iip[1], &iip[2], &iip[3]);

  for(int c=0; c<DIOIP_MAXIPADDRESS; c++)
    {
      bip[c] = (XBYTE)iip[c];
    }

  return Set(bip);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOIPADDRESS::Set(char* addresschar)
* @brief      Set
* @ingroup    DATAIO
* 
* @param[in]  addresschar : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOIPADDRESS::Set(char* addresschar)
{
  XSTRING _IPchar;

  _IPchar = addresschar;

  return Set(_IPchar);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOIPADDRESS::Compare(DIOIP& otheraddress)
* @brief      Compare
* @ingroup    DATAIO
* 
* @param[in]  otheraddress : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOIPADDRESS::Compare(DIOIP& otheraddress)
{
  for(int c=0; c<DIOIP_MAXIPADDRESS; c++)
    {
      if(address[c] != otheraddress.Get()[c]) return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOIPADDRESS::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOIPADDRESS::Clean()
{
  memset(address, 0, DIOIP_MAXIPADDRESS);
}


#pragma endregion


#pragma region CLASS_DIOIP


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOIP::DIOIP()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOIP::DIOIP()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOIP::~DIOIP()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOIP::~DIOIP()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOIPADDRESS* DIOIP::GetMask()
* @brief      Get mask
* @ingroup    DATAIO
* 
* @return     DIOIPADDRESS* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOIPADDRESS* DIOIP::GetMask()
{
  return &mask;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOIP::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOIP::Clean()
{

}


#pragma endregion


#pragma endregion
