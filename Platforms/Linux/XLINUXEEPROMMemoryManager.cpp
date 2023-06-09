/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XLINUXEEPROMMemoryManager.cpp
*
* @class      XLINUXEEPROMMEMORYMANAGER
* @brief      eXtended Windows EEPROM Memory Manager class
* @ingroup    XUTILS
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
#include <string.h>

#include "XLINUXEEPROMMemoryManager.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XLINUXEEPROMMEMORYMANAGER::XLINUXEEPROMMEMORYMANAGER()
* @brief      Constructor
* @ingroup    XUTILS
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XLINUXEEPROMMEMORYMANAGER::XLINUXEEPROMMEMORYMANAGER()
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XLINUXEEPROMMEMORYMANAGER::~XLINUXEEPROMMEMORYMANAGER()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    XUTILS
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XLINUXEEPROMMEMORYMANAGER::~XLINUXEEPROMMEMORYMANAGER()
{

}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXEEPROMMEMORYMANAGER::Ini()
* @brief      Ini
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXEEPROMMEMORYMANAGER::Ini()
{
  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXEEPROMMEMORYMANAGER::End()
* @brief      End
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXEEPROMMEMORYMANAGER::End()
{
  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXEEPROMMEMORYMANAGER::Read(XDWORD offset, XBYTE& data)
* @brief      Read
* @ingroup    XUTILS
*
* @param[in]  offset :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXEEPROMMEMORYMANAGER::Read(XDWORD offset, XBYTE& data)
{
  return Read(offset, (XBYTE*)&data, sizeof(XBYTE));
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXEEPROMMEMORYMANAGER::Read(XDWORD offset, XWORD& data)
* @brief      Read
* @ingroup    XUTILS
*
* @param[in]  offset :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXEEPROMMEMORYMANAGER::Read(XDWORD offset, XWORD& data)
{
  return Read(offset, (XBYTE*)&data, sizeof(XWORD));
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXEEPROMMEMORYMANAGER::Read(XDWORD offset, XDWORD& data)
* @brief      Read
* @ingroup    XUTILS
*
* @param[in]  offset :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXEEPROMMEMORYMANAGER::Read(XDWORD offset, XDWORD& data)
{
  return Read(offset, (XBYTE*)&data, sizeof(XDWORD));
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXEEPROMMEMORYMANAGER::Read(XDWORD offset, XBYTE* data, XDWORD size)
* @brief      Read
* @ingroup    XUTILS
*
* @param[in]  offset :
* @param[in]  data :
* @param[in]  size :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXEEPROMMEMORYMANAGER::Read(XDWORD offset, XBYTE* data, XDWORD size)
{
  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXEEPROMMEMORYMANAGER::Write(XDWORD offset, XBYTE data)
* @brief      Write
* @ingroup    XUTILS
*
* @param[in]  offset :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXEEPROMMEMORYMANAGER::Write(XDWORD offset, XBYTE data)
{
  return Write(offset, (XBYTE*)&data, sizeof(XBYTE));
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXEEPROMMEMORYMANAGER::Write(XDWORD offset, XWORD data)
* @brief      Write
* @ingroup    XUTILS
*
* @param[in]  offset :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXEEPROMMEMORYMANAGER::Write(XDWORD offset, XWORD data)
{
  return Write(offset, (XBYTE*)&data, sizeof(XWORD));
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXEEPROMMEMORYMANAGER::Write(XDWORD offset, XDWORD data)
* @brief      Write
* @ingroup    XUTILS
*
* @param[in]  offset :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXEEPROMMEMORYMANAGER::Write(XDWORD offset, XDWORD data)
{
  return Write(offset,(XBYTE*)&data, sizeof(XDWORD));
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXEEPROMMEMORYMANAGER::Write(XDWORD offset, XBYTE* data, XDWORD size)
* @brief      Write
* @ingroup    XUTILS
*
* @param[in]  offset :
* @param[in]  data :
* @param[in]  size :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXEEPROMMEMORYMANAGER::Write(XDWORD offset, XBYTE* data, XDWORD size)
{
  return false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXEEPROMMEMORYMANAGER::EraseAll()
* @brief      EraseAll
* @ingroup    XUTILS
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXEEPROMMEMORYMANAGER::EraseAll()
{
  return false;
}






