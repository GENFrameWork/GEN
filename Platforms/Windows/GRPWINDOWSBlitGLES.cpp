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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"

#ifdef GRP_OPENGL_ACTIVE


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "GRPWINDOWSBlitGLES.h"

#include "GRPWINDOWSScreen.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPWINDOWSBLITGLES::GRPWINDOWSBLITGLES()
* @brief      Constructor of class
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPWINDOWSBLITGLES::GRPWINDOWSBLITGLES() : GRPBLITGLES() 
{ 

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPWINDOWSBLITGLES::~GRPWINDOWSBLITGLES()
* @brief      Destructor of class
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPWINDOWSBLITGLES::~GRPWINDOWSBLITGLES()               
{ 

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         EGLNativeDisplayType GRPWINDOWSBLITGLES::GetNativeDisplay()
* @brief      Get native display
* @ingroup    PLATFORM_WINDOWS
* 
* @return     EGLNativeDisplayType : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
EGLNativeDisplayType GRPWINDOWSBLITGLES::GetNativeDisplay()
{
  GRPWINDOWSSCREEN* ws = (GRPWINDOWSSCREEN*)screen;
  if(!ws) 
    {
      return (EGLNativeDisplayType)EGL_DEFAULT_DISPLAY;
    }

  HDC hdc = (HDC)ws->GetHDC();
  return hdc ? (EGLNativeDisplayType)hdc : (EGLNativeDisplayType)EGL_DEFAULT_DISPLAY;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         EGLNativeWindowType GRPWINDOWSBLITGLES::GetNativeWindow()
* @brief      Get native window
* @ingroup    PLATFORM_WINDOWS
* 
* @return     EGLNativeWindowType : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
EGLNativeWindowType GRPWINDOWSBLITGLES::GetNativeWindow()
{
  GRPWINDOWSSCREEN* ws = (GRPWINDOWSSCREEN*)screen;
  return ws ? (EGLNativeWindowType)ws->GetHandle() : (EGLNativeWindowType)NULL;
}


#endif // GRP_OPENGL_ACTIVE
