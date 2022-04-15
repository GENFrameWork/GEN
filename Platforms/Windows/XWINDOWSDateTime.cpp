/**-------------------------------------------------------------------------------------------------------------------
*
* @file        XWINDOWSDateTime.cpp
*
* @class       XWINDOWSDATETIME
* @brief       eXtended WINDOWS Data Time class
* @ingroup     PLATFORM_WINDOWS
*
* @copyright  GEN Group. All right reserved.
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

#include <math.h>

#include "XString.h"
#include "XTrace.h"
#include "XPath.h"

#include "XWINDOWSDateTime.h"

#include "XMemory_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWINDOWSDATETIME::XWINDOWSDATETIME()
* @brief      Constructor
* @ingroup    PLATFORM_WINDOWS
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XWINDOWSDATETIME::XWINDOWSDATETIME()
{
  GetActualDateTime(this);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWINDOWSDATETIME::~XWINDOWSDATETIME()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_WINDOWS
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XWINDOWSDATETIME::~XWINDOWSDATETIME()
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDATETIME::Read()
* @brief      Read
* @ingroup    PLATFORM_WINDOWS
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDATETIME::Read()
{
  GetActualDateTime(this);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDATETIME::Write()
* @brief      Write
* @ingroup    PLATFORM_WINDOWS
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDATETIME::Write()
{
  SetActualDateTime(this);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDATETIME::GetFileDateTime(XPATH& xpath, void* tmzip, XDWORD* dt)
* @brief      GetFileDateTime
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  xpath :
* @param[in]  tmzip :
* @param[in]  dt :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDATETIME::GetFileDateTime(XPATH& xpath, void* tmzip, XDWORD* dt)
{
  HANDLE           hfind;
  FILETIME         ftlocal;
  WIN32_FIND_DATA  ff32;

  hfind = FindFirstFile(xpath.Get(),&ff32);
  if(hfind != INVALID_HANDLE_VALUE)
    {
      FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftlocal);
      FileTimeToDosDateTime(&ftlocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
      FindClose(hfind);

      return true;
    }

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDATETIME::GetFileDateTime(XPATH& xpath)
* @brief      GetFileDateTime
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  xpath :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDATETIME::GetFileDateTime(XPATH& xpath)
{
  HANDLE           hfind;
  FILETIME         ftlocal;
  WIN32_FIND_DATA  ff32;
  SYSTEMTIME       systemtime;

  hfind = FindFirstFile(xpath.Get(),&ff32);
  if(hfind != INVALID_HANDLE_VALUE)
    {
      FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftlocal);
      FileTimeToSystemTime(&ftlocal, &systemtime);
      FindClose(hfind);

      year          = systemtime.wYear;
      month         = systemtime.wMonth;
      day           = systemtime.wDay;

      hours         = systemtime.wHour;
      minutes       = systemtime.wMinute;
      seconds       = systemtime.wSecond;
      milliseconds  = systemtime.wMilliseconds;

      return true;
    }

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XWINDOWSDATETIME:GetMeridianDifference()
* @brief      GetMeridianDifference
* @ingroup    PLATFORM_WINDOWS
*
* @return     int :
*
*---------------------------------------------------------------------------------------------------------------------*/
int XWINDOWSDATETIME::GetMeridianDifference()
{
  TIME_ZONE_INFORMATION timezoneinfo = { 0 };

  ::GetTimeZoneInformation(&timezoneinfo);

  float difference = -(float(timezoneinfo.Bias)/60);

  return (int)difference;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDATETIME::IsDayLigthSavingTime(int* bias)
* @brief      IsDayLigthSavingTime
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  bias :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDATETIME::IsDayLigthSavingTime(int* bias)
{
  TIME_ZONE_INFORMATION timezoneinfo = { 0 };

  XDWORD ret = (XDWORD)::GetTimeZoneInformation(&timezoneinfo);

  switch(ret)
    {
      case TIME_ZONE_ID_UNKNOWN   : break;

      case TIME_ZONE_ID_INVALID   :
      case TIME_ZONE_ID_STANDARD  : if(bias)
                                      {
                                        (*bias)  = timezoneinfo.StandardBias;
                                        (*bias) *= 60;
                                      }
                                    break;

      case TIME_ZONE_ID_DAYLIGHT  : if(bias)
                                      {
                                        (*bias)  = timezoneinfo.DaylightBias;
                                        (*bias) *= 60;
                                      }
                                    return true;
    }

  return false;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSDATETIME::GetActualDateTime(XWINDOWSDATETIME* time)
* @brief      GetActualDateTime
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  time :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSDATETIME::GetActualDateTime(XWINDOWSDATETIME* time)
{
  SYSTEMTIME st;

  GetLocalTime(&st);

  time->year          = st.wYear;
  time->month         = st.wMonth;
  time->day           = st.wDay;

  time->hours         = st.wHour;
  time->minutes       = st.wMinute;
  time->seconds       = st.wSecond;
  time->milliseconds  = st.wMilliseconds;
};



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSDATETIME::SetActualDateTime(XWINDOWSDATETIME* time)
* @brief      SetActualDateTime
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  time :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSDATETIME::SetActualDateTime(XWINDOWSDATETIME* time)
{
  SYSTEMTIME st;

  st.wYear          = time->year;
  st.wMonth         = time->month;
  st.wDay           = time->day;

  st.wHour          = time->hours;
  st.wMinute        = time->minutes;
  st.wSecond        = time->seconds;
  st.wMilliseconds  = time->milliseconds;

  SetLocalTime(&st);
};

