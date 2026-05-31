/**-------------------------------------------------------------------------------------------------------------------
*
* @file       GRPLINUXBlitGLES.cpp
*
* @class      GRPLINUXBLITGLES
* @brief      Linux X11 specialisation. Provides Display* and Window to EGL.
* @ingroup    PLATFORM_LINUX
*
* @copyright  EndoraSoft. All rights reserved.
*
* --------------------------------------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"

#if defined(GRP_OPENGL_ACTIVE) && defined(LINUX_X11_ACTIVE)


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "GRPLINUXBlitGLES.h"

#include "GRPLINUXScreenX11.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


GRPLINUXBLITGLES::GRPLINUXBLITGLES()  : GRPBLITGLES() { }
GRPLINUXBLITGLES::~GRPLINUXBLITGLES()                 { }


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         EGLNativeDisplayType GRPLINUXBLITGLES::GetNativeDisplay()
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
EGLNativeDisplayType GRPLINUXBLITGLES::GetNativeDisplay()
{
  GRPLINUXSCREENX11* xs = (GRPLINUXSCREENX11*)screen;
  if(!xs) return (EGLNativeDisplayType)EGL_DEFAULT_DISPLAY;
  Display* dpy = xs->GetDisplay();
  return dpy ? (EGLNativeDisplayType)dpy : (EGLNativeDisplayType)EGL_DEFAULT_DISPLAY;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         EGLNativeWindowType GRPLINUXBLITGLES::GetNativeWindow()
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
EGLNativeWindowType GRPLINUXBLITGLES::GetNativeWindow()
{
  GRPLINUXSCREENX11* xs = (GRPLINUXSCREENX11*)screen;
  if(!xs)            return (EGLNativeWindowType)0;
  Window* w = xs->GetWindow();
  return w ? (EGLNativeWindowType)(*w) : (EGLNativeWindowType)0;
}


#endif // GRP_OPENGL_ACTIVE && LINUX_X11_ACTIVE
