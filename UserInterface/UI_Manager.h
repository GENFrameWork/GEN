/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_Manager.h
* 
* @class      UI_MANAGER
* @brief      User Interface Manager class
* @ingroup    USERINTERFACE
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

#ifndef _UI_MANAGER_H_
#define _UI_MANAGER_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XVector.h"
#include "XPath.h"
#include "XObserver.h"
#include "XSubject.h"
#include "XFileXML.h"
#include "XString.h"
#include "XFileZIP.h"

#include "UI_XEvent.h"
#include "UI_Element.h"
#include "UI_Skin.h"
#include "UI_Layout.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

#define UI_MANAGER_LAYOUT_NOTSELECTED       -1
#define UI_MANAGER_LAYOUT_INALL_NAME        __L("IN_ALL")
#define UI_MANAGER_LAYOUT_TEXT_INI          __L("#[")
#define UI_MANAGER_LAYOUT_TEXT_END          __L("]")
#define UI_MANAGER_LAYOUT_TEXT_TRANSLATE    __L("TRANSLATE_")

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class GRPSCREEN;
class GRPCONTEXT;
class GRPCANVAS;
class INPCURSORMOTION;
class UI_ANIMATION;
class UI_ELEMENT_TEXT;
class UI_ELEMENT_FORM;
class UI_VIRTUALKEYBOARD;


class UI_MANAGER : public XOBSERVER, public XSUBJECT
{
  public:

    static bool                     GetIsInstanced                            ();
    static UI_MANAGER&              GetInstance                               ();
    static bool                     DelInstance                               ();

    bool                            Load                                      (XPATH& pathfile);
    bool                            LoadLayout                                (XPATH& pathfile);

    bool                            IsZippedFile                              (); 
    XPATH*                          GetUnzipPathFile                          (); 
    XFILEUNZIP*                     GetUnzipFile                              ();
    bool                            DeleteTemporalUnZipFile                   (XPATH& pathfile);
    bool                            CloseUnZipFile                            ();

    bool                            Layouts_Add                               (UI_LAYOUT* layout);
    XVECTOR<UI_LAYOUT*>*            Layouts_Get                               ();
    UI_LAYOUT*                      Layouts_Get                               (int index);
    UI_LAYOUT*                      Layouts_Get                               (XCHAR* name);
    UI_LAYOUT*                      Layouts_Get                               (XSTRING& tname);
    int                             Layouts_GetSelected                       ();
    bool                            Layouts_SetSelected                       (int index);
    bool                            Layouts_SetSelected                       (XCHAR* name);
    bool                            Layouts_SetSelected                       (XSTRING& name);
    bool                            Layouts_DeleteAll                         ();     
    UI_LAYOUT*                      Layouts_GetSelectedLayout                 ();
    UI_LAYOUT*                      Layouts_GetInAllLayout                    ();

    bool                            Skin_CreateAll                            (GRPSCREEN* screen = NULL, GRPCONTEXT* context = NULL);
    bool                            Skin_Add                                  (UI_SKIN* skin);   
    UI_SKIN*                        Skin_Get                                  (XCHAR* name, UI_SKIN_DRAWMODE drawmode);
    UI_SKIN*                        Skin_Get                                  (XSTRING& name, UI_SKIN_DRAWMODE drawmode);
    UI_SKIN*                        Skin_Selected                             ();
    bool                            Skin_DeleteAll                            ();   
    
    bool                            SetModalElement                           (UI_ELEMENT* element_modal);  

    bool                            Update                                    ();

    bool                            Background_Put                            ();    
    bool                            Background_PutColor                       ();    
    bool                            Background_PutBitmap                      ();    
  
    UI_ELEMENT*                     GetElement                                (XCHAR* name, UI_ELEMENT_TYPE type = UI_ELEMENT_TYPE_UNKNOWN);  
    UI_ELEMENT*                     GetElement                                (XSTRING& name, UI_ELEMENT_TYPE type = UI_ELEMENT_TYPE_UNKNOWN);   
    
    UI_ELEMENT*                     GetElementAllLayouts                      (XCHAR* name, UI_ELEMENT_TYPE type);
    UI_ELEMENT*                     GetElementAllLayouts                      (XSTRING& name, UI_ELEMENT_TYPE type);

    UI_LAYOUT*                      GetElementLayout                          (UI_ELEMENT* element);
    UI_LAYOUT*                      GetElementLayout                          (XCHAR* name, UI_ELEMENT_TYPE type = UI_ELEMENT_TYPE_UNKNOWN);    
    UI_LAYOUT*                      GetElementLayout                          (XSTRING& name, UI_ELEMENT_TYPE type = UI_ELEMENT_TYPE_UNKNOWN);    

    bool                            PutElementLastPositionLayout              (UI_ELEMENT* element);      
    bool                            PutElementLastPositionLayout              (XCHAR* name, UI_ELEMENT_TYPE type = UI_ELEMENT_TYPE_UNKNOWN);      
    bool                            PutElementLastPositionLayout              (XSTRING& name, UI_ELEMENT_TYPE type = UI_ELEMENT_TYPE_UNKNOWN);    

    bool                            AddElementToLayout                        (XCHAR* layoutname, UI_ELEMENT* element);                      
    bool                            AddElementToLayout                        (XSTRING& layoutname, UI_ELEMENT* element);  

    bool                            Elements_SetToRedraw                      (); 
    bool                            Elements_SetToRedraw                      (UI_ELEMENT* element, bool recursive = true);
    bool                            Elements_RebuildDrawAreas                 (); 
    bool                            Elements_RebuildDrawAreas                 (UI_ELEMENT* element);
    
    int                             GetOutputTextChangeID                     (XSTRING* text, int start, XSTRING& value);
    UI_ANIMATION*                   GetOrAddAnimationCache                    (XCHAR* name, XCHAR* resource);
    
    bool                            SetLevelAuto                              (UI_ELEMENT* element, UI_ELEMENT* father, XDWORD addlevel = 0);

    bool                            SetPreselectElement                       (UI_ELEMENT* element = NULL);
    bool                            ResetPreselect                            ();
      
    bool                            SendEvent                                 (UI_XEVENT_TYPE event, ...);
    bool                            ChangeAutomaticTextElementValue           (UI_ELEMENT* element, XSTRING* maskvalue, XSTRING* maskresolved);
    bool                            ChangeTextElementValue                    (UI_ELEMENT* element, XSTRING* text, XSTRING& value);
    bool                            ChangeTextElementValue                    (UI_ELEMENT* element, UI_SKIN* skin);
    bool                            ChangeTextElementValue                    (UI_LAYOUT* layout);

    
    bool                            SubscribeInputEvents                      (bool active);    
    bool                            SubscribeOutputEvents                     (bool active, XOBSERVER* observer, XSUBJECT* subject); 

    bool                            CreaterVirtualKeyboard                    (GRPSCREEN* screen, UI_LAYOUT* layout, UI_SKIN* skin);    
    bool                            DeleteVirtualKeyboard                     (); 
    
  private:                                 
                                    UI_MANAGER                                ();
                                    UI_MANAGER                                (UI_MANAGER const&);        // Don't implement
    virtual                        ~UI_MANAGER                                ();

    void                            operator =                                (UI_MANAGER const&);        // Don't implement

    bool                            GetLayoutElementValue                     (XFILEXMLELEMENT* node, XCHAR* leyend, double& value);
    bool                            GetLayoutElementValue                     (XFILEXMLELEMENT* node, XCHAR* leyend, XSTRING& value);
    bool                            GetLayoutElement_Base                     (XFILEXMLELEMENT* node, UI_ELEMENT* element, bool adjusttoparent = false);
    bool                            GetLayoutElement_CalculateBoundaryLine    (UI_ELEMENT* element, bool adjustsizemargin = false);

    bool                            GetParentSizeFont                         (XFILEXMLELEMENT* node, double& sizefont);

    UI_ELEMENT*                     GetLayoutElement_Text                     (XFILEXMLELEMENT* node, UI_ELEMENT* father, UI_ELEMENT* element_legacy = NULL);
    UI_ELEMENT*                     GetLayoutElement_TextBox                  (XFILEXMLELEMENT* node, UI_ELEMENT* father, UI_ELEMENT* element_legacy = NULL);
    UI_ELEMENT*                     GetLayoutElement_Image                    (XFILEXMLELEMENT* node, UI_ELEMENT* father, UI_ELEMENT* element_legacy = NULL);
    UI_ELEMENT*                     GetLayoutElement_Animation                (XFILEXMLELEMENT* node, UI_ELEMENT* father, UI_ELEMENT* element_legacy = NULL);
    UI_ELEMENT*                     GetLayoutElement_Option                   (XFILEXMLELEMENT* node, UI_ELEMENT* father, UI_ELEMENT* element_legacy = NULL);
    UI_ELEMENT*                     GetLayoutElement_MultiOption              (XFILEXMLELEMENT* node, UI_ELEMENT* father, UI_ELEMENT* element_legacy = NULL);
    UI_ELEMENT*                     GetLayoutElement_Button                   (XFILEXMLELEMENT* node, UI_ELEMENT* father, UI_ELEMENT* element_legacy = NULL);
    UI_ELEMENT*                     GetLayoutElement_CheckBox                 (XFILEXMLELEMENT* node, UI_ELEMENT* father, UI_ELEMENT* element_legacy = NULL);
    UI_ELEMENT*                     GetLayoutElement_EditText                 (XFILEXMLELEMENT* node, UI_ELEMENT* father, UI_ELEMENT* element_legacy = NULL);
    UI_ELEMENT*                     GetLayoutElement_Form                     (XFILEXMLELEMENT* node, UI_ELEMENT* father, UI_ELEMENT* element_legacy = NULL);
    UI_ELEMENT*                     GetLayoutElement_Menu                     (XFILEXMLELEMENT* node, UI_ELEMENT* father, UI_ELEMENT* element_legacy = NULL);
    UI_ELEMENT*                     GetLayoutElement_ListBox                  (XFILEXMLELEMENT* node, UI_ELEMENT* father, UI_ELEMENT* element_legacy = NULL);
    UI_ELEMENT*                     GetLayoutElement_ProgressBar              (XFILEXMLELEMENT* node, UI_ELEMENT* father, UI_ELEMENT* element_legacy = NULL);

    UI_ELEMENT*                     CreatePartialLayout                       (XFILEXMLELEMENT* nodeelement, UI_ELEMENT* father);
    bool                            CreateLayouts                             (XFILEXML& xml);
    
    bool                            CreateCacheElements                       (XFILEXMLELEMENT* nodeelement, bool recursive = true); 
    
    bool                            RegisterEvents                            (bool active); 

    UI_ELEMENT*                     PreSelectElement                          (UI_ELEMENT* element, int x, int y);
    
    bool                            SelectElement                             (UI_ELEMENT* element);
    bool                            SelectedElement                           (UI_ELEMENT* element);
    
    bool                            UnSelectedElement                         (UI_ELEMENT* element);
    bool                            UnSelectedElement                         ();

    bool                            UseMotionInElement                        (UI_ELEMENT* element, INPCURSORMOTION* cursormotion);
    bool                            UseMotion                                 (INPCURSORMOTION* cursormotion);

    void                            HandleEvent_UI                            (UI_XEVENT* event);
    void                            HandleEvent                               (XEVENT* xevent);
    
    void                            Clean                                     ();   

    static UI_MANAGER*              instance; 

    bool                            iszippedfile;
    XPATH                           unzippathfile;
    XFILEUNZIP*                     unzipfile;
        
    int                             selected_layout;
    int                             inall_layout;
    XVECTOR<UI_LAYOUT*>             layouts;

    XVECTOR<UI_SKIN*>               skins;
    UI_SKIN*                        skin_selected;

    XMUTEX*                         xmutex_modal;
    UI_ELEMENT*                     element_modal;

    XMUTEX*                         xmutex_UIevent;
    
    int                             last_xposition;
    int                             last_yposition;

    UI_ELEMENT*                     preselect_element;
    
    UI_VIRTUALKEYBOARD*             virtualkeyboard;     
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

