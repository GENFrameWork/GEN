/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_Skin.h
* 
* @class      UI_SKIN
* @brief      User Interface Skin Canvas Class
* @ingroup    USERINTERFACE
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @copyright  Copyright(c) 2005 - 2020 GEN Group.
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
* *---------------------------------------------------------------------------------------------------------------------*/

#ifndef _UI_SKIN_H_
#define _UI_SKIN_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "UI_Element.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/


enum UI_SKIN_TYPE
{
  UI_SKIN_TYPE_UNKNOWN             =  0  ,
  UI_SKIN_TYPE_FLAT                      ,
};


enum UI_SKIN_DRAWMODE
{
  UI_SKIN_DRAWMODE_UNKNOWN         =  0  ,
  UI_SKIN_DRAWMODE_CANVAS                ,
  UI_SKIN_DRAWMODE_CONTEXT               ,
};


#define UI_SKIN_NAME_UNKNOWN      __L("")
#define UI_SKIN_NAME_FLAT         __L("FLAT")

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class GRPBITMAP;
class UI_ANIMATION;


class UI_SKIN_TEXTBOX_PART
{
	public :
																			UI_SKIN_TEXTBOX_PART					      ();
		virtual													 ~UI_SKIN_TEXTBOX_PART					      ();

    XDWORD														GetLineNumber     									();		
		void															SetLineNumber   										(XDWORD linenumber);		

		XDWORD														GetXPos															();		
		void															SetXPos															(XDWORD xpos);		

		XDWORD														GetYPos															();		
		void															SetYPos															(XDWORD ypos);		

    UI_COLOR*                         GetColor                            ();

    XDWORD														GetWidth														();		
		void															SetWidth									  				(XDWORD width);		

    XDWORD														GetHeight 		  										();		
		void															SetHeight     						  				(XDWORD height);		

		XSTRING*													GetText															();

	private:

		void															Clean																();		

    XDWORD                            linenumber;
		XDWORD														xpos;
		XDWORD														ypos;	
    UI_COLOR                          color;
    XDWORD                            width;
    XDWORD                            height;    
		XSTRING														text;
};	



class UI_SKIN_TEXTBOX_IMAGE
{
  public :
																			UI_SKIN_TEXTBOX_IMAGE 				      ();
		virtual													 ~UI_SKIN_TEXTBOX_IMAGE     		      ();

    XSTRING*                          GetName                             ();
    UI_BOUNDARYLINE*                  GetBoundaryLine                     ();

    UI_ANIMATION*                     GetAnimation                        ();
    void                              SetAnimation                        (UI_ANIMATION* animation);

    XVECTOR<XDWORD>* 									GetLineNumbers     									();		
    bool                							IsInLine           									(XDWORD nline);		
	
	private:

		void															Clean																();		

    XSTRING                           name;
    UI_BOUNDARYLINE                   boundaryline;
    UI_ANIMATION*                     animation;
    XVECTOR<XDWORD> 									linenumbers; 
};



class UI_SKIN 
{
  public:
																		  UI_SKIN							      	        ();
    virtual													 ~UI_SKIN					      			        ();

    XSTRING*                          GetName                             ();
    UI_SKIN_TYPE                      GetType                             (XCHAR* name = NULL);

    UI_SKIN_DRAWMODE                  GetDrawMode                         ();
    bool                              SetDrawMode                         (UI_SKIN_DRAWMODE drawmode);   

    XSTRING*                          GetRasterFont                       ();
    XSTRING*                          GetVectorFont                       ();   
    virtual bool                      LoadFonts                           ();

    virtual double                    GetWithString                       (XCHAR* string, XDWORD sizefont = 12);  
    virtual double                    GetWithString                       (XSTRING& string, XDWORD sizefont = 12);  
    virtual double                    GetHeightString                     (XCHAR* string, XDWORD sizefont = 12);  
    virtual double                    GetHeightString                     (XSTRING& string, XDWORD sizefont = 12);   

    XSTRING*                          Background_GetColor                 ();
    XSTRING*                          Background_GetNameFile              ();
    virtual bool                      Background_LoadBitmap               ();    
    GRPBITMAP*                        Background_GetBitmap                ();    
    	
    bool                              CalculateBoundaryLine               (UI_ELEMENT* element);

    virtual bool                      CalculateBoundaryLine_Scroll        (UI_ELEMENT* element);    
	  virtual bool                      CalculateBoundaryLine_Text		      (UI_ELEMENT* element);
    virtual bool                      CalculateBoundaryLine_TextBox	      (UI_ELEMENT* element);	  
	  virtual bool                      CalculateBoundaryLine_Image   	    (UI_ELEMENT* element);
    virtual bool                      CalculateBoundaryLine_Animation	    (UI_ELEMENT* element);
    virtual bool                      CalculateBoundaryLine_Option		    (UI_ELEMENT* element);
    virtual bool                      CalculateBoundaryLine_MultiOption	  (UI_ELEMENT* element);
	  virtual bool                      CalculateBoundaryLine_Button		    (UI_ELEMENT* element);	
    virtual bool                      CalculateBoundaryLine_CheckBox      (UI_ELEMENT* element);	  
    virtual bool                      CalculateBoundaryLine_EditText      (UI_ELEMENT* element);	      
    virtual bool                      CalculateBoundaryLine_Form          (UI_ELEMENT* element); 
    virtual bool                      CalculateBoundaryLine_Menu				  (UI_ELEMENT* element); 
    virtual bool                      CalculateBoundaryLine_ListBox 		  (UI_ELEMENT* element);  
    virtual bool                      CalculateBoundaryLine_ProgressBar	  (UI_ELEMENT* element);  

    bool											        CalculeBoundaryLine_AllElements			(UI_ELEMENT* element, bool recursive = true);
      
    bool                              GetAddPositionScrollSteps           (UI_ELEMENT* element, double& x_positionwithscroll, double& y_positionwithscroll);

    virtual bool                      SetElementPosition                  (UI_ELEMENT* element, double x_position, double y_position);
    bool                              Elements_SetToRedraw                (UI_ELEMENT* element, bool recursive = true);

    bool                              Draw                                (UI_ELEMENT* element);    
	  
    virtual bool                      Draw_Scroll                         (UI_ELEMENT* element);
	  virtual bool                      Draw_Text		                        (UI_ELEMENT* element);	
    virtual bool                      Draw_TextBox	                      (UI_ELEMENT* element); 
	  virtual bool                      Draw_Image   	                      (UI_ELEMENT* element);
    virtual bool                      Draw_Animation   	                  (UI_ELEMENT* element);
    virtual bool                      Draw_Option                         (UI_ELEMENT* element);
    virtual bool                      Draw_MultiOption                    (UI_ELEMENT* element);
	  virtual bool                      Draw_Button		                      (UI_ELEMENT* element);	 	  
    virtual bool                      Draw_CheckBox                       (UI_ELEMENT* element);	 	  
    virtual bool                      Draw_EditText                       (UI_ELEMENT* element);	    
    virtual bool                      Draw_Form                           (UI_ELEMENT* element);
    virtual bool                      Draw_Menu				                    (UI_ELEMENT* element);
    virtual bool                      Draw_ListBox                        (UI_ELEMENT* element);  
    virtual bool                      Draw_ProgressBar                    (UI_ELEMENT* element);  
    

  protected:

    XSTRING                           name;
    UI_SKIN_DRAWMODE                  drawmode;

    XSTRING                           rasterfontname;
    XSTRING                           vectorfontname;

    XSTRING                           background_color;
    XSTRING                           background_namefile;  
    GRPBITMAP*                        background_bitmap;

  private:

		void															Clean												();    
};


/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif


