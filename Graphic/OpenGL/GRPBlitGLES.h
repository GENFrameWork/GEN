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

    // Computes how the canvas texture maps onto the drawable surface (the fullscreen quad's NDC
    // scale AND position) and whether it should be drawn at all. Default (shared) policy:
    // letterbox/pillarbox, i.e. keep the canvas aspect ratio inside the surface, scaling the quad
    // down on one axis, CENTERED (translatex/translatey = 0), with black bars filling the rest --
    // this rescales the presented bitmap as the surface is resized. Platform specialisations can
    // override this to implement a different presentation policy (see GRPWINDOWSBLITGLES, which
    // presents the canvas at its native pixel size with no rescaling, anchored to the top-left
    // corner instead of centered).
    virtual void                          ComputePresentationScale          (GLsizei surfacewidth, GLsizei surfaceheight, float& scalex, float& scaley, float& translatex, float& translatey, bool& visible);

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

    // Screen-space translation applied AFTER the lboxsx/lboxsy scale (see BuildModelMatrix). 0,0
    // (the default) keeps the presented quad centered, exactly like before. Platform
    // specialisations that anchor the content to a fixed corner instead of centering it (see
    // GRPWINDOWSBLITGLES::ComputePresentationScale) set these to something else.
    float                                 lboxtx;
    float                                 lboxty;

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
