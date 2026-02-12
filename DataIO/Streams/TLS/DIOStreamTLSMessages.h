/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTLSMessages.h
* 
* @class      DIOSTREAMTLSMESSAGES
* @brief      Data Input/Output Stream TLS (Transport Layer Security) Messages class
* @ingroup    DATAIO
* 
* @copyright  EndoraSoft. All rights reserved.
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

#define DIOSTREAMTLS_MSG_CURVEID_X25519	                                      0x001D    // Optimizated              // Curvas modernas (TLS 1.3 recomendadas)
#define DIOSTREAMTLS_MSG_CURVEID_X448	                                        0x001E	
#define DIOSTREAMTLS_MSG_CURVEID_SECP256R1                                    0x0017	  // 256 bits
#define DIOSTREAMTLS_MSG_CURVEID_SECP384R1	                                  0x0018    // 384 bits 	
#define DIOSTREAMTLS_MSG_CURVEID_SECP521R1	                                  0x0019	  // 521 bits
#define DIOSTREAMTLS_MSG_CURVEID_FFDHE2048	                                  0x0100		// 2048 bits                // Finite Field DH (FFDHE, TLS 1.3)
#define DIOSTREAMTLS_MSG_CURVEID_FFDHE3072	                                  0x0101		// 3072 bits
#define DIOSTREAMTLS_MSG_CURVEID_FFDHE4096	                                  0x0102		// 4096 bits
#define DIOSTREAMTLS_MSG_CURVEID_FFDHE6144	                                  0x0103		// 6144 bits
#define DIOSTREAMTLS_MSG_CURVEID_FFDHE8192	                                  0x0104		// 8192 bits
#define DIOSTREAMTLS_MSG_CURVEID_SECP192R1   	                                0x0013	  // 192 bits (obsolete)      // Old Curves  (TLS 1.2)
#define DIOSTREAMTLS_MSG_CURVEID_SECP224R1 	                                  0x0015	  // 254 bits (obsolete)
#define DIOSTREAMTLS_MSG_CURVEID_SECP256K1	                                  0x0016	  // Bitcoin, not for TLS
#define DIOSTREAMTLS_MSG_CURVEID_BRAINPOOLP256R1	                            0x001A	  // Alternativa europea
#define DIOSTREAMTLS_MSG_CURVEID_BRAINPOOLP384R1	                            0x001B		
#define DIOSTREAMTLS_MSG_CURVEID_BRAINPOOLP512R1	                            0x001C	

#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA256	                0x0804	  // RSA-PSS con SHA-256      // Algoritm RSA-PSS (TLS 1.3 Mandatory)
#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA384	                0x0805	  // RSA-PSS con SHA-384
#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_RSAE_SHA512	                0x0806	  // RSA-PSS con SHA-512
#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_PSS_SHA256	                  0x0809	  // RSA-PSS puro, SHA-256
#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_PSS_SHA384	                  0x080A	  // RSA-PSS puro, SHA-384
#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PSS_PSS_SHA512	                  0x080B	  // RSA-PSS puro, SHA-512
#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP256R1_SHA256	              0x0403	  // ECDSA P-256 con SHA-256  // Algoritm ECDSA
#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP384R1_SHA384	              0x0503	  // ECDSA P-384 con SHA-384
#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SECP521R1_SHA512	              0x0603	  // ECDSA P-521 con SHA-512
#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PKCS1_SHA256	                    0x0401                                // Algoritm RSA PKCS#1 v1.5 (obsolete, only TLS 1.2)
#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PKCS1_SHA384	                    0x0501    
#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PKCS1_SHA512	                    0x0601    
#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_ED25519	                            0x0807                                // Algoritm EdDSA (TLS 1.3 last)
#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_ED448	                              0x0808     
#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_RSA_PKCS1_SHA1	                      0x0201                                // Other old (only TLS 1.2, rare)
#define DIOSTREAMTLS_MSG_SIGNATURESCHEME_ECDSA_SHA1	                          0x0203       

#define DIOSTREAMTLS_MSG_PSKKEYEXCHANGEMODE_PSK                             	0x00		  // PSK pure, without Diffie-Hellman 
#define DIOSTREAMTLS_MSG_PSKKEYEXCHANGEMODE_PSK_DHE	                          0x01	    // PSK with Diffie-Hellman (DHE) 


enum DIOSTREAMTLS_CONTENTTYPE
{
   DIOSTREAMTLS_MSG_CONTENTTYPE_CHANGE_CIPHER_SPEC                            =    0x14  , 
   DIOSTREAMTLS_MSG_CONTENTTYPE_ALERT                                         =    0x15  , 
   DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE                                     =    0x16  ,
   DIOSTREAMTLS_MSG_CONTENTTYPE_APPLICATION_DATA                              =    0x17  , 
   DIOSTREAMTLS_MSG_CONTENTTYPE_TLS_1_3_HEARTBEAT                             =    0x18  , 
};


enum DIOSTREAMTLS_CONTENTTYPE_HANDSHAKE
{
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_HELLO_REQUEST                        =      0  , 
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_CLIENT_HELLO                         =      1  , 
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_SERVER_HELLO                         =      2  ,
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_CERTIFICATE                          =     11  , 
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_SERVER_KEY_EXCHANGE                  =     12  ,
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_CERTIFICATE_REQUEST                  =     13  , 
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_SERVER_HELLO_DONE                    =     14  ,
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_CERTIFICATE_VERIFY                   =     15  , 
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_CLIENT_KEY_EXCHANGE                  =     16  ,
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_FINISHED                             =     20  , 
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_KEY_UPDATE                           =     24  ,
  DIOSTREAMTLS_MSG_CONTENTTYPE_HANDSHAKE_MESSAGE_HASH                         =    254  ,      
 };


enum DIOSTREAMTLS_ALPN_TYPE
{
  DIOSTREAMTLS_ALPN_TYPE_HTTP_1_1       ,
  DIOSTREAMTLS_ALPN_TYPE_HTTP_2         ,
  DIOSTREAMTLS_ALPN_TYPE_HTTP_3         
};



#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class DIOSTREAMTLS_MSG_INTERFACE
{ 
  public:

                                            DIOSTREAMTLS_MSG_INTERFACE                        ();                                            
    virtual                                ~DIOSTREAMTLS_MSG_INTERFACE                        ();
                                            
    virtual bool                            SetToBuffer                                       (XBUFFER& buffer, bool showdebug) = 0;
    virtual bool                            GetFromBuffer                                     (XBUFFER& buffer, bool showdebug) = 0;

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
 

    bool                                    SetToBuffer                                       (XBUFFER& buffer, bool showdebug)
                                            {  
                                              buffer.Delete();

                                              buffer.Add((XBYTE)contenttype);
                                              buffer.Add(protocolversion);
                                              buffer.Add(length);

                                              fragment.SetToBuffer(buffer, showdebug);
                                                
                                              return true;
                                            }


    bool                                    GetFromBuffer                                     (XBUFFER& buffer, bool showdebug)
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
                                            DIOSTREAMTLS_MSG_FRAGMENT                         ()
                                            {
                                              Clean();
                                            }


    virtual                                ~DIOSTREAMTLS_MSG_FRAGMENT                         ()
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


    bool                                    SetToBuffer                                       (XBUFFER& buffer, bool showdebug)
                                            {
                                              buffer.Add((XBYTE)msgtype);  

                                              XDWORD _length = length;
                                              SWAPDWORD(_length);  
                                              buffer.Add((XBYTE*)(&_length)+1, 3);  

                                              body.SetToBuffer(buffer,  showdebug);

                                              return true;
                                            }


    bool                                    GetFromBuffer                                     (XBUFFER& buffer, bool showdebug)
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


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

