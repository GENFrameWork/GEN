/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       SNDOpenALFactory.cpp
* 
* @class      SNDOPENALFACTORY
* @brief      Sound OpenAL Play Item class
* @ingroup    SOUND
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

#include "SNDOpenALPlayItem.h"

#include "SNDOpenALSource.h"
#include "SNDItem.h"
#include "SNDPlayCFG.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SNDOPENALPLAYITEM::SNDOPENALPLAYITEM()
* @brief      Constructor
* @ingroup    SOUND
* 
* --------------------------------------------------------------------------------------------------------------------*/
SNDOPENALPLAYITEM::SNDOPENALPLAYITEM() : XFSMACHINE(0)
{
  Clean();

  IniFSMachine();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SNDOPENALPLAYITEM::~SNDOPENALPLAYITEM()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    SOUND
* 
* --------------------------------------------------------------------------------------------------------------------*/
SNDOPENALPLAYITEM::~SNDOPENALPLAYITEM()
{
  Delete();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SNDOPENALPLAYITEM::IniFSMachine()
* @brief      IniFSMachine
* @ingroup    SOUND
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALPLAYITEM::IniFSMachine()
{
  if(!AddState( SNDOPENALPLAYITEM_XFSMSTATE_NONE            ,
                SNDOPENALPLAYITEM_XFSMEVENT_INI             , SNDOPENALPLAYITEM_XFSMSTATE_INI           ,
                SNDOPENALPLAYITEM_XFSMEVENT_END             , SNDOPENALPLAYITEM_XFSMSTATE_END           ,
                XFSMACHINESTATE_EVENTDEFEND)) return false;


  if(!AddState( SNDOPENALPLAYITEM_XFSMSTATE_INI             ,
                SNDOPENALPLAYITEM_XFSMEVENT_PLAY            , SNDOPENALPLAYITEM_XFSMSTATE_PLAY          ,
                SNDOPENALPLAYITEM_XFSMEVENT_END             , SNDOPENALPLAYITEM_XFSMSTATE_END           ,
                XFSMACHINESTATE_EVENTDEFEND)) return false;


  if(!AddState( SNDOPENALPLAYITEM_XFSMSTATE_PLAY            ,
                SNDOPENALPLAYITEM_XFSMEVENT_PAUSE           , SNDOPENALPLAYITEM_XFSMSTATE_PAUSE         ,                
                SNDOPENALPLAYITEM_XFSMEVENT_STOP            , SNDOPENALPLAYITEM_XFSMSTATE_STOP          ,
                SNDOPENALPLAYITEM_XFSMEVENT_END             , SNDOPENALPLAYITEM_XFSMSTATE_END           ,
                XFSMACHINESTATE_EVENTDEFEND)) return false;

  if(!AddState( SNDOPENALPLAYITEM_XFSMSTATE_PAUSE           ,
                SNDOPENALPLAYITEM_XFSMEVENT_PLAY            , SNDOPENALPLAYITEM_XFSMSTATE_PLAY          ,                
                SNDOPENALPLAYITEM_XFSMEVENT_STOP            , SNDOPENALPLAYITEM_XFSMSTATE_STOP          ,
                SNDOPENALPLAYITEM_XFSMEVENT_END             , SNDOPENALPLAYITEM_XFSMSTATE_END           ,
                XFSMACHINESTATE_EVENTDEFEND)) return false;


  if(!AddState( SNDOPENALPLAYITEM_XFSMSTATE_STOP            ,
                SNDOPENALPLAYITEM_XFSMEVENT_PLAY            , SNDOPENALPLAYITEM_XFSMSTATE_PLAY          ,                
                SNDOPENALPLAYITEM_XFSMEVENT_PAUSE           , SNDOPENALPLAYITEM_XFSMSTATE_PAUSE         ,
                SNDOPENALPLAYITEM_XFSMEVENT_END             , SNDOPENALPLAYITEM_XFSMSTATE_END           ,
                XFSMACHINESTATE_EVENTDEFEND)) return false;


  if(!AddState( SNDOPENALPLAYITEM_XFSMSTATE_END             ,
                SNDOPENALPLAYITEM_XFSMEVENT_NONE            , SNDOPENALPLAYITEM_XFSMSTATE_NONE          ,
                SNDOPENALPLAYITEM_XFSMEVENT_INI             , SNDOPENALPLAYITEM_XFSMSTATE_INI           ,
                XFSMACHINESTATE_EVENTDEFEND)) return false;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SNDITEM* SNDOPENALPLAYITEM::GetItem()
* @brief      GetItem
* @ingroup    SOUND
* 
* @return     SNDITEM* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
SNDITEM* SNDOPENALPLAYITEM::GetItem()
{
  return item;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void SNDOPENALPLAYITEM::SetItem(SNDITEM* item)
* @brief      SetItem
* @ingroup    SOUND
* 
* @param[in]  item : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void SNDOPENALPLAYITEM::SetItem(SNDITEM* item)
{
  this->item = item;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SNDOPENALSOURCE* SNDOPENALPLAYITEM::GetSource()
* @brief      GetSource
* @ingroup    SOUND
* 
* @return     SNDOPENALSOURCE* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
SNDOPENALSOURCE* SNDOPENALPLAYITEM::GetSource()
{
  return source;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void SNDOPENALPLAYITEM::SetSource(SNDOPENALSOURCE* source)
* @brief      SetSource
* @ingroup    SOUND
* 
* @param[in]  source : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void SNDOPENALPLAYITEM::SetSource(SNDOPENALSOURCE* source)
{
  this->source = source;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SNDOPENALPLAYITEM::Delete()
* @brief      Delete
* @ingroup    SOUND
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALPLAYITEM::Delete()
{
  item = NULL;

  if(source)
    {
      delete source;
      source = NULL;
    }   

  return true;
}
  

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void SNDOPENALPLAYITEM::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    SOUND
* 
* --------------------------------------------------------------------------------------------------------------------*/
void SNDOPENALPLAYITEM::Clean()
{
  item      = NULL;
  source    = NULL;
}


#pragma endregion

