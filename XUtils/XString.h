/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XString.h
*
* @class      XSTRING
* @brief      eXtended Unicode String class
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

#ifndef _XSTRING_H_
#define _XSTRING_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>

#include "XBase.h"
#include "XVector.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

typedef wchar_t XCHAR;

#define XSTRING_NOTFOUND     NOTFOUND

enum XSTRINGCONTEXT
{
  XSTRINGCONTEXT_FROM_FIRST    = 0 ,
  XSTRINGCONTEXT_TO_END            ,
  XSTRINGCONTEXT_ALLSTRING         ,
};


#pragma pack(push, r1, 1)
typedef struct
{
  XBYTE size;
  char  data[255];

} SHORTSTRING;
#pragma pack(pop, r1)


#ifdef MICROCONTROLLER
#define XSTRING_BLOCKMEM          8
#define XSTRING_MAXTEMPOSTR       128
#define XSTRING_STANDARDSIZE      64
#else
#define XSTRING_BLOCKMEM          64
#define XSTRING_MAXTEMPOSTR       1024
#define XSTRING_STANDARDSIZE      256
#endif

#define XSTRING_SIZETABLE64BITS   64
#define XSTRING_TABLE64BITS       __L("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/")

#define XSTRING_VALIDCHARSINT     __L("0123456789-+");
#define XSTRING_VALIDCHARSFLOAT   __L("0123456789-+.eE");

#define __L(x)          (XCHAR*)(L##x)
#define __C(x)          (XCHAR)(L##x)

#if (defined(LINUX) || defined(ANDROID))
  #define SPRINTF(str, ...)       sprintf(str, ## __VA_ARGS__)
  #define SSCANF(str, ...)        sscanf(str, ## __VA_ARGS__)
#else

  #if defined(WINDOWS)
    #if defined(BUILDER)
      #define SPRINTF             sprintf
      #define SSCANF              sscanf
    #else
      #define SPRINTF(str, ...)   sprintf_s(str, XSTRING_MAXTEMPOSTR-1, ## __VA_ARGS__)
      #define SSCANF(str, ...)    sscanf_s(str, ## __VA_ARGS__)
    #endif
  #endif

#endif

#ifdef MICROCONTROLLER
  #define SPRINTF(str, ...)       sprintf(str, ## __VA_ARGS__)
  #define SSCANF(str, ...)        sscanf(str, ## __VA_ARGS__)
#endif

#define XSTRING_CREATEOEM(xstring, variable)                          char* variable = NULL;                    \
                                                                      xstring.CreateOEM(variable);

#define XSTRING_DELETEOEM(xstring, variable)                          xstring.DeleteOEM(variable);              \
                                                                      variable = NULL;

#define XSTRING_CREATENORMALIZE(xstring, buffernormalize, inverse)    XWORD* buffernormalize = NULL;            \
                                                                      xstring.CreateNormalize(buffernormalize, inverse);

#define XSTRING_DELETENORMALIZE(xstring, buffernormalize)             xstring.DeleteNormalize(buffernormalize); \
                                                                      buffernormalize = NULL;

#define XSTRING_COPYOEMMAXSIZE(xstring, charstr, maxsize)             { XSTRING_CREATEOEM(xstring, charstrbase)                                                       \
                                                                        memcpy(charstr, charstrbase, (xstring.GetSize()>(maxsize-1)?(maxsize-1):xstring.GetSize()));  \
                                                                        XSTRING_DELETEOEM(xstring, charstrbase)                                                       \
                                                                      }

#define XSTRING_COPYOEMARRAYMAXSIZE(xstring, chararray, maxsize)      { XSTRING_CREATEOEM(xstring, chararraybase)                                                     \
                                                                        memcpy(chararray, chararraybase, (xstring.GetSize()>=maxsize?maxsize:xstring.GetSize()));     \
                                                                        XSTRING_DELETEOEM(xstring, chararraybase)                                                     \
                                                                      }

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XBUFFER;
class XMUTEX;

class GEN_API_LIB XSTRING
{
  public:
                          XSTRING                         ();
                          XSTRING                         (XDWORD size);
                          XSTRING                         (const char* string);
                          XSTRING                         (const XCHAR* string);
                          XSTRING                         (const XCHAR* string,XDWORD size);
                          XSTRING                         (const XSTRING& string);
                          XSTRING                         (XWORD* string);

    virtual              ~XSTRING                         ();

    XCHAR*                Get                             () const;
    XDWORD                GetSize                         () const;
    static XDWORD         GetSize                         (const XCHAR* string);
    static XDWORD         GetSize                         (XWORD* string);

    bool                  IsOEM                           ();
    bool                  CreateOEM                       (char*& text) const;
    bool                  DeleteOEM                       (char* test);
    bool                  CreateNormalize                 (XWORD*& text, bool inverse = false);
    bool                  DeleteNormalize                 (XWORD*& _textnormalize);

    bool                  Set                             (XDWORD size);
    bool                  Set                             (const char* string);
    bool                  Set                             (const XCHAR* string);
    bool                  Set                             (const XCHAR* string, XDWORD size);
    bool                  Set                             (const XSTRING& string);
    bool                  Set                             (XWORD* string);
    bool                  Set                             (const XCHAR character);
    static bool           Set                             (XCHAR* str1, const XCHAR* str2);
    bool                  Set                             (const XBYTE* buffer, XDWORD size);
    bool                  Set                             (XBUFFER& buffer);
    bool                  Set                             (const XDWORD* buffer, XDWORD size);

    bool                  Add                             (const char* string);
    bool                  Add                             (const XCHAR* string);
    bool                  Add                             (XSTRING& string);
    bool                  Add                             (XCHAR character);
    static bool           Add                             (XCHAR* str1, const XCHAR* str2);
    bool                  Add                             (const XBYTE* buffer, XDWORD size);
    bool                  Add                             (XBUFFER& buffer);


    void                  operator =                      (const char* string);
    void                  operator =                      (const XCHAR* string);
    void                  operator =                      (const XSTRING& string);
    void                  operator =                      (XWORD* string);
    void                  operator =                      (XBUFFER& string);

    void                  operator +=                     (const char* string);
    void                  operator +=                     (const XCHAR* string);
    void                  operator +=                     (XSTRING& string);
    void                  operator +=                     (XCHAR character);

    bool                  operator <                      (const XSTRING& string) const;
    bool                  operator >                      (XSTRING& string);
    bool                  operator <=                     (XSTRING& string);
    bool                  operator >=                     (XSTRING& string);
    bool                  operator ==                     (const XSTRING& string) const;
    bool                  operator !=                     (XSTRING& string);

    XCHAR                 operator []                     (int position);

    bool                  FillChar                        (XCHAR character = 0);
    bool                  Empty                           ();
    bool                  IsEmpty                         ();

    bool                  HaveCharacters                  ();
    bool                  HaveOnlyNumbers                 ();

    bool                  DeleteCharacters                (XDWORD index, XDWORD ncharacteres);
    bool                  DeleteCharactersToEnd           (XDWORD index);
    bool                  DeleteCharacter                 (XCHAR character, XSTRINGCONTEXT context = XSTRINGCONTEXT_ALLSTRING);
    bool                  DeleteNoCharacters              (XSTRINGCONTEXT context);
    int                   DeleteNoCharacters              (XCHAR* needle=__L("\t\r\n"), int start=0, XSTRINGCONTEXT context= XSTRINGCONTEXT_ALLSTRING);
    bool                  DeleteLastCharacter             ();

    bool                  Character_IsAlpha               (XCHAR character) const;
    bool                  Character_IsUpperCase           (XCHAR character);
    bool                  Character_IsLowerCase           (XCHAR character);
    bool                  Character_IsNumber              (XCHAR character);
    bool                  Character_IsNOT                 (XCHAR character);
    XCHAR                 Character_ToUpper               (XCHAR character) const;
    XCHAR                 Character_ToLower               (XCHAR character) const;
    XCHAR                 Character_GetFirst              ();
    XCHAR                 Character_GetLast               ();
    bool                  Character_Change                (XCHAR source, XCHAR target);

    bool                  IsNumber                        ();               

    bool                  ToUpperCase                     ();
    bool                  ToLowerCase                     ();

    int                   Compare                         (const XCHAR* string, bool ignorecase = false);
    int                   Compare                         (const XCHAR* string, int csize, bool ignorecase= false);
    int                   Compare                         (const XSTRING& string, bool ignorecase = false) const;
    int                   Compare                         (const char* string, bool ignorecase = false);
    static int            Compare                         (const XCHAR* string, const XCHAR* string2, bool ignorecase = false);

    int                   Find                            (const XCHAR* string, bool ignorecase, int startindex = 0);
    int                   Find                            (XSTRING& string, bool ignorecase, int startindex = 0);
    int                   FindCharacter                   (XCHAR character, XDWORD startindex = 0, bool endtostart = false);
    int                   FindDiferentCharacter           (XCHAR character, int startindex = 0, bool endtostart = false);
    XDWORD                CountCharacter                  (XCHAR character, int startindex = 0, bool endtostart = false);

    int                   GetNCharacters                  (XCHAR character);

    bool                  AreValidCharacters              (XCHAR* validcharacterslist);

    int                   Copy                            (const XCHAR* startmark, const XCHAR* endmark, bool ignorecase , int addstartindex, XSTRING& string);
    int                   Copy                            (int startindex, const XCHAR* endmark, bool ignorecase, XSTRING& string);
    int                   Copy                            (const XCHAR* startmark, int endindex, bool ignorecase, int startindex, XSTRING& string);
    int                   Copy                            (int startindex, int endindex, XSTRING& string);
    int                   Copy                            (int startindex, XSTRING& string);

    int                   Replace                         (XCHAR* findWhat, XCHAR* replaceBy);
    int                   ReplaceFirst                    (XCHAR* findWhat, XCHAR* replaceBy);
    bool                  Insert                          (XSTRING str, XDWORD position);

    bool                  ConvertFromInt                  (int value, const XCHAR* mask = NULL);
    bool                  ConvertFromDWord                (XDWORD value, const XCHAR* mask = NULL);
    bool                  ConvertFromQWord                (XQWORD value, const XCHAR* mask = NULL);
    bool                  ConvertFromLongLong             (long long value, const XCHAR* mask = NULL);
    bool                  ConvertFromFloat                (float value, const XCHAR* mask = NULL);
    bool                  ConvertFromDouble               (double value, const XCHAR* mask = NULL);
    bool                  ConvertFromBoolean              (bool boolean, bool uppercase = false);
    bool                  ConvertFromXBUFFER              (XBUFFER& buffer);

    bool                  ConvertHexFormatChars           ();
    bool                  ConvertToHexString              (XSTRING& string, bool uppercase = true);
    bool                  ConvertHexStringToBuffer        (XBUFFER& xbuffer);
    bool                  ConvertHexStringFromBuffer      (XBUFFER& xbuffer, bool upercase = true);

    int                   ConvertToInt                    (int index = 0, const XCHAR* mask = NULL, bool checkvalidchars = true);
    XDWORD                ConvertToDWord                  (int index = 0, const XCHAR* mask = NULL, bool checkvalidchars = true);
    XQWORD                ConvertToQWord                  (int index = 0, const XCHAR* mask = NULL, bool checkvalidchars = true);
    float                 ConvertToFloat                  (int index = 0, const XCHAR* mask = NULL, bool checkvalidchars = true);
    double                ConvertToDouble                 (int index = 0, const XCHAR* mask = NULL, bool checkvalidchars = true);
    bool                  ConvertToXBUFFER                (XBUFFER& buffer);
    bool                  ConvertToBoolean                ();

    bool                  ConvertFromWide                 (XWORD* widechars, XDWORD maxsize);

    bool                  ConvertFromUTF8                 (XBYTE* data, XDWORD size);
    bool                  ConvertFromUTF8                 (XBUFFER& xbuffer);
    int                   GetSizeConvertToUTF8            ();
    bool                  ConvertToUTF8                   (XBYTE* data, int& size);
    bool                  ConvertToUTF8                   (XBUFFER& xbuffer);

    bool                  ConvertToBase64                 (XSTRING& string);
    bool                  ConvertFromBase64               (XSTRING& string);

    bool                  ConvertToPascal                 (SHORTSTRING& pascalstring);
    bool                  ConvertFromPascal               (SHORTSTRING& pascalstring);

    bool                  ConvertFromDoubleToSpanishText  (double value, bool withintegerpart, bool withdecimalpart, double& integerpart, double& decimalpart);

    bool                  Swab                            ();

    bool                  Split                           (XCHAR separator, XVECTOR<XSTRING*>& results,  bool addsubstringempty = false);

    bool                  FormatArg                       (const XCHAR* mask, va_list* arg, bool isspecialweb = false);
    bool                  Format                          (const XCHAR* mask, ...);
    bool                  AddFormat                       (const XCHAR* mask, ...);
    bool                  UnFormat                        (const XCHAR* mask, ...);

    int                   FindCharacterFromSet            (const XCHAR* set);

    bool                  AdjustSize                      (XDWORD size, bool ahead = false, XCHAR* characters = NULL, bool addstring = false);
    bool                  AdjustSize                      ();

    bool                  BinaryCompare                   (XSTRING& string);

    bool                  Explode                         (XCHAR character, XVECTOR<XSTRING*>* receive);

    bool                  GetTypeOfLineEnd                (XSTRING& lineend);


    static XBYTE          ConvertXCHARToBase64            (XCHAR character);

  protected: 

    bool                  ReAllocBuffer                   (XDWORD size);
    bool                  FreeBuffer                      ();

    XCHAR                 ConvertIndexBase64ToXCHAR       (int index); 
    bool                  ConvertStringWithMask           (XCHAR* mask, XCHAR* string, XCHAR* result);


    XCHAR*                text;
    XDWORD                size;
    XDWORD                sizemem;

    XMUTEX*               xmutexfreemen;

    int*                  intvalue;
    double*               doublevalue;
    XCHAR*                xcharvalue;

    static XCHAR          table64bits[XSTRING_SIZETABLE64BITS];
    static bool           istable64bitsinit;

  private:

    void                  Clean                           ();
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif


