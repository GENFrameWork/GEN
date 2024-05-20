/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOOSPipeline.cpp
* 
* @class      DIOOSPIPELINE
* @brief      Data Input/Output Operative System Pipeline in/out class
* @note       Can�t be construct Factory + singelton without depends of system. IT�S NOT A SINGLETON.
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
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOOSPipeline.h"

#include "XFactory.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

DIOOSPIPELINE* DIOOSPIPELINE::instance  = NULL;

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOOSPIPELINE::DIOOSPIPELINE()
* @brief      Constructor
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOOSPIPELINE::DIOOSPIPELINE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOOSPIPELINE::~DIOOSPIPELINE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOOSPIPELINE::~DIOOSPIPELINE()
{
  Close();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOOSPIPELINE::GetIsInstanced()
* @brief      GetIsInstanced
* @note       STATIC
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOOSPIPELINE::GetIsInstanced()
{
  return instance!=NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOOSPIPELINE& DIOOSPIPELINE::GetInstance()
* @brief      GetInstance
* @note       STATIC
* @ingroup    XUTILS
*
* @return     DIOOSPIPELINE& :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOOSPIPELINE& DIOOSPIPELINE::GetInstance()
{
  if(!instance) instance = new DIOOSPIPELINE();

  return (*instance);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOOSPIPELINE::SetInstance(DIOOSPIPELINE* _instance)
* @brief      SetInstance
* @note       STATIC
* @ingroup    XUTILS
*
* @param[in]  _instance : new instance
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOOSPIPELINE::SetInstance(DIOOSPIPELINE* _instance)
{
  if(!_instance) return false;

  instance = _instance;

  return (instance)?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOOSPIPELINE::DelInstance()
* @brief      DelInstance
* @note       STATIC
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOOSPIPELINE::DelInstance()
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
* @fn         bool DIOOSPIPELINE::Open()
* @brief      Open
* @ingroup    XUTILS
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOOSPIPELINE::Open()
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOOSPIPELINE::IsOpen()
* @brief      IsOpen
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOOSPIPELINE::IsOpen()
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOOSPIPELINE::Read(XBUFFER& buffer)
* @brief      Read
* @ingroup    XUTILS
* 
* @param[in]  buffer : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOOSPIPELINE::Read(XBUFFER& buffer)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOOSPIPELINE::Write(XBUFFER& buffer)
* @brief      Write
* @ingroup    XUTILS
* 
* @param[in]  buffer : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOOSPIPELINE::Write(XBUFFER& buffer)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOOSPIPELINE::Close()
* @brief      Close
* @ingroup    XUTILS
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOOSPIPELINE::Close()
{
  return false;
}    


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOOSPIPELINE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOOSPIPELINE::Clean()
{

}


#pragma endregion

