/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XSAMD5xE5xFactory.h
*
* @class      XSAMD5XE5XFACTORY
* @brief       SAMD5xE5x  platform factory class
* @ingroup    PLATFORM_SAMD5XE5X
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

#ifndef _XSAMD5XE5XFACTORY_H_
#define _XSAMD5XE5XFACTORY_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XFactory.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XSAMD5XE5XFACTORY : public XFACTORY
{
  public:

    XTIMER*             CreateTimer             ();
    bool                DeleteTimer             (XTIMER* timer);

    XDATETIME*          CreateDateTime          ();
    bool                DeleteDateTime          (XDATETIME* time);

    XRAND*              CreateRand              ();
    bool                DeleteRand              (XRAND* rand);

    XSYSTEM*            CreateSystem            ();
    bool                DeleteSystem            (XSYSTEM* system);

    XMUTEX*             Create_Mutex            ();
    bool                Delete_Mutex            (XMUTEX* phone);

    XTHREAD*            CreateThread            (XTHREADGROUPID groupID, XCHAR* ID,XTHREADFUNCTION function = NULL,void* data = NULL);
    bool                DeleteThread            (XTHREADGROUPID groupID, XTHREAD* xthread);

    XFLASHMEMORY*       Create_FlashMemory      ();
    virtual bool        Delete_FlashMemory      (XFLASHMEMORY* flashmemory);
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif



