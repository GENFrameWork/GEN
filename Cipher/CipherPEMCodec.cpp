/**-------------------------------------------------------------------------------------------------------------------
*
* @file       CipherPEMCodec.cpp
*
* @class      CIPHERPEMCODEC
* @brief      Cipher PEM/DER private key codec class
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

#include "CipherPEMCodec.h"

#include <string.h>



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

// rsaEncryption (1.2.840.113549.1.1.1) and id-ecPublicKey (1.2.840.10045.2.1), DER-encoded OBJECT IDENTIFIER
// content (i.e. without the leading 0x06 tag / length bytes) -- the only two PKCS#8 algorithms recognised.
static const XBYTE CIPHERPEMCODEC_OID_RSAENCRYPTION[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01 };
static const XBYTE CIPHERPEMCODEC_OID_ECPUBLICKEY[]   = { 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01 };



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERPEMCODEC::DER_ReadLength(XBYTE* data, XDWORD size, XDWORD& index, XDWORD& length)
* @brief      Read a DER length (short or long form) at data[index], advancing index past it.
* @note       INTERNAL. index/size are absolute offsets into data (size is an exclusive upper bound, not
*             necessarily the whole buffer's size -- this lets callers bound a read to a single nested SEQUENCE).
* @ingroup    CIPHER
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERPEMCODEC::DER_ReadLength(XBYTE* data, XDWORD size, XDWORD& index, XDWORD& length)
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
  if(!nbytes || (nbytes > sizeof(XDWORD)) || (nbytes > (size - index))) return false;

  for(XDWORD c=0; c<nbytes; c++)
    {
      if(length > (0xFFFFFFFFUL >> 8)) return false;
      length = (length << 8) | data[index++];
    }

  return (length <= (size - index));
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERPEMCODEC::DER_ReadTagLength(XBYTE* data, XDWORD size, XDWORD& index, XBYTE& tag, XDWORD& valueoffset, XDWORD& valuelength)
* @brief      Read a DER tag + length at data[index], advancing index past the whole TLV (tag+length+value).
* @note       INTERNAL
* @ingroup    CIPHER
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERPEMCODEC::DER_ReadTagLength(XBYTE* data, XDWORD size, XDWORD& index, XBYTE& tag, XDWORD& valueoffset, XDWORD& valuelength)
{
  if(!data || (index >= size)) return false;

  tag = data[index++];

  if(!DER_ReadLength(data, size, index, valuelength)) return false;

  valueoffset = index;
  index += valuelength;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERPEMCODEC::DER_ReadInteger(XBYTE* data, XDWORD size, XDWORD& index, XMPINTEGER& value)
* @brief      Read a DER INTEGER at data[index] into value, advancing index past it.
* @note       INTERNAL. A leading 0x00 pad byte (present when the DER encoder needed to keep the value
*             non-negative) does not change the imported magnitude, so it is not stripped here.
* @ingroup    CIPHER
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERPEMCODEC::DER_ReadInteger(XBYTE* data, XDWORD size, XDWORD& index, XMPINTEGER& value)
{
  XBYTE  tag;
  XDWORD valueoffset;
  XDWORD valuelength;

  if(!DER_ReadTagLength(data, size, index, tag, valueoffset, valuelength) || (tag != 0x02) || !valuelength) return false;

  return value.ImportFromBinary(&data[valueoffset], valuelength);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERPEMCODEC::IsPEM(XBUFFER& filedata)
* @brief      Is PEM (starts with "-----BEGIN ", ignoring leading whitespace)
* @ingroup    CIPHER
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERPEMCODEC::IsPEM(XBUFFER& filedata)
{
  static const char marker[]  = "-----BEGIN ";
  XDWORD             markerlen = (XDWORD)(sizeof(marker) - 1);

  XBYTE* data = filedata.Get();
  XDWORD size = filedata.GetSize();
  XDWORD index = 0;

  if(!data || !size) return false;

  while((index < size) && ((data[index]==' ') || (data[index]=='\t') || (data[index]=='\r') || (data[index]=='\n'))) index++;

  if((size - index) < markerlen) return false;

  for(XDWORD c=0; c<markerlen; c++)
    {
      if(data[index + c] != (XBYTE)marker[c]) return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERPEMCODEC::PrivateKeyBlock_Decode(XVECTOR<XSTRING*>& lines, XSTRING& blockheader, XBUFFER& der)
* @brief      Scan lines for the first complete "-----BEGIN xxx PRIVATE KEY----- ... -----END xxx PRIVATE KEY-----"
*             block and base64-decode its body into DER.
* @ingroup    CIPHER
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERPEMCODEC::PrivateKeyBlock_Decode(XVECTOR<XSTRING*>& lines, XSTRING& blockheader, XBUFFER& der)
{
  XSTRING keybase64;
  bool    inblock = false;

  blockheader.Empty();
  der.Delete();

  for(XDWORD c=0; c<lines.GetSize(); c++)
    {
      XSTRING* line = lines.Get(c);
      if(!line) continue;

      if(!inblock)
        {
          if((line->Find(__L("-----BEGIN "), true) != XSTRING_NOTFOUND) && (line->Find(__L("PRIVATE KEY-----"), true) != XSTRING_NOTFOUND))
            {
              blockheader = (*line);
              keybase64.Empty();
              inblock = true;
            }

          continue;
        }

      if(line->Find(__L("-----END "), true) != XSTRING_NOTFOUND)
        {
          if(keybase64.IsEmpty())
            {
              blockheader.Empty();
              return false;
            }

          if(!der.ConvertFromBase64(keybase64) || der.IsEmpty())
            {
              blockheader.Empty();
              der.Delete();
              return false;
            }

          return true;
        }

      if(!line->IsEmpty()) keybase64.Add(line->Get());
    }

  // BEGIN found but no matching END (or no BEGIN at all): incomplete/absent block.
  blockheader.Empty();
  der.Delete();

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERPEMCODEC::RSAPrivateKey_Decode(XBUFFER& der, XMPINTEGER& prime1, XMPINTEGER& prime2, XMPINTEGER& exponent)
* @brief      Decode a PKCS#1 RSAPrivateKey DER blob into prime1/prime2/privateExponent.
* @ingroup    CIPHER
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERPEMCODEC::RSAPrivateKey_Decode(XBUFFER& der, XMPINTEGER& prime1, XMPINTEGER& prime2, XMPINTEGER& exponent)
{
  XBYTE* data = der.Get();
  XDWORD size = der.GetSize();
  XDWORD index = 0;

  XBYTE  tag;
  XDWORD seqoffset;
  XDWORD seqlength;

  if(!data || !size) return false;
  if(!DER_ReadTagLength(data, size, index, tag, seqoffset, seqlength) || (tag != 0x30)) return false;

  XDWORD childindex = seqoffset;
  XDWORD seqend      = seqoffset + seqlength;

  XMPINTEGER version;
  XMPINTEGER modulus;
  XMPINTEGER publicexponent;

  // RSAPrivateKey ::= SEQUENCE { version, modulus, publicExponent, privateExponent, prime1, prime2,
  //                              exponent1, exponent2, coefficient, otherPrimeInfos OPTIONAL }
  // Only privateExponent/prime1/prime2 are needed by CIPHERKEYPRIVATERSA::Set() -- the rest are read (version,
  // modulus, publicExponent) or simply left unread (exponent1/exponent2/coefficient/otherPrimeInfos) as needed.
  if(!DER_ReadInteger(data, seqend, childindex, version))        return false;
  if(!DER_ReadInteger(data, seqend, childindex, modulus))        return false;
  if(!DER_ReadInteger(data, seqend, childindex, publicexponent)) return false;
  if(!DER_ReadInteger(data, seqend, childindex, exponent))       return false;
  if(!DER_ReadInteger(data, seqend, childindex, prime1))         return false;
  if(!DER_ReadInteger(data, seqend, childindex, prime2))         return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERPEMCODEC::ECPrivateKey_Decode(XBUFFER& der, XBUFFER& privatekey)
* @brief      Decode a SEC1 ECPrivateKey DER blob into its privateKey OCTET STRING (the scalar D).
* @ingroup    CIPHER
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERPEMCODEC::ECPrivateKey_Decode(XBUFFER& der, XBUFFER& privatekey)
{
  XBYTE* data = der.Get();
  XDWORD size = der.GetSize();
  XDWORD index = 0;

  XBYTE  tag;
  XDWORD seqoffset;
  XDWORD seqlength;

  privatekey.Delete();

  if(!data || !size) return false;
  if(!DER_ReadTagLength(data, size, index, tag, seqoffset, seqlength) || (tag != 0x30)) return false;

  XDWORD childindex = seqoffset;
  XDWORD seqend      = seqoffset + seqlength;

  XDWORD childoffset;
  XDWORD childlength;

  // ECPrivateKey ::= SEQUENCE { version INTEGER, privateKey OCTET STRING, parameters [0] OPTIONAL, publicKey [1] OPTIONAL }
  if(!DER_ReadTagLength(data, seqend, childindex, tag, childoffset, childlength) || (tag != 0x02)) return false; // version
  if(!DER_ReadTagLength(data, seqend, childindex, tag, childoffset, childlength) || (tag != 0x04) || !childlength) return false; // privateKey

  return privatekey.Add(&data[childoffset], childlength);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERPEMCODEC::PKCS8PrivateKey_Decode(XBUFFER& der, bool& isrsa, bool& isec, XBUFFER& innerkey)
* @brief      Decode an unencrypted PKCS#8 PrivateKeyInfo DER blob: identify its algorithm and return the nested
*             RSAPrivateKey / ECPrivateKey DER blob.
* @ingroup    CIPHER
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERPEMCODEC::PKCS8PrivateKey_Decode(XBUFFER& der, bool& isrsa, bool& isec, XBUFFER& innerkey)
{
  XBYTE* data = der.Get();
  XDWORD size = der.GetSize();
  XDWORD index = 0;

  XBYTE  tag;
  XDWORD seqoffset;
  XDWORD seqlength;

  isrsa = false;
  isec  = false;
  innerkey.Delete();

  if(!data || !size) return false;
  if(!DER_ReadTagLength(data, size, index, tag, seqoffset, seqlength) || (tag != 0x30)) return false;

  XDWORD childindex = seqoffset;
  XDWORD seqend      = seqoffset + seqlength;

  XDWORD childoffset;
  XDWORD childlength;

  // PrivateKeyInfo ::= SEQUENCE { version INTEGER, privateKeyAlgorithm AlgorithmIdentifier, privateKey OCTET STRING, ... }
  if(!DER_ReadTagLength(data, seqend, childindex, tag, childoffset, childlength) || (tag != 0x02)) return false; // version

  // privateKeyAlgorithm ::= SEQUENCE { algorithm OBJECT IDENTIFIER, parameters ANY OPTIONAL }
  XDWORD algseqoffset;
  XDWORD algseqlength;

  if(!DER_ReadTagLength(data, seqend, childindex, tag, algseqoffset, algseqlength) || (tag != 0x30)) return false;

  XDWORD algindex = algseqoffset;
  XDWORD algseqend = algseqoffset + algseqlength;

  XDWORD oidoffset;
  XDWORD oidlength;

  if(!DER_ReadTagLength(data, algseqend, algindex, tag, oidoffset, oidlength) || (tag != 0x06) || !oidlength) return false;

  if((oidlength == sizeof(CIPHERPEMCODEC_OID_RSAENCRYPTION)) && !memcmp(&data[oidoffset], CIPHERPEMCODEC_OID_RSAENCRYPTION, oidlength))
    {
      isrsa = true;
    }
   else if((oidlength == sizeof(CIPHERPEMCODEC_OID_ECPUBLICKEY)) && !memcmp(&data[oidoffset], CIPHERPEMCODEC_OID_ECPUBLICKEY, oidlength))
    {
      isec = true;
    }
   else
    {
      return false; // Unrecognised/unsupported algorithm OID.
    }

  // privateKey OCTET STRING -- contains the nested RSAPrivateKey / ECPrivateKey DER blob.
  if(!DER_ReadTagLength(data, seqend, childindex, tag, childoffset, childlength) || (tag != 0x04) || !childlength) return false;

  return innerkey.Add(&data[childoffset], childlength);
}
