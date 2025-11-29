#pragma once/**-------------------------------------------------------------------------------------------------------------------

@file       XWINDOWSWinget.h

@class      XWINDOWSWINGET
@brief      WINDOWS eXtended Utils WinGet API class
@ingroup    PLATFORM_WINDOWS

@copyright  GEN Group. All rights reserved.

@cond
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files(the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/ or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of
the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
@endcond

--------------------------------------------------------------------------------------------------------------------*/

#ifndef _XWINDOWSWINGET_H_
#define _XWINDOWSWINGET_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XString.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#define XWINDOWSWINGET_MAX_LINES       512
#define XWINDOWSWINGET_MAX_COLUMNS     16
#define XWINDOWSWINGET_MAX_COL_NAME    64
#define XWINDOWSWINGET_MAX_COL_VALUE   256

#define XWINDOWSWINGET_MAX_KV_ITEMS    256
#define XWINDOWSWINGET_MAX_KEY_LEN     64
#define XWINDOWSWINGET_MAX_VAL_LEN     256


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


typedef struct 
{
  char                    name[XWINDOWSWINGET_MAX_COL_NAME];
  int                     start;
  int                     end;

} XWINDOWSWINGET_COLUMN;


typedef struct 
{
  char                    values[XWINDOWSWINGET_MAX_COLUMNS][XWINDOWSWINGET_MAX_COL_VALUE];

} XWINDOWSWINGET_ROW;


typedef struct 
{
  XWINDOWSWINGET_COLUMN   columns[XWINDOWSWINGET_MAX_COLUMNS];
  int                     columncount;

  XWINDOWSWINGET_ROW      rows[XWINDOWSWINGET_MAX_LINES];
  int                     rowcount;

} XWINDOWSWINGET_TABLE;


typedef struct 
{
  char                    key[XWINDOWSWINGET_MAX_KEY_LEN];
  char                    value[XWINDOWSWINGET_MAX_VAL_LEN];

} XWINDOWSWINGET_KVITEM;


typedef struct 
{
  XWINDOWSWINGET_KVITEM   items[XWINDOWSWINGET_MAX_KV_ITEMS];
  int                     count;

} XWINDOWSWINGET_KVLIST;


class XWINDOWSWINGET
{
  public:
                    XWINDOWSWINGET        ();
    virtual        ~XWINDOWSWINGET        ();

    bool            List                  (XCHAR* addparam, XWINDOWSWINGET_TABLE* output);
   
        
  private:
    
    bool            Exec                  (XCHAR* params, XBUFFER& output);

    int             LooksLikeUTF16        (XBYTE* buffer, int size);
    bool            LooksLikeUTF8         (XBYTE* buffer, int size);

    void            ConvertUTF16ToUTF8    (XBYTE* data);
    void            CP437ToASCII          (XBYTE* dst, const XBYTE* src, int max);
    void            NormalizeUnicode      (XBYTE* data);
    void            RemoveANSICodes       (char* s);

    char*           Trim                  (char* s);
    int             IsSeparatorLine       (const char* line);
    int             ParseWinget           (const char* text, XWINDOWSWINGET_TABLE* outtable);
    int             ParseWinget           (const char* text, XWINDOWSWINGET_KVLIST* outlist);

    void            Clean                 ();
};

#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif



