/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOWebPageHTMLCreator.cpp
* 
* @class      DIOWEBPAGEHTMLCREATOR
* @brief      Data Input/Output Web Page HTML Create class
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

#include "DIOWebPageHTMLCreator.h"

#include "XFactory.h"
#include "XFileTXT.h"
#include "XBuffer.h"
#include "XTrace.h"

#include "DIOWebServer.h"
#include "DIOWebPageHTMLCreator_XEvent.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOWEBPAGEHTMLCREATOR::DIOWEBPAGEHTMLCREATOR()
* @brief      Constructor
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOWEBPAGEHTMLCREATOR::DIOWEBPAGEHTMLCREATOR()
{
  Clean();

  RegisterEvent(DIOWEBPAGEHTMLCREATOR_XEVENT_TYPE_XTAGFOUND);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOWEBPAGEHTMLCREATOR::~DIOWEBPAGEHTMLCREATOR()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOWEBPAGEHTMLCREATOR::~DIOWEBPAGEHTMLCREATOR()
{
  DeRegisterEvent(DIOWEBPAGEHTMLCREATOR_XEVENT_TYPE_XTAGFOUND);

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOWEBPAGEHTMLCREATOR::operator = (const char* htlmpage)
* @brief      operator =
* @ingroup    DATAIO
*
* @param[in]  htlmpage :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBPAGEHTMLCREATOR::operator = (const char* htlmpage)
{
  Set(htlmpage);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOWEBPAGEHTMLCREATOR::operator = (const XCHAR* htlmpage)
* @brief      operator =
* @ingroup    DATAIO
*
* @param[in]  htlmpage :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBPAGEHTMLCREATOR::operator = (const XCHAR* htlmpage)
{
  Set(htlmpage);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOWEBPAGEHTMLCREATOR::operator = (DIOWEBPAGEHTMLCREATOR& htlmpage)
* @brief      operator =
* @ingroup    DATAIO
*
* @param[in]  htlmpage :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBPAGEHTMLCREATOR::operator = (DIOWEBPAGEHTMLCREATOR& htlmpage)
{
  Set(htlmpage);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOWEBPAGEHTMLCREATOR::operator = (XSTRING& string)
* @brief      operator =
* @ingroup    DATAIO
*
* @param[in]  string :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBPAGEHTMLCREATOR::operator = (XSTRING& string)
{
  Set(string);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOWEBPAGEHTMLCREATOR::operator = (XWORD* htlmpage)
* @brief      operator =
* @ingroup    DATAIO
*
* @param[in]  htlmpage :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBPAGEHTMLCREATOR::operator = (XWORD* htlmpage)
{
  Set(htlmpage);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOWEBPAGEHTMLCREATOR::operator += (const char* htlmpage)
* @brief      operator +=
* @ingroup    DATAIO
*
* @param[in]  htlmpage :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBPAGEHTMLCREATOR::operator += (const char* htlmpage)
{
  Add(htlmpage);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOWEBPAGEHTMLCREATOR::operator += (const XCHAR* htlmpage)
* @brief      operator +=
* @ingroup    DATAIO
*
* @param[in]  htlmpage :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBPAGEHTMLCREATOR::operator += (const XCHAR* htlmpage)
{
  Add(htlmpage);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOWEBPAGEHTMLCREATOR::operator += (XPATH& htlmpage)
* @brief      operator +=
* @ingroup    DATAIO
*
* @param[in]  htlmpage :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBPAGEHTMLCREATOR::operator += (XPATH& htlmpage)
{
  Add(htlmpage);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOWEBPAGEHTMLCREATOR::operator += (XSTRING& string)
* @brief      operator +=
* @ingroup    DATAIO
*
* @param[in]  string :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBPAGEHTMLCREATOR::operator += (XSTRING& string)
{
  Add(string);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOWEBPAGEHTMLCREATOR::operator += (XCHAR htlmpage)
* @brief      operator +=
* @ingroup    DATAIO
*
* @param[in]  htlmpage :
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOWEBPAGEHTMLCREATOR::operator += (XCHAR htlmpage)
{
  Add(htlmpage);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::operator < (DIOWEBPAGEHTMLCREATOR& htlmpage)
* @brief      operator <
* @ingroup    DATAIO
*
* @param[in]  htlmpage :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::operator <  (DIOWEBPAGEHTMLCREATOR& htlmpage)
{
  return (bool)(Compare(htlmpage) == -1);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::operator > (DIOWEBPAGEHTMLCREATOR& htlmpage)
* @brief      operator >
* @ingroup    DATAIO
*
* @param[in]  htlmpage :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::operator >  (DIOWEBPAGEHTMLCREATOR& htlmpage)
{
  return (bool)(Compare(htlmpage) ==  1);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::operator <= (DIOWEBPAGEHTMLCREATOR& htlmpage)
* @brief      operator <=
* @ingroup    DATAIO
*
* @param[in]  htlmpage :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::operator <= (DIOWEBPAGEHTMLCREATOR& htlmpage)
{
  return (bool)(Compare(htlmpage) !=  1);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::operator >= (DIOWEBPAGEHTMLCREATOR& htlmpage)
* @brief      operator >=
* @ingroup    DATAIO
*
* @param[in]  htlmpage :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::operator >= (DIOWEBPAGEHTMLCREATOR& htlmpage)
{
  return (bool)(Compare(htlmpage) != -1);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::operator == (DIOWEBPAGEHTMLCREATOR& htlmpage)
* @brief      operator ==
* @ingroup    DATAIO
*
* @param[in]  htlmpage :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::operator == (DIOWEBPAGEHTMLCREATOR& htlmpage)
{
  return (bool)(Compare(htlmpage) ==  0);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::operator != (DIOWEBPAGEHTMLCREATOR& htlmpage)
* @brief      operator !=
* @ingroup    DATAIO
*
* @param[in]  htlmpage :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::operator != (DIOWEBPAGEHTMLCREATOR& htlmpage)
{
  return (bool)(Compare(htlmpage) !=  0);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::operator < (XSTRING& string)
* @brief      operator <
* @ingroup    DATAIO
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::operator <  (XSTRING& string)
{
  return (bool)(Compare(string) == -1);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::operator > (XSTRING& string)
* @brief      operator >
* @ingroup    DATAIO
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::operator >  (XSTRING& string)
{
  return (bool)(Compare(string) ==  1);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::operator <= (XSTRING& string)
* @brief      operator <=
* @ingroup    DATAIO
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::operator <= (XSTRING& string)
{
  return (bool)(Compare(string) !=  1);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::operator >= (XSTRING& string)
* @brief      operator >=
* @ingroup    DATAIO
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::operator >= (XSTRING& string)
{
  return (bool)(Compare(string) != -1);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::operator == (XSTRING& string)
* @brief      operator ==
* @ingroup    DATAIO
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::operator == (XSTRING& string)
{
  return (bool)(Compare(string) ==  0);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::operator != (XSTRING& string)
* @brief      operator !=
* @ingroup    DATAIO
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::operator != (XSTRING& string)
{
  return (bool)(Compare(string) !=  0);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR DIOWEBPAGEHTMLCREATOR::operator [] (int position)
* @brief      operator []
* @ingroup    DATAIO
*
* @param[in]  position :
*
* @return     XCHAR :
*
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR DIOWEBPAGEHTMLCREATOR::operator [] (int position)
{
  if(IsEmpty())           return 0;
  if(position<0)          return text[0];
  if(position>=(int)size) return text[size-1];
  return text[position];
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::Printf(XCHAR* mask,...)
* @brief      Printf
* @ingroup    DATAIO
*
* @param[in]  mask :
* @param[in]  ... :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::Printf(XCHAR* mask,...)
{
  XSTRING outstring;

  va_list arg;

  va_start(arg, mask);

  outstring.FormatArg(mask, &arg, true);

  va_end(arg);

  Add(outstring.Get());

  //--------------------------------------------------
  // To See better the sorce code of WEB page
  #ifdef GEN_DEBUG
  //Add(__L("\n"));
  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::Print(XCHAR* text)
* @brief      Print
* @ingroup    DATAIO
*
* @param[in]  text :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::Print(XCHAR* text)
{
  Add(text);

  //--------------------------------------------------
  // To See better the sorce code of WEB page
  #ifdef GEN_DEBUG
  //Add(__L("\n"));
  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBPAGEHTMLCREATOR::Table_Ini(int border, XCHAR* addstyle)
* @brief      Table_Ini
* @ingroup    DATAIO
* 
* @param[in]  border : 
* @param[in]  addstyle : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::Table_Ini(int border, XCHAR* addstyle)
{
  this->tableborder = border;

  Printf(__L("<table style=\"border-collapse: collapse; %s \">"), (addstyle?addstyle:__L("")));

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::Table_Line(int ncolumns, ...)
* @brief      Table_Line
* @ingroup    DATAIO
*
* @param[in]  ncolumns :
* @param[in]  ... :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::Table_Line(int ncolumns, ...)
{
  va_list arg;

  va_start(arg, ncolumns);

  Printf(__L("<tr>"));

  for(int c=0; c<ncolumns; c++)
    {
      int                         width = va_arg(arg, int);
      DIOWEBPAGEHTMLCREATORALIGN  align = (DIOWEBPAGEHTMLCREATORALIGN)va_arg(arg, int);
      XCHAR*                      text  = va_arg(arg, XCHAR*);
      XSTRING                     alignstr;
      XSTRING                     textrev;

      textrev.FormatArg(text, &arg, true);

      alignstr.Empty();

      switch(align)
        {
          case DIOWEBPAGEHTMLCREATORALIGN_NONE   : alignstr = __L("");        break;
          case DIOWEBPAGEHTMLCREATORALIGN_LEFT   : alignstr = __L("left");    break;
          case DIOWEBPAGEHTMLCREATORALIGN_RIGHT  : alignstr = __L("right");   break;
          case DIOWEBPAGEHTMLCREATORALIGN_CENTER : alignstr = __L("center");  break;
        }

      Printf(__L("<td style=\"border: %dpx solid silver;\" width=\"%d\" align=\"%s\">%s</td>"), tableborder, width, alignstr.Get(), textrev.Get());

    }

  Printf(__L("</tr>"));

  va_end(arg);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOWEBPAGEHTMLCREATOR::Table_Line(XCHAR* colorbackground, int ncolumns, ...)
* @brief      Table_Line
* @ingroup    DATAIO
* 
* @param[in]  colorbackground : 
* @param[in]  ncolumns : 
* @param[in]  ... : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::Table_Line(XCHAR* colorbackground, int ncolumns, ...)
{
  va_list arg;

  va_start(arg, ncolumns);

  XSTRING tr;
  XSTRING color;
  color = colorbackground;

  if(!color.IsEmpty())
    {
      tr = __L("<tr bgcolor=\"");
      tr += colorbackground;
      tr += "\">";
    }
   else tr = __L("<tr>");

  Printf(tr.Get());

  for(int c=0; c<ncolumns; c++)
    {
      int                         width = va_arg(arg, int);
      DIOWEBPAGEHTMLCREATORALIGN  align = (DIOWEBPAGEHTMLCREATORALIGN)va_arg(arg, int);
      XCHAR*                      text  = va_arg(arg, XCHAR*);
      XSTRING                     alignstr;
      XSTRING                     textrev;

      textrev.FormatArg(text, &arg, true);

      alignstr.Empty();

      switch(align)
        {
          case DIOWEBPAGEHTMLCREATORALIGN_NONE   : alignstr = __L("");        break;
          case DIOWEBPAGEHTMLCREATORALIGN_LEFT   : alignstr = __L("left");    break;
          case DIOWEBPAGEHTMLCREATORALIGN_RIGHT  : alignstr = __L("right");   break;
          case DIOWEBPAGEHTMLCREATORALIGN_CENTER : alignstr = __L("center");  break;
        }

      Printf(__L("<td style=\"border: %dpx solid silver;\" width=\"%d\" align=\"%s\">%s</td>"), tableborder, width, alignstr.Get(), textrev.Get());
    }

  Printf(__L("</tr>"));

  va_end(arg);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::Table_LineColor(int columns, ...)
* @brief      Table_LineColor
* @ingroup    DATAIO
*
* @param[in]  columns :
* @param[in]  ... :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::Table_LineColor(int ncolumns, ...)
{
  va_list arg;

  va_start(arg, ncolumns);

  Printf(__L("<tr>"));

  for(int c=0; c<ncolumns; c++)
    {
      int                         width     = va_arg(arg, int);
      XCHAR*                      colortxt  = va_arg(arg, XCHAR*);
      XCHAR*                      colorbkg  = va_arg(arg, XCHAR*);
      DIOWEBPAGEHTMLCREATORALIGN  align     = (DIOWEBPAGEHTMLCREATORALIGN)va_arg(arg, int);
      XCHAR*                      text      = va_arg(arg, XCHAR*);
      XSTRING                     alignstr;
      XSTRING                     textrev;

      textrev.FormatArg(text, &arg, true);

      alignstr.Empty();

      switch(align)
        {
          case DIOWEBPAGEHTMLCREATORALIGN_NONE   : alignstr = __L("");        break;
          case DIOWEBPAGEHTMLCREATORALIGN_LEFT   : alignstr = __L("left");    break;
          case DIOWEBPAGEHTMLCREATORALIGN_RIGHT  : alignstr = __L("right");   break;
          case DIOWEBPAGEHTMLCREATORALIGN_CENTER : alignstr = __L("center");  break;
        }

      Printf(__L("<td bgcolor=\"%s\" style=\"border: %dpx solid silver;\" width=\"%d\" align=\"%s\"><font color=\"%s\">%s</font></td>"), colorbkg, tableborder, width, alignstr.Get(), colortxt, textrev.Get());
    }

  Printf(__L("</tr>"));

  va_end(arg);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::Table_End()
* @brief      Table_End
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::Table_End()
{
  Printf(__L("</table>"));
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::AddAutoRefresh(int refresh, XSTRING* url)
* @brief      AddAutoRefresh
* @ingroup    DATAIO
*
* @param[in]  refresh :
* @param[in]  url :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::AddAutoRefresh(int refresh, XSTRING* url)
{
  if(refresh<0) return false;

  XSTRING meta;

  if(url)
        meta.Format(__L("<META http-equiv=\"refresh\" content=\"%d;URL=%s\"/>"), refresh, url->Get());
   else meta.Format(__L("<META http-equiv=\"refresh\" content=\"%d;\"/>")      , refresh);

  Add(meta.Get());

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOWEBPAGEHTMLCREATOR::AddFile(XPATH& xpath)
* @brief      AddFile
* @ingroup    DATAIO
*
* @param[in]  xpath :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOWEBPAGEHTMLCREATOR::AddFile(XPATH& xpath)
{
  XFILETXT filetxt;
  bool     status = true;
  bool     intag  = false;

  if(!filetxt.Open(xpath)) return false;

  if(filetxt.ReadAllFile())
    {
      Empty();

      XBUFFER LFdata;

      filetxt.GetLF(LFdata);

      for(int c=0;c<filetxt.GetNLines();c++)
        {
          XSTRING* line = filetxt.GetLine(c);
          if(line)
            {
              XSTRING inioutput;
              XSTRING endoutput;
              int     result;

              result = CheckXTAG(line, intag, inioutput, endoutput);
              switch(result)
                {
                  case DIOWEBPAGEHTMLCREATORTAGXENGINESTATUS_NOTFOUND   : { if(!line->IsEmpty())
                                                                              {
                                                                                if(Add(*line)) Add(filetxt.GetLF()->Get());
                                                                              }
                                                                          }
                                                                          break;

                  case DIOWEBPAGEHTMLCREATORTAGXENGINESTATUS_FOUNDEND   : { if(!inioutput.IsEmpty())
                                                                              {
                                                                                if(Add(inioutput)) Add(filetxt.GetLF()->Get());
                                                                              }

                                                                            DIOWEBPAGEHTMLCREATOR_XEVENT xevent(this, DIOWEBPAGEHTMLCREATOR_XEVENT_TYPE_XTAGFOUND);

                                                                            xevent.SetPageHTMLCreator(this);
                                                                            xevent.SetXTAG(&lastXTAG);

                                                                            PostEvent(&xevent);

                                                                            if(!endoutput.IsEmpty())
                                                                              {
                                                                                if(Add(endoutput)) Add(filetxt.GetLF()->Get());
                                                                              }
                                                                          }
                                                                          break;

                  case DIOWEBPAGEHTMLCREATORTAGXENGINESTATUS_FOUNDNOTEND: { if(!inioutput.IsEmpty())
                                                                              {
                                                                                if(Add(inioutput)) Add(filetxt.GetLF()->Get());
                                                                              }

                                                                            if(!endoutput.IsEmpty())
                                                                              {
                                                                                if(Add(endoutput)) Add(filetxt.GetLF()->Get());
                                                                              }
                                                                          }
                                                                          break;
                }
            }
        }
    }

  filetxt.Close();

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOWEBPAGEHTMLCREATORTAGXENGINESTATUS DIOWEBPAGEHTMLCREATOR::CheckXTAG(XSTRING* originalline,bool& intag, XSTRING& inioutputline,XSTRING& endoutputline)
* @brief      CheckXTAG
* @ingroup    DATAIO
*
* @param[in]  originalline :
* @param[in]  intag :
* @param[in]  inioutputline :
* @param[in]  endoutputline :
*
* @return     DIOWEBPAGEHTMLCREATORTAGXENGINESTATUS :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOWEBPAGEHTMLCREATORTAGXENGINESTATUS DIOWEBPAGEHTMLCREATOR::CheckXTAG(XSTRING* originalline,bool& intag, XSTRING& inioutputline,XSTRING& endoutputline)
{
  int index[3];

  inioutputline.Empty();
  endoutputline.Empty();

  if(!intag)
    {
      index[0] = originalline->Find(__L("<?"), true);
      if(index[0]!=XSTRING_NOTFOUND)
        {
          index[1] = originalline->Find(DIOWEBPAGEHTMLCREATOR_TAGXENGINE, true);
          if(index[1]!=XSTRING_NOTFOUND)
            {
              lastXTAG.Empty();

              originalline->Copy(0,index[0],inioutputline);

              index[2] = originalline->Find(__L("?>"),true);
              if(index[2]!=XSTRING_NOTFOUND)
                {
                  XSTRING endline;

                  originalline->Copy(index[1]+3,index[2],lastXTAG);
                  originalline->Copy(index[2]+2,endoutputline);

                  return DIOWEBPAGEHTMLCREATORTAGXENGINESTATUS_FOUNDEND;

                }
               else
                {
                  originalline->Copy(index[1]+3,lastXTAG);
                  intag = true;
                  return  DIOWEBPAGEHTMLCREATORTAGXENGINESTATUS_FOUNDNOTEND;
                }
            }
        }

      inioutputline = originalline->Get();
    }
   else
    {
      index[0] = originalline->Find(__L("?>"),true);
      if(index[0]!=XSTRING_NOTFOUND)
        {
          XSTRING TAGstring;

          originalline->Copy(0,index[0],TAGstring);
          lastXTAG+=TAGstring.Get();

          originalline->Copy(index[0]+2,endoutputline);

          intag = false;

          return DIOWEBPAGEHTMLCREATORTAGXENGINESTATUS_FOUNDEND;
        }

      lastXTAG += originalline->Get();

      inioutputline.Empty();
      endoutputline.Empty();

      return DIOWEBPAGEHTMLCREATORTAGXENGINESTATUS_FOUNDNOTEND;
    }

  return DIOWEBPAGEHTMLCREATORTAGXENGINESTATUS_NOTFOUND;
}


#pragma endregion

