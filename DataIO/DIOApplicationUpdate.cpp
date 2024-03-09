/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOApplicationUpdate.cpp
* 
* @class      DIOAPPLICATIONUPDATE
* @brief      Data Input/Output Application Update class
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

#include "DIOApplicationUpdate.h"

#include "XFactory.h"
#include "XPath.h"
#include "XPathsManager.h"
#include "XDir.h"
#include "XFileINI.h"
#include "XFileZIP.h"
#include "XString.h"
#include "XSystem.h"
#include "XProcessManager.h"
#include "XTrace.h"

#include "HashCRC32.h"

#include "DIOFactory.h"
#include "DIOURL.h"
#include "DIOWebClient.h"
#include "DIOApplicationUpdate_XEvent.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


#pragma region CLASS_DIOAPPLICATIONUPDATE_VERSIONDATA


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOAPPLICATIONUPDATE_VERSIONDATA::DIOAPPLICATIONUPDATE_VERSIONDATA()
* @brief      Constructor
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOAPPLICATIONUPDATE_VERSIONDATA::DIOAPPLICATIONUPDATE_VERSIONDATA()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOAPPLICATIONUPDATE_VERSIONDATA::~DIOAPPLICATIONUPDATE_VERSIONDATA()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOAPPLICATIONUPDATE_VERSIONDATA::~DIOAPPLICATIONUPDATE_VERSIONDATA()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOAPPLICATIONUPDATE_VERSIONDATA::GetVersion()
* @brief      GetVersion
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOAPPLICATIONUPDATE_VERSIONDATA::GetVersion()
{
  return version;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOAPPLICATIONUPDATE_VERSIONDATA::SetVersion(XDWORD version)
* @brief      SetVersion
* @ingroup    DATAIO
*
* @param[in]  version :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOAPPLICATIONUPDATE_VERSIONDATA::SetVersion(XDWORD version)
{
  this->version = version;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOAPPLICATIONUPDATE_VERSIONDATA::GetSubVersion()
* @brief      GetSubVersion
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOAPPLICATIONUPDATE_VERSIONDATA::GetSubVersion()
{
  return subversion;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOAPPLICATIONUPDATE_VERSIONDATA::SetSubVersion(XDWORD subversion)
* @brief      SetSubVersion
* @ingroup    DATAIO
*
* @param[in]  subversion :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOAPPLICATIONUPDATE_VERSIONDATA::SetSubVersion(XDWORD subversion)
{
  this->subversion = subversion;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOAPPLICATIONUPDATE_VERSIONDATA::GetSubVersionError()
* @brief      GetErrorControl
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOAPPLICATIONUPDATE_VERSIONDATA::GetSubVersionError()
{
  return subversionerror;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOAPPLICATIONUPDATE_VERSIONDATA::SetSubVersionError(XDWORD subversionerror)
* @brief      SetErrorControl
* @ingroup    DATAIO
*
* @param[in]  subversionerror :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOAPPLICATIONUPDATE_VERSIONDATA::SetSubVersionError(XDWORD subversionerror)
{
  this->subversionerror = subversionerror;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE_VERSIONDATA::SystemMustBeInit()
* @brief      SystemMustBeInit
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE_VERSIONDATA::SystemMustBeInit()
{
  return systemmustbeinit;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE_VERSIONDATA::CopyTo(DIOAPPLICATIONUPDATE_VERSIONDATA& versiondata)
* @brief      CopyTo
* @ingroup    DATAIO
*
* @param[in]  versiondata :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE_VERSIONDATA::CopyTo(DIOAPPLICATIONUPDATE_VERSIONDATA& versiondata)
{
  versiondata.SetVersion(version);
  versiondata.SetSubVersion(subversion);
  versiondata.SetSubVersionError(subversionerror);
  versiondata.SetSystemMustBeInit(systemmustbeinit);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE_VERSIONDATA::CopyFrom(DIOAPPLICATIONUPDATE_VERSIONDATA* versiondata)
* @brief      CopyFrom
* @ingroup    DATAIO
*
* @param[in]  versiondata :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE_VERSIONDATA::CopyFrom(DIOAPPLICATIONUPDATE_VERSIONDATA* versiondata)
{
  if(!versiondata) return false;

  version           = versiondata->GetVersion();
  subversion        = versiondata->GetSubVersion();
  subversionerror   = versiondata->GetSubVersionError();
  systemmustbeinit  = versiondata->SystemMustBeInit();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOAPPLICATIONUPDATE_VERSIONDATA::Compare(XDWORD version, XDWORD subversion, XDWORD subversionerror)
* @brief      Compare
* @ingroup    DATAIO
*
* @param[in]  version :
* @param[in]  subversion :
* @param[in]  subversionerror :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int DIOAPPLICATIONUPDATE_VERSIONDATA::Compare(XDWORD version, XDWORD subversion, XDWORD subversionerror)
{
  // Equal;
  if((GetVersion() == version) && (GetSubVersion() == subversion) && (GetSubVersionError() == subversionerror)) return 0;

  bool ismayorversion = false;

  if(GetVersion() > version)
    {
      ismayorversion = true;
    }
   else
    {
      if(GetVersion() == version)
        {
          if(GetSubVersion() > subversion)
            {
              ismayorversion = true;
            }
           else
            {
              if(GetSubVersion() == subversion)
                {
                  if(GetSubVersionError() > subversionerror)  ismayorversion = true;
                }
            }
        }
    }

  return ismayorversion?1:-1;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOAPPLICATIONUPDATE_VERSIONDATA::SetSystemMustBeInit(bool applicationmustbeinit)
* @brief      SetSystemMustBeInit
* @ingroup    DATAIO
*
* @param[in]  applicationmustbeinit :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOAPPLICATIONUPDATE_VERSIONDATA::SetSystemMustBeInit(bool applicationmustbeinit)
{
  this->systemmustbeinit = applicationmustbeinit;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOAPPLICATIONUPDATE_VERSIONDATA::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOAPPLICATIONUPDATE_VERSIONDATA::Clean()
{
  version                 = 0;
  subversion              = 0;
  subversionerror         = 0;
  systemmustbeinit        = false;
}


#pragma endregion


#pragma region CLASS_DIOAPPLICATIONUPDATE_FILE


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOAPPLICATIONUPDATE_FILE::DIOAPPLICATIONUPDATE_FILE(XPATH& xpathfile, XDWORD CRC32, XDWORD size)
* @brief      Constructor
* @ingroup    DATAIO
*
* @param[in]  XPATH& :
* @param[in]   XDWORD CRC32 :
* @param[in]   XDWORD size :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOAPPLICATIONUPDATE_FILE::DIOAPPLICATIONUPDATE_FILE(XPATH& xpathfile, XDWORD CRC32, XQWORD size)
{
  Clean();

  this->xpathfile = xpathfile;
  this->CRC32     = CRC32;
  this->size      = size;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOAPPLICATIONUPDATE_FILE::~DIOAPPLICATIONUPDATE_FILE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOAPPLICATIONUPDATE_FILE::~DIOAPPLICATIONUPDATE_FILE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XPATH* DIOAPPLICATIONUPDATE_FILE::GetXPathFile()
* @brief      GetXPathFile
* @ingroup    DATAIO
*
* @return     XPATH* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XPATH* DIOAPPLICATIONUPDATE_FILE::GetXPathFile()
{
  return &xpathfile;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOAPPLICATIONUPDATE_FILE::GetCRC32()
* @brief      GetCRC32
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOAPPLICATIONUPDATE_FILE::GetCRC32()
{
  return CRC32;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int DIOAPPLICATIONUPDATE_FILE::GetSize()
* @brief      GetSize
* @ingroup    DATAIO
*
* @return     XQWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XQWORD DIOAPPLICATIONUPDATE_FILE::GetSize()
{
  return size;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE_FILE::HasSpecialControlUpdate()
* @brief      HasSpecialControlUpdate
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE_FILE::HasSpecialControlUpdate()
{
  return specialcontrolupdate;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOAPPLICATIONUPDATE_FILE::SetHasSpecialControlUpdate(bool specialcontrolupdate)
* @brief      SetHasSpecialControlUpdate
* @ingroup    DATAIO
*
* @param[in]  specialcontrolupdate :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOAPPLICATIONUPDATE_FILE::SetHasSpecialControlUpdate(bool specialcontrolupdate)
{
  this->specialcontrolupdate = specialcontrolupdate;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOAPPLICATIONUPDATE_FILE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOAPPLICATIONUPDATE_FILE::Clean()
{
  CRC32                 = 0;
  size                  = 0;
  specialcontrolupdate  = false;
}


#pragma endregion


#pragma region CLASS_DIOAPPLICATIONUPDATE


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOAPPLICATIONUPDATE::DIOAPPLICATIONUPDATE(XDWORD applicationversion, XDWORD applicationsubversion, XDWORD applicationsubversionerror, XCHAR* applicationname, XPATH& xpathrootapp)
* @brief      Constructor
* @ingroup    DATAIO
*
* @param[in]  applicationversion :
* @param[in]  applicationsubversion :
* @param[in]  applicationsubversionerror :
* @param[in]  applicationname :
* @param[in]  xpathrootapp :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOAPPLICATIONUPDATE::DIOAPPLICATIONUPDATE(XDWORD applicationversion, XDWORD applicationsubversion, XDWORD applicationsubversionerror, XCHAR* applicationname, XPATH& xpathrootapp)
{
  Clean();

  this->applicationversion          = applicationversion;
  this->applicationsubversion       = applicationsubversion;
  this->applicationsubversionerror  = applicationsubversionerror;

  this->applicationname             = applicationname;
  this->xpathrootapp                = xpathrootapp;

  #ifndef GEN_DEBUG
  DeleteFilesRenamed(xpathrootapp);
  #endif

  GEN_XFACTORY_CREATE(xdir, Create_Dir())
  webclient = new DIOWEBCLIENT;

  RegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_GETVERSIONAVAILABLE);
  RegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_CHECKVERSIONAVAILABLE);
  RegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_DOWNLOADFILE);
  RegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_UNZIPFILE);
  RegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_BACKUPORIGINALFILE);
  RegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_BACKUPORIGINALFILES);
  RegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_UPDATEFILE);
  RegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_RESTOREUPDATEFILE);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOAPPLICATIONUPDATE::~DIOAPPLICATIONUPDATE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOAPPLICATIONUPDATE::~DIOAPPLICATIONUPDATE()
{
  DeRegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_GETVERSIONAVAILABLE);
  DeRegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_CHECKVERSIONAVAILABLE);
  DeRegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_DOWNLOADFILE);
  DeRegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_UNZIPFILE);
  DeRegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_BACKUPORIGINALFILE);
  DeRegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_BACKUPORIGINALFILES);
  DeRegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_UPDATEFILE);
  DeRegisterEvent(DIOAPPLICATIONUPDATE_XEVENT_TYPE_RESTOREUPDATEFILE);

  FilesToUpdate_DeleteList();

  if(xdir)      GEN_XFACTORY.Delete_Dir(xdir);
  if(webclient) delete webclient;

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::GetVersionAvailable(DIOURL& url, int port, DIOAPPLICATIONUPDATE_VERSIONDATA& updateversiondata, int timeout, DIOAPPLICATIONUPDATE_ERROR& error)
* @brief      GetVersionAvailable
* @ingroup    DATAIO
*
* @param[in]  url :
* @param[in]  port :
* @param[in]  updateversiondata :
* @param[in]  timeout :
* @param[in]  error :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::GetVersionAvailable(DIOURL& url, int port, DIOAPPLICATIONUPDATE_VERSIONDATA& updateversiondata, int timeout, DIOAPPLICATIONUPDATE_ERROR& error)
{
  error  = DIOAPPLICATIONUPDATE_ERROR_INVALIDOPERATION;

  if(!webclient) return false;

  bool  status = false;

  DIOURL urlfile;
  XPATH  xpath;

  urlfile  = url;
  urlfile.Slash_Add();
  urlfile += DIOAPPLICATIONUPDATE_INIFILE;

  urlfile.Slash_Normalize();

  GEN_XPATHSMANAGER.GetPathOfSection(XPATHSMANAGERSECTIONTYPE_ROOT, xpath);
  xpath.Slash_Add();
  xpath += DIOAPPLICATIONUPDATE_INIFILE;

  webclient->Set_Port(port);

  status = webclient->Get(urlfile, xpath ,NULL, timeout);
  if(!status)
    {
      if(stopprocess)
        {
          error = DIOAPPLICATIONUPDATE_ERROR_BREAKFORUSER;
        }
       else
        {
          error = DIOAPPLICATIONUPDATE_ERROR_DOWNLOADFILE;
        }
    }
   else
    {
      error = DIOAPPLICATIONUPDATE_ERROR_INVALIDOPERATION;

      XFILEINI* ini = new XFILEINI();
      if(ini)
        {
          if(ini->Open(xpath))
            {
              XSTRING key;
              XSTRING value;
              int     index;

              if(ini->ReadValue(DIOAPPLICATIONUPDATE_GENERALSECTION , DIOAPPLICATIONUPDATE_GENERALSECTION_VERSION           , value))  updateversiondata.SetVersion(value.ConvertToInt());
              if(ini->ReadValue(DIOAPPLICATIONUPDATE_GENERALSECTION , DIOAPPLICATIONUPDATE_GENERALSECTION_SUBVERSION        , value))  updateversiondata.SetSubVersion(value.ConvertToInt());
              if(ini->ReadValue(DIOAPPLICATIONUPDATE_GENERALSECTION , DIOAPPLICATIONUPDATE_GENERALSECTION_ERRORCONTROL      , value))  updateversiondata.SetSubVersionError(value.ConvertToInt());
              if(ini->ReadValue(DIOAPPLICATIONUPDATE_GENERALSECTION , DIOAPPLICATIONUPDATE_GENERALSECTION_SYSTEMMUSTBEINIT  , value))  updateversiondata.SetSystemMustBeInit(value.ConvertToBoolean());

              //----------------------------------------------------------------------------------------

              FilesToUpdate_DeleteList();

              index = 0;

              do{ key.Format(__L("%s%d"), DIOAPPLICATIONUPDATE_FILESSECTION_FILES, index);

                  ini->ReadValue(DIOAPPLICATIONUPDATE_FILESSECTION, key, value);
                  if(!value.IsEmpty())
                    {
                      XPATH     xpathfile(_MAXSTR);
                      int       sizefile = 0;
                      XDWORD    CRC32    = 0;

                      int comma=value.FindCharacter(__C(','));
                      XSTRING name;
                      value.Copy(0,comma,name);
                      value.DeleteCharacters(0,comma+1);
                      value.UnFormat(__L("%d,%08X"), &sizefile, &CRC32);
                      xpathfile.Set(name.Get());

                      if(xpathfile.GetSize() && sizefile && CRC32)
                        {
                          DIOAPPLICATIONUPDATE_FILE* updatefile = new DIOAPPLICATIONUPDATE_FILE(xpathfile, CRC32, sizefile);
                          if(updatefile) filestoupdate.Add(updatefile);
                        }
                    }

                  index++;

                } while(index<DIOAPPLICATIONUPDATE_MAXINDEXFILES);

              //----------------------------------------------------------------------------------------

              ini->Close();
              error = DIOAPPLICATIONUPDATE_NOT_ERROR;
            }

          delete ini;
       }
    }

  //----------------------------------------------------------------------------------------
  DIOAPPLICATIONUPDATE_XEVENT xevent(this, DIOAPPLICATIONUPDATE_XEVENT_TYPE_GETVERSIONAVAILABLE);

  xevent.SetVersionData(&updateversiondata);
  xevent.SetFilesToUpdate(&filestoupdate);
  xevent.SetError(error);

  PostEvent(&xevent);
  //----------------------------------------------------------------------------------------

  XFILE* GEN_XFACTORY_CREATE(file, Create_File())
  if(file)
    {
      file->Erase(xpath);
      GEN_XFACTORY.Delete_File(file);
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::Download(DIOURL& url, int port, XCHAR* addtargetpath, DIOAPPLICATIONUPDATE_VERSIONDATA& updateversiondata, int timeout, DIOAPPLICATIONUPDATE_ERROR& error)
* @brief      Download
* @ingroup    DATAIO
*
* @param[in]  url :
* @param[in]  port :
* @param[in]  addtargetpath :
* @param[in]  updateversiondata :
* @param[in]  timeout :
* @param[in]  error :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::Download(DIOURL& url, int port, XCHAR* addtargetpath, DIOAPPLICATIONUPDATE_VERSIONDATA& updateversiondata, int timeout, DIOAPPLICATIONUPDATE_ERROR& error)
{
  error  = DIOAPPLICATIONUPDATE_NOT_ERROR;

  if(!xdir) return false;

  XPATH   namefile;
  XPATH   extenxion;
  DIOURL  urlfile;
  int     c;
  bool    status = false;

  if(!filestoupdate.GetSize())
    {
      error = DIOAPPLICATIONUPDATE_ERROR_NOTFILES;
      return false;
    }

  for(c=0;c<(int)filestoupdate.GetSize();c++)
    {
      XPATH xpathfile;

      progressfiletoupdate = c;

      DIOAPPLICATIONUPDATE_FILE* updatefile = (DIOAPPLICATIONUPDATE_FILE*)filestoupdate.Get(c);
      if(!updatefile)  continue;

      urlfile  = url;
      urlfile.Slash_Add();
      urlfile += updatefile->GetXPathFile()->Get();
      urlfile.Slash_Normalize();

      xpathfile.Empty();

      if(addtargetpath)
        {
          if(addtargetpath[0])
            {
              xpathfile = addtargetpath;
              xpathfile.Slash_Add();
            }
        }

      xpathfile    += updatefile->GetXPathFile()->Get();
      xpathfile.Slash_Normalize();

      if(!xpathfile.IsEmpty())
        {
          XPATH xpath;

          xpathfile.GetDriveAndPath(xpath);

          if(!xpath.IsEmpty())
            {
              if(xdir) xdir->Make(xpath);
            }
        }

      xpathfile.GetNamefileExt(namefile);
      xpathfile.GetExt(extenxion);

      HASHCRC32 hashCRC32;
      
      hashCRC32.ResetResult();
      hashCRC32.Do(xpathfile);

      if(updatefile->GetCRC32() != hashCRC32.GetResultCRC32())
        {
          webclient->Set_Port(port);

          status = webclient->Get(urlfile, xpathfile, NULL, timeout);
          if(!status)
            {
              if(stopprocess)
                        error = DIOAPPLICATIONUPDATE_ERROR_BREAKFORUSER;
                  else  error = DIOAPPLICATIONUPDATE_ERROR_DOWNLOADFILE;

              break;
            }

          hashCRC32.ResetResult();
          hashCRC32.Do(xpathfile);

          if(updatefile->GetCRC32() != hashCRC32.GetResultCRC32())
            {
              error = DIOAPPLICATIONUPDATE_ERROR_DOWNLOADFILE;
              status = false;
              break;

            } else status = true;

        } else status = true;


      DIOAPPLICATIONUPDATE_XEVENT xevent(this, DIOAPPLICATIONUPDATE_XEVENT_TYPE_DOWNLOADFILE);

      xevent.SetVersionData(&updateversiondata);
      xevent.SetFilesToUpdate(&filestoupdate);
      xevent.SetIndexOfOperation(c);
      xevent.SetError(error);

      PostEvent(&xevent);


      //-------------------------------------------------------------
      // Descompress File
      if(!extenxion.Compare(__L(".zip"), true))
        {
          XFILEUNZIP* unzip = new XFILEUNZIP();
          if(unzip)
            {
              if(unzip->Open(xpathfile))
                {
                  XPATH xpath;
                  int   sourceindex = 0;

                  xpath = xpathfile;
                  xpath.SetOnlyDriveAndPath();

                  status = true;

                  do{ XFILECMPZIP* xfilecmp =  unzip->GotoFile(sourceindex);
                      if(!xfilecmp) break;

                      XPATH xpathcomplete;

                      xpathcomplete  = xpath;
                      xpathcomplete += xfilecmp->GetName();

                      if(xfilecmp->IsDirectory())
                        {
                          xdir->Make(xpathcomplete);
                          status = true;
                        }
                       else
                        {
                          hashCRC32.ResetResult();
                          hashCRC32.Do(xpathcomplete);

                          if(xfilecmp->GetCRC32() != hashCRC32.GetResultCRC32())
                            {
                              status = unzip->DecompressFile(sourceindex, xpath);

                            } else status = true;

                          DIOAPPLICATIONUPDATE_XEVENT xevent(this, DIOAPPLICATIONUPDATE_XEVENT_TYPE_UNZIPFILE);

                          xevent.SetVersionData(&updateversiondata);
                          xevent.SetFilesToUpdate(&filestoupdate);
                          xevent.SetIndexOfOperation(c);
                          xevent.GetActualUnzipFileName()->Set(xfilecmp->GetName());
                          xevent.SetError(status?DIOAPPLICATIONUPDATE_NOT_ERROR:DIOAPPLICATIONUPDATE_ERROR_INVALIDOPERATION);

                          PostEvent(&xevent);
                        }

                      sourceindex++;

                      delete xfilecmp;

                    } while(status);

                  unzip->Close();

                  XFILE* GEN_XFACTORY_CREATE(file, Create_File())
                  if(file)
                    {
                      file->Erase(xpathfile);
                      GEN_XFACTORY.Delete_File(file);
                    }
                }

              delete unzip;
            }
        }

      //-------------------------------------------------------------
    }

  if(!status)  error = DIOAPPLICATIONUPDATE_ERROR_INVALIDOPERATION;

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::Download(DIOURL& url, int port, XPATH& addtargetpath, DIOAPPLICATIONUPDATE_VERSIONDATA& updateversiondata, int timeout, DIOAPPLICATIONUPDATE_ERROR& error)
* @brief      Download
* @ingroup    DATAIO
*
* @param[in]  url :
* @param[in]  port :
* @param[in]  addtargetpath :
* @param[in]  updateversiondata :
* @param[in]  timeout :
* @param[in]  error :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::Download(DIOURL& url, int port, XPATH& addtargetpath, DIOAPPLICATIONUPDATE_VERSIONDATA& updateversiondata, int timeout, DIOAPPLICATIONUPDATE_ERROR& error)
{
  return Download(url, port, addtargetpath.Get(), updateversiondata, timeout, error);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::CopyOriginalFilesToBackup(XPATH& xpathbackup, DIOAPPLICATIONUPDATE_ERROR& error)
* @brief      CopyOriginalFilesToBackup
* @ingroup    DATAIO
*
* @param[in]  xpathbackup :
* @param[in]  error :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::CopyOriginalFilesToBackup(XPATH& xpathbackup, DIOAPPLICATIONUPDATE_ERROR& error)
{
  error = DIOAPPLICATIONUPDATE_NOT_ERROR;

  if(!filestoupdate.GetSize())
    {
      error = DIOAPPLICATIONUPDATE_ERROR_NOTFILES;
      return false;
    }

  bool status        = false;

  for(XDWORD c=0; c<filestoupdate.GetSize(); c++)
    {
      XPATH   xpathbackupadd;
      XPATH   xpathrootappadd;
      XSTRING drivepath;
      bool    origenfilefound = false;

      xpathbackupadd   = xpathbackup;
      xpathbackupadd.Slash_Add();
      xpathbackupadd  += filestoupdate.Get(c)->GetXPathFile()->Get();

      xpathrootappadd  = xpathrootapp;
      xpathrootappadd.Slash_Add();
      xpathrootappadd += filestoupdate.Get(c)->GetXPathFile()->Get();

      XFILE* GEN_XFACTORY_CREATE(xfile, Create_File())
      if(xfile)
        {
          if(xfile->Open(xpathrootappadd))
            {
              origenfilefound = true;
              xfile->Close();
            }

          GEN_XFACTORY.Delete_File(xfile);
        }

      if(origenfilefound)
        {
          xpathbackupadd.GetDriveAndPath(drivepath);

          XDIR* GEN_XFACTORY_CREATE(xdir, Create_Dir())
          if(xdir)
            {
              xdir->Make(drivepath.Get());

              GEN_XFACTORY.Delete_Dir(xdir);
            }

          status = CopyFile(xpathrootappadd, xpathbackupadd);

          //----------------------------------------------------------------
          DIOAPPLICATIONUPDATE_XEVENT xevent(this, DIOAPPLICATIONUPDATE_XEVENT_TYPE_BACKUPORIGINALFILE);

          xevent.SetFilesToUpdate(&filestoupdate);
          xevent.SetIndexOfOperation(c);
          xevent.SetError(status?DIOAPPLICATIONUPDATE_NOT_ERROR:DIOAPPLICATIONUPDATE_ERROR_INVALIDOPERATION);

          PostEvent(&xevent);
          //----------------------------------------------------------------

          if(!status) break;

        }
    }

  if(!status) error = DIOAPPLICATIONUPDATE_ERROR_INVALIDOPERATION;

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::CopyUpdateFilesToOriginal(XPATH& xpathupdate, DIOAPPLICATIONUPDATE_ERROR& error, bool restore)
* @brief      CopyUpdateFilesToOriginal
* @ingroup    DATAIO
*
* @param[in]  xpathupdate :
* @param[in]  error :
* @param[in]  restore :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::CopyUpdateFilesToOriginal(XPATH& xpathupdate, DIOAPPLICATIONUPDATE_ERROR& error, bool restore)
{
  error = DIOAPPLICATIONUPDATE_NOT_ERROR;

  if(!filestoupdate.GetSize())
    {
      error = DIOAPPLICATIONUPDATE_ERROR_NOTFILES;
      return false;
    }

  bool status = false;

  for(XDWORD c=0; c<filestoupdate.GetSize(); c++)
    {
      XPATH   xpathupdateadd;
      XPATH   xpathrootappadd;
      XPATH   xpathrootappaddrename;
      XSTRING drivepath;

      xpathupdateadd   = xpathupdate;
      xpathupdateadd.Slash_Add();
      xpathupdateadd  += filestoupdate.Get(c)->GetXPathFile()->Get();

      xpathrootappadd  = xpathrootapp;
      xpathrootappadd.Slash_Add();
      xpathrootappadd += filestoupdate.Get(c)->GetXPathFile()->Get();

      xpathrootappadd.GetDriveAndPath(drivepath);

      XDIR* GEN_XFACTORY_CREATE(xdir, Create_Dir())
      if(xdir)
        {
          xdir->Make(drivepath.Get());
          GEN_XFACTORY.Delete_Dir(xdir);
        }

      status = true;

      if(filestoupdate.Get(c)->HasSpecialControlUpdate())
        {
          DIOAPPLICATIONUPDATE_XEVENT xevent(this, DIOAPPLICATIONUPDATE_XEVENT_TYPE_UPDATEEXECFILE);

          xevent.SetFilesToUpdate(&filestoupdate);
          xevent.SetIndexOfOperation(c);
          xevent.SetError(DIOAPPLICATIONUPDATE_NOT_ERROR);

          PostEvent(&xevent);

          if(xevent.GetError() == DIOAPPLICATIONUPDATE_NOT_ERROR)
            {
               XSTRING originfilename;

               xpathrootappadd.GetNamefileExt(originfilename);

               xpathrootappaddrename  = xpathrootappadd;
               xpathrootappaddrename.SetOnlyDriveAndPath();

               xpathrootappaddrename += DIOAPPLICATIONUPDATE_BACKUPPREFIX;
               xpathrootappaddrename += originfilename.Get();

               XFILE* GEN_XFACTORY_CREATE(xfile, Create_File())
               if(xfile)
                 {
                   status = xfile->Rename(xpathrootappadd, xpathrootappaddrename);
                   GEN_XFACTORY.Delete_File(xfile);
                 }
            }
           else
            {
              error = xevent.GetError();
              break;
            }
        }

      if(status)
        {
          status = CopyFile(xpathupdateadd, xpathrootappadd);
          if(status)
            {
               if(filestoupdate.Get(c)->HasSpecialControlUpdate() && !xpathrootappaddrename.IsEmpty())
                 {
                   // Try to erase. In windows probably fails.

                   XFILE* GEN_XFACTORY_CREATE(xfile, Create_File())
                   if(xfile)
                     {
                       xfile->Erase(xpathrootappaddrename);
                       GEN_XFACTORY.Delete_File(xfile);
                     }
                 }
            }
        }

      DIOAPPLICATIONUPDATE_XEVENT xevent(this, (restore?DIOAPPLICATIONUPDATE_XEVENT_TYPE_RESTOREUPDATEFILE:DIOAPPLICATIONUPDATE_XEVENT_TYPE_UPDATEFILE));

      xevent.SetFilesToUpdate(&filestoupdate);
      xevent.SetIndexOfOperation(c);
      xevent.SetError(status?DIOAPPLICATIONUPDATE_NOT_ERROR:DIOAPPLICATIONUPDATE_ERROR_INVALIDOPERATION);

      PostEvent(&xevent);

      if(!status)
        {
          error  = DIOAPPLICATIONUPDATE_ERROR_COPYORIGINALFILE;
          break;
        }
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::DeleteFilesRenamed(XPATH& xpathtocheck)
* @brief      DeleteFilesRenamed
* @ingroup    DATAIO
*
* @param[in]  xpathtocheck :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::DeleteFilesRenamed(XPATH& xpathtocheck)
{
  XDIRELEMENT xdirelement;
  XDIR*       xdir;
  bool        status = true;

  GEN_XFACTORY_CREATE(xdir, Create_Dir())
  if(!xdir) return false;

  if(xdir->FirstSearch(xpathtocheck, __L("*") , &xdirelement))
    {
      XPATH xpathadd;

      do{ xpathadd   = xpathtocheck;
          xpathadd.Slash_Add();
          xpathadd  += xdirelement.GetNameFile()->Get();

          if(xdirelement.GetType() == XDIRELEMENTTYPE_DIR)
            {
              if(!FilesToUpdate_CreateList(xpathadd))
                {
                  status = false;
                  break;
                }
            }
           else
            {
              int ifound = xdirelement.GetNameFile()->Find(DIOAPPLICATIONUPDATE_BACKUPPREFIX, false);
              if(ifound!= XSTRING_NOTFOUND)
                {
                  XFILE* GEN_XFACTORY_CREATE(xfile, Create_File())
                  if(xfile)
                    {
                      xfile->Erase(xpathadd);
                      GEN_XFACTORY.Delete_File(xfile);
                    }
                }
            }

        } while(xdir->NextSearch(&xdirelement));
    }

  GEN_XFACTORY.Delete_Dir(xdir);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::ChangeFilesFromDownload(DIOAPPLICATIONUPDATE_VERSIONDATA& updateversiondata, XPATH& xpathupdate, XPATH& xpathbackup, DIOAPPLICATIONUPDATE_ERROR& error)
* @brief      ChangeFilesFromDownload
* @ingroup    DATAIO
*
* @param[in]  updateversiondata :
* @param[in]  xpathupdate :
* @param[in]  xpathbackup :
* @param[in]  error :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::ChangeFilesFromDownload(DIOAPPLICATIONUPDATE_VERSIONDATA& updateversiondata, XPATH& xpathupdate, XPATH& xpathbackup, DIOAPPLICATIONUPDATE_ERROR& error)
{
  bool status = false;

  FilesToUpdate_DeleteList();

  if(FilesToUpdate_CreateList(xpathupdate))
    {
      FilesToUpdate_EliminateFixedDir(xpathupdate);
      FilesToUpdate_CheckListChangedFiles(xpathupdate);

      if(filestoupdate.GetSize())
        {
          status = CopyOriginalFilesToBackup(xpathbackup, error);

          //----------------------------------------------------------------------------------------
          DIOAPPLICATIONUPDATE_XEVENT xevent(this, DIOAPPLICATIONUPDATE_XEVENT_TYPE_BACKUPORIGINALFILES);

          xevent.SetError(error);

          PostEvent(&xevent);
          //----------------------------------------------------------------------------------------

          if(status) status = CopyUpdateFilesToOriginal(xpathupdate, error, false);

        } else status = true;

    } else status = true;

  FilesToUpdate_DeleteList();

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::RestartApplication(DIOAPPLICATIONUPDATE_ERROR& error)
* @brief      RestartApplication
* @ingroup    DATAIO
*
* @param[in]  error :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::RestartApplication(DIOAPPLICATIONUPDATE_ERROR& error)
{
  XSTRING command;
  XSTRING appname;

  appname = applicationname.Get();  
  //appname.ToLowerCase();

  command.Format(__L("%s/%s"), xpathrootapp.Get(), appname.Get());

  #ifdef WINDOWS
  command += __L(".exe");
  #endif

  GEN_XPROCESSMANAGER.Application_Execute(command.Get(), NULL);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::GetListOfVersionsToRestore(XVECTOR<DIOAPPLICATIONUPDATE_VERSIONDATA*> listversiontorestore)
* @brief      GetListOfVersionsToRestore
* @ingroup    DATAIO
*
* @param[in]  listversiontorestore :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::GetListOfVersionsToRestore(XVECTOR<DIOAPPLICATIONUPDATE_VERSIONDATA*>& listversionstorestore)
{
  XDIRELEMENT xdirelement;
  XDIR*       xdir;
  XSTRING     searchstring;
  XSTRING     dir_mask;
  bool        status = true;

  GEN_XFACTORY_CREATE(xdir, Create_Dir())
  if(!xdir) 
    {
      return false;
    }

  dir_mask  = DIOAPPLICATIONUPDATE_RESTOREDIR_ID;
  dir_mask += DIOAPPLICATIONUPDATE_RESTOREDIR_MASK;

  searchstring  = DIOAPPLICATIONUPDATE_RESTOREDIR_ID;
  searchstring += __L("*");

  if(xdir->FirstSearch(xpathrootapp, searchstring.Get() , &xdirelement))
    {
      do{  xdirelement.GetNameFile()->Get();

          if(xdirelement.GetType() == XDIRELEMENTTYPE_DIR)
            {
              if(!xdirelement.GetNameFile()->Find(DIOAPPLICATIONUPDATE_RESTOREDIR_ID, false))
                {
                  int version         = 0;
                  int subversion      = 0;
                  int subversionerror = 0;

                  xdirelement.GetNameFile()->UnFormat(dir_mask.Get(), &version, &subversion, &subversionerror);

                  if(version || subversion || subversionerror)
                    {
                      DIOAPPLICATIONUPDATE_VERSIONDATA* versiondata = new DIOAPPLICATIONUPDATE_VERSIONDATA();
                      if(versiondata)
                        {
                          versiondata->SetVersion(version);
                          versiondata->SetSubVersion(subversion);
                          versiondata->SetSubVersionError(subversionerror);

                          listversionstorestore.Add(versiondata);
                        }
                    }
                }
            }

        } while(xdir->NextSearch(&xdirelement));
    }

  GEN_XFACTORY.Delete_Dir(xdir);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::GetPreviousVersionToRestore(XVECTOR<DIOAPPLICATIONUPDATE_VERSIONDATA*>& listversionstorestore, DIOAPPLICATIONUPDATE_VERSIONDATA& versiontorestore)
* @brief      GetBeforeVersionToRestore
* @ingroup    DATAIO
*
* @param[in]  listversionstorestore :
* @param[in]  versiontorestore :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::GetPreviousVersionToRestore(XVECTOR<DIOAPPLICATIONUPDATE_VERSIONDATA*>& listversionstorestore, DIOAPPLICATIONUPDATE_VERSIONDATA& versiontorestore)
{
  XVECTOR<int> indexversiontorestore;

  for(XDWORD c=0; c<listversionstorestore.GetSize(); c++)
    {
      DIOAPPLICATIONUPDATE_VERSIONDATA* versiondata = listversionstorestore.Get(c);
      if(versiondata)
        {
          int comparateversion = versiondata->Compare(applicationversion, applicationsubversion, applicationsubversionerror);
          if(comparateversion < 0)
            {
              indexversiontorestore.Add(c);
            }
        }
    }

  if(!indexversiontorestore.GetSize()) 
    {
      return false;
    }

  if(indexversiontorestore.GetSize() == 1)
    {
      DIOAPPLICATIONUPDATE_VERSIONDATA* versiondata = listversionstorestore.Get(0);
      if(!versiondata) return false;

      versiontorestore.CopyFrom(versiondata);
    }
   else
    {
      int indexprevius = indexversiontorestore.GetLast();

      for(XDWORD c=0; c<indexversiontorestore.GetSize(); c++)
        {
          DIOAPPLICATIONUPDATE_VERSIONDATA* versiondata = listversionstorestore.Get(indexprevius);
          DIOAPPLICATIONUPDATE_VERSIONDATA* versiondatacheck = listversionstorestore.Get(indexversiontorestore.Get(c));
          if(versiondatacheck && versiondata)
            {
              if(versiondatacheck->Compare(versiondata->GetVersion(), versiondata->GetSubVersion(), versiondata->GetSubVersionError()) == 1)
                {
                  indexprevius = indexversiontorestore.Get(c);
                }
            }
        }

      DIOAPPLICATIONUPDATE_VERSIONDATA* versiondata = listversionstorestore.Get(indexprevius);
      if(!versiondata) return false;

      versiontorestore.CopyFrom(versiondata);
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::RestorePrevious(DIOAPPLICATIONUPDATE_ERROR& error)
* @brief      RestoreBeforeUpdate
* @ingroup    DATAIO
*
* @param[in]  error :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::RestorePrevious(DIOAPPLICATIONUPDATE_ERROR& error)
{
  DIOAPPLICATIONUPDATE_VERSIONDATA            versiontorestore;
  XVECTOR<DIOAPPLICATIONUPDATE_VERSIONDATA*>  listversionstorestore;
  bool                                        status = false;

  GetListOfVersionsToRestore(listversionstorestore);
  if(listversionstorestore.GetSize())
    {
      if(GetPreviousVersionToRestore(listversionstorestore, versiontorestore))
        {
          status = Restore(versiontorestore, error);
        }
    }

  listversionstorestore.DeleteContents();
  listversionstorestore.DeleteAll();

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::Restore(DIOAPPLICATIONUPDATE_VERSIONDATA& applicationversiondata, DIOAPPLICATIONUPDATE_ERROR& error)
* @brief      RestoreUpdate
* @ingroup    DATAIO
*
* @param[in]  applicationversiondata :
* @param[in]  error :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::Restore(DIOAPPLICATIONUPDATE_VERSIONDATA& applicationversiondata, DIOAPPLICATIONUPDATE_ERROR& error)
{
  XPATH     xpathbackup;
  XSTRING   dir_mask;
  bool      status = true;

  dir_mask  = DIOAPPLICATIONUPDATE_RESTOREDIR_ID;
  dir_mask += DIOAPPLICATIONUPDATE_RESTOREDIR_MASK;

  xpathbackup =  xpathrootapp;
  xpathbackup.Slash_Add();
  xpathbackup.AddFormat(dir_mask.Get(), applicationversiondata.GetVersion()
                                      , applicationversiondata.GetSubVersion()
                                      , applicationversiondata.GetSubVersionError());

  status = Restore(xpathbackup, error);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOAPPLICATIONUPDATE::Restore(XPATH& xpathbackup, DIOAPPLICATIONUPDATE_ERROR& error)
* @brief      Restore
* @ingroup    DATAIO
* 
* @param[in]  xpathbackup : 
* @param[in]  error : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::Restore(XPATH& xpathbackup, DIOAPPLICATIONUPDATE_ERROR& error)
{
   bool status = false;

  FilesToUpdate_DeleteList();

  if(FilesToUpdate_CreateList(xpathbackup))
    {
      FilesToUpdate_EliminateFixedDir(xpathbackup);
      FilesToUpdate_CheckListChangedFiles(xpathbackup);

      if(filestoupdate.GetSize())
        {
          status = CopyUpdateFilesToOriginal(xpathbackup, error, true);
        }
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::EliminateOldRestorations(int maxrestorations)
* @brief      EliminateOldRestorations
* @ingroup    DATAIO
*
* @param[in]  maxrestorations :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::EliminateOldRestorations(int maxrestorations)
{
  XVECTOR<DIOAPPLICATIONUPDATE_VERSIONDATA*>  listversionstorestore;
  XVECTOR<int>                                indexversiontorestore;
  bool                                        status = false;

  GetListOfVersionsToRestore(listversionstorestore);

  for(XDWORD c=0; c<listversionstorestore.GetSize(); c++)
    {
      DIOAPPLICATIONUPDATE_VERSIONDATA* versiondata = listversionstorestore.Get(c);
      if(versiondata)
        {
          int comparateversion = versiondata->Compare(applicationversion, applicationsubversion, applicationsubversionerror);
          if(comparateversion < 0)
            {
              indexversiontorestore.Add(c);
            }
        }
    }

  if(indexversiontorestore.GetSize())
    {
      for(XDWORD c=0; c<indexversiontorestore.GetSize()-1; c++)
        {
          for(XDWORD d=1; d<indexversiontorestore.GetSize(); d++)
            {
              DIOAPPLICATIONUPDATE_VERSIONDATA* versiondata = listversionstorestore.Get(d);
              DIOAPPLICATIONUPDATE_VERSIONDATA* versiondatacheck = listversionstorestore.Get(indexversiontorestore.Get(d-1));
              if(versiondatacheck && versiondata)
                {
                  if(versiondatacheck->Compare(versiondata->GetVersion(), versiondata->GetSubVersion(), versiondata->GetSubVersionError()) == 1)
                    {
                      int index = indexversiontorestore.Get(c);

                      indexversiontorestore.Set(c, indexversiontorestore.Get(d));
                      indexversiontorestore.Set(d, index);
                    }
                }
            }
        }
    }

  status = true;

  int toeliminate =  indexversiontorestore.GetSize()-maxrestorations;
  if(toeliminate>0)
    {
      for(int c=0; c<toeliminate; c++)
        {
          DIOAPPLICATIONUPDATE_VERSIONDATA* versiondata = listversionstorestore.Get(indexversiontorestore.Get(c));
          if(versiondata)
            {
              XPATH     xpath;
              XSTRING   dir_mask;

              xpath = xpathrootapp;
              xpath.Slash_Add();

              dir_mask  = DIOAPPLICATIONUPDATE_RESTOREDIR_ID;
              dir_mask += DIOAPPLICATIONUPDATE_RESTOREDIR_MASK;

              xpath.AddFormat(dir_mask.Get(), versiondata->GetVersion(), versiondata->GetSubVersion(), versiondata->GetSubVersionError());

              XDIR* GEN_XFACTORY_CREATE(xdir, Create_Dir());
              if(xdir)
                {
                  status = xdir->Delete(xpath.Get(), true);
                  GEN_XFACTORY.Delete_Dir(xdir);

                  if(!status) break;
                }

              //XTRACE_PRINTCOLOR(3, xpath.Get());
            }
        }
    }

  listversionstorestore.DeleteContents();
  listversionstorestore.DeleteAll();

  indexversiontorestore.DeleteAll();

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::IsNewVersion(DIOAPPLICATIONUPDATE_VERSIONDATA& updateversiondata)
* @brief      IsNewVersion
* @ingroup    DATAIO
*
* @param[in]  updateversiondata :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::IsNewVersion(DIOAPPLICATIONUPDATE_VERSIONDATA& updateversiondata)
{
  int versioncomparation = updateversiondata.Compare(applicationversion, applicationsubversion, applicationsubversionerror);

  //----------------------------------------------------------------------------------------

  DIOAPPLICATIONUPDATE_XEVENT xevent(this, DIOAPPLICATIONUPDATE_XEVENT_TYPE_CHECKVERSIONAVAILABLE);

  xevent.SetVersionComparation(versioncomparation);
  xevent.SetVersionData(&updateversiondata);
  xevent.SetFilesToUpdate(&filestoupdate);
  xevent.SetError(DIOAPPLICATIONUPDATE_NOT_ERROR);

  PostEvent(&xevent);

  //----------------------------------------------------------------------------------------

  return (versioncomparation == 1)?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOAPPLICATIONUPDATE::GetApplicationVersion()
* @brief      GetApplicationVersion
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOAPPLICATIONUPDATE::GetApplicationVersion()
{
  return this->applicationversion;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOAPPLICATIONUPDATE::GetApplicationSubversion()
* @brief      GetApplicationSubversion
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOAPPLICATIONUPDATE::GetApplicationSubversion()
{
  return this->applicationsubversion;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD DIOAPPLICATIONUPDATE::GetApplicationSubVersionError()
* @brief      GetApplicationSubVersionError
* @ingroup    DATAIO
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOAPPLICATIONUPDATE::GetApplicationSubVersionError()
{
  return this->applicationsubversionerror;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::FilesToUpdate_CreateList(XPATH& xpathupdate, bool checkaredifferent)
* @brief      FilesToUpdate_CreateList
* @ingroup    DATAIO
*
* @param[in]  xpathupdate :
* @param[in]  checkaredifferent :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::FilesToUpdate_CreateList(XPATH& xpathupdate)
{
  XDIRELEMENT xdirelement;
  XDIR*       xdir;
  bool        status = true;

  GEN_XFACTORY_CREATE(xdir, Create_Dir())
  if(!xdir) return false;

  if(xdir->FirstSearch(xpathupdate, __L("*") , &xdirelement))
    {
      XPATH xpathadd;

      do{ xpathadd   = xpathupdate;
          xpathadd.Slash_Add();
          xpathadd  += xdirelement.GetNameFile()->Get();

          // XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("%s"), xpathadd.Get());  

          if(xdirelement.GetType() == XDIRELEMENTTYPE_FILE)
            {
              HASHCRC32 CRC32file;
              XQWORD    sizefile = 0;

              CRC32file.Do(xpathadd);

              XFILE* GEN_XFACTORY_CREATE(xfile, Create_File())
              if(xfile)
                {
                  if(xfile->Open(xpathadd))
                    {
                      sizefile = xfile->GetSize();
                      xfile->Close();
                    }

                  GEN_XFACTORY.Delete_File(xfile);
                }

              DIOAPPLICATIONUPDATE_FILE* updatefile = new DIOAPPLICATIONUPDATE_FILE(xpathadd, CRC32file.GetResultCRC32(), sizefile);
              if(updatefile)
                {
                  XSTRING filename;
                  XSTRING fileext;

                  updatefile->GetXPathFile()->GetNamefile(filename);
                  updatefile->GetXPathFile()->GetExt(fileext);

                  if(!filename.Compare(applicationname, true))
                    {
                      updatefile->SetHasSpecialControlUpdate(true);
                    }

                  filestoupdate.Add(updatefile);
                }
            }

          if(xdirelement.GetType() == XDIRELEMENTTYPE_DIR)
            {
              if(!FilesToUpdate_CreateList(xpathadd))
                {
                  status = false;
                  break;
                }
            }

        } while(xdir->NextSearch(&xdirelement));
    }

  GEN_XFACTORY.Delete_Dir(xdir);

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::FilesToUpdate_EliminateFixedDir(XPATH& xpath)
* @brief      FilesToUpdate_EliminateFixedDir
* @ingroup    DATAIO
*
* @param[in]  xpath :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::FilesToUpdate_EliminateFixedDir(XPATH& xpath)
{
  XDWORD c = 0;

  if(!filestoupdate.GetSize()) return false;

  while(c < filestoupdate.GetSize())
    {
      DIOAPPLICATIONUPDATE_FILE* updatefile = filestoupdate.Get(c);
      if(updatefile)
        {
           updatefile->GetXPathFile()->DeleteCharacters(0, xpath.GetSize()+1);
           c++;
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::FilesToUpdate_CheckListChangedFiles(XPATH& xpathupdate)
* @brief      FilesToUpdate_CheckListChangedFiles
* @ingroup    DATAIO
*
* @param[in]  xpathupdate :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::FilesToUpdate_CheckListChangedFiles(XPATH& xpathupdate)
{
  XDWORD c = 0;

  while(c < filestoupdate.GetSize())
    {
      DIOAPPLICATIONUPDATE_FILE* updatefile = filestoupdate.Get(c);
      if(updatefile)
        {
          HASHCRC32 CRC32file;
          XPATH     xpath;

          xpath  = xpathrootapp;
          xpath.Slash_Add();
          xpath += updatefile->GetXPathFile()->Get();

          CRC32file.Do(xpath);

          if(CRC32file.GetResultCRC32() == updatefile->GetCRC32())
            {
              filestoupdate.Delete(updatefile);
              delete updatefile;
            }
           else
            {
              c++;
            }
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::FilesToUpdate_DeleteList()
* @brief      DeleteAllFilesToUpload
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::FilesToUpdate_DeleteList()
{
  if(filestoupdate.IsEmpty()) return false;

  filestoupdate.DeleteContents();
  filestoupdate.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOAPPLICATIONUPDATE::CopyFile(XPATH& origin, XPATH& target)
* @brief      CopyFile
* @ingroup    DATAIO
*
* @param[in]  origin :
* @param[in]  target :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOAPPLICATIONUPDATE::CopyFile(XPATH& origin, XPATH& target)
{
  XFILE* xfile[2] = { NULL, NULL };
  bool   status   = false;

  GEN_XFACTORY_CREATE(xfile[0], Create_File())
  GEN_XFACTORY_CREATE(xfile[1], Create_File())

  if(xfile[0] && xfile[1])
    {
      if(xfile[0]->Open(origin))
        {
          if(xfile[1]->Create(target))
            {
              #define COPYFILEBLOCKSIZE  500*1024

              XBUFFER xbuffer;
              XDWORD  br = COPYFILEBLOCKSIZE;

              xbuffer.Resize(COPYFILEBLOCKSIZE);

              do{ xfile[0]->Read(xbuffer.Get(), &br);
                  if(!br) break;

                  if(!xfile[1]->Write(xbuffer.Get(), br)) break;

                } while(br == COPYFILEBLOCKSIZE);


              xfile[1]->Close();

              status = true;
            }

          xfile[0]->Close();
        }
    }

  GEN_XFACTORY.Delete_File(xfile[0]);
  GEN_XFACTORY.Delete_File(xfile[1]);

  if(status)
    {
      status = false;

      HASHCRC32* CRC32file[2];

      CRC32file[0] = new HASHCRC32();
      CRC32file[1] = new HASHCRC32();

      if(CRC32file[0] && CRC32file[1])
        {
          CRC32file[0]->Do(origin);
          CRC32file[1]->Do(target);

          if(CRC32file[0]->GetResultCRC32() == CRC32file[1]->GetResultCRC32()) status = true;
        }

      delete CRC32file[0];
      delete CRC32file[1];
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOAPPLICATIONUPDATE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOAPPLICATIONUPDATE::Clean()
{
  applicationversion          = 0;
  applicationsubversion       = 0;
  applicationsubversionerror  = 0;

  xdir                        = NULL;
  webclient                   = NULL;

  progressfiletoupdate        = -1;
  stopprocess                = false;
}


#pragma endregion


#pragma endregion

