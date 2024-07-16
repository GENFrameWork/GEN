/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPBitmapFileTGA.cpp
* 
* @class      GRPBITMAPFILETGA
* @brief      Graphic bitmap file TGA / TARGA format class (TGA  : Truevision Graphics Adapter)
* @brief                                                   (TARGA: Truevision Advanced Raster Graphics Adapter)

* @ingroup    GRAPHIC
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "GRPBitmapFileTGA.h"

#include "GRPFactory.h"
#include "GRPBitmap.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


#pragma region CLASS_GRPBITMAPFILETGAHEADER


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPBITMAPFILETGAHEADER::GRPBITMAPFILETGAHEADER()
* @brief      Constructor
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPBITMAPFILETGAHEADER::GRPBITMAPFILETGAHEADER()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPBITMAPFILETGAHEADER::~GRPBITMAPFILETGAHEADER()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPBITMAPFILETGAHEADER::~GRPBITMAPFILETGAHEADER()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPBITMAPFILETGAHEADER::Load(XFILE* file)
* @brief      Load
* @ingroup    GRAPHIC
*
* @param[in]  file : file handle to load bitmap
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPBITMAPFILETGAHEADER::Load(XFILE* file)
{
  if(!file->Read((XBYTE*)&headerinfo.IDlength         , sizeof(XBYTE))) return false;
  if(!file->Read((XBYTE*)&headerinfo.colourmaptype    , sizeof(XBYTE))) return false;
  if(!file->Read((XBYTE*)&headerinfo.datatypecode     , sizeof(XBYTE))) return false;
  if(!file->Read((XBYTE*)&headerinfo.colourmaporigin  , sizeof(XWORD))) return false;
  if(!file->Read((XBYTE*)&headerinfo.colourmaplength  , sizeof(XWORD))) return false;
  if(!file->Read((XBYTE*)&headerinfo.colourmapdepth   , sizeof(XBYTE))) return false;
  if(!file->Read((XBYTE*)&headerinfo.xorigin          , sizeof(XWORD))) return false;
  if(!file->Read((XBYTE*)&headerinfo.yorigin          , sizeof(XWORD))) return false;
  if(!file->Read((XBYTE*)&headerinfo.width            , sizeof(XWORD))) return false;
  if(!file->Read((XBYTE*)&headerinfo.height           , sizeof(XWORD))) return false;
  if(!file->Read((XBYTE*)&headerinfo.bitsperpixel     , sizeof(XBYTE))) return false;
  if(!file->Read((XBYTE*)&headerinfo.imagedescriptor  , sizeof(XBYTE))) return false;

  // Only RGB format valid.
  if((headerinfo.datatypecode != GRPBITMAPFILETGATYPE_UNCOMPRESS_RGB) &&
     (headerinfo.datatypecode != GRPBITMAPFILETGATYPE_RLECOMPRESS_RGB))  return false;

  // Only 16,24,32 bits format valid.
  if((headerinfo.bitsperpixel != 16) &&
     (headerinfo.bitsperpixel != 24) &&
     (headerinfo.bitsperpixel != 32)) return false;

  // Only 0,1 colour map types
  if((headerinfo.colourmaptype != 0) && (headerinfo.colourmaptype != 1)) return false;

  switch(headerinfo.bitsperpixel)
    {
      case 16 : mode = GRPPROPERTYMODE_16_RGBA_5551;  break;
      case 24 : mode = GRPPROPERTYMODE_24_RGB_888;    break;
      case 32 : mode = GRPPROPERTYMODE_32_RGBA_8888;  break;
    }

  offsetdata += sizeof(GRPBITMAPFILETGAHEADERINFO);
  offsetdata += headerinfo.IDlength;
  offsetdata += headerinfo.colourmaptype * headerinfo.colourmaplength;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPBITMAPFILETGAHEADER::Load(XBYTE* buffer, XDWORD size, XDWORD& indexbuffer)
* @brief      Load
* @ingroup    GRAPHIC
*
* @param[in]  buffer :
* @param[in]  size :
* @param[in]  indexbuffer :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPBITMAPFILETGAHEADER::Load(XBYTE* buffer, XDWORD size, XDWORD& indexbuffer)
{
  memcpy((XBYTE*)&headerinfo.IDlength         , &buffer[indexbuffer]   , sizeof(XBYTE));     indexbuffer += sizeof(XBYTE);
  memcpy((XBYTE*)&headerinfo.colourmaptype    , &buffer[indexbuffer]   , sizeof(XBYTE));     indexbuffer += sizeof(XBYTE);
  memcpy((XBYTE*)&headerinfo.datatypecode     , &buffer[indexbuffer]   , sizeof(XBYTE));     indexbuffer += sizeof(XBYTE);
  memcpy((XBYTE*)&headerinfo.colourmaporigin  , &buffer[indexbuffer]   , sizeof(XWORD));     indexbuffer += sizeof(XWORD);
  memcpy((XBYTE*)&headerinfo.colourmaplength  , &buffer[indexbuffer]   , sizeof(XWORD));     indexbuffer += sizeof(XWORD);
  memcpy((XBYTE*)&headerinfo.colourmapdepth   , &buffer[indexbuffer]   , sizeof(XBYTE));     indexbuffer += sizeof(XBYTE);
  memcpy((XBYTE*)&headerinfo.xorigin          , &buffer[indexbuffer]   , sizeof(XWORD));     indexbuffer += sizeof(XWORD);
  memcpy((XBYTE*)&headerinfo.yorigin          , &buffer[indexbuffer]   , sizeof(XWORD));     indexbuffer += sizeof(XWORD);
  memcpy((XBYTE*)&headerinfo.width            , &buffer[indexbuffer]   , sizeof(XWORD));     indexbuffer += sizeof(XWORD);
  memcpy((XBYTE*)&headerinfo.height           , &buffer[indexbuffer]   , sizeof(XWORD));     indexbuffer += sizeof(XWORD);
  memcpy((XBYTE*)&headerinfo.bitsperpixel     , &buffer[indexbuffer]   , sizeof(XBYTE));     indexbuffer += sizeof(XBYTE);
  memcpy((XBYTE*)&headerinfo.imagedescriptor  , &buffer[indexbuffer]   , sizeof(XBYTE));     indexbuffer += sizeof(XBYTE);

  // Only RGB format valid.
  if((headerinfo.datatypecode != GRPBITMAPFILETGATYPE_UNCOMPRESS_RGB) &&
     (headerinfo.datatypecode != GRPBITMAPFILETGATYPE_RLECOMPRESS_RGB))  return false;

  // Only 16,24,32 bits format valid.
  if((headerinfo.bitsperpixel != 16) &&
     (headerinfo.bitsperpixel != 24) &&
     (headerinfo.bitsperpixel != 32)) return false;

  // Only 0,1 colour map types
  if((headerinfo.colourmaptype != 0) && (headerinfo.colourmaptype != 1)) return false;

  switch(headerinfo.bitsperpixel)
    {
      case 16 : mode = GRPPROPERTYMODE_16_RGBA_5551;  break;
      case 24 : mode = GRPPROPERTYMODE_24_RGB_888;    break;
      case 32 : mode = GRPPROPERTYMODE_32_RGBA_8888;  break;
    }

  offsetdata += sizeof(GRPBITMAPFILETGAHEADERINFO);
  offsetdata += headerinfo.IDlength;
  offsetdata += headerinfo.colourmaptype * headerinfo.colourmaplength;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPBITMAPFILETGAHEADERINFO* GRPBITMAPFILETGAHEADER::GetHeaderInfo()
* @brief      Get Header Info
* @ingroup    GRAPHIC
*
* @return     GRPBITMAPFILETGAHEADERINFO* :
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPBITMAPFILETGAHEADERINFO* GRPBITMAPFILETGAHEADER::GetHeaderInfo()
{
  return &headerinfo;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int GRPBITMAPFILETGAHEADER::GetWidth()
* @brief      Get Width
* @ingroup    GRAPHIC
*
* @return     int : Width of TGA
*
* --------------------------------------------------------------------------------------------------------------------*/
int GRPBITMAPFILETGAHEADER::GetWidth()
{
  return headerinfo.width;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int GRPBITMAPFILETGAHEADER::GetHeight()
* @brief      GetHeight
* @ingroup    GRAPHIC
*
* @return     int : Height if TGA
*
* --------------------------------------------------------------------------------------------------------------------*/
int GRPBITMAPFILETGAHEADER::GetHeight()
{
  return headerinfo.height;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int GRPBITMAPFILETGAHEADER::GetBitsPerPixel()
* @brief      Get Bits Per Pixel
* @ingroup    GRAPHIC
*
* @return     int : Bits per pixel
*
* --------------------------------------------------------------------------------------------------------------------*/
int GRPBITMAPFILETGAHEADER::GetBitsPerPixel()
{
  return headerinfo.bitsperpixel;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int GRPBITMAPFILETGAHEADER::GetOffsetData()
* @brief      Get Offset Data
* @ingroup    GRAPHIC
*
* @return     int : Offset data
*
* --------------------------------------------------------------------------------------------------------------------*/
int GRPBITMAPFILETGAHEADER::GetOffsetData()
{
  return offsetdata;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPPROPERTYMODE GRPBITMAPFILETGAHEADER::GetMode()
* @brief      Get Mode
* @ingroup    GRAPHIC
*
* @return     GRPPROPERTYMODE : property mode
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPPROPERTYMODE GRPBITMAPFILETGAHEADER::GetMode()
{
  return mode;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPBITMAPFILETGATYPE GRPBITMAPFILETGAHEADER::GetType()
* @brief      Get Type
* @ingroup    GRAPHIC
*
* @return     GRPBITMAPFILETGATYPE : type of TGA
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPBITMAPFILETGATYPE GRPBITMAPFILETGAHEADER::GetType()
{
  return (GRPBITMAPFILETGATYPE)headerinfo.datatypecode;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRPBITMAPFILETGAHEADER::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
void GRPBITMAPFILETGAHEADER::Clean()
{
  mode = GRPPROPERTYMODE_XX_UNKNOWN;

  memset((XBYTE*)&headerinfo, 0 , sizeof(GRPBITMAPFILETGAHEADERINFO));

  offsetdata = 0;
}


#pragma endregion


#pragma region CLASS_GRPBITMAPFILETGA


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPBITMAPFILETGA::GRPBITMAPFILETGA()
* @brief      Constructor
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPBITMAPFILETGA::GRPBITMAPFILETGA(): GRPBITMAPFILEBASE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPBITMAPFILETGA::~GRPBITMAPFILETGA()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPBITMAPFILETGA::~GRPBITMAPFILETGA()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPBITMAP* GRPBITMAPFILETGA::CreateBitmapFromFile(XPATH& xpath, GRPPROPERTYMODE forcemode)
* @brief      CreateBitmapFromFile
* @ingroup    GRAPHIC
*
* @param[in]  xpath :
* @param[in]  forcemode :
*
* @return     GRPBITMAP* :
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPBITMAP* GRPBITMAPFILETGA::CreateBitmapFromFile(XPATH& xpath, GRPPROPERTYMODE forcemode)
{
  if(!Open(xpath))  return NULL;

  GRPBITMAPFILETGAHEADER  header;
  GRPBITMAP*              bitmap = NULL;
  bool                    status = true;

  if(!ReadHeaderFile(&header))
    {
      Close();
      return NULL;
    }

  int                   width       = header.GetWidth();
  int                   height      = header.GetHeight();
  int                   bytesppixel = (header.GetBitsPerPixel()/8);
  GRP2DCOLOR_RGBA8*     colors      = NULL;
  int                   index       = 0;

  bitmap = GRPFACTORY::GetInstance().CreateBitmap(width, height, header.GetMode());
  if(bitmap)
    {
      XBYTE databuffer[8];

      file->SetPosition(header.GetOffsetData());

      XBUFFER* filebuffer = new XBUFFER((height * width * bytesppixel), true);

      file->Read(filebuffer->Get(), height * width * bytesppixel);
      filebuffer->SetPosition(0);

      colors = new GRP2DCOLOR_RGBA8[(height * width)+ 256 ];
      if(colors)
        {
          while(index < (height * width) )
            {
              switch(header.GetType())
                {
                  case GRPBITMAPFILETGATYPE_UNCOMPRESS_RGB  : if(filebuffer->Get(databuffer, bytesppixel))
                                                                {
                                                                  CreateColor(header.GetMode(), databuffer, colors[index]);
                                                                  index++;

                                                                } else status = false;

                                                              break;

                  case GRPBITMAPFILETGATYPE_RLECOMPRESS_RGB : { XBYTE typepacket = 0;
                                                                int   sizepacket = 0;

                                                                if(filebuffer->Get(&typepacket, 1))
                                                                  {
                                                                    sizepacket = (typepacket & 0x7F) + 1;

                                                                    if(typepacket & 0x80) // run-length packet
                                                                      {
                                                                        if(filebuffer->Get(databuffer, bytesppixel))
                                                                          {
                                                                            for(int c=0; c<sizepacket; c++)
                                                                              {
                                                                                CreateColor(header.GetMode(), databuffer, colors[index]);
                                                                                index++;
                                                                              }
                                                                          }
                                                                      }
                                                                     else
                                                                      {
                                                                        // raw packet
                                                                        for(int c=0; c<sizepacket; c++)
                                                                          {
                                                                            if(filebuffer->Get(databuffer, bytesppixel))
                                                                              {
                                                                                CreateColor(header.GetMode(), databuffer, colors[index]);
                                                                                index++;
                                                                              }
                                                                          }
                                                                      }
                                                                  }
                                                              }

                                                              break;

                                                default     : break;
                }

              if(!status) break;
            }
        }

      delete filebuffer;

    }

  index = 0;

  if(status)
    {
      for(int y=height-1; y>-1; y--)
        {
          for(int x=0; x<width; x++)
            {
              switch(header.GetMode())
                {
                  case GRPPROPERTYMODE_16_RGBA_5551 : bitmap->PutBlendPixel(x, y, (GRP2DCOLOR*)&colors[index], (int)colors[index].a);
                                                      break;

                  case GRPPROPERTYMODE_24_RGB_888   : bitmap->PutPixel(x, y, (GRP2DCOLOR*)&colors[index]);
                                                      break;

                  case GRPPROPERTYMODE_32_RGBA_8888 : bitmap->PutPixel(x, y, (GRP2DCOLOR*)&colors[index]);
                                                      break;
                  default: break;
                }

              index++;
            }
        }
    }
   else
    {
      if(bitmap)
        {
          GRPFACTORY::GetInstance().DeleteBitmap(bitmap);
          bitmap = NULL;
        }
    }

  delete [] colors;

  Close();

  return ForceModeOfBitmap(forcemode, bitmap);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPBITMAP* GRPBITMAPFILETGA::CreateBitmapFromBuffer(XBYTE* buffer, XDWORD size, GRPPROPERTYMODE forcemode)
* @brief      CreateBitmapFromBuffer
* @ingroup    GRAPHIC
*
* @param[in]  buffer :
* @param[in]  size :
* @param[in]  forcemode :
*
* @return     GRPBITMAP* :
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPBITMAP* GRPBITMAPFILETGA::CreateBitmapFromBuffer(XBYTE* buffer, XDWORD size, GRPPROPERTYMODE forcemode)
{
  GRPBITMAPFILETGAHEADER  header;
  GRPBITMAP*              bitmap = NULL;
  bool                    status = true;

  indexbuffer = 0;

  if(!ReadHeaderBuffer(buffer, size, &header))
    {
      return NULL;
    }

  int                   width       = header.GetWidth();
  int                   height      = header.GetHeight();
  int                   bytesppixel = (header.GetBitsPerPixel()/8);
  GRP2DCOLOR_RGBA8*     colors      = NULL;
  int                   index       = 0;

  bitmap = GRPFACTORY::GetInstance().CreateBitmap(width, height, header.GetMode());
  if(bitmap)
    {
      XBYTE databuffer[8];

      indexbuffer = header.GetOffsetData();

      XDWORD sizetoread = (height * width * bytesppixel);
      if(size > sizetoread) sizetoread = size;

      XBUFFER* filebuffer = new XBUFFER(sizetoread, true);

      memcpy(filebuffer->Get(), &buffer[indexbuffer], (size - indexbuffer));

      filebuffer->SetPosition(0);

      colors = new GRP2DCOLOR_RGBA8[(height * width)+ 256 ];
      if(colors)
        {
          while(index < (height * width) )
            {
              switch(header.GetType())
                {
                  case GRPBITMAPFILETGATYPE_UNCOMPRESS_RGB  : if(filebuffer->Get(databuffer, bytesppixel))
                                                                {
                                                                  CreateColor(header.GetMode(), databuffer, colors[index]);
                                                                  index++;

                                                                } else status = false;

                                                              break;

                  case GRPBITMAPFILETGATYPE_RLECOMPRESS_RGB : { XBYTE typepacket = 0;
                                                                int   sizepacket = 0;

                                                                if(filebuffer->Get(&typepacket, 1))
                                                                  {
                                                                    sizepacket = (typepacket & 0x7F) + 1;

                                                                    if(typepacket & 0x80) // run-length packet
                                                                      {
                                                                        if(filebuffer->Get(databuffer, bytesppixel))
                                                                          {
                                                                            for(int c=0; c<sizepacket; c++)
                                                                              {
                                                                                CreateColor(header.GetMode(), databuffer, colors[index]);
                                                                                index++;
                                                                              }
                                                                          }
                                                                      }
                                                                     else
                                                                      {
                                                                        // raw packet
                                                                        for(int c=0; c<sizepacket; c++)
                                                                          {
                                                                            if(filebuffer->Get(databuffer, bytesppixel))
                                                                              {
                                                                                CreateColor(header.GetMode(), databuffer, colors[index]);
                                                                                index++;
                                                                              }
                                                                          }
                                                                      }
                                                                  }
                                                              }

                                                              break;

                                                default     : break;
                }

              if(!status) break;
            }
        }

      delete filebuffer;

    }

  index = 0;

  if(status)
    {
      for(int y=height-1; y>-1; y--)
        {
          for(int x=0; x<width; x++)
            {
              switch(header.GetMode())
                {
                  case GRPPROPERTYMODE_16_RGBA_5551 : bitmap->PutBlendPixel(x, y, (GRP2DCOLOR*)&colors[index], (int)colors[index].a);
                                                      break;

                  case GRPPROPERTYMODE_24_RGB_888   : bitmap->PutPixel(x, y, (GRP2DCOLOR*)&colors[index]);
                                                      break;

                  case GRPPROPERTYMODE_32_RGBA_8888 : bitmap->PutPixel(x, y, (GRP2DCOLOR*)&colors[index]);
                                                      break;
                  default: break;
                }

              index++;
            }
        }
    }
   else
    {
      if(bitmap)
        {
          GRPFACTORY::GetInstance().DeleteBitmap(bitmap);
          bitmap = NULL;
        }
    }

  delete [] colors;

  return ForceModeOfBitmap(forcemode, bitmap);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPBITMAPFILETGA::CreateFileFromBitmap(XPATH& xpath, GRPBITMAP* bitmap, int quality)
* @brief      CreateFileFromBitmap
* @ingroup    GRAPHIC
*
* @param[in]  xpath :
* @param[in]  bitmap :
* @param[in]  quality :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPBITMAPFILETGA::CreateFileFromBitmap(XPATH& xpath, GRPBITMAP* bitmap, int quality)
{
  if(!bitmap)         return false;

  if(!Create(xpath))  return false;

  GRPBITMAPFILETGAHEADER header;

  header.GetHeaderInfo()->IDlength            = 0;
  header.GetHeaderInfo()->colourmaptype       = 0;
  header.GetHeaderInfo()->datatypecode        = GRPBITMAPFILETGATYPE_UNCOMPRESS_RGB;
  header.GetHeaderInfo()->colourmaporigin     = 0;
  header.GetHeaderInfo()->colourmaplength     = 0;
  header.GetHeaderInfo()->colourmapdepth      = 0;
  header.GetHeaderInfo()->xorigin             = 0;
  header.GetHeaderInfo()->yorigin             = 0;
  header.GetHeaderInfo()->width               = bitmap->GetWidth();
  header.GetHeaderInfo()->height              = bitmap->GetHeight();
  header.GetHeaderInfo()->bitsperpixel        = 32;
  header.GetHeaderInfo()->imagedescriptor     = 8;

  int  width        = bitmap->GetWidth();
  int  height       = bitmap->GetHeight();
  int  bytesppixel  = (header.GetBitsPerPixel()/8);
  int  sizebmp      = (header.GetWidth()*header.GetHeight()*bytesppixel);
  int  index        = 0;
  bool status       = true;

  if(file->Write((XBYTE*)header.GetHeaderInfo(), sizeof(GRPBITMAPFILETGAHEADERINFO)))
    {
      XBYTE* databuffer = new XBYTE[sizebmp];
      if(databuffer)
        {
          for(int y=height-1; y>-1; y--)
            {
              for(int x=0; x<width; x++)
                {
                  GRP2DCOLOR_RGBA8* color = (GRP2DCOLOR_RGBA8*)bitmap->GetPixel(x, y);
                  if(color)
                    {
                      databuffer[index+2] = color->r;
                      databuffer[index+1] = color->g;
                      databuffer[index+0] = color->b;
                      databuffer[index+3] = color->a;
                    }

                  index  += bytesppixel;
                }
            }

          if(!file->Write((XBYTE*)databuffer, sizebmp)) status = false;

          delete [] databuffer;

        } else status = false;

    } else status = false;

  Close();

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPBITMAPFILETGA::ReadHeaderFile(GRPBITMAPFILETGAHEADER* header)
* @brief      ReadHeaderFile
* @ingroup    GRAPHIC
*
* @param[in]  header :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPBITMAPFILETGA::ReadHeaderFile(GRPBITMAPFILETGAHEADER* header)
{
  if(!header)           return false;
  if(!GetPrimaryFile()) return false;

  if(!header->Load(GetPrimaryFile())) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPBITMAPFILEBMP::ReadHeaderBuffer(XBYTE* buffer, XDWORD size, GRPBITMAPFILEBMPHEADER* header)
* @brief      ReadHeaderBuffer
* @ingroup    GRAPHIC
*
* @param[in]  buffer :
* @param[in]  size :
* @param[in]  header :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPBITMAPFILETGA::ReadHeaderBuffer(XBYTE* buffer, XDWORD size, GRPBITMAPFILETGAHEADER* header)
{
  if(!header->Load(buffer, size, indexbuffer)) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPBITMAPFILETGA::CreateColor(GRPPROPERTYMODE mode, XBYTE* databuffer, GRP2DCOLOR_RGBA8& color)
* @brief      CreateColor
* @ingroup    GRAPHIC
*
* @param[in]  mode : mode of color
* @param[in]  databuffer : data buffer color
* @param[in]  color : color class
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPBITMAPFILETGA::CreateColor(GRPPROPERTYMODE mode, XBYTE* databuffer, GRP2DCOLOR_RGBA8& color)
{
  if(!databuffer) return false;

  switch(mode)
    {
      case GRPPROPERTYMODE_16_RGBA_5551 : color.r =  (databuffer[1] & 0x7c) << 1;
                                          color.g = ((databuffer[1] & 0x03) << 6) | ((databuffer[0] & 0xe0) >> 2);
                                          color.b =  (databuffer[0] & 0x1f) << 3;
                                          color.a =  (databuffer[1] & 0x80)?0:255;
                                          break;

      case GRPPROPERTYMODE_24_RGB_888   : color.r = databuffer[2];
                                          color.g = databuffer[1];
                                          color.b = databuffer[0];
                                          color.a = 255;
                                          break;

      case GRPPROPERTYMODE_32_RGBA_8888 : color.r = databuffer[2];
                                          color.g = databuffer[1];
                                          color.b = databuffer[0];
                                          color.a = databuffer[3];
                                          break;
      default: break;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRPBITMAPFILETGA::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
void GRPBITMAPFILETGA::Clean()
{
   indexbuffer = 0;
}


#pragma endregion


#pragma endregion

