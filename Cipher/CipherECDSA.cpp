/**-------------------------------------------------------------------------------------------------------------------
*
* @file       CipherECDSA.cpp
*
* @class      CIPHERECDSA
* @brief      Cipher ECDSA class
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

#include "CipherECDSA.h"

#include <string.h>

#include "XFactory.h"
#include "XRand.h"

#include "CipherKeyECDSA.h"
#include "Hash.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- INTERNAL CLASSES ----------------------------------------------------------------------------------------------*/


class CIPHERECDSA_POINT
{
  public:

    CIPHERECDSA_POINT()
      {
        isinfinity = true;
      }

    XMPINTEGER                    X;
    XMPINTEGER                    Y;
    XMPINTEGER                    Z;
    bool                          isinfinity;
};



/*---- INTERNAL FUNCTIONS --------------------------------------------------------------------------------------------*/


static bool CIPHERECDSA_ModularMultiplication(XMPINTEGER& result, XMPINTEGER& value1, XMPINTEGER& value2,
                                               XMPINTEGER& modulus)
{
  XMPINTEGER multiplication;

  if(!multiplication.Multiplication(&value1, &value2)) return false;

  return result.Module(&result, &multiplication, &modulus);
}


static bool CIPHERECDSA_ModularMultiplication(XMPINTEGER& result, XMPINTEGER& value, int integer,
                                               XMPINTEGER& modulus)
{
  XMPINTEGER multiplication;

  if(!multiplication.Multiplication(&value, integer)) return false;

  return result.Module(&result, &multiplication, &modulus);
}


static bool CIPHERECDSA_ModularAddition(XMPINTEGER& result, XMPINTEGER& value1, XMPINTEGER& value2,
                                        XMPINTEGER& modulus)
{
  XMPINTEGER addition;

  if(!addition.AdditionSigned(&value1, &value2)) return false;

  return result.Module(&result, &addition, &modulus);
}


static bool CIPHERECDSA_ModularSubtraction(XMPINTEGER& result, XMPINTEGER& value1, XMPINTEGER& value2,
                                           XMPINTEGER& modulus)
{
  XMPINTEGER subtraction;

  if(!subtraction.SubtractionSigned(&value1, &value2)) return false;

  return result.Module(&result, &subtraction, &modulus);
}


static bool CIPHERECDSA_PointSet(CIPHERECDSA_POINT& point, XMPINTEGER& X, XMPINTEGER& Y)
{
  if(!point.X.CopyFrom(&X) || !point.Y.CopyFrom(&Y) || !point.Z.LeftSet(1)) return false;

  point.isinfinity = false;

  return true;
}


static bool CIPHERECDSA_PointDouble(CIPHERECDSA_POINT& point, XMPINTEGER& prime)
{
  XMPINTEGER delta;
  XMPINTEGER gamma;
  XMPINTEGER beta;
  XMPINTEGER alpha;
  XMPINTEGER gamma2;
  XMPINTEGER temporary1;
  XMPINTEGER temporary2;
  XMPINTEGER X3;
  XMPINTEGER Y3;
  XMPINTEGER Z3;

  if(point.isinfinity) return true;

  if(!point.Y.CompareSignedValues(0))
    {
      point.isinfinity = true;
      return true;
    }

  if(!CIPHERECDSA_ModularMultiplication(delta, point.Z, point.Z, prime) ||
     !CIPHERECDSA_ModularMultiplication(gamma, point.Y, point.Y, prime) ||
     !CIPHERECDSA_ModularMultiplication(beta, point.X, gamma, prime) ||
     !CIPHERECDSA_ModularSubtraction(temporary1, point.X, delta, prime) ||
     !CIPHERECDSA_ModularAddition(temporary2, point.X, delta, prime) ||
     !CIPHERECDSA_ModularMultiplication(alpha, temporary1, temporary2, prime) ||
     !CIPHERECDSA_ModularMultiplication(alpha, alpha, 3, prime) ||
     !CIPHERECDSA_ModularMultiplication(X3, alpha, alpha, prime) ||
     !CIPHERECDSA_ModularMultiplication(temporary1, beta, 8, prime) ||
     !CIPHERECDSA_ModularSubtraction(X3, X3, temporary1, prime) ||
     !CIPHERECDSA_ModularMultiplication(gamma2, gamma, gamma, prime) ||
     !CIPHERECDSA_ModularMultiplication(temporary1, beta, 4, prime) ||
     !CIPHERECDSA_ModularSubtraction(temporary1, temporary1, X3, prime) ||
     !CIPHERECDSA_ModularMultiplication(Y3, alpha, temporary1, prime) ||
     !CIPHERECDSA_ModularMultiplication(temporary2, gamma2, 8, prime) ||
     !CIPHERECDSA_ModularSubtraction(Y3, Y3, temporary2, prime) ||
     !CIPHERECDSA_ModularAddition(temporary1, point.Y, point.Z, prime) ||
     !CIPHERECDSA_ModularMultiplication(Z3, temporary1, temporary1, prime) ||
     !CIPHERECDSA_ModularSubtraction(Z3, Z3, gamma, prime) ||
     !CIPHERECDSA_ModularSubtraction(Z3, Z3, delta, prime))
    {
      return false;
    }

  if(!point.X.CopyFrom(&X3) || !point.Y.CopyFrom(&Y3) || !point.Z.CopyFrom(&Z3)) return false;

  return true;
}


static bool CIPHERECDSA_PointAddAffine(CIPHERECDSA_POINT& point, XMPINTEGER& affineX, XMPINTEGER& affineY,
                                       XMPINTEGER& prime)
{
  XMPINTEGER Z2;
  XMPINTEGER Z3temporary;
  XMPINTEGER U2;
  XMPINTEGER S2;
  XMPINTEGER H;
  XMPINTEGER R;
  XMPINTEGER HH;
  XMPINTEGER HHH;
  XMPINTEGER V;
  XMPINTEGER temporary;
  XMPINTEGER X3;
  XMPINTEGER Y3;
  XMPINTEGER pointZ3;

  if(point.isinfinity) return CIPHERECDSA_PointSet(point, affineX, affineY);

  if(!CIPHERECDSA_ModularMultiplication(Z2, point.Z, point.Z, prime) ||
     !CIPHERECDSA_ModularMultiplication(U2, affineX, Z2, prime) ||
     !CIPHERECDSA_ModularMultiplication(Z3temporary, Z2, point.Z, prime) ||
     !CIPHERECDSA_ModularMultiplication(S2, affineY, Z3temporary, prime) ||
     !CIPHERECDSA_ModularSubtraction(H, U2, point.X, prime) ||
     !CIPHERECDSA_ModularSubtraction(R, S2, point.Y, prime))
    {
      return false;
    }

  if(!H.CompareSignedValues(0))
    {
      if(!R.CompareSignedValues(0)) return CIPHERECDSA_PointDouble(point, prime);

      point.isinfinity = true;
      return true;
    }

  if(!CIPHERECDSA_ModularMultiplication(HH, H, H, prime) ||
     !CIPHERECDSA_ModularMultiplication(HHH, H, HH, prime) ||
     !CIPHERECDSA_ModularMultiplication(V, point.X, HH, prime) ||
     !CIPHERECDSA_ModularMultiplication(X3, R, R, prime) ||
     !CIPHERECDSA_ModularSubtraction(X3, X3, HHH, prime) ||
     !CIPHERECDSA_ModularMultiplication(temporary, V, 2, prime) ||
     !CIPHERECDSA_ModularSubtraction(X3, X3, temporary, prime) ||
     !CIPHERECDSA_ModularSubtraction(temporary, V, X3, prime) ||
     !CIPHERECDSA_ModularMultiplication(Y3, R, temporary, prime) ||
     !CIPHERECDSA_ModularMultiplication(temporary, point.Y, HHH, prime) ||
     !CIPHERECDSA_ModularSubtraction(Y3, Y3, temporary, prime) ||
     !CIPHERECDSA_ModularMultiplication(pointZ3, point.Z, H, prime))
    {
      return false;
    }

  if(!point.X.CopyFrom(&X3) || !point.Y.CopyFrom(&Y3) || !point.Z.CopyFrom(&pointZ3)) return false;

  return true;
}


static bool CIPHERECDSA_PointMultiply(CIPHERECDSA_POINT& point, XMPINTEGER& scalar, XMPINTEGER& affineX,
                                      XMPINTEGER& affineY, XMPINTEGER& prime, XDWORD curvebits)
{
  point.isinfinity = true;

  for(int bit=(int)curvebits-1; bit>=0; bit--)
    {
      if(!CIPHERECDSA_PointDouble(point, prime)) return false;

      if(scalar.GetBit(bit) && !CIPHERECDSA_PointAddAffine(point, affineX, affineY, prime)) return false;
    }

  return !point.isinfinity;
}


static bool CIPHERECDSA_PointToAffine(CIPHERECDSA_POINT& point, XMPINTEGER& affineX, XMPINTEGER& affineY,
                                      XMPINTEGER& prime)
{
  XMPINTEGER inverseZ;
  XMPINTEGER inverseZ2;
  XMPINTEGER inverseZ3;

  if(point.isinfinity || !point.Z.CompareSignedValues(0) ||
     !inverseZ.ModularInverse(&point.Z, &prime) ||
     !CIPHERECDSA_ModularMultiplication(inverseZ2, inverseZ, inverseZ, prime) ||
     !CIPHERECDSA_ModularMultiplication(inverseZ3, inverseZ2, inverseZ, prime) ||
     !CIPHERECDSA_ModularMultiplication(affineX, point.X, inverseZ2, prime) ||
     !CIPHERECDSA_ModularMultiplication(affineY, point.Y, inverseZ3, prime))
    {
      return false;
    }

  return true;
}


static bool CIPHERECDSA_PointCheck(XMPINTEGER& X, XMPINTEGER& Y, XMPINTEGER& prime,
                                   XMPINTEGER& coefficientA, XMPINTEGER& coefficientB)
{
  XMPINTEGER left;
  XMPINTEGER X2;
  XMPINTEGER X3;
  XMPINTEGER AX;
  XMPINTEGER right;

  if((X.CompareSignedValues(prime) >= 0) || (Y.CompareSignedValues(prime) >= 0) ||
     !CIPHERECDSA_ModularMultiplication(left, Y, Y, prime) ||
     !CIPHERECDSA_ModularMultiplication(X2, X, X, prime) ||
     !CIPHERECDSA_ModularMultiplication(X3, X2, X, prime) ||
     !CIPHERECDSA_ModularMultiplication(AX, coefficientA, X, prime) ||
     !CIPHERECDSA_ModularAddition(right, X3, AX, prime) ||
     !CIPHERECDSA_ModularAddition(right, right, coefficientB, prime))
    {
      return false;
    }

  return !left.CompareSignedValues(right);
}


static bool CIPHERECDSA_DERLength(XBYTE* data, XDWORD size, XDWORD& index, XDWORD& length)
{
  XBYTE value;

  length = 0;

  if(!data || (index >= size)) return false;

  value = data[index++];
  if(!(value & 0x80))
    {
      length = value;
      return (length <= (size - index));
    }

  XDWORD nbytes = value & 0x7F;
  if(!nbytes || (nbytes > sizeof(XDWORD)) || (nbytes > (size - index)) || !data[index]) return false;

  for(XDWORD c=0; c<nbytes; c++)
    {
      if(length > (0xFFFFFFFFUL >> 8)) return false;
      length = (length << 8) | data[index++];
    }

  if(length < 0x80) return false;

  return (length <= (size - index));
}


static bool CIPHERECDSA_DERInteger(XBYTE* data, XDWORD size, XDWORD& index, XMPINTEGER& integer,
                                   XMPINTEGER& order, XDWORD maxsize)
{
  XDWORD length;
  XDWORD offset;

  if(!data || (index >= size) || (data[index++] != 0x02) ||
     !CIPHERECDSA_DERLength(data, size, index, length) || !length || (data[index] & 0x80))
    {
      return false;
    }

  offset = index;
  if((length > 1) && !data[offset])
    {
      if(!(data[offset+1] & 0x80)) return false;

      offset++;
      length--;
    }

  if(!length || (length > maxsize) ||
     !integer.ImportFromBinary(&data[offset], length) ||
     (integer.CompareSignedValues(0) <= 0) || (integer.CompareSignedValues(order) >= 0))
    {
      return false;
    }

  index += ((offset - index) + length);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* Curve-selection helpers: map the CIPHERTYPE the constructor was given to the matching public-key type
* (CIPHERKEYECDSA::GetType()) and the required TLS hash algorithm (RFC 8446 4.2.3 curve/hash pairing), so the
* rest of the class never has to special-case a curve outside of Parameters_Set().
* --------------------------------------------------------------------------------------------------------------------*/
static CIPHERKEYTYPE CIPHERECDSA_ExpectedKeyType(CIPHERTYPE curvetype)
{
  switch(curvetype)
    {
      case CIPHERTYPE_ECDSA_SECP256R1 : return CIPHERKEYTYPE_ECDSA_SECP256R1_PUBLIC;
      case CIPHERTYPE_ECDSA_SECP384R1 : return CIPHERKEYTYPE_ECDSA_SECP384R1_PUBLIC;
      case CIPHERTYPE_ECDSA_SECP521R1 : return CIPHERKEYTYPE_ECDSA_SECP521R1_PUBLIC;
                                default : break;
    }

  return CIPHERKEYTYPE_UNKNOWN;
}


static HASHTYPE CIPHERECDSA_RequiredHashType(CIPHERTYPE curvetype)
{
  switch(curvetype)
    {
      case CIPHERTYPE_ECDSA_SECP256R1 : return HASHTYPE_SHA256;
      case CIPHERTYPE_ECDSA_SECP384R1 : return HASHTYPE_SHA384;
      case CIPHERTYPE_ECDSA_SECP521R1 : return HASHTYPE_SHA512;
                                default : break;
    }

  return HASHTYPE_NONE;
}


static CIPHERKEYTYPE CIPHERECDSA_ExpectedPrivateKeyType(CIPHERTYPE curvetype)
{
  switch(curvetype)
    {
      case CIPHERTYPE_ECDSA_SECP256R1 : return CIPHERKEYTYPE_ECDSA_SECP256R1_PRIVATE;
      case CIPHERTYPE_ECDSA_SECP384R1 : return CIPHERKEYTYPE_ECDSA_SECP384R1_PRIVATE;
      case CIPHERTYPE_ECDSA_SECP521R1 : return CIPHERKEYTYPE_ECDSA_SECP521R1_PRIVATE;
                                default : break;
    }

  return CIPHERKEYTYPE_UNKNOWN;
}


/**-------------------------------------------------------------------------------------------------------------------
* Encode one XMPINTEGER as a minimal, non-negative DER INTEGER (tag 0x02): strip leading zero bytes down to a
* single byte, then prepend one 0x00 pad byte if the remaining high bit is set (otherwise the value would decode
* as negative). maxsize bounds the fixed-size export used to obtain the big-endian bytes (coordinatesize is
* always enough: both R and S are reduced mod order, and order fits in coordinatesize bytes for every curve here).
* --------------------------------------------------------------------------------------------------------------------*/
static bool CIPHERECDSA_DERIntegerEncode(XMPINTEGER& value, XDWORD maxsize, XBUFFER& output)
{
  XBYTE  buffer[CIPHERECDSA_MAXCOORDINATE_SIZE];
  XDWORD index = 0;
  XDWORD length;
  bool   padzero;

  if(!maxsize || (maxsize > sizeof(buffer)) || !value.ExportToBinary(buffer, maxsize)) return false;

  while((index < (maxsize - 1)) && !buffer[index]) index++;

  length  = maxsize - index;
  padzero = (buffer[index] & 0x80)?true:false;

  XDWORD fulllength = length + (padzero?1:0);

  if(!output.Add((XBYTE)0x02)) return false;

  if(fulllength < 0x80)
    {
      if(!output.Add((XBYTE)fulllength)) return false;
    }
   else
    {
      // Not reached by any curve supported today (worst case is P-521: 66 bytes + 1 pad byte = 67 < 0x80), kept
      // for correctness if a larger curve is ever added.
      if(!output.Add((XBYTE)0x81) || !output.Add((XBYTE)fulllength)) return false;
    }

  if(padzero && !output.Add((XBYTE)0x00)) return false;

  return output.Add(&buffer[index], length);
}



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         CIPHERECDSA::CIPHERECDSA(CIPHERTYPE curvetype) : CIPHER()
* @brief      Constructor of class
* @ingroup    CIPHER
*
* @param[in]  curvetype : Curve to use (CIPHERTYPE_ECDSA_SECP256R1, _SECP384R1 or _SECP521R1). Defaults to P-256
*                         for source compatibility with existing default-constructed callers. Any other value
*                         leaves the cipher unusable: Parameters_Set() (called from SetKey()/KeyPair_Create()/
*                         SharedSecret_Create()/PublicKey_Check()) fails closed instead of guessing a curve.
*
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERECDSA::CIPHERECDSA(CIPHERTYPE curvetype) : CIPHER()
{
  Clean();

  type = curvetype;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         CIPHERECDSA::~CIPHERECDSA()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    CIPHER
*
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERECDSA::~CIPHERECDSA()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERECDSA::SetKey(CIPHERKEY* key, bool integritycheck)
* @brief      Set and validate an uncompressed ECDSA public key for the curve given to the constructor
* @ingroup    CIPHER
*
* @param[in]  key : Public key to use.
* @param[in]  integritycheck : Retained for the common cipher key interface; public points are always validated.
*
* @return     bool : true if the key is valid; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERECDSA::SetKey(CIPHERKEY* key, bool integritycheck)
{
  CIPHERKEYECDSA* ECDSAkey;
  XBUFFER*        keydata;

  if(!key || !Parameters_Set()) return false;

  // Private key branch: does NOT reset havepublickey/publickeyX/publickeyY -- SetKey() is meant to be called once
  // per key part (mirrors CIPHERRSA::SetKey(), where SetKey(RSA_PUBLIC) then SetKey(RSA_PRIVATE) accumulate into
  // the same context instead of each call starting over), so DIOSTREAMTLSSIGNATURE::Sign() can do
  // ECDSA.SetKey(publickey, true) && ECDSA.SetKey(privatekey, true) the same way it already does for CIPHERRSA.
  if(key->GetType() == CIPHERECDSA_ExpectedPrivateKeyType(GetType()))
    {
      haveprivatekey = false;

      ECDSAkey = (CIPHERKEYECDSA*)key;
      keydata  = ECDSAkey->Get();

      if(!keydata || (keydata->GetSize() != coordinatesize) ||
         !privatekeyD.ImportFromBinary(keydata->Get(), coordinatesize) ||
         (privatekeyD.CompareSignedValues(0) <= 0) || (privatekeyD.CompareSignedValues(order) >= 0))
        {
          return false;
        }

      if(integritycheck && havepublickey)
        {
          // Cross-check that the private scalar actually matches the public point already set (privatekey*G ==
          // publickey), the same purpose as CIPHERRSA::SetKey()'s P*Q == N check for a mismatched RSA key pair.
          CIPHERECDSA_POINT point;
          XMPINTEGER        affineX;
          XMPINTEGER        affineY;

          if(!CIPHERECDSA_PointMultiply(point, privatekeyD, generatorX, generatorY, prime, curvebits) ||
             !CIPHERECDSA_PointToAffine(point, affineX, affineY, prime) ||
             affineX.CompareSignedValues(publickeyX) || affineY.CompareSignedValues(publickeyY))
            {
              return false;
            }
        }

      haveprivatekey = true;

      return true;
    }

  // Public key branch (unchanged behaviour).
  havepublickey = false;

  if(key->GetType() != CIPHERECDSA_ExpectedKeyType(GetType())) return false;

  ECDSAkey = (CIPHERKEYECDSA*)key;
  keydata  = ECDSAkey->Get();

  if(!keydata || (keydata->GetSize() != publickeysize) || (keydata->GetByte(0) != 0x04) ||
     !publickeyX.ImportFromBinary(&keydata->Get()[1], coordinatesize) ||
     !publickeyY.ImportFromBinary(&keydata->Get()[1 + coordinatesize], coordinatesize))
    {
      return false;
    }

  if((publickeyX.CompareSignedValues(prime) >= 0) || (publickeyY.CompareSignedValues(prime) >= 0) ||
     !PublicKey_Check())
    {
      return false;
    }

  havepublickey = true;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERECDSA::Verify(XBYTE* input, XDWORD size, XBUFFER& signature, HASH* hash)
* @brief      Verify an ECDSA signature encoded as a DER ECDSA-Sig-Value, for the curve given to the constructor
* @ingroup    CIPHER
*
* @param[in]  input : Signed data.
* @param[in]  size : Signed data size.
* @param[in]  signature : DER signature value.
* @param[in]  hash : Hash algorithm used by the signature; must match the curve's RFC 8446 4.2.3 pairing
*                    (SHA-256 for P-256, SHA-384 for P-384, SHA-512 for P-521).
*
* @return     bool : true if the signature is valid; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERECDSA::Verify(XBYTE* input, XDWORD size, XBUFFER& signature, HASH* hash)
{
  XMPINTEGER       R;
  XMPINTEGER       S;
  XMPINTEGER       digest;
  XMPINTEGER       inverseS;
  XMPINTEGER       U1;
  XMPINTEGER       U2;
  XMPINTEGER       multiplication;
  XMPINTEGER       inverseZ;
  XMPINTEGER       inverseZ2;
  XMPINTEGER       affineX;
  XMPINTEGER       verification;
  CIPHERECDSA_POINT point;

  if(!havepublickey || !input || !size || signature.IsEmpty() || !hash ||
     (hash->GetType() != CIPHERECDSA_RequiredHashType(GetType())) || !Signature_Decode(signature, R, S) ||
     !hash->ResetResult() || !hash->Do(input, size) || !hash->GetResult() ||
     !hash->GetResult()->GetSize() || (hash->GetResult()->GetSize() > coordinatesize) ||
     !digest.ImportFromBinary(hash->GetResult()->Get(), hash->GetResult()->GetSize()) ||
     !inverseS.ModularInverse(&S, &order) ||
     !multiplication.Multiplication(&digest, &inverseS) || !U1.Module(&U1, &multiplication, &order) ||
     !multiplication.Multiplication(&R, &inverseS) || !U2.Module(&U2, &multiplication, &order))
    {
      return false;
    }

  for(int bit=(int)curvebits-1; bit>=0; bit--)
    {
      if(!CIPHERECDSA_PointDouble(point, prime)) return false;

      if(U1.GetBit(bit) && !CIPHERECDSA_PointAddAffine(point, generatorX, generatorY, prime)) return false;
      if(U2.GetBit(bit) && !CIPHERECDSA_PointAddAffine(point, publickeyX, publickeyY, prime)) return false;
    }

  if(point.isinfinity || !point.Z.CompareSignedValues(0) ||
     !inverseZ.ModularInverse(&point.Z, &prime) ||
     !CIPHERECDSA_ModularMultiplication(inverseZ2, inverseZ, inverseZ, prime) ||
     !CIPHERECDSA_ModularMultiplication(affineX, point.X, inverseZ2, prime) ||
     !verification.Module(&verification, &affineX, &order))
    {
      return false;
    }

  return !verification.CompareSignedValues(R);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERECDSA::Verify(XBUFFER& input, XBUFFER& signature, HASH* hash)
* @brief      Verify an ECDSA P-256 signature encoded as a DER ECDSA-Sig-Value
* @ingroup    CIPHER
*
* @param[in]  input : Signed data.
* @param[in]  signature : DER signature value.
* @param[in]  hash : Hash algorithm used by the signature.
*
* @return     bool : true if the signature is valid; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERECDSA::Verify(XBUFFER& input, XBUFFER& signature, HASH* hash)
{
  return Verify(input.Get(), input.GetSize(), signature, hash);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERECDSA::Sign(XBYTE* input, XDWORD size, HASH* hash)
* @brief      Sign with the private key set via SetKey(), for the curve given to the constructor
* @note       Standard ECDSA: e = leftmost curvebits of HASH(input); pick a random per-signature nonce k in
*             [1, order-1]; R = (k*G).x mod order; S = k^-1 * (e + R*privatekeyD) mod order; retry with a fresh k
*             if either R or S comes out 0 (the point at infinity / an unusable signature -- astronomically rare,
*             the retry is just standard defensive practice). Result (a DER ECDSA-Sig-Value) is left in
*             GetResult(), same convention as CIPHERRSA::Sign().
* @ingroup    CIPHER
*
* @param[in]  input : Data to sign.
* @param[in]  size : Data size.
* @param[in]  hash : Hash algorithm to use; must match the curve's RFC 8446 4.2.3 pairing (SHA-256 for P-256,
*                    SHA-384 for P-384, SHA-512 for P-521).
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERECDSA::Sign(XBYTE* input, XDWORD size, HASH* hash)
{
  XRAND*            xrand;
  XBYTE             kbuffer[CIPHERECDSA_MAXCOORDINATE_SIZE];
  XMPINTEGER        digest;
  XMPINTEGER        k;
  XMPINTEGER        inverseK;
  XMPINTEGER        R;
  XMPINTEGER        S;
  XMPINTEGER        rd;
  XMPINTEGER        sum;
  CIPHERECDSA_POINT point;
  XMPINTEGER        affineX;
  XMPINTEGER        affineY;
  bool              status = false;

  if(!result) return false;

  result->Delete();

  if(!haveprivatekey || !input || !size || !hash ||
     (hash->GetType() != CIPHERECDSA_RequiredHashType(GetType())) ||
     !hash->ResetResult() || !hash->Do(input, size) || !hash->GetResult() ||
     !hash->GetResult()->GetSize() || (hash->GetResult()->GetSize() > coordinatesize) ||
     !digest.ImportFromBinary(hash->GetResult()->Get(), hash->GetResult()->GetSize()))
    {
      return false;
    }

  xrand = GEN_XFACTORY.CreateRand();
  if(!xrand) return false;

  if(xrand->Ini())
    {
      // See KeyPair_Create() for why the top bits need masking before the rejection-sampling check (P-521:
      // coordinatesize is 66 bytes = 528 bits for a 521-bit curve).
      XBYTE topbytemask = (XBYTE)(0xFF >> (((XDWORD)coordinatesize * 8) - curvebits));

      for(int attempt=0; attempt<128; attempt++)
        {
          memset(kbuffer, 0, sizeof(kbuffer));

          if(!xrand->Generate(kbuffer, coordinatesize)) break;

          kbuffer[0] &= topbytemask;

          if(!k.ImportFromBinary(kbuffer, coordinatesize)) break;

          if((k.CompareSignedValues(0) <= 0) || (k.CompareSignedValues(order) >= 0)) continue;

          if(!CIPHERECDSA_PointMultiply(point, k, generatorX, generatorY, prime, curvebits) ||
             !CIPHERECDSA_PointToAffine(point, affineX, affineY, prime) ||
             !R.Module(&R, &affineX, &order))
            {
              continue;
            }

          if(!R.CompareSignedValues(0)) continue;      // R == 0: unusable, retry with a fresh k

          if(!inverseK.ModularInverse(&k, &order)) continue;

          if(!CIPHERECDSA_ModularMultiplication(rd, R, privatekeyD, order) ||
             !CIPHERECDSA_ModularAddition(sum, digest, rd, order) ||
             !CIPHERECDSA_ModularMultiplication(S, inverseK, sum, order))
            {
              continue;
            }

          if(!S.CompareSignedValues(0)) continue;      // S == 0: unusable, retry with a fresh k

          status = true;
          break;
        }
    }

  GEN_XFACTORY.DeleteRand(xrand);

  memset(kbuffer, 0, sizeof(kbuffer));

  if(!status) return false;

  return Signature_Encode(R, S, (*result));
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERECDSA::Sign(XBUFFER& input, HASH* hash)
* @brief      Sign with the private key set via SetKey(), for the curve given to the constructor
* @ingroup    CIPHER
*
* @param[in]  input : Data to sign.
* @param[in]  hash : Hash algorithm to use.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERECDSA::Sign(XBUFFER& input, HASH* hash)
{
  return Sign(input.Get(), input.GetSize(), hash);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERECDSA::KeyPair_Create(XBUFFER& privatekey, XBUFFER& publickey)
* @brief      Create an ephemeral ECDHE key pair for the curve given to the constructor
* @ingroup    CIPHER
*
* @param[out] privatekey : Private scalar encoded in coordinatesize bytes.
* @param[out] publickey : Uncompressed public point encoded in publickeysize bytes.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERECDSA::KeyPair_Create(XBUFFER& privatekey, XBUFFER& publickey)
{
  XRAND*           xrand;
  XBYTE            scalarbuffer[CIPHERECDSA_MAXCOORDINATE_SIZE];
  XMPINTEGER       scalar;
  CIPHERECDSA_POINT point;
  XMPINTEGER       affineX;
  XMPINTEGER       affineY;
  XBYTE            coordinate[CIPHERECDSA_MAXCOORDINATE_SIZE];
  bool             status = false;

  if((&privatekey == &publickey) || !Parameters_Set()) return false;

  privatekey.Delete();
  publickey.Delete();
  memset(scalarbuffer, 0, sizeof(scalarbuffer));
  memset(coordinate, 0, sizeof(coordinate));

  xrand = GEN_XFACTORY.CreateRand();
  if(!xrand) return false;

  if(xrand->Ini())
    {
      // coordinatesize is a byte count, but curvebits is not always an exact multiple of 8 (P-521: 66 bytes =
      // 528 bits for a 521-bit curve). Left over top bits must be masked off before the rejection-sampling
      // check below, or a byte-aligned random draw for P-521 almost never lands under the (521-bit) order and
      // every one of the 128 attempts can plausibly be rejected.
      XBYTE topbytemask = (XBYTE)(0xFF >> (((XDWORD)coordinatesize * 8) - curvebits));

      for(int attempt=0; attempt<128; attempt++)
        {
          if(!xrand->Generate(scalarbuffer, coordinatesize)) break;

          scalarbuffer[0] &= topbytemask;

          if(!scalar.ImportFromBinary(scalarbuffer, coordinatesize)) break;

          if((scalar.CompareSignedValues(0) > 0) && (scalar.CompareSignedValues(order) < 0))
            {
              status = true;
              break;
            }
        }
    }

  GEN_XFACTORY.DeleteRand(xrand);

  if(status)
    {
      status = CIPHERECDSA_PointMultiply(point, scalar, generatorX, generatorY, prime, curvebits) &&
               CIPHERECDSA_PointToAffine(point, affineX, affineY, prime) &&
               privatekey.Add(scalarbuffer, coordinatesize) &&
               publickey.Add((XBYTE)0x04) &&
               affineX.ExportToBinary(coordinate, coordinatesize) &&
               publickey.Add(coordinate, coordinatesize) &&
               affineY.ExportToBinary(coordinate, coordinatesize) &&
               publickey.Add(coordinate, coordinatesize);
    }

  memset(scalarbuffer, 0, sizeof(scalarbuffer));
  memset(coordinate, 0, sizeof(coordinate));

  if(!status)
    {
      privatekey.FillBuffer(0);
      privatekey.Delete();
      publickey.Delete();
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERECDSA::SharedSecret_Create(XBUFFER& privatekey, XBUFFER& publickey, XBUFFER& sharedsecret)
* @brief      Calculate the ECDHE shared secret from a private scalar and a validated peer point, for the curve
*             given to the constructor
* @ingroup    CIPHER
*
* @param[in]  privatekey : Private scalar encoded in coordinatesize bytes.
* @param[in]  publickey : Peer uncompressed public point encoded in publickeysize bytes.
* @param[out] sharedsecret : X coordinate of the shared point encoded in coordinatesize bytes.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERECDSA::SharedSecret_Create(XBUFFER& privatekey, XBUFFER& publickey, XBUFFER& sharedsecret)
{
  XMPINTEGER       scalar;
  XMPINTEGER       peerX;
  XMPINTEGER       peerY;
  CIPHERECDSA_POINT point;
  XMPINTEGER       affineX;
  XMPINTEGER       affineY;
  XBYTE            secret[CIPHERECDSA_MAXCOORDINATE_SIZE];
  bool             status;

  sharedsecret.Delete();
  memset(secret, 0, sizeof(secret));

  if((&privatekey == &sharedsecret) || (&publickey == &sharedsecret) ||
     !Parameters_Set() || (privatekey.GetSize() != coordinatesize) ||
     (publickey.GetSize() != publickeysize) || (publickey.GetByte(0) != 0x04) ||
     !scalar.ImportFromBinary(privatekey.Get(), privatekey.GetSize()) ||
     (scalar.CompareSignedValues(0) <= 0) || (scalar.CompareSignedValues(order) >= 0) ||
     !peerX.ImportFromBinary(&publickey.Get()[1], coordinatesize) ||
     !peerY.ImportFromBinary(&publickey.Get()[1 + coordinatesize], coordinatesize) ||
     !CIPHERECDSA_PointCheck(peerX, peerY, prime, coefficientA, coefficientB))
    {
      return false;
    }

  status = CIPHERECDSA_PointMultiply(point, scalar, peerX, peerY, prime, curvebits) &&
           CIPHERECDSA_PointToAffine(point, affineX, affineY, prime) &&
           affineX.ExportToBinary(secret, coordinatesize) &&
           sharedsecret.Add(secret, coordinatesize);

  memset(secret, 0, sizeof(secret));

  if(!status) sharedsecret.Delete();

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERECDSA::PublicKey_Check(XBUFFER& publickey)
* @brief      Validate an uncompressed public point on the curve given to the constructor
* @ingroup    CIPHER
*
* @param[in]  publickey : Uncompressed point encoded in publickeysize bytes.
*
* @return     bool : true if the point is valid; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERECDSA::PublicKey_Check(XBUFFER& publickey)
{
  XMPINTEGER X;
  XMPINTEGER Y;

  if(!Parameters_Set() || (publickey.GetSize() != publickeysize) ||
     (publickey.GetByte(0) != 0x04) ||
     !X.ImportFromBinary(&publickey.Get()[1], coordinatesize) ||
     !Y.ImportFromBinary(&publickey.Get()[1 + coordinatesize], coordinatesize))
    {
      return false;
    }

  return CIPHERECDSA_PointCheck(X, Y, prime, coefficientA, coefficientB);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERECDSA::Parameters_Set()
* @brief      Set the NIST domain parameters for the curve given to the constructor
* @note       INTERNAL
* @ingroup    CIPHER
*
* @return     bool : true if the parameters are available; otherwise false (curve not one of the three supported).
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERECDSA::Parameters_Set()
{
  switch(GetType())
    {
      case CIPHERTYPE_ECDSA_SECP256R1 :

        coordinatesize = CIPHERECDSA_P256_COORDINATE_SIZE;
        publickeysize  = CIPHERECDSA_P256_PUBLICKEY_SIZE;
        curvebits      = CIPHERECDSA_P256_CURVEBITS;

        return prime.SetFromString(16, __L("FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFF")) &&
               coefficientA.SetFromString(16, __L("FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFC")) &&
               coefficientB.SetFromString(16, __L("5AC635D8AA3A93E7B3EBBD55769886BC651D06B0CC53B0F63BCE3C3E27D2604B")) &&
               generatorX.SetFromString(16, __L("6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296")) &&
               generatorY.SetFromString(16, __L("4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5")) &&
               order.SetFromString(16, __L("FFFFFFFF00000000FFFFFFFFFFFFFFFFBCE6FAADA7179E84F3B9CAC2FC632551"));

      // NIST P-384 (secp384r1). Parameters per FIPS 186-4 / SEC 2, cross-checked against
      // `openssl ecparam -name secp384r1 -param_enc explicit -text`.
      case CIPHERTYPE_ECDSA_SECP384R1 :

        coordinatesize = CIPHERECDSA_P384_COORDINATE_SIZE;
        publickeysize  = CIPHERECDSA_P384_PUBLICKEY_SIZE;
        curvebits      = CIPHERECDSA_P384_CURVEBITS;

        return prime.SetFromString(16, __L("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFF")) &&
               coefficientA.SetFromString(16, __L("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFC")) &&
               coefficientB.SetFromString(16, __L("B3312FA7E23EE7E4988E056BE3F82D19181D9C6EFE8141120314088F5013875AC656398D8A2ED19D2A85C8EDD3EC2AEF")) &&
               generatorX.SetFromString(16, __L("AA87CA22BE8B05378EB1C71EF320AD746E1D3B628BA79B9859F741E082542A385502F25DBF55296C3A545E3872760AB7")) &&
               generatorY.SetFromString(16, __L("3617DE4A96262C6F5D9E98BF9292DC29F8F41DBD289A147CE9DA3113B5F0B8C00A60B1CE1D7E819D7A431D7C90EA0E5F")) &&
               order.SetFromString(16, __L("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC7634D81F4372DDF581A0DB248B0A77AECEC196ACCC52973"));

      // NIST P-521 (secp521r1). Parameters per FIPS 186-4 / SEC 2, cross-checked against
      // `openssl ecparam -name secp521r1 -param_enc explicit -text`. The 521-bit field is encoded in 66 bytes,
      // with the top byte only carrying bit 0 (0x01) of the value.
      case CIPHERTYPE_ECDSA_SECP521R1 :

        coordinatesize = CIPHERECDSA_P521_COORDINATE_SIZE;
        publickeysize  = CIPHERECDSA_P521_PUBLICKEY_SIZE;
        curvebits      = CIPHERECDSA_P521_CURVEBITS;

        return prime.SetFromString(16, __L("01FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")) &&
               coefficientA.SetFromString(16, __L("01FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC")) &&
               coefficientB.SetFromString(16, __L("0051953EB9618E1C9A1F929A21A0B68540EEA2DA725B99B315F3B8B489918EF109E156193951EC7E937B1652C0BD3BB1BF073573DF883D2C34F1EF451FD46B503F00")) &&
               generatorX.SetFromString(16, __L("00C6858E06B70404E9CD9E3ECB662395B4429C648139053FB521F828AF606B4D3DBAA14B5E77EFE75928FE1DC127A2FFA8DE3348B3C1856A429BF97E7E31C2E5BD66")) &&
               generatorY.SetFromString(16, __L("011839296A789A3BC0045C8A5FB42C7D1BD998F54449579B446817AFBD17273E662C97EE72995EF42640C550B9013FAD0761353C7086A272C24088BE94769FD16650")) &&
               order.SetFromString(16, __L("01FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFA51868783BF2F966B7FCC0148F709A5D03BB5C9B8899C47AEBB6FB71E91386409"));

                                default : break;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERECDSA::PublicKey_Check()
* @brief      Check that the public point belongs to the NIST P-256 curve
* @note       INTERNAL
* @ingroup    CIPHER
*
* @return     bool : true if the point is valid; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERECDSA::PublicKey_Check()
{
  return CIPHERECDSA_PointCheck(publickeyX, publickeyY, prime, coefficientA, coefficientB);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERECDSA::Signature_Decode(XBUFFER& signature, XMPINTEGER& R, XMPINTEGER& S)
* @brief      Decode a strict DER ECDSA-Sig-Value
* @note       INTERNAL
* @ingroup    CIPHER
*
* @param[in]  signature : DER signature.
* @param[out] R : First ECDSA integer.
* @param[out] S : Second ECDSA integer.
*
* @return     bool : true if both integers are canonical and in range; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERECDSA::Signature_Decode(XBUFFER& signature, XMPINTEGER& R, XMPINTEGER& S)
{
  XBYTE* data  = signature.Get();
  XDWORD size  = signature.GetSize();
  XDWORD index = 0;
  XDWORD sequencelength;

  if(!data || !size || (data[index++] != 0x30) ||
     !CIPHERECDSA_DERLength(data, size, index, sequencelength) || (sequencelength != (size - index)) ||
     !CIPHERECDSA_DERInteger(data, size, index, R, order, coordinatesize) ||
     !CIPHERECDSA_DERInteger(data, size, index, S, order, coordinatesize))
    {
      return false;
    }

  return (index == size);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERECDSA::Signature_Encode(XMPINTEGER& R, XMPINTEGER& S, XBUFFER& signature)
* @brief      Encode R and S as a DER ECDSA-Sig-Value (SEQUENCE of two INTEGERs)
* @note       INTERNAL
* @ingroup    CIPHER
*
* @param[in]  R : First ECDSA integer.
* @param[in]  S : Second ECDSA integer.
* @param[out] signature : DER signature.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERECDSA::Signature_Encode(XMPINTEGER& R, XMPINTEGER& S, XBUFFER& signature)
{
  XBUFFER content;
  XDWORD  length;

  signature.Delete();

  if(!CIPHERECDSA_DERIntegerEncode(R, coordinatesize, content) ||
     !CIPHERECDSA_DERIntegerEncode(S, coordinatesize, content))
    {
      return false;
    }

  length = content.GetSize();

  if(!signature.Add((XBYTE)0x30)) return false;

  if(length < 0x80)
    {
      if(!signature.Add((XBYTE)length)) return false;
    }
   else
    {
      // Reached only for the larger curves: two P-521 INTEGERs (up to 67 bytes each incl. tag+len+pad) can add
      // up to ~138 bytes of content, past the 0x80 short-form limit. A single length-of-length byte is always
      // enough (well under 256).
      if(!signature.Add((XBYTE)0x81) || !signature.Add((XBYTE)length)) return false;
    }

  return signature.Add(content);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void CIPHERECDSA::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    CIPHER
*
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERECDSA::Clean()
{
  havepublickey  = false;
  haveprivatekey = false;
  coordinatesize = 0;
  publickeysize  = 0;
  curvebits      = 0;
}






