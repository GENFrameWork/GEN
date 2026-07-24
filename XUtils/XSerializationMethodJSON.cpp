/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XSerializationMethodJSON.cpp
* 
* @class      XSERIALIZATIONMETHODJSON
* @brief      eXtended Utils Serializable method JSON class
* @ingroup    XUTILS
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

#include "XSerializationMethodJSON.h"

#include <stdio.h>
#include <string.h>



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSERIALIZATIONMETHODJSON::XSERIALIZATIONMETHODJSON()
* @brief      Constructor of class
* @ingroup    XUTILS
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSERIALIZATIONMETHODJSON::XSERIALIZATIONMETHODJSON()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSERIALIZATIONMETHODJSON::~XSERIALIZATIONMETHODJSON()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    XUTILS
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSERIALIZATIONMETHODJSON::~XSERIALIZATIONMETHODJSON()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Add(bool var, XCHAR* name)
* @brief      Add value
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Add(bool var, XCHAR* name)
{
  if(!fileJSON) 
    {
      return false;
    }

  XFILEJSON_ADDVALUE(GetActualObject(), name, (bool)var);
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Add(char var, XCHAR* name)
* @brief      Add value
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Add(char var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSON_ADDVALUE(GetActualObject(), name, (char)var);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Add(int var, XCHAR* name)
* @brief      Add value
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Add(int var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSON_ADDVALUE(GetActualObject(), name, (int)var);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Add(float var, XCHAR* name)
* @brief      Add value
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Add(float var, XCHAR* name)  
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSON_ADDVALUE(GetActualObject(), name, (float)var);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Add(double var, XCHAR* name)
* @brief      Add value
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Add(double var, XCHAR* name) 
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSON_ADDVALUE(GetActualObject(), name, (double)var);
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Add(long long var, XCHAR* name)
* @brief      Add value
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Add(long long var, XCHAR* name) 
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSON_ADDVALUE(GetActualObject(), name, (long long)var);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Add(XBYTE var, XCHAR* name)
* @brief      Add value
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Add(XBYTE var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSON_ADDVALUE(GetActualObject(), name, (XBYTE)var);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Add(XWORD var, XCHAR* name)
* @brief      Add value
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Add(XWORD var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSON_ADDVALUE(GetActualObject(), name, (XWORD)var);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Add(XDWORD var, XCHAR* name)
* @brief      Add value
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Add(XDWORD var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSON_ADDVALUE(GetActualObject(), name, (XDWORD)var);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Add(XQWORD var, XCHAR* name)
* @brief      Add value
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Add(XQWORD var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSON_ADDVALUE(GetActualObject(), name, (XQWORD)var);

  return true;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Add(XSTRING* var, XCHAR* name)
* @brief      Add value
* @ingroup    XUTILS
* 
* @param[in]  var : Var pointer to use.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Add(XSTRING* var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSON_ADDVALUE(GetActualObject(), name, (XCHAR*)var->Get());

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Add(XBUFFER* var, XCHAR* name)
* @brief      Add value
* @ingroup    XUTILS
* 
* @param[in]  var : Var pointer to use.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Add(XBUFFER* var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  //XFILEJSON_ADDVALUE(GetActualObject(), name, (XBYTE*)var.Get());

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Add(XVARIANT* var, XCHAR* name)
* @brief      Add value
* @ingroup    XUTILS
* 
* @param[in]  var : Var pointer to use.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Add(XVARIANT* var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSON_ADDVALUE(GetActualObject(), name, (XVARIANT*)(var));

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::AddStruct(XCHAR* name, bool open)
* @brief      Add struct
* @ingroup    XUTILS
* 
* @param[in]  name : Name to use.
* @param[in]  open : Open value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::AddStruct(XCHAR* name, bool open)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  if(open)
    {
      XFILEJSONOBJECT* structnode =  GEN_NEW XFILEJSONOBJECT();
      if(!structnode)
        {
          return false;
        }

      fathers.Add(GetActualObject());      
      SetActualObject(structnode);
    }
   else
    {
      fathers.GetLast()->Add(name, (XFILEJSONOBJECT*)GetActualObject());  
      SetActualObject(fathers.GetLast());      
      fathers.DeleteLast();
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::AddArray(XDWORD nelements, XCHAR* name, bool open)
* @brief      Add array
* @ingroup    XUTILS
* 
* @param[in]  nelements : Nelements value.
* @param[in]  name : Name to use.
* @param[in]  open : Open value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::AddArray(XDWORD nelements, XCHAR* name, bool open)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }  

  if(open)
    {
      XFILEJSONARRAY* arraynode =  GEN_NEW XFILEJSONARRAY();
      if(!arraynode)
        {
          return false;
        }

      fathers.Add(GetActualObject());      
      SetActualObject(arraynode);
    }
   else
    {
      fathers.GetLast()->Add(name, (XFILEJSONARRAY*)GetActualObject());  
      SetActualObject(fathers.GetLast());      
      fathers.DeleteLast();
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Extract(bool& var, XCHAR* name)
* @brief      Extract
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Extract(bool& var, XCHAR* name)
{ 
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSONVALUE* jsonvalue = fileJSON->GetValue(name, extractobject);
  if(jsonvalue)
    {
      var = jsonvalue->GetValueBoolean();
      return true;  
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Extract(char& var, XCHAR* name)
* @brief      Extract
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Extract(char& var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSONVALUE* jsonvalue = fileJSON->GetValue(name, extractobject);
  if(jsonvalue)
    {
      var = (char)jsonvalue->GetValueDWord();
      return true;  
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Extract(int& var, XCHAR* name)
* @brief      Extract
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Extract(int& var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSONVALUE* jsonvalue = fileJSON->GetValue(name, extractobject);
  if(jsonvalue)
    {
      var = jsonvalue->GetValueInteger();
      return true;  
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Extract(float& var, XCHAR* name)
* @brief      Extract
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Extract(float& var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSONVALUE* jsonvalue = fileJSON->GetValue(name, extractobject);
  if(jsonvalue)
    {
      var = jsonvalue->GetValueFloating();
      return true;  
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Extract(double& var, XCHAR* name)
* @brief      Extract
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Extract(double& var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSONVALUE* jsonvalue = fileJSON->GetValue(name, extractobject);
  if(jsonvalue)
    {
      var = jsonvalue->GetValueDoubleFloat();
      return true;  
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Extract(long& var, XCHAR* name)
* @brief      Extract
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Extract(long& var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSONVALUE* jsonvalue = fileJSON->GetValue(name, extractobject);
  if(jsonvalue)
    {
      var = jsonvalue->GetValueInteger();
      return true;  
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Extract(long long& var, XCHAR* name)
* @brief      Extract
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Extract(long long& var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSONVALUE* jsonvalue = fileJSON->GetValue(name, extractobject);
  if(jsonvalue)
    {
      var = jsonvalue->GetValueDoubleInteger();
      return true;  
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Extract(XBYTE& var, XCHAR* name)
* @brief      Extract
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Extract(XBYTE& var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSONVALUE* jsonvalue = fileJSON->GetValue(name, extractobject);
  if(jsonvalue)
    {
      var = (XBYTE)jsonvalue->GetValueDWord();
      return true;  
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Extract(XWORD& var, XCHAR* name)
* @brief      Extract
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Extract(XWORD& var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSONVALUE* jsonvalue = fileJSON->GetValue(name, extractobject);
  if(jsonvalue)
    {
      var = jsonvalue->GetValueWord();
      return true;  
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Extract(XDWORD& var, XCHAR* name)
* @brief      Extract
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Extract(XDWORD& var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSONVALUE* jsonvalue = fileJSON->GetValue(name, extractobject);
  if(jsonvalue)
    {
      var = jsonvalue->GetValueDWord();
      return true;  
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Extract(XQWORD& var, XCHAR* name)
* @brief      Extract
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Extract(XQWORD& var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSONVALUE* jsonvalue = fileJSON->GetValue(name, extractobject);
  if(jsonvalue)
    {
      var = jsonvalue->GetValueQWord();
      return true;  
    }

  return true;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Extract(XSTRING& var, XCHAR* name)
* @brief      Extract
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Extract(XSTRING& var, XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSONVALUE* jsonvalue = fileJSON->GetValue(name, extractobject);
  if(jsonvalue)
    {
      var = jsonvalue->GetValueString();
      return true;  
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Extract(XBUFFER& var, XCHAR* name)
* @brief      Extract
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Extract(XBUFFER& var, XCHAR* name)
{ 
  if(!CheckHandleActive()) 
    {
      return false;
    }

  XFILEJSONVALUE* jsonvalue = fileJSON->GetValue(name, extractobject);
  if(jsonvalue)
    {
      //var = jsonvalue->
      return true;  
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::Extract(XVARIANT& var, XCHAR* name)
* @brief      Extract
* @ingroup    XUTILS
* 
* @param[in]  var : Var value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::Extract(XVARIANT& var, XCHAR* name)
{ 
  if(!CheckHandleActive()) 
    {
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::ExtractStruct(XCHAR* name)
* @brief      Extract struct
* @ingroup    XUTILS
* 
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::ExtractStruct(XCHAR* name)
{
  if(!CheckHandleActive()) 
    {
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::ExtractArray(XDWORD nelements, XCHAR* name)
* @brief      Extract array
* @ingroup    XUTILS
* 
* @param[in]  nelements : Nelements value.
* @param[in]  name : Name to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::ExtractArray(XDWORD nelements, XCHAR* name)
{
  if(!CheckHandleActive())
    {
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSERIALIZATIONMETHODJSON::ExtractArrayElement(XDWORD index, XCHAR* name, bool open)
* @brief      Positions the extraction context (actualobject) on the index-th object of the named array
*             (open = true), or restores the previous context (open = false).
*             (FIX: without this, every element of an array was deserialized searching from the JSON root,
*             so all the entries of a XVECTOR ended up with the data of the first element).
* @ingroup    XUTILS
*
* @param[in]  index : Index of the element inside the array.
* @param[in]  name : Name of the array.
* @param[in]  open : true to enter the element; false to leave it.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::ExtractArrayElement(XDWORD index, XCHAR* name, bool open)
{
  if(!fileJSON)
    {
      return false;
    }

  if(open)
    {
      // NOTE: XFILEJSON::GetValue() never compares the name of OBJECT/ARRAY values (it only recurses into them),
      // so arrays cannot be located with it: GetContainerValue() is used instead.
      XFILEJSONVALUE* jsonvalue = GetContainerValue(name, extractobject);
      if(!jsonvalue)
        {
          return false;
        }

      if(jsonvalue->GetType() != XFILEJSONVALUETYPE_ARRAY)
        {
          return false;
        }

      XFILEJSONARRAY* array = jsonvalue->GetValueArray();
      if(!array || !array->GetValues())
        {
          return false;
        }

      XFILEJSONVALUE* elementvalue = (XFILEJSONVALUE*)array->GetValues()->Get(index);
      if(!elementvalue)
        {
          return false;
        }

      if(elementvalue->GetType() != XFILEJSONVALUETYPE_OBJECT)
        {
          return false;
        }

      XFILEJSONOBJECT* elementobject = elementvalue->GetValueObject();
      if(!elementobject)
        {
          return false;
        }

      extractfathers.Add(extractobject);
      extractobject = elementobject;
    }
   else
    {
      if(!extractfathers.GetSize())
        {
          return false;
        }

      extractobject = extractfathers.GetLast();
      extractfathers.DeleteLast();
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEJSONVALUE* XSERIALIZATIONMETHODJSON::GetContainerValue(XCHAR* name, XFILEJSONOBJECT* startobject)
* @brief      Search a container value (OBJECT or ARRAY) by name. XFILEJSON::GetValue() cannot be used for this
*             because it never compares the name of container values, only the name of primitive values.
*             Direct children are searched first; if not found, the search descends recursively.
* @ingroup    XUTILS
*
* @param[in]  name : Name of the container to search.
* @param[in]  startobject : Start object of the search (NULL = root of the JSON).
*
* @return     XFILEJSONVALUE* : Pointer to the requested value; NULL if it is not available.
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILEJSONVALUE* XSERIALIZATIONMETHODJSON::GetContainerValue(XCHAR* name, XFILEJSONOBJECT* startobject)
{
  if(!fileJSON)
    {
      return NULL;
    }

  XFILEJSONOBJECT* object = startobject?startobject:fileJSON->GetRoot();
  if(!object)
    {
      return NULL;
    }

  if(!object->GetValues())
    {
      return NULL;
    }

  // First pass: direct children.
  for(XDWORD c=0; c<object->GetValues()->GetSize(); c++)
    {
      XFILEJSONVALUE* value = (XFILEJSONVALUE*)object->GetValues()->Get(c);
      if(value)
        {
          XFILEJSONVALUETYPE type = value->GetType();
          if((type == XFILEJSONVALUETYPE_OBJECT) || (type == XFILEJSONVALUETYPE_ARRAY))
            {
              if(value->GetName())
                {
                  if(!value->GetName()->Compare(name))
                    {
                      return value;
                    }
                }
            }
        }
    }

  // Second pass: descend recursively.
  for(XDWORD c=0; c<object->GetValues()->GetSize(); c++)
    {
      XFILEJSONVALUE* value = (XFILEJSONVALUE*)object->GetValues()->Get(c);
      if(value)
        {
          XFILEJSONVALUETYPE type = value->GetType();
          if((type == XFILEJSONVALUETYPE_OBJECT) || (type == XFILEJSONVALUETYPE_ARRAY))
            {
              XFILEJSONVALUE* found = GetContainerValue(name, (XFILEJSONOBJECT*)value->GetValuePointer());
              if(found)
                {
                  return found;
                }
            }
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEJSON* XSERIALIZATIONMETHODJSON::GetFileJSON()
* @brief      Get file JSON
* @ingroup    XUTILS
* 
* @return     XFILEJSON* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XFILEJSON* XSERIALIZATIONMETHODJSON::GetFileJSON()
{
  return fileJSON;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XSERIALIZATIONMETHODJSON::SetFileJSON(XFILEJSON* fileJSON)
* @brief      Set file JSON
* @ingroup    XUTILS
* 
* @param[in]  fileJSON : File JSON pointer to use.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XSERIALIZATIONMETHODJSON::SetFileJSON(XFILEJSON* fileJSON)
{
  this->fileJSON = fileJSON;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEJSONOBJECT* XSERIALIZATIONMETHODJSON::GetActualObject()
* @brief      Get actual object
* @ingroup    XUTILS
* 
* @return     XFILEJSONOBJECT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XFILEJSONOBJECT* XSERIALIZATIONMETHODJSON::GetActualObject()
{
  return actualobject;
}
    
    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XSERIALIZATIONMETHODJSON::SetActualObject(XFILEJSONOBJECT* actualobject)
* @brief      Set actual object
* @ingroup    XUTILS
* 
* @param[in]  actualobject : Actualobject pointer to use.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XSERIALIZATIONMETHODJSON::SetActualObject(XFILEJSONOBJECT* actualobject)
{
  this->actualobject = actualobject;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZATIONMETHODJSON::CheckHandleActive()
* @brief      Check handle active
* @ingroup    XUTILS
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZATIONMETHODJSON::CheckHandleActive()
{
  if(!fileJSON)
    {
      return false;
    }

  if(!actualobject)
    {
      return false;    
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XSERIALIZATIONMETHODJSON::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    XUTILS
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XSERIALIZATIONMETHODJSON::Clean()
{
  fileJSON      = NULL;

  fathers.DeleteAll();
  extractfathers.DeleteAll();

  actualobject  = NULL;
  extractobject = NULL;
}



