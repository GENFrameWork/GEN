/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XSTM32File_NotUnicode.h
*
* @class      XSTM32FILE_NOTUNICODE
* @brief      STM32 eXtended Not Unicode File class (Use FatFs - Generic FAT file system module)
* @ingroup    PLATFORM_STM32
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

#ifndef _XSTM32FILE_NOTUNICODE_H_
#define _XSTM32FILE_NOTUNICODE_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <ff.h>

#include "XBase.h"
#include "XFile.h"
#include "XPath.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class CIPHER;

class XSTM32FILE_NOTUNICODE : public XFILE
{
  public:
                      XSTM32FILE_NOTUNICODE       ();
    virtual          ~XSTM32FILE_NOTUNICODE       ();

    bool              Exist                       (XCHAR* path);

    bool              Open                        (XCHAR* path, bool readonly = true);
    bool              Create                      (XCHAR* path);

    bool              SetSize                     (XQWORD size);

    bool              GetPosition                 (XQWORD& position);
    bool              SetPosition                 (XQWORD position);

    bool              Read                        (XBYTE* buffer, XDWORD size , CIPHER* cipher = NULL);
    bool              Read                        (XBYTE* buffer, XDWORD* size, CIPHER* cipher = NULL);

    bool              Write                       (XBYTE* buffer, XDWORD size , CIPHER* cipher = NULL);

    bool              Flush                       ();

    bool              Close                       ();

    bool              Erase                       (XCHAR* path, bool overwrite = false);
    bool              Rename                      (XCHAR* pathold, XCHAR* pathnew);
    
    FILE*             CreateStructHandle          ();
    bool              DeleteStructHandle          ();
  
  protected:

    bool              ActualizeSize               ();
     
  private:

    void              Clean                       ();  
    
    FATFS             userFATFS;
    FRESULT           fresult; 
    
    FIL               file;   
    FILINFO           fileinfo;
        
    bool              ismount;    

};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif

