/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOModBusElectricMeter.cpp
* 
* @class      DIOMODBUSELECTRICMETER
* @brief      Data Input/Output ModBus Electric Meter class
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOModBusElectricMeter.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "XDateTime.h"
#include "XTimer.h"
#include "XFactory.h"
#include "XFSMachine.h"
#include "XBuffer.h"
#include "XTrace.h"

#include "DIOStream.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


#pragma region DIOMODBUSELECTRICMETERXEVENT


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOMODBUSELECTRICMETERXEVENT::DIOMODBUSELECTRICMETERXEVENT(XSUBJECT* subject,XDWORD type) : XEVENT(subject,type)
* @brief      Constructor
* @ingroup    DATAIO
* 
* @param[in]  subject : 
* @param[in]  type : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOMODBUSELECTRICMETERXEVENT::DIOMODBUSELECTRICMETERXEVENT(XSUBJECT* subject,XDWORD type) : XEVENT(subject,type)
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOMODBUSELECTRICMETERXEVENT::~DIOMODBUSELECTRICMETERXEVENT()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOMODBUSELECTRICMETERXEVENT::~DIOMODBUSELECTRICMETERXEVENT()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOMODBUSELECTRICMETERXEVENT::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOMODBUSELECTRICMETERXEVENT::Clean()
{

}


#pragma endregion


#pragma region DIOMODBUSELECTRICMETER


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOMODBUSELECTRICMETER::DIOMODBUSELECTRICMETER(DIOSTREAM* diostream, DIOMODBUS_CLIENTMODE mode)
* @brief      Constructor
* @ingroup    DATAIO
* 
* @param[in]  diostream : 
* @param[in]  mode : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOMODBUSELECTRICMETER::DIOMODBUSELECTRICMETER(DIOSTREAM* diostream, DIOMODBUS_CLIENTMODE mode)
{
  Clean();

  modbusprotocol = new DIOMODBUS_CLIENT(diostream, mode);
  if(modbusprotocol)
    {
      RegisterEvent(DIOMODBUSELECTRICMETERXEVENT_TYPE_C_TR_AA_READVALUES);
      RegisterEvent(DIOMODBUSELECTRICMETERXEVENT_TYPE_C_CI_NU_2_READVALUE);

      GEN_XFACTORY_CREATE(xtimer, CreateTimer())
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOMODBUSELECTRICMETER::~DIOMODBUSELECTRICMETER()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOMODBUSELECTRICMETER::~DIOMODBUSELECTRICMETER()
{
  GEN_XFACTORY.DeleteTimer(xtimer);

  Disconnect();

  DeRegisterEvent(DIOMODBUSELECTRICMETERXEVENT_TYPE_C_TR_AA_READVALUES);
  DeRegisterEvent(DIOMODBUSELECTRICMETERXEVENT_TYPE_C_CI_NU_2_READVALUE);

  delete modbusprotocol;

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOMODBUS_CLIENT* DIOMODBUSELECTRICMETER::GetModBusProtocol()
* @brief      GetModBusProtocol
* @ingroup    DATAIO
* 
* @return     DIOMODBUS_CLIENT* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOMODBUS_CLIENT* DIOMODBUSELECTRICMETER::GetModBusProtocol()
{
  return modbusprotocol;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOMODBUSELECTRICMETER::Connect(XBYTE unit, bool inlittleendian, int timeout)
* @brief      Connect
* @ingroup    DATAIO
* 
* @param[in]  unit : 
* @param[in]  inlittleendian : 
* @param[in]  timeout : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOMODBUSELECTRICMETER::Connect(XBYTE unit, bool inlittleendian, int timeout)
{
  if(!modbusprotocol)                   return false;
  if(!modbusprotocol->GetDIOStream())   return false;

  if(modbusprotocol->GetDIOStream()->GetStatus()!=DIOSTREAMSTATUS_DISCONNECTED) return false;

  if(!modbusprotocol->Connect(unit,timeout)) return false;

  this->unit            = unit;
  this->inlittleendian  = inlittleendian;

  canceloperations      = false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOMODBUSELECTRICMETER::CMD_C_CI_NU_2_ReadAllValues(int TM, XDATETIME* timestart, XDATETIME* timeend, XVECTOR<DIO_C_CI_NU_2_RESULT*>* results, int timeout)
* @brief      CMD C CI NU 2  read all values
* @ingroup    DATAIO
* 
* @param[in]  TM : 
* @param[in]  timestart : 
* @param[in]  timeend : 
* @param[in]  results : 
* @param[in]  timeout : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOMODBUSELECTRICMETER::CMD_C_CI_NU_2_ReadAllValues(int TM, XDATETIME* timestart, XDATETIME* timeend, XVECTOR<DIO_C_CI_NU_2_RESULT*>* results, int timeout)
{
  if(!results) return false;
  if(!modbusprotocol) return false;

  XBUFFER answer;
  float   fresult = 0.0f;
  int     iresult = 0;
  bool    status;

  status = modbusprotocol->ReadHoldingRegisters(50812, 14, answer, timeout);
  if(status)
    {
      DIO_C_CI_NU_2_RESULT* result;

      result = new DIO_C_CI_NU_2_RESULT();
      if(result)
        {
          result->GetDateTime()->Read();
          result->GetDateTime()->SetSeconds(0);

          switch(TM)
            {
              case 1  : for(int c=0; c<8; c++)
                          {
                            result->GetIndexValues()->Add(c+1);
                            result->GetValues()->Add(0);
                          }

                        ReadRegister(50780, 2, timeout, iresult);
                        result->GetIndexValues()->Add(1);
                        result->GetValues()->Add(iresult);

                        ReadRegister(50782, 2, timeout, iresult);
                        result->GetIndexValues()->Add(2);
                        result->GetValues()->Add(iresult);

                        break;


              case 2  : ReadRegister(51046, 2, 100, timeout, fresult);
                        result->GetIndexValues()->Add(1);
                        result->GetValues()->Add((int)fresult);

                        for(int c=1; c<8; c++)
                          {
                            result->GetIndexValues()->Add(c+1);
                            result->GetValues()->Add(0);
                          }

                        break;

              default : status = false;
                        break;

            }


          results->Add(result);

        } else status = false;

    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOMODBUSELECTRICMETER::CMD_C_CI_NU_2_DeleteResults(XVECTOR<DIO_C_CI_NU_2_RESULT*>* results)
* @brief      CMD_C_CI_NU_2_DeleteResults
* @ingroup    DATAIO
* 
* @param[in]  results : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOMODBUSELECTRICMETER::CMD_C_CI_NU_2_DeleteResults(XVECTOR<DIO_C_CI_NU_2_RESULT*>* results)
{
  if(!results) return false;

  if(results->IsEmpty()) return false;

  results->DeleteContents();
  results->DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOMODBUSELECTRICMETER::CMD_C_TR_AA_ReadValues(DIO_C_TR_AA_RESULT* result, int timeout)
* @brief      CMD_C_TR_AA_ReadValues
* @ingroup    DATAIO
* 
* @param[in]  result : 
* @param[in]  timeout : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOMODBUSELECTRICMETER::CMD_C_TR_AA_ReadValues(DIO_C_TR_AA_RESULT* result, int timeout)
{
  if(!result)         return false;
  if(!modbusprotocol) return false;

  float fresult = 0.0f;
  int   iresult = 0;

  // Totalizadores Energeticos
  ReadRegister(50780, 2, timeout, iresult);           result->SetActiveImport(iresult);                       // Activa Importacion
                                                      result->SetActiveExport(0);                             // Activa Exportacion

                                                      result->SetReactiveQ(1, 0);                             // Reactiva Q1
                                                      result->SetReactiveQ(2, 0);                             // Reactiva Q2
                                                      result->SetReactiveQ(3, 0);                             // Reactiva Q3
                                                      result->SetReactiveQ(4, 0);                             // Reactiva Q4

  // Potencias Instantaneas
  ReadRegister(50536, 2,  100, timeout, fresult);     result->SetTotalActivePower((int)fresult);              // Potencia Activa    Total
  ReadRegister(50538, 2,  100, timeout, fresult);     result->SetTotalReactivePower((int)fresult);            // Potencia Reactiva  Total
  ReadRegister(50542, 2, 1000, timeout, fresult);     result->SetTotalPowerFactor(fresult);                   // Factor de Potencia Total


  ReadRegister(50544, 2,  100, timeout, fresult);     result->SetActivePowerPhase(1, (int)(fresult));         // Potencia Activa    Fase I
  ReadRegister(50550, 2,  100, timeout, fresult);     result->SetReactivePowerPhase(1, (int)(fresult));       // Potencia Reactiva  Fase I
  ReadRegister(50562, 2, 1000, timeout, fresult);     result->SetPowerFactorPhase(1, fresult);                // Factor de Potencia Fase I

  ReadRegister(50546, 2,  100, timeout, fresult);     result->SetActivePowerPhase(2, (int)(fresult));         // Potencia Activa    Fase II
  ReadRegister(50552, 2,  100, timeout, fresult);     result->SetReactivePowerPhase(2, (int)(fresult));       // Potencia Reactiva  Fase II
  ReadRegister(50564, 2, 1000, timeout, fresult);     result->SetPowerFactorPhase(2, fresult);                // Factor de Potencia Fase II

  ReadRegister(50548, 2,  100, timeout, fresult);     result->SetActivePowerPhase(3, (int)(fresult));         // Potencia Activa    Fase III
  ReadRegister(50554, 2,  100, timeout, fresult);     result->SetReactivePowerPhase(3, (int)(fresult));       // Potencia Reactiva  Fase III
  ReadRegister(50566, 2, 1000, timeout, fresult);     result->SetPowerFactorPhase(3, fresult);                // Factor de Potencia Fase III

  // Valores Secundarios
  ReadRegister(50520, 2,  100, timeout, fresult);     result->SetVoltagePhase(1, fresult);                    // Tension    Fase I
  ReadRegister(50528, 2, 1000, timeout, fresult);     result->SetCurrentPhase(1, fresult);                    // Intensidad Fase I

  ReadRegister(50522, 2,  100, timeout, fresult);     result->SetVoltagePhase(2, fresult);                    // Tension    Fase II
  ReadRegister(50530, 2, 1000, timeout, fresult);     result->SetCurrentPhase(2, fresult);                    // Intensidad Fase II

  ReadRegister(50524, 2,  100, timeout, fresult);     result->SetVoltagePhase(3, fresult);                    // Tension    Fase III
  ReadRegister(50532, 2, 1000, timeout, fresult);     result->SetCurrentPhase(3, fresult);                    // Intensidad Fase III

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOMODBUSELECTRICMETER::CancelOperations()
* @brief      CancelOperations
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOMODBUSELECTRICMETER::CancelOperations()
{
  canceloperations = true;
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOMODBUSELECTRICMETER::Disconnect()
* @brief      Disconnect
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOMODBUSELECTRICMETER::Disconnect()
{
  if(!modbusprotocol) return;

  modbusprotocol->Disconnect();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOMODBUSELECTRICMETER::ReadRegister(int registerID, int nwords, int divisor, int timeout, float& result)
* @brief      ReadRegister
* @ingroup    DATAIO
* 
* @param[in]  registerID : 
* @param[in]  nwords : 
* @param[in]  divisor : 
* @param[in]  timeout : 
* @param[in]  result : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOMODBUSELECTRICMETER::ReadRegister(int registerID, int nwords, int divisor, int timeout, float& result)
{
  XBUFFER   answer;
  XDWORD    dword = 0;
  XWORD     word  = 0;
  int       data  = 0;

  result = 0.0f;

  if(!modbusprotocol->ReadHoldingRegisters(registerID, nwords, answer, timeout)) return false;

  if(nwords == 2)
    {
      answer.Get(dword);
      data = (int)dword;
    }

  if(nwords == 1)
    {
      answer.Get(word);
      data = (int)word;
    }

  result = ((float)data / divisor);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOMODBUSELECTRICMETER::ReadRegister(int registerID, int nwords, int timeout, int& result)
* @brief      ReadRegister
* @ingroup    DATAIO
* 
* @param[in]  registerID : 
* @param[in]  nwords : 
* @param[in]  timeout : 
* @param[in]  result : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOMODBUSELECTRICMETER::ReadRegister(int registerID, int nwords, int timeout, int& result)
{
  XBUFFER   answer;
  XDWORD    dword = 0;
  XWORD     word  = 0;

  result = 0;

  if(!modbusprotocol->ReadHoldingRegisters(registerID, nwords, answer, timeout)) return false;

  if(nwords == 2)
    {
      answer.Get(dword);
      result = (int)dword;
    }

  if(nwords == 1)
    {
      answer.Get(word);
      result = (int)word;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOMODBUSELECTRICMETER::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOMODBUSELECTRICMETER::Clean()
{
  xtimer                  = NULL;

  unit                    = 0;
  inlittleendian          = true;

  lastactiveenergysum     = 0;
  lastreactiveenergysum   = 0;

  modbusprotocol          = NULL;

  canceloperations        = false;
}


#pragma endregion
