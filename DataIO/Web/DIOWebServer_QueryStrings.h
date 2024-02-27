/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOWebServer_QueryStrings.h
* 
* @class      DIOWEBSERVER_QUERYSTRINGS
* @brief      Data Input/Output Web Server Query Strings Event class
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

#ifndef _DIOWEBSERVER_QUERYSTRINGS_H_
#define _DIOWEBSERVER_QUERYSTRINGS_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XString.h"
#include "XMap.h"

#include "DIOURL.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class DIOWEBSERVER_QUERYSTRINGS
{
  public:
                                    DIOWEBSERVER_QUERYSTRINGS         ();
    virtual                        ~DIOWEBSERVER_QUERYSTRINGS         ();

    int                             GetNParams                        ();
    XMAP<XSTRING*, XSTRING*>*       GetParams                         ();

    bool                            AddParam                          (XSTRING& ID, XSTRING& value);
    bool                            AddParam                          (XCHAR* ID  , XSTRING& value);
    bool                            AddParam                          (XSTRING& ID, XCHAR* value);
    bool                            AddParam                          (XCHAR* ID  , XCHAR* value);
    bool                            AddParam                          (XCHAR* ID  , int value);

    XSTRING*                        GetParam                          (XSTRING& ID);
    XSTRING*                        GetParam                          (XCHAR* ID);

    XSTRING*                        GetParamID                        (int index);
    XSTRING*                        GetParamValue                     (int index);

    bool                            DelParam                          (XSTRING& ID);
    bool                            DelParam                          (XCHAR* ID);

    bool                            DelAllParam                       ();

    bool                            GetAllParam                       (XSTRING& allparam);

    int                             GetParamsFromURL                  (DIOURL& url);
    int                             GetParamsFromURL                  (XSTRING& url);
    int                             GetParamsFromURL                  (XCHAR* url);

    bool                            DeleteParamsFromURL               (XSTRING& url);

    bool                            CreateURLFromParams               (XSTRING& urlpart);

  protected:

    XMAP<XSTRING*, XSTRING*>        params;

  private:

    void                            Clean                             ();
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

