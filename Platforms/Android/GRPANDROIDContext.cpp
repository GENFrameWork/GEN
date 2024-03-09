/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPANDROIDContext.cpp
* 
* @class      GRPANDROIDCONTEXT
* @brief      ANDROID Graphics Context class
* @ingroup    PLATFORM_ANDROID
* 
* @copyright  GEN Group. All rights reserved.
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

/*---- PRECOMPILATION INCLUDES ----------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "GRPANDROIDContext.h"

#include "GRPANDROIDContext.h"
#include "GRPANDROIDScreen.h"

#include "XTrace.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPANDROIDCONTEXT::GRPANDROIDCONTEXT()
* @brief      Constructor
* @ingroup    PLATFORM_ANDROID
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPANDROIDCONTEXT::GRPANDROIDCONTEXT() : GRPCONTEXT()
{

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPANDROIDCONTEXT::~GRPANDROIDCONTEXT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_ANDROID
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPANDROIDCONTEXT::~GRPANDROIDCONTEXT()
{

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPANDROIDCONTEXT::Create(void* handle)
* @brief      Create
* @ingroup    PLATFORM_ANDROID
* 
* @param[in]  handle : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPANDROIDCONTEXT::Create(void* handle)
{
  /*
  GRPANDROIDSCREEN* androidscreen = (GRPANDROIDSCREEN*)this->screen;

  androidscreen->Create(handle);
  // EGL

  // initialize OpenGL ES and EGL

  // Here specify the attributes of the desired configuration.
  // Below, we select an EGLConfig with at least 8 bits per color
  // component compatible with on-screen windows

  static const EGLint   attribute_list[] =  { EGL_RED_SIZE      , 8,
                                              EGL_GREEN_SIZE    , 8,
                                              EGL_BLUE_SIZE     , 8,
                                              EGL_ALPHA_SIZE    , 8,
                                              EGL_SURFACE_TYPE  , EGL_WINDOW_BIT,
                                              EGL_NONE
                                            };


  const EGLint attribute_list[] = {
                                    EGL_RENDERABLE_TYPE , EGL_OPENGL_ES2_BIT  ,
                                    EGL_BLUE_SIZE       ,   8,
                                    EGL_GREEN_SIZE      ,   8,
                                    EGL_RED_SIZE        ,   8,
                                    EGL_DEPTH_SIZE      ,  24,
                                    EGL_SURFACE_TYPE    , EGL_WINDOW_BIT,
                                    EGL_SAMPLE_BUFFERS, (this->multisampling != 0) ? 1 : 0,
                                    EGL_SAMPLES, this->multisampling,
                                    EGL_NONE
                                 };


  EGLint      dummy;
  EGLint      format = WINDOW_FORMAT_RGBA_8888;
  EGLint      nconfig;
  EGLBoolean  result;

  // get an EGL display connection
  display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

  // initialize the EGL display connection
  result = eglInitialize(display, NULL, NULL);

  // get an appropriate EGL frame buffer configuration
  result = eglChooseConfig(display, attribute_list, &config, 1, &nconfig);

  //create an EGL rendering context
  //context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
  //if(nconfig <= 0) return false;

  // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
  // guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
  // As soon as we picked a EGLConfig, we can safely reconfigure the
  // ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.

  if(EGL_FALSE == eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) return false;

  ANativeWindow_setBuffersGeometry(androidscreen->GetAndroidHandle(), 0, 0, format);

  surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)handle, NULL);

  //int attrib_list[3] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
  //context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);
  //
  //if(eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) return false;

  int width;
  int height;

  eglQuerySurface(display, surface, EGL_WIDTH,  (EGLint*)&width);
  eglQuerySurface(display, surface, EGL_HEIGHT, (EGLint*)&height);

  androidscreen->SetWidth(width);
  androidscreen->SetHeight(height);

  //const char* str = (const char*)glGetString(GL_VERSION);
  //
  //XSTRING xstr(str);
  //
  //XTRACE_PRINTCOLOR(2, __L("OpenGL Version: %s"), xstr.Get());
  //XTRACE_PRINTCOLOR(2, __L("Ini screen  Width: %d, Height: %d") , width, height);
  //OPENGLCHECKERROR(__L("Context Creation Error, PreIni"));
  //
  //openglctrl.Ini();
  //
  //GLint depthBufferBits;
  //glGetIntegerv(GL_DEPTH_BITS, &depthBufferBits );
  //XTRACE_PRINTCOLOR(3, __L("Depth buffer bits: %d"), depthBufferBits);

  OPENGLCHECKERROR(__L("OpenGLctrl Error"));

  // Context

  int attrib_list[3] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
  context = eglCreateContext(this->display, this->config, EGL_NO_CONTEXT, attrib_list);

  if(eglMakeCurrent(this->display, this->surface, this->surface, context) == EGL_FALSE) return false;

  const char* str = (const char*)glGetString(GL_VERSION);

  XSTRING xstr(str);

  XTRACE_PRINTCOLOR(2, __L("OpenGL Version: %s"), xstr.Get());
  XTRACE_PRINTCOLOR(2, __L("Ini screen  Width: %d, Height: %d") , androidscreen->GetWidth(), androidscreen->GetHeight());
  OPENGLCHECKERROR(__L("Context Creation Error, PreIni"));

  openglctrl.Ini();

  GLint depthBufferBits;
  glGetIntegerv(GL_DEPTH_BITS, &depthBufferBits );
  XTRACE_PRINTCOLOR(3, __L("Depth buffer bits: %d"), depthBufferBits);

  OPENGLCHECKERROR(__L("OpenGLctrl Error"));
  */

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPANDROIDCONTEXT::Destroy()
* @brief      Destroy
* @ingroup    PLATFORM_ANDROID
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPANDROIDCONTEXT::Destroy()
{
  /*
  GRPANDROIDSCREEN* androidscreen = (GRPANDROIDSCREEN*)this->screen;
  eglDestroyContext(this->display, this->context);
  eglDestroySurface(this->display, this->surface);
  eglTerminate(this->display);
  */

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPANDROIDCONTEXT::IsLost()
* @brief      IsLost
* @ingroup    PLATFORM_ANDROID
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPANDROIDCONTEXT::IsLost()
{
  return false;
  //return eglGetCurrentContext() == EGL_NO_CONTEXT;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPANDROIDCONTEXT::Update()
* @brief      Update
* @ingroup    PLATFORM_ANDROID
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPANDROIDCONTEXT::Update()
{
  /*
  if(!IsLost())
  {
    eglSwapBuffers(display, surface);
  }
  */

  return true;
}


#pragma endregion

