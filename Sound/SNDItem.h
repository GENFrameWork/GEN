/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       SNDElement.h
* 
* @class      SNDITEM
* @brief      Sound Item class
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

#ifndef _SNDITEM_H_
#define _SNDITEM_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XBase.h"
#include "XString.h"

#include "SNDFile.h"
#include "SNDNote.h"
#include "SNDPlayCFG.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

enum SNDITEM_TYPE
{
  SNDITEM_TYPE_UNKNOWN       = 0 , 
  SNDITEM_TYPE_NOTE              ,
  SNDITEM_TYPE_FILE              , 
};

enum SNDITEM_STATUS
{
  SNDITEM_STATUS_NONE        = 0 ,
  SNDITEM_STATUS_INI             ,
  SNDITEM_STATUS_PLAY            ,
  SNDITEM_STATUS_PAUSE           ,     
  SNDITEM_STATUS_STOP            ,  
  SNDITEM_STATUS_END               
};

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class XTIMER;

class SNDITEM
{
  public:
                              SNDITEM                 ();
    virtual                  ~SNDITEM                 ();

    SNDITEM_TYPE              GetType                 ();
    void                      SetType                 (SNDITEM_TYPE type);
    bool                      GetType                 (XSTRING& typestr); 

    XSTRING*                  GetID                   ();
    
    SNDITEM_STATUS            GetStatus               (); 
    void                      SetStatus               (SNDITEM_STATUS status); 
    bool                      GetStatus               (XSTRING& statusstr); 

    XDWORD                    GetNTimesPlayed         ();
    void                      AddOneNTimesPlayed      ();    

    int                       GetNTimesToPlay         ();
    void                      SetNTimesToPlay         (int ntimestoplay);

    int                       GetCounterPlay          ();
    void                      SetCounterPlay          (int counterplay);

    XDWORD                    GetPlayingTime          ();        
    void                      SetPlayingTime          (XDWORD playingtime);       

    XTIMER*                   GetTimerPlay            ();    

    SNDPLAYCFG*               GetPlayCFG              ();
    bool                      SetPlayCFG              (SNDPLAYCFG& playCFG);
    
    SNDFILE*                  GetSoundFile            ();
    void                      SetSoundFile            (SNDFILE* soundfile);

    SNDNOTE*                  GetSoundNote            ();
    void                      SetSoundNote            (SNDNOTE* soundnote);
        
  private:

    void                      Clean                   ();

    SNDITEM_TYPE              type;
    XSTRING                   ID;
    SNDITEM_STATUS            status;
    
    XDWORD                    ntimesplayed;
    int                       ntimestoplay;
    int                       counterplay;

    SNDPLAYCFG                playCFG;

    XTIMER*                   timerplay;  

    XDWORD                    playingtime;

    SNDFILE*                  soundfile;
    SNDNOTE*                  soundnote; 
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

