/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOScraperWebGeolocationIP.cpp
*
* @class      DIOSCRAPERWEBGEOLOCATIONIP
* @brief      Data Input/Output Scraper Web Geolocation IP class
* @ingroup    DATAIO
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

#include "XThread.h"

#include "DIOScraperWebGeolocationIP.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/
/*  DIOGEOLOCATIONIP_RESULT                                                                                           */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOGEOLOCATIONIP_RESULT::DIOGEOLOCATIONIP_RESULT()
* @brief      Constructor
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOGEOLOCATIONIP_RESULT::DIOGEOLOCATIONIP_RESULT(): DIOSCRAPERWEBCACHE_RESULT()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOGEOLOCATIONIP_RESULT::~DIOGEOLOCATIONIP_RESULT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOGEOLOCATIONIP_RESULT::~DIOGEOLOCATIONIP_RESULT()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* DIOGEOLOCATIONIP_RESULT::GetCountry()
* @brief      GetCountry
* @ingroup    DATAIO
*
* @return     XCHAR* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XCHAR* DIOGEOLOCATIONIP_RESULT::GetCountry()
{
  return country.Get();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* DIOGEOLOCATIONIP_RESULT::GetState()
* @brief      GetState
* @ingroup    DATAIO
*
* @return     XCHAR* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XCHAR* DIOGEOLOCATIONIP_RESULT::GetState()
{
  return state.Get();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* DIOGEOLOCATIONIP_RESULT::GetCity()
* @brief      GetCity
* @ingroup    DATAIO
*
* @return     XCHAR* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XCHAR* DIOGEOLOCATIONIP_RESULT::GetCity()
{
  return city.Get();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* DIOGEOLOCATIONIP_RESULT::GetISP()
* @brief      GetISP
* @ingroup    DATAIO
*
* @return     XCHAR* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XCHAR* DIOGEOLOCATIONIP_RESULT::GetISP()
{
  return ISP.Get();
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* DIOGEOLOCATIONIP_RESULT::GetOrganization()
* @brief      GetOrganization
* @ingroup    DATAIO
*
* @return     XCHAR* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XCHAR* DIOGEOLOCATIONIP_RESULT::GetOrganization()
{
  return organization.Get();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         float DIOGEOLOCATIONIP_RESULT::GetLatitude()
* @brief      GetLatitude
* @ingroup    DATAIO
*
* @return     float :
*
*---------------------------------------------------------------------------------------------------------------------*/
float DIOGEOLOCATIONIP_RESULT::GetLatitude()
{
  return latitude;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         float DIOGEOLOCATIONIP_RESULT::GetLongitude()
* @brief      GetLongitude
* @ingroup    DATAIO
*
* @return     float :
*
*---------------------------------------------------------------------------------------------------------------------*/
float DIOGEOLOCATIONIP_RESULT::GetLongitude()
{
  return longitude;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOGEOLOCATIONIP_RESULT::IsEmpty()
* @brief      IsEmpty
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOGEOLOCATIONIP_RESULT::IsEmpty()
{
  if(country.IsEmpty() || state.IsEmpty() || city.IsEmpty()) return true;

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOGEOLOCATIONIP_RESULT::CopyFrom(DIOSCRAPERWEBCACHE_RESULT* result)
* @brief      CopyFrom
* @ingroup    DATAIO
*
* @param[in]  result :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOGEOLOCATIONIP_RESULT::CopyFrom(DIOSCRAPERWEBCACHE_RESULT* result)
{
  if(!result)   return false;

  DIOGEOLOCATIONIP_RESULT* geolocationIP = (DIOGEOLOCATIONIP_RESULT*)result;

  if(geolocationIP->IsEmpty()) return false;

  country       = geolocationIP->country;
  state         = geolocationIP->state;
  city          = geolocationIP->city;
  ISP           = geolocationIP->ISP;
  organization  = geolocationIP->organization;

  latitude      = geolocationIP->latitude;
  longitude     = geolocationIP->longitude;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOGEOLOCATIONIP_RESULT::CopyTo(DIOSCRAPERWEBCACHE_RESULT* result)
* @brief      CopyTo
* @ingroup    DATAIO
*
* @param[in]  result :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOGEOLOCATIONIP_RESULT::CopyTo(DIOSCRAPERWEBCACHE_RESULT* result)
{
  if(!result)   return false;
  if(IsEmpty()) return false;

  DIOGEOLOCATIONIP_RESULT* geolocationIP = (DIOGEOLOCATIONIP_RESULT*)result;

  geolocationIP->country       = country;
  geolocationIP->state         = state;
  geolocationIP->city          = city;
  geolocationIP->ISP           = ISP;
  geolocationIP->organization  = organization;

  geolocationIP->latitude      = latitude;
  geolocationIP->longitude     = longitude;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOGEOLOCATIONIP_RESULT::Set(XSTRING& country, XSTRING& state, XSTRING& city, XSTRING& ISP, XSTRING& organization)
* @brief      Set
* @ingroup    DATAIO
*
* @param[in]  country :
* @param[in]  state :
* @param[in]  city :
* @param[in]  ISP :
* @param[in]  organization :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOGEOLOCATIONIP_RESULT::Set(XSTRING& country, XSTRING& state, XSTRING& city, XSTRING& ISP, XSTRING& organization)
{
  return Set(country.Get(), state.Get(), city.Get(), ISP.Get(), organization.Get());
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOGEOLOCATIONIP_RESULT::Set(XCHAR* country, XCHAR* state, XCHAR* city, XCHAR* ISP, XCHAR* organization)
* @brief      Set
* @ingroup    DATAIO
*
* @param[in]  country :
* @param[in]  state :
* @param[in]  city :
* @param[in]  ISP :
* @param[in]  organization :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOGEOLOCATIONIP_RESULT::Set(XCHAR* country, XCHAR* state, XCHAR* city, XCHAR* ISP, XCHAR* organization)
{
  if(country)       this->country       = country;
  if(state)         this->state         = state;
  if(city)          this->city          = city;
  if(ISP)           this->ISP           = ISP;
  if(organization)  this->organization  = organization;

  XCHAR character[3] = { 0x09, 0x0A, 0x0D };

  for(int c=0;c<3;c++)
    {
      this->country.DeleteCharacter(character[c],XSTRINGCONTEXT_ALLSTRING);
      this->state.DeleteCharacter(character[c],XSTRINGCONTEXT_ALLSTRING);
      this->city.DeleteCharacter(character[c],XSTRINGCONTEXT_ALLSTRING);
      this->ISP.DeleteCharacter(character[c],XSTRINGCONTEXT_ALLSTRING);
      this->organization.DeleteCharacter(character[c],XSTRINGCONTEXT_ALLSTRING);
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOGEOLOCATIONIP_RESULT::Set(float latitude, float longitude)
* @brief      Set
* @ingroup    DATAIO
*
* @param[in]  latitude :
* @param[in]  longitude :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOGEOLOCATIONIP_RESULT::Set(float latitude, float longitude)
{
  this->latitude  = latitude;
  this->longitude = longitude;

  return true;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOGEOLOCATIONIP_RESULT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void DIOGEOLOCATIONIP_RESULT::Clean()
{
  latitude  = 0;
  longitude = 0;
}



/*--------------------------------------------------------------------------------------------------------------------*/
/*  DIOSCRAPERWEBGEOLOCATIONIP                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSCRAPERWEBGEOLOCATIONIP::DIOSCRAPERWEBGEOLOCATIONIP(DIOWEBCLIENT* webclient)
* @brief      Constructor
* @ingroup    DATAIO
*
* @param[in]  webclient :
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOSCRAPERWEBGEOLOCATIONIP::DIOSCRAPERWEBGEOLOCATIONIP(DIOWEBCLIENT* webclient): DIOSCRAPERWEB()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOSCRAPERWEBGEOLOCATIONIP::~DIOSCRAPERWEBGEOLOCATIONIP()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
DIOSCRAPERWEBGEOLOCATIONIP::~DIOSCRAPERWEBGEOLOCATIONIP()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSCRAPERWEBGEOLOCATIONIP::ChangeURL(XCHAR* maskurl, DIOURL& url)
* @brief      ChangeURL
* @ingroup    DATAIO
*
* @param[in]  maskurl :
* @param[in]  url :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOSCRAPERWEBGEOLOCATIONIP::ChangeURL(XCHAR* maskurl, DIOURL& url)
{
  XSTRING IPstring;

  IP.GetXString(IPstring);

  url.Format(maskurl, IPstring.Get());

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSCRAPERWEBGEOLOCATIONIP::Get(XCHAR* IP, DIOGEOLOCATIONIP& geolocationIP, int timeoutforurl, XSTRING* localIP, bool usecache)
* @brief      Get
* @ingroup    DATAIO
*
* @param[in]  IP :
* @param[in]  geolocationIPresult :
* @param[in]  timeoutforurl :
* @param[in]  localIP :
* @param[in]  usecache :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOSCRAPERWEBGEOLOCATIONIP::Get(XCHAR* IP, DIOGEOLOCATIONIP_RESULT& geolocationIPresult, int timeoutforurl, XSTRING* localIP, bool usecache)
{
  DIOIP _IP;

  _IP.Set(IP);

  return Get(_IP, geolocationIPresult, timeoutforurl, localIP, usecache);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSCRAPERWEBGEOLOCATIONIP::Get(XSTRING& IP, DIOGEOLOCATIONIP& geolocationIP, int timeoutforurl, XSTRING* localIP, bool usecache)
* @brief      Get
* @ingroup    DATAIO
*
* @param[in]  IP :
* @param[in]  geolocationIPresult :
* @param[in]  timeoutforurl :
* @param[in]  localIP :
* @param[in]  usecache :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOSCRAPERWEBGEOLOCATIONIP::Get(XSTRING& IP, DIOGEOLOCATIONIP_RESULT& geolocationIPresult, int timeoutforurl, XSTRING* localIP, bool usecache)
{
  return Get(IP.Get(), geolocationIPresult, timeoutforurl, localIP, usecache);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOSCRAPERWEBGEOLOCATIONIP::Get(DIOIP& IP, DIOGEOLOCATIONIP& geolocationIP, int timeoutforurl, XSTRING* localIP, bool usecache)
* @brief      Get
* @ingroup    DATAIO
*
* @param[in]  IP :
* @param[in]  geolocationIPresult :
* @param[in]  timeoutforurl :
* @param[in]  localIP :
* @param[in]  usecache :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool DIOSCRAPERWEBGEOLOCATIONIP::Get(DIOIP& IP, DIOGEOLOCATIONIP_RESULT& geolocationIPresult, int timeoutforurl, XSTRING* localIP, bool usecache)
{
  bool status = false;

  if(xmutexdo) xmutexdo->Lock();

  this->IP.Set(IP.Get());

  if(usecache)
    {
      XSTRING IPstring;

      IP.GetXString(IPstring);

      DIOGEOLOCATIONIP_RESULT* geoIPresult = (DIOGEOLOCATIONIP_RESULT*)cache->Get(IPstring);
      if(geoIPresult)
        {
          geolocationIPresult.CopyFrom(geoIPresult);

          if(xmutexdo) xmutexdo->UnLock();
          return true;
        }
    }

  if(Load(DIOSCRAPERWEBGEOLOCATIONIP_NAMEFILE))
    {
      if(Do(DIOSCRAPERWEBGEOLOCATIONIP_NAMESERVICE, timeoutforurl, localIP))
       {
          XSTRING           country;
          XSTRING           state;
          XSTRING           city;
          XSTRING           ISP;
          XSTRING           organization;
          double            latitude  = 0;
          double            longitude = 0;
          XSTRING           string;

          country       = GetValue(__L("COUNTRY"));
          state         = GetValue(__L("STATE"));
          city          = GetValue(__L("CITY"));
          ISP           = GetValue(__L("ISP"));
          organization  = GetValue(__L("ORGANIZATION"));

          string        = GetValue(__L("LATITUDE"));
          if(!string.IsEmpty()) string.UnFormat(__L("%f"), &latitude);

          string  = GetValue(__L("LONGITUDE"));
          if(!string.IsEmpty()) string.UnFormat(__L("%f"), &longitude);

          geolocationIPresult.Set(country, state, city, ISP, organization);
          geolocationIPresult.Set((float)latitude, (float)longitude);

          if(!geolocationIPresult.IsEmpty())
            {
              if(usecache)
                {
                  DIOGEOLOCATIONIP_RESULT* geoIPresult = new DIOGEOLOCATIONIP_RESULT();
                  if(geoIPresult)
                    {
                      XSTRING IPstring;

                      IP.GetXString(IPstring);

                      geoIPresult->CopyFrom(&geolocationIPresult);
                      cache->Add(IPstring, geoIPresult);
                    }
                }

              status = true;
            }
        }
    }

  if(xmutexdo) xmutexdo->UnLock();

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOSCRAPERWEBGEOLOCATIONIP::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void DIOSCRAPERWEBGEOLOCATIONIP::Clean()
{

}


