/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamTLSSession.cpp
*
* @class      DIOSTREAMTLSSESSION
* @brief      Data Input/Output Stream TLS 1.3 role-neutral Session class
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

#include "DIOStreamTLSSession.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLSSESSION::DIOSTREAMTLSSESSION()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSSESSION::DIOSTREAMTLSSESSION() : keyexchangep256(CIPHERTYPE_ECDSA_SECP256R1)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLSSESSION::~DIOSTREAMTLSSESSION()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSSESSION::~DIOSTREAMTLSSESSION()
{
  End();
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::Ini(XWORD ciphersuite, DIOSTREAMTLSKEYSCHEDULE_ROLE role)
* @brief      Initialize a role-neutral TLS 1.3 session
* @ingroup    DATAIO
*
* @param[in]  ciphersuite : TLS 1.3 cipher suite.
* @param[in]  role : Role of this end.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::Ini(XWORD ciphersuite, DIOSTREAMTLSKEYSCHEDULE_ROLE role)
{
  End();

  if((role != DIOSTREAMTLSKEYSCHEDULE_ROLE_CLIENT) &&
     (role != DIOSTREAMTLSKEYSCHEDULE_ROLE_SERVER))
    {
      return false;
    }

  if(!keyschedule.Ini(ciphersuite, role))
    {
      return false;
    }

  if(!record.Ini(&keyschedule))
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
* @fn         void DIOSTREAMTLSSESSION::End()
* @brief      End the session and erase its transient state
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLSSESSION::End()
{
  record.End();
  keyschedule.End();
  KeyExchange_Delete();

  recordinput.Delete();
  handshakeinput.Delete();
  transcript.Delete();
  applicationinput.Delete();
  posthandshakeoutput.Delete();

  for(int c=0; c<DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS; c++)
    {
      epoch[c]      = DIOSTREAMTLSSESSION_EPOCH_CLEAR;
      keyupdates[c] = 0;
    }

  applicationsecretscalculated = false;
  closenotifysent              = false;
  closenotifyreceived          = false;
  iserror                      = false;
  transportclosedwithoutnotify = false;
  receivedalertlevel           = (DIOSTREAMTLS_ALERT_LEVEL)0;
  receivedalertdescription     = (DIOSTREAMTLS_ALERT_DESCRIPTION)0;
  isini                       = false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::IsIni()
* @brief      Check whether the session is initialized
* @ingroup    DATAIO
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::IsIni()
{
  return isini;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLSKEYSCHEDULE_ROLE DIOSTREAMTLSSESSION::GetRole()
* @brief      Get the role of this end of the session
* @ingroup    DATAIO
*
* @return     DIOSTREAMTLSKEYSCHEDULE_ROLE : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSKEYSCHEDULE_ROLE DIOSTREAMTLSSESSION::GetRole()
{
  return role;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLSSESSION_EPOCH DIOSTREAMTLSSESSION::GetEpoch(DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction)
* @brief      Get the traffic key epoch for one direction
* @ingroup    DATAIO
*
* @param[in]  direction : Local or remote traffic direction.
*
* @return     DIOSTREAMTLSSESSION_EPOCH : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSSESSION_EPOCH DIOSTREAMTLSSESSION::GetEpoch(DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction)
{
  if(direction >= DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS)
    {
      return DIOSTREAMTLSSESSION_EPOCH_CLEAR;
    }

  return epoch[direction];
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLSKEYSCHEDULE* DIOSTREAMTLSSESSION::GetKeySchedule()
* @brief      Get the key schedule
* @ingroup    DATAIO
*
* @return     DIOSTREAMTLSKEYSCHEDULE* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSKEYSCHEDULE* DIOSTREAMTLSSESSION::GetKeySchedule()
{
  return &keyschedule;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLSRECORD* DIOSTREAMTLSSESSION::GetRecord()
* @brief      Get the record layer
* @ingroup    DATAIO
*
* @return     DIOSTREAMTLSRECORD* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSRECORD* DIOSTREAMTLSSESSION::GetRecord()
{
  return &record;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         CIPHERECDSAX25519* DIOSTREAMTLSSESSION::GetKeyExchange()
* @brief      Get the X25519 key exchange object
* @ingroup    DATAIO
*
* @return     CIPHERECDSAX25519* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERECDSAX25519* DIOSTREAMTLSSESSION::GetKeyExchange()
{
  return &keyexchange;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::KeyExchange_Generate(XWORD group, XBUFFER& publickey)
* @brief      Generate a fresh ephemeral key pair for a supported TLS group
* @ingroup    DATAIO
*
* @param[in]  group : TLS supported group value.
* @param[out] publickey : Encoded public key share.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::KeyExchange_Generate(XWORD group, XBUFFER& publickey)
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

                                      default : return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::KeyExchange_SharedSecret(XWORD group, XBUFFER& publickey, XBUFFER& sharedsecret)
* @brief      Calculate the shared secret for a supported TLS group
* @ingroup    DATAIO
*
* @param[in]  group : Negotiated TLS supported group value.
* @param[in]  publickey : Encoded public key share of the remote end.
* @param[out] sharedsecret : Calculated shared secret.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::KeyExchange_SharedSecret(XWORD group, XBUFFER& publickey, XBUFFER& sharedsecret)
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

                                      default : return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLSSESSION::KeyExchange_Delete()
* @brief      Erase all ephemeral key exchange material
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLSSESSION::KeyExchange_Delete()
{
  keyexchange.CleanAllKeys();

  keyexchangep256private.FillBuffer(0);
  keyexchangep256private.Delete();
  keyexchangep256public.Delete();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::CipherSuite_Select(XWORD ciphersuite)
* @brief      Select an offered cipher suite before the handshake keys are derived
* @ingroup    DATAIO
*
* @param[in]  ciphersuite : Cipher suite selected by the remote end.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::CipherSuite_Select(XWORD ciphersuite)
{
  if(!isini ||
     (epoch[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL]  != DIOSTREAMTLSSESSION_EPOCH_CLEAR) ||
     (epoch[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE] != DIOSTREAMTLSSESSION_EPOCH_CLEAR))
    {
      return false;
    }

  if(keyschedule.GetCipherSuite() == ciphersuite) return true;

  record.End();

  if(!keyschedule.Ini(ciphersuite, role) || !record.Ini(&keyschedule))
    {
      keyschedule.End();
      isini = false;
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* DIOSTREAMTLSSESSION::GetRecordInput()
* @brief      Get the transport record accumulator
* @ingroup    DATAIO
*
* @return     XBUFFER* : Pointer to the requested buffer; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* DIOSTREAMTLSSESSION::GetRecordInput()
{
  return &recordinput;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::RecordInput_Add(XBYTE* data, XDWORD size)
* @brief      Add bytes to the transport record accumulator
* @ingroup    DATAIO
*
* @param[in]  data : Bytes to add.
* @param[in]  size : Number of bytes to add.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::RecordInput_Add(XBYTE* data, XDWORD size)
{
  if(!isini || (!data && size) ||
     (size > DIOSTREAMTLSSESSION_MAXRECORDINPUTSIZE) ||
     (recordinput.GetSize() > (DIOSTREAMTLSSESSION_MAXRECORDINPUTSIZE - size)))
    {
      return false;
    }

  return size?recordinput.Add(data, size):true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::RecordInput_Add(XBUFFER& data)
* @brief      Add a buffer to the transport record accumulator
* @ingroup    DATAIO
*
* @param[in]  data : Buffer to add.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::RecordInput_Add(XBUFFER& data)
{
  return RecordInput_Add(data.Get(), data.GetSize());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLSSESSION_RESULT DIOSTREAMTLSSESSION::Record_Extract(DIOSTREAMTLS_CONTENTTYPE& contenttype, XBUFFER& plain)
* @brief      Extract and decode one record from the transport accumulator
* @ingroup    DATAIO
*
* @param[out] contenttype : Actual type of the record content.
* @param[out] plain : Decoded record content.
*
* @return     DIOSTREAMTLSSESSION_RESULT : Complete, incomplete or error.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSSESSION_RESULT DIOSTREAMTLSSESSION::Record_Extract(DIOSTREAMTLS_CONTENTTYPE& contenttype, XBUFFER& plain)
{
  DIOSTREAMTLS_MSG_RECORDHEADER header;
  XBUFFER                       onerecord;

  plain.Delete();

  if(!isini)
    {
      return DIOSTREAMTLSSESSION_RESULT_ERROR;
    }

  if(recordinput.GetSize() < DIOSTREAMTLS_MSG_RECORDHEADER_SIZE)
    {
      return DIOSTREAMTLSSESSION_RESULT_INCOMPLETE;
    }

  if(!header.Peek(recordinput) || (header.GetLength() > DIOSTREAMTLSRECORD_MAXCIPHERSIZE))
    {
      return DIOSTREAMTLSSESSION_RESULT_ERROR;
    }

  if(recordinput.GetSize() < ((XDWORD)DIOSTREAMTLS_MSG_RECORDHEADER_SIZE + header.GetLength()))
    {
      return DIOSTREAMTLSSESSION_RESULT_INCOMPLETE;
    }

  if(!DIOSTREAMTLSRECORD::Record_Extract(recordinput, onerecord))
    {
      return DIOSTREAMTLSSESSION_RESULT_ERROR;
    }

  if(!record.Unprotect(onerecord, contenttype, plain))
    {
      return DIOSTREAMTLSSESSION_RESULT_ERROR;
    }

  return DIOSTREAMTLSSESSION_RESULT_COMPLETE;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* DIOSTREAMTLSSESSION::GetHandshakeInput()
* @brief      Get the handshake message accumulator
* @ingroup    DATAIO
*
* @return     XBUFFER* : Pointer to the requested buffer; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* DIOSTREAMTLSSESSION::GetHandshakeInput()
{
  return &handshakeinput;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::HandshakeInput_Add(XBYTE* data, XDWORD size)
* @brief      Add bytes to the handshake message accumulator
* @ingroup    DATAIO
*
* @param[in]  data : Bytes to add.
* @param[in]  size : Number of bytes to add.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::HandshakeInput_Add(XBYTE* data, XDWORD size)
{
  if(!isini || (!data && size) ||
     (size > DIOSTREAMTLSSESSION_MAXHANDSHAKESIZE) ||
     (handshakeinput.GetSize() > (DIOSTREAMTLSSESSION_MAXHANDSHAKESIZE - size)))
    {
      return false;
    }

  return size?handshakeinput.Add(data, size):true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::HandshakeInput_Add(XBUFFER& data)
* @brief      Add a buffer to the handshake message accumulator
* @ingroup    DATAIO
*
* @param[in]  data : Buffer to add.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::HandshakeInput_Add(XBUFFER& data)
{
  return HandshakeInput_Add(data.Get(), data.GetSize());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLSSESSION_RESULT DIOSTREAMTLSSESSION::Handshake_Extract(XBUFFER& message)
* @brief      Extract one complete handshake message from the handshake accumulator
* @ingroup    DATAIO
*
* @param[out] message : Complete handshake message, including its header.
*
* @return     DIOSTREAMTLSSESSION_RESULT : Complete, incomplete or error.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSSESSION_RESULT DIOSTREAMTLSSESSION::Handshake_Extract(XBUFFER& message)
{
  message.Delete();

  if(!isini)
    {
      return DIOSTREAMTLSSESSION_RESULT_ERROR;
    }

  if(handshakeinput.GetSize() < DIOSTREAMTLS_MSG_HANDSHAKEHEADER_SIZE)
    {
      return DIOSTREAMTLSSESSION_RESULT_INCOMPLETE;
    }

  XBYTE* data = handshakeinput.Get();
  if(!data)
    {
      return DIOSTREAMTLSSESSION_RESULT_ERROR;
    }

  XDWORD length        = ((XDWORD)data[1] << 16) | ((XDWORD)data[2] << 8) | (XDWORD)data[3];
  XDWORD messagelength = DIOSTREAMTLS_MSG_HANDSHAKEHEADER_SIZE + length;

  if(messagelength > DIOSTREAMTLSSESSION_MAXHANDSHAKESIZE)
    {
      return DIOSTREAMTLSSESSION_RESULT_ERROR;
    }

  if(handshakeinput.GetSize() < messagelength)
    {
      return DIOSTREAMTLSSESSION_RESULT_INCOMPLETE;
    }

  if(!DIOSTREAMTLS_MSG_HANDSHAKE::Message_Extract(handshakeinput, message))
    {
      return DIOSTREAMTLSSESSION_RESULT_ERROR;
    }

  return DIOSTREAMTLSSESSION_RESULT_COMPLETE;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* DIOSTREAMTLSSESSION::GetTranscript()
* @brief      Get the exact handshake transcript
* @ingroup    DATAIO
*
* @return     XBUFFER* : Pointer to the requested buffer; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* DIOSTREAMTLSSESSION::GetTranscript()
{
  return &transcript;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::Transcript_Add(XBUFFER& message)
* @brief      Add one exact handshake message to the transcript
* @ingroup    DATAIO
*
* @param[in]  message : Complete handshake message, including its header.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::Transcript_Add(XBUFFER& message)
{
  XBUFFER                   workbuffer;
  DIOSTREAMTLS_MSG_HANDSHAKE handshake;

  if(!isini || message.IsEmpty())
    {
      return false;
    }

  workbuffer.Add(message);

  if(!handshake.GetFromBuffer(workbuffer, false) || !workbuffer.IsEmpty())
    {
      return false;
    }

  return transcript.Add(message);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::TranscriptHash(XBUFFER& transcripthash)
* @brief      Calculate the current handshake transcript hash
* @ingroup    DATAIO
*
* @param[out] transcripthash : Calculated transcript hash.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::TranscriptHash(XBUFFER& transcripthash)
{
  if(!isini)
    {
      return false;
    }

  return keyschedule.TranscriptHash(transcript, transcripthash);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::HandshakeKeys_Activate(XBUFFER& sharedsecret)
* @brief      Derive and install both handshake traffic directions
* @ingroup    DATAIO
*
* @param[in]  sharedsecret : Shared secret produced by the negotiated key exchange.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::HandshakeKeys_Activate(XBUFFER& sharedsecret)
{
  XBUFFER transcripthash;

  if(!isini || (sharedsecret.GetSize() != CIPHERECDSAX25519_MAXKEY))
    {
      return false;
    }

  if((epoch[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL]  != DIOSTREAMTLSSESSION_EPOCH_CLEAR) ||
     (epoch[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE] != DIOSTREAMTLSSESSION_EPOCH_CLEAR))
    {
      return false;
    }

  if(!keyschedule.EarlySecret_Calculate())                         return false;
  if(!keyschedule.HandshakeSecret_Calculate(sharedsecret))         return false;
  if(!TranscriptHash(transcripthash))                              return false;
  if(!keyschedule.HandshakeTrafficSecrets_Calculate(transcripthash)) return false;
  if(!keyschedule.MasterSecret_Calculate())                        return false;

  if(!record.SetKeys(DIOSTREAMTLSKEYSCHEDULE_LEVEL_HANDSHAKE, DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL))  return false;
  if(!record.SetKeys(DIOSTREAMTLSKEYSCHEDULE_LEVEL_HANDSHAKE, DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE)) return false;

  epoch[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL]  = DIOSTREAMTLSSESSION_EPOCH_HANDSHAKE;
  epoch[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE] = DIOSTREAMTLSSESSION_EPOCH_HANDSHAKE;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::ApplicationTrafficSecrets_Calculate()
* @brief      Calculate both application traffic secrets from the current transcript
* @ingroup    DATAIO
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::ApplicationTrafficSecrets_Calculate()
{
  XBUFFER transcripthash;

  if(!isini)
    {
      return false;
    }

  if(applicationsecretscalculated)
    {
      return true;
    }

  if((epoch[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL]  != DIOSTREAMTLSSESSION_EPOCH_HANDSHAKE) ||
     (epoch[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE] != DIOSTREAMTLSSESSION_EPOCH_HANDSHAKE))
    {
      return false;
    }

  if(!TranscriptHash(transcripthash)) return false;
  if(!keyschedule.ApplicationTrafficSecrets_Calculate(transcripthash)) return false;

  applicationsecretscalculated = true;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::ApplicationKeys_Activate(DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction)
* @brief      Install the application traffic keys for one direction
* @ingroup    DATAIO
*
* @param[in]  direction : Local or remote traffic direction.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::ApplicationKeys_Activate(DIOSTREAMTLSKEYSCHEDULE_DIRECTION direction)
{
  if(!isini || (direction >= DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS) ||
     (epoch[direction] != DIOSTREAMTLSSESSION_EPOCH_HANDSHAKE))
    {
      return false;
    }

  if(!ApplicationTrafficSecrets_Calculate())
    {
      return false;
    }

  if(!record.SetKeys(DIOSTREAMTLSKEYSCHEDULE_LEVEL_APPLICATION, direction))
    {
      return false;
    }

  epoch[direction] = DIOSTREAMTLSSESSION_EPOCH_APPLICATION;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* DIOSTREAMTLSSESSION::GetApplicationInput()
* @brief      Get the decoded application data accumulator
* @ingroup    DATAIO
*
* @return     XBUFFER* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* DIOSTREAMTLSSESSION::GetApplicationInput()
{
  return &applicationinput;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::ApplicationData_Protect(XBYTE* data, XDWORD size, XBUFFER& records)
* @brief      Protect application data with the local application traffic keys
* @ingroup    DATAIO
*
* @param[in]  data : Application data pointer.
* @param[in]  size : Number of application bytes.
* @param[out] records : Buffer where the protected records are added.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::ApplicationData_Protect(XBYTE* data, XDWORD size, XBUFFER& records)
{
  if(!isini || (!data && size) || iserror || closenotifysent || closenotifyreceived ||
     (epoch[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL] != DIOSTREAMTLSSESSION_EPOCH_APPLICATION))
    {
      return false;
    }

  return record.Protect(DIOSTREAMTLS_MSG_CONTENTTYPE_APPLICATION_DATA, data, size, records);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::ApplicationData_Protect(XBUFFER& data, XBUFFER& records)
* @brief      Protect an application buffer with the local application traffic keys
* @ingroup    DATAIO
*
* @param[in]  data : Application data.
* @param[out] records : Buffer where the protected records are added.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::ApplicationData_Protect(XBUFFER& data, XBUFFER& records)
{
  return ApplicationData_Protect(data.Get(), data.GetSize(), records);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSTREAMTLSSESSION::ApplicationData_Read(XBYTE* data, XDWORD size)
* @brief      Read decoded application data
* @ingroup    DATAIO
*
* @param[out] data : Destination buffer.
* @param[in]  size : Maximum number of bytes to read.
*
* @return     XDWORD : Number of bytes read.
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOSTREAMTLSSESSION::ApplicationData_Read(XBYTE* data, XDWORD size)
{
  if(!data || !size) return 0;

  return applicationinput.Extract(data, 0, size);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLSSESSION_RESULT DIOSTREAMTLSSESSION::ApplicationData_Process()
* @brief      Process all complete records received after the handshake
* @note       NewSessionTicket is consumed, KeyUpdate is processed and other post-handshake messages are rejected.
* @ingroup    DATAIO
*
* @return     DIOSTREAMTLSSESSION_RESULT : Complete, incomplete or error.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLSSESSION_RESULT DIOSTREAMTLSSESSION::ApplicationData_Process()
{
  bool processed = false;

  if(!isini || iserror ||
     (epoch[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE] != DIOSTREAMTLSSESSION_EPOCH_APPLICATION))
    {
      return DIOSTREAMTLSSESSION_RESULT_ERROR;
    }

  while(true)
    {
      DIOSTREAMTLS_CONTENTTYPE   contenttype = (DIOSTREAMTLS_CONTENTTYPE)0;
      DIOSTREAMTLSSESSION_RESULT result;
      XBUFFER                    plain;

      result = Record_Extract(contenttype, plain);

      if(result == DIOSTREAMTLSSESSION_RESULT_INCOMPLETE)
        {
          return processed?DIOSTREAMTLSSESSION_RESULT_COMPLETE:DIOSTREAMTLSSESSION_RESULT_INCOMPLETE;
        }

      if(result == DIOSTREAMTLSSESSION_RESULT_ERROR)
        {
          iserror = true;
          return DIOSTREAMTLSSESSION_RESULT_ERROR;
        }

      processed = true;

      switch(contenttype)
        {
          case DIOSTREAMTLS_MSG_CONTENTTYPE_APPLICATION_DATA    : if(closenotifyreceived || (plain.GetSize() && !applicationinput.Add(plain)))
                                                                    {
                                                                      iserror = true;
                                                                      return DIOSTREAMTLSSESSION_RESULT_ERROR;
                                                                    }
                                                                  break;

          case DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE           : { if(!HandshakeInput_Add(plain))
                                                                      {
                                                                        iserror = true;
                                                                        return DIOSTREAMTLSSESSION_RESULT_ERROR;
                                                                      }

                                                                    while(true)
                                                                      {
                                                                        DIOSTREAMTLSSESSION_RESULT handshakeresult;
                                                                        DIOSTREAMTLS_MSG_HANDSHAKE handshake;
                                                                        XBUFFER                    message;
                                                                        XBUFFER                    workbuffer;

                                                                        handshakeresult = Handshake_Extract(message);
                                                                        if(handshakeresult == DIOSTREAMTLSSESSION_RESULT_INCOMPLETE) break;

                                                                        if(handshakeresult == DIOSTREAMTLSSESSION_RESULT_ERROR)
                                                                          {
                                                                            iserror = true;
                                                                            return DIOSTREAMTLSSESSION_RESULT_ERROR;
                                                                          }

                                                                        workbuffer.Add(message);
                                                                        if(!handshake.GetFromBuffer(workbuffer, false) || !workbuffer.IsEmpty())
                                                                          {
                                                                            iserror = true;
                                                                            return DIOSTREAMTLSSESSION_RESULT_ERROR;
                                                                          }

                                                                        switch(handshake.GetMsgType())
                                                                          {
                                                                            case DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_NEW_SESSION_TICKET : break;

                                                                            case DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_KEY_UPDATE         : if(!KeyUpdate_Process(handshake))
                                                                                                                                              {
                                                                                                                                                iserror = true;
                                                                                                                                                return DIOSTREAMTLSSESSION_RESULT_ERROR;
                                                                                                                                              }

                                                                                                                                            if(!handshakeinput.IsEmpty())
                                                                                                                                              {
                                                                                                                                                iserror = true;
                                                                                                                                                return DIOSTREAMTLSSESSION_RESULT_ERROR;
                                                                                                                                              }
                                                                                                                                            break;

                                                                                                                                  default : iserror = true;
                                                                                                                                            return DIOSTREAMTLSSESSION_RESULT_ERROR;
                                                                          }
                                                                      }
                                                                  }
                                                                  break;

          case DIOSTREAMTLS_MSG_CONTENTTYPE_ALERT               : { DIOSTREAMTLS_MSG_ALERT alert;

                                                                    if(!alert.GetFromBuffer(plain, false) || !plain.IsEmpty())
                                                                      {
                                                                        iserror = true;
                                                                        return DIOSTREAMTLSSESSION_RESULT_ERROR;
                                                                      }

                                                                    receivedalertlevel       = alert.GetLevel();
                                                                    receivedalertdescription = alert.GetDescription();

                                                                    if(receivedalertdescription == DIOSTREAMTLS_ALERT_DESCRIPTION_CLOSE_NOTIFY)
                                                                      {
                                                                        if(closenotifyreceived)
                                                                          {
                                                                            iserror = true;
                                                                            return DIOSTREAMTLSSESSION_RESULT_ERROR;
                                                                          }

                                                                        closenotifyreceived = true;
                                                                      }
                                                                     else
                                                                      {
                                                                        iserror = true;
                                                                        return DIOSTREAMTLSSESSION_RESULT_ERROR;
                                                                      }
                                                                  }
                                                                  break;

          case DIOSTREAMTLS_MSG_CONTENTTYPE_CHANGE_CIPHER_SPEC  : iserror = true;
                                                                  return DIOSTREAMTLSSESSION_RESULT_ERROR;

                                                        default : iserror = true;
                                                                  return DIOSTREAMTLSSESSION_RESULT_ERROR;
        }
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::KeyUpdate_Create(bool requestpeer, XBUFFER& records)
* @brief      Create a protected TLS 1.3 KeyUpdate and advance the local application traffic keys
* @ingroup    DATAIO
*
* @param[in]  requestpeer : Request the remote end to update its sending keys too.
* @param[out] records : Protected KeyUpdate records.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::KeyUpdate_Create(bool requestpeer, XBUFFER& records)
{
  DIOSTREAMTLS_MSG_HANDSHAKE keyupdate;
  XBUFFER                    message;

  records.Delete();

  if(!isini || iserror || closenotifysent || closenotifyreceived ||
     (epoch[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL] != DIOSTREAMTLSSESSION_EPOCH_APPLICATION) ||
     (keyupdates[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL] >= DIOSTREAMTLSSESSION_MAXKEYUPDATES))
    {
      return false;
    }

  keyupdate.SetMsgType(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_KEY_UPDATE);

  if(!keyupdate.GetBody()->Add((XBYTE)(requestpeer?1:0)) ||
     !keyupdate.SetToBuffer(message, false) ||
     !record.Protect(DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE, message, records) ||
     !keyschedule.UpdateTrafficSecret(DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL) ||
     !record.SetKeys(DIOSTREAMTLSKEYSCHEDULE_LEVEL_APPLICATION, DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL))
    {
      iserror = true;
      records.Delete();
      return false;
    }

  keyupdates[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL]++;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::PostHandshakeOutput_Extract(XBUFFER& records)
* @brief      Extract protected records generated while processing post-handshake messages
* @ingroup    DATAIO
*
* @param[out] records : Pending records.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::PostHandshakeOutput_Extract(XBUFFER& records)
{
  records.Delete();

  if(!isini) return false;
  if(posthandshakeoutput.IsEmpty()) return true;

  if(!records.Add(posthandshakeoutput)) return false;

  posthandshakeoutput.Delete();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::KeyUpdate_Process(DIOSTREAMTLS_MSG_HANDSHAKE& handshake)
* @brief      Validate a remote KeyUpdate, advance the reading keys and prepare an optional response
* @note       INTERNAL
* @ingroup    DATAIO
*
* @param[in]  handshake : Decoded KeyUpdate message.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::KeyUpdate_Process(DIOSTREAMTLS_MSG_HANDSHAKE& handshake)
{
  XBYTE requestupdate;

  if((handshake.GetMsgType() != DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_KEY_UPDATE) ||
     (handshake.GetBody()->GetSize() != 1) ||
     (keyupdates[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE] >= DIOSTREAMTLSSESSION_MAXKEYUPDATES))
    {
      return false;
    }

  requestupdate = handshake.GetBody()->GetByte(0);
  if(requestupdate > 1) return false;

  if(!keyschedule.UpdateTrafficSecret(DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE) ||
     !record.SetKeys(DIOSTREAMTLSKEYSCHEDULE_LEVEL_APPLICATION, DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE))
    {
      return false;
    }

  keyupdates[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_REMOTE]++;

  if(requestupdate)
    {
      XBUFFER records;

      if(!KeyUpdate_Create(false, records) || !posthandshakeoutput.Add(records)) return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::Alert_Create(DIOSTREAMTLS_ALERT_LEVEL level, DIOSTREAMTLS_ALERT_DESCRIPTION description, XBUFFER& records)
* @brief      Create a TLS alert protected with the current local traffic keys
* @ingroup    DATAIO
*
* @param[in]  level : Alert level.
* @param[in]  description : Alert description.
* @param[out] records : Encoded alert records.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::Alert_Create(DIOSTREAMTLS_ALERT_LEVEL level, DIOSTREAMTLS_ALERT_DESCRIPTION description, XBUFFER& records)
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
* @fn         bool DIOSTREAMTLSSESSION::CloseNotify_Create(XBUFFER& records)
* @brief      Create an idempotent close_notify alert
* @ingroup    DATAIO
*
* @param[out] records : Encoded close_notify record, or an empty buffer when it was already sent.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::CloseNotify_Create(XBUFFER& records)
{
  records.Delete();

  if(closenotifysent) return true;

  if(!isini ||
     (epoch[DIOSTREAMTLSKEYSCHEDULE_DIRECTION_LOCAL] != DIOSTREAMTLSSESSION_EPOCH_APPLICATION))
    {
      return false;
    }

  return Alert_Create(DIOSTREAMTLS_ALERT_LEVEL_WARNING, DIOSTREAMTLS_ALERT_DESCRIPTION_CLOSE_NOTIFY, records);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::IsCloseNotifySent()
* @brief      Check whether close_notify was sent
* @ingroup    DATAIO
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::IsCloseNotifySent()
{
  return closenotifysent;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::IsCloseNotifyReceived()
* @brief      Check whether close_notify was received
* @ingroup    DATAIO
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::IsCloseNotifyReceived()
{
  return closenotifyreceived;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::IsError()
* @brief      Check whether the application record stream is in error
* @ingroup    DATAIO
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::IsError()
{
  return iserror;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::IsTransportClosedWithoutNotify()
* @brief      Check whether the transport ended without close_notify
* @ingroup    DATAIO
*
* @return     bool : true if the condition is met; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::IsTransportClosedWithoutNotify()
{
  return transportclosedwithoutnotify;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS_ALERT_LEVEL DIOSTREAMTLSSESSION::GetReceivedAlertLevel()
* @brief      Get the last received alert level
* @ingroup    DATAIO
*
* @return     DIOSTREAMTLS_ALERT_LEVEL : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_ALERT_LEVEL DIOSTREAMTLSSESSION::GetReceivedAlertLevel()
{
  return receivedalertlevel;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS_ALERT_DESCRIPTION DIOSTREAMTLSSESSION::GetReceivedAlertDescription()
* @brief      Get the last received alert description
* @ingroup    DATAIO
*
* @return     DIOSTREAMTLS_ALERT_DESCRIPTION : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS_ALERT_DESCRIPTION DIOSTREAMTLSSESSION::GetReceivedAlertDescription()
{
  return receivedalertdescription;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLSSESSION::TransportClosed()
* @brief      Register the end of the underlying transport
* @ingroup    DATAIO
*
* @return     bool : true when close_notify had been received; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLSSESSION::TransportClosed()
{
  if(!isini) return false;

  if(closenotifyreceived) return true;

  transportclosedwithoutnotify = true;
  iserror                      = true;

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLSSESSION::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLSSESSION::Clean()
{
  role                         = DIOSTREAMTLSKEYSCHEDULE_ROLE_CLIENT;
  isini                        = false;
  applicationsecretscalculated = false;

  for(int c=0; c<DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS; c++)
    {
      epoch[c] = DIOSTREAMTLSSESSION_EPOCH_CLEAR;
    }

  recordinput.Delete();
  handshakeinput.Delete();
  transcript.Delete();
  applicationinput.Delete();
  posthandshakeoutput.Delete();

  KeyExchange_Delete();

  closenotifysent               = false;
  closenotifyreceived           = false;
  iserror                       = false;
  transportclosedwithoutnotify = false;
  receivedalertlevel            = (DIOSTREAMTLS_ALERT_LEVEL)0;
  receivedalertdescription      = (DIOSTREAMTLS_ALERT_DESCRIPTION)0;

  for(int c=0; c<DIOSTREAMTLSKEYSCHEDULE_MAXDIRECTIONS; c++)
    {
      keyupdates[c] = 0;
    }
}
