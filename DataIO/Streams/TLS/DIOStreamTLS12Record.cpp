/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamTLS12Record.cpp
*
* @class      DIOSTREAMTLS12RECORD
* @brief      Data Input/Output Stream TLS 1.2 record protection class (RFC 5246 + RFC 5288)
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

#include <string.h>

#include "CipherKeySymmetrical.h"
#include "CipherAESGCM.h"

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

  cipher[direction] = GEN_NEW CIPHERAESGCM();
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
* @brief      Build the AEAD nonce as salt + explicit nonce
* @note       INTERNAL
* @ingroup    DATAIO
*
* @param[in]  direction : Direction value.
* @param[in]  explicitnonce : The 8 bytes that travel in the clear at the front of the fragment.
* @param[out] nonce : Buffer that receives the 12 byte nonce.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12RECORD::Nonce_Build(DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction, XBYTE* explicitnonce, XBUFFER& nonce)
{
  if(direction >= DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS) return false;
  if(!explicitnonce)                                     return false;

  nonce.Delete();

  if(!nonce.Add(fixedIV[direction]))                                        return false;
  if(!nonce.Add(explicitnonce, DIOSTREAMTLS12RECORD_EXPLICITNONCESIZE))     return false;

  return true;
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
      header.SetContenType(contenttype);
      header.SetLength(size);

      if(!header.SetToBuffer(records, false)) return false;
      if(size && !records.Add(plain, size))   return false;

      return true;
    }

  if(!cipher[direction]) return false;

  if(sequence[direction] == 0xFFFFFFFFFFFFFFFFULL) return false;                // The sequence number must never wrap

  // The explicit nonce only has to be unique for the key. Using the sequence number is what every
  // implementation does, and it makes a capture reproducible.
  XBYTE explicitnonce[DIOSTREAMTLS12RECORD_EXPLICITNONCESIZE];

  for(int c=7; c>=0; c--)
    {
      explicitnonce[7-c] = (XBYTE)((sequence[direction] >> (c*8)) & 0xFF);
    }

  XBUFFER nonce;
  XBUFFER additionaldata;
  XBUFFER tag;

  if(!Nonce_Build(direction, explicitnonce, nonce))                                                      return false;
  if(!AAD_Build(sequence[direction], contenttype, DIOSTREAMTLS_MSG_VERSION_TLS_1_2, size, additionaldata)) return false;

  if(!cipher[direction]->CipherAEAD(plain, size, nonce, additionaldata, tag)) return false;

  XDWORD fragmentsize = DIOSTREAMTLS12RECORD_EXPLICITNONCESIZE + size + tag.GetSize();

  if(fragmentsize > DIOSTREAMTLS12RECORD_MAXCIPHERSIZE) return false;

  header.SetContenType(contenttype);                                            // In TLS 1.2 the real type stays in the header
  header.SetLength((XWORD)fragmentsize);

  if(!header.SetToBuffer(records, false))                                            return false;
  if(!records.Add(explicitnonce, DIOSTREAMTLS12RECORD_EXPLICITNONCESIZE))           return false;
  if(!records.Add(cipher[direction]->GetResult()))                                   return false;
  if(!records.Add(tag))                                                              return false;

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
* @brief      Unprotect one complete record and authenticate it
* @ingroup    DATAIO
*
* @param[in]  record : One complete record, header included.
* @param[out] contenttype : Content type of the record.
* @param[out] plain : Buffer that receives the plain text.
*
* @return     bool : true if the record authenticates; false if it does not, and then nothing is returned.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12RECORD::Unprotect(XBUFFER& record, DIOSTREAMTLS_CONTENTTYPE& contenttype, XBUFFER& plain)
{
  DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction = DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE;
  DIOSTREAMTLS_MSG_RECORDHEADER     header;
  XWORD                             length;

  plain.Delete();

  if(!isini)                return false;
  if(!header.Peek(record))  return false;

  contenttype = header.GetContenType();
  length      = header.GetLength();

  if(record.GetSize() != (XDWORD)(DIOSTREAMTLS_MSG_RECORDHEADER_SIZE + length)) return false;

  // ChangeCipherSpec always travels in the clear: it is precisely the message that announces the change,
  // so it can never be read with the keys it is installing.
  if(!isprotected[direction] || (contenttype == DIOSTREAMTLS_MSG_CONTENTTYPE_CHANGE_CIPHER_SPEC))
    {
      if(length && !plain.Add(&record.Get()[DIOSTREAMTLS_MSG_RECORDHEADER_SIZE], length)) return false;

      return true;
    }

  if(!cipher[direction]) return false;

  if(sequence[direction] == 0xFFFFFFFFFFFFFFFFULL) return false;

  XDWORD tagsize = cipher[direction]->GetAEADTagSize();

  if((XDWORD)length < (DIOSTREAMTLS12RECORD_EXPLICITNONCESIZE + tagsize)) return false;

  XWORD  plainlength = (XWORD)(length - DIOSTREAMTLS12RECORD_EXPLICITNONCESIZE - tagsize);
  XBYTE* fragment    = &record.Get()[DIOSTREAMTLS_MSG_RECORDHEADER_SIZE];

  XBUFFER nonce;
  XBUFFER additionaldata;
  XBUFFER ciphertext;
  XBUFFER tag;

  if(!Nonce_Build(direction, fragment, nonce))                                                              return false;
  if(!AAD_Build(sequence[direction], contenttype, header.GetProtocolVersion(), plainlength, additionaldata)) return false;

  if(plainlength && !ciphertext.Add(&fragment[DIOSTREAMTLS12RECORD_EXPLICITNONCESIZE], plainlength))         return false;
  if(!tag.Add(&fragment[DIOSTREAMTLS12RECORD_EXPLICITNONCESIZE + plainlength], tagsize))                     return false;

  if(!cipher[direction]->UncipherAEAD(ciphertext.Get(), plainlength, nonce, additionaldata, tag)) return false;

  if(plainlength && !plain.Add(cipher[direction]->GetResult()->Get(), plainlength)) return false;

  sequence[direction]++;

  return true;
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
  isini = false;

  for(int c=0; c<DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS; c++)
    {
      cipher[c]      = NULL;
      key[c]         = NULL;
      sequence[c]    = 0;
      isprotected[c] = false;
    }
}
