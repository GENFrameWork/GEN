/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       UI_Colors.cpp
* 
* @class      UI_COLORS
* @brief      User Interface Colors class
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

/*---- PRECOMPILATION INCLUDES ----------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "UI_Colors.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

UI_COLORS* UI_COLORS::instance = NULL;

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_COLORS::GetIsInstanced()
* @brief      GetIsInstanced
* @ingroup    USERINTERFACE
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_COLORS::GetIsInstanced()
{
  return instance!=NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_COLORS& UI_COLORS::GetInstance()
* @brief      GetInstance
* @ingroup    USERINTERFACE
*
* @return     UI_COLORS& : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
UI_COLORS& UI_COLORS::GetInstance()
{
  if(!instance) instance = new UI_COLORS();

  return (*instance);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_COLORS::DelInstance()
* @brief      DelInstance
* @ingroup    USERINTERFACE
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_COLORS::DelInstance()
{
  if(instance)
    {
      delete instance;
      instance = NULL;

      return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_COLORS::Add(XCHAR* name, XCHAR* colorstr)
* @brief      Add
* @ingroup    USERINTERFACE
*
* @param[in]  name : 
* @param[in]  colorstr : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_COLORS::Add(XCHAR* name, XCHAR* colorstr)
{
  if(!name)       return false;
  if(!colorstr)   return false;

  XSTRING* _name = new XSTRING();
  if(!_name) return false;

  _name->Set(name);

  XSTRING* _colorstr = new XSTRING();
  if(_colorstr) 
    {     
      _colorstr->Set(colorstr);

      if(!_colorstr->IsEmpty())
        {                 
          colors.Add(_name, _colorstr); 
          return true;         
        }
    }
          
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_COLORS::Add(XSTRING& name, XSTRING& colorstr)
* @brief      Add
* @ingroup    USERINTERFACE
*
* @param[in]  name : 
* @param[in]  colorstr : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_COLORS::Add(XSTRING& name, XSTRING& colorstr)
{
  return Add(name.Get(), colorstr.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* UI_COLORS::Get(XCHAR* name)
* @brief      Get
* @ingroup    USERINTERFACE
*
* @param[in]  name : 
* 
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* UI_COLORS::Get(XCHAR* name)
{
  if(!name) return NULL;
  
  for(XDWORD c=0; c<colors.GetSize(); c++)
    {
      XSTRING* key = colors.GetKey(c);
      if(key)
        {
          if(!key->Compare(name)) return colors.GetElement(c);
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* UI_COLORS::Get(XSTRING& name)
* @brief      Get
* @ingroup    USERINTERFACE
*
* @param[in]  name : 
* 
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* UI_COLORS::Get(XSTRING& name)
{
  return Get(name.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_COLORS::DeleteAll()
* @brief      DeleteAll
* @ingroup    USERINTERFACE
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool UI_COLORS::DeleteAll()
{
  colors.DeleteKeyContents();
  colors.DeleteElementContents();
  colors.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_COLORS::UI_COLORS()
* @brief      Constructor
* @ingroup    USERINTERFACE
*
* ---------------------------------------------------------------------------------------------------------------------*/
UI_COLORS::UI_COLORS()
{
  Clean();

  Add(__L("red")      , __L("255,0,0"));
  Add(__L("green")    , __L("0,255,0"));
  Add(__L("blue")     , __L("0,0,255"));  
  Add(__L("white")    , __L("255,255,255"));
  Add(__L("black")    , __L("0,0,0"));
  Add(__L("yellow")   , __L("255,255,0"));
  Add(__L("cyan")     , __L("0,255,255"));
  Add(__L("purple")   , __L("128,0,128"));
  Add(__L("gray")     , __L("128,128,128"));
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_COLORS::~UI_COLORS()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    USERINTERFACE
*
* ---------------------------------------------------------------------------------------------------------------------*/
UI_COLORS::~UI_COLORS()
{
  DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_COLORS::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    USERINTERFACE
*
* ---------------------------------------------------------------------------------------------------------------------*/
void UI_COLORS::Clean()
{

}


#pragma endregion

