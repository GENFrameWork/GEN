/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       CipherCertificateX509.cpp
* 
* @class      CIPHERCERTIFICATEX509
* @brief      Cipher Certificate X509 class
* @ingroup    CIPHER
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

#include "CipherCertificateX509.h"

#include <string.h>

#include "XFactory.h"
#include "XDateTime.h"
#include "XTrace.h"

#include "CipherKeyPublicRSA.h"
#include "CipherKeyECDSA.h"
#include "CipherRSA.h"
#include "CipherECDSA.h"
#include "HashSHA2.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


class CIPHERCERTIFICATEX509_DERITEM
{
  public:

    XBYTE         tag;
    const XBYTE*  data;
    XDWORD        size;
    const XBYTE*  encoded;
    XDWORD        encodedsize;
};


class CIPHERCERTIFICATEX509_DERREADER
{
  public:

                    CIPHERCERTIFICATEX509_DERREADER   (const XBYTE* data, XDWORD size)
                    {
                      this->data = data;
                      this->size = size;
                      position   = 0;
                    }

    bool            Read                              (CIPHERCERTIFICATEX509_DERITEM& item)
                    {
                      XDWORD length;
                      XDWORD headersize;

                      if(!data || (position >= size))
                        {
                          return false;
                        }

                      item.encoded = &data[position];
                      item.tag     = data[position++];

                      if((item.tag & 0x1F) == 0x1F)
                        {
                          return false;
                        }

                      if(position >= size)
                        {
                          return false;
                        }

                      length = data[position++];
                      if(length & 0x80)
                        {
                          XDWORD sizesize = length & 0x7F;

                          if(!sizesize || (sizesize > 4) || ((position + sizesize) > size) || !data[position])
                            {
                              return false;
                            }

                          length = 0;
                          for(XDWORD c=0; c<sizesize; c++)
                            {
                              length = (length << 8) | data[position++];
                            }

                          if(length < 128)
                            {
                              return false;
                            }
                        }

                      if(length > (size - position))
                        {
                          return false;
                        }

                      headersize       = (XDWORD)(&data[position] - item.encoded);
                      item.data         = &data[position];
                      item.size         = length;
                      item.encodedsize  = headersize + length;
                      position         += length;

                      return true;
                    }

    bool            IsEnd                             ()
                    {
                      return (position == size);
                    }

  private:

    const XBYTE*    data;
    XDWORD          size;
    XDWORD          position;
};


static bool CIPHERCERTIFICATEX509_DER_OIDCompare(CIPHERCERTIFICATEX509_DERITEM& item, const XBYTE* OID, XDWORD OIDsize)
{
  if((item.tag != 0x06) || !OID || (item.size != OIDsize))
    {
      return false;
    }

  return !memcmp(item.data, OID, OIDsize);
}


static bool CIPHERCERTIFICATEX509_DER_OIDToString(CIPHERCERTIFICATEX509_DERITEM& item, XSTRING& OID)
{
  XQWORD value;
  XQWORD second;
  XDWORD index;
  XDWORD first;

  if((item.tag != 0x06) || !item.size)
    {
      return false;
    }

  value = 0;
  index = 0;

  if(item.data[index] == 0x80)
    {
      return false;
    }

  do
    {
      if((value >> 57) || (index >= item.size))
        {
          return false;
        }

      value = (value << 7) | (item.data[index] & 0x7F);
    }
  while(item.data[index++] & 0x80);

  if(value < 40)
    {
      first  = 0;
      second = value;
    }
  else if(value < 80)
    {
      first  = 1;
      second = value - 40;
    }
   else
    {
      first  = 2;
      second = value - 80;
    }

  OID.Format(__L("%d.%llu"), first, second);

  while(index < item.size)
    {
      value = 0;

      if(item.data[index] == 0x80)
        {
          return false;
        }

      do
        {
          if((value >> 57) || (index >= item.size))
            {
              return false;
            }

          value = (value << 7) | (item.data[index] & 0x7F);
        }
      while(item.data[index++] & 0x80);

      OID.AddFormat(__L(".%llu"), value);
    }

  return true;
}


static bool CIPHERCERTIFICATEX509_DER_Algorithm(CIPHERCERTIFICATEX509_DERITEM& item, XSTRING& OID,
                                                CIPHERCERTIFICATEX509_DERITEM* algorithmparameters = NULL)
{
  CIPHERCERTIFICATEX509_DERREADER reader(item.data, item.size);
  CIPHERCERTIFICATEX509_DERITEM   OIDitem;
  CIPHERCERTIFICATEX509_DERITEM   parameters;

  if(algorithmparameters)
    {
      memset(algorithmparameters, 0, sizeof(CIPHERCERTIFICATEX509_DERITEM));
    }

  if((item.tag != 0x30) || !reader.Read(OIDitem) || !CIPHERCERTIFICATEX509_DER_OIDToString(OIDitem, OID))
    {
      return false;
    }

  if(!reader.IsEnd())
    {
      if(!reader.Read(parameters) || !reader.IsEnd())
        {
          return false;
        }

      if(algorithmparameters)
        {
          (*algorithmparameters) = parameters;
        }
       else
        {
          if((parameters.tag != 0x05) || parameters.size) return false;
        }
    }

  return true;
}


static bool CIPHERCERTIFICATEX509_DER_PositiveInteger(CIPHERCERTIFICATEX509_DERITEM& item, XDWORD& value)
{
  XDWORD index = 0;

  value = 0;

  if((item.tag != 0x02) || !item.size || (item.data[0] & 0x80)) return false;

  if((item.size > 1) && !item.data[0])
    {
      if(!(item.data[1] & 0x80)) return false;
      index = 1;
    }

  if((item.size - index) > sizeof(XDWORD)) return false;

  while(index < item.size)
    {
      value = (value << 8) | item.data[index++];
    }

  return true;
}


static bool CIPHERCERTIFICATEX509_DER_RSASSAPSSHash(CIPHERCERTIFICATEX509_DERITEM& item,
                                                     CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE& hashtype)
{
  XSTRING OID;

  hashtype = CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_UNKNOWN;

  if(!CIPHERCERTIFICATEX509_DER_Algorithm(item, OID)) return false;

  if(!OID.Compare(__L("2.16.840.1.101.3.4.2.1"), false))
    {
      hashtype = CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_SHA256;
      return true;
    }

  if(!OID.Compare(__L("2.16.840.1.101.3.4.2.2"), false))
    {
      hashtype = CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_SHA384;
      return true;
    }

  if(!OID.Compare(__L("2.16.840.1.101.3.4.2.3"), false))
    {
      hashtype = CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_SHA512;
      return true;
    }

  return false;
}


static bool CIPHERCERTIFICATEX509_DER_RSASSAPSSParameters(CIPHERCERTIFICATEX509_DERITEM& parameters,
                                                           CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE& hashtype,
                                                           XDWORD& saltsize)
{
  CIPHERCERTIFICATEX509_DERREADER reader(parameters.data, parameters.size);
  CIPHERCERTIFICATEX509_DERITEM   item;
  CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE MGFhashtype;
  bool                            hashpresent    = false;
  bool                            MGFpresent     = false;
  bool                            saltpresent    = false;
  bool                            trailerpresent = false;
  XBYTE                           lasttag        = 0;

  hashtype    = CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_UNKNOWN;
  MGFhashtype = CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_UNKNOWN;
  saltsize    = 0;

  if(parameters.tag != 0x30) return false;

  while(!reader.IsEnd())
    {
      CIPHERCERTIFICATEX509_DERREADER explicitreader(NULL, 0);
      CIPHERCERTIFICATEX509_DERITEM   value;

      if(!reader.Read(item)) return false;
      if((item.tag < 0xA0) || (item.tag > 0xA3) || (item.tag <= lasttag)) return false;

      lasttag = item.tag;

      explicitreader = CIPHERCERTIFICATEX509_DERREADER(item.data, item.size);
      if(!explicitreader.Read(value) || !explicitreader.IsEnd()) return false;

      switch(item.tag)
        {
          case 0xA0 : if(hashpresent || !CIPHERCERTIFICATEX509_DER_RSASSAPSSHash(value, hashtype)) return false;
                      hashpresent = true;
                      break;

          case 0xA1 : { CIPHERCERTIFICATEX509_DERITEM MGFparameters;
                        XSTRING                        MGFOID;

                        if(MGFpresent || !CIPHERCERTIFICATEX509_DER_Algorithm(value, MGFOID, &MGFparameters) ||
                           MGFOID.Compare(__L("1.2.840.113549.1.1.8"), false) ||
                           !CIPHERCERTIFICATEX509_DER_RSASSAPSSHash(MGFparameters, MGFhashtype)) return false;

                        MGFpresent = true;
                      }
                      break;

          case 0xA2 : if(saltpresent || !CIPHERCERTIFICATEX509_DER_PositiveInteger(value, saltsize)) return false;
                      saltpresent = true;
                      break;

          case 0xA3 : { XDWORD trailerfield;

                        if(trailerpresent || !CIPHERCERTIFICATEX509_DER_PositiveInteger(value, trailerfield) ||
                           (trailerfield != 1)) return false;

                        trailerpresent = true;
                      }
                      break;

              default : return false;
        }
    }

  if(!hashpresent || !MGFpresent || !saltpresent || (hashtype != MGFhashtype)) return false;

  switch(hashtype)
    {
      case CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_SHA256 : return (saltsize == HASHSHA2_256_DIGEST_SIZE);
      case CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_SHA384 : return (saltsize == HASHSHA2_384_DIGEST_SIZE);
      case CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_SHA512 : return (saltsize == HASHSHA2_512_DIGEST_SIZE);
                                                      default : break;
    }

  return false;
}


static bool CIPHERCERTIFICATEX509_DER_String(CIPHERCERTIFICATEX509_DERITEM& item, XSTRING& string)
{
  string.Empty();

  switch(item.tag)
    {
      case 0x0C : return string.ConvertFromUTF8((XBYTE*)item.data, item.size);

      case 0x12 :
      case 0x13 :
      case 0x14 :
      case 0x16 :
      case 0x1A : for(XDWORD c=0; c<item.size; c++)
                    {
                      if(item.data[c] & 0x80)
                        {
                          return false;
                        }

                      string.Add((XCHAR)item.data[c]);
                    }
                  return true;

      case 0x1E : if(item.size & 1)
                    {
                      return false;
                    }

                  for(XDWORD c=0; c<item.size; c+=2)
                    {
                      string.Add((XCHAR)(((XWORD)item.data[c] << 8) | item.data[c+1]));
                    }
                  return true;
    }

  return false;
}


static bool CIPHERCERTIFICATEX509_DER_Name(CIPHERCERTIFICATEX509_DERITEM& item, CIPHERCERTIFICATEX509_ID* ID)
{
  static const XBYTE OIDcountry[]          = { 0x55, 0x04, 0x06 };
  static const XBYTE OIDorganization[]     = { 0x55, 0x04, 0x0A };
  static const XBYTE OIDorganizationunit[] = { 0x55, 0x04, 0x0B };
  static const XBYTE OIDcommonname[]       = { 0x55, 0x04, 0x03 };

  CIPHERCERTIFICATEX509_DERREADER reader(item.data, item.size);
  CIPHERCERTIFICATEX509_DERITEM   set;

  if((item.tag != 0x30) || !ID)
    {
      return false;
    }

  while(!reader.IsEnd())
    {
      CIPHERCERTIFICATEX509_DERREADER setreader(NULL, 0);
      CIPHERCERTIFICATEX509_DERITEM   attribute;

      if(!reader.Read(set) || (set.tag != 0x31))
        {
          return false;
        }

      setreader = CIPHERCERTIFICATEX509_DERREADER(set.data, set.size);

      while(!setreader.IsEnd())
        {
          CIPHERCERTIFICATEX509_DERREADER attributereader(NULL, 0);
          CIPHERCERTIFICATEX509_DERITEM   OID;
          CIPHERCERTIFICATEX509_DERITEM   value;
          XSTRING                        string;
          XSTRING*                       target = NULL;

          if(!setreader.Read(attribute) || (attribute.tag != 0x30))
            {
              return false;
            }

          attributereader = CIPHERCERTIFICATEX509_DERREADER(attribute.data, attribute.size);

          if(!attributereader.Read(OID) || !attributereader.Read(value) || !attributereader.IsEnd())
            {
              return false;
            }

          if(CIPHERCERTIFICATEX509_DER_OIDCompare(OID, OIDcountry, sizeof(OIDcountry)))
            {
              target = ID->GetCountryName();
            }
          else if(CIPHERCERTIFICATEX509_DER_OIDCompare(OID, OIDorganization, sizeof(OIDorganization)))
            {
              target = ID->GetOrganizationName();
            }
          else if(CIPHERCERTIFICATEX509_DER_OIDCompare(OID, OIDorganizationunit, sizeof(OIDorganizationunit)))
            {
              target = ID->GetOrganizationalUnitName();
            }
          else if(CIPHERCERTIFICATEX509_DER_OIDCompare(OID, OIDcommonname, sizeof(OIDcommonname)))
            {
              target = ID->GetCommonName();
            }

          if(target)
            {
              if(!CIPHERCERTIFICATEX509_DER_String(value, string) || !target->Set(string))
                {
                  return false;
                }
            }
        }
    }

  return true;
}


static bool CIPHERCERTIFICATEX509_DER_Time(CIPHERCERTIFICATEX509_DERITEM& item, XDATETIME& datetime)
{
  int     year;
  int     index;
  XDWORD  timesize;

  if((item.tag != 0x17) && (item.tag != 0x18))
    {
      return false;
    }

  timesize = (item.tag == 0x17)?13:15;
  if((item.size != timesize) || (item.data[item.size-1] != 'Z'))
    {
      return false;
    }

  for(XDWORD c=0; c<(item.size-1); c++)
    {
      if((item.data[c] < '0') || (item.data[c] > '9'))
        {
          return false;
        }
    }

  if(item.tag == 0x17)
    {
      year  = ((item.data[0] - '0') * 10) + (item.data[1] - '0');
      year += (year >= 50)?1900:2000;
      index = 2;
    }
   else
    {
      year  = ((item.data[0] - '0') * 1000) + ((item.data[1] - '0') * 100);
      year += ((item.data[2] - '0') * 10) + (item.data[3] - '0');
      index = 4;
    }

  datetime.SetYear(year);
  datetime.SetMonth  (((item.data[index]    - '0') * 10) + (item.data[index+1]  - '0'));
  datetime.SetDay    (((item.data[index+2]  - '0') * 10) + (item.data[index+3]  - '0'));
  datetime.SetHours  (((item.data[index+4]  - '0') * 10) + (item.data[index+5]  - '0'));
  datetime.SetMinutes(((item.data[index+6]  - '0') * 10) + (item.data[index+7]  - '0'));
  datetime.SetSeconds(((item.data[index+8]  - '0') * 10) + (item.data[index+9]  - '0'));
  datetime.SetMilliSeconds(0);
  datetime.SetIsLocal(false);

  return datetime.IsValidDate();
}


static int CIPHERCERTIFICATEX509_HexValue(XCHAR character)
{
  if((character >= __C('0')) && (character <= __C('9'))) return character - __C('0');
  if((character >= __C('a')) && (character <= __C('f'))) return character - __C('a') + 10;
  if((character >= __C('A')) && (character <= __C('F'))) return character - __C('A') + 10;

  return -1;
}


static bool CIPHERCERTIFICATEX509_IPAddress(XCHAR* address, XBUFFER& binary)
{
  XSTRING string;
  XDWORD  size;

  if(!address)
    {
      return false;
    }

  string = address;
  size   = string.GetSize();

  if(string.FindCharacter(__C(':')) < 0)
    {
      XDWORD value = 0;
      XDWORD count = 0;
      XDWORD digits = 0;

      for(XDWORD c=0; c<=size; c++)
        {
          XCHAR character = (c<size)?string[c]:__C('.');

          if((character >= __C('0')) && (character <= __C('9')))
            {
              value = (value * 10) + character - __C('0');
              digits++;

              if((digits > 3) || (value > 255))
                {
                  return false;
                }
            }
          else if(character == __C('.'))
            {
              if(!digits || (count >= 4))
                {
                  return false;
                }

              binary.Add((XBYTE)value);
              count++;
              value  = 0;
              digits = 0;
            }
          else
            {
              return false;
            }
        }

      return (count == 4);
    }

  XWORD groups[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  int   ngroups   = 0;
  int   compressed = -1;
  XDWORD position = 0;

  if((size >= 2) && (string[0] == __C(':')) && (string[1] == __C(':')))
    {
      compressed = 0;
      position   = 2;
    }

  while(position < size)
    {
      XDWORD value  = 0;
      int    digits = 0;

      if(ngroups >= 8)
        {
          return false;
        }

      while((position < size) && (string[position] != __C(':')))
        {
          int hexadecimal = CIPHERCERTIFICATEX509_HexValue(string[position]);
          if((hexadecimal < 0) || (digits >= 4))
            {
              return false;
            }

          value = (value << 4) | hexadecimal;
          digits++;
          position++;
        }

      if(!digits)
        {
          return false;
        }

      groups[ngroups++] = (XWORD)value;

      if(position < size)
        {
          if((position + 1 < size) && (string[position+1] == __C(':')))
            {
              if(compressed >= 0)
                {
                  return false;
                }

              compressed = ngroups;
              position  += 2;
            }
           else
            {
              position++;
            }
        }
    }

  if(compressed >= 0)
    {
      int missing = 8 - ngroups;
      if(missing < 1)
        {
          return false;
        }

      for(int c=ngroups-1; c>=compressed; c--)
        {
          groups[c+missing] = groups[c];
        }

      for(int c=0; c<missing; c++)
        {
          groups[compressed+c] = 0;
        }

      ngroups = 8;
    }

  if(ngroups != 8)
    {
      return false;
    }

  for(int c=0; c<8; c++)
    {
      binary.Add((XBYTE)(groups[c] >> 8));
      binary.Add((XBYTE)(groups[c] & 0xFF));
    }

  return true;
}


static bool CIPHERCERTIFICATEX509_DNSName(XSTRING& pattern, XCHAR* servername)
{
  XSTRING hostname;
  int     dot;

  if(!servername || pattern.IsEmpty())
    {
      return false;
    }

  hostname = servername;

  if(hostname.Character_GetLast() == __C('.')) hostname.DeleteLastCharacter();
  if(pattern.Character_GetLast()  == __C('.')) pattern.DeleteLastCharacter();

  if(!pattern.Compare(hostname, true))
    {
      return true;
    }

  if((pattern.GetSize() < 3) || (pattern[0] != __C('*')) || (pattern[1] != __C('.')) ||
     (pattern.FindCharacter(__C('*'), 1) >= 0))
    {
      return false;
    }

  dot = hostname.FindCharacter(__C('.'));
  if(dot <= 0)
    {
      return false;
    }

  XSTRING hostnamesuffix(&hostname.Get()[dot]);
  XSTRING patternsuffix(&pattern.Get()[1]);

  return !hostnamesuffix.Compare(patternsuffix, true);
}



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/




/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERCERTIFICATEX509_ID::CIPHERCERTIFICATEX509_ID()
* @brief      Constructor of class
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERCERTIFICATEX509_ID::CIPHERCERTIFICATEX509_ID()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERCERTIFICATEX509_ID::~CIPHERCERTIFICATEX509_ID()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERCERTIFICATEX509_ID::~CIPHERCERTIFICATEX509_ID()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* CIPHERCERTIFICATEX509_ID::GetCountryName()
* @brief      get country name
* @ingroup    CIPHER
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* CIPHERCERTIFICATEX509_ID::GetCountryName()
{
  return &countryname;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* CIPHERCERTIFICATEX509_ID::GetOrganizationName()
* @brief      get organization name
* @ingroup    CIPHER
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* CIPHERCERTIFICATEX509_ID::GetOrganizationName()
{
  return &organizationame;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* CIPHERCERTIFICATEX509_ID::GetOrganizationalUnitName()
* @brief      get organizational unit name
* @ingroup    CIPHER
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* CIPHERCERTIFICATEX509_ID::GetOrganizationalUnitName()
{
  return &organizationalunitname;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* CIPHERCERTIFICATEX509_ID::GetOrganizationalUnitNamePlus()
* @brief      get organizational unit name plus
* @ingroup    CIPHER
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* CIPHERCERTIFICATEX509_ID::GetOrganizationalUnitNamePlus()
{
  return &organizationalunitnameplus;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* CIPHERCERTIFICATEX509_ID::GetCommonName()
* @brief      get common name
* @ingroup    CIPHER
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* CIPHERCERTIFICATEX509_ID::GetCommonName()
{
  return &commonname;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int CIPHERCERTIFICATEX509_ID::Compare(CIPHERCERTIFICATEX509_ID* certificateID, bool ignorecase )
* @brief      compare
* @ingroup    CIPHER
* 
* @param[in]  certificateID : Certificate ID pointer to use.
* @param[in]  ignorecase : Ignorecase value.
* 
* @return     int : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int CIPHERCERTIFICATEX509_ID::Compare(CIPHERCERTIFICATEX509_ID* certificateID, bool ignorecase )
{
  int index = 0;

  if(!certificateID)
    {
      return -1;
    } 

  if(GetCountryName()->Compare(certificateID->GetCountryName()->Get(), ignorecase))
    {   
      index++;
    }

  if(GetOrganizationName()->Compare(certificateID->GetOrganizationName()->Get(), ignorecase))
    {   
      index++;
    }

  if(GetOrganizationalUnitName()->Compare(certificateID->GetOrganizationalUnitName()->Get(), ignorecase))
    {   
      index++;
    }

  if(GetOrganizationalUnitNamePlus()->Compare(certificateID->GetOrganizationalUnitNamePlus()->Get(), ignorecase))
    {   
      index++;
    }

  if(GetCommonName()->Compare(certificateID->GetCommonName()->Get(), ignorecase))
    {   
      index++;
    }

  return index;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int CIPHERCERTIFICATEX509_ID::Compare(CIPHERCERTIFICATEX509_ID& certificateID, bool ignorecase)
* @brief      compare
* @ingroup    CIPHER
* 
* @param[in]  certificateID : Certificate ID value.
* @param[in]  ignorecase : Ignorecase value.
* 
* @return     int : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int CIPHERCERTIFICATEX509_ID::Compare(CIPHERCERTIFICATEX509_ID& certificateID, bool ignorecase)
{
  return Compare(&certificateID, ignorecase);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCERTIFICATEX509_ID::CopyTo(CIPHERCERTIFICATEX509_ID* certificateID)
* @brief      copy to
* @ingroup    CIPHER
* 
* @param[in]  certificateID : Certificate ID pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509_ID::CopyTo(CIPHERCERTIFICATEX509_ID* certificateID)
{
  if(certificateID)
    {
      return false;
    }
  
  if(!certificateID->GetCountryName()->Set(GetCountryName()->Get()))
    {
      return false;
    }

  if(!certificateID->GetOrganizationName()->Set(GetOrganizationName()->Get()))
    {
      return false;
    }

  if(!certificateID->GetOrganizationalUnitName()->Set(GetOrganizationalUnitName()->Get()))
    {
      return false;
    }

  if(!certificateID->GetOrganizationalUnitNamePlus()->Set(GetOrganizationalUnitNamePlus()->Get()))
    {
      return false;
    }

  if(!certificateID->GetCommonName()->Set(GetCommonName()->Get()))
    {
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCERTIFICATEX509_ID::CopyTo(CIPHERCERTIFICATEX509_ID& certificateID)
* @brief      copy to
* @ingroup    CIPHER
* 
* @param[in]  certificateID : Certificate ID value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509_ID::CopyTo(CIPHERCERTIFICATEX509_ID& certificateID)
{
  return CopyTo(&certificateID);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCERTIFICATEX509_ID::CopyFrom(CIPHERCERTIFICATEX509_ID* certificateID)
* @brief      copy from
* @ingroup    CIPHER
* 
* @param[in]  certificateID : Certificate ID pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509_ID::CopyFrom(CIPHERCERTIFICATEX509_ID* certificateID)
{
  if(certificateID)
    {
      return false;
    }
  
  if(!GetCountryName()->Set(certificateID->GetCountryName()->Get()))
    {
      return false;
    }

  if(!GetOrganizationName()->Set(certificateID->GetOrganizationName()->Get()))
    {
      return false;
    }

  if(!GetOrganizationalUnitName()->Set(certificateID->GetOrganizationalUnitName()->Get()))
    {
      return false;
    }

  if(!GetOrganizationalUnitNamePlus()->Set(certificateID->GetOrganizationalUnitNamePlus()->Get()))
    {
      return false;
    }

  if(!GetCommonName()->Set(certificateID->GetCommonName()->Get()))
    {
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCERTIFICATEX509_ID::CopyFrom(CIPHERCERTIFICATEX509_ID& certificateID)
* @brief      copy from
* @ingroup    CIPHER
* 
* @param[in]  certificateID : Certificate ID value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509_ID::CopyFrom(CIPHERCERTIFICATEX509_ID& certificateID)
{
  return CopyFrom(&certificateID);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCERTIFICATEX509_ID::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCERTIFICATEX509_ID::Clean()
{
  countryname.Empty();    
  organizationame.Empty();    
  organizationalunitname.Empty();    
  organizationalunitnameplus.Empty();    
  commonname.Empty();    
}





/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERCERTIFICATEX509::CIPHERCERTIFICATEX509()
* @brief      Constructor of class
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERCERTIFICATEX509::CIPHERCERTIFICATEX509() 
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERCERTIFICATEX509::~CIPHERCERTIFICATEX509()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERCERTIFICATEX509::~CIPHERCERTIFICATEX509()
{
  if(publiccipherkey)
    {
      GEN_DELETE publiccipherkey;
    }

  if(hash)
    {
      GEN_DELETE hash;
    }

  subjectalternativenamesDNS.DeleteContents();
  subjectalternativenamesDNS.DeleteAll();

  subjectalternativenamesIP.DeleteContents();
  subjectalternativenamesIP.DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD CIPHERCERTIFICATEX509::GetVersion()
* @brief      Get version
* @ingroup    CIPHER
* 
* @return     XWORD : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWORD CIPHERCERTIFICATEX509::GetVersion()
{
  return version;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCERTIFICATEX509::SetVersion(XWORD version)
* @brief      Set version
* @ingroup    CIPHER
* 
* @param[in]  version : Version value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCERTIFICATEX509::SetVersion(XWORD version)
{
  this->version = version;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBUFFER* CIPHERCERTIFICATEX509::GetSerial()
* @brief      Get serial
* @ingroup    CIPHER
* 
* @return     XBUFFER* : Pointer to the requested buffer; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* CIPHERCERTIFICATEX509::GetSerial()
{
  return &serial;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERCERTIFICATEX509_ALGORITHM_TYPE CIPHERCERTIFICATEX509::GetAlgorithmType()
* @brief      get algorithm type
* @ingroup    CIPHER
* 
* @return     CIPHERCERTIFICATEX509_ALGORITHM_TYPE : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERCERTIFICATEX509_ALGORITHM_TYPE CIPHERCERTIFICATEX509::GetAlgorithmType()
{
  return algorithmtype;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCERTIFICATEX509::SetAlgorithmType(XCHAR* OID)
* @brief      Set algorithm type
* @ingroup    CIPHER
* 
* @param[in]  OID : OID pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::SetAlgorithmType(XCHAR* OID)
{
  XSTRING _OID;

  if(!OID)
    {
      return false;    
    }

  _OID = OID;

  algorithmtype = CIPHERCERTIFICATEX509_ALGORITHM_TYPE_UNKNOWN;
  algorithmtypestr.Empty();

  if(!_OID.Compare(__L("1.2.840.113549.1.1.5"), false))
    {
      algorithmtype     = CIPHERCERTIFICATEX509_ALGORITHM_TYPE_SHA1WITHRSAENCRYPTION;
      algorithmtypestr  = __L("RSA signature with SHA-1");
    }

  if(!_OID.Compare(__L("1.2.840.113549.1.1.11"), false))
    {
      algorithmtype     = CIPHERCERTIFICATEX509_ALGORITHM_TYPE_SHA256WITHRSAENCRYPTION;
      algorithmtypestr  = __L("RSA signature using SHA-256");
    }

  if(!_OID.Compare(__L("1.2.840.113549.1.1.12"), false))
    {    
      algorithmtype     = CIPHERCERTIFICATEX509_ALGORITHM_TYPE_SHA384WITHRSAENCRYPTION;
      algorithmtypestr  = __L("RSA signature with SHA-384");
    }

  if(!_OID.Compare(__L("1.2.840.113549.1.1.13"), false))
    {   
      algorithmtype     = CIPHERCERTIFICATEX509_ALGORITHM_TYPE_SHA512WITHRSAENCRYPTION;
      algorithmtypestr  = __L("RSA signature using SHA-512");
    }

  if(!_OID.Compare(__L("1.2.840.113549.1.1.4"), false))
    {
      algorithmtype     = CIPHERCERTIFICATEX509_ALGORITHM_TYPE_MD5WITHRSAENCRYPTION;
      algorithmtypestr  = __L("RSA signature with MD5");
    }
 
  if(!_OID.Compare(__L("1.2.840.10045.4.1"), false))
    {
      algorithmtype     = CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA1;
      algorithmtypestr  = __L("ECDSA signature with SHA-1");
    }

  if(!_OID.Compare(__L("1.2.840.10045.4.3.2"), false))
    {
      algorithmtype     = CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA256;
      algorithmtypestr  = __L("ECDSA signature using SHA-256");
    }

  if(!_OID.Compare(__L("1.2.840.10045.4.3.3"), false))
    {           
      algorithmtype     = CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA384;
      algorithmtypestr  = __L("ECDSA signature with SHA-384");
    }	            

  if(!_OID.Compare(__L("1.2.840.10045.4.3.4"), false))
    {
      algorithmtype     = CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA512;
      algorithmtypestr  = __L("ECDSA signature using SHA-512");
    }	      
      
  if(!_OID.Compare(__L("1.2.840.10040.4.3"), false))
    {
      algorithmtype     = CIPHERCERTIFICATEX509_ALGORITHM_TYPE_DSAWITHSHA1;
      algorithmtypestr  = __L("DSA signature with SHA-1");
    }	     
           
  if(!_OID.Compare(__L("2.16.840.1.101.3.4.3.2"), false))
    {
      algorithmtype     = CIPHERCERTIFICATEX509_ALGORITHM_TYPE_DSAWITHSHA256;
      algorithmtypestr  = __L("DSA signature using SHA-256");
    }	   
               
  if(!_OID.Compare(__L("1.2.840.113549.1.1.10"), false))
    {
      algorithmtype     = CIPHERCERTIFICATEX509_ALGORITHM_TYPE_RSASSAPSS;    	                  
      algorithmtypestr  = __L("Probabilistic RSA signature scheme");  
    }
 
  return (algorithmtype != CIPHERCERTIFICATEX509_ALGORITHM_TYPE_UNKNOWN)?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* CIPHERCERTIFICATEX509::GetAlgorithmTypeStr()
* @brief      get algorithm type str
* @ingroup    CIPHER
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* CIPHERCERTIFICATEX509::GetAlgorithmTypeStr()
{
  return &algorithmtypestr;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE CIPHERCERTIFICATEX509::GetRSASSAPSSHashType()
* @brief      Get the validated RSA-PSS hash algorithm
* @ingroup    CIPHER
*
* @return     CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE CIPHERCERTIFICATEX509::GetRSASSAPSSHashType()
{
  return RSASSAPSShashtype;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD CIPHERCERTIFICATEX509::GetRSASSAPSSSaltSize()
* @brief      Get the validated RSA-PSS salt size
* @ingroup    CIPHER
*
* @return     XDWORD : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD CIPHERCERTIFICATEX509::GetRSASSAPSSSaltSize()
{
  return RSASSAPSSsaltsize;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERCERTIFICATEX509_ID* CIPHERCERTIFICATEX509::GetIssuerID()
* @brief      get issuer Id
* @ingroup    CIPHER
* 
* @return     CIPHERCERTIFICATEX509_ID* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERCERTIFICATEX509_ID* CIPHERCERTIFICATEX509::GetIssuerID()
{
  return &issuerID;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCERTIFICATEX509::IsValidDates()
* @brief      is valid dates
* @ingroup    CIPHER
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::IsValidDates()
{
  XDATETIME* xdatetime = GEN_XFACTORY.CreateDateTime();
  if(!xdatetime)
    {
      return false;
    }

  xdatetime->Read(false);
  bool status = IsValidDates(xdatetime);

  GEN_XFACTORY.DeleteDateTime(xdatetime);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERCERTIFICATEX509::IsValidDates(XDATETIME* datetime)
* @brief      Check certificate validity at a supplied UTC time
* @ingroup    CIPHER
*
* @param[in]  datetime : Validation time.
*
* @return     bool : true if notBefore <= datetime <= notAfter; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::IsValidDates(XDATETIME* datetime)
{
  if(!datetime || !datetime->IsValidDate() || !datenotbefore.IsValidDate() || !datenotafter.IsValidDate())
    {
      return false;
    }

  XQWORD secondsactual    = datetime->GetSeconsFromDate();
  XQWORD secondsnotbefore = datenotbefore.GetSeconsFromDate();
  XQWORD secondsnotafter  = datenotafter.GetSeconsFromDate();

  return ((secondsactual >= secondsnotbefore) && (secondsactual <= secondsnotafter));
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDATETIME* CIPHERCERTIFICATEX509::GetDateNotBefore()
* @brief      Get date not before
* @ingroup    CIPHER
* 
* @return     XDATETIME* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDATETIME* CIPHERCERTIFICATEX509::GetDateNotBefore()
{
  return &datenotbefore;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDATETIME* CIPHERCERTIFICATEX509::GetDateNotAfter()
* @brief      Get date not after
* @ingroup    CIPHER
* 
* @return     XDATETIME* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDATETIME* CIPHERCERTIFICATEX509::GetDateNotAfter()
{
  return &datenotafter;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCERTIFICATEX509::IsSelfSigned()
* @brief      is self signed
* @ingroup    CIPHER
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::IsSelfSigned()
{
  if(!GetIssuerID()->Compare(GetSubjectID(), true))
    {
      return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERCERTIFICATEX509_ID* CIPHERCERTIFICATEX509::GetSubjectID()
* @brief      get subject Id
* @ingroup    CIPHER
* 
* @return     CIPHERCERTIFICATEX509_ID* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERCERTIFICATEX509_ID* CIPHERCERTIFICATEX509::GetSubjectID()
{
  return &subjectID;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCERTIFICATEX509::IsPublicCipherKeyUsage()
* @brief      is public cipher key usage
* @ingroup    CIPHER
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::IsPublicCipherKeyUsage()
{
  return publiccipherkeyusaged;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCERTIFICATEX509::SetPublicCipherKeyUsage(bool isusage)
* @brief      set public cipher key usage
* @ingroup    CIPHER
* 
* @param[in]  isusage : Isusage value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCERTIFICATEX509::SetPublicCipherKeyUsage(bool isusage)
{
  publiccipherkeyusaged = isusage;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCERTIFICATEX509::IsPublicCipherKeyBasicConstraints()
* @brief      is public cipher key basic constraints
* @ingroup    CIPHER
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::IsPublicCipherKeyBasicConstraints()
{
  return publiccipherkeybasicconstraints;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCERTIFICATEX509::SetPublicCipherKeyBasicConstraints(bool basicconstraints)
* @brief      set public cipher key basic constraints
* @ingroup    CIPHER
* 
* @param[in]  basicconstraints : Basicconstraints value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCERTIFICATEX509::SetPublicCipherKeyBasicConstraints(bool basicconstraints)
{
  publiccipherkeybasicconstraints = basicconstraints;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCERTIFICATEX509::IsPublicCipherKeyValid()
* @brief      is public cipher key valid
* @ingroup    CIPHER
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::IsPublicCipherKeyValid()
{
  return publiccipherkeyvalid;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCERTIFICATEX509::SetPublicCipherKeyValid(bool isvalid)
* @brief      set public cipher key valid
* @ingroup    CIPHER
* 
* @param[in]  isvalid : Isvalid value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCERTIFICATEX509::SetPublicCipherKeyValid(bool isvalid)
{
  publiccipherkeyvalid = isvalid;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* CIPHERCERTIFICATEX509::GetPublicCipherKeyID()
* @brief      get public cipher key Id
* @ingroup    CIPHER
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* CIPHERCERTIFICATEX509::GetPublicCipherKeyID()
{
  return &publiccipherkeyID;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERKEY* CIPHERCERTIFICATEX509::GetPublicCipherKey()
* @brief      get public cipher key
* @ingroup    CIPHER
* 
* @return     CIPHERKEY* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERKEY* CIPHERCERTIFICATEX509::GetPublicCipherKey()
{
  return publiccipherkey;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCERTIFICATEX509::SetPublicCipherKey(CIPHERKEY* publiccipherkey)
* @brief      Set public cipher key
* @ingroup    CIPHER
* 
* @param[in]  publiccipherkey : Publiccipherkey pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::SetPublicCipherKey(CIPHERKEY* publiccipherkey)
{
  if(!publiccipherkey)
    {
      return false;
    }

  this->publiccipherkey = publiccipherkey;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         HASH* CIPHERCERTIFICATEX509::GetHash()
* @brief      get hash
* @ingroup    CIPHER
* 
* @return     HASH* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
HASH* CIPHERCERTIFICATEX509::GetHash()
{
  return hash;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCERTIFICATEX509::SetHash(HASH* hash)
* @brief      set hash
* @ingroup    CIPHER
* 
* @param[in]  hash : Hash pointer to use.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCERTIFICATEX509::SetHash(HASH* hash)
{
  this->hash = hash;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBUFFER* CIPHERCERTIFICATEX509::GetHashData()
* @brief      get hash data
* @ingroup    CIPHER
* 
* @return     XBUFFER* : Pointer to the requested buffer; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* CIPHERCERTIFICATEX509::GetHashData()
{
  return &hashdata;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERCERTIFICATEX509::Decode(XBUFFER& certificate)
* @brief      Decode a bounded DER X.509 certificate
* @ingroup    CIPHER
*
* @param[in]  certificate : Complete DER certificate.
*
* @return     bool : true if the certificate is structurally valid and its public key is supported; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::Decode(XBUFFER& certificate)
{
  static const XBYTE OIDbasicconstraints[] = { 0x55, 0x1D, 0x13 };
  static const XBYTE OIDkeyusage[]         = { 0x55, 0x1D, 0x0F };
  static const XBYTE OIDextendedkeyusage[] = { 0x55, 0x1D, 0x25 };
  static const XBYTE OIDsubjectaltname[]   = { 0x55, 0x1D, 0x11 };
  static const XBYTE OIDserverauth[]       = { 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x01 };

  CIPHERCERTIFICATEX509_DERREADER certificatereader(certificate.Get(), certificate.GetSize());
  CIPHERCERTIFICATEX509_DERITEM   certificatesequence;
  CIPHERCERTIFICATEX509_DERITEM   TBS;
  CIPHERCERTIFICATEX509_DERITEM   outeralgorithm;
  CIPHERCERTIFICATEX509_DERITEM   outeralgorithmparameters;
  CIPHERCERTIFICATEX509_DERITEM   signatureitem;
  XSTRING                          outeralgorithmOID;
  XSTRING                          inneralgorithmOID;

  if(certificate.IsEmpty() || !certificatedata.IsEmpty() ||
     !certificatereader.Read(certificatesequence) || !certificatereader.IsEnd() ||
     (certificatesequence.tag != 0x30))
    {
      return false;
    }

  CIPHERCERTIFICATEX509_DERREADER sequence(certificatesequence.data, certificatesequence.size);

  if(!sequence.Read(TBS) || (TBS.tag != 0x30) ||
     !sequence.Read(outeralgorithm) || !sequence.Read(signatureitem) || !sequence.IsEnd() ||
     !CIPHERCERTIFICATEX509_DER_Algorithm(outeralgorithm, outeralgorithmOID, &outeralgorithmparameters) ||
     (signatureitem.tag != 0x03) || (signatureitem.size < 2) || signatureitem.data[0])
    {
      return false;
    }

  if(!SetAlgorithmType(outeralgorithmOID.Get()))
    {
      return false;
    }

  if(algorithmtype == CIPHERCERTIFICATEX509_ALGORITHM_TYPE_RSASSAPSS)
    {
      if(!CIPHERCERTIFICATEX509_DER_RSASSAPSSParameters(outeralgorithmparameters,
                                                        RSASSAPSShashtype, RSASSAPSSsaltsize)) return false;

      switch(RSASSAPSShashtype)
        {
          case CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_SHA256 : algorithmtypestr = __L("RSA-PSS signature using SHA-256");
                                                                  break;

          case CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_SHA384 : algorithmtypestr = __L("RSA-PSS signature using SHA-384");
                                                                  break;

          case CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_SHA512 : algorithmtypestr = __L("RSA-PSS signature using SHA-512");
                                                                  break;

                                                          default : return false;
        }
    }
   else if((algorithmtype == CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA1)   ||
           (algorithmtype == CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA256) ||
           (algorithmtype == CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA384) ||
           (algorithmtype == CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA512))
    {
      if(outeralgorithmparameters.tag) return false;
    }
   else
    {
      if(outeralgorithmparameters.tag &&
         ((outeralgorithmparameters.tag != 0x05) || outeralgorithmparameters.size)) return false;
    }

  CIPHERCERTIFICATEX509_DERREADER tbsreader(TBS.data, TBS.size);
  CIPHERCERTIFICATEX509_DERITEM   item;

  if(!tbsreader.Read(item))
    {
      return false;
    }

  if(item.tag == 0xA0)
    {
      CIPHERCERTIFICATEX509_DERREADER versionreader(item.data, item.size);
      CIPHERCERTIFICATEX509_DERITEM   versionitem;

      if(!versionreader.Read(versionitem) || !versionreader.IsEnd() ||
         (versionitem.tag != 0x02) || (versionitem.size != 1) || (versionitem.data[0] > 2))
        {
          return false;
        }

      XWORD versionvalue = 0;
      for(XDWORD c=0; c<versionitem.size; c++)
        {
          versionvalue = (XWORD)((versionvalue << 8) | versionitem.data[c]);
        }

      version = versionvalue + 1;

      if(!tbsreader.Read(item))
        {
          return false;
        }
    }
   else
    {
      version = 1;
    }

  if((item.tag != 0x02) || !item.size || (item.data[0] & 0x80) ||
     ((item.size > 1) && !item.data[0] && !(item.data[1] & 0x80)) ||
     !serial.Add((XBYTE*)item.data, item.size))
    {
      return false;
    }

  CIPHERCERTIFICATEX509_DERITEM inneralgorithmparameters;

  if(!tbsreader.Read(item) || !CIPHERCERTIFICATEX509_DER_Algorithm(item, inneralgorithmOID, &inneralgorithmparameters) ||
     inneralgorithmOID.Compare(outeralgorithmOID) ||
     (item.encodedsize != outeralgorithm.encodedsize) ||
     memcmp(item.encoded, outeralgorithm.encoded, item.encodedsize))
    {
      return false;
    }

  if(!tbsreader.Read(item) || (item.tag != 0x30) ||
     !issuerdata.Add((XBYTE*)item.encoded, item.encodedsize) ||
     !CIPHERCERTIFICATEX509_DER_Name(item, &issuerID))
    {
      return false;
    }

  if(!tbsreader.Read(item) || (item.tag != 0x30))
    {
      return false;
    }

  CIPHERCERTIFICATEX509_DERREADER validityreader(item.data, item.size);
  CIPHERCERTIFICATEX509_DERITEM   notbefore;
  CIPHERCERTIFICATEX509_DERITEM   notafter;

  if(!validityreader.Read(notbefore) || !validityreader.Read(notafter) || !validityreader.IsEnd() ||
     !CIPHERCERTIFICATEX509_DER_Time(notbefore, datenotbefore) ||
     !CIPHERCERTIFICATEX509_DER_Time(notafter, datenotafter) ||
     (datenotbefore.GetSeconsFromDate() > datenotafter.GetSeconsFromDate()))
    {
      return false;
    }

  if(!tbsreader.Read(item) || (item.tag != 0x30) ||
     !subjectdata.Add((XBYTE*)item.encoded, item.encodedsize) ||
     !CIPHERCERTIFICATEX509_DER_Name(item, &subjectID))
    {
      return false;
    }

  if(!tbsreader.Read(item) || (item.tag != 0x30))
    {
      return false;
    }

  CIPHERCERTIFICATEX509_DERREADER publickeyinforeader(item.data, item.size);
  CIPHERCERTIFICATEX509_DERITEM   publickeyalgorithm;
  CIPHERCERTIFICATEX509_DERITEM   publickeyalgorithmparameters;
  CIPHERCERTIFICATEX509_DERITEM   publickeybits;
  XSTRING                          publickeyalgorithmOID;

  if(!publickeyinforeader.Read(publickeyalgorithm) || !publickeyinforeader.Read(publickeybits) ||
     !publickeyinforeader.IsEnd() ||
     !CIPHERCERTIFICATEX509_DER_Algorithm(publickeyalgorithm, publickeyalgorithmOID,
                                          &publickeyalgorithmparameters) ||
     (publickeybits.tag != 0x03) || (publickeybits.size < 2) || publickeybits.data[0])
    {
      return false;
    }

  if(!publickeyalgorithmOID.Compare(__L("1.2.840.113549.1.1.1"), false))
    {
      CIPHERCERTIFICATEX509_DERREADER publickeyreader(&publickeybits.data[1], publickeybits.size - 1);
      CIPHERCERTIFICATEX509_DERITEM   publickeysequence;

      if((publickeyalgorithmparameters.tag &&
          ((publickeyalgorithmparameters.tag != 0x05) || publickeyalgorithmparameters.size)) ||
         !publickeyreader.Read(publickeysequence) || !publickeyreader.IsEnd() || (publickeysequence.tag != 0x30))
        {
          return false;
        }

      CIPHERCERTIFICATEX509_DERREADER RSAreader(publickeysequence.data, publickeysequence.size);
      CIPHERCERTIFICATEX509_DERITEM   modulusitem;
      CIPHERCERTIFICATEX509_DERITEM   exponentitem;

      if(!RSAreader.Read(modulusitem) || !RSAreader.Read(exponentitem) || !RSAreader.IsEnd() ||
         (modulusitem.tag != 0x02) || !modulusitem.size || (modulusitem.data[0] & 0x80) ||
         ((modulusitem.size > 1) && !modulusitem.data[0] && !(modulusitem.data[1] & 0x80)) ||
         (exponentitem.tag != 0x02) || !exponentitem.size || (exponentitem.data[0] & 0x80) ||
         ((exponentitem.size > 1) && !exponentitem.data[0] && !(exponentitem.data[1] & 0x80)))
        {
          return false;
        }

      XBUFFER modulusbuffer;
      XBUFFER exponentbuffer;
      XMPINTEGER modulus;
      XMPINTEGER exponent;

      if(!modulusbuffer.Add((XBYTE*)modulusitem.data, modulusitem.size) ||
         !exponentbuffer.Add((XBYTE*)exponentitem.data, exponentitem.size) ||
         !modulus.SetFromXBuffer(modulusbuffer) || !exponent.SetFromXBuffer(exponentbuffer))
        {
          return false;
        }

      CIPHERKEYPUBLICRSA* RSApublickey = GEN_NEW CIPHERKEYPUBLICRSA();
      if(!RSApublickey)
        {
          return false;
        }

      if(!RSApublickey->Set(modulus, exponent) || !RSApublickey->Check() || !SetPublicCipherKey(RSApublickey))
        {
          GEN_DELETE RSApublickey;
          return false;
        }
    }
   else if(!publickeyalgorithmOID.Compare(__L("1.2.840.10045.2.1"), false))
    {
      XSTRING publickeycurveOID;

      if((publickeyalgorithmparameters.tag != 0x06) ||
         !CIPHERCERTIFICATEX509_DER_OIDToString(publickeyalgorithmparameters, publickeycurveOID) ||
         publickeycurveOID.Compare(__L("1.2.840.10045.3.1.7"), false) ||
         (publickeybits.size != (CIPHERECDSA_P256_PUBLICKEY_SIZE + 1)) ||
         (publickeybits.data[1] != 0x04))
        {
          return false;
        }

      CIPHERKEYECDSA* ECDSApublickey = GEN_NEW CIPHERKEYECDSA();
      if(!ECDSApublickey)
        {
          return false;
        }

      ECDSApublickey->SetType(CIPHERKEYTYPE_ECDSA_SECP256R1_PUBLIC);

      CIPHERECDSA ECDSA;
      if(!ECDSApublickey->Set((XBYTE*)&publickeybits.data[1], CIPHERECDSA_P256_PUBLICKEY_SIZE) ||
         !ECDSA.SetKey(ECDSApublickey, true) || !SetPublicCipherKey(ECDSApublickey))
        {
          GEN_DELETE ECDSApublickey;
          return false;
        }
    }
   else
    {
      return false;
    }

  publiccipherkeyvalid = true;
  publiccipherkeyID    = publickeyalgorithmOID;

  bool hasextensionsblock = false;

  while(!tbsreader.IsEnd())
    {
      if(!tbsreader.Read(item))
        {
          return false;
        }

      if((item.tag == 0x81) || (item.tag == 0x82))
        {
          if(version < 2)
            {
              return false;
            }

          continue;
        }

      if((item.tag != 0xA3) || (version != 3) || hasextensionsblock)
        {
          return false;
        }

      hasextensionsblock = true;

      CIPHERCERTIFICATEX509_DERREADER explicitreader(item.data, item.size);
      CIPHERCERTIFICATEX509_DERITEM   extensionssequence;

      if(!explicitreader.Read(extensionssequence) || !explicitreader.IsEnd() || (extensionssequence.tag != 0x30))
        {
          return false;
        }

      CIPHERCERTIFICATEX509_DERREADER extensionsreader(extensionssequence.data, extensionssequence.size);

      while(!extensionsreader.IsEnd())
        {
          CIPHERCERTIFICATEX509_DERITEM extensionsequence;
          CIPHERCERTIFICATEX509_DERITEM extensionOID;
          CIPHERCERTIFICATEX509_DERITEM extensionvalue;
          CIPHERCERTIFICATEX509_DERITEM criticalitem;
          bool                          critical = false;

          if(!extensionsreader.Read(extensionsequence) || (extensionsequence.tag != 0x30))
            {
              return false;
            }

          CIPHERCERTIFICATEX509_DERREADER extensionreader(extensionsequence.data, extensionsequence.size);
          if(!extensionreader.Read(extensionOID) || !extensionreader.Read(extensionvalue))
            {
              return false;
            }

          if(extensionvalue.tag == 0x01)
            {
              criticalitem = extensionvalue;
              if((criticalitem.size != 1) ||
                 ((criticalitem.data[0] != 0x00) && (criticalitem.data[0] != 0xFF)) ||
                 !extensionreader.Read(extensionvalue))
                {
                  return false;
                }

              critical = criticalitem.data[0]?true:false;
            }

          if(!extensionreader.IsEnd() || (extensionvalue.tag != 0x04))
            {
              return false;
            }

          if(CIPHERCERTIFICATEX509_DER_OIDCompare(extensionOID, OIDbasicconstraints, sizeof(OIDbasicconstraints)))
            {
              CIPHERCERTIFICATEX509_DERREADER constraintsouter(extensionvalue.data, extensionvalue.size);
              CIPHERCERTIFICATEX509_DERITEM   constraintssequence;

              if(hasbasicconstraints || !constraintsouter.Read(constraintssequence) || !constraintsouter.IsEnd() ||
                 (constraintssequence.tag != 0x30))
                {
                  return false;
                }

              hasbasicconstraints = true;

              CIPHERCERTIFICATEX509_DERREADER constraintsreader(constraintssequence.data, constraintssequence.size);
              if(!constraintsreader.IsEnd())
                {
                  CIPHERCERTIFICATEX509_DERITEM constraint;
                  if(!constraintsreader.Read(constraint))
                    {
                      return false;
                    }

                  if(constraint.tag == 0x01)
                    {
                      if((constraint.size != 1) ||
                         ((constraint.data[0] != 0x00) && (constraint.data[0] != 0xFF)))
                        {
                          return false;
                        }

                      iscertificateauthority = constraint.data[0]?true:false;

                      if(!constraintsreader.IsEnd() && !constraintsreader.Read(constraint))
                        {
                          return false;
                        }
                    }

                  if(constraint.tag == 0x02)
                    {
                      if(!constraint.size || (constraint.size > 4) || (constraint.data[0] & 0x80))
                        {
                          return false;
                        }

                      basicconstraintspathlength = 0;
                      for(XDWORD c=0; c<constraint.size; c++)
                        {
                          basicconstraintspathlength = (basicconstraintspathlength << 8) | constraint.data[c];
                        }
                    }
                   else if(constraint.tag != 0x01)
                    {
                      return false;
                    }

                  if(!constraintsreader.IsEnd())
                    {
                      return false;
                    }
                }

              if((basicconstraintspathlength >= 0) && !iscertificateauthority)
                {
                  return false;
                }

              publiccipherkeybasicconstraints = iscertificateauthority;
            }
          else if(CIPHERCERTIFICATEX509_DER_OIDCompare(extensionOID, OIDkeyusage, sizeof(OIDkeyusage)))
            {
              CIPHERCERTIFICATEX509_DERREADER keyusagereader(extensionvalue.data, extensionvalue.size);
              CIPHERCERTIFICATEX509_DERITEM   keyusage;

              if(haskeyusage || !keyusagereader.Read(keyusage) || !keyusagereader.IsEnd() ||
                 (keyusage.tag != 0x03) || (keyusage.size < 2) || (keyusage.data[0] > 7))
                {
                  return false;
                }

              if(keyusage.data[0] &&
                 (keyusage.data[keyusage.size-1] & (XBYTE)((1 << keyusage.data[0]) - 1)))
                {
                  return false;
                }

              haskeyusage             = true;
              keyusagedigitalsignature = (keyusage.data[1] & 0x80)?true:false;
              keyusagecertificatesign  = (keyusage.data[1] & 0x04)?true:false;
              publiccipherkeyusaged    = keyusagedigitalsignature;
            }
          else if(CIPHERCERTIFICATEX509_DER_OIDCompare(extensionOID, OIDextendedkeyusage, sizeof(OIDextendedkeyusage)))
            {
              CIPHERCERTIFICATEX509_DERREADER extendedouter(extensionvalue.data, extensionvalue.size);
              CIPHERCERTIFICATEX509_DERITEM   extendedsequence;

              if(hasextendedkeyusage || !extendedouter.Read(extendedsequence) || !extendedouter.IsEnd() ||
                 (extendedsequence.tag != 0x30))
                {
                  return false;
                }

              hasextendedkeyusage = true;
              CIPHERCERTIFICATEX509_DERREADER extendedreader(extendedsequence.data, extendedsequence.size);

              while(!extendedreader.IsEnd())
                {
                  CIPHERCERTIFICATEX509_DERITEM extendedOID;
                  if(!extendedreader.Read(extendedOID) || (extendedOID.tag != 0x06))
                    {
                      return false;
                    }

                  if(CIPHERCERTIFICATEX509_DER_OIDCompare(extendedOID, OIDserverauth, sizeof(OIDserverauth)))
                    {
                      extendedkeyusageserverauthentication = true;
                    }
                }
            }
          else if(CIPHERCERTIFICATEX509_DER_OIDCompare(extensionOID, OIDsubjectaltname, sizeof(OIDsubjectaltname)))
            {
              CIPHERCERTIFICATEX509_DERREADER namesouter(extensionvalue.data, extensionvalue.size);
              CIPHERCERTIFICATEX509_DERITEM   namessequence;

              if(hassubjectalternativename || !namesouter.Read(namessequence) || !namesouter.IsEnd() ||
                 (namessequence.tag != 0x30))
                {
                  return false;
                }

              hassubjectalternativename = true;
              CIPHERCERTIFICATEX509_DERREADER namesreader(namessequence.data, namessequence.size);

              while(!namesreader.IsEnd())
                {
                  CIPHERCERTIFICATEX509_DERITEM name;
                  if(!namesreader.Read(name))
                    {
                      return false;
                    }

                  if(name.tag == 0x82)
                    {
                      XSTRING* DNSname = GEN_NEW XSTRING();
                      if(!DNSname)
                        {
                          return false;
                        }

                      for(XDWORD c=0; c<name.size; c++)
                        {
                          if((name.data[c] & 0x80) || !name.data[c])
                            {
                              GEN_DELETE DNSname;
                              return false;
                            }

                          DNSname->Add((XCHAR)name.data[c]);
                        }

                      if(DNSname->IsEmpty() || !subjectalternativenamesDNS.Add(DNSname))
                        {
                          GEN_DELETE DNSname;
                          return false;
                        }
                    }
                  else if(name.tag == 0x87)
                    {
                      if((name.size != 4) && (name.size != 16))
                        {
                          return false;
                        }

                      XBUFFER* IPname = GEN_NEW XBUFFER();
                      if(!IPname || !IPname->Add((XBYTE*)name.data, name.size) ||
                         !subjectalternativenamesIP.Add(IPname))
                        {
                          if(IPname) GEN_DELETE IPname;
                          return false;
                        }
                    }
                }
            }
          else if(critical)
            {
              hasunknowncriticalextension = true;
            }
        }
    }

  if(!certificatedata.Add(certificate) ||
     !tbsdata.Add((XBYTE*)TBS.encoded, TBS.encodedsize) ||
     !signature.Add((XBYTE*)&signatureitem.data[1], signatureitem.size - 1))
    {
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* CIPHERCERTIFICATEX509::GetCertificateData()
* @brief      Get the complete DER certificate
* @ingroup    CIPHER
*
* @return     XBUFFER* : Certificate buffer.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* CIPHERCERTIFICATEX509::GetCertificateData()
{
  return &certificatedata;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* CIPHERCERTIFICATEX509::GetTBSData()
* @brief      Get the exact DER TBSCertificate signed by the issuer
* @ingroup    CIPHER
*
* @return     XBUFFER* : TBSCertificate buffer.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* CIPHERCERTIFICATEX509::GetTBSData()
{
  return &tbsdata;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* CIPHERCERTIFICATEX509::GetSignature()
* @brief      Get the certificate signature
* @ingroup    CIPHER
*
* @return     XBUFFER* : Signature buffer.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* CIPHERCERTIFICATEX509::GetSignature()
{
  return &signature;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* CIPHERCERTIFICATEX509::GetIssuerData()
* @brief      Get the exact DER issuer name
* @ingroup    CIPHER
*
* @return     XBUFFER* : Issuer name buffer.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* CIPHERCERTIFICATEX509::GetIssuerData()
{
  return &issuerdata;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* CIPHERCERTIFICATEX509::GetSubjectData()
* @brief      Get the exact DER subject name
* @ingroup    CIPHER
*
* @return     XBUFFER* : Subject name buffer.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* CIPHERCERTIFICATEX509::GetSubjectData()
{
  return &subjectdata;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERCERTIFICATEX509::HasBasicConstraints()
* @brief      Check whether BasicConstraints is present
* @ingroup    CIPHER
*
* @return     bool : true if the extension is present; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::HasBasicConstraints()
{
  return hasbasicconstraints;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERCERTIFICATEX509::IsCertificateAuthority()
* @brief      Check whether BasicConstraints authorizes certificate signing
* @ingroup    CIPHER
*
* @return     bool : true if the certificate is a CA; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::IsCertificateAuthority()
{
  return hasbasicconstraints && iscertificateauthority;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int CIPHERCERTIFICATEX509::GetBasicConstraintsPathLength()
* @brief      Get the BasicConstraints path length
* @ingroup    CIPHER
*
* @return     int : Maximum subordinate CA count, or -1 if it is not limited.
*
* --------------------------------------------------------------------------------------------------------------------*/
int CIPHERCERTIFICATEX509::GetBasicConstraintsPathLength()
{
  return basicconstraintspathlength;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERCERTIFICATEX509::HasKeyUsage()
* @brief      Check whether KeyUsage is present
* @ingroup    CIPHER
*
* @return     bool : true if the extension is present; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::HasKeyUsage()
{
  return haskeyusage;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERCERTIFICATEX509::IsKeyUsageDigitalSignature()
* @brief      Check the digitalSignature KeyUsage bit
* @ingroup    CIPHER
*
* @return     bool : true if digital signatures are authorized; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::IsKeyUsageDigitalSignature()
{
  return keyusagedigitalsignature;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERCERTIFICATEX509::IsKeyUsageCertificateSign()
* @brief      Check the keyCertSign KeyUsage bit
* @ingroup    CIPHER
*
* @return     bool : true if certificate signing is authorized; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::IsKeyUsageCertificateSign()
{
  return keyusagecertificatesign;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERCERTIFICATEX509::HasExtendedKeyUsage()
* @brief      Check whether ExtendedKeyUsage is present
* @ingroup    CIPHER
*
* @return     bool : true if the extension is present; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::HasExtendedKeyUsage()
{
  return hasextendedkeyusage;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERCERTIFICATEX509::IsExtendedKeyUsageServerAuthentication()
* @brief      Check the id-kp-serverAuth ExtendedKeyUsage
* @ingroup    CIPHER
*
* @return     bool : true if TLS server authentication is authorized; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::IsExtendedKeyUsageServerAuthentication()
{
  return extendedkeyusageserverauthentication;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERCERTIFICATEX509::HasUnknownCriticalExtension()
* @brief      Check whether an unsupported critical extension was found
* @ingroup    CIPHER
*
* @return     bool : true if validation must reject the certificate; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::HasUnknownCriticalExtension()
{
  return hasunknowncriticalextension;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<XSTRING*>* CIPHERCERTIFICATEX509::GetSubjectAlternativeNamesDNS()
* @brief      Get decoded dNSName subject alternative names
* @ingroup    CIPHER
*
* @return     XVECTOR<XSTRING*>* : DNS name list.
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XSTRING*>* CIPHERCERTIFICATEX509::GetSubjectAlternativeNamesDNS()
{
  return &subjectalternativenamesDNS;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<XBUFFER*>* CIPHERCERTIFICATEX509::GetSubjectAlternativeNamesIP()
* @brief      Get decoded iPAddress subject alternative names
* @ingroup    CIPHER
*
* @return     XVECTOR<XBUFFER*>* : Binary IP address list.
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XBUFFER*>* CIPHERCERTIFICATEX509::GetSubjectAlternativeNamesIP()
{
  return &subjectalternativenamesIP;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERCERTIFICATEX509::IsServerNameValid(XCHAR* servername)
* @brief      Match a TLS server name against subjectAltName or the legacy commonName fallback
* @ingroup    CIPHER
*
* @param[in]  servername : Expected DNS name or IP address.
*
* @return     bool : true if the identity matches; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::IsServerNameValid(XCHAR* servername)
{
  XBUFFER binaryIP;

  if(!servername || !servername[0])
    {
      return false;
    }

  if(CIPHERCERTIFICATEX509_IPAddress(servername, binaryIP))
    {
      for(XDWORD c=0; c<subjectalternativenamesIP.GetSize(); c++)
        {
          XBUFFER* alternative = subjectalternativenamesIP.Get(c);
          if(alternative && alternative->Compare(binaryIP))
            {
              return true;
            }
        }

      return false;
    }

  for(XDWORD c=0; c<subjectalternativenamesDNS.GetSize(); c++)
    {
      XSTRING* alternative = subjectalternativenamesDNS.Get(c);
      if(alternative)
        {
          XSTRING pattern((*alternative));
          if(CIPHERCERTIFICATEX509_DNSName(pattern, servername))
            {
              return true;
            }
        }
    }

  if(hassubjectalternativename)
    {
      return false;
    }

  XSTRING commonname((*subjectID.GetCommonName()));
  return CIPHERCERTIFICATEX509_DNSName(commonname, servername);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERCERTIFICATEX509::VerifySignature(CIPHERKEY* issuerpublickey)
* @brief      Verify the X.509 signature with the issuer public key
* @ingroup    CIPHER
*
* @param[in]  issuerpublickey : Issuer public key.
*
* @return     bool : true if the signature is valid and its algorithm is accepted; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::VerifySignature(CIPHERKEY* issuerpublickey)
{
  if(!issuerpublickey || tbsdata.IsEmpty() || signature.IsEmpty())
    {
      return false;
    }

  if(issuerpublickey->GetType() == CIPHERKEYTYPE_RSA_PUBLIC)
    {
      CIPHERRSA RSA;
      if(!RSA.SetKey(issuerpublickey, true)) return false;

      switch(algorithmtype)
        {
          case CIPHERCERTIFICATEX509_ALGORITHM_TYPE_SHA256WITHRSAENCRYPTION : { HASHSHA2 hash(HASHSHA2TYPE_256);
                                                                                return RSA.Verify(tbsdata, signature, &hash, CIPHERRSAPKCS1VERSIONV15);
                                                                              }

          case CIPHERCERTIFICATEX509_ALGORITHM_TYPE_SHA384WITHRSAENCRYPTION : { HASHSHA2 hash(HASHSHA2TYPE_384);
                                                                                return RSA.Verify(tbsdata, signature, &hash, CIPHERRSAPKCS1VERSIONV15);
                                                                              }

          case CIPHERCERTIFICATEX509_ALGORITHM_TYPE_SHA512WITHRSAENCRYPTION : { HASHSHA2 hash(HASHSHA2TYPE_512);
                                                                                return RSA.Verify(tbsdata, signature, &hash, CIPHERRSAPKCS1VERSIONV15);
                                                                              }

          case CIPHERCERTIFICATEX509_ALGORITHM_TYPE_RSASSAPSS               : switch(RSASSAPSShashtype)
                                                                              {
                                                                                case CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_SHA256 : { HASHSHA2 hash(HASHSHA2TYPE_256);
                                                                                                                                          return RSA.Verify(tbsdata, signature, &hash,
                                                                                                                                                            CIPHERRSAPKCS1VERSIONV21,
                                                                                                                                                            RSASSAPSSsaltsize);
                                                                                                                                        }

                                                                                case CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_SHA384 : { HASHSHA2 hash(HASHSHA2TYPE_384);
                                                                                                                                          return RSA.Verify(tbsdata, signature, &hash,
                                                                                                                                                            CIPHERRSAPKCS1VERSIONV21,
                                                                                                                                                            RSASSAPSSsaltsize);
                                                                                                                                        }

                                                                                case CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_SHA512 : { HASHSHA2 hash(HASHSHA2TYPE_512);
                                                                                                                                          return RSA.Verify(tbsdata, signature, &hash,
                                                                                                                                                            CIPHERRSAPKCS1VERSIONV21,
                                                                                                                                                            RSASSAPSSsaltsize);
                                                                                                                                        }

                                                                                                                                default : break;
                                                                              }
                                                                              break;

                                                                        default : break;
        }
    }

  if((issuerpublickey->GetType() == CIPHERKEYTYPE_ECDSA_SECP256R1_PUBLIC) &&
     (algorithmtype == CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA256))
    {
      CIPHERECDSA ECDSA;
      HASHSHA2    hash(HASHSHA2TYPE_256);

      return ECDSA.SetKey(issuerpublickey, true) && ECDSA.Verify(tbsdata, signature, &hash);
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERCERTIFICATEX509::ConvertDateTime(XCHAR* datestr, XDATETIME* datetime)
* @brief      Convert date time
* @ingroup    CIPHER
*
* @param[in]  datestr : Datestr pointer to use.
* @param[in]  datetime : Datetime pointer to use.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::ConvertDateTime(XCHAR* datestr, XDATETIME* datetime)
{
  if(!datestr)
    {
      return false;
    }

  if(!datetime)
    {
      return false;
    }

  XSTRING year;
  XSTRING month;
  XSTRING day;

  XSTRING hour;
  XSTRING minutes;
  XSTRING seconds;

  XSTRING datestring;

  if(datestr[0] == __C('9'))
    {
      year = __L("19");    
    }
   else
    {
      year = __L("20");    
    }  

  year    += datestr[0];
  year    += datestr[1];

  month   += datestr[2];
  month   += datestr[3];

  day     += datestr[4];  
  day     += datestr[5];  

  hour    += datestr[6];  
  hour    += datestr[7];  

  minutes += datestr[8];  
  minutes += datestr[9];  

  seconds += datestr[10];  
  seconds += datestr[11];  

  datestring.Format(__L("%s/%s/%s %s:%s:%s"), year.Get(), month.Get(), day.Get(), hour.Get(), minutes.Get(), seconds.Get());

  datetime->GetDateTimeFromString(datestring, XDATETIME_FORMAT_POSTGRESQL); 

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCERTIFICATEX509::XTraceCertificatedPropertys()
* @brief      Xtrace certificated propertys
* @ingroup    CIPHER
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::XTraceCertificatedPropertys()
{ 
  XSTRING     string; 
  XSTRING     validdatestr;  
  XSTRING     selfsignedstr;  
  XDATETIME*  xdatetime = NULL;

  ////------------------------------------------------------------------

  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Cipher Key Certificate] -----------------------------------------------------------------------"));  
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L(""));    
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Version                      : %d "), GetVersion()); 

  for(XDWORD c=0; c<GetSerial()->GetSize(); c++)
    {
      string.AddFormat(__L("%02X "), GetSerial()->Get()[c]); 
    }

  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Serial                       : %s"), string.Get());
  
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Algorithm Identifier         : %s"), GetAlgorithmTypeStr()->Get());

  //------------------------------------------------------------------
  
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L(""));   
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Issuer ID"));   
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Contry name                  : %s"), GetIssuerID()->GetCountryName()->Get()); 
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Organization name            : %s"), GetIssuerID()->GetOrganizationName()->Get()); 
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Organization unit name       : %s"), GetIssuerID()->GetOrganizationalUnitName()->Get()); 
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Organization unit name plus  : %s"), GetIssuerID()->GetOrganizationalUnitNamePlus()->Get()); 
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Common name                  : %s"), GetIssuerID()->GetCommonName()->Get()); 

  //-----------------------------------------------------------------

  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L(""));   

  if(IsValidDates())
    {
      validdatestr = __L("Valid certificate");
    }
   else
    {
      validdatestr = __L("Expired certificate");
    }  
 
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Validity dates               : %s"), validdatestr.Get());

  xdatetime = GetDateNotBefore();
  if(xdatetime)
    {
      xdatetime->GetDateTimeToString(XDATETIME_FORMAT_STANDARD, string);
    }    
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("UTC Time Not Before          : %s"), string.Get());
  
  xdatetime = GetDateNotAfter();
  if(xdatetime)
    {
      xdatetime->GetDateTimeToString(XDATETIME_FORMAT_STANDARD, string);
    }  
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("UTC Time Not After           : %s"), string.Get() );

  //------------------------------------------------------------------

  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L(""));   

  if(IsSelfSigned())
    {
      selfsignedstr = __L("[This certificate is self-signed, Subject equal Issuer] ");
    }

  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Subject ID                     %s"), selfsignedstr.Get());   
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Contry name                  : %s"), GetSubjectID()->GetCountryName()->Get()); 
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Organization name            : %s"), GetSubjectID()->GetOrganizationName()->Get()); 
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Organization unit name       : %s"), GetSubjectID()->GetOrganizationalUnitName()->Get()); 
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Organization unit name plus  : %s"), GetSubjectID()->GetOrganizationalUnitNamePlus()->Get()); 
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("Common name                  : %s"), GetSubjectID()->GetCommonName()->Get()); 

  //------------------------------------------------------------------

  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("")); 

  bool havepubliccipherkey = false;
  if(GetPublicCipherKey())
    {
      havepubliccipherkey = (GetPublicCipherKey()->GetType() != CIPHERKEYTYPE_UNKNOWN)?true:false;
      if(havepubliccipherkey) 
        {
          havepubliccipherkey = IsPublicCipherKeyValid();
        }
    }  

  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE                                       , __L("Cipher key usage             : %s"), IsPublicCipherKeyUsage()?__L("true"):__L("false"));   
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE                                       , __L("Cipher key basic constraints : %s"), IsPublicCipherKeyBasicConstraints()?__L("true"):__L("false"));   
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE                                       , __L("Cipher key ID                : %s"), GetPublicCipherKeyID()->Get());   
  XTRACE_PRINTCOLOR((havepubliccipherkey?XTRACE_COLOR_BLUE:XTRACE_COLOR_RED), __L("Cipher key                   : %s [%s]"), (havepubliccipherkey?__L("Obtained and valid"):__L("Not valid")), (havepubliccipherkey?GetPublicCipherKey()->GetTypeStr():__L("")));   

  bool havehash = false;
  if(GetHash())
    {
      havehash = (GetHash()->GetType() != HASHTYPE_NONE)?true:false;
    }
 
  XTRACE_PRINTCOLOR((havehash?XTRACE_COLOR_BLUE:XTRACE_COLOR_RED), __L("Hash                         : %s [%s]"), (havehash?__L("Obtained and valid"):__L("Not valid")), havehash?GetHash()->GetTypeStr():__L(""));  

  //------------------------------------------------------------------
  
  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L(""));  

  return true;
}

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCERTIFICATEX509::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCERTIFICATEX509::Clean()
{                                         
  version                         = 0;
  algorithmtype                   = CIPHERCERTIFICATEX509_ALGORITHM_TYPE_UNKNOWN;
  algorithmtypestr.Empty();
  RSASSAPSShashtype               = CIPHERCERTIFICATEX509_RSASSAPSS_HASH_TYPE_UNKNOWN;
  RSASSAPSSsaltsize               = 0;

  publiccipherkeyusaged           = false;
  publiccipherkeybasicconstraints = false;
  publiccipherkeyvalid            = false;
  publiccipherkey                 = NULL; 

  hash                            = NULL;

  hasbasicconstraints             = false;
  iscertificateauthority          = false;
  basicconstraintspathlength      = -1;

  haskeyusage                     = false;
  keyusagedigitalsignature        = false;
  keyusagecertificatesign         = false;

  hasextendedkeyusage                   = false;
  extendedkeyusageserverauthentication  = false;
  hasunknowncriticalextension           = false;
  hassubjectalternativename              = false;
}
