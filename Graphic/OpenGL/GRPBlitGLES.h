/**-------------------------------------------------------------------------------------------------------------------
*
* @file       GRPBlitGLES.h
*
* @class      GRPBLITGLES
* @brief      OpenGL ES 3.0 buffer blitter. Cross-platform abstraction that takes a GRP2DCANVAS
*             pixel buffer (BGRA8888) and renders it as a fullscreen textured quad.
*             Platform specialisations (Windows/Linux/Android) only provide native window/display
*             handles via virtual methods.
* @ingroup    GRAPHIC
*
* @copyright  EndoraSoft. All rights reserved.
*
* --------------------------------------------------------------------------------------------------------------------*/

#pragma once

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#ifdef GRP_OPENGL_ACTIVE

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

#include "XBase.h"
#include "GRPScreen.h"


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


class GRP2DCANVAS;
class GRPEGLCONTEXT;


class GRPBLITGLES
{
  public:
                                          GRPBLITGLES                       ();
    virtual                              ~GRPBLITGLES                       ();

    virtual EGLNativeDisplayType          GetNativeDisplay                  () = 0;
    virtual EGLNativeWindowType           GetNativeWindow                   () = 0;

    virtual bool                          GetNativeWindowSize               (int& width, int& height);

    virtual bool                          PreCreateHook                     ();
    virtual bool                          PostCreateHook                    (EGLint native_visual_id);
    
    bool                                  ChooseVisualID                    (EGLNativeDisplayType native_display, EGLint& out_native_visual_id);


    bool                                  Create                            (GRPSCREEN* screen);
    bool                                  Resize                            (int width, int height);
    bool                                  Update                            (GRP2DCANVAS* canvas);
    bool                                  SwapBuffers                       ();
    bool                                  Destroy                           ();


    void                                  SetUseVSync                       (bool active);
    bool                                  GetUseVSync                       ();

    void                                  SetUsePBO                         (bool active);  // requires ES 3.0
    bool                                  GetUsePBO                         ();

    void                                  SetFlipY                          (bool active);
    bool                                  GetFlipY                          ();

    void                                  SetFlipX                          (bool active);
    bool                                  GetFlipX                          ();

    void                                  SetRotation                       (GRPSCREENROTATION rotation);
    GRPSCREENROTATION                     GetRotation                       ();

    void                                  SetUseAlpha                       (bool active);
    bool                                  GetUseAlpha                       ();
    
    bool                                  IsES3                             ();
    bool                                  HasBGRAExtension                  ();
    GRPEGLCONTEXT*                        GetEGLContext                     ();

  protected:

    GRPSCREEN*                            screen;
    GRPEGLCONTEXT*                        eglctx;

    GLuint                                texid;
    GLuint                                vbo;
    GLuint                                vao;        // ES 3.0 only
    GLuint                                program;
    GLint                                 u_modelview;
    GLint                                 u_tex;
    GLint                                 a_pos;
    GLint                                 a_uv;

    
    GLuint                                pbo[2];
    int                                   pbo_index;
    XDWORD                                pbo_size;
    
    int                                   texw;
    int                                   texh;
    bool                                  usevsync;
    bool                                  usepbo;
    bool                                  flipy;
    bool                                  flipx;
    bool                                  usealpha;
    bool                                  hasbgraext;
    GRPSCREENROTATION                     rotation;

    float                                 lboxsx;
    float                                 lboxsy;

  private:

    bool                                  CompileShaders                    ();
    bool                                  BuildQuad                         ();
    bool                                  AllocTexture                      (int width, int height);
    bool                                  AllocPBOs                         (int width, int height);

    void                                  BuildModelMatrix                  (float* m4x4);

    bool                                  DetectBGRAExtension               ();
    GLuint                                CompileShader                     (GLenum stage, const char* source);

    void                                  Clean                             ();
};


#endif // GRP_OPENGL_ACTIVE
