/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPLINUXScreenX11.h
* 
* @class      GRPLINUXSCREENX11
* @brief      LINUX Graphics X11 Screen class
* @ingroup    PLATFORM_LINUX
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

#ifndef _GRPLINUXSCREENX11_H_
#define _GRPLINUXSCREENX11_H_


#ifdef LINUX_X11_ACTIVE


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES


#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/extensions/Xrandr.h>

#include "GRPScreen.h"


#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


typedef struct
{
   unsigned long   flags;
   unsigned long   functions;
   unsigned long   decorations;
   long            inputMode;
   unsigned long   status;

} HITNS;


class GRPCANVAS;

class GRPLINUXSCREENX11 : public GRPSCREEN
{
  public:
                                          GRPLINUXSCREENX11           ();
    virtual                              ~GRPLINUXSCREENX11           ();

    bool                                  Create                      (bool show);

    bool                                  Update                      ();
    bool                                  Update                      (GRPCANVAS* canvas);
    bool                                  Delete                      ();

    bool                                  Resize                      (int width, int height);

    bool                                  Show                        (bool active);
    bool                                  ShowCursor                  (bool active);

    void*                                 GetHandle                   ();
    
    bool                                  IsDesktop                   ();
    void                                  SetIsDesktop                (bool isdesktop);

    bool                                  HasFocus                    ();

    Display*                              GetDisplay                  ();
    Window*                               GetWindow                   ();

  protected:

    Display*                              display;
    Window                                window;
    Window                                root;

    bool                                  isdesktop;

  private:

    bool                                  Create_Window               (bool show);

    int                                   ScreenResolution            (Display* display, Window root, int xsz, int ysz, int rate, int just_checking);

    XImage*                               CreateXImageFromBuffer      (Display* display, int screen, XBYTE* buffer, int width, int height);

    int                                   GetByteOrder                ();

    void                                  Clean                       ();

    int                                   originalwidth;
    int                                   originalheight;
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif


#endif

