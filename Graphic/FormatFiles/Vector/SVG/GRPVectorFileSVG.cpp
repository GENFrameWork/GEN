/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPVectorFileSVG.cpp
* 
* @class      GRPVECTORFILESVG
* @brief      Graphic Vector File SVG class
* @ingroup    GRAPHIC
* 
* @copyright  EndoraSoft. All rights reserved.
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

#include "GEN_Defines.h"



/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "GRPVectorFileSVG.h"

#include "XTrace.h"
#include "XFileTXT.h"
#include "XFileXML.h"

#include "GRPVectorFileSVGObj.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPVECTORFILESVG::GRPVECTORFILESVG()
* @brief      Constructor of class
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILESVG::GRPVECTORFILESVG()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPVECTORFILESVG::~GRPVECTORFILESVG()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILESVG::~GRPVECTORFILESVG()
{
  if(root)
    {
      GEN_DELETE root;
      root = NULL;
    }

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPVECTORFILERESULT GRPVECTORFILESVG::DetectType()
* @brief      Detect type : check the extension (.svg) and that the file is a text file with a <svg> root tag
* @ingroup    GRAPHIC
*
* @return     GRPVECTORFILERESULT : result
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILERESULT GRPVECTORFILESVG::DetectType()
{
  GRPVECTORFILERESULT result = GRPVECTORFILERESULT_ERRORINVALIDFORMAT;

  if(pathfile.IsEmpty())
    {
      type = GRPVECTORFILETYPE_UNKNOWN;

      return GRPVECTORFILERESULT_ERRORNOTFILE;
    }

  XSTRING extension;
  pathfile.GetExt(extension);

  if(extension.IsEmpty())
    {
      type = GRPVECTORFILETYPE_UNKNOWN;

      return GRPVECTORFILERESULT_ERRORNOTFILE;
    }

  if(!extension.Compare(GRPVECTORFILESVG_EXTENSION, true))
    {
      type = GRPVECTORFILETYPE_SVG;
    }

  if(DetectFileFormatText(pathfile))
    {
      XFILETXT* file = GEN_NEW XFILETXT();
      if(file)
        {
          if(file->Open(pathfile))
            {
              int nlines = 200;

              file->ReadNLines(nlines);

              nlines = file->GetNLines();

              if(DetectIsSVG(file, nlines))
                {
                  result = GRPVECTORFILERESULT_OK;
                }
               else
                {
                  result = GRPVECTORFILERESULT_ERRORINVALIDFORMAT;
                }

              file->Close();
            }
           else
            {
              result = GRPVECTORFILERESULT_ERRORNOTFILE;
            }

          GEN_DELETE file;
        }
       else
        {
          result = GRPVECTORFILERESULT_ERRORNOTMEMORY;
        }
    }
   else
    {
      result = GRPVECTORFILERESULT_ERRORINVALIDFORMAT;
    }

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPVECTORFILERESULT GRPVECTORFILESVG::Load()
* @brief      Load : parse the XML, read the <svg> config and build the SVG object tree
* @ingroup    GRAPHIC
*
* @return     GRPVECTORFILERESULT : result
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILERESULT GRPVECTORFILESVG::Load()
{
  GRPVECTORFILERESULT result = GRPVECTORFILERESULT_ERRORNOTMEMORY;

  XFILEXML* file = GEN_NEW XFILEXML();
  if(!file)
    {
      return GRPVECTORFILERESULT_ERRORNOTMEMORY;
    }

  if(file->Open(pathfile, true))
    {
      if(file->ReadAndDecodeAllLines())
        {
          XFILEXMLELEMENT* xmlroot = file->GetRoot();
          if(xmlroot && !xmlroot->GetName().Compare(__L("svg"), true))
            {
              config.ApplyData(xmlroot);

              if(root)
                {
                  GEN_DELETE root;
                  root = NULL;
                }

              root = GRPVECTORFILESVGOBJ::CreateInstance(xmlroot);

              #ifdef XTRACE_ACTIVE
              if(root)
                {
                  XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("SVG loaded: width=%f height=%f viewBox=%d"), config.GetWidth(),
                                                                                                        config.GetHeight(),
                                                                                                        config.HasViewBox()?1:0);
                  root->Trace(0);
                }
              #endif

              result = root ? GRPVECTORFILERESULT_OK : GRPVECTORFILERESULT_ERRORNOTMEMORY;
            }
           else
            {
              result = GRPVECTORFILERESULT_ERRORINVALIDFORMAT;
            }
        }
       else
        {
          result = GRPVECTORFILERESULT_ERRORINVALIDFORMAT;
        }

      file->Close();
    }
   else
    {
      result = GRPVECTORFILERESULT_ERRORNOTFILE;
    }

  GEN_DELETE file;

  return result;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPVECTORFILECONFIG* GRPVECTORFILESVG::GetConfig()
* @brief      Get config
* @ingroup    GRAPHIC
*
* @return     GRPVECTORFILECONFIG* : config
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILECONFIG* GRPVECTORFILESVG::GetConfig()
{
  return &config;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPVECTORFILESVGOBJ* GRPVECTORFILESVG::GetRoot()
* @brief      Get root : the root SVG object (the <svg> node)
* @ingroup    GRAPHIC
*
* @return     GRPVECTORFILESVGOBJ* : root object
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILESVGOBJ* GRPVECTORFILESVG::GetRoot()
{
  return root;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRPVECTORFILESVG::DetectIsSVG(XFILETXT* file, int nlinesmax)
* @brief      Detect is SVG : look for a <svg tag in the first lines of the file
* @note       INTERNAL
* @ingroup    GRAPHIC
*
* @param[in]  file : text file
* @param[in]  nlinesmax : max lines to scan
*
* @return     bool : true if a <svg tag is found.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILESVG::DetectIsSVG(XFILETXT* file, int nlinesmax)
{
  if(!file) return false;

  for(int c=0; c<nlinesmax; c++)
    {
      XSTRING* line = file->GetLine(c);
      if(line)
        {
          if(line->Find(__L("<svg"), true, 0) >= 0)
            {
              return true;
            }
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRPVECTORFILESVG::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILESVG::Clean()
{
  root = NULL;
}




