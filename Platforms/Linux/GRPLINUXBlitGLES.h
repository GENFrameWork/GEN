/**-------------------------------------------------------------------------------------------------------------------
*
* @file       GRPLINUXBlitGLES.h
*
* @class      GRPLINUXBLITGLES
* @brief      Linux X11 specialisation of GRPBLITGLES (uses Mesa EGL).
*             Works on PC Linux and Raspberry Pi 4/5 with Bullseye+ (KMS V3D).
* @ingroup    PLATFORM_LINUX
*
* @copyright  EndoraSoft. All rights reserved.
*
* --------------------------------------------------------------------------------------------------------------------*/

#pragma once

#if defined(GRP_OPENGL_ACTIVE) && defined(LINUX_X11_ACTIVE)

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <X11/X.h>
#include <X11/Xlib.h>

#include "GRPBlitGLES.h"


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class GRPLINUXBLITGLES : public GRPBLITGLES
{
  public:
                                          GRPLINUXBLITGLES                  ();
    virtual                              ~GRPLINUXBLITGLES                  ();

    EGLNativeDisplayType                  GetNativeDisplay                  ();
    EGLNativeWindowType                   GetNativeWindow                   ();
};


#endif // GRP_OPENGL_ACTIVE && LINUX_X11_ACTIVE
