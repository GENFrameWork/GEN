/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       CipherCurve25519.cpp
* 
* @class      CIPHERCURVE25519
* @brief      Cipher Curve25519 class
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

#include "CipherCurve25519.h"

#include <stdio.h>
#include <string.h>

#include "XBase.h"
#include "XFactory.h"
#include "XRand.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

const XBYTE CIPHERCURVE25519::basepoint[CIPHERCURVE25519_MAXKEY] = { 9 };

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERCURVE25519::CIPHERCURVE25519()
* @brief      Constructor
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERCURVE25519::CIPHERCURVE25519()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERCURVE25519::~CIPHERCURVE25519()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERCURVE25519::~CIPHERCURVE25519()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCURVE25519::GenerateRandomPrivateKey()
* @brief      GenerateRandomPrivateKey
* @ingroup    CIPHER
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCURVE25519::GenerateRandomPrivateKey()
{
  XRAND* xrand = GEN_XFACTORY.CreateRand();
  if(!xrand) return false;

  for(int c=0; c<CIPHERCURVE25519_MAXKEY; c++)
    {
      keys[CIPHERCURVE25519_TYPEKEY_PRIVATE][c] = xrand->Max(255);      
    }

  GEN_XFACTORY.DeleteRand(xrand);

  return IsKeyCreated(CIPHERCURVE25519_TYPEKEY_PRIVATE);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCURVE25519::CreatePublicKey()
* @brief      CreatePublicKey
* @ingroup    CIPHER
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCURVE25519::CreatePublicKey()
{
  curve25519(keys[CIPHERCURVE25519_TYPEKEY_PUBLIC], keys[CIPHERCURVE25519_TYPEKEY_PRIVATE], NULL);

  return IsKeyCreated(CIPHERCURVE25519_TYPEKEY_PUBLIC);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCURVE25519::CreateSharedKey(XBYTE publickey[CIPHERCURVE25519_MAXKEY])
* @brief      CreateSharedKey
* @ingroup    CIPHER
* 
* @param[in]  publickey[CIPHERCURVE25519_MAXKEY] : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCURVE25519::CreateSharedKey(XBYTE publickey[CIPHERCURVE25519_MAXKEY])
{
  curve25519(keys[CIPHERCURVE25519_TYPEKEY_SHARED], keys[CIPHERCURVE25519_TYPEKEY_PRIVATE], publickey);

  return IsKeyCreated(CIPHERCURVE25519_TYPEKEY_SHARED);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE* CIPHERCURVE25519::GetKey(CIPHERCURVE25519_TYPEKEY typekey)
* @brief      GetKey
* @ingroup    CIPHER
* 
* @param[in]  typekey : 
* 
* @return     XBYTE* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE* CIPHERCURVE25519::GetKey(CIPHERCURVE25519_TYPEKEY typekey)
{
  if(!IsKeyCreated(typekey))  return NULL;

  return keys[typekey];
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERCURVE25519::IsKeyCreated(CIPHERCURVE25519_TYPEKEY typekey)
* @brief      IsKeyCreated
* @ingroup    CIPHER
* 
* @param[in]  typekey : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERCURVE25519::IsKeyCreated(CIPHERCURVE25519_TYPEKEY typekey)
{
  for(int c=0; c<CIPHERCURVE25519_MAXKEY; c++)
    {
      if(keys[typekey][c])  return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::CleanAllKeys()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::CleanAllKeys()
{
  for(int c=0; c<CIPHERCURVE25519_MAXKEYS; c++)
    {
      memset(keys[c], 0, CIPHERCURVE25519_MAXKEY);
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::fsum(XQWORDSIG* output, const XQWORDSIG* in)
* @brief      fsum    Sum two numbers: output += in
*                     Field element representation:
*
*                     Field elements are written as an array of signed, 64-bit limbs, least significant first. 
*                     The value of the field element is: x[0] + 2^26·x[1] + x^51·x[2] + 2^102·x[3] + ...
*                     i.e. the limbs are 26, 25, 26, 25, ... bits wide.
* @ingroup    CIPHER
* 
* @param[in]  output : 
* @param[in]  in : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::fsum(XQWORDSIG* output, const XQWORDSIG* in) 
{
  XDWORD i;

  for(i=0; i<10; i+=2) 
    {
      output[0+i] = (output[0+i] + in[0+i]);
      output[1+i] = (output[1+i] + in[1+i]);
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::fdifference(XQWORDSIG* output, const XQWORDSIG* in)
* @brief      fdifference Find the difference of two numbers: output = in - output (note the order of the arguments!)
* @ingroup    CIPHER
* 
* @param[in]  output : 
* @param[in]  in : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::fdifference(XQWORDSIG* output, const XQWORDSIG* in) 
{
  XDWORD i;

  for(i=0; i<10; ++i) 
    {
      output[i] = (in[i] - output[i]);
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::fscalar_product(XQWORDSIG* output, const XQWORDSIG* in, const XQWORDSIG scalar)
* @brief      fscalar_product  Multiply a number by a scalar: output = in * scalar
* @ingroup    CIPHER
* 
* @param[in]  output : 
* @param[in]  in : 
* @param[in]  scalar : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::fscalar_product(XQWORDSIG* output, const XQWORDSIG* in, const XQWORDSIG scalar) 
{
  XDWORD i;

  for(i=0; i< 10; ++i) 
    {
      output[i] = in[i] * scalar;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::fproduct(XQWORDSIG* output, const XQWORDSIG* in2, const XQWORDSIG* in)
* @brief      fproduct  Multiply two numbers: output = in2 * in output must be distinct to both inputs. 
*                       The inputs are reduced coefficient form, the output is not.
* @ingroup    CIPHER
* 
* @param[in]  output : 
* @param[in]  in2 : 
* @param[in]  in : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::fproduct(XQWORDSIG* output, const XQWORDSIG* in2, const XQWORDSIG* in) 
{
  output[0] =       ((XQWORDSIG) ((XDWORDSIG) in2[0])) * ((XDWORDSIG) in[0]);
  output[1] =       ((XQWORDSIG) ((XDWORDSIG) in2[0])) * ((XDWORDSIG) in[1]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[1])) * ((XDWORDSIG) in[0]);
  output[2] =  2 *  ((XQWORDSIG) ((XDWORDSIG) in2[1])) * ((XDWORDSIG) in[1]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[0])) * ((XDWORDSIG) in[2]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[2])) * ((XDWORDSIG) in[0]);
  output[3] =       ((XQWORDSIG) ((XDWORDSIG) in2[1])) * ((XDWORDSIG) in[2]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[2])) * ((XDWORDSIG) in[1]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[0])) * ((XDWORDSIG) in[3]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[3])) * ((XDWORDSIG) in[0]);
  output[4] =       ((XQWORDSIG) ((XDWORDSIG) in2[2])) * ((XDWORDSIG) in[2]) +
               2 * (((XQWORDSIG) ((XDWORDSIG) in2[1])) * ((XDWORDSIG) in[3]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[3])) * ((XDWORDSIG) in[1])) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[0])) * ((XDWORDSIG) in[4]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[4])) * ((XDWORDSIG) in[0]);
  output[5] =       ((XQWORDSIG) ((XDWORDSIG) in2[2])) * ((XDWORDSIG) in[3]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[3])) * ((XDWORDSIG) in[2]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[1])) * ((XDWORDSIG) in[4]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[4])) * ((XDWORDSIG) in[1]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[0])) * ((XDWORDSIG) in[5]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[5])) * ((XDWORDSIG) in[0]);
  output[6] =  2 * (((XQWORDSIG) ((XDWORDSIG) in2[3])) * ((XDWORDSIG) in[3]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[1])) * ((XDWORDSIG) in[5]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[5])) * ((XDWORDSIG) in[1])) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[2])) * ((XDWORDSIG) in[4]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[4])) * ((XDWORDSIG) in[2]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[0])) * ((XDWORDSIG) in[6]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[6])) * ((XDWORDSIG) in[0]);
  output[7] =       ((XQWORDSIG) ((XDWORDSIG) in2[3])) * ((XDWORDSIG) in[4]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[4])) * ((XDWORDSIG) in[3]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[2])) * ((XDWORDSIG) in[5]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[5])) * ((XDWORDSIG) in[2]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[1])) * ((XDWORDSIG) in[6]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[6])) * ((XDWORDSIG) in[1]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[0])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[7])) * ((XDWORDSIG) in[0]);
  output[8] =       ((XQWORDSIG) ((XDWORDSIG) in2[4])) * ((XDWORDSIG) in[4]) +
               2 * (((XQWORDSIG) ((XDWORDSIG) in2[3])) * ((XDWORDSIG) in[5]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[5])) * ((XDWORDSIG) in[3]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[1])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[7])) * ((XDWORDSIG) in[1])) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[2])) * ((XDWORDSIG) in[6]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[6])) * ((XDWORDSIG) in[2]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[0])) * ((XDWORDSIG) in[8]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[8])) * ((XDWORDSIG) in[0]);
  output[9] =       ((XQWORDSIG) ((XDWORDSIG) in2[4])) * ((XDWORDSIG) in[5]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[5])) * ((XDWORDSIG) in[4]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[3])) * ((XDWORDSIG) in[6]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[6])) * ((XDWORDSIG) in[3]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[2])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[7])) * ((XDWORDSIG) in[2]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[1])) * ((XDWORDSIG) in[8]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[8])) * ((XDWORDSIG) in[1]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[0])) * ((XDWORDSIG) in[9]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[9])) * ((XDWORDSIG) in[0]);
  output[10] = 2 * (((XQWORDSIG) ((XDWORDSIG) in2[5])) * ((XDWORDSIG) in[5]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[3])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[7])) * ((XDWORDSIG) in[3]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[1])) * ((XDWORDSIG) in[9]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[9])) * ((XDWORDSIG) in[1])) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[4])) * ((XDWORDSIG) in[6]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[6])) * ((XDWORDSIG) in[4]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[2])) * ((XDWORDSIG) in[8]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[8])) * ((XDWORDSIG) in[2]);
  output[11] =      ((XQWORDSIG) ((XDWORDSIG) in2[5])) * ((XDWORDSIG) in[6]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[6])) * ((XDWORDSIG) in[5]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[4])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[7])) * ((XDWORDSIG) in[4]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[3])) * ((XDWORDSIG) in[8]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[8])) * ((XDWORDSIG) in[3]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[2])) * ((XDWORDSIG) in[9]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[9])) * ((XDWORDSIG) in[2]);
  output[12] =      ((XQWORDSIG) ((XDWORDSIG) in2[6])) * ((XDWORDSIG) in[6]) +
               2 * (((XQWORDSIG) ((XDWORDSIG) in2[5])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[7])) * ((XDWORDSIG) in[5]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[3])) * ((XDWORDSIG) in[9]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[9])) * ((XDWORDSIG) in[3])) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[4])) * ((XDWORDSIG) in[8]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[8])) * ((XDWORDSIG) in[4]);
  output[13] =      ((XQWORDSIG) ((XDWORDSIG) in2[6])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[7])) * ((XDWORDSIG) in[6]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[5])) * ((XDWORDSIG) in[8]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[8])) * ((XDWORDSIG) in[5]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[4])) * ((XDWORDSIG) in[9]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[9])) * ((XDWORDSIG) in[4]);
  output[14] = 2 * (((XQWORDSIG) ((XDWORDSIG) in2[7])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[5])) * ((XDWORDSIG) in[9]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[9])) * ((XDWORDSIG) in[5])) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[6])) * ((XDWORDSIG) in[8]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[8])) * ((XDWORDSIG) in[6]);
  output[15] =      ((XQWORDSIG) ((XDWORDSIG) in2[7])) * ((XDWORDSIG) in[8]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[8])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[6])) * ((XDWORDSIG) in[9]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[9])) * ((XDWORDSIG) in[6]);
  output[16] =      ((XQWORDSIG) ((XDWORDSIG) in2[8])) * ((XDWORDSIG) in[8]) +
               2 * (((XQWORDSIG) ((XDWORDSIG) in2[7])) * ((XDWORDSIG) in[9]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[9])) * ((XDWORDSIG) in[7]));
  output[17] =      ((XQWORDSIG) ((XDWORDSIG) in2[8])) * ((XDWORDSIG) in[9]) +
                    ((XQWORDSIG) ((XDWORDSIG) in2[9])) * ((XDWORDSIG) in[8]);
  output[18] = 2 *  ((XQWORDSIG) ((XDWORDSIG) in2[9])) * ((XDWORDSIG) in[9]);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::freduce_degree(XQWORDSIG* output)
* @brief      freduce_degree Reduce a long form to a short form by taking the input mod 2^255 - 19.
* @ingroup    CIPHER
* 
* @param[in]  output : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::freduce_degree(XQWORDSIG* output) 
{
  // Each of these shifts and adds ends up multiplying the value by 19.
  output[8] += output[18] << 4;
  output[8] += output[18] << 1;
  output[8] += output[18];
  output[7] += output[17] << 4;
  output[7] += output[17] << 1;
  output[7] += output[17];
  output[6] += output[16] << 4;
  output[6] += output[16] << 1;
  output[6] += output[16];
  output[5] += output[15] << 4;
  output[5] += output[15] << 1;
  output[5] += output[15];
  output[4] += output[14] << 4;
  output[4] += output[14] << 1;
  output[4] += output[14];
  output[3] += output[13] << 4;
  output[3] += output[13] << 1;
  output[3] += output[13];
  output[2] += output[12] << 4;
  output[2] += output[12] << 1;
  output[2] += output[12];
  output[1] += output[11] << 4;
  output[1] += output[11] << 1;
  output[1] += output[11];
  output[0] += output[10] << 4;
  output[0] += output[10] << 1;
  output[0] += output[10];
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XQWORDSIG CIPHERCURVE25519::div_by_2_26(const XQWORDSIG v)
* @brief      div_by_2_26 return v / 2^26, using only shifts and adds. 
* @ingroup    CIPHER
* 
* @param[in]  v : 
* 
* @return     XQWORDSIG : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XQWORDSIG CIPHERCURVE25519::div_by_2_26(const XQWORDSIG v) 
{  
  const XDWORD highword     = (XDWORD)(((XQWORD) v) >> 32);   // High word of v; no shift needed
  const XDWORDSIG sign      = ((XDWORDSIG) highword) >> 31;   // Set to all 1s if v was negative; else set to 0s.   
  const XDWORDSIG roundoff  = ((XDWORD) sign) >> 6;           // Set to 0x3ffffff if v was negative; else set to 0. 
  
  return (v + roundoff) >> 26;    // Should return v / (1<<26) 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XQWORDSIG CIPHERCURVE25519::div_by_2_25(const XQWORDSIG v)
* @brief      div_by_2_25 return v / (2^25), using only shifts and adds.
* @ingroup    CIPHER
* 
* @param[in]  v : 
* 
* @return     XQWORDSIG : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XQWORDSIG CIPHERCURVE25519::div_by_2_25(const XQWORDSIG v) 
{  
  const XDWORD    highword  = (XDWORD)(((XQWORD) v) >> 32);   // High word of v; no shift needed  
  const XDWORDSIG sign      = ((XDWORDSIG) highword) >> 31;   // Set to all 1s if v was negative; else set to 0s.   
  const XDWORDSIG roundoff  = ((XDWORD) sign) >> 7;           // Set to 0x1ffffff if v was negative; else set to 0. 
 
  return (v + roundoff) >> 25; // Should return v / (1<<25) 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORDSIG CIPHERCURVE25519::div_s32_by_2_25(const XDWORDSIG v)
* @brief      div_s32_by_2_25
* @ingroup    CIPHER
* 
* @param[in]  v : 
* 
* @return     XDWORDSIG : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORDSIG CIPHERCURVE25519::div_s32_by_2_25(const XDWORDSIG v) 
{
   const XDWORDSIG roundoff = ((XDWORD)(v >> 31)) >> 7;

   return (v + roundoff) >> 25;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::freduce_coefficients(XQWORDSIG* output)
* @brief      freduce_coefficients  Reduce all coefficients of the short form input so that |x| < 2^26.
                                    On entry: |output[i]| < 2^62
* @ingroup    CIPHER
* 
* @param[in]  output : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::freduce_coefficients(XQWORDSIG* output) 
{
  XDWORD i;

  output[10] = 0;

  for(i=0; i<10; i+=2) 
    {
      XQWORDSIG over = div_by_2_26(output[i]);

      output[i] -= over << 26;
      output[i+1] += over;

      over = div_by_2_25(output[i+1]);
      output[i+1] -= over << 25;
      output[i+2] += over;
    }

  // Now |output[10]| < 2 ^ 38 and all other coefficients are reduced. 
  output[0] += output[10] << 4;
  output[0] += output[10] << 1;
  output[0] += output[10];

  output[10] = 0;

  // Now output[1..9] are reduced, and |output[0]| < 2^26 + 19 * 2^38  So |over| will be no more than 77825  
  {
    XQWORDSIG over = div_by_2_26(output[0]);
    output[0] -= over << 26;
    output[1] += over;
  }

  // Now output[0,2..9] are reduced, and |output[1]| < 2^25 + 77825  So |over| will be no more than 1. 
  {
    // output[1] fits in 32 bits, so we can use div_s32_by_2_25 here. 
    XDWORDSIG over32 = div_s32_by_2_25((XDWORDSIG)output[1]);
    output[1] -= over32 << 25;
    output[2] += over32;
  }

  // Finally, output[0,1,3..9] are reduced, and output[2] is "nearly reduced": 
  // we have |output[2]| <= 2^26.  This is good enough for all of our math, but it will require an extra freduce_coefficients before fcontract.
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::fmul(XQWORDSIG*output, const XQWORDSIG* in, const XQWORDSIG* in2)
* @brief      fmul    A helpful wrapper around fproduct: output = in * in2.
                      output must be distinct to both inputs. The output is reduced degree and reduced coefficient.
* @ingroup    CIPHER
* 
* @param[in]  output : 
* @param[in]  in : 
* @param[in]  in2 : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::fmul(XQWORDSIG* output, const XQWORDSIG* in, const XQWORDSIG* in2) 
{
  XQWORDSIG t[19];

  fproduct(t, in, in2);
  freduce_degree(t);
  freduce_coefficients(t);

  memcpy(output, t, sizeof(XQWORDSIG) * 10);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::fsquare_inner(XQWORDSIG* output, const XQWORDSIG* in)
* @brief      fsquare_inner
* @ingroup    CIPHER
* 
* @param[in]  output : 
* @param[in]  in : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::fsquare_inner(XQWORDSIG* output, const XQWORDSIG* in) 
{
  output[0] =       ((XQWORDSIG) ((XDWORDSIG) in[0])) * ((XDWORDSIG) in[0]);
  output[1] =  2 *  ((XQWORDSIG) ((XDWORDSIG) in[0])) * ((XDWORDSIG) in[1]);
  output[2] =  2 * (((XQWORDSIG) ((XDWORDSIG) in[1])) * ((XDWORDSIG) in[1]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[0])) * ((XDWORDSIG) in[2]));
  output[3] =  2 * (((XQWORDSIG) ((XDWORDSIG) in[1])) * ((XDWORDSIG) in[2]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[0])) * ((XDWORDSIG) in[3]));
  output[4] =       ((XQWORDSIG) ((XDWORDSIG) in[2])) * ((XDWORDSIG) in[2]) +
               4 *  ((XQWORDSIG) ((XDWORDSIG) in[1])) * ((XDWORDSIG) in[3]) +
               2 *  ((XQWORDSIG) ((XDWORDSIG) in[0])) * ((XDWORDSIG) in[4]);
  output[5] =  2 * (((XQWORDSIG) ((XDWORDSIG) in[2])) * ((XDWORDSIG) in[3]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[1])) * ((XDWORDSIG) in[4]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[0])) * ((XDWORDSIG) in[5]));
  output[6] =  2 * (((XQWORDSIG) ((XDWORDSIG) in[3])) * ((XDWORDSIG) in[3]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[2])) * ((XDWORDSIG) in[4]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[0])) * ((XDWORDSIG) in[6]) +
               2 *  ((XQWORDSIG) ((XDWORDSIG) in[1])) * ((XDWORDSIG) in[5]));
  output[7] =  2 * (((XQWORDSIG) ((XDWORDSIG) in[3])) * ((XDWORDSIG) in[4]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[2])) * ((XDWORDSIG) in[5]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[1])) * ((XDWORDSIG) in[6]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[0])) * ((XDWORDSIG) in[7]));
  output[8] =       ((XQWORDSIG) ((XDWORDSIG) in[4])) * ((XDWORDSIG) in[4]) +
               2 * (((XQWORDSIG) ((XDWORDSIG) in[2])) * ((XDWORDSIG) in[6]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[0])) * ((XDWORDSIG) in[8]) +
               2 * (((XQWORDSIG) ((XDWORDSIG) in[1])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[3])) * ((XDWORDSIG) in[5])));
  output[9] =  2 * (((XQWORDSIG) ((XDWORDSIG) in[4])) * ((XDWORDSIG) in[5]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[3])) * ((XDWORDSIG) in[6]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[2])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[1])) * ((XDWORDSIG) in[8]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[0])) * ((XDWORDSIG) in[9]));
  output[10] = 2 * (((XQWORDSIG) ((XDWORDSIG) in[5])) * ((XDWORDSIG) in[5]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[4])) * ((XDWORDSIG) in[6]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[2])) * ((XDWORDSIG) in[8]) +
               2 * (((XQWORDSIG) ((XDWORDSIG) in[3])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[1])) * ((XDWORDSIG) in[9])));
  output[11] = 2 * (((XQWORDSIG) ((XDWORDSIG) in[5])) * ((XDWORDSIG) in[6]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[4])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[3])) * ((XDWORDSIG) in[8]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[2])) * ((XDWORDSIG) in[9]));
  output[12] =      ((XQWORDSIG) ((XDWORDSIG) in[6])) * ((XDWORDSIG) in[6]) +
               2 * (((XQWORDSIG) ((XDWORDSIG) in[4])) * ((XDWORDSIG) in[8]) +
               2 * (((XQWORDSIG) ((XDWORDSIG) in[5])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[3])) * ((XDWORDSIG) in[9])));
  output[13] = 2 * (((XQWORDSIG) ((XDWORDSIG) in[6])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[5])) * ((XDWORDSIG) in[8]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[4])) * ((XDWORDSIG) in[9]));
  output[14] = 2 * (((XQWORDSIG) ((XDWORDSIG) in[7])) * ((XDWORDSIG) in[7]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[6])) * ((XDWORDSIG) in[8]) +
               2 *  ((XQWORDSIG) ((XDWORDSIG) in[5])) * ((XDWORDSIG) in[9]));
  output[15] = 2 * (((XQWORDSIG) ((XDWORDSIG) in[7])) * ((XDWORDSIG) in[8]) +
                    ((XQWORDSIG) ((XDWORDSIG) in[6])) * ((XDWORDSIG) in[9]));
  output[16] =      ((XQWORDSIG) ((XDWORDSIG) in[8])) * ((XDWORDSIG) in[8]) +
               4 *  ((XQWORDSIG) ((XDWORDSIG) in[7])) * ((XDWORDSIG) in[9]);
  output[17] = 2 *  ((XQWORDSIG) ((XDWORDSIG) in[8])) * ((XDWORDSIG) in[9]);
  output[18] = 2 *  ((XQWORDSIG) ((XDWORDSIG) in[9])) * ((XDWORDSIG) in[9]);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::fsquare(XQWORDSIG* output, const XQWORDSIG* in)
* @brief      fsquare
* @ingroup    CIPHER
* 
* @param[in]  output : 
* @param[in]  in : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::fsquare(XQWORDSIG* output, const XQWORDSIG* in) 
{
  XQWORDSIG t[19];

  fsquare_inner(t, in);
  freduce_degree(t);
  freduce_coefficients(t);

  memcpy(output, t, sizeof(XQWORDSIG) * 10);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::fexpand(XQWORDSIG* output, const XBYTE* input)
* @brief      fexpand Take a little-endian, 32-byte number and expand it into polynomial form
* @ingroup    CIPHER
* 
* @param[in]  output : 
* @param[in]  input : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::fexpand(XQWORDSIG* output, const XBYTE* input) 
{
  output[0] = ((((XQWORDSIG) input[ 0 + 0]) | ((XQWORDSIG) input[ 0 + 1]) << 8 | ((XQWORDSIG) input[ 0 + 2]) << 16 | ((XQWORDSIG) input[ 0 + 3]) << 24) >> 0) & 0x3ffffff;;
  output[1] = ((((XQWORDSIG) input[ 3 + 0]) | ((XQWORDSIG) input[ 3 + 1]) << 8 | ((XQWORDSIG) input[ 3 + 2]) << 16 | ((XQWORDSIG) input[ 3 + 3]) << 24) >> 2) & 0x1ffffff;;
  output[2] = ((((XQWORDSIG) input[ 6 + 0]) | ((XQWORDSIG) input[ 6 + 1]) << 8 | ((XQWORDSIG) input[ 6 + 2]) << 16 | ((XQWORDSIG) input[ 6 + 3]) << 24) >> 3) & 0x3ffffff;;
  output[3] = ((((XQWORDSIG) input[ 9 + 0]) | ((XQWORDSIG) input[ 9 + 1]) << 8 | ((XQWORDSIG) input[ 9 + 2]) << 16 | ((XQWORDSIG) input[ 9 + 3]) << 24) >> 5) & 0x1ffffff;;
  output[4] = ((((XQWORDSIG) input[12 + 0]) | ((XQWORDSIG) input[12 + 1]) << 8 | ((XQWORDSIG) input[12 + 2]) << 16 | ((XQWORDSIG) input[12 + 3]) << 24) >> 6) & 0x3ffffff;;
  output[5] = ((((XQWORDSIG) input[16 + 0]) | ((XQWORDSIG) input[16 + 1]) << 8 | ((XQWORDSIG) input[16 + 2]) << 16 | ((XQWORDSIG) input[16 + 3]) << 24) >> 0) & 0x1ffffff;;
  output[6] = ((((XQWORDSIG) input[19 + 0]) | ((XQWORDSIG) input[19 + 1]) << 8 | ((XQWORDSIG) input[19 + 2]) << 16 | ((XQWORDSIG) input[19 + 3]) << 24) >> 1) & 0x3ffffff;;
  output[7] = ((((XQWORDSIG) input[22 + 0]) | ((XQWORDSIG) input[22 + 1]) << 8 | ((XQWORDSIG) input[22 + 2]) << 16 | ((XQWORDSIG) input[22 + 3]) << 24) >> 3) & 0x1ffffff;;
  output[8] = ((((XQWORDSIG) input[25 + 0]) | ((XQWORDSIG) input[25 + 1]) << 8 | ((XQWORDSIG) input[25 + 2]) << 16 | ((XQWORDSIG) input[25 + 3]) << 24) >> 4) & 0x3ffffff;;
  output[9] = ((((XQWORDSIG) input[28 + 0]) | ((XQWORDSIG) input[28 + 1]) << 8 | ((XQWORDSIG) input[28 + 2]) << 16 | ((XQWORDSIG) input[28 + 3]) << 24) >> 6) & 0x1ffffff;;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::fcontract(XBYTE* output, XQWORDSIG* input)
* @brief      fcontract   Take a fully reduced polynomial form number and contract it into a little-endian, 32-byte array
* @ingroup    CIPHER
* 
* @param[in]  output : 
* @param[in]  input : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::fcontract(XBYTE* output, XQWORDSIG* input) 
{
  int i;
  int j;

  for(j=0; j<2; ++j) 
    {
      for(i=0; i<9; ++i) 
        {
          if((i & 1) == 1) 
            {
              // This calculation is a time-invariant way to make input[i] positive by borrowing from the next-larger XQWORDSIG.        
              
              const XDWORDSIG mask  = (XDWORDSIG)(input[i]) >> 31;
              const XDWORDSIG carry = -(((XDWORDSIG)(input[i]) & mask) >> 25);
        
              input[i]    = (XDWORDSIG)(input[i]) + (carry << 25);
              input[i+1]  = (XDWORDSIG)(input[i+1]) - carry;

            } 
           else 
            {
              const XDWORDSIG mask  = (XDWORDSIG)(input[i]) >> 31;
              const XDWORDSIG carry = -(((XDWORDSIG)(input[i]) & mask) >> 26);
        
              input[i]    = (XDWORDSIG)(input[i]) + (carry << 26);
              input[i+1]  = (XDWORDSIG)(input[i+1]) - carry;
            }
        }
    
      const XDWORDSIG mask  = (XDWORDSIG)(input[9]) >> 31;
      const XDWORDSIG carry = -(((XDWORDSIG)(input[9]) & mask) >> 25);

      input[9] = (XDWORDSIG)(input[9]) + (carry << 25);
      input[0] = (XDWORDSIG)(input[0]) - (carry * 19);
    }

  // The first borrow-propagation pass above ended with every XQWORDSIG except (possibly) input[0] non-negative.
  //
  // Since each input XQWORDSIG except input[0] is decreased by at most 1 by a borrow-propagation pass, the second borrow-propagation pass
  // could only have wrapped around to decrease input[0] again if the first pass left input[0] negative *and* input[1] through input[9]
  // were all zero.  In that case, input[1] is now 2^25 - 1, and this last borrow-propagation step will leave input[1] non-negative.
  
  const XDWORDSIG mask  =  (XDWORDSIG)(input[0]) >> 31;
  const XDWORDSIG carry = -(((XDWORDSIG)(input[0]) & mask) >> 26);

  input[0] = (XDWORDSIG)(input[0]) + (carry << 26);
  input[1] = (XDWORDSIG)(input[1]) - carry;

  // Both passes through the above loop, plus the last 0-to-1 step, are necessary: if input[9] is -1 and input[0] through input[8] are 0,
  //   negative values will remain in the array until the end.
   

  input[1] <<= 2;
  input[2] <<= 3;
  input[3] <<= 5;
  input[4] <<= 6;
  input[6] <<= 1;
  input[7] <<= 3;
  input[8] <<= 4;
  input[9] <<= 6;

  output[   0]  = 0;
  output[  16]  = 0;

  output[ 0+0] |= input[0] & 0xff; output[ 0+1] = (input[0] >> 8) & 0xff; output[ 0+2] = (input[0] >> 16) & 0xff; output[ 0+3] = (input[0] >> 24) & 0xff;;
  output[ 3+0] |= input[1] & 0xff; output[ 3+1] = (input[1] >> 8) & 0xff; output[ 3+2] = (input[1] >> 16) & 0xff; output[ 3+3] = (input[1] >> 24) & 0xff;;
  output[ 6+0] |= input[2] & 0xff; output[ 6+1] = (input[2] >> 8) & 0xff; output[ 6+2] = (input[2] >> 16) & 0xff; output[ 6+3] = (input[2] >> 24) & 0xff;;
  output[ 9+0] |= input[3] & 0xff; output[ 9+1] = (input[3] >> 8) & 0xff; output[ 9+2] = (input[3] >> 16) & 0xff; output[ 9+3] = (input[3] >> 24) & 0xff;;
  output[12+0] |= input[4] & 0xff; output[12+1] = (input[4] >> 8) & 0xff; output[12+2] = (input[4] >> 16) & 0xff; output[12+3] = (input[4] >> 24) & 0xff;;
  output[16+0] |= input[5] & 0xff; output[16+1] = (input[5] >> 8) & 0xff; output[16+2] = (input[5] >> 16) & 0xff; output[16+3] = (input[5] >> 24) & 0xff;;
  output[19+0] |= input[6] & 0xff; output[19+1] = (input[6] >> 8) & 0xff; output[19+2] = (input[6] >> 16) & 0xff; output[19+3] = (input[6] >> 24) & 0xff;;
  output[22+0] |= input[7] & 0xff; output[22+1] = (input[7] >> 8) & 0xff; output[22+2] = (input[7] >> 16) & 0xff; output[22+3] = (input[7] >> 24) & 0xff;;
  output[25+0] |= input[8] & 0xff; output[25+1] = (input[8] >> 8) & 0xff; output[25+2] = (input[8] >> 16) & 0xff; output[25+3] = (input[8] >> 24) & 0xff;;
  output[28+0] |= input[9] & 0xff; output[28+1] = (input[9] >> 8) & 0xff; output[28+2] = (input[9] >> 16) & 0xff; output[28+3] = (input[9] >> 24) & 0xff;;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::fmonty(XQWORDSIG* x2, XQWORDSIG* z2, XQWORDSIG* x3, XQWORDSIG* z3, XQWORDSIG* x, XQWORDSIG* z, XQWORDSIG* xprime, XQWORDSIG* zprime, const XQWORDSIG* qmqp)
* @brief      fmonty  Input: Q, Q', Q-Q'  Output: 2Q, Q+Q'
* @ingroup    CIPHER
* 
* @param[in]  x2 : x2 z3: long form output 2Q 
* @param[in]  z2 : output 2Q 
* @param[in]  x3 : x3 z3: long form output Q + Q' 
* @param[in]  z3 : output Q + Q'
* @param[in]  x :  x z: short form, destroyed input Q 
* @param[in]  z : input Q 
* @param[in]  xprime : xprime zprime: short form, destroyed input Q'
* @param[in]  zprime : input Q'
* @param[in]  qmqp : qmqp: short form, preserved input Q - Q' 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::fmonty(XQWORDSIG* x2, XQWORDSIG* z2, XQWORDSIG* x3, XQWORDSIG* z3, XQWORDSIG* x, XQWORDSIG* z, XQWORDSIG* xprime, XQWORDSIG* zprime, const XQWORDSIG* qmqp) 
{
  XQWORDSIG origx[10];
  XQWORDSIG origxprime[10];
  XQWORDSIG zzz[19];
  XQWORDSIG xx[19]; 
  XQWORDSIG zz[19];
  XQWORDSIG xxprime[19];
  XQWORDSIG zzprime[19];
  XQWORDSIG zzzprime[19]; 
  XQWORDSIG xxxprime[19];

  memcpy(origx, x, 10 * sizeof(XQWORDSIG));
  fsum(x, z);
  fdifference(z, origx);                          // does x - z

  memcpy(origxprime, xprime, sizeof(XQWORDSIG) * 10);
  fsum(xprime, zprime);
  fdifference(zprime, origxprime);
  fproduct(xxprime, xprime, z);
  fproduct(zzprime, x, zprime);
  freduce_degree(xxprime);
  freduce_coefficients(xxprime);
  freduce_degree(zzprime);
  freduce_coefficients(zzprime);
  memcpy(origxprime, xxprime, sizeof(XQWORDSIG) * 10);
  fsum(xxprime, zzprime);
  fdifference(zzprime, origxprime);
  fsquare(xxxprime, xxprime);
  fsquare(zzzprime, zzprime);
  fproduct(zzprime, zzzprime, qmqp);
  freduce_degree(zzprime);
  freduce_coefficients(zzprime);
  memcpy(x3, xxxprime, sizeof(XQWORDSIG) * 10);
  memcpy(z3, zzprime, sizeof(XQWORDSIG) * 10);

  fsquare(xx, x);
  fsquare(zz, z);
  fproduct(x2, xx, zz);
  freduce_degree(x2);
  freduce_coefficients(x2);
  fdifference(zz, xx);                            // does zz = xx - zz
  memset(zzz + 10, 0, sizeof(XQWORDSIG) * 9);
  fscalar_product(zzz, zz, 121665);

  // No need to call freduce_degree here: fscalar_product doesn't increase the degree of its input. 
  freduce_coefficients(zzz);
  fsum(zzz, xx);
  fproduct(z2, zz, zzz);
  freduce_degree(z2);
  freduce_coefficients(z2);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::swap_conditional(XQWORDSIG a[19], XQWORDSIG b[19], XQWORDSIG iswap)
* @brief      swap_conditional  Conditionally swap two reduced-form XQWORDSIG arrays if 'iswap' is 1, but leave
*                               them unchanged if 'iswap' is 0.  Runs in data-invariant time to avoid side-channel 
*                               attacks.
*
*                               NOTE that this function requires that 'iswap' be 1 or 0; other values give
*                                    wrong results.  Also, the two XQWORDSIG arrays must be in reduced-coefficient,
*                                    reduced-degree form: the values in a[10..19] or b[10..19] aren't swapped,
*                                    and all all values in a[0..9],b[0..9] must have magnitude less than INT32_MAX.
* @ingroup    CIPHER
* 
* @param[in]  a[19] : 
* @param[in]  b[19] : 
* @param[in]  iswap : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::swap_conditional(XQWORDSIG a[19], XQWORDSIG b[19], XQWORDSIG iswap) 
{
  const XDWORDSIG swap = (XDWORDSIG)-iswap;  
  XDWORD i;

  for(i=0; i<10; ++i) 
    {
      const XDWORDSIG x = swap & ( ((XDWORDSIG)a[i]) ^ ((XDWORDSIG)b[i]) );

      a[i] = ((XDWORDSIG)a[i]) ^ x;
      b[i] = ((XDWORDSIG)b[i]) ^ x;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::cmult(XQWORDSIG* resultx, XQWORDSIG* resultz, const XBYTE* n, const XQWORDSIG* q)
* @brief      cmult         Calculates nQ where Q is the x-coordinate of a point on the curve
*                           resultx/resultz: the x coordinate of the resulting curve point (short form)
*                           n: a little endian, 32-byte number
*                           q: a point of the curve (short form)
* @ingroup    CIPHER
* 
* @param[in]  resultx : 
* @param[in]  resultz : 
* @param[in]  n : 
* @param[in]  q : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::cmult(XQWORDSIG* resultx, XQWORDSIG* resultz, const XBYTE* n, const XQWORDSIG* q) 
{
  XQWORDSIG   a[19]   = {0};  
  XQWORDSIG   b[19]   = {1};  
  XQWORDSIG   c[19]   = {1};  
  XQWORDSIG   d[19]   = {0};
  XQWORDSIG*  nqpqx   = a;
  XQWORDSIG*  nqpqz   = b; 
  XQWORDSIG*  nqx     = c;
  XQWORDSIG*  nqz     = d;
  XQWORDSIG*  t;
  XQWORDSIG   e[19]   = {0}; 
  XQWORDSIG   f[19]   = {1}; 
  XQWORDSIG   g[19]   = {0}; 
  XQWORDSIG   h[19]   = {1};
  XQWORDSIG*  nqpqx2  = e;
  XQWORDSIG*  nqpqz2  = f;
  XQWORDSIG*  nqx2    = g;
  XQWORDSIG*  nqz2    = h;
  XDWORD    i;
  XDWORD    j;

  memcpy(nqpqx, q, sizeof(XQWORDSIG) * 10);

  for(i=0; i<32; ++i) 
    {
      XBYTE byte = n[31 - i];
      for(j=0; j<8; ++j) 
        {
          const XQWORDSIG bit = byte >> 7;

          swap_conditional(nqx, nqpqx, bit);
          swap_conditional(nqz, nqpqz, bit);
          fmonty(nqx2, nqz2, nqpqx2, nqpqz2, nqx, nqz, nqpqx, nqpqz, q);
          swap_conditional(nqx2, nqpqx2, bit);
          swap_conditional(nqz2, nqpqz2, bit);

          t       = nqx;
          nqx     = nqx2;
          nqx2    = t;
          t       = nqz;
          nqz     = nqz2;
          nqz2    = t;
          t       = nqpqx;
          nqpqx   = nqpqx2;
          nqpqx2  = t;
          t       = nqpqz;
          nqpqz   = nqpqz2;
          nqpqz2  = t;

          byte  <<= 1;
        }
    }

  memcpy(resultx, nqx, sizeof(XQWORDSIG) * 10);
  memcpy(resultz, nqz, sizeof(XQWORDSIG) * 10);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::crecip(XQWORDSIG* out, const XQWORDSIG* z)
* @brief      crecip    Shamelessly copied from djb's code
* @ingroup    CIPHER
* 
* @param[in]  out : 
* @param[in]  z : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::crecip(XQWORDSIG* out, const XQWORDSIG* z) 
{
  XQWORDSIG z2[10];
  XQWORDSIG z9[10];
  XQWORDSIG z11[10];
  XQWORDSIG z2_5_0[10];
  XQWORDSIG z2_10_0[10];
  XQWORDSIG z2_20_0[10];
  XQWORDSIG z2_50_0[10];
  XQWORDSIG z2_100_0[10];
  XQWORDSIG t0[10];
  XQWORDSIG t1[10];
  int       i;

  fsquare(z2,z);                    // 2 
  fsquare(t1,z2);                   // 4 
  fsquare(t0,t1);                   // 8 
  fmul(z9,t0,z);                    // 9 
  fmul(z11,z9,z2);                  // 11 
  fsquare(t0,z11);                  // 22  
  fmul(z2_5_0,t0,z9);               // 2^5 - 2^0 = 31 

  fsquare(t0,z2_5_0);               // 2^6 - 2^1  
  fsquare(t1,t0);                   // 2^7 - 2^2  
  fsquare(t0,t1);                   // 2^8 - 2^3 
  fsquare(t1,t0);                   // 2^9 - 2^4  
  fsquare(t0,t1);                   // 2^10 - 2^5 
  fmul(z2_10_0,t0,z2_5_0);          // 2^10 - 2^0 

  fsquare(t0,z2_10_0);              // 2^11 - 2^1 
  fsquare(t1,t0);                   // 2^12 - 2^2 
  for(i=2; i<10; i+=2)              // 2^20 - 2^10   
    { 
      fsquare(t0,t1); fsquare(t1,t0); 
    }
  fmul(z2_20_0,t1,z2_10_0);         // 2^20 - 2^0 

  fsquare(t0,z2_20_0);              // 2^21 - 2^1 
  fsquare(t1,t0);                   // 2^22 - 2^2 
  for (i=2; i<20; i+=2)             // 2^40 - 2^20 
    { fsquare(t0,t1); fsquare(t1,t0); 
    }
  fmul(t0,t1,z2_20_0);              // 2^40 - 2^0 

  fsquare(t1,t0);                   // 2^41 - 2^1 
  fsquare(t0,t1);                   // 2^42 - 2^2  
  for(i=2; i<10; i+=2)              // 2^50 - 2^10  
  { 
    fsquare(t1,t0); fsquare(t0,t1); 
  }
  fmul(z2_50_0,t0,z2_10_0);         // 2^50 - 2^0 

  fsquare(t0,z2_50_0);              // 2^51 - 2^1 
  fsquare(t1,t0);                   // 2^52 - 2^2 
  for(i=2; i<50; i+=2)              // 2^100 - 2^50  
    { 
      fsquare(t0,t1); fsquare(t1,t0); 
    }
  fmul(z2_100_0,t1,z2_50_0);        // 2^100 - 2^0  

  fsquare(t1,z2_100_0);             // 2^101 - 2^1 
  fsquare(t0,t1);                   // 2^102 - 2^2 
  for(i=2; i<100; i+=2)             // 2^200 - 2^100   
    { 
      fsquare(t1,t0); fsquare(t0,t1); 
    }
  fmul(t1,t0,z2_100_0);             // 2^200 - 2^0 

  fsquare(t0,t1);                   // 2^201 - 2^1 
  fsquare(t1,t0);                   // 2^202 - 2^2 
  for(i=2; i<50; i+=2)              // 2^250 - 2^50 
    { 
      fsquare(t0,t1); fsquare(t1,t0); 
    }
  fmul(t0,t1,z2_50_0);              // 2^250 - 2^0 

  fsquare(t1,t0);                   // 2^251 - 2^1 
  fsquare(t0,t1);                   // 2^252 - 2^2 
  fsquare(t1,t0);                   // 2^253 - 2^3 
  fsquare(t0,t1);                   // 2^254 - 2^4 
  fsquare(t1,t0);                   // 2^255 - 2^5 
  fmul(out,t1,z11);                 // 2^255 - 21 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::curve25519(XBYTE*mypublic, const XBYTE* secret, const XBYTE* basepoint)
* @brief      curve25519
* @ingroup    CIPHER
* 
* @param[in]  mypublic : 
* @param[in]  secret : 
* @param[in]  basepoint : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::curve25519(XBYTE* mypublic, const XBYTE* secret, const XBYTE* basepoint) 
{
  XQWORDSIG bp[10];
  XQWORDSIG x[10];
  XQWORDSIG z[11]; 
  XQWORDSIG zmone[10];
  XBYTE     e[CIPHERCURVE25519_MAXKEY];
  int       i;

  if(basepoint == 0) 
    {
      basepoint = CIPHERCURVE25519::basepoint;
    }

  for(i = 0; i < CIPHERCURVE25519_MAXKEY; ++i) 
    { 
      e[i] = secret[i];
    }

  e[0]  &= 248;
  e[31] &= 127;
  e[31] |= 64;

  fexpand(bp, basepoint);
  cmult(x, z, e, bp);
  crecip(zmone, z);
  fmul(z, x, zmone);
  freduce_coefficients(z);
  fcontract(mypublic, z);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERCURVE25519::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERCURVE25519::Clean()
{
  CleanAllKeys();
}


#pragma endregion

