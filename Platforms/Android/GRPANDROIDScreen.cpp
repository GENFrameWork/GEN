/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPANDROIDScreen.cpp
* 
* @class      GRPANDROIDSCREEN
* @brief      ANDROID Graphics Screen class
* @ingroup    PLATFORM_ANDROID
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



/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "GRPANDROIDScreen.h"

#include "XTrace.h"

#include "GRPCanvas.h"



#ifdef GRP_OPENGL_ACTIVE
#include "GRPANDROIDBlitGLES.h"
#endif



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPANDROIDSCREEN::GRPANDROIDSCREEN()
* @brief      Constructor of class
* @ingroup    PLATFORM_ANDROID
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPANDROIDSCREEN::GRPANDROIDSCREEN(): GRPSCREEN()
{
  Clean();

  type = GRPSCREENTYPE_ANDROID;

  SetMode(GRPPROPERTYMODE_32_RGBA_8888);

  SetIsBufferInverse(true);

  Styles_Set(GRPSCREENSTYLE_FULLSCREEN);

  isvalid = true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPANDROIDSCREEN::~GRPANDROIDSCREEN()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_ANDROID
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPANDROIDSCREEN::~GRPANDROIDSCREEN()
{
  Delete();
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPANDROIDSCREEN::Create(bool show)
* @brief      Create
* @ingroup    PLATFORM_ANDROID
*
* @param[in]  show :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPANDROIDSCREEN::Create(bool show)
{
  return GRPSCREEN::Create(show);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPANDROIDSCREEN::Update()
* @brief      Update
* @ingroup    PLATFORM_ANDROID
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPANDROIDSCREEN::Update()
{
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPANDROIDSCREEN::Update(GRPCANVAS* canvas)
* @brief      Update
* @ingroup    PLATFORM_ANDROID
*
* @param[in]  canvas :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPANDROIDSCREEN::Update(GRPCANVAS* canvas)
{
  if(!canvas)        return false;
  if(!anativehandle) return false;

  #ifndef GRP_OPENGL_ACTIVE

  ANativeWindow_Buffer abuffer;

  if(IsEqualSizeTo(canvas) == ISEQUAL)
    {
      if(ANativeWindow_lock(anativehandle, &abuffer, NULL) >= 0)
        {
          memcpy(abuffer.bits, canvas->Buffer_Get(), height * width * GetBytesperPixel());
          ANativeWindow_unlockAndPost(anativehandle);
        }
    }
   else
    {
      canvas->CopyBufferRenderToScreen(this);

      if(ANativeWindow_lock(anativehandle, &abuffer, NULL) >= 0)
        {
          memcpy((XBYTE*)abuffer.bits, canvas->Buffer_Get(), height * width * GetBytesperPixel());

          ANativeWindow_unlockAndPost(anativehandle);
        }

      /*
      if(ANativeWindow_lock(anativehandle, &abuffer, NULL) >= 0)
        {
          canvas->CopyBufferRenderToBufferScreen((XBYTE*)abuffer.bits, width, height, isbufferinverse);

          ANativeWindow_unlockAndPost(anativehandle);
        }
      */
    }

  return true;

  #else

  // On Android the native window handle arrives via SetAndroidHandle() AFTER
  // the activity is in the foreground; so blitter creation is deferred to the
  // first Update() call.
  if(!blitgles)
    {
      blitgles = GEN_NEW GRPANDROIDBLITGLES();
      if(!blitgles) return false;

      if(!blitgles->Create(this))
        {
          XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[Screen Android] BlitGLES create failed"));
          GEN_DELETE blitgles;
          blitgles = NULL;
          return false;
        }
    }

  return blitgles->Update(canvas);

  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPANDROIDSCREEN::Delete()
* @brief      Delete
* @ingroup    PLATFORM_ANDROID
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPANDROIDSCREEN::Delete()
{
  #ifdef GRP_OPENGL_ACTIVE
  if(blitgles)
    {
      blitgles->Destroy();
      GEN_DELETE blitgles;
      blitgles = NULL;
    }
  #endif

  isactive = false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPANDROIDSCREEN::Resize(int width, int height)
* @brief      Resize
* @ingroup    PLATFORM_ANDROID
*
* @param[in]  width :
* @param[in]  height :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPANDROIDSCREEN::Resize(int width, int height)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void* GRPANDROIDSCREEN::GetHandle()
* @brief      Get handle
* @ingroup    PLATFORM_ANDROID
*
* @return     void* :
*
* --------------------------------------------------------------------------------------------------------------------*/
void* GRPANDROIDSCREEN::GetHandle()
{
  return (void*)anativehandle;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         ANativeWindow* GRPANDROIDSCREEN::GetAndroidHandle()
* @brief      Get android handle
* @ingroup    PLATFORM_ANDROID
*
* @return     ANativeWindow* :
*
* --------------------------------------------------------------------------------------------------------------------*/
ANativeWindow* GRPANDROIDSCREEN::GetAndroidHandle()
{
  return anativehandle;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRPANDROIDSCREEN::SetAndroidHandle(ANativeWindow* anativehandle)
* @brief      Set android handle
* @ingroup    PLATFORM_ANDROID
*
* @param[in]  anativehandle :
*
* --------------------------------------------------------------------------------------------------------------------*/
void GRPANDROIDSCREEN::SetAndroidHandle(ANativeWindow* anativehandle)
{
  this->anativehandle = anativehandle;
}


#ifdef GRP_OPENGL_ACTIVE
/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPANDROIDBLITGLES* GRPANDROIDSCREEN::GetBlitGLES()
* @brief      Get OpenGL ES blitter (only present when GRP_OPENGL_ACTIVE is defined)
* @ingroup    PLATFORM_ANDROID
*
* @return     GRPANDROIDBLITGLES* :
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPANDROIDBLITGLES* GRPANDROIDSCREEN::GetBlitGLES()
{
  return blitgles;
}
#endif


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRPANDROIDSCREEN::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    PLATFORM_ANDROID
*
* --------------------------------------------------------------------------------------------------------------------*/
void GRPANDROIDSCREEN::Clean()
{
  anativehandle  = NULL;

  #ifdef GRP_OPENGL_ACTIVE
  blitgles       = NULL;
  #endif
}



