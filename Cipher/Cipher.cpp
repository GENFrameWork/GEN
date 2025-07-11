/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       Cipher.cpp
* 
* @class      CIPHER
* @brief      Cipher interface class
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

#include "Cipher.h"

#include <string.h>

#include "XFactory.h"

#include "CipherKeySymmetrical.h"

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


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHER::CIPHER()
* @brief      Constructor of class
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHER::CIPHER()
{
  Clean();

  type             = CIPHERTYPE_XOR;
  paddingadjustsize = 0;

  inivector = new XBUFFER();
  result    = new XBUFFER();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHER::~CIPHER()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHER::~CIPHER()
{
  if(inivector)
    {
      delete inivector;
      inivector = NULL;
    }

  if(result)
    {
      delete result;
      result = NULL;
    }

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERTYPE CIPHER::GetType()
* @brief      Get type
* @ingroup    CIPHER
* 
* @return     CIPHERTYPE : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERTYPE CIPHER::GetType()
{ 
  return type;                                      
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHER::SetType(CIPHERTYPE type)
* @brief      Set type
* @ingroup    CIPHER
* 
* @param[in]  type : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHER::SetType(CIPHERTYPE type)
{ 
  this->type = type;                                
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERCHAININGMODE CIPHER::GetChainingMode()
* @brief      Get chaining mode
* @ingroup    CIPHER
* 
* @return     CIPHERCHAININGMODE : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERCHAININGMODE CIPHER::GetChainingMode()
{ 
  return chainingmode;                              
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHER::SetChainingMode(CIPHERCHAININGMODE chainingmode)
* @brief      Set chaining mode
* @ingroup    CIPHER
* 
* @param[in]  chainingmode : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHER::SetChainingMode(CIPHERCHAININGMODE chainingmode)
{ 
  this->chainingmode = chainingmode;                
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBUFFER_PADDINGTYPE CIPHER::GetPaddingType()
* @brief      Get padding type
* @ingroup    CIPHER
* 
* @return     XBUFFER_PADDINGTYPE : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER_PADDINGTYPE CIPHER::GetPaddingType()
{ 
  return paddingtype;                               
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHER::SetPaddingType(XBUFFER_PADDINGTYPE paddingtype)
* @brief      Set padding type
* @ingroup    CIPHER
* 
* @param[in]  paddingtype : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHER::SetPaddingType(XBUFFER_PADDINGTYPE paddingtype)
{ 
  this->paddingtype = paddingtype;                  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int CIPHER::GetPaddingAdjustSize()
* @brief      Get padding adjust size
* @ingroup    CIPHER
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int CIPHER::GetPaddingAdjustSize()
{ 
  return paddingadjustsize;                         
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHER::SetPaddingAdjustSize(int paddingadjustsize)
* @brief      Set padding adjust size
* @ingroup    CIPHER
* 
* @param[in]  paddingadjustsize : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHER::SetPaddingAdjustSize(int paddingadjustsize)
{ 
  this->paddingadjustsize = paddingadjustsize;      
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERKEY* CIPHER::GetKey(CIPHERKEYTYPE type)
* @brief      Get key
* @ingroup    CIPHER
* 
* @param[in]  type : 
* 
* @return     CIPHERKEY* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERKEY* CIPHER::GetKey(CIPHERKEYTYPE type)
{
  int index;

  switch(type)
    {
      case CIPHERKEYTYPE_SYMMETRICAL  :
      case CIPHERKEYTYPE_RSA_PUBLIC   : index = 0;   break;
      case CIPHERKEYTYPE_RSA_PRIVATE  : index = 1;   break;
                      default         : return NULL;
    }

  return keys[index];
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHER::SetKey(CIPHERKEY* key, bool integritycheck)
* @brief      Set key
* @ingroup    CIPHER
* 
* @param[in]  key : 
* @param[in]  integritycheck : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHER::SetKey(CIPHERKEY* key, bool integritycheck)
{
  if(!key) return false;

  int index;

  switch(key->GetType())
    {
      case CIPHERKEYTYPE_SYMMETRICAL  :
      case CIPHERKEYTYPE_RSA_PUBLIC   : index = 0;   break;
      case CIPHERKEYTYPE_RSA_PRIVATE  : index = 1;   break;
                default               : return false;
    }

  return SetKey(index, key);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE* CIPHER::GetInitVector()
* @brief      Get init vector
* @ingroup    CIPHER
* 
* @return     XBYTE* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE* CIPHER::GetInitVector()
{
  if(!inivector) return NULL;

  return inivector->Get();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHER::SetInitVector(XBYTE* vector, XDWORD size)
* @brief      Set init vector
* @ingroup    CIPHER
* 
* @param[in]  vector : 
* @param[in]  size : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHER::SetInitVector(XBYTE* vector, XDWORD size)
{
  if(!vector)    return false;
  if(!inivector) return false;

  inivector->Delete();
  inivector->Add(vector,size);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHER::SetInitVector(XBUFFER& vector)
* @brief      Set init vector
* @ingroup    CIPHER
* 
* @param[in]  vector : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHER::SetInitVector(XBUFFER& vector)
{
  inivector->Delete();
  inivector->Add(vector);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHER::Cipher(XBYTE* input,XDWORD size)
* @brief      Cipher
* @ingroup    CIPHER
* 
* @param[in]  input : 
* @param[in]  size : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHER::Cipher(XBYTE* input,XDWORD size)
{
  result->Delete();
  result->Add(input, size);

  CIPHERKEYSYMMETRICAL* key = (CIPHERKEYSYMMETRICAL*)GetKey(0);
  if(!key)                return false;
  if(!key->Get()->Get())  return false;

  int indexkey = 0;

  for(XDWORD c=0; c<size; c++)
    {
      result->Get()[c] ^= key->Get()->GetByte(indexkey);

      indexkey++;
      if(indexkey >= (int)key->Get()->GetSize()) indexkey = 0;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHER::Cipher(XBUFFER& input)
* @brief      Cipher
* @ingroup    CIPHER
* 
* @param[in]  input : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHER::Cipher(XBUFFER& input)
{
  return Cipher(input.Get(), input.GetSize());
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHER::Uncipher(XBYTE* input,XDWORD size)
* @brief      Uncipher
* @ingroup    CIPHER
* 
* @param[in]  input : 
* @param[in]  size : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHER::Uncipher(XBYTE* input,XDWORD size)
{
  return Cipher(input,size);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHER::Uncipher(XBUFFER& input)
* @brief      Uncipher
* @ingroup    CIPHER
* 
* @param[in]  input : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHER::Uncipher(XBUFFER& input)
{
  return Uncipher(input.Get(), input.GetSize());
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE* CIPHER::GetResult(int& resultsize)
* @brief      Get result
* @ingroup    CIPHER
* 
* @param[in]  resultsize : 
* 
* @return     XBYTE* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE* CIPHER::GetResult(int& resultsize)
{
  if(!result) return NULL;

  resultsize = result->GetSize();

  return result->Get();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBUFFER* CIPHER::GetResult()
* @brief      Get result
* @ingroup    CIPHER
* 
* @return     XBUFFER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* CIPHER::GetResult()
{
  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHER::GetResultString(XSTRING& stringhex)
* @brief      Get result string
* @ingroup    CIPHER
* 
* @param[in]  stringhex : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHER::GetResultString(XSTRING& stringhex)
{
  stringhex.Empty();

  XBUFFER* xbuffer = GetResult();
  if(!xbuffer) return false;

  if(!xbuffer->GetSize()) return false;

  for(int c=0;c<(int)xbuffer->GetSize();c++)
    {
      XSTRING strbyte;

      strbyte.Format(__L("%02x"),xbuffer->GetByte(c));

      stringhex += strbyte;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERKEY* CIPHER::GetKey(int index)
* @brief      Get key
* @ingroup    CIPHER
* 
* @param[in]  index : 
* 
* @return     CIPHERKEY* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERKEY* CIPHER::GetKey(int index)
{
  if(index < 0)              return NULL;
  if(index >= CIPHERMAXKEYS) return NULL;

  return keys[index];
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHER::SetKey(int index, CIPHERKEY* key)
* @brief      Set key
* @ingroup    CIPHER
* 
* @param[in]  index : 
* @param[in]  key : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHER::SetKey(int index, CIPHERKEY* key)
{
  if(index < 0)               return false;
  if(index >= CIPHERMAXKEYS) return false;

  this->keys[index] = key;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHER::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHER::Clean()
{
  paddingtype  = XBUFFER_PADDINGTYPE_NONE;
  chainingmode = CIPHERCHAININGMODE_UNKNOWN;

  for(int c=0; c<CIPHERMAXKEYS; c++)
    {
      keys[c] = NULL;
    }

  inivector = NULL;
  result    = NULL;
}


#pragma endregion
