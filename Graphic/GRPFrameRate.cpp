/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPFrameRate.cpp
* 
* @class      GRPFRAMERATE
* @brief      Graphic Frame Rate class
* @ingroup    GRAPHIC
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

#include "GRPFrameRate.h"

#include "XFactory.h"
#include "XTimer.h"
#include "XThreadCollected.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPFRAMERATE::GRPFRAMERATE()
* @brief      Constructor
* @ingroup    GRAPHIC
*
* ---------------------------------------------------------------------------------------------------------------------*/
GRPFRAMERATE::GRPFRAMERATE()    
{ 
  Clean(); 

  GEN_XFACTORY_CREATE(xtimertotal, CreateTimer())
    
  thread_framerate = CREATEXTHREAD((XTHREADGROUPID)(XTHREADGROUPID_GRPFRAMERATE), __L("GRPFRAMERATE::GRPFRAMERATE"), ThreadRunFunction_Framerate, this);    
  if(!thread_framerate)  return;

  if(!thread_framerate->Ini()) return;                      
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPFRAMERATE::~GRPFRAMERATE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    GRAPHIC
*
* ---------------------------------------------------------------------------------------------------------------------*/
GRPFRAMERATE::~GRPFRAMERATE()    
{ 
  if(thread_framerate)
    {
      thread_framerate->End();

      DELETEXTHREAD((XTHREADGROUPID)(XTHREADGROUPID_GRPFRAMERATE), thread_framerate);
      thread_framerate = NULL;  
    }

  if(xtimertotal) GEN_XFACTORY.DeleteTimer(xtimertotal);

  Clean();                       
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XTIMER* GRPFRAMERATE::GetXTimer()
* @brief      GetXTimer
* @ingroup    GRAPHIC
* 
* @return     XTIMER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XTIMER* GRPFRAMERATE::GetXTimer()
{
  return xtimertotal;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPFRAMERATE::AddNFrames()
* @brief      AddNFrames
* @ingroup    GRAPHIC
*
* ---------------------------------------------------------------------------------------------------------------------*/
void GRPFRAMERATE::AddNFrames()
{
  nframes++;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD GRPFRAMERATE::GetNFrames()
* @brief      GetNFrames
* @ingroup    GRAPHIC
*
* @return     XDWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XDWORD GRPFRAMERATE::GetNFrames()
{
  return nframes;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPFRAMERATE::Reset()
* @brief      Reset
* @ingroup    GRAPHIC
*
* ---------------------------------------------------------------------------------------------------------------------*/
void GRPFRAMERATE::Reset()
{  
  if(xtimertotal)  xtimertotal->Reset();

  nframes           = 0; 
  ngets             = 0;
  
  actual_framerate  = 0.0f;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         float GRPFRAMERATE::Get()
* @brief      Get
* @ingroup    GRAPHIC
*
* @return     float : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
float GRPFRAMERATE::Get()
{  
  ngets++;

  return actual_framerate;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPFRAMERATE::ThreadRunFunction_Framerate(void* param)
* @brief      ThreadRunFunction_Framerate
* @ingroup    GRAPHIC
*
* @param[in]  param : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void GRPFRAMERATE::ThreadRunFunction_Framerate(void* param)
{
  GRPFRAMERATE* grpframerate = (GRPFRAMERATE*)param;
  if(!grpframerate) return;

  XQWORD msec = grpframerate->xtimertotal->GetMeasureMilliSeconds();
  if(msec < 1000)  return;

  grpframerate->actual_framerate = (float)grpframerate->nframes;
      
  grpframerate->xtimertotal->Reset();
  grpframerate->nframes = 0;
  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPFRAMERATE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    GRAPHIC
*
* ---------------------------------------------------------------------------------------------------------------------*/
void GRPFRAMERATE::Clean()
{
  xtimertotal       = NULL;
  nframes           = 0;

  ngets             = 0;
  
  actual_framerate  = 0.0f;  
}


#pragma endregion

