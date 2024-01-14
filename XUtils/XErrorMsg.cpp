/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XErrorMsg.cpp
* 
* @class      XERRORMSG
* @brief      eXtended Utils Error Message class
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

#include "XErrorMsg.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


#pragma region CLASS_XERRORMSG


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XERRORMSG::XERRORMSG()
* @brief      Constructor
* @ingroup    XUTILS
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XERRORMSG::XERRORMSG()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XERRORMSG::XERRORMSG(int code, XCHAR* title, XCHAR* msg)
* @brief      Constructor
* @ingroup    XUTILS
*
* @param[in]  int :
* @param[in]   XCHAR* title :
* @param[in]   XCHAR* msg :
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XERRORMSG::XERRORMSG(int code, XCHAR* title, XCHAR* msg)
{
  Clean();

  Set(code, title, msg);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XERRORMSG::~XERRORMSG()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XERRORMSG::~XERRORMSG()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XERRORMSG* XERRORMSG::Get()
* @brief      Get
* @ingroup    XUTILS
*
* @return     XERRORMSG* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XERRORMSG* XERRORMSG::Get()
{
  return this;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORMSG::Set(int code,XCHAR* title,XCHAR* msg)
* @brief      Set
* @ingroup    XUTILS
*
* @param[in]  code :
* @param[in]  title :
* @param[in]  msg :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORMSG::Set(int code, XCHAR* title, XCHAR* msg)
{
  if(!SetCode(code))   return false;
  if(!SetTitle(title)) return false;
  if(!SetMsg(msg))     return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORMSG::Set(int code, int level, int image, int sound, XCHAR* title, XCHAR* msg)
* @brief      Set
* @ingroup    XUTILS
*
* @param[in]  code :
* @param[in]  level :
* @param[in]  image :
* @param[in]  sound :
* @param[in]  title :
* @param[in]  msg :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORMSG::Set(int code, int level, int image, int sound, XCHAR* title, XCHAR* msg)
{
  if(!SetCode(code))   return false;
  if(!SetLevel(level)) return false;
  if(!SetImage(image)) return false;
  if(!SetSound(sound)) return false;
  if(!SetTitle(title)) return false;
  if(!SetMsg(msg))     return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XERRORMSG::GetCode()
* @brief      GetCode
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XERRORMSG::GetCode()
{
  return code;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORMSG::SetCode(int code)
* @brief      SetCode
* @ingroup    XUTILS
*
* @param[in]  code :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORMSG::SetCode(int code)
{
  this->code=code;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XERRORMSG::GetLevel()
* @brief      GetLevel
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XERRORMSG::GetLevel()
{
  return level;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORMSG::SetLevel(int level)
* @brief      SetLevel
* @ingroup    XUTILS
*
* @param[in]  level :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORMSG::SetLevel(int level)
{
  this->level=level;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XERRORMSG::GetImage()
* @brief      GetImage
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XERRORMSG::GetImage()
{
  return image;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORMSG::SetImage(int image)
* @brief      SetImage
* @ingroup    XUTILS
*
* @param[in]  image :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORMSG::SetImage(int image)
{
  this->image=image;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XERRORMSG::GetSound()
* @brief      GetSound
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XERRORMSG::GetSound()
{
  return sound;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORMSG::SetSound(int sound)
* @brief      SetSound
* @ingroup    XUTILS
*
* @param[in]  sound :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORMSG::SetSound(int sound)
{
  this->sound=sound;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XERRORMSG::GetTitle()
* @brief      GetTitle
* @ingroup    XUTILS
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* XERRORMSG::GetTitle()
{
  return &title;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORMSG::SetTitle(XCHAR* title)
* @brief      SetTitle
* @ingroup    XUTILS
*
* @param[in]  title :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORMSG::SetTitle(XCHAR* title)
{
  if(!title) return false;

  this->title = title;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORMSG::SetTitle(XSTRING& title)
* @brief      SetTitle
* @ingroup    XUTILS
*
* @param[in]  title :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORMSG::SetTitle(XSTRING& title)
{
  return SetTitle(title.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XERRORMSG::GetMsg()
* @brief      GetMsg
* @ingroup    XUTILS
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* XERRORMSG::GetMsg()
{
  return &msg;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORMSG::SetMsg(XCHAR* msg)
* @brief      SetMsg
* @ingroup    XUTILS
*
* @param[in]  msg :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORMSG::SetMsg(XCHAR* msg)
{
  if(!msg) return false;

  this->msg = msg;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORMSG::SetMsg(XSTRING& msg)
* @brief      SetMsg
* @ingroup    XUTILS
*
* @param[in]  msg :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORMSG::SetMsg(XSTRING& msg)
{
  return SetMsg(msg.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORMSG::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORMSG::Clean()
{
  code  = 0;
  level = 0;
  image = 0;
  sound = 0;

  title.Empty();
  msg.Empty();

  return true;
}


#pragma endregion


#pragma region CLASS_XERRORSMSG


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XERRORSMSG::XERRORSMSG()
* @brief      Constructor
* @ingroup    XUTILS
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XERRORSMSG::XERRORSMSG()
{

}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XERRORSMSG::XERRORSMSG(XCHAR* titleapp, XCHAR* version)
* @brief      Constructor
* @ingroup    XUTILS
*
* @param[in]  XCHAR* :
* @param[in]   XCHAR* version :
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XERRORSMSG::XERRORSMSG(XCHAR* titleapp, XCHAR* version)
{
  Clean();

  SetTitleApplication(titleapp);
  SetVersion(version);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XERRORSMSG::~XERRORSMSG()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XERRORSMSG::~XERRORSMSG()
{
  EraseAllMsg();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XERRORSMSG::GetTitleApplication()
* @brief      GetTitleApplication
* @ingroup    XUTILS
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* XERRORSMSG::GetTitleApplication()
{
  return &titleapp;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XERRORSMSG::GetVersion()
* @brief      GetVersion
* @ingroup    XUTILS
*
* @return     XSTRING* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* XERRORSMSG::GetVersion()
{
  return &version;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORSMSG::SetTitleApplication(XCHAR* titleapp)
* @brief      SetTitleApplication
* @ingroup    XUTILS
*
* @param[in]  titleapp :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORSMSG::SetTitleApplication(XCHAR* titleapp)
{
  if(!titleapp) return false;

  this->titleapp = titleapp;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORSMSG::SetTitleApplication(XSTRING& titleapp)
* @brief      SetTitleApplication
* @ingroup    XUTILS
*
* @param[in]  titleapp :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORSMSG::SetTitleApplication(XSTRING& titleapp)
{
  return SetTitleApplication(titleapp.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORSMSG::SetVersion(XCHAR* version)
* @brief      SetVersion
* @ingroup    XUTILS
*
* @param[in]  version :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORSMSG::SetVersion(XCHAR* version)
{
  if(!version) return false;

  this->version = version;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORSMSG::SetVersion(XSTRING& version)
* @brief      SetVersion
* @ingroup    XUTILS
*
* @param[in]  version :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORSMSG::SetVersion(XSTRING& version)
{
  return SetVersion(version.Get());
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XERRORMSG* XERRORSMSG::FindMsg(int code)
* @brief      FindMsg
* @ingroup    XUTILS
*
* @param[in]  code :
*
* @return     XERRORMSG* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XERRORMSG* XERRORSMSG::FindMsg(int code)
{
  for(XDWORD c=0;c<errors.GetSize();c++)
    {
      XERRORMSG* error = errors.Get(c);
      if(error)
        {
          if(error->GetCode()==code) return error;
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORSMSG::AddMsg(int code, int level, int image, int sound, XCHAR* title, XCHAR* msg)
* @brief      AddMsg
* @ingroup    XUTILS
*
* @param[in]  code :
* @param[in]  level :
* @param[in]  image :
* @param[in]  sound :
* @param[in]  title :
* @param[in]  msg :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORSMSG::AddMsg(int code, int level, int image, int sound, XCHAR* title, XCHAR* msg)
{
  XERRORMSG* error;

  if(FindMsg(code)) return false;

  error = new XERRORMSG(code,title,msg);
  if(!error) return false;

  error->SetLevel(level);
  error->SetImage(image);
  error->SetSound(sound);

  errors.Add(error);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORSMSG::EraseMsg(int code)
* @brief      EraseMsg
* @ingroup    XUTILS
*
* @param[in]  code :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORSMSG::EraseMsg(int code)
{
  if(errors.IsEmpty()) return false;

  for(XDWORD c=0; c<errors.GetSize(); c++)
    {
      XERRORMSG* error = errors.Get(c);

      if(!error)
        {
          if(error->GetCode()==code)
            {
              errors.Delete(error);

              delete error;

              return true;
            }
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORSMSG::EraseAllMsg()
* @brief      EraseAllMsg
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORSMSG::EraseAllMsg()
{
  if(errors.IsEmpty()) return false;

  errors.DeleteContents();

  errors.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XERRORMSG_RETURN XERRORSMSG::ShowMsg(int code,bool statusline,XBYTE returns,XCHAR* addstr,bool sound)
* @brief      ShowMsg
* @ingroup    XUTILS
*
* @param[in]  code :
* @param[in]  statusline :
* @param[in]  returns :
* @param[in]  addstr :
* @param[in]  sound :
*
* @return     XERRORMSG_RETURN :
*
* --------------------------------------------------------------------------------------------------------------------*/
XERRORMSG_RETURN XERRORSMSG::ShowMsg(int code,bool statusline,XBYTE returns,XCHAR* addstr,bool sound)
{
  return XERRORMSG_RETURN_NONE;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XERRORSMSG::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XERRORSMSG::Clean()
{
  titleapp.Empty();
  version.Empty();

  return true;
}


#pragma endregion

