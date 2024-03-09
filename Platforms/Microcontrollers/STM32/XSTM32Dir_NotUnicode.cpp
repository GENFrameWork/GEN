/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XSTM32Dir.cpp
*
* @class      XSTM32DIR
* @brief      eXtended WINDOWS Directory class
* @ingroup    PLATFORM_STM32
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XBase.h"
#include "XFactory.h"
#include "XFile.h"
#include "XString.h"
#include "XPath.h"
#include "XTrace.h"

#include "XSTM32Dir.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTM32DIR::XSTM32DIR()
* @brief      Constructor
* @ingroup    PLATFORM_STM32
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTM32DIR::XSTM32DIR(): XDIR()
{
  Clean();
  
  fresult = f_mount(&userFATFS, "/", 1);
  if(fresult == FR_OK) ismount = true; 
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTM32DIR::~XSTM32DIR()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_STM32
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTM32DIR::~XSTM32DIR()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSTM32DIR::Exist(XCHAR* path)
* @brief      Exist
* @ingroup    PLATFORM_STM32
*
* @param[in]  path :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSTM32DIR::Exist(XCHAR* path)
{   
  XSTRING pathstring;
  DIR     dir;
 
  pathstring = path;
  if(pathstring.IsEmpty()) return false; 
   
  XBUFFER charstr;
  
  pathstring.ConvertToASCII(charstr);  
  fresult = f_opendir(&dir, charstr.GetPtrChar());						
  if(fresult) fresult = f_closedir(&dir);	
  
  return (fresult == FR_OK)?true:false;  
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSTM32DIR::Make(XCHAR* path, bool recursive)
* @brief      Make
* @ingroup    PLATFORM_STM32
*
* @param[in]  path : 
* @param[in]  recursive : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XSTM32DIR::Make(XCHAR* path, bool recursive)
{
  if(!path)   return false;
  if(!path[0]) return false;

  if(Exist(path)) return true;

  XPATH   xpath;

  xpath = path;

  xpath.Slash_Normalize(true);

  if(recursive)
    {      
      XPATH   xpathsequence;
      XSTRING pathpart;
      int     index = 0;

      xpathsequence.Empty();
    
      do{
          if(xpath.GetPathInSequence(index,pathpart))
            {
              xpathsequence += pathpart;
              xpathsequence += __L("\\");

              if(!Exist(xpathsequence.Get()))
                {
                  //XTRACE_PRINTCOLOR(2, __L("---> Make: %s"), xpathsequence.Get());
                      
                  xpathsequence.Slash_Delete();
                      
                  XBUFFER charstr;
                  
                  xpathsequence.ConvertToASCII(charstr);                  
                  fresult = f_mkdir(charpath.GetPtrChar());						
                  
                  if(fresult != FR_OK) return false;  
                }
                
            } else break;

          index++;

        } while(1);
    }
   else
    {      
      XBUFFER charstr;
      
      xpath.ConvertToASCII(charstr);
      fresult = f_mkdir(charstr.GetPtrChar());						      
      if(fresult != FR_OK) return false;  
    }

  return true;  
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSTM32DIR::ChangeTo(XCHAR* path)
* @brief      ChangeTo
* @ingroup    PLATFORM_STM32
*
* @param[in]  path :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSTM32DIR::ChangeTo(XCHAR* path)
{  
  if(!path)       return false;
  if(!path[0])    return false;

  if(Exist(path)) return true;

  XPATH xpath;

  xpath = path;

  xpath.Slash_Normalize(true);
  
  XBUFFER charstr;
  
  xpath.ConvertToASCII(charstr);
  fresult = f_chdir(charstr.GetPtrChar());	  
  if(fresult != FR_OK) return false;  

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSTM32DIR::Delete(XCHAR* path,bool all)
* @brief      Delete
* @ingroup    PLATFORM_STM32
*
* @param[in]  path :
* @param[in]  all :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSTM32DIR::Delete(XCHAR* path, bool all)
{
  XDIRELEMENT search;
  XPATH       xpathname;
  XSTRING     patternsearch;

  if(all)
    {
      xpathname     = path;
      patternsearch = __L("*");

      if(FirstSearch(xpathname.Get(),patternsearch.Get(),&search))
        {
          do{ xpathname  = path;
              xpathname.Slash_Add();
              xpathname += search.GetNameFile()->Get();

              if(search.GetType() == XDIRELEMENTTYPE_DIR)
                {
                  Delete(xpathname.Get(), all);
                }
               else
                {
                  XFILE* GEN_XFACTORY_CREATE(xfile, Create_File())
                  if(xfile) xfile->Erase(xpathname.Get());
                  GEN_XFACTORY.Delete_File(xfile);
                 }
                
            } while(NextSearch(&search));
        }
    }

  
  if(!path)       return false;
  if(!path[0])    return false; 

  XPATH xpath;

  xpath = path;

  xpath.Slash_Normalize(true);
  
  XBUFFER charstr;
  
  xpath.ConvertToASCII(charstr);
  fresult = f_rmdir(charstr.GetPtrChar());	
  if(fresult != FR_OK) return false;  
 
  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSTM32DIR::GetActual(XPATH& xpath)
* @brief      GetActual
* @ingroup    PLATFORM_STM32
*
* @param[in]  xpath :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSTM32DIR::GetActual(XPATH& xpath)
{ 
  char path[_MAXPATH];
  
  memset(path, 0, _MAXPATH);
  
  fresult = f_getcwd(path, _MAXPATH);							
  if(fresult != FR_OK) return false;  
  
  xpath = path;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSTM32DIR::FirstSearch(XCHAR* xpath, XCHAR* patternsearch, XDIRELEMENT* searchelement)
* @brief      FirstSearch
* @ingroup    PLATFORM_STM32
*
* @param[in]  xpath :
* @param[in]  patternsearch :
* @param[in]  searchelement :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSTM32DIR::FirstSearch(XCHAR* xpath, XCHAR* patternsearch, XDIRELEMENT* searchelement)
{  
  if(!xpath)         return false;
  if(!patternsearch) return false;

  XBYTE* findfiledata = new XBYTE[sizeof(XSTM32DIR_SEARCHINFO)];
  if(!findfiledata) return false;

  memset(findfiledata, 0, sizeof(XSTM32DIR_SEARCHINFO));

  searchelement->SetFindFileData(findfiledata);

  searchelement->GetPathSearch()->Set(xpath);

  XPATH   pathsearch;
  XSTRING _patternsearch;
  
  _patternsearch = patternsearch;
  
  searchelement->GetPatternSearch()->Set(_patternsearch);

  pathsearch  = xpath;
  pathsearch.Slash_Add();
  
  searchelement->GetPathSearch()->Set(pathsearch);
    
  XSTM32DIR_SEARCHINFO* searchinfo = (XSTM32DIR_SEARCHINFO*)searchelement->GetFindFileData();

  XBUFFER charpathsearch;
  XBUFFER charpatternsearch;
  
  pathsearch.ConvertToASCII(charpathsearch);
  _patternsearch.ConvertToASCII(charpatternsearch);

  fresult = f_findfirst(&searchinfo->dir, &searchinfo->fileinfo, (const TCHAR*)charpathsearch.GetPtrChar(), (const TCHAR*)charpatternsearch.GetPtrChar());  
  if(fresult != FR_OK || !searchinfo->fileinfo.fname[0]) 
    {      
      searchelement->DeleteFindFileData();
      return false;
    }

  XSTRING searchfilename(searchinfo->fileinfo.fname);

  if((!searchfilename.Compare(__L(".")))||
     (!searchfilename.Compare(__L(".."))))
    {
      return NextSearch(searchelement);
    }

   ReconvertSearchData(searchelement, &searchinfo->fileinfo);
  
  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSTM32DIR::NextSearch(XDIRELEMENT* searchelement)
* @brief      NextSearch
* @ingroup    PLATFORM_STM32
*
* @param[in]  searchelement :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XSTM32DIR::NextSearch(XDIRELEMENT* searchelement)
{
  XSTM32DIR_SEARCHINFO* searchinfo = (XSTM32DIR_SEARCHINFO*)searchelement->GetFindFileData();
  if(!searchinfo) return false;
  
  XBUFFER charstr;
  
  (*searchelement->GetPatternSearch()).ConvertToASCII(charstr);  
  searchinfo->dir.pat = charstr.GetPtrChar(); 
  fresult = f_findnext(&searchinfo->dir, &searchinfo->fileinfo);  
  if(fresult != FR_OK || !searchinfo->fileinfo.fname[0]) 
    {
      searchelement->DeleteFindFileData();
      f_closedir(&searchinfo->dir);
      return false;
    }

   XSTRING searchfilename(searchinfo->fileinfo.fname);

   if((!searchfilename.Compare(__L(".")))||
      (!searchfilename.Compare(__L(".."))))
     {
       return NextSearch(searchelement);
     }

  ReconvertSearchData(searchelement, &searchinfo->fileinfo);

  return true;
}




/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSTM32DIR::ReconvertSearchData(XDIRELEMENT* searchelement, FILINFO* fileinfo)
* @brief      ReconvertSearchData
* @ingroup    PLATFORM_STM32
* 
* @param[in]  searchelement : 
* @param[in]  fileinfo : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XSTM32DIR::ReconvertSearchData(XDIRELEMENT* searchelement, FILINFO* fileinfo)
{
  if(!searchelement) return false;

  searchelement->GetNameFile()->Set(fileinfo->fname);

  if(fileinfo->fattrib & AM_DIR)
          searchelement->SetType(XDIRELEMENTTYPE_DIR);
    else  searchelement->SetType(XDIRELEMENTTYPE_FILE);
 
  return true;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XSTM32DIR::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_STM32
*
* --------------------------------------------------------------------------------------------------------------------*/
void XSTM32DIR::Clean()
{
  memset(&userFATFS, 0, sizeof(FATFS));
  
  fresult = FR_OK; 
}

