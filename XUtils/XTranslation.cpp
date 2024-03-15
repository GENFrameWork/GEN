/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XTranslation.cpp
* 
* @class      XTRANSLATION
* @brief      eXtended Utils Language class
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

/*---- PRECOMPILATION INCLUDES ----------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XTranslation.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "XBase.h"
#ifdef XFILE_ACTIVE
#include "XFileJSON.h"
#endif
#include "XTranslation_GEN.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

XTRANSLATION* XTRANSLATION::instance  = NULL;

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRANSLATION::GetIsInstanced()
* @brief      Get Is Instanced
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRANSLATION::GetIsInstanced()
{
  return instance!=NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XTRANSLATION& XTRANSLATION::GetInstance()
* @brief      Get Instance
* @ingroup    XUTILS
*
* @return     XTRANSLATION& : language instance
*
* --------------------------------------------------------------------------------------------------------------------*/
XTRANSLATION& XTRANSLATION::GetInstance()
{
  if(!instance) instance = new XTRANSLATION();

  return (*instance);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRANSLATION::DelInstance()
* @brief      Del Instance
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRANSLATION::DelInstance()
{
  if(instance)
    {
      delete instance;
      instance = NULL;

      return true;
    }

  return false;
}



#ifdef XFILE_ACTIVE
/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRANSLATION::Ini(XPATH& xpath)
* @brief      Ini language class
* @ingroup    XUTILS
*
* @param[in]  xpath : path of the language file
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRANSLATION::Ini(XPATH& xpath)
{
   this->xpath = xpath;

   return LoadLanguageAvailable();
}
#else
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XTRANSLATION::Ini()
* @brief      Ini
* @ingroup    XUTILS
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRANSLATION::Ini()
{   
   return LoadLanguageAvailable();
}
#endif


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD XTRANSLATION::GetActual()
* @brief      GetActual
* @ingroup    XUTILS
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD XTRANSLATION::GetActual()
{
  return code;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XTRANSLATION::SetActual(XDWORD code)
* @brief      SetActual
* @ingroup    XUTILS
* 
* @param[in]  code : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRANSLATION::SetActual(XDWORD code)
{
  if(this->code == code) return false;

  if((!IsLanguageAvailable(code)) && (!XTRANSLATION_GEN::GetInstance().IsLanguageAvailable(code))) return false;

  this->code = code;

  Translate_Delete();

  XTRANSLATION_GEN::GetInstance().Sentences_AddToTranslation(code);

  return Translate_Load();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRANSLATION::Translate_Add(XDWORD ID, XCHAR* sentence, XDWORD fixed)
* @brief      Translate_Add
* @ingroup    XUTILS
*
* @param[in]  ID :
* @param[in]  sentence :
* @param[in]  fixed :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRANSLATION::Translate_Add(XDWORD ID, XCHAR* sentence, XDWORD fixed)
{
  if(!ID)       return false;
  if(!sentence) return false;

  XDWORD sizesentence = XSTRING::GetSize(sentence);
  if(fixed)
    {
      if(sizesentence > fixed) sizesentence = fixed;
    }

  XCHAR* newsentence = new XCHAR[sizesentence+1];
  if(newsentence)
    {
      memset(newsentence, 0        , (sizesentence+1) * sizeof(XCHAR));
      memcpy(newsentence, sentence ,  sizesentence    * sizeof(XCHAR));

      sentences[ID] = newsentence;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XCHAR* XTRANSLATION::Translate_GetSentence(XDWORD ID)
* @brief      Translate_GetSentence
* @ingroup    XUTILS
* 
* @param[in]  ID : 
* 
* @return     XCHAR* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* XTRANSLATION::Translate_GetSentence(XDWORD ID)
{
  if(ID >= XTRANSLATION_MAXSENTENCES) return emptysentence.Get();
  if(!sentences[ID])  return emptysentence.Get();

  return sentences[ID];
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRANSLATION::End()
* @brief      End
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRANSLATION::End()
{
  languageavailables.DeleteAll();

  Translate_Delete();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XTRANSLATION::XTRANSLATION()
* @brief      Constructor
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
XTRANSLATION::XTRANSLATION()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XTRANSLATION::~XTRANSLATION()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
XTRANSLATION::~XTRANSLATION()
{
  End();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRANSLATION::LoadLanguageAvailable()
* @brief      LoadLanguageAvailable
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRANSLATION::LoadLanguageAvailable()
{
  #ifdef XFILE_ACTIVE

  XFILEJSON xfilejson;

  if(!xfilejson.Open(xpath))       return false;
  if(!xfilejson.ReadAllFile())     return false;
  if(!xfilejson.DecodeAllLines())  return false;

  XFILEJSONARRAY* languages  = (XFILEJSONARRAY*)xfilejson.GetObject(__L("languages"));
  if(languages)
    {
      for(XDWORD c=0; c<languages->GetValues()->GetSize(); c++)
        {
          XFILEJSONVALUE* value = languages->GetValues()->Get(c);
          if(value)
            {
              if(value->GetType() == XFILEJSONVALUETYPE_STRING)
                {
                  XSTRING strvalue = (XSTRING)(value->GetValueString());
                  
                  if(!strvalue.IsEmpty())
                    {
                      XDWORD code = ISO_639_3.Code_GetByCodeAlpha3(strvalue.Get());
                      if(code != XLANGUAGE_ISO_639_3_CODE_INVALID)
                        {
                          languageavailables.Add(code);
                        }
                        else
                        {
                          XDWORD code = ISO_639_3.Code_GetByEnglishName(strvalue.Get());
                          if(code != XLANGUAGE_ISO_639_3_CODE_INVALID)
                            {
                              languageavailables.Add(code);
                            }
                            else
                            {
                              XDWORD code = ISO_639_3.Code_GetByAlias(strvalue.Get());
                              if(code != XLANGUAGE_ISO_639_3_CODE_INVALID)
                                {
                                  languageavailables.Add(code);
                                }
                            }
                        }
                    }
                    
                }
            }
        }
    }

  xfilejson.Close();

  return true;

  #else

  return false;

  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRANSLATION::IsLanguageAvailable(XDWORD code)
* @brief      IsLanguageAvailable
* @ingroup    XUTILS
*
* @param[in]  code :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRANSLATION::IsLanguageAvailable(XDWORD code)
{
  if(GetIndexLanguageByCode(code)!= -1) return true;

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRANSLATION::AddLanguageAvailable(XDWORD code)
* @brief      AddLanguageAvailable
* @ingroup    XUTILS
*
* @param[in]  code :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRANSLATION::AddLanguageAvailable(XDWORD code)
{
  if(!IsLanguageAvailable(code)) return false;

  languageavailables.Add(code);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XTRANSLATION::GetIndexLanguageByCode(XDWORD code)
* @brief      GetIndexLanguageByCode
* @ingroup    XUTILS
*
* @param[in]  code :
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XTRANSLATION::GetIndexLanguageByCode(XDWORD code)
{
  for(XDWORD c=0; c<languageavailables.GetSize(); c++)
    {
      if(languageavailables.Get(c) == code) return c;
    }

  return -1;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRANSLATION::Translate_Load()
* @brief      LoadTranslate
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRANSLATION::Translate_Load()
{
  #ifdef XFILE_ACTIVE

  XFILEJSON xfilejson;

  if(!xfilejson.Open(xpath))       return false;
  if(!xfilejson.ReadAllFile())     return false;
  if(!xfilejson.DecodeAllLines())  return false;

  int languageindex = GetIndexLanguageByCode(code);
  if(languageindex == -1) return false;

  XFILEJSONARRAY* translations  = (XFILEJSONARRAY*)xfilejson.GetObject(__L("translations"));
  if(translations)
    {
      for(XDWORD c=0; c<translations->GetValues()->GetSize(); c++)
        {
          XFILEJSONVALUE* translation = translations->GetValues()->Get(c);
          if(translation)
            {
              XSTRING IDstr = translation->GetName()->Get();
              if(!IDstr.IsEmpty())
                {
                  XDWORD ID = IDstr.ConvertToDWord();
                  if(ID < XTRANSLATION_MAXSENTENCES)
                    {
                      XFILEJSONARRAY* translationarray  = (XFILEJSONARRAY*)translation->GetValuePointer();
                      if(translationarray)
                        {
                          XDWORD   sizesentencefixed = (XDWORD)translationarray->GetValues()->Get(0)->GetValueInteger();
                          XSTRING  sentence          = (XSTRING)translationarray->GetValues()->Get(languageindex+1)->GetValueString();

                          Translate_Add(ID, sentence.Get(), sizesentencefixed);
                        }
                    }
                }
            }
        }
    }

  xfilejson.Close();

  return true;

  #else

  return false;

  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XTRANSLATION::Translate_Delete()
* @brief      Translate_Delete
* @ingroup    XUTILS
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRANSLATION::Translate_Delete()
{
  for(XDWORD c=0; c<XTRANSLATION_MAXSENTENCES; c++)
    {
      if(sentences[c])
        {
          delete [] sentences[c];
          sentences[c] = NULL;
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRANSLATION::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRANSLATION::Clean()
{
  code  = XLANGUAGE_ISO_639_3_CODE_INVALID;

  emptysentence = __L("--");

  for(XDWORD c=0; c<XTRANSLATION_MAXSENTENCES; c++)
    {
      sentences[c] = NULL;
    }
}


#pragma endregion


