/**-------------------------------------------------------------------------------------------------------------------
*
* @file       GRPBitmapFilePNG.cpp
*
* @class      GRPBITMAPFILEPNG
* @brief      Graphic bitmap file PNG format (PNG: Portable Network Graphics) class
* @ingroup    GRAPHIC
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @copyright  Copyright(c) 2008 - 2016 GEN Group.
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "GRPFactory.h"
#include "GRPBitmap.h"

#include "GRPBitmapFilePNG.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPBITMAPFILEPNG::GRPBITMAPFILEPNG()
* @brief      Constructor
* @ingroup    GRAPHIC
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
GRPBITMAPFILEPNG::GRPBITMAPFILEPNG(): GRPBITMAPFILEBASE()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPBITMAPFILEPNG::~GRPBITMAPFILEPNG()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    GRAPHIC
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
GRPBITMAPFILEPNG::~GRPBITMAPFILEPNG()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPBITMAP* GRPBITMAPFILEPNG::CreateBitmapFromFile(XPATH& xpath, GRPPROPERTYMODE forcemode)
* @brief      CreateBitmapFromFile
* @ingroup    GRAPHIC
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  xpath :
* @param[in]  forcemode :
*
* @return     GRPBITMAP* :
*
*---------------------------------------------------------------------------------------------------------------------*/
GRPBITMAP* GRPBITMAPFILEPNG::CreateBitmapFromFile(XPATH& xpath, GRPPROPERTYMODE forcemode)
{
  if(!Open(xpath))  return NULL;

  GRPBITMAP*    bitmap      = NULL;
  png_bytep*    databuffer  = NULL;
  XBYTE         header[8];
  png_structp   png;
  png_infop     info;
  png_uint_32   width       = 0;
  png_uint_32   height      = 0;
  png_byte      colortype   = 0;
  int           bytesppixel = 0;
  int           rowbytes    = 0;
  int           npasses     = 0;

  if(file->Read(header, GRPBITMAPFILEPNG_HEADERSIZE))
    {
      if(!png_sig_cmp(header, 0, 8))
        {
          png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
          if(png)
            {
              info = png_create_info_struct(png);
              if(info)
                {
                  if(!setjmp(png_jmpbuf(png)))
                    {
                      png_set_read_fn(png , (void *)this, GRPBITMAPFILEPNG::ReadFileData);

                      png_set_sig_bytes(png, 8);

                      png_read_info(png, info);

                      width        = png_get_image_width(png, info);
                      height       = png_get_image_height(png, info);
                      colortype    = png_get_color_type(png, info);
                      bytesppixel  = png_get_channels(png, info);
                      rowbytes     = (int)png_get_rowbytes(png, info);
                      npasses      = png_set_interlace_handling(png);

                      png_read_update_info(png, info);

                      if(!setjmp(png_jmpbuf(png)))
                        {
                          databuffer = (png_bytep*)new XBYTE[sizeof(png_bytep) * height];
                          for(int y=0; y<(int)height; y++)
                            {
                              databuffer[y] = (png_byte*)new XBYTE[rowbytes];
                            }

                          png_read_image(png, databuffer);
                          png_destroy_read_struct(&png, &info, NULL);

                          GRPPROPERTYMODE mode = GRPPROPERTYMODE_XX_UNKNOWN;

                          switch(colortype)
                            {
                              case PNG_COLOR_TYPE_RGB         : mode = GRPPROPERTYMODE_24_RGB_888;
                              case PNG_COLOR_TYPE_RGB_ALPHA   : mode = GRPPROPERTYMODE_32_RGBA_8888;
                            }

                          bitmap = GRPFACTORY::GetInstance().CreateBitmap(width, height, mode);
                          if(bitmap)
                            {
                              int index = 0;

                              for(int y=0; y<(int)height; y++)
                                {
                                  index = 0;

                                  for(int x=0; x<(int)width; x++)
                                    {
                                      GRP2DCOLOR_RGBA8 color;

                                      color.r = (XBYTE)(*(XBYTE*)(databuffer[y] + index + 0));
                                      color.g = (XBYTE)(*(XBYTE*)(databuffer[y] + index + 1));
                                      color.b = (XBYTE)(*(XBYTE*)(databuffer[y] + index + 2));

                                      if(colortype == PNG_COLOR_TYPE_RGB)
                                        {
                                          color.a = 255;
                                        }
                                      else
                                        {
                                          color.a = (XBYTE)(*(XBYTE*)(databuffer[y] + index + 3));
                                        }

                                      bitmap->PutPixel(x, y, (GRP2DCOLOR*)&color);

                                      index  += bytesppixel;
                                    }
                                }

                            }

                          for(int y=0; y<(int)height; y++)
                            {
                              delete [] databuffer[y];
                            }

                          delete [] databuffer;
                        }
                    }
                }
            }
        }
    }

  Close();

  return ForceModeOfBitmap(forcemode, bitmap);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPBITMAP* GRPBITMAPFILEPNG::CreateBitmapFromBuffer(XBYTE* buffer, XDWORD size, GRPPROPERTYMODE forcemode)
* @brief      CreateBitmapFromBuffer
* @ingroup    GRAPHIC
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  buffer :
* @param[in]  size :
* @param[in]  forcemode :
*
* @return     GRPBITMAP* :
*
*---------------------------------------------------------------------------------------------------------------------*/
GRPBITMAP* GRPBITMAPFILEPNG::CreateBitmapFromBuffer(XBYTE* buffer, XDWORD size, GRPPROPERTYMODE forcemode)
{
  if(!buffer)                            return NULL;
  if(size<GRPBITMAPFILEPNG_HEADERSIZE)   return NULL;

  GRPBITMAP*    bitmap      = NULL;
  png_bytep*    databuffer  = NULL;
  XBYTE         header[8];
  png_structp   png;
  png_infop     info;
  png_uint_32   width       = 0;
  png_uint_32   height      = 0;
  png_byte      colortype   = 0;
  int           bytesppixel = 0;
  int           rowbytes    = 0;
  int           npasses     = 0;

  bufferdata      = buffer;
  bufferdatasize  = size;
  bufferdataindex = GRPBITMAPFILEPNG_HEADERSIZE;

  memcpy(header, buffer, GRPBITMAPFILEPNG_HEADERSIZE);

  bufferdataindex = GRPBITMAPFILEPNG_HEADERSIZE;

  if(!png_sig_cmp(header, 0, 8))
    {
      png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
      if(png)
        {
          info = png_create_info_struct(png);
          if(info)
            {
              if(!setjmp(png_jmpbuf(png)))
                {
                  png_set_read_fn(png , (void *)this, GRPBITMAPFILEPNG::ReadBufferData);

                  png_set_sig_bytes(png, 8);

                  png_read_info(png, info);

                  width        = png_get_image_width(png, info);
                  height       = png_get_image_height(png, info);
                  colortype    = png_get_color_type(png, info);
                  bytesppixel  = png_get_channels(png, info);
                  rowbytes     = (int)png_get_rowbytes(png, info);
                  npasses      = png_set_interlace_handling(png);

                  png_read_update_info(png, info);

                  if(!setjmp(png_jmpbuf(png)))
                    {
                      databuffer = (png_bytep*)new XBYTE[sizeof(png_bytep) * height];
                      for(int y=0; y<(int)height; y++)
                        {
                          databuffer[y] = (png_byte*)new XBYTE[rowbytes];
                        }

                      png_read_image(png, databuffer);
                      png_destroy_read_struct(&png, &info, NULL);

                      GRPPROPERTYMODE mode = GRPPROPERTYMODE_XX_UNKNOWN;

                      switch(colortype)
                        {
                          case PNG_COLOR_TYPE_RGB         : mode = GRPPROPERTYMODE_24_RGB_888;
                          case PNG_COLOR_TYPE_RGB_ALPHA   : mode = GRPPROPERTYMODE_32_RGBA_8888;
                        }

                      bitmap = GRPFACTORY::GetInstance().CreateBitmap(width, height, mode);
                      if(bitmap)
                        {
                          int index = 0;

                          for(int y=0; y<(int)height; y++)
                            {
                              index = 0;

                              for(int x=0; x<(int)width; x++)
                                {
                                  GRP2DCOLOR_RGBA8 color;

                                  color.r = (XBYTE)(*(XBYTE*)(databuffer[y] + index + 0));
                                  color.g = (XBYTE)(*(XBYTE*)(databuffer[y] + index + 1));
                                  color.b = (XBYTE)(*(XBYTE*)(databuffer[y] + index + 2));

                                  if(colortype == PNG_COLOR_TYPE_RGB)
                                    {
                                      color.a = 255;
                                    }
                                  else
                                    {
                                      color.a = (XBYTE)(*(XBYTE*)(databuffer[y] + index + 3));
                                    }

                                  bitmap->PutPixel(x, y, (GRP2DCOLOR*)&color);

                                  index  += bytesppixel;
                                }
                            }

                        }

                      for(int y=0; y<(int)height; y++)
                        {
                          delete [] databuffer[y];
                        }

                      delete [] databuffer;
                    }
                }
            }
        }
    }

  return ForceModeOfBitmap(forcemode, bitmap);
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPBITMAPFILEPNG::CreateFileFromBitmap(XPATH& xpath, GRPBITMAP* bitmap, int quality)
* @brief      CreateFileFromBitmap
* @ingroup    GRAPHIC
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  xpath :
* @param[in]  bitmap :
* @param[in]  quality :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool GRPBITMAPFILEPNG::CreateFileFromBitmap(XPATH& xpath, GRPBITMAP* bitmap, int quality)
{
  if(!bitmap)     return false;

  if(!Create(xpath)) return false;

  png_structp   png;
  png_infop     info;
  png_bytep*    databuffer  = NULL;
  png_uint_32   width       = bitmap->GetWidth();
  png_uint_32   height      = bitmap->GetHeight();
  png_byte      bitdepth    = 8;
  int           bytesppixel = 4;
  int           rowbytes    = 0;
  bool          status      = true;

  png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(png)
    {
      info = png_create_info_struct(png);
      if(info)
        {
          if(!setjmp(png_jmpbuf(png)))
            {
              png_set_write_fn(png  , (void *)this, GRPBITMAPFILEPNG::WriteFileData, NULL);

              if(!setjmp(png_jmpbuf(png)))
                {
                  rowbytes = width * 4;

                  databuffer = (png_bytep*)new XBYTE[sizeof(png_bytep) * height];
                  for(int y=0; y<(int)height; y++)
                    {
                      databuffer[y] = (png_byte*)new XBYTE[rowbytes];
                    }

                  png_set_IHDR(png, info, width                     ,
                                          height                    ,
                                          bitdepth                  ,
                                          PNG_COLOR_TYPE_RGB_ALPHA  ,
                                          PNG_INTERLACE_NONE        ,
                                          PNG_COMPRESSION_TYPE_BASE ,
                                          PNG_FILTER_TYPE_BASE);

                  png_write_info(png, info);

                  if(!setjmp(png_jmpbuf(png)))
                    {
                      int index = 0;

                      for(int y=0; y<(int)height; y++)
                        {
                          index = 0;

                          for(int x=0; x<(int)width; x++)
                            {
                              GRP2DCOLOR_RGBA8* color = (GRP2DCOLOR_RGBA8*)bitmap->GetPixel(x, y);
                              if(color)
                                {
                                  XBYTE* ptr = (XBYTE*)(databuffer[y] + index);

                                  ptr[0] = color->r;
                                  ptr[1] = color->g;
                                  ptr[2] = color->b;
                                  ptr[3] = color->a;
                                }

                              index  += bytesppixel;
                            }
                        }

                      png_write_image(png, databuffer);

                      if(!setjmp(png_jmpbuf(png)))
                        {
                          png_write_end(png, NULL);
                        }
                    }

                  for(int y=0; y<(int)height; y++)
                    {
                      delete [] databuffer[y];
                    }

                  delete [] databuffer;

                }
            }
        }
    }

  Close();

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE* GRPBITMAPFILEPNG::BufferData_Get()
* @brief      BufferData_Get
* @ingroup    GRAPHIC
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     XBYTE* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XBYTE* GRPBITMAPFILEPNG::BufferData_Get()
{
  return bufferdata;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD GRPBITMAPFILEPNG::BufferData_GetIndex()
* @brief      BufferData_GetIndex
* @ingroup    GRAPHIC
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     XDWORD :
*
*---------------------------------------------------------------------------------------------------------------------*/
XDWORD GRPBITMAPFILEPNG::BufferData_GetIndex()
{
  return bufferdataindex;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRPBITMAPFILEPNG::BufferData_SetIndex(XDWORD index)
* @brief      BufferData_SetIndex
* @ingroup    GRAPHIC
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  index :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void GRPBITMAPFILEPNG::BufferData_SetIndex(XDWORD index)
{
  this->bufferdataindex = index;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD GRPBITMAPFILEPNG::BufferData_GetSize()
* @brief      BufferData_GetSize
* @ingroup    GRAPHIC
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     XDWORD :
*
*---------------------------------------------------------------------------------------------------------------------*/
XDWORD GRPBITMAPFILEPNG::BufferData_GetSize()
{
  return bufferdatasize;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRPBITMAPFILEPNG::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    GRAPHIC
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void GRPBITMAPFILEPNG::Clean()
{
  bufferdata        = NULL;
  bufferdataindex   = 0;
  bufferdatasize    = 0;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRPBITMAPFILEPNG::ReadFileData(png_structp png, png_bytep data, png_size_t length)
* @brief      ReadData
* @ingroup    GRAPHIC
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  png : struct png
* @param[in]  data : data of PNG
* @param[in]  length : size of data
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void GRPBITMAPFILEPNG::ReadFileData(png_structp png, png_bytep data, png_size_t length)
{
  GRPBITMAPFILEPNG* xfilepng = (GRPBITMAPFILEPNG*)png_get_io_ptr(png);
  if(!xfilepng) return;

  xfilepng->GetPrimaryFile()->Read((XBYTE*)data, (int)length);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRPBITMAPFILEPNG::ReadBufferData(png_structp png, png_bytep data, png_size_t length)
* @brief      ReadBufferData
* @ingroup    GRAPHIC
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  png :
* @param[in]  data :
* @param[in]  length :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void GRPBITMAPFILEPNG::ReadBufferData(png_structp png, png_bytep data, png_size_t length)
{
  GRPBITMAPFILEPNG* xfilepng = (GRPBITMAPFILEPNG*)png_get_io_ptr(png);
  if(!xfilepng) return;

  XBYTE* buffer = xfilepng->BufferData_Get();
  int    index  = xfilepng->BufferData_GetIndex();

  memcpy((XBYTE*)data, &buffer[index], (int)length);

  index+= (int)length;

  xfilepng->BufferData_SetIndex(index);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRPBITMAPFILEPNG::WriteFileData(png_structp png, png_bytep data, png_size_t length)
* @brief      WriteData
* @ingroup    GRAPHIC
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  png : png struct
* @param[in]  data : data of PNG
* @param[in]  length : size of data
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void GRPBITMAPFILEPNG::WriteFileData(png_structp png, png_bytep data, png_size_t length)
{
  GRPBITMAPFILEPNG* xfilepng = (GRPBITMAPFILEPNG*)png_get_io_ptr(png);
  if(!xfilepng) return;

  xfilepng->GetPrimaryFile()->Write((XBYTE*)data, (int)length);
}
