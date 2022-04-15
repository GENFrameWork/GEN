
/*------------------------------------------------------------------------------------------
//  SNDANDROIDFACTORY.CPP
//
//  windows sound system
//
//  Author            : Imanol Celaya Ruiz de Alegria
//  Date Of Creation  : 11/11/2015 10:52:15
//  Last Modification :
//
//  GEN  Copyright (C).  All right reserved.
//----------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES --------------------------------------------------------------------------*/

#include "SNDAndroidFactory.h"
#include "SNDOpenAL.h"
#include "SNDFileOGG.h"
#include "SNDDummyResampler.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ------------------------------------------------------------------*/


/*---- CLASS MEMBERS ---------------------------------------------------------------------*/




/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::SNDANDROIDFACTORY
*/
/**
//
//  Class Constructor SNDANDROIDFACTORY
//
//  ""
//  @version      11/11/2015 10:57:23
//


//  @param        SNDFACTORY(xfactory :


*/
/*-----------------------------------------------------------------*/
SNDANDROIDFACTORY::SNDANDROIDFACTORY() : SNDFACTORY()
{
  Clean();
  sndopenal = new SNDOPENAL(this);
}






/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::~SNDANDROIDFACTORY
*/
/**
//
//   Class Destructor SNDANDROIDFACTORY
//
//  ""
//  @version      11/11/2015 10:57:28
//
*/
/*-----------------------------------------------------------------*/
SNDANDROIDFACTORY::~SNDANDROIDFACTORY()
{
  delete sndopenal;
  Clean();
}




/*-------------------------------------------------------------------
//   SNDANDROIDFACTORY::AddFile
*/
/**
//
//
//  ""
//  @version  22/02/2018 9:59:02
//
//  @return   SNDELEMENT* :
//
//  @param    XPATH& :
//  @param    XSTRING* :
//  @param    bool :
//
*//*-----------------------------------------------------------------*/
SNDELEMENT* SNDANDROIDFACTORY::AddFile(XPATH& xpath, XSTRING* namefile, bool stream)
{
  return sndopenal->AddFile(xpath, namefile, stream);
}




/*-------------------------------------------------------------------
//   SNDANDROIDFACTORY::AddFile
*/
/**
//
//
//  ""
//  @version  22/02/2018 9:59:35
//
//  @return   SNDELEMENT* :
//
//  @param    XPATH& :
//  @param    XCHAR* :
//  @param    bool :
//
*//*-----------------------------------------------------------------*/
SNDELEMENT* SNDANDROIDFACTORY::AddFile(XPATH& xpath, XCHAR* namefile, bool stream)
{
  return sndopenal->AddFile(xpath, namefile, stream);
}




/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::GetFile
*/
/**
//
//  gets an already loaded file
//
//  ""
//  @version      11/11/2015 10:58:08
//
//  @return       SNDELEMENT* :
//
//  @param        namefile :
*/
/*-----------------------------------------------------------------*/
SNDELEMENT* SNDANDROIDFACTORY::GetFile(XSTRING* namefile, bool stream)
{
  return sndopenal->GetFile(namefile, stream);
}



/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::GetFile
*/
/**
//
//  gets an already loaded file
//
//  ""
//  @version      11/11/2015 10:58:18
//
//  @return       SNDELEMENT* :
//
//  @param        file :
*/
/*-----------------------------------------------------------------*/
SNDELEMENT* SNDANDROIDFACTORY::GetFile(XCHAR* namefile, bool stream)
{
  return sndopenal->GetFile(namefile, stream);
}



/*-------------------------------------------------------------------
//   SNDANDROIDFACTORY::RemoveFile
*/
/**
//
//
//
//  ""
//  @version

//  @return   bool :
//
//  @param    SNDELEMENT* :
//
*//*-----------------------------------------------------------------*/
bool SNDANDROIDFACTORY::RemoveFile(SNDELEMENT* element)
{
  return sndopenal->RemoveFile(element);
}


/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::GetStreamer
*/
/**
//
//
//
//  ""
//  @version      20/07/2016 11:56:58
//
//  @return       SNDSTREAMELEMENT* :
//
*/
/*-----------------------------------------------------------------*/
SNDSTREAMELEMENT* SNDANDROIDFACTORY::GetStreamer()
{
  return sndopenal->GetStreamer();
}



/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::PlaySound
*/
/**
//
//  plays a sund
//
//  ""
//  @version      11/11/2015 10:58:27
//
//  @return       SNDSOURCE* :
//
//  @param        element :
*/
/*-----------------------------------------------------------------*/
SNDINSTANCE* SNDANDROIDFACTORY::PlaySound(SNDELEMENT* element)
{
  return sndopenal->PlaySound(element);
}



/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::StopSound
*/
/**
//
//  stops a sound
//
//  ""
//  @version      11/11/2015 10:58:38
//
//  @param        element :
*/
/*-----------------------------------------------------------------*/
void SNDANDROIDFACTORY::StopSound(SNDELEMENT* element)
{
  sndopenal->StopSound(element);
}





/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::PauseSound
*/
/**
//
//  pauses a sound
//
//  ""
//  @version      11/11/2015 10:58:47
//
//  @return       SNDSOURCE* :
//
//  @param        element :
*/
/*-----------------------------------------------------------------*/
SNDINSTANCE* SNDANDROIDFACTORY::PauseSound(SNDELEMENT* element)
{
  return sndopenal->PauseSound(element);
}




/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::Update
*/
/**
//
//  system update
//
//  ""
//  @version      11/11/2015 10:58:55
//
*/
/*-----------------------------------------------------------------*/
void SNDANDROIDFACTORY::ImpUpdate()
{
  sndopenal->Update();
}





/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::IsAnyPlaying
*/
/**
//
//  checks if any sound is playing
//
//  ""
//  @version      11/11/2015 10:59:06
//
//  @return       bool :
//
*/
/*-----------------------------------------------------------------*/
bool SNDANDROIDFACTORY::IsAnyPlaying()
{
  return sndopenal->IsAnyPlaying();
}




/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::StopAll
*/
/**
//
//  stops all sounds
//
//  ""
//  @version      11/11/2015 10:59:15
//
*/
/*-----------------------------------------------------------------*/
void SNDANDROIDFACTORY::StopAll()
{
  sndopenal->StopAll();
}




/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::SetMasterVolume
*/
/**
//
//  sets the master volume
//
//  ""
//  @version      11/11/2015 10:59:25
//
//  @param        mastervolume :
*/
/*-----------------------------------------------------------------*/
void SNDANDROIDFACTORY::SetMasterVolume(float mastervolume)
{
  sndopenal->SetMasterVolume(mastervolume);
}




/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::GetMasterVolume
*/
/**
//
//  gets the master volume
//
//  ""
//  @version      11/11/2015 10:59:33
//
//  @return       float :
//
*/
/*-----------------------------------------------------------------*/
float SNDANDROIDFACTORY::GetMasterVolume()
{
  return sndopenal->GetMasterVolume();
}



/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::GetResampler
*/
/**
//
//
//
//  ""
//  @version      16/03/2017 16:24:48
//
//  @return       SNDRESAMPLER* :
//
*/
/*-----------------------------------------------------------------*/
SNDRESAMPLER* SNDANDROIDFACTORY::GetResampler()
{
  return new SNDDUMMYRESAMPLER();
}



/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::CreateFile
*/
/**
//
//
//
//  ""
//  @version      16/03/2017 16:24:43
//
//  @return       SNDFILE* :
//
*/
/*-----------------------------------------------------------------*/
SNDFILE* SNDANDROIDFACTORY::CreateSNDFile()
{
  return new SNDFILEOGG();
}



/*-------------------------------------------------------------------
//  SNDANDROIDFACTORY::DeleteFile
*/
/**
//
//
//
//  ""
//  @version      16/03/2017 16:24:36
//
//  @param        file :
*/
/*-----------------------------------------------------------------*/
void SNDANDROIDFACTORY::DeleteSNDFile(SNDFILE* file)
{
  delete file;
}
