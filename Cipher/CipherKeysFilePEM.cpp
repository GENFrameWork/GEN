/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       CipherKeysFilePEM.cpp
* 
* @class      CIPHERKEYSFILEPEM
* @brief      Cipher File Keys PEM (GEN Key File) class
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "CipherKeysFilePEM.h"

#include "XBuffer.h"
#include "XBER_XEvent.h"
#include "XBER.h"
#include "XFileTXT.h"
#include "XTrace.h"

#include "HashSHA1.h"
#include "HashSHA2.h"

#include "CipherKey.h"
#include "CipherKeyPublicRSA.h"
#include "CipherKeyECDSA.h"
#include "CipherCertificateX509.h"

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


#pragma region CLASS_CIPHERKEYSFILEPEM_ENTRYBUFFER


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERKEYSFILEPEM_ENTRYBUFFER::CIPHERKEYSFILEPEM_ENTRYBUFFER()
* @brief      Constructor of class
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERKEYSFILEPEM_ENTRYBUFFER::CIPHERKEYSFILEPEM_ENTRYBUFFER()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERKEYSFILEPEM_ENTRYBUFFER::~CIPHERKEYSFILEPEM_ENTRYBUFFER()
* @brief      Destructor of class
* @ingroup    CIPHER
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERKEYSFILEPEM_ENTRYBUFFER::~CIPHERKEYSFILEPEM_ENTRYBUFFER()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERKEYSFILEPEM_ENTRYBUFFER::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @ingroup    CIPHER
* @note       INTERNAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERKEYSFILEPEM_ENTRYBUFFER::Clean()
{
  type.Empty();

  ini_line = 0;
  end_line = 0;

  data.Empty();  
}


#pragma region CIPHERKEYSFILEPEM


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERKEYSFILEPEM::CIPHERKEYSFILEPEM()
* @brief      Constructor of class
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERKEYSFILEPEM::CIPHERKEYSFILEPEM()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERKEYSFILEPEM::~CIPHERKEYSFILEPEM()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERKEYSFILEPEM::~CIPHERKEYSFILEPEM()
{
  Key_DelAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERKEYSFILEPEM::Key_Add(CIPHERKEY* key)
* @brief      Key add
* @ingroup    CIPHER
* 
* @param[in]  key : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERKEYSFILEPEM::Key_Add(CIPHERKEY* key)
{
  if(!key)
    {
      return false;
    }

  return keys.Add(key);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERKEYSFILEPEM::Key_Del(CIPHERKEY* key)
* @brief      Key del
* @ingroup    CIPHER
* 
* @param[in]  key : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERKEYSFILEPEM::Key_Del(CIPHERKEY* key)
{
  if(!key)
    {
      return false;
    }

  keys.Delete(key);
  delete key;
 
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERKEYSFILEPEM::Key_DelAll()
* @brief      Key del all
* @ingroup    CIPHER
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERKEYSFILEPEM::Key_DelAll()
{
  if(keys.IsEmpty()) 
    {
      return false;
    }

  keys.DeleteContents();
  keys.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERKEYSFILEPEM::DecodeCertificates(XVECTOR<XSTRING*>* lines)
* @brief      decode certificates
* @ingroup    CIPHER
* 
* @param[in]  lines : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERKEYSFILEPEM::DecodeCertificates(XVECTOR<XSTRING*>* lines)
{
  XVECTOR<CIPHERKEYSFILEPEM_ENTRYBUFFER*> entrysbuffer;
  CIPHERKEYSFILEPEM_ENTRYBUFFER*          entrybuffer = NULL;


  ndecodeobj = 0;
    
  for(XDWORD c=0; c<lines->GetSize(); c++)
    {
      XSTRING line;      
      int     ini = XSTRING_NOTFOUND;
      int     end = XSTRING_NOTFOUND;    

      if(lines->Get(c))
        {
          line = lines->Get(c)->Get();
        }

      if(line.IsEmpty())
        {
          continue;
        }

      ini = line.Find(CIPHERKEYSFILEPEM_BEGINKEY, true);
      if(ini != XSTRING_NOTFOUND)
        {
          ini += XSTRING::GetSize(CIPHERKEYSFILEPEM_BEGINKEY);

          if(!entrybuffer)
            {
              entrybuffer = new CIPHERKEYSFILEPEM_ENTRYBUFFER(); 
              if(entrybuffer)
                {
                  entrybuffer->end_line = XSTRING_NOTFOUND;
                  entrybuffer->ini_line = c+1;  
                  end = line.Find(CIPHERKEYSFILEPEM_FINISH, true, ini);
                  if(end != XSTRING_NOTFOUND)
                    {
                      entrybuffer->type.AdjustSize(_MAXSTR);                                                                  
                      line.Copy(ini, end, entrybuffer->type);
                      entrybuffer->type.AdjustSize();                
                    }
                   else
                    {
                      delete entrybuffer;
                      entrybuffer = NULL;
                    }
                }
            }
        }

      ini = line.Find(CIPHERKEYSFILEPEM_ENDKEY, true);
      if(ini != XSTRING_NOTFOUND)
        {
          ini += XSTRING::GetSize(CIPHERKEYSFILEPEM_ENDKEY);

          if(entrybuffer)
            {
              entrybuffer->end_line = c;                          
              end = line.Find(CIPHERKEYSFILEPEM_FINISH, true, ini);
              if(end != XSTRING_NOTFOUND)
                {
                  XSTRING type;  

                  type.AdjustSize(_MAXSTR);                                                                  
                  line.Copy(ini, end, type);
                  type.AdjustSize();  

                  if(!type.Compare(entrybuffer->type))
                    {
                      entrysbuffer.Add(entrybuffer);  
                      entrybuffer = NULL;
                    }
                   else
                    {
                      delete entrybuffer;
                      entrybuffer = NULL;
                    }                                          
                }
               else
                {
                  delete entrybuffer;
                  entrybuffer = NULL;
                }         
            }  
        }                
    }


  for(XDWORD c=0; c<entrysbuffer.GetSize(); c++)
    {
      CIPHERKEYSFILEPEM_ENTRYBUFFER* entrybuffer = entrysbuffer.Get(c);
      if(entrybuffer)
        {
          XSTRING datastr;
          XSTRING datastr1;
        
          for(int d=entrybuffer->ini_line; d<entrybuffer->end_line; d++)
            {
              datastr += lines->Get(d)->Get();                                 
            }

          if(!entrybuffer->data.ConvertFromBase64(datastr))
            {
              XFEEDBACK_ADD(XFEEDBACK_CODE_ERRORINFUNCTION , 1, __L("CIPHERKEYSFILEPEM::DecodeCertificates invalid Convert From Base64"));
              return false;
            }      
        }
    }

  
  XASN1 asn1;
    
  for(XDWORD c=0; c<entrysbuffer.GetSize(); c++)
    {
      CIPHERKEYSFILEPEM_ENTRYBUFFER* entrybuffer = entrysbuffer.Get(c);
      if(entrybuffer)
        {
          decodeobj = NULL;
         
          if(!entrybuffer->type.Compare(__L("CERTIFICATE")))
            {      
              decodeobjtype = CIPHERKEYSFILETYPE_CERTIFICATEX509;     

              decodeobj = (CIPHERKEY*)new CIPHERCERTIFICATEX509();  
              if(decodeobj)
                {                  
                  if(asn1.Decode(entrybuffer->data, this))
                    {
                      Key_Add((CIPHERKEY*)decodeobj);  

                      CIPHERCERTIFICATEX509* certificate = (CIPHERCERTIFICATEX509*)decodeobj;                      
                      if(certificate)
                        {
                          certificate->XTraceCertificatedPropertys();
                        }
                    }                 
                   else                
                    {
                      delete (CIPHERCERTIFICATEX509*)decodeobj;
                    }                   
                } 
            }         
        }  
    }
  

  entrysbuffer.DeleteContents();
  entrysbuffer.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERKEYSFILEPEM::GetCertificatedPropertys(CIPHERKEYCERTIFICATE* certificate, XBER_XEVENT* event)
* @brief      get certificated propertys
* @ingroup    CIPHER
* 
* @param[in]  certificate : 
* @param[in]  event : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERKEYSFILEPEM::GetCertificatedPropertys(CIPHERCERTIFICATEX509* certificate, XBER_XEVENT* event)
{
  if(!certificate)
    {
      return false;
    }

  if(!event)
    {
      return false;
    }

  XSTRING   levelsstr;
  bool      ismanaged =  false;     
  XVARIANT* value     = (XVARIANT*)event->GetValue();

  levelsstr = event->GetLevelsString()->Get();           

  if(event->GetTagType() == XBER_TAGTYPE_OBJECT_IDENTIFIER)
    {
      if(event->GetProperty())
        {
          lastOID = event->GetProperty()->OID; 

          if(!levelsstr.Compare(__L("1.1.3.1")))
            {
              if(event->GetProperty())  
                {
                  certificate->SetAlgorithmType(event->GetProperty()->OID);
                  ismanaged = true;  
                }
            }
        }
    }
   else
    {       
      if(value)
        {

          // ---------------------------------------------------------------------------------

          if(!levelsstr.Compare(__L("1.1.1.1")))
            {
              certificate->SetVersion((XWORD)event->GetData()->Get()[0] + 1);                  
              ismanaged = true;
            }

          if(!levelsstr.Compare(__L("1.1.2")))
            {
              certificate->GetSerial()->CopyFrom((*event->GetData()));
              ismanaged = true;  
            }
 
          // ---------------------------------------------------------------------------------

          if(!lastOID.Compare(__L("2.5.4.6"), false))
            {
              if(certificate->GetIssuerID()->GetCountryName()->IsEmpty())  
                {
                  certificate->GetIssuerID()->GetCountryName()->Set((XSTRING)(*value));  
                }
               else 
                {
                  certificate->GetSubjectID()->GetCountryName()->Set((XSTRING)(*value));  
                }

              ismanaged = true;
            }

          if(!lastOID.Compare(__L("2.5.4.10"), false))
            {              
              if(certificate->GetIssuerID()->GetOrganizationName()->IsEmpty())  
                {
                  certificate->GetIssuerID()->GetOrganizationName()->Set((XSTRING)(*value));  
                }
               else 
                {
                  certificate->GetSubjectID()->GetOrganizationName()->Set((XSTRING)(*value));  
                }

              ismanaged = true; 
            }

          if(!lastOID.Compare(__L("2.5.4.11"), false))
            {              
              if(certificate->GetIssuerID()->GetOrganizationalUnitName()->IsEmpty())  
                {
                  certificate->GetIssuerID()->GetOrganizationalUnitName()->Set((XSTRING)(*value));  
                }
               else 
                {
                  certificate->GetSubjectID()->GetOrganizationalUnitName()->Set((XSTRING)(*value));  
                }

              ismanaged = true; 
            }

          if(!lastOID.Compare(__L("2.5.4.11"), false))
            {             
              if(certificate->GetIssuerID()->GetOrganizationalUnitNamePlus()->IsEmpty())  
                {
                  certificate->GetIssuerID()->GetOrganizationalUnitNamePlus()->Set((XSTRING)(*value));  
                }
               else 
                {
                  certificate->GetSubjectID()->GetOrganizationalUnitNamePlus()->Set((XSTRING)(*value));  
                }

              ismanaged = true; 
            }

          if(!lastOID.Compare(__L("2.5.4.3"), false))
            {              
              if(certificate->GetIssuerID()->GetCommonName()->IsEmpty())  
                {
                  certificate->GetIssuerID()->GetCommonName()->Set((XSTRING)(*value));  
                }
               else 
                {
                  certificate->GetSubjectID()->GetCommonName()->Set((XSTRING)(*value));  
                }

              ismanaged = true; 
            }   

          // ---------------------------------------------------------------------------------  

          if(!levelsstr.Compare(__L("1.1.5.1")))
            {
              XVARIANT* value;
              XSTRING   valuestr;  

              value     = (XVARIANT*)event->GetValue();
              valuestr  = (XSTRING)(*value);

              certificate->ConvertDateTime(valuestr.Get(), certificate->GetDateNotBefore());   
              ismanaged = true;
            }

          if(!levelsstr.Compare(__L("1.1.5.2")))
            {
              XVARIANT* value;
              XSTRING   valuestr;  

              value     = (XVARIANT*)event->GetValue();
              valuestr  = (XSTRING)(*value);

              certificate->ConvertDateTime(valuestr.Get(), certificate->GetDateNotAfter());       
              ismanaged = true;
            }
        
          // ---------------------------------------------------------------------------------
          //  Cipher Key

          static CIPHERKEYTYPE keytype = CIPHERKEYTYPE_UNKNOWN;

          // RSA
          if(!lastOID.Compare(__L("1.2.840.113549.1.1.1"), false))
            {
              keytype = CIPHERKEYTYPE_RSA_PUBLIC;
            }

          // NIST / SECG   

          // ECDSA secp192r1	        
          if(!lastOID.Compare(__L("1.2.840.10045.3.1.1"), false))
            {
              keytype = CIPHERKEYTYPE_ECDSA_SECP192R1_PUBLIC;
            }
          
          // ECDSA secp224r1	        
          if(!lastOID.Compare(__L("1.3.132.0.33"), false))	
            {
              keytype = CIPHERKEYTYPE_ECDSA_SECP224R1_PUBLIC;
            }

          // ECDSA secp256r1	        
          if(!lastOID.Compare(__L("1.2.840.10045.3.1.7"), false))
            {
              keytype = CIPHERKEYTYPE_ECDSA_SECP256R1_PUBLIC;
            }
          
          // ECDSA secp384r1	        
          if(!lastOID.Compare(__L("1.3.132.0.34"), false))
            {
              keytype = CIPHERKEYTYPE_ECDSA_SECP384R1_PUBLIC;
            }

          // ECDSA secp521r1	        
          if(!lastOID.Compare(__L("1.3.132.0.35"), false))	   
            {
              keytype = CIPHERKEYTYPE_ECDSA_SECP521R1_PUBLIC;
            }

          // SECG (special)

          // ECDSA secp256k1	        
          if(!lastOID.Compare(__L("1.3.132.0.10"), false))
            {
              keytype = CIPHERKEYTYPE_ECDSA_SECP256K1_PUBLIC;
            }

          // ECDSA sect163k1	        
          if(!lastOID.Compare(__L("1.3.132.0.1"), false))
            {
              keytype = CIPHERKEYTYPE_ECDSA_SECT163K1_PUBLIC;
            }  

          // ECDSA sect233k1	        
          if(!lastOID.Compare(__L("1.3.132.0.26"), false))
            {
              keytype = CIPHERKEYTYPE_ECDSA_SECT233K1_PUBLIC;
            }

          // Brainpool 

          // ECDSA brainpoolP256r1	  
          if(!lastOID.Compare(__L("1.3.36.3.3.2.8.1.1.7"), false))
            {
              keytype = CIPHERKEYTYPE_ECDSA_BRAINPOOLP256R1_PUBLIC;
            }

          // ECDSA brainpoolP384r1	  
          if(!lastOID.Compare(__L("1.3.36.3.3.2.8.1.1.11"), false))
            {
              keytype = CIPHERKEYTYPE_ECDSA_BRAINPOOLP384R1_PUBLIC;
            }

          // ECDSA brainpoolP512r1	  
          if(!lastOID.Compare(__L("1.3.36.3.3.2.8.1.1.13"), false))	
            {
              keytype = CIPHERKEYTYPE_ECDSA_BRAINPOOLP512R1_PUBLIC;
            }

          switch(keytype)
            {   
              case CIPHERKEYTYPE_RSA_PUBLIC                   : { static XMPINTEGER modulus; 
                                                                  static XMPINTEGER exponent;

                                                                  // RSA key 
                                                                  if(event->GetTagType() == XBER_TAGTYPE_INTEGER)
                                                                    {                 
                                                                      if(!certificate->GetPublicCipherKey())
                                                                        {
                                                                          CIPHERKEYPUBLICRSA* cipherkey =  new CIPHERKEYPUBLICRSA();
                                                                          if(cipherkey)
                                                                            {
                                                                              modulus.ImportFromBinary(event->GetData()->Get(), event->GetData()->GetSize());
                          
                                                                              certificate->SetPublicCipherKey((CIPHERKEY*)cipherkey);
                                                                            } 
                                                                        }         
                                                                       else
                                                                        {
                                                                          CIPHERKEYPUBLICRSA* cipherkey = (CIPHERKEYPUBLICRSA*)certificate->GetPublicCipherKey();
                                                                          if(cipherkey)
                                                                            {       
                                                                              exponent.ImportFromBinary(event->GetData()->Get(), event->GetData()->GetSize());                    
                                                                              cipherkey->Set(modulus, exponent);

                                                                              modulus.End();
                                                                              exponent.End();

                                                                              certificate->SetPublicCipherKeyValid(cipherkey->Check());

                                                                              keytype = CIPHERKEYTYPE_UNKNOWN;

                                                                              ismanaged = true; 
                                                                            }
                                                                        } 
                                                                    }
                                                                }
                                                                break; 

              case CIPHERKEYTYPE_ECDSA_SECP192R1_PUBLIC	      :
              case CIPHERKEYTYPE_ECDSA_SECP224R1_PUBLIC	      :
              case CIPHERKEYTYPE_ECDSA_SECP256R1_PUBLIC	      :
              case CIPHERKEYTYPE_ECDSA_SECP384R1_PUBLIC	      :
              case CIPHERKEYTYPE_ECDSA_SECP521R1_PUBLIC	      :
              case CIPHERKEYTYPE_ECDSA_SECP256K1_PUBLIC	      :
              case CIPHERKEYTYPE_ECDSA_SECT163K1_PUBLIC	      :
              case CIPHERKEYTYPE_ECDSA_SECT233K1_PUBLIC	      :
              case CIPHERKEYTYPE_ECDSA_BRAINPOOLP256R1_PUBLIC	:
              case CIPHERKEYTYPE_ECDSA_BRAINPOOLP384R1_PUBLIC	:
              case CIPHERKEYTYPE_ECDSA_BRAINPOOLP512R1_PUBLIC	: if((event->GetTagType() == XBER_TAGTYPE_BIT_STRING) && (keytype != CIPHERKEYTYPE_UNKNOWN))
                                                                  {
                                                                    CIPHERKEYECDSA* cipherkey = new CIPHERKEYECDSA();
                                                                    if(cipherkey)
                                                                      {
                                                                        cipherkey->SetType(keytype);
                                                                        cipherkey->Set((*event->GetData()));
                                                                        
                                                                        certificate->SetPublicCipherKey((CIPHERKEY*)cipherkey);
                                                                        certificate->SetPublicCipherKeyValid(true);

                                                                        keytype = CIPHERKEYTYPE_UNKNOWN;

                                                                        ismanaged = true; 
                                                                      }
                                                                  }
                                                                break; 
            }                                                
                      
          // ---------------------------------------------------------------------------------  
          // Hash 
  
          HASH*           hash     = NULL;
          static HASHTYPE hashtype = HASHTYPE_NONE;
      
          // RSA SHA1
          if(!lastOID.Compare(__L("1.2.840.113549.1.1.5"), false))
            {
              hashtype = HASHTYPE_SHA1; 
            }

          // RSA SHA256
          if(!lastOID.Compare(__L("1.2.840.113549.1.1.11"), false))
            {
              hashtype = HASHTYPE_SHA256; 
            }

          // RSA SHA384
          if(!lastOID.Compare(__L("1.2.840.113549.1.1.12"), false))
            {
              hashtype = HASHTYPE_SHA384; 
            }

          // RSA SHA512
          if(!lastOID.Compare(__L("1.2.840.113549.1.1.13"), false))
            {
              hashtype = HASHTYPE_SHA512; 
            }

          // ECDSA SHA256
          if(!lastOID.Compare(__L("1.2.840.10045.4.3.2"), false))
            {
              hashtype = HASHTYPE_SHA256;                       
            }

          // ECDSA SHA384
          if(!lastOID.Compare(__L("1.2.840.10045.4.3.3"), false))
            {
              hashtype = HASHTYPE_SHA384;              
            }

          // ECDSA SHA512
          if(!lastOID.Compare(__L("1.2.840.10045.4.3.4"), false))
            {
              hashtype = HASHTYPE_SHA512;              
            }

          if((event->GetTagType() == XBER_TAGTYPE_BIT_STRING) && (hashtype != HASHTYPE_NONE))
            { 
              switch(hashtype)
                { 
                  case HASHTYPE_SHA1   : hash  = new HASHSHA1();                  break;
                  case HASHTYPE_SHA224 : hash  = new HASHSHA2(HASHSHA2TYPE_224);  break;      
                  case HASHTYPE_SHA256 : hash  = new HASHSHA2(HASHSHA2TYPE_256);  break;
                  case HASHTYPE_SHA384 : hash  = new HASHSHA2(HASHSHA2TYPE_384);  break;
                  case HASHTYPE_SHA512 : hash  = new HASHSHA2(HASHSHA2TYPE_512);  break;                   
                              default  : break;
                }            

              hashtype = HASHTYPE_NONE;

              if(hash)
                {
                  certificate->GetHashData()->Set(event->GetData()->Get(), event->GetData()->GetSize());
                  certificate->SetHash(hash);
                  

                  ismanaged = true; 
                }
            }

          // ---------------------------------------------------------------------------------  

        }
    }

  if(ismanaged)
    {
      lastOID.Empty();
    }


  return ismanaged;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERKEYSFILEPEM::HandleEvent_XBER(XBER_XEVENT* event)
* @brief      Handle event XBER
* @note       INTERNAL
* @ingroup    CIPHER
* 
* @param[in]  event : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERKEYSFILEPEM::HandleEvent_XBER(XBER_XEVENT* event)
{  
  if(!decodeobj)
    {
      return;
    }
    
  switch(event->GetEventType())
    {
      case XBERXEVENT_TYPE_DECODE_START   : //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Cipher Keys File PEM] Start"));                                            
                                            break;

      case XBERXEVENT_TYPE_DECODE_DATA    : { bool  ismanaged = false;
                                                                                                                                            
                                              if(decodeobjtype == CIPHERKEYSFILETYPE_CERTIFICATEX509)
                                                {
                                                  CIPHERCERTIFICATEX509* certificate = (CIPHERCERTIFICATEX509*)decodeobj;
                                                  if(certificate)
                                                    {    
                                                      ismanaged = GetCertificatedPropertys(certificate, event);
                                                    }
                                                } 

                                              
                                              XSTRING levelsstr;

                                              levelsstr = event->GetLevelsString()->Get();                                              
                                              levelsstr.AddFormat(__L(" %s"), event->GetLine()->Get());  

                                              //XTRACE_PRINTCOLOR((ismanaged?XTRACE_COLOR_BLUE:XTRACE_COLOR_BLACK), levelsstr.Get(), NULL);                                                                                                      
                                            }                                                                                       
                                            break;

      case XBERXEVENT_TYPE_DECODE_END     : //XTRACE_PRINTCOLOR((event->GetStatus()?XTRACE_COLOR_BLUE:XTRACE_COLOR_RED), __L("[Cipher Keys File PEM] End"));   
                                            ndecodeobj++;
                                            break;

    } 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERKEYSFILEPEM::HandleEvent(XEVENT* xevent)
* @brief      Handle event
* @note       INTERNAL
* @ingroup    CIPHER
* 
* @param[in]  xevent : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERKEYSFILEPEM::HandleEvent(XEVENT* xevent)
{
  if(!xevent) return;

  switch(xevent->GetEventFamily())
    {
      case XEVENT_TYPE_XBER             : { XBER_XEVENT* event = (XBER_XEVENT*)xevent;
                                            if(!event) return;

                                            HandleEvent_XBER(event);
                                          }
                                          break;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERKEYSFILEPEM::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERKEYSFILEPEM::Clean()
{  
  decodeobj     = NULL;
  decodeobjtype = CIPHERKEYTYPE_UNKNOWN;
  ndecodeobj    = 0;
}


#pragma endregion


#pragma endregion

