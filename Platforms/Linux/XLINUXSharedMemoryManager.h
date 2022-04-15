/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XLINUXSharedMemoryManager.h
*
* @class      XLINUXSHAREDMEMORYMANAGER
* @brief      eXtended LINUX Shared Memory Manager(Exchange between applications)
* @ingroup    PLATFORM_LINUX
*
* @copyright  GEN Group. All right reserved.
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

#ifndef _XLINUXSHAREDMEMORYMANAGER_H_
#define _XLINUXSHAREDMEMORYMANAGER_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XSharedMemoryManager.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class XLINUXSHAREDMEMORYMANAGER : public XSHAREDMEMORYMANAGER
{
  public:
                            XLINUXSHAREDMEMORYMANAGER       ();
    virtual                ~XLINUXSHAREDMEMORYMANAGER       ();

    XBYTE*                  Create                          (XCHAR* ID, XDWORD size, bool ispublic = true);
    XBYTE*                  Open                            (XCHAR* ID, XDWORD& size);

    bool                    Close                           ();

  private:

    void                    Clean                           ();

    int                     handlefile;
};

/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif


