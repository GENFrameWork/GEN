/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamTLS12Session.cpp
*
* @class      DIOSTREAMTLS12SESSION
* @brief      Data Input/Output Stream TLS 1.2 (RFC 5246) role-neutral Session class
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

#include "Hash.h"

#include "DIOStreamTLS12Session.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12SESSION::DIOSTREAMTLS12SESSION()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12SESSION::DIOSTREAMTLS12SESSION() : keyexchangep384(CIPHERTYPE_ECDSA_SECP384R1)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12SESSION::~DIOSTREAMTLS12SESSION()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12SESSION::~DIOSTREAMTLS12SESSION()
{
  End();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::Ini(XWORD ciphersuite, DIOSTREAMTLSKEYSCHEDULE_ROLE role)
* @brief      Initialize the session for a given cipher suite and role
* @ingroup    DATAIO
*
* @param[in]  ciphersuite : One of the four DIOSTREAMTLS12_CIPHER_* suites.
* @param[in]  role : Client or server (only client is exercised so far).
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::Ini(XWORD ciphersuite, DIOSTREAMTLSKEYSCHEDULE_ROLE role)
{
  // Deliberately NOT calling End() here: unlike TLS 1.3, this session is constructed lazily, only once the
  // cipher suite is known from ServerHello. By then the caller has already fed ClientHello (and possibly the
  // very ServerHello being processed right now) into the transcript / record input accumulators below, via
  // GetTranscript()/GetRecordInput() directly; wiping them here would silently corrupt the transcript hash
  // that both ends compute for Finished.
  keyschedule.End();
  record.End();
  KeyExchange_Delete();

  if(!keyschedule.Ini(ciphersuite, role) || !record.Ini())
    {
      keyschedule.End();
      return false;
    }

  this->role = role;
  isini      = true;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLS12SESSION::End()
* @brief      End the session and release all state
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS12SESSION::End()
{
  KeyExchange_Delete();

  keyschedule.End();
  record.End();

  recordinput.Delete();
  handshakeinput.Delete();
  transcript.Delete();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::IsIni()
* @brief      Check whether the session is initialized
* @ingroup    DATAIO
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::IsIni()
{
  return isini;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLSKEYSCHEDULE_ROLE DIOSTREAMTLS12SESSION::GetRole()
* @brief      Get the session role
* @ingroup    DATAIO
*
* @return     DIOSTREAMTLSKEYSCHEDULE_ROLE : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSKEYSCHEDULE_ROLE DIOSTREAMTLS12SESSION::GetRole()
{
  return role;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12KEYSCHEDULE* DIOSTREAMTLS12SESSION::GetKeySchedule()
* @brief      Get the key schedule
* @ingroup    DATAIO
*
* @return     DIOSTREAMTLS12KEYSCHEDULE* : Pointer to the requested object.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12KEYSCHEDULE* DIOSTREAMTLS12SESSION::GetKeySchedule()
{
  return &keyschedule;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12RECORD* DIOSTREAMTLS12SESSION::GetRecord()
* @brief      Get the record protection instance
* @ingroup    DATAIO
*
* @return     DIOSTREAMTLS12RECORD* : Pointer to the requested object.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12RECORD* DIOSTREAMTLS12SESSION::GetRecord()
{
  return &record;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::KeyExchange_Generate(XWORD group, XBUFFER& publickey)
* @brief      Generate a fresh ephemeral ECDHE key pair
* @ingroup    DATAIO
*
* @param[in]  group : TLS supported group value (X25519, secp256r1 or secp384r1).
* @param[out] publickey : Encoded public key (uncompressed EC point / raw X25519 key).
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::KeyExchange_Generate(XWORD group, XBUFFER& publickey)
{
  publickey.Delete();

  if(!isini) return false;

  switch(group)
    {
      case DIOSTREAMTLS_MSG_CURVEID_X25519    : keyexchange.CleanAllKeys();

                                                if(!keyexchange.GenerateRandomPrivateKey() ||
                                                   !keyexchange.CreatePublicKey() ||
                                                   !publickey.Add(keyexchange.GetKey(CIPHERECDSAX25519_TYPEKEY_PUBLIC),
                                                                  CIPHERECDSAX25519_MAXKEY))
                                                  {
                                                    keyexchange.CleanAllKeys();
                                                    return false;
                                                  }
                                                break;

      case DIOSTREAMTLS_MSG_CURVEID_SECP256R1 : keyexchangep256private.FillBuffer(0);
                                                keyexchangep256private.Delete();
                                                keyexchangep256public.Delete();

                                                if(!keyexchangep256.KeyPair_Create(keyexchangep256private,
                                                                                  keyexchangep256public) ||
                                                   !publickey.Add(keyexchangep256public))
                                                  {
                                                    keyexchangep256private.FillBuffer(0);
                                                    keyexchangep256private.Delete();
                                                    keyexchangep256public.Delete();
                                                    return false;
                                                  }
                                                break;

      case DIOSTREAMTLS_MSG_CURVEID_SECP384R1 : keyexchangep384private.FillBuffer(0);
                                                keyexchangep384private.Delete();
                                                keyexchangep384public.Delete();

                                                if(!keyexchangep384.KeyPair_Create(keyexchangep384private,
                                                                                  keyexchangep384public) ||
                                                   !publickey.Add(keyexchangep384public))
                                                  {
                                                    keyexchangep384private.FillBuffer(0);
                                                    keyexchangep384private.Delete();
                                                    keyexchangep384public.Delete();
                                                    return false;
                                                  }
                                                break;

                                      default : return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::KeyExchange_SharedSecret(XWORD group, XBUFFER& publickey, XBUFFER& sharedsecret)
* @brief      Calculate the ECDHE shared secret (== pre_master_secret, RFC 4492 section 5.10)
* @ingroup    DATAIO
*
* @param[in]  group : Negotiated TLS supported group value.
* @param[in]  publickey : Encoded public key of the remote end.
* @param[out] sharedsecret : Calculated shared secret (this IS the pre_master_secret, unlike TLS 1.3).
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::KeyExchange_SharedSecret(XWORD group, XBUFFER& publickey, XBUFFER& sharedsecret)
{
  sharedsecret.Delete();

  if(!isini) return false;

  switch(group)
    {
      case DIOSTREAMTLS_MSG_CURVEID_X25519    : if((publickey.GetSize() != CIPHERECDSAX25519_MAXKEY) ||
                                                   !keyexchange.GetKey(CIPHERECDSAX25519_TYPEKEY_PRIVATE) ||
                                                   !keyexchange.CreateSharedKey(publickey.Get()) ||
                                                   !sharedsecret.Add(keyexchange.GetKey(CIPHERECDSAX25519_TYPEKEY_SHARED),
                                                                     CIPHERECDSAX25519_MAXKEY))
                                                  {
                                                    return false;
                                                  }
                                                break;

      case DIOSTREAMTLS_MSG_CURVEID_SECP256R1 : if(keyexchangep256private.IsEmpty() ||
                                                   !keyexchangep256.SharedSecret_Create(keyexchangep256private,
                                                                                       publickey, sharedsecret))
                                                  {
                                                    return false;
                                                  }
                                                break;

      case DIOSTREAMTLS_MSG_CURVEID_SECP384R1 : if(keyexchangep384private.IsEmpty() ||
                                                   !keyexchangep384.SharedSecret_Create(keyexchangep384private,
                                                                                       publickey, sharedsecret))
                                                  {
                                                    return false;
                                                  }
                                                break;

                                      default : return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLS12SESSION::KeyExchange_Delete()
* @brief      Erase all ephemeral key exchange material
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS12SESSION::KeyExchange_Delete()
{
  keyexchange.CleanAllKeys();

  keyexchangep256private.FillBuffer(0);
  keyexchangep256private.Delete();
  keyexchangep256public.Delete();

  keyexchangep384private.FillBuffer(0);
  keyexchangep384private.Delete();
  keyexchangep384public.Delete();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::Keys_Activate(XBUFFER& premastersecret, XBUFFER& clientrandom, XBUFFER& serverrandom)
* @brief      Derive master_secret and key_block, and load both record directions
* @ingroup    DATAIO
*
* @param[in]  premastersecret : ECDHE shared secret.
* @param[in]  clientrandom : ClientHello random.
* @param[in]  serverrandom : ServerHello random.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::Keys_Activate(XBUFFER& premastersecret, XBUFFER& clientrandom, XBUFFER& serverrandom)
{
  if(!isini ||
     !keyschedule.MasterSecret_Create(premastersecret, clientrandom, serverrandom) ||
     !keyschedule.KeyBlock_Create(clientrandom, serverrandom) ||
     !record.SetKeys(DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL,
                     *keyschedule.GetKey(DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL),
                     *keyschedule.GetFixedIV(DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL)) ||
     !record.SetKeys(DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE,
                     *keyschedule.GetKey(DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE),
                     *keyschedule.GetFixedIV(DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE)))
    {
      return false;
    }

  keysactivated = true;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* DIOSTREAMTLS12SESSION::GetRecordInput()
* @brief      Get the transport record accumulator
* @ingroup    DATAIO
*
* @return     XBUFFER* : Pointer to the requested buffer; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* DIOSTREAMTLS12SESSION::GetRecordInput()
{
  return &recordinput;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::RecordInput_Add(XBYTE* data, XDWORD size)
* @brief      Add bytes to the transport record accumulator
* @ingroup    DATAIO
*
* @param[in]  data : Bytes to add.
* @param[in]  size : Number of bytes to add.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::RecordInput_Add(XBYTE* data, XDWORD size)
{
  if(!isini || (!data && size) ||
     (size > DIOSTREAMTLS12SESSION_MAXRECORDINPUTSIZE) ||
     (recordinput.GetSize() > (DIOSTREAMTLS12SESSION_MAXRECORDINPUTSIZE - size)))
    {
      return false;
    }

  return size?recordinput.Add(data, size):true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::RecordInput_Add(XBUFFER& data)
* @brief      Add a buffer to the transport record accumulator
* @ingroup    DATAIO
*
* @param[in]  data : Buffer to add.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::RecordInput_Add(XBUFFER& data)
{
  return RecordInput_Add(data.Get(), data.GetSize());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12SESSION_RESULT DIOSTREAMTLS12SESSION::Record_Extract(DIOSTREAMTLS_CONTENTTYPE& contenttype, XBUFFER& plain)
* @brief      Extract and decode one record from the transport accumulator
* @ingroup    DATAIO
*
* @param[out] contenttype : Actual type of the record content.
* @param[out] plain : Decoded record content.
*
* @return     DIOSTREAMTLS12SESSION_RESULT : Complete, incomplete or error.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12SESSION_RESULT DIOSTREAMTLS12SESSION::Record_Extract(DIOSTREAMTLS_CONTENTTYPE& contenttype, XBUFFER& plain)
{
  DIOSTREAMTLS_MSG_RECORDHEADER header;
  XBUFFER                       onerecord;

  plain.Delete();

  if(!isini)
    {
      return DIOSTREAMTLS12SESSION_RESULT_ERROR;
    }

  if(recordinput.GetSize() < DIOSTREAMTLS_MSG_RECORDHEADER_SIZE)
    {
      return DIOSTREAMTLS12SESSION_RESULT_INCOMPLETE;
    }

  if(!header.Peek(recordinput) || (header.GetLength() > DIOSTREAMTLS12RECORD_MAXCIPHERSIZE))
    {
      return DIOSTREAMTLS12SESSION_RESULT_ERROR;
    }

  if(recordinput.GetSize() < ((XDWORD)DIOSTREAMTLS_MSG_RECORDHEADER_SIZE + header.GetLength()))
    {
      return DIOSTREAMTLS12SESSION_RESULT_INCOMPLETE;
    }

  if(!DIOSTREAMTLS12RECORD::Record_Extract(recordinput, onerecord))
    {
      return DIOSTREAMTLS12SESSION_RESULT_ERROR;
    }

  if(!record.Unprotect(onerecord, contenttype, plain))
    {
      return DIOSTREAMTLS12SESSION_RESULT_ERROR;
    }

  return DIOSTREAMTLS12SESSION_RESULT_COMPLETE;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* DIOSTREAMTLS12SESSION::GetHandshakeInput()
* @brief      Get the handshake message accumulator
* @ingroup    DATAIO
*
* @return     XBUFFER* : Pointer to the requested buffer; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* DIOSTREAMTLS12SESSION::GetHandshakeInput()
{
  return &handshakeinput;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::HandshakeInput_Add(XBYTE* data, XDWORD size)
* @brief      Add bytes to the handshake message accumulator
* @ingroup    DATAIO
*
* @param[in]  data : Bytes to add.
* @param[in]  size : Number of bytes to add.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::HandshakeInput_Add(XBYTE* data, XDWORD size)
{
  if(!isini || (!data && size) ||
     (size > DIOSTREAMTLS12SESSION_MAXHANDSHAKESIZE) ||
     (handshakeinput.GetSize() > (DIOSTREAMTLS12SESSION_MAXHANDSHAKESIZE - size)))
    {
      return false;
    }

  return size?handshakeinput.Add(data, size):true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::HandshakeInput_Add(XBUFFER& data)
* @brief      Add a buffer to the handshake message accumulator
* @ingroup    DATAIO
*
* @param[in]  data : Buffer to add.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::HandshakeInput_Add(XBUFFER& data)
{
  return HandshakeInput_Add(data.Get(), data.GetSize());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12SESSION_RESULT DIOSTREAMTLS12SESSION::Handshake_Extract(XBUFFER& message)
* @brief      Extract one complete handshake message from the handshake accumulator
* @ingroup    DATAIO
*
* @param[out] message : Complete handshake message, including its header.
*
* @return     DIOSTREAMTLS12SESSION_RESULT : Complete, incomplete or error.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12SESSION_RESULT DIOSTREAMTLS12SESSION::Handshake_Extract(XBUFFER& message)
{
  message.Delete();

  if(!isini)
    {
      return DIOSTREAMTLS12SESSION_RESULT_ERROR;
    }

  if(handshakeinput.GetSize() < DIOSTREAMTLS_MSG_HANDSHAKEHEADER_SIZE)
    {
      return DIOSTREAMTLS12SESSION_RESULT_INCOMPLETE;
    }

  XBYTE* data = handshakeinput.Get();
  if(!data)
    {
      return DIOSTREAMTLS12SESSION_RESULT_ERROR;
    }

  XDWORD length        = ((XDWORD)data[1] << 16) | ((XDWORD)data[2] << 8) | (XDWORD)data[3];
  XDWORD messagelength = DIOSTREAMTLS_MSG_HANDSHAKEHEADER_SIZE + length;

  if(messagelength > DIOSTREAMTLS12SESSION_MAXHANDSHAKESIZE)
    {
      return DIOSTREAMTLS12SESSION_RESULT_ERROR;
    }

  if(handshakeinput.GetSize() < messagelength)
    {
      return DIOSTREAMTLS12SESSION_RESULT_INCOMPLETE;
    }

  if(!DIOSTREAMTLS_MSG_HANDSHAKE::Message_Extract(handshakeinput, message))
    {
      return DIOSTREAMTLS12SESSION_RESULT_ERROR;
    }

  return DIOSTREAMTLS12SESSION_RESULT_COMPLETE;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* DIOSTREAMTLS12SESSION::GetTranscript()
* @brief      Get the exact handshake transcript
* @ingroup    DATAIO
*
* @return     XBUFFER* : Pointer to the requested buffer; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* DIOSTREAMTLS12SESSION::GetTranscript()
{
  return &transcript;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::Transcript_Add(XBUFFER& message)
* @brief      Add one exact handshake message (with its header) to the running transcript
* @ingroup    DATAIO
*
* @param[in]  message : Complete handshake message, including its header.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::Transcript_Add(XBUFFER& message)
{
  return transcript.Add(message);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::TranscriptHash(XBUFFER& transcripthash)
* @brief      Hash(handshake_messages): the negotiated PRF hash over the transcript so far
* @ingroup    DATAIO
*
* @param[out] transcripthash : Calculated hash.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::TranscriptHash(XBUFFER& transcripthash)
{
  HASH* hash = keyschedule.GetHash();

  transcripthash.Delete();

  if(!hash || !hash->ResetResult() || !hash->Do(transcript))
    {
      return false;
    }

  return transcripthash.Add(*hash->GetResult());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLS12SESSION::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS12SESSION::Clean()
{
  role          = DIOSTREAMTLSKEYSCHEDULE_ROLE_CLIENT;
  isini         = false;
  keysactivated = false;

  iserror                      = false;
  closenotifysent               = false;
  closenotifyreceived           = false;
  transportclosedwithoutnotify  = false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* DIOSTREAMTLS12SESSION::GetApplicationInput()
* @brief      Get the decoded application data accumulator
* @ingroup    DATAIO
*
* @return     XBUFFER* : Pointer to the requested buffer.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* DIOSTREAMTLS12SESSION::GetApplicationInput()
{
  return &applicationinput;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::ApplicationData_Protect(XBYTE* data, XDWORD size, XBUFFER& records)
* @brief      Protect application data with the active LOCAL key
* @ingroup    DATAIO
*
* @param[in]  data : Application data pointer.
* @param[in]  size : Number of application bytes.
* @param[out] records : Buffer where the protected records are added.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::ApplicationData_Protect(XBYTE* data, XDWORD size, XBUFFER& records)
{
  if(!isini || !keysactivated || (!data && size) || iserror || closenotifysent || closenotifyreceived)
    {
      return false;
    }

  return record.Protect(DIOSTREAMTLS_MSG_CONTENTTYPE_APPLICATION_DATA, data, size, records);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::ApplicationData_Protect(XBUFFER& data, XBUFFER& records)
* @brief      Protect an application buffer with the active LOCAL key
* @ingroup    DATAIO
*
* @param[in]  data : Application data.
* @param[out] records : Buffer where the protected records are added.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::ApplicationData_Protect(XBUFFER& data, XBUFFER& records)
{
  return ApplicationData_Protect(data.Get(), data.GetSize(), records);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSTREAMTLS12SESSION::ApplicationData_Read(XBYTE* data, XDWORD size)
* @brief      Read decoded application data
* @ingroup    DATAIO
*
* @param[out] data : Destination buffer.
* @param[in]  size : Maximum number of bytes to read.
*
* @return     XDWORD : Number of bytes read.
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOSTREAMTLS12SESSION::ApplicationData_Read(XBYTE* data, XDWORD size)
{
  if(!data || !size) return 0;

  return applicationinput.Extract(data, 0, size);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12SESSION_RESULT DIOSTREAMTLS12SESSION::ApplicationData_Process()
* @brief      Process all complete records received after the handshake
* @note       RFC 5246 has no KeyUpdate/NewSessionTicket: any post-Finished HANDSHAKE-type record is renegotiation,
*             which this client does not support and rejects as an error, matching the TLS 1.3 session's default
*             case for anything it does not recognize.
* @ingroup    DATAIO
*
* @return     DIOSTREAMTLS12SESSION_RESULT : Complete, incomplete or error.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12SESSION_RESULT DIOSTREAMTLS12SESSION::ApplicationData_Process()
{
  bool processed = false;

  if(!isini || !keysactivated || iserror)
    {
      return DIOSTREAMTLS12SESSION_RESULT_ERROR;
    }

  while(true)
    {
      DIOSTREAMTLS_CONTENTTYPE     contenttype = (DIOSTREAMTLS_CONTENTTYPE)0;
      DIOSTREAMTLS12SESSION_RESULT result;
      XBUFFER                      plain;

      result = Record_Extract(contenttype, plain);

      if(result == DIOSTREAMTLS12SESSION_RESULT_INCOMPLETE)
        {
          return processed?DIOSTREAMTLS12SESSION_RESULT_COMPLETE:DIOSTREAMTLS12SESSION_RESULT_INCOMPLETE;
        }

      if(result == DIOSTREAMTLS12SESSION_RESULT_ERROR)
        {
          iserror = true;
          return DIOSTREAMTLS12SESSION_RESULT_ERROR;
        }

      processed = true;

      switch(contenttype)
        {
          case DIOSTREAMTLS_MSG_CONTENTTYPE_APPLICATION_DATA : if(closenotifyreceived || (plain.GetSize() && !applicationinput.Add(plain)))
                                                                  {
                                                                    iserror = true;
                                                                    return DIOSTREAMTLS12SESSION_RESULT_ERROR;
                                                                  }
                                                                break;

          case DIOSTREAMTLS_MSG_CONTENTTYPE_ALERT            : { DIOSTREAMTLS_MSG_ALERT alert;

                                                                  if(!alert.GetFromBuffer(plain, false) || !plain.IsEmpty())
                                                                    {
                                                                      iserror = true;
                                                                      return DIOSTREAMTLS12SESSION_RESULT_ERROR;
                                                                    }

                                                                  if(alert.GetDescription() == DIOSTREAMTLS_ALERT_DESCRIPTION_CLOSE_NOTIFY)
                                                                    {
                                                                      if(closenotifyreceived)
                                                                        {
                                                                          iserror = true;
                                                                          return DIOSTREAMTLS12SESSION_RESULT_ERROR;
                                                                        }

                                                                      closenotifyreceived = true;
                                                                    }
                                                                   else
                                                                    {
                                                                      iserror = true;
                                                                      return DIOSTREAMTLS12SESSION_RESULT_ERROR;
                                                                    }
                                                                }
                                                                break;

                                                      default : iserror = true;
                                                                return DIOSTREAMTLS12SESSION_RESULT_ERROR;
        }
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::Alert_Create(DIOSTREAMTLS_ALERT_LEVEL level, DIOSTREAMTLS_ALERT_DESCRIPTION description, XBUFFER& records)
* @brief      Create a TLS 1.2 alert protected with the current LOCAL key (or clear, before keys are active)
* @ingroup    DATAIO
*
* @param[in]  level : Alert level.
* @param[in]  description : Alert description.
* @param[out] records : Encoded alert records.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::Alert_Create(DIOSTREAMTLS_ALERT_LEVEL level, DIOSTREAMTLS_ALERT_DESCRIPTION description, XBUFFER& records)
{
  DIOSTREAMTLS_MSG_ALERT alert;
  XBUFFER                plain;

  if(!isini || closenotifysent) return false;

  alert.SetLevel(level);
  alert.SetDescription(description);

  if(!alert.SetToBuffer(plain, false) ||
     !record.Protect(DIOSTREAMTLS_MSG_CONTENTTYPE_ALERT, plain, records))
    {
      return false;
    }

  if(description == DIOSTREAMTLS_ALERT_DESCRIPTION_CLOSE_NOTIFY) closenotifysent = true;
  if(level == DIOSTREAMTLS_ALERT_LEVEL_FATAL)                     iserror         = true;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::CloseNotify_Create(XBUFFER& records)
* @brief      Create an idempotent close_notify alert
* @ingroup    DATAIO
*
* @param[out] records : Encoded close_notify record, or an empty buffer when it was already sent.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::CloseNotify_Create(XBUFFER& records)
{
  records.Delete();

  if(closenotifysent) return true;

  if(!isini || !keysactivated) return false;

  return Alert_Create(DIOSTREAMTLS_ALERT_LEVEL_WARNING, DIOSTREAMTLS_ALERT_DESCRIPTION_CLOSE_NOTIFY, records);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::IsCloseNotifySent()
* @brief      Check whether close_notify was sent
* @ingroup    DATAIO
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::IsCloseNotifySent()
{
  return closenotifysent;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::IsCloseNotifyReceived()
* @brief      Check whether close_notify was received
* @ingroup    DATAIO
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::IsCloseNotifyReceived()
{
  return closenotifyreceived;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::IsError()
* @brief      Check whether the application record stream is in error
* @ingroup    DATAIO
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::IsError()
{
  return iserror;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::IsTransportClosedWithoutNotify()
* @brief      Check whether the transport ended without close_notify
* @ingroup    DATAIO
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::IsTransportClosedWithoutNotify()
{
  return transportclosedwithoutnotify;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12SESSION::TransportClosed()
* @brief      Register the end of the underlying transport
* @ingroup    DATAIO
*
* @return     bool : true when close_notify had been received; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12SESSION::TransportClosed()
{
  if(!isini) return false;

  if(closenotifyreceived) return true;

  transportclosedwithoutnotify = true;
  iserror                      = true;

  return false;
}
