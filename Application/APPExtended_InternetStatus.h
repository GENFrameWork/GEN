/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       APPExtended_InternetStatus.h
* 
* @class      APPEXTENDED_INTERNETSTATUS
* @brief      Application Extended Internet Status class
* @ingroup    APPLICATION
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

#ifndef _APPEXTENDED_INTERNETSTATUS_H_
#define _APPEXTENDED_INTERNETSTATUS_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOWebServer_ResolveEndPoint.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class APPCFG;
class XCONSOLE;
class APPINTERNETSERVICES;


class APPEXTENDED_INTERNETSTATUS : public DIOWEBSERVER_RESOLVEENDPOINT
{
  public:
                                        APPEXTENDED_INTERNETSTATUS                  (APPCFG* cfg);
    virtual                            ~APPEXTENDED_INTERNETSTATUS                  ();

    APPINTERNETSERVICES*                GetAPPInternetServices                      ();

    XSTRING*                            GetLocalIP                                  ();

    bool                                GetHaveInternetConnection                   ();
    XSTRING*                            GetPublicIP                                 ();    
   
    bool                                Update                                      (); 
    bool                                Show                                        (XCONSOLE* console);

    bool                                Serialize                                   ();    
    bool                                Deserialize                                 ();
                                                               
  private:

    void                                Clean                                      ();

    APPCFG*                             cfg;
    APPINTERNETSERVICES*                appinternetservices;

    XSTRING                             localIP;

    bool                                haveinternetconnection;
    XSTRING                             publicIP;    
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

