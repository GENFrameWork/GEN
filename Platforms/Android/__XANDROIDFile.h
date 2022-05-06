/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XANDROIDFile.h
*
* @class      XANDROIDFILE
* @brief      ANDROID eXtended File class
* @ingroup    PLATFORM_ANDROID
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

#ifndef _XANDROIDFILE_H_
#define _XANDROIDFILE_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XFile.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XANDROIDFILE : public XFILE
{
  public:
                        XANDROIDFILE            ();
    virtual            ~XANDROIDFILE            ();

    bool                Exist                   (XCHAR* xpath);

    bool                Open                    (XCHAR* xpath, bool readonly = true);
    bool                Create                  (XCHAR* xpath);

    bool                SetSize                 (XQWORD size);

    bool                GetPosition             (XQWORD& position);
    bool                SetPosition             (XQWORD position);

    bool                Read                    (XBYTE* buffer, XDWORD size , CIPHER* cipher = NULL);
    bool                Read                    (XBYTE* buffer, XDWORD* size, CIPHER* cipher = NULL);

    bool                Write                   (XBYTE* buffer, XDWORD size , CIPHER* cipher = NULL);

    bool                Flush                   ();

    bool                Close                   ();

    bool                Erase                   (XCHAR* xpath, bool overwrite = false);
    bool                Rename                  (XCHAR* xpathold, XCHAR* xpathnew);

    FILE*               CreateStructHandle      ();
    bool                DeleteStructHandle      ();

    virtual void        ChangeAttributes        (XCHAR* attributes);

  protected:

    bool                ActualizeSize           ();


  private:

    void                Clean                   ();  

};

/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif

