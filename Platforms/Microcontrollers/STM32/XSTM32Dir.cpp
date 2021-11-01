/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XSTM32Dir.cpp
*
* @class      XSTM32DIR
* @brief      eXtended WINDOWS Directory class
* @ingroup    PLATFORM_STM32
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @copyright  Copyright(c) 2008 - 2016 GEN Group.
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
*---------------------------------------------------------------------------------------------------------------------*/

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
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTM32DIR::XSTM32DIR(): XDIR()
{
  Clean();
  
  XSTRING root;
  
  root = __L("/");
  
  XSTRING_CREATENORMALIZE(root, buffer, false); 
  fresult = f_mount(&userFATFS, buffer, 1);
  XSTRING_DELETENORMALIZE(root, buffer); 
  
  if(fresult == FR_OK) ismount = true; 
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTM32DIR::~XSTM32DIR()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTM32DIR::~XSTM32DIR()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSTM32DIR::Exist(XCHAR* path)
* @brief      Exist
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  path :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSTM32DIR::Exist(XCHAR* path)
{   
  XPATH  pathstring;
  DIR    dir;
 
  pathstring = path;
  if(pathstring.IsEmpty()) return false; 
  
  pathstring.Slash_Delete();
  
  pathstring.Slash_Normalize(true);
    
  XSTRING_CREATENORMALIZE(pathstring, buffer, false); 
  fresult = f_opendir(&dir, buffer);						
  XSTRING_DELETENORMALIZE(pathstring, buffer);
  
  if(fresult == FR_OK) fresult = f_closedir(&dir);	
  
  return (fresult == FR_OK)?true:false;  
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XSTM32DIR::Make(XCHAR* path, bool recursive)
* @brief      Make
* @ingroup    PLATFORM_WINDOWS
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
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
              xpathsequence.Slash_Add();
              xpathsequence.Slash_Normalize(true);
              xpathsequence += pathpart;                        

              if(!Exist(xpathsequence.Get()) && (!xpathsequence.IsEmpty()))
                {
                  //XTRACE_PRINTCOLOR(2, __L("---> Make: %s"), xpathsequence.Get());
                      
                  xpathsequence.Slash_Delete(); 
                  
                  XSTRING_CREATENORMALIZE(xpathsequence, buffer, false); 
                  fresult = f_mkdir(buffer);
                  XSTRING_DELETENORMALIZE(xpathsequence, buffer);
                  
                  xpathsequence.Slash_Add(); 
                  
                  if(fresult != FR_OK) return false;  
                }
                
            } else break;

          index++;

        } while(1);
    }
   else
    {
      XSTRING_CREATENORMALIZE(xpath, buffer, false); 
      fresult = f_mkdir(buffer);	
      XSTRING_DELETENORMALIZE(xpath, buffer);
      if(fresult != FR_OK) return false;  
    }

  return true;  
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSTM32DIR::ChangeTo(XCHAR* path)
* @brief      ChangeTo
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  path :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSTM32DIR::ChangeTo(XCHAR* path)
{  
  if(!path)       return false;
  if(!path[0])    return false;

  if(Exist(path)) return true;

  XPATH xpath;

  xpath = path;
  xpath.Slash_Normalize(true);

  XSTRING_CREATENORMALIZE(xpath, buffer, false); 
  fresult = f_chdir(buffer);	
  XSTRING_DELETENORMALIZE(xpath, buffer);
  
  if(fresult != FR_OK) return false;  

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSTM32DIR::Delete(XCHAR* path,bool all)
* @brief      Delete
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  path :
* @param[in]  all :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
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
                  XFILE* xfile = GEN_XFACTORY.Create_File();
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
  
  XSTRING_CREATENORMALIZE(xpath, buffer, false); 
  fresult = f_unlink(buffer);	
  XSTRING_DELETENORMALIZE(xpath, buffer); 
  
  if(fresult != FR_OK) return false;  
 
  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSTM32DIR::GetActual(XPATH& xpath)
* @brief      GetActual
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  xpath :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSTM32DIR::GetActual(XPATH& xpath)
{   
  XWORD buffer[_MAXPATH];
  
  memset(buffer, 0, sizeof(buffer));
  
  xpath.Empty();
    
  fresult = f_getcwd(buffer, _MAXPATH);							
  if(fresult != FR_OK) return false;  
  
  xpath.Set(buffer);
    
  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XSTM32DIR::FirstSearch(XCHAR* xpath, XCHAR* patternsearch, XDIRELEMENT* searchelement)
* @brief      FirstSearch
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  xpath :
* @param[in]  patternsearch :
* @param[in]  searchelement :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSTM32DIR::FirstSearch(XCHAR* xpath, XCHAR* patternsearch, XDIRELEMENT* searchelement)
{  
  if(!xpath)         return false;
  if(!patternsearch) return false;

  XBYTE* findfiledata = new XBYTE[sizeof(XSTM32DIR_SEARCHINFO)];
  if(!findfiledata) return false;

  memset(findfiledata, 0, sizeof(XSTM32DIR_SEARCHINFO));

  searchelement->SetFindFileData(findfiledata);

  searchelement->GetPathSearch()->Set(xpath);

  XPATH   pathsearch_str;
  XSTRING patternsearch_str;
  
  patternsearch_str = patternsearch;
  searchelement->GetPatternSearch()->Set(patternsearch_str);
    
  //pathsearch_str.Slash_Add();  
  pathsearch_str  += xpath;
  pathsearch_str.Slash_Normalize(true);
  
  pathsearch_str.Slash_Delete();  
    
  searchelement->GetPathSearch()->Set(pathsearch_str);
    
  XSTM32DIR_SEARCHINFO* searchinfo = (XSTM32DIR_SEARCHINFO*)searchelement->GetFindFileData();
  
  XSTRING_CREATENORMALIZE(pathsearch_str    , bufferpath, false); 
  XSTRING_CREATENORMALIZE(patternsearch_str , bufferpattern, false); 
  fresult = f_findfirst(&searchinfo->dir, &searchinfo->fileinfo, bufferpath, bufferpattern);  
  XSTRING_DELETENORMALIZE(pathsearch_str    , bufferpath); 
  XSTRING_DELETENORMALIZE(patternsearch_str , bufferpattern); 
  
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
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  searchelement :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XSTM32DIR::NextSearch(XDIRELEMENT* searchelement)
{
  XSTM32DIR_SEARCHINFO* searchinfo = (XSTM32DIR_SEARCHINFO*)searchelement->GetFindFileData();
  if(!searchinfo) return false;
  
  XSTRING_CREATENORMALIZE((*searchelement->GetPatternSearch()) , bufferpattern, false); 
  searchinfo->dir.pat = bufferpattern;   
  fresult = f_findnext(&searchinfo->dir, &searchinfo->fileinfo);
  XSTRING_DELETENORMALIZE((*searchelement->GetPatternSearch()), bufferpattern);
  
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
* @fn         bool XSTM32DIR::ReconvertSearchData(XDIRELEMENT* searchelement)
* @brief      ReconvertSearchData
* @ingroup    PLATFORM_STM32
* 
* @author     Abraham J. Velez 
* @date       01/03/2016 12:00
* 
* @param[in]  searchelement : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XSTM32DIR::ReconvertSearchData(XDIRELEMENT* searchelement, FILINFO* fileinfo)
{
  if(!searchelement)  return false;
  if(!fileinfo)       return false;

  searchelement->GetNameFile()->Set(fileinfo->fname);
     
  XDATETIME*  datetime  = searchelement->GetDateTimeFile_Modificated();
  XWORD       date      = fileinfo->fdate;
  XWORD       time      = fileinfo->ftime;
  
  
  // bit31:25    Year origin from the 1980 (0..127, e.g. 37 for 2017)
  // bit24:21    Month (1..12)
  // bit20:16    Day of the month (1..31)
    
  // bit15:11    Hour (0..23)
  // bit10:5     Minute (0..59)
  // bit4:0      Second / 2 (0..29, e.g. 25 for 50)
  
  datetime->SetYear(((date >> 9) & 0x7F) + 1980);
  datetime->SetMonth((date >> 5) & 0x0F);
  datetime->SetDay((date & 0x1F));
  
  datetime->SetHours((time >> 11) & 0x1F);
  datetime->SetMinutes((time >> 5) & 0x3F);
  datetime->SetSeconds((time & 0x1F) * 2);
    
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
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XSTM32DIR::Clean()
{
  memset(&userFATFS, 0, sizeof(FATFS));
  
  fresult = FR_OK; 
}

