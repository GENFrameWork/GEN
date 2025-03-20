/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIONotificationsManager.cpp
* 
* @class      DIONOTIFICATIONSMANAGER
* @brief      Data Input/Output Notifications Manager class
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIONotificationsManager.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


#pragma region CLASS_DIONOTIFICATION


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIONOTIFICATION::DIONOTIFICATION()
* @brief      Constructor of class
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIONOTIFICATION::DIONOTIFICATION()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIONOTIFICATION::~DIONOTIFICATION()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIONOTIFICATION::~DIONOTIFICATION()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIONOTIFICATION::Get_Title()
* @brief      Get title
* @ingroup    DATAIO
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIONOTIFICATION::Get_Title()
{
  return &title;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIONOTIFICATION::Get_Message()
* @brief      Get message
* @ingroup    DATAIO
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIONOTIFICATION::Get_Message()
{
  return &message;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void* DIONOTIFICATION::Application_GetHandle()
* @brief      Application get handle
* @ingroup    DATAIO
*
* @return     void* :
*
* --------------------------------------------------------------------------------------------------------------------*/
void* DIONOTIFICATION::Application_GetHandle()
{
  return applicationhandle;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIONOTIFICATION::Application_SetHandle(void* applicationhandle)
* @brief      Application set handle
* @ingroup    DATAIO
*
* @param[in]  applicationhandle :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIONOTIFICATION::Application_SetHandle(void* applicationhandle)
{
  this->applicationhandle = applicationhandle;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void* DIONOTIFICATION::GetApplicationIcon()
* @brief      Get application icon
* @ingroup    DATAIO
*
* @return     void* :
*
* --------------------------------------------------------------------------------------------------------------------*/
void* DIONOTIFICATION::GetApplicationIcon()
{
  return applicationicon;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIONOTIFICATION::SetApplicationIcon(void* applicationicon)
* @brief      Set application icon
* @ingroup    DATAIO
*
* @param[in]  applicationicon :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIONOTIFICATION::SetApplicationIcon(void* applicationicon)
{
  this->applicationicon = applicationicon;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIONOTIFICATION::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIONOTIFICATION::Clean()
{
  applicationhandle = NULL;
  applicationicon   = NULL;
}


#pragma endregion


#pragma region CLASS_DIONOTIFICATIONMANAGER 


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIONOTIFICATIONSMANAGER::DIONOTIFICATIONSMANAGER()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIONOTIFICATIONSMANAGER::DIONOTIFICATIONSMANAGER()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIONOTIFICATIONSMANAGER::~DIONOTIFICATIONSMANAGER()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIONOTIFICATIONSMANAGER::~DIONOTIFICATIONSMANAGER()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIONOTIFICATIONSMANAGER::Ini(XCHAR* ownertitle, XCHAR* appname)
* @brief      ini
* @ingroup    DATAIO
* 
* @param[in]  ownertitle : 
* @param[in]  appname : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIONOTIFICATIONSMANAGER::Ini(XCHAR* ownertitle, XCHAR* appname)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* DIONOTIFICATIONSMANAGER::GetOwnerTitle()
* @brief      get owner title
* @ingroup    DATAIO
* 
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIONOTIFICATIONSMANAGER::GetOwnerTitle()
{
  return &ownertitle;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* DIONOTIFICATIONSMANAGER::GetAppName()
* @brief      get app name
* @ingroup    DATAIO
* 
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIONOTIFICATIONSMANAGER::GetAppName()
{
  return &appname;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIONOTIFICATIONSMANAGER::Do(DIONOTIFICATION* notification)
* @brief      Do
* @ingroup    DATAIO
*
* @param[in]  notification :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIONOTIFICATIONSMANAGER::Do(DIONOTIFICATION* notification)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIONOTIFICATIONSMANAGER::Do(DIONOTIFICATION& notification)
* @brief      Do
* @ingroup    DATAIO
*
* @param[in]  notification :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIONOTIFICATIONSMANAGER::Do(DIONOTIFICATION& notification)
{
  return Do(&notification);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIONOTIFICATIONSMANAGER::End()
* @brief      End
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIONOTIFICATIONSMANAGER::End()
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIONOTIFICATIONSMANAGER::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIONOTIFICATIONSMANAGER::Clean()
{

}


#pragma endregion


#pragma endregion

