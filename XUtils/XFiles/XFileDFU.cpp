/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XFileDFU.cpp
* 
* @class      XFILEDFU
* @brief      eXtended Utils (DFU Device Firmware Update) file class
* @ingroup    XUTILS
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

/*---- PRECOMPILATION INCLUDES ----------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XFileDFU.h"

#include "XFactory.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

XDWORD XFILEDFU::CRCtable[256] =   {  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535,
			                                0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
			                                0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856,
			                                0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e,
			                                0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6,
			                                0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
			                                0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87,
			                                0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
			                                0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97,
			                                0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
			                                0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 0x4db26158,
			                                0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc,
			                                0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010,
			                                0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f, 0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
			                                0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739,
			                                0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d,
			                                0x0a00ae27, 0x7d079eb1, 0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671,
			                                0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
			                                0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda,
			                                0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a,
			                                0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92,
			                                0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
			                                0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b,
			                                0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b,
			                                0x6fb077e1, 0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3,
			                                0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
			                                0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9, 0xbdbdf21c,
			                                0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8,
			                                0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d 
                                    };

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


#pragma region CLASS_XFILEDFU_PREFIX


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEDFU_PREFIX::XFILEDFU_PREFIX()
* @brief      Constructor
* @ingroup    XUTILS
*
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEDFU_PREFIX::XFILEDFU_PREFIX()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEDFU_PREFIX::~XFILEDFU_PREFIX()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEDFU_PREFIX::~XFILEDFU_PREFIX()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XFILEDFU_PREFIX::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XFILEDFU_PREFIX::Clean()
{
  memset(signature, 0, XFILEDFU_PREFIX_SIGNATUREPREXIF_MAX);  // DfuSe
  version   = 0;                                              // version 
  imagesize = 0;                                              // image size;          
  ntargets  = 0;                                              // number images; 
}


#pragma endregion


#pragma region CLASS_XFILEDFU_IMAGE


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEDFU_IMAGE::XFILEDFU_IMAGE()
* @brief      Constructor
* @ingroup    XUTILS
*
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEDFU_IMAGE::XFILEDFU_IMAGE()
{
  Clean();
}
 

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEDFU_IMAGE::~XFILEDFU_IMAGE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEDFU_IMAGE::~XFILEDFU_IMAGE()
{
  elements.DeleteContents();
  elements.DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XFILEDFU_IMAGE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XFILEDFU_IMAGE::Clean()
{
  memset(signature, 0, XFILEDFU_PREFIX_SIGNATUREIMAGE_MAX);
  alternatesetting = 0;

  targetnameexist  = 0;
  memset(targetname, 0, XFILEDFU_IMAGEPREFIX_NAME_MAX);
      
  targetsize       = 0;
  nelements        = 0;    
}


#pragma endregion


#pragma region CLASS_XFILEDFU_ELEMENT


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEDFU_ELEMENT::XFILEDFU_ELEMENT()
* @brief      Constructor
* @ingroup    XUTILS
*
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEDFU_ELEMENT::XFILEDFU_ELEMENT()
{
  Clean();
}
 

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEDFU_ELEMENT::~XFILEDFU_ELEMENT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEDFU_ELEMENT::~XFILEDFU_ELEMENT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XFILEDFU_ELEMENT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XFILEDFU_ELEMENT::Clean()
{
  address = 0;
  size    = 0;
}


#pragma endregion


#pragma region CLASS_XFILEDFU_SUFFIX


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEDFU_SUFFIX::XFILEDFU_SUFFIX()
* @brief      Constructor
* @ingroup    XUTILS
*
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEDFU_SUFFIX::XFILEDFU_SUFFIX()
{
  Clean();
}
 

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEDFU_SUFFIX::~XFILEDFU_SUFFIX()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEDFU_SUFFIX::~XFILEDFU_SUFFIX()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XFILEDFU_SUFFIX::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XFILEDFU_SUFFIX::Clean()
{
  BCDdevice   = 0;   
  IDproduct   = 0; 
  IDvendor    = 0;  
  BCDDFU      = 0;

  memset(signature, 0, XFILEDFU_PREFIX_SIGNATURESUFFIX_MAX);

  length      = 0;
  CRC         = 0;
}


#pragma endregion


#pragma region CLASS_XFILEDFU


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEDFU::XFILEDFU()
* @brief      Constructor
* @ingroup    XUTILS
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEDFU::XFILEDFU()
{
  Clean();

  GEN_XFACTORY_CREATE(xfilebase, Create_File())
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEDFU::~XFILEDFU()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEDFU::~XFILEDFU()
{
  images.DeleteContents();
  images.DeleteAll();

  if(xfilebase) GEN_XFACTORY.Delete_File(xfilebase);

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFILEDFU::Open(XCHAR* xpath, bool isreadonly)
* @brief      Open
* @ingroup    XUTILS
*
* @param[in]  xpath : 
* @param[in]  isreadonly : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XFILEDFU::Open(XCHAR* xpath, bool isreadonly)
{
  if(!xfilebase) return false;

  return xfilebase->Open(xpath, isreadonly);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFILEDFU::ReadAll()
* @brief      ReadAll
* @ingroup    XUTILS
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XFILEDFU::ReadAll()
{
  if(!xfilebase) return false;

  bool status = false;

  images.DeleteContents();
  images.DeleteAll();

  xfilebase->SetPosition(0);  

  //------------------------------------------
  // Read Prefix

  status = xfilebase->Read(prefix.signature, XFILEDFU_PREFIX_SIGNATUREPREXIF_MAX);
  if(status) status = xfilebase->Read(&prefix.version, 1);
  if(status) status = xfilebase->Read((XBYTE*)&prefix.imagesize, sizeof(XDWORD));
  if(status) status = xfilebase->Read(&prefix.ntargets, sizeof(XBYTE));

  if(status)
    {
      for(int c=0; c<prefix.ntargets; c++)
        {      
          XFILEDFU_IMAGE* image = Read_Image();
          if(image)images.Add(image);      
        }  
    }

  //------------------------------------------
  // Read Suffix

  status =xfilebase->Read((XBYTE*)&suffix.BCDdevice, sizeof(XWORD));
  if(status) status = xfilebase->Read((XBYTE*)&suffix.IDproduct, sizeof(XWORD));
  if(status) status = xfilebase->Read((XBYTE*)&suffix.IDvendor, sizeof(XWORD));
  if(status) status = xfilebase->Read((XBYTE*)&suffix.BCDDFU, sizeof(XWORD));
  if(status) status = xfilebase->Read((XBYTE*)&suffix.signature, XFILEDFU_PREFIX_SIGNATURESUFFIX_MAX);
  if(status) status = xfilebase->Read(&suffix.length, 1);
  if(status) status = xfilebase->Read((XBYTE*)&suffix.CRC, sizeof(XDWORD));


  //--------------------------------------------
  // Check CRC used in DFU.
  
  status = CheckCRC();
   
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEDFU_PREFIX* XFILEDFU::GetPrefix()
* @brief      GetPrefix
* @ingroup    XUTILS
*
* @return     XFILEDFU_PREFIX* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEDFU_PREFIX* XFILEDFU::GetPrefix()
{
  return &prefix;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XFILEDFU_IMAGE*>* XFILEDFU::GetImages()
* @brief      GetImages
* @ingroup    XUTILS
*
* @return     XVECTOR<XFILEDFU_IMAGE*>* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XFILEDFU_IMAGE*>*  XFILEDFU::GetImages()
{
  return &images;
}
    


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEDFU_SUFFIX* XFILEDFU::GetSuffix()
* @brief      GetSuffix
* @ingroup    XUTILS
*
* @return     XFILEDFU_SUFFIX* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEDFU_SUFFIX* XFILEDFU::GetSuffix()
{
  return &suffix;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFILEDFU::Close()
* @brief      Close
* @ingroup    XUTILS
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XFILEDFU::Close()
{
  if(!xfilebase) return false;

  return xfilebase->Close();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEDFU_IMAGE* XFILEDFU::Read_Image()
* @brief      Read_Image
* @ingroup    XUTILS
*
* @return     XFILEDFU_IMAGE* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEDFU_IMAGE* XFILEDFU::Read_Image()
{
  XFILEDFU_IMAGE* image = new XFILEDFU_IMAGE();
  if(!image) return NULL;

  bool status = false;

  status = xfilebase->Read(image->signature, XFILEDFU_PREFIX_SIGNATUREIMAGE_MAX);
  if(status) xfilebase->Read(&image->alternatesetting, 1);
  if(status) xfilebase->Read((XBYTE*)&image->targetnameexist, sizeof(XDWORD));
  if(status) xfilebase->Read((XBYTE*)&image->targetname, XFILEDFU_IMAGEPREFIX_NAME_MAX);
  if(status) xfilebase->Read((XBYTE*)&image->targetsize, sizeof(XDWORD));
  if(status) xfilebase->Read((XBYTE*)&image->nelements, sizeof(XDWORD));

  for(XDWORD c=0; c<image->nelements; c++)
    {
      XFILEDFU_ELEMENT* element = Read_Element();
      if(element) image->elements.Add(element);       
    }

  if(!status) 
    { 
      delete image;
      return NULL;
    }

  return image;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEDFU_ELEMENT* XFILEDFU::ReadElement()
* @brief      ReadElement
* @ingroup    XUTILS
*
* @return     XFILEDFU_ELEMENT* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEDFU_ELEMENT* XFILEDFU::Read_Element()
{
  XFILEDFU_ELEMENT* element = new XFILEDFU_ELEMENT();
  if(!element) return NULL;

  bool status = false;

  status = xfilebase->Read((XBYTE*)&element->address, sizeof(XDWORD));
  if(status) xfilebase->Read((XBYTE*)&element->size, sizeof(XDWORD));
  if(status)
    {
      status = false;
      if(element->size)
        {
          status = element->data.Resize(element->size);
          if(status) status = xfilebase->Read(element->data.Get(), element->data.GetSize());            
        }
    }

  if(!status) 
    { 
      delete element;
      return NULL;
    }

  return element;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFILEDFU::CheckCRC()
* @brief      CheckCRC
* @ingroup    XUTILS
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XFILEDFU::CheckCRC()
{
  XBUFFER buffer;
  bool    status;

  xfilebase->SetPosition(0);
  buffer.Resize((XDWORD)(xfilebase->GetSize() - sizeof(XDWORD)));
      
  status = xfilebase->Read(buffer.Get(), buffer.GetSize());
  if(status)
    {
      XDWORD CRC = 0xFFFFFFFF;
  		for (XDWORD i = 0; i < buffer.GetSize(); i++) 
        {
		  	  CRC = CRCtable[(CRC ^ buffer.Get()[i]) & 0xff] ^ (CRC >> 8);
		    }

      if(CRC != suffix.CRC) status = false;                 		
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void Clean()
* @brief      lean
* @ingroup    XUTILS
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XFILEDFU::Clean()
{
  xfilebase     = NULL;
}


#pragma endregion


#pragma endregion

