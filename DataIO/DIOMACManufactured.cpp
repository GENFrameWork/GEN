/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOMACManufactured.cpp
* 
* @class      DIOMACMANUFACTURED
* @brief      Data Input/Output MAC Manufactured Device class
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

#include "DIOMACManufactured.h"

#include <stdio.h>
#include <string.h>

#include "HashMD5.h"

#include "XFileTXT.h"
#include "XFileXDB.h"
#include "XTrace.h"

#include "DIOScraperWebMACManufacturer.h"

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


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOMACMANUFACTURED::DIOMACMANUFACTURED()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOMACMANUFACTURED::DIOMACMANUFACTURED()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOMACMANUFACTURED::~DIOMACMANUFACTURED()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOMACMANUFACTURED::~DIOMACMANUFACTURED()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOMACMANUFACTURED::Web_GetManufactured(DIOMAC& MAC, XSTRING& manufactured)
* @brief      Web get manufactured
* @ingroup    DATAIO
* 
* @param[in]  MAC : 
* @param[in]  manufactured : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOMACMANUFACTURED::Web_GetManufactured(DIOMAC& MAC, XSTRING& manufactured)
{
  bool status = false;

  DIOSCRAPERWEBMACMANUFACTURER* macmanufactured = new DIOSCRAPERWEBMACMANUFACTURER;
  if(macmanufactured)
    {
      status = macmanufactured->Get(MAC, manufactured);
      delete macmanufactured;
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOMACMANUFACTURED::File_GetManufactured(XPATH& xpath, DIOMAC& MAC, XSTRING& manufactured)
* @brief      File get manufactured
* @ingroup    DATAIO
* 
* @param[in]  xpath : 
* @param[in]  MAC : 
* @param[in]  manufactured : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOMACMANUFACTURED::File_GetManufactured(XPATH& xpath, DIOMAC& MAC, XSTRING& manufactured)
{
  if(xpath.IsEmpty())
    {
      XPATH xpathresource;

      GEN_XPATHSMANAGER.GetPathOfSection(XPATHSMANAGERSECTIONTYPE_ROOT, xpathresource);

      xpath.Format(__L("%s%s"), xpathresource.Get(), DIOMACMANUFACTURED_FILE_NAME);
    }

  HASHMD5   hashmd5;
  XFILEXDB* xfilexdb = new XFILEXDB( &hashmd5, xpath);
  bool      status;

  if(xfilexdb)
    {
      xfilexdb->Set(XFILE_XDB_ID, XFILEXDB_TYPE, DIOMACMANUFACTURED_FILE_VERSION, DIOMACMANUFACTURED_FILE_IDSTRING);

      status = xfilexdb->Ini();
      if(status)
        {
          manufactured.Empty();

          XBUFFER* xbuffer = xfilexdb->GetRecord(MAC.GetManufactured());
          if(xbuffer)
            {
              manufactured.Set((XWORD*)xbuffer->Get());
              delete xbuffer;
            }
        }

      xfilexdb->End();

      delete xfilexdb;
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOMACMANUFACTURED::File_GetManufacturedMACs(XPATH& xpath, XSTRING& manufactured, XVECTOR<XDWORD>& MACs)
* @brief      File get manufactured MA cs
* @ingroup    DATAIO
* 
* @param[in]  xpath : 
* @param[in]  manufactured : 
* @param[in]  MACs : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOMACMANUFACTURED::File_GetManufacturedMACs(XPATH& xpath, XSTRING& manufactured, XVECTOR<XDWORD>& MACs)
{
  if(xpath.IsEmpty())
    {
      XPATH xpathresource;

      GEN_XPATHSMANAGER.GetPathOfSection(XPATHSMANAGERSECTIONTYPE_ROOT, xpathresource);

      xpath.Format(__L("%s%s"), xpathresource.Get(), DIOMACMANUFACTURED_FILE_NAME);
    }

  HASHMD5   hashmd5;
  XFILEXDB* xfilexdb = new XFILEXDB( &hashmd5, xpath);
  bool      status;

  if(xfilexdb)
    {
      xfilexdb->Set(XFILE_XDB_ID, XFILEXDB_TYPE, DIOMACMANUFACTURED_FILE_VERSION, DIOMACMANUFACTURED_FILE_IDSTRING);

      status = xfilexdb->Ini();
      if(status)
        {
          for(int c=0;c<xfilexdb->GetNumberRecords();c++)
            {
              XSTRING _manufactured;

              _manufactured.Empty();

              XBUFFER* xbuffer = xfilexdb->GetRecordIndex(c);
              if(xbuffer)
                {
                  _manufactured.Set((XWORD*)xbuffer->Get());

                  if(_manufactured.Find(manufactured,true)!=XSTRING_NOTFOUND)
                    {
                      XDWORD ID =  xfilexdb->GetIndexMap()->GetKey(c);

                      MACs.Add(ID);                      
                    }

                  delete xbuffer;
                }
            }
        }

      xfilexdb->End();

      delete xfilexdb;
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOMACMANUFACTURED::File_Download(XBUFFER& xbuffer, int timeout)
* @brief      File download
* @ingroup    DATAIO
* 
* @param[in]  xbuffer : 
* @param[in]  timeout : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOMACMANUFACTURED::File_Download(XBUFFER& xbuffer, int timeout)
{
  DIOURL url;
  bool   status;

  url  = DIOMACMANUFACTURED_URL;
  url += DIOMACMANUFACTURED_URLNAMEFILE;

  DIOWEBCLIENT* webclient = new DIOWEBCLIENT;
  if(!webclient) return false;

  status = webclient->Get(url, xbuffer, DIOWEBCLIENT_DEFAULTUSERAGENT, timeout);

  delete webclient;

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOMACMANUFACTURED::File_Convert(XBUFFER& xbuffer, XPATH& xpath)
* @brief      File convert
* @ingroup    DATAIO
* 
* @param[in]  xbuffer : 
* @param[in]  xpath : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOMACMANUFACTURED::File_Convert(XBUFFER& xbuffer, XPATH& xpath)
{
  if(xpath.IsEmpty()) return false;

  XFILETXT* xfiletxtoui = new XFILETXT();
  if(!xfiletxtoui) return false;

  XMAP<XDWORD, XSTRING*>  idmap;
  XMAP<XDWORD, int>       idmapindex;
  XDWORD                  c;

  idmap.DeleteAll();
  idmapindex.DeleteAll();

  idmap.SetAddInLimit(10000);
  idmapindex.SetAddInLimit(10000);

  xfiletxtoui->AddBufferLines(XFILETXTFORMATCHAR_ASCII, xbuffer);

  int nlines = xfiletxtoui->GetNLines();
  for(int c=0;c<nlines;c++)
    {
      XSTRING* line  = xfiletxtoui->GetLine(c);
      if(line)
        {
          if(!line->IsEmpty())
            {
              int start = line->Find(__L("(base 16)"),true);
              if(start!=XSTRING_NOTFOUND)
                {
                  XDWORD  ID = 0;
                
                  start+=11;
                  line->UnFormat(__L("%X    (base 16)\t\t"),&ID);

                  if(idmap.Find(ID)==-1)
                    {
                      XSTRING* name = new XSTRING();
                      if(name)
                        {
                          line->Copy(start,(*name));
                          idmap.Add(ID,name);
                        }
                    }
                }
            }
        }
    }

  delete xfiletxtoui;

  XDWORD    nrecords =  idmap.GetSize();
  HASHMD5   hashmd5;
  XDWORD    initablepos = 0;
  XDWORD    filepos     = 0;

  XFILEXDB* xfilexdb = new XFILEXDB( &hashmd5, xpath);
  if(xfilexdb)
    {
      xfilexdb->Set(XFILE_XDB_ID, XFILEXDB_TYPE, DIOMACMANUFACTURED_FILE_VERSION, DIOMACMANUFACTURED_FILE_IDSTRING);

      if(xfilexdb->Create(xpath))
        {
          xfilexdb->GetPrimaryFile()->Write((XBYTE*)&nrecords, sizeof(XDWORD));
          xfilexdb->GetPrimaryFile()->GetPosition(initablepos);

          XDWORD ID = 0;

          for(c=0;c<nrecords;c++)
            {
              xfilexdb->GetPrimaryFile()->Write((XBYTE*)&ID, sizeof(XDWORD));
              xfilexdb->GetPrimaryFile()->Write((XBYTE*)&ID, sizeof(XDWORD));
            }

          for(c=0;c<nrecords;c++)
            {
              size_t   _name  = (size_t)idmap.GetElement(c);
              XSTRING* name   = (XSTRING*)_name;
              if(name)
                {
                  ID = (XDWORD)idmap.GetKey(c);

                  xfilexdb->GetPrimaryFile()->GetPosition(filepos);

                  XDWORD size = (name->GetSize()+1)*sizeof(XWORD);

                  xfilexdb->GetPrimaryFile()->Write((XBYTE*)&size                 , sizeof(XDWORD));

                  XBUFFER xbufferexchange;            

                  (*name)->ConvertToBufferExchange(xbufferexchange, true);
                  xfilexdb->GetPrimaryFile()->Write(xbufferexchange->Get(), size);
                                        
                  idmapindex.Add(ID,filepos);

                  delete name;
                }
            }

          xfilexdb->GetPrimaryFile()->SetPosition(initablepos-sizeof(XDWORD));
          xfilexdb->GetPrimaryFile()->Write((XBYTE*)&nrecords,sizeof(XDWORD));

          for(c=0;c<nrecords;c++)
            {
              ID      = (XDWORD)idmapindex.GetKey(c);
              filepos = (XDWORD)idmapindex.GetElement(c);

              xfilexdb->GetPrimaryFile()->Write((XBYTE*)&ID     , sizeof(XDWORD));
              xfilexdb->GetPrimaryFile()->Write((XBYTE*)&filepos, sizeof(XDWORD));
            }

          xfilexdb->CloseFile();
        }
    }

  idmap.DeleteAll();
  idmapindex.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOMACMANUFACTURED::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOMACMANUFACTURED::Clean()
{

}


#pragma endregion

