/**-------------------------------------------------------------------------------------------------------------------
*
* @file       GRPWINDOWSBlitGLES.cpp
*
* @class      GRPWINDOWSBLITGLES
* @brief      Windows specialisation. ANGLE accepts HDC as EGLNativeDisplayType and HWND as
*             EGLNativeWindowType. The HDC is obtained from the screen at this point.
* @ingroup    PLATFORM_WINDOWS
*
* @copyright  EndoraSoft. All rights reserved.
*
* --------------------------------------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"

#ifdef GRP_OPENGL_ACTIVE


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "GRPWINDOWSBlitGLES.h"

#include "GRPWINDOWSScreen.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


GRPWINDOWSBLITGLES::GRPWINDOWSBLITGLES() : GRPBLITGLES() { }
GRPWINDOWSBLITGLES::~GRPWINDOWSBLITGLES()               { }


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         EGLNativeDisplayType GRPWINDOWSBLITGLES::GetNativeDisplay()
* @ingroup    PLATFORM_WINDOWS
*
* --------------------------------------------------------------------------------------------------------------------*/
EGLNativeDisplayType GRPWINDOWSBLITGLES::GetNativeDisplay()
{
  GRPWINDOWSSCREEN* ws = (GRPWINDOWSSCREEN*)screen;
  if(!ws) return (EGLNativeDisplayType)EGL_DEFAULT_DISPLAY;
  HDC hdc = (HDC)ws->GetHDC();
  return hdc ? (EGLNativeDisplayType)hdc : (EGLNativeDisplayType)EGL_DEFAULT_DISPLAY;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         EGLNativeWindowType GRPWINDOWSBLITGLES::GetNativeWindow()
* @ingroup    PLATFORM_WINDOWS
*
* --------------------------------------------------------------------------------------------------------------------*/
EGLNativeWindowType GRPWINDOWSBLITGLES::GetNativeWindow()
{
  GRPWINDOWSSCREEN* ws = (GRPWINDOWSSCREEN*)screen;
  return ws ? (EGLNativeWindowType)ws->GetHandle() : (EGLNativeWindowType)NULL;
}


#endif // GRP_OPENGL_ACTIVE
