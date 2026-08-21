/**-------------------------------------------------------------------------------------------------------------------
*
* @file       CompressDeflate.cpp
*
* @class      COMPRESS_DEFLATE
* @brief      Compress Deflate class
* @ingroup    COMPRESS
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

#include "CompressDeflate.h"

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

#include "XBuffer.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         COMPRESS_DEFLATE::COMPRESS_DEFLATE() : COMPRESSBASE()
* @brief      Constructor of class
* @ingroup    COMPRESS
*
* --------------------------------------------------------------------------------------------------------------------*/
COMPRESS_DEFLATE::COMPRESS_DEFLATE() : COMPRESSBASE()
{
  Clean();

  SetType(COMPRESSBASE_TYPE_DEFLATE);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         COMPRESS_DEFLATE::~COMPRESS_DEFLATE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    COMPRESS
*
* --------------------------------------------------------------------------------------------------------------------*/
COMPRESS_DEFLATE::~COMPRESS_DEFLATE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool COMPRESS_DEFLATE::Compress(XBYTE* source, XDWORD size, XBUFFER* xbuffer)
* @brief      Compress into a zlib-wrapped (RFC 1950) deflate stream
* @ingroup    COMPRESS
*
* @param[in]  source : Source pointer to use.
* @param[in]  size : Size value.
* @param[in]  xbuffer : Buffer to use.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool COMPRESS_DEFLATE::Compress(XBYTE* source, XDWORD size, XBUFFER* xbuffer)
{
  if(!source)  return false;
  if(!size)    return false;
  if(!xbuffer) return false;

  xbuffer->Delete();

  if(ZCompress(xbuffer, source, size) != Z_OK)
    {
      xbuffer->Delete();
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool COMPRESS_DEFLATE::Decompress(XBYTE* source, XDWORD size, XBUFFER* xbuffer)
* @brief      Decompress a "Content-Encoding: deflate" body: zlib-wrapped first, raw deflate as a fallback
* @ingroup    COMPRESS
*
* @param[in]  source : Source pointer to use.
* @param[in]  size : Size value.
* @param[in]  xbuffer : Buffer to use.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool COMPRESS_DEFLATE::Decompress(XBYTE* source, XDWORD size, XBUFFER* xbuffer)
{
  if(!source)  return false;
  if(!size)    return false;
  if(!xbuffer) return false;

  xbuffer->Delete();

  if(ZDecompress(xbuffer, source, size, COMPRESS_DEFLATE_WINDOWBITS) == Z_OK) return true;

  // Not a valid zlib-wrapped stream: a number of servers send raw (headerless) deflate for "Content-Encoding:
  // deflate" regardless of the RFC. Retry once, this time telling zlib to expect raw DEFLATE (negative windowBits).
  xbuffer->Delete();

  if(ZDecompress(xbuffer, source, size, -COMPRESS_DEFLATE_WINDOWBITS) == Z_OK) return true;

  xbuffer->Delete();

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int COMPRESS_DEFLATE::ZCompress(XBUFFER* target, XBYTE* source, XDWORD sourcesize)
* @brief      Z compress
* @note       INTERNAL. Streams the deflate output into target in COMPRESS_DEFLATE_CHUNK-sized blocks via
*             XBUFFER::Add() until Z_STREAM_END, so target grows to whatever size is actually needed.
* @ingroup    COMPRESS
*
* @param[out] target : Buffer that receives the deflate-encoded data (appended to; caller clears it first).
* @param[in]  source : Source pointer to use.
* @param[in]  sourcesize : Sourcesize value.
*
* @return     int : Z_OK if the operation is successful; a zlib error code otherwise.
*
* --------------------------------------------------------------------------------------------------------------------*/
int COMPRESS_DEFLATE::ZCompress(XBUFFER* target, XBYTE* source, XDWORD sourcesize)
{
  z_stream  stream;
  XBYTE     chunk[COMPRESS_DEFLATE_CHUNK];
  int       result;

  stream.zalloc = Z_NULL;
  stream.zfree  = Z_NULL;
  stream.opaque = Z_NULL;

  if(deflateInit2(&stream, -1, Z_DEFLATED, COMPRESS_DEFLATE_WINDOWBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK)
    {
      return Z_BUF_ERROR;
    }

  stream.next_in  = (Bytef*)source;
  stream.avail_in = (uInt)sourcesize;

  do{ stream.avail_out = sizeof(chunk);
      stream.next_out  = chunk;

      result = deflate(&stream, Z_FINISH);
      if(result == Z_STREAM_ERROR)
        {
          deflateEnd(&stream);
          return Z_STREAM_ERROR;
        }

      XDWORD have = sizeof(chunk) - stream.avail_out;

      if(have && !target->Add(chunk, have))
        {
          deflateEnd(&stream);
          return Z_MEM_ERROR;
        }

    } while(result != Z_STREAM_END);

  if(deflateEnd(&stream) != Z_OK) return Z_BUF_ERROR;

  return Z_OK;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int COMPRESS_DEFLATE::ZDecompress(XBUFFER* target, XBYTE* source, XDWORD sourcesize, int windowbits)
* @brief      Z decompress
* @note       INTERNAL. windowbits selects the expected wire format: COMPRESS_DEFLATE_WINDOWBITS (15) for a
*             zlib-wrapped stream, -COMPRESS_DEFLATE_WINDOWBITS for raw headerless deflate. Streams the inflated
*             output into target in COMPRESS_DEFLATE_CHUNK-sized blocks via XBUFFER::Add().
* @ingroup    COMPRESS
*
* @param[out] target : Buffer that receives the inflated data (appended to; caller clears it first).
* @param[in]  source : Source pointer to use.
* @param[in]  sourcesize : Sourcesize value.
* @param[in]  windowbits : zlib windowBits to pass to inflateInit2 (see note).
*
* @return     int : Z_OK if the operation is successful; a zlib error code otherwise.
*
* --------------------------------------------------------------------------------------------------------------------*/
int COMPRESS_DEFLATE::ZDecompress(XBUFFER* target, XBYTE* source, XDWORD sourcesize, int windowbits)
{
  z_stream  stream;
  XBYTE     chunk[COMPRESS_DEFLATE_CHUNK];
  int       result;

  stream.zalloc   = Z_NULL;
  stream.zfree    = Z_NULL;
  stream.opaque   = Z_NULL;
  stream.avail_in = 0;
  stream.next_in  = Z_NULL;

  if(inflateInit2(&stream, windowbits) != Z_OK) return Z_DATA_ERROR;

  stream.next_in  = (Bytef*)source;
  stream.avail_in = (uInt)sourcesize;

  do{ stream.avail_out = sizeof(chunk);
      stream.next_out  = chunk;

      result = inflate(&stream, Z_NO_FLUSH);

      switch(result)
        {
          case Z_ERRNO          :
          case Z_STREAM_ERROR   :
          case Z_BUF_ERROR      :
          case Z_VERSION_ERROR  :
          case Z_NEED_DICT      :
          case Z_DATA_ERROR     :
          case Z_MEM_ERROR      : inflateEnd(&stream);
                                  return Z_DATA_ERROR;
        }

      XDWORD have = sizeof(chunk) - stream.avail_out;

      if(have && !target->Add(chunk, have))
        {
          inflateEnd(&stream);
          return Z_MEM_ERROR;
        }

    } while(result != Z_STREAM_END);

  if(inflateEnd(&stream) != Z_OK) return Z_BUF_ERROR;

  return Z_OK;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void COMPRESS_DEFLATE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    COMPRESS
*
* --------------------------------------------------------------------------------------------------------------------*/
void COMPRESS_DEFLATE::Clean()
{
  param1 = NULL;
}
