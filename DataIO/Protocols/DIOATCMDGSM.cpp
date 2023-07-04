/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIOATCMDGSM.cpp
*
* @class      DIOATCMDGSM
* @brief      Data Input/Output AT command for GSM class
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "XFactory.h"
#include "XFSMachine.h"
#include "XTrace.h"

#include "DIOStream.h"

#include "DIOATCMDGSM.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMDGSM_ALPHABET_8859_1::DIOATCMDGSM_ALPHABET_8859_1()
* @brief      Constructor
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMDGSM_ALPHABET_8859_1::DIOATCMDGSM_ALPHABET_8859_1()
{
  Clean();
  CreateTable();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMDGSM_ALPHABET_8859_1::~DIOATCMDGSM_ALPHABET_8859_1()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMDGSM_ALPHABET_8859_1::~DIOATCMDGSM_ALPHABET_8859_1()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOATCMDGSM_ALPHABET_8859_1::CreateTable()
* @brief      CreateTable
* @ingroup    DATAIO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOATCMDGSM_ALPHABET_8859_1::CreateTable()
{
  char table2[138][2] = { // Normal Characters
                          { 0x00,'@'   }, { 0x10,'?'   },{ 0x20,' '   },{ 0x30,'0'   },{ 0x40,'�'   },{ 0x50,'P'   },{ 0x60,'�'   },{ 0x70,'p'   },
                          { 0x01,'�'   }, { 0x11,'_'   },{ 0x21,'!'   },{ 0x31,'1'   },{ 0x41,'A'   },{ 0x51,'Q'   },{ 0x61,'a'   },{ 0x71,'q'   },
                          { 0x02,'$'   }, { 0x12,'F'   },{ 0x22,'"'   },{ 0x32,'2'   },{ 0x42,'B'   },{ 0x52,'R'   },{ 0x62,'b'   },{ 0x72,'r'   },
                          { 0x03,'�'   }, { 0x13,'G'   },{ 0x23,'#'   },{ 0x33,'3'   },{ 0x43,'C'   },{ 0x53,'S'   },{ 0x63,'c'   },{ 0x73,'s'   },
                          { 0x04,'�'   }, { 0x14,'?'   },{ 0x24,'�'   },{ 0x34,'4'   },{ 0x44,'D'   },{ 0x54,'T'   },{ 0x64,'d'   },{ 0x74,'t'   },
                          { 0x05,'�'   }, { 0x15,'O'   },{ 0x25,'%'   },{ 0x35,'5'   },{ 0x45,'E'   },{ 0x55,'U'   },{ 0x65,'e'   },{ 0x75,'u'   },
                          { 0x06,'�'   }, { 0x16,'?'   },{ 0x26,'&'   },{ 0x36,'6'   },{ 0x46,'F'   },{ 0x56,'V'   },{ 0x66,'f'   },{ 0x76,'v'   },
                          { 0x07,'�'   }, { 0x17,'?'   },{ 0x27,'\\'  },{ 0x37,'7'   },{ 0x47,'G'   },{ 0x57,'W'   },{ 0x67,'g'   },{ 0x77,'w'   },
                          { 0x08,'�'   }, { 0x18,'S'   },{ 0x28,'('   },{ 0x38,'8'   },{ 0x48,'H'   },{ 0x58,'X'   },{ 0x68,'h'   },{ 0x78,'x'   },
                          { 0x09,'�'   }, { 0x19,'T'   },{ 0x29,')'   },{ 0x39,'9'   },{ 0x49,'I'   },{ 0x59,'Y'   },{ 0x69,'i'   },{ 0x79,'y'   },
                          { 0x0A,'\r'  }, { 0x1A,'?'   },{ 0x2A,'*'   },{ 0x3A,':'   },{ 0x4A,'J'   },{ 0x5A,'Z'   },{ 0x6A,'j'   },{ 0x7A,'z'   },
                          { 0x0B,'�'   }, { 0x1B,0x1B  },{ 0x2B,'+'   },{ 0x3B,';'   },{ 0x4B,'K'   },{ 0x5B,'�'   },{ 0x6B,'k'   },{ 0x7B,'�'   },
                          { 0x0C,'�'   }, { 0x1C,'�'   },{ 0x2C,','   },{ 0x3C,'<'   },{ 0x4C,'L'   },{ 0x5C,'�'   },{ 0x6C,'l'   },{ 0x7C,'�'   },
                          { 0x0D,'\n'  }, { 0x1D,'�'   },{ 0x2D,'-'   },{ 0x3D,'='   },{ 0x4D,'M'   },{ 0x5D,'�'   },{ 0x6D,'m'   },{ 0x7D,'�'   },
                          { 0x0E,'�'   }, { 0x1E,'�'   },{ 0x2E,'.'   },{ 0x3E,'>'   },{ 0x4E,'N'   },{ 0x5E,'�'   },{ 0x6E,'n'   },{ 0x7E,'�'   },
                          { 0x0F,'�'   }, { 0x1F,'�'   },{ 0x2F,'/'   },{ 0x3F,'?'   },{ 0x4F,'O'   },{ 0x5F,'�'   },{ 0x6F,'o'   },{ 0x7F,'�'   },
                          // Extended Characters
                          { 0x0A,'\t'  },
                          { 0x14,'^'   },
                          { 0x28,'{'   },
                          { 0x29,'}'   },
                          { 0x2F,'\\'  },
                          { 0x3C,'['   },
                          { 0x3D,'~'   },
                          { 0x3E,']'   },
                          { 0x40,'|'   },
                          { 0x65,'�'   }
                        };

  for(int c=0;c<138;c++)
    {
      table[c][0] = table2[c][0];
      table[c][1] = table2[c][1];
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWORD DIOATCMDGSM_ALPHABET_8859_1::Traslate(bool from, XWORD data)
* @brief      Traslate
* @ingroup    DATAIO
*
* @param[in]  from :
* @param[in]  data :
*
* @return     XWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XWORD DIOATCMDGSM_ALPHABET_8859_1::Traslate(bool from, XWORD data)
{
  XDWORD end         = (sizeof(table)/sizeof(char[2]));
  char  extended   = ((data&0xFF00)>>8);
  char  character  = (data&0x00FF);
  char  escape     = 0x1B;
  XDWORD c;

  if(from)
    {
      for(c=0;c<127;c++)
        {
          if(table[c][1]==character) return (XWORD)table[c][0];
        }

      for(c=128;c<end;c++)
        {
          if(table[c][1]==character) return (XWORD)((escape<<8)|table[c][0]);
        }
    }
   else
    {
      int first =0;

      if(extended==escape)
            first = 128;
       else end  = 127;

      for(c=first;c<end;c++)
        {
          if(table[c][0]==character) return (XWORD)table[c][1];
        }
     }

  return 0xFF00;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOATCMDGSM_ALPHABET_8859_1::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOATCMDGSM_ALPHABET_8859_1::Clean()
{
  memset(table,0, 138*2);
}




/* --------------------------------------------------------------------------------------------------------------------*/
/* DIOATCMDGSM                                                                                                         */
/* --------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMDGSM::DIOATCMDGSM(DIOSTREAM* diostream)
* @brief      Constructor
* @ingroup    DATAIO
*
* @param[in]  DIOSTREAM* :
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMDGSM::DIOATCMDGSM(DIOSTREAM* diostream) : DIOATCMDS(diostream)
{
  Clean();

  AddCommand(DIOATCMD_TYPE_GSMGETMANUFACTURED       , __L("+GMI"));
  AddCommand(DIOATCMD_TYPE_GSMGETMODEL              , __L("+GMM"));
  AddCommand(DIOATCMD_TYPE_GSMGETSERIAL             , __L("+GSN"));
  AddCommand(DIOATCMD_TYPE_GSMGETVERSION            , __L("+GMR"));
  AddCommand(DIOATCMD_TYPE_GSMGETSIGNALQUALITY      , __L("+CSQ"));
  AddCommand(DIOATCMD_TYPE_GSMGETSERVICES           , __L("+GCAP"));
  AddCommand(DIOATCMD_TYPE_GSMGETIMSI               , __L("+CIMI"));
  AddCommand(DIOATCMD_TYPE_GSMPIN                   , __L("+CPIN"));
  AddCommand(DIOATCMD_TYPE_GSMCOMMAND               , __L("+CLCK"));
  AddCommand(DIOATCMD_TYPE_GSMGETSMSSUPPORT         , __L("+CSMS"));
  AddCommand(DIOATCMD_TYPE_GSMGETSMSFORMAT          , __L("+CMGF"));
  AddCommand(DIOATCMD_TYPE_GSMSETSMSFORMAT          , __L("+CMGF"));
  AddCommand(DIOATCMD_TYPE_GSMGETSMSCENTER          , __L("+CSCA"));
  AddCommand(DIOATCMD_TYPE_GSMSETSMSCENTER          , __L("+CSCA"));
  AddCommand(DIOATCMD_TYPE_GSMSENDSMS               , __L("+CMGS"));
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMDGSM::~DIOATCMDGSM()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMDGSM::~DIOATCMDGSM()
{
  Clean();
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMD_ERROR DIOATCMDGSM::GetManufactured(XSTRING& manufactured, int timeout)
* @brief      GetManufactured
* @ingroup    DATAIO
*
* @param[in]  manufactured :
* @param[in]  timeout :
*
* @return     DIOATCMD_ERROR :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::GetManufactured(XSTRING& manufactured, int timeout)
{
  return MakeCommandWithSimpleAnswer(DIOATCMD_TYPE_GSMGETMANUFACTURED,manufactured,NULL,timeout);
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMD_ERROR DIOATCMDGSM::GetModel(XSTRING& model, int timeout)
* @brief      GetModel
* @ingroup    DATAIO
*
* @param[in]  model :
* @param[in]  timeout :
*
* @return     DIOATCMD_ERROR :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::GetModel(XSTRING& model, int timeout)
{
  return MakeCommandWithSimpleAnswer(DIOATCMD_TYPE_GSMGETMODEL, model, NULL, timeout);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMD_ERROR DIOATCMDGSM::GetIMEI(XSTRING& IMEI, int timeout)
* @brief      GetIMEI
* @ingroup    DATAIO
*
* @param[in]  IMEI :
* @param[in]  timeout :
*
* @return     DIOATCMD_ERROR :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::GetIMEI(XSTRING& IMEI, int timeout)
{
  return MakeCommandWithSimpleAnswer(DIOATCMD_TYPE_GSMGETSERIAL, IMEI, NULL, timeout);
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMD_ERROR DIOATCMDGSM::GetVersion(XSTRING& version,int timeout)
* @brief      GetVersion
* @ingroup    DATAIO
*
* @param[in]  version :
* @param[in]  timeout :
*
* @return     DIOATCMD_ERROR :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::GetVersion(XSTRING& version, int timeout)
{
  DIOATCMD_ERROR error;
  int            index[2];

  index[0]= GetAnswers()->GetSize()-1;
  if(index[0]<0) index[0]=0;

  error = SendCommand(DIOATCMD_TYPE_GSMGETVERSION);
  if(error!=DIOATCMD_ERROR_NONE) return error;

  error = WaitToProcessAnswer(timeout);
  if(error!=DIOATCMD_ERROR_NONE) return error;

  index[1]= GetAnswers()->GetSize();

  version.Empty();

  for(int c=index[0];c<index[1];c++)
    {
      XSTRING* string = GetAnswer(c);
      if(string)
        {
          version += (*string);
          version += __L(" ");
        }
    }

  for(int c=index[0];c<index[1];c++)
    {
      DeleteLastAnswer();
    }

  return error;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMD_ERROR DIOATCMDGSM::GetSignalQuality(int& RSSI, int& BER, int timeout)
* @brief      GetSignalQuality
* @ingroup    DATAIO
*
* @param[in]  RSSI :
* @param[in]  BER :
* @param[in]  timeout :
*
* @return     DIOATCMD_ERROR :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::GetSignalQuality(int& RSSI, int& BER, int timeout)
{
  DIOATCMD_ERROR error = SendCommand(DIOATCMD_TYPE_GSMGETSIGNALQUALITY);
  if(error!=DIOATCMD_ERROR_NONE) return error;

  error = WaitToProcessAnswer(timeout);
  if(error!=DIOATCMD_ERROR_NONE) return error;

  int index = SearchInAnswers(DIOATCMD_TYPE_GSMGETSIGNALQUALITY);
  if(index!=DIOATCMDS_ANSWERNOTFOUND)
    {
      XSTRING* answer = GetAnswer(index);
      if(answer)
        {
          XSTRING  mask;

          mask  = GetCommandString(DIOATCMD_TYPE_GSMGETSIGNALQUALITY);
          mask += __L(":%d,%d");

          answer->UnFormat(mask.Get(),&RSSI,&BER);
        }

      DeleteAnswer(index);
    }

  return DIOATCMD_ERROR_NONE;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMD_ERROR DIOATCMDGSM::IsAvailableServiceGSM(bool& available, int timeout)
* @brief      IsAvailableServiceGSM
* @ingroup    DATAIO
*
* @param[in]  available :
* @param[in]  timeout :
*
* @return     DIOATCMD_ERROR :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::IsAvailableServiceGSM(bool& available, int timeout)
{
  DIOATCMD_ERROR error = SendCommand(DIOATCMD_TYPE_GSMGETSERVICES);
  if(error!=DIOATCMD_ERROR_NONE) return error;

  error = WaitToProcessAnswer(timeout);
  if(error!=DIOATCMD_ERROR_NONE) return error;

  available = false;

  XSTRING* answer = GetLastAnswer();
  if(answer)
    {
      if(answer->Find(__L("+CGSM"),true)!=XSTRING_NOTFOUND) available = true;
      DeleteLastAnswer();
    }

  DeleteAllAnswers();

  return DIOATCMD_ERROR_NONE;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMD_ERROR DIOATCMDGSM::GetIMSI(XSTRING& IMSI, int timeout)
* @brief      GetIMSI
* @ingroup    DATAIO
*
* @param[in]  IMSI :
* @param[in]  timeout :
*
* @return     DIOATCMD_ERROR :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::GetIMSI(XSTRING& IMSI, int timeout)
{
  return MakeCommandWithSimpleAnswer(DIOATCMD_TYPE_GSMGETIMSI, IMSI, NULL, timeout);
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOATCMD_ERROR DIOATCMDGSM::PIN_IsResolved(bool& isresolved, int timeout)
* @brief      PIN_IsResolved
* @ingroup    DATAIO
*
* @param[in]  isresolved : 
* @param[in]  timeout : 
* 
* @return     DIOATCMD_ERROR : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::PIN_IsResolved(bool& isresolved, int timeout)
{
  isresolved = false;

  DIOATCMD_ERROR error = SendCommand(DIOATCMD_TYPE_GSMPIN, __L("?"));
  if(error!=DIOATCMD_ERROR_NONE) return error;

  error = WaitToProcessAnswer(timeout);
  if(error!=DIOATCMD_ERROR_NONE) return error;

  XSTRING* answer = GetLastAnswer();
  if(answer)
    {      
      if(answer->Find(__L("SIM PIN")  ,true)!=XSTRING_NOTFOUND) isresolved = false; 
      if(answer->Find(__L("READY")    ,true)!=XSTRING_NOTFOUND) isresolved = true;     
    }

  DeleteAllAnswers();

  return error;
}



/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOATCMD_ERROR DIOATCMDGSM::PIN_Enter(XCHAR* PIN, bool& isenter, int timeout)
* @brief      PIN_Enter
* @ingroup    DATAIO
*
* @param[in]  PIN : 
* @param[in]  isenter : 
* @param[in]  timeout : 
* 
* @return     DIOATCMD_ERROR : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::PIN_Enter(XCHAR* PIN, bool& isenter, int timeout)
{
  XSTRING PINstr;  

  isenter = false;

  PINstr.Format(__L("=%s"), PIN);

  DIOATCMD_ERROR error = SendCommand(DIOATCMD_TYPE_GSMPIN, PINstr.Get());
  if(error!=DIOATCMD_ERROR_NONE) return error;  

  error = WaitToProcessAnswer(timeout);   
  if(error!=DIOATCMD_ERROR_NONE) return error;

  error = PIN_IsResolved(isenter, timeout);

  DeleteAllAnswers();

  return DIOATCMD_ERROR_NONE;
}




/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOATCMD_ERROR DIOATCMDGSM::PIN_Activate(XCHAR* PIN, bool activate, int timeout)
* @brief      PIN_Activate
* @ingroup    DATAIO
*
* @param[in]  PIN : 
* @param[in]  activate : 
* @param[in]  timeout : 
* 
* @return     DIOATCMD_ERROR : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::PIN_Activate(XCHAR* PIN, bool activate, int timeout)
{
  XSTRING PINstr;  

  PINstr.Format(__L("=\"SC\",%c,\"%s\""), (activate?__C('1'):__C('0')), PIN);

  DIOATCMD_ERROR error = SendCommand(DIOATCMD_TYPE_GSMCOMMAND, PINstr.Get());
  if(error!=DIOATCMD_ERROR_NONE) return error;  
  
  error = WaitToProcessAnswer(timeout);   
  if(error!=DIOATCMD_ERROR_NONE) return error;
 
  DeleteAllAnswers();

  return DIOATCMD_ERROR_NONE;
}
 



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMD_ERROR DIOATCMDGSM::GetSMSSupport(XBYTE& support, int timeout)
* @brief      GetSMSSupport
* @ingroup    DATAIO
*
* @param[in]  support :
* @param[in]  timeout :
*
* @return     DIOATCMD_ERROR :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::GetSMSSupport(XBYTE& support, int timeout)
{
  DIOATCMD_ERROR error = SendCommand(DIOATCMD_TYPE_GSMGETSMSSUPPORT,__L("?"));
  if(error!=DIOATCMD_ERROR_NONE) return error;

  error = WaitToProcessAnswer(timeout);
  if(error!=DIOATCMD_ERROR_NONE) return error;

  int index = SearchInAnswers(DIOATCMD_TYPE_GSMGETSMSSUPPORT,timeout);
  if(index!=DIOATCMDS_ANSWERNOTFOUND)
    {
      support = 0;

      XSTRING* answer = GetAnswer(index);
      if(answer)
        {
          int      service;
          int      mt = 0;
          int      mo = 0;
          int      bc = 0;
          XSTRING  mask;

          mask  = GetCommandString(DIOATCMD_TYPE_GSMGETSMSSUPPORT);
          mask += __L(": %d,%d,%d,%d");
          answer->UnFormat(mask.Get(),&service,&mt,&mo,&bc);

          support|= (service<<4);
          if(mt) support|=DIOATCMDGSM_SMSSUPPORT_MOVILETERMINATED;
          if(mo) support|=DIOATCMDGSM_SMSSUPPORT_MOVILEORIGINATED;
          if(bc) support|=DIOATCMDGSM_SMSSUPPORT_BROADCAST;
        }

      DeleteAnswer(index);
    }

  return DIOATCMD_ERROR_NONE;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMD_ERROR DIOATCMDGSM::GetSMSFormat(XBYTE& format,bool support, int timeout)
* @brief      GetSMSFormat
* @ingroup    DATAIO
*
* @param[in]  format :
* @param[in]  support :
* @param[in]  timeout :
*
* @return     DIOATCMD_ERROR :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::GetSMSFormat(XBYTE& format,bool support, int timeout)
{
  DIOATCMD_ERROR error = SendCommand(DIOATCMD_TYPE_GSMGETSMSFORMAT,support?__L("=?"):__L("?"));
  if(error!=DIOATCMD_ERROR_NONE) return error;

  error = WaitToProcessAnswer(timeout);
  if(error!=DIOATCMD_ERROR_NONE) return error;

  int index = SearchInAnswers(DIOATCMD_TYPE_GSMGETSMSFORMAT,timeout);
  if(index!=DIOATCMDS_ANSWERNOTFOUND)
    {
      XSTRING* answer = GetAnswer(index);
      XSTRING  mask;

      format = 0;

      if(answer)
        {
          mask  = GetCommandString(DIOATCMD_TYPE_GSMGETSMSFORMAT);

          if(support)
            {
              mask += __L(": (%d,%d)");

              int value[2] = { -1 , -1 };

              answer->UnFormat(mask.Get(),&value[0],&value[1]);

              if(value[0]==0) format |= DIOATCMDGSM_SMSFORMAT_PDU;
              if(value[1]==1) format |= DIOATCMDGSM_SMSFORMAT_TXT;

            }
           else
            {
              mask += __L(": %d");

              int value;

              answer->UnFormat(mask.Get(),&value);

              if(value==0) format = DIOATCMDGSM_SMSFORMAT_PDU;
              if(value==1) format = DIOATCMDGSM_SMSFORMAT_TXT;
            }
        }

      DeleteAnswer(index);
    }

  return DIOATCMD_ERROR_NONE;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMD_ERROR DIOATCMDGSM::SetSMSFormat(XBYTE format, int timeout)
* @brief      SetSMSFormat
* @ingroup    DATAIO
*
* @param[in]  format :
* @param[in]  timeout :
*
* @return     DIOATCMD_ERROR :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::SetSMSFormat(XBYTE format, int timeout)
{
  XSTRING param;

  switch(format)
    {
      case DIOATCMDGSM_SMSFORMAT_PDU : param = __L("=0"); break;
      case DIOATCMDGSM_SMSFORMAT_TXT : param = __L("=1"); break;
    }

  DIOATCMD_ERROR error = SendCommand(DIOATCMD_TYPE_GSMSETSMSFORMAT,param.Get());
  if(error!=DIOATCMD_ERROR_NONE) return error;

  return WaitToProcessAnswer(timeout);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMD_ERROR DIOATCMDGSM::GetSMSCenter(XSTRING& SMScenter, int timeout)
* @brief      GetSMSCenter
* @ingroup    DATAIO
*
* @param[in]  SMScenter :
* @param[in]  timeout :
*
* @return     DIOATCMD_ERROR :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::GetSMSCenter(XSTRING& SMScenter, int timeout)
{
  DIOATCMD_ERROR error = SendCommand(DIOATCMD_TYPE_GSMGETSMSCENTER,__L("?"));
  if(error!=DIOATCMD_ERROR_NONE) return error;

  error = WaitToProcessAnswer(timeout);
  if(error!=DIOATCMD_ERROR_NONE) return error;

  int index = SearchInAnswers(DIOATCMD_TYPE_GSMGETSMSCENTER,timeout);
  if(index!=DIOATCMDS_ANSWERNOTFOUND)
    {
      XSTRING* answer = GetAnswer(index);
      if(answer)
        {
          XSTRING  mask;
          XCHAR    smscenter[64];
          int      value;

          mask  = GetCommandString(DIOATCMD_TYPE_GSMGETSMSCENTER);
          mask += __L(": \"%s\",%d");

          answer->UnFormat(mask.Get(),smscenter,&value);

          if(smscenter[0])
            {
              SMScenter = smscenter;

            } else return DIOATCMD_ERROR_INVALIDCMD;
        }

      DeleteAnswer(index);
    }

  return DIOATCMD_ERROR_NONE;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMD_ERROR DIOATCMDGSM::SetSMSCenter(XCHAR* smscenter, int timeout)
* @brief      SetSMSCenter
* @ingroup    DATAIO
*
* @param[in]  smscenter :
* @param[in]  timeout :
*
* @return     DIOATCMD_ERROR :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::SetSMSCenter(XCHAR* smscenter, int timeout)
{
  XSTRING param;

  param.Format(__L("=\"%s\""),smscenter);

  DIOATCMD_ERROR error = SendCommand(DIOATCMD_TYPE_GSMSETSMSCENTER,param.Get());
  if(error!=DIOATCMD_ERROR_NONE) return error;

  return WaitToProcessAnswer(timeout);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         DIOATCMD_ERROR DIOATCMDGSM::SendSMS(XCHAR* number, XCHAR* message, int* mrID, int timeout)
* @brief      SendSMS
* @ingroup    DATAIO
*
* @param[in]  number :
* @param[in]  message :
* @param[in]  mrID :
* @param[in]  timeout :
*
* @return     DIOATCMD_ERROR :
*
* --------------------------------------------------------------------------------------------------------------------*/
DIOATCMD_ERROR DIOATCMDGSM::SendSMS(XCHAR* number, XCHAR* message, int* mrID, int timeout)
{
  DIOATCMD_ERROR error;
  XBYTE          SMSformat = DIOATCMDGSM_SMSFORMAT_PDU;
  XSTRING        gsmnumber;
  XSTRING        gsmmessage;

  error=GetSMSFormat(SMSformat);
  if(error!=DIOATCMD_ERROR_NONE) return error;

  switch(SMSformat)
    {
      case DIOATCMDGSM_SMSFORMAT_PDU    : { XSTRING _number;
                                            XSTRING _message;

                                            CodecPDUFormat(number,message,false,20,_number,_message);

                                            gsmnumber.Format(__L("= %s ")  ,_number.Get());
                                            gsmmessage.Format(__L("%s\x1A"),_message.Get());
                                          }
                                          break;

      case DIOATCMDGSM_SMSFORMAT_TXT    : gsmnumber.Format(__L("=\"%s\""),number);
                                          gsmmessage.Format(__L("%s\x1A"),message);
                                          break;

                                default : return DIOATCMD_ERROR_INVALIDCMD;
    }

  error = SendCommand(DIOATCMD_TYPE_GSMSENDSMS,gsmnumber.Get());
  if(error!=DIOATCMD_ERROR_NONE) return error;

  error = DIOATCMD_ERROR_INVALIDCMD;

  if(ReadAnswerWithOutEndChar(__L("> "),timeout))
    {
      error = SendParam(gsmmessage.Get());
      if(error!=DIOATCMD_ERROR_NONE) return error;

      error = WaitToProcessAnswer(timeout);
      if(error!=DIOATCMD_ERROR_NONE) return error;

      int index = SearchInAnswers(DIOATCMD_TYPE_GSMSENDSMS,timeout);
      if(index!=DIOATCMDS_ANSWERNOTFOUND)
        {
          XSTRING* answer = GetAnswer(index);
          if(answer)
            {
              if(mrID)
                {
                  XSTRING  mask;

                  mask  = GetCommandString(DIOATCMD_TYPE_GSMSENDSMS);
                  mask += __L(": %d");

                  answer->UnFormat(mask.Get(),mrID);
                }

              error = DIOATCMD_ERROR_NONE;
            }

           DeleteAnswer(index);
        }
    }

  return error;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool DIOATCMDGSM::CodecPDUFormat(XCHAR* number, XCHAR* message, bool inoctets, int validperiod, XSTRING& length, XSTRING& result)
* @brief      CodecPDUFormat
* @ingroup    DATAIO
*
* @param[in]  number :
* @param[in]  message :
* @param[in]  inoctets :
* @param[in]  validperiod :
* @param[in]  length :
* @param[in]  result :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOATCMDGSM::CodecPDUFormat(XCHAR* number, XCHAR* message, bool inoctets, int validperiod, XSTRING& length, XSTRING& result)
{
  if(!number)  return false;
  if(!message) return false;

  DIOATCMDGSM_ALPHABET_8859_1  alphabet;
  XSTRING                      string;
  XSTRING                      _number;
  XSTRING                      _message;
  int                          value;
  XBYTE*                       codec7 = new XBYTE[256];
  XBYTE*                       codec8 = new XBYTE[256];
  int                          size7  = 0;
  int                          size8  = 0;
  int                          c;

  if((!codec7) || (!codec8)) return false;

  memset(codec7 ,0 , 256);
  memset(codec8 ,0 , 256);

  result.Empty();

  _number  = number;
  _message = message;

  // No SMS Center.
  result += __L("00");
  // SMS-SUBMIT mensaje.
  result += __L("11");
  result += __L("00");

  // Size Number (National)
  string.Format(__L("%02X"),_number.GetSize());
  result += string;

  if(_number.GetSize()>9)
         result += __L("91");    // Type Number (International);
    else result += __L("81");    // Type Number (National);

  // GSM Number
  string = number;
  if(string.GetSize()%2) string += __L("F");
  string.Swab();
  result += string;
  result += __L("00");

  // Codec 7/8 Bits
  if(inoctets)
         result += __L("02");
    else result += __L("00");

  // Time Max Msg <28 days  >28 weeks
  if(validperiod>28)
          value = 168 + validperiod;
    else  value = 197 + validperiod;
  string.Format(__L("%02X"),value);
  result += string;

  // Convert message
  for(c=0;c<(int)_message.GetSize();c++)
    {
      XWORD data=alphabet.Traslate(false,(XBYTE)message[c]);
      XBYTE byte[2];

      byte[1]=(XBYTE)(data>>8);
      byte[0]=(XBYTE)(data&0x00FF);

      if(byte[1])
        {
          codec8[size8]=byte[1];
          size8++;
        }

      codec8[size8]=byte[0];
      size8++;
    }


  // Compress 7 Bits.
  if(!inoctets)
    {
      int shift=0;

      for(c=0;c<size8;c++)
        {
          XBYTE mask = (0xFF>>(7-shift));
          XBYTE data = (codec8[c+1]&mask)<<(XBYTE)(7-shift);

          if(shift!=7)
            {
              codec7[size7]=(data|(codec8[c]>>(XBYTE)shift));
              size7++;
            }

          shift++;
          if(shift>7) shift=0;
        }
    }

  string.Format(__L("%02X"),_message.GetSize());
  result += string;

  for(c=0;c<((inoctets)?size8:size7);c++)
    {
      string.Format(__L("%02X"),(inoctets)?codec8[c]:codec7[c]);
      result += string;
    }

  delete [] codec7;
  delete [] codec8;

  length.Format(__L("%d"),(result.GetSize()/2)-1);

  return true;
}





/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void DIOATCMDGSM::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void DIOATCMDGSM::Clean()
{

}

