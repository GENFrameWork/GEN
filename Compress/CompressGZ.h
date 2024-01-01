/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       CompressGZ.h
* 
* @class      COMPRESSGZ
* @brief      Compress GZ class
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

#ifndef _COMPRESSGZ_H_
#define _COMPRESSGZ_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "CompressBase.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

#define COMPRESS_GZ_CHUNK          16384
#define COMPRESS_GZ_WINDOWBITS     15
#define COMPRESS_GZ_ENCODING       16

#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES

class XBUFFER;

class COMPRESS_GZ : public COMPRESSBASE
{
  public:
                            COMPRESS_GZ           ();
    virtual                ~COMPRESS_GZ           ();

    bool                    Compress              (XBYTE* source,XDWORD size,XBUFFER* buffer);
    bool                    Decompress            (XBYTE* source,XDWORD size,XBUFFER* buffer);

  private:

    void                    Clean                 ();

    int                     ZCompress             (XBYTE* target,XDWORD* targetsize,XBYTE* source,XDWORD sourcesize);
    int                     ZDecompress           (XBYTE* target,XDWORD* targetsize,XBYTE* source,XDWORD sourcesize);

};

#pragma endregion


#endif

