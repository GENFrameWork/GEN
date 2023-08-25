/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XTranslation_GEN.h
*
* @class      XTRANSLATION_GEN
* @brief      eXtension Translation GEN (sencences within the framework) fram class
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

#ifndef _XTRANSLATION_GEN_H_
#define _XTRANSLATION_GEN_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XString.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

enum XTRANSLATION_GEN_ID
{
  XTRANSLATION_GEN_ID_UNKWON                                                  = 0 ,

  XTRANSLATION_GEN_ID_XDATETIME_MONTH_JANUARY                                     ,
  XTRANSLATION_GEN_ID_XDATETIME_MONTH_FEBRUARY                                    ,
  XTRANSLATION_GEN_ID_XDATETIME_MONTH_MARCH                                       ,
  XTRANSLATION_GEN_ID_XDATETIME_MONTH_APRIL                                       ,
  XTRANSLATION_GEN_ID_XDATETIME_MONTH_MAY                                         ,
  XTRANSLATION_GEN_ID_XDATETIME_MONTH_JUNE                                        ,
  XTRANSLATION_GEN_ID_XDATETIME_MONTH_JULY                                        ,
  XTRANSLATION_GEN_ID_XDATETIME_MONTH_AUGUST                                      ,
  XTRANSLATION_GEN_ID_XDATETIME_MONTH_SEPTEMBER                                   ,
  XTRANSLATION_GEN_ID_XDATETIME_MONTH_OCTOBER                                     ,
  XTRANSLATION_GEN_ID_XDATETIME_MONTH_NOVEMBER                                    ,
  XTRANSLATION_GEN_ID_XDATETIME_MONTH_DECEMBER                                    ,
  XTRANSLATION_GEN_ID_XDATETIME_MONTH_PRESEPARATOR                                ,

  XTRANSLATION_GEN_ID_XDATETIME_DAYOFWEEK_SUNDAY                                  ,
  XTRANSLATION_GEN_ID_XDATETIME_DAYOFWEEK_MONDAY                                  ,
  XTRANSLATION_GEN_ID_XDATETIME_DAYOFWEEK_TUESDAY                                 ,
  XTRANSLATION_GEN_ID_XDATETIME_DAYOFWEEK_WEDNESDAY                               ,
  XTRANSLATION_GEN_ID_XDATETIME_DAYOFWEEK_THURSDAY                                ,
  XTRANSLATION_GEN_ID_XDATETIME_DAYOFWEEK_FRIDAY                                  ,
  XTRANSLATION_GEN_ID_XDATETIME_DAYOFWEEK_SATURDAY                                ,

  XTRANSLATION_GEN_ID_XTIMER_XX_YEARS                                             ,
  XTRANSLATION_GEN_ID_XTIMER_ONE_YEAR                                             ,
  XTRANSLATION_GEN_ID_XTIMER_XX_MONTHS                                            ,
  XTRANSLATION_GEN_ID_XTIMER_ONE_MONTH                                            ,
  XTRANSLATION_GEN_ID_XTIMER_XX_DAYS                                              ,
  XTRANSLATION_GEN_ID_XTIMER_ONE_DAY                                              ,
  XTRANSLATION_GEN_ID_XTIMER_XX_HOURS                                             ,
  XTRANSLATION_GEN_ID_XTIMER_ONE_HOUR                                             ,
  XTRANSLATION_GEN_ID_XTIMER_XX_MINUTES                                           ,
  XTRANSLATION_GEN_ID_XTIMER_ONE_MINUTE                                           ,
  XTRANSLATION_GEN_ID_XTIMER_XX_SECONDS                                           ,
  XTRANSLATION_GEN_ID_XTIMER_ONE_SECOND                                           ,
  XTRANSLATION_GEN_ID_XTIMER_ZERO_SECONDS                                         ,

  XTRANSLATION_GEN_ID_APPLOG_INITMSG                                              ,
  XTRANSLATION_GEN_ID_APPLOG_TOTALMEMORY                                          ,  

  XTRANSLATION_GEN_ID_APPCONSOLE_EXIT_NOTINFO                                     ,
  XTRANSLATION_GEN_ID_APPCONSOLE_EXIT_BY_SERIOUSERROR                             ,
  XTRANSLATION_GEN_ID_APPCONSOLE_EXIT_BY_UPDATE                                   ,
  XTRANSLATION_GEN_ID_APPCONSOLE_EXIT_BY_APPLICATION                              ,
  XTRANSLATION_GEN_ID_APPCONSOLE_EXIT_BY_USER                                     ,
  XTRANSLATION_GEN_ID_APPCONSOLE_EXIT_BY_SHUTDOWN                                 ,
  XTRANSLATION_GEN_ID_APPCONSOLE_EXIT_BY_INVALIDLICENSE                           ,
  XTRANSLATION_GEN_ID_APPCONSOLE_EXIT_BY_EXPIREDLICENSE                           ,

};


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XTRANSLATION_GEN_SENTENCE
{
  public:

                                          XTRANSLATION_GEN_SENTENCE          ();
    virtual                              ~XTRANSLATION_GEN_SENTENCE          ();

    void                                  Clean                              ();

    XDWORD                                ID;
    XDWORD                                codelanguage;
    XCHAR*                                sentence;
    XDWORD                                fixed;
};


class XTRANSLATION_GEN
{
  public:

    static bool                           GetIsInstanced                      ();
    static XTRANSLATION_GEN&              GetInstance                         ();
    static bool                           DelInstance                         ();

    bool                                  IsLanguageAvailable                 (XDWORD code);

    bool                                  Sentence_Add                        (XDWORD ID, XDWORD codelanguage, XCHAR* sentence, XDWORD fixed = 0);
    XTRANSLATION_GEN_SENTENCE*            Sentence_Get                        (XDWORD ID, XDWORD codelanguage);

    bool                                  Sentences_AddAll                    ();
    bool                                  Sentences_DeleteAll                 ();

    bool                                  Sentences_AddToTranslation          (XDWORD codelanguage);

  private:
                                          XTRANSLATION_GEN                    ();
                                          XTRANSLATION_GEN                    (XTRANSLATION_GEN const&);        // Don't implement
    virtual                              ~XTRANSLATION_GEN                    ();

    void                                  operator =                          (XTRANSLATION_GEN const&);        // Don't implement

    void                                  Clean                               ();

    static XTRANSLATION_GEN*              instance;
    XVECTOR<XDWORD>                       languageavailables;
    XVECTOR<XTRANSLATION_GEN_SENTENCE*>   sentences;
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif

