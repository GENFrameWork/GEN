/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       INPCapture.h
* 
* @class      INPCAPTURE
* @brief      Input Capture class
* @ingroup    INPUT
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

#ifndef _INPCAPTURE_H_
#define _INPCAPTURE_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include <stdio.h>

#include "XBase.h"
#include "XBuffer.h"
#include "XString.h"
#include "XSubject.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

#define INPCAPTURE_DEFAULTLIMIT  10000

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class INPCAPTURE : public XSUBJECT
{
  public:
                                      INPCAPTURE                  ();                                      
    virtual                          ~INPCAPTURE                  ();

    void                              Application_SetHandle       (void* applicationhandle);

    virtual bool                      Activate                    ();
    virtual bool                      Deactivate                  ();
    
    XDWORD                            GetNKeys                    ();   
    void                              SetNKeys                    (XDWORD nkeys); 

    XDWORD                            GetLimit                    ();
    void                              SetLimit                    (XDWORD limit = INPCAPTURE_DEFAULTLIMIT);

    XBUFFER*                          GetBuffer                   ();
    XSTRING*                          GetString                   ();

  protected: 

    void*                             applicationhandle; 
    
    XDWORD                            nkeys; 
    XDWORD                            limit;
    XBUFFER                           buffer;
    XSTRING                           string; 

  private:

    void                              Clean                       ();      
};

#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

