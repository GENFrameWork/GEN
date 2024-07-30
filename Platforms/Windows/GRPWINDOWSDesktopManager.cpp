/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPWINDOWSDesktopManager.cpp
* 
* @class      GRPWINDOWSDESKTOPMANAGER
* @brief      WINDOWS Graphics Desktop Manager class
* @ingroup    PLATFORM_WINDOWS
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

#include "GRPWINDOWSDesktopManager.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS

#pragma region CLASS_GRPWINDOWSDESKTOPMONITORS

   
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPWINDOWSDESKTOPMONITORS::GRPWINDOWSDESKTOPMONITORS()
* @brief      Constructor
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPWINDOWSDESKTOPMONITORS::GRPWINDOWSDESKTOPMONITORS() : GRPDESKTOPMONITORS()
{
  Clean();

  EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPWINDOWSDESKTOPMONITORS::~GRPWINDOWSDESKTOPMONITORS()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPWINDOWSDESKTOPMONITORS::~GRPWINDOWSDESKTOPMONITORS()
{
  monitorsrects.DeleteContents();
  monitorsrects.DeleteAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         static BOOL CALLBACK GRPWINDOWSDESKTOPMONITORS::MonitorEnum(HMONITOR hmon,HDC hdc,LPRECT rectmonitor,LPARAM pdata)
* @brief      MonitorEnum
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  hmon : 
* @param[in]  hdc : 
* @param[in]  rectmonitor : 
* @param[in]  pdata : 
* 
* @return     static : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
BOOL CALLBACK GRPWINDOWSDESKTOPMONITORS::MonitorEnum(HMONITOR hmon,HDC hdc,LPRECT rectmonitor,LPARAM pdata)
{
  GRPWINDOWSDESKTOPMONITORS* deskmonitors = (GRPWINDOWSDESKTOPMONITORS*)pdata;
  if(!deskmonitors)
    {
      return FALSE;  
    }

  GRPRECTINT* newmonitor = new GRPRECTINT();
  if(newmonitor)
    {
      newmonitor->x1  = rectmonitor->left;
      newmonitor->x2  = rectmonitor->right;
      newmonitor->y1  = rectmonitor->top;
      newmonitor->y2  = rectmonitor->bottom;

      deskmonitors->GetMonitorsRects()->Add(newmonitor);
    }

  GRPRECTINT* allmonitor = deskmonitors->GetCombinedRect();
  if(!allmonitor)
    {
      return false;
    }

  GRPRECTINT allmonitortempo =  UniteRectangles((*allmonitor), (*newmonitor));

  allmonitor->CopyFrom(&allmonitortempo);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPWINDOWSDESKTOPMONITORS::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPWINDOWSDESKTOPMONITORS::Clean()
{

}


#pragma endregion


#pragma region CLASS_GRPWINDOWSDESKTOPMANAGER


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPWINDOWSDESKTOPMANAGER::GRPWINDOWSDESKTOPMANAGER()
* @brief      Constructor
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPWINDOWSDESKTOPMANAGER::GRPWINDOWSDESKTOPMANAGER()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPWINDOWSDESKTOPMANAGER::~GRPWINDOWSDESKTOPMANAGER()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPWINDOWSDESKTOPMANAGER::~GRPWINDOWSDESKTOPMANAGER()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPWINDOWSDESKTOPMONITORS* GRPWINDOWSDESKTOPMANAGER::GetDesktopMonitors()
* @brief      GetDesktopMonitors
* @ingroup    PLATFORM_WINDOWS
* 
* @return     XDESKTOPMONITORS* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPDESKTOPMONITORS* GRPWINDOWSDESKTOPMANAGER::GetDesktopMonitors()
{
  return (GRPDESKTOPMONITORS*)&desktopmonitors;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPWINDOWSDESKTOPMANAGER::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPWINDOWSDESKTOPMANAGER::Clean()
{

}


#pragma endregion


#pragma endregion


