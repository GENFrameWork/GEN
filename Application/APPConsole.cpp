/**-------------------------------------------------------------------------------------------------------------------
*
* @file       APPConsole.cpp
*
* @class      APPCONSOLE
* @brief      Application Console class
* @ingroup    APPLICATION
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @copyright  Copyright(c) 2008 - 2016 GEN Group.
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
*---------------------------------------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XFactory.h"
#include "XConsole.h"
#include "XLog.h"
#include "XTranslation_GEN.h"
#include "XTranslation.h"

#include "APPCFG.h"

#include "APPConsole.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         APPCONSOLE::APPCONSOLE()
* @brief      Constructor
* @ingroup    APPLICATION
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
APPCONSOLE::APPCONSOLE()
{
  Clean();
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         APPCONSOLE::~APPCONSOLE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    APPLICATION
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
APPCONSOLE::~APPCONSOLE()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPCONSOLE::Ini(XSYSTEM* xsystem, XVECTOR<XSTRING*>* execparams)
* @brief      Ini Application console function
* @ingroup    APPLICATION
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  execparams : Params of command line (params of exec application)
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool APPCONSOLE::Ini(XVECTOR<XSTRING*>* execparams)
{
  console = GEN_XFACTORY.CreateConsole();
  if(!console) return false;

  //console->Clear();

  return APPBASE::Ini(execparams);
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPCONSOLE::End()
* @brief      End application console function
* @ingroup    APPLICATION
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool APPCONSOLE::End()
{
  XSTRING exittypestring;

  GetExitTypeString(exittypestring);
  if(!exittypestring.IsEmpty()) GEN_XLOG.AddEntry(XLOGLEVEL_INFO, APP_CFG_LOG_SECTIONID_GENERIC, false, exittypestring.Get());

  bool status = APPBASE::End();

  if(console)
    {
      PrintExitMessage(exittypestring);

      GEN_XFACTORY.DeleteConsole(console);
      console = NULL;
    }

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool APPCONSOLE::PrintExitMessage(XSTRING& message)
* @brief      PrintExitMessage
* @ingroup    APPLICATION
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  message :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool APPCONSOLE::PrintExitMessage(XSTRING& exitmessage)
{
  if(!console)              return false;
  if(exitmessage.IsEmpty()) return false;

  console->PrintMessage(exitmessage.Get(), 1 ,true, true);

  console->PrintMessage(__L(" "), 0 ,false, true);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void APPCONSOLE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    APPLICATION
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void APPCONSOLE::Clean()
{
  console = NULL;
}






