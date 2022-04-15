/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XFileCobol.cpp
*
* @class      XFILECOBOL
* @brief      eXtended Cobol file class (legacy OLD World)
* @ingroup    UTILS
*
* @copyright  GEN Group. All right reserved.
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

#include <string.h>
#include <stdarg.h>

#include "XFactory.h"
#include "XPath.h"
#include "XFile.h"
#include "XFileTXT.h"
#include "XVariant.h"
#include "XDateTime.h"
#include "XSystem.h"
#include "XTrace.h"

#include "XFileCobol.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/
/*  XFILECOBOL_PIC                                                                                                    */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_PIC::XFILECOBOL_PIC()
* @brief      Constructor
* @ingroup    UTILS
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_PIC::XFILECOBOL_PIC()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_PIC::~XFILECOBOL_PIC()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    UTILS
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_PIC::~XFILECOBOL_PIC()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_PIC_TYPE XFILECOBOL_PIC::GetType()
* @brief      GetType
* @ingroup    UTILS
*
* @return     XFILECOBOL_PIC_TYPE :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_PIC_TYPE XFILECOBOL_PIC::GetType()
{
  return type;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XFILECOBOL_PIC::GetMask()
* @brief      GetDefinition
* @ingroup    UTILS
*
* @return     XSTRING* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XFILECOBOL_PIC::GetMask()
{
  return &mask;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XFILECOBOL_PIC::GetMaskDecondensed()
* @brief      GetMaskDecondensed
* @ingroup    UTILS
*
* @return     XSTRING* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XFILECOBOL_PIC::GetMaskDecondensed()
{
  return &maskdecondensed;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_PIC::ResolveMask()
* @brief      ResolveMask
* @ingroup    UTILS
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_PIC::ResolveMask()
{
  if(mask.IsEmpty())
    {
      XTRACE_PRINTCOLOR(4, __L("Cobol Reader: Error PIC Missing"));
      return false;
    }

  hassign    = false;
  ndecimals  = 0;
  sizedata   = 0;

  XDWORD index      = 0;

  while(index < mask.GetSize())
    {
      switch(mask.Get()[index])
        {
          case __C('X')   : type = XFILECOBOL_PIC_TYPE_BINARY;
                            maskdecondensed += __C('X');
                            sizedata++;
                            index++;
                            break;

          case __C('A')   : type = XFILECOBOL_PIC_TYPE_ALPHA;
                            maskdecondensed += __C('A');
                            sizedata++;
                            index++;
                            break;

          case __C('9')   : if(type != XFILECOBOL_PIC_TYPE_DECIMAL) type = XFILECOBOL_PIC_TYPE_NUMERIC;
                            if(type == XFILECOBOL_PIC_TYPE_DECIMAL) ndecimals++;
                            maskdecondensed += __C('9');
                            sizedata++;
                            index++;
                            break;

          case __C(',')   : type = XFILECOBOL_PIC_TYPE_DECIMAL;
                            maskdecondensed += __C(',');
                            sizedata++;
                            index++;
                            break;

          case __C('S')   : hassign = true;
                            maskdecondensed += __C('S');
                            index++;
                            break;

          case __C('(')   : { XSTRING modificator;
                              int     sizemodificator;

                              int end = mask.Find(__L(")"), true, index+1);
                              if(end == XSTRING_NOTFOUND)
                                {
                                  XTRACE_PRINTCOLOR(4, __L("Cobol Reader: Error PIC invalid %s. Missing ')'"), mask.Get());
                                  return false;
                                }

                             mask.Copy(index, end+1, modificator);
                             modificator.UnFormat(__L("(%d)"), &sizemodificator);

                             if(!sizemodificator)
                               {
                                 XTRACE_PRINTCOLOR(4, __L("Cobol Reader: Error PIC invalid %s. Modificador is zero '(0)'"), mask.Get());
                                 return false;
                               }

                             for(int c=0; c<sizemodificator-1; c++)
                                {
                                  maskdecondensed += mask.Get()[index-1];
                                  sizedata++;
                                  if(type == XFILECOBOL_PIC_TYPE_DECIMAL) ndecimals++;
                                }

                             index = end;
                           }
                           break;

          case __C(')')  : index++;
                           break;

          case __C('V')  : type = XFILECOBOL_PIC_TYPE_DECIMAL;
                           maskdecondensed += __C('V');
                           index++;
                           break;

                 default : XTRACE_PRINTCOLOR(4, __L("Cobol Reader: Error PIC invalid %s. Unknown Specifier '%c'"), mask.Get(), mask.Get()[index]);
                           return false;
                           break;
        }
    }

   switch(computationaltype)
     {
        case 1  : // Float point 4 Bytes
                  break;

        case 2  : // Float Point 8 Bytes
                  break;

        case 3  : // BCD + Sing
                  sizedata = ((sizedata + 1) / 2) + (((sizedata + 1) % 2)?1:0);
                  break;

        case 4  : break;

        case 5  : // Conversion Packet number standard binary data
                  if((sizedata >= 1) && (sizedata <= 2))
                    {
                      sizedata = 1;
                    }
                   else
                    {
                      if((sizedata >=3) && (sizedata <= 4))
                        {
                          sizedata = 2;
                        }
                       else
                        {
                          if((sizedata >= 5) && (sizedata <= 9))
                            {
                              sizedata = 4;
                            }
                           else
                            {
                              if((sizedata >= 10) && (sizedata <= 18))
                                {
                                  sizedata = 8;
                                }
                               else
                                {
                                  XTRACE_PRINTCOLOR(4, __L("Cobol Reader: Error PIC invalid %s. Size of modificator to long %s > 18 "), mask.Get(), sizedata);
                                  return false;
                                }
                            }
                        }
                    }
                  break;
     }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD XFILECOBOL_PIC::GetSizeData()
* @brief      GetSizeData
* @ingroup    UTILS
*
* @return     XDWORD :
*
*---------------------------------------------------------------------------------------------------------------------*/
XDWORD XFILECOBOL_PIC::GetSizeData()
{
  return sizedata;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XFILECOBOL_PIC::GetNDecimals()
* @brief      GetNDecimals
* @ingroup    UTILS
*
* @return     int :
*
*---------------------------------------------------------------------------------------------------------------------*/
int XFILECOBOL_PIC::GetNDecimals()
{
  return ndecimals;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_PIC::HasSign()
* @brief      HasSign
* @ingroup    UTILS
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_PIC::HasSign()
{
  return hassign;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XFILECOBOL_PIC::GetComputationalType()
* @brief      GetComputationalType
* @ingroup    UTILS
*
* @return     int :
*
*---------------------------------------------------------------------------------------------------------------------*/
int XFILECOBOL_PIC::GetComputationalType()
{
  return computationaltype;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILECOBOL_PIC::SetComputationalType(int computationaltype)
* @brief      SetComputationalType
* @ingroup    UTILS
*
* @param[in]  computationaltype :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILECOBOL_PIC::SetComputationalType(int computationaltype)
{
  this->computationaltype = computationaltype;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILECOBOL_PIC::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    UTILS
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILECOBOL_PIC::Clean()
{
  type                =  XFILECOBOL_PIC_TYPE_UNKNOWN;
  mask.Empty();
  sizedata            = 0;
  computationaltype   = 0;
  ndecimals           = 0;
  hassign             = false;
}


/*--------------------------------------------------------------------------------------------------------------------*/
/*  XFILECOBOL_FIELD                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_FIELD::XFILECOBOL_FIELD()
* @brief      Constructor
* @ingroup    UTILS
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_FIELD::XFILECOBOL_FIELD()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_FIELD::~XFILECOBOL_FIELD()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    UTILS
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_FIELD::~XFILECOBOL_FIELD()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_FIELD_TYPE XFILECOBOL_FIELD::GetType()
* @brief      GetType
* @ingroup    UTILS
*
* @return     XFILECOBOL_FIELD_TYPE :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_FIELD_TYPE XFILECOBOL_FIELD::GetType()
{
  return type;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILECOBOL_FIELD::SetType(XFILECOBOL_FIELD_TYPE type)
* @brief      SetType
* @ingroup    UTILS
*
* @param[in]  type :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILECOBOL_FIELD::SetType(XFILECOBOL_FIELD_TYPE type)
{
  this->type = type;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XFILECOBOL_FIELD::GetLevel()
* @brief      GetLevel
* @ingroup    UTILS
*
* @return     int :
*
*---------------------------------------------------------------------------------------------------------------------*/
int XFILECOBOL_FIELD::GetLevel()
{
  return level;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILECOBOL_FIELD::SetLevel(int level)
* @brief      SetLevel
* @ingroup    UTILS
*
* @param[in]  level :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILECOBOL_FIELD::SetLevel(int level)
{
  this->level = level;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XFILECOBOL_FIELD::GetName()
* @brief      GetName
* @ingroup    UTILS
*
* @return     XSTRING* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XFILECOBOL_FIELD::GetName()
{
  return &name;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XFILECOBOL_FIELD::GetPIC()
* @brief      GetPIC
* @ingroup    UTILS
*
* @return     XSTRING* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_PIC* XFILECOBOL_FIELD::GetPIC()
{
  return &PIC;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XFILECOBOL_FIELD::GetNOccurs()
* @brief      GetNOccurs
* @ingroup    UTILS
*
* @return     int :
*
*---------------------------------------------------------------------------------------------------------------------*/
int XFILECOBOL_FIELD::GetNOccurs()
{
  return noccurs;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILECOBOL_FIELD::SetNOccurs(int noccurs)
* @brief      SetNOccurs
* @ingroup    UTILS
*
* @param[in]  noccurs :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILECOBOL_FIELD::SetNOccurs(int noccurs)
{
  this->noccurs = noccurs;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_FIELD::IsRedefine()
* @brief      IsRedefine
* @ingroup    UTILS
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_FIELD::IsRedefine()
{
  return isredefine;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILECOBOL_FIELD::SetIsRedefine(bool isredefine)
* @brief      SetIsRedefine
* @ingroup    UTILS
*
* @param[in]  isredefine :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILECOBOL_FIELD::SetIsRedefine(bool isredefine)
{
  this->isredefine = isredefine;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XFILECOBOL_FIELD::GetRedefineName()
* @brief      GetRedefineName
* @ingroup    UTILS
*
* @return     XSTRING* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XFILECOBOL_FIELD::GetRedefineName()
{
  return &redefinename;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XFILECOBOL_FIELD::GetRedefineOffset()
* @brief      GetRedefineOffset
* @ingroup    UTILS
*
* @return     int :
*
*---------------------------------------------------------------------------------------------------------------------*/
int XFILECOBOL_FIELD::GetRedefineOffset()
{
  return redefineoffset;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILECOBOL_FIELD::SetRedefineOffset(int redefineoffset)
* @brief      SetRedefineOffset
* @ingroup    UTILS
*
* @param[in]  redefineoffset :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILECOBOL_FIELD::SetRedefineOffset(int redefineoffset)
{
  this->redefineoffset = redefineoffset;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILECOBOL_FIELD::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    UTILS
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILECOBOL_FIELD::Clean()
{
  type            = XFILECOBOL_FIELD_TYPE_UNKNOWN;
  level           = 0;
  name.Empty();
  noccurs         = 0;

  isredefine      = false;
  redefinename.Empty();
  redefineoffset  = 0;
}



/*--------------------------------------------------------------------------------------------------------------------*/
/*  XFILECOBOL_RECORD                                                                                                 */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_RECORD::XFILECOBOL_RECORD(XVECTOR<XFILECOBOL_FIELD*>* fields)
* @brief      Constructor
* @ingroup    UTILS
*
* @param[in]  XVECTOR<XFILECOBOL_FIELD*>* :
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_RECORD::XFILECOBOL_RECORD(XFILECOBOL_HANDLE* handle)
{
  Clean();

  this->handle = handle;

  XDWORD sizedata = handle->CalculateDataSize();
  data.Resize(sizedata);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_RECORD::~XFILECOBOL_RECORD()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    UTILS
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_RECORD::~XFILECOBOL_RECORD()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_RECORD_TYPE XFILECOBOL_RECORD::GetType()
* @brief      GetType
* @ingroup    UTILS
*
* @return     XFILECOBOL_RECORD_TYPE :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_RECORD_TYPE XFILECOBOL_RECORD::GetType()
{
  if(!handle) return XFILECOBOL_RECORD_TYPE_UNKNOWN;

  return handle->GetTypeRecord();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_RECORD::SetType(XFILECOBOL_RECORD_TYPE type)
* @brief      SetType
* @ingroup    UTILS
*
* @param[in]  type :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_RECORD::SetType(XFILECOBOL_RECORD_TYPE type)
{
  if(!handle) return false;

  handle->SetTypeRecord(type);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD XFILECOBOL_RECORD::GetNFields()
* @brief      GetNFields
* @ingroup    UTILS
*
* @return     XDWORD :
*
*---------------------------------------------------------------------------------------------------------------------*/
XDWORD XFILECOBOL_RECORD::GetNFields()
{
  if(!handle)               return 0;
  if(!handle->GetFDTree())  return 0;

  XDWORD nnodes = handle->GetFDTree()->GetNNodes();
  if(nnodes) nnodes--;

  return nnodes;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_FIELD* XFILECOBOL_RECORD::GetField(int index)
* @brief      GetField
* @ingroup    UTILS
*
* @param[in]  index :
*
* @return     XFILECOBOL_FIELD* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_FIELD* XFILECOBOL_RECORD::GetField(int index)
{
  if(!handle)               return NULL;
  if(!handle->GetFDTree())  return NULL;

  XFILECOBOL_FD_TREENODEITERATOR  it(handle->GetFDTree()->Begin());
  XFILECOBOL_FD_TREENODEITERATOR  it_end(handle->GetFDTree()->End());

  int counterindex = 0;

  while(it != it_end)
    {
      XFILECOBOL_FIELD* field  = (*it);
      if(!field) break;

      if(counterindex >= index) return field;

      counterindex++;
      ++it;
    }

  return NULL;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_FIELD* XFILECOBOL_RECORD::GetField(XCHAR* name)
* @brief      GetField
* @ingroup    UTILS
*
* @param[in]  name :
*
* @return     XFILECOBOL_FIELD* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_FIELD* XFILECOBOL_RECORD::GetField(XCHAR* name)
{
  if(!handle) return NULL;

  XFILECOBOL_FD_TREENODEITERATOR it(handle->GetFDTree()->Begin());
  XFILECOBOL_FD_TREENODEITERATOR it_end(handle->GetFDTree()->End());

  while(it != it_end)
    {
      XFILECOBOL_FIELD* field = (*it);
      if(!field) break;

      if(!field->GetName()->Compare(name, true)) return field;

      ++it;
    }

  return NULL;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_FIELD* XFILECOBOL_RECORD::GetField(XFILECOBOL_FD_TREENODEITERATOR it)
* @brief      GetField
* @ingroup    UTILS
*
* @param[in]  it :
*
* @return     XFILECOBOL_FIELD* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_FIELD* XFILECOBOL_RECORD::GetField(XFILECOBOL_FD_TREENODEITERATOR& it)
{
  XFILECOBOL_FIELD* field = (*it);

  return field;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_RECORD::GetFieldData(int index, XVARIANT& variant)
* @brief      GetFieldData
* @ingroup    UTILS
*
* @param[in]  index :
* @param[in]  variant :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_RECORD::GetFieldData(int index, XVARIANT& variant)
{
  if(!handle)               return false;
  if(!handle->GetFDTree())  return false;

  XFILECOBOL_FD_TREENODEITERATOR  it((*handle->GetFDTree()), NULL);
  if(!handle->GetInterator(index, it)) return false;

  return GetFieldData(it, variant);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_RECORD::GetFieldData(XCHAR* name, XVARIANT& variant)
* @brief      GetFieldData
* @ingroup    UTILS
*
* @param[in]  name :
* @param[in]  variant :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_RECORD::GetFieldData(XCHAR* name, XVARIANT& variant)
{
  if(!handle)               return false;
  if(!handle->GetFDTree())  return false;

  XFILECOBOL_FD_TREENODEITERATOR  it((*handle->GetFDTree()), NULL);
  if(!handle->GetInterator(name, it)) return false;

  return GetFieldData(it, variant);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVARIANT* XFILECOBOL_RECORD::GetFieldData(XFILECOBOL_FD_TREENODEITERATOR it)
* @brief      GetFieldData
* @ingroup    UTILS
*
* @param[in]  it :
*
* @return     XVARIANT* :
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_RECORD::GetFieldData(XFILECOBOL_FD_TREENODEITERATOR& it, XVARIANT& variant)
{
  XBUFFER           data;
  XFILECOBOL_FIELD* field = GetField(it);

  if(!field) return false;

  if(!GetFieldData(it, data)) return false;

  bool status = false;
  if(data.GetSize()) status = ConvertDataTo(field->GetPIC(), data, variant);

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_RECORD::GetFieldData(int index, XSTRING& datastring)
* @brief      GetFieldData
* @ingroup    UTILS
*
* @param[in]  index :
* @param[in]  datastring :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_RECORD::GetFieldData(int index, XSTRING& datastring)
{
  if(!handle)               return false;
  if(!handle->GetFDTree())  return false;

  XFILECOBOL_FD_TREENODEITERATOR  it((*handle->GetFDTree()), NULL);
  if(!handle->GetInterator(index, it)) return false;

  return GetFieldData(it, datastring);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_RECORD::GetFieldData(XCHAR* name, XSTRING& datastring)
* @brief      GetFieldData
* @ingroup    UTILS
*
* @param[in]  name :
* @param[in]  datastring :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_RECORD::GetFieldData(XCHAR* name, XSTRING& datastring)
{
  if(!handle)               return false;
  if(!handle->GetFDTree())  return false;

  XFILECOBOL_FD_TREENODEITERATOR  it((*handle->GetFDTree()), NULL);
  if(!handle->GetInterator(name, it)) return false;

  return GetFieldData(it, datastring);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_RECORD::GetFieldData(XFILECOBOL_FD_TREENODEITERATOR& it, XSTRING& datastring)
* @brief      GetFieldData
* @ingroup    UTILS
*
* @param[in]  it :
* @param[in]  datastring :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_RECORD::GetFieldData(XFILECOBOL_FD_TREENODEITERATOR& it, XSTRING& datastring)
{
  XBUFFER           data;
  XFILECOBOL_FIELD* field = GetField(it);

  if(!field)                  return false;
  if(!GetFieldData(it, data)) return false;

  bool status = false;
  if(data.GetSize()) status = ConvertDataTo(field->GetPIC(), data, datastring);

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_RECORD::GetFieldData(int index, XBUFFER& data)
* @brief      GetFieldData
* @ingroup    UTILS
*
* @param[in]  index :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_RECORD::GetFieldData(int index, XBUFFER& data)
{
  if(!handle)               return false;
  if(!handle->GetFDTree())  return false;

  XFILECOBOL_FD_TREENODEITERATOR  it((*handle->GetFDTree()), NULL);
  if(!handle->GetInterator(index, it)) return false;

  return GetFieldData(it, data);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_RECORD::GetFieldData(XCHAR* name, XBUFFER& data)
* @brief      GetFieldData
* @ingroup    UTILS
*
* @param[in]  name :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_RECORD::GetFieldData(XCHAR* name, XBUFFER& data)
{
  if(!handle)               return false;
  if(!handle->GetFDTree())  return false;

  XFILECOBOL_FD_TREENODEITERATOR  it((*handle->GetFDTree()), NULL);
  if(!handle->GetInterator(name, it)) return false;

  return GetFieldData(it, data);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_RECORD::GetFieldData(XFILECOBOL_FD_TREENODEITERATOR it, XBUFFER& data)
* @brief      GetFieldData
* @ingroup    UTILS
*
* @param[in]  it :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_RECORD::GetFieldData(XFILECOBOL_FD_TREENODEITERATOR& it, XBUFFER& data)
{
  if(!handle)               return false;
  if(!handle->GetFDTree())  return false;

  XFILECOBOL_FIELD* field = (*it);
  if(!field)                           return false;
  if(!field->GetPIC())                 return false;
  if(!field->GetPIC()->GetSizeData())  return false;

  int position = handle->CalculatePositionData(it);
  if(position == -1) return false;

  data.Resize(field->GetPIC()->GetSizeData());
  this->data.Get(data.Get(), data.GetSize(), position);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XBUFFER* XFILECOBOL_RECORD::GetData()
* @brief      GetData
* @ingroup    UTILS
*
* @return     XBUFFER* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XBUFFER* XFILECOBOL_RECORD::GetData()
{
  return &data;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_RECORD::ConvertDataTo(XFILECOBOL_PIC* picture, XBUFFER& bufferdata, XVARIANT& data)
* @brief      ConvertDataTo
* @ingroup    UTILS
*
* @param[in]  picture :
* @param[in]  bufferdata :
* @param[in]  data :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_RECORD::ConvertDataTo(XFILECOBOL_PIC* picture, XBUFFER& bufferdata, XVARIANT& datavariant)
{
  if(!bufferdata.GetSize()) return false;


  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_RECORD::ConvertDataTo(XFILECOBOL_PIC* picture, XBUFFER& bufferdata, XSTRING& datastring)
* @brief      ConvertDataTo
* @ingroup    UTILS
*
* @param[in]  picture :
* @param[in]  bufferdata :
* @param[in]  datastring :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_RECORD::ConvertDataTo(XFILECOBOL_PIC* picture, XBUFFER& bufferdata, XSTRING& datastring)
{
  datastring.Empty();

  if(!picture)              return false;
  if(!picture->GetMask())   return false;
  if(!bufferdata.GetSize()) return false;

  XSTRING mask;

  mask = picture->GetMaskDecondensed()->Get();
  if(!mask.GetSize()) return false;

  int indexdata = 0;

  switch(picture->GetComputationalType())
    {
      case  0 : for(XDWORD c=0; c<mask.GetSize(); c++)
                  {
                    switch(mask.Get()[c])
                      {
                        case __C('S') : { XBYTE lastdata = bufferdata.GetLastByte();
                                          if(lastdata & XFILECOBOL_MASK_SIGN)
                                            {
                                              datastring += __L("-");
                                              lastdata &= ~XFILECOBOL_MASK_SIGN;

                                              bufferdata.Get()[bufferdata.GetSize()-1] = lastdata;
                                            }
                                        }
                                        break;

                        case __C('9') :
                        case __C('X') :
                        case __C('A') : datastring.AddFormat(__L("%c"),  bufferdata.Get()[indexdata]);
                                        indexdata++;
                                        break;

                        case __C('V') :
                        case __C('.') : datastring += __L(".");
                                        break;
                      }
                  }
                break;

      case  1 : break;

      case  3 : { XBYTE lastdata = bufferdata.GetLastByte();
                  lastdata &= 0x0F;

                  switch(lastdata)
                    {
                      case 0x0C : break;
                      case 0x0D : datastring = __L("-");
                                  break;
                      case 0x0F : break;
                    }

                  lastdata>>=0x4;

                  for(XDWORD c=0; c< bufferdata.GetSize()-1; c++)
                    {
                      datastring.AddFormat(__L("%02X"), bufferdata.Get()[c]);
                    }

                  datastring.AddFormat(__L("%1X"), lastdata);
                }
                break;

      case  5 : switch(bufferdata.GetSize())
                  {
                    case 1 : { char byte;

                               bufferdata.Get((XBYTE&)byte);
                               datastring.Format(__L("%02d"), byte);
                             }
                             break;

                    case 2 : { short word;

                               bufferdata.Get((XWORD&)word);
                               datastring.Format(__L("%04d"), word);
                             }
                             break;

                    case 4 : { int integer;

                               bufferdata.Get((XDWORD&)integer);
                               datastring.Format(__L("%08d"), integer);
                             }
                             break;

                    case 8 : { long long large;

                               bufferdata.Get((XQWORD&)large);
                               datastring.Format(__L("%016d"), large);
                             }
                             break;
                  }
                break;

    }

  return true;
}





/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILECOBOL_RECORD::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    UTILS
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILECOBOL_RECORD::Clean()
{
  handle = NULL;
}



/*--------------------------------------------------------------------------------------------------------------------*/
/*  XFILECOBOL_HANDLE                                                                                                 */
/*--------------------------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_HANDLE::XFILECOBOL_HANDLE()
* @brief      Constructor
* @ingroup    UTILS
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_HANDLE::XFILECOBOL_HANDLE()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_HANDLE::~XFILECOBOL_HANDLE()
* @brief      ~XFILECOBOL_HANDLE
* @ingroup    UTILS
*
* @return     virtual :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_HANDLE::~XFILECOBOL_HANDLE()
{
  fdtree.DeleteContents();
  fdtree.DeleteAll();

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XFILECOBOL_HANDLE::GetName()
* @brief      GetName
* @ingroup    UTILS
*
* @return     XSTRING* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XFILECOBOL_HANDLE::GetName()
{
  return &name;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XFILECOBOL_HANDLE::GetValueLabel()
* @brief      GetValueLabel
* @ingroup    UTILS
*
* @return     XSTRING* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XFILECOBOL_HANDLE::GetValueLabel()
{
  return &valuelabel;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XSTRING* XFILECOBOL_HANDLE::GetDataRecord()
* @brief      GetDataRecord
* @ingroup    UTILS
*
* @return     XSTRING* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XSTRING* XFILECOBOL_HANDLE::GetDataRecord()
{
  return &datarecord;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_FD_TREE* XFILECOBOL_HANDLE::GetFDTree()
* @brief      GetFDTree
* @ingroup    UTILS
*
* @return     XFILECOBOL_FD_TREE* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_FD_TREE* XFILECOBOL_HANDLE::GetFDTree()
{
  return &fdtree;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XFILECOBOL_HANDLE::GetIndexNodeForName(XFILECOBOL_FD_TREENODE* node, XCHAR* name)
* @brief      GetIndexNodeForName
* @ingroup    UTILS
*
* @param[in]  node :
* @param[in]  name :
*
* @return     int :
*
*---------------------------------------------------------------------------------------------------------------------*/
int XFILECOBOL_HANDLE::GetIndexNodeForName(XFILECOBOL_FD_TREENODE* node, XCHAR* name)
{
  if(!node) return NOTFOUND;

  int index = NOTFOUND;

  for(XDWORD c=0; c<node->GetChildren()->GetSize(); c++)
    {
      XFILECOBOL_FD_TREENODE* subnode = node->GetChildren()->Get(c);
      if(subnode)
        {
          XFILECOBOL_FIELD* field = subnode->GetData();
          if(field)
            {
              if(!field->GetName()->Compare(name)) return (int)c;
            }
        }
    }

  return index;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_RECORD_TYPE XFILECOBOL_HANDLE::GetTypeRecord()
* @brief      GetTypeRecord
* @ingroup    UTILS
*
* @return     XFILECOBOL_RECORD_TYPE :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_RECORD_TYPE  XFILECOBOL_HANDLE::GetTypeRecord()
{
  return typerecord;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILECOBOL_HANDLE::SetTypeRecord(XFILECOBOL_RECORD_TYPE type)
* @brief      SetTypeRecord
* @ingroup    UTILS
*
* @param[in]  type :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILECOBOL_HANDLE::SetTypeRecord(XFILECOBOL_RECORD_TYPE type)
{
  typerecord = type;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_HANDLE::GetInterator(XDWORD index, XFILECOBOL_FD_TREENODEITERATOR& it);
* @brief      GetInterator
* @ingroup    UTILS
*
* @param[in]  index :
* @param[in]  it :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_HANDLE::GetInterator(XDWORD index, XFILECOBOL_FD_TREENODEITERATOR& it)
{
  if(!GetFDTree()) return false;

  XFILECOBOL_FD_TREENODEITERATOR  it_start(GetFDTree()->Begin());
  XFILECOBOL_FD_TREENODEITERATOR  it_end(GetFDTree()->End());

  XDWORD counterindex = 0;

  it = it_start;

  while(it_start != it_end)
    {
      if(counterindex >= index)
        {
          it = it_start;
          return true;
        }

      counterindex++;

      ++it_start;
    }

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL_HANDLE::GetInterator(XCHAR* name, XFILECOBOL_FD_TREENODEITERATOR& it)
* @brief      GetInterator
* @ingroup    UTILS
*
* @param[in]  name :
* @param[in]  it :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL_HANDLE::GetInterator(XCHAR* name, XFILECOBOL_FD_TREENODEITERATOR& it)
{
  if(!GetFDTree()) return false;

  XFILECOBOL_FD_TREENODEITERATOR  it_start(GetFDTree()->Begin());
  XFILECOBOL_FD_TREENODEITERATOR  it_end(GetFDTree()->End());

  it = it_start;

  while(it_start != it_end)
    {
      XFILECOBOL_FIELD* field = (*it_start);
      if(!field) return false;

      if(!field->GetName()->Compare(name, true))
        {
          it = it_start;
          return true;
        }

      ++it_start;
    }

  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD XFILECOBOL_HANDLE::CalculateDataSizeNode(XFILECOBOL_FD_TREENODE* node)
* @brief      CalculateDataSizeNode
* @ingroup    UTILS
*
* @param[in]  node :
*
* @return     XDWORD :
*
*---------------------------------------------------------------------------------------------------------------------*/
XDWORD XFILECOBOL_HANDLE::CalculateDataSizeNode(XFILECOBOL_FD_TREENODE* node)
{
  static XDWORD redefineoffset = 0;
  XDWORD size     = 0;
  if(!node) return size;

  if(node->GetData()->IsRedefine())
    {
      node->GetData()->SetRedefineOffset(redefineoffset);

      redefineoffset  = node->GetData()->GetPIC()->GetSizeData();

      for(XDWORD c=0; c<(XDWORD)node->GetNChildren(); c++)
        {
          XFILECOBOL_FD_TREENODE* subnode = node->GetChild(c);
          if(subnode)
            {
              subnode->GetData()->SetIsRedefine(true);
              CalculateDataSizeNode(subnode);
            }
        }
    }
   else
    {
      redefineoffset = 0;

      if(node->GetData()->GetPIC())
        {
          size = node->GetData()->GetPIC()->GetSizeData();
        }

      for(XDWORD c=0; c<(XDWORD)node->GetNChildren(); c++)
        {
          XFILECOBOL_FD_TREENODE* subnode = node->GetChild(c);
          if(subnode)
            {
              size +=  CalculateDataSizeNode(subnode);
            }
        }
    }

  return size;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD XFILECOBOL_HANDLE::CalculateDataSize()
* @brief      CalculateDataSize
* @ingroup    UTILS
*
* @return     XDWORD :
*
*---------------------------------------------------------------------------------------------------------------------*/
XDWORD XFILECOBOL_HANDLE::CalculateDataSize()
{
  XDWORD size = 0;

  XFILECOBOL_FD_TREENODE* node = GetFDTree()->GetRoot();
  if(!node)  return size;

  for(XDWORD c=0; c<(XDWORD)node->GetNChildren(); c++)
    {
      XFILECOBOL_FD_TREENODE* subnode = node->GetChild(c);
      if(subnode) size =  CalculateDataSizeNode(subnode);
    }

  return size;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         int XFILECOBOL_HANDLE::CalculatePositionData(XFILECOBOL_FD_TREENODEITERATOR it)
* @brief      CalculatePositionData
* @ingroup    UTILS
*
* @param[in]  it :
*
* @return     int :
*
*---------------------------------------------------------------------------------------------------------------------*/
int XFILECOBOL_HANDLE::CalculatePositionData(XFILECOBOL_FD_TREENODEITERATOR& it)
{
  if(!GetFDTree()) return -1;

  XFILECOBOL_FD_TREENODEITERATOR  it_start(GetFDTree()->Begin());
  XFILECOBOL_FD_TREENODEITERATOR  it_end(GetFDTree()->End());

  int        position            = 0;
  static int positionposredefine = 0;

  while(it_start != it_end)
    {
      XFILECOBOL_FIELD* fieldtmp  = (*it_start);
      if(!fieldtmp) break;

      if(fieldtmp)
        {
          if(fieldtmp->GetPIC())
            {
              if(!fieldtmp->IsRedefine())
                {
                  if(positionposredefine)
                    {
                      position            = positionposredefine;
                      positionposredefine = 0;
                    }

                  if(it_start == it) break;

                  position += fieldtmp->GetPIC()->GetSizeData();
                }
               else
                {
                  if(fieldtmp->GetRedefineName()->IsEmpty())
                    {
                      position += fieldtmp->GetRedefineOffset();

                      if(it_start == it) break;
                    }
                   else
                    {
                      XFILECOBOL_FD_TREENODEITERATOR itredefine(GetFDTree()->Begin());

                      if(GetInterator(fieldtmp->GetRedefineName()->Get(), itredefine))
                        {
                          position            = CalculatePositionData(itredefine);
                          positionposredefine = position + (*itredefine)->GetPIC()->GetSizeData();
                        }

                      if(it_start == it) break;
                    }
                }
            }
        }

      ++it_start;
    }

  return position;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_FIELD* XFILECOBOL_HANDLE::IsFieldRedefinedByAnother(XCHAR* name)
* @brief      IsFieldRedefinedByAnother
* @ingroup    UTILS
*
* @param[in]  name :
*
* @return     XFILECOBOL_FIELD* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_FIELD* XFILECOBOL_HANDLE::IsFieldRedefinedByAnother(XCHAR* name)
{
  if(!GetFDTree()) return NULL;

  XFILECOBOL_FD_TREENODEITERATOR  it_start(GetFDTree()->Begin());
  XFILECOBOL_FD_TREENODEITERATOR  it_end(GetFDTree()->End());

  while(it_start != it_end)
    {
      XFILECOBOL_FIELD* fieldtmp  = (*it_start);
      if(!fieldtmp) break;

      if(!fieldtmp->GetRedefineName()->IsEmpty())
        {
          if(!fieldtmp->GetRedefineName()->Compare(name))
            {
              return fieldtmp;
            }
        }

      ++it_start;
    }

  return NULL;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILECOBOL_HANDLE::Clean();
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    UTILS
*
* @param[in]  ) :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILECOBOL_HANDLE::Clean()
{
  typerecord =  XFILECOBOL_RECORD_TYPE_UNKNOWN;
}



/*--------------------------------------------------------------------------------------------------------------------*/
/*  XFILECOBOL                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL::XFILECOBOL()
* @brief      Constructor
* @ingroup    UTILS
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL::XFILECOBOL()
{
  Clean();

  validchars = XFILECOBOL_VALIDCHARS;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL::~XFILECOBOL()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    UTILS
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL::~XFILECOBOL()
{
  Close();

  CapabilitiesFile_UnLoad();

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::Open(int nfiles, XCHAR* xpathcpyfiles, XCHAR* xpathdatfiles, ...)
* @brief      Open
* @ingroup    UTILS
*
* @param[in]  nfiles :
* @param[in]  xpathcpyfiles :
* @param[in]  xpathdatfiles :
* @param[in]  ... :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::Open(int nfiles, XCHAR* xpathcpyfiles, XCHAR* xpathdatfiles, ...)
{
  XPATH     xpath_cpy;
  va_list   arg;

  this->xpathcpyfiles      = xpathcpyfiles;
  this->xpathcpyfiles.Slash_Add();

  this->xpathdatfiles      = xpathdatfiles;
  this->xpathdatfiles.Slash_Add();

  if(capabilitiesfilemap.IsEmpty())
    {
      XPATH  xpathcapabilitiesfile;

      xpathcapabilitiesfile  = xpathcpyfiles;
      xpathcapabilitiesfile.Slash_Add();
      xpathcapabilitiesfile += XFILECOBOL_CAPABILITIESFILE;

      CapabilitiesFile_Load(xpathcapabilitiesfile);
    }

  va_start(arg, xpathdatfiles);

  handle.GetFDTree()->DeleteContents();
  handle.GetFDTree()->DeleteAll();

  for(int c=0; c<nfiles; c++)
    {
      XCHAR* namefile = va_arg(arg, XCHAR*);

      xpath_cpy = xpathcpyfiles;
      xpath_cpy.Slash_Add();
      xpath_cpy += namefile;

      XFILETXT*  file_fd = new XFILETXT();
      if(file_fd)
        {
          if(file_fd->Open(xpath_cpy, true))
            {
              file_fd->ReadAllFile();

              TraslateTextToHandle(file_fd->GetLines(), handle);

              file_fd->Close();
            }

          delete file_fd;
        }
    }

  va_end(arg);

  sizedatarecord = handle.CalculateDataSize();
  if(!sizedatarecord) return false;

  if(this->xpathdatafile.IsEmpty()) return false;


  XPATH xpathnamefile;

  this->xpathdatafile.GetNamefileExt(xpathnamefile);
  xpathnamefile.Insert(this->xpathdatfiles.Get(), 0);


  GEN_XFACTORY_CREATE(xfiledata, Create_File())
  if(!xfiledata) return false;

  bool status = xfiledata->Open(xpathnamefile.Get());

  if(status)
    {
      xfiledata->SetPosition(0);
      if(xfiledata->Read((XBYTE*)&fileheadersize, sizeof(XDWORD)))
        {
          if(GEN_XSYSTEM.HardwareUseLittleEndian()) fileheadersize = SWAPDWORD(fileheadersize);

          switch(fileheadersize)
            {
              case 0x307E0000 : { fileheadersize = 0x80;
                                  SetHaveFileHeaders(true);
                                  type = XFILECOBOL_FILE_TYPE_MF4;
                                }
                                break;

              case 0x3000007C : { fileheadersize = 0x80;
                                  SetHaveFileHeaders(true);
                                  type = XFILECOBOL_FILE_TYPE_MF4;
                                }
                                break;

              case 0x300003FC : { fileheadersize = 0x80;
                                  SetHaveFileHeaders(true);
                                  type = XFILECOBOL_FILE_TYPE_C_ISAM;
                                }
                                break;

                    default   : { fileheadersize = 0x00;
                                  SetHaveFileHeaders(false);
                                  type = XFILECOBOL_FILE_TYPE_C_ISAM;
                                }
                                break;
            }

          if(fileheadersize)
            {
              XWORD ID = 0;

              status = false;

              xfiledata->SetPosition(36);
              if(xfiledata->Read((XBYTE*)&ID, sizeof(XWORD)))
                {
                  if(GEN_XSYSTEM.HardwareUseLittleEndian()) ID = SWAPWORD(ID);
                  if(ID == 0x003E)
                    {
                      xfiledata->SetPosition(54);
                      if(xfiledata->Read((XBYTE*)&maxfilerecordsize, sizeof(XDWORD)))
                        {
                          if(GEN_XSYSTEM.HardwareUseLittleEndian()) maxfilerecordsize = SWAPDWORD(maxfilerecordsize);

                          xfiledata->SetPosition(58);
                          if(xfiledata->Read((XBYTE*)&minfilerecordsize, sizeof(XDWORD)))
                            {
                              if(GEN_XSYSTEM.HardwareUseLittleEndian()) minfilerecordsize = SWAPDWORD(minfilerecordsize);

                              if(maxfilerecordsize < 4095) fileheaderrecordize = 2; else fileheaderrecordize = 4;

                              // Check if the size in the file header is diferent
                              if(maxfilerecordsize == handle.CalculateDataSize())
                                {
                                  status = true;
                                }

                              if(status) filerecordpadding = 16 - ((fileheaderrecordize + maxfilerecordsize) % 16);
                            }
                        }
                    }
                }
            }
        }
    }

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::IsOpen()
* @brief      IsOpen
* @ingroup    UTILS
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::IsOpen()
{
  if(!xfiledata) return false;
  return xfiledata->IsOpen();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_FILE_TYPE XFILECOBOL::GetType()
* @brief      GetType
* @ingroup    UTILS
*
* @return     XFILECOBOL_FILE_TYPE :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_FILE_TYPE XFILECOBOL::GetType()
{
  return type;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILECOBOL::SetType(XFILECOBOL_FILE_TYPE type)
* @brief      SetType
* @ingroup    UTILS
*
* @param[in]  type :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILECOBOL::SetType(XFILECOBOL_FILE_TYPE type)
{
  this->type = type;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::HaveFileHeaders()
* @brief      HaveFileHeaders
* @ingroup    UTILS
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::HaveFileHeaders()
{
  return havefileheaders;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILECOBOL::SetHaveFileHeaders(bool havefileheaders)
* @brief      SetHaveFileHeaders
* @ingroup    UTILS
*
* @param[in]  havefileheaders :
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILECOBOL::SetHaveFileHeaders(bool havefileheaders)
{
  this->havefileheaders = havefileheaders;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD XFILECOBOL::GetNRecords()
* @brief      GetNRecords
* @ingroup    UTILS
*
* @return     XDWORD :
*
*---------------------------------------------------------------------------------------------------------------------*/
XDWORD XFILECOBOL::GetNRecords()
{
  XQWORD size       = xfiledata->GetSize();
  XDWORD sizerecord = handle.CalculateDataSize();

  if(havefileheaders)
    {
      sizerecord += (filerecordpadding + fileheaderrecordize);
      size       -= fileheadersize;
    }

  if(!size || !sizerecord) return 0;

  return (XDWORD)(size / sizerecord);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::ReadRecord(XDWORD index, XFILECOBOL_RECORD& record)
* @brief      ReadRecord
* @ingroup    UTILS
*
* @param[in]  index :
* @param[in]  record :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::ReadRecord(XDWORD index, XFILECOBOL_RECORD& record)
{
  if(!IsOpen()) return false;

  if(index<1) return false;

  int position = 0;

  if(havefileheaders)  position = fileheadersize;

  switch(record.GetType())
    {
      case XFILECOBOL_RECORD_TYPE_UNKNOWN           :
                                        default     :

      case XFILECOBOL_RECORD_TYPE_INDEXED           :
      case XFILECOBOL_RECORD_TYPE_LINE_SEQUENTIAL   :  if(havefileheaders)
                                                          {
                                                              position += ((index-1) * (sizedatarecord + fileheaderrecordize + filerecordpadding));
                                                              position += fileheaderrecordize;
                                                          }
                                                         else position += ((index-1) * (sizedatarecord + 1));

                                                      break;
      case XFILECOBOL_RECORD_TYPE_SEQUENTIAL        :
      case XFILECOBOL_RECORD_TYPE_RELATIVE          : break;
    }



  xfiledata->SetPosition((XDWORD)position);

  return xfiledata->Read(record.GetData()->Get(), record.GetData()->GetSize());
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::ReadRecords(XVECTOR<XFILECOBOL_RECORD*>& records, XDWORD startindex)
* @brief      ReadRecords
* @ingroup    UTILS
*
* @param[in]  records :
* @param[in]  startindex :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::ReadRecords(XVECTOR<XFILECOBOL_RECORD*>& records, XDWORD startindex)
{

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::Close()
* @brief      Close
* @ingroup    UTILS
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::Close()
{
  xpathdatafile.Empty();

  handle.GetFDTree()->DeleteContents();
  handle.GetFDTree()->DeleteAll();

  if(xfiledata)
    {
      xfiledata->Close();

      GEN_XFACTORY.Delete_File(xfiledata);
      xfiledata = NULL;
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_HANDLE* XFILECOBOL::GetHandle()
* @brief      GetHandle
* @ingroup    UTILS
*
* @return     XFILECOBOL_HANDLE* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_HANDLE* XFILECOBOL::GetHandle()
{
  return &handle;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XPATH* XFILECOBOL::GetPathDataFile()
* @brief      GetPathDataFile
* @ingroup    UTILS
*
* @return     XPATH* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XPATH* XFILECOBOL::GetPathDataFile()
{
  return &xpathdatafile;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL:CapabilitiesFile_Load(XPATH& xpathcapabilitiesfile)
* @brief      LoadCapabilitiesFile
* @ingroup    UTILS
*
* @param[in]  xpathcapabilitiesfile :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::CapabilitiesFile_Load(XPATH& xpathcapabilitiesfile)
{
  XFILETXT* filetxt;
  bool      status = false;

  CapabilitiesFile_UnLoad();

  filetxt = new XFILETXT();
  if(!filetxt) return false;

  if(filetxt->Open(xpathcapabilitiesfile))
    {
      filetxt->ReadAllFile();

      XVECTOR<XSTRING*>* lines = filetxt->GetLines();
      if(lines)
        {
          for(XDWORD c=0; c<lines->GetSize(); c++)
            {
              XSTRING* IDfile    = new XSTRING();
              XPATH*   xpathfile = new XPATH();

              if(IDfile && xpathfile)
                {
                  XSTRING* line  = lines->Get(c);
                  if(line)
                    {
                      if(!line->IsEmpty())
                        {
                          if(line->Get()[0] != __C('#'))
                            {
                              IDfile->AdjustSize(_MAXSTR);
                              xpathfile->AdjustSize(_MAXSTR);

                              line->UnFormat(__L("%s:%s"), IDfile->Get(), xpathfile->Get());

                              IDfile->AdjustSize();
                              xpathfile->AdjustSize();

                              if(!IDfile->IsEmpty() && !xpathfile->IsEmpty())
                                {
                                  capabilitiesfilemap.Add(IDfile, xpathfile);
                                  continue;
                                }
                            }
                        }
                    }
                }

              delete IDfile;
              delete xpathfile;
            }
        }

      filetxt->Close();
      status = true;
    }

  delete filetxt;

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::CapabilitiesFile_UnLoad()
* @brief      UnLoadCapabilitiesFile
* @ingroup    UTILS
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::CapabilitiesFile_UnLoad()
{
  if(capabilitiesfilemap.IsEmpty()) return false;

  capabilitiesfilemap.DeleteKeyContents();
  capabilitiesfilemap.DeleteElementContents();
  capabilitiesfilemap.DeleteAll();

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::CapabilitiesFile_GetFile(XSTRING* fileID, XPATH& xpathfile)
* @brief      CapabilitiesFile_GetFile
* @ingroup    UTILS
*
* @param[in]  fileID :
* @param[in]  xpathfile :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::CapabilitiesFile_GetFile(XSTRING* fileID, XPATH& xpathfile)
{
  if(!fileID) return false;

  xpathfile.Empty();

  for(XDWORD c=0; c<capabilitiesfilemap.GetSize(); c++)
    {
      if(capabilitiesfilemap.GetKey(c))
        {
          if(!capabilitiesfilemap.GetKey(c)->Compare(fileID->Get(), true))
            {
              if(capabilitiesfilemap.GetElement(c))
                {
                  xpathfile = capabilitiesfilemap.GetElement(c)->Get();
                  break;
                }
            }
        }
    }

  return xpathfile.IsEmpty()?false:true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::IsValidCharacter(XCHAR character)
* @brief      IsValidCharacter
* @ingroup    UTILS
*
* @param[in]  character :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::IsValidCharacter(XCHAR character)
{
  bool found = false;

  for(XDWORD c=0; c<validchars.GetSize(); c++)
    {
      if(validchars.Get()[c] == character) found = true;
    }

  return found;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::Eliminate_InvalidCharacters(XSTRING& line)
* @brief      Eliminate_InvalidCharacters
* @ingroup    UTILS
*
* @param[in]  line :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::Eliminate_InvalidCharacters(XSTRING& line)
{
  if(line.IsEmpty()) return false;

  XDWORD c = 0;
  while(c<line.GetSize())
    {
      if(!IsValidCharacter(line.Get()[c]))
        {
          line.DeleteCharacters(c,1);

        } else c++;
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::Eliminate_TabsForSpaces(XVECTOR<XSTRING*>* lines)
* @brief      Eliminate_TabsForSpaces
* @ingroup    UTILS
*
* @param[in]  lines :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::Eliminate_TabsForSpaces(XVECTOR<XSTRING*>* lines)
{
  if(!lines) return false;

  XSTRING tabspaces;

  for(int c=0; c<XFILECOBOL_TABSFORSPACES; c++)
    {
      tabspaces += __L(" ");
    }

  for(XDWORD c=0; c<lines->GetSize(); c++)
    {
      XSTRING* line = lines->Get(c);
      if(line)
        {
          int index = line->Find(__L("\x09"), true);
          if(index != XSTRING_NOTFOUND)
            {
              line->DeleteCharacters(index, 1);
              line->Insert(tabspaces, index);
            }
        }
    }

  return true;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::Eliminate_InvalidCharacters(XVECTOR<XSTRING*>* lines)
* @brief      Eliminate_InvalidCharacters
* @ingroup    UTILS
*
* @param[in]  lines :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::Eliminate_InvalidCharacters(XVECTOR<XSTRING*>* lines)
{
  if(!lines) return false;

  for(XDWORD c=0; c<lines->GetSize(); c++)
    {
      XSTRING* line = lines->Get(c);
      if(line) Eliminate_InvalidCharacters((*line));
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::Eliminate_Coments(XVECTOR<XSTRING*>* lines)
* @brief      Eliminate_Coments
* @ingroup    UTILS
*
* @param[in]  lines :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::Eliminate_Comments(XVECTOR<XSTRING*>* lines)
{
  if(!lines) return false;

  for(XDWORD c=0; c<lines->GetSize(); c++)
    {
      XSTRING* line = lines->Get(c);
      if(line)
        {
          int index = line->Find(__L("*"), true);
          if(index != XSTRING_NOTFOUND) line->DeleteCharacters(index, line->GetSize()-index);
        }
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::Eliminate_NumberLines(XVECTOR<XSTRING*>* lines)
* @brief      Eliminate_NumberLines
* @ingroup    UTILS
*
* @param[in]  lines :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::Eliminate_NumberLines(XVECTOR<XSTRING*>* lines)
{
  if(!lines) return false;

  for(XDWORD c=0; c<lines->GetSize(); c++)
    {
      XSTRING* line = lines->Get(c);
      if(line) line->DeleteCharacters(0, 7);
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::Eliminate_EmptyLines(XVECTOR<XSTRING*>* lines)
* @brief      Eliminate_EmptyLines
* @ingroup    UTILS
*
* @param[in]  lines :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::Eliminate_EmptyLines(XVECTOR<XSTRING*>* lines)
{
  if(!lines) return false;

  XDWORD c=0;
  while(c<lines->GetSize())
    {
      XSTRING* line = lines->Get(c);
      if(line)
        {
          line->DeleteCharacter(__C(' '), XSTRINGCONTEXT_FROM_FIRST);
          line->DeleteCharacter(__C(' '), XSTRINGCONTEXT_TO_END);
          if(line->IsEmpty())
            {
              delete line;
              lines->Delete(line);

            } else c++;
        }
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::Eliminate_UnnecessarySpaces(XVECTOR<XSTRING*>* lines)
* @brief      Eliminate_UnnecessarySpaces
* @ingroup    UTILS
*
* @param[in]  lines :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::Eliminate_UnnecessarySpaces(XVECTOR<XSTRING*>* lines)
{
  if(!lines) return false;

  for(XDWORD c=0; c<lines->GetSize(); c++)
    {
      XSTRING* line = lines->Get(c);
      if(line)
        {
          XDWORD d = 0;

          while(d<line->GetSize()-1)
            {
              if((line->Get()[d] == __C(' ')) && (line->Get()[d+1] == __C(' ')))
                {
                  line->DeleteCharacters(d, 1);

                } else d++;
            }
        }
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::CreateUniqueLines(XVECTOR<XSTRING*>* lines)
* @brief      CreateUniqueLines
* @ingroup    UTILS
*
* @param[in]  lines :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::CreateUniqueLines(XVECTOR<XSTRING*>* lines)
{
  if(!lines) return false;

  XDWORD c=0;
  while(c<lines->GetSize())
    {
      XSTRING* line = lines->Get(c);
      if(line)
        {
          XSTRING* linenext = lines->Get(c+1);
          if((linenext) && (line->Get()[line->GetSize()-1] != __C('.')))
            {
              line->Add(__C(' '));
              line->Add(linenext->Get());

              delete linenext;

              lines->Delete(linenext);

            } else c++;
        }
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::ExtractReservedToken(XSTRING& line, XCHAR* token, XSTRING& var)
* @brief      ExtractReservedToken
* @ingroup    UTILS
*
* @param[in]  line :
* @param[in]  token :
* @param[in]  var :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::ExtractReservedToken(XSTRING& line, XCHAR* token, XSTRING& var)
{
  XSTRING mask;
  XSTRING strvar;

  mask.Format(__L("%s "), token);

  int start         = line.Find(mask.Get(), true);
  int startendtoken = 0;
  int end           = 0;
  if(start == XSTRING_NOTFOUND)  return false;

  startendtoken = start + mask.GetSize();

  end = line.Find(__L(" "), true, startendtoken);
  if(end == XSTRING_NOTFOUND) end = line.Find(__L("."), true, startendtoken);
  if(end == XSTRING_NOTFOUND) return false;

  line.Copy(startendtoken, end, var);
  line.DeleteCharacters(start, (end-start));

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::ExtractReservedToken(XSTRING& line, XCHAR* token)
* @brief      ExtractReservedToken
* @ingroup    UTILS
*
* @param[in]  line :
* @param[in]  token :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::ExtractReservedToken(XSTRING& line, XCHAR* token)
{
  XSTRING mask;
  XSTRING strvar;

  mask = token;

  int start         = line.Find(mask.Get(), true);
  int end           = 0;
  if(start == XSTRING_NOTFOUND)  return false;

  end = start + mask.GetSize();
  line.DeleteCharacters(start, (end-start));

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_LINE_TYPE XFILECOBOL::InterpreterLineForHandle(XSTRING& line, int& level, XFILECOBOL_FIELD_TYPE& fieldtype, XFILECOBOL_HANDLE& handle, XSTRING& name, XSTRING& param, XSTRING& param2, int& noccurs, int& PICcomputationaltype)
* @brief      InterpreterLineForHandle
* @ingroup    UTILS
*
* @param[in]  line :
* @param[in]  level :
* @param[in]  fieldtype :
* @param[in]  handle :
* @param[in]  name :
* @param[in]  param :
* @param[in]  param2 :
* @param[in]  noccurs :
* @param[in]  PICcomputationaltype :
*
* @return     XFILECOBOL_LINE_TYPE :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_LINE_TYPE XFILECOBOL::InterpreterLineForHandle(XSTRING& line, int& level, XFILECOBOL_FIELD_TYPE& fieldtype, XFILECOBOL_HANDLE& handle, XSTRING& name, XSTRING& param, XSTRING& param2, int& noccurs, int& PICcomputationaltype)
{
  XFILECOBOL_LINE_TYPE  returntype = XFILECOBOL_LINE_TYPE_UNKNOWN;
  int                   index      = -1;

  level                 = 0;
  fieldtype             =  XFILECOBOL_FIELD_TYPE_UNKNOWN;
  PICcomputationaltype  = 0;

  //----------------------------------------------------------
  // FD
  index = line.Find(__L("FD "), true);
  if(!index)
    {
      ExtractReservedToken(line, __L("VALUE OF LABEL IS"), (*handle.GetValueLabel()));
      param.DeleteCharacter(__C('\''), XSTRINGCONTEXT_ALLSTRING);

      ExtractReservedToken(line, __L("DATA RECORD IS"), (*handle.GetDataRecord()));
      ExtractReservedToken(line, __L("FD"), (*handle.GetName()));

      returntype = XFILECOBOL_LINE_TYPE_FD;
    }
   else
    {
      index = line.Find(__L("SELECT "), true);
      if(!index)
        {
          ExtractReservedToken(line, __L("SELECT")          , name);
          ExtractReservedToken(line, __L("ORGANIZATION IS") , param);
          ExtractReservedToken(line, __L("ASSIGN TO")       , param2);

          returntype = XFILECOBOL_LINE_TYPE_SELECT;
        }
       else
        {
          //----------------------------------------------------------
          // FIELD LEVEL

          XSTRING levelstring;

          line.Copy(0, 2, levelstring);

          level = levelstring.ConvertToInt();
          if(level)
            {
              noccurs = 0;

              ExtractReservedToken(line, __L(" TIMES"));
              ExtractReservedToken(line, __L("OCCURS"), param2);
              if(!param2.IsEmpty())
                {
                  param2.UnFormat(__L("%d"), &noccurs);
                  param2.Empty();
                }

              ExtractReservedToken(line, __L("REDEFINES"), param2);

              if(line.Find(__L("PIC "), true) != XSTRING_NOTFOUND)
                {
                  if(ExtractReservedToken(line, __L("COMP-1")))  PICcomputationaltype = 1;
                  if(ExtractReservedToken(line, __L("COMP-2")))  PICcomputationaltype = 2;
                  if(ExtractReservedToken(line, __L("COMP-3")))  PICcomputationaltype = 3;
                  if(ExtractReservedToken(line, __L("COMP-4")))  PICcomputationaltype = 4;
                  if(ExtractReservedToken(line, __L("COMP-5")))  PICcomputationaltype = 5;

                  ExtractReservedToken(line, __L("PIC"), param);

                  name.AdjustSize(_MAXSTR);
                  line.UnFormat(__L("%02d %s"), &level, name.Get());
                  name.AdjustSize();

                  fieldtype =  XFILECOBOL_FIELD_TYPE_VAR;
                }
               else
                {
                  param.Empty();

                  name.AdjustSize(_MAXSTR);
                  line.UnFormat(__L("%02d %s."), &level, name.Get());
                  name.AdjustSize();

                  fieldtype =  XFILECOBOL_FIELD_TYPE_KEY;
                }

              returntype = XFILECOBOL_LINE_TYPE_FIELD;
            }

           name.DeleteCharacter(__C(' '));
           param.DeleteCharacter(__C(' '));
           param2.DeleteCharacter(__C(' '));

           name.DeleteCharacter(__C('.'));
           param.DeleteCharacter(__C('.'));
           param2.DeleteCharacter(__C('.'));
        }
    }

  return returntype;
}

 /**-------------------------------------------------------------------------------------------------------------------
 *
 *  @fn         XFILECOBOL_FD_TREENODE* XFILECOBOL::GetParentNodeOfLevel(int level)
 *  @brief      GetParentNodeOfLevel
 *  @ingroup    UTILS
 *
 *  ""
 *  ""
 *
 *  @param[in]  level :
 *
 *  @return     XFILECOBOL_FD_TREENODE* :
 *
 *---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_FD_TREENODE* XFILECOBOL::GetParentNodeOfLevel(int level)
{
  if(!handle.GetFDTree()) return NULL;

  XFILECOBOL_FD_TREENODEITERATOR  it_start(handle.GetFDTree()->Begin());
  XFILECOBOL_FD_TREENODEITERATOR  it_end(handle.GetFDTree()->End());

  while(it_start != it_end)
    {
      XFILECOBOL_FIELD* fieldtmp  = (*it_start);
      if(!fieldtmp) break;

      if(fieldtmp->GetLevel() == level)
        {
          return it_start.GetCurrent()->GetParent();
        }

      ++it_start;
    }

  return NULL;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::TraslateTextToHandle(XVECTOR<XSTRING*>* lines, XFILECOBOL_HANDLE& handle)
* @brief      TraslateTextToHandle
* @ingroup    UTILS
*
* @param[in]  lines :
* @param[in]  handle :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::TraslateTextToHandle(XVECTOR<XSTRING*>* lines, XFILECOBOL_HANDLE& handle)
{
  if(!lines) return false;

  Eliminate_TabsForSpaces(lines);
  Eliminate_InvalidCharacters(lines);
  Eliminate_Comments(lines);
  Eliminate_NumberLines(lines);
  Eliminate_EmptyLines(lines);
  Eliminate_UnnecessarySpaces(lines);

  CreateUniqueLines(lines);

  XFILECOBOL_FD_TREENODE* actualfieldnode = NULL;
  int                     actuallevel     = 0;

  for(XDWORD c=0; c<lines->GetSize(); c++)
    {
      XFILECOBOL_LINE_TYPE    linetype          = XFILECOBOL_LINE_TYPE_UNKNOWN;
      int                     level             = 0;
      XFILECOBOL_FIELD_TYPE   fieldtype         = XFILECOBOL_FIELD_TYPE_UNKNOWN;
      XSTRING                 name;
      XSTRING                 param;
      XSTRING                 param2;
      int                     noccurs                = 0;
      int                     PICcomputationaltype   = 0;

      linetype = InterpreterLineForHandle((*lines->Get(c)), level, fieldtype, handle, name, param, param2, noccurs, PICcomputationaltype);

      switch(linetype)
        {
          case XFILECOBOL_LINE_TYPE_FD          : { XFILECOBOL_FIELD* rootfielddata = new XFILECOBOL_FIELD();
                                                    if(!rootfielddata) return false;

                                                    XFILECOBOL_FD_TREENODE* rootfieldnode = new XFILECOBOL_FD_TREENODE(rootfielddata);
                                                    if(!rootfieldnode)
                                                      {
                                                        delete rootfielddata;
                                                        return false;
                                                      }

                                                    rootfielddata->GetName()->Set(__L("ROOT"));
                                                    rootfielddata->SetLevel(0);

                                                    handle.GetFDTree()->SetRoot(rootfieldnode);
                                                    actualfieldnode = rootfieldnode;

                                                    actuallevel     = 1;
                                                  }
                                                  break;

          case XFILECOBOL_LINE_TYPE_SELECT      : { XPATH xpathdata;

                                                    if(handle.GetName()->Compare(name, true))
                                                      {
                                                        return false;
                                                      }

                                                    if(!param.IsEmpty())
                                                      {
                                                        if(!param.Compare(__L("INDEXED")         , true)) handle.SetTypeRecord(XFILECOBOL_RECORD_TYPE_INDEXED);
                                                        if(!param.Compare(__L("LINE SEQUENTIAL") , true)) handle.SetTypeRecord(XFILECOBOL_RECORD_TYPE_LINE_SEQUENTIAL);
                                                        if(!param.Compare(__L("SEQUENTIAL")      , true)) handle.SetTypeRecord(XFILECOBOL_RECORD_TYPE_SEQUENTIAL);
                                                        if(!param.Compare(__L("RELATIVE")        , true)) handle.SetTypeRecord(XFILECOBOL_RECORD_TYPE_RELATIVE);
                                                      }

                                                    if(!param2.IsEmpty())
                                                      {
                                                        if(!param2.Compare(__L("DISK"), true))
                                                          {
                                                            CapabilitiesFile_GetFile(handle.GetName(), xpathdata);
                                                            xpathdata.SetOnlyPathAndNamefileExt();
                                                          }
                                                         else
                                                          {
                                                            xpathdata.AdjustSize(_MAXSTR);

                                                            if(param2.Find(__L("'") , true) != XSTRING_NOTFOUND)  param2.UnFormat(__L("'%s'")  , xpathdata.Get());
                                                            if(param2.Find(__L("\""), true) != XSTRING_NOTFOUND)  param2.UnFormat(__L("\"%s\""), xpathdata.Get());

                                                            xpathdata.AdjustSize();
                                                          }
                                                      }

                                                    if(xpathdatafile.IsEmpty())
                                                      {
                                                        xpathdatafile = xpathdata.Get();
                                                      }

                                                  }

                                                  break;


          case XFILECOBOL_LINE_TYPE_FIELD       : { XFILECOBOL_FIELD*       fielddata = NULL;
                                                    XFILECOBOL_FD_TREENODE* fieldnode = NULL;

                                                    fielddata = new XFILECOBOL_FIELD();
                                                    if(fielddata)
                                                      {
                                                        fieldnode = new XFILECOBOL_FD_TREENODE(fielddata);
                                                        if(!fieldnode) break;

                                                      } else break;

                                                    if(fieldnode)
                                                      {
                                                        fieldnode->GetData()->SetType(fieldtype);
                                                        fieldnode->GetData()->SetLevel(level);
                                                        fieldnode->GetData()->GetName()->Set(name);
                                                        fieldnode->GetData()->GetPIC()->GetMask()->Set(param);
                                                        fieldnode->GetData()->GetPIC()->SetComputationalType(PICcomputationaltype);
                                                        fieldnode->GetData()->SetNOccurs(noccurs);

                                                        if(param2.GetSize())
                                                          {
                                                            fieldnode->GetData()->SetIsRedefine(true);
                                                            fieldnode->GetData()->GetRedefineName()->Set(param2);
                                                          }
                                                         else
                                                          {
                                                            fieldnode->GetData()->SetIsRedefine(false);
                                                          }

                                                        if(fieldnode->GetData()->GetType() == XFILECOBOL_FIELD_TYPE_VAR)
                                                          {
                                                            fieldnode->GetData()->GetPIC()->ResolveMask();
                                                          }

                                                        if(level == actuallevel)
                                                          {
                                                            actualfieldnode->AddChild(fieldnode);
                                                          }
                                                          else
                                                          {
                                                            if((level > actuallevel))
                                                              {
                                                                actualfieldnode->AddChild(fieldnode);
                                                                actuallevel     = level;
                                                              }
                                                             else
                                                              {
                                                                actualfieldnode = GetParentNodeOfLevel(level);
                                                                if(!actualfieldnode)
                                                                  {
                                                                    actualfieldnode = handle.GetFDTree()->GetRoot();
                                                                    break;
                                                                  }

                                                                /*
                                                                for(int d=0; d<(actuallevel-level); d++)
                                                                  {
                                                                    actualfieldnode = actualfieldnode->GetParent();
                                                                    if(!actualfieldnode)
                                                                      {
                                                                        actualfieldnode = handle.GetFDTree()->GetRoot();
                                                                        break;
                                                                      }
                                                                  }
                                                                */

                                                                actualfieldnode->AddChild(fieldnode);
                                                                actuallevel = level;
                                                              }
                                                          }

                                                        if(fieldtype == XFILECOBOL_FIELD_TYPE_KEY)
                                                          {
                                                            actualfieldnode = fieldnode;
                                                          }
                                                      }
                                                  }

                                                  break;

        }
    }

  // -----------------------------------------------------------------------------
  // Recreate the nodes with "Occurs" in Cobol
  // Root is special usually only has one child, but it may not ...

  XFILECOBOL_FD_TREENODE* node = handle.GetFDTree()->GetRoot();
  if(node)
    {
      for(XDWORD c=0; c<(XDWORD)node->GetNChildren(); c++)
        {
          XFILECOBOL_FD_TREENODE* subnode = node->GetChild(c);
          if(subnode)
            {
              if(subnode->GetData())
                {
                  AjustNodeWithOccurs(subnode);
                }
            }
        }
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::ChangeNameNodeWithOccurs(int index, XSTRING* name)
* @brief      ChangeNameNodeWithOccurs
* @ingroup    UTILS
*
* @param[in]  index :
* @param[in]  name :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::ChangeNameNodeWithOccurs(int index, XSTRING* name)
{
  if(!name)             return false;
  if(name->IsEmpty())   return false;

  int indexstart = name->Find(__L("("), true);
  if(indexstart == XSTRING_NOTFOUND)
    {
      name->AddFormat(__L("(%d)"), index);
    }
   else
    {
      int indexend = name->Find(__L(")"), true);
      if(indexend == XSTRING_NOTFOUND) return false;

      XSTRING addindexstr;
      addindexstr.Format(__L(",%d"), index);

      name->Insert(addindexstr, indexend);
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XFILECOBOL_FD_TREENODE* XFILECOBOL::DuplicateNodeWithSubnodes(int index, XFILECOBOL_FD_TREENODE* node, XFILECOBOL_FD_TREENODE* cretednode)
* @brief      DuplicateNodeWithSubnodes
* @ingroup    UTILS
*
* @param[in]  index :
* @param[in]  node :
* @param[in]  cretednode :
*
* @return     XFILECOBOL_FD_TREENODE* :
*
*---------------------------------------------------------------------------------------------------------------------*/
XFILECOBOL_FD_TREENODE* XFILECOBOL::DuplicateNodeWithSubnodes(int index, XFILECOBOL_FD_TREENODE* node, XFILECOBOL_FD_TREENODE* cretednode)
{
  XFILECOBOL_FIELD*       newfielddata = NULL;
  XFILECOBOL_FD_TREENODE* newfieldnode = NULL;
  XSTRING                 namesubnode;

  namesubnode =  node->GetData()->GetName()->Get();
  if(!ChangeNameNodeWithOccurs(index, &namesubnode)) return NULL;

  newfielddata = new XFILECOBOL_FIELD();
  if(newfielddata)
    {
      newfieldnode = new XFILECOBOL_FD_TREENODE(newfielddata);
      if(!newfieldnode) return NULL;

    } else return NULL;

  newfieldnode->GetData()->SetType(node->GetData()->GetType());
  newfieldnode->GetData()->SetLevel(node->GetData()->GetLevel());
  newfieldnode->GetData()->GetName()->Set(namesubnode);
  newfieldnode->GetData()->GetPIC()->GetMask()->Set(node->GetData()->GetPIC()->GetMask()->Get());
  newfieldnode->GetData()->GetPIC()->SetComputationalType(node->GetData()->GetPIC()->GetComputationalType());
  newfieldnode->GetData()->SetNOccurs(node->GetData()->GetNOccurs());

  if(newfieldnode->GetData()->GetType() == XFILECOBOL_FIELD_TYPE_VAR)
    {
      newfieldnode->GetData()->GetPIC()->ResolveMask();
    }

 if(!cretednode)
    {
      int indexnode = handle.GetIndexNodeForName(node->GetParent(), node->GetData()->GetName()->Get());
      if(indexnode != NOTFOUND)
             node->GetParent()->InsertChild(indexnode, newfieldnode);
       else  node->GetParent()->AddChild(newfieldnode);
    }
   else
    {
      cretednode->AddChild(newfieldnode);
    }

  for(XDWORD c=0; c<(XDWORD)node->GetNChildren(); c++)
    {
      XFILECOBOL_FD_TREENODE* subnode = node->GetChild(c);
      if(subnode) DuplicateNodeWithSubnodes(index, subnode, newfieldnode);
    }

  return newfieldnode;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XFILECOBOL::AjustNodeWithOccurs(XFILECOBOL_FD_TREENODE* node)
* @brief      AjustNodeWithOccurs
* @ingroup    UTILS
*
* @param[in]  node :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XFILECOBOL::AjustNodeWithOccurs(XFILECOBOL_FD_TREENODE* node)
{
  if(!node)            return false;
  if(!node->GetData()) return false;

  XDWORD noccurs = node->GetData()->GetNOccurs();
  if(noccurs)
    {
      XFILECOBOL_FD_TREENODE*  newnode  = NULL;
      XFILECOBOL_FD_TREENODE*  parent   = node->GetParent();

      node->GetData()->SetNOccurs(0);

      for(XDWORD c=0; c<(XDWORD)noccurs; c++)
        {
          newnode = DuplicateNodeWithSubnodes(c+1, node);
          if(newnode)
            {
              for(XDWORD c=0; c<(XDWORD)newnode->GetNChildren(); c++)
                {
                  XFILECOBOL_FD_TREENODE* subnode = newnode->GetChild(c);
                  if(subnode) AjustNodeWithOccurs(subnode);
                }
            }
        }

      if(parent) parent->DelChild(node);
    }
   else
    {
      for(XDWORD c=0; c<(XDWORD)node->GetNChildren(); c++)
        {
          XFILECOBOL_FD_TREENODE* subnode = node->GetChild(c);
          if(subnode) AjustNodeWithOccurs(subnode);
        }
    }

  return true;
}





/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XFILECOBOL::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    UTILS
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XFILECOBOL::Clean()
{
  havefileheaders     = false;
  type                = XFILECOBOL_FILE_TYPE_UNKNOWN;
  fileheadersize      = 0;

  xfiledata           = NULL;

  sizedatarecord      = 0;
  maxfilerecordsize   = 0;
  minfilerecordsize   = 0;
}

