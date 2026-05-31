/**-------------------------------------------------------------------------------------------------------------------
*
* @file       GRPBlitGLES.h
*
* @class      GRPBLITGLES
* @brief      OpenGL ES 3.0 buffer blitter. Cross-platform abstraction that takes a GRPCANVAS
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


class GRPCANVAS;
class GRPEGLCONTEXT;


class GRPBLITGLES
{
  public:
                                          GRPBLITGLES                       ();
    virtual                              ~GRPBLITGLES                       ();

    /* Platform contract --------------------------------------------------------------------------*/

    virtual EGLNativeDisplayType          GetNativeDisplay                  () = 0;
    virtual EGLNativeWindowType           GetNativeWindow                   () = 0;

    /* Optional: report the CURRENT native window drawable size. Used to detect a window/surface */
    /* size change (e.g. Android device rotation) so the EGL surface can be recreated. Return    */
    /* false (default) when the platform has no rotation/resize concern (Windows/Linux): the      */
    /* blitter then trusts the EGL surface size as reported by eglQuerySurface.                   */
    virtual bool                          GetNativeWindowSize               (int& width, int& height);

    /* Optional hooks (default no-op). PreCreate runs BEFORE EGL is created                        */
    /* (e.g. for any platform-specific window setup). PostCreate runs AFTER (e.g. Android needs    */
    /* to reconfigure the ANativeWindow geometry to the chosen EGL visual id).                    */
    virtual bool                          PreCreateHook                     ();
    virtual bool                          PostCreateHook                    (EGLint native_visual_id);

    /* Visual selection (used by Linux X11 BEFORE creating the window) ----------------------------*/

    bool                                  ChooseVisualID                    (EGLNativeDisplayType native_display,
                                                                             EGLint& out_native_visual_id);

    /* Main API -----------------------------------------------------------------------------------*/

    bool                                  Create                            (GRPSCREEN* screen);
    bool                                  Resize                            (int width, int height);
    bool                                  Update                            (GRPCANVAS* canvas);
    bool                                  SwapBuffers                       ();
    bool                                  Destroy                           ();

    /* Runtime configuration ----------------------------------------------------------------------*/

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

    /* Capability queries -------------------------------------------------------------------------*/

    bool                                  IsES3                             ();
    bool                                  HasBGRAExtension                  ();
    GRPEGLCONTEXT*                        GetEGLContext                     ();

  protected:

    GRPSCREEN*                            screen;
    GRPEGLCONTEXT*                        eglctx;

    /* GL objects ---------------------------------------------------------------------------------*/
    GLuint                                texid;
    GLuint                                vbo;
    GLuint                                vao;        // ES 3.0 only
    GLuint                                program;
    GLint                                 u_modelview;
    GLint                                 u_tex;
    GLint                                 a_pos;
    GLint                                 a_uv;

    /* Pixel Buffer Object ring (ES 3.0 only) -----------------------------------------------------*/
    GLuint                                pbo[2];
    int                                   pbo_index;
    XDWORD                                pbo_size;

    /* State --------------------------------------------------------------------------------------*/
    int                                   texw;
    int                                   texh;
    bool                                  usevsync;
    bool                                  usepbo;
    bool                                  flipy;
    bool                                  flipx;
    bool                                  usealpha;
    bool                                  hasbgraext;
    GRPSCREENROTATION                     rotation;

    /* Letterbox/pillarbox scale (NDC) applied so the canvas keeps its aspect ratio when the EGL */
    /* surface has a different aspect (e.g. a 1024x768 canvas on a 2712x1220 device screen).      */
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
