/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       CipherKeyPrivateRSA.cpp
* 
* @class      CIPHERKEYPRIVATERSA
* @brief      Cipher Key Private RSA class
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
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "CipherKeyPrivateRSA.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERKEYPRIVATERSA::CIPHERKEYPRIVATERSA()
* @brief      Constructor
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERKEYPRIVATERSA::CIPHERKEYPRIVATERSA() : CIPHERKEY()
{
  Clean();

  type = CIPHERKEYTYPE_PRIVATE;

  prime1factor.Ini();
  prime2factor.Ini();
  exponent.Ini();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERKEYPRIVATERSA::~CIPHERKEYPRIVATERSA()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERKEYPRIVATERSA::~CIPHERKEYPRIVATERSA()
{
  prime1factor.End();
  prime2factor.End();
  exponent.End();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERKEYPRIVATERSA::Get(XMPINTEGER& prime1factor, XMPINTEGER& prime2factor, XMPINTEGER& exponent)
* @brief      Get
* @ingroup    CIPHER
* 
* @param[in]  prime1factor : 
* @param[in]  prime2factor : 
* @param[in]  exponent : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERKEYPRIVATERSA::Get(XMPINTEGER& prime1factor, XMPINTEGER& prime2factor, XMPINTEGER& exponent)
{
  prime1factor.CopyFrom(&this->prime1factor);
  prime2factor.CopyFrom(&this->prime2factor);
  exponent.CopyFrom(&this->exponent);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERKEYPRIVATERSA::Set(XMPINTEGER& prime1factor, XMPINTEGER& prime2factor, XMPINTEGER& exponent)
* @brief      Set
* @ingroup    CIPHER
* 
* @param[in]  prime1factor : 
* @param[in]  prime2factor : 
* @param[in]  exponent : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERKEYPRIVATERSA::Set(XMPINTEGER& prime1factor, XMPINTEGER& prime2factor, XMPINTEGER& exponent)
{
  this->prime1factor.CopyFrom(&prime1factor);
  this->prime2factor.CopyFrom(&prime2factor);
  this->exponent.CopyFrom(&exponent);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int CIPHERKEYPRIVATERSA::GetSizeInBytes()
* @brief      GetSizeInBytes
* @ingroup    CIPHER
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int CIPHERKEYPRIVATERSA::GetSizeInBytes()
{ 
  return this->exponent.GetSize();      
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERKEYPRIVATERSA::CopyFrom(CIPHERKEYPRIVATERSA* key)
* @brief      CopyFrom
* @ingroup    CIPHER
* 
* @param[in]  key : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERKEYPRIVATERSA::CopyFrom(CIPHERKEYPRIVATERSA* key)
{
  if(!key) return false;

  if(!CIPHERKEY::CopyFrom((CIPHERKEY*)key)) return false;

  return key->Get(prime1factor, prime2factor, exponent);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERKEYPRIVATERSA::Check(CIPHERKEYPUBLICRSA& publickey)
* @brief      Check
* @ingroup    CIPHER
* 
* @param[in]  publickey : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERKEYPRIVATERSA::Check(CIPHERKEYPUBLICRSA& publickey)
{
  XMPINTEGER PQ;
  XMPINTEGER DE;
  XMPINTEGER P1;
  XMPINTEGER Q1;
  XMPINTEGER H;
  XMPINTEGER I;
  XMPINTEGER G;
  XMPINTEGER G2;
  XMPINTEGER L1;
  XMPINTEGER L2;
  XMPINTEGER publicmodulus;
  XMPINTEGER publicexponent;

  if(!publickey.Check())  return false;

  if((!prime1factor.GetLimbs()) || (!prime2factor.GetLimbs()) || (!exponent.GetLimbs()))  return false;

  publickey.Get(publicmodulus, publicexponent);

  PQ.Ini();
  DE.Ini();
  P1.Ini();
  Q1.Ini();
  H.Ini();
  I.Ini();
  G.Ini();
  G2.Ini();
  L1.Ini();
  L2.Ini();

  if(!PQ.Multiplication(&prime1factor, &prime2factor))  return false;
  if(!DE.Multiplication(&exponent, &publicexponent))    return false;
  if(!P1.SubtractionSigned(&prime1factor, 1))           return false;
  if(!Q1.SubtractionSigned(&prime2factor, 1))           return false;
  if(!H.Multiplication(&P1, &Q1))                       return false;
  if(!G.GreatestCommonDivisor(&G, &publicexponent, &H)) return false;
  if(!G2.GreatestCommonDivisor(&G2, &P1, &Q1))          return false;
  if(!L1.Division(&L1, &L2, &H, &G2 ))                  return false;
  if(!I.Module(&I, &DE, &L1))                           return false;

  // Check for a valid PKCS1v2 private key
  if((PQ.CompareSignedValues(publicmodulus) != 0) ||
     (L2.CompareSignedValues(0)             != 0) ||
     (I.CompareSignedValues(1)              != 0) ||
     (G.CompareSignedValues(1)              != 0)) return false;

  PQ.End();
  DE.End();
  P1.End();
  Q1.End();
  H.End();
  I.End();
  G.End();
  G2.End();
  L1.End();
  L2.End();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERKEYPRIVATERSA::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERKEYPRIVATERSA::Clean()
{            

}


#pragma endregion



