/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       CipherRSA.cpp
* 
* @class      CIPHERRSA
* @brief      Cipher RSA class
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

#include "CipherRSA.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "XFactory.h"
#include "XRand.h"
#include "XBER.h"

#include "Hash.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

CIPHERRSA* CIPHERRSA::instance = NULL;

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


#pragma region CIPHERRSA_CONTEXT


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERRSA_CONTEXT::CIPHERRSA_CONTEXT()
* @brief      Constructor of class
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERRSA_CONTEXT::CIPHERRSA_CONTEXT()

{
  Ini();
  Clean();
};


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERRSA_CONTEXT::~CIPHERRSA_CONTEXT()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERRSA_CONTEXT::~CIPHERRSA_CONTEXT()
{
  End();
  Clean();
};


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERRSA_CONTEXT::Ini()
* @brief      Ini
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERRSA_CONTEXT::Ini()
{
  N.Ini();
  E.Ini();
  D.Ini();
  P.Ini();
  Q.Ini();
  DP.Ini();
  DQ.Ini();
  QP.Ini();
  RN.Ini();
  RP.Ini();
  RQ.Ini();
  #if !defined(CIPHERRSA_RSANOCRT)
  Vi.Ini();
  Vf.Ini();
  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERRSA_CONTEXT::End()
* @brief      End
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERRSA_CONTEXT::End()
{
  N.End();
  E.End();
  D.End();
  P.End();
  Q.End();
  DP.End();
  DQ.End();
  QP.End();
  RN.End();
  RP.End();
  RQ.End();
  #if !defined(CIPHERRSA_RSANOCRT)
  Vi.End();
  Vf.End();
  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERRSA_CONTEXT::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERRSA_CONTEXT::Clean()
{
  len     = 0;
  padding = 0;
  hashID  = 0;
}


#pragma endregion 


#pragma region CIPHERRSA


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERRSA::CIPHERRSA()
* @brief      Constructor of class
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERRSA::CIPHERRSA() : CIPHER()
{
  Clean();

  type = CIPHERTYPE_RSA;

  GEN_XFACTORY_CREATE(xrand, CreateRand())

  CIPHERRSA::instance = this;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERRSA::~CIPHERRSA()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERRSA::~CIPHERRSA()
{
  GEN_XFACTORY.DeleteRand(xrand);

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::SetKey(CIPHERKEY* key, bool integritycheck)
* @brief      Set key
* @ingroup    CIPHER
* 
* @param[in]  key : 
* @param[in]  integritycheck : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::SetKey(CIPHERKEY* key, bool integritycheck)
{
  if(!key) return false;

  switch(key->GetType())
    {
      case CIPHERKEYTYPE_SYMMETRICAL  : return false;

      case CIPHERKEYTYPE_RSA_PUBLIC   : { CIPHERKEYPUBLICRSA* publickey = (CIPHERKEYPUBLICRSA*)key;

                                          publickey->Get(context.N, context.E); // Assign key public to context

                                          if(integritycheck)
                                            {
                                              if(!CheckContextPublicKey()) return false;
                                            }
                                        }
                                        break;

      case CIPHERKEYTYPE_RSA_PRIVATE  : { CIPHERKEYPRIVATERSA* privatekey = (CIPHERKEYPRIVATERSA*)key;
                                          XMPINTEGER           P1;
                                          XMPINTEGER           Q1;

                                          privatekey->Get(context.P, context.Q, context.D); // Assign key public to context

                                          if(!P1.SubtractionSigned(&context.P, 1))                  return false;
                                          if(!Q1.SubtractionSigned(&context.Q, 1))                  return false;
                                          if(!context.DP.Module(&context.DP, &context.D, &P1))      return false;
                                          if(!context.DQ.Module(&context.DQ, &context.D, &Q1))      return false;
                                          if(!context.QP.ModularInverse(&context.Q, &context.P))    return false;

                                          if(integritycheck)
                                            {
                                              if(!CheckContextPrivateKey()) return false;
                                            }

                                        }
                                        break;
                          default     : break;

    }

  return CIPHER::SetKey(key);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::Cipher(XBYTE* input, XDWORD size, CIPHERKEYTYPE keytouse, CIPHERRSAPKCS1VERSION pkcs1version)
* @brief      Cipher
* @ingroup    CIPHER
* 
* @param[in]  input : 
* @param[in]  size : 
* @param[in]  keytouse : 
* @param[in]  pkcs1version : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::Cipher(XBYTE* input, XDWORD size, CIPHERKEYTYPE keytouse, CIPHERRSAPKCS1VERSION pkcs1version)
{
  if(!size) return false;

  int keysize   = GetKeySizeInBytes(keytouse);
  if(!keysize) return false;

  XBUFFER _input;
  XBUFFER output;
  XBUFFER inputparcial;
  int     sizeblock = (keysize-3);
  int     rest      = (size%sizeblock);
  int     nblocks   = (size/sizeblock);
  bool    status    = false;

  _input.Add(input, size);

  result->Delete();

  for(int c=0; c<(nblocks+(rest?1:0)); c++)
    {
      int dosizeblock = sizeblock;

      inputparcial.Delete();
      output.Delete();

      if(c==nblocks) dosizeblock = rest;

      inputparcial.Add(&_input.Get()[sizeblock*c], dosizeblock);

      switch(pkcs1version)
        {
          case CIPHERRSAPKCS1VERSIONV15   : status = Cipher_PKCS1_V15(inputparcial.Get(), dosizeblock,  output, keytouse, pkcs1version, GenerateRandom, NULL);
                                            result->Add(output);
                                            break;

          case CIPHERRSAPKCS1VERSIONV21   : break;
        }
    }


  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::Cipher(XBUFFER& input, CIPHERKEYTYPE keytouse, CIPHERRSAPKCS1VERSION pkcs1version)
* @brief      Cipher
* @ingroup    CIPHER
* 
* @param[in]  input : 
* @param[in]  keytouse : 
* @param[in]  pkcs1version : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::Cipher(XBUFFER& input, CIPHERKEYTYPE keytouse, CIPHERRSAPKCS1VERSION pkcs1version)
{
  return Cipher(input.Get(), input.GetSize(), keytouse, pkcs1version);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::Uncipher(XBYTE* input,XDWORD size, CIPHERKEYTYPE keytouse, CIPHERRSAPKCS1VERSION pkcs1version)
* @brief      Uncipher
* @ingroup    CIPHER
* 
* @param[in]  input : 
* @param[in]  size : 
* @param[in]  keytouse : 
* @param[in]  pkcs1version : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::Uncipher(XBYTE* input,XDWORD size, CIPHERKEYTYPE keytouse, CIPHERRSAPKCS1VERSION pkcs1version)
{
  if(!size) return false;

  int keysize = GetKeySizeInBytes(keytouse);
  if(!keysize) return false;

  XBUFFER _input;
  XBUFFER output;
  XBUFFER inputparcial;
  int     sizeblock = keysize;
  int     rest      = (size%sizeblock);
  int     nblocks   = (size/sizeblock);
  bool    status    = false;

  _input.Add(input, size);

  result->Delete();

  for(int c=0; c<(nblocks+(rest?1:0)); c++)
    {
      int dosizeblock = sizeblock;

      inputparcial.Delete();
      output.Delete();

      if(c==nblocks) dosizeblock = rest;

      inputparcial.Add(&_input.Get()[sizeblock*c], dosizeblock);

      switch(pkcs1version)
        {
          case CIPHERRSAPKCS1VERSIONV15   : status = Uncipher_PKCS1_V15(inputparcial.Get(), dosizeblock,  output, keytouse, pkcs1version, GenerateRandom, NULL);
                                            result->Add(output);
                                            break;

          case CIPHERRSAPKCS1VERSIONV21   : break;
        }
    }


  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::Uncipher(XBUFFER& input, CIPHERKEYTYPE keytouse, CIPHERRSAPKCS1VERSION pkcs1version)
* @brief      Uncipher
* @ingroup    CIPHER
* 
* @param[in]  input : 
* @param[in]  keytouse : 
* @param[in]  pkcs1version : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::Uncipher(XBUFFER& input, CIPHERKEYTYPE keytouse, CIPHERRSAPKCS1VERSION pkcs1version)
{
  return Uncipher(input.Get(), input.GetSize(), keytouse, pkcs1version);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::Sign(XBYTE* input, XDWORD size, CIPHERKEYTYPE keytouse, HASH* hash, CIPHERRSAPKCS1VERSION pkcs1version)
* @brief      Sign
* @ingroup    CIPHER
* 
* @param[in]  input : 
* @param[in]  size : 
* @param[in]  keytouse : 
* @param[in]  hash : 
* @param[in]  pkcs1version : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::Sign(XBYTE* input, XDWORD size, CIPHERKEYTYPE keytouse, HASH* hash, CIPHERRSAPKCS1VERSION pkcs1version)
{
  if(!hash) return false;

  XBER      beroui;
  XBER      bernull;
  XBER      berhash;
  XBER      berseq1;
  XBER      berseq2;
  XSTRING*  OUINoSignstring = hash->GetOUINoSign();
  XBUFFER   sign;

  if(!OUINoSignstring) return false;

  if(!OUINoSignstring->GetSize()) return false;

  beroui.SetOID(OUINoSignstring->Get());
  bernull.SetNULL();

  hash->Do((XBYTE*)input, size);

  berhash.SetOCTETSTRING((*hash->GetResult()));

  berseq1.Sequence_AddTo(beroui);
  berseq1.Sequence_AddTo(bernull);

  berseq1.SetTagType(berseq1.GetTagType()|(XBYTE)(XBERTYPE_ISCONSTRUCTED));

  berseq2.Sequence_AddTo(berseq1);
  berseq2.Sequence_AddTo(berhash);
  berseq2.SetTagType(berseq2.GetTagType()|(XBYTE)(XBERTYPE_ISCONSTRUCTED));

  berseq2.GetDump(sign);

  return Cipher(sign, CIPHERKEYTYPE_RSA_PRIVATE, pkcs1version);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::Sign(XBUFFER& input, CIPHERKEYTYPE keytouse, HASH* hash, CIPHERRSAPKCS1VERSION pkcs1version)
* @brief      Sign
* @ingroup    CIPHER
* 
* @param[in]  input : 
* @param[in]  keytouse : 
* @param[in]  hash : 
* @param[in]  pkcs1version : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::Sign(XBUFFER& input, CIPHERKEYTYPE keytouse, HASH* hash, CIPHERRSAPKCS1VERSION pkcs1version)
{
  return Sign(input.Get(), input.GetSize(), keytouse, hash, pkcs1version);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERRSA* CIPHERRSA::GetInstance()
* @brief      Get instance
* @ingroup    CIPHER
* 
* @return     CIPHERRSA* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERRSA* CIPHERRSA::GetInstance()
{ 
  return instance; 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XRAND* CIPHERRSA::GetXRand()
* @brief      Get X rand
* @ingroup    CIPHER
* 
* @return     XRAND* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XRAND* CIPHERRSA::GetXRand()
{ 
  return xrand;    
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::GenerateKeys(XDWORD nbits, int exponent, CIPHERKEYPUBLICRSA& publickey, CIPHERKEYPRIVATERSA& privatekey, XMPINTEGER_FUNCRANDOM funcrandom, void* paramrandom)
* @brief      Generate keys
* @ingroup    CIPHER
* 
* @param[in]  nbits : 
* @param[in]  exponent : 
* @param[in]  publickey : 
* @param[in]  privatekey : 
* @param[in]  funcrandom : 
* @param[in]  paramrandom : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::GenerateKeys(XDWORD nbits, int exponent, CIPHERKEYPUBLICRSA& publickey, CIPHERKEYPRIVATERSA& privatekey, XMPINTEGER_FUNCRANDOM funcrandom, void* paramrandom)
{
  XMPINTEGER P1;
  XMPINTEGER Q1;
  XMPINTEGER H;
  XMPINTEGER G;
  bool       status;

  if(nbits<128 || exponent<3 )  return false;

  P1.Ini();
  Q1.Ini();
  H.Ini();
  G.Ini();

  // find primes P and Q with Q < P so that: GCD( E, (P-1)*(Q-1) ) == 1
  status = context.E.LeftSet(exponent);
  if(status)
    {
      do{ if(!context.P.GeneratePrime((nbits+1)>>1, 0, funcrandom?funcrandom:(XMPINTEGER_FUNCRANDOM)GenerateRandom, NULL)) return false;
          if(!context.Q.GeneratePrime((nbits+1)>>1, 0, funcrandom?funcrandom:(XMPINTEGER_FUNCRANDOM)GenerateRandom, NULL)) return false;

          if(context.P.CompareSignedValues(context.Q) < 0)  context.P.Swap(&context.Q);

          if(context.P.CompareSignedValues(context.Q) == 0) continue;

          if(!context.N.Multiplication(&context.P, &context.Q)) return false;

          if(context.N.GetMSB()!=nbits) continue;

          if(!P1.SubtractionSigned(&context.P, 1)) return false;
          if(!Q1.SubtractionSigned(&context.Q, 1)) return false;

          if(!H.Multiplication(&P1, &Q1)) return false;

          if(!G.GreatestCommonDivisor(&G, &context.E, &H)) return false;

        } while(G.CompareSignedValues(1) != 0);


      // D  = E^-1 mod ((P-1)*(Q-1))
      // DP = D mod (P - 1)
      // DQ = D mod (Q - 1)
      // QP = Q^-1 mod P

      if(!context.D.ModularInverse(&context.E, &H))           return false;
      if(!context.DP.Module(&context.DP, &context.D, &P1))    return false;
      if(!context.DP.Module(&context.DQ, &context.D, &Q1))    return false;
      if(!context.QP.ModularInverse(&context.Q, &context.P))  return false;

      context.len = (context.N.GetMSB() + 7) >> 3;
    }

  P1.End();
  Q1.End();
  H.End();
  G.End();

  if(!status)
    {
      context.End();
    }
   else
    {
      publickey.Set(context.N, context.E);
      privatekey.Set(context.P, context.Q, context.D);
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::CheckContextPublicKey()
* @brief      Check context public key
* @ingroup    CIPHER
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::CheckContextPublicKey()
{
  if(!context.N.GetLimbs()      || !context.E.GetLimbs())                       return false;
  if((context.N.GetLimbs()[0] & 1) == 0 || (context.E.GetLimbs()[0] & 1 ) == 0) return false;
  if((context.N.GetMSB() < 128) || (context.N.GetMSB() >  XMPINTEGER_MAXBITS )) return false;
  if((context.E.GetMSB() < 2)   || (context.E.GetMSB() >  64 ))                 return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::CheckContextPrivateKey()
* @brief      Check context private key
* @ingroup    CIPHER
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::CheckContextPrivateKey()
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
  XMPINTEGER DP;
  XMPINTEGER DQ;
  XMPINTEGER QP;

  if(!CheckContextPublicKey()) return false;

  if((!context.P.GetLimbs()) || (!context.Q.GetLimbs()) || (!context.D.GetLimbs()))  return false;

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
  DP.Ini();
  DQ.Ini();
  QP.Ini();

  if(!PQ.Multiplication(&context.P, &context.Q))    return false;
  if(!DE.Multiplication(&context.D, &context.E))    return false;
  if(!P1.SubtractionSigned(&context.P, 1))          return false;
  if(!Q1.SubtractionSigned(&context.Q, 1))          return false;
  if(!H.Multiplication(&P1, &Q1))                   return false;
  if(!G.GreatestCommonDivisor(&G, &context.E, &H))  return false;
  if(!G2.GreatestCommonDivisor(&G2, &P1, &Q1))      return false;
  if(!L1.Division(&L1, &L2, &H, &G2 ))              return false;
  if(!I.Module(&I, &DE, &L1))                       return false;
  if(!DP.Module( &DP, &context.D, &P1))             return false;
  if(!DQ.Module(&DQ, &context.D, &Q1))              return false;
  if(!QP.ModularInverse(&context.Q, &context.P))    return false;

  // Check for a valid PKCS1v2 private key

  if((PQ.CompareSignedValues(context.N ) != 0) || (DP.CompareSignedValues(context.DP) != 0) ||
     (DQ.CompareSignedValues(context.DQ) != 0) || (QP.CompareSignedValues(context.QP) != 0) ||
     (L2.CompareSignedValues(0)          != 0) || (I.CompareSignedValues(1)           != 0) ||
     (G.CompareSignedValues(1)           != 0)) return false;

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
  DP.End();
  DQ.End();
  QP.End();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::Cipher_PKCS1_V15(XBYTE* buffer, XDWORD size, XBUFFER& output, CIPHERKEYTYPE keytypetouse, CIPHERRSAPKCS1VERSION pkcs1version, XMPINTEGER_FUNCRANDOM funcrandom, void* paramrandom)
* @brief      Cipher PKCS1V15
* @ingroup    CIPHER
* 
* @param[in]  buffer : 
* @param[in]  size : 
* @param[in]  output : 
* @param[in]  keytypetouse : 
* @param[in]  pkcs1version : 
* @param[in]  funcrandom : 
* @param[in]  paramrandom : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::Cipher_PKCS1_V15(XBYTE* buffer, XDWORD size, XBUFFER& output, CIPHERKEYTYPE keytypetouse, CIPHERRSAPKCS1VERSION pkcs1version,  XMPINTEGER_FUNCRANDOM funcrandom, void* paramrandom)
{
  XBUFFER input;
  int     nb_pad;
  bool    status;

  if(!funcrandom) return false;

  int keysize = GetKeySizeInBytes(keytypetouse);
  if(!keysize) return false;

  //if(keysize < (size + 11)) return false;

  nb_pad = keysize - 3 - size;

  input.Add((XBYTE)0);

  if(keytypetouse == CIPHERKEYTYPE_RSA_PUBLIC)
    {
      XBYTE data = 0;

      input.Add((XBYTE)CIPHERRSA_CIPHERDATA);

       while(nb_pad > 0)
        {
          if(funcrandom(&data, 1, paramrandom))
            {
              input.Add((XBYTE)data);
              nb_pad--;
            }
        }
    }
   else
    {
      input.Add((XBYTE)CIPHERRSA_SINGDATA);

       while(nb_pad-- > 0)
         {
           input.Add((XBYTE)0xFF);
         }
    }


  input.Add((XBYTE)0);
  input.Add(buffer,size);

  status = false;

  switch(keytypetouse)
    {
      case CIPHERKEYTYPE_RSA_PUBLIC   : status = DoRSAPublicOperation(input, output);                              break;
      case CIPHERKEYTYPE_RSA_PRIVATE  : status = DoRSAPrivateOperation(input, output, funcrandom, paramrandom);    break;
                            default   : break;
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::Uncipher_PKCS1_V15(XBYTE* buffer, XDWORD size, XBUFFER& output, CIPHERKEYTYPE keytypetouse, CIPHERRSAPKCS1VERSION pkcs1version, XMPINTEGER_FUNCRANDOM funcrandom, void* paramrandom)
* @brief      Uncipher PKCS1V15
* @ingroup    CIPHER
* 
* @param[in]  buffer : 
* @param[in]  size : 
* @param[in]  output : 
* @param[in]  keytypetouse : 
* @param[in]  pkcs1version : 
* @param[in]  funcrandom : 
* @param[in]  paramrandom : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::Uncipher_PKCS1_V15(XBYTE* buffer, XDWORD size, XBUFFER& output, CIPHERKEYTYPE keytypetouse, CIPHERRSAPKCS1VERSION pkcs1version,  XMPINTEGER_FUNCRANDOM funcrandom, void* paramrandom)
{
  XBUFFER   input;
  XBUFFER   _output;
  XBYTE     pad_chk;
  int       pad_count = 0;
  XBYTE     pad_done = 0;
  bool      status;

  if(!funcrandom) return false;

  int keysize = GetKeySizeInBytes(keytypetouse);
  if(!keysize) return false;

  if((keysize < 16) || (keysize > XMPINTEGER_MAXSIZE)) return false;

  input.Add(buffer, size);

  status = false;

  switch(keytypetouse)
    {
      case CIPHERKEYTYPE_RSA_PUBLIC   : status = DoRSAPublicOperation(input , _output);                            break;
      case CIPHERKEYTYPE_RSA_PRIVATE  : status = DoRSAPrivateOperation(input, _output, funcrandom, paramrandom);   break;
                            default   : break;
    }

  if(!status) return false;

  int index = 0;

  pad_chk  = 0;
  pad_chk |= _output.GetByte(index++);

  pad_chk |= (_output.GetByte(index++)) ^  ((keytypetouse == CIPHERKEYTYPE_RSA_PRIVATE)? CIPHERRSA_CIPHERDATA : CIPHERRSA_SINGDATA);

  for(XDWORD i=0; i<size-3; i++)
    {
      pad_done  |= (_output.GetByte(index+i) == 0);
      pad_count += (pad_done == 0);
    }

  index += pad_count;
  pad_chk |= _output.GetByte(index++); // Must be zero


  if(pad_chk) return false;

  output.Add(&_output.Get()[index], (size-index));

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::DoRSAPublicOperation(XBUFFER& input, XBUFFER& output)
* @brief      Do RSA public operation
* @ingroup    CIPHER
* 
* @param[in]  input : 
* @param[in]  output : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::DoRSAPublicOperation(XBUFFER& input, XBUFFER& output)
{
  int keysize = GetKeySizeInBytes(CIPHERKEYTYPE_RSA_PUBLIC);
  if(!keysize) return false;

  XMPINTEGER  T;

  T.Ini();

  if(!T.SetFromXBuffer(input)) return false;

  if(T.CompareSignedValues(context.N)>=0) return false;

  if(!T.SlidingWindowExponentiation(&T, &context.E, &context.N, &context.RN)) return false;

  if(!T.GetToXBuffer(output, keysize)) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::DoRSAPrivateOperation(XBUFFER& input, XBUFFER& output, XMPINTEGER_FUNCRANDOM funcrandom, void* paramrandom)
* @brief      Do RSA private operation
* @ingroup    CIPHER
* 
* @param[in]  input : 
* @param[in]  output : 
* @param[in]  funcrandom : 
* @param[in]  paramrandom : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::DoRSAPrivateOperation(XBUFFER& input, XBUFFER& output, XMPINTEGER_FUNCRANDOM funcrandom, void* paramrandom)
{
  XMPINTEGER  T;
  XMPINTEGER  T1;
  XMPINTEGER  T2;

  #if !defined(CIPHERRSA_RSANOCRT)
  XMPINTEGER* Vi;
  XMPINTEGER* Vf;

  Vi = &context.Vi;
  Vf = &context.Vf;
  #endif

  T.Ini();
  T1.Ini();
  T2.Ini();

  if(!T.SetFromXBuffer(input)) return false;

  if(T.CompareSignedValues(context.N)>=0) return false;

  #if defined(CIPHERRSA_RSANOCRT)
    if(!T.SlidingWindowExponentiation(&T, &context.D, &context.N, &context.RN)) return false;
  #else
    if(funcrandom != NULL)
      {
        // Blinding   T = T * Vi mod N
        if(!PrepareBlinding(Vi, Vf, funcrandom, paramrandom)) return false;
        if(!T.Multiplication(&T, Vi ))                        return false;
        if(!T.Module( &T, &T, &context.N))                    return false;
      }

    // faster decryption using the CRT
    // T1 = input ^ dP mod P
    // T2 = input ^ dQ mod Q

    if(!T1.SlidingWindowExponentiation(&T, &context.DP, &context.P, &context.RP)) return false;
    if(!T2.SlidingWindowExponentiation(&T, &context.DQ, &context.Q, &context.RQ)) return false;

    // T = (T1 - T2) * (Q^-1 mod P) mod P
    if(!T.SubtractionSigned(&T1, &T2))      return false;
    if(!T1.Multiplication(&T, &context.QP)) return false;
    if(!T.Module(&T, &T1, &context.P))      return false;

    // T = T2 + T * Q
    if(!T1.Multiplication(&T, &context.Q )) return false;
    if(!T.AdditionSigned(&T2, &T1))         return false;

    if(funcrandom!= NULL)
      {
        // Unblind T = T * Vf mod N
        if(!T.Multiplication(&T, Vf))   return false;
        if(!T.Module(&T, &T, &context.N)) return false;
      }
  #endif

  int keysize = GetKeySizeInBytes(CIPHERKEYTYPE_RSA_PRIVATE);
  if(!keysize) return false;

  if(!T.GetToXBuffer(output, keysize)) return false;

  T.End();
  T1.End();
  T2.End();

  return true;
}


#if !defined(CIPHERRSA_RSANOCRT)
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::PrepareBlinding(XMPINTEGER* Vi, XMPINTEGER* Vf, XMPINTEGER_FUNCRANDOM funcrandom, void* paramrandom)
* @brief      Prepare blinding
* @ingroup    CIPHER
* 
* @param[in]  Vi : 
* @param[in]  Vf : 
* @param[in]  funcrandom : 
* @param[in]  paramrandom : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::PrepareBlinding(XMPINTEGER* Vi, XMPINTEGER* Vf, XMPINTEGER_FUNCRANDOM funcrandom, void* paramrandom)
{
  int count = 0;

  if(context.Vf.GetLimbs()!= NULL)
    {
      // We already have blinding values, just update them by squaring
      if(!context.Vi.Multiplication(&context.Vi, &context.Vi))      return false;
      if(!context.Vi.Module(&context.Vi, &context.Vi, &context.N )) return false;
      if(!context.Vf.Multiplication(&context.Vf, &context.Vf))      return false;
      if(!context.Vf.Module(&context.Vf, &context.Vf, &context.N )) return false;
    }
   else
    {
      int keysize = GetKeySizeInBytes(CIPHERKEYTYPE_RSA_PRIVATE);
      if(!keysize) return false;

      // Unblinding value: Vf = random number, invertible mod N
      do{ if(count++ > 10) return false;

          if(!context.Vf.FillRandom(keysize-1, funcrandom, paramrandom))             return false;
          if(!context.Vi.GreatestCommonDivisor(&context.Vi, &context.Vf, &context.N )) return false;

        } while(context.Vi.CompareSignedValues(1)!=0);

      // Blinding value: Vi =  Vf^(-e) mod N
      if(!context.Vi.ModularInverse( &context.Vf, &context.N))                                       return false;
      if(!context.Vi.SlidingWindowExponentiation(&context.Vi, &context.E, &context.N, &context.RN )) return false;
    }

  if(Vi != &context.Vi)
    {
      if(!Vi->CopyFrom(&context.Vi)) return false;
      if(!Vf->CopyFrom(&context.Vf)) return false;
    }

  return true;
}
#endif


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int CIPHERRSA::GetKeySizeInBytes(CIPHERKEYTYPE keytouse)
* @brief      Get key size in bytes
* @ingroup    CIPHER
* 
* @param[in]  keytouse : 
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int CIPHERRSA::GetKeySizeInBytes(CIPHERKEYTYPE keytouse)
{
  int keysize = 0;

  CIPHERKEY* key =  GetKey(CIPHERKEYTYPE_RSA_PRIVATE);
  if(!key) return 0;

  keysize = key->GetSizeInBytes();

  return keysize;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERRSA::GenerateRandom(XBYTE* buffer, XDWORD size, void* param)
* @brief      Generate random
* @ingroup    CIPHER
* 
* @param[in]  buffer : 
* @param[in]  size : 
* @param[in]  param : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERRSA::GenerateRandom(XBYTE* buffer, XDWORD size, void* param)
{
  if(!GetInstance()) return false;

  XRAND* xrand = instance->GetXRand();
  if(!xrand) return false;

  if(param != NULL) param = NULL;

  for(XDWORD c=0; c<size; c++)
    {
      buffer[c] = xrand->Max(255);
      if(!buffer[c]) buffer[c]++;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERRSA::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERRSA::Clean()
{
  xrand = NULL;
}


#pragma endregion


#pragma endregion 

