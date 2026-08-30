/**-------------------------------------------------------------------------------------------------------------------
*
* @file       HashHMAC.cpp
*
* @class      HASHHMAC
* @brief      Hash HMAC (Keyed-Hashing for Message Authentication, RFC 2104) class
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

#include "HashHMAC.h"

#include "XFactory.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         HASHHMAC::HASHHMAC(HASH* hash)
* @brief      Constructor of class
* @ingroup    CIPHER
*
* @param[in]  hash : Hash to use as the underlying compression function. It is not owned by this class, so it must
*                    remain valid while this object is in use.
*
* --------------------------------------------------------------------------------------------------------------------*/
HASHHMAC::HASHHMAC(HASH* hash) : HASH()
{
  Clean();

  SetHash(hash);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         HASHHMAC::~HASHHMAC()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    CIPHER
*
* --------------------------------------------------------------------------------------------------------------------*/
HASHHMAC::~HASHHMAC()
{
  key.SecureDelete();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         HASH* HASHHMAC::GetHash()
* @brief      Get hash
* @ingroup    CIPHER
*
* @return     HASH* : Pointer to the requested object; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
HASH* HASHHMAC::GetHash()
{
  return hash;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool HASHHMAC::SetHash(HASH* hash)
* @brief      Set hash
* @note       The hash is not owned by this class. Hashes without a defined block size (checksums and CRCs) are
*             rejected, because HMAC is not defined for them.
* @ingroup    CIPHER
*
* @param[in]  hash : Hash pointer to use.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool HASHHMAC::SetHash(HASH* hash)
{
  XDWORD blocksize = CalculateBlockSize(hash);

  if(!blocksize)
    {
      return false;
    }

  this->hash      = hash;
  this->blocksize = blocksize;

  this->type      = hash->GetType();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD HASHHMAC::GetBlockSize()
* @brief      Get block size of the underlying hash, in bytes
* @ingroup    CIPHER
*
* @return     XDWORD : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD HASHHMAC::GetBlockSize()
{
  return blocksize;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* HASHHMAC::GetKey()
* @brief      Get key
* @ingroup    CIPHER
*
* @return     XBUFFER* : Pointer to the requested buffer; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER* HASHHMAC::GetKey()
{
  return &key;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool HASHHMAC::SetKey(XBYTE* key, XDWORD size)
* @brief      Set key
* @note       A zero sized key is valid: RFC 2104 allows it and it is used by the HKDF extract step.
* @ingroup    CIPHER
*
* @param[in]  key : Key pointer to use.
* @param[in]  size : Size of the key, in bytes.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool HASHHMAC::SetKey(XBYTE* key, XDWORD size)
{
  if(!key && size)
    {
      return false;
    }

  this->key.SecureDelete();

  if(size)
    {
      return this->key.Add(key, size);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool HASHHMAC::SetKey(XBUFFER& key)
* @brief      Set key
* @ingroup    CIPHER
*
* @param[in]  key : Key buffer to use.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool HASHHMAC::SetKey(XBUFFER& key)
{
  return SetKey(key.Get(), key.GetSize());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool HASHHMAC::Do(XBYTE* input, XQWORD size)
* @brief      Execute the operation: HMAC(K, text) = H((K0 XOR opad) || H((K0 XOR ipad) || text))
* @note       Following the HASH convention, the digest is added to the result buffer. Call ResetResult() before
*             reusing the object.
* @note       The file oriented Do() overloads of HASH are deliberately not exposed, because they hash in chunks and
*             would produce one HMAC per chunk instead of a single one over the whole content.
* @ingroup    CIPHER
*
* @param[in]  input : Input pointer to use.
* @param[in]  size : Size value.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool HASHHMAC::Do(XBYTE* input, XQWORD size)
{
  if(!hash || !blocksize || !result)
    {
      return false;
    }

  if(!input && size)
    {
      return false;
    }

  XBUFFER normalizedkey;
  XBUFFER pad;

  if(!NormalizeKey(normalizedkey))
    {
      return false;
    }

  // ----- Inner hash: H((K0 XOR ipad) || text) ----------------------------------------------------------------------

  if(!pad.Resize(blocksize))
    {
      return false;
    }

  for(XDWORD c=0; c<blocksize; c++)
    {
      pad.Get()[c] = (XBYTE)(normalizedkey.Get()[c] ^ HASHHMAC_IPAD);
    }

  if(size)
    {
      pad.Add(input, (XDWORD)size);
    }

  hash->ResetResult();

  if(!hash->Do(pad.Get(), (XQWORD)pad.GetSize()))
    {
      return false;
    }

  // ----- Outer hash: H((K0 XOR opad) || innerdigest) ----------------------------------------------------------------

  pad.Delete();

  if(!pad.Resize(blocksize))
    {
      return false;
    }

  for(XDWORD c=0; c<blocksize; c++)
    {
      pad.Get()[c] = (XBYTE)(normalizedkey.Get()[c] ^ HASHHMAC_OPAD);
    }

  pad.Add(hash->GetResult());

  hash->ResetResult();

  if(!hash->Do(pad.Get(), (XQWORD)pad.GetSize()))
    {
      return false;
    }

  result->Add(hash->GetResult());

  hash->ResetResult();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool HASHHMAC::Do(XBUFFER& input)
* @brief      Execute the operation
* @ingroup    CIPHER
*
* @param[in]  input : Input buffer to use.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool HASHHMAC::Do(XBUFFER& input)
{
  return Do(input.Get(), (XQWORD)input.GetSize());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int HASHHMAC::GetDefaultSize()
* @brief      Get default size: the digest size of the underlying hash
* @ingroup    CIPHER
*
* @return     int : Requested value.
*
* --------------------------------------------------------------------------------------------------------------------*/
int HASHHMAC::GetDefaultSize()
{
  if(!hash)
    {
      return 0;
    }

  return hash->GetDefaultSize();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool HASHHMAC::NormalizeKey(XBUFFER& normalizedkey)
* @brief      Normalize key to exactly one block: hash it if it is longer than the block, then zero pad it
* @note       INTERNAL
* @ingroup    CIPHER
*
* @param[in]  normalizedkey : Buffer that receives the normalized key.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool HASHHMAC::NormalizeKey(XBUFFER& normalizedkey)
{
  if(!hash || !blocksize)
    {
      return false;
    }

  normalizedkey.Delete();

  if(key.GetSize() > blocksize)
    {
      hash->ResetResult();

      if(!hash->Do(key.Get(), (XQWORD)key.GetSize()))
        {
          return false;
        }

      normalizedkey.Add(hash->GetResult());

      hash->ResetResult();
    }
   else
    {
      if(key.GetSize())
        {
          normalizedkey.Add(key.Get(), key.GetSize());
        }
    }

  if(normalizedkey.GetSize() > blocksize)
    {
      return false;
    }

  XDWORD sizekey = normalizedkey.GetSize();

  if(!normalizedkey.Resize(blocksize))
    {
      return false;
    }

  for(XDWORD c=sizekey; c<blocksize; c++)
    {
      normalizedkey.Get()[c] = 0x00;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD HASHHMAC::CalculateBlockSize(HASH* hash)
* @brief      Calculate the block size, in bytes, of the compression function of a hash
* @note       INTERNAL
* @ingroup    CIPHER
*
* @param[in]  hash : Hash pointer to use.
*
* @return     XDWORD : Requested value. Zero if HMAC is not defined for this hash.
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD HASHHMAC::CalculateBlockSize(HASH* hash)
{
  if(!hash)
    {
      return 0;
    }

  XDWORD blocksize = 0;

  switch(hash->GetType())
    {
      case HASHTYPE_NONE      :
      case HASHTYPE_CKS16     :
      case HASHTYPE_CRC16     :
      case HASHTYPE_CRC32     :
                    default   : blocksize = 0;                            break;

      case HASHTYPE_MD2       : blocksize = HASHHMAC_BLOCKSIZE_16;        break;

      case HASHTYPE_MD4       :
      case HASHTYPE_MD5       :
      case HASHTYPE_SHA1      :
      case HASHTYPE_SHA224    :
      case HASHTYPE_SHA256    :
      case HASHTYPE_RIPEMD160 : blocksize = HASHHMAC_BLOCKSIZE_64;        break;

      case HASHTYPE_SHA384    :
      case HASHTYPE_SHA512    : blocksize = HASHHMAC_BLOCKSIZE_128;       break;
    }

  return blocksize;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void HASHHMAC::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    CIPHER
*
* --------------------------------------------------------------------------------------------------------------------*/
void HASHHMAC::Clean()
{
  hash        = NULL;
  blocksize   = 0;
}
