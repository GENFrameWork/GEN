/**-------------------------------------------------------------------------------------------------------------------
*
* @file       GRPWINDOWSBlitGLES.h
*
* @class      GRPWINDOWSBLITGLES
* @brief      Windows specialisation of GRPBLITGLES. Uses ANGLE to translate GLES3 -> D3D11.
* @ingroup    PLATFORM_WINDOWS
*
* @copyright  EndoraSoft. All rights reserved.
*
* --------------------------------------------------------------------------------------------------------------------*/

#pragma once

#ifdef GRP_OPENGL_ACTIVE

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <Windows.h>

#include "GRPBlitGLES.h"


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class GRPWINDOWSBLITGLES : public GRPBLITGLES
{
  public:
                                          GRPWINDOWSBLITGLES                ();
    virtual                              ~GRPWINDOWSBLITGLES                ();

    EGLNativeDisplayType                  GetNativeDisplay                  ();
    EGLNativeWindowType                   GetNativeWindow                   ();
};


#endif // GRP_OPENGL_ACTIVE
