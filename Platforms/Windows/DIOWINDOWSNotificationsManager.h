/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOWINDOWSNotificationsManager.h
* 
* @class      DIOWINDOWSNOTIFICATIONSMANAGER
* @brief      WINDOWS Data Input/Output Notifications manager class
* @ingroup    PLATFORM_WINDOWS
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

#ifndef _DIOWINDOWSNOTIFICATIONSMANAGER_H_
#define _DIOWINDOWSNOTIFICATIONSMANAGER_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#undef XMEMORY_CONTROL_ACTIVE

#pragma region INCLUDES

#include <windows.h>
#include <string>

#include <wintoastlib.h>

#include "XString.h"

#include "DIONotificationsManager.h"


using namespace WinToastLib;

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class DIOWINDOWSNOTIFICATIONSMANAGER_HANDLER : public IWinToastHandler 
{
  public:
            
    void                                      toastActivated                          () const;     
    void                                      toastActivated                          (int actionindex) const;    
    void                                      toastActivated                          (const char* response) const;
    void                                      toastDismissed                          (WinToastDismissalReason state) const;
    void                                      toastFailed                             () const;
};


class DIOWINDOWSNOTIFICATIONSMANAGER : public DIONOTIFICATIONSMANAGER
{
  public:
                                              DIOWINDOWSNOTIFICATIONSMANAGER          ();
    virtual                                  ~DIOWINDOWSNOTIFICATIONSMANAGER          ();

    bool                                      Ini                                     (XCHAR* titleowner, XCHAR* genericapp) override;
    bool                                      Do                                      (DIONOTIFICATION* notification) override;
    bool                                      End                                     () override;

  private:
    
    void                                      Clean                                   ();  

    DIOWINDOWSNOTIFICATIONSMANAGER_HANDLER*   handler; 
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

