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

#include <android/log.h>
#define GENBLITLOG(...) __android_log_print(ANDROID_LOG_INFO, "GEN_BLIT", __VA_ARGS__)




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
  GENBLITLOG("PATH Update() no-arg (does nothing)");
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
  GENBLITLOG("PATH Update(canvas) ENTER canvas=%p handle=%p", (void*)canvas, (void*)anativehandle);
  if(!canvas)        { GENBLITLOG("Update(canvas): canvas NULL -> false"); return false; }
  if(!anativehandle) { GENBLITLOG("Update(canvas): anativehandle NULL -> false"); return false; }

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

      // FIX: propagate the screen's TRANSPARENT style to the blitter BEFORE
      // Create() is called.  Create() drives EGL config selection (ChooseConfig)
      // which requests EGL_ALPHA_SIZE only when usealpha==true.  If we skip
      // this call the EGL surface is created without an alpha channel and the
      // Android compositor treats the window as fully opaque (black background).
      // This mirrors what the Linux X11 blitter does via ChooseVisualID().
      blitgles->SetUseAlpha(Style_Is(GRPSCREENSTYLE_TRANSPARENT));
      GENBLITLOG("Update(canvas): usealpha=%d (screen style TRANSPARENT=%d)",
                 (int)Style_Is(GRPSCREENSTYLE_TRANSPARENT),
                 (int)Style_Is(GRPSCREENSTYLE_TRANSPARENT));

      GENBLITLOG("Update(canvas): creating blitgles...");
      if(!blitgles->Create(this))
        {
          GENBLITLOG("Update(canvas): blitgles->Create FAILED");
          XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[Screen Android] BlitGLES create failed"));
          GEN_DELETE blitgles;
          blitgles = NULL;
          return false;
        }
    }

  GENBLITLOG("Update(canvas): GL branch, blitgles=%p, calling blit", (void*)blitgles);
  {
    bool br = blitgles->Update(canvas);
    GENBLITLOG("Update(canvas): GL blit result=%d", (int)br);
    return br;
  }

  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPANDROIDSCREEN::UpdateTransparent(GRPCANVAS* canvas)
* @brief      Present a transparent-style screen.
*
*             GRPSCREEN::UpdateViewports() routes transparent screens (the style
*             GRPSCREENSTYLE_TRANSPARENT, used e.g. by UI_Message) to
*             UpdateTransparent() instead of Update(). The base class
*             GRPSCREEN::UpdateTransparent() returns false (does nothing), so
*             without this override an Android transparent screen presented
*             NOTHING — most visibly with GRP_OPENGL_ACTIVE, where the popup
*             never appeared.
*
*             On Android there is no desktop window/compositor to composite a
*             layered window against (unlike Windows' UpdateLayeredWindow): the
*             activity owns a single surface and the canvas (RGBA, with the dialog
*             opaque and the rest alpha=0) is simply presented onto it. So we
*             present it through the normal path — exactly as the Linux X11 screen
*             does. In the software build this posts the canvas via
*             ANativeWindow_lock/unlockAndPost; in the OpenGL build it goes through
*             the blitter. The dialog becomes visible either way.
*
*             NOTE: for the see-through-to-launcher effect (truly transparent
*             background) the activity must additionally be translucent and the
*             GL path must use an alpha surface; see the delivery notes. Making the
*             popup VISIBLE (the reported bug) only needs this override.
* @ingroup    PLATFORM_ANDROID
*
* @param[in]  canvas : canvas to present.
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPANDROIDSCREEN::UpdateTransparent(GRPCANVAS* canvas)
{
  GENBLITLOG("PATH UpdateTransparent ENTER canvas=%p handle=%p style_transp=%d w=%d h=%d", (void*)canvas, (void*)anativehandle, (int)Style_Is(GRPSCREENSTYLE_TRANSPARENT), (int)width, (int)height);
  // Sample canvas pixels BEFORE blitting to see if the UI system drew the dialog.
  // If ALL pixels are 0x00000000 (transparent black), the canvas is empty and the
  // problem is NOT the blitter — it is GEN_USERINTERFACE.Update() not rendering.
  if(canvas && canvas->Buffer_Get())
    {
      auto* buf  = (unsigned int*)canvas->Buffer_Get();
      int   cx   = (int)width / 2;
      int   cy   = (int)height / 2;
      unsigned int p0     = buf[0];
      unsigned int pcenter= buf[cx + cy * (int)width];
      unsigned int p10_30 = ((int)width > 10 && (int)height > 30) ? buf[10 + 30 * (int)width] : 0;
      GENBLITLOG("CANVAS PIXELS top-left=0x%08X center(%d,%d)=0x%08X pos(10,30)=0x%08X", p0, cx, cy, pcenter, p10_30);
      GENBLITLOG("CANVAS EMPTY=%s  (0x00000000 = transparent/nothing drawn)", (p0==0 && pcenter==0 && p10_30==0) ? "YES -> UI not drawing" : "NO -> blitter issue");
    }
  bool r = Update(canvas);
  GENBLITLOG("PATH UpdateTransparent EXIT result=%d", (int)r);
  return r;
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



