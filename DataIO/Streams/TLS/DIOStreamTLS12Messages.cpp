/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOStreamTLS12Messages.cpp
*
* @class      DIOSTREAMTLS12MESSAGES
* @brief      Data Input/Output Stream TLS 1.2 (RFC 5246) Handshake Messages classes
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

#include "DIOStreamTLS12Messages.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12_MSG_SERVERHELLODONE::DIOSTREAMTLS12_MSG_SERVERHELLODONE()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12_MSG_SERVERHELLODONE::DIOSTREAMTLS12_MSG_SERVERHELLODONE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12_MSG_SERVERHELLODONE::~DIOSTREAMTLS12_MSG_SERVERHELLODONE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12_MSG_SERVERHELLODONE::~DIOSTREAMTLS12_MSG_SERVERHELLODONE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12_MSG_SERVERHELLODONE::SetToBuffer(XBUFFER& buffer, bool showdebug)
* @brief      Encode the (empty) ServerHelloDone body
* @ingroup    DATAIO
*
* @param[in]  buffer : Destination buffer.
* @param[in]  showdebug : Show debug information.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12_MSG_SERVERHELLODONE::SetToBuffer(XBUFFER& buffer, bool showdebug)
{
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12_MSG_SERVERHELLODONE::GetFromBuffer(XBUFFER& buffer, bool showdebug)
* @brief      Decode the (empty) ServerHelloDone body
* @ingroup    DATAIO
*
* @param[in]  buffer : Source buffer. Must be empty: ServerHelloDone carries no payload (RFC 5246 section 7.4.5).
* @param[in]  showdebug : Show debug information.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12_MSG_SERVERHELLODONE::GetFromBuffer(XBUFFER& buffer, bool showdebug)
{
  return buffer.IsEmpty();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLS12_MSG_SERVERHELLODONE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS12_MSG_SERVERHELLODONE::Clean()
{
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::~DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::~DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::GetCurveType()
* @brief      Get the ECCurveType byte read from the wire
* @ingroup    DATAIO
*
* @return     XBYTE : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::GetCurveType()
{
  return curvetype;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::GetNamedCurve()
* @brief      Get the negotiated named curve
* @ingroup    DATAIO
*
* @return     XWORD : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::GetNamedCurve()
{
  return namedcurve;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::SetNamedCurve(XWORD namedcurve)
* @brief      Set the named curve
* @ingroup    DATAIO
*
* @param[in]  namedcurve : TLS NamedCurve value.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::SetNamedCurve(XWORD namedcurve)
{
  this->namedcurve = namedcurve;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::GetPublicKey()
* @brief      Get the server's ECDHE public key (uncompressed EC point)
* @ingroup    DATAIO
*
* @return     XBUFFER* : Pointer to the requested buffer; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::GetPublicKey()
{
  return &publickey;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::GetSignatureAlgorithm()
* @brief      Get the SignatureAndHashAlgorithm used to sign the params
* @ingroup    DATAIO
*
* @return     XWORD : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::GetSignatureAlgorithm()
{
  return signaturealgorithm;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::SetSignatureAlgorithm(XWORD signaturealgorithm)
* @brief      Set the SignatureAndHashAlgorithm
* @ingroup    DATAIO
*
* @param[in]  signaturealgorithm : TLS SignatureScheme value (RFC 5246 section 7.4.1.4.1 encoding).
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::SetSignatureAlgorithm(XWORD signaturealgorithm)
{
  this->signaturealgorithm = signaturealgorithm;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::GetSignature()
* @brief      Get the encoded signature over client_random + server_random + params
* @ingroup    DATAIO
*
* @return     XBUFFER* : Pointer to the requested buffer; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::GetSignature()
{
  return &signature;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::GetSignedParams(XBUFFER& params)
* @brief      Rebuild the ServerECDHParams bytes (curve_type + named_curve + public key) covered by the signature
* @ingroup    DATAIO
*
* @param[out] params : Destination buffer; cleared first.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::GetSignedParams(XBUFFER& params)
{
  if(publickey.IsEmpty() || (publickey.GetSize() > 0xFF))
    {
      return false;
    }

  params.Delete();

  if(!params.Add(curvetype))                          return false;
  if(!params.Add(namedcurve))                          return false;
  if(!params.Add((XBYTE)publickey.GetSize()))          return false;

  return params.Add(publickey);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::SetToBuffer(XBUFFER& buffer, bool showdebug)
* @brief      Encode the ServerKeyExchange body (ECDHE, signed)
* @ingroup    DATAIO
*
* @param[in]  buffer : Destination buffer.
* @param[in]  showdebug : Show debug information.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::SetToBuffer(XBUFFER& buffer, bool showdebug)
{
  XBUFFER params;

  if(!GetSignedParams(params))                        return false;
  if(signature.IsEmpty() || (signature.GetSize() > 0xFFFF)) return false;

  if(!buffer.Add(params))                             return false;
  if(!buffer.Add(signaturealgorithm))                 return false;
  if(!buffer.Add((XWORD)signature.GetSize()))         return false;

  return buffer.Add(signature);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::GetFromBuffer(XBUFFER& buffer, bool showdebug)
* @brief      Decode an exact ServerKeyExchange body (ECDHE, signed)
* @ingroup    DATAIO
*
* @param[in]  buffer : Source buffer.
* @param[in]  showdebug : Show debug information.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::GetFromBuffer(XBUFFER& buffer, bool showdebug)
{
  XBUFFER workbuffer;
  XBUFFER newpublickey;
  XBUFFER newsignature;
  XBYTE   newcurvetype    = 0;
  XWORD   newnamedcurve   = 0;
  XBYTE   publickeysize   = 0;
  XWORD   newsigalgorithm = 0;
  XWORD   signaturesize   = 0;
  XDWORD  sizeconsumed    = buffer.GetSize();

  workbuffer.Add(buffer);

  if(!workbuffer.Extract(newcurvetype))                                             return false;
  if(newcurvetype != DIOSTREAMTLS12_MSG_ECCURVETYPE_NAMED_CURVE)                     return false;

  if(!workbuffer.Extract(newnamedcurve))                                            return false;

  if(!workbuffer.Extract(publickeysize))                                            return false;
  if(!publickeysize || (workbuffer.GetSize() < (XDWORD)(publickeysize + (XDWORD)sizeof(XWORD)))) return false;

  if(!newpublickey.Resize(publickeysize)) return false;
  if(workbuffer.Extract(newpublickey.Get(), 0, publickeysize) != publickeysize)      return false;

  if(!workbuffer.Extract(newsigalgorithm))                                          return false;
  if(!workbuffer.Extract(signaturesize))                                            return false;
  if(!signaturesize || (workbuffer.GetSize() != signaturesize))                      return false;

  if(!newsignature.Resize(signaturesize)) return false;
  if(workbuffer.Extract(newsignature.Get(), 0, signaturesize) != signaturesize)      return false;

  curvetype          = newcurvetype;
  namedcurve         = newnamedcurve;
  publickey.Delete();
  if(!publickey.Add(newpublickey))                                                  return false;
  signaturealgorithm = newsigalgorithm;
  signature.Delete();
  if(!signature.Add(newsignature))                                                  return false;

  return (buffer.Extract(NULL, 0, sizeconsumed) == sizeconsumed);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS12_MSG_SERVERKEYEXCHANGE_ECDHE::Clean()
{
  curvetype          = DIOSTREAMTLS12_MSG_ECCURVETYPE_NAMED_CURVE;
  namedcurve         = 0;
  publickey.Delete();
  signaturealgorithm = 0;
  signature.Delete();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE::DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE::DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE::~DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE::~DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE::GetPublicKey()
* @brief      Get the client's ECDHE public key (uncompressed EC point)
* @ingroup    DATAIO
*
* @return     XBUFFER* : Pointer to the requested buffer; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE::GetPublicKey()
{
  return &publickey;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE::SetPublicKey(XBUFFER& publickey)
* @brief      Set the client's ECDHE public key
* @ingroup    DATAIO
*
* @param[in]  publickey : Uncompressed EC point, as produced by DIOSTREAMTLS13SESSION::KeyExchange_Generate().
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE::SetPublicKey(XBUFFER& publickey)
{
  if(publickey.IsEmpty() || (publickey.GetSize() > 0xFF))
    {
      return false;
    }

  this->publickey.Delete();

  return this->publickey.Add(publickey);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE::SetToBuffer(XBUFFER& buffer, bool showdebug)
* @brief      Encode the ClientKeyExchange body (ECDHE)
* @ingroup    DATAIO
*
* @param[in]  buffer : Destination buffer.
* @param[in]  showdebug : Show debug information.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE::SetToBuffer(XBUFFER& buffer, bool showdebug)
{
  if(publickey.IsEmpty() || (publickey.GetSize() > 0xFF))
    {
      return false;
    }

  if(!buffer.Add((XBYTE)publickey.GetSize())) return false;

  return buffer.Add(publickey);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE::GetFromBuffer(XBUFFER& buffer, bool showdebug)
* @brief      Decode an exact ClientKeyExchange body (ECDHE)
* @ingroup    DATAIO
*
* @param[in]  buffer : Source buffer.
* @param[in]  showdebug : Show debug information.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE::GetFromBuffer(XBUFFER& buffer, bool showdebug)
{
  XBUFFER workbuffer;
  XBUFFER newpublickey;
  XBYTE   publickeysize = 0;
  XDWORD  sizeconsumed  = buffer.GetSize();

  workbuffer.Add(buffer);

  if(!workbuffer.Extract(publickeysize))                                    return false;
  if(!publickeysize || (workbuffer.GetSize() != publickeysize))             return false;

  if(!newpublickey.Resize(publickeysize)) return false;
  if(workbuffer.Extract(newpublickey.Get(), 0, publickeysize) != publickeysize) return false;

  publickey.Delete();
  if(!publickey.Add(newpublickey)) return false;

  return (buffer.Extract(NULL, 0, sizeconsumed) == sizeconsumed);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS12_MSG_CLIENTKEYEXCHANGE_ECDHE::Clean()
{
  publickey.Delete();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12_MSG_CERTIFICATE::DIOSTREAMTLS12_MSG_CERTIFICATE()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12_MSG_CERTIFICATE::DIOSTREAMTLS12_MSG_CERTIFICATE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAMTLS12_MSG_CERTIFICATE::~DIOSTREAMTLS12_MSG_CERTIFICATE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMTLS12_MSG_CERTIFICATE::~DIOSTREAMTLS12_MSG_CERTIFICATE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOSTREAMTLS12_MSG_CERTIFICATE::CertificateList_GetLength()
* @brief      Get the encoded certificate list length
* @ingroup    DATAIO
*
* @return     XDWORD : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOSTREAMTLS12_MSG_CERTIFICATE::CertificateList_GetLength()
{
  return certificatelistlength;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<XBUFFER*>* DIOSTREAMTLS12_MSG_CERTIFICATE::CertificateList_GetAll()
* @brief      Get every decoded certificate, leaf first, in raw DER encoding
* @ingroup    DATAIO
*
* @return     XVECTOR<XBUFFER*>* : Pointer to the certificate list.
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XBUFFER*>* DIOSTREAMTLS12_MSG_CERTIFICATE::CertificateList_GetAll()
{
  return &certificatelist;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12_MSG_CERTIFICATE::CertificateList_Add(XBUFFER* certificate)
* @brief      Add a DER certificate
* @ingroup    DATAIO
*
* @param[in]  certificate : Buffer whose ownership is transferred to the message.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12_MSG_CERTIFICATE::CertificateList_Add(XBUFFER* certificate)
{
  if(!certificate)
    {
      return false;
    }

  return certificatelist.Add(certificate);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12_MSG_CERTIFICATE::CertificateList_DeleteAll()
* @brief      Delete every certificate
* @ingroup    DATAIO
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12_MSG_CERTIFICATE::CertificateList_DeleteAll()
{
  certificatelist.DeleteContents();
  certificatelist.DeleteAll();
  certificatelistlength = 0;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12_MSG_CERTIFICATE::SetToBuffer(XBUFFER& buffer, bool showdebug)
* @brief      Encode the Certificate body (RFC 5246 section 7.4.2: plain DER list, no per-entry extensions)
* @ingroup    DATAIO
*
* @param[in]  buffer : Destination buffer.
* @param[in]  showdebug : Show debug information.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12_MSG_CERTIFICATE::SetToBuffer(XBUFFER& buffer, bool showdebug)
{
  XBUFFER certificatelistbuffer;

  for(XDWORD c=0; c<certificatelist.GetSize(); c++)
    {
      XBUFFER* certificate = certificatelist.Get(c);

      if(!certificate || certificate->IsEmpty() || (certificate->GetSize() > DIOSTREAMTLS_MSG_MAXLENGTH24))
        {
          return false;
        }

      if(!DIOSTREAMTLS_MSG_AddLength24(certificatelistbuffer, certificate->GetSize())) return false;
      if(!certificatelistbuffer.Add(*certificate))                                    return false;
    }

  if(certificatelistbuffer.GetSize() > DIOSTREAMTLS_MSG_MAXLENGTH24)
    {
      return false;
    }

  certificatelistlength = certificatelistbuffer.GetSize();

  if(!DIOSTREAMTLS_MSG_AddLength24(buffer, certificatelistlength)) return false;

  return certificatelistlength?buffer.Add(certificatelistbuffer):true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSTREAMTLS12_MSG_CERTIFICATE::GetFromBuffer(XBUFFER& buffer, bool showdebug)
* @brief      Decode an exact Certificate body
* @ingroup    DATAIO
*
* @param[in]  buffer : Source buffer.
* @param[in]  showdebug : Show debug information.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSTREAMTLS12_MSG_CERTIFICATE::GetFromBuffer(XBUFFER& buffer, bool showdebug)
{
  XBUFFER workbuffer;
  XBUFFER certificatelistbuffer;
  XDWORD  newcertificatelistlength = 0;
  XDWORD  sizeconsumed             = buffer.GetSize();

  workbuffer.Add(buffer);

  if(!DIOSTREAMTLS_MSG_ExtractLength24(workbuffer, newcertificatelistlength)) return false;
  if(workbuffer.GetSize() != newcertificatelistlength)                       return false;

  certificatelistbuffer.Add(workbuffer);
  CertificateList_DeleteAll();

  while(!certificatelistbuffer.IsEmpty())
    {
      XDWORD certificatelength = 0;

      if(!DIOSTREAMTLS_MSG_ExtractLength24(certificatelistbuffer, certificatelength))
        {
          CertificateList_DeleteAll();
          return false;
        }

      if(!certificatelength || (certificatelistbuffer.GetSize() < certificatelength))
        {
          CertificateList_DeleteAll();
          return false;
        }

      XBUFFER* certificate = GEN_NEW XBUFFER();
      if(!certificate)
        {
          CertificateList_DeleteAll();
          return false;
        }

      if(!certificate->Resize(certificatelength) ||
         (certificatelistbuffer.Extract(certificate->Get(), 0, certificatelength) != certificatelength) ||
         !CertificateList_Add(certificate))
        {
          GEN_DELETE certificate;
          CertificateList_DeleteAll();
          return false;
        }
    }

  certificatelistlength = newcertificatelistlength;

  return (buffer.Extract(NULL, 0, sizeconsumed) == sizeconsumed);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSTREAMTLS12_MSG_CERTIFICATE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSTREAMTLS12_MSG_CERTIFICATE::Clean()
{
  CertificateList_DeleteAll();
}
