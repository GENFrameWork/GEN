/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       INPFactory.h
* 
* @class      INPFACTORY
* @brief      INPUT Factory class
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

#ifndef _INPFACTORY_H_
#define _INPFACTORY_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "INPDevice.h"
#include "INPSimulate.h"
#include "INPCapture.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class INPDEVICE;

class INPFACTORY
{
  public:
                                    INPFACTORY                ();
    virtual                        ~INPFACTORY                ();

    static bool                     GetIsInstanced            ();
    static INPFACTORY&              GetInstance               ();
    static bool                     SetInstance               (INPFACTORY* instance);
    static bool                     DelInstance               ();

    virtual INPDEVICE*              CreateDevice              (INPDEVICE_TYPE type, void* param = NULL);
    virtual bool                    DeleteDevice              (INPDEVICE* device);

    #ifdef INP_SIMULATE_ACTIVE
    virtual INPSIMULATE*            CreateSimulator           ();
    virtual bool                    DeleteSimulator           (INPSIMULATE* inputsimulated);
    #endif

    #ifdef INP_CAPTURE_ACTIVE  
    virtual INPCAPTURE*             CreateCapture             ();
    virtual bool                    DeleteCapture             (INPCAPTURE* inputcapture);  
    #endif

  private:

    void                            Clean                     ();

    static INPFACTORY*              instance;
};

#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

