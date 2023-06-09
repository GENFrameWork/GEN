/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOScraperWebUserAgentID.cpp
*
* @class      DIOSCRAPERWEBUSERAGENTID
* @brief      Data Input/Output Scraper Web User Agent ID class
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XThread.h"
#include "DIOScraperWebUserAgentID.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/




/*--------------------------------------------------------------------------------------------------------------------*/
/* DIOUSERAGENTID_RESULT                                                                                              */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOUSERAGENTID_RESULT::DIOUSERAGENTID_RESULT()
* @brief      Constructor
* @ingroup    DATAIO
*
* @return     Does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOUSERAGENTID_RESULT::DIOUSERAGENTID_RESULT()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOUSERAGENTID_RESULT::~ DIOUSERAGENTID_RESULT()
* @brief      IOUSERAGENTID_RESULT
* @ingroup    DATAIO
*
* @return     DIOUSERAGENTID_RESULT::~ : 
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOUSERAGENTID_RESULT::~ DIOUSERAGENTID_RESULT()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOUSERAGENTID_RESULT::GetBrowser()
* @brief      GetBrowser
* @ingroup    DATAIO
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOUSERAGENTID_RESULT::GetBrowser()
{
  return &browser;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* DIOUSERAGENTID_RESULT::GetSO()
* @brief      GetSO
* @ingroup    DATAIO
*
* @return     XSTRING* : 
*
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* DIOUSERAGENTID_RESULT::GetSO()
{
  return &systemoperative;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOUSERAGENTID_RESULT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @return     void : does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOUSERAGENTID_RESULT::Clean()
{

}


/*--------------------------------------------------------------------------------------------------------------------*/
/* DIOSCRAPERWEBUSERAGENTID                                                                                           */
/*--------------------------------------------------------------------------------------------------------------------*/




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSCRAPERWEBUSERAGENTID::DIOSCRAPERWEBUSERAGENTID()
* @brief      Constructor
* @ingroup    DATAIO
*
* @return     Does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSCRAPERWEBUSERAGENTID::DIOSCRAPERWEBUSERAGENTID() : DIOSCRAPERWEB()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSCRAPERWEBUSERAGENTID::~DIOSCRAPERWEBUSERAGENTID()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @return     Does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOSCRAPERWEBUSERAGENTID::~DIOSCRAPERWEBUSERAGENTID()
{
  Clean();
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSCRAPERWEBUSERAGENTID::ChangeURL(XCHAR* maskurl, DIOURL& url)
* @brief      ChangeURL
* @ingroup    DATAIO
*
* @param[in]  maskurl : 
* @param[in]  url : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSCRAPERWEBUSERAGENTID::ChangeURL(XCHAR* maskurl, DIOURL& url)
{
  DIOURL urluseragent;

  urluseragent = useragent.Get();
  urluseragent.EncodeUnsafeChars();

  url.Format(maskurl, urluseragent.Get());

  //url.EncodeUnsafeChars();

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSCRAPERWEBUSERAGENTID::Get(XCHAR* useragent, XSTRING& browser, XSTRING& systemoperative, int timeout, XSTRING* localIP, bool usecache)
* @brief      Get
* @ingroup    DATAIO
*
* @param[in]  useragent : 
* @param[in]  browser : 
* @param[in]  systemoperative : 
* @param[in]  timeout : 
* @param[in]  localIP : 
* @param[in]  usecache : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSCRAPERWEBUSERAGENTID::Get(XCHAR* useragent, XSTRING& browser, XSTRING& systemoperative, int timeout, XSTRING* localIP, bool usecache)
{
  bool status = false;
  if(xmutexdo) xmutexdo->Lock();

  this->useragent = useragent;
  if(this->useragent.IsEmpty())
    {
      if(xmutexdo) xmutexdo->UnLock();
      return false;
    }

  DIOUSERAGENTID_RESULT* useragentresult;
  if(usecache)
    {
      useragentresult= (DIOUSERAGENTID_RESULT*)cache->Get(this->useragent);
      if(useragentresult)
        {
          browser         = (*useragentresult->GetBrowser());
          systemoperative = (*useragentresult->GetSO());

          if(xmutexdo) xmutexdo->UnLock();
          return true;
        }
    }

  if(Load(DIOSCRAPERWEBUSERAGENTID_NAMEFILE))
    {
      if(Do(DIOSCRAPERWEBUSERAGENTID_NAMESERVICE, timeout, localIP))
        {
          browser         = GetValue(__L("BROWSER"));
          systemoperative = GetValue(__L("SO"));

          if(usecache)
            {
              useragentresult = new DIOUSERAGENTID_RESULT();
              if(useragentresult)
                {
                  useragentresult->GetBrowser()->Set(browser);
                  useragentresult->GetSO()->Set(systemoperative);

                  cache->Add(this->useragent, useragentresult);
                }
            }

          status = true;
        }
    }

  if(xmutexdo) xmutexdo->UnLock();

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSCRAPERWEBUSERAGENTID::Get(XSTRING& useragent, XSTRING& browser, XSTRING& systemoperative, int timeoutforurl, XSTRING* localIP, bool usecache)
* @brief      Get
* @ingroup    DATAIO
*
* @param[in]  useragent : 
* @param[in]  browser : 
* @param[in]  systemoperative : 
* @param[in]  timeoutforurl : 
* @param[in]  localIP : 
* @param[in]  usecache : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOSCRAPERWEBUSERAGENTID::Get(XSTRING& useragent, XSTRING& browser, XSTRING& systemoperative, int timeoutforurl, XSTRING* localIP, bool usecache)
{
  return Get(useragent.Get(), browser, systemoperative, timeoutforurl, localIP, usecache);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSCRAPERWEBUSERAGENTID::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @return     void : does not return anything. 
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOSCRAPERWEBUSERAGENTID::Clean()
{

}

