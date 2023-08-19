/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XESP32Thread.h
* 
* @class      XESP32THREAD
* @brief      eXtended utils ESP32 mutex & thread class
* @ingroup    PLATFORM_ESP32
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

#ifndef _XESP32THREAD_H_
#define _XESP32THREAD_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XThread.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class XESP32MUTEX : public XMUTEX
{
  public:

                          XESP32MUTEX               ();
    virtual              ~XESP32MUTEX               ();

    virtual bool          Lock                      ();
    virtual bool          UnLock                    ();

  protected:

     void                 Clean                     ();

     XDWORD               counterlock;
};


class XESP32THREAD  : public XTHREAD
{
  public:
                          XESP32THREAD              (XTHREADGROUPID groupID, XCHAR* ID,XTHREADFUNCTION function = NULL,void* data = NULL);
     virtual             ~XESP32THREAD              ();

     virtual bool         Ini                       (bool run = true);
     virtual bool         Wait                      (int miliseconds = XTHREAD_DEFAULTWAITYIELD);
     virtual bool         End                       ();


  private:

     void                 Clean                     ();
};

#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

