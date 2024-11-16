/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOCoreProtocol_Connection.h
* 
* @class      DIOCOREPROTOCOL_CONNECTION
* @brief      Data Input/Output Core Protocol Connection class
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

#ifndef _DIOCOREPROTOCOL_CONNECTION_H_
#define _DIOCOREPROTOCOL_CONNECTION_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES


enum DIOCOREPROTOCOL_CONNECTION_XFSMEVENTS
{
  DIOCOREPROTOCOL_CONNECTION_XFSMEVENT_NONE                 = 0 ,
  DIOCOREPROTOCOL_CONNECTION_XFSMEVENT_CONNECTED                ,
  DIOCOREPROTOCOL_CONNECTION_XFSMEVENT_INI_AUTHENTICATION       ,
  DIOCOREPROTOCOL_CONNECTION_XFSMEVENT_END_AUTHENTICATION       ,
  DIOCOREPROTOCOL_CONNECTION_XFSMEVENT_INI_INITIALIZATION       ,
  DIOCOREPROTOCOL_CONNECTION_XFSMEVENT_END_INITIALIZATION       ,
  DIOCOREPROTOCOL_CONNECTION_XFSMEVENT_IN_PROGRESS              ,
  DIOCOREPROTOCOL_CONNECTION_XFSMEVENT_DISCONNECTED             ,

  DIOCOREPROTOCOL_CONNECTION_LASTEVENT
};


enum DIOCOREPROTOCOL_CONNECTION_XFSMSTATES
{
  DIOCOREPROTOCOL_CONNECTION_XFSMSTATE_NONE                 = 0 ,
  DIOCOREPROTOCOL_CONNECTION_XFSMSTATE_CONNECTED                ,
  DIOCOREPROTOCOL_CONNECTION_XFSMSTATE_INI_AUTHENTICATION       ,
  DIOCOREPROTOCOL_CONNECTION_XFSMSTATE_END_AUTHENTICATION       ,
  DIOCOREPROTOCOL_CONNECTION_XFSMSTATE_INI_INITIALIZATION       ,
  DIOCOREPROTOCOL_CONNECTION_XFSMSTATE_END_INITIALIZATION       ,
  DIOCOREPROTOCOL_CONNECTION_XFSMSTATE_IN_PROGRESS              ,  
  DIOCOREPROTOCOL_CONNECTION_XFSMSTATE_DISCONNECTED             ,

  DIOCOREPROTOCOL_CONNECTION_LASTSTATE
};


enum DIOCOREPROTOCOL_CONNECTION_STATUS
{
  DIOCOREPROTOCOL_CONNECTION_STATUS_NONE                    = 0 , 
  DIOCOREPROTOCOL_CONNECTION_STATUS_CONNECTED                   , 
  DIOCOREPROTOCOL_CONNECTION_STATUS_AUTHENTICATED               , 
  DIOCOREPROTOCOL_CONNECTION_STATUS_INITIALIZED                 ,
  DIOCOREPROTOCOL_CONNECTION_STATUS_DISCONNECTED                ,
};

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

#include "XFSMachine.h"

#include "DIOCoreProtocol_Messages.h"

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class DIOCOREPROTOCOL_CFG;
class DIOCOREPROTOCOL;


class DIOCOREPROTOCOL_CONNECTION : public XFSMACHINE
{
  public:
                                          DIOCOREPROTOCOL_CONNECTION            ();
    virtual                              ~DIOCOREPROTOCOL_CONNECTION            ();

    bool                                  InitFSMachine                         (); 
    
    bool                                  IsServer                              ();
    
    DIOCOREPROTOCOL*                      GetCoreProtocol                       ();
    bool                                  SetCoreProtocol                       (DIOCOREPROTOCOL* protocol);

    XBUFFER*                              GetAuthenticationChallenge            ();
    XBUFFER*                              GetAuthenticationResponse             ();

    DIOCOREPROTOCOL_CONNECTION_STATUS     GetStatus                             ();  
    void                                  SetStatus                             (DIOCOREPROTOCOL_CONNECTION_STATUS status);
    bool                                  GetStatusString                       (XSTRING& statusstring); 
    XTIMER*                               GetXTimerStatus                       ();

    DIOCOREPROTOCOL_MESSAGES*             GetMessages                           ();

    bool                                  SendMsg                               (XUUID* ID_message, XBYTE message_priority, DIOCOREPROTOCOL_HEADER_OPERATION operation, XCHAR* operation_param, XBUFFER& content);
    bool                                  SendMsg                               (XUUID* ID_message, XBYTE message_priority, DIOCOREPROTOCOL_HEADER_OPERATION operation, XCHAR* operation_param, XSTRING& content);
    bool                                  SendMsg                               (XUUID* ID_message, XBYTE message_priority, DIOCOREPROTOCOL_HEADER_OPERATION operation, XCHAR* operation_param, XFILEJSON& content);    

    bool                                  Update                                ();  

  private:

    void                                  Clean                                 ();    
    
    DIOCOREPROTOCOL*                      protocol;   

    XBUFFER                               authentication_challenge;  
    XBUFFER                               authentication_response;     

    DIOCOREPROTOCOL_CONNECTION_STATUS     status;
    XTIMER*                               xtimerstatus;  

    DIOCOREPROTOCOL_MESSAGES              messages;    
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif


