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

#ifndef _DIOSTREAMTLSMESSAGE_H_
#define _DIOSTREAMTLSMESSAGE_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XBuffer.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#define DIOSTREAMTLS_MSG_VERSION_SSL_3_0                                      0x0300    // SSL v3.0
#define DIOSTREAMTLS_MSG_VERSION_TLS_1_0                                      0x0301    // TLS v1.0 
#define DIOSTREAMTLS_MSG_VERSION_TLS_1_1                                      0x0302    // TLS v1.1 
#define DIOSTREAMTLS_MSG_VERSION_TLS_1_2                                      0x0303    // TLS v1.2 
#define DIOSTREAMTLS_MSG_VERSION_TLS_1_3                                      0x0304    // TLS v1.3 

#define DIOSTREAMTLS_MSG_RANDOM_SIZE                                          32
#define DIOSTREAMTLS_MSG_SESSIONID_SIZE                                       32

enum DIOSTREAMTLS_CONTENTTYPE
{
   DIOSTREAMTLS_MSG_CONTENTTYPE_CHANGE_CIPHER_SPEC                            =  0x14   , 
   DIOSTREAMTLS_MSG_CONTENTTYPE_ALERT                                         =  0x15   , 
   DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE                                     =  0x16   ,
   DIOSTREAMTLS_MSG_CONTENTTYPE_APPLICATION_DATA                              =  0x17   , 
   DIOSTREAMTLS_MSG_CONTENTTYPE_TLS_1_3_HEARTBEAT                             =  0x18   , 
};


enum DIOSTREAMTLS_CONTENTTYPE_HANDSHAKETYPE
{
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKETYPE_HELLO_REQUEST                    =    0  , 
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKETYPE_CLIENT_HELLO                     =    1  , 
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKETYPE_SERVER_HELLO                     =    2  ,
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKETYPE_CERTIFICATE                      =   11  , 
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKETYPE_SERVER_KEY_EXCHANGE              =   12  ,
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKETYPE_CERTIFICATE_REQUEST              =   13  , 
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKETYPE_SERVER_HELLO_DONE                =   14  ,
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKETYPE_CERTIFICATE_VERIFY               =   15  , 
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKETYPE_CLIENT_KEY_EXCHANGE              =   16  ,
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKETYPE_FINISHED                         =   20  , 
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKETYPE_KEY_UPDATE                       =   24  ,
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKETYPE_MESSAGE_HASH                     =  254  ,      
 };


#define DIOSTREAMTLS_MSG_CIPHER_DES_CBC3_SHA                                  0x000A    // SSLv3   
#define DIOSTREAMTLS_MSG_CIPHER_RSA_WITH_AES_128_CBC_SHA                      0x002F    // SSLv3   
#define DIOSTREAMTLS_MSG_CIPHER_RSA_WITH_AES_256_CBC_SHA                      0x0035    // SSLv3  
#define DIOSTREAMTLS_MSG_CIPHER_RSA_WITH_AES_128_CBC_SHA256                   0x003C    // TLSv1.2 
#define DIOSTREAMTLS_MSG_CIPHER_RSA_WITH_AES_256_CBC_SHA256                   0x003D    // TLSv1.2 
#define DIOSTREAMTLS_MSG_CIPHER_RSA_WITH_AES_128_GCM_SHA256                   0x009C    // TLSv1.2 
#define DIOSTREAMTLS_MSG_CIPHER_RSA_WITH_AES_256_GCM_SHA384                   0x009D    // TLSv1.2 
#define DIOSTREAMTLS_MSG_CIPHER_DHE_RSA_WITH_AES_128_CBC_SHA                  0x0033
#define DIOSTREAMTLS_MSG_CIPHER_DHE_RSA_WITH_AES_256_CBC_SHA                  0x0039
#define DIOSTREAMTLS_MSG_CIPHER_DHE_RSA_WITH_AES_128_CBC_SHA256               0x0067    // TLSv1.2 
#define DIOSTREAMTLS_MSG_CIPHER_DHE_RSA_WITH_AES_256_CBC_SHA256               0x006B    // TLSv1.2 
#define DIOSTREAMTLS_MSG_CIPHER_DHE_RSA_WITH_AES_128_GCM_SHA256               0x009E    // TLSv1.2 
#define DIOSTREAMTLS_MSG_CIPHER_DHE_RSA_WITH_AES_256_GCM_SHA384               0x009F    // TLSv1.2 
#define DIOSTREAMTLS_MSG_CIPHER_AES_128_GCM_SHA256                            0x1301    // TLSv1.3 
#define DIOSTREAMTLS_MSG_CIPHER_AES_256_GCM_SHA384                            0x1302    // TLSv1.3 
#define DIOSTREAMTLS_MSG_CIPHER_CHACHA20_POLY1305_SHA256                      0x1303    // TLSv1.3 
#define DIOSTREAMTLS_MSG_CIPHER_AES_128_CCM_SHA256                            0x1304
#define DIOSTREAMTLS_MSG_CIPHER_AES_128_CCM_8_SHA256                          0x1305
#define DIOSTREAMTLS_MSG_CIPHER_ECDHE_ECDSA_WITH_AES_128_CBC_SHA              0xC009    // TLSv1   
#define DIOSTREAMTLS_MSG_CIPHER_ECDHE_ECDSA_WITH_AES_256_CBC_SHA              0xC00A    // TLSv1  
#define DIOSTREAMTLS_MSG_CIPHER_ECDHE_RSA_WITH_AES_128_CBC_SHA                0xC013    // TLSv1
#define DIOSTREAMTLS_MSG_CIPHER_ECDHE_RSA_WITH_AES_256_CBC_SHA                0xC014    // TLSv1 
#define DIOSTREAMTLS_MSG_CIPHER_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256           0xC023    // TLSv1.2 
#define DIOSTREAMTLS_MSG_CIPHER_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384           0xC024    // TLSv1.2 
#define DIOSTREAMTLS_MSG_CIPHER_ECDHE_RSA_WITH_AES_128_CBC_SHA256             0xC027    // TLSv1.2
#define DIOSTREAMTLS_MSG_CIPHER_ECDHE_RSA_WITH_AES256_SHA384                  0xC028    // TLSv1.2 
#define DIOSTREAMTLS_MSG_CIPHER_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256           0xC02B    // TLSv1.2 
#define DIOSTREAMTLS_MSG_CIPHER_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384           0xC02C    // TLSv1.2 
#define DIOSTREAMTLS_MSG_CIPHER_ECDHE_RSA_WITH_AES_128_GCM_SHA256             0xC02F    // TLSv1.2 
#define DIOSTREAMTLS_MSG_CIPHER_ECDHE_RSA_WITH_AES_256_GCM_SHA384             0xC030    // TLSv1.2 
#define DIOSTREAMTLS_MSG_CIPHER_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256       0xCCA8    // TLSv1.2
#define DIOSTREAMTLS_MSG_CIPHER_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256     0xCCA9    // TLSv1.2
#define DIOSTREAMTLS_MSG_CIPHER_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256         0xCCAA    // TLSv1.2 
                    
                    
#define DIOSTREAMTLS_MSG_COMPRESS_METHOD_NULL                                 0x00

/*
#define DIOSTREAMTLS_VERSION_DATAGRAMA_TLS_1_0                                0xFEFF    // Datagrama TLS v1.0
#define DIOSTREAMTLS_VERSION_DATAGRAMA_TLS_1_2                                0xFEFD    // Datagrama TLS v1.2
#define DIOSTREAMTLS_VERSION_DATAGRAMA_TLS_1_3                                0xFEFC    // Datagrama TLS v1.3
*/



#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class DIOSTREAMTLS_MSG_INTERFACE
{ 
  public:

                                            DIOSTREAMTLS_MSG_INTERFACE                        ();                                            
    virtual                                ~DIOSTREAMTLS_MSG_INTERFACE                        ();
                                            
    virtual bool                            SetToBuffer                                       (XBUFFER& buffer) = 0;
    virtual bool                            GetFromBuffer                                     (XBUFFER& buffer) = 0;

    XDWORD                                  GetLengthBuffer                                   ();
    
  private:

    void                                    Clean                                             ();    
};


template<typename T>
class DIOSTREAMTLS_MSG_RECORD : public DIOSTREAMTLS_MSG_INTERFACE
{
  public:
                                            DIOSTREAMTLS_MSG_RECORD                           ()
                                            {
                                              Clean();
                                            }


    virtual                                ~DIOSTREAMTLS_MSG_RECORD                           ()
                                            { 
                                              Clean();
                                            }


    DIOSTREAMTLS_CONTENTTYPE                GetContenType                                     ()
                                            {
                                              return contenttype;
                                            }    


    void                                    SetContenType                                     (DIOSTREAMTLS_CONTENTTYPE contenttype)
                                            {
                                              this->contenttype = contenttype;
                                            }    
    

    XWORD                                   GetProtocolVersion                                ()
                                            {
                                              return protocolversion;
                                            }


    void                                    SetProtocolVersion                                (XWORD protocolversion)
                                            {
                                              this->protocolversion = protocolversion;
                                            }

   
    XWORD                                   GetLength                                         ()
                                            {
                                              return length;    
                                            } 
    

    void                                    SetLength                                         (XWORD length)
                                            {
                                              this->length = length;    
                                            } 

    void                                    CalculateLength                                   ()
                                            {      
                                              GetFragment()->SetLength(GetFragment()->GetBody()->GetLengthBuffer());     
                                   
                                              SetLength(GetFragment()->GetLengthBuffer());                                              
                                            } 


    T*                                      GetFragment                                       ()
                                            {
                                              return &fragment;
                                            } 
 

    bool                                    SetToBuffer                                       (XBUFFER& buffer)
                                            {  
                                              buffer.Delete();

                                              buffer.Add((XBYTE)contenttype);
                                              buffer.Add((XWORD)protocolversion);
                                              buffer.Add((XWORD)length);

                                              fragment.SetToBuffer(buffer);
                                                
                                              return true;
                                            }


    bool                                    GetFromBuffer                                     (XBUFFER& buffer)
                                            {
                                              return true;
                                            }

  private:

    void                                    Clean                                             ()
                                            {
                                              contenttype       = (DIOSTREAMTLS_CONTENTTYPE)0;        
                                              protocolversion   = 0;   
                                              length            = 0;  
                                            }
   
    DIOSTREAMTLS_CONTENTTYPE                contenttype;        
    XWORD                                   protocolversion;   
    XWORD                                   length;  
    T                                       fragment;           
};

template<typename T>
class DIOSTREAMTLS_MSG_FRAGMENT : public DIOSTREAMTLS_MSG_INTERFACE
{ 
  public:
                                            DIOSTREAMTLS_MSG_FRAGMENT                     ()
                                            {
                                              Clean();
                                            }


    virtual                                ~DIOSTREAMTLS_MSG_FRAGMENT                     ()
                                            {
                                              Clean();
                                            }


    XBYTE                                   GetMsgType                                        ()
                                            {
                                              return msgtype;
                                            }


    void                                    SetMsgType                                        (XBYTE msgtype)
                                            {
                                              this->msgtype = msgtype;
                                            }


    XWORD                                   GetLength                                         ()
                                            {
                                              return length;    
                                            } 
    

    void                                    SetLength                                         (XWORD length)
                                            {
                                              this->length = length;    
                                            } 

    T*                                      GetBody                                           ()
                                            {
                                              return &body;  
                                            }  


    bool                                    SetToBuffer                                       (XBUFFER& buffer)
                                            {
                                              buffer.Add((XBYTE)msgtype);  

                                              XDWORD _length = length;
                                              SWAPDWORD(_length);  
                                              buffer.Add((XBYTE*)(&_length)+1, 3);  

                                              body.SetToBuffer(buffer);

                                              return true;
                                            }


    bool                                    GetFromBuffer                                     (XBUFFER& buffer)
                                            {
                                              return true;
                                            }
 
  private:

    void                                    Clean                                             ()
                                            {
                                              msgtype = 0;
                                              length  = 0;
                                            }    


    XBYTE                                   msgtype;      
    XDWORD                                  length;      
    T                                       body;
};


template<typename T>
class DIOSTREAMTLS_MSG_EXTENSION : public DIOSTREAMTLS_MSG_INTERFACE
{
  public:
                                            DIOSTREAMTLS_MSG_EXTENSION                        ()
                                            {
                                              Clean();
                                            }

    virtual                                ~DIOSTREAMTLS_MSG_EXTENSION                        ()
                                            {
                                              Clean();
                                            }


    bool                                    SetToBuffer                                       (XBUFFER& buffer)
                                            {
                                              return true;
                                            }


    bool                                    GetFromBuffer                                     (XBUFFER& buffer)
                                            {
                                              return true;
                                            }

 private:

    void                                    Clean                                             ()
                                            {

                                            }

    XWORD                                   type;
    XWORD                                   length;    
    T                                       extension;

};


class DIOSTREAMTLS_MSG_HANDSHAKETYPE_CLIENTHELLO : public DIOSTREAMTLS_MSG_INTERFACE
{
  public:
                                            DIOSTREAMTLS_MSG_HANDSHAKETYPE_CLIENTHELLO        ();
    virtual                                ~DIOSTREAMTLS_MSG_HANDSHAKETYPE_CLIENTHELLO        ();
                                            

    XWORD                                   GetClientVersion                                  ();    
    void                                    SetClientVersion                                  (XWORD clientversion);
                                            
    XBYTE*                                  GetRandom                                         ();

    XBYTE                                   GetSessionIDLength                                ();    
    void                                    SetSessionIDLength                                (XBYTE sessionID_length);
    XBYTE*                                  GetSessionID                                      ();

    XWORD                                   GetCiphersuitesLength                             ();
    void                                    SetCiphersuitesLength                             (XWORD ciphersuites_length);
    XVECTOR<XWORD>*                         GetCipherSuites                                   ();

    XBYTE                                   GetCompressionLength                              ();
    void                                    SetCompressionLength                              (XBYTE compress_length);

    XBYTE                                   GetCompressionMethod                              ();
    void                                    SetCompressionMethod                              (XBYTE compress_method);

    XWORD                                   GetExtensionLenght                                ();
    void                                    SetExtensionLenght                                (XWORD extension_lenght);
                                                
    bool                                    SetToBuffer                                       (XBUFFER& buffer);    
    bool                                    GetFromBuffer                                     (XBUFFER& buffer);
                                            
  private:

    void                                    Clean                                             ();
    
    XWORD                                   clientversion;

    XBYTE                                   random[DIOSTREAMTLS_MSG_RANDOM_SIZE];

    XBYTE                                   sessionID_length;
    XBYTE                                   sessionID[DIOSTREAMTLS_MSG_SESSIONID_SIZE];
    
    XWORD                                   ciphersuites_length;
    XVECTOR<XWORD>                          ciphersuites;

    XBYTE                                   compress_length;
    XBYTE                                   compress_method;
                                             
    XWORD                                   extensions_lenght;
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

