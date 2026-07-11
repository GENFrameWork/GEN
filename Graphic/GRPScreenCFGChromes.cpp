/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPScreenCFGChromes.cpp
* 
* @class      GRPSCREENCFGCHROMES
* @brief      Graphics Screen Chromes Configuration class
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "GRPScreenCFGChromes.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPSCREENCFGCHROMES::GRPSCREENCFGCHROMES()
* @brief      Constructor of class
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPSCREENCFGCHROMES::GRPSCREENCFGCHROMES()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPSCREENCFGCHROMES::~GRPSCREENCFGCHROMES()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPSCREENCFGCHROMES::~GRPSCREENCFGCHROMES()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREENCFGCHROMES::GetUseNativeChromes()
* @brief      Get use native chromes
* @ingroup    GRAPHIC
* 
* @return     bool : true if native chromes (OS managed) are used; otherwise false (custom GEN chromes).
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREENCFGCHROMES::GetUseNativeChromes()
{
  return usenativechromes;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREENCFGCHROMES::SetUseNativeChromes(bool usenativechromes)
* @brief      Set use native chromes
* @ingroup    GRAPHIC
* 
* @param[in]  usenativechromes : Usenativechromes value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREENCFGCHROMES::SetUseNativeChromes(bool usenativechromes)
{
  this->usenativechromes = usenativechromes;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREENCFGCHROMES::GetCaptionActive()
* @brief      Get caption active
* @ingroup    GRAPHIC
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREENCFGCHROMES::GetCaptionActive()
{
  return captionactive;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREENCFGCHROMES::SetCaptionActive(bool active)
* @brief      Set caption active
* @ingroup    GRAPHIC
* 
* @param[in]  active : Active value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREENCFGCHROMES::SetCaptionActive(bool active)
{
  captionactive = active;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREENCFGCHROMES::GetTitleActive()
* @brief      Get title active
* @ingroup    GRAPHIC
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREENCFGCHROMES::GetTitleActive()
{
  return titleactive;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREENCFGCHROMES::SetTitleActive(bool active)
* @brief      Set title active
* @ingroup    GRAPHIC
* 
* @param[in]  active : Active value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREENCFGCHROMES::SetTitleActive(bool active)
{
  titleactive = active;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREENCFGCHROMES::GetIconActive()
* @brief      Get icon active
* @ingroup    GRAPHIC
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREENCFGCHROMES::GetIconActive()
{
  return iconactive;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREENCFGCHROMES::SetIconActive(bool active)
* @brief      Set icon active
* @ingroup    GRAPHIC
* 
* @param[in]  active : Active value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREENCFGCHROMES::SetIconActive(bool active)
{
  iconactive = active;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREENCFGCHROMES::GetMaximizeActive()
* @brief      Get maximize active
* @ingroup    GRAPHIC
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREENCFGCHROMES::GetMaximizeActive()
{
  return maximizeactive;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREENCFGCHROMES::SetMaximizeActive(bool active)
* @brief      Set maximize active
* @ingroup    GRAPHIC
* 
* @param[in]  active : Active value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREENCFGCHROMES::SetMaximizeActive(bool active)
{
  maximizeactive = active;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREENCFGCHROMES::GetMinimizeActive()
* @brief      Get minimize active
* @ingroup    GRAPHIC
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREENCFGCHROMES::GetMinimizeActive()
{
  return minimizeactive;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREENCFGCHROMES::SetMinimizeActive(bool active)
* @brief      Set minimize active
* @ingroup    GRAPHIC
* 
* @param[in]  active : Active value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREENCFGCHROMES::SetMinimizeActive(bool active)
{
  minimizeactive = active;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREENCFGCHROMES::GetCloseActive()
* @brief      Get close active
* @ingroup    GRAPHIC
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREENCFGCHROMES::GetCloseActive()
{
  return closeactive;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREENCFGCHROMES::SetCloseActive(bool active)
* @brief      Set close active
* @ingroup    GRAPHIC
* 
* @param[in]  active : Active value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREENCFGCHROMES::SetCloseActive(bool active)
{
  closeactive = active;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREENCFGCHROMES::GetResizeActive()
* @brief      Get resize active
* @ingroup    GRAPHIC
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREENCFGCHROMES::GetResizeActive()
{
  return resizeactive;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREENCFGCHROMES::SetResizeActive(bool active)
* @brief      Set resize active
* @ingroup    GRAPHIC
* 
* @param[in]  active : Active value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREENCFGCHROMES::SetResizeActive(bool active)
{
  resizeactive = active;
}


#ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* GRPSCREENCFGCHROMES::GetCustomLayoutFile()
* @brief      Get custom layout file
* @ingroup    GRAPHIC
* 
* @return     XSTRING* : Pointer to the requested string.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* GRPSCREENCFGCHROMES::GetCustomLayoutFile()
{
  return &customlayoutfile;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREENCFGCHROMES::SetCustomLayoutFile(XCHAR* namelayoutfile)
* @brief      Set custom layout file
* @ingroup    GRAPHIC
* 
* @param[in]  namelayoutfile : Namelayoutfile value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREENCFGCHROMES::SetCustomLayoutFile(XCHAR* namelayoutfile)
{
  customlayoutfile.Set(namelayoutfile);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* GRPSCREENCFGCHROMES::GetCustomLayoutName()
* @brief      Get custom layout name
* @ingroup    GRAPHIC
* 
* @return     XSTRING* : Pointer to the requested string.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* GRPSCREENCFGCHROMES::GetCustomLayoutName()
{
  return &customlayoutname;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREENCFGCHROMES::SetCustomLayoutName(XCHAR* namelayout)
* @brief      Set custom layout name
* @ingroup    GRAPHIC
* 
* @param[in]  namelayout : Namelayout value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREENCFGCHROMES::SetCustomLayoutName(XCHAR* namelayout)
{
  customlayoutname.Set(namelayout);
}

#endif


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREENCFGCHROMES::CopyTo(GRPSCREENCFGCHROMES& cfgchromes)
* @brief      Copy to
* @ingroup    GRAPHIC
* 
* @param[in]  cfgchromes : Chromes configuration value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREENCFGCHROMES::CopyTo(GRPSCREENCFGCHROMES& cfgchromes)
{
  cfgchromes.usenativechromes = usenativechromes;
  cfgchromes.captionactive    = captionactive;
  cfgchromes.titleactive      = titleactive;
  cfgchromes.iconactive       = iconactive;
  cfgchromes.maximizeactive   = maximizeactive;
  cfgchromes.minimizeactive   = minimizeactive;
  cfgchromes.closeactive      = closeactive;
  cfgchromes.resizeactive     = resizeactive;

  #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
  cfgchromes.customlayoutfile = customlayoutfile;
  cfgchromes.customlayoutname = customlayoutname;
  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPSCREENCFGCHROMES::CopyFrom(GRPSCREENCFGCHROMES& cfgchromes)
* @brief      Copy from
* @ingroup    GRAPHIC
* 
* @param[in]  cfgchromes : Chromes configuration value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPSCREENCFGCHROMES::CopyFrom(GRPSCREENCFGCHROMES& cfgchromes)
{
  usenativechromes = cfgchromes.usenativechromes;
  captionactive    = cfgchromes.captionactive;
  titleactive      = cfgchromes.titleactive;
  iconactive       = cfgchromes.iconactive;
  maximizeactive   = cfgchromes.maximizeactive;
  minimizeactive   = cfgchromes.minimizeactive;
  closeactive      = cfgchromes.closeactive;
  resizeactive     = cfgchromes.resizeactive;

  #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
  customlayoutfile = cfgchromes.customlayoutfile;
  customlayoutname = cfgchromes.customlayoutname;
  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPSCREENCFGCHROMES::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPSCREENCFGCHROMES::Clean()
{
  usenativechromes  = true;

  captionactive     = true;

  titleactive       = true;

  iconactive        = true;

  maximizeactive    = true;
  minimizeactive    = true;
  closeactive       = true;

  resizeactive      = true;

  #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE
  customlayoutfile.Empty();
  customlayoutname.Empty();
  #endif
}

