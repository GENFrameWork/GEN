/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XWINDOWSWinget.cpp
* 
* @class      XWINDOWSWINGET
* @brief      WINDOWS eXtended Utils WinGet API class
* @ingroup    PLATFORM_WINDOWS
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
#pragma region PRECOMPILATION_DEFINE_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XWINDOWSWinget.h"

#include "XBuffer.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


#pragma region CLASS_MEMBERS_XWINDOWSWINGET_RESULT


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWINDOWSWINGET_RESULT::XWINDOWSWINGET_RESULT()
* @brief      Constructor of class
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWINDOWSWINGET_RESULT::XWINDOWSWINGET_RESULT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWINDOWSWINGET_RESULT::~XWINDOWSWINGET_RESULT()
* @brief      Destructor of class
* @ingroup    PLATFORM_WINDOWS
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWINDOWSWINGET_RESULT::~XWINDOWSWINGET_RESULT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XWINDOWSWINGET_RESULT::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @ingroup    PLATFORM_WINDOWS
* @note       INTERNAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSWINGET_RESULT::Clean()
{

}


#pragma endregion


#pragma region CLASS_MEMBERS_XWINDOWSWINGET


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWINDOWSWINGET::XWINDOWSWINGET()
* @brief      Constructor of class
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWINDOWSWINGET::XWINDOWSWINGET()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWINDOWSWINGET::~XWINDOWSWINGET()
* @brief      Destructor of class
* @ingroup    PLATFORM_WINDOWS
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWINDOWSWINGET::~XWINDOWSWINGET()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XWINDOWSWINGET::List(XCHAR* addparam, XWINDOWSWINGET_TABLE* output)
* @brief      list
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  addparam : 
* @param[in]  output : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSWINGET::List(XWINDOWSWINGET_RESULT& result)
{
  XBUFFER buffer_output;
  bool    status;

  status = Exec(__L("Get-WinGetPackage | Select-Object Name"), buffer_output);
  if(!status)
    {
      return false;
    }    

  XSTRING string;

  string.ConvertFromUTF8(buffer_output);

  /*
  if(!ParseWinget((char*)buffer_output.Get(),  output)) 
    {
      return false;
    }    
  */

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XWINDOWSWINGET::Exec(XCHAR* params, XBUFFER& output)
* @brief      exec
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  params : 
* @param[in]  output : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSWINGET::Exec(XCHAR* params, XBUFFER& output)
{
  XSTRING cmd_str;
  XBUFFER cmd;

  output.Delete();
 
  cmd_str.Format(__L("powershell -NonInteractive -command \"%s\" 2>&1"), params);
  cmd_str.ConvertToASCII(cmd);
    
  FILE* pipe = _popen((char*)cmd.Get(), "rb");
  if(!pipe) 
    {
      return false;
    }

  while(!feof(pipe)) 
    {
      int c = fgetc(pipe);
      if(c == EOF) 
        {
          break;
        }

      output.Add((XBYTE)c);
    }    
    
  _pclose(pipe);

  NormalizeUnicode(output.Get());

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int XWINDOWSWINGET::LooksLikeUTF16(XBYTE* buffer, int size)
* @brief      looks like UTf16
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  buffer : 
* @param[in]  size : 
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int XWINDOWSWINGET::LooksLikeUTF16(XBYTE* buffer, int size) 
{
  return (size >= 2 && buffer[0] == 0xFF && buffer[1] == 0xFE);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XWINDOWSWINGET::LooksLikeUTF8(XBYTE* buffer, int size)
* @brief      looks like UTf8
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  buffer : 
* @param[in]  size : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSWINGET::LooksLikeUTF8(XBYTE* buffer, int size) 
{
  int i = 0;

  while(i < size) 
    {
      XBYTE c = buffer[i];

      if(c < 0x80) 
        { 
          i++; 
          continue; 
        }

      if((c & 0xE0) == 0xC0 && i + 1 < size && (buffer[i+1] & 0xC0) == 0x80) 
        { 
          i += 2; 
          continue; 
        }

      if((c & 0xF0) == 0xE0 && i + 2 < size && (buffer[i+1] & 0xC0) == 0x80 && (buffer[i+2] & 0xC0) == 0x80) 
        { 
          i += 3; 
          continue; 
        }

      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XWINDOWSWINGET::ConvertUTF16ToUTF8(XBYTE* data)
* @brief      convert UTf16 to UTf8
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  data : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSWINGET::ConvertUTF16ToUTF8(XBYTE* data) 
{
  int       len   = (int)strlen((char*)data);
  wchar_t*  w     = (wchar_t*)(data + 2);
  int       wlen  = (len - 2) / 2;
  XBYTE*    out   = (XBYTE*)malloc(wlen * 3 + 1);
  
  if(!out) 
    {
      return;
    }
  
  int r = WideCharToMultiByte(CP_UTF8, 0, w, wlen, (char*)out, wlen * 3, NULL, NULL);
  if(r < 0) 
    {
      r = 0;
    }
  
  out[r] = 0;
  
  strcpy((char*)data, (char*)out);
  
  free(out);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XWINDOWSWINGET::CP437ToASCII(XBYTE* dst, const XBYTE* src, int max)
* @brief      Cp437 to ASCIi
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  dst : 
* @param[in]  src : 
* @param[in]  max : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSWINGET::CP437ToASCII(XBYTE* dst, const XBYTE* src, int max) 
{
  int di = 0;
  
  for(int i = 0; src[i] && di < max - 1; i++) 
    {
      XBYTE c = (XBYTE)src[i];
      if(c < 128) 
        {
          dst[di++] = (char)c;
        }
       else 
        {
          dst[di++] = '?';
        }
    }

  dst[di] = 0;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XWINDOWSWINGET::NormalizeUnicode(XBYTE* data)
* @brief      normalize unicode
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  data : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSWINGET::NormalizeUnicode(XBYTE* data) 
{
  int     len    = (int)strlen((char*)data);
  XBYTE*  buffer = (XBYTE*)data;
  
  if(LooksLikeUTF16(buffer, len)) 
    {
      ConvertUTF16ToUTF8(data);
      return;
    }
  
  if(!LooksLikeUTF8(buffer, len)) 
    {
      char* tmp = new char[32768];
      
      if(tmp)
        {  
          memset(tmp, 0, 32768);

          CP437ToASCII((XBYTE*)tmp, data, 32768);
          strcpy((char*)data, tmp);

          delete [] tmp;        
        }
    }
}



/*
bool XWINDOWSWINGET::ParseWinget(const char* text, XWINDOWSWINGET_LIST* list) 
{
  
  return false;
}
*/

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XWINDOWSWINGET::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @ingroup    PLATFORM_WINDOWS
* @note       INTERNAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSWINGET::Clean()
{

}


#pragma endregion


#pragma endregion
