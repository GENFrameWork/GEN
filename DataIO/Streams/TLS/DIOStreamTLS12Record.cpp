/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamTLS12Record.cpp
*
* @class      DIOSTREAMTLS12RECORD
* @brief      Data Input/Output Stream TLS 1.2 record protection class (RFC 5246 + RFC 5288 + RFC 7905)
* @ingroup    DATAIO
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

#include "DIOStreamTLS12Record.h"
#include "DIOStreamTLS12KeySchedule.h"

#include <string.h>

#include "Cipher.h"
#include "CipherKeySymmetrical.h"
#include "CipherAESGCM.h"

#ifdef CIPHER_SYMMETRIC_CHACHA20POLY1305_ACTIVE
#include "CipherChaCha20Poly1305.h"
#endif

#include "DIOStreamTLSRecord.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12RECORD::DIOSTREAMTLS12RECORD()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12RECORD::DIOSTREAMTLS12RECORD()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12RECORD::~DIOSTREAMTLS12RECORD()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12RECORD::~DIOSTREAMTLS12RECORD()
{
  End();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12RECORD::Ini()
* @brief      Initialize the record layer, with both directions still in the clear
* @ingroup    DATAIO
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12RECORD::Ini()
{
  End();

  for(int c=0; c<DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS; c++)
    {
      sequence[c]    = 0;
      isprotected[c] = false;
    }

  isini = true;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLS12RECORD::End()
* @brief      Release the keys of both directions
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS12RECORD::End()
{
  for(int c=0; c<DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS; c++)
    {
      if(cipher[c])
        {
          GEN_DELETE cipher[c];
          cipher[c] = NULL;
        }

      if(key[c])
        {
          GEN_DELETE key[c];
          key[c] = NULL;
        }

      fixedIV[c].Delete();

      sequence[c]    = 0;
      isprotected[c] = false;
    }

  isini = false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12RECORD::IsIni()
* @brief      Check whether the record layer is initialized
* @ingroup    DATAIO
*
* @return     bool : true if it is initialized; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12RECORD::IsIni()
{
  return isini;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12RECORD::SetKeys(DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction, XBUFFER& key, XBUFFER& fixedIV)
* @brief      Install the write key and AEAD salt of one direction, and restart its sequence number
* @note       RFC 5246 section 6.1: the sequence number goes back to zero after each ChangeCipherSpec.
* @ingroup    DATAIO
*
* @param[in]  direction : Local, what this end writes, or remote, what the other end writes.
* @param[in]  key : Write key of that direction.
* @param[in]  fixedIV : AEAD salt of that direction, 4 bytes.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12RECORD::SetKeys(DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction, XBUFFER& key, XBUFFER& fixedIV)
{
  if(!isini)                                                    return false;
  if(direction >= DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS)        return false;
  if(key.IsEmpty() || fixedIV.IsEmpty())                        return false;

  if(cipher[direction])
    {
      GEN_DELETE cipher[direction];
      cipher[direction] = NULL;
    }

  if(this->key[direction])
    {
      GEN_DELETE this->key[direction];
      this->key[direction] = NULL;
    }

  this->fixedIV[direction].Delete();

  this->key[direction] = GEN_NEW CIPHERKEYSYMMETRICAL();
  if(!this->key[direction]) return false;

  if(!this->key[direction]->Set(key))
    {
      GEN_DELETE this->key[direction];
      this->key[direction] = NULL;
      return false;
    }

  if(fixedIV.GetSize() == DIOSTREAMTLS12KEYSCHEDULE_AESGCM_FIXEDIVSIZE)
    {
      if((key.GetSize() != 16) && (key.GetSize() != 32))
        {
          GEN_DELETE this->key[direction];
          this->key[direction] = NULL;
          return false;
        }

      cipher[direction] = GEN_NEW CIPHERAESGCM();
    }
  else
    {
      #ifdef CIPHER_SYMMETRIC_CHACHA20POLY1305_ACTIVE

      if((fixedIV.GetSize() == DIOSTREAMTLS12KEYSCHEDULE_CHACHA20_FIXEDIVSIZE) && (key.GetSize() == 32))
        {
          cipher[direction] = GEN_NEW CIPHERCHACHA20POLY1305();
        }

      #endif
    }

  if(!cipher[direction])
    {
      GEN_DELETE this->key[direction];
      this->key[direction] = NULL;
      return false;
    }

  if(!cipher[direction]->SetKey(this->key[direction]) || !this->fixedIV[direction].Add(fixedIV))
    {
      GEN_DELETE cipher[direction];
      cipher[direction] = NULL;

      GEN_DELETE this->key[direction];
      this->key[direction] = NULL;

      this->fixedIV[direction].Delete();
      return false;
    }

  sequence[direction]    = 0;
  isprotected[direction] = true;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12RECORD::IsProtected(DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction)
* @brief      Check whether one direction already has its keys installed
* @ingroup    DATAIO
*
* @param[in]  direction : Direction value.
*
* @return     bool : true if it is protected; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12RECORD::IsProtected(DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction)
{
  if(direction >= DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS) return false;

  return isprotected[direction];
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XQWORD DIOSTREAMTLS12RECORD::GetSequence(DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction)
* @brief      Get the sequence number of one direction
* @ingroup    DATAIO
*
* @param[in]  direction : Direction value.
*
* @return     XQWORD : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
XQWORD DIOSTREAMTLS12RECORD::GetSequence(DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction)
{
  if(direction >= DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS) return 0;

  return sequence[direction];
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12RECORD::ResetSequence(DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction)
* @brief      Restart the sequence number of one direction
* @ingroup    DATAIO
*
* @param[in]  direction : Direction value.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12RECORD::ResetSequence(DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction)
{
  if(direction >= DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS) return false;

  sequence[direction] = 0;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12RECORD::Nonce_Build(DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction, XBYTE* explicitnonce, XBUFFER& nonce)
* @brief      Build the AEAD nonce for AES-GCM (RFC 5288) or ChaCha20-Poly1305 (RFC 7905)
* @note       INTERNAL
* @ingroup    DATAIO
*
* @param[in]  direction : Direction value.
* @param[in]  explicitnonce : AES-GCM nonce_explicit, or NULL for ChaCha20-Poly1305.
* @param[out] nonce : Buffer that receives the 12 byte AEAD nonce.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12RECORD::Nonce_Build(DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction, XBYTE* explicitnonce, XBUFFER& nonce)
{
  if(direction >= DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS) return false;

  nonce.Delete();

  if(fixedIV[direction].GetSize() == DIOSTREAMTLS12KEYSCHEDULE_AESGCM_FIXEDIVSIZE)
    {
      if(!explicitnonce) return false;

      if(!nonce.Add(fixedIV[direction])) return false;
      if(!nonce.Add(explicitnonce, DIOSTREAMTLS12RECORD_EXPLICITNONCESIZE)) return false;

      return (nonce.GetSize() == 12);
    }

  #ifdef CIPHER_SYMMETRIC_CHACHA20POLY1305_ACTIVE

  if(fixedIV[direction].GetSize() == DIOSTREAMTLS12KEYSCHEDULE_CHACHA20_FIXEDIVSIZE)
    {
      if(!nonce.Add(fixedIV[direction])) return false;

      XBYTE paddedsequence[12] = { 0 };

      for(int c=7; c>=0; c--)
        {
          paddedsequence[11-c] = (XBYTE)((sequence[direction] >> (c*8)) & 0xFF);
        }

      for(XDWORD c=0; c<12; c++) nonce.Get()[c] ^= paddedsequence[c];

      return true;
    }

  #endif

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12RECORD::AAD_Build(XQWORD sequence, DIOSTREAMTLS_CONTENTTYPE contenttype, XWORD version, XWORD plainlength, XBUFFER& additionaldata)
* @brief      Build the additional authenticated data of one record
* @note       INTERNAL
* @ingroup    DATAIO
*
* @param[in]  sequence : Sequence number of the record.
* @param[in]  contenttype : Content type of the record.
* @param[in]  version : Protocol version written in the record header.
* @param[in]  plainlength : Length of the PLAIN text, not the length written in the header.
* @param[out] additionaldata : Buffer that receives the 13 bytes of authenticated data.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12RECORD::AAD_Build(XQWORD sequence, DIOSTREAMTLS_CONTENTTYPE contenttype, XWORD version, XWORD plainlength, XBUFFER& additionaldata)
{
  additionaldata.Delete();

  for(int c=7; c>=0; c--)
    {
      if(!additionaldata.Add((XBYTE)((sequence >> (c*8)) & 0xFF))) return false;
    }

  if(!additionaldata.Add((XBYTE)contenttype))              return false;
  if(!additionaldata.Add((XBYTE)((version >> 8) & 0xFF)))  return false;
  if(!additionaldata.Add((XBYTE)(version & 0xFF)))         return false;
  if(!additionaldata.Add((XBYTE)((plainlength >> 8) & 0xFF))) return false;
  if(!additionaldata.Add((XBYTE)(plainlength & 0xFF)))        return false;

  return (additionaldata.GetSize() == DIOSTREAMTLS12RECORD_AADSIZE);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12RECORD::Protect_OneRecord(DIOSTREAMTLS_CONTENTTYPE contenttype, XBYTE* plain, XWORD size, XBUFFER& records)
* @brief      Protect one record that already fits the maximum plain text size
* @note       INTERNAL
* @ingroup    DATAIO
*
* @param[in]  contenttype : Content type of the record.
* @param[in]  plain : Plain text.
* @param[in]  size : Plain text size.
* @param[out] records : Buffer the encoded record is appended to.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12RECORD::Protect_OneRecord(DIOSTREAMTLS_CONTENTTYPE contenttype, XBYTE* plain, XWORD size, XBUFFER& records)
{
  DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction = DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL;
  DIOSTREAMTLS_MSG_RECORDHEADER     header;

  header.SetProtocolVersion(DIOSTREAMTLS_MSG_VERSION_TLS_1_2);

  // Before the ChangeCipherSpec of this end, every record travels in the clear, exactly as it is.
  if(!isprotected[direction])
    {
      XBUFFER encodedrecord;

      header.SetContenType(contenttype);
      header.SetLength(size);

      if(!header.SetToBuffer(encodedrecord, false)) return false;
      if(size && !encodedrecord.Add(plain, size))    return false;

      return records.Add(encodedrecord);
    }

  if(!cipher[direction]) return false;

  bool isAESGCM = (fixedIV[direction].GetSize() == DIOSTREAMTLS12KEYSCHEDULE_AESGCM_FIXEDIVSIZE);

  if(isAESGCM && sequence[direction] >= DIOSTREAMTLS_AESGCM_MAXKEYUSAGERECORDS) return false;
  if(sequence[direction] == 0xFFFFFFFFFFFFFFFFULL) return false;                  // The sequence number must never wrap

  XBYTE explicitnonce[DIOSTREAMTLS12RECORD_EXPLICITNONCESIZE] = { 0 };
  XDWORD explicitnoncesize = isAESGCM?DIOSTREAMTLS12RECORD_EXPLICITNONCESIZE:DIOSTREAMTLS12RECORD_CHACHA20_EXPLICITNONCESIZE;

  if(isAESGCM)
    {
      // RFC 5288: AES-GCM carries the sequence-derived nonce_explicit in each record.
      for(int c=7; c>=0; c--)
        {
          explicitnonce[7-c] = (XBYTE)((sequence[direction] >> (c*8)) & 0xFF);
        }
    }

  XBUFFER nonce;
  XBUFFER additionaldata;
  XBUFFER tag;

  if(!Nonce_Build(direction, explicitnoncesize?explicitnonce:NULL, nonce))                                                      return false;
  if(!AAD_Build(sequence[direction], contenttype, DIOSTREAMTLS_MSG_VERSION_TLS_1_2, size, additionaldata)) return false;

  if(!cipher[direction]->CipherAEAD(plain, size, nonce, additionaldata, tag)) return false;

  XDWORD fragmentsize = explicitnoncesize + size + tag.GetSize();

  if(fragmentsize > DIOSTREAMTLS12RECORD_MAXCIPHERSIZE) return false;

  header.SetContenType(contenttype);                                            // In TLS 1.2 the real type stays in the header
  header.SetLength((XWORD)fragmentsize);

  XBUFFER encodedrecord;

  if(!header.SetToBuffer(encodedrecord, false))                                      return false;
  if(explicitnoncesize && !encodedrecord.Add(explicitnonce, explicitnoncesize))                   return false;
  if(!encodedrecord.Add(cipher[direction]->GetResult()))                              return false;
  if(!encodedrecord.Add(tag))                                                         return false;
  if(!records.Add(encodedrecord))                                                     return false;

  sequence[direction]++;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12RECORD::Protect(DIOSTREAMTLS_CONTENTTYPE contenttype, XBYTE* plain, XDWORD size, XBUFFER& records)
* @brief      Protect a plain text, splitting it into as many records as the maximum size needs
* @ingroup    DATAIO
*
* @param[in]  contenttype : Content type of the records.
* @param[in]  plain : Plain text.
* @param[in]  size : Plain text size.
* @param[out] records : Buffer the encoded records are appended to.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12RECORD::Protect(DIOSTREAMTLS_CONTENTTYPE contenttype, XBYTE* plain, XDWORD size, XBUFFER& records)
{
  if(!isini)            return false;
  if(size && !plain)    return false;

  if(!size)
    {
      return Protect_OneRecord(contenttype, plain, 0, records);
    }

  XDWORD position = 0;

  while(position < size)
    {
      XDWORD blocksize = size - position;

      if(blocksize > DIOSTREAMTLS12RECORD_MAXPLAINSIZE) blocksize = DIOSTREAMTLS12RECORD_MAXPLAINSIZE;

      if(!Protect_OneRecord(contenttype, &plain[position], (XWORD)blocksize, records)) return false;

      position += blocksize;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12RECORD::Protect(DIOSTREAMTLS_CONTENTTYPE contenttype, XBUFFER& plain, XBUFFER& records)
* @brief      Protect a plain text, splitting it into as many records as the maximum size needs
* @ingroup    DATAIO
*
* @param[in]  contenttype : Content type of the records.
* @param[in]  plain : Plain text.
* @param[out] records : Buffer the encoded records are appended to.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12RECORD::Protect(DIOSTREAMTLS_CONTENTTYPE contenttype, XBUFFER& plain, XBUFFER& records)
{
  return Protect(contenttype, plain.Get(), plain.GetSize(), records);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12RECORD::Unprotect(XBUFFER& record, DIOSTREAMTLS_CONTENTTYPE& contenttype, XBUFFER& plain)
* @brief      Decode and authenticate one complete TLS record
* @ingroup    DATAIO
*
* @param[in]  record : One complete record, header included.
* @param[out] contenttype : Decoded content type.
* @param[out] plain : Buffer that receives the plain text.
*
* @return     bool : true if the record is valid; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12RECORD::Unprotect(XBUFFER& record, DIOSTREAMTLS_CONTENTTYPE& contenttype, XBUFFER& plain)
{
  DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction = DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE;
  DIOSTREAMTLS_MSG_RECORDHEADER     header;
  XWORD                             length;

  plain.Delete();
  lastalertdescription = DIOSTREAMTLS_ALERT_DESCRIPTION_DECODE_ERROR;

  if(!isini)               return false;
  if(!header.Peek(record)) return false;

  contenttype = header.GetContenType();
  length      = header.GetLength();

  if(record.GetSize() != (XDWORD)(DIOSTREAMTLS_MSG_RECORDHEADER_SIZE + length)) return false;

  if(!isprotected[direction] || (contenttype == DIOSTREAMTLS_MSG_CONTENTTYPE_CHANGE_CIPHER_SPEC))
    {
      if(length > DIOSTREAMTLS12RECORD_MAXPLAINSIZE)
        {
          lastalertdescription = DIOSTREAMTLS_ALERT_DESCRIPTION_RECORD_OVERFLOW;
          return false;
        }

      if(length && !plain.Add(&record.Get()[DIOSTREAMTLS_MSG_RECORDHEADER_SIZE], length))
        {
          lastalertdescription = DIOSTREAMTLS_ALERT_DESCRIPTION_INTERNAL_ERROR;
          return false;
        }

      return true;
    }

  if(!cipher[direction])
    {
      lastalertdescription = DIOSTREAMTLS_ALERT_DESCRIPTION_INTERNAL_ERROR;
      return false;
    }

  if(sequence[direction] == 0xFFFFFFFFFFFFFFFFULL)
    {
      lastalertdescription = DIOSTREAMTLS_ALERT_DESCRIPTION_INTERNAL_ERROR;
      return false;
    }

  XDWORD tagsize = cipher[direction]->GetAEADTagSize();
  bool   isAESGCM = (fixedIV[direction].GetSize() == DIOSTREAMTLS12KEYSCHEDULE_AESGCM_FIXEDIVSIZE);
  XDWORD explicitnoncesize = isAESGCM?DIOSTREAMTLS12RECORD_EXPLICITNONCESIZE:DIOSTREAMTLS12RECORD_CHACHA20_EXPLICITNONCESIZE;

  if((XDWORD)length < (explicitnoncesize + tagsize))
    {
      lastalertdescription = DIOSTREAMTLS_ALERT_DESCRIPTION_BAD_RECORD_MAC;
      return false;
    }

  XWORD  plainlength = (XWORD)(length - explicitnoncesize - tagsize);
  XBYTE* fragment    = &record.Get()[DIOSTREAMTLS_MSG_RECORDHEADER_SIZE];

  if(plainlength > DIOSTREAMTLS12RECORD_MAXPLAINSIZE)
    {
      lastalertdescription = DIOSTREAMTLS_ALERT_DESCRIPTION_RECORD_OVERFLOW;
      return false;
    }

  XBUFFER nonce;
  XBUFFER additionaldata;
  XBUFFER ciphertext;
  XBUFFER tag;

  if(!Nonce_Build(direction, explicitnoncesize?fragment:NULL, nonce) ||
     !AAD_Build(sequence[direction], contenttype, header.GetProtocolVersion(), plainlength, additionaldata))
    {
      lastalertdescription = DIOSTREAMTLS_ALERT_DESCRIPTION_INTERNAL_ERROR;
      return false;
    }

  if((plainlength && !ciphertext.Add(&fragment[explicitnoncesize], plainlength)) ||
     !tag.Add(&fragment[explicitnoncesize + plainlength], tagsize))
    {
      lastalertdescription = DIOSTREAMTLS_ALERT_DESCRIPTION_INTERNAL_ERROR;
      return false;
    }

  if(!cipher[direction]->UncipherAEAD(ciphertext.Get(), plainlength, nonce, additionaldata, tag))
    {
      lastalertdescription = DIOSTREAMTLS_ALERT_DESCRIPTION_BAD_RECORD_MAC;
      return false;
    }

  if(plainlength && !plain.Add(cipher[direction]->GetResult()->Get(), plainlength))
    {
      lastalertdescription = DIOSTREAMTLS_ALERT_DESCRIPTION_INTERNAL_ERROR;
      return false;
    }

  sequence[direction]++;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS_ALERT_DESCRIPTION DIOSTREAMTLS12RECORD::GetLastAlertDescription()
* @brief      Get the alert that best describes the last record-layer error
* @ingroup    DATAIO
*
* @return     DIOSTREAMTLS_ALERT_DESCRIPTION : Alert description.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_ALERT_DESCRIPTION DIOSTREAMTLS12RECORD::GetLastAlertDescription()
{
  return lastalertdescription;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12RECORD::Record_Extract(XBUFFER& input, XBUFFER& record)
* @brief      Extract one complete record from the pending input, if there is one
* @note       STATIC. The record header is identical in TLS 1.2 and TLS 1.3, so the TLS 1.3 reader is reused.
* @ingroup    DATAIO
*
* @param[in]  input : Pending input; the record is removed from it.
* @param[out] record : Buffer that receives one complete record.
*
* @return     bool : true if one complete record was extracted; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12RECORD::Record_Extract(XBUFFER& input, XBUFFER& record)
{
  return DIOSTREAMTLSRECORD::Record_Extract(input, record);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLS12RECORD::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS12RECORD::Clean()
{
  isini                = false;
  lastalertdescription = DIOSTREAMTLS_ALERT_DESCRIPTION_DECODE_ERROR;

  for(int c=0; c<DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS; c++)
    {
      cipher[c]      = NULL;
      key[c]         = NULL;
      sequence[c]    = 0;
      isprotected[c] = false;
    }
}
