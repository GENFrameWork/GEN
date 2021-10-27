/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_Property_Scrolleable.h
* 
* @class      UI_PROPERTY_SCROLLEABLE
* @brief      User Interface Element Scrolleable class
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

#ifndef _UI_PROPERTY_SCROLLEABLE_H_
#define _UI_PROPERTY_SCROLLEABLE_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "UI_BoundaryLine.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

enum UI_PROPERTY_SCROLLEABLE_TYPE
{
  UI_PROPERTY_SCROLLEABLE_TYPE_UNKWOWN        = 0 ,
  UI_PROPERTY_SCROLLEABLE_TYPE_HORIZONTAL         ,
  UI_PROPERTY_SCROLLEABLE_TYPE_VERTICAL           ,
};


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class UI_PROPERTY_SCROLLEABLE_STATUS
{
  public:
																		          UI_PROPERTY_SCROLLEABLE_STATUS	        ();
    virtual											         	   ~UI_PROPERTY_SCROLLEABLE_STATUS          ();

    bool                                      active;
    double                                    limit;        
    double                                    displacement;    
    double                                    step;                     

  private:

    void															        Clean										                ();              
};



class UI_PROPERTY_SCROLLEABLE 
{
  public:
																		          UI_PROPERTY_SCROLLEABLE		              ();
    virtual											         	   ~UI_PROPERTY_SCROLLEABLE	  	            ();
    
    double                                    H_GetLimit                              ();
    void                                      H_SetLimit                              (double  limit);

    double                                    V_GetLimit                              ();
    void                                      V_SetLimit                              (double limit);

    double                                    H_GetDisplacement                       ();
    double                                    V_GetDisplacement                       ();

    double                                    H_GetStep                               ();
    void                                      H_SetStep                               (double h_step);

    double                                    V_GetStep                               ();
    void                                      V_SetStep                               (double v_step);

    void                                      ResetSteps                              ();


    
    double                                    GetLimit                                (UI_PROPERTY_SCROLLEABLE_TYPE type);
    void                                      SetLimit                                (UI_PROPERTY_SCROLLEABLE_TYPE type, double  limit);

    double                                    GetDisplacement                         (UI_PROPERTY_SCROLLEABLE_TYPE type);

    double                                    GetStep                                 (UI_PROPERTY_SCROLLEABLE_TYPE type);
    void                                      SetStep                                 (UI_PROPERTY_SCROLLEABLE_TYPE type, double h_step);

    void                                      Step_Reset                              (UI_PROPERTY_SCROLLEABLE_TYPE type);

   
      	
  private:

		void															        Clean										                ();  


    double                                    h_limit;    
    double                                    v_limit;
    
    double                                    h_displacement;
    double                                    v_displacement;

    double                                    h_step;
    double                                    v_step;


    UI_PROPERTY_SCROLLEABLE_STATUS            horizontal;  
    UI_PROPERTY_SCROLLEABLE_STATUS            vertical;  
};



/*
class UI_PROPERTY_SCROLLEABLE 
{
  public:
																		          UI_PROPERTY_SCROLLEABLE		              ();
    virtual											         	   ~UI_PROPERTY_SCROLLEABLE	  	            ();
        
    
    double                                    GetLimit                                (UI_PROPERTY_SCROLLEABLE_TYPE type);
    void                                      SetLimit                                (UI_PROPERTY_SCROLLEABLE_TYPE type, double  limit);

    double                                    GetDisplacement                         (UI_PROPERTY_SCROLLEABLE_TYPE type);

    double                                    GetStep                                 (UI_PROPERTY_SCROLLEABLE_TYPE type);
    void                                      SetStep                                 (UI_PROPERTY_SCROLLEABLE_TYPE type, double h_step);

    void                                      Step_Reset                              (UI_PROPERTY_SCROLLEABLE_TYPE type);

   
      	
  private:

		void															        Clean										                ();  


    UI_PROPERTY_SCROLLEABLE_STATUS            horizontal;  
    UI_PROPERTY_SCROLLEABLE_STATUS            vertical;  
};
*/

/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif


