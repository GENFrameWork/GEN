/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       CipherKeyCertificate.h
* 
* @class      CIPHERCERTIFICATEX509
* @brief      Cipher Certificate X.509 class
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

#ifndef _CIPHERCERTIFICATEX509_H_
#define _CIPHERCERTIFICATEX509_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XString.h"
#include "XBuffer.h"
#include "XDateTime.h"
#include "XMPInteger.h"

#include "CipherKey.h"
#include "Hash.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


enum CIPHERCERTIFICATEX509_ALGORITHM_TYPE
{
  CIPHERCERTIFICATEX509_ALGORITHM_TYPE_UNKNOWN                       = 0x00   ,

  CIPHERCERTIFICATEX509_ALGORITHM_TYPE_SHA1WITHRSAENCRYPTION	                ,
  CIPHERCERTIFICATEX509_ALGORITHM_TYPE_SHA256WITHRSAENCRYPTION	              ,  
  CIPHERCERTIFICATEX509_ALGORITHM_TYPE_SHA384WITHRSAENCRYPTION	              ,
  CIPHERCERTIFICATEX509_ALGORITHM_TYPE_SHA512WITHRSAENCRYPTION                ,	
  CIPHERCERTIFICATEX509_ALGORITHM_TYPE_MD5WITHRSAENCRYPTION 	                , 

  CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA1	                        ,
  CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA256	                      ,
  CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA384	                      ,
  CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA512	                      , 

  CIPHERCERTIFICATEX509_ALGORITHM_TYPE_DSAWITHSHA1	                          ,
  CIPHERCERTIFICATEX509_ALGORITHM_TYPE_DSAWITHSHA256	                        ,  

  CIPHERCERTIFICATEX509_ALGORITHM_TYPE_RSASSAPSS	                                
};


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class CIPHERCERTIFICATEX509_ID 
{
  public:
                                            CIPHERCERTIFICATEX509_ID          ();
    virtual                                ~CIPHERCERTIFICATEX509_ID          ();

    XSTRING*                                GetCountryName                    ();    
    XSTRING*                                GetOrganizationName               ();
    XSTRING*                                GetOrganizationalUnitName         ();
    XSTRING*                                GetOrganizationalUnitNamePlus     ();
    XSTRING*                                GetCommonName                     ();

    int                                     Compare                           (CIPHERCERTIFICATEX509_ID* certificateID, bool ignorecase = false);
    int                                     Compare                           (CIPHERCERTIFICATEX509_ID& certificateID, bool ignorecase = false);

    bool                                    CopyTo                            (CIPHERCERTIFICATEX509_ID* certificateID);
    bool                                    CopyTo                            (CIPHERCERTIFICATEX509_ID& certificateID);

    bool                                    CopyFrom                          (CIPHERCERTIFICATEX509_ID* certificateID);
    bool                                    CopyFrom                          (CIPHERCERTIFICATEX509_ID& certificateID);

  private:

    void                                    Clean                             ();  

    XSTRING                                 countryname;    
    XSTRING                                 organizationame;
    XSTRING                                 organizationalunitname;
    XSTRING                                 organizationalunitnameplus;
    XSTRING                                 commonname;
};


class CIPHERCERTIFICATEX509 
{
  public:
                                            CIPHERCERTIFICATEX509             ();
    virtual                                ~CIPHERCERTIFICATEX509             ();

    XWORD                                   GetVersion                        ();
    void                                    SetVersion                        (XWORD version);

    XBUFFER*                                GetSerial                         ();  

    CIPHERCERTIFICATEX509_ALGORITHM_TYPE    GetAlgorithmType                  ();
    bool                                    SetAlgorithmType                  (XCHAR* OID);
    XSTRING*                                GetAlgorithmTypeStr               ();

    CIPHERCERTIFICATEX509_ID*               GetIssuerID                       ();  

    bool                                    IsValidDates                      ();
    XDATETIME*                              GetDateNotBefore                  ();
    XDATETIME*                              GetDateNotAfter                   ();

    bool                                    IsSelfSigned                      ();
  
    CIPHERCERTIFICATEX509_ID*               GetSubjectID                      ();

    bool                                    IsPublicCipherKeyValid            ();
    void                                    SetPublicCipherKeyValid           (bool isvalid);

    CIPHERKEY*                              GetPublicCipherKey                ();
    bool                                    SetPublicCipherKey                (CIPHERKEY* publiccipherkey);

    HASH*                                   GetHash                           ();  
    void                                    SetHash                           (HASH* hash);
    XBUFFER*                                GetHashData                       ();

    bool                                    ConvertDateTime                   (XCHAR* datestr, XDATETIME* datetime);

    bool                                    XTraceCertificatedPropertys       ();
    
  private:
  
    void                                    Clean                             ();  

    XWORD                                   version;
    XBUFFER                                 serial;

    CIPHERCERTIFICATEX509_ALGORITHM_TYPE    algorithmtype;
    XSTRING                                 algorithmtypestr;

    CIPHERCERTIFICATEX509_ID                issuerID;
    
    XDATETIME                               datenotbefore;
    XDATETIME                               datenotafter;

    CIPHERCERTIFICATEX509_ID                subjectID;
 
    bool                                    publiccipherkeyvalid;
    CIPHERKEY*                              publiccipherkey; 

    HASH*                                   hash;
    XBUFFER                                 hashdata;
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif


