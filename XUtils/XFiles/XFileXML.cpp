/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XFileXML.cpp
* 
* @class      XFILEXML
* @brief      eXtended Utils XML (eXtensible Markup Language) file class
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

/*---- PRECOMPILATION INCLUDES ----------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XFileXML.h"

#include <stdio.h>
#include <string.h>

#include "XTrace.h"
#include "XFactory.h"
#include "XTimer.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


#pragma region CLASS_XFILEXMLATTRIBUTE


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLATTRIBUTE::XFILEXMLATTRIBUTE()
* @brief      Constructor
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLATTRIBUTE::XFILEXMLATTRIBUTE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLATTRIBUTE::XFILEXMLATTRIBUTE(XSTRING& name)
* @brief      Constructor
* @ingroup    XUTILS
*
* @param[in]  XSTRING& :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLATTRIBUTE::XFILEXMLATTRIBUTE(XSTRING& name)
{
  Clean();

  this->name = name;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLATTRIBUTE::XFILEXMLATTRIBUTE(XCHAR* name)
* @brief      Constructor
* @ingroup    XUTILS
*
* @param[in]  XCHAR* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLATTRIBUTE::XFILEXMLATTRIBUTE(XCHAR* name)
{
  Clean();

  this->name = name;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLATTRIBUTE::XFILEXMLATTRIBUTE(XSTRING& name,XSTRING& value)
* @brief      Constructor
* @ingroup    XUTILS
*
* @param[in]  XSTRING& :
* @param[in]  XSTRING& :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLATTRIBUTE::XFILEXMLATTRIBUTE(XSTRING& name, XSTRING& value)
{
  Clean();

  this->name  = name;
  this->value = value;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLATTRIBUTE::XFILEXMLATTRIBUTE(XCHAR* name, XCHAR* value)
* @brief      Constructor
* @ingroup    XUTILS
*
* @param[in]  XCHAR* :
* @param[in]   XCHAR* value :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLATTRIBUTE::XFILEXMLATTRIBUTE(XCHAR* name, XCHAR* value)
{
  Clean();

  this->name  = name;
  this->value = value;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLATTRIBUTE::~XFILEXMLATTRIBUTE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLATTRIBUTE::~XFILEXMLATTRIBUTE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING& XFILEXMLATTRIBUTE::GetName()
* @brief      GetName
* @ingroup    XUTILS
*
* @return     XSTRING& :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING& XFILEXMLATTRIBUTE::GetName()
{
  return name;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLATTRIBUTE::SetName(XSTRING& name)
* @brief      SetName
* @ingroup    XUTILS
*
* @param[in]  name :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLATTRIBUTE::SetName(XSTRING& name)
{
  this->name = name;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLATTRIBUTE::SetName(XCHAR* name)
* @brief      SetName
* @ingroup    XUTILS
*
* @param[in]  name :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLATTRIBUTE::SetName(XCHAR* name)
{
  this->name = name;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING& XFILEXMLATTRIBUTE::GetValue()
* @brief      GetValue
* @ingroup    XUTILS
*
* @return     XSTRING& :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING& XFILEXMLATTRIBUTE::GetValue()
{
  return value;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLATTRIBUTE::SetValue(XSTRING& value)
* @brief      SetValue
* @ingroup    XUTILS
*
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLATTRIBUTE::SetValue(XSTRING& value)
{
  this->value = value;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLATTRIBUTE::SetValue(XCHAR* value)
* @brief      SetValue
* @ingroup    XUTILS
*
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLATTRIBUTE::SetValue(XCHAR* value)
{
  this->value = value;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILEXMLATTRIBUTE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
void XFILEXMLATTRIBUTE::Clean()
{
  name  = "";
  value = "";
}


#pragma endregion


#pragma region CLASS_XFILEXMLELEMENT


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT::XFILEXMLELEMENT()
* @brief      Constructor
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT::XFILEXMLELEMENT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEXMLELEMENT::XFILEXMLELEMENT(XSTRING& name, XFILEXMLELEMENTTYPE type)
* @brief      Constructor
* @ingroup    XUTILS
* 
* @param[in]  XSTRING& : 
* @param[in]  XFILEXMLELEMENTTYPE type : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT::XFILEXMLELEMENT(XSTRING& name, XFILEXMLELEMENTTYPE type)
{
  Clean();

  this->name  = name;
  this->type  = type;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT::XFILEXMLELEMENT(XCHAR* name, XFILEXMLELEMENTTYPE type)
* @brief      Constructor
* @ingroup    XUTILS
*
* @param[in]  XCHAR* :
* @param[in]   XFILEXMLELEMENTTYPE type :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT::XFILEXMLELEMENT(XCHAR* name, XFILEXMLELEMENTTYPE type)
{
  Clean();

  this->name  = name;
  this->type  = type;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT::XFILEXMLELEMENT(XSTRING& name,XSTRING& value, XFILEXMLELEMENTTYPE type)
* @brief      Constructor
* @ingroup    XUTILS
*
* @param[in]  XSTRING& :
* @param[in]  XSTRING& :
* @param[in]   XFILEXMLELEMENTTYPE type :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT::XFILEXMLELEMENT(XSTRING& name,XSTRING& value, XFILEXMLELEMENTTYPE type)
{
  Clean();

  this->name  = name;
  this->value = value;
  this->type  = type;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT::XFILEXMLELEMENT(XCHAR* name,XCHAR* value, XFILEXMLELEMENTTYPE type)
* @brief      Constructor
* @ingroup    XUTILS
*
* @param[in]  XCHAR* :
* @param[in]  XCHAR* :
* @param[in]   XFILEXMLELEMENTTYPE type :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT::XFILEXMLELEMENT(XCHAR* name,XCHAR* value, XFILEXMLELEMENTTYPE type)
{
  Clean();

  this->name  = name;
  this->value = value;
  this->type  = type;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT::~XFILEXMLELEMENT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT::~XFILEXMLELEMENT()
{
  DeleteAllAtributes();
  DeleteAllElements();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING& XFILEXMLELEMENT::GetName()
* @brief      GetName
* @ingroup    XUTILS
*
* @return     XSTRING& :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING& XFILEXMLELEMENT::GetName()
{
  return name;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLELEMENT::SetName(XSTRING& name)
* @brief      SetName
* @ingroup    XUTILS
*
* @param[in]  name :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLELEMENT::SetName(XSTRING& name)
{
  this->name = name;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLELEMENT::SetName(XCHAR* name)
* @brief      SetName
* @ingroup    XUTILS
*
* @param[in]  name :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLELEMENT::SetName(XCHAR* name)
{
  this->name = name;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLATTRIBUTE* XFILEXMLELEMENT::AddAtribute(XSTRING& name, XSTRING& value)
* @brief      AddAtribute
* @ingroup    XUTILS
*
* @param[in]  name :
* @param[in]  value :
*
* @return     XFILEXMLATTRIBUTE* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLATTRIBUTE* XFILEXMLELEMENT::AddAtribute(XSTRING& name, XSTRING& value)
{
  XFILEXMLATTRIBUTE* attribute = new XFILEXMLATTRIBUTE(name,value);
  if(!attribute) return NULL;

  attributes.Add(attribute);

  return attribute;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLATTRIBUTE* XFILEXMLELEMENT::AddAtribute(XCHAR* name, XCHAR* value)
* @brief      AddAtribute
* @ingroup    XUTILS
*
* @param[in]  name :
* @param[in]  value :
*
* @return     XFILEXMLATTRIBUTE* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLATTRIBUTE* XFILEXMLELEMENT::AddAtribute(XCHAR* name, XCHAR* value)
{
  XFILEXMLATTRIBUTE* attribute = new XFILEXMLATTRIBUTE(name,value);
  if(!attribute) return NULL;

  attributes.Add(attribute);

  return attribute;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XFILEXMLELEMENT::GetNAttributes()
* @brief      GetNAttributes
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XFILEXMLELEMENT::GetNAttributes()
{
  return attributes.GetSize();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLATTRIBUTE* XFILEXMLELEMENT::GetAttribute(int index)
* @brief      GetAttribute
* @ingroup    XUTILS
*
* @param[in]  index :
*
* @return     XFILEXMLATTRIBUTE* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLATTRIBUTE* XFILEXMLELEMENT::GetAttribute(int index)
{
  if(attributes.IsEmpty()) return NULL;

  XFILEXMLATTRIBUTE* attribute = (XFILEXMLATTRIBUTE*)attributes.Get(index);
  return attribute;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* XFILEXMLELEMENT::GetValueAttribute(XSTRING& name)
* @brief      GetValueAttribute
* @ingroup    XUTILS
*
* @param[in]  name :
*
* @return     XCHAR* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* XFILEXMLELEMENT::GetValueAttribute(XSTRING& name)
{
  return GetValueAttribute(name.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* XFILEXMLELEMENT::GetValueAttribute(XCHAR* name)
* @brief      GetValueAttribute
* @ingroup    XUTILS
*
* @param[in]  name :
*
* @return     XCHAR* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* XFILEXMLELEMENT::GetValueAttribute(XCHAR* name)
{
  int nattributes = GetNAttributes();

  for(int c=0; c<nattributes; c++)
    {
      XFILEXMLATTRIBUTE* attribute =  GetAttribute(c);
      if(attribute)
        {
          if(!attribute->GetName().Compare(name))
            {
              return attribute->GetValue().Get();
            }
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLELEMENT::GetValueAttribute(XSTRING& name, XSTRING& value)
* @brief      GetValueAttribute
* @ingroup    XUTILS
*
* @param[in]  name :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLELEMENT::GetValueAttribute(XSTRING& name, XSTRING& value)
{
  return GetValueAttribute(name.Get(),value);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLELEMENT::GetValueAttribute(XCHAR* name,XSTRING& value)
* @brief      GetValueAttribute
* @ingroup    XUTILS
*
* @param[in]  name :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLELEMENT::GetValueAttribute(XCHAR* name,XSTRING& value)
{
  for(int c=0;c<GetNAttributes();c++)
    {
      XFILEXMLATTRIBUTE* attribute =  GetAttribute(c);
      if(attribute)
        {
          if(!attribute->GetName().Compare(name))
            {
              value = attribute->GetValue();

              return true;
            }
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XFILEXMLELEMENT::GetIntValueAttribute(XSTRING& name)
* @brief      GetIntValueAttribute
* @ingroup    XUTILS
*
* @param[in]  name :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XFILEXMLELEMENT::GetIntValueAttribute(XSTRING& name)
{
  return GetIntValueAttribute(name.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XFILEXMLELEMENT::GetIntValueAttribute(XCHAR* name)
* @brief      GetIntValueAttribute
* @ingroup    XUTILS
*
* @param[in]  name :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XFILEXMLELEMENT::GetIntValueAttribute(XCHAR* name)
{
  XSTRING value;

  if(GetValueAttribute(name,value)) return value.ConvertToInt();

  return 0;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLELEMENT::GetBoolValueAttribute(XCHAR* name)
* @brief      GetBoolValueAttribute
* @ingroup    XUTILS
*
* @param[in]  name :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLELEMENT::GetBoolValueAttribute(XCHAR* name)
{
  XSTRING value;

  if(GetValueAttribute(name,value))
    return value.ConvertToBoolean();

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double XFILEXMLELEMENT::GetFloatValueAttribute(XCHAR* name)
* @brief      GetFloatValueAttribute
* @ingroup    XUTILS
*
* @param[in]  name :
*
* @return     double :
*
* --------------------------------------------------------------------------------------------------------------------*/
double XFILEXMLELEMENT::GetFloatValueAttribute(XCHAR* name)
{
  XSTRING value;

  if(GetValueAttribute(name,value))
    return value.ConvertToDouble();

  return 0.0;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLELEMENT::DeleteAtribute(int index)
* @brief      DeleteAtribute
* @ingroup    XUTILS
*
* @param[in]  index :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLELEMENT::DeleteAtribute(int index)
{
  XFILEXMLATTRIBUTE* attribute = (XFILEXMLATTRIBUTE*)attributes.Get(index);
  if(!attribute)  return false;

  attributes.Delete(attribute);

  delete attribute;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLELEMENT::DeleteAllAtributes()
* @brief      DeleteAllAtributes
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLELEMENT::DeleteAllAtributes()
{
  if(attributes.IsEmpty()) return false;

  attributes.DeleteContents();

  attributes.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING& XFILEXMLELEMENT::GetValue()
* @brief      GetValue
* @ingroup    XUTILS
*
* @return     XSTRING& :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING& XFILEXMLELEMENT::GetValue()
{
  return value;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLELEMENT::SetValue(XSTRING& value)
* @brief      SetValue
* @ingroup    XUTILS
*
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLELEMENT::SetValue(XSTRING& value)
{
  this->value = value;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLELEMENT::SetValue(XCHAR* value)
* @brief      SetValue
* @ingroup    XUTILS
*
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLELEMENT::SetValue(XCHAR* value)
{
  this->value = value;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT* XFILEXMLELEMENT::AddElement()
* @brief      AddElement
* @ingroup    XUTILS
*
* @return     XFILEXMLELEMENT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT* XFILEXMLELEMENT::AddElement()
{
  XFILEXMLELEMENT* element = new XFILEXMLELEMENT();
  if(!element) return NULL;

  element->SetFather(this);

  elements.Add(element);

  return element;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT* XFILEXMLELEMENT::AddElement(XSTRING& name)
* @brief      AddElement
* @ingroup    XUTILS
*
* @param[in]  name :
*
* @return     XFILEXMLELEMENT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT* XFILEXMLELEMENT::AddElement(XSTRING& name)
{
  XFILEXMLELEMENT* element = new XFILEXMLELEMENT(name);
  if(!element) return NULL;

  element->SetFather(this);

  elements.Add(element);

  return element;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT* XFILEXMLELEMENT::AddElement(XCHAR* name)
* @brief      AddElement
* @ingroup    XUTILS
*
* @param[in]  name :
*
* @return     XFILEXMLELEMENT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT* XFILEXMLELEMENT::AddElement(XCHAR* name)
{
  XFILEXMLELEMENT* element = new XFILEXMLELEMENT(name);
  if(!element) return NULL;

  element->SetFather(this);

  elements.Add(element);

  return element;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT* XFILEXMLELEMENT::AddElement(XSTRING& name,XSTRING& value)
* @brief      AddElement
* @ingroup    XUTILS
*
* @param[in]  name :
* @param[in]  value :
*
* @return     XFILEXMLELEMENT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT* XFILEXMLELEMENT::AddElement(XSTRING& name,XSTRING& value)
{
  XFILEXMLELEMENT* element = new XFILEXMLELEMENT(name,value);
  if(!element) return NULL;

  element->SetFather(this);

  elements.Add(element);

  return element;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT* XFILEXMLELEMENT::AddElement(XCHAR* name,XCHAR* value)
* @brief      AddElement
* @ingroup    XUTILS
*
* @param[in]  name :
* @param[in]  value :
*
* @return     XFILEXMLELEMENT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT* XFILEXMLELEMENT::AddElement(XCHAR* name,XCHAR* value)
{
  XFILEXMLELEMENT* element = new XFILEXMLELEMENT(name,value);
  if(!element) return NULL;

  element->SetFather(this);

  elements.Add(element);


  return element;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XFILEXMLELEMENT::GetNElements()
* @brief      GetNElements
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XFILEXMLELEMENT::GetNElements()
{
  return elements.GetSize();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT* XFILEXMLELEMENT::GetElement(int index)
* @brief      GetElement
* @ingroup    XUTILS
*
* @param[in]  index :
*
* @return     XFILEXMLELEMENT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT* XFILEXMLELEMENT::GetElement(int index)
{
  if(elements.IsEmpty()) return NULL;

  XFILEXMLELEMENT* element = (XFILEXMLELEMENT*)elements.Get(index);
  return element;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLELEMENT::DeleteElement(int index)
* @brief      DeleteElement
* @ingroup    XUTILS
*
* @param[in]  index :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLELEMENT::DeleteElement(int index)
{
  XFILEXMLELEMENT* element = (XFILEXMLELEMENT*)elements.Get(index);
  if(!element)  return false;

  elements.Delete(element);

  delete element;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLELEMENT::DeleteAllElements()
* @brief      DeleteAllElements
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLELEMENT::DeleteAllElements()
{
  if(elements.IsEmpty()) return false;

  elements.DeleteContents();
  elements.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT* XFILEXMLELEMENT::GetFather()
* @brief      GetFather
* @ingroup    XUTILS
*
* @return     XFILEXMLELEMENT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT* XFILEXMLELEMENT::GetFather()
{
  return father;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLELEMENT::SetFather(XFILEXMLELEMENT* father)
* @brief      SetFather
* @ingroup    XUTILS
*
* @param[in]  father :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLELEMENT::SetFather(XFILEXMLELEMENT* father)
{
  this->father = father;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENTTYPE XFILEXMLELEMENT::GetType()
* @brief      GetType
* @ingroup    XUTILS
*
* @return     XFILEXMLELEMENTTYPE :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENTTYPE XFILEXMLELEMENT::GetType()
{
  return type;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXMLELEMENT::SetType(XFILEXMLELEMENTTYPE type)
* @brief      SetType
* @ingroup    XUTILS
*
* @param[in]  type :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXMLELEMENT::SetType(XFILEXMLELEMENTTYPE type)
{
  this->type = type;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILEXMLELEMENT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
void XFILEXMLELEMENT::Clean()
{
  type   = XFILEXMLELEMENTTYPE_UNKNOWN;
  father = NULL;
  name   = __L("");
  value  = __L("");
}


#pragma endregion


#pragma region CLASS_XFILEXML


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXML::XFILEXML()
* @brief      Constructor
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXML::XFILEXML() : XFILETXT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXML::~XFILEXML()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXML::~XFILEXML()
{
  DeleteAllCFGAtributes();

  DeleteAllElements();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT* XFILEXML::GetRoot()
* @brief      GetRoot
* @ingroup    XUTILS
*
* @return     XFILEXMLELEMENT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT* XFILEXML::GetRoot()
{
  return root;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::SetRoot(XFILEXMLELEMENT* root)
* @brief      SetRoot
* @ingroup    XUTILS
*
* @param[in]  root :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::SetRoot(XFILEXMLELEMENT* root)
{
  if(!root) return false;

  this->root = root;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::EncodeAllLines(bool istabulatedline)
* @brief      EncodeAllLines
* @ingroup    XUTILS
*
* @param[in]  istabulatedline :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::EncodeAllLines(bool istabulatedline)
{
  DeleteAllLines();

  XSTRING line;

  EncodeConfig(line);
  AddLine(line);

  //XTRACE_PRINTCOLOR(1,line.Get());

  encodelevel  = -1;

  if(root) EncodeElements(istabulatedline,root);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::DecodeAllLines()
* @brief      DecodeAllLines
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::DecodeAllLines()
{
  DeleteAllElements();

  XSTRING   string;
  XSTRING*  stringml;
  bool      quote      =  false;
  bool      cdata      =  false;
  int       c          =  0;

  string.Empty();

  while(c<GetNLines())
    {
      stringml = GetLine(c);

      for(int d=0;d<(int)stringml->GetSize();d++)
        {
          XCHAR letter[2] = { 0, 0 };
          letter[0] = (*stringml)[d];

          if((*stringml)[d]==__C('\"')) quote=!quote;

          if((*stringml)[d]==__C('<')&&(!quote))
            {
              if(string.Find(__L("[CDATA["),false)!=XSTRING_NOTFOUND) cdata=true;

              if(!string.IsEmpty() && string.HaveCharacters() && (!cdata))
                {
                  DecodeLine(string,false);
                  string.Empty();
                }

              quote = false;
              string += letter;
            }
           else
            {
              if(((*stringml)[d]==__C('>'))&&(!quote))
                {
                  if(string.Find(__L("]]"),false)!=XSTRING_NOTFOUND) cdata=false;

                  string += letter;
                  if(!string.IsEmpty() && string.HaveCharacters() && (!cdata))
                    {
                      DecodeLine(string,true);
                      string.Empty();
                    }
                }
               else string += letter;
            }
        }

      c++;
    }

  DeleteAllLines();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::ReadAndDecodeAllLines()
* @brief      ReadAndDecodeAllLines
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::ReadAndDecodeAllLines()
{
  if(!IsOpen())      return false;

  if(!ReadAllFile()) return false;

  DecodeAllLines();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::WriteAndEncodeAllLines(bool istabulatedline,XFILETXTFORMATCHAR formatchar, XFILETXTTYPELF typeLF)
* @brief      WriteAndEncodeAllLines
* @ingroup    XUTILS
*
* @param[in]  istabulatedline :
* @param[in]  formatchar :
* @param[in]  typeLF :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::WriteAndEncodeAllLines(bool istabulatedline, XFILETXTFORMATCHAR formatchar, XFILETXTTYPELF  typeLF)
{
  SetFormatChar(formatchar);
  SetTypeLF(typeLF);

  return WriteAndEncodeAllLines(istabulatedline);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::WriteAndEncodeAllLines(bool istabulatedline)
* @brief      WriteAndEncodeAllLines
* @ingroup    XUTILS
*
* @param[in]  istabulatedline :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::WriteAndEncodeAllLines(bool istabulatedline)
{
  if(!IsOpen()) return false;

  file->SetPosition(0);

  EncodeAllLines(istabulatedline);

  bool status = WriteAllFile();

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT* XFILEXML::SearchChild(XCHAR* name, XFILEXMLELEMENT* father)
* @brief      SearchChild
* @ingroup    XUTILS
*
* @param[in]  name :
* @param[in]  father :
*
* @return     XFILEXMLELEMENT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT* XFILEXML::SearchChild(XCHAR* name, XFILEXMLELEMENT* father)
{
  XFILEXMLELEMENT* _father = NULL;
  XFILEXMLELEMENT* element = NULL;

  if(!_father) _father = root;
  if(!_father) return element;

  for(int c=0;c<_father->GetNElements();c++)
    {
      XFILEXMLELEMENT* element = _father->GetElement(c);
      if(element->GetName().Compare(name)==0)
        return element;
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT* XFILEXML::SearchElement(XSTRING& name, int& index, XFILEXMLELEMENT* father)
* @brief      SearchElement
* @ingroup    XUTILS
*
* @param[in]  name :
* @param[in]  index :
* @param[in]  father :
*
* @return     XFILEXMLELEMENT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT* XFILEXML::SearchElement(XSTRING& name, int& index, XFILEXMLELEMENT* father)
{
  return SearchElement(name.Get(), index, father);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT* XFILEXML::SearchElement(XCHAR* name, int& index, XFILEXMLELEMENT* father)
* @brief      SearchElement
* @ingroup    XUTILS
*
* @param[in]  name :
* @param[in]  index :
* @param[in]  father :
*
* @return     XFILEXMLELEMENT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT* XFILEXML::SearchElement(XCHAR* name, int& index, XFILEXMLELEMENT* father)
{
  XFILEXMLELEMENT* _father = father;
  XFILEXMLELEMENT* element = NULL;

  if(!_father) _father = root;
  if(!_father) return element;

  XSTRING nameelement(name);

  for(int c=index; c<_father->GetNElements(); c++)
    {
      XFILEXMLELEMENT* etemp = _father->GetElement(c);
      if(etemp)
        {
          if(etemp->GetName()==nameelement)
            {
              index = c;
              return etemp;
            }
           else
            {
              XFILEXMLELEMENT* etemp2 = SearchElement(name, index, etemp);
              if(etemp2) return etemp2;
            }
        }
    }

  return element;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT* XFILEXML::SearchElement(XCHAR* name, int& index, XSTRING& namefather)
* @brief      SearchElement
* @ingroup    XUTILS
*
* @param[in]  name :
* @param[in]  index :
* @param[in]  namefather :
*
* @return     XFILEXMLELEMENT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT* XFILEXML::SearchElement(XCHAR* name, int& index, XSTRING& namefather)
{
  return SearchElement(name, index, namefather.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEXMLELEMENT* XFILEXML::SearchElement(XCHAR* name, int& index, XCHAR* namefather)
* @brief      SearchElement
* @ingroup    XUTILS
*
* @param[in]  name :
* @param[in]  index :
* @param[in]  namefather :
*
* @return     XFILEXMLELEMENT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEXMLELEMENT* XFILEXML::SearchElement(XCHAR* name, int& index, XCHAR* namefather)
{
  int              _index  = 0;
  XFILEXMLELEMENT* father  = SearchElement(namefather, _index);
  if(!father) return NULL;

  return SearchElement(name, index, father);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::SearchAttributeValue(XFILEXMLELEMENT* element, XSTRING& nameattribute, XSTRING& value)
* @brief      SearchAttributeValue
* @ingroup    XUTILS
*
* @param[in]  element :
* @param[in]  nameattribute :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::SearchAttributeValue(XFILEXMLELEMENT* element, XSTRING& nameattribute, XSTRING& value)
{
  return SearchAttributeValue(element,nameattribute.Get(),value);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::SearchAttributeValue(XFILEXMLELEMENT* element,XCHAR* nameattribute,XSTRING& value)
* @brief      SearchAttributeValue
* @ingroup    XUTILS
*
* @param[in]  element :
* @param[in]  nameattribute :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::SearchAttributeValue(XFILEXMLELEMENT* element,XCHAR* nameattribute,XSTRING& value)
{
  if(!element)                   return false;
  if(!element->GetNAttributes()) return false;

  for(int c=0;c<element->GetNAttributes();c++)
    {
      XFILEXMLATTRIBUTE* attribute = element->GetAttribute(c);
      if(attribute)
        {
          XSTRING _nameattribute(nameattribute);
          if(attribute->GetName() == _nameattribute)
            {
               value = attribute->GetValue();
               return true;
            }
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<XFILEXMLATTRIBUTE*>* XFILEXML::GetCFGAtributes()
* @brief      GetCFGAtributes
* @ingroup    XUTILS
*
* @return     XVECTOR<XFILEXMLATTRIBUTE*>* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XFILEXMLATTRIBUTE*>* XFILEXML::GetCFGAtributes()
{
  return &cfgattributes;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::DeleteAllCFGAtributes()
* @brief      DeleteAllCFGAtributes
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::DeleteAllCFGAtributes()
{
  if(cfgattributes.IsEmpty()) return false;

  cfgattributes.DeleteContents();
  cfgattributes.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::DeleteAllElements()
* @brief      DeleteAllElements
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::DeleteAllElements()
{
  actualelement = NULL;

  if(root)
    {
      delete root;
      root = NULL;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XFILEXML::DecodeLine(XSTRING& string,bool iselement)
* @brief      DecodeLine
* @ingroup    XUTILS
*
* @param[in]  string :
* @param[in]  iselement :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XFILEXML::DecodeLine(XSTRING& string,bool iselement)
{
  XFILEXMLELEMENTTYPE     elementtype  = XFILEXMLELEMENTTYPE_UNKNOWN;
  XFILEXMLELEMENTTYPELINE typeline     = XFILEXMLELEMENTTYPELINE_UNKNOWN;

  if(iselement)
    {
      XSTRING str = string;

      str.DeleteNoCharacters(XSTRINGCONTEXT_ALLSTRING);

      int iini = str.FindCharacter(__C('<'));
      int iend = str.FindCharacter(__C('>'),0,true);

      if(iini!=XSTRING_NOTFOUND) iini++;
      if(iend!=XSTRING_NOTFOUND) iend--;

      switch(str[iini])
        {
          case __C('?') : if(str[iend]==__C('?')) elementtype = XFILEXMLELEMENTTYPE_CONFIG;
                          break;

          case __C('/') : elementtype = XFILEXMLELEMENTTYPE_NORMAL;
                          typeline    = XFILEXMLELEMENTTYPELINE_END;
                          break;

          case __C('!') : if((str[iini+1]==__C('-')) && (str[iini+2]==__C('-')) && (str[iend]  ==__C('-')) && (str[iend-1]==__C('-')))
                            elementtype = XFILEXMLELEMENTTYPE_COMMENT;
                          else
                          if(str.Find(__L("[CDATA["),false)!=XSTRING_NOTFOUND)
                            elementtype = XFILEXMLELEMENTTYPE_CDATA;
                          else
                          {
                            if((str[iini+1]==__C('-')) && (str[iini+2]==__C('-')))
                              is_multicomment=true;
                          }

                          break;

               default  : elementtype  = XFILEXMLELEMENTTYPE_NORMAL;
                          typeline     = XFILEXMLELEMENTTYPELINE_START;
                          if(str[iend]==__C('/'))  typeline    = XFILEXMLELEMENTTYPELINE_STARTEND;
                          break;
        }

      switch(elementtype)
        {
          case XFILEXMLELEMENTTYPE_UNKNOWN    : break;
          case XFILEXMLELEMENTTYPE_CONFIG   : DecodeConfig(string);             break;
          case XFILEXMLELEMENTTYPE_COMMENT  : DecodeComent(string);             break;
          case XFILEXMLELEMENTTYPE_CDATA    : DecodeCDATA(string);              break;
          case XFILEXMLELEMENTTYPE_NORMAL   : DecodeElement(string,typeline);   break;
        }
    }
   else
    {
      if(actualelement)
        {
          actualelement->SetValue(string);
        }
    }


  return 0;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::DecodeConfig(XSTRING& string)
* @brief      DecodeConfig
* @ingroup    XUTILS
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::DecodeConfig(XSTRING& string)
{
  XSTRING values;

  if(string.Copy(__L("<?xml"),__L("?>"),false,0,values)==XSTRING_NOTFOUND)  return false;

  XSTRING name;
  XSTRING value;

  int indexname = 0;

  while(1)
    {
      indexname = values.Copy(indexname, __L("="),false,name);
      if(indexname==XSTRING_NOTFOUND) break;

      name.DeleteNoCharacters(XSTRINGCONTEXT_ALLSTRING);

      indexname = values.Copy(__L("\""),__L("\""),false,indexname,value);
      if(indexname==XSTRING_NOTFOUND) break;

      XFILEXMLATTRIBUTE* attribute = new XFILEXMLATTRIBUTE(name,value);
      if(!attribute) break;

      cfgattributes.Add(attribute);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::DecodeComent(XSTRING& string)
* @brief      DecodeComent
* @ingroup    XUTILS
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::DecodeComent(XSTRING& string)
{
  if(!actualelement) return false;
  //if(ignorecomments) return true;

  XSTRING value;

  if(string.Copy(__L("<!--"),__L("-->"),false,0,value)==XSTRING_NOTFOUND)  return false;

  XFILEXMLELEMENT* element = actualelement->AddElement();
  if(!element) return false;

  element->SetType(XFILEXMLELEMENTTYPE_COMMENT);
  element->SetValue(value);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::DecodeCDATA(XSTRING& string)
* @brief      DecodeCDATA
* @ingroup    XUTILS
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::DecodeCDATA(XSTRING& string)
{
  if(!actualelement) return false;

  XSTRING value;

  if(string.Copy(__L("[CDATA["),__L("]]"),false,0,value)==XSTRING_NOTFOUND)  return false;

  XFILEXMLELEMENT* element = actualelement->AddElement();
  if(!element) return false;

  element->SetType(XFILEXMLELEMENTTYPE_CDATA);
  element->SetValue(value);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::DecodeElement(XSTRING& string, XFILEXMLELEMENTTYPELINE typeline)
* @brief      DecodeElement
* @ingroup    XUTILS
*
* @param[in]  string :
* @param[in]  typeline :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::DecodeElement(XSTRING& string, XFILEXMLELEMENTTYPELINE typeline)
{
  switch(typeline)
    {
      case XFILEXMLELEMENTTYPELINE_UNKNOWN    : return false;

      case XFILEXMLELEMENTTYPELINE_START    :
      case XFILEXMLELEMENTTYPELINE_STARTEND : { XSTRING values;
                                                XSTRING name;
                                                XSTRING value;

                                                if(string.Copy(__L("<"),__L(" "),false,0,name)==XSTRING_NOTFOUND) return false;

                                                //name.Set(string);
                                                name.DeleteCharacter(__C('<'),XSTRINGCONTEXT_ALLSTRING);
                                                name.DeleteCharacter(__C('>'),XSTRINGCONTEXT_ALLSTRING);

                                                name.DeleteNoCharacters();
                                                name.DeleteNoCharacters(__L("\20"),0, XSTRINGCONTEXT_FROM_FIRST);
                                                name.DeleteNoCharacters(__L("\20"),0, XSTRINGCONTEXT_TO_END);


                                                XFILEXMLELEMENT* element;

                                                if(!root)
                                                  {
                                                    element = new XFILEXMLELEMENT();
                                                    if(!element) return false;
                                                    root          = element;
                                                    actualelement = root;
                                                  }
                                                 else element = actualelement->AddElement();

                                                if(!element) return false;

                                                element->SetType(XFILEXMLELEMENTTYPE_NORMAL);
                                                element->SetName(name);

                                                if(string.Copy(name.Get(), (const XCHAR*)NULL, false, 0, values)!=XSTRING_NOTFOUND)
                                                  {
                                                    int indexname = 0;

                                                    while(1)
                                                      {
                                                        indexname = values.Copy(indexname, __L("="), false, name);
                                                        if(indexname==XSTRING_NOTFOUND) break;

                                                        name.DeleteNoCharacters(XSTRINGCONTEXT_ALLSTRING);

                                                        indexname = values.Copy(__L("\""),__L("\""),false,indexname,value);
                                                        if(indexname==XSTRING_NOTFOUND) break;

                                                        element->AddAtribute(name,value);

                                                        name.Empty();
                                                        value.Empty();

                                                      }
                                                  }

                                                if(typeline==XFILEXMLELEMENTTYPELINE_START) actualelement = element;

                                              }
                                              break;

      case XFILEXMLELEMENTTYPELINE_END      : if(actualelement->GetFather()) actualelement = actualelement->GetFather();
                                              break;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::AddTabs(int level,int spacetabs,XSTRING& string)
* @brief      AddTabs
* @ingroup    XUTILS
*
* @param[in]  level :
* @param[in]  spacetabs :
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::AddTabs(int level,int spacetabs,XSTRING& string)
{
  if(spacetabs<=0) return false;

  for(int c=0;c<level;c++)
    {
      for(int d=0;d<spacetabs;d++)
        {
          string += __L(" ");
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::EncodeConfig(XSTRING& string)
* @brief      EncodeConfig
* @ingroup    XUTILS
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::EncodeConfig(XSTRING& string)
{
  string.Empty();

  string += __L("<?xml ");

  for(int c=0;c<(int)cfgattributes.GetSize();c++)
    {
      XFILEXMLATTRIBUTE* attribute = (XFILEXMLATTRIBUTE*)cfgattributes.Get(c);
      if(!attribute) break;

      string+=attribute->GetName();
      string+=__L("=");
      string+=__L("\"");
      string+=attribute->GetValue();
      string+=__L("\" ");
    }

  string += __L("?>");

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEXML::EncodeElements(bool istabulatedline, XFILEXMLELEMENT* element)
* @brief      EncodeElements
* @ingroup    XUTILS
*
* @param[in]  istabulatedline :
* @param[in]  element :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILEXML::EncodeElements(bool istabulatedline, XFILEXMLELEMENT* element)
{
  if(!element) return false;

  XSTRING string;
  XSTRING value  = element->GetValue();
  int     c;

  string.Empty();
  if(istabulatedline)
    {
      encodelevel++;
      AddTabs(encodelevel,XFILEXML_SPACETABS,string);
    }

  switch(element->GetType())
    {
      case XFILEXMLELEMENTTYPE_UNKNOWN    : break;

      case XFILEXMLELEMENTTYPE_CONFIG   : break;

      case XFILEXMLELEMENTTYPE_COMMENT  : { string += __L("<!--");
                                            string += value;
                                            string += __L("-->");
                                            AddLine(string);
                                            //XTRACE_PRINTCOLOR(1,string.Get());
                                          }
                                          break;

      case XFILEXMLELEMENTTYPE_CDATA    : { string += __L("<![CDATA[");
                                            string += value;
                                            string += __L("]]>");
                                            AddLine(string);
                                            //XTRACE_PRINTCOLOR(1,string.Get());
                                          }
                                          break;

      case XFILEXMLELEMENTTYPE_NORMAL   : { bool isclose = false;

                                            string += __L("<");
                                            string += element->GetName();
                                            if(element->GetNAttributes()) string += __L(" ");

                                            for(c=0;c<(int)element->GetNAttributes();c++)
                                              {
                                                XFILEXMLATTRIBUTE* attribute = (XFILEXMLATTRIBUTE*)element->GetAttribute(c);
                                                if(!attribute) break;

                                                string+=attribute->GetName();
                                                string+=__L("=");
                                                string+=__L("\"");
                                                string+=attribute->GetValue();
                                                string+=__L("\"");
                                                if(c!=element->GetNAttributes()-1) string+=__L(" ");
                                              }

                                            if(!element->GetNElements() && value.IsEmpty())
                                              {
                                                string +=__L("/>");
                                                isclose = true;
                                              }
                                             else string +=__L(">");

                                            AddLine(string);
                                            //XTRACE_PRINTCOLOR(1,string.Get());

                                            if(!value.IsEmpty())
                                              {
                                                string.Empty();
                                                if(istabulatedline) AddTabs(encodelevel+1,XFILEXML_SPACETABS,string);
                                                string+=value;

                                                AddLine(string);
                                                //XTRACE_PRINTCOLOR(1,string.Get());
                                              }

                                            for(c=0;c<(int)element->GetNElements();c++)
                                              {
                                                XFILEXMLELEMENT* subelement = element->GetElement(c);
                                                if(subelement)  EncodeElements(istabulatedline,subelement);
                                              }

                                            if(!isclose)
                                              {
                                                string.Empty();

                                                if(istabulatedline) AddTabs(encodelevel,XFILEXML_SPACETABS,string);

                                                string += __L("</");
                                                string += element->GetName();
                                                string += __L(">");

                                                AddLine(string);
                                                //XTRACE_PRINTCOLOR(1,string.Get());
                                              }
                                          }
                                          break;


    }

  if(istabulatedline) encodelevel--;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILEXML::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
void XFILEXML::Clean()
{
  root          = NULL;
  actualelement = NULL;
  encodelevel   = -1;
  ignorecomments= false;
}


#pragma endregion



#pragma endregion




