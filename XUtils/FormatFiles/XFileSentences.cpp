/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XFileSentences.cpp
* 
* @class      XFILESENTENCES
* @brief      eXtended Utils Sentences HASH file class
* @ingroup    XUTILS
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

#include "XFileSentences.h"

#include <stdlib.h>
#include <string.h>

#include "XBuffer.h"
#include "XFileDBF.h"
#include "XFactory.h"

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
* @fn         XFILESENTENCES::XFILESENTENCES(HASH* hash, XPATH& xpath) : XFILEXDB(hash, xpath)
* @brief      Constructor of class
* @ingroup    XUTILS
*
* @param[in]  hash :
* @param[in]  xpath:
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILESENTENCES::XFILESENTENCES(HASH* hash, XPATH& xpath) : XFILEXDB(hash, xpath)
{
  Set(XFILE_SEN_ID, XFILESENTENCES_TYPE, XFILESENTENCES_VERSION, XFILESENTENCES_IDSTRING);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILESENTENCES::~XFILESENTENCES()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
XFILESENTENCES::~XFILESENTENCES()
{

}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILESENTENCES::ConvertFileFromDBF(XPATH& xpath)
* @brief      Convert file from DBF
* @ingroup    XUTILS
*
* @param[in]  xpath :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILESENTENCES::ConvertFileFromDBF(XPATH& xpath)
{
  XFILEDBF* filedbf;
  XDWORD    c;

  CloseFile();

  filedbf = new XFILEDBF();
  if(!filedbf) return false;

  if(!filedbf->Open(xpath))
    {
      delete filedbf;
      return false;
    }

  XFILEDBF_HEADER*  header = filedbf->GetHeader();

  if((!header)||(!header->Load(filedbf->GetPrimaryFile())))
    {
      delete filedbf;
      return false;
    }

  XDWORD nrecords = header->GetNRecords();

  if(!nrecords)
    {
      delete filedbf;
      return false;
    }

  XQWORD initablepos;

  Create(this->xpath);

  GetPrimaryFile()->Write((XBYTE*)&nrecords,sizeof(XDWORD));

  GetPrimaryFile()->GetPosition(initablepos);

  for(c=0;c<nrecords;c++)
    {
      XDWORD idx=0;
      for(int d=0;d<2;d++)
        {
          GetPrimaryFile()->Write((XBYTE*)&idx,sizeof(XDWORD));
        }
    }

  XDWORD  index;
  XQWORD  fpos  = 0;

  for(c=0;c<nrecords;c++)
    {
      XFILEDBF_RECORD* record    = filedbf->ReadRecord(c);
      XSTRING*         fsentence = new XSTRING(_MAXSTR);

      if(fsentence)
        {
          fsentence->FillChar();
          memcpy(fsentence->Get(),(char*)record->GetData(0),record->GetSizeField(0));
          index = fsentence->ConvertToInt();

          fsentence->FillChar();
          memcpy(fsentence->Get(),(char*)record->GetData(1),record->GetSizeField(1));

          fsentence->DeleteCharacter(__C(' '),XSTRINGCONTEXT_TO_END);

          GetPrimaryFile()->GetPosition(fpos);

          indexmap.Add(index, (XDWORD)fpos);

          XDWORD size = (XDWORD)fsentence->GetSize();
          GetPrimaryFile()->Write((XBYTE*)&size,sizeof(XDWORD));
          
          XBUFFER xbufferexchange;
          (*fsentence).ConvertToBufferExchange(xbufferexchange);
          GetPrimaryFile()->Write(xbufferexchange.Get(), xbufferexchange.GetSize());

          
          delete fsentence;
        }

      delete record;
    }

  GetPrimaryFile()->SetPosition(initablepos);

  XDWORD ifilepos = 0;
  XDWORD  filepos  = 0;

  for(XDWORD c=0;c<nrecords;c++)
    {
      ifilepos = indexmap.GetKey(c);
      filepos  = indexmap.GetElement(c);

      GetPrimaryFile()->Write((XBYTE*)&ifilepos ,sizeof(XDWORD));
      GetPrimaryFile()->Write((XBYTE*)&filepos   ,sizeof(XDWORD));
    }

  filedbf->Close();
  delete filedbf;

  CloseFile();

  return OpenFile();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XFILESENTENCES::GetNumberSentences()
* @brief      Get number sentences
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XFILESENTENCES::GetNumberSentences()
{
  return GetNumberRecords();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILESENTENCES::GetSentence(XDWORD index,XSTRING& sentence)
* @brief      Get sentence
* @ingroup    XUTILS
*
* @param[in]  index :
* @param[in]  sentence :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XFILESENTENCES::GetSentence(XDWORD index,XSTRING& sentence)
{
  sentence.Empty();

  XBUFFER* xbuffer = GetRecord(index);
  if(!xbuffer) return false;

  sentence.Set((XWORD*)xbuffer->Get());

  delete xbuffer;

  return true;
}


#pragma endregion


