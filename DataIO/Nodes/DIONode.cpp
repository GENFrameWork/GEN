/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIONode.cpp
* 
* @class      DIONODE
* @brief      Data Input/Output NODE (Base class for a control of nodes)
* @ingroup    DATAIO
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "DIONodeElement.h"

#include "DIONode.h"

#include "XMemory_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIONODE::DIONODE()
* @brief      Constructor
* @ingroup    
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIONODE::DIONODE()
{
  Clean();
  
  UUID.GenerateRandom();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIONODE::~DIONODE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIONODE::~DIONODE()
{
  elements.DeleteContents();
  elements.DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XUUID& DIONODE::GetID()
* @brief      GetID
* @ingroup    DATAIO
* 
* @return     XUUID& : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XUUID& DIONODE::GetID()
{
  return UUID;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIONODE::SetID(XUUID& UUID)
* @brief      SetID
* @ingroup    DATAIO
* 
* @param[in]  UUID : 
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIONODE::SetID(XUUID& UUID)
{
  this->UUID.CopyFrom(UUID);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<DIONODEELEMENT*>* DIONODE::GetElements()
* @brief      GetElements
* @ingroup    DATAIO
* 
* @return     XVECTOR<DIONODEELEMENT*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<DIONODEELEMENT*>* DIONODE::GetElements()
{
  return &elements;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIONODE::Serialize()
* @brief      Serialize
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIONODE::Serialize()
{  
  XSTRING ID;
 
  UUID.GetToString(ID); 

  Primitive_Add<XSTRING>(ID, __L("ID"));
  XVectorClass_Add<DIONODEELEMENT>(&elements, __L("elements"), __L("element"));

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIONODE::Deserialize()
* @brief      Deserialize
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIONODE::Deserialize()
{
  XSTRING ID;

  Primitive_Extract<XSTRING>(ID, __L("ID"));
  XVectorClass_Extract<DIONODEELEMENT>(&elements, __L("elements"), __L("element"));

  UUID.SetFromString(ID);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIONODE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIONODE::Clean()
{

}