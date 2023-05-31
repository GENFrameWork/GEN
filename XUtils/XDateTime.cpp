/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XDateTime.cpp
*
* @class      XDATETIME
* @brief      eXtended date time class
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "XBase.h"
#include "XFactory.h"
#include "XTranslation_GEN.h"

#include "XDateTime.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDATETIME::XDATETIME()
* @brief      Constructor
* @ingroup    XUTILS
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XDATETIME::XDATETIME()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDATETIME::~XDATETIME()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XDATETIME::~XDATETIME()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XDATETIME::GetYear()
* @brief      GetYear
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XDATETIME::GetYear()
{
  return year;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XDATETIME::GetMonth()
* @brief      GetMonth
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XDATETIME::GetMonth()
{
  return month;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XDATETIME::GetDay()
* @brief      GetDay
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XDATETIME::GetDay()
{
  return day;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XDATETIME::GetHours()
* @brief      GetHours
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XDATETIME::GetHours()
{
  return hours;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XDATETIME::GetMinutes()
* @brief      GetMinutes
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XDATETIME::GetMinutes()
{
  return minutes;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XDATETIME::GetSeconds()
* @brief      GetSeconds
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XDATETIME::GetSeconds()
{
  return seconds;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XDATETIME::GetMilliSeconds()
* @brief      GetMilliSeconds
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XDATETIME::GetMilliSeconds()
{
  return milliseconds;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::IsZero()
* @brief      IsZero
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::IsZero()
{
  if(!year && !month && !day && !hours && !minutes && !seconds) 
    {
      return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XDATETIME::IsLocal()
* @brief      IsLocal
* @ingroup    XUTILS
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::IsLocal()
{
  return islocal;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::IsLeapYear()
* @brief      IsLeapYear
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::IsLeapYear()
{
  return ((((year%4) == 0) & ((year%100) != 0)) | ((year%400) == 0))?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDATETIME_DAYWEEK XDATETIME::GetDayOfWeek()
* @brief      GetDayOfWeek
* @ingroup    XUTILS
*
* @return     XDATETIME_DAYWEEK :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDATETIME_DAYWEEK XDATETIME::GetDayOfWeek()
{
  int monthcode[] = { 0, 6, 2, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
  int result      = year%100 + (year%100)/4 + day + monthcode[month];

  if((year/100) == 17) 
    {
      result += 5;
    }    
   else 
    { 
      if((year/100) == 18) 
        {
          result += 3;
        }
       else 
        {
          if((year/100) == 19) 
            {
              result += 1;
            }
           else 
            {
              if((year/100) == 20) 
                {
                  result += 0;
                }
               else 
                {
                  if((year/100) == 21) 
                    {
                      result += -2;
                    }
                   else 
                    {
                      if((year/100) == 22)  
                        {
                          result += -4;
                        }
                    }
                }
            }
        }
    }

  if(IsLeapYear() & (month == 1 || month == 2)) 
    {
      result += -1;
    }

  return (XDATETIME_DAYWEEK)(result%7);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XDATETIME::GetDaysOfYear()
* @brief      GetDaysOfYear
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XDATETIME::GetDaysOfYear()
{
  int lastndays = GetNDaysInGregorian();

  int old_day    = day;
  int old_month  = month;

  day   = 1;
  month = 1;

  int ndays = GetNDaysInGregorian();

  day   = old_day;
  month = old_month;

  return (lastndays - ndays);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         long XDATETIME::GetNDaysInJulian()
* @brief      GetNDaysInJulian
* @ingroup    XUTILS
*
* @return     long :
*
* --------------------------------------------------------------------------------------------------------------------*/
long XDATETIME::GetNDaysInJulian()
{
  int _year   = year;
  int _month  = month;
  int _day    = day;

  if(_month<3)
    {
      _year--;
      _month += 12;
    }

  _year  += 4800;
  _month -= 3;

  return (_year*365) + (_year/4) + (30*_month) + (_month*3+2)/5 + _day - 32083;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         long XDATETIME::GetNDaysInGregorian()
* @brief      GetNDaysInGregorian
* @ingroup    XUTILS
*
* @return     long :
*
* --------------------------------------------------------------------------------------------------------------------*/
long XDATETIME::GetNDaysInGregorian()
{
  int _year   = year;
  int _month  = month;
  int _day    = day;

  if(_month<3)
    {
      _year--;
      _month+=12;
    }

  _year += 8000;

  return (_year*365) + (_year/4) - (_year/100) + (_year/400) + (_month*153+3)/5-92 + _day - 1 -1200820;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XDATETIME::GetDateFromNDays(long ndays, bool injulian)
* @brief      GetDateFromNDays
* @ingroup    XUTILS
*
* @param[in]  ndays :
* @param[in]  injulian :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XDATETIME::GetDateFromNDays(long ndays, bool injulian)
{
  int _year;
  int _month;
  int _day;

  if(injulian)
    {
      for(_year=ndays/366-4715; GetNDaysInJulian( 1, 1, _year+1)<=ndays             ; _year++);
      for(_month=1;             GetNDaysInJulian(1, _month+1, _year)<=ndays         ; _month++);
      for(_day=1;               GetNDaysInJulian( _day+1, _month, _year)<=ndays     ; _day++);
    }
   else
    {
      for(_year=ndays/366-4715; GetNDaysInGregorian( 1, 1, _year+1)<=ndays          ; _year++);
      for(_month=1;             GetNDaysInGregorian(1, _month+1, _year)<=ndays      ; _month++);
      for(_day=1;               GetNDaysInGregorian( _day+1, _month, _year)<=ndays  ; _day++);
    }

  year  = _year;
  month = _month;
  day   = _day;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XQWORD XDATETIME::GetSeconsFromDate(bool injulian)
* @brief      GetSeconsFromDate
* @ingroup    XUTILS
*
* @param[in]  injulian :
*
* @return     XQWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XQWORD XDATETIME::GetSeconsFromDate(bool injulian)
{
  XQWORD  allseconds = injulian?GetNDaysInJulian():GetNDaysInGregorian();

  allseconds *= (24*60*60);
  allseconds += (((hours*60)+minutes)*60)+seconds;

  return allseconds;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::SetDateFromSeconds(XQWORD allseconds, bool injulian)
* @brief      SetDateFromSeconds
* @ingroup    XUTILS
*
* @param[in]  allseconds :
* @param[in]  injulian :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::SetDateFromSeconds(XQWORD allseconds, bool injulian)
{
  long   ndays  = (long)(allseconds / (24*60*60));
  XQWORD hinsec = (XDWORD)(allseconds - (ndays*(24*60*60)));

  hours   = (int)(hinsec / (60*60));
  minutes = (int)((hinsec % (60*60)) / 60);
  seconds = (int)((hinsec % (60*60)) % 60);

  GetDateFromNDays(ndays,injulian);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD XDATETIME::GetSecondsFromADay()
* @brief      GetSecondsFromADay
* @ingroup    XUTILS
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD XDATETIME::GetSecondsFromADay()
{
  XDWORD _seconds = 0;

  _seconds += (hours*3600);
  _seconds += (minutes*60);
  _seconds += seconds;

  return _seconds;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XDATETIME::SetIsLocal(bool islocal)
* @brief      SetIsLocal
* @ingroup    XUTILS
* 
* @param[in]  islocal : 
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void XDATETIME::SetIsLocal(bool islocal)
{
  this->islocal = islocal;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XDATETIME::SetYear(int year)
* @brief      SetYear
* @ingroup    XUTILS
*
* @param[in]  year :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XDATETIME::SetYear(int year)
{
  this->year = year;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XDATETIME::SetMonth(int month)
* @brief      SetMonth
* @ingroup    XUTILS
*
* @param[in]  month :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XDATETIME::SetMonth(int month)
{
  this->month = month;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XDATETIME::SetDay(int day)
* @brief      SetDay
* @ingroup    XUTILS
*
* @param[in]  day :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XDATETIME::SetDay(int day)
{
  this->day = day;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XDATETIME::SetHours(int hours)
* @brief      SetHours
* @ingroup    XUTILS
*
* @param[in]  hours :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XDATETIME::SetHours(int hours)
{
  this->hours = hours;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XDATETIME::SetMinutes(int minutes)
* @brief      SetMinutes
* @ingroup    XUTILS
*
* @param[in]  minutes :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XDATETIME::SetMinutes(int minutes)
{
  this->minutes = minutes;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XDATETIME::SetSeconds(int seconds)
* @brief      SetSeconds
* @ingroup    XUTILS
*
* @param[in]  seconds :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XDATETIME::SetSeconds(int seconds)
{
  this->seconds = seconds;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XDATETIME::SetMilliSeconds(int milliseconds)
* @brief      SetMilliSeconds
* @ingroup    XUTILS
*
* @param[in]  milliseconds :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XDATETIME::SetMilliSeconds(int milliseconds)
{
  this->milliseconds = milliseconds;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::IsValidDate()
* @brief      IsValidDate
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::IsValidDate()
{
  XWORD monthlen[]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  if((month<=0) || (day<=0)) 
    {
      return false;
    }

  if(month>12)               
    {
      return false;
    }

  if(IsLeapYear() && month==2) 
    {
      monthlen[1]++;
    }

  if(day>monthlen[month-1]) 
    {
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::CopyFrom(XDATETIME* xtimesource)
* @brief      CopyFrom
* @ingroup    XUTILS
*
* @param[in]  xtimesource :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::CopyFrom(XDATETIME* xtimesource)
{
  if(!xtimesource)                 
    {
      return false;
    }

  year         = xtimesource->GetYear();
  month        = xtimesource->GetMonth();
  day          = xtimesource->GetDay();
  hours        = xtimesource->GetHours();
  minutes      = xtimesource->GetMinutes();
  seconds      = xtimesource->GetSeconds();
  milliseconds = xtimesource->GetMilliSeconds();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::CopyFrom(XDATETIME& xtimesource)
* @brief      CopyFrom
* @ingroup    XUTILS
*
* @param[in]  xtimesource :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::CopyFrom(XDATETIME& xtimesource)
{
  return CopyFrom(&xtimesource);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::CopyTo(XDATETIME* xtimetarget)
* @brief      CopyTo
* @ingroup    XUTILS
*
* @param[in]  xtimetarget :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::CopyTo(XDATETIME* xtimetarget)
{
  if(!xtimetarget) 
    {
      return false;
    }

  xtimetarget->SetYear(year);
  xtimetarget->SetMonth(month);
  xtimetarget->SetDay(day);
  xtimetarget->SetHours(hours);
  xtimetarget->SetMinutes(minutes);
  xtimetarget->SetSeconds(seconds);
  xtimetarget->SetMilliSeconds(milliseconds);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::CopyTo(XDATETIME& xtimetarget)
* @brief      CopyTo
* @ingroup    XUTILS
*
* @param[in]  xtimetarget :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::CopyTo(XDATETIME& xtimetarget)
{
  return CopyTo(&xtimetarget);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::IsTheSameDay(XDATETIME* xtime)
* @brief      IsTheSameDay
* @ingroup    XUTILS
*
* @param[in]  xtime :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::IsTheSameDay(XDATETIME* xtime)
{
  if(year != xtime->GetYear())   
    {
      return false;
    }

  if(month != xtime->GetMonth())  
    {
      return false;
    }

  if(day != xtime->GetDay())    
    {
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::IsTheSameDay(XDATETIME& xtime)
* @brief      IsTheSameDay
* @ingroup    XUTILS
*
* @param[in]  xtime :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::IsTheSameDay(XDATETIME& xtime)
{
  return IsTheSameDay(&xtime);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::AddYears(int years)
* @brief      AddYears
* @ingroup    XUTILS
*
* @param[in]  years :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::AddYears(int years)
{
  this->year += years;

  int c=0;

  while(!IsValidDate())
    {
      if(years<0) 
        {
          AddDays(-1); 
        }
       else 
        {
          AddDays(1);
        }

      c++;

      if(c>5) 
        {
          return false;
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::AddMonths(int months)
* @brief      AddMonths
* @ingroup    XUTILS
*
* @param[in]  months :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::AddMonths(int months)
{
  int  years = (months / 12);
  int  rest  = (months % 12);

  if(years) AddYears(years);

  this->month +=  rest;
  if(this->month>12)
    {
      AddYears(1);
      this->month -= 12;
    }

  if(this->month<1)
    {
      AddYears(-1);
      this->month = 13 + rest;
    }

  int c=0;
  while(!IsValidDate())
    {
      if(months<0) AddDays(-1); else AddDays(1);
      c++;
      if(c>5) return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::AddDays(int days)
* @brief      AddDays
* @ingroup    XUTILS
*
* @param[in]  days :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::AddDays(int days)
{
  long ndays = GetNDaysInGregorian();

  ndays += days;

  GetDateFromNDays(ndays,false);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::AddHours(XQWORD hours)
* @brief      AddHours
* @ingroup    XUTILS
*
* @param[in]  hours :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::AddHours(XQWORD hours)
{
  XQWORD allseconds = GetSeconsFromDate();

  allseconds += hours*3600;

  return SetDateFromSeconds(allseconds);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::AddMinutes(XQWORD minutes)
* @brief      AddMinutes
* @ingroup    XUTILS
*
* @param[in]  minutes :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::AddMinutes(XQWORD minutes)
{
  XQWORD allseconds = GetSeconsFromDate();

  allseconds += minutes*60;

  return SetDateFromSeconds(allseconds);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::AddSeconds(XQWORD seconds)
* @brief      AddSeconds
* @ingroup    XUTILS
*
* @param[in]  seconds :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::AddSeconds(XQWORD seconds)
{
  XQWORD allseconds = GetSeconsFromDate();

  allseconds += seconds;

  return SetDateFromSeconds(allseconds);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::SubtractYears(int years)
* @brief      SubtractYears
* @ingroup    XUTILS
*
* @param[in]  years :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::SubtractYears(int years)
{
  this->year -= years;

  int c=0;
  while(!IsValidDate())
    {
      if(years<0) 
        {
          AddDays(-1); 
        }
       else 
        {
          AddDays(1);
        }

      c++;

      if(c>5) 
        {
          return false;
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::SubtractMonths(int months)
* @brief      SubtractMonths
* @ingroup    XUTILS
*
* @param[in]  months :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::SubtractMonths(int months)
{
  int  years = (months / 12);
  int  rest  = (months % 12);

  if(years) 
    {
      SubtractYears(years);
    }

  this->month -=  rest;
  if(this->month>12)
    {
      SubtractYears(1);
      this->month -= 12;
    }

  if(this->month<1)
    {
      SubtractYears(-1);
      this->month = 13 + rest;
    }

  int c=0;
  while(!IsValidDate())
    {
      if(months<0) 
        {
          SubtractDays(-1); 
        }
       else 
        {
          SubtractDays(1);
        }

      c++;

      if(c>5) 
        {
          return false;
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::SubtractDays(int days)
* @brief      SubtractDays
* @ingroup    XUTILS
*
* @param[in]  days :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::SubtractDays(int days)
{
  long ndays = GetNDaysInGregorian();

  ndays -= days;

  GetDateFromNDays(ndays,false);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::SubtractHours(XQWORD hours)
* @brief      SubtractHours
* @ingroup    XUTILS
*
* @param[in]  hours :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::SubtractHours(XQWORD hours)
{
  XQWORD allseconds = GetSeconsFromDate();

  allseconds -= hours*3600;

  return SetDateFromSeconds(allseconds);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::SubtractMinutes(XQWORD minutes)
* @brief      SubtractMinutes
* @ingroup    XUTILS
*
* @param[in]  minutes :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::SubtractMinutes(XQWORD minutes)
{
  XQWORD allseconds = GetSeconsFromDate();

  allseconds -= minutes*60;

  return SetDateFromSeconds(allseconds);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::SubtractSeconds(XQWORD seconds)
* @brief      SubtractSeconds
* @ingroup    XUTILS
*
* @param[in]  seconds :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::SubtractSeconds(XQWORD seconds)
{
  XQWORD allseconds = GetSeconsFromDate();

  allseconds -= seconds;

  return SetDateFromSeconds(allseconds);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::operator == (XDATETIME xtime)
* @brief      operator ==
* @ingroup    XUTILS
*
* @param[in]  xtime :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::operator == (XDATETIME xtime)
{
  XQWORD s1 =  GetSeconsFromDate();
  XQWORD s2 =  xtime.GetSeconsFromDate();

  if((s1 == s2) && (xtime.GetMilliSeconds() == milliseconds)) 
    {
      return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::operator != (XDATETIME xtime)
* @brief      operator !=
* @ingroup    XUTILS
*
* @param[in]  xtime :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::operator != (XDATETIME xtime)
{
  XQWORD s1 =  GetSeconsFromDate();
  XQWORD s2 =  xtime.GetSeconsFromDate();

  if((s1 != s2) || (xtime.GetMilliSeconds() != milliseconds)) 
    {
      return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::operator > (XDATETIME xtime)
* @brief      operator >
* @ingroup    XUTILS
*
* @param[in]  xtime :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::operator >  (XDATETIME xtime)
{
  XQWORD s1 = GetSeconsFromDate();
  XQWORD s2 = xtime.GetSeconsFromDate();

  if(s1 > s2) 
    {
      return true;
    }

  if(s1 == s2)
    {
      if(milliseconds > xtime.GetMilliSeconds()) 
        {
          return true;
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::operator >= (XDATETIME xtime)
* @brief      operator >=
* @ingroup    XUTILS
*
* @param[in]  xtime :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::operator >= (XDATETIME xtime)
{
  XQWORD s1 =  GetSeconsFromDate();
  XQWORD s2 =  xtime.GetSeconsFromDate();

  if(s1 >= s2) 
    {
      return true;
    }

  if(s1 == s2)
    {
      if(milliseconds >= xtime.GetMilliSeconds())
        {
          return true;
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::operator < (XDATETIME xtime)
* @brief      operator <
* @ingroup    XUTILS
*
* @param[in]  xtime :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::operator <  (XDATETIME xtime)
{
  XQWORD s1 =  GetSeconsFromDate();
  XQWORD s2 =  xtime.GetSeconsFromDate();

  if(s1 < s2) 
    { 
      return true;
    }

  if(s1 == s2)
    {
      if(milliseconds < xtime.GetMilliSeconds()) 
        {
          return true;
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::operator <= (XDATETIME xtime)
* @brief      operator <=
* @ingroup    XUTILS
*
* @param[in]  xtime :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::operator <= (XDATETIME xtime)
{
  XQWORD s1 =  GetSeconsFromDate();
  XQWORD s2 =  xtime.GetSeconsFromDate();

  if(s1 <= s2) 
    {
      return true;
    }

  if(s1 == s2)
    {
      if(milliseconds <= xtime.GetMilliSeconds()) 
        {
          return true;
        }
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::IsBetween(XDATETIME& start, XDATETIME& end)
* @brief      IsBetween
* @ingroup    XUTILS
*
* @param[in]  start :
* @param[in]  end :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::IsBetween(XDATETIME& start, XDATETIME& end)
{
  return ((start <= (*this)) &&  (end >= (*this)))?true:false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XDATETIME::Read(bool islocal)
* @brief      Read
* @ingroup    XUTILS
* 
* @param[in]  ismode : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::Read(bool islocal)
{
  SetIsLocal(islocal);

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XDATETIME::Write(bool localmode)
* @brief      Write
* @ingroup    XUTILS
* 
* @param[in]  localmode : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::Write(bool islocal)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::GetFileDateTime(XPATH& xpath, void* tmzip, XDWORD* dt)
* @brief      GetFileDateTime
* @ingroup    XUTILS
*
* @param[in]  xpath :
* @param[in]  tmzip :
* @param[in]  dt :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::GetFileDateTime(XPATH& xpath, void* tmzip, XDWORD* dt)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::GetFileDateTime(XPATH& xpath)
* @brief      GetFileDateTime
* @ingroup    XUTILS
*
* @param[in]  xpath :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::GetFileDateTime(XPATH& xpath)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XDATETIME::GetMeridianDifference()
* @brief      GetMeridianDifference
* @ingroup    XUTILS
*
* @return     int :
*
* --------------------------------------------------------------------------------------------------------------------*/
int XDATETIME::GetMeridianDifference()
{
  return 0;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::IsDayLigthSavingTime(int* bias)
* @brief      IsDayLigthSavingTime
* @ingroup    XUTILS
*
* @param[in]  bias :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::IsDayLigthSavingTime(int* bias)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::GetDateTimeToString(XDWORD modificator, XSTRING& string)
* @brief      GetDateTimeToString
* @ingroup    XUTILS
*
* @param[in]  modificator :
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::GetDateTimeToString(XDWORD modificator, XSTRING& string)
{ 
  string.Empty();

  XSTRING datestr;
  XSTRING timestr;
  XCHAR   separator = ((IsModificatorActive(modificator, XDATETIME_FORMAT_DATEWITHDASH))?__C('-'):__C('/'));

  if(IsModificatorActive(modificator, XDATETIME_FORMAT_TEXTMONTH))
    {
      XSTRING monthtext;
      XSTRING preseparator;

      preseparator =  XT_L(XTRANSLATION_GEN_ID_XDATETIME_MONTH_PRESEPARATOR);

      GetMonthString(monthtext);

      if(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTDATEDAY)) 
        {
          datestr.Format(__L("%02d %s %s %s %04d"), day, preseparator.Get(), monthtext.Get(), preseparator.Get(), year);
        }
       else 
        { 
          if(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTDATEMONTH)) 
            { 
              datestr.Format(__L("%s, %02d %s %04d"), monthtext.Get(), day, preseparator.Get(), year);
            }
           else 
            { 
              if(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTDATEYEAR)) 
                {
                  datestr.Format(__L("%04d %s %s %s %02d"), year, preseparator.Get(), monthtext.Get(), preseparator.Get(), day);
                }
               else 
                {
                  datestr.Format(__L("%02d %s %s %s %04d"), day, preseparator.Get(), monthtext.Get(), preseparator.Get(), year);
                }
            }
        }
    }
   else
    {
      if(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTDATEDAY)) 
        {
          datestr.Format(__L("%02d%c%02d%c%04d"), day, separator, month, separator, year);
        }
       else 
        {
          if(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTDATEMONTH)) 
            {
              datestr.Format(__L("%02d%c%02d%c%04d"), month, separator, day, separator, year);
            }
           else 
            {
              if(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTDATEYEAR)) 
                {
                  datestr.Format(__L("%04d%c%02d%c%02d"), year, separator, month, separator, day);
                }
               else 
                {
                  datestr.Format(__L("%02d%c%02d%c%04d"), day, separator, month, separator, year);
                }
            }
        }
    }

  if(IsModificatorActive(modificator, XDATETIME_FORMAT_TIMEWITHSECONDS))
    {
      timestr.Format(__L("%02d:%02d:%02d"), hours, minutes, seconds);
    }
   else  
    {
      timestr.Format(__L("%02d:%02d"), hours, minutes);
    }

  if(IsModificatorActive(modificator, XDATETIME_FORMAT_TIMEWITHMILLISECONDS))
    {
      timestr.AddFormat(__L(".%03d"), milliseconds);
    }  

  if(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTTIME))
    {
      if(IsModificatorActive(modificator, XDATETIME_FORMAT_ADDTIME))
        {
          string += timestr;
        }

      if(IsModificatorActive(modificator, XDATETIME_FORMAT_ADDDATE))
        {
          if(!string.IsEmpty()) string += __L(" ");
          string += datestr;
        }
    }
   else
    {
      if(IsModificatorActive(modificator, XDATETIME_FORMAT_ADDDATE)) 
        {
          string += datestr;
        }

      if(IsModificatorActive(modificator, XDATETIME_FORMAT_ADDTIME))
        {
          if(!string.IsEmpty()) string += __L(" ");
          string += timestr;
        }
    }

  if(IsModificatorActive(modificator, XDATETIME_FORMAT_ADDDAYOFWEEK))
    {
      if(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTDAYOFWEEK))
        {
          XSTRING dayofweek;
          XSTRING string2;

          GetDayOfWeekString(dayofweek);

          string2 += dayofweek;

          if(!string.IsEmpty()) string2 += __L(" ");

          string2 += string;

          string = string2;
        }
       else
        {
          XSTRING dayofweek;

          GetDayOfWeekString(dayofweek);

          if(!string.IsEmpty()) string += __L(" ");
          string += dayofweek;
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::GetDateTimeToStringISO8601(XDWORD modificator, XSTRING& string)
* @brief      GetDateTimeToStringISO8601
* @ingroup    XUTILS
*
* @param[in]  modificator : 
* @param[in]  string : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::GetDateTimeToStringISO8601(XDWORD modificator, XSTRING& string)
{
  string.Empty();
  
  XSTRING datestr;
  XSTRING timestr;
  XCHAR   dateseparator = __C('-');
  XCHAR   timeseparator = __C(':');

  if(!islocal) 
    {
      modificator |= XDATETIME_FORMAT_ISO8601_UTC;
    }

  if(IsModificatorActive(modificator, XDATETIME_FORMAT_ISO8601_BASIC))
    {
      datestr.Format(__L("%04d%02d%02d"), year, month, day);
    
      if(IsModificatorActive(modificator, XDATETIME_FORMAT_TIMEWITHSECONDS))
        {
          timestr.Format(__L("%02d%02d%02d"), hours, minutes, seconds);
        }
       else  
        {
          timestr.Format(__L("%02d%02d"), hours, minutes);
        }
    }
   else
    {
      datestr.Format(__L("%04d%c%02d%c%02d") , year, dateseparator, month, dateseparator, day);
    
      if(IsModificatorActive(modificator, XDATETIME_FORMAT_TIMEWITHSECONDS))
        {
          timestr.Format(__L("%02d%c%02d%c%02d"), hours, timeseparator, minutes, timeseparator , seconds);
        }
       else  
        {
          timestr.Format(__L("%02d%c%02d"), hours, timeseparator, minutes, timeseparator);
        }
    }

  if(IsModificatorActive(modificator, XDATETIME_FORMAT_TIMEWITHMILLISECONDS))
    {
      timestr.AddFormat(__L(".%03d"), milliseconds);
    }  

  if(IsModificatorActive(modificator, XDATETIME_FORMAT_ADDDATE))
    {
      string += datestr;
    }

  if(IsModificatorActive(modificator, XDATETIME_FORMAT_ADDTIME))
    {
      if(!string.IsEmpty()) 
        {
          if(IsModificatorActive(modificator, XDATETIME_FORMAT_ISO8601_UTC))
            {
              string += __L("T");
            }
           else    
            {
              string += __L(" ");
            }
        }

      string += timestr;
    }

  if(IsModificatorActive(modificator, XDATETIME_FORMAT_ISO8601_UTC))
    {
      if(IsModificatorActive(modificator, XDATETIME_FORMAT_ISO8601_ADDHOUROFFSET))
        {
          int meridiandiff  = GetMeridianDifference();

          if(!meridiandiff)
            {
              string.Add(__L("Z"));
            }
           else
            {
              XDATETIME*  datetime = GEN_XFACTORY.CreateDateTime();
              if(datetime)
                {
                  XSTRING meridiandiffstr;  
                  int     bias         = 0;
                  
                  datetime->IsDayLigthSavingTime(&bias);
                  
                  datetime->SetToZero();

                  datetime->SetIsLocal(true);

                  datetime->SetDay(1);
                  datetime->SetMonth(1);
                  datetime->SetYear(1);

                  datetime->AddMinutes(abs(meridiandiff) + abs(bias));
              
                  datetime->GetDateTimeToString(XDATETIME_FORMAT_ADDTIME, meridiandiffstr);

                  string.AddFormat(__L("%c%s"), (meridiandiff>0)?__C('+'):__C('-'), meridiandiffstr.Get());

                  GEN_XFACTORY.DeleteDateTime(datetime);
                }
            }
        }
       else
        {
          string.Add(__L("Z"));
        }
    }
     
  return IsValidDate(); 
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::GetDateTimeFromString(XSTRING& string, XWORD modificator)
* @brief      GetDateTimeFromString
* @ingroup    XUTILS
*
* @param[in]  string :
* @param[in]  modificator :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::GetDateTimeFromString(XSTRING& string, XWORD modificator)
{
  int           index        = 0;
  XVECTOR<int>  typeblock;
  int           nblock       = 0;
  int           startindex   = 0;
  XCHAR         separator    = ((IsModificatorActive(modificator, XDATETIME_FORMAT_DATEWITHDASH))?__C('-'):__C('/'));
  XSTRING       validcharacters;

  validcharacters  = __L("0123456789 :.");
  validcharacters += separator;

  if(!string.AreValidCharacters(validcharacters.Get())) return false;

  if((IsModificatorActive(modificator, XDATETIME_FORMAT_ADDDAYOFWEEK)) && (IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTDAYOFWEEK)))    
    {
      typeblock.Add(1);
    }

  if((IsModificatorActive(modificator, XDATETIME_FORMAT_ADDTIME)) && (IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTTIME)))         
    {
      typeblock.Add(2);
    }

  if(IsModificatorActive(modificator, XDATETIME_FORMAT_ADDDATE))                                                                                  
    {
      typeblock.Add(3);
    }

  if((IsModificatorActive(modificator, XDATETIME_FORMAT_ADDTIME)) && (!(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTTIME))))      
    {
      typeblock.Add(2);
    }

  if((IsModificatorActive(modificator, XDATETIME_FORMAT_ADDDAYOFWEEK)) && (!(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTDAYOFWEEK)))) 
    {
      typeblock.Add(1);
    }

  while(index<=(int)string.GetSize())
    {
      XCHAR character = string.Get()[index];

      if((character == 0x20) || (index==(int)string.GetSize()))
        {
          XSTRING partial;

          if(index<(int)string.GetSize())
            {
              string.Copy(startindex, index, partial);
            }
           else 
            {
              string.Copy(startindex, partial);
            }

          if(!partial.IsEmpty())
            {
              switch(typeblock.Get(nblock))
                {
                  case 1: break; // Day of Week not used (can be calculated).

                  case 2: if(IsModificatorActive(modificator, XDATETIME_FORMAT_TIMEWITHMILLISECONDS))
                            {
                              if(IsModificatorActive(modificator, XDATETIME_FORMAT_TIMEWITHSECONDS))
                                {
                                  partial.UnFormat(__L("%d:%d:%d.%d"), &hours ,&minutes ,&seconds, &milliseconds);
                                }
                               else  
                                {
                                  partial.UnFormat(__L("%d:%d:%d"), &hours ,&minutes, &milliseconds);
                                }
                            }
                           else
                            {
                              if(IsModificatorActive(modificator, XDATETIME_FORMAT_TIMEWITHSECONDS))
                                {
                                  partial.UnFormat(__L("%d:%d:%d"), &hours ,&minutes ,&seconds);
                                }
                               else  
                                {
                                  partial.UnFormat(__L("%d:%d"), &hours ,&minutes);
                                }
                            }
                          break;

                  case 3: if(separator == __C('-'))
                            {
                              if(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTDATEDAY)) 
                                {
                                  partial.UnFormat(__L("%d-%d-%d"), &day, &month, &year);
                                }
                               else 
                                {
                                  if(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTDATEMONTH))
                                    {
                                      partial.UnFormat(__L("%d-%d-%d"), &month, &day, &year);
                                    }
                                   else 
                                    {
                                      if(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTDATEYEAR))
                                        {
                                          partial.UnFormat(__L("%d-%d-%d"), &year, &month, &day);
                                        }
                                       else 
                                        {
                                          partial.UnFormat(__L("%d-%d-%d"), &day, &month, &year);
                                        }
                                    }
                                }
                            }

                          if(separator == __C('/'))
                            {
                              if(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTDATEDAY))
                                {
                                  partial.UnFormat(__L("%d/%d/%d"), &day, &month, &year);
                                }
                               else 
                                {
                                  if(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTDATEMONTH))
                                    {
                                      partial.UnFormat(__L("%d/%d/%d"), &month, &day, &year);
                                    }
                                   else 
                                    {
                                      if(IsModificatorActive(modificator, XDATETIME_FORMAT_FIRSTDATEYEAR))
                                        {
                                          partial.UnFormat(__L("%d/%d/%d"), &year, &month, &day);
                                        }
                                       else 
                                        {
                                          partial.UnFormat(__L("%d/%d/%d"), &day, &month, &year);
                                        }
                                    }
                                }
                            }

                          break;
                }

              nblock++;
            }

          startindex = index+1;
        }

      index++;
    }

  typeblock.DeleteAll();

  return IsValidDate();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XDATETIME::GetDateTimeFromStringISO8601(XSTRING& string, XWORD modificator)
* @brief      GetDateTimeFromStringISO8601
* @ingroup    XUTILS
* 
* @param[in]  string : 
* @param[in]  modificator : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::GetDateTimeFromStringISO8601(XSTRING& string, XWORD modificator)
{
  int           index            = 0;
  XVECTOR<int>  typeblock;
  int           nblock           = 0;
  int           startindex       = 0;
  XCHAR         dateseparator    = __C('-');
  XCHAR         timeseparator    = __C(':');
  XSTRING       validcharacters;


  if(!islocal) 
    {
      modificator |= XDATETIME_FORMAT_ISO8601_UTC;
    }

  validcharacters  = __L("0123456789 -:.TZ");
  
  if(!string.AreValidCharacters(validcharacters.Get())) return false;

  if(IsModificatorActive(modificator, XDATETIME_FORMAT_ADDDATE))                                                                                  
    {
      typeblock.Add(1);
    }

  if((IsModificatorActive(modificator, XDATETIME_FORMAT_ADDTIME)))      
    {
      typeblock.Add(2);
    }

  while(index<=(int)string.GetSize())
    {
      XCHAR character = string.Get()[index];

      if((character == 0x20) || (character == __C('T')) || (index==(int)string.GetSize()))
        {
          XSTRING partial;

          if(index<(int)string.GetSize())
            {
              string.Copy(startindex, index, partial);
            }
           else 
            {
              string.Copy(startindex, partial);
            }

          if(!partial.IsEmpty())
            {
              switch(typeblock.Get(nblock))
                {            
                 case 1: if(IsModificatorActive(modificator, XDATETIME_FORMAT_ISO8601_BASIC))
                           {
                             partial.UnFormat(__L("%04d%02d%02d"), &year , &month, &day);
                           }
                          else 
                           {
                             partial.UnFormat(__L("%04d-%02d-%02d"), &year, &month, &day);
                           }
                          
                          break;     

                  case 2: if(IsModificatorActive(modificator, XDATETIME_FORMAT_ISO8601_BASIC))
                            {
                              if(IsModificatorActive(modificator, XDATETIME_FORMAT_TIMEWITHMILLISECONDS))
                                {
                                  if(IsModificatorActive(modificator, XDATETIME_FORMAT_TIMEWITHSECONDS))
                                    {
                                      partial.UnFormat(__L("%02d%02d%02d.%03d"), &hours ,&minutes ,&seconds, &milliseconds);
                                    }
                                   else  
                                    {
                                      partial.UnFormat(__L("%02d%02d%02d.%03d"), &hours ,&minutes, &milliseconds);
                                    }
                                }
                               else
                                {
                                  if(IsModificatorActive(modificator, XDATETIME_FORMAT_TIMEWITHSECONDS))
                                    {
                                      partial.UnFormat(__L("%02d%02d%02d"), &hours ,&minutes ,&seconds);
                                    }
                                   else  
                                    {
                                      partial.UnFormat(__L("%02d%02d"), &hours ,&minutes);
                                    }
                                }
                            }
                           else
                            {   
                              if(IsModificatorActive(modificator, XDATETIME_FORMAT_TIMEWITHMILLISECONDS))
                                {
                                  if(IsModificatorActive(modificator, XDATETIME_FORMAT_TIMEWITHSECONDS))
                                    {
                                      partial.UnFormat(__L("%02d:%02d:%02d.%03d"), &hours ,&minutes ,&seconds, &milliseconds);
                                    }
                                   else  
                                    {
                                      partial.UnFormat(__L("%02d:%02d:%02d.%03d"), &hours ,&minutes, &milliseconds);
                                    }
                                }
                               else
                                {
                                  if(IsModificatorActive(modificator, XDATETIME_FORMAT_TIMEWITHSECONDS))
                                    {
                                      partial.UnFormat(__L("%d:%d:%d"), &hours ,&minutes ,&seconds);
                                    }
                                   else  
                                    {
                                      partial.UnFormat(__L("%d:%d")   , &hours ,&minutes);
                                    }
                                }
                            }

                          break;                
                }

              nblock++;
            }

          startindex = index+1;
        }

      index++;
    }

  typeblock.DeleteAll();

  return IsValidDate();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::GetMonthString(XSTRING& string)
* @brief      GetMonthString
* @ingroup    XUTILS
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::GetMonthString(XSTRING& string)
{
  XSTRING strmonth;

  int index = (month-1);

  if(index>=12) return false;
  if(index<0)   return false;

  string.Empty();

  switch(index)
    {
      case  0 : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_MONTH_JANUARY);     break;
      case  1 : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_MONTH_FEBRUARY);    break;
      case  2 : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_MONTH_MARCH);       break;
      case  3 : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_MONTH_APRIL);       break;
      case  4 : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_MONTH_MAY);         break;
      case  5 : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_MONTH_JUNE);        break;
      case  6 : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_MONTH_JULY);        break;
      case  7 : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_MONTH_AUGUST);      break;
      case  8 : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_MONTH_SEPTEMBER);   break;
      case  9 : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_MONTH_OCTOBER);     break;
      case 10 : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_MONTH_NOVEMBER);    break;
      case 11 : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_MONTH_DECEMBER);    break;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::GetDayOfWeekString(XSTRING& string)
* @brief      GetDayOfWeekString
* @ingroup    XUTILS
*
* @param[in]  string :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::GetDayOfWeekString(XSTRING& string)
{
  XDATETIME_DAYWEEK dayweek = GetDayOfWeek();

  string.Empty();

  switch(dayweek)
    {
      case XDATETIME_DAYWEEK_SUNDAY    : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_DAYOFWEEK_SUNDAY);       break;
      case XDATETIME_DAYWEEK_MONDAY    : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_DAYOFWEEK_MONDAY);       break;
      case XDATETIME_DAYWEEK_TUESDAY   : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_DAYOFWEEK_TUESDAY);      break;
      case XDATETIME_DAYWEEK_WEDNESDAY : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_DAYOFWEEK_WEDNESDAY);    break;
      case XDATETIME_DAYWEEK_THURSDAY  : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_DAYOFWEEK_THURSDAY);     break;
      case XDATETIME_DAYWEEK_FRIDAY    : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_DAYOFWEEK_FRIDAY);       break;
      case XDATETIME_DAYWEEK_SATURDAY  : string = XT_L(XTRANSLATION_GEN_ID_XDATETIME_DAYOFWEEK_SATURDAY);     break;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int XDATETIME::GetDayOfYear()
* @brief      GetDayOfYear
* @ingroup    XUTILS
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int XDATETIME::GetDayOfYear() 
{
  int dayspermonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int dayofyear = 0;
  int i;

  // Check for leap year and update the number of days in February
  if((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) 
    {
      dayspermonth[2] = 29;
    }

  // Sum the days of the previous months
  for(i = 1; i < month; i++) 
    {
      dayofyear += dayspermonth[i];
    }

  // Add the current day
  dayofyear += day;

  return dayofyear;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int XDATETIME::GetWeekOfYear()
* @brief      GetWeekOfYear
* @ingroup    XUTILS
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int XDATETIME::GetWeekOfYear() 
{
  int dayofyear   = XDATETIME::GetDayOfYear();
  int daysperweek = 7;
  int weekofyear  = 0;
    
  // Calculate the week of the year
  weekofyear = (dayofyear - 1) / daysperweek + 1;

  return weekofyear;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XQWORD XDATETIME::GetEPOCHFormat()
* @brief      GetEPOCHFormat
* @ingroup    XUTILS
*
* @return     XQWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XQWORD XDATETIME::GetEPOCHFormat()
{
  Read();

  XDATETIME origin;

  origin.SetToZero();
  origin.SetDay(1);
  origin.SetMonth(1);
  origin.SetYear(1970);

  XQWORD seconds_origin = origin.GetSeconsFromDate(false);
  XQWORD seconds_actual = GetSeconsFromDate(false);

  return (seconds_actual - seconds_origin);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::SetToZero()
* @brief      SetToZero
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::SetToZero()
{
  Clean();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::SetDateToZero()
* @brief      SetDateToZero
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::SetDateToZero()
{
  year          = 0;
  month         = 0;
  day           = 0;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::SetHourToZero()
* @brief      SetHourToZero
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::SetHourToZero()
{
  hours         = 0;
  minutes       = 0;
  seconds       = 0;
  milliseconds  = 0;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XDATETIME::Set(XDATETIME* datetime)
* @brief      Set
* @ingroup    XUTILS
*
* @param[in]  datetime :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::Set(XDATETIME* datetime)
{
  if(!datetime) return false;

  SetYear(datetime->GetYear());
  SetMonth(datetime->GetMonth());
  SetDay(datetime->GetDay());

  SetHours(datetime->GetHours());
  SetMinutes(datetime->GetMinutes());
  SetSeconds(datetime->GetSeconds());
  SetMilliSeconds(datetime->GetMilliSeconds());

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XDATETIME::Set(XDATETIME& datetime)
* @brief      Set
* @ingroup    XUTILS
*
* @param[in]  datetime :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XDATETIME::Set(XDATETIME& datetime)
{
  Set(&datetime);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XDATETIME::IsModificatorActive(XDWORD modificator, XWORD tocheck)
* @brief      IsModificatorActive
* @ingroup    XUTILS
* 
* @param[in]  modificator : 
* @param[in]  tocheck : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XDATETIME::IsModificatorActive(XDWORD modificator, XDWORD tocheck)
{
  if((XDWORD)(modificator & tocheck) == tocheck)
    {
      return true;
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         long XDATETIME::GetNDaysInJulian(int day, int month, int year)
* @brief      GetNDaysInJulian
* @ingroup    XUTILS
*
* @param[in]  day :
* @param[in]  month :
* @param[in]  year :
*
* @return     long :
*
* --------------------------------------------------------------------------------------------------------------------*/
long XDATETIME::GetNDaysInJulian(int day, int month, int year)
{
  this->day   = day;
  this->month = month;
  this->year  = year;

  return GetNDaysInJulian();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         long XDATETIME::GetNDaysInGregorian(int day, int month, int year)
* @brief      GetNDaysInGregorian
* @ingroup    XUTILS
*
* @param[in]  day :
* @param[in]  month :
* @param[in]  year :
*
* @return     long :
*
* --------------------------------------------------------------------------------------------------------------------*/
long XDATETIME::GetNDaysInGregorian(int day, int month, int year)
{
  this->day   = day;
  this->month = month;
  this->year  = year;

  return GetNDaysInGregorian();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XDATETIME::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XDATETIME::Clean()
{
  islocal       = false;

  year          = 0;
  month         = 0;
  day           = 0;

  hours         = 0;
  minutes       = 0;
  seconds       = 0;
  milliseconds  = 0;
}
