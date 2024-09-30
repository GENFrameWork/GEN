/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_VirtualKeyboard.h
* 
* @class      UI_VIRTUALKEYBOARD
* @brief      User Interface Virtual Keyboard class
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

#ifndef _UI_VIRTUALKEYBOARD_H_
#define _UI_VIRTUALKEYBOARD_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XBase.h"
#include "XString.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

enum UI_VIRTUALKEYBOARD_SET
{
  UI_VIRTUALKEYBOARD_SET_UPPERCASE          =  0  , 
  UI_VIRTUALKEYBOARD_SET_LOWERCASE                , 
  UI_VIRTUALKEYBOARD_SET_SYMBOLS                  , 
};

#define UI_VIRTUALKEYBOARD_ELEMENTID   __L("@[VK_KEY]_")

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class GRPSCREEN;
class GRPBITMAP;
class UI_LAYOUT;
class UI_SKIN;
class UI_ANIMATION;
class UI_ELEMENT;
class UI_ELEMENT_FORM;


class UI_VIRTUALKEYBOARD_KEYINFO
{
  public:
                                  UI_VIRTUALKEYBOARD_KEYINFO        ();    
    virtual                      ~UI_VIRTUALKEYBOARD_KEYINFO        ();    

    XSTRING                       color;
    XDWORD                        sizefont;
    double                        margin;
    double                        xpos;
    double                        ypos;
    double                        width;
    double                        height;
  
  private:

    void                          Clean                             ();
};


class UI_VIRTUALKEYBOARD
{
  public:
                                  UI_VIRTUALKEYBOARD                ();    
    virtual                      ~UI_VIRTUALKEYBOARD                ();    

    bool                          Ini                               (UI_LAYOUT* layout, GRPSCREEN* screen);      

    bool                          IsShow                            ();
    bool                          Show                              (bool on, UI_ELEMENT* element_editable = NULL);

    UI_ELEMENT*                   GetElementEditable                ();

    bool                          SelectInput                       (UI_ELEMENT* key_select);      

    bool                          End                               ();
  
  private:

    UI_ANIMATION*                 AddImageCache                     (XCHAR* name, XCHAR* resource);
    GRPBITMAP*                    LoadKeyImage                      (XCHAR* pathimage);
    bool                          AddKeyButton                      (XCHAR* leyend, XCHAR* text, XCHAR* pathbitmap, UI_VIRTUALKEYBOARD_KEYINFO& keyinfo);
    bool                          CreateAllKeys                     (double x, double y);
    bool                          DeleteAllKeys                     ();
        
    void                          Clean                             ();

    GRPSCREEN*                    screen;
    UI_LAYOUT*                    layout;
    UI_SKIN*                      skin;
    bool                          isshow;
    UI_ELEMENT_FORM*              main_form;
    UI_ELEMENT*                   element_editable;
    double                        width;
    double                        height;
    UI_VIRTUALKEYBOARD_SET        actualset;
    bool                          contentchanged;           
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

