/**-------------------------------------------------------------------------------------------------------------------
*
* @file        XTrace.cpp
*
* @class       XTRACE
* @brief       eXtended Trace class (for debug)
* @ingroup     UTILS
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

#include "XFactory.h"

#include "DIOFactory.h"
#include "DIOURL.h"
#include "DIODNSResolver.h"
#include "DIOStreamEnumDevices.h"

#include "DIOStreamDeviceIP.h"
#include "DIOStreamIPLocalEnumDevices.h"
#include "DIOScraperWebPublicIP.h"

#include "XTrace.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

XTRACE* XTRACE::instance = NULL;

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/
/*  XTRACE_TARGET                                                                                                     */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XTRACE_TARGET::XTRACE_TARGET()
* @brief      Constructor
* @ingroup    XUTILS
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XTRACE_TARGET::XTRACE_TARGET()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XTRACE_TARGET::~XTRACE_TARGET()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XTRACE_TARGET::~XTRACE_TARGET()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XTRACE_TYPE XTRACE_TARGET::GetType()
* @brief      Get Type of debug trace (enum XTRACE_TYPE)
* @ingroup    XUTILS
*
* @return     XTRACE_TYPE : type of debug trace (enum XTRACE_TYPE)
*
* --------------------------------------------------------------------------------------------------------------------*/
XTRACE_TYPE XTRACE_TARGET::GetType()
{
  return type;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE_TARGET::SetType(XTRACE_TYPE type)
* @brief      Set debug trace type (enum XTRACE_TYPE)
* @ingroup    XUTILS
*
* @param[in]  type : type of debug trace (enum XTRACE_TYPE)
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE_TARGET::SetType(XTRACE_TYPE type)
{
  this->type = type;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* XTRACE_TARGET::GetAim()
* @brief      Get aim of target
* @ingroup    XUTILS
*
* @return     XCHAR* : aim of target
*
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* XTRACE_TARGET::GetAim()
{
  return aim;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE_TARGET::GetAim(XSTRING& aim)
* @brief      Get aim of target
* @ingroup    XUTILS
*
* @param[out] aim : Get aim of target
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE_TARGET::GetAim(XSTRING& aim)
{
  aim.Empty();
  aim = this->aim;

  return aim.IsEmpty()?false:true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE_TARGET::SetAim(XSTRING& aim)
* @brief      Set aim of target
* @ingroup    XUTILS
*
* @param[in]  aim : new aim of target
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE_TARGET::SetAim(XSTRING& aim)
{
  memset(this->aim, 0, (_MAXSTR * sizeof(XCHAR)));
  memcpy(this->aim, aim.Get(), (aim.GetSize() * sizeof(XCHAR)));
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE_TARGET::SetAim(XCHAR* aim)
* @brief      Set aim of target
* @ingroup    XUTILS
*
* @param[in]  aim : new aim of target
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE_TARGET::SetAim(XCHAR* aim)
{
  XDWORD size = XSTRING::GetSize(aim);
  memset(this->aim, 0, (_MAXSTR * sizeof(XCHAR)));
  memcpy(this->aim, aim, size * sizeof(XCHAR));
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD XTRACE_TARGET::GetNSending()
* @brief      Get number of sendings
* @ingroup    XUTILS
*
* @return     XDWORD : number of sendings
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD XTRACE_TARGET::GetNSendings()
{
  return nsendings;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE_TARGET::AddNSendings()
* @brief      Add One to number of sendings
* @ingroup    XUTILS
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE_TARGET::AddNSendings()
{
  nsendings++;
}


#if (defined(DIO_ACTIVE) && defined(DIO_STREAMUDP_ACTIVE))

/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD XTRACE_TARGET::GetPort()
* @brief      Get port of target
* @ingroup    XUTILS
*
* @return     XWORD : port of target
*
* --------------------------------------------------------------------------------------------------------------------*/
XWORD XTRACE_TARGET::GetPort()
{
  return port;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE_TARGET::SetPort(XWORD port)
* @brief      Set Port of target
* @ingroup    XUTILS
*
* @param[in]  port : port of target
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE_TARGET::SetPort(XWORD port)
{
  this->port = port;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE_TARGET::IPTarget()
* @brief       IP of target aim = URL -> IP
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE_TARGET::IPTarget()
{
  if(type != XTRACE_TYPE_NET) return false;

  XSTRING  _aim;
  DIOIP    _IP;
  XSTRING  _IPstring;

  _aim = aim;

  if(!_aim.Compare(__L("*"), true))
    {
      _IPstring = __L("*");
    }
   else
    {          
      #ifndef XTRACE_NOINTERNET

      GEN_DIODNSRESOLVER.ResolveURL(_aim.Get(), _IP);
      _IP.GetXString(_IPstring);

      #else
 
      _IPstring = _aim;

      #endif
    }
    
    
  XBUFFER charstr;
 
  _IPstring.ConvertToASCII(charstr);    
  memcpy(IP, charstr.GetPtrChar(), strlen(charstr.GetPtrChar())+1);
  
  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         char* XTRACE_TARGET::GetIP()
* @brief      Get IP of target
* @ingroup    XUTILS
*
* @return     char* : string with the IP
*
* --------------------------------------------------------------------------------------------------------------------*/
char* XTRACE_TARGET::GetIP()
{
  return IP;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XQWORD XTRACE_TARGET::GetNETHandle()
* @brief      Get net Handle of target
* @ingroup    XUTILS
*
* @return     XQWORD : Net Handle of target
*
* --------------------------------------------------------------------------------------------------------------------*/
XQWORD XTRACE_TARGET::GetNETHandle()
{
  return NEThandle;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE_TARGET:: SetNETHandle(XQWORD NEThandle)
* @brief      Set Net Handle of target
* @ingroup    XUTILS
*
* @param[in]  NEThandle : new Net Handle of target
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE_TARGET:: SetNETHandle(XQWORD NEThandle)
{
  this->NEThandle = NEThandle;
}


#endif


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE_TARGET::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE_TARGET::Clean()
{
  type          = XTRACE_TYPE_NONE;

  memset(aim  , 0, (_MAXSTR * sizeof(XCHAR)));
  nsendings     = 0;

  #if (defined(DIO_ACTIVE) && defined(DIO_STREAMUDP_ACTIVE))

  port          = 0;
  memset(IP, 0, _MAXSTR * sizeof(char));
  NEThandle     = 0;

  #endif
}



/*--------------------------------------------------------------------------------------------------------------------*/
/*  XTRACE_STATUS_MSG                                                                                                 */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XTRACE_STATUS_MSG::XTRACE_STATUS_MSG()
* @brief      Constructor
* @ingroup    XUTILS
*
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XTRACE_STATUS_MSG::XTRACE_STATUS_MSG()
{
  Clean();

  GEN_XFACTORY_CREATE(xtimerlastupdate, CreateTimer())
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XTRACE_STATUS_MSG::~XTRACE_STATUS_MSG()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XTRACE_STATUS_MSG::~XTRACE_STATUS_MSG()
{
  if(xtimerlastupdate)
    {
      GEN_XFACTORY.DeleteTimer(xtimerlastupdate);
      xtimerlastupdate = NULL;
    }

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XTRACE_TYPE_STATUS_MSG XTRACE_STATUS_MSG::GetType()
* @brief      GetType
* @ingroup    XUTILS
*
* @return     XTRACE_TYPE_STATUS_MSG : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XTRACE_TYPE_STATUS_MSG XTRACE_STATUS_MSG::GetType()
{
  return type;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XTRACE_STATUS_MSG::SetType(XTRACE_TYPE_STATUS_MSG type)
* @brief      SetType
* @ingroup    XUTILS
*
* @param[in]  type : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XTRACE_STATUS_MSG::SetType(XTRACE_TYPE_STATUS_MSG type)
{
  this->type = type;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* XTRACE_STATUS_MSG::GetName()
* @brief      GetName
* @ingroup    XUTILS
*
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XTRACE_STATUS_MSG::GetName()
{
  return &name;  
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XTIMER* XTRACE_STATUS_MSG::GetXTimerLastUpdate()
* @brief      GetXTimerLastUpdate
* @ingroup    XUTILS
*
* @return     XTIMER* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XTIMER* XTRACE_STATUS_MSG::GetXTimerLastUpdate()
{
  return xtimerlastupdate;
}




/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XTRACE_STATUS_MSG::Value_GetBoolean()
* @brief      Value_GetBoolean
* @ingroup    XUTILS
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XTRACE_STATUS_MSG::Value_GetBoolean()
{
  return value_boolean;
}
    


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XTRACE_STATUS_MSG::Value_SetBoolean(bool value)
* @brief      Value_SetBoolean
* @ingroup    XUTILS
*
* @param[in]  value : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XTRACE_STATUS_MSG::Value_SetBoolean(bool value)
{
  value_boolean = value;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int XTRACE_STATUS_MSG::Value_GetInteger()
* @brief      Value_GetInteger
* @ingroup    XUTILS
*
* @return     int : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
int XTRACE_STATUS_MSG::Value_GetInteger()
{
  return value_integer;
}
    


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XTRACE_STATUS_MSG::Value_SetInteger(int value)
* @brief      Value_SetInteger
* @ingroup    XUTILS
*
* @param[in]  value : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XTRACE_STATUS_MSG::Value_SetInteger(int value)
{
  this->value_integer = value;
}


    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* XTRACE_STATUS_MSG::Value_GetString()
* @brief      Value_GetString
* @ingroup    XUTILS
*
* @return     XSTRING* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XTRACE_STATUS_MSG::Value_GetString()
{
  return &value_string;
}


    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD XTRACE_STATUS_MSG::Value_GetDword()
* @brief      Value_GetDword
* @ingroup    XUTILS
*
* @return     XDWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XDWORD XTRACE_STATUS_MSG::Value_GetDword()
{
  return value_dword;
}
    


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XTRACE_STATUS_MSG::Value_SetDword(XDWORD value)
* @brief      Value_SetDword
* @ingroup    XUTILS
*
* @param[in]  value : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XTRACE_STATUS_MSG::Value_SetDword(XDWORD value)
{
  value_dword = value;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         float XTRACE_STATUS_MSG::Value_GetFloat()
* @brief      Value_GetFloat
* @ingroup    XUTILS
*
* @return     float : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
float XTRACE_STATUS_MSG::Value_GetFloat()
{
  return value_float;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XTRACE_STATUS_MSG::Value_SetFloat(float value)
* @brief      Value_SetFloat
* @ingroup    XUTILS
*
* @param[in]  value : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XTRACE_STATUS_MSG::Value_SetFloat(float value)
{
  value_float = value;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XBYTE* XTRACE_STATUS_MSG::Value_GetColor()
* @brief      Value_GetColor
* @ingroup    XUTILS
*
* @return     XBYTE* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XBYTE* XTRACE_STATUS_MSG::Value_GetColor()
{
  return value_color;
}
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XTRACE_STATUS_MSG::Value_SetColor(XBYTE* value)
* @brief      Value_SetColor
* @ingroup    XUTILS
*
* @param[in]  value : 
* 
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XTRACE_STATUS_MSG::Value_SetColor(XBYTE* value)
{
  for(int c=0; c<3; c++)
    {
      value_color[c] = value[c];
    }
}


      

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XTRACE_STATUS_MSG::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XTRACE_STATUS_MSG::Clean()
{
  type              = XTRACE_TYPE_STATUS_MSG_UNKNOWN;  

  xtimerlastupdate  = NULL;

  value_boolean     = false;
  value_integer     = 0;
  value_string.Empty();
  value_dword       = 0;
  value_float       = 0.0f;

  for(int c=0; c<3; c++)
    {
      value_color[c] = 0;        
    }
}  



/*--------------------------------------------------------------------------------------------------------------------*/
/*  XTRACE_STATUS_MSGS                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XTRACE_STATUS_MSGS::XTRACE_STATUS_MSGS()
* @brief      Constructor
* @ingroup    XUTILS
*
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XTRACE_STATUS_MSGS::XTRACE_STATUS_MSGS()
{
  Clean();
}
       


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XTRACE_STATUS_MSGS::~XTRACE_STATUS_MSGS()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @return     Does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XTRACE_STATUS_MSGS::~XTRACE_STATUS_MSGS()
{
  StatusMsg_DeleteAll();

  Clean();
}




/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XTRACE_STATUS_MSG* XTRACE_STATUS_MSGS::StatusMsg_Add(XSTRING* line)
* @brief      StatusMsg_Add
* @ingroup    XUTILS
*
* @param[in]  line : 
* 
* @return     XTRACE_STATUS_MSG* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XTRACE_STATUS_MSG* XTRACE_STATUS_MSGS::StatusMsg_Add(XSTRING* line)
{
  if(!line) return NULL;

  XSTRING   ID;
  XSTRING   name;
  XCHAR     typechar;
  XSTRING   valuestr;
  bool      isnew = false;


  int indexstatus = line->Find(XTRACE_IDMSGSTATUSCLEAR, false);
  if(indexstatus != XSTRING_NOTFOUND) 
    {
      statusmsgs.DeleteContents();
      statusmsgs.DeleteAll();

      return NULL;
    }


  indexstatus = line->Find(XTRACE_IDMSGSTATUS, false);
  if(indexstatus == XSTRING_NOTFOUND) 
    {
      return NULL;
    }


  XSTRING string;

  line->Copy(indexstatus, string);

  ID.AdjustSize(_MAXSTR);
  name.AdjustSize(_MAXSTR);
  valuestr.AdjustSize(_MAXSTR);

  string.UnFormat(__L("%s,%s,%c,%s"), ID.Get(), name.Get(), &typechar, valuestr.Get());

  ID.AdjustSize();
  name.AdjustSize();
  valuestr.AdjustSize();


  XTRACE_STATUS_MSG* status_msg = StatusMsg_Get(name.Get());
  if(!status_msg)
    {
      status_msg = new XTRACE_STATUS_MSG();
      if(!status_msg) return NULL;

      isnew = true;  
    }

  switch(typechar)
    {      
      case XTRACE_IDMSGSTATUS_BOOLEAN : { status_msg->SetType(XTRACE_TYPE_STATUS_MSG_BOOLEAN);
                                          if(!valuestr.Compare(__L("true"), true))  status_msg->Value_SetBoolean(true); else status_msg->Value_SetBoolean(false);
                                        }
                                        break;

      case XTRACE_IDMSGSTATUS_INTEGER : { status_msg->SetType(XTRACE_TYPE_STATUS_MSG_INTEGER);
                                          status_msg->Value_SetInteger(valuestr.ConvertToInt());
                                        } 
                                        break;
    
      case XTRACE_IDMSGSTATUS_STRING  : { status_msg->SetType(XTRACE_TYPE_STATUS_MSG_STRING);
                                          status_msg->Value_GetString()->Set(valuestr);
                                        } 
                                        break;

      case XTRACE_IDMSGSTATUS_DWORD   : { status_msg->SetType(XTRACE_TYPE_STATUS_MSG_XDWORD);
                                          status_msg->Value_SetDword(valuestr.ConvertToDWord());
                                        }
                                        break;
    
      case XTRACE_IDMSGSTATUS_FLOAT   : { status_msg->SetType(XTRACE_TYPE_STATUS_MSG_FLOAT);
                                          status_msg->Value_SetFloat(valuestr.ConvertToFloat());
                                        }
                                        break;

      case XTRACE_IDMSGSTATUS_COLOR   : { status_msg->SetType(XTRACE_TYPE_STATUS_MSG_RGBCOLOR);

                                          int    icolor[3];
                                          XBYTE  color[3]; 
                                          valuestr.UnFormat(__L("%02X,%02X,%02X"), &icolor[0], &icolor[1], &icolor[2]);

                                          for(int c=0; c<3; c++)
                                            {  
                                              color[c] = icolor[c];
                                            }

                                          status_msg->Value_SetColor(color);
                                        }
                                        break;    
    }

  if(isnew)
    {
      status_msg->GetName()->Set(name);      
      statusmsgs.Add(status_msg);      
    }  

  return status_msg;
}


    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XTRACE_STATUS_MSG* XTRACE_STATUS_MSGS::StatusMsg_Get(XCHAR* name)
* @brief      StatusMsg_Get
* @ingroup    XUTILS
*
* @param[in]  name : 
* 
* @return     XTRACE_STATUS_MSG* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XTRACE_STATUS_MSG* XTRACE_STATUS_MSGS::StatusMsg_Get(XCHAR* name)
{
  if(statusmsgs.IsEmpty()) return NULL;

  for(XDWORD c=0; c<statusmsgs.GetSize(); c++)
    {
      XTRACE_STATUS_MSG* status_msg = statusmsgs.Get(c);
      if(status_msg)
        {
          if(!status_msg->GetName()->Compare(name, true)) return status_msg;
        }
    }

  return NULL;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XTRACE_STATUS_MSG* XTRACE_STATUS_MSGS::StatusMsg_Get(int index)
* @brief      StatusMsg_Get
* @ingroup    XUTILS
*
* @param[in]  index : 
* 
* @return     XTRACE_STATUS_MSG* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XTRACE_STATUS_MSG* XTRACE_STATUS_MSGS::StatusMsg_Get(int index)
{
  if(statusmsgs.IsEmpty()) return NULL;

  XTRACE_STATUS_MSG* status_msg = statusmsgs.Get(index);
  return status_msg;  
}




/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XTRACE_STATUS_MSG*>* XTRACE_STATUS_MSGS::StatusMsg_GetAll()
* @brief      StatusMsg_GetAll
* @ingroup    XUTILS
*
* @return     XVECTOR<XTRACE_STATUS_MSG*>* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XTRACE_STATUS_MSG*>* XTRACE_STATUS_MSGS::StatusMsg_GetAll()
{
  return &statusmsgs;
}




/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XTRACE_STATUS_MSGS::StatusMsg_DeleteAll()
* @brief      StatusMsg_DeleteAll
* @ingroup    XUTILS
*
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XTRACE_STATUS_MSGS::StatusMsg_DeleteAll()
{
  if(statusmsgs.IsEmpty()) return false;

  statusmsgs.DeleteContents();
  statusmsgs.DeleteAll();

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void XTRACE_STATUS_MSGS::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* @return     void : does not return anything. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void XTRACE_STATUS_MSGS::Clean()
{

}



/*--------------------------------------------------------------------------------------------------------------------*/
/*  XTRACE                                                                                                            */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XTRACE::XTRACE()
* @brief      Constructor
* @ingroup    XUTILS
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XTRACE::XTRACE()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XTRACE::~XTRACE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XTRACE::~XTRACE()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::SetTarget(int index, XTRACE_TYPE type, XCHAR* aim)
* @brief      Set Target by index
* @ingroup    XUTILS
*
* @param[in]  index : index to set target
* @param[in]  type : type of the target
* @param[in]  aim : aim of the target
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::SetTarget(int index, XTRACE_TYPE type, XCHAR* aim)
{
  if(index >= XTRACE_MAXNTARGETS) return false;

  if(!aim) return false;

  targets[index].SetType(type);

  if(type == XTRACE_TYPE_NET)
    {
      #if(defined(DIO_ACTIVE) && defined(DIO_STREAMUDP_ACTIVE))

      XSTRING string;
      XSTRING url;
      int     port;

      string = aim;
      url.AdjustSize(_MAXSTR);
      string.UnFormat(__L("%s:%d"), url.Get(), &port);
      url.AdjustSize();

      targets[index].SetAim(url.Get());
      targets[index].SetPort(port);
      targets[index].IPTarget();

      if(targets[index].GetNETHandle()) CloseHandleNet(&targets[index]);
      GetHandleNet(&targets[index]);

      if(!localIP)    ObtainLocalIP();

      #ifndef XTRACE_NOINTERNET
      if(!publicIP)   ObtainPublicIP();
      #endif

      #endif
    }
   else
    {
      targets[index].SetAim(aim);
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::AddTarget(XTRACE_TYPE type, XCHAR* aim)
* @brief      Add target (first index free)
* @ingroup    XUTILS
*
* @param[in]  type : type of the target
* @param[in]  aim : aim of the target
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::AddTarget(XTRACE_TYPE type, XCHAR* aim)
{
  for(int c=0; c<XTRACE_MAXNTARGETS; c++)
    {
      if(targets[c].GetType() == XTRACE_TYPE_NONE)  return SetTarget(c, type, aim);
    }

  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XTRACE::DeactivateAllTargets()
* @brief      DeactivateAllTargets
* @ingroup    XUTILS
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::DeactivateAllTargets()
{
  for(int c=0; c<XTRACE_MAXNTARGETS; c++)
    {
      targets[c].Clean();
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD XTRACE::GetSizeLimit()
* @brief      Get size limit
* @ingroup    XUTILS
*
* @return     XDWORD : size limit
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD XTRACE::GetSizeLimit()
{
  return sizelimit;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::SetSizeLimit(XDWORD sizelimit)
* @brief      Set size limit
* @ingroup    XUTILS
*
* @param[in]  sizelimit : new size limit
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::SetSizeLimit(XDWORD sizelimit)
{
  if(!sizelimit) return false;

  this->sizelimit = sizelimit;
  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* XTRACE::GetApplicationName()
* @brief      Get Application Name
* @ingroup    XUTILS
*
* @return     XCHAR* : application name
*
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* XTRACE::GetApplicationName()
{
  return applicationname;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE::SetApplicationName(XCHAR* applicationname)
* @brief      Set Application Name
* @ingroup    XUTILS
*
* @param[in]  applicationname : new application name
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE::SetApplicationName(XCHAR* applicationname)
{
  XDWORD size = 0;
  if(applicationname) size=XSTRING::GetSize(applicationname);

  memset(this->applicationname, 0, _MAXSTR * sizeof(XCHAR));
  if(applicationname) memcpy(this->applicationname, applicationname, (size + 1) * sizeof(XCHAR));
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE::SetApplicationName(XCHAR* applicationname, XDWORD size)
* @brief      Set Application Name
* @ingroup    XUTILS
*
* @param[in]  applicationname : new application name
* @param[in]  size : size of application name
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE::SetApplicationName(XCHAR* applicationname, XDWORD size)
{
  memset(this->applicationname, 0, _MAXSTR*sizeof(XCHAR));
  if(applicationname) memcpy(this->applicationname, applicationname, (size+1)*sizeof(XCHAR));
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE::SetApplicationName(XSTRING& applicationname)
* @brief      Set Application name
* @ingroup    XUTILS
*
* @param[in]  applicationname : new application name
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE::SetApplicationName(XSTRING& applicationname)
{
  SetApplicationName(applicationname.Get(), applicationname.GetSize());
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE::GetApplicationVersion(int& applicationversion, int& applicationsubversion, int& applicationsubversionerr)
* @brief      Get Application version (three data)
* @ingroup    XUTILS
*
* @param[in]  applicationversion : version number
* @param[in]  applicationsubversion : subversion number
* @param[in]  applicationsubversionerr : subversion error number
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE::GetApplicationVersion(int& applicationversion, int& applicationsubversion, int& applicationsubversionerr)
{
  applicationversion        = this->applicationversion;
  applicationsubversion     = this->applicationsubversion;
  applicationsubversionerr  = this->applicationsubversionerr;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE::SetApplicationVersion(int applicationversion, int applicationsubversion, int applicationsubversionerr)
* @brief      Set Application version
* @ingroup    XUTILS
*
* @param[in]  applicationversion : new version number
* @param[in]  applicationsubversion : new subversion number
* @param[in]  applicationsubversionerr : new subversion error number
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE::SetApplicationVersion(int applicationversion, int applicationsubversion, int applicationsubversionerr)
{
  this->applicationversion        = applicationversion;
  this->applicationsubversion     = applicationsubversion;
  this->applicationsubversionerr  = applicationsubversionerr;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* XTRACE::GetApplicationID()
* @brief      Get Application ID
* @ingroup    XUTILS
*
* @return     XCHAR* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* XTRACE::GetApplicationID()
{
  return applicationID;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE::SetApplicationID(XCHAR* applicationID, XDWORD size)
* @brief      Set Application ID
* @ingroup    XUTILS
*
* @param[in]  applicationID : new string application ID
* @param[in]  size : size application ID
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE::SetApplicationID(XCHAR* applicationID, XDWORD size)
{
  memset(this->applicationID, 0, _MAXSTR*sizeof(XCHAR)),
  memcpy(this->applicationID, applicationID, (size+1)*sizeof(XCHAR));
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE::SetApplicationID(XSTRING& applicationID)
* @brief      Set Application ID
* @ingroup    XUTILS
*
* @param[in]  applicationID : new string application ID
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE::SetApplicationID(XSTRING& applicationID)
{
  SetApplicationID(applicationID.Get(), applicationID.GetSize());
}




/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XTRACE::ClearScreen(XBYTE level)
* @brief      ClearScreen
* @ingroup    XUTILS
*
* @param[in]  level : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::ClearScreen(XBYTE level)
{
  XSTRING string;

  string.Format(__L("%s"), XTRACE_IDMSGCLEARSCREEN);

  return Print(level, string.Get());
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE::PrintHeader(XCHAR* header)
* @brief      Print Header
* @ingroup    XUTILS
*
* @param[in]  header : header string
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE::PrintHeader(XCHAR* header)
{
  XSTRING line;

  line.Empty();

  line  = __L("---");

  if(header)
    {
      line += __L(" ");
      line += header;
      line += __L(" ");
    }

  while(line.GetSize()<120)
    {
      line += __L("-");
    }

  Print(XTRACE_LEVEL_WITHCOLOR,line.Get());

  openheader = header?true:false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::Print(XBYTE level, XCHAR* mask,...)
* @brief      Print format line
* @ingroup    XUTILS
*
* @param[in]  level : level of line
* @param[in]  mask : mask of format of print
* @param[in]  ... : variables to print
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::Print(XBYTE level, XCHAR* mask,...)
{
  XSTRING string;
  XSTRING outstring;

  string.Add(applicationname);
  if(string.GetSize())
    {
      string.AdjustSize(XTRACE_MAXAPPLICATIONNAMESIZE, false, __L(" "));
      outstring += string;
      outstring += __L(" ");
    }

  string.Format(__L("%d.%d.%d"), applicationversion, applicationsubversion, applicationsubversionerr);
  if(string.GetSize())
    {
      string.AdjustSize(XTRACE_MAXAPPLICATIONVERSIONSIZE, false, __L(" "));      
      outstring += string;
      outstring += __L(" ");
    }

  string = applicationID;
  if(string.GetSize())
    {
      string.AdjustSize(XTRACE_MAXAPPLICATIONIDSIZE, false, __L(" "));
      outstring += string;
      outstring += __L(" ");
    }

  va_list arg;

  va_start(arg, mask);

  string.FormatArg(mask, &arg);

  va_end(arg);

  sequence++;

  
  if(level)
    {
      if((level & XTRACE_LEVEL_WITHCOLOR) == XTRACE_LEVEL_WITHCOLOR)
        {

        }
        else
        {
          if((level & XTRACE_LEVEL_WITHTAB) == XTRACE_LEVEL_WITHTAB)
            {
              XSTRING tabstr;
              GenerateTab(level&0x0F, tabstr);

              string.Insert(tabstr, 0);

            }
            else
            {
              if((level & XTRACE_LEVEL_WITHCODE) == XTRACE_LEVEL_WITHCODE)
                {
                  XSTRING codestr;

                  codestr.Format(__L("%02d: "), (level&0x0F));

                  string.Insert(codestr, 0);
                }
            }
        }
    }
  
  outstring += string;

  for(int c=0; c<XTRACE_MAXNTARGETS ; c++)
    {
      switch(targets[c].GetType())
        {
          case XTRACE_TYPE_SPECIAL   : PrintSpecial(&targets[c], level, outstring.Get());  break;
          case XTRACE_TYPE_FILE      : PrintFile(&targets[c], level, outstring.Get());     break;
          case XTRACE_TYPE_NET       : PrintNet(&targets[c], level, outstring.Get());      break;
                          default     : break;
        }
    }

  return true;
}





/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::PrintDataBlock(XBYTE level, XBYTE* data, XDWORD size, XDWORD marginsize, XDWORD sizeline, bool showoffset, bool showtext)
* @brief      Print data block
* @ingroup    XUTILS
*
* @param[in]  level : level of line
* @param[in]  data : buffer of data to print
* @param[in]  size : size of buffer
* @param[in]  marginsize : size of margin
* @param[in]  sizeline : size of line
* @param[in]  showoffset : true if had offset
* @param[in]  showtext : true if show assci of buffer
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::PrintDataBlock(XBYTE level, XBYTE* data, XDWORD size, XDWORD marginsize, XDWORD sizeline, bool showoffset, bool showtext)
{
  XSTRING    margin;
  XSTRING    strdata;
  XDWORD     _size     = 0;
  XDWORD     _sizeline = sizeline;
  int        index     = 0;

  for(XDWORD c=0; c<marginsize; c++)
    {
      margin += __L(" ");
    }

  while(_size<size)
    {
      XSTRING string;

      string   = margin;
      if(_sizeline > (size - _size))
         _sizeline = (size - _size);

      if(showoffset)
        {
          strdata.Format(__L("%04X"),index);
          string += strdata;
          string += __L("   ");
        }

      for(XDWORD c=0; c<_sizeline; c++)
        {
          strdata.Format(__L("%02X "),data[index]);
          string += strdata;
          _size++;
          index++;
        }

      //if(_sizeline != sizeline)
        {
          for(XDWORD c=_sizeline; c<sizeline; c++)
            {
              string += __L("   ");
            }
        }


      if(showtext)
        {
          index -= _sizeline;
          string += __L(" ");

          for(XDWORD c=0; c<_sizeline; c++)
            {
              XCHAR character = (XCHAR)data[index];

              if((character<0x20) || (character>0x80) || (character==__C('%')) || (character==__C('\\'))) character = __C('.');

              strdata.Format(__L("%c"),character);
              string += strdata;
              index++;
            }
        }

      Print(level, string.Get());

      if(_size>XTRACE_MAXSIZEDATABLOCK) break;
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::PrintDataBlock(XBYTE level, XBUFFER& data, XDWORD marginsize, XDWORD sizeline, bool showoffset, bool showtext)
* @brief      Print data block
* @ingroup    XUTILS
*
* @param[in]  level : level of line
* @param[in]  data : buffer of data to print
* @param[in]  marginsize : size of margin
* @param[in]  sizeline : size of line
* @param[in]  showoffset : true if had offset
* @param[in]  showtext : true if show assci of buffer
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::PrintDataBlock(XBYTE level, XBUFFER& data, XDWORD marginsize, XDWORD sizeline, bool showoffset, bool showtext)
{
  return PrintDataBlock(level,data.Get(), data.GetSize(), marginsize, sizeline, showoffset, showtext);
}




/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XTRACE::ClearMsgsStatus(XBYTE level)
* @brief      ClearMsgsStatus
* @ingroup    XUTILS
*
* @param[in]  level : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::ClearMsgsStatus(XBYTE level)
{
  XSTRING string;

  string.Format(__L("%s"), XTRACE_IDMSGSTATUSCLEAR);

  return Print(level, string.Get());
}





/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::PrintMsgStatus(XBYTE level, XCHAR* name, bool value)
* @brief      PrintMsgStatus
* @ingroup    XUTILS
*
* @param[in]  level :
* @param[in]  name :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
* ---------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::PrintMsgStatus(XBYTE level, XCHAR* name, bool value)
{
  XSTRING string;

  string.Format(__L("%s,%s,%c,%s"), XTRACE_IDMSGSTATUS, name, XTRACE_IDMSGSTATUS_BOOLEAN,(value?__L("true"):__L("false")));

  return Print(level, string.Get());
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XTRACE::PrintMsgStatus(XBYTE level, XCHAR* name, int value)
* @brief      PrintMsgStatus
* @ingroup    XUTILS
*
* @param[in]  level : 
* @param[in]  name : 
* @param[in]  value : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::PrintMsgStatus(XBYTE level, XCHAR* name, int value)
{
   XSTRING string;

  string.Format(__L("%s,%s,%c,%d"), XTRACE_IDMSGSTATUS, name, XTRACE_IDMSGSTATUS_INTEGER, value);

  return Print(level, string.Get());
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::PrintMsgStatus(XBYTE level, XCHAR* name, XCHAR* value)
* @brief      PrintMsgStatus
* @ingroup    XUTILS
*
* @param[in]  level :
* @param[in]  name :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
* ---------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::PrintMsgStatus(XBYTE level, XCHAR* name, XCHAR* value)
{
  XSTRING string;

  string.Format(__L("%s,%s,%c,%s"), XTRACE_IDMSGSTATUS, name, XTRACE_IDMSGSTATUS_STRING, value);

  return Print(level, string.Get());
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::PrintMsgStatus(XBYTE level, XCHAR* name, XDWORD value)
* @brief      PrintMsgStatus
* @ingroup    XUTILS
*
* @param[in]  level :
* @param[in]  name :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
* ---------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::PrintMsgStatus(XBYTE level, XCHAR* name, XDWORD value)
{
  XSTRING string;

  string.Format(__L("%s,%s,%c,%d"), XTRACE_IDMSGSTATUS, name, XTRACE_IDMSGSTATUS_DWORD, value);

  return Print(level, string.Get());
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::PrintMsgStatus(XBYTE level, XCHAR* name, float value)
* @brief      PrintMsgStatus
* @ingroup    XUTILS
*
* @param[in]  level :
* @param[in]  name :
* @param[in]  value :
*
* @return     bool : true if is succesful.
*
* ---------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::PrintMsgStatus(XBYTE level, XCHAR* name, float value)
{
  XSTRING string;

  string.Format(__L("%s,%s,%c,%f"), XTRACE_IDMSGSTATUS, name, XTRACE_IDMSGSTATUS_FLOAT, value);

  return Print(level, string.Get());
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::PrintMsgStatus(XBYTE level, XCHAR* name, XBYTE value[3])
* @brief      PrintMsgStatus
* @ingroup    XUTILS
*
* @param[in]  level :
* @param[in]  name :
* @param[in]  value[3] :
*
* @return     bool : true if is succesful.
*
* ---------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::PrintMsgStatus(XBYTE level, XCHAR* name, XBYTE value[3])
{
  XSTRING string;

  string.Format(__L("%s,%s,%c,%02X,%02X,%02X"), XTRACE_IDMSGSTATUS, name, XTRACE_IDMSGSTATUS_COLOR, value[0], value[1], value[2]);

  return Print(level, string.Get());
}





/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE::PrintSpecial(XTRACE_TARGET* target, XBYTE level, XCHAR* string)
* @brief      Print Special aim
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @param[in]  target : target to print the debug
* @param[in]  level : level of debug
* @param[in]  string : string with the line
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE::PrintSpecial(XTRACE_TARGET* target, XBYTE level, XCHAR* string)
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE::PrintFile(XTRACE_TARGET* target, XBYTE level, XCHAR* string)
* @brief      Print in file aim
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @param[in]  target : target to print the debug
* @param[in]  level : level of debug
* @param[in]  string : string with the line
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE::PrintFile(XTRACE_TARGET* target, XBYTE level, XCHAR* string)
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE::PrintNet(XTRACE_TARGET* target, XBYTE level, XCHAR* string)
* @brief      Print net aim
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @param[in]  target : target to print the debug
* @param[in]  level : level of debug
* @param[in]  string : string with the line
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE::PrintNet(XTRACE_TARGET* target, XBYTE level, XCHAR* string)
{

}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::GetHandleNet(XTRACE_TARGET* target)
* @brief      Get Handle NET (depends of plataform)
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @param[in]  target : target of debug
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::GetHandleNet(XTRACE_TARGET* target)
{
  return false;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::CloseHandleNet(XTRACE_TARGET* target)
* @brief      Close Handle NET (depends of plataform)
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @param[in]  target : target of debug
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::CloseHandleNet(XTRACE_TARGET* target)
{
  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD XTRACE::GetTraceFromXBuffer(XBUFFER& xbufferpacket, XDWORD& publicIP, XDWORD& localIP, XBYTE& level, XDWORD& sequence, XDATETIME* xtime, XBUFFER& data)
* @brief      GetTraceFromXBuffer
* @ingroup    XUTILS
*
* @param[in]  xbufferpacket : 
* @param[in]  publicIP : 
* @param[in]  localIP : 
* @param[in]  level : 
* @param[in]  sequence : 
* @param[in]  xtime : 
* @param[in]  data : 
* 
* @return     XDWORD : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
XDWORD XTRACE::GetTraceFromXBuffer(XBUFFER& xbufferpacket, XDWORD& publicIP, XDWORD& localIP, XBYTE& level, XDWORD& sequence, XDATETIME* xtime, XBUFFER& data)
{
  if(!xtime)                   return 1;
  if(!xbufferpacket.GetSize()) return 1;

  XDWORD IDpacket = XTRACE_IDPACKET;
  
  XDWORD    sizepacketread  = 0;
  XDWORD    dword;
  XDWORD    sizeinbuffer    = xbufferpacket.GetSize();
  XBUFFER   xbuffer(false);
  XDWORD    sizeheader      = (sizeof(XDWORD)    +    // ID.
                               sizeof(XDWORD)    +    // SizePacket.
                               sizeof(XDWORD)    +    // public IP address origin.
                               sizeof(XDWORD)    +    // local  IP address origin.
                               sizeof(XBYTE)     +    // Level.
                               sizeof(XDWORD)    +    // Sequence.
                              (sizeof(XDWORD)*7) +    // Date Time.
                               sizeof(XWORD));        // CRC16

  if(!sizeinbuffer)             return 1;
  if(sizeinbuffer < sizeheader) return 1;


  while(sizeinbuffer-4)
    {
      xbufferpacket.Get(IDpacket, 0);
      if(IDpacket == XTRACE_IDPACKET) break;
     
      xbufferpacket.Extract(NULL, 0, 1);
      sizeinbuffer--;
    }

  if(IDpacket != XTRACE_IDPACKET) return 1;

  if(!sizeinbuffer)               return 1;
  if(sizeinbuffer < sizeheader)   return 1;

  if(!xbufferpacket.Get(sizepacketread, sizeof(IDpacket)))
    {
      return 1;
    }

  if(!sizepacketread)
    {
      return 2;
    }

  if(sizepacketread > sizeinbuffer)
    {
      return 1;
    }

 
  XDWORD sizeassing    = sizepacketread;
  XDWORD sizeassingmin = 5*1024;

  if(sizeassing < sizeassingmin) sizeassing = sizeassingmin;
  xbuffer.Resize(sizeassing);

  xbufferpacket.Extract(xbuffer.Get(), 0, sizepacketread);

  XWORD crc16;
  XTRACE_CRC16(xbuffer.Get(), (xbuffer.GetSize()-sizeof(XWORD)), crc16)

  XWORD crc16read;

  xbuffer.Get(crc16read, xbuffer.GetSize()-sizeof(XWORD));
  if(crc16 != crc16read)
    {
      return 2;
    }

  xbuffer.SetPosition(0);

  xbuffer.Get(IDpacket);
  xbuffer.Get(sizepacketread);
  xbuffer.Get(publicIP);
  xbuffer.Get(localIP);
  xbuffer.Get(level);
  xbuffer.Get(sequence);

  xbuffer.Get(dword); xtime->SetYear(dword);
  xbuffer.Get(dword); xtime->SetMonth(dword);
  xbuffer.Get(dword); xtime->SetDay(dword);
  xbuffer.Get(dword); xtime->SetHours(dword);
  xbuffer.Get(dword); xtime->SetMinutes(dword);
  xbuffer.Get(dword); xtime->SetSeconds(dword);
  xbuffer.Get(dword); xtime->SetMilliSeconds(dword);

  XDWORD sizestr = sizepacketread - sizeheader;        // CRC16  
  data.Empty();
  data.Add(&xbuffer.Get()[xbuffer.GetPosition()], sizestr);


  return 0;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD XTRACE::GetTraceFromDIOStream(DIOSTREAM* DIOstream, XDWORD& publicIP, XDWORD& localIP, XBYTE& level, XDWORD& sequence, XDATETIME* xtime, XBUFFER& data)
* @brief      GetTraceFromDIOStream
* @ingroup    XUTILS
* 
* @param[in]  DIOstream : 
* @param[in]  publicIP : 
* @param[in]  localIP : 
* @param[in]  level : 
* @param[in]  sequence : 
* @param[in]  xtime : 
* @param[in]  data : 
* 
* @return     XDWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
#ifdef DIO_ACTIVE
XDWORD XTRACE::GetTraceFromDIOStream(DIOSTREAM* DIOstream, XDWORD& publicIP, XDWORD& localIP, XBYTE& level, XDWORD& sequence, XDATETIME* xtime, XBUFFER& data)
{
  if(!DIOstream) return false;
  if(!xtime)    return false;

  return GetTraceFromXBuffer((*DIOstream->GetInXBuffer()), publicIP, localIP, level, sequence, xtime, data);
}
#endif


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XTRACE::SetTraceDataToText(XBUFFER& data, XSTRING& string)
* @brief      SetTraceDataToText
* @ingroup    XUTILS
*
* @param[in]  data : 
* @param[in]  string : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::SetTraceDataToText(XBUFFER& data, XSTRING& string)
{
  string.Empty();

  for(XDWORD c=0; c<data.GetSize()/2; c++)
    {
      XWORD  word = 0;

      data.Get(word);
      string.Add((XCHAR)word);
    }  

  return (string.IsEmpty()?false:true);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XTRACE::SetTraceToXBuffer(XDWORD publicIP, XDWORD localIP, XBYTE level, XDWORD sequence, XDATETIME* xtime, XDWORD& sizepacket, XBUFFER& xbufferpacket)
* @brief      SetTraceToXBuffer
* @ingroup    XUTILS
*
* @param[in]  publicIP : 
* @param[in]  localIP : 
* @param[in]  level : 
* @param[in]  sequence : 
* @param[in]  xtime : 
* @param[in]  sizepacket : 
* @param[in]  xbufferpacket : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::SetTraceToXBuffer(XDWORD publicIP, XDWORD localIP, XBYTE level, XDWORD sequence, XDATETIME* xtime, XDWORD& sizepacket, XBUFFER& xbufferpacket)
{
  if(!xtime)  return false;
    
  xbufferpacket.Empty();

  XDWORD IDpacket = XTRACE_IDPACKET;
 
  sizepacket += sizeof(XDWORD) +                        // ID.
                sizeof(XDWORD) +                        // SizePacket.
                sizeof(XDWORD) +                        // public IP address origin.
                sizeof(XDWORD) +                        // local  IP address origin.
                sizeof(XBYTE)  +                        // Level.
                sizeof(XDWORD) +                        // Sequence.
                (sizeof(XDWORD)*7) +                    // Date Time.                
                sizeof(XWORD);                          // CRC16

  xbufferpacket.Resize(sizepacket);

  xbufferpacket.Set(IDpacket);  
  xbufferpacket.Set(sizepacket);
  xbufferpacket.Set(publicIP);
  xbufferpacket.Set(localIP);
  xbufferpacket.Set(level);
  xbufferpacket.Set(sequence);

  xbufferpacket.Set((XDWORD)xtime->GetYear());
  xbufferpacket.Set((XDWORD)xtime->GetMonth());
  xbufferpacket.Set((XDWORD)xtime->GetDay());
  xbufferpacket.Set((XDWORD)xtime->GetHours());
  xbufferpacket.Set((XDWORD)xtime->GetMinutes());
  xbufferpacket.Set((XDWORD)xtime->GetSeconds());
  xbufferpacket.Set((XDWORD)xtime->GetMilliSeconds());

  return true;  
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::SetTraceTextToXBuffer(XDWORD publicIP, XDWORD localIP, XBYTE level, XDWORD sequence, XDATETIME* xtime, XCHAR* string, XBUFFER& xbufferpacket)
* @brief      Set Trace Text pack to XBuffer
* @ingroup    XUTILS
*
* @param[in]  publicIP : Public IP (in DWORD format)
* @param[in]  localIP : Local IP (in DWORD format)
* @param[in]  level : Level of debug
* @param[in]  sequence : sequence of debug
* @param[in]  xtime : date/time of line
* @param[in]  string : string with of line
* @param[out] xbufferpacket : buffer with debug pack
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::SetTraceTextToXBuffer(XDWORD publicIP, XDWORD localIP, XBYTE level, XDWORD sequence, XDATETIME* xtime, XCHAR* string, XBUFFER& xbufferpacket)
{     
  if(!string) return false;
  
  XSTRING  line;
  XBUFFER  bufferline;

  line = string;  
  line.ConvertToExchangeXBuffer(bufferline);
  
  XDWORD sizepacket = bufferline.GetSize();    // Line (WORD) each character.
  
  if(!SetTraceToXBuffer(publicIP, localIP, level, sequence, xtime, sizepacket, xbufferpacket)) return false;
  
  xbufferpacket.Set(bufferline.Get(), bufferline.GetSize());
  
  XWORD crc16;
  XTRACE_CRC16(xbufferpacket.Get(), (xbufferpacket.GetSize()-sizeof(XWORD)), crc16)

  xbufferpacket.Set((XWORD)crc16);
    
  return true; 
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE::SetLocalIPFilter(XBYTE localIPfilter)
* @brief      SetLocalIPFilter
* @ingroup    XUTILS
*
* @param[in]  localIPfilter :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE::SetLocalIPFilter(XBYTE localIPfilter)
{
  this->localIPfilter = localIPfilter;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::ObtainLocalIP()
* @brief      Obtain Local IP
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::ObtainLocalIP()
{
  bool status = false;

  #if(defined(DIO_ACTIVE) && defined(DIO_STREAMUDP_ACTIVE))

  DIOSTREAMIPLOCALENUMDEVICES* enumdevices = (DIOSTREAMIPLOCALENUMDEVICES*)GEN_DIOFACTORY.CreateStreamEnumDevices(DIOSTREAMENUMTYPE_IP_LOCAL);
  if(enumdevices)
    {
      if(!localIPfilter)
        {
          DIOSTREAMDEVICEIP* device = (DIOSTREAMDEVICEIP*)enumdevices->GetFirstActiveDevice();
          if(device)
            {
              XSTRING string;

              device->GetIP()->GetXString(string);
              SetLocalIPString(string.Get());

              status = true;
            }
        }
       else
        {
          enumdevices->Search();

          for(XDWORD c=0; c<enumdevices->GetDevices()->GetSize(); c++)
            {
              DIOSTREAMDEVICEIP* device = (DIOSTREAMDEVICEIP*)enumdevices->GetDevices()->Get(c);
              if(device)
                {
                  //device->DebugPrintInfo();

                  if((!device->GetIP()->IsEmpty()) &&
                     ((device->GetIPType() == DIOSTREAMIPDEVICE_TYPE_ETHERNET) || (device->GetIPType() == DIOSTREAMIPDEVICE_TYPE_WIFI) || (device->GetIPType() == DIOSTREAMIPDEVICE_TYPE_PPP)) &&
                      (device->GetIP()->Get()[0] == localIPfilter))
                    {
                      XSTRING string;

                      device->GetIP()->GetXString(string);
                      SetLocalIPString(string.Get());
                      status = true;
                    }
                }
            }
        }

      GEN_DIOFACTORY.DeleteStreamEnumDevices(enumdevices);
    }

  #endif

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::SetLocalIPString(XSTRING* localIPstring)
* @brief      Set Local IP (in string format)
* @ingroup    XUTILS
*
* @param[in]  localIPstring : new local ip in string format
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::SetLocalIPString(XCHAR* localIPstring)
{
  XSTRING _localIPstring;
  int data[4];

  _localIPstring = localIPstring;
  _localIPstring.UnFormat(__L("%d.%d.%d.%d"), &data[0], &data[1], &data[2], &data[3]);

  localIP  |= (data[0] << 24);
  localIP  |= (data[1] << 16);
  localIP  |= (data[2] << 8);
  localIP  |= (data[3]);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD XTRACE::GetLocalIP()
* @brief      Get Local IP (in DWORD format)
* @ingroup    XUTILS
*
* @return     XDWORD : local IP in DWORD format
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD XTRACE::GetLocalIP()
{
  return localIP;
}


#ifndef XTRACE_NOINTERNET
/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::ObtainPublicIP()
* @brief      Obtain Public IP
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::ObtainPublicIP()
{
  bool  status = false;

  #if(defined(DIO_ACTIVE) && defined(DIO_STREAMUDP_ACTIVE))

  DIOIP                  ip;
  DIOSCRAPERWEBPUBLICIP* publicip = new DIOSCRAPERWEBPUBLICIP();
  if(!publicip) return false;

  if(publicip->GetURLDownload()) publicip->GetURLDownload()->Set(scraperwebscripturldownload);

  if(publicip->Get(ip, 5))
    {
      XSTRING string;

      ip.GetXString(string);
      SetPublicIPString(string.Get());
    }

  delete publicip;

  #endif

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCHAR* XTRACE::GetScraperWebScriptURLDownload()
* @brief      Get Web Scrapper Script URL download
* @ingroup    XUTILS
*
* @return     XCHAR* : URL download
*
* --------------------------------------------------------------------------------------------------------------------*/
XCHAR* XTRACE::GetScraperWebScriptURLDownload()
{
  return scraperwebscripturldownload;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE::SetScraperWebScriptURLDownload(XCHAR* scraperwebscripturldownload)
* @brief      Get Web Scrapper Script URL download
* @ingroup    XUTILS
*
* @param[in]  scraperwebscripturldownload : URL download
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE::SetScraperWebScriptURLDownload(XCHAR* scraperwebscripturldownload)
{
  XDWORD size = 0;
  if(scraperwebscripturldownload) size = XSTRING::GetSize(scraperwebscripturldownload);

  memset(this->scraperwebscripturldownload, 0, _MAXSTR * sizeof(XCHAR));
  if(scraperwebscripturldownload) memcpy(this->scraperwebscripturldownload, scraperwebscripturldownload, (size + 1) * sizeof(XCHAR));
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD XTRACE::GetPublicIP()
* @brief      Get Public IP (in DWORD format)
* @ingroup    XUTILS
*
* @return     XDWORD : public IP in DWORD format
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD XTRACE::GetPublicIP()
{
  return publicIP;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XTRACE::SetPublicIPString(XCHAR* publicIPstring)
* @brief      SetPublicIPString
* @ingroup    XUTILS
* 
* @param[in]  publicIPstring : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::SetPublicIPString(XCHAR* publicIPstring)
{
  XSTRING _publicIPstring;
  int data[4];

  _publicIPstring = publicIPstring;

  _publicIPstring.UnFormat(__L("%d.%d.%d.%d"), &data[0], &data[1], &data[2], &data[3]);

  publicIP  |= (data[0] << 24);
  publicIP  |= (data[1] << 16);
  publicIP  |= (data[2] << 8);
  publicIP  |= (data[3]);

  return true;
}

#endif


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::ObtainResourcesIP()
* @brief      Obtain Resources IP (public + local)
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::ObtainResourcesIP()
{
  if(!ObtainLocalIP())  return false;

  #ifndef XTRACE_NOINTERNET
  if(!ObtainPublicIP()) return false;
  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::ReConnectedAllNetTargets()
* @brief      Re connect all Net Targets
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::ReConnectedAllNetTargets()
{
  for(int c=0; c<XTRACE_MAXNTARGETS; c++)
    {
      if(targets[c].GetType() == XTRACE_TYPE_NET)
        {
          #if(defined(DIO_ACTIVE) && defined(DIO_STREAMUDP_ACTIVE))

          if(targets[c].GetNETHandle()) CloseHandleNet(&targets[c]);

          GetHandleNet(&targets[c]);

          #endif
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::ResolveAllNetTargets(bool& changed)
* @brief      Resolve All Net Targets
* @ingroup    XUTILS
*
* @param[out] changed : true if the target net changed IP
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::ResolveAllNetTargets(bool& changed)
{
  changed = false;

  for(int c=0; c<XTRACE_MAXNTARGETS; c++)
    {
      if(targets[c].GetType() == XTRACE_TYPE_NET)
        {
          #if(defined(DIO_ACTIVE) && defined(DIO_STREAMUDP_ACTIVE))

          XSTRING IP;

          IP = targets[c].GetIP();

          targets[c].IPTarget();

          if(IP.Compare(targets[c].GetIP())) changed = true;

          #endif
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::ResolveAllResources()
* @brief      Resolve All Resources local + public: (if chnaged obtain and reconnect)
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::ResolveAllResources()
{
  bool changed;

  ResolveAllNetTargets(changed);
  if(changed)
    {
      ObtainResourcesIP();
      ReConnectedAllNetTargets();
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XTRACE::GenerateTab(int ntab, XSTRING& tab)
* @brief      Generate Tab for the line
* @ingroup    XUTILS
*
* @param[in]  ntab : number of tab
* @param[out  tab : string with the tab generated
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XTRACE::GenerateTab(int ntab, XSTRING& tab)
{
  tab.Empty();

  for(int c=0;c<ntab;c++)
    {
      tab += __L("  ");
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XTRACE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    XUTILS
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XTRACE::Clean()
{
  instance                  = NULL;

  sizelimit                 = XTRACE_SIZELIMITDEFAULT;

  for(int c=0; c<_MAXSTR; c++)
    {
      applicationname[c] = 0;
      applicationID[c]   = 0;
    }

  applicationversion        = 0;
  applicationsubversion     = 0;
  applicationsubversionerr  = 0;

  #ifndef XTRACE_NOINTERNET
  publicIP                  = 0;
  #endif
  localIPfilter             = 0;
  localIP                   = 0;

  openheader                = false;
  sequence                  = 0;
}
