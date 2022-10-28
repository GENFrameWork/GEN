/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XSerializable.cpp
* 
* @class      XSERIALIZABLE
* @brief      eXtended Serializable base class
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XSerializableBinary.h"

#include "XSerializable.h"


#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSERIALIZATIONMETHOD::XSERIALIZATIONMETHOD()
* @brief      Constructor
* @ingroup    XUTILS
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSERIALIZATIONMETHOD::XSERIALIZATIONMETHOD()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSERIALIZATIONMETHOD::~XSERIALIZATIONMETHOD()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSERIALIZATIONMETHOD::~XSERIALIZATIONMETHOD()
{
  Clean();
}

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XSERIALIZATIONMETHOD::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XSERIALIZATIONMETHOD::Clean()
{

}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSERIALIZABLE::XSERIALIZABLE()
* @brief      Constructor
* @ingroup    XUTILS
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSERIALIZABLE::XSERIALIZABLE()
{
  Clean();
}
    
    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSERIALIZABLE::~XSERIALIZABLE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSERIALIZABLE::~XSERIALIZABLE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSERIALIZATIONMETHOD* XSERIALIZATIONMETHOD::CreateInstance(XBUFFER& databinary)
* @brief      CreateInstance
* @ingroup    XUTILS
* 
* @param[in]  databinary : 
* 
* @return     XSERIALIZATIONMETHOD* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSERIALIZATIONMETHOD* XSERIALIZABLE::CreateBinary(XBUFFER& databinary)
{
  XSERIALIZATIONMETHODBINARY* serializable  = new XSERIALIZATIONMETHODBINARY();
  if(serializable)
    {
      serializable->SetBufferData(&databinary);
    }

  return (XSERIALIZATIONMETHOD*)serializable;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSERIALIZATIONMETHOD* XSERIALIZABLE::GetSerialization()
* @brief      GetSerialization
* @ingroup    XUTILS
* 
* @return     XSERIALIZATIONMETHOD* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSERIALIZATIONMETHOD* XSERIALIZABLE::GetSerialization()
{
  return serialization; 
}
    
    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XSERIALIZABLE::SetSerialization(XSERIALIZATIONMETHOD* serialization)
* @brief      SetSerialization
* @ingroup    XUTILS
* 
* @param[in]  serialization : 
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XSERIALIZABLE::SetSerialization(XSERIALIZATIONMETHOD* serialization)
{
  this->serialization = serialization;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZABLE::InitSerialize(XSERIALIZATIONMETHOD* serialization)
* @brief      InitSerialize
* @ingroup    XUTILS
* 
* @param[in]  serialization : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZABLE::InitSerialize(XSERIALIZATIONMETHOD* serialization)
{
  SetSerialization(serialization);

  return Serialize();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZABLE::InitDeserialize(XSERIALIZATIONMETHOD* serialization)
* @brief      InitDeserialize
* @ingroup    XUTILS
* 
* @param[in]  serialization : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZABLE::InitDeserialize(XSERIALIZATIONMETHOD* serialization)
{
  SetSerialization(serialization);

  return Deserialize();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn          bool XSERIALIZATIONMETHOD::Serialize()
* @brief       Serialize
* @ingroup     XUTILS
* 
* @return      bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZABLE::Serialize()
{
  if(!serialization) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSERIALIZABLE::Deserialize()
* @brief      Deserialize
* @ingroup    XUTILS
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSERIALIZABLE::Deserialize()
{
  if(!serialization) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XSERIALIZABLE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XSERIALIZABLE::Clean()
{
  serialization = NULL;
}