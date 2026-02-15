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

#include "XFactory.h"
#include "XDateTime.h"
#include "XTrace.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



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
* @ingroup    CIPHER
* @note       VIRTUAL
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
* @return     XSTRING* : 
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
* @return     XSTRING* : 
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
* @return     XSTRING* : 
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
* @return     XSTRING* : 
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
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* CIPHERCERTIFICATEX509_ID::GetCommonName()
{
  return &commonname;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int CIPHERCERTIFICATEX509_ID::Compare(CIPHERCERTIFICATEX509_ID* certificateID, bool ignorecase)
* @brief      compare
* @ingroup    CIPHER
* 
* @param[in]  certificateID : 
* @param[in]  ignorecase : 
* 
* @return     int : 
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
* @param[in]  certificateID : 
* @param[in]  ignorecase : 
* 
* @return     int : 
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
* @param[in]  certificateID : 
* 
* @return     bool : true if is succesful. 
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
* @param[in]  certificateID : 
* 
* @return     bool : true if is succesful. 
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
* @param[in]  certificateID : 
* 
* @return     bool : true if is succesful. 
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
* @param[in]  certificateID : 
* 
* @return     bool : true if is succesful. 
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
* @ingroup    CIPHER
* @note       INTERNAL
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
      delete publiccipherkey;
    }

  if(hash)
    {
      delete hash;
    }

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWORD CIPHERCERTIFICATEX509::GetVersion()
* @brief      Get version
* @ingroup    CIPHER
* 
* @return     XDWORD : 
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
* @param[in]  version : 
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
* @return     XBUFFER* : 
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
* @return     CIPHERCERTIFICATEX509_ALGORITHM_TYPE : 
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
* @param[in]  OID : 
* 
* @return     bool : true if is succesful. 
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
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* CIPHERCERTIFICATEX509::GetAlgorithmTypeStr()
{
  return &algorithmtypestr;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERCERTIFICATEX509_ID* CIPHERCERTIFICATEX509::GetIssuerID()
* @brief      get issuer Id
* @ingroup    CIPHER
* 
* @return     CIPHERCERTIFICATEX509_ID* : 
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
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCERTIFICATEX509::IsValidDates()
{
  XDATETIME*  xdatetime_notbefore = GetDateNotBefore();
  XDATETIME*  xdatetime_notafter  = GetDateNotAfter();

  if(!xdatetime_notbefore || !xdatetime_notafter)
    {
      return false;
    }    

  XQWORD seconds_notbefore = xdatetime_notbefore->GetSeconsFromDate();
  XQWORD seconds_notafter  = xdatetime_notafter->GetSeconsFromDate();
  XQWORD seconds_actual    = 0;

  XDATETIME* xdatetime = GEN_XFACTORY.CreateDateTime();
  if(xdatetime)
    {
      xdatetime->Read();
      seconds_actual = xdatetime->GetSeconsFromDate();   

      GEN_XFACTORY.DeleteDateTime(xdatetime);
    }

  if((seconds_actual > seconds_notbefore) && (seconds_actual < seconds_notafter))
    {
      return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDATETIME* CIPHERCERTIFICATEX509::GetDateNotBefore()
* @brief      Get date not before
* @ingroup    CIPHER
* 
* @return     XDATETIME* : 
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
* @return     XDATETIME* : 
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
* @return     bool : true if is succesful. 
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
* @return     CIPHERCERTIFICATEX509_ID* : 
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
* @return     bool : true if is succesful. 
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
* @param[in]  isusage : 
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
* @return     bool : true if is succesful. 
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
* @param[in]  basicconstraints : 
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
* @return     bool : true if is succesful. 
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
* @param[in]  isvalid : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCERTIFICATEX509::SetPublicCipherKeyValid(bool isvalid)
{
  publiccipherkeyvalid = isvalid;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBUFFER* CIPHERCERTIFICATEX509::GetPublicCipherKeyID()
* @brief      get public cipher key Id
* @ingroup    CIPHER
* 
* @return     XBUFFER* : 
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
* @return     CIPHERKEY* : 
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
* @param[in]  publiccipherkey : 
* 
* @return     bool : true if is succesful. 
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
* @return     HASH* : 
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
* @param[in]  hash : 
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
* @return     XBUFFER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* CIPHERCERTIFICATEX509::GetHashData()
{
  return &hashdata;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCERTIFICATEX509::ConvertDateTime(XCHAR* datestr, XDATETIME* datetime)
* @brief      Convert date time
* @ingroup    CIPHER
* 
* @param[in]  datestr : 
* @param[in]  datetime : 
* 
* @return     bool : true if is succesful. 
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
* @return     bool : true if is succesful. 
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

  publiccipherkeyusaged           = false;
  publiccipherkeybasicconstraints = false;
  publiccipherkeyvalid            = false;
  publiccipherkey                 = NULL; 

  hash                            = NULL;
}




