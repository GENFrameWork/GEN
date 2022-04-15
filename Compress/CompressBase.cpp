//------------------------------------------------------------------------------------------
//  COMPRESSBASE.CPP
//
//  Compression/Descompression Class
//
//  Author            : Abraham J. Velez
//  Date Of Creation  : 28/10/2003
//  Last Mofificacion :
//
//  GEN  Copyright (C).  All right reserved.
//------------------------------------------------------------------------------------------

 /*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


//---- INCLUDES ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "XBuffer.h"

#include "CompressBase.h"

#include "XMemory_Control.h"

//---- GENERAL VARIABLE --------------------------------------------------------------------


//---- CLASS MEMBERS -----------------------------------------------------------------------


//-------------------------------------------------------------------
//  COMPRESSBASE::COMPRESSBASE
/**
//
//
//  ""
//  @version      21/11/2003 16:28:44
//
//  @return
//  */
//-------------------------------------------------------------------
COMPRESSBASE::COMPRESSBASE()
{
  SetType();
}


//-------------------------------------------------------------------
//  COMPRESSBASE::COMPRESSBASE
/**
//
//
//  ""
//  @version      21/11/2003 16:28:55
//
//  @return
//  @param        type :
*/
//-------------------------------------------------------------------
COMPRESSBASE::COMPRESSBASE(COMPRESSBASE_TYPE type)
{
  SetType(type);
}



//-------------------------------------------------------------------
//  COMPRESSBASE::~COMPRESSBASE
/**
//
//
//  ""
//  @version      21/11/2003 16:29:12
//
//  @return
//  */
//-------------------------------------------------------------------
COMPRESSBASE::~COMPRESSBASE()
{
  SetType();
}



//-------------------------------------------------------------------
//  COMPRESSBASE::GetType
/**
//
//
//  ""
//  @version      21/11/2003 16:28:34
//
//  @return       COMPRESSBASE_TYPE :
//  */
//-------------------------------------------------------------------
COMPRESSBASE_TYPE COMPRESSBASE::GetType()
{
  return type;
}



//-------------------------------------------------------------------
//  COMPRESSBASE::SetType
/**
//
//
//  ""
//  @version      21/11/2003 16:28:27
//
//  @return
//  @param        type :
*/
//-------------------------------------------------------------------
COMPRESSBASE_TYPE COMPRESSBASE::SetType(COMPRESSBASE_TYPE type)
{
  this->type = type;

  return type;
}


//-------------------------------------------------------------------
//  COMPRESSBASE::Compress
/**
//
//
//  ""
//  @version      10/02/2004 18:00:06
//
//  @return       bool :
//  @param        origin :
//  @param        size :
//  @param        buffer :
*/
//-------------------------------------------------------------------
bool COMPRESSBASE::Compress(XBYTE* origin,XDWORD size,XBUFFER* buffer)
{
  if(!origin)   return false;
  if(!size)     return false;
  if(!buffer)   return false;

  memcpy(buffer->Get(),origin,size);
  buffer->SetSize(size);

  return true;
}



//-------------------------------------------------------------------
//  COMPRESSBASE::Decompress
/**
//
//
//  ""
//  @version      10/02/2004 18:00:11
//
//  @return       bool :
//  @param        origin :
//  @param        size :
//  @param        buffer :
*/
//-------------------------------------------------------------------
bool COMPRESSBASE::Decompress(XBYTE* origin,XDWORD size,XBUFFER* buffer)
{
  if(!origin)   return false;
  if(!size)     return false;
  if(!buffer)   return false;

  memcpy(buffer->Get(),origin,size);
  buffer->SetSize(size);

  return true;
}


//-------------------------------------------------------------------
//  COMPRESSBASE::Compress
/**
//
//
//  ""
//  @version      21/11/2003 16:42:18
//
//  @return       bool :
//  @param        origin :
//  @param        size :
//  @param        param1 :
*/
//-------------------------------------------------------------------
bool COMPRESSBASE::Compress(XBYTE* origin,XDWORD size,void* param1)
{
  if(!origin) return false;
  if(!size)   return false;

  for(XDWORD c=0; c<size; c++)
    {
      CompressResult(true,origin[c],param1);
    }

  return false;
}



//-------------------------------------------------------------------
//  COMPRESSBASE::Decompress
/**
//
//
//  ""
//  @version      21/11/2003 16:43:39
//
//  @return       bool :
//  @param        origin :
//  @param        size :
//  @param        param1 :
*/
//-------------------------------------------------------------------
bool COMPRESSBASE::Decompress(XBYTE* origin,XDWORD size,void* param1)
{
  if(!origin) return false;
  if(!size)   return false;

  for(XDWORD c=0; c<size; c++)
    {
      CompressResult(false,origin[c],param1);
    }

  return false;
}



//-------------------------------------------------------------------
//  COMPRESSBASE::CompressResult
/**
//
//
//  ""
//  @version      21/11/2003 16:44:04
//
//  @return       bool :
//  @param        compress :
//  @param        data :
//  @param        param1 :
*/
//-------------------------------------------------------------------
bool COMPRESSBASE::CompressResult(bool compress,XBYTE data,void* param1)
{
  if(compress)
    {

    }
   else
    {

    }

  return false;
}




