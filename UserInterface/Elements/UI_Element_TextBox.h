/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_Element_TextBox.h
* 
* @class      UI_ELEMENT_TEXTBOX
* @brief      User Interface Element Text Box class
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

#ifndef _UI_ELEMENT_TEXTBOX_H_
#define _UI_ELEMENT_TEXTBOX_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "UI_Property_Scrolleable.h"
#include "UI_Element.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

#define UI_ELEMENT_TEXTBOX_DEFAULTLINESPACING     4

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class UI_ELEMENT_TEXTBOX : public UI_ELEMENT, public UI_PROPERTY_SCROLLEABLE
{
  public:
																		  UI_ELEMENT_TEXTBOX			    ();
    virtual													 ~UI_ELEMENT_TEXTBOX				  ();
    
    XDWORD                            GetSizeFont                 ();      
    void                              SetSizeFont                 (XDWORD sizefont);   

    XDWORD                            GetLineSpacing              ();
    void                              SetLineSpacing              (XDWORD linespacing = UI_ELEMENT_TEXTBOX_DEFAULTLINESPACING);

    UI_ELEMENT_TYPE_ALIGN             GetTextAlignment            ();     
    void                              SetTextAlignment            (UI_ELEMENT_TYPE_ALIGN textalignment);     

    bool                              IsWordClipping              ();
    void                              SetIsWordClipping           (bool iswordclipping);

    XSTRING*                          GetText                     (); 
    XSTRING*                          GetMaskText                 (); 
	  
  private:

		void															Clean												();    
      
    XDWORD                            sizefont;   
    XDWORD                            linespacing;
    UI_ELEMENT_TYPE_ALIGN             textalignment;
    bool                              iswordclipping;
    XSTRING                           text;         
    XSTRING                           masktext;         
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

