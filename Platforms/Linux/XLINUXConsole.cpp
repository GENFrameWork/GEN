/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XLINUXConsole.cpp
* 
* @class      XLINUXCONSOLE
* @brief      LINUX eXtended Utils Console class
* @ingroup    PLATFORM_LINUX
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

#include "XLINUXConsole.h"

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "XTrace.h"

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


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XLINUXCONSOLE::XLINUXCONSOLE()
* @brief      Constructor of class
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
XLINUXCONSOLE::XLINUXCONSOLE(): XCONSOLE()
{
  RawModeEnable();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XLINUXCONSOLE::~XLINUXCONSOLE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
XLINUXCONSOLE::~XLINUXCONSOLE()
{
  RawModeDisable();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXCONSOLE::GetSizeText(int& columns, int& rows)
* @brief      Get size text
* @ingroup    PLATFORM_LINUX
*
* @param[in]  columns :
* @param[in]  rows :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXCONSOLE::GetSizeText(int& columns, int& rows)
{
  #ifndef APPFLOW_GRAPHICS_NOTCONSOLE_ACTIVE

  struct winsize console_size;

  //ioctl(0, TIOCGWINSZ, &console_size);
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &console_size);

  columns = console_size.ws_col;
  rows    = console_size.ws_row;

  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXCONSOLE::Maximize()
* @brief      Maximize
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXCONSOLE::Maximize()
{
  #ifndef APPFLOW_GRAPHICS_NOTCONSOLE_ACTIVE
  setvbuf(stdout, NULL,_IONBF,0);
  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXCONSOLE::Minimize()
* @brief      Minimize
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXCONSOLE::Minimize()
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXCONSOLE::Hide()
* @brief      Hide
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXCONSOLE::Hide()
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXCONSOLE::IsHide()
* @brief      Is hide
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXCONSOLE::IsHide()
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXCONSOLE::Print(XCHAR* string)
* @brief      Print
* @ingroup    PLATFORM_LINUX
*
* @param[in]  string : string to print
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXCONSOLE::Print(XCHAR* string)
{
  #ifndef APPFLOW_GRAPHICS_NOTCONSOLE_ACTIVE

  if(!string)     return false;
  if(!string[0])  return false;

  XSTRING _string;

  _string = string;

  XBUFFER charstr;
  
  _string.ConvertToASCII(charstr);  
  printf("%s", charstr.GetPtrChar());
  
  fflush(stdout);

  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XLINUXCONSOLE::Clear(bool fill)
* @brief      Clear
* @ingroup    PLATFORM_LINUX
*
* @param[in]  fill : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XLINUXCONSOLE::Clear(bool fill)
{
  #ifndef APPFLOW_GRAPHICS_NOTCONSOLE_ACTIVE

  printf("\033[2J");
  printf("\033[H");


  //printf("\033[2J\033[H");    
  //printf("\e[3J");

  //printf("\033c");    // KDE
 
  //system("clear");

  #endif

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXCONSOLE::KBHit(void)
* @brief      KB hit
* @ingroup    PLATFORM_LINUX
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXCONSOLE::KBHit(void)
{

  #ifndef APPFLOW_GRAPHICS_NOTCONSOLE_ACTIVE
  /*
  struct termios oldt;
  struct termios newt;
  int            ch;
  int            oldf;

  tcgetattr(STDIN_FILENO, &oldt);

  newt          = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);

  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);

  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
    {
      ungetc(ch, stdin);
      return true;
    }

  return false;
  */

  int byteswaiting;
  ioctl(0, FIONREAD, &byteswaiting);

  return byteswaiting > 0;

  #else

  return false;

  #endif


}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int XLINUXCONSOLE::GetChar()
* @brief      Get char
* @ingroup    PLATFORM_LINUX
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int XLINUXCONSOLE::GetChar()
{
  #ifndef APPFLOW_GRAPHICS_NOTCONSOLE_ACTIVE

  struct termios oldt, newt;
  int ch;

  tcgetattr( STDIN_FILENO, &oldt );               /* Obtenemos atributos del terminal */
  newt = oldt;
  newt.c_lflag &= ~ICANON;
  newt.c_lflag &= ~ECHO;                          /* Eliminamos el Echo */

  tcsetattr( STDIN_FILENO, TCSANOW, &newt );      /* Definimos los nuevos atributos al terminal */

  ch = getchar();

  tcsetattr( STDIN_FILENO, TCSANOW, &oldt );      /* Ponemos los atributos como estaban al principio */

  return ch;

  #else

  return 0;

  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XLINUXCONSOLE::RawModeEnable()
* @brief      Raw mode enable
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
void XLINUXCONSOLE::RawModeEnable()
{
  #ifndef APPFLOW_GRAPHICS_NOTCONSOLE_ACTIVE

  termios term;

  tcgetattr(0, &term);
  term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
  tcsetattr(0, TCSANOW, &term);

  #endif
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XLINUXCONSOLE::RawModeDisable()
* @brief      Raw mode disable
* @ingroup    PLATFORM_LINUX
*
* --------------------------------------------------------------------------------------------------------------------*/
void XLINUXCONSOLE::RawModeDisable()
{
  #ifndef APPFLOW_GRAPHICS_NOTCONSOLE_ACTIVE

  termios term;
  
  tcgetattr(0, &term);
  term.c_lflag |= ICANON | ECHO;
  tcsetattr(0, TCSANOW, &term);

  #endif
}


#pragma endregion


