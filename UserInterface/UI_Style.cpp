/**-------------------------------------------------------------------------------------------------------------------
*
* @file       UI_Style.cpp
*
* @class      UI_STYLE
* @brief      User Interface neutral property/style bag (source-format independent IR)
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

#include "UI_Style.h"

#include "XFileXML.h"

#include "UI_Element.h"
#include "UI_StyleSheet.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_STYLE::UI_STYLE()
* @brief      Constructor of class
* @ingroup    USERINTERFACE
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_STYLE::UI_STYLE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_STYLE::~UI_STYLE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    USERINTERFACE
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_STYLE::~UI_STYLE()
{
  DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_STYLE::Set(XCHAR* key, XCHAR* value)
* @brief      Set (insert or overwrite) a property by key
* @ingroup    USERINTERFACE
* 
* @param[in]  key : Key pointer to use.
* @param[in]  value : Value pointer to use.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_STYLE::Set(XCHAR* key, XCHAR* value)
{
  if(!key)   return;
  if(!value) return;

  UI_STYLEPROPERTY* property = Find(key);
  if(property)
    {
      property->GetValue().Set(value);
      return;
    }

  property = GEN_NEW UI_STYLEPROPERTY(key, value);
  if(property) properties.Add(property);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_STYLE::Set(XCHAR* key, XSTRING& value)
* @brief      Set (insert or overwrite) a property by key
* @ingroup    USERINTERFACE
* 
* @param[in]  key : Key pointer to use.
* @param[in]  value : Value value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_STYLE::Set(XCHAR* key, XSTRING& value)
{
  Set(key, value.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_STYLE::Exists(XCHAR* key)
* @brief      Exists
* @ingroup    USERINTERFACE
* 
* @param[in]  key : Key pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_STYLE::Exists(XCHAR* key)
{
  return Find(key) ? true : false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_STYLE::Get(XCHAR* key, XSTRING& value)
* @brief      Get a property as string. Mirrors GetLayoutElementValue(node, key, XSTRING&).
* @ingroup    USERINTERFACE
* 
* @param[in]  key : Key pointer to use.
* @param[in]  value : Value value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_STYLE::Get(XCHAR* key, XSTRING& value)
{
  value.Empty();

  UI_STYLEPROPERTY* property = Find(key);
  if(!property) return false;

  value = property->GetValue();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_STYLE::Get(XCHAR* key, double& value)
* @brief      Get a property as double. Mirrors GetLayoutElementValue(node, key, double&).
* @ingroup    USERINTERFACE
* 
* @param[in]  key : Key pointer to use.
* @param[in]  value : Value value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_STYLE::Get(XCHAR* key, double& value)
{
  UI_STYLEPROPERTY* property = Find(key);
  if(!property) return false;

  value = property->GetValue().ConvertToDouble();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<UI_STYLEPROPERTY*>* UI_STYLE::GetProperties()
* @brief      Get properties
* @ingroup    USERINTERFACE
* 
* @return     XVECTOR<UI_STYLEPROPERTY*>* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<UI_STYLEPROPERTY*>* UI_STYLE::GetProperties()
{
  return &properties;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_STYLE::DeleteAll()
* @brief      Delete all
* @ingroup    USERINTERFACE
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_STYLE::DeleteAll()
{
  for(XDWORD c=0; c<properties.GetSize(); c++)
    {
      UI_STYLEPROPERTY* property = properties.Get(c);
      if(property) GEN_DELETE property;
    }

  properties.DeleteAll();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool UI_STYLE::FillFromXMLElement(XFILEXMLELEMENT* node)
* @brief      XML front-end: copy every attribute of the node into the neutral bag.
* @ingroup    USERINTERFACE
* 
* @param[in]  node : Node pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_STYLE::FillFromXMLElement(XFILEXMLELEMENT* node)
{
  if(!node) return false;

  for(int c=0; c<node->GetNAttributes(); c++)
    {
      XFILEXMLATTRIBUTE* attribute = node->GetAttribute(c);
      if(attribute)
        {
          Set(attribute->GetName().Get(), attribute->GetValue().Get());
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool UI_STYLE::FillFromCSSDeclarations(UI_STYLESHEET* sheet, UI_ELEMENT* element)
* @brief      CSS front-end. Layers all matching stylesheet rules on top of the current bag using the element's
*             identity (type / id / class list). Existing keys are overwritten by matches (CSS-wins semantics).
* @ingroup    USERINTERFACE
*
* @param[in]  sheet : Parsed stylesheet; NULL is a valid no-op.
* @param[in]  element : Element whose identity drives selector matching; NULL is a no-op.
*
* @return     bool : true if at least one rule matched; false otherwise.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool UI_STYLE::FillFromCSSDeclarations(UI_STYLESHEET* sheet, UI_ELEMENT* element)
{
  if(!sheet)    return false;
  if(!element)  return false;

  XSTRING* type_string = element->GetTypeString();
  XSTRING* name        = element->GetName();

  XSTRING            emptystr;
  XVECTOR<XSTRING*>  emptyclasses;

  XSTRING&           elem_type    = type_string ? *type_string  : emptystr;
  XSTRING&           elem_id      = name        ? *name         : emptystr;
  XVECTOR<XSTRING*>& elem_classes = element->GetClassNames() ? *element->GetClassNames() : emptyclasses;

  // Active pseudo-classes: derived from the element's current live state (ispreselect/isselected/isactive).
  // At load time all state flags default to false / active-true, so the pseudo set is effectively empty and
  // this call reproduces the stateless cascade of step 2. When the element later transitions state, the same
  // helper is invoked again to layer state-specific rules on top.
  XVECTOR<XSTRING*> activepseudos;
  element->GetActivePseudos(activepseudos);

  bool ok = sheet->Resolve(elem_type, elem_id, elem_classes, activepseudos, *this);

  // Owned strings produced by GetActivePseudos().
  for(XDWORD c=0; c<activepseudos.GetSize(); c++)
    {
      XSTRING* p = activepseudos.Get(c);
      if(p) GEN_DELETE p;
    }
  activepseudos.DeleteAll();

  return ok;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         UI_STYLEPROPERTY* UI_STYLE::Find(XCHAR* key)
* @brief      Find a property by key (case-insensitive, matching XML attribute lookup semantics).
* @note       INTERNAL
* @ingroup    USERINTERFACE
* 
* @param[in]  key : Key pointer to use.
* 
* @return     UI_STYLEPROPERTY* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
UI_STYLEPROPERTY* UI_STYLE::Find(XCHAR* key)
{
  if(!key) return NULL;

  XSTRING keystr = key;

  for(XDWORD c=0; c<properties.GetSize(); c++)
    {
      UI_STYLEPROPERTY* property = properties.Get(c);
      if(property)
        {
          if(!property->GetKey().Compare(keystr, true)) return property;
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void UI_STYLE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    USERINTERFACE
* 
* --------------------------------------------------------------------------------------------------------------------*/
void UI_STYLE::Clean()
{

}
