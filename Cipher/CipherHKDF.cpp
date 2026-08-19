/**-------------------------------------------------------------------------------------------------------------------
*
* @file       CipherHKDF.cpp
*
* @class      CIPHERHKDF
* @brief      Cipher HKDF (HMAC-based Extract-and-Expand Key Derivation Function, RFC 5869) class
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

#include "CipherHKDF.h"

#include "XFactory.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         CIPHERHKDF::CIPHERHKDF(HASH* hash)
* @brief      Constructor of class
* @ingroup    CIPHER
*
* @param[in]  hash : Hash to use as the underlying function. It is not owned by this class, so it must remain valid
*                    while this object is in use.
*
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERHKDF::CIPHERHKDF(HASH* hash)
{
  Clean();

  labelprefix = CIPHERHKDF_LABELPREFIX_TLS13;

  hashhmac    = GEN_NEW HASHHMAC();

  SetHash(hash);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         CIPHERHKDF::~CIPHERHKDF()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    CIPHER
*
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERHKDF::~CIPHERHKDF()
{
  if(hashhmac)
    {
      GEN_DELETE hashhmac;
      hashhmac = NULL;
    }

  labelprefix.Empty();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         HASH* CIPHERHKDF::GetHash()
* @brief      Get hash
* @ingroup    CIPHER
*
* @return     HASH* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
HASH* CIPHERHKDF::GetHash()
{
  return hash;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERHKDF::SetHash(HASH* hash)
* @brief      Set hash
* @note       The hash is not owned by this class.
* @ingroup    CIPHER
*
* @param[in]  hash : Hash pointer to use.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERHKDF::SetHash(HASH* hash)
{
  if(!hash || !hashhmac)
    {
      return false;
    }

  if(!hashhmac->SetHash(hash))
    {
      return false;
    }

  this->hash = hash;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD CIPHERHKDF::GetHashSize()
* @brief      Get the digest size, in bytes, of the underlying hash
* @ingroup    CIPHER
*
* @return     XDWORD : Requested value. Zero if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD CIPHERHKDF::GetHashSize()
{
  if(!hash)
    {
      return 0;
    }

  int size = hash->GetDefaultSize();

  if(size <= 0)
    {
      return 0;
    }

  return (XDWORD)size;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* CIPHERHKDF::GetLabelPrefix()
* @brief      Get label prefix used by ExpandLabel()
* @ingroup    CIPHER
*
* @return     XSTRING* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* CIPHERHKDF::GetLabelPrefix()
{
  return &labelprefix;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERHKDF::SetLabelPrefix(XCHAR* labelprefix)
* @brief      Set label prefix used by ExpandLabel()
* @note       It defaults to CIPHERHKDF_LABELPREFIX_TLS13. Other protocols built on top of HKDF-Expand-Label, such as
*             QUIC, use a different one.
* @ingroup    CIPHER
*
* @param[in]  labelprefix : Label prefix to use.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERHKDF::SetLabelPrefix(XCHAR* labelprefix)
{
  if(!labelprefix)
    {
      return false;
    }

  this->labelprefix = labelprefix;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERHKDF::Extract(XBUFFER& salt, XBUFFER& inputkeymaterial, XBUFFER& pseudorandomkey)
* @brief      HKDF-Extract(salt, IKM) = HMAC-Hash(key = salt, data = IKM)
* @note       An empty salt is replaced by a string of HashLen zero bytes (RFC 5869, section 2.2).
* @ingroup    CIPHER
*
* @param[in]  salt : Optional salt. It may be empty.
* @param[in]  inputkeymaterial : Input keying material. It may be empty.
* @param[in]  pseudorandomkey : Buffer that receives the pseudorandom key, of HashLen bytes.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERHKDF::Extract(XBUFFER& salt, XBUFFER& inputkeymaterial, XBUFFER& pseudorandomkey)
{
  if(!hashhmac)
    {
      return false;
    }

  XDWORD hashsize = GetHashSize();

  if(!hashsize)
    {
      return false;
    }

  pseudorandomkey.Delete();

  if(salt.GetSize())
    {
      if(!hashhmac->SetKey(salt))
        {
          return false;
        }
    }
   else
    {
      XBUFFER zerosalt;

      if(!zerosalt.Resize(hashsize))
        {
          return false;
        }

      memset(zerosalt.Get(), 0, hashsize);

      if(!hashhmac->SetKey(zerosalt))
        {
          return false;
        }
    }

  hashhmac->ResetResult();

  if(!hashhmac->Do(inputkeymaterial.Get(), (XQWORD)inputkeymaterial.GetSize()))
    {
      return false;
    }

  pseudorandomkey.Add(hashhmac->GetResult());

  hashhmac->ResetResult();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERHKDF::Expand(XBUFFER& pseudorandomkey, XBUFFER& info, XDWORD size, XBUFFER& outputkeymaterial)
* @brief      HKDF-Expand(PRK, info, L): T(i) = HMAC-Hash(PRK, T(i-1) || info || i), OKM = first L bytes of T(1)..T(N)
* @ingroup    CIPHER
*
* @param[in]  pseudorandomkey : Pseudorandom key, normally the output of Extract().
* @param[in]  info : Optional context and application specific information. It may be empty.
* @param[in]  size : Size, in bytes, of the requested output. Up to 255 * HashLen.
* @param[in]  outputkeymaterial : Buffer that receives the output keying material.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERHKDF::Expand(XBUFFER& pseudorandomkey, XBUFFER& info, XDWORD size, XBUFFER& outputkeymaterial)
{
  if(!hashhmac)
    {
      return false;
    }

  XDWORD hashsize = GetHashSize();

  if(!hashsize)
    {
      return false;
    }

  if(!size || (size > (CIPHERHKDF_MAXEXPANDBLOCKS * hashsize)))
    {
      return false;
    }

  outputkeymaterial.Delete();

  XBUFFER block;
  XBYTE   counter = 1;

  while(outputkeymaterial.GetSize() < size)
    {
      XBUFFER data;

      if(block.GetSize())
        {
          data.Add(block);
        }

      if(info.GetSize())
        {
          data.Add(info);
        }

      data.Add((XBYTE)counter);

      if(!hashhmac->SetKey(pseudorandomkey))
        {
          return false;
        }

      hashhmac->ResetResult();

      if(!hashhmac->Do(data.Get(), (XQWORD)data.GetSize()))
        {
          return false;
        }

      block.Delete();
      block.Add(hashhmac->GetResult());

      hashhmac->ResetResult();

      if(!block.GetSize())
        {
          return false;
        }

      outputkeymaterial.Add(block);

      counter++;
    }

  if(outputkeymaterial.GetSize() > size)
    {
      outputkeymaterial.Resize(size);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERHKDF::ExpandLabel(XBUFFER& secret, XCHAR* label, XBUFFER& context, XDWORD size, XBUFFER& outputkeymaterial)
* @brief      HKDF-Expand-Label(Secret, Label, Context, Length) as defined in RFC 8446, section 7.1
* @ingroup    CIPHER
*
* @param[in]  secret : Secret to expand.
* @param[in]  label : Label, without the prefix. The prefix is added by this method.
* @param[in]  context : Context. It may be empty.
* @param[in]  size : Size, in bytes, of the requested output.
* @param[in]  outputkeymaterial : Buffer that receives the output keying material.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERHKDF::ExpandLabel(XBUFFER& secret, XCHAR* label, XBUFFER& context, XDWORD size, XBUFFER& outputkeymaterial)
{
  XBUFFER hkdflabel;

  if(!MakeHKDFLabel(label, context, size, hkdflabel))
    {
      return false;
    }

  return Expand(secret, hkdflabel, size, outputkeymaterial);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERHKDF::DeriveSecret(XBUFFER& secret, XCHAR* label, XBUFFER& messages, XBUFFER& outputsecret)
* @brief      Derive-Secret(Secret, Label, Messages) as defined in RFC 8446, section 7.1
* @note       The transcript hash of the messages is calculated here. When it is already available, call ExpandLabel()
*             directly passing it as the context.
* @ingroup    CIPHER
*
* @param[in]  secret : Secret to derive from.
* @param[in]  label : Label, without the prefix.
* @param[in]  messages : Concatenation of the handshake messages. It may be empty.
* @param[in]  outputsecret : Buffer that receives the derived secret, of HashLen bytes.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERHKDF::DeriveSecret(XBUFFER& secret, XCHAR* label, XBUFFER& messages, XBUFFER& outputsecret)
{
  if(!hash)
    {
      return false;
    }

  XDWORD hashsize = GetHashSize();

  if(!hashsize)
    {
      return false;
    }

  XBUFFER transcripthash;
  XBYTE   emptymessages = 0;

  hash->ResetResult();

  if(!hash->Do(messages.GetSize()?messages.Get():&emptymessages, (XQWORD)messages.GetSize()))
    {
      return false;
    }

  transcripthash.Add(hash->GetResult());

  hash->ResetResult();

  if(transcripthash.GetSize() != hashsize)
    {
      return false;
    }

  return ExpandLabel(secret, label, transcripthash, hashsize, outputsecret);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERHKDF::MakeHKDFLabel(XCHAR* label, XBUFFER& context, XDWORD size, XBUFFER& hkdflabel)
* @brief      Build the HkdfLabel structure of RFC 8446, section 7.1
* @note       INTERNAL
* @ingroup    CIPHER
*
* @param[in]  label : Label, without the prefix.
* @param[in]  context : Context. It may be empty.
* @param[in]  size : Size, in bytes, of the requested output.
* @param[in]  hkdflabel : Buffer that receives the encoded structure.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERHKDF::MakeHKDFLabel(XCHAR* label, XBUFFER& context, XDWORD size, XBUFFER& hkdflabel)
{
  if(!label)
    {
      return false;
    }

  XSTRING fulllabel;
  XBUFFER bufferlabel;

  fulllabel  = labelprefix;
  fulllabel += label;

  if(fulllabel.IsEmpty())
    {
      return false;
    }

  fulllabel.ConvertToASCII(bufferlabel);

  // ConvertToASCII() adds a zero terminator that does not belong to the wire format

  if(bufferlabel.GetSize() > 1)
    {
      bufferlabel.Resize(bufferlabel.GetSize()-1);
    }

  if(bufferlabel.IsEmpty() || (bufferlabel.GetSize() > CIPHERHKDF_MAXLABELSIZE))
    {
      return false;
    }

  if(context.GetSize() > CIPHERHKDF_MAXCONTEXTSIZE)
    {
      return false;
    }

  hkdflabel.Delete();

  hkdflabel.Add((XWORD)size);
  hkdflabel.Add((XBYTE)bufferlabel.GetSize());
  hkdflabel.Add(bufferlabel);
  hkdflabel.Add((XBYTE)context.GetSize());

  if(context.GetSize())
    {
      hkdflabel.Add(context);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void CIPHERHKDF::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    CIPHER
*
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERHKDF::Clean()
{
  hash        = NULL;
  hashhmac    = NULL;
}
