/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOURL.cpp
* 
* @class      DIOURL
* @brief      Data Input/Output URL class
* @ingroup    DATAIO
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
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOURL.h"

#include "XBuffer.h"

#include "DIODNSResolver.h"

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
* @fn         DIOURL::DIOURL()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOURL::DIOURL()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOURL::~DIOURL()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOURL::~DIOURL()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOURL::DIOURL(XDWORD size)
* @brief      Constructor of class
* @ingroup    DATAIO
*
* @param[in]  size :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOURL::DIOURL(XDWORD size) : XSTRING(size)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOURL::DIOURL(const char* url)
* @brief      Constructor of class
* @ingroup    DATAIO
*
* @param[in]  url :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOURL::DIOURL(const char* url) : XSTRING(url)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOURL::DIOURL(const XCHAR* url)
* @brief      Constructor of class
* @ingroup    DATAIO
*
* @param[in]  url :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOURL::DIOURL(const XCHAR* url) : XSTRING(url)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOURL::DIOURL(const XCHAR* url,XDWORD size)
* @brief      Constructor of class
* @ingroup    DATAIO
*
* @param[in]  url :
* @param[in]  size :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOURL::DIOURL(const XCHAR* url,XDWORD size) : XSTRING(url, size)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOURL::DIOURL(DIOURL& url)
* @brief      Constructor of class
* @ingroup    DATAIO
*
* @param[in]  url :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOURL::DIOURL(DIOURL& url) : XSTRING(url)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOURL::DIOURL(XSTRING& url)
* @brief      Constructor of class
* @ingroup    DATAIO
*
* @param[in]  url :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOURL::DIOURL(XSTRING& url) : XSTRING(url)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOURL::DIOURL(XWORD* url)
* @brief      Constructor of class
* @ingroup    DATAIO
*
* @param[in]  url :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOURL::DIOURL(XWORD* url) : XSTRING(url)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::GetHTTPServer(XSTRING& server, XSTRING& login, XSTRING& password)
* @brief      Get HTTP server
* @ingroup    DATAIO
*
* @param[in]  server :
* @param[in]  login :
* @param[in]  password :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::GetHTTPServer(XSTRING& server, XSTRING& login, XSTRING& password)
{
  server.Empty();
  //login.Empty();
  //password.Empty();

  if(IsEmpty()) return false;

  Copy(DIOURL_WEBURLID,__L("/"),true,0,server);
  if(server.IsEmpty())
    {
      int index = Find(__L("/"), true);
      if(index != XSTRING_NOTFOUND)
        {
          Copy(0,index,server);
        }
       else server = Get();
    }

  if(server.IsEmpty()) return false;

  int indexlp = server.Find(__L("@"), true);
  if(indexlp!=XSTRING_NOTFOUND)
    {
      XSTRING  lpstring;

      server.Copy(0, indexlp, lpstring);
      server.DeleteCharacters(0,indexlp+1);

      int indexp = lpstring.Find(__L(":"), true);
      if(indexp!=XSTRING_NOTFOUND)
        {
          lpstring.Copy(0, indexp, login);
          lpstring.Copy(indexp+1 , password);
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::GetHTTPResource(XSTRING& resource)
* @brief      Get HTTP resource
* @ingroup    DATAIO
*
* @param[in]  resource :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::GetHTTPResource(XSTRING& resource)
{
  resource.Empty();

  if(IsEmpty()) return false;

  XSTRING weburlid;

  weburlid = DIOURL_WEBURLID;

  int index = Find(weburlid.Get(), true);
  if(index == XSTRING_NOTFOUND)
        index  = 0;
   else index += weburlid.GetSize();

  index = Find(__L("/"), true,index);
  if(index != XSTRING_NOTFOUND)
    {
      Copy(index,GetSize(),resource);
    }

  if(resource.IsEmpty()) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::IsAURL()
* @brief      Is AURL
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::IsAURL()
{
  //int count = 0;

  for(XDWORD c=0; c<size; c++)
    {
      if(!text[c]) break;
  
      if((text[c] >= __C('0')) && (text[c] <= '9') || (text[c] == __C('.'))) 
        {
          

        }
       else
        {
          return true;        
        }     
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::ResolveURL()
* @brief      Resolve URL
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::ResolveURL()
{
  XSTRING IPstring;

  if(!ResolveURL(IPstring)) return false;

  Set(IPstring.Get());

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::ResolveURL(XSTRING& IPstring)
* @brief      Resolve URL
* @ingroup    DATAIO
*
* @param[in]  IPstring :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::ResolveURL(XSTRING& IPstring)
{
  IPstring.Empty();

  DIOIP IP;
  bool  status = false;
 
  status = GEN_DIODNSRESOLVER.ResolveURL(Get(), IP, DIODNSPROTOCOL_TYPEQUERY_A);
  if(status)  IP.GetXString(IPstring);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::ResolveURL(DIOIP& IP)
* @brief      Resolve URL
* @ingroup    DATAIO
*
* @param[in]  IP :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::ResolveURL(DIOIP& IP)
{
  XSTRING stringIP;

  stringIP.Empty();

  if(!ResolveURL(stringIP)) return false;

  IP.Set(stringIP);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::IsLocalAddress()
* @brief      Is local address
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::IsLocalAddress()
{
  DIOIP IP;

  if(IsAURL())
    {
      ResolveURL(IP);

    } else IP.Set(Get());

  return IP.IsLocal();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::Slash_Add()
* @brief      Slash add
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::Slash_Add()
{
  int    count[2] = { 0 , 0 };
  XCHAR* str      = Get();
  bool   status   = false;

  int c=0;
  while(str[c])
    {
      if(str[c]==__C('/'))  count[0]++;
      if(str[c]==__C('\\')) count[1]++;

      c++;
    }

  if((Character_GetLast()!=__C('\\')) && (Character_GetLast()!=__C('/')))
    {
      if(count[0]>count[1])
             Add("/");
        else Add("\\");

      Slash_Normalize();

      status = true;
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::Slash_Normalize()
* @brief      Slash normalize
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::Slash_Normalize()
{
  if(IsEmpty()) return false;

  XCHAR* str = Get();
  int    c   = 0;

  while(str[c])
    {
      if(str[c]==__C('\\')) str[c] = __C('/');
      c++;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::Slash_Delete()
* @brief      Slash delete
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::Slash_Delete()
{
  bool status = false;

  if((Character_GetLast()==__C('\\')) || (Character_GetLast()==__C('/')))
    {
      DeleteCharacter('\\' , XSTRINGCONTEXT_TO_END);
      DeleteCharacter('/'  , XSTRINGCONTEXT_TO_END);

      status = true;
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::IsUnsafeChar(XCHAR character)
* @brief      Is unsafe char
* @ingroup    DATAIO
*
* @param[in]  character :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::IsUnsafeChar(XCHAR character)
{
  //XSTRING safecharacters(__L("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~!*'();:@=+$,/?[]"));
  XSTRING safecharacters(__L("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_/\\."));

  for(int c=0; c<(int)safecharacters.GetSize(); c++)
    {
      if(character == safecharacters.Get()[c])  return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::EncodeUnsafeChar(XCHAR character, XSTRING& encodechar)
* @brief      Encode unsafe char
* @ingroup    DATAIO
*
* @param[in]  character :
* @param[in]  encodechar :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::EncodeUnsafeChar(XCHAR character, XSTRING& encodechar)
{
  XWORD normalizechar = (XWORD)character;

  encodechar.Empty();

  if(character&0xFF00)
        encodechar.Format(__L("%%%02X%%%02X"), (normalizechar>>8), (normalizechar&0x00FF));
   else encodechar.Format(__L("%%%02X"), normalizechar);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::EncodeUnsafeCharsFromString(XSTRING& string)
* @brief      Encode unsafe chars from string
* @ingroup    DATAIO
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::EncodeUnsafeCharsFromString(XSTRING& string)
{
  Empty();

  for(int c=0; c<(int)string.GetSize(); c++)
    {
      if(!IsUnsafeChar(string.Get()[c]))
        {
          Add(string.Get()[c]);
        }
       else
        {
          XSTRING encodechar;

          encodechar.Empty();

          switch(string.Get()[c])
            {
              case __C('&')  : if(string.Get()[c+1] == __C(' '))
                                     EncodeUnsafeChar(string.Get()[c], encodechar);
                                else Add(string.Get()[c]);
                               break;

              case __C('\\') :
              case __C('%')  :
                    default  : EncodeUnsafeChar(string.Get()[c], encodechar);
                               break;
            }

          if(!encodechar.IsEmpty()) Add(encodechar);

        }
    }

  if(IsEmpty()) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::EncodeUnsafeCharsFromString(const char* string)
* @brief      Encode unsafe chars from string
* @ingroup    DATAIO
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::EncodeUnsafeCharsFromString(const char* string)
{
  XSTRING string2(string);

  return EncodeUnsafeCharsFromString(string2);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::EncodeUnsafeCharsFromString(const XCHAR* string)
* @brief      Encode unsafe chars from string
* @ingroup    DATAIO
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::EncodeUnsafeCharsFromString(const XCHAR* string)
{
  XSTRING string2(string);

  return EncodeUnsafeCharsFromString(string2);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::EncodeUnsafeCharsFromBuffer(XBUFFER& xbuffer)
* @brief      Encode unsafe chars from buffer
* @ingroup    DATAIO
*
* @param[in]  xbuffer :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::EncodeUnsafeCharsFromBuffer(XBUFFER& xbuffer)
{
  Empty();

  for(int c=0; c<(int)xbuffer.GetSize(); c++)
    {
      if(!IsUnsafeChar(xbuffer.Get()[c]))
        {
          Add(xbuffer.Get()[c]);
        }
       else
        {
          XSTRING encodechar;

          encodechar.Empty();

          switch(xbuffer.Get()[c])
            {
              case __C('&')  : if(xbuffer.GetByte(c+1) == __C(' '))
                                     EncodeUnsafeChar(xbuffer.GetByte(c), encodechar);
                                else Add(xbuffer.GetByte(c));
                               break;

              case __C('\\') :
              case __C('%')  :
                    default  : EncodeUnsafeChar(xbuffer.GetByte(c), encodechar);
                               break;
            }

          if(!encodechar.IsEmpty()) Add(encodechar);

        }
    }

  if(IsEmpty()) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::EncodeUnsafeChars()
* @brief      Encode unsafe chars
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::EncodeUnsafeChars()
{
  XSTRING string;

  string = Get();

  if(!EncodeUnsafeCharsFromString(string)) return false;

  return true;

}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::DecodeUnsafeCharsToString(XSTRING& string)
* @brief      Decode unsafe chars to string
* @ingroup    DATAIO
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::DecodeUnsafeCharsToString(XSTRING& string)
{
  string.Empty();

  if(IsEmpty()) return false;

  for(int c=0;c<(int)GetSize();c++)
    {
      XCHAR character[2] = { 0 , 0 };

      character[0] = Get()[c];

      if(character[0]==__C('%'))
        {
          XSTRING   numberhex;
          XCHAR part[2] = { 0 , 0 };

          c++;

          for(int d=0;d<2;d++)
            {
              part[0] = Get()[c+d];
              numberhex.Add(part);
            }

          numberhex.UnFormat(__L("%02X"),&part[0]);

          part[0]&=0x00FF;
          string.Add(part);

          c++;
        }
       else
        {
          if(character[0]==__C('+'))
            {
              string.Add(__C(' '));
            }
           else string.Add(character);
        }
    }

  if(string.IsEmpty()) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::DecodeUnsafeChars()
* @brief      Decode unsafe chars
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::DecodeUnsafeChars()
{
  XSTRING string;

  string = Get();

  if(!DecodeUnsafeCharsToString(string)) return false;

  Set(string);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::Normalize()
* @brief      Normalize
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::Normalize()
{
  XSTRING string;

  string = Get();

  EncodeUnsafeCharsFromString(string);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::HaveHTTPID()
* @brief      Have HTTPID
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::HaveHTTPID()
{
  if(Find(DIOURL_WEBURLID,true)        != XSTRING_NOTFOUND) return true;
  if(Find(DIOURL_WEBURLID_SECURE,true) != XSTRING_NOTFOUND) return true;

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::AddHTTPID()
* @brief      Add HTTPID
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::AddHTTPID()
{
  if(HaveHTTPID()) return false;
  if(IsEmpty())    return false;

  XSTRING url;

  url  = DIOURL_WEBURLID;
  url += Get();

  Set(url);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::DeleteHTTPID()
* @brief      Delete HTTPID
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::DeleteHTTPID()
{
  if(!HaveHTTPID()) return false;
  if(IsEmpty())    return false;

  XSTRING url;
  XSTRING urlID;

  urlID = DIOURL_WEBURLID;
  url   = Get();
  if(url.Find(urlID, true) != XSTRING_NOTFOUND) 
    {
      url.DeleteCharacters(0, urlID.GetSize());
    }
   else 
    {
      urlID = DIOURL_WEBURLID_SECURE;
      if(url.Find(urlID, true) != XSTRING_NOTFOUND) 
        {
          url.DeleteCharacters(0, urlID.GetSize());
        }
    }

  Set(url);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOURL::GetExtension(XSTRING& extension)
* @brief      Get extension
* @ingroup    DATAIO
*
* @param[in]  extension :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::GetExtension(XSTRING& extension)
{
  extension.Empty();

  int indexext;
  for(indexext = GetSize(); indexext>0; indexext--)
    {
      if(Get()[indexext] == __C('.'))
        {
          break;
        }

      if((Get()[indexext] == __C('\\')) ||
          Get()[indexext] == __C('/')   ||
          Get()[indexext] == __C(':'))
        {
          indexext = -1;
          break;
        }
    }

  if(indexext > 0) Copy(indexext, extension);

  return (extension.IsEmpty()?false:true);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOURL::GetFileName(XSTRING& filename)
* @brief      Get file name
* @ingroup    DATAIO
*
* @param[in]  filename : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool DIOURL::GetFileName(XSTRING& filename)
{
  filename.Empty();

  int indexext;
  for(indexext = GetSize(); indexext>0; indexext--)
    {
      if((Get()[indexext] == __C('\\')) ||
          Get()[indexext] == __C('/')   ||
          Get()[indexext] == __C(':'))
        {
          Copy(indexext+1, filename);
          break;
        }
    }

  return (filename.IsEmpty()?false:true);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOURL::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOURL::Clean()
{

}


#pragma endregion
