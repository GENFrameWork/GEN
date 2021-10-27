/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XWindowsRegistryManager.cpp
*
* @class      XWINDOWSREGISTRYMANAGER
* @brief      eXtended WINDOWS Registry Manager class
* @ingroup    PLATFORM_WINDOWS
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

#ifdef WINDOWS

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <Windows.h>

#include "XString.h"
#include "XPath.h"
#include "XBuffer.h"

#include "XWINDOWSRegistryManager.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWINDOWSREGISTRYKEY::XWINDOWSREGISTRYKEY()
* @brief      Constructor
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XWINDOWSREGISTRYKEY::XWINDOWSREGISTRYKEY()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWINDOWSREGISTRYKEY::~XWINDOWSREGISTRYKEY()
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
XWINDOWSREGISTRYKEY::~XWINDOWSREGISTRYKEY()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         HKEY XWINDOWSREGISTRYKEY::GetHandle()
* @brief      GetHandleKey
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     HKEY :
*
*---------------------------------------------------------------------------------------------------------------------*/
HKEY XWINDOWSREGISTRYKEY::GetHandle()
{
  return handlekey;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSREGISTRYKEY::SetHandle(HKEY handlekey)
* @brief      SetHandle
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  handlekey :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSREGISTRYKEY::SetHandle(HKEY handlekey)
{
  this->handlekey = handlekey;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD XWINDOWSREGISTRYKEY::GetType()
* @brief      GetType
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     XDWORD :
*
*---------------------------------------------------------------------------------------------------------------------*/
XDWORD XWINDOWSREGISTRYKEY::GetType()
{
  return type;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSREGISTRYKEY::SetType(XDWORD type)
* @brief      SetType
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  type :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSREGISTRYKEY::SetType(XDWORD type)
{
  this->type = type;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYKEY::ReadValue(XCHAR* name, XVARIANT& data)
* @brief      ReadValue
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  name :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYKEY::ReadValue(XCHAR* name, XVARIANT& data)
{
  if(handlekey == NULL) return false;

  XBUFFER databuffer;
  XDWORD  size = _MAXBUFFER;
  LSTATUS status;

  databuffer.Resize(size);

  status = RegQueryValueEx(handlekey, name, 0, (DWORD*)&type, (LPBYTE)databuffer.Get(), (DWORD*)&size);
  if(status != ERROR_SUCCESS) return false;

  SetType(type);

  switch(type)
    {
      case REG_NONE                         : // No value type
                                              return false;

      case REG_SZ                           : // Unicode nul terminated string
                                              data = (XCHAR*)databuffer.Get();
                                              break;

      case REG_EXPAND_SZ                    : // Unicode nul terminated string (with environment variable references)
                                              break;

      case REG_BINARY                       : // Free form binary
                                              data.Set(XVARIANT_TYPE_SERIALIZABLE, databuffer.Get(), size);
                                              break;

      case REG_DWORD                        : { // 32-bit number
                                                XDWORD _data = 0;
                                                databuffer.Get(_data);
                                                data = _data;
                                              }
                                              break;

    //case REG_DWORD_LITTLE_ENDIAN          : // 32-bit number (same as REG_DWORD)
                                              //break;

      case REG_DWORD_BIG_ENDIAN             : { // 32-bit number
                                                XDWORD _data = 0;
                                                databuffer.Get(_data);
                                                SWAPDWORD(_data);
                                                data = _data;
                                              }
                                              break;

      case REG_LINK                         : // Symbolic Link (unicode)
                                              break;

      case REG_MULTI_SZ                     : // Multiple Unicode strings
                                              data.Set(XVARIANT_TYPE_SERIALIZABLE, databuffer.Get(), size);
                                              break;

      case REG_RESOURCE_LIST                : // Resource list in the resource map
                                              break;

      case REG_FULL_RESOURCE_DESCRIPTOR     : // Resource list in the hardware description
                                              break;

      case REG_RESOURCE_REQUIREMENTS_LIST   : break;

      case REG_QWORD                        : { // 64-bit number
                                                XQWORD _data = 0;
                                                databuffer.Get(_data);
                                                data = _data;
                                              }
                                              break;

  //  case REG_QWORD_LITTLE_ENDIAN          : // 64-bit number (same as REG_QWORD)

                                    default : return false;

    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYKEY::ReadValue(XSTRING& name, XVARIANT& data)
* @brief      ReadValue
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  name :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYKEY::ReadValue(XSTRING& name, XVARIANT& data)
{
  return ReadValue(name.Get(), data);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYKEY::WriteValue(XCHAR* name, XDWORD type, XVARIANT& data)
* @brief      WriteValue
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  name :
* @param[in]  type :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYKEY::WriteValue(XCHAR* name, XDWORD type, XVARIANT& data)
{
  if(handlekey == NULL) return false;

  LSTATUS status;

  status = RegSetValueEx(handlekey, name, 0, (DWORD)type, (LPBYTE)data.GetData(), (DWORD)data.GetSize());
  if(status != ERROR_SUCCESS) return false;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYKEY::WriteValue(XCHAR* name, XVARIANT& data)
* @brief      WriteValue
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  name :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYKEY::WriteValue(XCHAR* name, XVARIANT& data)
{
  if(handlekey == NULL) return false;

  LSTATUS status;

  if(type == REG_NONE)
    {
      switch(data.GetType())
        {
          case XVARIANT_TYPE_NULL            :
          case XVARIANT_TYPE_SERIALIZABLE    :

          case XVARIANT_TYPE_DOUBLEINTEGER   :
          case XVARIANT_TYPE_FLOAT           :

          case XVARIANT_TYPE_DATE            :
          case XVARIANT_TYPE_TIME            :
          case XVARIANT_TYPE_DATETIME        : break;

          case XVARIANT_TYPE_INTEGER         : type = REG_DWORD;
                                               break;

          case XVARIANT_TYPE_DOUBLE          : type = REG_QWORD;
                                               break;

          case XVARIANT_TYPE_STRING          :
          case XVARIANT_TYPE_CHAR            :
          case XVARIANT_TYPE_XCHAR           : type = REG_SZ;
                                               break;
       }
    }

  if(data.GetType() == XVARIANT_TYPE_STRING)
    {
      XSTRING* string =  (XSTRING*)data.GetData();
      status = RegSetValueEx(handlekey, name, 0, (DWORD)type, (LPBYTE)(string->Get()), (DWORD)string->GetSize()*sizeof(XCHAR));
    }
   else
    {
      status = RegSetValueEx(handlekey, name, 0, (DWORD)type, (LPBYTE)data.GetData(), (DWORD)data.GetSize());
    }

  if(status != ERROR_SUCCESS) return false;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYKEY::WriteValue(XSTRING& name, XVARIANT& data)
* @brief      WriteValue
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  name :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYKEY::WriteValue(XSTRING& name, XVARIANT& data)
{
  return WriteValue(name.Get(), data);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYKEY::WriteValue(XSTRING& name, XDWORD type, XVARIANT& data);
* @brief      WriteValue
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  name :
* @param[in]  type :
* @param[in]  data) :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYKEY::WriteValue(XSTRING& name, XDWORD type, XVARIANT& data)
{
  return WriteValue(name.Get(), type, data);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYKEY::EnumValues(XVECTOR<XSTRING*>* valuelist)
* @brief      EnumValues
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  valuelist :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYKEY::EnumValues(XVECTOR<XSTRING*>* valuelist)
{
  if(GetHandle() == NULL) return false;

  LSTATUS status;
  XSTRING namevalue;
  DWORD   size;
  int     index = 0;

  do{ size  = _MAXPATH;

      namevalue.AdjustSize(size);
      status = RegEnumValue(GetHandle(), index, namevalue.Get(), &size, 0, NULL, NULL, NULL);
      namevalue.AdjustSize();

      if(status == ERROR_SUCCESS)
        {
          XSTRING* _namevalue = new XSTRING();
          if(_namevalue)
            {
              (*_namevalue) = namevalue;
              valuelist->Add(_namevalue);
            }
        }

      index++;

    } while(status != ERROR_NO_MORE_ITEMS);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYKEY::DeleteValue(XCHAR* name)
* @brief      DeleteValue
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  name :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYKEY::DeleteValue(XCHAR* name)
{
  if(handlekey == NULL) return false;

  LSTATUS status = RegDeleteValueW(handlekey, name);
  if(status != ERROR_SUCCESS) return false;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYKEY::DeleteValue(XSTRING& name)
* @brief      DeleteValue
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  name :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYKEY::DeleteValue(XSTRING& name)
{
  return DeleteValue(name.Get());
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSREGISTRYKEY::Clear()
* @brief      Clear
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSREGISTRYKEY::Clear()
{
  handlekey = NULL;
  type      = REG_NONE;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSREGISTRYKEY::Clean()
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
void XWINDOWSREGISTRYKEY::Clean()
{
  Clear();
}




/*---------------------------------------------------------------------------------------------------------------------*/
/*  XWINDOWSREGISTRYMANAGER                                                                                            */
/*---------------------------------------------------------------------------------------------------------------------*/





/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWINDOWSREGISTRYMANAGER::XWINDOWSREGISTRYMANAGER()
* @brief      Constructor
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XWINDOWSREGISTRYMANAGER::XWINDOWSREGISTRYMANAGER()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWINDOWSREGISTRYMANAGER::~XWINDOWSREGISTRYMANAGER()
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
XWINDOWSREGISTRYMANAGER::~XWINDOWSREGISTRYMANAGER()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYMANAGER::CreateKey(HKEY handlekey, XCHAR* subkeystring , XWINDOWSREGISTRYKEY& registrykey)
* @brief      CreateKey
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  handlekey :
* @param[in]  subkeystring :
* @param[in]  registrykey :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
/*
bool XWINDOWSREGISTRYMANAGER::CreateKey(HKEY handlekey, XCHAR* subkeystring, XWINDOWSREGISTRYKEY& registrykey)
{
  if(registrykey.GetHandle() != NULL) return false;

  HKEY  oper_handlekey;
  DWORD disposition;

  XDWORD error = RegCreateKeyEx(handlekey, subkeystring, 0, NULL, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, NULL, &oper_handlekey, &disposition);
  if(error != ERROR_SUCCESS)  return false;

  registrykey.SetHandle(oper_handlekey);

  return true;
}
*/

//-------------------------------------------------------------------------------------------------------------------

bool XWINDOWSREGISTRYMANAGER::CreateKey(HKEY handlekey, XCHAR* subkeystring, XWINDOWSREGISTRYKEY& registrykey)
{
  if(registrykey.GetHandle() != NULL) return false;

  HKEY              oper_handlekey;
  DWORD             disposition;
  XPATH             subkeystr1;
  XSTRING           subkeystr2;
  XVECTOR<XSTRING*> partkey;
  XSTRING           subkeystr3;
  XDWORD            error;

  subkeystr1  = subkeystring;
  subkeystr1.Slash_Normalize(true);
  subkeystr2 = subkeystr1;

  subkeystr2.Split(__C('\\'), partkey);

  for(XDWORD c=0; c<partkey.GetSize(); c++)
     {
        subkeystr3 += partkey.Get(c)->Get();

        error = RegCreateKeyEx(handlekey, subkeystr3.Get(), 0, NULL, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, NULL, &oper_handlekey, &disposition);
        if(error != ERROR_SUCCESS)
          {
            RegCloseKey(oper_handlekey);
            break;
          }
         else
          {
            if((partkey.GetSize()-1) != c)
              {
                RegCloseKey(oper_handlekey);
              }
          }

        subkeystr3 +=__C('\\');
     }

  if(error == ERROR_SUCCESS)
    {
      registrykey.SetHandle(oper_handlekey);
    }

  partkey.DeleteContents();
  partkey.DeleteAll();

  return (error == ERROR_SUCCESS)?true:false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYMANAGER::CreateKey(HKEY handlekey, XSTRING& subkeystring , XWINDOWSREGISTRYKEY& registrykey)
* @brief      CreateKey
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  handlekey :
* @param[in]  subkeystring :
* @param[in]  registrykey :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYMANAGER::CreateKey(HKEY handlekey, XSTRING& subkeystring, XWINDOWSREGISTRYKEY& registrykey)
{
  return CreateKey(handlekey, subkeystring.Get(), registrykey);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYMANAGER::OpenKey(HKEY handlekey, XCHAR* subkeystring, XWINDOWSREGISTRYKEY& registrykey)
* @brief      OpenKey
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  handlekey :
* @param[in]  subkeystring :
* @param[in]  registrykey :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYMANAGER::OpenKey(HKEY handlekey, XCHAR* subkeystring, XWINDOWSREGISTRYKEY& registrykey)
{
  if(registrykey.GetHandle() != NULL) return false;

  HKEY    oper_handlekey;
  LSTATUS status;

  status = RegOpenKeyEx(handlekey, subkeystring, 0,  KEY_ALL_ACCESS | KEY_WOW64_64KEY, &oper_handlekey);
  if(status != ERROR_SUCCESS)  return false;

  registrykey.SetHandle(oper_handlekey);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYMANAGER::OpenKey(HKEY handlekey, XSTRING& subkeystring, XWINDOWSREGISTRYKEY& registrykey)
* @brief      OpenKey
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  handlekey :
* @param[in]  subkeystring :
* @param[in]  registrykey :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYMANAGER::OpenKey(HKEY handlekey, XSTRING& subkeystring, XWINDOWSREGISTRYKEY& registrykey)
{
  return OpenKey(handlekey, subkeystring.Get(), registrykey);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYMANAGER::EnumKeys(HKEY handlekey, XSTRING& subkeystring, XVECTOR<XSTRING*> keylist)
* @brief      EnumKeys
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  handlekey :
* @param[in]  subkeystring :
* @param[out] keylist :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYMANAGER::EnumKeys(XWINDOWSREGISTRYKEY& registrykey, XVECTOR<XSTRING*>* keylist)
{
  if(registrykey.GetHandle() == NULL) return false;

  LSTATUS status;
  XSTRING namekey;
  DWORD   size;
  int     index = 0;

  do{ size  = _MAXPATH;

      namekey.AdjustSize(size);
      status = RegEnumKeyEx(registrykey.GetHandle(), index, namekey.Get(), &size, 0, NULL, NULL, NULL);
      namekey.AdjustSize();

      //XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("--- %d"), status);

      if(status == ERROR_SUCCESS)
        {
          XSTRING* _namekey = new XSTRING();
          if(_namekey)
            {
              (*_namekey) = namekey;
              keylist->Add(_namekey);
            }

        } else break;

      index++;

    } while(status != ERROR_NO_MORE_ITEMS);

  return ((status == ERROR_SUCCESS) || (status == ERROR_NO_MORE_ITEMS))?true:false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYMANAGER::CloseKey(XWINDOWSREGISTRYKEY& registyrkey)
* @brief      CloseKey
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  registyrkey :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYMANAGER::CloseKey(XWINDOWSREGISTRYKEY& registrykey)
{
  if(registrykey.GetHandle() == NULL) return false;

  if(RegCloseKey(registrykey.GetHandle()) != ERROR_SUCCESS) return false;

  registrykey.Clear();

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYMANAGER::DeleteKey(HKEY handlekey, XCHAR* subkeystring)
* @brief      DeleteKey
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  handlekey :
* @param[in]  subkeystring :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYMANAGER::DeleteKey(HKEY handlekey, XCHAR* subkeystring)
{
  HKEY      operhandlekey = NULL;
  XSTRING   namekey;
  DWORD     size;
  LONG      result;
  FILETIME  ftwrite;

  result = RegDeleteKey(handlekey, subkeystring);
  if(result == ERROR_SUCCESS)  return false;

  result = RegOpenKeyEx(handlekey, subkeystring, 0, KEY_READ, &operhandlekey);
  if(result != ERROR_SUCCESS)
    {
      if(result == ERROR_FILE_NOT_FOUND)
             return true;
        else return false;
    }

  XPATH subkey = subkeystring;
  subkey.Slash_Add();

  size = _MAXPATH;

  namekey.AdjustSize(size);
  result = RegEnumKeyEx(operhandlekey, 0, namekey.Get(), &size, NULL, NULL, NULL, &ftwrite);
  namekey.AdjustSize();

  if(result == ERROR_SUCCESS)
    {
      do{ subkey.Add(namekey);
          if(!DeleteKey(handlekey, namekey))  break;

          size = _MAXPATH;

          namekey.AdjustSize(size);
          result = RegEnumKeyEx(operhandlekey, 0, namekey.Get(), &size, NULL, NULL, NULL, &ftwrite);
          namekey.AdjustSize();

        } while(result == ERROR_SUCCESS);
    }

  RegCloseKey(operhandlekey);

  // Try again to delete the key.
  result = RegDeleteKey(handlekey, subkeystring);

 if(result == ERROR_SUCCESS)  return true;

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSREGISTRYMANAGER::DeleteKey(HKEY handlekey, XSTRING& subkeystring)
* @brief      DeleteKey
* @ingroup    PLATFORM_WINDOWS
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  handlekey :
* @param[in]  subkeystring :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSREGISTRYMANAGER::DeleteKey(HKEY handlekey, XSTRING& subkeystring)
{
  return XWINDOWSREGISTRYMANAGER::DeleteKey(handlekey, subkeystring.Get());
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSREGISTRYMANAGER::Clean()
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
void XWINDOWSREGISTRYMANAGER::Clean()
{

}


#endif