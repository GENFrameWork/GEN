/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       SNDOpenALFactory.cpp
* 
* @class      SNDOPENALFACTORY
* @brief      Sound OpenAL Factory class
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

#include "SNDOpenALFactory.h"

#include "XFactory.h"
#include "XSleep.h"
#include "XTimer.h"

#include "SNDFactory_XEvent.h"
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
* @fn         SNDOPENALFACTORY::SNDOPENALFACTORY()
* @brief      Constructor
* @ingroup    SOUND
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
SNDOPENALFACTORY::SNDOPENALFACTORY() 
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SNDOPENALFACTORY::~SNDOPENALFACTORY()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    SOUND
* 
* @return     Does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
SNDOPENALFACTORY::~SNDOPENALFACTORY() 
{
  End();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SNDOPENALFACTORY::Ini()
* @brief      Ini
* @ingroup    SOUND
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALFACTORY::Ini()
{
  device = alcOpenDevice(NULL);
  if(!device) 
    {
      return false;
    }
      
  ALCenum error;
  
  context = alcCreateContext(device, NULL);
  error   = alcGetError(device);

  if(!context || (error != ALC_NO_ERROR))
    {
      alcCloseDevice(device);
      return false;
    }

  alcMakeContextCurrent(context);
  error = alcGetError(device);
  if(!context || (error != ALC_NO_ERROR))
    {
      alcCloseDevice(device);
      return false;
    }

  maxchannels   = 16;
  mastervolume  = 1.0f;

  playmutex = GEN_XFACTORY.Create_Mutex();
  if(!playmutex)
    {
      return false;
    }
  
  playthread = GEN_XFACTORY.CreateThread(XTHREADGROUPID_SOUND, __L("SNDOPENALFACTORY::SNDOPENALFACTORY"), SNDOPENALFACTORY::ThreadPlay, this);  
  if(!playthread)
    {
      return false;
    }

  if(!playthread->Ini())
    {
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SNDOPENALFACTORY::Update()
* @brief      Update
* @ingroup    SOUND
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALFACTORY::Update()
{  
  if(playmutex)
    {
      playmutex->Lock();
    }

  for(XDWORD c=0; c<soundplayitems.GetSize(); c++)
    {
      SNDOPENALPLAYITEM* playitem = soundplayitems.Get(c);
      if(playitem)
        {
          SNDITEM*         item   = playitem->GetItem();  
          SNDOPENALSOURCE* source = playitem->GetSource();        
          if(item && source)            
            {  
              SNDNOTE* soundnote = NULL;
              SNDFILE* soundfile = NULL;

              switch(item->GetType())
                {
                  case SNDITEM_TYPE_UNKNOWN : break;

                  case SNDITEM_TYPE_NOTE    : soundnote = playitem->GetItem()->GetSoundNote();
                                              break;

                  case SNDITEM_TYPE_FILE    : soundfile = playitem->GetItem()->GetSoundFile();
                                              break;              
                }
              
              if(!soundnote && !soundfile)
                {
                  continue;
                }

              if(playitem->GetEvent() == SNDOPENALPLAYITEM_XFSMEVENT_NONE) // Not new event
                {
                  switch(playitem->GetCurrentState())
                    {
                      case SNDOPENALPLAYITEM_XFSMSTATE_NONE         : break;
                      case SNDOPENALPLAYITEM_XFSMSTATE_INI          : break;

                      case SNDOPENALPLAYITEM_XFSMSTATE_PLAY         : { XQWORD  millisecond = item->GetTimerPlay()->GetMeasureMilliSeconds();
                                                                        bool    outoftime   = false;                                                                       

                                                                        item->SetPlayingTime((XDWORD)item->GetPlayingTime() + (XDWORD)millisecond);
                                                                        item->SetCurrentPlayingTime((XDWORD)item->GetCurrentPlayingTime() + (XDWORD)millisecond);

                                                                        item->GetTimerPlay()->Reset();

                                                                        if(item->GetNTimesToPlay() == SNDFACTORY_INLOOP)
                                                                          {
                                                                            if(!source->IsPLaying())
                                                                              {
                                                                                outoftime = true;  
                                                                              }
                                                                          }
                                                                          else     
                                                                          { 
                                                                            if(item->GetCurrentPlayingTime() >= (item->GetDuration()))
                                                                              {
                                                                                outoftime = true;    
                                                                              }
                                                                          }                                                             

                                                                        if(outoftime)
                                                                          {
                                                                            item->SetCurrentPlayingTime(0);  
                                                                          
                                                                            if(!item->GetCounterPlay())
                                                                              {
                                                                                playitem->SetEvent(SNDOPENALPLAYITEM_XFSMEVENT_STOP); 
                                                                                item->AddOneNTimesPlayed();  
                                                                              }
                                                                              else
                                                                              {
                                                                                item->SetCounterPlay(item->GetCounterPlay()-1);

                                                                                item->GetTimerPlay()->Reset();

                                                                                source->ResetPlay();
                                                                                source->Play();
                                                                              }                                                              
                                                                          }
                                                                      }
                                                                      break;

                      case SNDOPENALPLAYITEM_XFSMSTATE_PAUSE        : break;

                      case SNDOPENALPLAYITEM_XFSMSTATE_STOP         : break;

                      case SNDOPENALPLAYITEM_XFSMSTATE_END          : break;
                    }
                }
               else //  New event
                {
                  if(playitem->GetEvent() < SNDOPENALPLAYITEM_LASTEVENT)
                    {
                      playitem->CheckTransition();

                      switch(playitem->GetCurrentState())
                        {
                          case SNDOPENALPLAYITEM_XFSMSTATE_NONE     : break;

                          case SNDOPENALPLAYITEM_XFSMSTATE_INI      : {  switch(item->GetType())
                                                                          {
                                                                            case SNDITEM_TYPE_UNKNOWN : break;

                                                                            case SNDITEM_TYPE_NOTE    : GenerateBuffer(playitem, soundnote);
                                                                                                        break;

                                                                            case SNDITEM_TYPE_FILE    : GenerateBuffer(playitem, soundfile);
                                                                                                        break;              
                                                                          }   

                                                                        if(item->GetNTimesToPlay())
                                                                          {     
                                                                            item->SetCounterPlay(item->GetNTimesToPlay()-1);                                                                
                                                                            playitem->SetEvent(SNDOPENALPLAYITEM_XFSMEVENT_PLAY);                                                              
                                                                          }

                                                                        SNDFACTORY_XEVENT xevent(this, SNDFACTORY_XEVENT_TYPE_SOUND_INI);      
                                                                        xevent.SetItem(item);
                                                                        PostEvent(&xevent); 
                                                                      }
                                                                      break;

                          case SNDOPENALPLAYITEM_XFSMSTATE_PLAY     : { item->GetTimerPlay()->Reset();
                                                                        source->Play();
                                                                        item->SetStatus(SNDITEM_STATUS_PLAY);

                                                                        SNDFACTORY_XEVENT xevent(this, SNDFACTORY_XEVENT_TYPE_SOUND_PLAY);      
                                                                        xevent.SetItem(item);
                                                                        PostEvent(&xevent); 
                                                                      }
                                                                      break;

                          case SNDOPENALPLAYITEM_XFSMSTATE_PAUSE    : { source->Pause();
                                                                        item->SetStatus(SNDITEM_STATUS_PAUSE);
                                                                        
                                                                        SNDFACTORY_XEVENT xevent(this, SNDFACTORY_XEVENT_TYPE_SOUND_PAUSE);      
                                                                        xevent.SetItem(item);
                                                                        PostEvent(&xevent); 
                                                                      }
                                                                      break;

                          case SNDOPENALPLAYITEM_XFSMSTATE_STOP     : { source->Stop(); 

                                                                        item->SetStatus(SNDITEM_STATUS_STOP);

                                                                        if(item->GetNTimesToPlay() == SNDFACTORY_INLOOP)
                                                                          {
                                                                            playitem->SetEvent(SNDOPENALPLAYITEM_XFSMEVENT_END);                                                        
                                                                          }
                                                                          else
                                                                          {     
                                                                            if(!item->GetCounterPlay())
                                                                              {
                                                                                playitem->SetEvent(SNDOPENALPLAYITEM_XFSMEVENT_END);  
                                                                              }
                                                                          }

                                                                        SNDFACTORY_XEVENT xevent(this, SNDFACTORY_XEVENT_TYPE_SOUND_STOP);      
                                                                        xevent.SetItem(item);
                                                                        PostEvent(&xevent); 

                                                                      }
                                                                      break;

                            case SNDOPENALPLAYITEM_XFSMSTATE_END    : { if(playmutex)
                                                                          {
                                                                            playmutex->Lock();
                                                                          } 
                                                          
                                                                        soundplayitems.Delete(playitem);
                                                                        delete playitem;

                                                                        if(playmutex)
                                                                          {
                                                                            playmutex->UnLock();
                                                                          }

                                                                        item->SetStatus(SNDITEM_STATUS_END);

                                                                        SNDFACTORY_XEVENT xevent(this, SNDFACTORY_XEVENT_TYPE_SOUND_END);      
                                                                        xevent.SetItem(item);
                                                                        PostEvent(&xevent); 
                                                                      }
                                                                      break;
                        }
                    }
                }              
            }
        }
    }

  if(playmutex)
    {
      playmutex->UnLock();
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SNDOPENALFACTORY::End()
* @brief      End
* @ingroup    SOUND
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALFACTORY::End()
{  
  Sound_StopAll();

  if(playthread)
    {
      playthread->End();

      GEN_XFACTORY.DeleteThread(XTHREADGROUPID_SOUND, playthread);
      playthread = NULL;
    }
    
  if(playmutex)
    {
      GEN_XFACTORY.Delete_Mutex(playmutex);
      playmutex = NULL;
    }

  alcMakeContextCurrent(NULL);

  if(context)
    {
      alcDestroyContext(context);
      context = NULL;
    }

  if(device)
    {
      alcCloseDevice(device);
      device = NULL;
    }

  soundplayitems.DeleteContents();
  soundplayitems.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SNDOPENALFACTORY::Sound_Play(SNDITEM* item, SNDPLAYCFG* playCFG, int ntimestoplay)
* @brief      Sound_Play
* @ingroup    SOUND
* 
* @param[in]  item : 
* @param[in]  playCFG : 
* @param[in]  ntimestoplay : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALFACTORY::Sound_Play(SNDITEM* item, SNDPLAYCFG* playCFG, int ntimestoplay)
{   
  if(!SNDFACTORY::Sound_Play(item, playCFG, ntimestoplay))
    {
      return false;
    }

  if(soundplayitems.GetSize() >= maxchannels)
    {
      return false;
    }

  SNDOPENALSOURCE* source = new SNDOPENALSOURCE();
  if(!source)
    {
      return false;
    }

  SNDOPENALPLAYITEM* soundplayitem = new SNDOPENALPLAYITEM();
  if(!soundplayitem) 
    {
      delete source;
      return false;
    }

  soundplayitem->SetItem(item);
  soundplayitem->SetSource(source);  

  if(!soundplayitems.Add(soundplayitem))
    {
      return false;
    }  

  if(item->GetPlayCFG()->GetVolume() != SNDFACTORY_UNDEFINED)
    {
      source->SetVolume(item->GetPlayCFG()->GetVolume());
    }

  if(item->GetPlayCFG()->GetPitch() != SNDFACTORY_UNDEFINED)
    {
      source->SetPitch(item->GetPlayCFG()->GetPitch());
    }

  if(ntimestoplay == SNDFACTORY_INLOOP)
    {
      source->SetInLoop(true);      
    }
   else
    { 
      item->SetPlayingTime(0);
      source->SetInLoop(false);
    }

  soundplayitem->SetEvent(SNDOPENALPLAYITEM_XFSMEVENT_INI);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SNDINSTANCE* SNDOPENALFACTORY::Sound_Pause(SNDITEM* item)
* @brief      Sound_Pause
* @ingroup    SOUND
* 
* @param[in]  element : 
* 
* @return     SNDINSTANCE* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALFACTORY::Sound_Pause(SNDITEM* item)
{
  if(!item)
    {
      return false;
    }

  if(item->GetStatus() != SNDITEM_STATUS_PLAY)
    {   
      return false;
    }

  if(playmutex)
    {
      playmutex->Lock();
    }

  for(XDWORD c=0; c<soundplayitems.GetSize(); c++)
    {
      SNDOPENALPLAYITEM* playitem = soundplayitems.Get(c);
      if(playitem)
        {
          SNDITEM* pitem = playitem->GetItem();          
          if(item == pitem)
            {  
              playitem->SetEvent(SNDOPENALPLAYITEM_XFSMSTATE_PAUSE);              
            }
        }
    }

  if(playmutex)
    {
      playmutex->UnLock();
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SNDOPENALFACTORY::Sound_Stop(SNDITEM* item)
* @brief      Sound_Stop
* @ingroup    SOUND
* 
* @param[in]  item : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALFACTORY::Sound_Stop(SNDITEM* item)
{  
  if(!item)
    {
      return false;
    }

  if((item->GetStatus() != SNDITEM_STATUS_PLAY) && (item->GetStatus() != SNDITEM_STATUS_PAUSE))
    {   
      return false;
    }

  for(XDWORD c=0; c<soundplayitems.GetSize(); c++)
    {
      SNDOPENALPLAYITEM* playitem = soundplayitems.Get(c);
      if(playitem)
        {
          SNDITEM* pitem = playitem->GetItem();          
          if(item == pitem)
            {  
              playitem->SetEvent(SNDOPENALPLAYITEM_XFSMSTATE_STOP);              
            }
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SNDOPENALFACTORY::Sound_StopAll()
* @brief      Sound_StopAll
* @ingroup    SOUND
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALFACTORY::Sound_StopAll()
{   
  for(XDWORD c=0; c<soundplayitems.GetSize(); c++)
    {
      SNDOPENALPLAYITEM* playitem = soundplayitems.Get(c);
      if(playitem)
        {
          SNDITEM* item = playitem->GetItem();          
          if((item->GetStatus() == SNDITEM_STATUS_PLAY) || (item->GetStatus() == SNDITEM_STATUS_PAUSE))
            {  
              playitem->SetEvent(SNDOPENALPLAYITEM_XFSMSTATE_STOP);              
            }
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SNDOPENALFACTORY::Sound_WaitToEnd(SNDITEM* item, int maxtimeout, SNDFACTORY_WAITFUNCTION waitfunction)
* @brief      Sound_WaitToEnd
* @ingroup    SOUND
* 
* @param[in]  item : 
* @param[in]  maxtimeout : 
* @param[in]  waitfunction : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALFACTORY::Sound_WaitToEnd(SNDITEM* item, int maxtimeout, SNDFACTORY_WAITFUNCTION waitfunction)
{
  if(!item)
    {
      return false;
    }

  XTIMER* timeout = NULL;
  bool    status  =  true;
  
  timeout = GEN_XFACTORY.CreateTimer();
  if(!timeout)
    {
      return false;
    }

  timeout->Reset();

  while(item->GetStatus() != SNDITEM_STATUS_END)
    {
      if(maxtimeout)
        {
          if(timeout->GetMeasureMilliSeconds() >= maxtimeout)
            {
              status = false;  
              break;
            }
        }

      if(waitfunction)
        {
          if(!waitfunction(item))
            {
              status = false;
              break;
            }
        }
       else
        { 
          GEN_XSLEEP.MilliSeconds(100);
        }
    }

  GEN_XFACTORY.DeleteTimer(timeout);

  if(waitfunction)
    {
      if(!waitfunction(item))
        {
          status = false;
        }
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SNDOPENALFACTORY::Sound_WaitAllToEnd(int maxtimeout, SNDFACTORY_WAITFUNCTION waitfunction)
* @brief      Sound_WaitAllToEnd
* @ingroup    SOUND
* 
* @param[in]  maxtimeout : 
* @param[in]  waitfunction : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALFACTORY::Sound_WaitAllToEnd(int maxtimeout, SNDFACTORY_WAITFUNCTION waitfunction)
{
  XTIMER* timeout = NULL;
  bool    status  =  true;
  
  timeout = GEN_XFACTORY.CreateTimer();
  if(!timeout)
    {
      return false;
    }

  timeout->Reset();

  while(GEN_SNDFACTORY.Sound_IsAnyActive())
    {
      if(maxtimeout)
        {
          if(timeout->GetMeasureMilliSeconds() >= maxtimeout)
            {
              status = false;  
              break;
            }
        }

      if(waitfunction)
        {
          if(!waitfunction(NULL))
            {
              status = false;
              break;
            }
        }
       else
        { 
          GEN_XSLEEP.MilliSeconds(100);
        }
    }

  GEN_XFACTORY.DeleteTimer(timeout);

  if(waitfunction)
    {
      if(!waitfunction(NULL))
        {
          status = false;          
        }
    }

  return status;
}  


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int SNDOPENALFACTORY::Sound_GetVolume(SNDITEM* item)
* @brief      Sound_GetVolume
* @ingroup    SOUND
* 
* @param[in]  item : 
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int SNDOPENALFACTORY::Sound_GetVolume(SNDITEM* item)
{
  SNDOPENALPLAYITEM* playitem =  GetPlayItemFromItem(item);
  if(playitem)
    {
      SNDOPENALSOURCE* source = playitem->GetSource();
      if(source)
        {
          return source->GetVolume();
        }    
    }

  return SNDFACTORY_UNDEFINED;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SNDOPENALFACTORY::Sound_SetVolume(SNDITEM* item, int volume)
* @brief      Sound_SetVolume
* @ingroup    SOUND
* 
* @param[in]  item : 
* @param[in]  volume : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALFACTORY::Sound_SetVolume(SNDITEM* item, int volume)
{
  SNDOPENALPLAYITEM* playitem =  GetPlayItemFromItem(item);
  if(playitem)
    {
      SNDOPENALSOURCE* source = playitem->GetSource();
      if(source)
        {
          return source->SetVolume(volume);
        }    
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         float SNDOPENALFACTORY::Sound_GetPitch(SNDITEM* item)
* @brief      Sound_GetPitch
* @ingroup    SOUND
* 
* @param[in]  item : 
* 
* @return     float : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
float SNDOPENALFACTORY::Sound_GetPitch(SNDITEM* item)
{
  SNDOPENALPLAYITEM* playitem =  GetPlayItemFromItem(item);
  if(playitem)
    {
      SNDOPENALSOURCE* source = playitem->GetSource();
      if(source)
        {
          return source->GetPitch();
        }    
    }

  return SNDFACTORY_UNDEFINED;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SNDOPENALFACTORY::Sound_SetPitch(SNDITEM* item, float pitch)
* @brief      Sound_SetPitch
* @ingroup    SOUND
* 
* @param[in]  item : 
* @param[in]  pitch : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALFACTORY::Sound_SetPitch(SNDITEM* item, float pitch)
{
  SNDOPENALPLAYITEM* playitem =  GetPlayItemFromItem(item);
  if(playitem)
    {
      SNDOPENALSOURCE* source = playitem->GetSource();
      if(source)
        {
          source->SetPitch(pitch);
          return true;
        }    
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SNDOPENALFACTORY::DeleteAllItems()
* @brief      DeleteAllItems
* @ingroup    SOUND
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALFACTORY::DeleteAllItems()
{
  Sound_StopAll();

  Sound_WaitAllToEnd(3000);  
  
  SNDFACTORY::DeleteAllItems();

  return true;
}

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XMUTEX* SNDOPENALFACTORY::GetPlayMutex()
* @brief      GetPlayMutex
* @ingroup    SOUND
* 
* @return     XMUTEX* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XMUTEX* SNDOPENALFACTORY::GetPlayMutex()
{
  return playmutex;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<SNDOPENALPLAYITEM*>* SNDOPENALFACTORY::GetSoundPlayItems()
* @brief      GetSoundPlayItems
* @ingroup    SOUND
* 
* @return     XVECTOR<SNDOPENALPLAYITEM*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<SNDOPENALPLAYITEM*>* SNDOPENALFACTORY::GetSoundPlayItems()
{
  return &soundplayitems;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SNDOPENALPLAYITEM* SNDOPENALFACTORY::GetPlayItemFromItem(SNDITEM* item)
* @brief      GetPlayItemFromItem
* @ingroup    SOUND
* 
* @param[in]  item : 
* 
* @return     SNDOPENALPLAYITEM* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
SNDOPENALPLAYITEM* SNDOPENALFACTORY::GetPlayItemFromItem(SNDITEM* item)
{ 
  for(XDWORD c=0; c<soundplayitems.GetSize(); c++)
    {
      SNDOPENALPLAYITEM* playitem = soundplayitems.Get(c);
      if(playitem)
        {
          SNDITEM* _item = playitem->GetItem();          
          if(item == _item)
            {  
              return playitem;
            }
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SNDOPENALFACTORY::GenerateBuffer(SNDOPENALPLAYITEM* playitem, SNDNOTE* soundnote)
* @brief      GenerateBuffer
* @ingroup    SOUND
* 
* @param[in]  playitem : 
* @param[in]  soundnote : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALFACTORY::GenerateBuffer(SNDOPENALPLAYITEM* playitem, SNDNOTE* soundnote)
{
  if(!playitem || !soundnote)
    {
      return false;
    }

  XDWORD           samplerate = 10000;
  XDWORD           size       = 0;
  SNDOPENALSOURCE* source     = playitem->GetSource();

  if(!source)
    {
      return false;
    }  

  source->GetBuffer()->GetXBuffer()->Empty();

  size += source->GetBuffer()->GenerateNote(soundnote->GetFrequency(), soundnote->GetDuration()*2, samplerate);
  size += source->GetBuffer()->GenerateNote(soundnote->GetFrequency(), 2000, samplerate);
                                                            
  source->GetBuffer()->Assign(1, size, samplerate);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SNDOPENALFACTORY::GenerateBuffer(SNDOPENALPLAYITEM* playitem, SNDFILE* soundfile)
* @brief      GenerateBuffer
* @ingroup    SOUND
* 
* @param[in]  playitem : 
* @param[in]  soundfile : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SNDOPENALFACTORY::GenerateBuffer(SNDOPENALPLAYITEM* playitem, SNDFILE* soundfile)
{
  if(!playitem || !soundfile)
    {
      return false;
    }

  XDWORD           size       = 0;
  SNDOPENALSOURCE* source     = playitem->GetSource();

  source->GetBuffer()->GetXBuffer()->Add(soundfile->GetData());

  source->GetBuffer()->Assign(soundfile->GetChannels(), soundfile->GetData()->GetSize(), soundfile->GetSampleRate());

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void SNDOPENALFACTORY::ThreadPlay(void* param)
* @brief      ThreadPlay
* @ingroup    SOUND
* 
* @param[in]  param : 
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void SNDOPENALFACTORY::ThreadPlay(void* param)
{
  SNDOPENALFACTORY* sndopenal = (SNDOPENALFACTORY*)param;
  if(!sndopenal) return;  

  sndopenal->Update();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void SNDOPENALFACTORY::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    SOUND
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void SNDOPENALFACTORY::Clean()
{        
  device        = NULL;
  context       = NULL;

  mastervolume  = 0.0f;
  maxchannels   = 0;
                                        
  playthread    = NULL;
  playmutex     = NULL;                                                    
}


#pragma endregion

