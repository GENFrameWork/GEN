/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XWINDOWSWinget.cpp
* 
* @class      XWINDOWSWINGET
* @brief      WINDOWS eXtended Utils WinGet API class
* @ingroup    PLATFORM_WINDOWS
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
bool XWINDOWSWINGET::List(XCHAR* addparam, XWINDOWSWINGET_TABLE* output)
{
  XSTRING cmd; 
  XBUFFER buffer_output;
  bool    status;

  if(!output)
    {
      return false;
    }

  cmd  = __L("list");
  if(addparam)
    {  
      cmd += __L(" ");
      cmd += addparam;
    }

  status = Exec(__L("list"), buffer_output);
  if(!status)
    {
      return false;
    }    

  XSTRING string;

  string.ConvertFromUTF8(buffer_output);

  if(!ParseWinget((char*)buffer_output.Get(),  output)) 
    {
      return false;
    }    
 
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
  
  //cmd_str.Format(__L("winget %s  2>&1"), params);

  cmd_str.Format(__L("powershell -NonInteractive -command \"Get-WinGetPackage | Select-Object Name 2>&1\""), params);
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


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XWINDOWSWINGET::RemoveANSICodes(char* s)
* @brief      delete ANSI escape sequences (color codes, etc.)
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  s : null-terminated string
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSWINGET::RemoveANSICodes(char* s)
{
  if(!s)
    {
      return;
    }

  char* src = s;
  char* dst = s;

  while(*src)
    {
      if(*src == 0x1B)            // ESC
        {
          src++;

          if(*src == '[')         // CSI: ESC [ ... <final>
            {
              src++;

              while(*src && !((*src >= '@') && (*src <= '~')))
                {
                  src++;
                }

              if(*src)
                {
                  src++;
                }

              continue;
            }
        }

      *dst++ = *src++;
    }

  *dst = 0;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         char* XWINDOWSWINGET::Trim(char* s)
* @brief      trim
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  s : 
* 
* @return     char* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
char* XWINDOWSWINGET::Trim(char* s) 
{
  while(*s == ' ' || *s == '\t')
    {
      s++;
    }
  
  char* end = s + strlen(s);
  
  while(end > s && (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\r' || end[-1] == '\n'))
    {
      end--;
    }
  
  *end = 0;
  
  return s;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int XWINDOWSWINGET::IsSeparatorLine(const char* line)
* @brief      is separator line
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  line : 
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int XWINDOWSWINGET::IsSeparatorLine(const char* line)
{
  int any = 0;

  for(int i = 0; line[i]; i++) 
    {
      if(line[i] != '-' && line[i] != ' ' && line[i] != '\r' && line[i] != '\n')
        {
          return 0;
        }

      if(line[i] == '-')
        {
          any = 1;
        }
    }

  return any;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int XWINDOWSWINGET::ParseWinget(const char* text, XWINDOWSWINGET_TABLE* outtable) 
* @brief      parse winget table output (multi-column)
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  text : raw winget output (UTF-8, CR/LF)
* @param[in]  outtable : parsed table
* 
* @return     int : 1 if successful, 0 otherwise
* 
* --------------------------------------------------------------------------------------------------------------------*/
static int XWINDOWSWINGET_SplitByMultiSpace(const char* line, char tokens[][XWINDOWSWINGET_MAX_COL_VALUE], int maxtokens)
{
  int len   = 0;
  int count = 0;

  if(!line || !tokens || (maxtokens <= 0))
    {
      return 0;
    }

  len = (int)strlen(line);

  int i = 0;
  while((i < len) && (count < maxtokens))
    {
      /* skip leading spaces / tabs / CR */
      while(i < len)
        {
          char ch = line[i];
          if((ch == ' ') || (ch == '\t') || (ch == '\r'))
            {
              i++;
              continue;
            }
          break;
        }

      if(i >= len)
        {
          break;
        }

      int start = i;

      /* advance until we see a run of >= 2 spaces/tabs/CR (column separator) */
      while(i < len)
        {
          char ch = line[i];

          if((ch == ' ') || (ch == '\t') || (ch == '\r'))
            {
              if((i + 1 < len) && ((line[i+1] == ' ') || (line[i+1] == '\t') || (line[i+1] == '\r')))
                {
                  /* first char of a multi-space separator -> end of this field */
                  break;
                }
            }

          i++;
        }

      int end = i;

      /* trim trailing spaces / tabs / CR from this field */
      while((end > start) && ((line[end-1] == ' ') || (line[end-1] == '\t') || (line[end-1] == '\r')))
        {
          end--;
        }

      int flen = end - start;
      if(flen < 0) flen = 0;
      if(flen >= (XWINDOWSWINGET_MAX_COL_VALUE - 1))
        {
          flen = XWINDOWSWINGET_MAX_COL_VALUE - 1;
        }

      if(flen > 0)
        {
          memcpy(tokens[count], &line[start], flen);
          tokens[count][flen] = 0;
        }
       else
        {
          tokens[count][0] = 0;
        }

      count++;

      /* skip the separator run (2 or more spaces / tabs / CR) */
      while(i < len)
        {
          char ch = line[i];
          if((ch == ' ') || (ch == '\t') || (ch == '\r'))
            {
              i++;
            }
           else
            {
              break;
            }
        }
    }

  return count;
}


int XWINDOWSWINGET::ParseWinget(const char* text, XWINDOWSWINGET_TABLE* outtable) 
{
  outtable->columncount = 0;
  outtable->rowcount    = 0;

  if(!text || !outtable)
    {
      return 0;
    }

  char* buffer = (char*)malloc(strlen(text) + 1);
  if(!buffer) 
    {
      return 0;
    }

  strcpy(buffer, text);

  /* remove ANSI color / escape sequences */
  RemoveANSICodes(buffer);

  char* lines[XWINDOWSWINGET_MAX_LINES];
  int   linecount = 0;

  char* tok = strtok(buffer, "\n");
  while(tok && (linecount < XWINDOWSWINGET_MAX_LINES))
    {
      /* skip leading control characters (keep spaces for column alignment) */
      unsigned char* p = (unsigned char*)tok;

      while(*p && (*p < 0x20) && (*p != ' ') && (*p != '\t'))
        {
          p++;
        }

      /* drop empty lines */
      char* line = (char*)p;
      while(*line == ' ' || *line == '\t' || *line == '\r')
        {
          line++;
        }

      if(*line)
        {
          lines[linecount++] = (char*)p;
        }

      tok = strtok(NULL, "\n");
    }

  if(linecount < 2) 
    {
      free(buffer);
      return 0;
    }

  /* locate header and separator line:
     header = line before the first line made of '-' and spaces only */
  int headerindex = -1;
  int sepindex    = -1;

  for(int i = 1; i < linecount; i++)
    {
      if(IsSeparatorLine(lines[i]))
        {
          char* cand = lines[i-1];

          if(IsSeparatorLine(cand))
            {
              continue;
            }

          /* count "words" in the candidate header line to ensure it is not junk */
          int words = 0;
          int inword = 0;

          for(int j = 0; cand[j] != 0; j++)
            {
              char ch = cand[j];
              bool isspacechar = (ch == ' ') || (ch == '\t') || (ch == '\r') || (ch == '\n');

              if(!isspacechar)
                {
                  if(!inword)
                    {
                      inword = 1;
                      words++;
                    }
                }
               else
                {
                  inword = 0;
                }
            }

          if(words >= 2)
            {
              headerindex = i - 1;
              sepindex    = i;
              break;
            }
        }
    }

  if(headerindex < 0)
    {
      /* can't find header */
      free(buffer);
      return 0;
    }

  /* parse header columns */
  char* header = Trim(lines[headerindex]);

  char headerfields[XWINDOWSWINGET_MAX_COLUMNS][XWINDOWSWINGET_MAX_COL_VALUE];
  int  headercount = XWINDOWSWINGET_SplitByMultiSpace(header, headerfields, XWINDOWSWINGET_MAX_COLUMNS);

  if(headercount <= 0)
    {
      free(buffer);
      return 0;
    }

  outtable->columncount = headercount;

  for(int c = 0; c < headercount; c++)
    {
      char* src = headerfields[c];
      char* t   = Trim(src);

      memset(outtable->columns[c].name, 0, sizeof(outtable->columns[c].name));

      if(t)
        {
          strncpy(outtable->columns[c].name, t, XWINDOWSWINGET_MAX_COL_NAME - 1);
        }

      /* start/end are not reliable with proportional fonts; set to 0 */
      outtable->columns[c].start = 0;
      outtable->columns[c].end   = 0;
    }

  /* parse data rows */
  for(int i = sepindex + 1; i < linecount; i++)
    {
      if(outtable->rowcount >= XWINDOWSWINGET_MAX_LINES)
        {
          break;
        }

      char* line = Trim(lines[i]);
      if(!line || !(*line))
        {
          continue;
        }

      int r = outtable->rowcount;

      int colparsed = XWINDOWSWINGET_SplitByMultiSpace(line, outtable->rows[r].values, outtable->columncount);
      if(colparsed <= 0)
        {
          continue;
        }

      /* ensure unused columns are empty */
      for(int c = colparsed; c < outtable->columncount; c++)
        {
          outtable->rows[r].values[c][0] = 0;
        }

      outtable->rowcount++;
    }

  free(buffer);

  return (outtable->columncount > 0 && outtable->rowcount > 0) ? 1 : 0;
}
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int XWINDOWSWINGET::ParseWinget(const char* text, XWINDOWSWINGET_KVLIST* outList) 
* @brief      parse winget 
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  text : 
* @param[in]  outList : 
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int XWINDOWSWINGET::ParseWinget(const char* text, XWINDOWSWINGET_KVLIST* outList) 
{
  outList->count = 0;
  
  char* buffer = (char*)malloc(strlen(text) + 1);
  if(!buffer) 
    {
      return 0;
    }
  
  strcpy(buffer, text);
  
  char* line = strtok(buffer, "\n");
  
  while(line && outList->count < XWINDOWSWINGET_MAX_KV_ITEMS) 
    {        
      char* colon = strchr(line, ':');
      if(colon) 
        {
          *colon = 0;
  
          char* key   = Trim(line);
          char* value = Trim(colon + 1);
  
          if(key[0] != 0 && value[0] != 0) 
            {
              int ki = outList->count;
  
              strncpy(outList->items[ki].key, key, XWINDOWSWINGET_MAX_KEY_LEN - 1);
              outList->items[ki].key[XWINDOWSWINGET_MAX_KEY_LEN - 1] = 0;
  
              strncpy(outList->items[ki].value, value, XWINDOWSWINGET_MAX_VAL_LEN - 1);
              outList->items[ki].value[XWINDOWSWINGET_MAX_VAL_LEN - 1] = 0;
              outList->count++;
            }
        }
  
      line = strtok(NULL, "\n");
    }
  
  free(buffer);
  return (outList->count > 0) ? 1 : 0;
}


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
