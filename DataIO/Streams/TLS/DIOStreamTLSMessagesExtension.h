/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOStreamTLSMessagesExtension.h
* 
* @class      DIOSTREAMTLSMESSAGE
* @brief      Data Input/Output Stream TLS (Transport Layer Security) Messages Extension class
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

#ifndef _DIOSTREAMTLSMESSAGEEXTENSION_H_
#define _DIOSTREAMTLSMESSAGEEXTENSION_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOStreamTLSMessages.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_SNI                   0x0000 // 1.0+  SNI 
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_SUPPORTEDGROUPS	      0x000a // 1.2+  Curve
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_ECPOINTFORMATS	      0x000b // 1.2	  ECP Point Formats
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_SIGNATUREALGORITHMS	  0x000d // 1.2+
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_ALPN	                0x0010 // 1.2+  Protocol app
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_EMS                   0x0017 // 1.2	  Extended Mster Secret 
 #define DIOSTREAMTLS_MSG_EXTENSION_TYPE_SESSIONTICKET	        0x0023 // 1.2	  Ticket
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_SUPPORTEDVERSIONS	    0x002b // 1.3	  TLS Versions
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_PSKKEYEXCHANGEMODES	  0x002d // 1.3	  Mode PSK
#define DIOSTREAMTLS_MSG_EXTENSION_TYPE_KEYSHARE	            0x0033 // 1.3	  Public Key                                                                    


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
    
    virtual bool                                          SetToBuffer                                       (XBUFFER& buffer, bool showdebug);                                           
    virtual bool                                          GetFromBuffer                                     (XBUFFER& buffer, bool showdebug);
     
  private:

    void                                                  Clean                                             ();
    
    XWORD                                                 type;
    XWORD                                                 length;    
};   


template<typename T, typename S>
class DIOSTREAMTLS_MSG_EXTENSION_LIST : public DIOSTREAMTLS_MSG_EXTENSION
{
  public:
                                                          DIOSTREAMTLS_MSG_EXTENSION_LIST                   ()
                                                          {
                                                            Clean();
                                                          }

    virtual                                              ~DIOSTREAMTLS_MSG_EXTENSION_LIST                   ()
                                                          {
                                                            Clean();
                                                          }

    XBYTE                                                 List_GetLength                                    () 
                                                          {
                                                            return list_length;
                                                          }
                                                         
    void                                                  List_SetLength                                    (XBYTE list_length)
                                                          {
                                                            this->list_length = list_length;
                                                          }
    
    XVECTOR<T>*                                           List_Get                                          ()  
                                                          {
                                                            return &list;
                                                          }
                                                       
    bool                                                  List_Add                                          (T element)
                                                          {
                                                            list.Add(element);

                                                            List_SetLength(list.GetSize() * sizeof(T));  

                                                            SetLength(List_GetLength() + sizeof(S));  

                                                            return true;
                                                          } 

    bool                                                  List_DeleteAll                                    ()
                                                          {
                                                            list.DeleteAll();
  
                                                            SetLength(0);

                                                            return true;
                                                          }
    
    bool                                                  CopyTo                                            (DIOSTREAMTLS_MSG_EXTENSION_LIST* extension)
                                                          {
                                                            if(!extension)
                                                              {
                                                                return false;
                                                              }

                                                            extension->SetType(GetType());  
                                                            extension->SetLength(GetLength());

                                                            extension->List_SetLength(List_GetLength());

                                                            for(XDWORD c=0; c<list.GetSize(); c++)
                                                              {
                                                                T element = list.Get(c);      
                                                                extension->List_Get()->Add(element);      
                                                              }

                                                            return true;
                                                          }                                                          

    bool                                                  CopyFrom                                          (DIOSTREAMTLS_MSG_EXTENSION_LIST* extension)
                                                          {
                                                            if(!extension)
                                                              {
                                                                return false;
                                                              }

                                                            SetType(extension->GetType());  
                                                            SetLength(extension->GetLength());

                                                            List_SetLength(extension->List_GetLength());

                                                            for(XDWORD c=0; c<list.GetSize(); c++)
                                                              {
                                                                T element = extension->List_Get()->Get(c);      
                                                                List_Get()->Add(element);      
                                                              }

                                                            return true;
                                                          }
                                                                                                                
    bool                                                  SetToBuffer                                       (XBUFFER& buffer, bool showdebug)
                                                          {
                                                            DIOSTREAMTLS_MSG_EXTENSION::SetToBuffer(buffer, showdebug);

                                                            buffer.Add((S)list_length);

                                                            for(XDWORD c=0; c<list.GetSize(); c++)
                                                              {
                                                                T element = List_Get()->Get(c);      
                                                                buffer.Add(element);
                                                              }

                                                            return true;
                                                          }                                                                                                     

    bool                                                  GetFromBuffer                                     (XBUFFER& buffer, bool showdebug)
                                                          {
                                                            DIOSTREAMTLS_MSG_EXTENSION::GetFromBuffer(buffer, showdebug);

                                                            buffer.Extract((S&)list_length);

                                                            XDWORD nsize = List_GetLength() / sizeof(T);

                                                            for(XDWORD c=0; c<nsize; c++)
                                                              {
                                                                T element = List_Get()->Get(c);      

                                                                buffer.Extract(element);

                                                                List_Get()->Add(element);      
                                                              }

                                                            return true;
                                                          }                                                        
                                               
  private:

    void                                                  Clean                                             ()
                                                          {
                                                            list_length = 0;
                                                          }
                                                          
    T                                                     list_length;
    XVECTOR<T>                                            list;
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

    bool                                                  SetToBuffer                                       (XBUFFER& buffer, bool showdebug);                                           
    bool                                                  GetFromBuffer                                     (XBUFFER& buffer, bool showdebug);

  private:

    void                                                  Clean                                             ();   

    XBYTE                                                 name_type;                                  
    XWORD                                                 name_length;     
    XSTRING                                               hostname;                                
};


class DIOSTREAMTLS_MSG_EXTENSION_SNI : public DIOSTREAMTLS_MSG_EXTENSION
{
  public:
                                                          DIOSTREAMTLS_MSG_EXTENSION_SNI                    ();
    virtual                                              ~DIOSTREAMTLS_MSG_EXTENSION_SNI                    ();

    XWORD                                                 List_GetLength                                    ();                                          
    void                                                  List_SetLength                                    (XWORD list_length);

    XVECTOR<DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME*>*  List_Get                                          ();
    bool                                                  List_Add                                          (DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME* servername);
    bool                                                  List_DeleteAll                                    ();

    bool                                                  CopyTo                                            (DIOSTREAMTLS_MSG_EXTENSION_SNI* extension);  
    bool                                                  CopyFrom                                          (DIOSTREAMTLS_MSG_EXTENSION_SNI* extension);  
                                                       
    bool                                                  SetToBuffer                                       (XBUFFER& buffer, bool showdebug);                                           
    bool                                                  GetFromBuffer                                     (XBUFFER& buffer, bool showdebug);
                                               
  private:

    void                                                  Clean                                             ();   

    XWORD                                                 list_length;
    XVECTOR<DIOSTREAMTLS_MSG_EXTENSION_SNI_SERVERNAME*>   list;
};


class DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDGROUPS : public DIOSTREAMTLS_MSG_EXTENSION_LIST<XWORD, XWORD>
{
  public:
                                                          DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDGROUPS        ();
    virtual                                              ~DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDGROUPS        ();
                                                
};


class DIOSTREAMTLS_MSG_EXTENSION_ECPOINTFORMATS : public DIOSTREAMTLS_MSG_EXTENSION_LIST<XWORD, XWORD>
{
  public:
                                                          DIOSTREAMTLS_MSG_EXTENSION_ECPOINTFORMATS         ();
    virtual                                              ~DIOSTREAMTLS_MSG_EXTENSION_ECPOINTFORMATS         ();
                                                
};


class DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS : public DIOSTREAMTLS_MSG_EXTENSION_LIST<XWORD, XWORD>
{
  public:
                                                          DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS    ();
    virtual                                              ~DIOSTREAMTLS_MSG_EXTENSION_SIGNATUREALGORITHMS    ();
                                                
};


class DIOSTREAMTLS_MSG_EXTENSION_ALPN : public DIOSTREAMTLS_MSG_EXTENSION
{
  public:
                                                          DIOSTREAMTLS_MSG_EXTENSION_ALPN                   ();
    virtual                                              ~DIOSTREAMTLS_MSG_EXTENSION_ALPN                   ();

    XWORD                                                 List_GetLength                                    ();                                          
    void                                                  List_SetLength                                    (XWORD list_length);

    XBUFFER*                                              List_GetBuffer                                    (); 
    bool                                                  List_Add                                          (DIOSTREAMTLS_ALPN_TYPE alpn_type);
    bool                                                  List_DeleteAll                                    ();

    bool                                                  CopyTo                                            (DIOSTREAMTLS_MSG_EXTENSION_ALPN* extension);  
    bool                                                  CopyFrom                                          (DIOSTREAMTLS_MSG_EXTENSION_ALPN* extension);  
                                                       
    bool                                                  SetToBuffer                                       (XBUFFER& buffer, bool showdebug);                                           
    bool                                                  GetFromBuffer                                     (XBUFFER& buffer, bool showdebug);
                                               
  private:

    void                                                  Clean                                             ();   
    
    XWORD                                                 list_length;
    XBUFFER                                               list_buffer;                                                
};


class DIOSTREAMTLS_MSG_EXTENSION_EMS : public DIOSTREAMTLS_MSG_EXTENSION
{
  public:
                                                          DIOSTREAMTLS_MSG_EXTENSION_EMS                    ();
    virtual                                              ~DIOSTREAMTLS_MSG_EXTENSION_EMS                    ();

    bool                                                  CopyTo                                            (DIOSTREAMTLS_MSG_EXTENSION_EMS* extension);  
    bool                                                  CopyFrom                                          (DIOSTREAMTLS_MSG_EXTENSION_EMS* extension);  
                                                       
    bool                                                  SetToBuffer                                       (XBUFFER& buffer, bool showdebug);                                           
    bool                                                  GetFromBuffer                                     (XBUFFER& buffer, bool showdebug);
                                              
  private:

    void                                                  Clean                                             ();   
};


class DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS : public DIOSTREAMTLS_MSG_EXTENSION_LIST<XWORD, XBYTE>
{
  public:
                                                          DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS      ();
    virtual                                              ~DIOSTREAMTLS_MSG_EXTENSION_SUPPORTEDVERSIONS      ();
                                                
};


class DIOSTREAMTLS_MSG_EXTENSION_PSKKEYEXCHANGEMODES : public DIOSTREAMTLS_MSG_EXTENSION_LIST<XBYTE, XBYTE>
{
  public:
                                                          DIOSTREAMTLS_MSG_EXTENSION_PSKKEYEXCHANGEMODES    ();
    virtual                                              ~DIOSTREAMTLS_MSG_EXTENSION_PSKKEYEXCHANGEMODES    ();
                                                
};


class DIOSTREAMTLS_MSG_EXTENSION_KEY
{
  public:
                                                          DIOSTREAMTLS_MSG_EXTENSION_KEY                    ();
    virtual                                              ~DIOSTREAMTLS_MSG_EXTENSION_KEY                    ();

    

    XWORD                                                 GetKeyType                                        ();
    void                                                  SetKeyType                                        (XWORD keytype);

    XWORD                                                 GetLengthKeyData                                  ();                                          
    void                                                  SetLengthKeyData                                  (XWORD lengthkeydata);

    XBUFFER*                                              GetKeyData                                        ();

    bool                                                  CopyTo                                            (DIOSTREAMTLS_MSG_EXTENSION_KEY* key);  
    bool                                                  CopyFrom                                          (DIOSTREAMTLS_MSG_EXTENSION_KEY* key);  

    bool                                                  SetToBuffer                                       (XBUFFER& buffer, bool showdebug);                                           
    bool                                                  GetFromBuffer                                     (XBUFFER& buffer, bool showdebug);

  private:

    void                                                  Clean                                             ();   

    XWORD                                                 keytype;
    XWORD                                                 lengthkeydata;    
    XBUFFER                                               keydata;
};


class DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE : public DIOSTREAMTLS_MSG_EXTENSION
{
  public:
                                                          DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE               ();
    virtual                                              ~DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE               ();

    XWORD                                                 List_GetLength                                    ();                                          
    void                                                  List_SetLength                                    (XWORD list_length);

    XVECTOR<DIOSTREAMTLS_MSG_EXTENSION_KEY*>*             List_Get                                          ();
    bool                                                  List_Add                                          (DIOSTREAMTLS_MSG_EXTENSION_KEY* key);
    bool                                                  List_DeleteAll                                    ();

    bool                                                  CopyTo                                            (DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE* extension);  
    bool                                                  CopyFrom                                          (DIOSTREAMTLS_MSG_EXTENSION_KEYSHARE* extension);  
                                                       
    bool                                                  SetToBuffer                                       (XBUFFER& buffer, bool showdebug);                                           
    bool                                                  GetFromBuffer                                     (XBUFFER& buffer, bool showdebug);
                                               
  private:

    void                                                  Clean                                             ();   

    XWORD                                                 list_length;
    XVECTOR<DIOSTREAMTLS_MSG_EXTENSION_KEY*>              list;
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

