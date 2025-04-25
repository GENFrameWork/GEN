/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XFeedback_Control.cpp
* 
* @class      XFEEDBACK_CONTROL
* @brief      eXtended Utils Feedback Control class
* @ingroup    XUTILS
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


#ifdef XFEEDBACK_CONTROL_ACTIVE

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XFeedback_Control.h"

#include <stdlib.h>
#include <string.h>

#include "XFactory.h"
#include "XString.h"
#include "XTrace.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

XFEEDBACK_CONTROL* XFeedback_Control = NULL;

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


#pragma region CLASS_XFEEDBACK


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFEEDBACK::XFEEDBACK()
* @brief      Constructor of class
* @ingroup    XUTILS
* 
* --------------------------------------------------------------------------------------------------------------------*/
XFEEDBACK::XFEEDBACK()
{
  Clean();

  modulename = new XSTRING();  
  text       = new XSTRING();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFEEDBACK::~XFEEDBACK()
* @brief      Destructor of class
* @ingroup    XUTILS
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
XFEEDBACK::~XFEEDBACK()
{
  if(modulename)
    {
      delete modulename;  
    }

  if(text)
    {
      delete text;
    }       

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFEEDBACK_CODE XFEEDBACK::GetCode()
* @brief      get code
* @ingroup    XUTILS
* 
* @return     XFEEDBACK_CODE : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XFEEDBACK_CODE XFEEDBACK::GetCode()
{
  return code;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XFEEDBACK::SetCode(XFEEDBACK_CODE code)
* @brief      set code
* @ingroup    XUTILS
* 
* @param[in]  code : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XFEEDBACK::SetCode(XFEEDBACK_CODE code)
{
  this->code = code;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* XFEEDBACK::GetModuleName()
* @brief      get module name
* @ingroup    XUTILS
* 
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* XFEEDBACK::GetModuleName()
{
  return modulename;
}

               
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int XFEEDBACK::GetModuleLine()
* @brief      get module line
* @ingroup    XUTILS
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int XFEEDBACK::GetModuleLine()
{
  return moduleline;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XFEEDBACK::SetModuleLine(int moduleline)
* @brief      set module line
* @ingroup    XUTILS
* 
* @param[in]  moduleline : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XFEEDBACK::SetModuleLine(int moduleline)
{
  this->moduleline = moduleline;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* XFEEDBACK::GetText()
* @brief      get text
* @ingroup    XUTILS
* 
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* XFEEDBACK::GetText()
{
  return text;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XFEEDBACK::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @ingroup    XUTILS
* @note       INTERNAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XFEEDBACK::Clean()
{
  code        = XFEEDBACK_CODE_UNKNOWN;
  modulename  = NULL;
  moduleline  = 0;
  text        = NULL;
}


#pragma endregion



#pragma region CLASS_XFEEDBACK_CONTROL

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFEEDBACK_CONTROL::XFEEDBACK_CONTROL()
* @brief      Constructor of class
* @ingroup    XUTILS
* 
* --------------------------------------------------------------------------------------------------------------------*/
XFEEDBACK_CONTROL::XFEEDBACK_CONTROL()
{
  Clean();

  isactive = true;

  AddFeedbakText(XFEEDBACK_CODE_UNKNOWN                         , __L("Unknown"));
  AddFeedbakText(XFEEDBACK_CODE_INVALIDPARAMFUNCTION            , __L("The %d parameter in the function call was invalid."));
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFEEDBACK_CONTROL::~XFEEDBACK_CONTROL()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    XUTILS
* 
* --------------------------------------------------------------------------------------------------------------------*/
XFEEDBACK_CONTROL::~XFEEDBACK_CONTROL()
{
  DeleteAllResources();

  isactive = false;

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFEEDBACK_CONTROL::IsActive()
* @brief      Is active
* @ingroup    XUTILS
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XFEEDBACK_CONTROL::IsActive()
{
  return isactive;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFEEDBACK_CONTROL::Activate(bool isactive)
* @brief      Activate
* @ingroup    XUTILS
* 
* @param[in]  isactive : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XFEEDBACK_CONTROL::Activate(bool isactive)
{
  if(this->isactive == isactive) return false;

  this->isactive = isactive;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFEEDBACK_CONTROL::AddFeedbak(char const* namefile, int line, XFEEDBACK_CODE code, ...)
* @brief      add feedbak
* @ingroup    XUTILS
* 
* @param[in]  const* namefile : 
* @param[in]  line : 
* @param[in]  code : 
* @param[in]  ... : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XFEEDBACK_CONTROL::AddFeedbak(char const* namefile, int line, XFEEDBACK_CODE code, ...)
{
  if(!isactive)
    {
      return false;
    }

  XFEEDBACK* feedback = new XFEEDBACK();
  if(feedback)
    {
      feedback->SetCode(code);
      feedback->GetModuleName()->Set(namefile);
      feedback->SetModuleLine(line);

      XSTRING text;
      
      if(GetFeedbakText(code, text))
        {
          if(!text.IsEmpty())
            {              
              va_list arg;

              va_start(arg, code);
           
              switch(code)
                {
                  case XFEEDBACK_CODE_UNKNOWN               : feedback->GetText()->Set(text);  
                                                              break;

                  case XFEEDBACK_CODE_INVALIDPARAMFUNCTION  : { int indexparam = va_arg(arg, int);
                                                                feedback->GetText()->Format(text.Get(), indexparam);
                                                              }
                                                              break;  

                                                  default   : feedback->GetText()->Set(text);  
                                                              break;   
                }

              va_end(arg);
              
              return feedbacks.Add(feedback);
            }
        }
    }

  delete feedback;
  
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFEEDBACK_CONTROL::DisplayAll()
* @brief      display all
* @ingroup    XUTILS
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XFEEDBACK_CONTROL::DisplayAll()
{     
  #ifdef XTRACE_ACTIVE
  
  XDWORD  nfeedback = feedbacks.GetSize();
  XBYTE   level     = (!nfeedback?XTRACE_COLOR_BLUE:XTRACE_COLOR_RED);

  XTRACE_PRINT(__L(" "));
  XTRACE_PRINTHEADER((level | XTRACE_LEVEL_WITHCOLOR), (!nfeedback)?__L("ALL FEEDBACK ENTRYS"):__L("NOT FEEDBACK ENTRYS"));
  XTRACE_PRINT(__L(" "));

  if(nfeedback)
    {
      XSTRING title;
      
      title.Format(__L("%-8s %-5s %-72s %s"), __L("Index"), __L("Line"), __L("Module"), __L("Description"));  

      XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("Number feedbacks  : %d"), nfeedback);
      XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L(" "));
      XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, title.Get());
      
      for(XDWORD c=0; c<nfeedback; c++)
        {
          XFEEDBACK* feedback = feedbacks.Get(c);

          if(feedback)
            {              
              XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("%08d %05d %-72s %s") , c
                                                                            , feedback->GetModuleLine()
                                                                            , feedback->GetModuleName()->Get()
                                                                            , feedback->GetText()->Get());                                                                               
            }
        }

      XTRACE_PRINTHEADER((level | XTRACE_LEVEL_WITHCOLOR), NULL);
    }

  #endif

  return true;
  
}

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFEEDBACK_CONTROL::DeleteAllResources()
* @brief      delete all resources
* @ingroup    XUTILS
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XFEEDBACK_CONTROL::DeleteAllResources()
{
  feedbacks_text.DeleteElementContents();
  feedbacks_text.DeleteAll();

  feedbacks.DeleteContents();
  feedbacks.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFEEDBACK_CONTROL::AddFeedbakText(XFEEDBACK_CODE code, XCHAR* text)
* @brief      add feedbak text
* @ingroup    XUTILS
* 
* @param[in]  code : 
* @param[in]  text : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XFEEDBACK_CONTROL::AddFeedbakText(XFEEDBACK_CODE code, XCHAR* text)
{
  XSTRING* textcontainer;

  textcontainer = new XSTRING();
  if(textcontainer)
    {
      textcontainer->Set(text);
      return feedbacks_text.Add(code, textcontainer);
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFEEDBACK_CONTROL::GetFeedbakText(XFEEDBACK_CODE code, XSTRING& text)
* @brief      get feedbak text
* @ingroup    XUTILS
* 
* @param[in]  code : 
* @param[in]  text : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XFEEDBACK_CONTROL::GetFeedbakText(XFEEDBACK_CODE code, XSTRING& text)
{
  if(feedbacks_text.IsEmpty())
    {
      return false;  
    }

  XSTRING* textcontainer = feedbacks_text.Get(code); 
  if(!textcontainer)
    {
      return false;
    }

  text = (*textcontainer);
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFEEDBACK_CONTROL::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
void XFEEDBACK_CONTROL::Clean()
{
  isactive      = false;  
}


#pragma endregion


#pragma endregion


#endif



