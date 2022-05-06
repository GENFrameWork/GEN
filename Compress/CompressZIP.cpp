/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       CompressZIP.cpp
* 
* @class      COMPRESSZIP
* @brief      Compress ZIP class
* @ingroup    COMPRESS
* 
* @copyright  GEN Group. All rights reserved.
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include <zlib.h>

#include "XBuffer.h"

#include "CompressZIP.h"

#include "XMemory_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         COMPRESS_ZIP::COMPRESS_ZIP()
* @brief      Constructor
* @ingroup    COMPRESS
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
COMPRESS_ZIP::COMPRESS_ZIP() : COMPRESSBASE()
{
  Clean();

  SetType(COMPRESSBASE_TYPE_ZIP);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         COMPRESS_ZIP::~COMPRESS_ZIP()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    COMPRESS
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
COMPRESS_ZIP::~COMPRESS_ZIP()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool COMPRESS_ZIP::Compress(XBYTE* source,XDWORD size,XBUFFER* xbuffer)
* @brief      Compress
* @ingroup    COMPRESS
* 
* @param[in]  source : 
* @param[in]  size : 
* @param[in]  xbuffer : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool COMPRESS_ZIP::Compress(XBYTE* source,XDWORD size,XBUFFER* xbuffer)
{
  if(!source)  return false;
  if(!size)    return false;
  if(!xbuffer) return false;

  xbuffer->Delete();
  xbuffer->Resize(size);

  XDWORD csize = size;

  ZCompress(xbuffer->Get(), &csize, source, size);

  xbuffer->Resize(csize);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool COMPRESS_ZIP::Decompress(XBYTE* source,XDWORD size,XBUFFER* xbuffer)
* @brief      Decompress
* @ingroup    COMPRESS
* 
* @param[in]  source : 
* @param[in]  size : 
* @param[in]  xbuffer : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool COMPRESS_ZIP::Decompress(XBYTE* source,XDWORD size,XBUFFER* xbuffer)
{
  if(!source)  return false;
  if(!size)    return false;
  if(!xbuffer) return false;

  //xbuffer->Delete();
  //xbuffer->Resize(xbuffer->size);

  XDWORD csize = xbuffer->GetSize();

  ZDecompress(xbuffer->Get(), &csize, source, size);

  //xbuffer->Resize(csize);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int COMPRESS_ZIP::ZCompress(XBYTE* target,XDWORD* targetsize,XBYTE* source,XDWORD sourcesize)
* @brief      ZCompress
* @ingroup    COMPRESS
* 
* @param[in]  target : 
* @param[in]  targetsize : 
* @param[in]  source : 
* @param[in]  sourcesize : 
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int COMPRESS_ZIP::ZCompress(XBYTE* target,XDWORD* targetsize,XBYTE* source,XDWORD sourcesize)
{
  z_stream stream;
  int      err;
  int      nExtraChunks;
  uInt     destlen;

  stream.next_in  = (Bytef*)source;
  stream.avail_in = (uInt)sourcesize;
  destlen         = (uInt)*targetsize;

  if ((uLong)destlen != (uLong)*targetsize) return Z_BUF_ERROR;

  stream.zalloc = (alloc_func)0;
  stream.zfree = (free_func)0;
  stream.opaque = (voidpf)0;

  err = deflateInit(&stream, Z_DEFAULT_COMPRESSION);
  if (err != Z_OK) return err;

  nExtraChunks = 0;

  do{ stream.next_out = target;
      stream.avail_out = destlen;

      err = deflate(&stream, Z_FINISH);
      if(err==Z_STREAM_END) break;

      if (err != Z_OK)
        {
          deflateEnd(&stream);
          return err;
        }

      nExtraChunks += 1;

    }while(stream.avail_out == 0);

 *targetsize = stream.total_out;

 err = deflateEnd(&stream);
 if(err!= Z_OK) return err;

 return nExtraChunks ? Z_BUF_ERROR : Z_OK;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int COMPRESS_ZIP::ZDecompress(XBYTE* target,XDWORD* targetsize,XBYTE* source,XDWORD sourcesize)
* @brief      ZDecompress
* @ingroup    COMPRESS
* 
* @param[in]  target : 
* @param[in]  targetsize : 
* @param[in]  source : 
* @param[in]  sourcesize : 
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int COMPRESS_ZIP::ZDecompress(XBYTE* target,XDWORD* targetsize,XBYTE* source,XDWORD sourcesize)
{
  z_stream stream;
  int err;

  int nExtraChunks;
  uInt destlen;

  stream.next_in = (Bytef*)source;
  stream.avail_in = (uInt)sourcesize;

  /* Check for source > 64K on 16-bit machine: */
  if ((uLong)stream.avail_in != (uLong)sourcesize) return Z_BUF_ERROR;

  destlen = (uInt)*targetsize;
  if ((uLong)destlen != (uLong)*targetsize) return Z_BUF_ERROR;

  stream.zalloc = (alloc_func)0;
  stream.zfree = (free_func)0;

  err = inflateInit(&stream);
  if (err != Z_OK) return err;

  nExtraChunks = 0;

  do{ stream.next_out = target;
      stream.avail_out = destlen;
      err = inflate(&stream, Z_FINISH);
      if(err==Z_STREAM_END) break;

      if(err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0)) err = Z_TICKET_ERROR;

      if(err!=Z_BUF_ERROR)
        {
          inflateEnd(&stream);
          return err;
        }

      nExtraChunks += 1;

    } while(stream.avail_out == 0);

  *targetsize = stream.total_out;

  err = inflateEnd(&stream);
  if(err!=Z_OK) return err;

  return nExtraChunks ? Z_BUF_ERROR : Z_OK;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void COMPRESS_ZIP::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    COMPRESS
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void COMPRESS_ZIP::Clean()
{
  param1 = NULL;
}