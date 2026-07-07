/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPVectorFileDXFEntity.cpp
* 
* @class      GRPVECTORFILEDXFENTITY
* @brief      Graphic Vector File DXF Entity class
* @ingroup    GRAPHIC
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

#include "GRPVectorFileDXFEntity.h"

#include "XMap.h"
#include "XVariant.h"

#include "GRPVectorFileDXF.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFENTITY::GRPVECTORFILEDXFENTITY()
* @brief      Constructor of class
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFENTITY::GRPVECTORFILEDXFENTITY()
{
  Clean();   
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFENTITY::~GRPVECTORFILEDXFENTITY()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFENTITY::~GRPVECTORFILEDXFENTITY()
{
  DeleteAllValues();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* GRPVECTORFILEDXFENTITY::GetName()
* @brief      Get name
* @ingroup    GRAPHIC
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* GRPVECTORFILEDXFENTITY::GetName()
{
  return &name;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPVECTORFILEDXFENTITY::AddValue(GRPVECTORFILEDXFVALUE* value)
* @brief      Add value
* @ingroup    GRAPHIC
* 
* @param[in]  value : Value pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILEDXFENTITY::AddValue(GRPVECTORFILEDXFVALUE* value)
{    
  values.Add(value);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<GRPVECTORFILEDXFVALUE*>* GRPVECTORFILEDXFENTITY::GetValues()
* @brief      Get values
* @ingroup    GRAPHIC
* 
* @return     XVECTOR<GRPVECTORFILEDXFVALUE*>* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<GRPVECTORFILEDXFVALUE*>* GRPVECTORFILEDXFENTITY::GetValues()
{
  return &values;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFVALUE* GRPVECTORFILEDXFENTITY::GetValue(int index)
* @brief      Get value
* @ingroup    GRAPHIC
* 
* @param[in]  index : Index value.
* 
* @return     GRPVECTORFILEDXFVALUE* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFVALUE* GRPVECTORFILEDXFENTITY::GetValue(int index)
{
  if(values.IsEmpty()) 
    {
      return NULL;
    }

  return values.Get(index);   
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPVECTORFILEDXFENTITY::DeleteValue(int index)
* @brief      Delete value
* @ingroup    GRAPHIC
* 
* @param[in]  index : Index value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILEDXFENTITY::DeleteValue(int index)
{
  if(values.IsEmpty()) 
    {
      return false;
    }

  return values.Delete(GetValue(index));
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPVECTORFILEDXFENTITY::DeleteAllValues()
* @brief      Delete all values
* @ingroup    GRAPHIC
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILEDXFENTITY::DeleteAllValues()
{
  if(values.IsEmpty()) 
    {
      return false;
    }

  values.DeleteContents();
  values.DeleteAll(); 
   
  xdatactrllist.DeleteContents();
  xdatactrllist.DeleteAll();

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<GRPVECTORFILEDXFXDATACTRL*>* GRPVECTORFILEDXFENTITY::GetXDataCtrlList()
* @brief      Get X data ctrl list
* @ingroup    GRAPHIC
* 
* @return     XVECTOR<GRPVECTORFILEDXFXDATACTRL*>* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<GRPVECTORFILEDXFXDATACTRL*>* GRPVECTORFILEDXFENTITY::GetXDataCtrlList()
{
  return &xdatactrllist;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPVECTORFILEDXFENTITY::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILEDXFENTITY::Clean()
{
  
}



