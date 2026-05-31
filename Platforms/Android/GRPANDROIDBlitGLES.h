/**-------------------------------------------------------------------------------------------------------------------
*
* @file       GRPANDROIDBlitGLES.h
*
* @class      GRPANDROIDBLITGLES
* @brief      Android specialisation of GRPBLITGLES. Uses native EGL + GLES3 (NDK).
* @ingroup    PLATFORM_ANDROID
*
* @copyright  EndoraSoft. All rights reserved.
*
* --------------------------------------------------------------------------------------------------------------------*/

#pragma once

#ifdef GRP_OPENGL_ACTIVE

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <android/native_window.h>

#include "GRPBlitGLES.h"


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class GRPANDROIDBLITGLES : public GRPBLITGLES
{
  public:
                                          GRPANDROIDBLITGLES                ();
    virtual                              ~GRPANDROIDBLITGLES                ();

    EGLNativeDisplayType                  GetNativeDisplay                  ();
    EGLNativeWindowType                   GetNativeWindow                   ();
    bool                                  GetNativeWindowSize               (int& width, int& height);

    bool                                  PostCreateHook                    (EGLint native_visual_id);
};


#endif // GRP_OPENGL_ACTIVE
