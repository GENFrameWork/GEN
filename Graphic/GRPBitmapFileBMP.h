/**-------------------------------------------------------------------------------------------------------------------
*
* @file       GRPBitmapFileBMP.h
*
* @class      GRPBITMAPFILEBMP
* @brief      Graphic bitmap file BMP (Windows bitmap Format) class
* @ingroup    GRAPHIC
*
* @copyright  GEN Group. All right reserved.
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
*---------------------------------------------------------------------------------------------------------------------*/

#ifndef _GRPBITMAPFILEBMP_H_
#define _GRPBITMAPFILEBMP_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XBase.h"
#include "XFile.h"
#include "XPath.h"
#include "XMap.h"

#include "GRPProperties.h"
#include "GRP2DColor.h"
#include "GRPBitmapFile.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


enum GRPBITMAPFILEBMPCOMPRESSION
{
  GRPBITMAPFILEBMPCOMPRESSION_NOT   = 0 ,
  GRPBITMAPFILEBMPCOMPRESSION_8RLE      ,
  GRPBITMAPFILEBMPCOMPRESSION_4RLE      ,
  GRPBITMAPFILEBMPCOMPRESSION_RGB
};


#pragma pack(push, r1, 1)
typedef struct
{
    XWORD                               type;             // Signature - 'BM'
    XDWORD                              size;             // File size in bytes
    XWORD                               reserved1;        // 0
    XWORD                               reserved2;        // 0
    XDWORD                              offsetdata;       // Offset to data of bitmap

    XDWORD                              headersize;       // size of this struct (40)
    XDWORD                              width;            // bmap width in pixels
    XDWORD                              height;           // bmap height in pixels

    XWORD                               planes;           // num planes - always 1
    XWORD                               bitsperpixel;     // bits per pixel

    XDWORD                              compression;      // compression flag
    XDWORD                              sizebmp;          // image size in bytes
    XDWORD                              xpixelspermeter;  // horz resolution
    XDWORD                              ypixelspermeter;  // vert resolution
    XDWORD                              colorused;        // 0 -> color table size
    XDWORD                              colorimportant;   // important color count

} GRPBITMAPFILEBMPHEADERINFO;

#pragma pack(pop, r1)

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class GRPBITMAPFILEBMPCOLORPALETE
{
  public:
                                                GRPBITMAPFILEBMPCOLORPALETE       ();
                                                GRPBITMAPFILEBMPCOLORPALETE       (XBYTE red, XBYTE green, XBYTE blue, XBYTE alpha);

    virtual                                    ~GRPBITMAPFILEBMPCOLORPALETE       ();

    XBYTE                                       red;
    XBYTE                                       green;
    XBYTE                                       blue;
    XBYTE                                       alpha;

  private:

    void                                        Clean                             ();
};



class GRPBITMAPFILEBMPHEADER
{
  public:
                                                GRPBITMAPFILEBMPHEADER            ();
    virtual                                    ~GRPBITMAPFILEBMPHEADER            ();

    bool                                        Load                              (XFILE* file);
    bool                                        Load                              (XBYTE* buffer, XDWORD size, XDWORD& indexbuffer);

    GRPBITMAPFILEBMPHEADERINFO*                 GetHeaderInfo                     ();

    int                                         GetWidth                          ();
    int                                         GetHeight                         ();
    int                                         GetBitsPerPixel                   ();
    int                                         GetOffsetData                     ();

    GRPPROPERTYMODE                             GetMode                           ();

    bool                                        GetColorPalette                   (XBYTE index, GRP2DCOLOR_RGBA8& color);
    bool                                        DeleteAllPalette                  ();

  private:

    void                                        Clean                             ();

    GRPPROPERTYMODE                             mode;
    GRPBITMAPFILEBMPHEADERINFO                  headerinfo;
    XMAP<XBYTE, GRPBITMAPFILEBMPCOLORPALETE*>   palette;
};



class GRPBITMAPFILEBMP : public GRPBITMAPFILEBASE
{
  public:
                                                GRPBITMAPFILEBMP                  ();
    virtual                                    ~GRPBITMAPFILEBMP                  ();

    GRPBITMAP*                                  CreateBitmapFromFile              (XPATH& xpath, GRPPROPERTYMODE forcemode = GRPPROPERTYMODE_XX_UNKNOWN);
    GRPBITMAP*                                  CreateBitmapFromBuffer            (XBYTE* buffer, XDWORD size, GRPPROPERTYMODE forcemode = GRPPROPERTYMODE_XX_UNKNOWN);

    bool                                        CreateFileFromBitmap              (XPATH& xpath, GRPBITMAP* bitmap, int quality = 100);

  private:

    void                                        Clean                             ();

    bool                                        ReadHeaderFile                    (GRPBITMAPFILEBMPHEADER* header);
    bool                                        ReadHeaderBuffer                  (XBYTE* buffer, XDWORD size, GRPBITMAPFILEBMPHEADER* header);

    XDWORD                                      indexbuffer;
};



/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif


