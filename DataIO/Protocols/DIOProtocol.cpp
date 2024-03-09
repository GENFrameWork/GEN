/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOProtocol.cpp
* 
* @class      DIOPROTOCOL
* @brief      Data Input/Output Generic Binary Protocol class
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

/*---- PRECOMPILATION INCLUDES ----------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOProtocol.h"

#include <stdarg.h>
#include <string.h>

#include "XSleep.h"
#include "XDateTime.h"
#include "XTimer.h"
#include "XRand.h"
#include "XFile.h"
#include "XSystem.h"
#include "XFactory.h"
#include "XString.h"
#include "XThreadCollected.h"
#include "XTrace.h"
#include "XVariant.h"

#include "HashCRC32.h"

#include "DIOFactory.h"
#include "DIOStream.h"
#include "DIOProtocol_ConnectionsManager.h"
#include "DIOProtocolConnectionsInUseDefinitions.h"

#include "DIOProtocol_ApplicationData.h"
#include "DIOProtocol_XEvent.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


#pragma region CLASS_DIOPROTOCOL_ANSWER


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL_ANSWER::DIOPROTOCOL_ANSWER(XDWORD type, XDWORD ID, XBUFFER& xbuffer)
* @brief      Constructor
* @ingroup    DATAIO
*
* @param[in]  XDWORD :
* @param[in]  XDWORD ID :
* @param[in]  XBUFFER& xbuffer :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_ANSWER::DIOPROTOCOL_ANSWER(XDWORD type, XDWORD ID, XBUFFER& xbuffer)
{
  Clean();

  this->type    = type;
  this->ID      = ID;

  if(!xbuffer.IsEmpty()) this->xbuffer.Add(xbuffer);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOPROTOCOL_ANSWER::~DIOPROTOCOL_ANSWER()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_ANSWER::~DIOPROTOCOL_ANSWER()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOPROTOCOL_ANSWER::GetType()
* @brief      GetType
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOPROTOCOL_ANSWER::GetType()
{
  return type;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD DIOPROTOCOL_ANSWER::GetID()
* @brief      GetID
* @ingroup    DATAIO
* 
* @return     XDWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOPROTOCOL_ANSWER::GetID()
{
  return ID;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER& DIOPROTOCOL_ANSWER::GetBuffer()
* @brief      GetBuffer
* @ingroup    DATAIO
*
* @return     XBUFFER& :
*
* --------------------------------------------------------------------------------------------------------------------*/
XBUFFER& DIOPROTOCOL_ANSWER::GetBuffer()
{
  return xbuffer;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL_ANSWER::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL_ANSWER::Clean()
{
  type      = DIOPROTOCOL_CMDTYPE_NONE;
  ID        = 0;
}


#pragma endregion



#pragma region CLASS_DIOPROTOCOL_COMMAND


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL_COMMAND::DIOPROTOCOL_COMMAND(XDWORD type, XCHAR* inmask, XCHAR* outmask, DIOPROTOCOL_RECEIVEDFUNC receivedfunc, XCHAR* description)
* @brief      Constructor
* @ingroup    DATAIO
*
* @param[in]  type :
* @param[in]  inmask :
* @param[in]  outmask :
* @param[in]  receivedfunc :
* @param[in]  description :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_COMMAND::DIOPROTOCOL_COMMAND(XDWORD type, XCHAR* inmask, XCHAR* outmask, DIOPROTOCOL_RECEIVEDFUNC receivedfunc, XCHAR* description)
{
  Clean();

  this->type    = type;
  this->inmask  = inmask;
  this->outmask = outmask;

  if(receivedfunc)  this->receivedfunc  = receivedfunc;
  if(description)   this->description   = description;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOPROTOCOL_COMMAND::~DIOPROTOCOL_COMMAND()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_COMMAND::~DIOPROTOCOL_COMMAND()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOPROTOCOL_COMMAND::GetType()
* @brief      GetType
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOPROTOCOL_COMMAND::GetType()
{
  return type;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* DIOPROTOCOL_COMMAND::GetINMask()
* @brief      GetINMask
* @ingroup    DATAIO
*
* @return     XCHAR* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* DIOPROTOCOL_COMMAND::GetINMask()
{
  return inmask.Get();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* DIOPROTOCOL_COMMAND::GetOUTMask()
* @brief      GetOUTMask
* @ingroup    DATAIO
*
* @return     XCHAR* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* DIOPROTOCOL_COMMAND::GetOUTMask()
{
  return outmask.Get();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL_RECEIVEDFUNC DIOPROTOCOL_COMMAND::GetReceivedFunc()
* @brief      GetReceivedFunc
* @ingroup    DATAIO
*
* @return     DIOPROTOCOL_RECEIVEDFUNC :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_RECEIVEDFUNC DIOPROTOCOL_COMMAND::GetReceivedFunc()
{
  return receivedfunc;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* DIOPROTOCOL_COMMAND::GetDescription()
* @brief      GetDescription
* @ingroup    DATAIO
*
* @return     XCHAR* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* DIOPROTOCOL_COMMAND::GetDescription()
{
  return description.Get();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOPROTOCOL_COMMAND::GetSINMask()
* @brief      GetSINMask
* @ingroup    DATAIO
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOPROTOCOL_COMMAND::GetSINMask()
{
  return &inmask;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOPROTOCOL_COMMAND::GetSOUTMask()
* @brief      GetSOUTMask
* @ingroup    DATAIO
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOPROTOCOL_COMMAND::GetSOUTMask()
{
  return &outmask;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOPROTOCOL_COMMAND::GetSDescription()
* @brief      GetSDescription
* @ingroup    DATAIO
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOPROTOCOL_COMMAND::GetSDescription()
{
  return &description;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL_COMMAND::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL_COMMAND::Clean()
{
  type         = DIOPROTOCOL_CMDTYPE_NONE;

  inmask.Empty();
  outmask.Empty();

  receivedfunc = NULL;

  description.Empty();
}


#pragma endregion


#pragma region CLASS_DIOPROTOCOL_FILE


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL_FILE::DIOPROTOCOL_FILE()
* @brief      Constructor
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_FILE::DIOPROTOCOL_FILE()
{
  Clean();

  GEN_XFACTORY_CREATE(xfile, Create_File())
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL_FILE::~DIOPROTOCOL_FILE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_FILE::~DIOPROTOCOL_FILE()
{
  GEN_XFACTORY.Delete_File(xfile);

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL_FILE::IsActive()
* @brief      IsActive
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL_FILE::IsActive()
{
  return isactive;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL_FILE::SetActive(bool isactive)
* @brief      SetActive
* @ingroup    DATAIO
*
* @param[in]  isactive :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL_FILE::SetActive(bool isactive)
{
  this->isactive = isactive;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL_DIRECTION DIOPROTOCOL_FILE::GetDirection()
* @brief      GetDirection
* @ingroup    DATAIO
*
* @return     DIOPROTOCOL_DIRECTION :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_DIRECTION DIOPROTOCOL_FILE::GetDirection()
{
  return direction;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL_FILE::SetDirection(DIOPROTOCOL_DIRECTION direction)
* @brief      SetDirection
* @ingroup    DATAIO
*
* @param[in]  direction :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL_FILE::SetDirection(DIOPROTOCOL_DIRECTION direction)
{
  this->direction = direction;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XPATH* DIOPROTOCOL_FILE::GetSourceName()
* @brief      GetSourceName
* @ingroup    DATAIO
*
* @return     XPATH* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XPATH* DIOPROTOCOL_FILE::GetSourceName()
{
  return &sourcename;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL_FILE::SetSourceName(XCHAR* sourcename)
* @brief      SetSourceName
* @ingroup    DATAIO
*
* @param[in]  sourcename :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL_FILE::SetSourceName(XCHAR* sourcename)
{
  if(!sourcename) return false;
  this->sourcename.Set(sourcename);
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL_FILE::SetSourceName(XSTRING& sourcename)
* @brief      SetSourceName
* @ingroup    DATAIO
*
* @param[in]  sourcename :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL_FILE::SetSourceName(XSTRING& sourcename)
{
  return SetSourceName(sourcename.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XPATH* DIOPROTOCOL_FILE::GetTargetName()
* @brief      GetTargetName
* @ingroup    DATAIO
*
* @return     XPATH* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XPATH* DIOPROTOCOL_FILE::GetTargetName()
{
  return &targetname;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL_FILE::SetTargetName(XCHAR* targetname)
* @brief      SetTargetName
* @ingroup    DATAIO
*
* @param[in]  targetname :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL_FILE::SetTargetName(XCHAR* targetname)
{
  if(!targetname) return false;
  this->targetname.Set(targetname);
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL_FILE::SetTargetName(XSTRING& targetname)
* @brief      SetTargetName
* @ingroup    DATAIO
*
* @param[in]  targetname :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL_FILE::SetTargetName(XSTRING& targetname)
{
  return SetTargetName(targetname.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOPROTOCOL_FILE::GetSize()
* @brief      GetSize
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XQWORD DIOPROTOCOL_FILE::GetSize()
{
  return size;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL_FILE::SetSize(XDWORD size)
* @brief      SetSize
* @ingroup    DATAIO
*
* @param[in]  size :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL_FILE::SetSize(XQWORD size)
{
  this->size = size;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOPROTOCOL_FILE::GetCRC32()
* @brief      GetCRC32
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOPROTOCOL_FILE::GetCRC32()
{
  return crc32;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL_FILE::SetCRC32(XDWORD crc32)
* @brief      SetCRC32
* @ingroup    DATAIO
*
* @param[in]  crc32 :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL_FILE::SetCRC32(XDWORD crc32)
{
  this->crc32 = crc32;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL_FILE::GetSizeBlock()
* @brief      GetSizeBlock
* @ingroup    DATAIO
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL_FILE::GetSizeBlock()
{
  return sizeblock;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL_FILE::SetSizeBlock(XDWORD sizeblock)
* @brief      SetSizeBlock
* @ingroup    DATAIO
*
* @param[in]  sizeblock :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL_FILE::SetSizeBlock(XDWORD sizeblock)
{
  this->sizeblock = sizeblock;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL_FILE::GetPosition()
* @brief      GetPosition
* @ingroup    DATAIO
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
XQWORD DIOPROTOCOL_FILE::GetPosition()
{
  return position;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL_FILE::SetPosition(int position)
* @brief      SetPosition
* @ingroup    DATAIO
*
* @param[in]  position :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL_FILE::SetPosition(XQWORD position)
{
  this->position = position;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBYTE DIOPROTOCOL_FILE::GetPercent()
* @brief      GetPercent
* @ingroup    DATAIO
*
* @return     XBYTE :
*
* --------------------------------------------------------------------------------------------------------------------*/
XBYTE DIOPROTOCOL_FILE::GetPercent()
{
  return percent;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL_FILE::SetPercent(XBYTE percent)
* @brief      SetPercent
* @ingroup    DATAIO
*
* @param[in]  percent :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL_FILE::SetPercent(XBYTE percent)
{
  this->percent = percent;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL_FILE::ReadBlock(XBYTE* buffer)
* @brief      ReadBlock
* @ingroup    DATAIO
*
* @param[in]  buffer :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL_FILE::ReadBlock(XBYTE* buffer)
{
  if(!sourcename[0]) return false;

  if(!xfile->Open(sourcename)) return false;

  bool status = false;

  size = xfile->GetSize();
  if(size)
    {
      xfile->SetPosition(position);

      xfile->Read(buffer, &sizeblock);
      if(sizeblock)
        {
          xfile->GetPosition(position);
          if(size) percent = (XBYTE)(position*100/size);

          status = true;
        }
    }

  xfile->Close();

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOPROTOCOL_FILE::WriteBlock(XBYTE* buffer)
* @brief      WriteBlock
* @ingroup    DATAIO
* 
* @param[in]  buffer : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL_FILE::WriteBlock(XBYTE* buffer)
{
  if(!xfile)         return false;
  if(!targetname[0]) return false;

  if(position)
    {
      if(!xfile->Open(targetname,false))
        {
          if(!xfile->Create(targetname)) return false;
        }
    }
   else
    {
      if(!xfile->Create(targetname)) return false;
    }

  bool status = false;

  if(sizeblock)
    {
      xfile->SetPosition((XDWORD)XFILE_SEEKEND);

      status = xfile->Write(buffer,sizeblock);
      if(status)
        {
          xfile->GetPosition(position);
          if(size) percent = (XBYTE)(position*100/size);
        }
    }

  xfile->Close();

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOPROTOCOL_FILE::Erase()
* @brief      Erase
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL_FILE::Erase()
{
  if(!xfile)         return false;
  if(!targetname[0]) return false;

  xfile->Erase(targetname);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL_FILE::Reset()
* @brief      Reset
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL_FILE::Reset()
{
  isactive        = false;
  direction       = DIOPROTOCOL_DIRECTION_UNKNOWN;
  size            = 0;
  sizeblock       = 0;
  position        = 0;
  percent         = 0;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL_FILE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL_FILE::Clean()
{
  xfile           = NULL;

  isactive        = false;
  direction       = DIOPROTOCOL_DIRECTION_UNKNOWN;

  size            = 0;
  crc32           = 0;
  sizeblock       = 0;
  position        = 0;
  percent         = 0;
}


#pragma endregion


#pragma region CLASS_DIOPROTOCOL


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL::DIOPROTOCOL(DIOSTREAM* diostream)
* @brief      Constructor
* @ingroup    DATAIO
*
* @param[in]  diostream :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL::DIOPROTOCOL(DIOSTREAM* diostream)
{
  Clean();

  this->diostream   = diostream;

  answers.SetIsMulti(true);
  asks.SetIsMulti(true);

  RegisterEvent(DIOPROTOCOL_XEVENT_TYPE_COMMAND);
  RegisterEvent(DIOPROTOCOL_XEVENT_TYPE_FILEBLOCK);
  RegisterEvent(DIOPROTOCOL_XEVENT_TYPE_ADDALERT);

  GEN_XFACTORY_CREATE(xdatetime         , CreateDateTime())
  GEN_XFACTORY_CREATE(xtimer            , CreateTimer())
  GEN_XFACTORY_CREATE(watchdog          , CreateTimer())
  GEN_XFACTORY_CREATE(xdir              , Create_Dir())
  GEN_XFACTORY_CREATE(timerfileprogress , CreateTimer())

  AddCommand(DIOPROTOCOL_CMDTYPE_SENDISINITSERVERPROTOCOL   , __L("O")                , __L("O")            , RCV_SendIsInitServerProtocol      , __L("SendIsInitServerProtocol")   );
  AddCommand(DIOPROTOCOL_CMDTYPE_GETPROTOCOLVERSION         , __L("")                 , __L("WWW")          , RCV_GetProtocolVersion            , __L("GetProtocolVersion")         );
  AddCommand(DIOPROTOCOL_CMDTYPE_GETAPPLICATIONVERSION      , __L("")                 , __L("WWW")          , RCV_GetApplicationVersion         , __L("GetApplicationVersion")      );
  AddCommand(DIOPROTOCOL_CMDTYPE_GETAPPLICATIONNAME         , __L("")                 , __L("S")            , RCV_GetApplicationName            , __L("GetApplicationName")         );
  AddCommand(DIOPROTOCOL_CMDTYPE_PING                       , __L("")                 , __L("O")            , RCV_Ping                          , __L("Ping")                       );
  AddCommand(DIOPROTOCOL_CMDTYPE_GETDATETIME                , __L("")                 , __L("S")            , RCV_GetDateTime                   , __L("GetDateTime")                );
  AddCommand(DIOPROTOCOL_CMDTYPE_SETDATETIME                , __L("S")                , __L("O")            , RCV_SetDateTime                   , __L("SetDateTime")                );
  AddCommand(DIOPROTOCOL_CMDTYPE_INITFILEBLOCK              , __L("SSBD")             , __L("B")            , RCV_InitFileBlock                 , __L("FileInit")                   );
  AddCommand(DIOPROTOCOL_CMDTYPE_GETFILEBLOCK               , __L("")                 , __L("BDDDBAX")      , RCV_GetFileBlock                  , __L("GetFileBlock")               );
  AddCommand(DIOPROTOCOL_CMDTYPE_SENDFILEBLOCK              , __L("BDDDBAX")          , __L("B")            , RCV_SendFileBlock                 , __L("SendFileBlock")              );
  AddCommand(DIOPROTOCOL_CMDTYPE_DELETEFILE                 , __L("SB")               , __L("O")            , RCV_DeleteFile                    , __L("DeleteFile")                 );
  AddCommand(DIOPROTOCOL_CMDTYPE_MAKEDIR                    , __L("S")                , __L("O")            , RCV_MakeDir                       , __L("MakeDir")                    );
  AddCommand(DIOPROTOCOL_CMDTYPE_DELETEDIR                  , __L("SB")               , __L("O")            , RCV_DeleteDir                     , __L("DeleteDir")                  );
  AddCommand(DIOPROTOCOL_CMDTYPE_GETFIRSTDIRELEMENT         , __L("SS")               , __L("SBDD")         , RCV_GetDirElement                 , __L("GetFirstDirElement")         );
  AddCommand(DIOPROTOCOL_CMDTYPE_GETNEXTDIRELEMENT          , __L("")                 , __L("SBDD")         , RCV_GetDirElement                 , __L("GetNextDirElement")          );
  #ifdef DIO_ALERTS_ACTIVE
  AddCommand(DIOPROTOCOL_CMDTYPE_SENDALERT                  , __L("SSDDDDBSSS")       , __L("B")            , RCV_SendAlert                     , __L("SendAlert")                  );
  #endif

  fileprogress = new DIOPROTOCOL_FILE();
  if(!fileprogress) return;

  GEN_XFACTORY_CREATE(xmutexanswers, Create_Mutex())
  if(!xmutexanswers) return;

  GEN_XFACTORY_CREATE(xmutexasks, Create_Mutex())
  if(!xmutexasks) return;

  GEN_XFACTORY_CREATE(xmutexcommands, Create_Mutex())
  if(!xmutexcommands) return;

  GEN_XFACTORY_CREATE(xmutexreads,Create_Mutex())
  if(!xmutexreads) return;

  GEN_XFACTORY_CREATE(xmutexwrites, Create_Mutex())
  if(!xmutexwrites) return;

  GEN_XFACTORY_CREATE(xmutexfilesprogress, Create_Mutex())
  if(!xmutexfilesprogress) return;

  xthreadreceived = CREATEXTHREAD(XTHREADGROUPID_DIOPROTOCOL, __L("DIOPROTOCOL::DIOPROTOCOL received"),ThreadReceived,(void*)this);
  if(xthreadreceived)
    {
      if(!xthreadreceived->Ini()) return;
    }

  xthreadasks = CREATEXTHREAD(XTHREADGROUPID_DIOPROTOCOL, __L("DIOPROTOCOL::DIOPROTOCOL ask"), ThreadAsks, (void*)this);
  if(xthreadasks)
    {
      if(!xthreadasks->Ini()) return;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL::~DIOPROTOCOL()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL::~DIOPROTOCOL()
{
  DeRegisterEvent(DIOPROTOCOL_XEVENT_TYPE_COMMAND);
  DeRegisterEvent(DIOPROTOCOL_XEVENT_TYPE_FILEBLOCK);
  DeRegisterEvent(DIOPROTOCOL_XEVENT_TYPE_ADDALERT);

  doexit = true;

  if(xthreadreceived)
    {
      xthreadreceived->End();
      DELETEXTHREAD(XTHREADGROUPID_DIOPROTOCOL, xthreadreceived);
      xthreadreceived = NULL;
    }

  if(xthreadasks)
    {
      xthreadasks->End();
      DELETEXTHREAD(XTHREADGROUPID_DIOPROTOCOL, xthreadasks);
      xthreadasks = NULL;
    }

  DeleteAllAnswers();
  DeleteAllAsks();

  DeleteAllCommands();

  GEN_XFACTORY.Delete_Mutex(xmutexanswers);
  GEN_XFACTORY.Delete_Mutex(xmutexasks);
  GEN_XFACTORY.Delete_Mutex(xmutexcommands);
  GEN_XFACTORY.Delete_Mutex(xmutexreads);
  GEN_XFACTORY.Delete_Mutex(xmutexwrites);
  GEN_XFACTORY.Delete_Mutex(xmutexfilesprogress);

  delete fileprogress;

  if(xdir)              GEN_XFACTORY.Delete_Dir(xdir);
  if(watchdog)          GEN_XFACTORY.DeleteTimer(watchdog);
  if(xtimer)            GEN_XFACTORY.DeleteTimer(xtimer);
  if(xdatetime)         GEN_XFACTORY.DeleteDateTime(xdatetime);
  if(timerfileprogress) GEN_XFACTORY.DeleteTimer(timerfileprogress);

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* DIOPROTOCOL::GetDescription()
* @brief      GetDescription
* @ingroup    DATAIO
* 
* @return     XSTRING* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOPROTOCOL::GetDescription()
{
  return &description;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOPROTOCOL::Connect()
* @brief      Connect
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::Connect()
{
  if(!diostream)              return false;
  if(!diostream->GetConfig()) return false;

  if(!diostream->Open())      return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOPROTOCOL::Disconnect()
* @brief      Disconnect
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::Disconnect()
{
  if(!diostream) return false;

  diostream->Disconnect();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOPROTOCOL::Reset()
* @brief      Reset
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::Reset()
{
  if(!Disconnect()) return false;

  return Connect();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOPROTOCOL::IsConnected()
* @brief      IsConnected
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::IsConnected()
{
  if(!diostream)                                                    return false;
  if(diostream->GetConnectStatus()==DIOSTREAMSTATUS_CONNECTED)      return true;

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOPROTOCOL::IsConnecting()
* @brief      IsConnecting
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::IsConnecting()
{
  if(!diostream)                                                       return false;
  if(diostream->GetConnectStatus()==DIOSTREAMSTATUS_GETTINGCONNECTION)  return true;

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOPROTOCOL::IsDisconnected()
* @brief      IsDisconnected
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::IsDisconnected()
{
  if(!diostream)                                                      return false;
  if(diostream->GetConnectStatus()==DIOSTREAMSTATUS_DISCONNECTED)     return true;

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOPROTOCOL::IsInitialized()
* @brief      IsInitialized
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::IsInitialized()
{
  return isinitialized;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOPROTOCOL::SetIsInitialized(bool isinitialized)
* @brief      SetIsInitialized
* @ingroup    DATAIO
* 
* @param[in]  isinitialized : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL::SetIsInitialized(bool isinitialized)
{
  this->isinitialized = isinitialized;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOPROTOCOL::IsServerInitialized()
* @brief      IsServerInitialized
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::IsServerInitialized()
{
  return isserverinitialized;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOPROTOCOL::IsOperational()
* @brief      IsOperational
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::IsOperational()
{
  if(!IsConnected())    return false;
  if(!IsInitialized())  return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOPROTOCOL::GetMaskID()
* @brief      GetMaskID
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOPROTOCOL::GetMaskID()
{
  return maskID;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::SetMaskID(XDWORD maskID)
* @brief      SetMaskID
* @ingroup    DATAIO
*
* @param[in]  maskID :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::SetMaskID(XDWORD maskID)
{
  this->maskID = (maskID&~DIOPROTOCOL_CMDTYPE_ANSWER);
  return (this->maskID > 65535)?false:true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void* DIOPROTOCOL::GetConnectionsManager()
* @brief      GetConnectionsManager
* @ingroup    DATAIO
*
* @return     void* :
*
* --------------------------------------------------------------------------------------------------------------------*/
void* DIOPROTOCOL::GetConnectionsManager()
{
  return connectionsmanager;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL::SetConnectionsManager(void* connectionsmanager)
* @brief      SetConnectionsManager
* @ingroup    DATAIO
*
* @param[in]  connectionsmanager :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL::SetConnectionsManager(void* connectionsmanager)
{
  this->connectionsmanager = connectionsmanager;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void* DIOPROTOCOL::GetConnection()
* @brief      GetConnection
* @ingroup    DATAIO
*
* @return     void* :
*
* --------------------------------------------------------------------------------------------------------------------*/
void* DIOPROTOCOL::GetConnection()
{
  return connection;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL::SetConnection(void* connection)
* @brief      SetConnection
* @ingroup    DATAIO
*
* @param[in]  connection :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL::SetConnection(void* connection)
{
  this->connection = connection;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void* DIOPROTOCOL::GetApplicationData()
* @brief      GetApplicationData
* @ingroup    DATAIO
*
* @return     void* :
*
* --------------------------------------------------------------------------------------------------------------------*/
void* DIOPROTOCOL::GetApplicationData()
{
  return applicationdata;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL::SetApplicationData(void* applicationdata)
* @brief      SetApplicationData
* @ingroup    DATAIO
*
* @param[in]  applicationdata :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL::SetApplicationData(void* applicationdata)
{
  this->applicationdata = applicationdata;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::IsOwnApplicationData()
* @brief      IsOwnApplicationData
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::IsOwnApplicationData()
{
  return isownapplicationdata;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL::SetIsOwnApplicationData(bool isownapplicationdata)
* @brief      SetIsOwnApplicationData
* @ingroup    DATAIO
*
* @param[in]  isownapplicationdata :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL::SetIsOwnApplicationData(bool isownapplicationdata)
{
  this->isownapplicationdata = isownapplicationdata;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSTREAM* DIOPROTOCOL::GetDIOStream()
* @brief      GetDIOStream
* @ingroup    DATAIO
*
* @return     DIOSTREAM* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAM* DIOPROTOCOL::GetDIOStream()
{
  return diostream;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDATETIME* DIOPROTOCOL::GetDateTime()
* @brief      GetDateTime
* @ingroup    DATAIO
*
* @return     XDATETIME* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDATETIME* DIOPROTOCOL::GetDateTime()
{
  return xdatetime;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::AddCommand(XDWORD type,XCHAR* inmask,XCHAR* outmask,DIOPROTOCOL_RECEIVEDFUNC receivedfunc,XCHAR* description)
* @brief      AddCommand
* @ingroup    DATAIO
*
* @param[in]  type :
* @param[in]  inmask :
* @param[in]  outmask :
* @param[in]  receivedfunc :
* @param[in]  description :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::AddCommand(XDWORD type, XCHAR* inmask, XCHAR* outmask, DIOPROTOCOL_RECEIVEDFUNC receivedfunc, XCHAR* description)
{
  DIOPROTOCOL_COMMAND* cmd = new DIOPROTOCOL_COMMAND(type,inmask,outmask,receivedfunc,description);
  if(!cmd) return false;

  commands.Add(cmd);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL_COMMAND* DIOPROTOCOL::GetCommand(XDWORD type)
* @brief      GetCommand
* @ingroup    DATAIO
*
* @param[in]  type :
*
* @return     DIOPROTOCOL_COMMAND* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_COMMAND* DIOPROTOCOL::GetCommand(XDWORD type)
{
  for(int c=0;c<(int)commands.GetSize();c++)
    {
      DIOPROTOCOL_COMMAND* cmd = (DIOPROTOCOL_COMMAND*)commands.Get(c);
      if(cmd)
        {
          if(cmd->GetType() == type) return cmd;
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL_ANSWER* DIOPROTOCOL::AddAnswer(XDWORD type, XDWORD ID, XBUFFER& xbuffer)
* @brief      AddAnswer
* @ingroup    DATAIO
*
* @param[in]  type :
* @param[in]  ID :
* @param[in]  xbuffer :
*
* @return     DIOPROTOCOL_ANSWER* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_ANSWER* DIOPROTOCOL::AddAnswer(XDWORD type, XDWORD ID, XBUFFER& xbuffer)
{
  if(xmutexanswers) xmutexanswers->Lock();

  DIOPROTOCOL_ANSWER* answer = new DIOPROTOCOL_ANSWER(type, ID, xbuffer);
  if(answer) answers.Add(answer);

  if(xmutexanswers) xmutexanswers->UnLock();

  return answer;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL_ANSWER* DIOPROTOCOL::GetAnswer(XDWORD type, XDWORD ID)
* @brief      GetAnswer
* @ingroup    DATAIO
*
* @param[in]  type :
* @param[in]  ID :
*
* @return     DIOPROTOCOL_ANSWER* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_ANSWER* DIOPROTOCOL::GetAnswer(XDWORD type, XDWORD ID)
{
  if(xmutexanswers) xmutexanswers->Lock();

  DIOPROTOCOL_ANSWER* answer = NULL;

  for(XDWORD c=0; c<answers.GetSize(); c++)
    {
      answer = (DIOPROTOCOL_ANSWER*)answers.Get(c);
      if(answer)
        {
          if((answer->GetType() == type) && (answer->GetID()  == ID))  break; else answer = NULL;
        }
    }

  if(xmutexanswers) xmutexanswers->UnLock();

  return answer;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::DeleteAnswer(XDWORD type, XDWORD ID)
* @brief      DeleteAnswer
* @ingroup    DATAIO
*
* @param[in]  type :
* @param[in]  ID :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::DeleteAnswer(XDWORD type, XDWORD ID)
{
  if(xmutexanswers) xmutexanswers->Lock();

  for(XDWORD c=0; c<answers.GetSize(); c++)
    {
      DIOPROTOCOL_ANSWER* answer = (DIOPROTOCOL_ANSWER*)answers.Get(c);
      if(answer)
        {
          if((answer->GetType() == type) && (answer->GetID() == ID))
            {
              answers.Delete(answer);
              delete answer;

              break;
            }
        }
    }

  if(xmutexanswers) xmutexanswers->UnLock();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::DeleteAllAnswers()
* @brief      DeleteAllAnswers
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::DeleteAllAnswers()
{
  if(xmutexanswers) xmutexanswers->Lock();

  bool status = false;

  if(!answers.IsEmpty())
    {
      answers.DeleteContents();
      answers.DeleteAll();

      status = true;
    }

  if(xmutexanswers) xmutexanswers->UnLock();

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL_ANSWER* DIOPROTOCOL::AddAsk(XDWORD type, XDWORD ID, XBUFFER& xbuffer)
* @brief      AddAsk
* @ingroup    DATAIO
*
* @param[in]  type :
* @param[in]  ID :
* @param[in]  xbuffer :
*
* @return     DIOPROTOCOL_ANSWER* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_ANSWER* DIOPROTOCOL::AddAsk(XDWORD type, XDWORD ID, XBUFFER& xbuffer)
{
  if(xmutexasks) xmutexasks->Lock();

  DIOPROTOCOL_ANSWER* ask = new DIOPROTOCOL_ANSWER(type, ID, xbuffer);
  if(ask) asks.Add(ask);

  if(xmutexasks) xmutexasks->UnLock();

  return ask;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL_ANSWER* DIOPROTOCOL::GetAsk(XDWORD index)
* @brief      GetAsk
* @ingroup    DATAIO
*
* @param[in]  index :
*
* @return     DIOPROTOCOL_ANSWER* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_ANSWER* DIOPROTOCOL::GetAsk(XDWORD index)
{
  if(xmutexasks) xmutexasks->Lock();

  DIOPROTOCOL_ANSWER* ask = (DIOPROTOCOL_ANSWER*)asks.Get(index);

  if(xmutexasks) xmutexasks->UnLock();

  return ask;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::DeleteAsk(XDWORD index)
* @brief      DeleteAsk
* @ingroup    DATAIO
*
* @param[in]  index :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::DeleteAsk(XDWORD index)
{
  if(xmutexasks) xmutexasks->Lock();

  DIOPROTOCOL_ANSWER* answer = (DIOPROTOCOL_ANSWER*)asks.Get(index);
  if(answer)
    {
      asks.Delete(answer);
      delete answer;
    }

  if(xmutexasks) xmutexasks->UnLock();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::DeleteAllAsks()
* @brief      DeleteAllAsks
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::DeleteAllAsks()
{
  if(xmutexasks) xmutexasks->Lock();

  bool status =  false;

  if(!asks.IsEmpty())
    {
      asks.DeleteContents();
      asks.DeleteAll();

      status = true;
    }

  if(xmutexasks) xmutexasks->UnLock();

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::ReadCommand(XDWORD& type, XDWORD& ID, XBUFFER& xbuffer)
* @brief      ReadCommand
* @ingroup    DATAIO
*
* @param[in]  type :
* @param[in]  ID :
* @param[in]  xbuffer :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::ReadCommand(XDWORD& type, XDWORD& ID, XBUFFER& xbuffer)
{
  if(!IsConnected()) return 0;

  XDWORD sizecmd = diostream->GetInXBuffer()->GetSize();
  if(sizecmd < DIOPROTOCOL_SIZEOFHEAD) return 0;

  XBYTE head[DIOPROTOCOL_SIZEOFHEAD];

  memset(head ,0 , DIOPROTOCOL_SIZEOFHEAD);
  diostream->GetInXBuffer()->Get(head, DIOPROTOCOL_SIZEOFHEAD);

  HASHCRC32 crc32;
  XDWORD*   headcmd         = (XDWORD*)head;
  XDWORD    IDhead          = 0;
  XDWORD    sizebuffer      = 0;
  XDWORD    bodycrc32value  = 0;
  XDWORD    headcrc32value  = 0;
  XDWORD    crc32value      = 0;

  IDhead          = headcmd[0];
  type            = headcmd[1];
  sizebuffer      = headcmd[2];
  ID              = headcmd[3];
  bodycrc32value  = headcmd[4];
  headcrc32value  = headcmd[5];

  crc32.ResetResult();
  crc32.Do((XBYTE*)headcmd, (DIOPROTOCOL_SIZEOFHEAD - sizeof(XDWORD)));
  crc32value  = crc32.GetResultCRC32();

  if((IDhead != DIOPROTOCOL_MSGIDHEAD) || (headcrc32value != crc32value))
    {
      return 0;
    }

  if(!diostream->WaitToFilledReadingBuffer((DIOPROTOCOL_SIZEOFHEAD + sizebuffer), 5))
    {
      return 0;
    }

  //diostream->GetInXBuffer()->Extract(NULL, 0, DIOPROTOCOL_SIZEOFHEAD);
  diostream->Read(head, DIOPROTOCOL_SIZEOFHEAD);

  if(sizebuffer)
    {
      xbuffer.Delete();
      xbuffer.Resize(sizebuffer);

    //if(diostream->GetInXBuffer()->Extract(xbuffer.Get(), 0, sizebuffer))
      if(diostream->Read(xbuffer.Get(), sizebuffer))
        {
          crc32.ResetResult();
          crc32.Do(xbuffer);
          crc32value  = crc32.GetResultCRC32();

          if(bodycrc32value == crc32value)
            {
              return (DIOPROTOCOL_SIZEOFHEAD + sizebuffer);
            }
           else
            {
              return 0;
            }

        }
       else
        {
          return 0;
        }
    }

  return DIOPROTOCOL_SIZEOFHEAD;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL_RESULT DIOPROTOCOL::Received()
* @brief      Received
* @ingroup    DATAIO
*
* @return     DIOPROTOCOL_RESULT :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_RESULT DIOPROTOCOL::Received()
{
  if(!IsConnected()) return DIOPROTOCOL_RESULT_NOTCONNEX;

  if(xmutexreads) xmutexreads->Lock();

  int                 size = 0;
  DIOPROTOCOL_RESULT  status = DIOPROTOCOL_RESULT_UNKNOWNCMD;

  XDWORD                type   = DIOPROTOCOL_CMDTYPE_NONE;
  XDWORD                ID     = 0;
  XBUFFER               xbuffer;

  size = ReadCommand(type, ID, xbuffer);
  if(size)
    {
      bool   isanswer = ((type&DIOPROTOCOL_CMDTYPE_ANSWER)==DIOPROTOCOL_CMDTYPE_ANSWER)?true:false;
      XDWORD rtype    = (type&~DIOPROTOCOL_CMDTYPE_ANSWER);

      /*
      DIOPROTOCOL_COMMAND* cmd  = GetCommand(rtype);
      if(cmd)
        {
         XTRACE_PRINTCOLOR(((isanswer)?1:2), __L("DIOPROTOCOL %s %s : %08X %08X %02d %-35s (%8d)") , GetDIOStream()->GetConfig()->IsServer()?__L("Server  "):__L("Client  ")
                                                                                                    , (isanswer)?__L("Answer  "):__L("Ask     ")
                                                                                                    , this
                                                                                                    , ID
                                                                                                    , rtype
                                                                                                    , cmd->GetDescription()
                                                                                                    , xbuffer.GetSize());
        }
      */
      if(isanswer)
        {
          if(AddAnswer(rtype, ID, xbuffer)) status = DIOPROTOCOL_RESULT_OK;
        }
       else
        {
          //status = ReceivedHandle(rtype, ID, xbuffer);
          if(AddAsk(rtype, ID, xbuffer))    status = DIOPROTOCOL_RESULT_OK;
        }

    }

  if(xmutexreads) xmutexreads->UnLock();

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL_RESULT DIOPROTOCOL::ReceivedHandle(XDWORD type, XDWORD ID, XBUFFER& xbuffer)
* @brief      ReceivedHandle
* @ingroup    DATAIO
*
* @param[in]  type :
* @param[in]  ID :
* @param[in]  xbuffer :
*
* @return     DIOPROTOCOL_RESULT :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_RESULT DIOPROTOCOL::ReceivedHandle(XDWORD type, XDWORD ID, XBUFFER& xbuffer)
{
  DIOPROTOCOL_RESULT status     = DIOPROTOCOL_RESULT_UNKNOWNCMD;
  XDWORD             param      = 0;

  if((type & DIOPROTOCOL_CMDTYPE_ANSWER) == DIOPROTOCOL_CMDTYPE_ANSWER) return DIOPROTOCOL_RESULT_UNKNOWNCMD;

  DIOPROTOCOL_COMMAND* cmd = GetCommand(type);
  if(!cmd)  return DIOPROTOCOL_RESULT_UNKNOWNCMD;

  DIOPROTOCOL_CONNECTION* connection =  (DIOPROTOCOL_CONNECTION*)GetConnection();

  if(connection) connection->SetInUse(true, DIOPROTOCOL_CONNECTIONSINUSEID_ANSWER);

  if(cmd->GetReceivedFunc())
    {
      int ssize = cmd->GetReceivedFunc()(this, cmd, xbuffer, ID, param);

      if(ssize != DIOPROTOCOL_ERRORRECEIVED)
        {
          if(cmd->GetSOUTMask()->GetSize())
            {
              if(ssize)
                {
                  if(SendCommand(type|DIOPROTOCOL_CMDTYPE_ANSWER, ID, xbuffer)) status = DIOPROTOCOL_RESULT_OK;
                }

            } else status = DIOPROTOCOL_RESULT_OK;
        }
    }

  SendEventCommand(this, DIOPROTOCOL_DIRECTION_GET, type, (status==DIOPROTOCOL_RESULT_OK)?true:false, NULL, param);

  if(connection) connection->SetInUse(false, DIOPROTOCOL_CONNECTIONSINUSEID_ANSWER);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::SendCommand(XDWORD type, XDWORD& ID, XBUFFER& xbuffer)
* @brief      SendCommand
* @ingroup    DATAIO
*
* @param[in]  type :
* @param[in]  ID :
* @param[in]  xbuffer :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::SendCommand(XDWORD type, XDWORD& ID, XBUFFER& xbuffer)
{
  if(!IsConnected()) return false;

  HASHCRC32 crc32;
  XDWORD    crc32head = 0;
  XDWORD    crc32body = 0;

  DIOPROTOCOL_COMMAND* cmd = GetCommand(type&~DIOPROTOCOL_CMDTYPE_ANSWER);
  if(!cmd) return false;

  if(xmutexwrites) xmutexwrites->Lock();

  XBYTE   head[DIOPROTOCOL_SIZEOFHEAD];
  XDWORD* ptr_head  = (XDWORD*)head;

  memset(head, 0, DIOPROTOCOL_SIZEOFHEAD);

  if(xbuffer.GetSize())
    {
      crc32.ResetResult();
      crc32.Do(xbuffer);
      crc32body = crc32.GetResultCRC32();
    }

  if(!ID)
    {
      ID = IDmsgcounter;
      ID |= maskID;

      IDmsgcounter++;
      if(IDmsgcounter > DIOPROTOCOL_CMDTYPE_SECUENCIALLIMIT)  IDmsgcounter = 0;
    }

  ptr_head[0] = DIOPROTOCOL_MSGIDHEAD;
  ptr_head[1] = type;
  ptr_head[2] = xbuffer.GetSize();
  ptr_head[3] = ID;
  ptr_head[4] = crc32body;

  crc32.ResetResult();
  crc32.Do(head, DIOPROTOCOL_SIZEOFHEAD-sizeof(XDWORD));
  crc32head = crc32.GetResultCRC32();

  ptr_head[5] = crc32head;

  bool status = false;

  if(xbuffer.GetSize())
    {
      XBUFFER xbuffersend;

      xbuffersend.Add(head, DIOPROTOCOL_SIZEOFHEAD);
      xbuffersend.Add(xbuffer);

      status = (diostream->Write(xbuffersend) == xbuffersend.GetSize())?true:false;
      if(status) diostream->WaitToFlushOutXBuffer(5);
    }
   else
    {
      status = (diostream->Write(head, DIOPROTOCOL_SIZEOFHEAD) == DIOPROTOCOL_SIZEOFHEAD)?true:false;
      if(status) status = diostream->WaitToFlushOutXBuffer(5);
    }

  if(xmutexwrites) xmutexwrites->UnLock();

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::Command(XDWORD type, XBUFFER& xbuffer, int timeout)
* @brief      Command
* @ingroup    DATAIO
*
* @param[in]  type :
* @param[in]  xbuffer :
* @param[in]  timeout :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::Command(XDWORD type, XBUFFER& xbuffer, int timeout)
{
  if(!IsConnected())  return false;
  if(!watchdog)       return false;

  if(xmutexcommands) xmutexcommands->Lock();

  XDWORD ID     = 0;
  bool   status = false;


  DIOPROTOCOL_COMMAND* cmd = GetCommand(type);
  if(cmd)
    {
      /*
     XTRACE_PRINTCOLOR(1, __L("DIOPROTOCOL %s Command  : %08X %08X %02d %-35s (%8d)") , GetDIOStream()->GetConfig()->IsServer()?__L("Server  "):__L("Client  ")
                                                                                       , this
                                                                                       , ID
                                                                                       , type
                                                                                       , cmd->GetDescription()
                                                                                       , xbuffer.GetSize());
      */
      if(SendCommand(type, ID, xbuffer))
        {
          /*
         XTRACE_PRINTCOLOR(1, __L("DIOPROTOCOL %s Send     : %08X %08X %02d %-35s (%8d)") , GetDIOStream()->GetConfig()->IsServer()?__L("Server  "):__L("Client  ")
                                                                                           , this
                                                                                           , ID
                                                                                           , type
                                                                                           , cmd->GetDescription()
                                                                                           , xbuffer.GetSize());
          */
          xbuffer.Delete();

          //--------------------------------------------
          // No Answer
          if(!cmd->GetSOUTMask()->IsEmpty())
            {
              watchdog->Reset();

              while(1)
                {
                  if(!IsConnected()) break;

                  DIOPROTOCOL_ANSWER* answer = GetAnswer(type, ID);
                  if(answer)
                    {
                      xbuffer.Delete();
                      xbuffer.Add(answer->GetBuffer());

                      DeleteAnswer(type, ID);

                      status = true;
                      break;
                    }

                  if((int)watchdog->GetMeasureSeconds() >= timeout)
                    {
                      break;
                    }

                  GEN_XSLEEP.MilliSeconds(10);
                }

            } else status = true;

          if(!status)
            {
             XTRACE_PRINTCOLOR((status?1:4), __L("DIOPROTOCOL %s  : %08X %08X %02d %-35s (%8d)  %s") , GetDIOStream()->GetConfig()->IsServer()?__L("Server  "):__L("Client  ")
                                                                                                              , this
                                                                                                              , ID
                                                                                                              , type
                                                                                                              , cmd->GetDescription()
                                                                                                              , xbuffer.GetSize()
                                                                                                              , status?__L("Ok"):__L("ERROR!"));
            }
        }

    }

  if(xmutexcommands) xmutexcommands->UnLock();

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::SendEvent(DIOPROTOCOL_XEVENT_TYPE type, DIOPROTOCOL* protocol)
* @brief      SendEvent
* @ingroup    DATAIO
*
* @param[in]  type :
* @param[in]  protocol :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::SendEvent(DIOPROTOCOL_XEVENT_TYPE type, DIOPROTOCOL* protocol)
{
  DIOPROTOCOL_XEVENT xevent(this,type);

  xevent.SetProtocol(protocol);
  PostEvent(&xevent);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::SendEventCommand(DIOPROTOCOL* protocol, DIOPROTOCOL_DIRECTION direction, XDWORD type, bool result, void* param1, XDWORD param2)
* @brief      SendEventCommand
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  direction :
* @param[in]  type :
* @param[in]  result :
* @param[in]  param1 :
* @param[in]  param2 :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::SendEventCommand(DIOPROTOCOL* protocol, DIOPROTOCOL_DIRECTION direction, XDWORD type, bool result, void* param1, XDWORD param2)
{
  DIOPROTOCOL_XEVENT xevent(this,DIOPROTOCOL_XEVENT_TYPE_COMMAND);

  xevent.SetProtocol(protocol);
  xevent.SetDirection(direction);
  xevent.SetType(type);
  xevent.SetResult(result);

  xevent.SetParam1(param1);
  xevent.SetParam2(param2);

  PostEvent(&xevent);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::SendEventFileBlock(DIOPROTOCOL_FILESTATUS filestatus, bool fileissend, XPATH* filesource, XPATH* filetarget, int filepercent, void* param)
* @brief      SendEventFileBlock
* @ingroup    DATAIO
*
* @param[in]  filestatus :
* @param[in]  fileissend :
* @param[in]  filesource :
* @param[in]  filetarget :
* @param[in]  filepercent :
* @param[in]  param :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::SendEventFileBlock(DIOPROTOCOL_FILESTATUS filestatus, bool fileissend, XPATH* filesource, XPATH* filetarget, int filepercent, void* param)
{
  DIOPROTOCOL_XEVENT xevent(this,DIOPROTOCOL_XEVENT_TYPE_FILEBLOCK);

  xevent.SetFileStatus(filestatus);
  xevent.SetFileIsSend(fileissend);
  xevent.SetFileSource(filesource->Get());
  xevent.SetFileTarget(filetarget->Get());
  xevent.SetFilePercent(filepercent);

  xevent.SetParam1(param);

  PostEvent(&xevent);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL::GetProtocolVersion(XDWORD& version, XDWORD& subversion, XDWORD& subversionerr)
* @brief      GetProtocolVersion
* @ingroup    DATAIO
*
* @param[in]  version :
* @param[in]  subversion :
* @param[in]  subversionerr :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL::GetProtocolVersion(XDWORD& version, XDWORD& subversion, XDWORD& subversionerr)
{
  version         = this->protocolversion;
  subversion      = this->protocolsubversion;
  subversionerr   = this->protocolsubversionerr;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL::SetProtocolVersion(XDWORD version, XDWORD subversion, XDWORD subversionerr)
* @brief      SetProtocolVersion
* @ingroup    DATAIO
*
* @param[in]  version :
* @param[in]  subversion :
* @param[in]  subversionerr :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL::SetProtocolVersion(XDWORD version, XDWORD subversion, XDWORD subversionerr)
{
  this->protocolversion           = version;
  this->protocolsubversion        = subversion;
  this->protocolsubversionerr     = subversionerr;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL::GetApplicationVersion(XDWORD& version, XDWORD& subversion, XDWORD& subversionerr)
* @brief      GetApplicationVersion
* @ingroup    DATAIO
*
* @param[in]  version :
* @param[in]  subversion :
* @param[in]  subversionerr :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL::GetApplicationVersion(XDWORD& version, XDWORD& subversion, XDWORD& subversionerr)
{
  version         = this->applicationversion;
  subversion      = this->applicationsubversion;
  subversionerr   = this->applicationsubversionerr;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL::SetApplicationVersion(XDWORD version, XDWORD subversion, XDWORD subversionerr)
* @brief      SetApplicationVersion
* @ingroup    DATAIO
*
* @param[in]  version :
* @param[in]  subversion :
* @param[in]  subversionerr :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL::SetApplicationVersion(XDWORD version, XDWORD subversion, XDWORD subversionerr)
{
  this->applicationversion        = version;
  this->applicationsubversion     = subversion;
  this->applicationsubversionerr  = subversionerr;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOPROTOCOL::GetApplicationName()
* @brief      GetApplicationName
* @ingroup    DATAIO
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOPROTOCOL::GetApplicationName()
{
  return &applicationname;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::SetFileProgress(DIOPROTOCOL_DIRECTION direction, XPATH& sourcename, XPATH& targetname, XDWORD CRC32)
* @brief      SetFileProgress
* @ingroup    DATAIO
*
* @param[in]  direction :
* @param[in]  sourcename :
* @param[in]  targetname :
* @param[in]  CRC32 :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::SetFileProgress(DIOPROTOCOL_DIRECTION direction, XPATH& sourcename, XPATH& targetname, XDWORD CRC32)
{
  if(sourcename.IsEmpty())  return false;

  fileprogress->Reset();

  fileprogress->SetSourceName(sourcename);

  if(targetname.IsEmpty())
         fileprogress->SetTargetName(sourcename);
    else fileprogress->SetTargetName(targetname);

  fileprogress->SetActive(false);
  fileprogress->SetDirection(direction);
  fileprogress->SetPosition(0);

  if(CRC32)
        fileprogress->SetCRC32(CRC32);
   else fileprogress->SetCRC32(CRC32OfFile(sourcename));

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOPROTOCOL_FILE* DIOPROTOCOL::GetFileProgress()
* @brief      GetFileProgress
* @ingroup    DATAIO
*
* @return     DIOPROTOCOL_FILE* :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOPROTOCOL_FILE* DIOPROTOCOL::GetFileProgress()
{
  return fileprogress;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XPATH* DIOPROTOCOL::GetPathSearch()
* @brief      GetPathSearch
* @ingroup    DATAIO
*
* @return     XPATH* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XPATH* DIOPROTOCOL::GetPathSearch()
{
  return &pathsearch;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDIRELEMENT* DIOPROTOCOL::GetSearchElement()
* @brief      GetSearchElement
* @ingroup    DATAIO
*
* @return     XDIRELEMENT* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDIRELEMENT* DIOPROTOCOL::GetSearchElement()
{
  return &searchelement;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDIR* DIOPROTOCOL::GetXDir()
* @brief      GetXDir
* @ingroup    DATAIO
*
* @return     XDIR* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDIR* DIOPROTOCOL::GetXDir()
{
  return xdir;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_GetProtocolVersion(XWORD& version, XWORD& subversion, XWORD& subversionerr)
* @brief      CMD_GetProtocolVersion
* @ingroup    DATAIO
*
* @param[in]  version :
* @param[in]  subversion :
* @param[in]  subversionerr :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_GetProtocolVersion(XWORD& version, XWORD& subversion, XWORD& subversionerr)
{
  bool result = false;

  DIOPROTOCOL_COMMAND* cmd = GetCommand(DIOPROTOCOL_CMDTYPE_GETPROTOCOLVERSION);
  if(!cmd) return false;

  XBUFFER xbuffer;

  if(Command(cmd->GetType(), xbuffer))
    {
      xbuffer.GetWithMask(cmd->GetSOUTMask(), 0, &version, &subversion, &subversionerr);
      result = true;
    }

  XDWORD param = (version<<8)|subversion;

  SendEventCommand(this, DIOPROTOCOL_DIRECTION_GET, cmd->GetType(), result, NULL, param);

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::RCV_GetProtocolVersion(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
* @brief      RCV_GetProtocolVersion
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  cmd :
* @param[in]  xbuffer :
* @param[in]  rID :
* @param[in]  param :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::RCV_GetProtocolVersion(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
{
  xbuffer.Delete();

  return xbuffer.AddWithMask(cmd->GetSOUTMask(),  protocol->protocolversion,  protocol->protocolsubversion,  protocol->protocolsubversionerr);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_GetApplicationVersion(XWORD& version, XWORD& subversion, XWORD& subversionerr)
* @brief      CMD_GetApplicationVersion
* @ingroup    DATAIO
*
* @param[in]  version :
* @param[in]  subversion :
* @param[in]  subversionerr :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_GetApplicationVersion(XWORD& version, XWORD& subversion, XWORD& subversionerr)
{
  bool result = false;

  DIOPROTOCOL_COMMAND* cmd = GetCommand(DIOPROTOCOL_CMDTYPE_GETAPPLICATIONVERSION);
  if(!cmd) return false;

  XBUFFER xbuffer;

  if(Command(cmd->GetType(), xbuffer))
    {
      xbuffer.GetWithMask(cmd->GetSOUTMask(), 0, &version, &subversion, &subversionerr);
      result = true;
    }

  XDWORD param = (version<<8)|subversion;

  SendEventCommand(this, DIOPROTOCOL_DIRECTION_GET, cmd->GetType(), result, NULL, param);

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::RCV_GetApplicationVersion(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
* @brief      RCV_GetApplicationVersion
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  cmd :
* @param[in]  xbuffer :
* @param[in]  rID :
* @param[in]  param :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::RCV_GetApplicationVersion(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
{
  xbuffer.Delete();

  return xbuffer.AddWithMask(cmd->GetSOUTMask(),  protocol->applicationversion,  protocol->applicationsubversion,  protocol->applicationsubversionerr);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_GetApplicationName(XSTRING& applicationname)
* @brief      CMD_GetApplicationName
* @ingroup    DATAIO
*
* @param[in]  applicationname :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_GetApplicationName(XSTRING& applicationname)
{
  DIOPROTOCOL_COMMAND* cmd = GetCommand(DIOPROTOCOL_CMDTYPE_GETAPPLICATIONNAME);
  if(!cmd) return false;

  XBUFFER xbuffer;
  bool    result = false;

  if(Command(cmd->GetType(), xbuffer))
    {
      xbuffer.GetWithMask(cmd->GetSOUTMask(), 0, &applicationname);
      result = true;
    }

  SendEventCommand(this, DIOPROTOCOL_DIRECTION_GET, cmd->GetType(), result, NULL, 0);

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::RCV_GetApplicationName(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
* @brief      RCV_GetApplicationName
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  cmd :
* @param[in]  xbuffer :
* @param[in]  rID :
* @param[in]  param :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::RCV_GetApplicationName(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
{
  xbuffer.Delete();
  return xbuffer.AddWithMask(cmd->GetSOUTMask(), &protocol->applicationname);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_Ping(XDWORD& milliseconds, int timeout)
* @brief      CMD_Ping
* @ingroup    DATAIO
*
* @param[in]  milliseconds :
* @param[in]  timeout :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_Ping(XDWORD& milliseconds, int timeout)
{
  if(!xtimer) return false;

  DIOPROTOCOL_COMMAND* cmd = GetCommand(DIOPROTOCOL_CMDTYPE_PING);
  if(!cmd) return false;

  XBUFFER xbuffer;
  XBYTE result = 0;

  xtimer->Reset();

  if(Command(cmd->GetType(), xbuffer, timeout))
    {
      xbuffer.GetWithMask(cmd->GetSOUTMask(), 0, &result);
    }

  milliseconds = (XDWORD)xtimer->GetMeasureMilliSeconds();

  SendEventCommand(this, DIOPROTOCOL_DIRECTION_SEND, cmd->GetType(),result?true:false,NULL,milliseconds);

  return result?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::RCV_Ping(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
* @brief      RCV_Ping
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  cmd :
* @param[in]  xbuffer :
* @param[in]  rID :
* @param[in]  param :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::RCV_Ping(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
{
  bool result = true;

  xbuffer.Delete();

  return xbuffer.AddWithMask(cmd->GetSOUTMask(), result);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_SendIsInitServerProtocol(bool isinit)
* @brief      CMD_SendIsInitServerProtocol
* @ingroup    DATAIO
*
* @param[in]  isinit :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_SendIsInitServerProtocol(bool isinit)
{
  DIOPROTOCOL_COMMAND* cmd = GetCommand(DIOPROTOCOL_CMDTYPE_SENDISINITSERVERPROTOCOL);
  if(!cmd) return false;

  XBUFFER  xbuffer;
  XBYTE    _isinit    = isinit?1:0;
  bool     result     = false;

  xbuffer.AddWithMask(cmd->GetSINMask(), _isinit);

  if(Command(cmd->GetType(), xbuffer))
    {
      xbuffer.GetWithMask(cmd->GetSOUTMask(), 0, &result);
    }

  SendEventCommand(this, DIOPROTOCOL_DIRECTION_SEND, cmd->GetType(), result, NULL, 0);

  return result;
}

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::RCV_SendIsInitServerProtocol(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
* @brief      RCV_SendIsInitServerProtocol
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  cmd :
* @param[in]  xbuffer :
* @param[in]  rID :
* @param[in]  param :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::RCV_SendIsInitServerProtocol(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
{
  XBYTE  isinit;
  bool   result = false;

  xbuffer.GetWithMask(cmd->GetSINMask(), 0, &isinit);

  if(isinit)
    {
      protocol->isserverinitialized = true;
      result                        = true;
    }

  xbuffer.Delete();

  return xbuffer.AddWithMask(cmd->GetSOUTMask(), result);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_GetDateTime(XSTRING& datestring)
* @brief      CMD_GetDateTime
* @ingroup    DATAIO
*
* @param[in]  datestring :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_GetDateTime(XSTRING& datestring)
{
  DIOPROTOCOL_COMMAND* cmd = GetCommand(DIOPROTOCOL_CMDTYPE_GETDATETIME);
  if(!cmd) return false;

  XBUFFER xbuffer;
  XBYTE   result = 0;

  if(Command(cmd->GetType(), xbuffer))
    {
      xbuffer.GetWithMask(cmd->GetSOUTMask(), 0, &datestring);

      if(!datestring.IsEmpty()) result = 1;
    }

  SendEventCommand(this, DIOPROTOCOL_DIRECTION_GET, cmd->GetType(),result?true:false, NULL, 0);

  return result?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::RCV_GetDateTime(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
* @brief      RCV_GetDateTime
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  cmd :
* @param[in]  xbuffer :
* @param[in]  rID :
* @param[in]  param :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::RCV_GetDateTime(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
{
  XSTRING datestring;

  protocol->GetDateTime()->Read();
  protocol->GetDateTime()->GetDateTimeToString(XDATETIME_FORMAT_STANDARD, datestring);

  xbuffer.Delete();
  return xbuffer.AddWithMask(cmd->GetSOUTMask(), &datestring);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_SetDateTime(XCHAR* datestring)
* @brief      CMD_SetDateTime
* @ingroup    DATAIO
*
* @param[in]  datestring :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_SetDateTime(XCHAR* datestring)
{
  if(!datestring) return false;

  DIOPROTOCOL_COMMAND* cmd = GetCommand(DIOPROTOCOL_CMDTYPE_SETDATETIME);
  if(!cmd) return false;

  XBUFFER  xbuffer;
  XSTRING  _datestring;
  bool     result     = 0;

  _datestring = datestring;

  xbuffer.AddWithMask(cmd->GetSINMask(), &_datestring);

  if(Command(cmd->GetType(), xbuffer))
    {
      xbuffer.GetWithMask(cmd->GetSOUTMask(), 0, &result);
    }

  SendEventCommand(this, DIOPROTOCOL_DIRECTION_SEND, cmd->GetType(), result, NULL, 0);

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::RCV_SetDateTime(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
* @brief      RCV_SetDateTime
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  cmd :
* @param[in]  xbuffer :
* @param[in]  rID :
* @param[in]  param :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::RCV_SetDateTime(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
{
  XSTRING datestring;
  bool    result = false;

  xbuffer.GetWithMask(cmd->GetSINMask(), 0, &datestring);

  if(protocol->GetDateTime()->GetDateTimeFromString(datestring, XDATETIME_FORMAT_STANDARD))
    {
      protocol->GetDateTime()->Write();
      result = true;
    }

  xbuffer.Delete();
  return xbuffer.AddWithMask(cmd->GetSOUTMask(), result);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_File(DIOPROTOCOL_DIRECTION direction, XCHAR* sourcename, XCHAR* targetname, int timeout, void* param)
* @brief      CMD_File
* @ingroup    DATAIO
*
* @param[in]  direction :
* @param[in]  sourcename :
* @param[in]  targetname :
* @param[in]  timeout :
* @param[in]  param :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_File(DIOPROTOCOL_DIRECTION direction, XCHAR* sourcename, XCHAR* targetname, int timeout, void* param)
{
  if(!xtimer)    return false;

  if(!sourcename) return 0;
  if(!targetname) return 0;

  XPATH _sourcename;
  XPATH _targetname;

  _sourcename = sourcename;
  _targetname = targetname;

  if(_sourcename.IsEmpty()) return false;
  if(_targetname.IsEmpty()) return false;

  if(!SetFileProgress(direction,_sourcename,_targetname,0)) return false;

  if(!CMD_InitFileBlock(fileprogress)) return false;

  bool status  = false;

  SendEventFileBlock(DIOPROTOCOL_FILESTATUS_REQUEST , fileprogress->IsActive()
                                                    , fileprogress->GetSourceName()
                                                    , fileprogress->GetTargetName()
                                                    , fileprogress->GetPercent()
                                                    , param);
  xtimer->Reset();

  while(1)
    {
      if(fileprogress->IsActive())
        {
          switch(fileprogress->GetDirection())
            {
              case DIOPROTOCOL_DIRECTION_UNKNOWN  : fileprogress->SetActive(false);     break;
              case DIOPROTOCOL_DIRECTION_GET      : CMD_GetFileBlock(fileprogress);     break;
              case DIOPROTOCOL_DIRECTION_SEND     : CMD_SendFileBlock(fileprogress);    break;
            }
        }

      SendEventFileBlock(DIOPROTOCOL_FILESTATUS_WAIT , fileprogress->IsActive()
                                                     , fileprogress->GetSourceName()
                                                     , fileprogress->GetTargetName()
                                                     , fileprogress->GetPercent()
                                                     , param);

      GEN_XSLEEP.MilliSeconds(10);

      if(!IsConnected())                              break;
      if((int)xtimer->GetMeasureSeconds()>=timeout)   break;
      if(!fileprogress->IsActive())                   break;
    }

  if(fileprogress->GetPercent()>=100)  status = true;

  DIOPROTOCOL_FILESTATUS fstatus = DIOPROTOCOL_FILESTATUS_END;
  if(!status) fstatus = DIOPROTOCOL_FILESTATUS_ERROR;

  SendEventFileBlock(fstatus , fileprogress->IsActive()
                             , fileprogress->GetSourceName()
                             , fileprogress->GetTargetName()
                             , fileprogress->GetPercent()
                             , param);
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_DeleteFile(XCHAR* namefile, bool overwrite)
* @brief      CMD_DeleteFile
* @ingroup    DATAIO
*
* @param[in]  namefile :
* @param[in]  overwrite :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_DeleteFile(XCHAR* namefile, bool overwrite)
{
  if(!namefile) return false;

  DIOPROTOCOL_COMMAND* cmd = GetCommand(DIOPROTOCOL_CMDTYPE_DELETEFILE);
  if(!cmd) return false;

  XBUFFER xbuffer;
  XPATH   _namefile;
  bool    result    = false;

  _namefile = namefile;
  if(_namefile.IsEmpty()) return false;

  xbuffer.AddWithMask(cmd->GetSINMask(), &_namefile, overwrite?1:0);

  if(Command(cmd->GetType(), xbuffer))
    {
      xbuffer.GetWithMask(cmd->GetSOUTMask(), 0, &result);
    }

  SendEventCommand(this, DIOPROTOCOL_DIRECTION_SEND, cmd->GetType(), result, NULL, 0);

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::RCV_DeleteFile(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
* @brief      RCV_DeleteFile
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  cmd :
* @param[in]  xbuffer :
* @param[in]  rID :
* @param[in]  param :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::RCV_DeleteFile(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
{
  XPATH path;
  XBYTE overwrite = 0;
  bool  result    = false;

  xbuffer.GetWithMask(cmd->GetSINMask(), 0, &path, &overwrite);

  XFILE* GEN_XFACTORY_CREATE(xfile, Create_File())
  if(xfile)
    {
      result = xfile->Erase(path,overwrite?true:false)?true:false;
      GEN_XFACTORY.Delete_File(xfile);
    }

  xbuffer.Delete();

  return xbuffer.AddWithMask(cmd->GetSOUTMask(), result);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_MakeDir(XCHAR* namedir)
* @brief      CMD_MakeDir
* @ingroup    DATAIO
*
* @param[in]  namedir :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_MakeDir(XCHAR* namedir)
{
  if(!namedir) return false;

  DIOPROTOCOL_COMMAND* cmd = GetCommand(DIOPROTOCOL_CMDTYPE_MAKEDIR);
  if(!cmd) return false;

  XBUFFER xbuffer;
  XPATH   _namedir;
  bool    result    = false;

  _namedir = namedir;
  if(_namedir.IsEmpty()) return false;

  xbuffer.AddWithMask(cmd->GetSINMask(), &_namedir);

  if(Command(cmd->GetType(), xbuffer))
    {
      xbuffer.GetWithMask(cmd->GetSOUTMask(), 0, &result);
    }

  SendEventCommand(this, DIOPROTOCOL_DIRECTION_SEND, cmd->GetType(), result, NULL, 0);

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::RCV_MakeDir(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
* @brief      RCV_MakeDir
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  cmd :
* @param[in]  xbuffer :
* @param[in]  rID :
* @param[in]  param :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::RCV_MakeDir(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
{
  XPATH path;
  bool  result = 0;

  xbuffer.GetWithMask(cmd->GetSINMask(), 0, &path);

  result = protocol->GetXDir()->Make(path.Get())?true:false;

  xbuffer.Delete();
  return xbuffer.AddWithMask(cmd->GetSOUTMask(), result);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_DeleteDir(XCHAR* namedir, bool all)
* @brief      CMD_DeleteDir
* @ingroup    DATAIO
*
* @param[in]  namedir :
* @param[in]  all :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_DeleteDir(XCHAR* namedir, bool all)
{
  if(!namedir) return false;

  DIOPROTOCOL_COMMAND* cmd = GetCommand(DIOPROTOCOL_CMDTYPE_DELETEDIR);
  if(!cmd) return false;

  XBUFFER xbuffer;
  XPATH   _namedir;
  bool    result    = false;

  _namedir = namedir;
  if(_namedir.IsEmpty()) return false;

  xbuffer.AddWithMask(cmd->GetSINMask(), &_namedir, all?1:0);

  if(Command(cmd->GetType(), xbuffer))
    {
      xbuffer.GetWithMask(cmd->GetSOUTMask(), 0, &result);
    }

  SendEventCommand(this, DIOPROTOCOL_DIRECTION_SEND, cmd->GetType(), result, NULL, 0);

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::RCV_DeleteDir(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
* @brief      RCV_DeleteDir
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  cmd :
* @param[in]  xbuffer :
* @param[in]  rID :
* @param[in]  param :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::RCV_DeleteDir(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
{
  XPATH path;
  XBYTE all   = 0;
  bool  result = 0;

  xbuffer.GetWithMask(cmd->GetSINMask(), 0, &path, &all);

  result = protocol->GetXDir()->Delete(path.Get(),all?true:false);

  xbuffer.Delete();
  return xbuffer.AddWithMask(cmd->GetSOUTMask(), 0, result);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_GetDirElement(XCHAR* pathsearch, XCHAR* mask, XPATH& namefile, XBYTE& type, int& sizefile, XDWORD& CRC32)
* @brief      CMD_GetDirElement
* @ingroup    DATAIO
*
* @param[in]  pathsearch :
* @param[in]  mask :
* @param[in]  namefile :
* @param[in]  type :
* @param[in]  sizefile :
* @param[in]  CRC32 :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_GetDirElement(XCHAR* pathsearch, XCHAR* mask, XPATH& namefile, XBYTE& type, int& sizefile, XDWORD& CRC32)
{
  if(!pathsearch) return false;
  if(!mask)       return false;

  XBUFFER   xbuffer;
  XPATH     _pathsearch;
  XSTRING   _mask;
  bool      result      = false;

  _pathsearch = pathsearch;
  _mask       = mask;

  DIOPROTOCOL_COMMAND* cmd = GetCommand((!_pathsearch.IsEmpty())?DIOPROTOCOL_CMDTYPE_GETFIRSTDIRELEMENT:DIOPROTOCOL_CMDTYPE_GETNEXTDIRELEMENT);
  if(!cmd) return false;

  if(_mask.IsEmpty()) _mask = __L("*");

  xbuffer.AddWithMask(cmd->GetSINMask(), &_pathsearch, &_mask);

  if(Command(cmd->GetType(), xbuffer))
    {
      namefile.Empty();

      xbuffer.GetWithMask(cmd->GetSOUTMask(), 0, &namefile, &type, &sizefile, &CRC32);

      if(!namefile.IsEmpty()) result = true;
    }

  SendEventCommand(this, DIOPROTOCOL_DIRECTION_GET, cmd->GetType(), result?true:false, NULL, 0);

  return result?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::RCV_GetDirElement(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
* @brief      RCV_GetDirElement
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  cmd :
* @param[in]  xbuffer :
* @param[in]  rID :
* @param[in]  param :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::RCV_GetDirElement(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
{
  XPATH     pathsearch;
  XSTRING   masksearch;
  XPATH     pathfile;
  XBYTE     typefile     = 0;
  int       sizefile     = 0;
  XDWORD    crc32value  = 0;
  bool      found       = false;

  pathfile.Empty();

  if(cmd->GetType()==DIOPROTOCOL_CMDTYPE_GETFIRSTDIRELEMENT)
    {
      pathsearch.Empty();

      xbuffer.GetWithMask(cmd->GetSINMask(), 0, &pathsearch, &masksearch);

      protocol->GetPathSearch()->Set(pathsearch);
      protocol->GetSearchElement()->DeleteFindFileData();

      found = protocol->GetXDir()->FirstSearch(pathsearch, masksearch, protocol->GetSearchElement());
    }
   else found =  protocol->GetXDir()->NextSearch(protocol->GetSearchElement());

  if(found)
    {
      typefile    = (XBYTE)protocol->GetSearchElement()->GetType();

      if(protocol->GetSearchElement()->GetType() == XDIRELEMENTTYPE_FILE)
        {
          pathfile  = protocol->GetPathSearch()->Get();
          pathfile += protocol->GetSearchElement()->GetNameFile()->Get();

          HASHCRC32 crc32;

          if(crc32.Do(pathfile, sizefile)) crc32value = crc32.GetResultCRC32();
        }

      pathfile  = protocol->GetSearchElement()->GetNameFile()->Get();
    }

  xbuffer.Delete();
  return xbuffer.AddWithMask(cmd->GetSOUTMask(), &pathfile, typefile, sizefile, crc32value);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::DeleteAllCommands()
* @brief      DeleteAllCommands
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::DeleteAllCommands()
{
  if(commands.IsEmpty())  return false;

  commands.DeleteContents();
  commands.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOPROTOCOL::CRC32OfFile(XPATH& xpath)
* @brief      CRC32OfFile
* @ingroup    DATAIO
*
* @param[in]  xpath :
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOPROTOCOL::CRC32OfFile(XPATH& xpath)
{
  XDWORD value = 0;

  HASHCRC32* crc32 = new HASHCRC32();
  if(!crc32) return false;

  XFILE* GEN_XFACTORY_CREATE(file, Create_File());
  if(file)
    {
      if(file->Open(xpath))
        {
          crc32->ResetResult();
          crc32->Do(file, file->GetSize(), 0);
          value  = crc32->GetResultCRC32();

          file->Close();
        }

      GEN_XFACTORY.Delete_File(file);
    }

  delete crc32;

  return value;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_InitFileBlock(DIOPROTOCOL_FILE* fp)
* @brief      CMD_InitFileBlock
* @ingroup    DATAIO
*
* @param[in]  fp :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_InitFileBlock(DIOPROTOCOL_FILE* fp)
{
  if(!fp) return false;

  DIOPROTOCOL_COMMAND* cmd = GetCommand(DIOPROTOCOL_CMDTYPE_INITFILEBLOCK);
  if(!cmd) return false;

  XBUFFER xbuffer;
  bool    result = false;

  xbuffer.AddWithMask(cmd->GetSINMask(), fp->GetSourceName()
                                       , fp->GetTargetName()
                                       , fp->GetDirection()
                                       , fp->GetCRC32());


  if(Command(cmd->GetType(), xbuffer))
    {
      if(xbuffer.GetWithMask(cmd->GetSOUTMask(), 0, &result))
        {
          if(result) GetFileProgress()->SetActive(true);
        }
    }

  SendEventCommand(this, fp->GetDirection(), cmd->GetType(), result, NULL, 0);

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::RCV_InitFileBlock(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
* @brief      RCV_InitFileBlock
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  cmd :
* @param[in]  xbuffer :
* @param[in]  rID :
* @param[in]  param :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::RCV_InitFileBlock(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
{
  XPATH                  sourcename;
  XPATH                  targetname;
  XBYTE                  _direction   = 0;
  DIOPROTOCOL_DIRECTION  direction    = (DIOPROTOCOL_DIRECTION)_direction;
  XDWORD                 CRC32        = 0;
  bool                   result       = false;

  xbuffer.GetWithMask(cmd->GetSINMask(), 0, &sourcename, &targetname, &_direction, &CRC32);

  direction = (DIOPROTOCOL_DIRECTION)_direction;

  if(protocol->SetFileProgress(direction, sourcename, targetname, CRC32))
    {
      protocol->GetFileProgress()->SetActive(true);
      result = true;
    }

  xbuffer.Delete();
  return xbuffer.AddWithMask(cmd->GetSOUTMask(), result);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_GetFileBlock(DIOPROTOCOL_FILE* fp)
* @brief      CMD_GetFileBlock
* @ingroup    DATAIO
*
* @param[in]  fp :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_GetFileBlock(DIOPROTOCOL_FILE* fp)
{
  DIOPROTOCOL_COMMAND* cmd = GetCommand(DIOPROTOCOL_CMDTYPE_GETFILEBLOCK);
  if(!cmd) return false;

  if(!fp) return false;

  if(!fp->IsActive()) return false;

  XBUFFER xbuffer;
  bool    status = false;

  if(Command(cmd->GetType(), xbuffer))
    {
      XBYTE* filebuffer = new XBYTE[DIOPROTOCOL_MAXFILEBLOCK];
      if(filebuffer)
        {
          XBYTE   islastblock   = 0;
          XDWORD  size          = 0;
          XDWORD  crc32         = 0;
          XDWORD  sizeblock     = DIOPROTOCOL_MAXFILEBLOCK;
          XBYTE   percent       = 0;

          memset(filebuffer, 0, DIOPROTOCOL_MAXFILEBLOCK);

          if(xbuffer.GetWithMask(cmd->GetSOUTMask(), 0  , &islastblock
                                                        , &size
                                                        , &crc32
                                                        , &sizeblock
                                                        , &percent
                                                        , filebuffer
                                                        , &sizeblock))

            {
              fp->SetSize(size);
              fp->SetCRC32(crc32);
              fp->SetSizeBlock(sizeblock);

              if(fp->GetSizeBlock()) status = fp->WriteBlock(filebuffer);

              if(islastblock)
                {
                  XDWORD valuecrc = CRC32OfFile((*fp->GetTargetName()));
                  if(crc32!=valuecrc) fp->Erase();

                  fp->SetActive(false);
                }
            }

          delete[] filebuffer;
        }
    }

  SendEventCommand(this, fp->GetDirection(), cmd->GetType(), status, NULL, 0);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::RCV_GetFileBlock(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
* @brief      RCV_GetFileBlock
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  cmd :
* @param[in]  xbuffer :
* @param[in]  rID :
* @param[in]  param :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::RCV_GetFileBlock(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
{
  DIOPROTOCOL_FILE*   fp;
  int                 ssize  = 0;
  bool                status = false;

  XBYTE* filebuffer = new XBYTE[DIOPROTOCOL_MAXFILEBLOCK];
  if(filebuffer)
    {
      memset(filebuffer, 0, DIOPROTOCOL_MAXFILEBLOCK);

      fp = protocol->GetFileProgress();
      if(fp)
        {
          XBYTE islastblock = 0;

          fp->SetSizeBlock(DIOPROTOCOL_MAXFILEBLOCK);

          fp->ReadBlock(filebuffer);

          //if(fp->GetSizeBlock()!=DIOPROTOCOL_MAXFILEBLOCK)  islastblock = 1;
          if(fp->GetSize() == fp->GetPosition()) islastblock = 1;

          if(fp->GetSizeBlock())
            {
              xbuffer.Delete();
              ssize = xbuffer.AddWithMask(cmd->GetSOUTMask(), islastblock
                                                            , fp->GetSize()
                                                            , fp->GetCRC32()
                                                            , fp->GetSizeBlock()
                                                            , fp->GetPercent()
                                                            , filebuffer
                                                            , fp->GetSizeBlock());



              param  = 1;
              status = true;
            }

          if(islastblock) fp->SetActive(false);
        }

      delete[] filebuffer;
    }

  if(!status) ssize = 0;

  return ssize;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_SendFileBlock(DIOPROTOCOL_FILE* fp)
* @brief      CMD_SendFileBlock
* @ingroup    DATAIO
*
* @param[in]  fp :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_SendFileBlock(DIOPROTOCOL_FILE* fp)
{
  DIOPROTOCOL_COMMAND* cmd = GetCommand(DIOPROTOCOL_CMDTYPE_SENDFILEBLOCK);
  if(!cmd) return false;

  if(!fp) return false;

  if(!fp->IsActive()) return false;

  XBUFFER xbuffer;
  bool    status = false;


  XBYTE* filebuffer = new XBYTE[DIOPROTOCOL_MAXFILEBLOCK];
  if(filebuffer)
    {
      XBYTE islastblock = 0;

      fp->SetSizeBlock(DIOPROTOCOL_MAXFILEBLOCK);

      fp->ReadBlock(filebuffer);

      //if(fp->GetSizeBlock()!=DIOPROTOCOL_MAXFILEBLOCK)  islastblock = 1;
      if(fp->GetSize() == fp->GetPosition()) islastblock = 1;

      if(fp->GetSizeBlock())
        {
          xbuffer.Delete();
          XDWORD size = xbuffer.AddWithMask(cmd->GetSINMask(), islastblock
                                                             , fp->GetSize()
                                                             , fp->GetCRC32()
                                                             , fp->GetSizeBlock()
                                                             , fp->GetPercent()
                                                             , filebuffer
                                                             , fp->GetSizeBlock());


          if(Command(cmd->GetType(), xbuffer))
            {
              XBYTE result = 0;

              if(xbuffer.GetWithMask(cmd->GetSOUTMask(), 0, &result))
                {
                  if(result) status = true;
                }

              if(islastblock) fp->SetActive(false);
            }
        }

      delete[] filebuffer;
    }

  SendEventCommand(this, fp->GetDirection(), cmd->GetType(), status, NULL, 0);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::RCV_SendFileBlock(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
* @brief      RCV_SendFileBlock
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  cmd :
* @param[in]  xbuffer :
* @param[in]  rID :
* @param[in]  param :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::RCV_SendFileBlock(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
{
  XDWORD  filesize    = 0;
  XDWORD  crc32       = 0;
  XDWORD  sizeblock   = DIOPROTOCOL_MAXFILEBLOCK;
  XBYTE percent     = 0;
  int   ssize       = 0;
  bool  status      = false;

  XBYTE* filebuffer = new XBYTE[DIOPROTOCOL_MAXFILEBLOCK];
  if(filebuffer)
    {
      XBYTE islastblock = 0;

      memset(filebuffer,0,DIOPROTOCOL_MAXFILEBLOCK);

      xbuffer.GetWithMask(cmd->GetSINMask(), 0, &islastblock
                                              , &filesize
                                              , &crc32
                                              , &sizeblock
                                              , &percent
                                              , filebuffer
                                              , &sizeblock);

      DIOPROTOCOL_FILE* fp = protocol->GetFileProgress();
      if(fp)
        {
          fp->SetSize(filesize);
          fp->SetCRC32(crc32);
          fp->SetSizeBlock(sizeblock);

          fp->WriteBlock(filebuffer);

          if(fp->GetSizeBlock())
            {
              xbuffer.Delete();
              ssize = xbuffer.AddWithMask(cmd->GetSOUTMask(), fp->GetSizeBlock()?1:0);

              param  = 1;
              status = true;
            }

          if(islastblock)
            {
              XDWORD valuecrc = protocol->CRC32OfFile((*fp->GetTargetName()));
              if(crc32!=valuecrc) fp->Erase();

              fp->SetActive(false);
            }
        }

      delete[] filebuffer;
    }

  if(!status) ssize = 0;

  return ssize;
}


#ifdef DIO_ALERTS_ACTIVE

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOPROTOCOL::CMD_SendAlert(DIOALERT* alert)
* @brief      CMD_SendAlert
* @ingroup    DATAIO
*
* @param[in]  alert :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOPROTOCOL::CMD_SendAlert(DIOALERT* alert)
{
  if(!alert) return false;

  DIOPROTOCOL_COMMAND* cmd = GetCommand(DIOPROTOCOL_CMDTYPE_SENDALERT);
  if(!cmd) return false;

  XBUFFER  xbuffer;
  XSTRING  datetimestr;
  XDWORD   applicationversion;
  XDWORD   applicationsubversion;
  XDWORD   applicationsubversionerr;
  bool     result     = false;

  alert->GetDateTime()->GetDateTimeToString(XDATETIME_FORMAT_STANDARD, datetimestr);
  alert->GetApplicationVersion(applicationversion, applicationsubversion, applicationsubversionerr);

  xbuffer.AddWithMask(cmd->GetSINMask() , &datetimestr
                                        , alert->GetApplicationID()
                                        , applicationversion
                                        , applicationsubversion
                                        , applicationsubversionerr
                                        , alert->GetID()                                        
                                        , (XBYTE)alert->GetLevel()
                                        , alert->GetOrigin()
                                        , alert->GetTitle()
                                        , alert->Get_Message());

  if(Command(cmd->GetType(), xbuffer))
    {
      xbuffer.GetWithMask(cmd->GetSOUTMask(), 0, &result);
    }

  SendEventCommand(this, DIOPROTOCOL_DIRECTION_SEND, cmd->GetType(), result, NULL, 0);

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOPROTOCOL::RCV_SendAlert(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
* @brief      RCV_SendAlert
* @ingroup    DATAIO
*
* @param[in]  protocol :
* @param[in]  cmd :
* @param[in]  xbuffer :
* @param[in]  rID :
* @param[in]  param :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOPROTOCOL::RCV_SendAlert(DIOPROTOCOL* protocol, DIOPROTOCOL_COMMAND* cmd, XBUFFER& xbuffer, XDWORD rID, XDWORD& param)
{
  XSTRING  datetimestr;
  XSTRING  applicationID;
  XDWORD   applicationversion       = 0;
  XDWORD   applicationsubversion    = 0;
  XDWORD   applicationsubversionerr = 0;
  XDWORD   ID                       = 0;
  XDWORD   type                     = 0;
  XBYTE    level                    = 0;
  XSTRING  origin;
  XSTRING  title;
  XSTRING  message;
  bool     result                   = false;

  datetimestr.AdjustSize(_MAXSTR);
  applicationID.AdjustSize(_MAXSTR);
  origin.AdjustSize(_MAXSTR);
  title.AdjustSize(_MAXSTR);
  message.AdjustSize(2048);

  xbuffer.GetWithMask(cmd->GetSINMask(), 0, &datetimestr
                                          , &applicationID
                                          , &applicationversion
                                          , &applicationsubversion
                                          , &applicationsubversionerr
                                          , &ID
                                          , &type
                                          , &level
                                          , &origin
                                          , &title
                                          , &message);

  datetimestr.AdjustSize();
  applicationID.AdjustSize();
  origin.AdjustSize();
  title.AdjustSize();
  message.AdjustSize();

  if((!title.IsEmpty())  && (!origin.IsEmpty()) && (!message.IsEmpty()))
    {
      DIOALERT alert;

      alert.GetDateTime()->GetDateTimeFromString(datetimestr, XDATETIME_FORMAT_STANDARD);
      alert.GetApplicationID()->Set(applicationID);
      alert.SetApplicationVersion(applicationversion, applicationsubversion, applicationsubversionerr);
      alert.SetID(ID);      
      alert.SetLevel((DIOALERTLEVEL)level);
      alert.GetOrigin()->Set(origin);
      alert.GetTitle()->Set(title);
      alert.Get_Message()->Set(message);

      if(protocol->GetApplicationData())
        {
          DIOPROTOCOL_APPLICATIONDATA* applicationdata = (DIOPROTOCOL_APPLICATIONDATA*)protocol->GetApplicationData();
          if(applicationdata)  result = applicationdata->AddAlert(alert);

          if(result)
            {
              DIOPROTOCOL_XEVENT xevent(protocol, DIOPROTOCOL_XEVENT_TYPE_ADDALERT);

              xevent.SetProtocol(protocol);
              xevent.SetAlert(&alert);

              protocol->PostEvent(&xevent);
            }
        }
    }

  xbuffer.Delete();
  return xbuffer.AddWithMask(cmd->GetSOUTMask(), result);
}


#endif


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL::ThreadReceived(void* param)
* @brief      ThreadReceived
* @ingroup    DATAIO
*
* @param[in]  param :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL::ThreadReceived(void* param)
{
  DIOPROTOCOL* protocol = (DIOPROTOCOL*)param;
  if(!protocol) return;

  if(protocol->doexit) return;

  if(!protocol->xthreadreceived)              return;
  if(!protocol->xthreadreceived->IsRunning()) return;

  if(protocol->GetDIOStream())
    {
      if(protocol->IsConnected())
        {
          protocol->Received();
        }
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL::ThreadAsks(void* param)
* @brief      ThreadAsks
* @ingroup    DATAIO
*
* @param[in]  param :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL::ThreadAsks(void* param)
{
  DIOPROTOCOL* protocol = (DIOPROTOCOL*)param;
  if(!protocol) return;

  if(protocol->doexit) return;

  if(!protocol->xthreadreceived)              return;
  if(!protocol->xthreadreceived->IsRunning()) return;

  if(protocol->GetDIOStream())
    {
      if(protocol->IsConnected())
        {
          if(!protocol->asks.IsEmpty())
            {
              DIOPROTOCOL_ANSWER* ask = protocol->GetAsk(0);
              if(ask)
                {
                  XDWORD t = ask->GetType();
                  protocol->ReceivedHandle(ask->GetType(), ask->GetID(), ask->GetBuffer());
                  protocol->DeleteAsk(0);
                }
            }

          //if(protocol->xtimer) protocol->GEN_XSLEEP.MilliSeconds(50);
        }
    }
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOPROTOCOL::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOPROTOCOL::Clean()
{
  diostream                     = NULL;

  protocolversion               = DIOPROTOCOL_VERSION;
  protocolsubversion            = DIOPROTOCOL_SUBVERSION;
  protocolsubversionerr         = DIOPROTOCOL_SUBVERSIONERR;

  applicationversion            = 0;
  applicationsubversion         = 0;
  applicationsubversionerr      = 0;

  isinitialized                 = false;
  isserverinitialized           = false;

  xdatetime                     = NULL;
  xtimer                        = NULL;
  watchdog                      = NULL;

  xdir                          = NULL;

  connectionsmanager             = NULL;
  connection                     = NULL;
  applicationdata               = NULL;
  isownapplicationdata          = false;

  maskID                        = 0;

  IDmsgcounter                  = 1;
  timerfileprogress             = NULL;

  xdir                          = NULL;

  doexit                        = false;

  xmutexanswers                 = NULL;
  xmutexasks                    = NULL;
  xmutexcommands                = NULL;
  xmutexreads                   = NULL;
  xmutexwrites                  = NULL;
  xmutexfilesprogress           = NULL;

  xthreadreceived               = NULL;
  xthreadasks                   = NULL;
}


#pragma endregion


#pragma endregion

