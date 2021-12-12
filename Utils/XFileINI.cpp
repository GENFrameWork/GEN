/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XFileINI.cpp
*
* @class      XFILEINI
* @brief      eXtended INI (Windows INItialization file) file class
* @ingroup    UTILS
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

#include <stdio.h>
#include <string.h>

#include "XPath.h"

#include "XFileINI.h"

#include "XMemory_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/
/*  XFILEINIKEY                                                                                                       */
/*--------------------------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEINIKEY::XFILEINIKEY()
* @brief      Constructor
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILEINIKEY::XFILEINIKEY()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEINIKEY::~XFILEINIKEY()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILEINIKEY::~XFILEINIKEY()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XFILEINIKEY::GetKey()
* @brief      GetKey
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     XSTRING* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XFILEINIKEY::GetKey()
{
  return &key;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINIKEY::SetKey(XSTRING& key)
* @brief      SetKey
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  key :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINIKEY::SetKey(XSTRING& key)
{
  this->key = key;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XFILEINIKEY::GetValue()
* @brief      GetValue
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     XSTRING* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XFILEINIKEY::GetValue()
{
  return &value;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINIKEY::SetValue(XSTRING& value)
* @brief      SetValue
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINIKEY::SetValue(XSTRING& value)
{
  this->value = value;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILEINIKEY::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILEINIKEY::Clean()
{

}



/*--------------------------------------------------------------------------------------------------------------------*/
/*  XFILEINISECTION                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEINISECTION::XFILEINISECTION()
* @brief      Constructor
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILEINISECTION::XFILEINISECTION()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEINISECTION::~XFILEINISECTION()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILEINISECTION::~XFILEINISECTION()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XFILEINISECTION::GetName()
* @brief      GetName
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     XSTRING* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XFILEINISECTION::GetName()
{
  return &section;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINISECTION::SetName(XSTRING& section)
* @brief      SetName
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINISECTION::SetName(XSTRING& section)
{
  this->section = section;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEINIKEY* XFILEINISECTION::FindKey(XSTRING& key)
* @brief      FindKey
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  key :
*
* @return     XFILEINIKEY* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILEINIKEY* XFILEINISECTION::FindKey(XSTRING& key)
{
  if(keys.IsEmpty()) return NULL;

  for(XDWORD c=0;c<keys.GetSize();c++)
    {
      XFILEINIKEY* okey = (XFILEINIKEY*)keys.Get(c);
      if(okey)
        {
          if(okey->GetKey())
            {
              if(!okey->GetKey()->Compare(key, true)) return okey;
            }
        }
    }

  return NULL;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINISECTION::GetKey(XSTRING& key,XSTRING& value)
* @brief      GetKey
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINISECTION::GetKey(XSTRING& key,XSTRING& value)
{
  XFILEINIKEY* okey=FindKey(key);

  if(!okey) return false;

  if(okey->GetValue()) value = (*okey->GetValue()); else return false;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINISECTION::SetKey(XSTRING& key,XSTRING& value)
* @brief      SetKey
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINISECTION::SetKey(XSTRING& key,XSTRING& value)
{
  XFILEINIKEY* okey  = FindKey(key);
  bool         isnew = false;

  if(!okey) isnew=true;

  if(isnew)
    {
      okey = new XFILEINIKEY();
      if(!okey) return false;
    }

  okey->SetKey(key);
  okey->SetValue(value);

  if(isnew) keys.Add(okey);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINISECTION::DeleteKey(XSTRING& key)
* @brief      DeleteKey
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  key :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINISECTION::DeleteKey(XSTRING& key)
{
  XFILEINIKEY* okey  = FindKey(key);
  if(!okey) return false;

  keys.Delete(okey);
  delete okey;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINISECTION::GetNextKey(XSTRING& key,XSTRING& value)
* @brief      GetNextKey
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINISECTION::GetNextKey(XSTRING& key,XSTRING& value)
{
  if(selectkey>=keys.GetSize()) return false;

  XFILEINIKEY* skey = (XFILEINIKEY*)keys.Get(selectkey);
  if(!skey) return false;

  if(skey->GetKey())    key   = (*skey->GetKey());
  if( skey->GetValue()) value = (* skey->GetValue());

  selectkey++;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINISECTION::ResetSelectionKey()
* @brief      ResetSelectionKey
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINISECTION::ResetSelectionKey()
{
  selectkey = 0;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILEINISECTION::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILEINISECTION::Clean()
{
  section.Empty();

  ResetSelectionKey();

  if(keys.IsEmpty()) return;

  keys.DeleteContents();
  keys.DeleteAll();
}


/*--------------------------------------------------------------------------------------------------------------------*/
/*  XFILEREMARK                                                                                                       */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEINIREMARK::XFILEINIREMARK()
* @brief      Constructor
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 11:25:12
* 
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEINIREMARK::XFILEINIREMARK()
{
  Clean();
}
 


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEINIREMARK::~XFILEINIREMARK()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 11:25:20
* 
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEINIREMARK::~XFILEINIREMARK()
{
  Clean();
}




/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEINI_TYPEREMARK XFILEINIREMARK::GetType()
* @brief      GetType
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 11:47:34
* 
* @return     XFILEINI_TYPEREMARK : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEINI_TYPEREMARK XFILEINIREMARK::GetType()
{
  return type;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XFILEINIREMARK::SetType(XFILEINI_TYPEREMARK type)
* @brief      SetType
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 11:47:40
* 
* @param[in]  type : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XFILEINIREMARK::SetType(XFILEINI_TYPEREMARK type)
{
  this->type = type;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD XFILEINIREMARK::GetXPos()
* @brief      GetXPos
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 11:39:17
* 
* @return     XDWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XDWORD XFILEINIREMARK::GetXPos()
{
  return xpos;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XFILEINIREMARK::SetXPos(XDWORD xpos)
* @brief      SetXPos
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 11:40:50
* 
* @param[in]  xpos : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XFILEINIREMARK::SetXPos(XDWORD xpos)
{
  this->xpos = xpos;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int XFILEINIREMARK::GetRelativeYPos()
* @brief      GetRelativeYPos
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 11:55:36
* 
* @return     int : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
int XFILEINIREMARK::GetRelativeYPos()
{
  return relativeypos;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XFILEINIREMARK::SetRelativeYPos(int relativeypos)
* @brief      SetRelativeYPos
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 11:55:47
* 
* @param[in]  relativeypos : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XFILEINIREMARK::SetRelativeYPos(int relativeypos)
{
  this->relativeypos = relativeypos;
}
    



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* XFILEINIREMARK::GetRelativeSection()
* @brief      GetRelativeSection
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       09/12/2021 8:19:44
* 
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XFILEINIREMARK::GetRelativeSection()
{
  return &relativesection;
}




/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* XFILEINIREMARK::GetRelativeKey()
* @brief      GetRelativeKey
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       09/12/2021 8:19:22
* 
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XFILEINIREMARK::GetRelativeKey()
{
  return &relativekey;
}




/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* XFILEINIREMARK::GetRemark()
* @brief      GetRemark
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 11:28:59
* 
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XFILEINIREMARK::GetTextRemark()
{
  return &textremark;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFILEINIREMARK::Compare(XFILEINIREMARK* remake)
* @brief      Compare
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       09/12/2021 19:23:24
* 
* @param[in]  remake : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINIREMARK::Compare(XFILEINIREMARK* remake)
{
  if(!remake)  return false;

  if(remake->GetType() != type)                                     return false;
  if(remake->GetRelativeSection()->Compare(relativesection, true))  return false;
  if(remake->GetRelativeKey()->Compare(relativekey, true))          return false;
  if(remake->GetRelativeYPos() != relativeypos)                     return false;
 
  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFILEINIREMARK::CopyTo(XFILEINIREMARK* remake)
* @brief      CopyTo
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       09/12/2021 21:08:19
* 
* @param[in]  remake : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINIREMARK::CopyTo(XFILEINIREMARK* remake)
{
  if(!remake) return false;

  remake->SetType(type);
  remake->GetRelativeSection()->Set(relativesection);
  remake->GetRelativeKey()->Set(relativekey);
  remake->SetXPos(xpos);
  remake->SetRelativeYPos(relativeypos);
  remake->GetTextRemark()->Set(textremark);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFILEINIREMARK::CopyFrom(XFILEINIREMARK* remake)
* @brief      CopyFrom
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       09/12/2021 21:09:16
* 
* @param[in]  remake : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINIREMARK::CopyFrom(XFILEINIREMARK* remake)
{
  if(!remake) return false;

  type            = remake->GetType();
  relativesection = remake->GetRelativeSection()->Get();
  relativekey     = remake->GetRelativeKey()->Get();
  xpos            = remake->GetXPos();
  relativeypos    = remake->GetRelativeYPos();
  textremark      = remake->GetTextRemark()->Get();

  return true;
}


    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XFILEINIREMARK::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 11:25:27
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XFILEINIREMARK::Clean()
{
  type              = XFILEINI_TYPEREMARK_UNKNOWN;

  xpos              = 0;
  relativeypos      = 0;

  relativesection.Empty();
  relativekey.Empty();

  textremark.Empty();
}




/*--------------------------------------------------------------------------------------------------------------------*/
/*  XFILEINI                                                                                                          */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEINI::XFILEINI()
* @brief      Constructor
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILEINI::XFILEINI(): XFILETXT()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEINI::XFILEINI(XPATH& xpath)
* @brief      Constructor
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  XPATH& :
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILEINI::XFILEINI(XPATH& xpath): XFILETXT()
{
  Clean();

  Open(xpath);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEINI::XFILEINI(XFILE* file)
* @brief      Constructor
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  XFILE* :
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILEINI::XFILEINI(XFILE* file): XFILETXT()
{
  Clean();

  this->file = file;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILEINI::~XFILEINI()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILEINI::~XFILEINI()
{
  Close();

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::Open(XPATH& xpath,bool readonly)
* @brief      Open
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  xpath :
* @param[in]  readonly :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::Open(XPATH& xpath,bool readonly)
{
  if(!XFILETXT::Open(xpath,readonly)) return false;

  Serialization(true);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::Close()
* @brief      Close
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::Close()
{
  if(!IsOpen()) return false;

  bool status[2] = { false , false };

  status[0] = Serialization(false);
  status[1] = XFILETXT::Close();

  DeleteAllRemarks();

  if(status[0] && status[1]) return true;

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEINISECTION* XFILEINI::GetSection(XSTRING& section)
* @brief      GetSection
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 21:12:01
* 
* @param[in]  section : 
* 
* @return     XFILEINISECTION* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEINISECTION* XFILEINI::GetSection(XSTRING& section)
{
  if(sections.IsEmpty()) return NULL;

  for(XDWORD c=0;c<sections.GetSize();c++)
    {
      XFILEINISECTION* osection=(XFILEINISECTION*)sections.Get(c);
      if(osection)
        {
          if(osection->GetName())
            {
              if(!osection->GetName()->Compare(section, true))
                {                  
                  return osection;
                }
            }
        }
    }

  return NULL;
}


    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEINISECTION* XFILEINI::GetSection(XCHAR* section)
* @brief      GetSection
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 21:11:31
* 
* @param[in]  section : 
* 
* @return     XFILEINISECTION* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEINISECTION* XFILEINI::GetSection(XCHAR* section)
{
  XSTRING _section(section);

  return GetSection(_section);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::SelectSection(XSTRING& section)
* @brief      SelectSection
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::SelectSection(XSTRING& section)
{
  if(sections.IsEmpty()) return false;

  for(XDWORD c=0;c<sections.GetSize();c++)
    {
      XFILEINISECTION* osection=(XFILEINISECTION*)sections.Get(c);
      if(osection)
        {
          if(osection->GetName())
            {
              if(!osection->GetName()->Compare(section, true))
                {
                  selectsection = c;
                  osection->ResetSelectionKey();

                  return true;
                }
            }
        }
    }

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::SelectSection(XCHAR* section)
* @brief      SelectSection
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::SelectSection(XCHAR* section)
{
  XSTRING _section(section);

  return SelectSection(_section);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::CreateSection(XSTRING& section)
* @brief      CreateSection
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::CreateSection(XSTRING& section)
{
  if(SelectSection(section)) return false;

  XFILEINISECTION* osection;

  osection = new XFILEINISECTION();
  if(!osection) return false;

  if(!osection->SetName(section)) return false;

  sections.Add(osection);
  selectsection = sections.GetSize()-1;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::CreateSection(XCHAR* section)
* @brief      CreateSection
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::CreateSection(XCHAR* section)
{
  XSTRING _section(section);

  return CreateSection(_section);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::DeleteSection(XSTRING& section)
* @brief      DeleteSection
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::DeleteSection(XSTRING& section)
{
  if(sections.IsEmpty()) return false;

  for(XDWORD c=0;c<sections.GetSize();c++)
    {
      XFILEINISECTION* osection=(XFILEINISECTION*)sections.Get(c);

      if(osection)
        {
          if(osection->GetName())
            {
              if(!osection->GetName()->Compare(section, true))
                {
                  sections.Delete(osection);

                  osection->Clean();
                  delete osection;

                  return true;
                }
            }
        }
    }

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::DeleteSection(XCHAR* section)
* @brief      DeleteSection
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::DeleteSection(XCHAR* section)
{
  XSTRING _section(section);

  return DeleteSection(_section);
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XFILEINIKEY* XFILEINI::GetKey(XSTRING& section,XSTRING& key)
* @brief      GetKey
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 21:30:00
* 
* @param[in]  section : 
* @param[in]  key : 
* 
* @return     XFILEINIKEY* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XFILEINIKEY* XFILEINI::GetKey(XSTRING& section,XSTRING& key)
{
  XFILEINISECTION* _section = GetSection(section);
  if(!_section) return NULL;
    
  XFILEINIKEY* _key = _section->FindKey(key);
  if(!_key) return NULL;  

  return _key;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::DeleteKey(XSTRING& section,XSTRING& key)
* @brief      DeleteKey
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
* @param[in]  key :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::DeleteKey(XSTRING& section,XSTRING& key)
{
  if(!SelectSection(section)) return false;

  XFILEINISECTION* ssection = (XFILEINISECTION*)sections.Get(selectsection);
  if(!ssection) return false;

  return ssection->DeleteKey(key);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::DeleteKey(XCHAR* section,XSTRING& key)
* @brief      DeleteKey
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
* @param[in]  key :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::DeleteKey(XCHAR* section,XSTRING& key)
{
  XSTRING _section(section);

  return DeleteKey(_section,key);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::DeleteKey(XSTRING& section,XCHAR* key)
* @brief      DeleteKey
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
* @param[in]  key :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::DeleteKey(XSTRING& section,XCHAR* key)
{
  XSTRING _key;

  _key = key;

  return DeleteKey(section,_key);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::DeleteKey(XCHAR* section,XCHAR* key)
* @brief      DeleteKey
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
* @param[in]  key :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::DeleteKey(XCHAR* section,XCHAR* key)
{
  XSTRING _section(section);
  XSTRING _key(key);

  return DeleteKey(_section,_key);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::ReadValue(XSTRING& section,XSTRING& key,XSTRING& value)
* @brief      ReadValue
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::ReadValue(XSTRING& section,XSTRING& key,XSTRING& value)
{
  value.Empty();

  if(!SelectSection(section)) return false;

  XFILEINISECTION* ssection = (XFILEINISECTION*)sections.Get(selectsection);
  if(!ssection) return false;

  XFILEINIKEY* okey=ssection->FindKey(key);
  if(!okey) return false;

  if(okey->GetValue()) value = (*okey->GetValue());

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::ReadValue(XCHAR* section,XSTRING& key,XSTRING& value)
* @brief      ReadValue
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::ReadValue(XCHAR* section,XSTRING& key,XSTRING& value)
{
  XSTRING _section(section);

  return ReadValue(_section,key,value);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::ReadValue(XSTRING& section,XCHAR* key,XSTRING& value)
* @brief      ReadValue
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::ReadValue(XSTRING& section,XCHAR* key,XSTRING& value)
{
  XSTRING _key(key);

  return ReadValue(section,_key,value);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::ReadValue(XCHAR* section,XCHAR* key,XSTRING& value)
* @brief      ReadValue
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::ReadValue(XCHAR* section,XCHAR* key,XSTRING& value)
{
  XSTRING _section(section);
  XSTRING _key(key);

  return ReadValue(_section,_key,value);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::ReadValue(XSTRING& key,XSTRING& value)
* @brief      ReadValue
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::ReadValue(XSTRING& key,XSTRING& value)
{
  value.Empty();

  if(sections.IsEmpty()) return false;

  selectsection=0;

  XFILEINISECTION* ssection = (XFILEINISECTION*)sections.Get(selectsection);
  if(!ssection)            return false;
  if(!ssection->GetName()) return false;

  return ReadValue((*ssection->GetName()),key,value);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::ReadValue(XCHAR* key,XSTRING& value)
* @brief      ReadValue
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::ReadValue(XCHAR* key,XSTRING& value)
{
  XSTRING _key(key);

  return ReadValue(_key,value);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::WriteValue(XSTRING& section,XSTRING& key,XSTRING& value)
* @brief      WriteValue
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::WriteValue(XSTRING& section,XSTRING& key,XSTRING& value)
{
  if(!SelectSection(section))
    {
      if(!CreateSection(section)) return false;
      if(!SelectSection(section)) return false;
    }

  XFILEINISECTION* ssection = (XFILEINISECTION*)sections.Get(selectsection);
  if(!ssection) return false;

  return ssection->SetKey(key,value);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::WriteValue(XCHAR* section,XSTRING& key,XSTRING& value)
* @brief      WriteValue
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::WriteValue(XCHAR* section,XSTRING& key,XSTRING& value)
{
  XSTRING _section(section);

  return WriteValue(_section,key,value);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::WriteValue(XSTRING& section,XCHAR* key,XSTRING& value)
* @brief      WriteValue
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::WriteValue(XSTRING& section,XCHAR* key,XSTRING& value)
{
  XSTRING _key(key);

  return WriteValue(section,_key,value);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::WriteValue(XCHAR* section,XCHAR* key,XSTRING& value)
* @brief      WriteValue
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::WriteValue(XCHAR* section,XCHAR* key,XSTRING& value)
{
  XSTRING _section(section);
  XSTRING _key(key);

  return WriteValue(_section,_key,value);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::WriteValue(XSTRING& key,XSTRING& value)
* @brief      WriteValue
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::WriteValue(XSTRING& key,XSTRING& value)
{
  if(sections.IsEmpty()) return false;

  selectsection=0;

  XFILEINISECTION* ssection = (XFILEINISECTION*)sections.Get(selectsection);
  if(!ssection)            return false;
  if(!ssection->GetName()) return false;

  return WriteValue((*ssection->GetName()),key,value);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::WriteValue(XCHAR* key,XSTRING& value)
* @brief      WriteValue
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::WriteValue(XCHAR* key,XSTRING& value)
{
  XSTRING _key(key);

  return ReadValue(_key,value);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::GetNextKey(XSTRING& key,XSTRING& value)
* @brief      GetNextKey
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  key :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::GetNextKey(XSTRING& key,XSTRING& value)
{
  if(sections.IsEmpty()) return false;

  selectsection=0;

  XFILEINISECTION* ssection = (XFILEINISECTION*)sections.Get(selectsection);
  if(!ssection) return false;

  return ssection->GetNextKey(key,value);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::ReadKeyMap(XSTRING& section,XFILEINIKEYMAP* keymap,int nkeymap)
* @brief      ReadKeyMap
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
* @param[in]  keymap :
* @param[in]  nkeymap :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::ReadKeyMap(XSTRING& section,XFILEINIKEYMAP* keymap,int nkeymap)
{
  if(!nkeymap)                return false;
  if(!SelectSection(section)) return false;

  XSTRING value;

  for(int c=0;c<nkeymap;c++)
    {
      if(ReadValue(keymap[c].key,value)) keymap[c].value = value;
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::ReadKeyMap(XCHAR* section,XFILEINIKEYMAP* keymap,int nkeymap)
* @brief      ReadKeyMap
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  section :
* @param[in]  keymap :
* @param[in]  nkeymap :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::ReadKeyMap(XCHAR* section,XFILEINIKEYMAP* keymap,int nkeymap)
{
  XSTRING _section(section);

  return ReadKeyMap(_section,keymap,nkeymap);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::ConvertFromLines()
* @brief      ConvertFromLines
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::ConvertFromLines()
{
  DeleteAllSections();

  XSTRING   section;
  XSTRING   key;  
  XSTRING   value;
  
  for(int c=0;c<GetNLines();c++)
    {
      XSTRING* line = GetLine(c);
      if(line)
        {          
          XFILEINIREMARK* remark =  new XFILEINIREMARK();   
          if(remark)
            {
              if(!line->HaveCharacters())
                {
                  section.Empty();                  
                  key.Empty(); 
                  value.Empty();
                }
               else
                {
                  if(Line_GetRemark((*line), (*remark)))
                    {
                      int a=0;
                      a++;
                    }
              
                  XSTRING _section;
                  if(Line_IsSection((*line), _section))
                    {
                      section = _section;

                      if(!remark->GetTextRemark()->IsEmpty()) remark->SetType(XFILEINI_TYPEREMARK_IN_SECTION);
                      
                      CreateSection(section);

                      int index = 1;  

                      for(XDWORD c=0; c<remarks.GetSize(); c++)
                        {
                          XFILEINIREMARK* tmp_remark = remarks.Get(c);
                          if(tmp_remark)
                            {
                              if(tmp_remark->GetType() == XFILEINI_TYPEREMARK_ALL_LINE)
                                {
                                  if(tmp_remark->GetRelativeSection()->IsEmpty())  
                                    {
                                      tmp_remark->GetRelativeSection()->Set(section); 
                                      tmp_remark->GetRelativeKey()->Set(key);
                                      tmp_remark->SetRelativeYPos(index);
                                      index++;
                                    }
                                } 
                            }                      
                        }
                    }
                   else
                    {            
                      XSTRING _key;
  
                      if(Line_IsKey((*line), _key, value))
                        {
                          key = _key;

                          if(!remark->GetTextRemark()->IsEmpty())
                            {
                              int index_found = value.Find(remark->GetTextRemark()->Get(), true, 0);
                              if(index_found != XSTRING_NOTFOUND) 
                                {
                                  value.DeleteCharactersToEnd(index_found-1);     
                                }
                            }
                          
                          if(!remark->GetTextRemark()->IsEmpty()) remark->SetType(XFILEINI_TYPEREMARK_IN_KEY);

                          int index = 1; 
                          for(XDWORD c=0; c<remarks.GetSize(); c++)
                            {
                              XFILEINIREMARK* tmp_remark = remarks.Get(c);
                              if(tmp_remark)
                                {
                                  if(tmp_remark->GetType() == XFILEINI_TYPEREMARK_ALL_LINE)
                                    {
                                      if(tmp_remark->GetRelativeSection()->IsEmpty())  
                                        {
                                          tmp_remark->GetRelativeSection()->Set(section); 
                                          tmp_remark->GetRelativeKey()->Set(key);
                                          tmp_remark->SetRelativeYPos(index);
                                          index++;
                                        }
                                    } 
                                }                      
                            }

                          WriteValue(section, key, value);
                        }
                    }
                }

              switch(remark->GetType())
                {
                  case XFILEINI_TYPEREMARK_UNKNOWN     : delete remark;   
                                                         break; 

                  case XFILEINI_TYPEREMARK_ALL_LINE    : remarks.Add(remark);
                                                         break; 

                  case XFILEINI_TYPEREMARK_IN_SECTION  : remark->GetRelativeSection()->Set(section);
                                                         remark->GetRelativeKey()->Empty();
                                                         remarks.Add(remark);
                                                         break; 

                  case XFILEINI_TYPEREMARK_IN_KEY      : remark->GetRelativeSection()->Set(section);
                                                         remark->GetRelativeKey()->Set(key);
                                                         remarks.Add(remark);
                                                         break; 
                }             
            }
        }
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::ConvertToLines()
* @brief      ConvertToLines
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::ConvertToLines()
{
  if(sections.IsEmpty()) return false;

  for(XDWORD c=0;c<sections.GetSize();c++)
    {
      XFILEINISECTION* section = (XFILEINISECTION*)sections.Get(c);
      if(section)
        { 
          XSTRING line(__L(" "));
          XSTRING text_remark;
          XSTRING key;
          XSTRING value;

          AddLine(line.Get());
         
          for(XDWORD d=0; d<remarks.GetSize(); d++)
            {
              XFILEINIREMARK* remark = remarks.Get(d);
              if(remark)
                { 
                  line.Empty();
  
                  if(remark->GetType() == XFILEINI_TYPEREMARK_ALL_LINE)
                    {
                      if((!remark->GetRelativeSection()->Compare(section->GetName()->Get(), true))  && (remark->GetRelativeKey()->IsEmpty()))
                        {
                          for(XDWORD e=0; e<remark->GetXPos(); e++)
                            {
                              line.Add(__L(" "));
                            }

                          line.AddFormat(__L(";%s"), remark->GetTextRemark()->Get());
                          AddLine(line.Get());
                        }    
                    }                     
                }  
            }            
       
          line.Format(__L("[%s]"), section->GetName()->Get());

          for(XDWORD d=0; d<remarks.GetSize(); d++)
            {
              XFILEINIREMARK* remark = remarks.Get(d);
              if(remark)
                { 
                  if(remark->GetType() == XFILEINI_TYPEREMARK_IN_SECTION)
                    {
                      if(!remark->GetRelativeSection()->Compare(section->GetName()->Get(), true))
                        {
                          XDWORD inisizeline = line.GetSize();

                          for(XDWORD e=inisizeline; e<remark->GetXPos(); e++)
                            {
                              line.Add(__L(" "));
                            }

                          line.AddFormat(__L(";%s"), remark->GetTextRemark()->Get());

                          break;
                        }    
                    }                     
                }  
            }           
          
          AddLine(line.Get());

          section->ResetSelectionKey();

          while(section->GetNextKey(key, value))
            {
              for(XDWORD d=0; d<remarks.GetSize(); d++)
                {
                  XFILEINIREMARK* remark = remarks.Get(d);
                  if(remark)
                    { 
                      line.Empty();

                      if(remark->GetType() == XFILEINI_TYPEREMARK_ALL_LINE)
                        {
                          if((!remark->GetRelativeSection()->Compare(section->GetName()->Get(), true)) && (!remark->GetRelativeKey()->Compare(key.Get(), true)))
                            {
                              for(XDWORD e=0; e<remark->GetXPos(); e++)
                                {
                                  line.Add(__L(" "));
                                }

                              line.AddFormat(__L(";%s"), remark->GetTextRemark()->Get());  
                              AddLine(line.Get());                     
                            }    
                        }                     
                    }  
                }           

              line.Format(__L("%s=%s"), key.Get(), value.Get());

              for(XDWORD d=0; d<remarks.GetSize(); d++)
                {
                  XFILEINIREMARK* remark = remarks.Get(d);
                  if(remark)
                    { 
                      if(remark->GetType() == XFILEINI_TYPEREMARK_IN_KEY)
                        {
                          if((!remark->GetRelativeSection()->Compare(section->GetName()->Get(), true)) && (!remark->GetRelativeKey()->Compare(key.Get(), true))) 
                            {
                              XDWORD inisizeline = line.GetSize();

                              for(XDWORD e=inisizeline; e<remark->GetXPos(); e++)
                                {
                                  line.Add(__L(" "));
                                }

                              line.AddFormat(__L(";%s"), remark->GetTextRemark()->Get());

                              break;
                            }    
                        }                     
                    }  
                }           

              AddLine(line.Get());
            }
        }
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::Serialization(bool read)
* @brief      Serialization
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  read :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::Serialization(bool read)
{
  bool status;

  file->SetPosition(XFILE_SEEKINI);

  DeleteAllLines();

  if(read)
    {
      ReadAllFile();
      status = ConvertFromLines();
    }
   else
    {
      status = ConvertToLines();
      if(status)
        {
          WriteAllFile();

          XQWORD pos = 0;
          file->GetPosition(pos);

          if(pos) file->SetSize(pos);
        }
    }

  DeleteAllLines();

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILEINI::DeleteAllSections()
* @brief      DeleteAllSections
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::DeleteAllSections()
{
  selectsection = 0;

  if(sections.IsEmpty()) return false;

  sections.DeleteContents();
  sections.DeleteAll();

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XFILEINIREMARK*>* XFILEINI::GetRemarks()
* @brief      GetRemarks
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       09/12/2021 14:41:43
* 
* @return     XVECTOR<XFILEINIREMARK*>* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XFILEINIREMARK*>* XFILEINI::GetRemarks()
{
  return &remarks;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFILEINI::DeleteAllRemarks()
* @brief      DeleteAllRemarks
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 12:41:49
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::DeleteAllRemarks()
{
  if(remarks.IsEmpty()) return false;

  remarks.DeleteContents();
  remarks.DeleteAll();

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFILEINI::Line_IsSection(XSTRING& line, XSTRING& section)
* @brief      Line_IsSection
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 22:05:37
* 
* @param[in]  line : 
* @param[in]  section : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::Line_IsSection(XSTRING& line, XSTRING& section)
{
  section.Empty();

  if(line.IsEmpty()) return false;

  line.Copy(__L("["),__L("]"),true,0,section);  
  if(section.IsEmpty()) return false;
 
  section.ToUpperCase();
  section.DeleteCharacter(__C(' '), XSTRINGCONTEXT_FROM_FIRST);
  section.DeleteCharacter(__C(' '), XSTRINGCONTEXT_TO_END);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFILEINI::Line_IsKey(XSTRING& line, XSTRING& key, XSTRING& value)
* @brief      Line_IsKey
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       07/12/2021 22:05:19
* 
* @param[in]  line : 
* @param[in]  key : 
* @param[in]  value : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::Line_IsKey(XSTRING& line, XSTRING& key, XSTRING& value)
{
  key.Empty();
  value.Empty();

  if(line.IsEmpty()) return false;

  if((line.Find(__L("["),true) != XSTRING_NOTFOUND) &&
     (line.Find(__L("]"),true) != XSTRING_NOTFOUND))  return false;
 
  int indexvalue = line.Find(__L("="),true);
  if(indexvalue == XSTRING_NOTFOUND) return false;

  line.Copy(0,indexvalue,key);
  line.Copy(indexvalue+1,line.GetSize(),value);

  if(key.IsEmpty()) return false;      

  //key.ToUpperCase();
  key.DeleteCharacter(__C(' '),XSTRINGCONTEXT_FROM_FIRST);
  key.DeleteCharacter(__C(' '),XSTRINGCONTEXT_TO_END);

  value.DeleteCharacter(__C(' '),XSTRINGCONTEXT_FROM_FIRST);
  value.DeleteCharacter(__C(' '),XSTRINGCONTEXT_TO_END);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XFILEINI::Line_GetRemark(XSTRING& line, XFILEINIREMARK& remark)
* @brief      Line_GetRemark
* @ingroup    UTILS
* 
* @author     Abraham J. Velez 
* @date       09/12/2021 13:03:24
* 
* @param[in]  line : 
* @param[in]  remark : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XFILEINI::Line_GetRemark(XSTRING& line, XFILEINIREMARK& remark)
{
  if(line.IsEmpty()) return false;

  int index = line.Find(__L(";"), true);
  if(index == XSTRING_NOTFOUND) return false;
 
  remark.SetXPos(index);
  line.Copy(index+1, (*remark.GetTextRemark()));

  XSTRING first;

  if(index) line.Copy(0, index, first);
    
  
  if(!first.HaveCharacters())
    {      
      remark.SetType(XFILEINI_TYPEREMARK_ALL_LINE);     
      remark.SetRelativeYPos(-1);
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILEINI::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    UTILS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILEINI::Clean()
{
  DeleteAllSections();

  selectsection = 0;

  DeleteAllRemarks();
}



