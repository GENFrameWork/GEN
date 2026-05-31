/**-------------------------------------------------------------------------------------------------------------------
*
* @file       GRPEGLContext.h
*
* @class      GRPEGLCONTEXT
* @brief      EGL context manager. Cross-platform OpenGL ES 3.0 context creation/management.
* @ingroup    GRAPHIC
*
* @copyright  EndoraSoft. All rights reserved.
*
* --------------------------------------------------------------------------------------------------------------------*/

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#ifdef GRP_OPENGL_ACTIVE

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "XBase.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define GRPEGLCONTEXT_DEFAULT_RED_SIZE        8
#define GRPEGLCONTEXT_DEFAULT_GREEN_SIZE      8
#define GRPEGLCONTEXT_DEFAULT_BLUE_SIZE       8
#define GRPEGLCONTEXT_DEFAULT_ALPHA_SIZE      8
#define GRPEGLCONTEXT_DEFAULT_DEPTH_SIZE      0
#define GRPEGLCONTEXT_DEFAULT_STENCIL_SIZE    0


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class GRPEGLCONTEXT
{
  public:
                                          GRPEGLCONTEXT                         ();
    virtual                              ~GRPEGLCONTEXT                         ();

    bool                                  ChooseConfig                          (EGLNativeDisplayType native_display,
                                                                                 bool with_alpha,
                                                                                 EGLint& out_native_visual_id);

    bool                                  Create                                (EGLNativeDisplayType native_display,
                                                                                 EGLNativeWindowType  native_window,
                                                                                 bool with_alpha = true);

    bool                                  MakeCurrent                           ();
    bool                                  RecreateSurface                       (EGLNativeWindowType native_window);
    bool                                  Release                               ();
    bool                                  SwapBuffers                           ();
    bool                                  SetSwapInterval                       (int interval);
    bool                                  Destroy                               ();

    bool                                  IsValid                               ();
    EGLint                                GetClientVersion                      ();
    bool                                  HasExtension                          (const char* name);

    EGLDisplay                            GetEGLDisplay                         ();
    EGLContext                            GetEGLContext                         ();
    EGLSurface                            GetEGLSurface                         ();
    EGLConfig                             GetEGLConfig                          ();

  private:

    EGLDisplay                            display;
    EGLContext                            context;
    EGLSurface                            surface;
    EGLConfig                             config;
    EGLint                                client_version;
    bool                                  isvalid;

    void                                  Clean                                 ();
};


#endif // GRP_OPENGL_ACTIVE
