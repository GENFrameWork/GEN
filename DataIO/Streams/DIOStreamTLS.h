/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTLS.h
* 
* @class      DIOSTREAMTLS
* @brief      Data Input/Output Stream TLS (Transport Layer Security) class
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

#ifndef _DIOSTREAMTLS_H_
#define _DIOSTREAMTLS_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XBuffer.h"

#include "DIOStreamTLSMessages.h"

#include "DIOIP.h"
#include "DIOStreamTCPIP.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS
     

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


class DIOSTREAMTLSCONFIG;


class DIOSTREAMTLS : public DIOSTREAM
{
  public:
                                            DIOSTREAMTLS                  (DIOSTREAMTLSCONFIG* config);
    virtual                                ~DIOSTREAMTLS                  ();

    DIOSTREAMCONFIG*                        GetConfig                     ();
    bool                                    SetConfig                     (DIOSTREAMCONFIG* config);  

    bool                                    Open                          ();    

    XDWORD                                  Read                          (XBYTE* buffer, XDWORD size);
    XDWORD                                  Write                         (XBYTE* buffer, XDWORD size);

    bool                                    Disconnect                    ();

    bool                                    Close                         (); 

    DIOSTREAMTCPIP*                         GetDIOStreamTCPIP             ();

    DIOSTREAMSTATUS                         GetStatus                     ();  

    bool                                    HandShake_Client_Hello        ();

    bool                                    GenerateRandom                (XBYTE* random);
    bool                                    GenerateSessionID             (XBYTE* sessionID, XBYTE sessionIDlength);
    
  private:

    void                                    Clean                         ();

    DIOSTREAMTLSCONFIG*                     config;
    DIOSTREAMTCPIP*                         diostream;

    XBYTE                                   random[DIOSTREAMTLS_MSG_RANDOM_SIZE];
    XBYTE                                   sessionID[DIOSTREAMTLS_MSG_SESSIONID_SIZE];
};


#pragma endregion


#endif

