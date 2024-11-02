/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOCoreProtocol_CFG.cpp
* 
* @class      DIOCOREPROTOCOL_CFG
* @brief      Data Input/Output Core Protocol CFG class
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
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "DIOCoreProtocol_CFG.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_DIOCOREPROTOCOL_CFG


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_CFG::DIOCOREPROTOCOL_CFG()
* @brief      Constructor
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_CFG::DIOCOREPROTOCOL_CFG()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOCOREPROTOCOL_CFG::~DIOCOREPROTOCOL_CFG()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOCOREPROTOCOL_CFG::~DIOCOREPROTOCOL_CFG()
{
  Clean();

  diostreams.SetIsMulti(false);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL_CFG::GetIsServer()
* @brief      GetIsServer
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL_CFG::GetIsServer()
{
  return isserver;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_CFG::SetIsServer(bool isserver)
* @brief      SetIsServer
* @ingroup    DATAIO
* 
* @param[in]  isserver : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_CFG::SetIsServer(bool isserver)
{
  this->isserver = isserver;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL_CFG::GetCompressHeader()
* @brief      GetCompressHeader
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL_CFG::GetCompressHeader()
{
  return compressheader;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_CFG::SetCompressHeader(bool compressheader)
* @brief      SetCompressHeader
* @ingroup    DATAIO
* 
* @param[in]  compressheader : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_CFG::SetCompressHeader(bool compressheader)
{
  this->compressheader = compressheader;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL_CFG::GetCompressContent()
* @brief      GetCompressContent
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL_CFG::GetCompressContent()
{
  return compresscontent;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_CFG::SetCompressContent(bool compresscontent)
* @brief      SetCompressContent
* @ingroup    DATAIO
* 
* @param[in]  compresscontent : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_CFG::SetCompressContent(bool compresscontent)
{
  this->compresscontent = compresscontent;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD DIOCOREPROTOCOL_CFG::GetMinSizeCompressContent()
* @brief      GetMinSizeCompressContent
* @ingroup    DATAIO
* 
* @return     XDWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIOCOREPROTOCOL_CFG::GetMinSizeCompressContent()
{
  return minsizecompresscontent;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_CFG::SetMinSizeCompressContent(XDWORD minsizecompresscontent)
* @brief      SetMinSizeCompressContent
* @ingroup    DATAIO
* 
* @param[in]  minsizecompresscontent : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_CFG::SetMinSizeCompressContent(XDWORD minsizecompresscontent)
{
  this->minsizecompresscontent = minsizecompresscontent;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XMAP<DIOSTREAMCONFIG*, DIOSTREAM*>* DIOCOREPROTOCOL_CFG::DIOStream_GetAll()
* @brief      DIOStream_GetAll
* @ingroup    DATAIO
* 
* @return     XMAP<DIOSTREAMCONFIG*, : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XMAP<DIOSTREAMCONFIG*, DIOSTREAM*>* DIOCOREPROTOCOL_CFG::DIOStream_GetAll()
{
  return &diostreams;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL_CFG::DIOStream_Add(DIOSTREAMCONFIG* diostreamCFG, DIOSTREAM* diostream)
* @brief      DIOStream_Add
* @ingroup    DATAIO
* 
* @param[in]  diostreamCFG : 
* @param[in]  diostream : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL_CFG::DIOStream_Add(DIOSTREAMCONFIG* diostreamCFG, DIOSTREAM* diostream)
{
  if(!diostreamCFG || !diostream)
    {
      return false;
    }

  return diostreams.Add(diostreamCFG, diostream);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL_CFG::DIOStream_Delete(DIOSTREAMCONFIG* diostreamCFG)
* @brief      DIOStream_Delete
* @ingroup    DATAIO
* 
* @param[in]  diostreamCFG : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL_CFG::DIOStream_Delete(DIOSTREAMCONFIG* diostreamCFG)
{
  if(!diostreamCFG)
    {
      return false;
    }

  DIOSTREAM* diostream = diostreams.Get(diostreamCFG);
  if(!diostream)
    {
      return false;
    }

  diostreams.Delete(diostreamCFG);

  delete diostreamCFG;
  delete diostream;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIOCOREPROTOCOL_CFG::DIOStream_DeleteAll()
* @brief      DIOStream_DeleteAll
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIOCOREPROTOCOL_CFG::DIOStream_DeleteAll()
{
  if(!diostreams.GetSize())
    {
      return false;
    }

  diostreams.DeleteKeyContents();
  diostreams.DeleteElementContents();
  
  diostreams.DeleteAll();

  return true;
}




/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAMCONFIG* DIOCOREPROTOCOL_CFG::GetDIOStreamCFG()
* @brief      GetDIOStreamCFG
* @ingroup    DATAIO
* 
* @return     DIOSTREAMCONFIG* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAMCONFIG* DIOCOREPROTOCOL_CFG::GetDIOStreamCFG()
{
  return diostreamCFG;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIOSTREAM* DIOCOREPROTOCOL_CFG::GetDIOStream()
* @brief      GetDIOStream
* @ingroup    DATAIO
* 
* @return     DIOSTREAM* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIOSTREAM* DIOCOREPROTOCOL_CFG::GetDIOStream()
{
  return diostream;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_CFG::SetDIOStreamCFG(DIOSTREAMCONFIG* diostreamCFG)
* @brief      SetDIOStreamCFG
* @ingroup    DATAIO
* 
* @param[in]  diostreamCFG : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_CFG::SetDIOStreamCFG(DIOSTREAMCONFIG* diostreamCFG)
{
  this->diostreamCFG = diostreamCFG;  
}
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_CFG::SetDIOStream(DIOSTREAM* diostream)
* @brief      SetDIOStream
* @ingroup    DATAIO
* 
* @param[in]  diostream : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_CFG::SetDIOStream(DIOSTREAM* diostream)
{
  this->diostream = diostream;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIOCOREPROTOCOL_CFG::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIOCOREPROTOCOL_CFG::Clean()
{
  compressheader           = false;      
  compresscontent          = false;
  minsizecompresscontent   = DIOCOREPROTOCOL_CFG_DEFAULTMINSIZECOMPRESS;
  
  diostreamCFG             = NULL;
  diostream                = NULL; 
}


#pragma endregion

