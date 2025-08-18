/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTLSMessage.h
* 
* @class      DIOSTREAMTLSMESSAGE
* @brief      Data Input/Output Stream TLS (Transport Layer Security) Messages class
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

#ifndef _DIOSTREAMTLSMESSAGEEXTENSION_H_
#define _DIOSTREAMTLSMESSAGEEXTENSION_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOStreamTLSMessages.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_SNI                   0x0000 // 1.0+ SNI 
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_SUPPORTEDGROUPS	      0x000a // 1.2+ Curve
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_ECPOINTFORMATS	      0x000b // 1.2	ECP Point Formats
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_SIGNATUREALGORITHMS	  0x000d // 1.2+
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_ALPN	                0x0010 // 1.2+	Protocol app
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_EXTENDEDMASTERSECRET	0x0017 // 1.2	
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_SESSIONTICKET	        0x0023 // 1.2	Ticket
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_SUPPORTEDVERSIONS	    0x002b // 1.3	TLS Versions
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_PSKKEYEXCHANGEMODES	  0x002d // 1.3	Mode PSK
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_KEYSHARE	            0x0033 // 1.3	Public Key                                                                    


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class DIOSTREAMTLS_MSG_EXTENSION : public DIOSTREAMTLS_MSG_INTERFACE
{
  public:
                                                          DIOSTREAMTLS_MSG_EXTENSION                        ();
    virtual                                              ~DIOSTREAMTLS_MSG_EXTENSION                        ();
                                                          
    XWORD                                                 GetType                                           ();                                            
    void                                                  SetType                                           (XWORD type);
                                                          
    XWORD                                                 GetLength                                         ();                                          
    void                                                  SetLength                                         (XWORD length);
    
    XBYTE*                                                GetExtensionData                                  ();    
    bool                                                  SetExtensionData                                  (XBYTE* extensiondata, XWORD length);  
     
  private:

    void                                                  Clean                                             ();
    
    XWORD                                                 type;
    XWORD                                                 length;    
    XBYTE*                                                extension_data;
};

class DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME : public DIOSTREAMTLS_MSG_INTERFACE
{
  public:
                                                          DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME         ();
    virtual                                              ~DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME         ();

    XWORD                                                 Name_GetType                                      ();                                            
    void                                                  Name_SetType                                      (XWORD name_type);
                                                          
    XWORD                                                 Name_GetLength                                    ();                                          
    void                                                  Name_SetLength                                    (XWORD name_length);

    XSTRING*                                              Name_GetHost                                      (); 

    bool                                                  CopyTo                                            (DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* servername);  
    bool                                                  CopyFrom                                          (DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* servername);  

    bool                                                  SetToBuffer                                       (XBUFFER& buffer);                                           
    bool                                                  GetFromBuffer                                     (XBUFFER& buffer);

  private:

    void                                                  Clean                                             ();   

    XBYTE                                                 name_type;                                  
    XWORD                                                 name_length;     
    XSTRING                                               hostname;                                
};


class DIOSTREAMTLS_MSG_EXTENSION_SNI : public DIOSTREAMTLS_MSG_INTERFACE
{
  public:
                                                          DIOSTREAMTLS_MSG_EXTENSION_SNI                    ();
    virtual                                              ~DIOSTREAMTLS_MSG_EXTENSION_SNI                    ();

    XWORD                                                 List_GetLength                                    ();                                          
    void                                                  List_SetLength                                    (XWORD list_length);

    XVECTOR<DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME*>*  List_Get                                          ();
    bool                                                  List_Add                                          (DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* servername);
    bool                                                  List_DeleteAll                                    ();

    bool                                                  CopyTo                                            (DIOSTREAMTLS_MSG_EXTENSION_SNI* servername);  
    bool                                                  CopyFrom                                          (DIOSTREAMTLS_MSG_EXTENSION_SNI* servername);  
                                                       
    bool                                                  SetToBuffer                                       (XBUFFER& buffer);                                           
    bool                                                  GetFromBuffer                                     (XBUFFER& buffer);
                                               
  private:

    void                                                  Clean                                             ();   

    XWORD                                                 list_length;
    XVECTOR<DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME*>   list;
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

