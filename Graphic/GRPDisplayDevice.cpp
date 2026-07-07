/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPDisplayDevice.cpp
* 
* @class      GRPDISPLAYDEVICE
* @brief      Graphics Display Device (screen class for display devices)
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

#include "GRPDisplayDevice.h"

#include "GRPXEvent.h"
#include "GRP2DCanvas.h"
#include "GRPViewPort.h"


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPDISPLAYDEVICE::GRPDISPLAYDEVICE()
* @brief      Constructor of class
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPDISPLAYDEVICE::GRPDISPLAYDEVICE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPDISPLAYDEVICE::~GRPDISPLAYDEVICE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPDISPLAYDEVICE::~GRPDISPLAYDEVICE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPDISPLAYDEVICE::Create(bool show)
* @brief      Create resource
* @ingroup    GRAPHIC
* 
* @param[in]  show : Show value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPDISPLAYDEVICE::Create(bool show)
{

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPDISPLAYDEVICE::Update()
* @brief      Update
* @ingroup    GRAPHIC
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPDISPLAYDEVICE::Update()
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPDISPLAYDEVICE::Update(GRP2DCANVAS* canvas)
* @brief      Update
* @ingroup    GRAPHIC
* 
* @param[in]  canvas : Canvas object to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPDISPLAYDEVICE::Update(GRP2DCANVAS* canvas)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPDISPLAYDEVICE::Delete()
* @brief      Delete resource
* @ingroup    GRAPHIC
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPDISPLAYDEVICE::Delete()
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPDISPLAYDEVICE::Resize(int width, int height)
* @brief      Resize
* @ingroup    GRAPHIC
* 
* @param[in]  width : Width value.
* @param[in]  height : Height value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPDISPLAYDEVICE::Resize(int width, int height)
{
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPDISPLAYDEVICE::IsShow()
* @brief      Is show
* @ingroup    GRAPHIC
* 
* @return     bool : true if the condition is met; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPDISPLAYDEVICE::IsShow()
{
  return isshow;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPDISPLAYDEVICE::Show(bool active)
* @brief      Show
* @ingroup    GRAPHIC
* 
* @param[in]  active : Active value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPDISPLAYDEVICE::Show(bool active)
{
  isshow  = active;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPDISPLAYDEVICE::ShowCursor(bool active)
* @brief      Show cursor
* @ingroup    GRAPHIC
* 
* @param[in]  active : Active value.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPDISPLAYDEVICE::ShowCursor(bool active)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void* GRPDISPLAYDEVICE::GetHandle()
* @brief      Get handle
* @ingroup    GRAPHIC
* 
* @return     void* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void* GRPDISPLAYDEVICE::GetHandle()
{
  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPDISPLAYDEVICE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPDISPLAYDEVICE::Clean()
{

}










