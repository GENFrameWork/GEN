/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_Manager.cpp
* 
* @class      UI_MANAGER
* @brief      User Interface Manager class
* @ingroup    USERINTERFACE
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

#include "UI_Manager.h"
#include "UI_Property_Scrolleable.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "XFactory.h"
#include "XThread.h"
#include "XTimer.h"
#include "XPath.h"
#include "XFile.h"
#include "XTrace.h"
#include "XTranslation.h"
#include "XSleep.h"

#include "INPCursorMotion.h"

#include "GRPProperties.h"
#include "GRPScreen.h"
#include "GRP2DCanvas.h"
#include "GRP2DColor.h"
#include "GRPBitmapFile.h"
#include "GRPFactory.h"
#include "GRPVectorFile.h"
#include "GRP2DVectorFileRenderAGG.h"

#include "APPFlowBase.h"

#include "UI_XEvent.h"
#include "UI_Style.h"
#include "UI_Color.h"
#include "UI_Colors.h"
#include "UI_Text.h"
#include "UI_Texts.h"
#include "UI_Animation.h"
#include "UI_Animations.h"
#include "UI_Element.h"
#include "UI_Element_Text.h"
#include "UI_Element_TextBox.h"
#include "UI_Element_Image.h"
#include "UI_Element_Animation.h"
#include "UI_Element_Option.h"
#include "UI_Element_MultiOption.h"
#include "UI_Element_Button.h"
#include "UI_Element_CheckBox.h"
#include "UI_Element_EditText.h"
#include "UI_Element_Form.h"
#include "UI_Element_Menu.h"
#include "UI_Element_ListBox.h"
#include "UI_Element_ProgressBar.h"
#include "UI_Element_Scroll.h"
#include "UI_Element_ProgressRadial.h"
#include "UI_Element_ProgressImage.h"

#include "UI_VirtualKeyboard.h"

#include "UI_Skin.h"
#include "UI_SkinCanvas.h"
#include "UI_SkinCanvas_Flat.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

UI_MANAGER* UI_MANAGER::instance = NULL;



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::GetIsInstanced()
* @brief      Get is instanced
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::GetIsInstanced()
{
  return instance!=NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_MANAGER& UI_MANAGER::GetInstance()
* @brief      Get instance
* @ingroup    USERINTERFACE
* 
* @return     UI_MANAGER& : Reference to the requested object.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_MANAGER& UI_MANAGER::GetInstance()
{
  if(!instance) 
    {
      instance = GEN_NEW UI_MANAGER();
    }

  return (*instance);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::DelInstance()
* @brief      Del instance
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::DelInstance()
{
  if(instance)
    {
      GEN_DELETE instance;
      instance = NULL;
    }  

  GEN_UI_COLORS.DelInstance();

  GEN_UI_TEXTS.DelInstance();

  GEN_UI_ANIMATIONS.DelInstance();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Load(XPATH& pathfile, GRPSCREEN* screen, int viewportindex)
* @brief      Load
* @ingroup    USERINTERFACE
* 
* @param[in]  pathfile : Pathfile value.
* @param[in]  screen : Screen object to use.
* @param[in]  viewportindex : Viewportindex value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Load(XPATH& pathfile, GRPSCREEN* screen, int viewportindex)
{
  bool status = false;  

  iszippedfile = false;

  if(pathfile.Find(__L(".zip"), true) != XSTRING_NOTFOUND) 
    {
      iszippedfile = true;
    }

  if(!iszippedfile)
    {
      status = LoadLayout(pathfile, screen, viewportindex);

      if(!status)
        {
          XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[UI Load] ERROR: cannot open layout XML [%s] (file missing? name case mismatch on a case-sensitive filesystem?)"), pathfile.Get());
        }
    }
   else
    {
      unzipfile = GEN_NEW XFILEUNZIP();
      if(!unzipfile)
        {
          return false;
        }

      bool openresult = unzipfile->Open(pathfile);
      if(openresult)
        {
          XSTRING origin_drive;
          XPATH   origin_path;
          XSTRING origin_namefile;
          XSTRING origin_ext;

          pathfile.GetDrive(origin_drive);
          pathfile.GetPath(origin_path);
          pathfile.GetNamefile(origin_namefile);
          pathfile.GetExt(origin_ext);

          unzippathfile  = origin_drive;
          unzippathfile += origin_path;

          XSTRING namefile;

          namefile    = origin_namefile;
          namefile   += __L(".xml");


          status = unzipfile->DecompressFile(namefile, unzippathfile, namefile.Get());

          #ifdef LINUX
          if(!status)
            {
              // The bundle's own directory is not writable (typical of a read-only install dir on native
              // Linux: /usr/share, /opt, or simply a directory owned by another user; note WSL runs from
              // /mnt/c/... where everything is always writable, which masks this). Extract to /tmp instead.
              // unzippathfile is redirected too, so every LATER extraction from this same bundle (vector
              // fonts, bitmaps, animation frames -- they all resolve their target directory through
              // GetUnzipPathFile()) automatically follows to the writable location.
              XPATH tmppath;

              tmppath = __L("/tmp/");

              status = unzipfile->DecompressFile(namefile, tmppath, namefile.Get());
              if(status)
                {
                  unzippathfile = tmppath;
                  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[UI Load] bundle dir not writable; extracting [%s] to /tmp instead"), namefile.Get());
                }
            }
          #endif

          if(status)
            {
              XPATH unzippathfile_tmp;

              unzippathfile_tmp  = unzippathfile;
              unzippathfile_tmp += namefile;

              status = LoadLayout(unzippathfile_tmp, screen, viewportindex);

              DeleteTemporalUnZipFile(unzippathfile_tmp);
            }
           else
            {
              XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[UI Load] ERROR: cannot extract [%s] from bundle [%s] (entry missing in zip, or target dir not writable)"), namefile.Get(), pathfile.Get());
            }
        }
       else
        {
          XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[UI Load] ERROR: cannot open layout bundle [%s] (file missing? name case mismatch on a case-sensitive filesystem?)"), pathfile.Get());
          CloseUnZipFile();
        }
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::LoadLayout(XPATH& pathfile, GRPSCREEN* screen, int viewportindex)
* @brief      Load layout
* @ingroup    USERINTERFACE
* 
* @param[in]  pathfile : Pathfile value.
* @param[in]  screen : Screen object to use.
* @param[in]  viewportindex : Viewportindex value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::LoadLayout(XPATH& pathfile, GRPSCREEN* screen, int viewportindex)
{
  XFILEXML xml;
  bool     status = false;  

  if(xml.Open(pathfile, true))
    {      
      CreateLayouts(xml, screen, viewportindex);

      xml.Close();

      status = true;
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::IsZippedFile()
* @brief      Is zipped file
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::IsZippedFile()
{
  if(iszippedfile && unzipfile)
    {
      return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XPATH* UI_MANAGER::GetUnzipPathFile()
* @brief      Get unzip path file
* @ingroup    USERINTERFACE
* 
* @return     XPATH* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XPATH* UI_MANAGER::GetUnzipPathFile()
{
  return &unzippathfile;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEUNZIP* UI_MANAGER::GetUnzipFile()
* @brief      Get unzip file
* @ingroup    USERINTERFACE
* 
* @return     XFILEUNZIP* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XFILEUNZIP* UI_MANAGER::GetUnzipFile()
{
  return unzipfile;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::DeleteTemporalUnZipFile(XPATH& pathfile)
* @brief      Delete temporal un zip file
* @ingroup    USERINTERFACE
* 
* @param[in]  pathfile : Pathfile value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::DeleteTemporalUnZipFile(XPATH& pathfile)
{
  XFILE* xfile = GEN_XFACTORY.Create_File();
  if(!xfile)
    {
      return false;
    }
                
  xfile->Erase(pathfile);

  GEN_XFACTORY.Delete_File(xfile);

  return true;
}      


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::CloseUnZipFile()
* @brief      Close un zip file
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::CloseUnZipFile()
{
  if(!unzipfile) 
    {
      return false;
    }

  unzipfile->Close();
  GEN_DELETE unzipfile;

  unzipfile = NULL;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Layouts_Add(UI_LAYOUT* layout)
* @brief      Layouts add
* @ingroup    USERINTERFACE
* 
* @param[in]  layout : Layout pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Layouts_Add(UI_LAYOUT* layout)
{
  if(!layout) 
    {
      return false;
    }

  layouts.Add(layout);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<UI_LAYOUT*>* UI_MANAGER::Layouts_GetAll()
* @brief      Layouts get all
* @ingroup    USERINTERFACE
* 
* @return     XVECTOR<UI_LAYOUT*>* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<UI_LAYOUT*>* UI_MANAGER::Layouts_GetAll()
{
  return &layouts;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_LAYOUT* UI_MANAGER::Layouts_Get(int index)
* @brief      Layouts get
* @ingroup    USERINTERFACE
* 
* @param[in]  index : Index value.
* 
* @return     UI_LAYOUT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUT* UI_MANAGER::Layouts_Get(int index)
{
  if(layouts.IsEmpty())                     
    {
      return NULL;
    }

  if((XDWORD)index >= layouts.GetSize())    
    {
      return NULL;
    }
  
  return layouts.Get(index);    
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_LAYOUT* UI_MANAGER::Layouts_Get(XCHAR* name)
* @brief      Layouts get
* @ingroup    USERINTERFACE
* 
* @param[in]  name : Name to use.
* 
* @return     UI_LAYOUT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUT* UI_MANAGER::Layouts_Get(XCHAR* name)
{
  if(layouts.IsEmpty()) 
    {
      return NULL;
    }

  for(XDWORD c=0; c<layouts.GetSize(); c++)
    {
      UI_LAYOUT* layout = layouts.Get(c);
      if(layout) 
        {
          if(!layout->GetNameID()->Compare(name, true)) 
            {
              return layout;          
            }
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_LAYOUT* UI_MANAGER::Layouts_Get(XSTRING& name)
* @brief      Layouts get
* @ingroup    USERINTERFACE
* 
* @param[in]  name : Name to use.
* 
* @return     UI_LAYOUT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUT* UI_MANAGER::Layouts_Get(XSTRING& name)
{
  return Layouts_Get(name.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Layouts_DeleteAll()
* @brief      Layouts GEN_DELETE all
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Layouts_DeleteAll()
{
  if(layouts.IsEmpty())  return false;

  layouts.DeleteContents();
  layouts.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_LAYOUT* UI_MANAGER::Layouts_GetCommonLayout()
* @brief      Layouts get common layout
* @ingroup    USERINTERFACE
* 
* @return     UI_LAYOUT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUT* UI_MANAGER::Layouts_GetCommonLayout()
{
  if(layout_commonindex == UI_MANAGER_LAYOUT_NOTFOUND) 
    {
      return NULL;  
    }

  if((XDWORD)layout_commonindex >= layouts.GetSize())     
    {
      return NULL;
    }

  return layouts.Get(layout_commonindex);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Layout_PutBackground(XCHAR* layoutname)
* @brief      Layout put background
* @note       Whether the layout's background is tiled (seamless pattern) or drawn once covering the whole layout
*             is decided by the "backgroundseamlesspattern" skin/layout attribute, resolved once at
*             CreateLayouts() time: it is stored as UI_BACKGROUND::GetPatternBitmap() instead of GetBitmap() when
*             the attribute is "yes"/"true"/"1", so at most one of the two is ever set for a given layout, and
*             this function simply routes to whichever one is present.
* @ingroup    USERINTERFACE
* 
* @param[in]  layoutname : Layoutname pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Layout_PutBackground(XCHAR* layoutname)
{
  bool        status = false;
  UI_LAYOUT*  layout = Layouts_Get(layoutname);

  if(layout && layout->GetBackground()->GetPatternBitmap())
    {
      status = Layout_PutBackgroundSeamlessPattern(layoutname);
    }
   else
    {
      status = Layout_PutBackgroundImage(layoutname);
    }

  if(!status) 
    {
      status = Layout_PutBackgroundColor(layoutname);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Layout_PutBackgroundColor(XCHAR* layoutname)
* @brief      Layout put background color
* @ingroup    USERINTERFACE
* 
* @param[in]  layoutname : Layoutname pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Layout_PutBackgroundColor(XCHAR* layoutname)
{
  UI_LAYOUT*  layout = NULL;
  UI_SKIN*    skin   = NULL;
  bool        status = false;

  layout = Layouts_Get(layoutname);
  if(!layout)
    {
      return false;
    }

  skin = layout->GetSkin();
  if(!skin)
    {
      return false;
    }
 
  switch(skin->GetDrawMode())
    {
      case UI_SKIN_DRAWMODE_UNKNOWN  :  break;

      case UI_SKIN_DRAWMODE_CANVAS   :  { UI_SKINCANVAS* skin_canvas  = (UI_SKINCANVAS*)skin;
                                          if(layout->GetBackground()->GetColor()->IsValid()) 
                                            { 
                                              GRP2DCANVAS* canvas = skin_canvas->GetCanvas();    
                                              if(canvas) 
                                                {
                                                  UI_COLOR color;

                                                  color.CopyFrom(layout->GetBackground()->GetColor()); 

                                                  GRP2DCOLOR_RGBA8 color_canvas(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());  
                                                                                                                    
                                                  canvas->Clear(&color_canvas);

                                                  status = true;                                                        
                                                }
                                            }
                                        }
                                        break;

      case UI_SKIN_DRAWMODE_CONTEXT  :  break;
    }
                      
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Layout_PutBackgroundImage(XCHAR* layoutname)
* @brief      Layout put background bitmap
* @ingroup    USERINTERFACE
* 
* @param[in]  layoutname : Layoutname pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Layout_PutBackgroundImage(XCHAR* layoutname)
{
  UI_LAYOUT*  layout = NULL;
  UI_SKIN*    skin   = NULL;
  bool        status = false;

  layout = Layouts_Get(layoutname);
  if(!layout)
    {
      return false;
    }

  skin = layout->GetSkin();
  if(!skin)
    {
      return false;
    }
 
  switch(skin->GetDrawMode())
    {
      case UI_SKIN_DRAWMODE_UNKNOWN  :  break;

      case UI_SKIN_DRAWMODE_CANVAS   :  { UI_SKINCANVAS* skin_canvas  = (UI_SKINCANVAS*)skin;
                                          if(layout->GetBackground()->GetBitmap()) 
                                            { 
                                              GRPSCREEN* screen = skin_canvas->GetScreen();      
                                              GRP2DCANVAS* canvas = skin_canvas->GetCanvas();    
                                              if(canvas && screen) 
                                                {
                                                  layout->GetBackground()->GetBitmap()->Scale(screen->GetWidth(), screen->GetHeight());          
                                                  canvas->PutBitmapNoAlpha(0, 0, layout->GetBackground()->GetBitmap());                                              

                                                  status = true;
                                                }
                                            }
                                        }
                                        break;

      case UI_SKIN_DRAWMODE_CONTEXT  :  break;
    }
   
                      
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Layout_PutBackgroundSeamlessPattern(XCHAR* layoutname)
* @brief      Layout put background seamless pattern
* @note       Tiles layout->GetBackground()->GetPatternBitmap() (the "backgroundimg" resource - bitmap or a
*             rasterized vector file - loaded by CreateLayouts()/LoadBackgroundBitmap() as a pattern instead of a
*             stretched background because "backgroundseamlesspattern" resolved to true) across the whole layout
*             area, repeating it as a mosaic instead of stretching a single copy like Layout_PutBackgroundImage()
*             does. Unlike Layout_PutBackgroundImage(), it has no "scale" parameter: the tile is always drawn at
*             its own loaded size and simply repeated, since scaling the tile up or down is what would break the
*             "seamless" look.
* @ingroup    USERINTERFACE
* 
* @param[in]  layoutname : Layoutname pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Layout_PutBackgroundSeamlessPattern(XCHAR* layoutname)
{
  UI_LAYOUT*  layout = NULL;
  UI_SKIN*    skin   = NULL;
  bool        status = false;

  layout = Layouts_Get(layoutname);
  if(!layout)
    {
      return false;
    }

  skin = layout->GetSkin();
  if(!skin)
    {
      return false;
    }
 
  switch(skin->GetDrawMode())
    {
      case UI_SKIN_DRAWMODE_UNKNOWN  :  break;

      case UI_SKIN_DRAWMODE_CANVAS   :  { UI_SKINCANVAS* skin_canvas  = (UI_SKINCANVAS*)skin;
                                          if(layout->GetBackground()->GetPatternBitmap()) 
                                            { 
                                              GRPSCREEN*   screen = skin_canvas->GetScreen();      
                                              GRP2DCANVAS* canvas = skin_canvas->GetCanvas();    

                                              status = PutBitmapAsSeamlessPattern(canvas, screen, layout->GetBackground()->GetPatternBitmap());
                                            }
                                        }
                                        break;

      case UI_SKIN_DRAWMODE_CONTEXT  :  break;
    }
   
                      
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Layout_PutBackground(bool scale)
* @brief      Layout put background
* @note       Layout_PutBackgroundImage(scale) only draws layouts whose UI_BACKGROUND has GetBitmap() set, and
*             Layout_PutBackgroundSeamlessPattern() only draws layouts whose GetPatternBitmap() is set instead
*             (see CreateLayouts()/Layout_PutBackground(XCHAR*) note) - the two are mutually exclusive per layout,
*             so both are attempted here to cover every layout regardless of its "backgroundseamlesspattern"
*             setting. This is purely additive: layouts without a pattern bitmap behave exactly as before.
* @ingroup    USERINTERFACE
* 
* @param[in]  scale : Scale value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Layout_PutBackground(bool scale)
{
  bool status = false;

  status = Layout_PutBackgroundImage(scale);

  if(Layout_PutBackgroundSeamlessPattern())
    {
      status = true;
    }

  if(!status) 
    {
      status = Layout_PutBackgroundColor();
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Layout_PutBackgroundColor()
* @brief      Layout put background color
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Layout_PutBackgroundColor()
{
  bool status = false;
  
  for(XDWORD c=0; c<Layouts_GetAll()->GetSize(); c++)
    { 
      UI_LAYOUT*  layout = NULL;
      UI_SKIN*    skin   = NULL;

      layout = Layouts_Get(c);
      if(layout)
        {       
          skin = layout->GetSkin();
          if(!skin)
            {
              return false;
            }
 
          switch(skin->GetDrawMode())
            {
              case UI_SKIN_DRAWMODE_UNKNOWN  :  break;

              case UI_SKIN_DRAWMODE_CANVAS   :  { UI_SKINCANVAS* skin_canvas  = (UI_SKINCANVAS*)skin;
                                                  if(layout->GetBackground()->GetColor()->IsValid()) 
                                                    { 
                                                      GRP2DCANVAS* canvas = skin_canvas->GetCanvas();    
                                                      if(canvas) 
                                                        {
                                                          UI_COLOR color;

                                                          color.CopyFrom(layout->GetBackground()->GetColor()); 

                                                          GRP2DCOLOR_RGBA8 color_canvas(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());  
                                                                                                                    
                                                          canvas->Clear(&color_canvas);

                                                          status = true;                                                        
                                                        }
                                                    }
                                                }
                                                break;

              case UI_SKIN_DRAWMODE_CONTEXT  :  break;
            }
        }
    }
                      
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Layout_PutBackgroundImage(bool scale)
* @brief      Layout put background bitmap
* @ingroup    USERINTERFACE
* 
* @param[in]  scale : Scale value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Layout_PutBackgroundImage(bool scale)
{  
  bool status = false;

  for(XDWORD c=0; c<Layouts_GetAll()->GetSize(); c++)
    { 
      UI_LAYOUT*  layout = NULL;
      UI_SKIN*    skin   = NULL;

      layout = Layouts_Get(c);
      if(layout)
        {    
          skin = layout->GetSkin();
          if(!skin)
            {
              return false;
            }
 
          switch(skin->GetDrawMode())
            {
              case UI_SKIN_DRAWMODE_UNKNOWN  :  break;

              case UI_SKIN_DRAWMODE_CANVAS   :  { UI_SKINCANVAS* skin_canvas  = (UI_SKINCANVAS*)skin;
                                                  if(layout->GetBackground()->GetBitmap()) 
                                                    { 
                                                      GRPSCREEN* screen = skin_canvas->GetScreen();      
                                                      GRP2DCANVAS* canvas = skin_canvas->GetCanvas();    
                                                      if(canvas && screen) 
                                                        {
                                                          if(scale)
                                                            {  
                                                              layout->GetBackground()->GetBitmap()->Scale(screen->GetWidth(), screen->GetHeight());          
                                                            }

                                                          canvas->PutBitmapNoAlpha(0, 0, layout->GetBackground()->GetBitmap());                                              

                                                          status = true;
                                                        }
                                                    }
                                                }
                                                break;

              case UI_SKIN_DRAWMODE_CONTEXT  :  break;
            }
        }
    }   
                      
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Layout_PutBackgroundSeamlessPattern()
* @brief      Layout put background seamless pattern
* @note       Tiles layout->GetBackground()->GetPatternBitmap() across every loaded layout, same tiling behavior
*             as the by-name overload; see that overload's note for details. Has no "scale" parameter, matching
*             the requested UI_MANAGER::Layout_PutBackgroundSeamlessPattern() signature: a tiled pattern is always
*             drawn at its own loaded size.
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Layout_PutBackgroundSeamlessPattern()
{  
  bool status = false;

  for(XDWORD c=0; c<Layouts_GetAll()->GetSize(); c++)
    { 
      UI_LAYOUT*  layout = NULL;
      UI_SKIN*    skin   = NULL;

      layout = Layouts_Get(c);
      if(layout)
        {    
          skin = layout->GetSkin();
          if(!skin)
            {
              return false;
            }
 
          switch(skin->GetDrawMode())
            {
              case UI_SKIN_DRAWMODE_UNKNOWN  :  break;

              case UI_SKIN_DRAWMODE_CANVAS   :  { UI_SKINCANVAS* skin_canvas  = (UI_SKINCANVAS*)skin;
                                                  if(layout->GetBackground()->GetPatternBitmap()) 
                                                    { 
                                                      GRPSCREEN*   screen = skin_canvas->GetScreen();      
                                                      GRP2DCANVAS* canvas = skin_canvas->GetCanvas();    

                                                      if(PutBitmapAsSeamlessPattern(canvas, screen, layout->GetBackground()->GetPatternBitmap()))
                                                        {
                                                          status = true;
                                                        }
                                                    }
                                                }
                                                break;

              case UI_SKIN_DRAWMODE_CONTEXT  :  break;
            }
        }
    }   
                      
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::PutBitmapAsSeamlessPattern(GRP2DCANVAS* canvas, GRPSCREEN* screen, GRPBITMAP* pattern)
* @brief      Put bitmap as seamless pattern
* @note       Shared tiling core used by both Layout_PutBackgroundSeamlessPattern() overloads. Repeats "pattern"
*             at its own natural size, left-to-right/top-to-bottom, starting at (0, 0), until the whole
*             (screen width, screen height) area is covered. The last column/row of tiles is naturally cropped
*             by GRP2DCANVAS::PutBitmapNoAlpha()'s own clipping against the canvas render area, so no partial-tile
*             bookkeeping is needed here.
* @ingroup    USERINTERFACE
* 
* @param[in]  canvas : Canvas to tile the pattern onto.
* @param[in]  screen : Screen used to know the area, in pixels, that must be covered.
* @param[in]  pattern : Bitmap : the seamless pattern tile to repeat; must have a non-zero size.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::PutBitmapAsSeamlessPattern(GRP2DCANVAS* canvas, GRPSCREEN* screen, GRPBITMAP* pattern)
{
  if(!canvas)  return false;
  if(!screen)  return false;
  if(!pattern) return false;

  XDWORD patternwidth  = pattern->GetWidth();
  XDWORD patternheight = pattern->GetHeight();

  if(!patternwidth)  return false;
  if(!patternheight) return false;

  XDWORD screenwidth  = screen->GetWidth();
  XDWORD screenheight = screen->GetHeight();

  for(XDWORD ypos=0; ypos<screenheight; ypos+=patternheight)
    {
      for(XDWORD xpos=0; xpos<screenwidth; xpos+=patternwidth)
        {
          canvas->PutBitmapNoAlpha((double)xpos, (double)ypos, pattern);
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Update(UI_LAYOUT* layout)
* @brief      Update
* @ingroup    USERINTERFACE
* 
* @param[in]  layout : Layout pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Update(UI_LAYOUT* layout)
{
  bool status = false;
  
  if(!layout)
    {
      return false;
    }

  ChangeTextElementValue(layout);

  status = layout->Update();  
  if(status)
    {         
      if(layout_commonindex != UI_MANAGER_LAYOUT_NOTFOUND)
        {         
          layout = Layouts_Get(layout_commonindex);
          if(!layout) 
            {
              return false;          
            }                   
        }

      if(element_modal)
        {
          if(xmutex_modal) xmutex_modal->Lock(); 
          
          Elements_SetToRedraw(element_modal);
        
          if(xmutex_modal) xmutex_modal->UnLock(); 
        }
    }

  if(virtualkeyboard) 
    {
      if(virtualkeyboard->IsShow())
        {
          Elements_SetToRedraw(virtualkeyboard->GetElementEditable());
        }
    }

  ChangeTextElementValue(layout);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Update(XCHAR* layoutname)
* @brief      Update
* @ingroup    USERINTERFACE
* 
* @param[in]  layoutname : Layoutname pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Update(XCHAR* layoutname)
{
  UI_LAYOUT*  layout = NULL;
  bool        status = false;

  layout = Layouts_Get(layoutname);
  if(!layout)
    {
      return false;
    }

  ChangeTextElementValue(layout);

  status = layout->Update();  
  if(status)
    {         
      if(layout_commonindex != UI_MANAGER_LAYOUT_NOTFOUND)
        {         
          layout = Layouts_Get(layout_commonindex);
          if(!layout) 
            {
              return false;          
            }                   
        }

      if(element_modal)
        {
          if(xmutex_modal) xmutex_modal->Lock(); 
          
          Elements_SetToRedraw(element_modal);
        
          if(xmutex_modal) xmutex_modal->UnLock(); 
        }
    }

  if(virtualkeyboard) 
    {
      if(virtualkeyboard->IsShow())
        {
          Elements_SetToRedraw(virtualkeyboard->GetElementEditable());
        }
    }

  ChangeTextElementValue(layout);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Update()
* @brief      Update
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Update()
{
  bool status = false;

  #ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE

  for(XDWORD c=0; c<layouts.GetSize(); c++)
    {
      UI_LAYOUT* layout = layouts.Get(c);
      if(!layout)                    continue;
      if(IsCFGChromesLayout(layout)) continue;

      status = Update(layout);
      if(!status) return status;
    }

  for(XDWORD c=0; c<layouts.GetSize(); c++)
    {
      UI_LAYOUT* layout = layouts.Get(c);
      if(!layout)                     continue;
      if(!IsCFGChromesLayout(layout)) continue;

  
      status = Update(layout);
      if(!status) return status; 

      layout->Elements_SetToRedraw();
    }

  #else

  for(XDWORD c=0; c<layouts.GetSize(); c++)
    { 
      UI_LAYOUT* layout = layouts.Get(c);
      if(layout)
        {
          status = Update(layout);
          if(!status)
            {
              break;
            }
        }
    } 

  #endif

  return status;
}


#ifdef GRP_SCREEN_CUSTOMCHROMES_ACTIVE

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::IsCFGChromesLayout(UI_LAYOUT* layout)
* @brief      Is CFG chromes layout
* @note       true when this layout IS the custom chrome layout of the screen it is drawn on -- identity, not
*             position: walks layout -> its own UI_SKINCANVAS -> the GRPSCREEN it belongs to, and compares
*             against that screen's own GRPSCREEN::GetCFGChromesLayout(). A layout that is not on a canvas skin,
*             not tied to any screen, or belongs to a screen without custom chromes active, is never one.
* @ingroup    USERINTERFACE
* 
* @param[in]  layout : Layout pointer to use.
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::IsCFGChromesLayout(UI_LAYOUT* layout)
{
  if(!layout)                                                      return false;
  if(!layout->GetSkin())                                           return false;
  if(layout->GetSkin()->GetDrawMode() != UI_SKIN_DRAWMODE_CANVAS)   return false;

  GRPSCREEN* screen = ((UI_SKINCANVAS*)layout->GetSkin())->GetScreen();
  if(!screen)                                 return false;
  if(!screen->IsCFGChromesActive())           return false;
  if(!screen->GetCFGChromes())                return false;
  if(screen->GetCFGChromes()->GetUseNativeChromes()) return false;

  return (screen->GetCFGChromesLayout() == layout);
}

#endif


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::Element_Get(XCHAR* layoutname, XCHAR* name, UI_ELEMENT_TYPE type)
* @brief      Element get
* @ingroup    USERINTERFACE
* 
* @param[in]  layoutname : Layoutname pointer to use.
* @param[in]  name : Name to use.
* @param[in]  type : Type value.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::Element_Get(XCHAR* layoutname, XCHAR* name, UI_ELEMENT_TYPE type)
{  
  UI_LAYOUT*  layout = NULL; 
  UI_ELEMENT* element = NULL;

  layout = Layouts_Get(layoutname);
  if(!layout)
    {
      return NULL;
    }

  element = layout->Elements_Get(name, type);
  if(!element)
    {
      if(layout_commonindex != UI_MANAGER_LAYOUT_NOTFOUND)
        {
          layout = Layouts_Get(layout_commonindex);
          if(!layout) 
            {
              return NULL;
            }

          element = layout->Elements_Get(name, type);
        }
    }

  return element;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::Element_Get(XCHAR* layoutname, XSTRING& name, UI_ELEMENT_TYPE type)
* @brief      Element get
* @ingroup    USERINTERFACE
* 
* @param[in]  layoutname : Layoutname pointer to use.
* @param[in]  name : Name to use.
* @param[in]  type : Type value.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::Element_Get(XCHAR* layoutname, XSTRING& name, UI_ELEMENT_TYPE type)
{
  return Element_Get(layoutname, name.Get(), type);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::Element_Get(XCHAR* name, UI_ELEMENT_TYPE type)
* @brief      Element get
* @ingroup    USERINTERFACE
* 
* @param[in]  name : Name to use.
* @param[in]  type : Type value.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::Element_Get(XCHAR* name, UI_ELEMENT_TYPE type)
{ 
  if(layouts.IsEmpty()) 
    {
      return NULL;
    }

  UI_ELEMENT* element = NULL; 

  for(XDWORD c=0; c<layouts.GetSize(); c++)
    { 
      UI_LAYOUT* layout = layouts.Get(c);
      if(layout) 
        {          
          element = layout->Elements_Get(name, type);
          if(element) 
            {
              return element;              
            }
        }
    }

  return element;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::Element_Get(XSTRING& name, UI_ELEMENT_TYPE type)
* @brief      Element get
* @ingroup    USERINTERFACE
* 
* @param[in]  name : Name to use.
* @param[in]  type : Type value.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::Element_Get(XSTRING& name, UI_ELEMENT_TYPE type)
{ 
  return Element_Get(name.Get(), type);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_LAYOUT* UI_MANAGER::Element_GetLayout(UI_ELEMENT* element)
* @brief      Element get layout
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* 
* @return     UI_LAYOUT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUT* UI_MANAGER::Element_GetLayout(UI_ELEMENT* element)
{
  if(!element) 
    {
      return NULL;
    }

  if(layouts.IsEmpty()) 
    {
      return NULL;
    }

  for(XDWORD c=0; c<layouts.GetSize(); c++)
    { 
      UI_LAYOUT* layout = layouts.Get(c);
      if(layout) 
        { 
          XVECTOR<UI_ELEMENT*>* layout_elements = layout->Elements_Get();
          if(layout_elements) 
            {
              bool    found = false;
              XDWORD  index = 0;
 
              do{ UI_ELEMENT* _element = layout_elements->Get(index);
                  if(_element) 
                    {
                      if(_element == element) 
                        {
                          found = true;
                          break;
                        }
                    } 

                  index++;

                } while(index < layout_elements->GetSize());    
                               
              if(found) 
                {
                  return layout;              
                }
            }
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_LAYOUT* UI_MANAGER::Element_GetLayout(XCHAR* name, UI_ELEMENT_TYPE type)
* @brief      Element get layout
* @ingroup    USERINTERFACE
* 
* @param[in]  name : Name to use.
* @param[in]  type : Type value.
* 
* @return     UI_LAYOUT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUT* UI_MANAGER::Element_GetLayout(XCHAR* name, UI_ELEMENT_TYPE type)
{
  if(layouts.IsEmpty()) 
    {
      return NULL;
    }

  UI_ELEMENT* element = NULL; 

  for(XDWORD c=0; c<layouts.GetSize(); c++)
    { 
      UI_LAYOUT* layout = layouts.Get(c);
      if(layout) 
        {          
          element = layout->Elements_Get(name, type);
          if(element) 
            {
              return layout;              
            }
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_LAYOUT* UI_MANAGER::Element_GetLayout(XSTRING& name, UI_ELEMENT_TYPE type)
* @brief      Element get layout
* @ingroup    USERINTERFACE
* 
* @param[in]  name : Name to use.
* @param[in]  type : Type value.
* 
* @return     UI_LAYOUT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_LAYOUT* UI_MANAGER::Element_GetLayout(XSTRING& name, UI_ELEMENT_TYPE type)
{
  return Element_GetLayout(name.Get(), type);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Element_PutToLastPositionLayout(UI_ELEMENT* element)
* @brief      Element put to last position layout
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Element_PutToLastPositionLayout(UI_ELEMENT* element)
{
  if(!element) 
    {
      return false;
    }

  UI_LAYOUT* layout = Element_GetLayout(element);
  if(!layout) 
    {
      return false;
    }

  XVECTOR<UI_ELEMENT*>* layout_elements = layout->Elements_Get();
  if(!layout_elements) 
    {
      return false;
    }
  
  bool   found = false;
  XDWORD index = 0;

  do{ UI_ELEMENT* _element = layout_elements->Get(index);
      if(_element) 
        {
          if(_element == element) 
            {
              found = true;
              break;
            }
        } 

       index++;

    } while(index < layout_elements->GetSize());

  if(!found) 
    {
      return false;  
    }

  UI_ELEMENT* sustitute_element =  layout_elements->Get(layout_elements->GetSize()-1);
  if(!sustitute_element) return false;

  layout_elements->Set(layout_elements->GetSize()-1, layout_elements->Get(index));
  layout_elements->Set(index, sustitute_element);         

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Element_PutToLastPositionLayout(XCHAR* name, UI_ELEMENT_TYPE type)
* @brief      Element put to last position layout
* @ingroup    USERINTERFACE
* 
* @param[in]  name : Name to use.
* @param[in]  type : Type value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Element_PutToLastPositionLayout(XCHAR* name, UI_ELEMENT_TYPE type)
{
  UI_LAYOUT* layout = Element_GetLayout(name, type);
  if(!layout) 
    {
      return false;
    }

  UI_ELEMENT* element = layout->Elements_Get(name, type);
  if(!element) 
    {
      return false;
    }

  XVECTOR<UI_ELEMENT*>* layout_elements = layout->Elements_Get();
  if(!layout_elements) 
    {
      return false;
    }
  
  bool    found = false;
  XDWORD  index = 0;

  do{ UI_ELEMENT* _element = layout_elements->Get(index);
      if(_element) 
        {
          if(_element == element) 
            {
              found = true;
              break;
            }
        } 

       index++;

    } while(index < layout_elements->GetSize());

  if(!found) 
    {
      return false;  
    }

  UI_ELEMENT* sustitute_element =  layout_elements->Get(layout_elements->GetSize()-1);
  if(!sustitute_element) 
    {
      return false;
    }

  layout_elements->Set(layout_elements->GetSize()-1, layout_elements->Get(index));
  layout_elements->Set(index, sustitute_element);         

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Element_PutToLastPositionLayout(XSTRING& name, UI_ELEMENT_TYPE type)
* @brief      Element put to last position layout
* @ingroup    USERINTERFACE
* 
* @param[in]  name : Name to use.
* @param[in]  type : Type value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Element_PutToLastPositionLayout(XSTRING& name, UI_ELEMENT_TYPE type)
{
  return Element_PutToLastPositionLayout(name.Get(), type);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Element_SetModal(UI_ELEMENT* element_modal)
* @brief      Element set modal
* @ingroup    USERINTERFACE
* 
* @param[in]  element_modal : Element modal pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Element_SetModal(UI_ELEMENT* element_modal)
{
  if(xmutex_modal) 
    {
      xmutex_modal->Lock();
    }

  if(element_modal) 
    {
      Element_PutToLastPositionLayout(element_modal);
    }

  this->element_modal = element_modal;

  if(xmutex_modal) 
    {
      xmutex_modal->UnLock();
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Elements_SetToRedraw()
* @brief      Elements set to redraw
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Elements_SetToRedraw()
{
  bool status = false; 
  
  for(XDWORD c=0; c<layouts.GetSize(); c++)
    {
      UI_LAYOUT* layout = layouts.Get(c);
      if(layout)
        {     
          status = layout->Elements_SetToRedraw();
        }
    }
  
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Elements_SetToRedraw(UI_ELEMENT* element, bool recursive)
* @brief      Elements set to redraw
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  recursive : Recursive value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Elements_SetToRedraw(UI_ELEMENT* element, bool recursive)
{
  bool status = false; 
  
  for(XDWORD c=0; c<layouts.GetSize(); c++)
    {
      UI_LAYOUT* layout = layouts.Get(c);
      if(layout)
        {     
          status = layout->Elements_SetToRedraw(element, recursive);         
        }
    }

  // A scrolleable ancestor paints its scrollbar as an overlay and (now) owns the single rebuild area covering its whole
  // content. If a descendant repaints, that overlay/content must be repainted by the area owner. The owner is the
  // OUTERMOST clipping scrolleable ancestor (an inner one — e.g. a MultiOption — owns no area, since it is itself inside
  // the outer container). So walk the whole chain, keep the outermost, and mark it (recursively) once.
  if(element)
    {
      UI_ELEMENT* father = element->GetFather();
      UI_ELEMENT* owner  = NULL;
      while(father)
        {
          UI_PROPERTY_SCROLLEABLE* sc = dynamic_cast<UI_PROPERTY_SCROLLEABLE*>(father);
          if(sc && sc->Scroll_NeedClip()) owner = father;      // keep the outermost clipping scrolleable
          father = father->GetFather();
        }

      if(owner)
        {
          for(XDWORD c=0; c<layouts.GetSize(); c++)
            {
              UI_LAYOUT* layout = layouts.Get(c);
              if(layout) layout->Elements_SetToRedraw(owner, true);   // mark container + content as one unit (no ghosts:
            }                                                         // descendants own no areas, see PreDrawFunction)
        }
    }
  
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Elements_RebuildDrawAreas()
* @brief      Elements rebuild draw areas
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Elements_RebuildDrawAreas()
{  
  bool status = false; 

  for(XDWORD c=0; c<layouts.GetSize(); c++)
    {
      UI_LAYOUT* layout = layouts.Get(c);
      if(layout)
        {    
          if(layout->GetSkin())
            {
              UnSelectedElement();  

              switch(layout->GetSkin()->GetDrawMode())
                {
                  case UI_SKIN_DRAWMODE_UNKNOWN   : break;

                  case UI_SKIN_DRAWMODE_CANVAS    : { UI_SKINCANVAS* skincanvas = (UI_SKINCANVAS*)layout->GetSkin();
                                                      if(skincanvas) 
                                                        {
                                                          status = skincanvas->RebuildAllAreas();                                                                                
                                                        }
                                                    }
                                                    break;

                  case UI_SKIN_DRAWMODE_CONTEXT   : break;
                }
            }
        }
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Elements_RebuildDrawAreas(UI_LAYOUT* layout)
* @brief      Elements rebuild draw areas
* @ingroup    USERINTERFACE
* 
* @param[in]  layout : Layout pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Elements_RebuildDrawAreas(UI_LAYOUT* layout)
{  
  bool status = false; 

  if(!layout)
    {
      return false;
    }

  if(!layout->GetSkin())
    {
      return false;
    }

  UnSelectedElement();  

  switch(layout->GetSkin()->GetDrawMode())
    {
      case UI_SKIN_DRAWMODE_UNKNOWN   : break;

      case UI_SKIN_DRAWMODE_CANVAS    : { UI_SKINCANVAS* skincanvas = (UI_SKINCANVAS*)layout->GetSkin();
                                          if(skincanvas) 
                                            {
                                              status = skincanvas->RebuildAllAreas();                                                                                
                                            }
                                        }
                                        break;

      case UI_SKIN_DRAWMODE_CONTEXT   : break;
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Elements_RebuildDrawAreas(UI_LAYOUT* layout, UI_ELEMENT* element)
* @brief      Elements rebuild draw areas
* @ingroup    USERINTERFACE
* 
* @param[in]  layout : Layout pointer to use.
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Elements_RebuildDrawAreas(UI_LAYOUT* layout, UI_ELEMENT* element)
{
  bool status = false; 
  
  if(!layout)
    {
      return false;
    }

  if(!layout->GetSkin())
    {
      return false;
    }

  UnSelectedElement();  

  switch(layout->GetSkin()->GetDrawMode())
    {
      case UI_SKIN_DRAWMODE_UNKNOWN   : break;

      case UI_SKIN_DRAWMODE_CANVAS    : { UI_SKINCANVAS* skincanvas = (UI_SKINCANVAS*)layout->GetSkin();
                                          if(skincanvas) 
                                            {
                                              status = skincanvas->RebuildAllAreas(element);                                                                                
                                            }
                                        }
                                        break;

      case UI_SKIN_DRAWMODE_CONTEXT   : break;
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Elements_RebuildDrawAreas(XCHAR* layoutname)
* @brief      Elements rebuild draw areas
* @ingroup    USERINTERFACE
* 
* @param[in]  layoutname : Layoutname pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Elements_RebuildDrawAreas(XCHAR* layoutname)
{ 
  UI_LAYOUT* layout = NULL;
  
  layout = Layouts_Get(layoutname);
  if(!layout)
    {
      return false;
    }
  
  return Elements_RebuildDrawAreas(layout);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::Elements_RebuildDrawAreas(XCHAR* layoutname, UI_ELEMENT* element)
* @brief      Elements rebuild draw areas
* @ingroup    USERINTERFACE
* 
* @param[in]  layoutname : Layoutname pointer to use.
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::Elements_RebuildDrawAreas(XCHAR* layoutname, UI_ELEMENT* element)
{
  UI_LAYOUT* layout = NULL;
  
  layout = Layouts_Get(layoutname);
  if(!layout)
    {
      return false;
    }
  
  return Elements_RebuildDrawAreas(layout, element);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int UI_MANAGER::GetOutputTextChangeID(XSTRING* text, int start, XSTRING& value)
* @brief      Get output text change ID
* @ingroup    USERINTERFACE
* 
* @param[in]  text : Text to use.
* @param[in]  start : Start value.
* @param[in]  value : Value value.
* 
* @return     int : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
int UI_MANAGER::GetOutputTextChangeID(XSTRING* text, int start, XSTRING& value)
{
  if(!text) 
    {
      return false;
    }
  
  int ini_mask;
  int end_mask;

  ini_mask = text->Find(UI_MANAGER_LAYOUT_TEXT_INI, true, start);
  if(ini_mask != XSTRING_NOTFOUND)
    {
      end_mask = text->Find(UI_MANAGER_LAYOUT_TEXT_END, true, ini_mask);
      if(end_mask != XSTRING_NOTFOUND)
        {
          text->Copy(ini_mask+2, end_mask, value);

          return ini_mask;
        }       
    }                             

  return XSTRING_NOTFOUND;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ANIMATION* UI_MANAGER::GetOrAddAnimationCache(UI_SKIN_DRAWMODE drawmode, GRPPROPERTYMODE grppropertymode, XCHAR* name, XCHAR* resource, GRP2DCANVAS* referencecanvas, double width, double height)
* @brief      Get or add animation cache
* @note       When resource is a vector file (SVG, DXF...) AND referencecanvas/width/height are supplied, the
*             resource is rasterized at that exact size and the cache key folds in the size (e.g. "icon.svg@32x32"),
*             so the same vector file requested at two different sizes never collides in the cache and each size
*             gets its own cached bitmap. Bitmap-file callers (referencecanvas = NULL, the default) are completely
*             unaffected: same lookup, same key, same LoadFromFile() path as before.
* @ingroup    USERINTERFACE
* 
* @param[in]  drawmode : Drawmode value.
* @param[in]  grppropertymode : Grppropertymode value.
* @param[in]  name : Name to use.
* @param[in]  resource : Resource pointer to use.
* @param[in]  referencecanvas : Canvas to rasterize vector resources with (NULL = bitmap-only, legacy behavior).
* @param[in]  width : Target width, in pixels, to rasterize a vector resource at (ignored for bitmap files).
* @param[in]  height : Target height, in pixels, to rasterize a vector resource at (ignored for bitmap files).
* 
* @return     UI_ANIMATION* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ANIMATION* UI_MANAGER::GetOrAddAnimationCache(UI_SKIN_DRAWMODE drawmode, GRPPROPERTYMODE grppropertymode, XCHAR* name, XCHAR* resource, GRP2DCANVAS* referencecanvas, double width, double height)
{
  UI_ANIMATION* animation = NULL;
  bool          status    = false; 

  bool isvectorsized = (referencecanvas && (width > 0.0) && (height > 0.0) && IsVectorResource(resource));

  XSTRING cachekey;

  cachekey = resource;

  if(isvectorsized)
    {
      XSTRING sizepart;

      sizepart.Format(__L("@%dx%d"), (int)width, (int)height);
      cachekey.Add(sizepart);
    }

  animation = GEN_UI_ANIMATIONS.Get(name);
  if(animation) 
    {
      return animation;
    }

  animation = GEN_UI_ANIMATIONS.Get(cachekey);
  if(animation) 
    {
      return animation;
    }

  animation = GEN_NEW UI_ANIMATION();
  if(!animation) 
    {
      return NULL;
    }
  
  XSTRING resourcename;
  
  resourcename = resource;

  if(isvectorsized)
    {
      status = animation->LoadFromFileVector(resourcename, referencecanvas, width, height);
    }
   else
    {
      switch(drawmode)
        {
          case UI_SKIN_DRAWMODE_UNKNOWN   : break;

          case UI_SKIN_DRAWMODE_CANVAS    : status = animation->LoadFromFile(resourcename, grppropertymode);                                                                           
                                            break;

          case UI_SKIN_DRAWMODE_CONTEXT   : break;
        } 
    }

  if(status)   
    {      
      animation->GetName()->Set(name);  
      animation->GetResource()->Set(cachekey.Get());

      GEN_UI_ANIMATIONS.Add(animation);

      return animation;
    }
  
  GEN_DELETE animation;

  return NULL;  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::IsVectorResource(XCHAR* resource)
* @brief      Is vector resource
* @note       Detects a vector resource (SVG, DXF...) by its file extension, the same way GRPBITMAPFILE::GetTypeFromExtenxion
*             detects a bitmap file type, so GetOrAddAnimationCache() knows whether to rasterize it (mandatory
*             target size) instead of decoding it as a regular bitmap file.
* @ingroup    USERINTERFACE
* 
* @param[in]  resource : Resource pointer to use.
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::IsVectorResource(XCHAR* resource)
{
  if(!resource) return false;

  XPATH   pathresource;
  XSTRING ext;

  pathresource = resource;

  if(!pathresource.GetExt(ext)) return false;

  if(!ext.Compare(__L(".svg"), true)) return true;
  if(!ext.Compare(__L(".dxf"), true)) return true;

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::SetLevelAuto(UI_ELEMENT* element, UI_ELEMENT* father, XDWORD addlevel)
* @brief      Set level auto
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  father : Father pointer to use.
* @param[in]  addlevel : Addlevel value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::SetLevelAuto(UI_ELEMENT* element, UI_ELEMENT* father, XDWORD addlevel)
{
  if(father)
    {
      XDWORD zlevel_father = father->GetZLevel();    
      if(element) 
        {
          element->SetZLevel(element->GetZLevel() + zlevel_father + addlevel);
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::SetPreselectElement(UI_ELEMENT* element)
* @brief      Set preselect element
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::SetPreselectElement(UI_ELEMENT* element)
{
  preselect_element = element;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::ResetPreselect()
* @brief      Reset preselect
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::ResetPreselect()
{
  if(!preselect_element) 
    {
      return false;
    }

  preselect_element->SetPreSelect(false); 
   
  Elements_SetToRedraw(preselect_element);                                                                

  preselect_element = NULL; 

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::SendEvent(int event, ...)
* @brief      Send event
* @ingroup    USERINTERFACE
* 
* @param[in]  event : Event to process.
* @param[in]  ... : Variable argument list.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::SendEvent(int event, ...)
{
  UI_XEVENT xevent(this, event);
  va_list   arg;
  bool      postevent = false;
  bool      status    = false;

  if(xmutex_UIevent) 
    {
      xmutex_UIevent->Lock();
    }

  va_start(arg, event);
  
  switch(event)
    {
      case UI_XEVENT_TYPE_INPUT_CURSOR_MOVE         : 
      case UI_XEVENT_TYPE_INPUT_SELECCTION          : { int x = (int)va_arg(arg, int);
                                                        int y = (int)va_arg(arg, int);

                                                        xevent.SetXPos((XDWORD)x);
                                                        xevent.SetYPos((XDWORD)y);

                                                        postevent = true;
                                                      }
                                                      break;

      case UI_XEVENT_TYPE_INPUT_CURSOR_MOTION       : { void* ptr = (void*)va_arg(arg, void*);
                                                        if(ptr)
                                                          {
                                                            INPCURSORMOTION* cursormotion = (INPCURSORMOTION*)ptr;
                                                            if(cursormotion)
                                                              {                  
                                                                xevent.SetCursorMotion(cursormotion);
                                                                postevent = true;
                                                              }
                                                          }  
                                                      }
                                                      break;  

      case UI_XEVENT_TYPE_OUTPUT_SELECTED           : { void* ptr = (void*)va_arg(arg, void*);
                                                        if(ptr)
                                                          {   
                                                            UI_ELEMENT* element = (UI_ELEMENT*)ptr;
                                                            if(element) 
                                                              {
                                                                UI_ELEMENT* element_father = element->GetFather();
                                                        
                                                                if(element_father) 
                                                                  {
                                                                    if(!element_father->IsVisible())
                                                                      {
                                                                        break;
                                                                      }
                                                                  }

                                                                if(element->IsVisible()) 
                                                                  { 
                                                                    //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("UI Element [%s]: Selected. "), element->GetName()->Get());

                                                                    xevent.SetElement(element);          
                                                                    postevent = true;
                                                                  }
                                                              }
                                                          }
                                                      }
                                                      break;

      case UI_XEVENT_TYPE_OUTPUT_TEXTTOCHANGE       : { void* ptr = (void*)va_arg(arg, void*);
                                                        if(ptr)
                                                          {   
                                                            UI_ELEMENT* element = (UI_ELEMENT*)ptr;
                                                            if(element) 
                                                              {
                                                                XSTRING* maskvalue = (XSTRING*)va_arg(arg, void*);
                                                                if(maskvalue)  xevent.GetMaskTextValue()->Set(maskvalue->Get());

                                                                /*
                                                                ChangeAutomaticLiteralText((UI_ELEMENT_TEXT*)element, maskvalue, xevent.GetMaskText());
                                                                
                                                                if(!xevent.GetMaskText()->IsEmpty())
                                                                  {
                                                                    XSTRING* maskresolve = (XSTRING*)va_arg(arg, void*);
                                                                    if(maskresolve) maskresolve->Set(xevent.GetMaskText()->Get()); 
                                                                    postevent = false;
                                                                  }
                                                                 else   
                                                                 */
                                                                  {
                                                                    xevent.SetElement(element);  
                                                                    postevent = true;    
                                                                  }
                                                              }
                                                          }
                                                      }  
                                                      break;

       case UI_XEVENT_TYPE_OUTPUT_CHANGECONTENTS    : { void* ptr = (void*)va_arg(arg, void*);
                                                        if(ptr)
                                                          {   
                                                            UI_ELEMENT* element = (UI_ELEMENT*)ptr;
                                                            if(element) 
                                                              {
                                                                xevent.SetElement(element);                                                                        
                                                                postevent = true;
                                                              }  
                                                          }
                                                      }
                                                      break;

                                      default       : break;



    }
  
  if(postevent) 
    {          
      status = PostEvent(&xevent);         
      if(status)
        {  
          switch(event)
            {
              case UI_XEVENT_TYPE_OUTPUT_TEXTTOCHANGE   : { XSTRING* maskresolve = (XSTRING*)va_arg(arg, void*);
                                                            if(maskresolve) maskresolve->Set(xevent.GetMaskTextResolved()->Get()); 
                                                          }  
                                                          break;

                                          default       : break;
            }
        }  
    }

  va_end(arg);

  if(xmutex_UIevent) xmutex_UIevent->UnLock();

  return status;

}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::ChangeAutomaticTextElementValue(UI_ELEMENT* element, XSTRING* maskvalue, XSTRING* maskresolved)
* @brief      Change automatic text element value
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  maskvalue : Maskvalue pointer to use.
* @param[in]  maskresolved : Maskresolved pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::ChangeAutomaticTextElementValue(UI_ELEMENT* element, XSTRING* maskvalue, XSTRING* maskresolved)
{
  if(!element)        return false;
  if(!maskvalue)      return false;
  if(!maskresolved)   return false;

  UI_ELEMENT* father = element->GetFather();
  if(father)
    {
      if(father->GetType() == UI_ELEMENT_TYPE_PROGRESSBAR     || 
         father->GetType() == UI_ELEMENT_TYPE_PROGRESSRADIAL  ||
         father->GetType() == UI_ELEMENT_TYPE_PROGRESSIMAGE)
        {
          if(!maskvalue->Compare(__L("PROGRESSBAR_PERCENT"), true))
            {
              float level = 0.0f;

              switch(father->GetType())                                                          // read the level from the ACTUAL father type: each progress class owns its
                {                                                                                // own non-virtual GetLevel()/level member at a different offset, so a blind
                  case UI_ELEMENT_TYPE_PROGRESSBAR    : level = ((UI_ELEMENT_PROGRESSBAR*)father)->GetLevel();    break;   // (PROGRESSBAR*) cast read the wrong memory for IMAGE -> always 0%.
                  case UI_ELEMENT_TYPE_PROGRESSRADIAL : level = ((UI_ELEMENT_PROGRESS_RADIAL*)father)->GetLevel(); break;
                  case UI_ELEMENT_TYPE_PROGRESSIMAGE  : level = ((UI_ELEMENT_PROGRESS_IMAGE*)father)->GetLevel();  break;
                  default                             : break;
                }

              maskresolved->Format(__L("%d"), (int)level);
              maskresolved->Add(__L("\x25"));

              return true;
            }
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::ChangeTextElementValue(UI_ELEMENT* element, XSTRING* text, XSTRING& value)
* @brief      Change text element value
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  text : Text to use.
* @param[in]  value : Value value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::ChangeTextElementValue(UI_ELEMENT* element, XSTRING* text, XSTRING& value)
{
  XSTRING  maskvalue;
  XSTRING  maskresolve;  
  int      start      = 0;
  int      new_start  = 0;
  bool     change     = false;
  
  do{ new_start = GEN_USERINTERFACE.GetOutputTextChangeID(text, start, maskvalue);

      if(new_start != XSTRING_NOTFOUND)
        {
          XSTRING subtext;
       
          text->Copy(start, new_start, subtext);
          if(!subtext.IsEmpty()) value.Add(subtext);
           
          maskresolve.Empty();

          if(ChangeAutomaticTextElementValue(element, &maskvalue, &maskresolve))
            {                
              value.Add(maskresolve);                 
              change = true;                    
            }
           else 
            {
              SendEvent(UI_XEVENT_TYPE_OUTPUT_TEXTTOCHANGE, element, &maskvalue, &maskresolve);
              if(!maskresolve.IsEmpty()) 
                {
                  value.Add(maskresolve);                 
                  change = true;              
                }  
            }

          start = new_start + maskvalue.GetSize() + 3;               

        }
       else 
        {
          XSTRING subtext;

          text->Copy(start, subtext);
          value.Add(subtext);                                  
        }

    } while(new_start != XSTRING_NOTFOUND);

  return change;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::ChangeTextElementValue(UI_LAYOUT* layout, UI_ELEMENT* element)
* @brief      Change text element value
* @ingroup    USERINTERFACE
* 
* @param[in]  layout : Layout pointer to use.
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::ChangeTextElementValue(UI_LAYOUT* layout, UI_ELEMENT* element)
{
  if(!element) 
    {
      return false;
    }

  if(!layout)
    {
      return false;
    }

  if(!layout->GetSkin())
    {
      return false;
    }

  bool change = false;

  switch(element->GetType())
    {
      case  UI_ELEMENT_TYPE_TEXT      : { UI_ELEMENT_TEXT*  element_text = (UI_ELEMENT_TEXT*)element;
                                          XSTRING           resolve;

                                          if(element_text->GetMaskText()->IsEmpty())    return false;
                                                                                                                         
                                          change = ChangeTextElementValue(element_text, element_text->GetMaskText(), resolve);   
                                                                                                            
                                          if(change)
                                            {                                                                                                                                                                                                                                                                                                                                                                                                                    
                                              switch(layout->GetSkin()->GetDrawMode())
                                                {
                                                  case UI_SKIN_DRAWMODE_UNKNOWN   : break;

                                                  case UI_SKIN_DRAWMODE_CANVAS    : { double width  = 0.0f;
                                                                                      double height = 0.0f;
                                                                                      
                                                                                      UI_SKINCANVAS* ui_skincanvas = (UI_SKINCANVAS*)layout->GetSkin(); 

                                                                                      UI_ELEMENT* father =(UI_ELEMENT_TEXT*)element->GetFather(); 
                                                                                      if(father)
                                                                                        {
                                                                                          width  = father->GetBoundaryLine()->width;
                                                                                          height = father->GetBoundaryLine()->height;

                                                                                          Elements_SetToRedraw(father);    
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                          width  =  ui_skincanvas->GetCanvas()->GetWidth();
                                                                                          height =  ui_skincanvas->GetCanvas()->GetHeight();
                                                                                        }                                 

                                                                                      element->GetBoundaryLine()->height  = 0;
                                                                                      element->GetBoundaryLine()->width   = 0;
                                                                                         
                                                                                      if(element_text->GetText()->Compare(resolve.Get(), true))  
                                                                                        { 
                                                                                          element_text->GetText()->Set(resolve);   
                                                                                              
                                                                                          ui_skincanvas->CalculeBoundaryLine_AllElements(element, false);
                                                                                          ui_skincanvas->CalculePosition(element, width, height);

                                                                                          if(father && ((father->GetType() == UI_ELEMENT_TYPE_PROGRESSBAR) || (father->GetType() == UI_ELEMENT_TYPE_PROGRESSRADIAL) || (father->GetType() == UI_ELEMENT_TYPE_PROGRESSIMAGE)))
                                                                                            {
                                                                                              ui_skincanvas->CalculateBoundaryLine(father, false);   // re-apply progress allocationtext/boundary with the new text size (don't leave the text mis-placed by the plain CalculePosition above)
                                                                                            }                                                                                              

                                                                                          Elements_SetToRedraw(element);    
                                                                                        }
                                                                                    }
                                                                                    break;

                                                  case UI_SKIN_DRAWMODE_CONTEXT   : break;
                                                }                                                                                                             
                                                                                             
                                            }
                                        }
                                        break;

      case UI_ELEMENT_TYPE_TEXTBOX    : { UI_ELEMENT_TEXTBOX* element_textbox = (UI_ELEMENT_TEXTBOX*)element;
                                          XSTRING             resolve;

                                          if(element_textbox->GetMaskText()->IsEmpty()) return false;

                                          change = ChangeTextElementValue(element_textbox, element_textbox->GetMaskText(), resolve);                                      
                                          if(change) 
                                            {                                             
                                              if(element_textbox->GetText()->Compare(resolve.Get(), true)) 
                                                {
                                                  element_textbox->GetText()->Set(resolve);
                                                  Elements_SetToRedraw(element);    
                                                }
                                            }
                                           else
                                            {
                                              if(element_textbox->GetText()->IsEmpty())
                                                {
                                                  if(!resolve.IsEmpty())  
                                                    {
                                                      element_textbox->GetText()->Set(resolve);
                                                    }
                                                }
                                            }


                                        }  
                                        break;

                        default       : break;
    }


  for(XDWORD c=0; c<element->GetComposeElements()->GetSize(); c++)
    {
      UI_ELEMENT* subelement = element->GetComposeElements()->Get(c);
      if(subelement) 
        {
          ChangeTextElementValue(layout, subelement);
        }
    }
  
  return true;
 }


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::ChangeTextElementValue(UI_LAYOUT* layout)
* @brief      Change text element value
* @ingroup    USERINTERFACE
* 
* @param[in]  layout : Layout pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
 bool UI_MANAGER::ChangeTextElementValue(UI_LAYOUT* layout)
{
  if(!layout) 
    {
      return false;
    }
 
  for(XDWORD c=0; c<layout->Elements_Get()->GetSize(); c++)
    {
      UI_ELEMENT* element = layout->Elements_Get()->Get(c);
      if(element) 
        {
          ChangeTextElementValue(layout, element);       
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::SubscribeInputEvents(bool active)
* @brief      Subscribe input events
* @ingroup    USERINTERFACE
* 
* @param[in]  active : Active value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::SubscribeInputEvents(bool active)
{
  UI_XEVENT_TYPE eventtype[] = {  UI_XEVENT_TYPE_INPUT_CURSOR_MOVE            ,
                                  UI_XEVENT_TYPE_INPUT_CURSOR_MOTION          ,
                                  UI_XEVENT_TYPE_INPUT_SELECCTION             ,                                           
                               };

  for(int c=0; c<(sizeof(eventtype) / sizeof(UI_XEVENT_TYPE)); c++)
    {
      if(active)        
            SubscribeEvent(eventtype[c]   , this);              
      else  UnSubscribeEvent(eventtype[c] , this);        
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::SubscribeOutputEvents(bool active, XOBSERVER* observer, XSUBJECT* subject)
* @brief      Subscribe output events
* @ingroup    USERINTERFACE
* 
* @param[in]  active : Active value.
* @param[in]  observer : Observer pointer to use.
* @param[in]  subject : Subject pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::SubscribeOutputEvents(bool active, XOBSERVER* observer, XSUBJECT* subject)
{
  UI_XEVENT_TYPE eventtype[] = {  UI_XEVENT_TYPE_OUTPUT_SELECTED          ,                              
                                  UI_XEVENT_TYPE_OUTPUT_TEXTTOCHANGE      ,
                                  UI_XEVENT_TYPE_OUTPUT_CHANGECONTENTS                                     
                               };

  for(int c=0; c<(sizeof(eventtype) / sizeof(UI_XEVENT_TYPE)); c++)
    {
      if(active)        
            observer->SubscribeEvent(eventtype[c]   , subject);              
      else  observer->UnSubscribeEvent(eventtype[c] , subject);        
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::CreaterVirtualKeyboard(UI_LAYOUT* layout, GRPSCREEN* screen)
* @brief      Creater virtual keyboard
* @ingroup    USERINTERFACE
* 
* @param[in]  layout : Layout pointer to use.
* @param[in]  screen : Screen object to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::CreaterVirtualKeyboard(UI_LAYOUT* layout, GRPSCREEN* screen)
{
  if(virtualkeyboard) 
    {
      return false;   
    }

  if(!layout)         
    {
      return false;
    }
  
  if(!screen)         
    {
      return false;
    }

  virtualkeyboard = GEN_NEW UI_VIRTUALKEYBOARD();
  if(!virtualkeyboard) 
    {
      return false;
    }

  virtualkeyboard->Ini(layout, screen);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::DeleteVirtualKeyboard()
* @brief      Delete virtual keyboard
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::DeleteVirtualKeyboard()
{
  if(!virtualkeyboard)
    {
      return false;
    }

  bool status = false;

  status = virtualkeyboard->End();

  GEN_DELETE virtualkeyboard;
  virtualkeyboard = NULL;

  return status;    
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_MANAGER::UI_MANAGER()
* @brief      Constructor of class
* @ingroup    USERINTERFACE
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_MANAGER::UI_MANAGER()    
{ 
  Clean();   

  xmutex_modal=GEN_XFACTORY.Create_Mutex();
  xmutex_UIevent=GEN_XFACTORY.Create_Mutex();

  RegisterEvents(true);                        
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_MANAGER::~UI_MANAGER()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    USERINTERFACE
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_MANAGER::~UI_MANAGER()    
{ 
  RegisterEvents(false); 

  DeleteVirtualKeyboard();

  Layouts_DeleteAll(); 
  
  if(unzipfile)
    {
      GEN_DELETE unzipfile;
      unzipfile = NULL;
    }  

  if(xmutex_modal)
    {
      GEN_XFACTORY.Delete_Mutex(xmutex_modal);
      xmutex_modal = NULL;
    }

  if(xmutex_UIevent)
    {
      GEN_XFACTORY.Delete_Mutex(xmutex_UIevent);
      xmutex_UIevent = NULL;
    }

  Clean();                            
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_SKIN* UI_MANAGER::Skin_Create(XSTRING& skintypename, UI_SKIN_DRAWMODE drawmode, GRPSCREEN* screen, int viewportindex)
* @brief      Skin create
* @ingroup    USERINTERFACE
* 
* @param[in]  skintypename : Skintypename value.
* @param[in]  drawmode : Drawmode value.
* @param[in]  screen : Screen object to use.
* @param[in]  viewportindex : Viewportindex value.
* 
* @return     UI_SKIN* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_SKIN* UI_MANAGER::Skin_Create(XSTRING& skintypename, UI_SKIN_DRAWMODE drawmode, GRPSCREEN* screen, int viewportindex)
{  
  if(!screen) 
    {
      return NULL;
    }

  UI_SKIN*  ui_skin;

  switch(drawmode)
    {
      case UI_SKIN_DRAWMODE_UNKNOWN     : return NULL;
                                         
      case UI_SKIN_DRAWMODE_CANVAS      : { UI_SKINCANVAS* skincanvas = NULL;

                                            if(!skintypename.Compare(UI_SKIN_NAME_DEFAULT, true) || !skintypename.Compare(UI_SKIN_NAME_UNKNOWN, true))
                                              {
                                                ui_skin = (UI_SKIN*)GEN_NEW UI_SKINCANVAS(screen, viewportindex); 
                                              }
                                             else
                                              {
                                                if(!skintypename.Compare(UI_SKIN_NAME_FLAT, true))
                                                  {
                                                    ui_skin = (UI_SKIN*)GEN_NEW UI_SKINCANVAS_FLAT(screen, viewportindex); 
                                                  }
                                              }
                                          }
                                          break;

      case UI_SKIN_DRAWMODE_CONTEXT     : break;

    }
  
  return ui_skin;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::GetLayoutElementValue(XFILEXMLELEMENT* node, XCHAR* leyend, double& value)
* @brief      Get layout element value
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  leyend : Leyend pointer to use.
* @param[in]  value : Value value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::GetLayoutElementValue(XFILEXMLELEMENT* node, XCHAR* leyend, double& value)
{
  XCHAR*    xcharstr;
  XSTRING   valuestr;

  valuestr.Empty();

  xcharstr = node->GetValueAttribute(leyend);
  if(!xcharstr) return false;

  valuestr = xcharstr;

  value = valuestr.ConvertToDouble();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::GetLayoutElementValue(XFILEXMLELEMENT* node, XCHAR* leyend, XSTRING& value)
* @brief      Get layout element value
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  leyend : Leyend pointer to use.
* @param[in]  value : Value value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::GetLayoutElementValue(XFILEXMLELEMENT* node, XCHAR* leyend, XSTRING& value)
{
  XCHAR* xcharstr;

  value.Empty();

  xcharstr = node->GetValueAttribute(leyend);
  if(!xcharstr) return false;

  value = xcharstr;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::GetLayoutElement_CalculateBoundaryLine(UI_LAYOUT* layout, UI_ELEMENT* element, bool adjustsizemargin)
* @brief      Get layout element calculate boundary line
* @ingroup    USERINTERFACE
* 
* @param[in]  layout : Layout pointer to use.
* @param[in]  element : Element to process.
* @param[in]  adjustsizemargin : Adjustsizemargin value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::GetLayoutElement_CalculateBoundaryLine(UI_LAYOUT* layout, UI_ELEMENT* element, bool adjustsizemargin)
{
  bool status = false;

 if(!element) 
    {
      return false;
    }

  if(!layout)
    {
      return false;
    }

  if(!layout->GetSkin())
    {
      return false;
    }

  switch(layout->GetSkin()->GetDrawMode())
    {
      case UI_SKIN_DRAWMODE_UNKNOWN   : break;

      case UI_SKIN_DRAWMODE_CANVAS    : { UI_SKINCANVAS* skincanvas = (UI_SKINCANVAS*)layout->GetSkin(); 
                                          if(skincanvas) 
                                            {
                                              status = skincanvas->CalculateBoundaryLine(element, adjustsizemargin);                                                                                    
                                            }
                                        }
                                        break;

      case UI_SKIN_DRAWMODE_CONTEXT   : break;
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::GetParentSizeFont(XFILEXMLELEMENT* node, double& sizefont)
* @brief      Get parent size font
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  sizefont : Sizefont value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::GetParentSizeFont(XFILEXMLELEMENT* node, double& sizefont)
{
  if(!node) return false;

  sizefont = 0;
    
  if(!GetLayoutElementValue(node, __L("sizefont"), sizefont))
    {
      return GetParentSizeFont(node->GetFather(), sizefont);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::GetLayoutElement_Base(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* element, bool adjusttoparent)
* @brief      Get layout element base
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  element : Element to process.
* @param[in]  adjusttoparent : Adjusttoparent value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::GetLayoutElement_Base(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* element, bool adjusttoparent)
{
  UI_STYLE style;
  style.FillFromXMLElement(node);

  XSTRING fathertagname;
  if(node && node->GetFather()) fathertagname = node->GetFather()->GetName();

  return GetLayoutElement_Base(style, fathertagname, layout, element, adjusttoparent);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::GetLayoutElement_Base(UI_STYLE& style, XSTRING& fathertagname, UI_LAYOUT* layout, UI_ELEMENT* element, bool adjusttoparent)
* @brief      Get layout element base from a neutral style bag (source-format independent).
* @ingroup    USERINTERFACE
* 
* @param[in]  style : Style value.
* @param[in]  fathertagname : Fathertagname value.
* @param[in]  layout : Layout pointer to use.
* @param[in]  element : Element to process.
* @param[in]  adjusttoparent : Adjusttoparent value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::GetLayoutElement_Base(UI_STYLE& style, XSTRING& fathertagname, UI_LAYOUT* layout, UI_ELEMENT* element, bool adjusttoparent)
{
  double xpos   = 0.0f;
  double ypos   = 0.0f;
  double width  = 0.0f;
  double height = 0.0f;

  if(!element) return false;

  element->SetIsDetached(false);

  if(!fathertagname.Compare(__L("layout"), true)) element->SetIsDetached(true);

  XSTRING name;
  style.Get(__L("name"), name);
  element->GetName()->Set(name);

  XSTRING position;
  if(style.Get(__L("xpos"), position))
    {
      if(!position.Compare(__L("left"), true))  xpos = UI_ELEMENT_TYPE_ALIGN_LEFT;
        else if(!position.Compare(__L("right"), true))  xpos = UI_ELEMENT_TYPE_ALIGN_RIGHT;
          else if(!position.Compare(__L("center"), true)) xpos = UI_ELEMENT_TYPE_ALIGN_CENTER;
            else style.Get(__L("xpos"), xpos);
    }

  if(style.Get(__L("ypos"), position))
    {
      if(!position.Compare(__L("up"), true))  ypos = UI_ELEMENT_TYPE_ALIGN_UP;
        else if(!position.Compare(__L("down"), true))  ypos = UI_ELEMENT_TYPE_ALIGN_DOWN;
          else if(!position.Compare(__L("center"), true)) ypos = UI_ELEMENT_TYPE_ALIGN_CENTER;
            else style.Get(__L("ypos"), ypos);
    }


  XSTRING size;
  if(style.Get(__L("width"), size))
    {
      if(!size.Compare(__L("max"), true))  width = UI_ELEMENT_TYPE_ALIGN_MAX;
        else if(!size.Compare(__L("auto"), true))  width = UI_ELEMENT_TYPE_ALIGN_AUTO;
            else style.Get(__L("width"), width);
    }
   else
    {
      if(element->GetFather() && adjusttoparent)
        {
          width = element->GetFather()->GetBoundaryLine()->width;
        }
    }

  if(style.Get(__L("height"), size))
    {
      if(!size.Compare(__L("max"), true))  height = UI_ELEMENT_TYPE_ALIGN_MAX;
        else if(!size.Compare(__L("auto"), true))  height = UI_ELEMENT_TYPE_ALIGN_AUTO;
            else style.Get(__L("height"), height);
    }
   else
    {
      if(element->GetFather() && adjusttoparent)
        {
          height = element->GetFather()->GetBoundaryLine()->height;
        }
    }

  element->GetBoundaryLine()->x       = xpos;
  element->GetBoundaryLine()->y       = ypos;
  element->GetBoundaryLine()->width   = width;
  element->GetBoundaryLine()->height  = height;

  XSTRING directionstr;
  if(style.Get(__L("direction"), directionstr))
    {
      if(!directionstr.Compare(__L("horizontal"), true))  element->SetDirection(UI_ELEMENT_TYPE_DIRECTION_HORIZONTAL);
        else if(!directionstr.Compare(__L("vertical"), true))  element->SetDirection(UI_ELEMENT_TYPE_DIRECTION_VERTICAL);
    }

  // NOTE: "role" is reserved for GEN custom Chromes (window caption) layouts: it lets GRPSCREEN find "the close
  // button", "the title", etc. without depending on element names. Any element type (image, button, text, a plain
  // rectangle...) can carry it; unrecognized/absent values leave chromerole at UI_ELEMENT_CHROMEROLE_NONE.
  XSTRING rolestr;
  if(style.Get(__L("role"), rolestr))
    {
      if(!rolestr.Compare(__L("caption")  , true))  element->SetChromeRole(UI_ELEMENT_CHROMEROLE_CAPTION);
        else if(!rolestr.Compare(__L("icon")     , true))  element->SetChromeRole(UI_ELEMENT_CHROMEROLE_ICON);
          else if(!rolestr.Compare(__L("title")    , true))  element->SetChromeRole(UI_ELEMENT_CHROMEROLE_TITLE);
            else if(!rolestr.Compare(__L("minimize") , true))  element->SetChromeRole(UI_ELEMENT_CHROMEROLE_MINIMIZE);
              else if(!rolestr.Compare(__L("maximize") , true))  element->SetChromeRole(UI_ELEMENT_CHROMEROLE_MAXIMIZE);
                else if(!rolestr.Compare(__L("close")    , true))  element->SetChromeRole(UI_ELEMENT_CHROMEROLE_CLOSE);
    }

  XSTRING color;
  style.Get(__L("color"), color);
  if(!color.IsEmpty()) element->GetColor()->SetFromString(color);

  XSTRING bckgrdcolor;
  style.Get(__L("bckgrdcolor"), bckgrdcolor);
  if(!bckgrdcolor.IsEmpty()) element->GetBackgroundColor()->SetFromString(bckgrdcolor);

  XSTRING visible;
  if(style.Get(__L("visible"), visible))
    {
      element->SetVisible(visible.ConvertToBoolean());
    }

  XSTRING hasscroll;
  if(style.Get(__L("scroll"), hasscroll))
    {
      element->SetHasScroll(hasscroll.ConvertToBoolean());
    }

  double roundrect = 0.0f;
  style.Get(__L("roundrect"), roundrect);
  element->SetRoundRect((XDWORD)roundrect);

  double blinktime;
  if(style.Get(__L("blink"), blinktime)) element->SetBlink((XDWORD)blinktime);

  XSTRING extra;
  style.Get(__L("extra"), extra);
  element->GetExtra()->Set(extra);

  XSTRING marginstr;
  if(style.Get(__L("margin"), marginstr))
    {
      int margin[UI_ELEMENT_MARGIN_MAX] = { 0, 0, 0, 0 };

      marginstr.UnFormat(__L("%d,%d,%d,%d") , &margin[0]
                                            , &margin[1]
                                            , &margin[2]
                                            , &margin[3]);

      element->SetMargin(UI_ELEMENT_TYPE_ALIGN_LEFT   , (double)margin[0]);
      element->SetMargin(UI_ELEMENT_TYPE_ALIGN_RIGHT  , (double)margin[1]);
      element->SetMargin(UI_ELEMENT_TYPE_ALIGN_UP     , (double)margin[2]);
      element->SetMargin(UI_ELEMENT_TYPE_ALIGN_DOWN   , (double)margin[3]);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::GetLayoutElement_Text(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
* @brief      Get layout element text
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* @param[in]  element_legacy : Element legacy pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::GetLayoutElement_Text(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
{
  XSTRING           text;
  double            sizefont      = 0;
  bool              havemask      = false;
  UI_ELEMENT_TEXT*  element_text  = NULL;
  
  if(element_legacy)
    {
      element_text = (UI_ELEMENT_TEXT*)element_legacy;
    }
   else
    {
      element_text = GEN_NEW UI_ELEMENT_TEXT();
      if(!element_text) return NULL;
    }

  element_text->SetFather(father);


  if(!GetLayoutElement_Base(node, layout, element_text))
    {
      GEN_DELETE element_text;
      return NULL;
    }

  SetLevelAuto(element_text, father);

  if(!GetLayoutElementValue(node, __L("sizefont"), sizefont))
    {
      if(!GetParentSizeFont(node->GetFather(), sizefont))
        {
          GEN_DELETE element_text;
          return NULL;
        }
    }

  element_text->SetSizeFont((XDWORD)sizefont);

  text = node->GetValue().Get();

  XSTRING* textcache = GEN_UI_TEXTS.Get(text);
  if(textcache)  text = textcache->Get();

  if(text.Find(UI_MANAGER_LAYOUT_TEXT_INI, true) != XSTRING_NOTFOUND) 
    {
      if(text.Find(UI_MANAGER_LAYOUT_TEXT_END, true) != XSTRING_NOTFOUND)  havemask = true;
    }
  
  if(havemask)
    {
      element_text->GetMaskText()->Set(text.Get());
      element_text->GetText()->Empty();
      
    } 
   else
    { 
      element_text->GetMaskText()->Empty();
      element_text->GetText()->Set(text.Get());
    }

  double maxsizetext = 0.0f;  
  if(GetLayoutElementValue(node, __L("maxsizetext"), maxsizetext))
    {               
      element_text->SetMaxSizeText((XDWORD)maxsizetext);    
    }

  if(!element_text->GetMaskText()->IsEmpty()) 
    {
      XSTRING  maskvalue;
      XSTRING  maskresolved;
      XSTRING  _text;
      int      start      = 0;
      int      new_start  = XSTRING_NOTFOUND;
      bool     change     = false;
  
      do{ int new_start = GEN_USERINTERFACE.GetOutputTextChangeID(element_text->GetMaskText(), start, maskvalue);
          if(new_start != XSTRING_NOTFOUND)
            {                                    
              if(maskvalue.Find(UI_MANAGER_LAYOUT_TEXT_TRANSLATE, true) != XSTRING_NOTFOUND)
                {
                  XCHAR*    translate_str  = NULL;
                  XSTRING   maskID;
                  int       ID             = 0;

                  maskID  = UI_MANAGER_LAYOUT_TEXT_TRANSLATE;
                  maskID += __L("%d");

                  maskvalue.UnFormat(maskID.Get(), &ID);

                  translate_str = GEN_XTRANSLATION.Translate_GetSentence(ID);
                  if(translate_str)  maskresolved.Set(translate_str);
                } 
  
              if(!maskresolved.IsEmpty()) 
                {
                  element_text->GetMaskText()->DeleteCharacters(new_start, maskvalue.GetSize() +3 ); 
                  element_text->GetMaskText()->Insert(maskresolved, new_start);  
                  change = true;                
                }
            }

        } while(new_start != XSTRING_NOTFOUND);    


      if(change)
        {
          havemask = false;

          text = element_text->GetMaskText()->Get();
          if(text.Find(UI_MANAGER_LAYOUT_TEXT_INI, true) != XSTRING_NOTFOUND) 
            {
              if(text.Find(UI_MANAGER_LAYOUT_TEXT_END, true) != XSTRING_NOTFOUND)  havemask = true;
            }
  
          if(havemask)
            {
              element_text->GetMaskText()->Set(text.Get());
              element_text->GetText()->Empty();
      
            } 
           else
            { 
              element_text->GetMaskText()->Empty();
              element_text->GetText()->Set(text.Get());
            } 
         }
  
    }
  
  GetLayoutElement_CalculateBoundaryLine(layout, element_text);
   
  return element_text;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::GetLayoutElement_TextBox(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
* @brief      Get layout element text box
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* @param[in]  element_legacy : Element legacy pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::GetLayoutElement_TextBox(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
{  
  UI_ELEMENT_TEXTBOX* element_textbox   = NULL;
  XSTRING             text;

  if(element_legacy)
    {
      element_textbox = (UI_ELEMENT_TEXTBOX*)element_legacy;
    }
   else
    {
      element_textbox = GEN_NEW UI_ELEMENT_TEXTBOX();
      if(!element_textbox) return NULL;
    }

  element_textbox->SetFather(father);

  if(!GetLayoutElement_Base(node, layout, element_textbox, true))
    {
      GEN_DELETE element_textbox;
      return NULL;
    }

  SetLevelAuto(element_textbox, father);

  if(!element_textbox->GetBoundaryLine()->width)  return NULL;
  if(!element_textbox->GetBoundaryLine()->height) return NULL;

  double sizefont = 0;  
  if(!GetLayoutElementValue(node, __L("sizefont"), sizefont))
    {
      if(!GetParentSizeFont(node->GetFather(), sizefont))
        {
          GEN_DELETE element_textbox;
          return NULL;
        }
    }

  element_textbox->SetSizeFont((XDWORD)sizefont);

  double linespacing = UI_ELEMENT_TEXTBOX_DEFAULTLINESPACING;
  GetLayoutElementValue(node, __L("linespacing"), linespacing);     
  element_textbox->SetLineSpacing((XDWORD)linespacing);

  XSTRING                textalignmentstr;
  UI_ELEMENT_TYPE_ALIGN  textalignment;
  if(GetLayoutElementValue(node, __L("textalignment"), textalignmentstr))
    {
      if(!textalignmentstr.Compare(__L("left"), true))  textalignment = UI_ELEMENT_TYPE_ALIGN_LEFT;
        else if(!textalignmentstr.Compare(__L("right"), true))  textalignment = UI_ELEMENT_TYPE_ALIGN_RIGHT;
          else if(!textalignmentstr.Compare(__L("center"), true)) textalignment = UI_ELEMENT_TYPE_ALIGN_CENTER;
    }
  element_textbox->SetTextAlignment(textalignment);


  XSTRING wordclippingstr;  
  element_textbox->SetIsWordClipping(false);
  if(GetLayoutElementValue(node, __L("wordclipping"), wordclippingstr))
    {
      element_textbox->SetIsWordClipping(wordclippingstr.ConvertToBoolean());         
    }

  text.Empty();

  for(int c=0; c<node->GetNElements(); c++)
    {
      XFILEXMLELEMENT* subnode = node->GetElement(c);
      if(subnode)
        {
          if(!subnode->GetName().Compare(__L("text"), true))
            {                                                                  
              text.Add(subnode->GetValue().Get());
              text.Add(__L("\n"));
            }    
        }
    }
  
  if(!text.IsEmpty()) 
    {
      XSTRING  value;
      XSTRING  resolved;      
      int      start      = 0;
      int      new_start  = XSTRING_NOTFOUND;
      bool     change     = false;
  
      do{ int new_start = GEN_USERINTERFACE.GetOutputTextChangeID(&text, start, value);
          if(new_start != XSTRING_NOTFOUND)
            {                                    
              if(value.Find(UI_MANAGER_LAYOUT_TEXT_TRANSLATE, true) != XSTRING_NOTFOUND)
                {
                  XCHAR*    translate_str  = NULL;
                  XSTRING   IDstr;
                  int       ID             = 0;

                  IDstr  = UI_MANAGER_LAYOUT_TEXT_TRANSLATE;
                  IDstr += __L("%d");

                  value.UnFormat(IDstr.Get(), &ID);

                  translate_str = GEN_XTRANSLATION.Translate_GetSentence(ID);
                  if(translate_str) resolved.Set(translate_str);
                } 
  
              if(!resolved.IsEmpty()) 
                {
                  text.DeleteCharacters(new_start, value.GetSize() +3 ); 
                  text.Insert(resolved, new_start);  
                  change = true;                
                }
            }

        } while(new_start != XSTRING_NOTFOUND);      
    }

  element_textbox->GetMaskText()->Set(text);

  GetLayoutElement_CalculateBoundaryLine(layout, element_textbox, true);

  return element_textbox;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::GetLayoutElement_Image(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
* @brief      Get layout element image
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* @param[in]  element_legacy : Element legacy pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::GetLayoutElement_Image(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
{
  XPATH             xpathimg;
  XSTRING           namefileimg;
  UI_ELEMENT_IMAGE* element_image = NULL;

  if(element_legacy)
    {
      element_image = (UI_ELEMENT_IMAGE*)element_legacy;
    }
   else
    {
      element_image = GEN_NEW UI_ELEMENT_IMAGE();
      if(!element_image) return NULL;
    }

  element_image->SetFather(father);
  
  if(!GetLayoutElement_Base(node, layout, element_image))
    {
      GEN_DELETE element_image;
      return NULL;
    }

  SetLevelAuto(element_image, father);

  double alpha = 0;
  if(GetLayoutElementValue(node, __L("alpha"), alpha))
    {
      element_image->SetAlpha((XBYTE)alpha);
    }

  namefileimg = node->GetValue().Get();

 
  if(!namefileimg.IsEmpty()) 
    {
      GRPPROPERTYMODE   grppropertymode = GRPPROPERTYMODE_XX_UNKNOWN;
      UI_SKIN_DRAWMODE  drawmode        = UI_SKIN_DRAWMODE_UNKNOWN;
      GRP2DCANVAS*      referencecanvas = NULL;

      if(layout->GetSkin())
        {
          drawmode = layout->GetSkin()->GetDrawMode();

          switch(drawmode)
            {
              case UI_SKIN_DRAWMODE_UNKNOWN   : break;

              case UI_SKIN_DRAWMODE_CANVAS    : { UI_SKINCANVAS* skincanvas = (UI_SKINCANVAS*)layout->GetSkin(); 
                                                  if(skincanvas)
                                                    {
                                                      referencecanvas = skincanvas->GetCanvas();
                                                      grppropertymode = referencecanvas->GetMode();
                                                    }
                                                }
                                                break;
   
              case UI_SKIN_DRAWMODE_CONTEXT   : break;
            }  
        }

      // NOTE: a vector resource (SVG, DXF...) has no pixel size of its own, so it is always rasterized at the
      // element's own configured width/height (already resolved by GetLayoutElement_Base() above). Bitmap files
      // ignore these two values entirely (see GetOrAddAnimationCache()).
      double width  = element_image->GetBoundaryLine()->width;
      double height = element_image->GetBoundaryLine()->height;

      UI_ANIMATION* animation = GetOrAddAnimationCache(drawmode, grppropertymode, __L(""), namefileimg.Get(), referencecanvas, width, height);
      if(animation) 
        { 
          GRPBITMAP* bitmap = NULL; 
          bitmap = animation->GetBitmap(); 
          if(bitmap) element_image->SetImage(bitmap);             
        }
    }
     
  GetLayoutElement_CalculateBoundaryLine(layout, element_image);
  
  return element_image;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::GetLayoutElement_Animation(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
* @brief      Get layout element animation
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* @param[in]  element_legacy : Element legacy pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::GetLayoutElement_Animation(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
{
  XSTRING               text;
  XSTRING               namefileimg;
  XPATH                 xpathimg;
  int                   sizefont          = 0;
  UI_ELEMENT_ANIMATION* element_animation = NULL;

  if(element_legacy)
    {
      element_animation = (UI_ELEMENT_ANIMATION*)element_legacy;
    }
   else
    {
      element_animation = GEN_NEW UI_ELEMENT_ANIMATION();
      if(!element_animation) return NULL;
    }

  element_animation->SetFather(father);
  
  if(!GetLayoutElement_Base(node, layout, element_animation))
    {
      GEN_DELETE element_animation;
      return NULL;
    } 

  SetLevelAuto(element_animation, father);

  XSTRING statestring;  
  if(GetLayoutElementValue(node, __L("state"), statestring))
    {
      if(!statestring.Compare(__L("play"), true))                                                                     
        {
          element_animation->Play();
        }
        else
        {
          if(!statestring.Compare(__L("stop"), true))                                                                     
            {
              element_animation->Stop();
            }   
            else
            {       
              if(!statestring.Compare(__L("pause"), true))                                                                     
                {
                  element_animation->Pause();    
                }
            }
        }
    }

  double timedelay = 0;
  if(GetLayoutElementValue(node, __L("timedelay"), timedelay))
    {
      element_animation->SetMilliSecondsDelay((XDWORD)timedelay);
    }

  double alpha = 0;
  if(GetLayoutElementValue(node, __L("alpha"), alpha))
    {
      element_animation->SetAlpha((XBYTE)alpha);
    }

  for(int c=0; c<node->GetNElements(); c++)
    {
      XFILEXMLELEMENT* nodeelement =  node->GetElement(c);
      if(nodeelement)
        {  
          if(!nodeelement->GetName().Compare(__L("image")))
            {              
              namefileimg = nodeelement->GetValue(); 

              GRPPROPERTYMODE   grppropertymode = GRPPROPERTYMODE_XX_UNKNOWN;
              UI_SKIN_DRAWMODE  drawmode        = UI_SKIN_DRAWMODE_UNKNOWN;
              GRP2DCANVAS*      referencecanvas = NULL;

              if(layout->GetSkin())
                {
                  drawmode = layout->GetSkin()->GetDrawMode();

                  switch(drawmode)
                    {
                      case UI_SKIN_DRAWMODE_UNKNOWN   : break;

                      case UI_SKIN_DRAWMODE_CANVAS    : { UI_SKINCANVAS* skincanvas = (UI_SKINCANVAS*)layout->GetSkin(); 
                                                          if(skincanvas)
                                                            {
                                                              referencecanvas = skincanvas->GetCanvas();
                                                              grppropertymode = referencecanvas ? referencecanvas->GetMode() : GRPPROPERTYMODE_XX_UNKNOWN;
                                                            }
                                                        }
                                                        break;
   
                      case UI_SKIN_DRAWMODE_CONTEXT   : break;
                    }  
                }      

              // NOTE: a vector resource (SVG, DXF...) has no pixel size of its own, so it is always rasterized at
              // the <image> tag's own width/height (same size-aware cache as GetLayoutElement_Image). Bitmap
              // frames (the usual case for sprite animations) ignore these two values entirely.
              double framewidth  = 0.0;
              double frameheight = 0.0;

              GetLayoutElementValue(nodeelement, __L("width") , framewidth);
              GetLayoutElementValue(nodeelement, __L("height"), frameheight);

              UI_ANIMATION* animation = GetOrAddAnimationCache(drawmode, grppropertymode, __L(""), namefileimg.Get(), referencecanvas, framewidth, frameheight);
              if(animation)
                {
                  for(XDWORD d=0; d<animation->GetBitmaps()->GetSize(); d++)
                    {
                      UI_ELEMENT_IMAGE* element_img = GEN_NEW UI_ELEMENT_IMAGE();
                      if(element_img) 
                        {
                          GRPBITMAP* bitmap = NULL; 

                          element_img->SetFather(element_animation);       

                          GetLayoutElement_Base(nodeelement, layout, element_img); 

                          SetLevelAuto(element_img, element_animation);

                          bitmap = animation->GetBitmaps()->Get(d);
                          if(bitmap) 
                            { 
                              element_img->SetImage(bitmap);
                              
                              if(layout->GetSkin())
                                {
                                  switch(layout->GetSkin()->GetDrawMode())
                                    {
                                      case UI_SKIN_DRAWMODE_UNKNOWN   : break;

                                      case UI_SKIN_DRAWMODE_CANVAS    : { UI_SKINCANVAS* skincanvas = (UI_SKINCANVAS*)layout->GetSkin(); 
                                                                          if(skincanvas) skincanvas->CalculateBoundaryLine(element_img); 
                                                                        }
                                                                        break;
   
                                      case UI_SKIN_DRAWMODE_CONTEXT   : break;

                                    }
                                 }

                              element_img->SetAlpha(element_animation->GetAlpha());
                                                                  
                              element_animation->GetComposeElements()->Add(element_img);                                                                                                
                            } 
                        }
                    }      
                }
            }
        }
    }

  GetLayoutElement_CalculateBoundaryLine(layout, element_animation);

  return element_animation;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::GetLayoutElement_Option(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
* @brief      Get layout element option
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* @param[in]  element_legacy : Element legacy pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::GetLayoutElement_Option(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
{
  XSTRING             text;
  XSTRING             namefileimg;
  XPATH               xpathimg;
  double              sizefont  = 0;
  UI_ELEMENT_OPTION*  element_option = NULL;

  if(element_legacy)
    {
      element_option =  (UI_ELEMENT_OPTION*)element_legacy;
    }
   else
    {
      element_option = GEN_NEW UI_ELEMENT_OPTION();
      if(!element_option) return NULL;
    }

  element_option->SetFather(father);

  if(!GetLayoutElement_Base(node, layout, element_option))
    {
      GEN_DELETE element_option;
      return NULL;
    } 

  SetLevelAuto(element_option, father);

  element_option->SetActive(true);

  GetLayoutElementValue(node, __L("sizefont"), sizefont);

 
  XSTRING selectablestr;
  GetLayoutElementValue(node, __L("selectablestatus"), selectablestr);
  element_option->SetSelectableStateFromString(selectablestr);
    
  XSTRING allocationtextstr;
  if(GetLayoutElementValue(node, __L("allocationtext"), allocationtextstr))
    {
      if(!allocationtextstr.Compare(__L("none")     , true)) element_option->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_NONE);  
      if(!allocationtextstr.Compare(__L("up")       , true)) element_option->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_UP);
      if(!allocationtextstr.Compare(__L("down")     , true)) element_option->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_DOWN);  
      if(!allocationtextstr.Compare(__L("right")    , true)) element_option->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_RIGHT);  
	    if(!allocationtextstr.Compare(__L("left")     , true)) element_option->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_LEFT); 
      if(!allocationtextstr.Compare(__L("center")   , true)) element_option->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_CENTER); 
    }
  
  element_option->SetVisibleLimitType(UI_ELEMENT_OPTION_VISIBLE_LIMIT_NONE);

  XSTRING visiblelimittypestr;
  if(GetLayoutElementValue(node, __L("visiblelimit"), visiblelimittypestr))
    {
      if(visiblelimittypestr.Find(__L("active"), true) != XSTRING_NOTFOUND) 
        {
          element_option->SetVisibleLimitType(element_option->GetVisibleLimitType() | UI_ELEMENT_OPTION_VISIBLE_LIMIT_ACTIVE);
        }

      if(visiblelimittypestr.Find(__L("deactive"), true) != XSTRING_NOTFOUND) 
        {
          element_option->SetVisibleLimitType(element_option->GetVisibleLimitType() | UI_ELEMENT_OPTION_VISIBLE_LIMIT_DEACTIVE);
        }

      if(visiblelimittypestr.Find(__L("preselect"), true) != XSTRING_NOTFOUND) 
        {
          element_option->SetVisibleLimitType(element_option->GetVisibleLimitType() | UI_ELEMENT_OPTION_VISIBLE_LIMIT_PRESELECT);
        }

      if(visiblelimittypestr.Find(__L("select"), true) != XSTRING_NOTFOUND) 
        {
          element_option->SetVisibleLimitType(element_option->GetVisibleLimitType() | UI_ELEMENT_OPTION_VISIBLE_LIMIT_SELECT);
        }
    }
  

  for(int c=0; c<node->GetNElements(); c++)
    {
      XFILEXMLELEMENT* nodeelement =  node->GetElement(c);
      if(nodeelement)
        {
          XSTRING type;
          if(GetLayoutElementValue(nodeelement, __L("type"), type))
            {        
              UI_ELEMENT* element = CreatePartialLayout(nodeelement, layout, element_option);
              if(element) 
                {                   
                  switch(element->GetType())
                    {
                      case UI_ELEMENT_TYPE_ANIMATION  : element_option->Set_UIAnimation((UI_ELEMENT_ANIMATION*)element);                    
                                                        element_option->GetComposeElements()->Add(element);
                                                        break;

                      case UI_ELEMENT_TYPE_TEXT       : element_option->Set_UIText((UI_ELEMENT_TEXT*)element);                    
                                                        element_option->GetComposeElements()->Add(element);
                                                        break; 

                                        default       : break;                    
                    }                     
                }              
            }
        }  
    }
  
  GetLayoutElement_CalculateBoundaryLine(layout, element_option);

  return element_option;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::GetLayoutElement_MultiOption(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
* @brief      Get layout element multi option
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* @param[in]  element_legacy : Element legacy pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::GetLayoutElement_MultiOption(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
{
  UI_ELEMENT_MULTIOPTION* element_multioption = NULL;
  double                  sizefont            = 0;

  if(element_legacy)
    {
      element_multioption =  (UI_ELEMENT_MULTIOPTION*)element_legacy;
    }
   else
    {
      element_multioption = GEN_NEW UI_ELEMENT_MULTIOPTION();
      if(!element_multioption) return NULL;     
    }

  element_multioption->SetFather(father);
 
  if(!GetLayoutElement_Base(node, layout, element_multioption))
    {
      GEN_DELETE element_multioption;
      return NULL;
    }

  SetLevelAuto(element_multioption, father);
    
  for(int c=0; c<node->GetNElements(); c++)
    {
      XFILEXMLELEMENT* nodeelement =  node->GetElement(c);
      if(nodeelement)
        {
          UI_ELEMENT* element = CreatePartialLayout(nodeelement, layout, element_multioption);
          if(element) 
            {  
              element->SetFather(element_multioption);                
              element_multioption->GetComposeElements()->Add(element);                        
            }       
        }  
    }
  
  GetLayoutElement_CalculateBoundaryLine(layout, element_multioption);

  for(XDWORD c=0; c<element_multioption->GetComposeElements()->GetSize(); c++)
    {
      UI_ELEMENT* element = element_multioption->GetComposeElements()->Get(c);
      if(element) GetLayoutElement_CalculateBoundaryLine(layout, element);
    }
 
  return element_multioption;  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::GetLayoutElement_Button(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
* @brief      Get layout element button
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* @param[in]  element_legacy : Element legacy pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::GetLayoutElement_Button(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
{  
  UI_ELEMENT_BUTTON* element_button = GEN_NEW UI_ELEMENT_BUTTON();
  if(!element_button) return NULL;

  return GetLayoutElement_Option(node, layout, father, element_button);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::GetLayoutElement_CheckBox(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
* @brief      Get layout element check box
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* @param[in]  element_legacy : Element legacy pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::GetLayoutElement_CheckBox(XFILEXMLELEMENT* node, UI_LAYOUT* layout,  UI_ELEMENT* father, UI_ELEMENT* element_legacy)
{  
  UI_ELEMENT_CHECKBOX* element_checkbox = GEN_NEW UI_ELEMENT_CHECKBOX();
  if(!element_checkbox) return NULL;

  return GetLayoutElement_Option(node, layout,  father, element_checkbox);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::GetLayoutElement_EditText(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
* @brief      Get layout element edit text
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* @param[in]  element_legacy : Element legacy pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::GetLayoutElement_EditText(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
{  
  UI_ELEMENT_EDITTEXT* element_edittext = GEN_NEW UI_ELEMENT_EDITTEXT();
  if(!element_edittext) return NULL;

  return GetLayoutElement_Text(node, layout, father, element_edittext);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::GetLayoutElement_Form(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
* @brief      Get layout element form
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* @param[in]  element_legacy : Element legacy pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::GetLayoutElement_Form(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
{
  UI_ELEMENT_FORM* element_form  = NULL;
  double           sizefont      = 0;

  if(element_legacy)
    {
      element_form =  (UI_ELEMENT_FORM*)element_legacy;
    }
   else
    {
      element_form = GEN_NEW UI_ELEMENT_FORM();
      if(!element_form) return NULL;      
    }

  element_form->SetFather(father);
 
  if(!GetLayoutElement_Base(node, layout, element_form))
    {
      GEN_DELETE element_form;
      return NULL;
    }

  SetLevelAuto(element_form, father);

  XSTRING linecolor;
  GetLayoutElementValue(node, __L("linecolor"), linecolor);    
  if(!linecolor.IsEmpty()) element_form->GetLineColor()->SetFromString(linecolor);  
    
  GetLayoutElement_CalculateBoundaryLine(layout, element_form);

  for(int c=0; c<node->GetNElements(); c++)
    {
      XFILEXMLELEMENT* nodeelement =  node->GetElement(c);
      if(nodeelement)
        {
          UI_ELEMENT* element = CreatePartialLayout(nodeelement, layout, element_form);
          if(element) 
            {  
              element->SetFather(element_form);                
              element_form->GetComposeElements()->Add(element);                        
            }       
        }  
    }

  GetLayoutElement_CalculateBoundaryLine(layout, element_form);

  for(XDWORD c=0; c<element_form->GetComposeElements()->GetSize(); c++)
    {
      UI_ELEMENT* subelement = element_form->GetComposeElements()->Get(c);
      if(subelement) GetLayoutElement_CalculateBoundaryLine(layout, subelement);
    }

  XSTRING visibleformstr;
  if(GetLayoutElementValue(node, __L("visiblerect"), visibleformstr))
    {
      if(!visibleformstr.IsEmpty())
        {                     
          int value[4] = { 0, 0, 0, 0 }; 

          visibleformstr.UnFormat(__L("%d,%d,%d,%d")  , &value[0], &value[1], &value[2], &value[3]); 

          element_form->GetVisibleRect()->x = element_form->GetXPosition() + value[0];
          element_form->GetVisibleRect()->y = element_form->GetYPosition() + value[1];      

          if(value[2]) 
            {       
              element_form->GetVisibleRect()->width = value[2]; 
            }
           else
            {
              element_form->GetVisibleRect()->width = element_form->GetBoundaryLine()->width; 
            }

          if(value[3])
            {
              element_form->GetVisibleRect()->height  = value[3];
            }
           else
            {
              element_form->GetVisibleRect()->height  = element_form->GetBoundaryLine()->height;
            }

          element_form->GetBoundaryLine()->SetAround((*element_form->GetVisibleRect()));
        }
    }
   else 
    {
      element_form->GetVisibleRect()->x       = element_form->GetXPosition();
      element_form->GetVisibleRect()->y       = element_form->GetYPosition();
      element_form->GetVisibleRect()->width   = element_form->GetBoundaryLine()->width;
      element_form->GetVisibleRect()->height  = element_form->GetBoundaryLine()->height;      
    }
   
  return element_form;  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::GetLayoutElement_Menu(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
* @brief      Get layout element menu
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* @param[in]  element_legacy : Element legacy pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::GetLayoutElement_Menu(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
{   
  UI_ELEMENT_MENU* element_menu = GEN_NEW UI_ELEMENT_MENU();
  if(!element_menu) return NULL;

  return GetLayoutElement_Form(node, layout, father, element_menu);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::GetLayoutElement_ListBox(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
* @brief      Get layout element list box
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* @param[in]  element_legacy : Element legacy pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::GetLayoutElement_ListBox(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
{
  UI_ELEMENT_LISTBOX* element_listbox = GEN_NEW UI_ELEMENT_LISTBOX();
  if(!element_listbox) return NULL;

  UI_ELEMENT* element_menu = NULL;
  XSTRING     menustr;
  GetLayoutElementValue(node, __L("menu"), menustr);    
  if(!menustr.IsEmpty()) 
    {
      element_menu = Element_Get(menustr, UI_ELEMENT_TYPE_MENU);  
      element_listbox->Set_UIMenu((UI_ELEMENT_MENU*)element_menu);              
    }

  double defaultoption = 0.0f;
  GetLayoutElementValue(node, __L("defaultoption"), defaultoption);    
  if(element_menu)
    {
      UI_ELEMENT_OPTION* element_option = (UI_ELEMENT_OPTION*)element_menu->GetComposeElements()->Get((XDWORD)defaultoption);
      if(element_option) 
        {
          XSTRING text;

          if(element_option->Get_UIText()) text = element_option->Get_UIText()->GetText()->Get();
          //element_listbox->GetMaskText()->Set(text);
          node->GetValue().Set(text);
        }
    }

  return GetLayoutElement_Text(node, layout, father, element_listbox);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::GetLayoutElement_ProgressBar(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
* @brief      Get layout element progress bar
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* @param[in]  element_legacy : Element legacy pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::GetLayoutElement_ProgressBar(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
{
  XSTRING                   text;
  XSTRING                   namefileimg;
  XPATH                     xpathimg;
  double                    sizefont  = 0;
  UI_ELEMENT_PROGRESSBAR*   element_progressbar = NULL;

  if(element_legacy)
    {
      element_progressbar =  (UI_ELEMENT_PROGRESSBAR*)element_legacy;
    }
   else
    {
      element_progressbar = GEN_NEW UI_ELEMENT_PROGRESSBAR();
      if(!element_progressbar) return NULL;
    }

  element_progressbar->SetFather(father);

  if(!GetLayoutElement_Base(node, layout, element_progressbar))
    {
      GEN_DELETE element_progressbar;
      return NULL;
    } 

  SetLevelAuto(element_progressbar, father);

  element_progressbar->SetActive(true);

  GetLayoutElementValue(node, __L("sizefont"), sizefont);

    
  XSTRING allocationtextstr;
  if(GetLayoutElementValue(node, __L("allocationtext"), allocationtextstr))
    {
      if(!allocationtextstr.Compare(__L("none")     , true)) element_progressbar->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_NONE);  
      if(!allocationtextstr.Compare(__L("up")       , true)) element_progressbar->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_UP);
      if(!allocationtextstr.Compare(__L("down")     , true)) element_progressbar->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_DOWN);  
      if(!allocationtextstr.Compare(__L("right")    , true)) element_progressbar->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_RIGHT);  
	    if(!allocationtextstr.Compare(__L("left")     , true)) element_progressbar->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_LEFT); 
      if(!allocationtextstr.Compare(__L("center")   , true)) element_progressbar->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_CENTER); 
    }  


  XSTRING linecolor;
  GetLayoutElementValue(node, __L("linecolor"), linecolor);    
  if(!linecolor.IsEmpty()) element_progressbar->GetLineColor()->SetFromString(linecolor);

  XSTRING gradientcolor;
  GetLayoutElementValue(node, __L("gradientcolor"), gradientcolor);
  if(!gradientcolor.IsEmpty()) element_progressbar->GetGradientColor()->SetFromString(gradientcolor);

  XSTRING gradientmode;
  if(GetLayoutElementValue(node, __L("gradientmode"), gradientmode))
    {
      if(!gradientmode.Compare(__L("track"), true)) element_progressbar->SetGradientMode(UI_ELEMENT_PROGRESS_GRADIENTMODE_TRACK);
      if(!gradientmode.Compare(__L("fill") , true)) element_progressbar->SetGradientMode(UI_ELEMENT_PROGRESS_GRADIENTMODE_FILL);
    }  

  double levelvalue = 0.0f;
  if(GetLayoutElementValue(node, __L("level"), levelvalue)) element_progressbar->SetLevel((float)levelvalue);

  for(int c=0; c<node->GetNElements(); c++)
    {
      XFILEXMLELEMENT* nodeelement =  node->GetElement(c);
      if(nodeelement)
        {
          XSTRING type;
          if(GetLayoutElementValue(nodeelement, __L("type"), type))
            {        
              UI_ELEMENT* element = CreatePartialLayout(nodeelement, layout, element_progressbar);
              if(element) 
                {            
                  element->SetFather(element_progressbar);  
       
                  switch(element->GetType())
                    {
                      case UI_ELEMENT_TYPE_ANIMATION  : element_progressbar->Set_UIAnimation((UI_ELEMENT_ANIMATION*)element);                    
                                                        element_progressbar->GetComposeElements()->Add(element);
                                                        break;

                      case UI_ELEMENT_TYPE_TEXT       : element_progressbar->Set_UIText((UI_ELEMENT_TEXT*)element);                    
                                                        element_progressbar->GetComposeElements()->Add(element);
                                                        break;  

                                        default       : break;                   
                    }                     
                }              
            }
        }  
    }

  XSTRING roundcapstr;
  if(GetLayoutElementValue(node, __L("roundcap"), roundcapstr))
    {
      if(!roundcapstr.Compare(__L("yes"),  true) ||
         !roundcapstr.Compare(__L("true"), true) ||
         !roundcapstr.Compare(__L("1"),    true))   element_progressbar->SetRoundCap(true);
    }

  XSTRING continuouscyclestr;
  if(GetLayoutElementValue(node, __L("continuouscycle"), continuouscyclestr))
    {
      if(!continuouscyclestr.IsEmpty())
        {
          int value[3] = { 0, 0, 0 }; 

          continuouscyclestr.UnFormat(__L("%d,%d,%d")  , &value[0], &value[1], &value[2]); 

          if(!value[0]) value[0] = UI_ELEMENT_PROGRESSBAR_CCYCLE_DEFAULTSIZESEGMENT;
          if(!value[1]) value[1] = UI_ELEMENT_PROGRESSBAR_CCYCLE_DEFAULTSTEPSEGMENT;
          if(!value[1]) value[1] = UI_ELEMENT_PROGRESSBAR_CCYCLE_DEFAULTTIMESTEP;

          element_progressbar->ContinuousCycle_Set(true,  value[0], value[1], value[2]);             
        }
    }

  XSTRING progressrectstr;
  if(GetLayoutElementValue(node, __L("progressrect"), progressrectstr))
    {
      if(!progressrectstr.IsEmpty())
        {
          UI_ELEMENT* element_progressrect = NULL;

          element_progressrect = GEN_NEW UI_ELEMENT();
          if(element_progressrect)
            {
              int value[4] = { 0, 0, 0, 0 }; 

              progressrectstr.UnFormat(__L("%d,%d,%d,%d")  , &value[0], &value[1], &value[2], &value[3]); 

              element_progressrect->GetBoundaryLine()->x = element_progressbar->GetXPosition() + value[0];
              element_progressrect->GetBoundaryLine()->y = element_progressbar->GetYPosition() + value[1];      

              if(value[2]) 
                {       
                  element_progressrect->GetBoundaryLine()->width = value[2]; 
                }
               else
                {
                  element_progressrect->GetBoundaryLine()->width = element_progressbar->GetBoundaryLine()->width; 
                }

              if(value[3])
                {
                  element_progressrect->GetBoundaryLine()->height  = value[3];
                }
               else
                {
                  element_progressrect->GetBoundaryLine()->height  = element_progressbar->GetBoundaryLine()->height;
                }

              //element_progressbar->GetBoundaryLine()->SetAround((*element_progressrect->GetBoundaryLine()));
              element_progressbar->SetProgressRect(element_progressrect);
              element_progressrect->SetFather(element_progressbar);

              element_progressbar->GetComposeElements()->Add(element_progressrect);
            }
        }
    }
   else 
    {
      element_progressbar->SetProgressRect(NULL);
    }


  ChangeTextElementValue(layout, element_progressbar);                                // FIX1: resolve the percent text to its real value BEFORE measuring the boundary

  GetLayoutElement_CalculateBoundaryLine(layout, element_progressbar);                // calculate the boundary / rebuild area. This call was accidentally overwritten by FIX1; for bars WITH text the boundary
                                                                                      // was re-derived inside ChangeTextElementValue (FIX2), masking the loss, but a textless continuous bar was never measured -> not drawn.

  return element_progressbar;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::GetLayoutElement_ProgressRadial(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
* @brief      Get layout element radial progress
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* @param[in]  element_legacy : Element legacy pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::GetLayoutElement_ProgressRadial(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
{
  double                    value         = 0.0f;
  UI_ELEMENT_PROGRESS_RADIAL*  element_progress = NULL;

  if(element_legacy)
    {
      element_progress = (UI_ELEMENT_PROGRESS_RADIAL*)element_legacy;
    }
   else
    {
      element_progress = GEN_NEW UI_ELEMENT_PROGRESS_RADIAL();
      if(!element_progress) return NULL;
    }

  element_progress->SetFather(father);

  if(!GetLayoutElement_Base(node, layout, element_progress))
    {
      GEN_DELETE element_progress;
      return NULL;
    }

  SetLevelAuto(element_progress, father);

  element_progress->SetActive(true);

  // Value arc gradient END color (gradient START is the base "color"; track ring is "bckgrdcolor").
  XSTRING linecolor;
  GetLayoutElementValue(node, __L("linecolor"), linecolor);
  if(!linecolor.IsEmpty()) element_progress->GetLineColor()->SetFromString(linecolor);

  XSTRING gradientcolor;
  GetLayoutElementValue(node, __L("gradientcolor"), gradientcolor);
  if(!gradientcolor.IsEmpty()) element_progress->GetGradientColor()->SetFromString(gradientcolor);

  XSTRING gradientmode;
  if(GetLayoutElementValue(node, __L("gradientmode"), gradientmode))
    {
      if(!gradientmode.Compare(__L("track"), true)) element_progress->SetGradientMode(UI_ELEMENT_PROGRESS_GRADIENTMODE_TRACK);
      if(!gradientmode.Compare(__L("fill") , true)) element_progress->SetGradientMode(UI_ELEMENT_PROGRESS_GRADIENTMODE_FILL);
    }

  // Level [0..100].
  value = 0.0f;
  if(GetLayoutElementValue(node, __L("level"), value))      element_progress->SetLevel((float)value);

  // Geometry.
  value = 0.0f;
  if(GetLayoutElementValue(node, __L("startangle"), value)) element_progress->SetStartAngle(value);

  value = 0.0f;
  if(GetLayoutElementValue(node, __L("sweepangle"), value)) element_progress->SetSweepAngle(value);

  value = 0.0f;
  if(GetLayoutElementValue(node, __L("thickness"), value))  element_progress->SetThickness(value);

  XSTRING roundcapstr;
  if(GetLayoutElementValue(node, __L("roundcap"), roundcapstr))
    {
      if(!roundcapstr.Compare(__L("yes"),  true) ||
         !roundcapstr.Compare(__L("true"), true) ||
         !roundcapstr.Compare(__L("1"),    true))   element_progress->SetRoundCap(true);
    }

  // Child <text> => centered caption.
  for(int c=0; c<node->GetNElements(); c++)
    {
      XFILEXMLELEMENT* nodeelement = node->GetElement(c);
      if(nodeelement)
        {
          XSTRING type;
          if(GetLayoutElementValue(nodeelement, __L("type"), type))
            {
              UI_ELEMENT* element = CreatePartialLayout(nodeelement, layout, element_progress);
              if(element)
                {
                  element->SetFather(element_progress);

                  switch(element->GetType())
                    {
                      case UI_ELEMENT_TYPE_TEXT : element_progress->Set_UIText((UI_ELEMENT_TEXT*)element);
                                                  element_progress->GetComposeElements()->Add(element);
                                                  break;

                                      default   : break;
                    }
                }
            }
        }
    }

  GetLayoutElement_CalculateBoundaryLine(layout, element_progress);

  return element_progress;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::GetLayoutElement_ProgressImage(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
* @brief      Get layout element progress image
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* @param[in]  element_legacy : Element legacy pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::GetLayoutElement_ProgressImage(XFILEXMLELEMENT* node, UI_LAYOUT* layout, UI_ELEMENT* father, UI_ELEMENT* element_legacy)
{
  double                      value                 = 0.0f;
  UI_ELEMENT_PROGRESS_IMAGE*  element_progressimage  = NULL;

  if(element_legacy)
    {
      element_progressimage = (UI_ELEMENT_PROGRESS_IMAGE*)element_legacy;
    }
   else
    {
      element_progressimage = GEN_NEW UI_ELEMENT_PROGRESS_IMAGE();
      if(!element_progressimage) return NULL;
    }

  element_progressimage->SetFather(father);

  if(!GetLayoutElement_Base(node, layout, element_progressimage))
    {
      GEN_DELETE element_progressimage;
      return NULL;
    }

  SetLevelAuto(element_progressimage, father);

  element_progressimage->SetActive(true);

  XSTRING allocationtextstr;
  if(GetLayoutElementValue(node, __L("allocationtext"), allocationtextstr))
    {
      if(!allocationtextstr.Compare(__L("none")     , true)) element_progressimage->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_NONE);
      if(!allocationtextstr.Compare(__L("up")       , true)) element_progressimage->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_UP);
      if(!allocationtextstr.Compare(__L("down")     , true)) element_progressimage->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_DOWN);
      if(!allocationtextstr.Compare(__L("right")    , true)) element_progressimage->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_RIGHT);
      if(!allocationtextstr.Compare(__L("left")     , true)) element_progressimage->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_LEFT);
      if(!allocationtextstr.Compare(__L("center")   , true)) element_progressimage->SetAllocationTextType(UI_ELEMENT_OPTION_ALLOCATION_TEXT_TYPE_CENTER);
    }

  // alpha [0..100]
  value = 0.0f;
  if(GetLayoutElementValue(node, __L("alpha"), value)) element_progressimage->SetAlpha((XBYTE)value);

  // level [0..100]
  value = 0.0f;
  if(GetLayoutElementValue(node, __L("level"), value)) element_progressimage->SetLevel((float)value);

  value = 0.0f;
  if(GetLayoutElementValue(node, __L("offsetstart"), value)) element_progressimage->SetOffsetStart(value);

  value = 0.0f;
  if(GetLayoutElementValue(node, __L("offsetend"),   value)) element_progressimage->SetOffsetEnd(value);

  // resolve the draw mode once (same as GetLayoutElement_Image)
  GRPPROPERTYMODE   grppropertymode = GRPPROPERTYMODE_XX_UNKNOWN;
  UI_SKIN_DRAWMODE  drawmode        = UI_SKIN_DRAWMODE_UNKNOWN;

  if(layout->GetSkin())
    {
      drawmode = layout->GetSkin()->GetDrawMode();

      switch(drawmode)
        {
          case UI_SKIN_DRAWMODE_UNKNOWN   : break;

          case UI_SKIN_DRAWMODE_CANVAS    : { UI_SKINCANVAS* skincanvas = (UI_SKINCANVAS*)layout->GetSkin();
                                              if(skincanvas) grppropertymode = skincanvas->GetCanvas()->GetMode();
                                            }
                                            break;

          case UI_SKIN_DRAWMODE_CONTEXT   : break;
        }
    }

  // empty (0%) graphic
  XSTRING fileempty;
  GetLayoutElementValue(node, __L("imageempty"), fileempty);
  if(!fileempty.IsEmpty())
    {
      UI_ANIMATION* animation = GetOrAddAnimationCache(drawmode, grppropertymode, __L(""), fileempty.Get());
      if(animation && animation->GetBitmap()) element_progressimage->SetImageEmpty(animation->GetBitmap());
    }

  // full (100%) graphic
  XSTRING filefull;
  GetLayoutElementValue(node, __L("imagefull"), filefull);
  if(!filefull.IsEmpty())
    {
      UI_ANIMATION* animation = GetOrAddAnimationCache(drawmode, grppropertymode, __L(""), filefull.Get());
      if(animation && animation->GetBitmap()) element_progressimage->SetImageFull(animation->GetBitmap());
    }

  // child <text> => centered caption
  for(int c=0; c<node->GetNElements(); c++)
    {
      XFILEXMLELEMENT* nodeelement = node->GetElement(c);
      if(nodeelement)
        {
          XSTRING type;
          if(GetLayoutElementValue(nodeelement, __L("type"), type))
            {
              UI_ELEMENT* element = CreatePartialLayout(nodeelement, layout, element_progressimage);
              if(element)
                {
                  element->SetFather(element_progressimage);

                  switch(element->GetType())
                    {
                      case UI_ELEMENT_TYPE_TEXT : element_progressimage->Set_UIText((UI_ELEMENT_TEXT*)element);
                                                  element_progressimage->GetComposeElements()->Add(element);
                                                  break;

                                      default   : break;
                    }
                }
            }
        }
    }

  ChangeTextElementValue(layout, element_progressimage);                            
  GetLayoutElement_CalculateBoundaryLine(layout, element_progressimage);

  return element_progressimage;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::CreatePartialLayout(XFILEXMLELEMENT* nodeelement, UI_LAYOUT* layout, UI_ELEMENT* father)
* @brief      Create partial layout
* @ingroup    USERINTERFACE
* 
* @param[in]  nodeelement : Nodeelement pointer to use.
* @param[in]  layout : Layout pointer to use.
* @param[in]  father : Father pointer to use.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::CreatePartialLayout(XFILEXMLELEMENT* nodeelement, UI_LAYOUT* layout, UI_ELEMENT* father)
{
  XSTRING       value;
  UI_ELEMENT*   element  = NULL;

  if(GetLayoutElementValue(nodeelement, __L("type"), value))
    {                              
      if(!value.Compare(__L("text")           , true))  
        {
          element = GetLayoutElement_Text(nodeelement, layout, father);          
        }

      if(!value.Compare(__L("textbox")        , true))  
        {
          element = GetLayoutElement_TextBox(nodeelement, layout, father);          
        }

      if(!value.Compare(__L("image")          , true))  
        {
          element = GetLayoutElement_Image(nodeelement, layout, father);
        }

      if(!value.Compare(__L("animation")      , true))  
        {
          element = GetLayoutElement_Animation(nodeelement, layout, father);          
        }

      if(!value.Compare(__L("option")         , true))  
        {
          element = GetLayoutElement_Option(nodeelement, layout, father);          
        }

      if(!value.Compare(__L("multioption")    , true))  
        {
          element = GetLayoutElement_MultiOption(nodeelement, layout, father);          
        }

      if(!value.Compare(__L("button")         , true))  
        { 
          element = GetLayoutElement_Button(nodeelement, layout, father); 
        }

      if(!value.Compare(__L("checkbox")       , true))  
        {
          element = GetLayoutElement_CheckBox(nodeelement, layout, father);          
        }

      if(!value.Compare(__L("edittext")       , true))  
        {
          element = GetLayoutElement_EditText(nodeelement, layout, father);          
        }

      if(!value.Compare(__L("form")           , true))  
        {
          element = GetLayoutElement_Form(nodeelement, layout, father);
        }

      if(!value.Compare(__L("menu")           , true))  
        {
          element = GetLayoutElement_Menu(nodeelement, layout, father);      
        }

      if(!value.Compare(__L("listbox")        , true))  
        {
          element = GetLayoutElement_ListBox(nodeelement, layout, father);      
        }

      if(!value.Compare(__L("progressbar")    , true))  
        {
          element = GetLayoutElement_ProgressBar(nodeelement, layout, father);      
        }
        
      if(!value.Compare(__L("progressradial") , true))
        {
          element = GetLayoutElement_ProgressRadial(nodeelement, layout, father);
        }
  
      if(!value.Compare(__L("progressimage")   , true))
        {
          element = GetLayoutElement_ProgressImage(nodeelement, layout, father);
        }

    }

  return element;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::CreateLayouts(XFILEXML& xml, GRPSCREEN* screen, int viewportindex)
* @brief      Create layouts
* @ingroup    USERINTERFACE
* 
* @param[in]  xml : XML value.
* @param[in]  screen : Screen object to use.
* @param[in]  viewportindex : Viewportindex value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::CreateLayouts(XFILEXML& xml, GRPSCREEN* screen, int viewportindex)
{ 
  if(!xml.ReadAndDecodeAllLines()) 
    {
      return false;
    }

  XFILEXMLELEMENT*  root          = xml.GetRoot();  
  XSTRING           nametypeskin;  
  UI_SKIN_DRAWMODE  drawmode      = UI_SKIN_DRAWMODE_UNKNOWN;  
  XSTRING           raster_fontname;
  XSTRING           vector_fontname; 
  XSTRING           background_color[2]; 
  XSTRING           background_namefile[2]; 
  XSTRING           background_seamlesspattern[2]; 
  XSTRING           background_patternwidth[2]; 
  XSTRING           background_patternheight[2]; 
  
  if(!root) 
    {
      return false;
    }
    
  for(int c=0; c<root->GetNElements(); c++)
    {
      XFILEXMLELEMENT* nodeskin = root->GetElement(c);
      if(nodeskin)
        {
          if(!nodeskin->GetName().Compare(__L("skin"), true))
            {                 
              XCHAR*    value;                             
              XSTRING   drawmodestr;
                 
              value = nodeskin->GetValueAttribute(__L("name"));
              if(value) nametypeskin = value;

              drawmodestr = nodeskin->GetValueAttribute(__L("drawmode"));  
              if(!drawmodestr.Compare(__L("canvas"), true))   drawmode = UI_SKIN_DRAWMODE_CANVAS;
              if(!drawmodestr.Compare(__L("context"), true))  drawmode = UI_SKIN_DRAWMODE_CONTEXT;                  

              value = nodeskin->GetValueAttribute(__L("rasterfont"));                          
              if(value) raster_fontname = value;
                                          
              value = nodeskin->GetValueAttribute(__L("vectorfont"));                          
              if(value) vector_fontname = value;    

              value = nodeskin->GetValueAttribute(__L("backgroundcolor"));                          
              if(value) background_color[1] = value;                                       

              value = nodeskin->GetValueAttribute(__L("backgroundimg"));                          
              if(value) background_namefile[1] = value;                                       

              value = nodeskin->GetValueAttribute(__L("backgroundseamlesspattern"));                          
              if(value) background_seamlesspattern[1] = value;                                       

              value = nodeskin->GetValueAttribute(__L("backgroundpatternwidth"));                          
              if(value) background_patternwidth[1] = value;                                       

              value = nodeskin->GetValueAttribute(__L("backgroundpatternheight"));                          
              if(value) background_patternheight[1] = value;                                       
            }                  
        }
    }

  UI_SKIN* ui_skin = Skin_Create(nametypeskin, drawmode, screen, viewportindex);
  if(!ui_skin) 
    {
      return false;
    }
       
  ui_skin->GetRasterFont()->Set(raster_fontname);
  ui_skin->GetVectorFont()->Set(vector_fontname);

  if(!ui_skin->LoadFonts()) 
    {
      return false;
    }

  GRPPROPERTYMODE grppropertymode      = GRPPROPERTYMODE_XX_UNKNOWN;
  GRP2DCANVAS*    backgroundcanvas     = NULL;
  double          backgroundwidth      = 0.0;
  double          backgroundheight     = 0.0;

  for(int c=0; c<root->GetNElements(); c++)
    {
      XFILEXMLELEMENT*  nodecacheelement = root->GetElement(c);
      if(nodecacheelement)
        {          
          if(ui_skin)
            {
              switch(drawmode)
                {
                  case UI_SKIN_DRAWMODE_UNKNOWN   : break;

                  case UI_SKIN_DRAWMODE_CANVAS    : { UI_SKINCANVAS* skincanvas = (UI_SKINCANVAS*)ui_skin;
                                                      if(skincanvas)
                                                        {
                                                          grppropertymode = skincanvas->GetCanvas()->GetMode();
                                                        }
                                                    }
                                                    break;
   
                  case UI_SKIN_DRAWMODE_CONTEXT   : break;
                }  
            }

          CreateCacheElements(nodecacheelement, drawmode, grppropertymode);
        }
    }

  // Vector (SVG, DXF...) backgrounds need a canvas to rasterize into and a target size to rasterize at, since,
  // unlike a raster bitmap file, they have no pixel size of their own. Backgrounds are always rasterized to fit
  // the destination screen, so the reference canvas/size are resolved once here, before the layouts loop below.
  if(ui_skin && screen)
    {
      switch(drawmode)
        {
          case UI_SKIN_DRAWMODE_UNKNOWN   : break;

          case UI_SKIN_DRAWMODE_CANVAS    : { UI_SKINCANVAS* skincanvas = (UI_SKINCANVAS*)ui_skin;
                                              if(skincanvas)
                                                {
                                                  backgroundcanvas = skincanvas->GetCanvas();
                                                  backgroundwidth  = (double)screen->GetWidth();
                                                  backgroundheight = (double)screen->GetHeight();
                                                }
                                            }
                                            break;

          case UI_SKIN_DRAWMODE_CONTEXT   : break;
        }
    }

  for(int c=0; c<root->GetNElements(); c++)
    {
      XFILEXMLELEMENT* nodelayout = root->GetElement(c);
      if(nodelayout)
        {
          if(!nodelayout->GetName().Compare(__L("layout"), true))
            {
              XSTRING   namelayout;                
              XCHAR*    value;                             

              value = nodelayout->GetValueAttribute(__L("name"));
              if(value) namelayout = value;         
              if(!namelayout.IsEmpty())
                {                      
                  value = nodelayout->GetValueAttribute(__L("backgroundcolor"));                          
                  if(value) background_color[0] = value;                                       

                  value = nodelayout->GetValueAttribute(__L("backgroundimg"));                          
                  if(value) background_namefile[0] = value;                  

                  value = nodelayout->GetValueAttribute(__L("backgroundseamlesspattern"));                          
                  if(value) background_seamlesspattern[0] = value;                  

                  value = nodelayout->GetValueAttribute(__L("backgroundpatternwidth"));                          
                  if(value) background_patternwidth[0] = value;                  

                  value = nodelayout->GetValueAttribute(__L("backgroundpatternheight"));                          
                  if(value) background_patternheight[0] = value;                  

                  UI_LAYOUT* layout = GEN_NEW UI_LAYOUT(ui_skin);
                  if(layout)
                    {
                      XSTRING  bckcolor;
                      XSTRING  bcknamefile;

                      layout->GetNameID()->Set(namelayout);                         
                      layout->GetBackground()->GetColor()->SetFromString(background_color[1]); 
                      layout->GetBackground()->GetBitmapFileName()->Set(background_namefile[1]);
                      
                      if(!background_color[0].IsEmpty())
                        {
                          bckcolor = background_color[0];   
                        }
                       else
                        {
                          if(!background_color[1].IsEmpty())
                            {
                              bckcolor = background_color[1];   
                            }
                        }
                        
                      if(!background_namefile[0].IsEmpty())
                        {
                          bcknamefile = background_namefile[0];   
                        }
                       else
                        {
                          if(!background_namefile[1].IsEmpty())
                            {
                              bcknamefile = background_namefile[1];   
                            }
                        }  

                      XSTRING  bckseamlesspatternstr;
                      XSTRING  bckpatternwidthstr;
                      XSTRING  bckpatternheightstr;

                      if(!background_seamlesspattern[0].IsEmpty())
                        {
                          bckseamlesspatternstr = background_seamlesspattern[0];
                        }
                       else
                        {
                          if(!background_seamlesspattern[1].IsEmpty())
                            {
                              bckseamlesspatternstr = background_seamlesspattern[1];
                            }
                        }

                      if(!background_patternwidth[0].IsEmpty())
                        {
                          bckpatternwidthstr = background_patternwidth[0];
                        }
                       else
                        {
                          if(!background_patternwidth[1].IsEmpty())
                            {
                              bckpatternwidthstr = background_patternwidth[1];
                            }
                        }

                      if(!background_patternheight[0].IsEmpty())
                        {
                          bckpatternheightstr = background_patternheight[0];
                        }
                       else
                        {
                          if(!background_patternheight[1].IsEmpty())
                            {
                              bckpatternheightstr = background_patternheight[1];
                            }
                        }

                      bool isseamlesspattern = false;

                      if(!bckseamlesspatternstr.Compare(__L("yes"),  true) ||
                         !bckseamlesspatternstr.Compare(__L("true"), true) ||
                         !bckseamlesspatternstr.Compare(__L("1"),    true))   isseamlesspattern = true;

                      // "backgroundimg" is always the same single resource, whether it ends up being drawn once,
                      // stretched to the layout (GetBitmap(), the default) or repeated as a tile (GetPatternBitmap(),
                      // when "backgroundseamlesspattern" resolves to true) - see UI_MANAGER::Layout_PutBackground().
                      // A layout never has both set: only one of the two branches below runs.
                      if(isseamlesspattern)
                        {
                          // Raster pattern files (bmp, jpg, png...) are loaded at their own natural pixel size -
                          // width/height are only used by LoadBackgroundBitmap() for the vector (SVG, DXF...) case,
                          // via "backgroundpatternwidth"/"backgroundpatternheight", to know the tile size to
                          // rasterize at, since a vector file has no pixel size of its own.
                          GRPBITMAP* pattern = LoadBackgroundBitmap(bcknamefile, grppropertymode, backgroundcanvas, bckpatternwidthstr.ConvertToDouble(), bckpatternheightstr.ConvertToDouble());
                          if(pattern)
                            {
                              layout->GetBackground()->SetPatternBitmap(pattern);
                            }
                        }
                       else
                        {
                          GRPBITMAP* background = LoadBackgroundBitmap(bcknamefile, grppropertymode, backgroundcanvas, backgroundwidth, backgroundheight);
                          if(background)
                            {
                              layout->GetBackground()->SetBitmap(background);          
                            }
                        }

                      if(!layout->GetNameID()->Compare(UI_MANAGER_LAYOUT_COMMON, true))
                        {
                          layout_commonindex = layouts.GetSize();
                        }

                      Layouts_Add(layout); 
                    
                      for(int d=0 ; d<nodelayout->GetNElements(); d++)
                        {
                          XFILEXMLELEMENT* nodeelement =  nodelayout->GetElement(d);
                          if(nodeelement)
                            {
                              UI_ELEMENT* element = CreatePartialLayout(nodeelement, layout, NULL);
                              if(element) layout->Elements_Add(element);                                                                                                                              
                            }                              
                        } 
                    }  
                }
            }                  
        }
    }
    
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPBITMAP* UI_MANAGER::LoadBackgroundBitmap(XSTRING& namefilebitmap, GRPPROPERTYMODE mode, GRP2DCANVAS* referencecanvas, double width, double height)
* @brief      Load background bitmap
* @note       Accepts both raster (bmp, jpg, png...) and vector (SVG, DXF...) background files, detected by their
*             file extension via IsVectorResource(). Raster files follow the original GRPBITMAPFILE path, unchanged.
*             Vector files are rasterized once, at load time, into a bitmap that already fits (width, height) -
*             typically the destination screen size - through LoadBackgroundVectorFileToBitmap(), so the rest of the
*             background pipeline (UI_BACKGROUND::SetBitmap()/GetBitmap(), Layout_PutBackgroundImage()) keeps
*             working exactly as before, unaware of whether the original resource was raster or vector.
*             referencecanvas/width/height are only required for vector resources; raster loading ignores them,
*             so existing callers that do not pass them keep their original behavior.
* @ingroup    USERINTERFACE
* 
* @param[in]  namefilebitmap : Namefilebitmap value.
* @param[in]  mode : Mode value.
* @param[in]  referencecanvas : Canvas to rasterize a vector resource with (NULL = vector resources are skipped).
* @param[in]  width : Target width, in pixels, to rasterize a vector resource at (ignored for raster files).
* @param[in]  height : Target height, in pixels, to rasterize a vector resource at (ignored for raster files).
* 
* @return     GRPBITMAP* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPBITMAP* UI_MANAGER::LoadBackgroundBitmap(XSTRING& namefilebitmap, GRPPROPERTYMODE mode, GRP2DCANVAS* referencecanvas, double width, double height)
{  
  if(IsVectorResource(namefilebitmap.Get()))
    {
      return LoadBackgroundVectorFileToBitmap(namefilebitmap, referencecanvas, width, height);
    }

  GRPBITMAPFILE*  bitmapfile;
  GRPBITMAP*      bitmap  = NULL;  
  bool            status  = false; 

  bitmapfile = GEN_NEW GRPBITMAPFILE();
  if(!bitmapfile) 
    {
      return NULL;
    }               

   if(iszippedfile && unzipfile)
    {
      if(unzipfile)
        {                
          XPATH pathnamefilecmp;

          pathnamefilecmp = APPFLOW_DEFAULT_DIRECTORY_GRAPHICS;
          pathnamefilecmp.Slash_Add();
          pathnamefilecmp += namefilebitmap;
 
          status = unzipfile->DecompressFile(pathnamefilecmp, unzippathfile, namefilebitmap.Get());   
          if(status)
            {  
              XPATH unzippathfile_tmp;

              unzippathfile_tmp  = unzippathfile.Get();
              unzippathfile_tmp += namefilebitmap;          

              bitmap = bitmapfile->Load(unzippathfile_tmp, mode);
    
              DeleteTemporalUnZipFile(unzippathfile_tmp);  
            }
        }
    }
   else
    { 
      XPATH xpath;

      GEN_XPATHSMANAGER.GetPathOfSection(XPATHSMANAGERSECTIONTYPE_GRAPHICS, xpath);
      xpath.Slash_Add();
      xpath.Add(namefilebitmap.Get());                                             

      bitmap = bitmapfile->Load(xpath, mode);    
    }

  GEN_DELETE bitmapfile;

  return bitmap;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPBITMAP* UI_MANAGER::LoadBackgroundVectorFileToBitmap(XSTRING& namefilevector, GRP2DCANVAS* referencecanvas, double width, double height)
* @brief      Load background vector bitmap
* @note       Rasterizes a vector background file (SVG, DXF...) into a single opaque-sized bitmap that already
*             fits (width, height), following the same offscreen-canvas render approach as
*             UI_ANIMATION::LoadFromFileVector(). Unlike a raster bitmap, a vector file has no pixel size of its
*             own, and a real GRPBITMAP::Scale() resample is not currently implemented; asking the vector renderer
*             to draw straight at the final target size avoids that limitation entirely, which is the reason
*             vector backgrounds are always rasterized already scaled to (width, height) at load time, rather
*             than loaded at some "native" size and rescaled later on each Layout_PutBackgroundImage() call.
* @ingroup    USERINTERFACE
* 
* @param[in]  namefilevector : Namefilevector value.
* @param[in]  referencecanvas : Canvas to copy the pixel format and the loaded vector font from.
* @param[in]  width : Target width, in pixels, to rasterize at.
* @param[in]  height : Target height, in pixels, to rasterize at.
* 
* @return     GRPBITMAP* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPBITMAP* UI_MANAGER::LoadBackgroundVectorFileToBitmap(XSTRING& namefilevector, GRP2DCANVAS* referencecanvas, double width, double height)
{
  if(!referencecanvas)                  return NULL;
  if((width <= 0.0) || (height <= 0.0)) return NULL;

  XPATH pathvector;
  bool  usedunzip = false;

  if(iszippedfile && unzipfile)
    {
      XPATH pathnamefilecmp;

      pathnamefilecmp = APPFLOW_DEFAULT_DIRECTORY_GRAPHICS;
      pathnamefilecmp.Slash_Add();
      pathnamefilecmp += namefilevector;

      if(!unzipfile->DecompressFile(pathnamefilecmp, unzippathfile, namefilevector.Get()))
        {
          return NULL;
        }

      pathvector  = unzippathfile.Get();
      pathvector += namefilevector;

      usedunzip = true;
    }
   else
    {
      GEN_XPATHSMANAGER.GetPathOfSection(XPATHSMANAGERSECTIONTYPE_GRAPHICS, pathvector);
      pathvector.Slash_Add();
      pathvector.Add(namefilevector.Get());
    }

  bool           status     = false;
  GRPBITMAP*     bitmap     = NULL;
  GRPVECTORFILE* vectorfile = GRPVECTORFILE::CreateInstance(pathvector);

  if(vectorfile)
    {
      if(vectorfile->Load() == GRPVECTORFILERESULT_OK)
        {
          GRPPROPERTIES properties;

          properties.CopyPropertysFrom(referencecanvas);
          properties.SetPosition(0, 0);
          properties.SetSize((XDWORD)width, (XDWORD)height);

          GRP2DCANVAS* offscreen = GEN_GRPFACTORY.CreateCanvas(&properties);
          if(offscreen)
            {
              offscreen->SetWidth(width);
              offscreen->SetHeight(height);

              if(offscreen->Buffer_Create())
                {
                  offscreen->VectorFont_CopyFrom(referencecanvas);          // SVG text needs the same loaded vector font as the real canvas

                  GRP2DCOLOR_RGBA8 opaque(0, 0, 0, 255);
                  offscreen->Clear(&opaque);                                // a background has no transparent holes to show through

                  GRP2DVECTORFILERENDERAGG vectorrender;

                  status = vectorrender.Render(vectorfile, offscreen, 0.0, 0.0, width, height);
                  if(status)
                    {
                      bitmap = offscreen->GetBitmap(0.0, 0.0, width, height);
                    }
                }

              GEN_GRPFACTORY.DeleteCanvas(offscreen);
            }
        }

      GEN_DELETE vectorfile;
    }

  if(usedunzip)
    {
      DeleteTemporalUnZipFile(pathvector);
    }

  return bitmap;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::CreateCacheElements(XFILEXMLELEMENT* nodeelement, UI_SKIN_DRAWMODE drawmode, GRPPROPERTYMODE grppropertymode, bool recursive)
* @brief      Create cache elements
* @ingroup    USERINTERFACE
* 
* @param[in]  nodeelement : Nodeelement pointer to use.
* @param[in]  drawmode : Drawmode value.
* @param[in]  grppropertymode : Grppropertymode value.
* @param[in]  recursive : Recursive value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::CreateCacheElements(XFILEXMLELEMENT* nodeelement, UI_SKIN_DRAWMODE drawmode, GRPPROPERTYMODE grppropertymode, bool recursive)
{
  XSTRING name;          
  XCHAR*  value;  
  bool    status = false;

  if(!nodeelement->GetName().Compare(__L("color"), true))
    {                                                                  
      value = nodeelement->GetValueAttribute(__L("name"));
      if(value) 
        {
          name = value;
        }
              
      value = nodeelement->GetValue().Get();
      if(value) 
        {
          status = GEN_UI_COLORS.Add(name.Get(), value);                                                                       
        }
    }                  
    
  if(!nodeelement->GetName().Compare(__L("text"), true))
    {                                      
      value = nodeelement->GetValueAttribute(__L("name"));
      if(value) 
        {
          name = value;
        }
              
      value = nodeelement->GetValue().Get();
      if(value) 
        {
          status = GEN_UI_TEXTS.Add(name.Get(), value);                                                                       
        }
    }                      
  
  if(!nodeelement->GetName().Compare(__L("animation"), true))
    {                     
      value = nodeelement->GetValueAttribute(__L("name"));
      if(value)        
        { 
          name = value;
        }
              
      value = nodeelement->GetValue().Get();
      if(value) 
        {
          if(GetOrAddAnimationCache(drawmode, grppropertymode, name.Get(), value)) 
            {
              status = true;
            }
        }
    }

  if(recursive)
    {
      for(int c=0; c<nodeelement->GetNElements(); c++)
        {
          XFILEXMLELEMENT* subnodeelement = nodeelement->GetElement(c);
          if(subnodeelement) 
            {
              CreateCacheElements(subnodeelement, drawmode, grppropertymode, recursive);
            }
        }
    }
                  
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::RegisterEvents(bool active)
* @brief      Register events
* @ingroup    USERINTERFACE
* 
* @param[in]  active : Active value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::RegisterEvents(bool active)
{
  UI_XEVENT_TYPE eventtype[] = {  UI_XEVENT_TYPE_INPUT_CURSOR_MOVE            ,
                                  UI_XEVENT_TYPE_INPUT_CURSOR_MOTION          , 
                                  UI_XEVENT_TYPE_INPUT_SELECCTION             , 
                                  UI_XEVENT_TYPE_OUTPUT_SELECTED              ,
                                  UI_XEVENT_TYPE_OUTPUT_TEXTTOCHANGE          ,
                                  UI_XEVENT_TYPE_OUTPUT_CHANGECONTENTS      
                               };

  for(int c=0; c<(sizeof(eventtype) / sizeof(UI_XEVENT_TYPE)); c++)
    {
      if(active)        
             RegisterEvent(eventtype[c]);              
       else  DeRegisterEvent(eventtype[c]);              
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_ELEMENT* UI_MANAGER::PreSelectElement(UI_ELEMENT* element, int x, int y)
* @brief      Pre select element
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  x : X coordinate.
* @param[in]  y : Y coordinate.
* 
* @return     UI_ELEMENT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_ELEMENT* UI_MANAGER::PreSelectElement(UI_ELEMENT* element, int x, int y)
{
  UI_BOUNDARYLINE bline;
  bool            preselect         = false;
  bool            intofather        = false;
  UI_ELEMENT*     preselect_element = NULL;
  
  if(dynamic_cast<UI_PROPERTY_SELECTABLE*>(element))                                                                                                                                                                                                                                          
    {    
      if(element->IsActive() && element->IsVisible())
        {                                                                              
          bline.x       = element->GetXPositionWithScroll();          
          bline.y       = element->GetYPositionWithScroll();          
          bline.width   = element->GetBoundaryLine()->width;
          bline.height  = element->GetBoundaryLine()->height;
          
          preselect = bline.IsWithin(x, y);
          if(preselect)
            {
              if(element->IsSelected()) 
                {
                  preselect = false;                        
                }
            }

          // A partially-hidden option (clipped by a scrollable ancestor's viewport) must NOT be selectable. Require the
          // option box to be fully inside the viewport on every axis the ancestor clips (a VISIBLE axis is never clipped,
          // so it is not restricted there).
          if(preselect)
            {
              UI_ELEMENT* anc = element->GetFather();
              while(anc)
                {
                  UI_PROPERTY_SCROLLEABLE* asc = dynamic_cast<UI_PROPERTY_SCROLLEABLE*>(anc);
                  if(asc && asc->Scroll_NeedClip())
                    {
                      double vp_w = 0.0f;
                      double vp_h = 0.0f;
                      UI_ELEMENT_FORM* af = dynamic_cast<UI_ELEMENT_FORM*>(anc);
                      if(af && af->GetVisibleRect()->width > 0 && af->GetVisibleRect()->height > 0)
                        { vp_w = af->GetVisibleRect()->width;     vp_h = af->GetVisibleRect()->height; }
                       else
                        { vp_w = anc->GetBoundaryLine()->width;   vp_h = anc->GetBoundaryLine()->height; }

                      double vp_x      = anc->GetXPositionWithScroll();
                      double vp_y      = anc->GetYPositionWithScroll();
                      double vp_left   = vp_x;
                      double vp_right  = vp_x + vp_w;
                      double vp_top    = vp_y - vp_h;
                      double vp_bottom = vp_y;

                      double o_left   = bline.x;
                      double o_right  = bline.x + bline.width;
                      double o_top    = bline.y - bline.height;
                      double o_bottom = bline.y;
                      double eps      = 1.0f;

                      bool clip_h = (asc->Scroll_GetOverflow(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL) != UI_OVERFLOW_VISIBLE);
                      bool clip_v = (asc->Scroll_GetOverflow(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL)   != UI_OVERFLOW_VISIBLE);

                      double o_w = o_right  - o_left;
                      double o_h = o_bottom - o_top;

                      // Reject only when the option COULD fit the viewport on that axis but is cut by the scroll edge
                      // (a partially scrolled-out item). If the option is larger than the viewport on that axis, the
                      // "cut" is just normal content overflow (e.g. text wider than the menu) and must stay selectable.
                      if(clip_h && (o_w <= (vp_w + eps)) && ((o_left < vp_left - eps) || (o_right  > vp_right  + eps))) preselect = false;
                      if(clip_v && (o_h <= (vp_h + eps)) && ((o_top  < vp_top  - eps) || (o_bottom > vp_bottom + eps))) preselect = false;

                      break;   // only the nearest clipping scrollable ancestor defines the visible viewport
                    }
                  anc = anc->GetFather();
                }
            }

          element->SetPreSelect(preselect);                                                                           
          if(preselect) 
            {
              Elements_SetToRedraw(element); 
            
              last_xposition = x;
              last_yposition = y;

              preselect_element = element;
            }
        }
    } 

  if(!preselect)
    {
      bline.x       = element->GetXPosition();
      bline.y       = element->GetYPosition();
      bline.width   = element->GetBoundaryLine()->width;
      bline.height  = element->GetBoundaryLine()->height;
          
      intofather = bline.IsWithin(x, y);     
    }

  if(intofather)
    {                                                                                                                                     
      if(element->GetComposeElements()->GetSize())
        {                                                                      
          for(XDWORD d=0; d<element->GetComposeElements()->GetSize(); d++)   
            {
              UI_ELEMENT* subelement = element->GetComposeElements()->Get(d);
              if(subelement) 
                {                  
                  preselect_element = PreSelectElement(subelement, x, y);                
                  if(preselect_element) break;  
                }
            }
        }     
    }

  return preselect_element;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::SelectElement(UI_ELEMENT* element)
* @brief      Select element
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::SelectElement(UI_ELEMENT* element)
{
  if(dynamic_cast<UI_PROPERTY_SELECTABLE*>(element))                                                                                                                                                                                                                                          
    {    
      if(SelectedElement(element)) 
        {
          return true;      
        }
    } 
                                                                                                                                     
  if(element->GetComposeElements()->GetSize())
    {                                                                      
      for(XDWORD d=0; d<element->GetComposeElements()->GetSize(); d++)   
        {
          UI_ELEMENT* subelement = element->GetComposeElements()->Get(d);
          if(subelement) 
            { 
              if(dynamic_cast<UI_PROPERTY_SELECTABLE*>(subelement))                                                                                                                                                                                                                                          
                {                  
                  if(SelectedElement(subelement)) 
                    {
                      return true;                  
                    }
                }
            }
        }
    }                             

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::SelectedElement(UI_ELEMENT* element)
* @brief      Selected element
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::SelectedElement(UI_ELEMENT* element)
{
  if(!element) return false;

  if(!element->IsPreSelect()) return false;

  UI_PROPERTY_SELECTABLE* element_selectable = dynamic_cast<UI_PROPERTY_SELECTABLE*>(element);
  if(!element_selectable) return false;

  // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("UI Element [%s]: Selected. "), element->GetName()->Get());

  if(dynamic_cast<UI_PROPERTY_EDITABLE*>(element))        
    {
      if(virtualkeyboard) virtualkeyboard->Show(true, element);
    }

  if(element->GetType() == UI_ELEMENT_TYPE_CHECKBOX)
    {
      UI_ELEMENT_CHECKBOX* element_checkbox = (UI_ELEMENT_CHECKBOX*)element;
      if(element_checkbox) 
        {
          bool status = element_checkbox->GetStatus();

          // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Checkbox select] name %s status: %s"), element_checkbox->GetName()->Get(), (status?__L("true"):__L("false"))); 
          
          status=!status;

          element_checkbox->SetStatus(status);
          
          // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Checkbox select] name %s status: %s"), element_checkbox->GetName()->Get(), (status?__L("true"):__L("false"))); 

          Elements_SetToRedraw(element);  
      }
    }
   else
    {
      if(virtualkeyboard)
        {
          if(virtualkeyboard->SelectInput(element)) 
            {
              return true;
            }
        }

      if(element->GetType() == UI_ELEMENT_TYPE_LISTBOX)
        {
          UI_ELEMENT_LISTBOX* element_listbox = (UI_ELEMENT_LISTBOX*)element;
          if(element_listbox)
            { 
              UI_ELEMENT_MENU* element_menu = element_listbox->Get_UIMenu();     
              if(element_menu)
                {           
                  Element_SetModal(element_menu);  
       
                  element_menu->SetVisible(!element_menu->IsVisible());     
                }
            }
        }
      
      element->SetPreSelect(false);     
      element->SetSelected(true);

      element_selectable->GetXTimerSelected()->Reset();
                      
      SendEvent(UI_XEVENT_TYPE_OUTPUT_SELECTED, element); 

      //element->SetSelected(false);
      Elements_SetToRedraw(element);                   
    }

  return true;
}                       


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::UnSelectedElement(UI_ELEMENT* element)
* @brief      Un selected element
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::UnSelectedElement(UI_ELEMENT* element)
{
  if(element->IsSelected()) 
    {
      UI_PROPERTY_SELECTABLE* element_selectable = dynamic_cast<UI_PROPERTY_SELECTABLE*>(element);
      if(!element_selectable) 
        {
          return false;
        }
                        
      if(element_selectable->GetXTimerSelected())
        {
          if(element_selectable->GetXTimerSelected()->GetMeasureMilliSeconds() > element_selectable->GetTimeSelected()) 
            {             
              element->SetSelected(false);
              Elements_SetToRedraw(element);     

              GEN_USERINTERFACE.SendEvent(UI_XEVENT_TYPE_INPUT_CURSOR_MOVE, last_xposition, last_yposition);   
            }
        }                 
    }
                                                                                                                                     
  if(element->GetComposeElements()->GetSize())
    {                                                                      
      for(XDWORD d=0; d<element->GetComposeElements()->GetSize(); d++)   
        {
          UI_ELEMENT* subelement = element->GetComposeElements()->Get(d);
          if(subelement) 
            { 
              if(dynamic_cast<UI_PROPERTY_SELECTABLE*>(subelement))                                                                                                                                                                                                                                          
                {                  
                  if(UnSelectedElement(subelement)) 
                    {
                      return true;                  
                    }
                }
            }
        }
    }                             

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::UnSelectedElement()
* @brief      Un selected element
* @ingroup    USERINTERFACE
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::UnSelectedElement()
{
  for(XDWORD d=0; d<layouts.GetSize(); d++)
    {    
      UI_LAYOUT* layout = layouts.Get(d);                                                          
      if(layout)
        {
          for(XDWORD c=0; c<layout->Elements_Get()->GetSize(); c++)
            {
              UI_ELEMENT* element = layout->Elements_Get()->Get(c);
              if(element) 
                {
                  UnSelectedElement(element);                        
                }
            }        
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::UseMotionInElement(UI_ELEMENT* element, INPCURSORMOTION* cursormotion)
* @brief      Use motion in element
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  cursormotion : Cursormotion pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::UseMotionInElement(UI_ELEMENT* element, INPCURSORMOTION* cursormotion)
{
  if(!element)      
    {
      return false;
    }

  if(!cursormotion) 
    {
      return false;
    }

  if(!element->IsVisible()) return false;

  UI_PROPERTY_SCROLLEABLE* property_scrolleable = dynamic_cast<UI_PROPERTY_SCROLLEABLE*>(element);
  if(!property_scrolleable) 
    {
      return false;
    }

  bool isinrect = cursormotion->IsInRect((int)element->GetXPosition()           , (int)(element->GetYPosition() - element->GetBoundaryLine()->height), 
                                         (int)element->GetBoundaryLine()->width , (int)element->GetBoundaryLine()->height);
  if(isinrect)
    {
      XDWORD differential = 0;
      INPCURSORMOTION_DIR motiondir = cursormotion->GetDirectionByDifferential(&differential);
      if(motiondir != INPCURSORMOTION_DIR_UNKNOWN)
        {
          switch(motiondir)
            {
              case INPCURSORMOTION_DIR_UP         :
              case INPCURSORMOTION_DIR_DOWN       : { double shift = (differential/3);
                                                      if(motiondir == INPCURSORMOTION_DIR_DOWN) shift *= -1;
                                                      
                                                      property_scrolleable->Scroll_SetStep(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL, shift);   
                                                      GEN_USERINTERFACE.Elements_SetToRedraw(element);                                                                                                                 
                                                                                                                                                                                                                                                                                          
                                                      //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("[Input Motion]  motion %s points: %d"), (motiondir == INPCURSORMOTION_DIR_UP)?__L("up"):__L("down"), cursormotion->GetNPoints());

                                                      return true;  
                                                    }
                                                    break;              

              case INPCURSORMOTION_DIR_RIGHT      : 
              case INPCURSORMOTION_DIR_LEFT       : break;                  

                                      default     : break;
            }             
        } 
    }
                                                                                                                                             
  if(element->GetComposeElements()->GetSize())
    {                                                                      
      for(XDWORD d=0; d<element->GetComposeElements()->GetSize(); d++)   
        {
          UI_ELEMENT* subelement = element->GetComposeElements()->Get(d);
          if(subelement) 
            { 
              if(dynamic_cast<UI_PROPERTY_SELECTABLE*>(subelement))                                                                                                                                                                                                                                          
                {                  
                  if(UseMotionInElement(subelement, cursormotion)) 
                    {
                      return true;                  
                    }
                }
            }
        }
    }                             

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::UseMotion(INPCURSORMOTION* cursormotion)
* @brief      Use motion
* @ingroup    USERINTERFACE
* 
* @param[in]  cursormotion : Cursormotion pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::UseMotion(INPCURSORMOTION* cursormotion)
{
  if(!cursormotion) 
    {
      return false;
    }

  for(int d=0; d<layouts.GetSize(); d++)
    {    
      UI_LAYOUT* layout = layouts.Get(d);                                                          
      if(layout) 
        {
          for(XDWORD c=0; c<layout->Elements_Get()->GetSize(); c++)
            {
              UI_ELEMENT* element = layout->Elements_Get()->Get(c);
              if(element) 
                {
                  if(UseMotionInElement(element, cursormotion)) return true;
                }
            }        
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::SelectScrollBarInElement(UI_ELEMENT* element, int x, int y)
* @brief      Hit-tests the overlay scrollbar(s) of a scrollable container (and its children) against a pointer.
* @note       Interactive scrollbar: a SELECCTION/touch on the bar track sets the scroll position proportionally to
* @ingroup    USERINTERFACE
* 
* @param[in]  element : Element to process.
* @param[in]  x : X coordinate.
* @param[in]  y : Y coordinate.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::SelectScrollBarInElement(UI_ELEMENT* element, int x, int y)
{
  if(!element) return false;

  UI_PROPERTY_SCROLLEABLE* scrolleable = dynamic_cast<UI_PROPERTY_SCROLLEABLE*>(element);
  if(scrolleable && element->IsActive() && element->IsVisible())
    {
      double box_w      = 0.0f;
      double box_h      = 0.0f;
      UI_SKINCANVAS::GetScrollViewportSize(element, box_w, box_h);
      double box_left   = element->GetXPositionWithScroll();
      double box_bottom = element->GetYPositionWithScroll();
      double box_right  = box_left   + box_w;
      double box_top    = box_bottom - box_h;
      double radius     = (double)element->GetRoundRect();

      double tl;
      double tt;
      double tr;
      double tb;

      // vertical bar : right gutter
      if(scrolleable->Scroll_GetTrackRect(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL, box_left, box_top, box_right, box_bottom, radius, tl, tt, tr, tb))
        {
          UI_BOUNDARYLINE track;
          track.x      = tl;
          track.width  = tr - tl;
          track.y      = tb;
          track.height = tb - tt;

          if(track.IsWithin(x, y))
            {
              double tracklen = tb - tt;
              double fraction = (tracklen != 0.0f) ? (((double)y - tt) / tracklen) : 0.0f;

              scrolleable->Scroll_SetFraction(UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL, fraction);
              Elements_SetToRedraw(element);

              return true;
            }
        }

      // horizontal bar : bottom gutter
      if(scrolleable->Scroll_GetTrackRect(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL, box_left, box_top, box_right, box_bottom, radius, tl, tt, tr, tb))
        {
          UI_BOUNDARYLINE track;
          track.x      = tl;
          track.width  = tr - tl;
          track.y      = tb;
          track.height = tb - tt;

          if(track.IsWithin(x, y))
            {
              double tracklen = tr - tl;
              double fraction = (tracklen != 0.0f) ? (((double)x - tl) / tracklen) : 0.0f;

              scrolleable->Scroll_SetFraction(UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL, fraction);
              Elements_SetToRedraw(element);

              return true;
            }
        }
    }

  for(XDWORD c=0; c<element->GetComposeElements()->GetSize(); c++)
    {
      UI_ELEMENT* subelement = element->GetComposeElements()->Get(c);
      if(subelement && SelectScrollBarInElement(subelement, x, y)) return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_MANAGER::SelectScrollBar(int x, int y)
* @brief      Hit-tests all layouts' scrollbars against a pointer; applies proportional scroll if hit.
* @ingroup    USERINTERFACE
* 
* @param[in]  x : X coordinate.
* @param[in]  y : Y coordinate.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_MANAGER::SelectScrollBar(int x, int y)
{
  for(int d=0; d<layouts.GetSize(); d++)
    {
      UI_LAYOUT* layout = layouts.Get(d);
      if(layout)
        {
          for(XDWORD c=0; c<layout->Elements_Get()->GetSize(); c++)
            {
              UI_ELEMENT* element = layout->Elements_Get()->Get(c);
              if(element && SelectScrollBarInElement(element, x, y)) return true;
            }
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_MANAGER::HandleEvent_UI(UI_XEVENT* event)
* @brief      Handle event UI
* @note       INTERNAL
* @ingroup    USERINTERFACE
* 
* @param[in]  event : Event to process.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_MANAGER::HandleEvent_UI(UI_XEVENT* event)
{
  switch(event->GetEventType())
    {
      case UI_XEVENT_TYPE_INPUT_CURSOR_MOVE         : { UI_ELEMENT*  _preselect_element = NULL;
                                                        int          x                  = event->GetXPos();
                                                        int          y                  = event->GetYPos();

                                                        //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("x: %d, y: %d"), x, y);

                                                        // Remember where the pointer is right now (not only where it last
                                                        // landed ON an element). UnSelectedElement() re-issues a CURSOR_MOVE at
                                                        // last_xposition/last_yposition once a selection's highlight expires, to
                                                        // restore the hover (preselect) under the pointer. On a mouse that point
                                                        // is wherever the cursor sits; on touch the finger is gone after release
                                                        // and the device parks the cursor off-canvas (-1,-1), so keeping these in
                                                        // sync here means the lifted finger leaves NO stuck preselect, while a
                                                        // real mouse still correctly re-preselects whatever it is hovering.
                                                        last_xposition = x;
                                                        last_yposition = y;
                                                  
                                                        if(element_modal)
                                                          {                                                              
                                                            _preselect_element = PreSelectElement(element_modal, x, y);                                                              
                                                          }
                                                         else
                                                          {
                                                            for(int d=0; d<layouts.GetSize(); d++)
                                                              {    
                                                                UI_LAYOUT* layout = layouts.Get(d);                                                          
                                                                if(layout) 
                                                                  {
                                                                    for(XDWORD c=0; c<layout->Elements_Get()->GetSize(); c++)
                                                                      {
                                                                        UI_ELEMENT* element = layout->Elements_Get()->Get(c);
                                                                        if(element) 
                                                                          {
                                                                            _preselect_element = PreSelectElement(element, x, y);
                                                                            if(_preselect_element) break;                                                                       
                                                                          }
                                                                      }
                                                                  }

                                                                if(_preselect_element) break;       
                                                              }
                                                          }

                                                        if(!_preselect_element)  
                                                          {
                                                            if(preselect_element)
                                                              {
                                                                ResetPreselect();                                                                                                 
                                                              }
                                                          } 
                                                         else
                                                          {
                                                            if(_preselect_element != preselect_element)
                                                              {
                                                                if(preselect_element) 
                                                                  { 
                                                                    ResetPreselect();                                     
                                                                  }

                                                                preselect_element = _preselect_element;                                                                      
                                                              }                                                           
                                                          }                                                                                                                                                  
                                                      }
                                                      break;

      case UI_XEVENT_TYPE_INPUT_CURSOR_MOTION       : { 
                                                        if(UseMotion(event->GetCursorMotion()))
                                                          {
                                                            UnSelectedElement();
                                                          }
                                                      }
                                                      break;

      case UI_XEVENT_TYPE_INPUT_SELECCTION          : { int scrollbar_x = event->GetXPos();
                                                        int scrollbar_y = event->GetYPos();
                                                        if(SelectScrollBar(scrollbar_x, scrollbar_y)) break;   // consumed by an interactive scrollbar

                                                        if(element_modal)
                                                          { 
                                                            if(xmutex_modal) 
                                                              {
                                                                xmutex_modal->Lock(); 
                                                              }

                                                            SelectElement(element_modal);                                                                                   

                                                            if(xmutex_modal) 
                                                              {
                                                                xmutex_modal->UnLock(); 
                                                              }
                                                          }
                                                         else
                                                          {
                                                            for(int d=0; d<layouts.GetSize(); d++)
                                                              {    
                                                                UI_LAYOUT* layout = layouts.Get(d);                                                          
                                                                if(layout) 
                                                                  {
                                                                    for(XDWORD c=0; c<layout->Elements_Get()->GetSize(); c++)
                                                                      {
                                                                        UI_ELEMENT* element = layout->Elements_Get()->Get(c);
                                                                        if(element) 
                                                                          {
                                                                            if(SelectElement(element)) 
                                                                              {
                                                                                break;
                                                                              }                                                                                                                                                                                                                                                                                                                                      
                                                                          }
                                                                      }
                                                                  }
                                                              }
                                                          }
                                                      }
                                                      break;      
    }
} 


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_MANAGER::HandleEvent(XEVENT* xevent)
* @brief      Handle event
* @note       INTERNAL
* @ingroup    USERINTERFACE
* 
* @param[in]  xevent : Xevent pointer to use.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_MANAGER::HandleEvent(XEVENT* xevent)
{
  if(!xevent) return;

  switch(xevent->GetEventFamily())
    {
      case XEVENT_TYPE_USERINTERFACE    : { UI_XEVENT* event = (UI_XEVENT*)xevent;
                                            if(!event) return;

                                            HandleEvent_UI(event);
                                          }
                                          break;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_MANAGER::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    USERINTERFACE
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_MANAGER::Clean()
{  
  iszippedfile        = false;
  unzipfile           = NULL;  
  
  layout_commonindex  = UI_MANAGER_LAYOUT_NOTFOUND;

  xmutex_modal        = NULL;
  element_modal       = NULL;

  xmutex_UIevent      = NULL;

  last_xposition      = 0;
  last_yposition      = 0;

  preselect_element   = NULL;

  virtualkeyboard     = NULL;
}



