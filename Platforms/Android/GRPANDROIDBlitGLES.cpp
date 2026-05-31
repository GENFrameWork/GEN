/**-------------------------------------------------------------------------------------------------------------------
*
* @file       GRPANDROIDBlitGLES.cpp
*
* @class      GRPANDROIDBLITGLES
* @brief      Android specialisation. Provides ANativeWindow* to EGL and reconfigures the
*             buffer geometry to the EGL-chosen native visual id (this is mandatory on Android).
* @ingroup    PLATFORM_ANDROID
*
* @copyright  EndoraSoft. All rights reserved.
*
* --------------------------------------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"

#ifdef GRP_OPENGL_ACTIVE


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "GRPANDROIDBlitGLES.h"

#include "GRPANDROIDScreen.h"
#include "GRPEGLContext.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


GRPANDROIDBLITGLES::GRPANDROIDBLITGLES()  : GRPBLITGLES() { }
GRPANDROIDBLITGLES::~GRPANDROIDBLITGLES()                 { }


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         EGLNativeDisplayType GRPANDROIDBLITGLES::GetNativeDisplay()
* @ingroup    PLATFORM_ANDROID
*
* --------------------------------------------------------------------------------------------------------------------*/
EGLNativeDisplayType GRPANDROIDBLITGLES::GetNativeDisplay()
{
  return (EGLNativeDisplayType)EGL_DEFAULT_DISPLAY;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         EGLNativeWindowType GRPANDROIDBLITGLES::GetNativeWindow()
* @ingroup    PLATFORM_ANDROID
*
* --------------------------------------------------------------------------------------------------------------------*/
EGLNativeWindowType GRPANDROIDBLITGLES::GetNativeWindow()
{
  GRPANDROIDSCREEN* as = (GRPANDROIDSCREEN*)screen;
  if(!as) return (EGLNativeWindowType)NULL;
  return (EGLNativeWindowType)as->GetAndroidHandle();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPANDROIDBLITGLES::GetNativeWindowSize(int&, int&)
* @brief      Report the CURRENT ANativeWindow size. Used by the base blitter to detect a
*             rotation/resize and recreate the EGL surface accordingly.
* @ingroup    PLATFORM_ANDROID
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPANDROIDBLITGLES::GetNativeWindowSize(int& width, int& height)
{
  GRPANDROIDSCREEN* as = (GRPANDROIDSCREEN*)screen;
  if(!as) return false;

  ANativeWindow* awin = (ANativeWindow*)as->GetAndroidHandle();
  if(!awin) return false;

  width  = ANativeWindow_getWidth(awin);
  height = ANativeWindow_getHeight(awin);
  return (width > 0 && height > 0);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPANDROIDBLITGLES::PostCreateHook(EGLint)
* @brief      On Android the ANativeWindow buffer geometry MUST be reconfigured to the
*             native visual id selected by EGL before the surface is used. Otherwise the
*             driver may either fail or silently mismatch the format.
* @ingroup    PLATFORM_ANDROID
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPANDROIDBLITGLES::PostCreateHook(EGLint native_visual_id)
{
  if(native_visual_id == 0 && eglctx)
    {
      EGLDisplay d = eglctx->GetEGLDisplay();
      EGLConfig  c = eglctx->GetEGLConfig();
      eglGetConfigAttrib(d, c, EGL_NATIVE_VISUAL_ID, &native_visual_id);
    }

  ANativeWindow* awin = (ANativeWindow*)GetNativeWindow();
  if(!awin) return false;

  ANativeWindow_setBuffersGeometry(awin, 0, 0, native_visual_id);

  // On Android the canvas reaches the texture with the opposite ROW order
  // compared to the screen (bottom-up vs top-down), so only the vertical axis
  // needs flipping. The horizontal axis is already correct.
  SetFlipX(false);
  SetFlipY(true);

  return true;
}


#endif // GRP_OPENGL_ACTIVE
