/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPVectorFileDXFConfig.cpp
* 
* @class      GRPVECTORFILEDXFCONFIG
* @brief      Graphic Vector File DXF Config class
* @ingroup    GRAPHIC
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

#include "GRPVectorFileDXFConfig.h"

#include "XPath.h"
#include "XFileINI.h"
#include "XFileCFG.h"

#include "GRPVectorFileDXF.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFCONFIG::GRPVECTORFILEDXFCONFIG()
* @brief      Constructor
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFCONFIG::GRPVECTORFILEDXFCONFIG()
{
  Clean();

  fileCFG = new XFILECFG();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFCONFIG::~GRPVECTORFILEDXFCONFIG()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFCONFIG::~GRPVECTORFILEDXFCONFIG()
{
  if(fileCFG)
    {
      delete fileCFG;
    }

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFTEXTSECTIONHEADER* GRPVECTORFILEDXFCONFIG::GetHeader()
* @brief      GetHeader
* @ingroup    GRAPHIC
* 
* @return     GRPVECTORFILEDXFTEXTSECTIONHEADER* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFTEXTSECTIONHEADER* GRPVECTORFILEDXFCONFIG::GetHeader()
{
  return header;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPVECTORFILEDXFCONFIG::SetHeader(GRPVECTORFILEDXFTEXTSECTIONHEADER* header)
* @brief      SetHeader
* @ingroup    GRAPHIC
* 
* @param[in]  header : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILEDXFCONFIG::SetHeader(GRPVECTORFILEDXFTEXTSECTIONHEADER* header)
{
  this->header = header;
  
  fileCFG->DeleteAllValues();
  
  for(XDWORD c=0; c<header->GetVariables()->GetSize(); c++)
    {   
      XSTRING* namekey = header->GetVariables()->GetKey(c);
      XVARIANT* variant = header->GetVariables()->GetElement(c);
     
      if(namekey && variant)
        {
          switch(variant->GetType())
            {         
              case XVARIANT_TYPE_BOOLEAN        : fileCFG->AddValue(XFILECFG_VALUETYPE_BOOLEAN, VECTORFILEDXFCONFIG_SECTION_DFX_CFG, namekey->Get(), (bool*)(variant->GetData()));      break;  
              case XVARIANT_TYPE_INTEGER        : fileCFG->AddValue(XFILECFG_VALUETYPE_INT, VECTORFILEDXFCONFIG_SECTION_DFX_CFG, namekey->Get(), (int*)(variant->GetData()));           break;  
              case XVARIANT_TYPE_FLOAT          : fileCFG->AddValue(XFILECFG_VALUETYPE_FLOAT, VECTORFILEDXFCONFIG_SECTION_DFX_CFG, namekey->Get(), (float*)(variant->GetData()));       break;  
              case XVARIANT_TYPE_STRING         : fileCFG->AddValue(XFILECFG_VALUETYPE_STRING, VECTORFILEDXFCONFIG_SECTION_DFX_CFG, namekey->Get(), (XSTRING*)(variant->GetData()));    break;
  
              case XVARIANT_TYPE_NULL           :
              case XVARIANT_TYPE_DOUBLEINTEGER  :            
              case XVARIANT_TYPE_DOUBLE         :
              case XVARIANT_TYPE_CHAR           :
              case XVARIANT_TYPE_XCHAR          :            
              case XVARIANT_TYPE_DATE           :
              case XVARIANT_TYPE_TIME           :
              case XVARIANT_TYPE_DATETIME       : 
                                   default      : return false;
                                                  break;
            }         
  
          XCHAR* remark = NULL;
          remark = header->GetVariableRemark((*namekey));
  
          if(remark) 
            {
              fileCFG->AddRemark(VECTORFILEDXFCONFIG_SECTION_DFX_CFG, namekey->Get(), remark, 64, 0);
            }
          
        }   
    }
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPVECTORFILEDXFCONFIG::Create_File(XCHAR* pathFile)
* @brief      Create_File
* @ingroup    GRAPHIC
* 
* @param[in]  pathFile : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILEDXFCONFIG::Create_File(XCHAR* pathFile)
{
  if(!fileCFG) return false;

  XPATH path;
   
  path = pathFile;

  return fileCFG->Save(path);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPVECTORFILEDXFCONFIG::Load_File(XCHAR* pathFile)
* @brief      Load_File
* @ingroup    GRAPHIC
* 
* @param[in]  pathFile : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILEDXFCONFIG::Load_File(XCHAR* pathFile)
{
  if(!fileCFG) 
    {
      return false;
    }

  if(!header)  
    {
      return false;
    }
  
  XPATH path;
  bool  status = false;
  
  path = pathFile;
  
  header->DeleteAllVariables();
  
  fileCFG->DeleteAllValues();
  
  if(fileCFG->IniFile(path)) 
    {
      fileCFG->AjustRemarks();
  
      XFILEINISECTION* sectionINI = fileCFG->GetFileINI()->GetSection(VECTORFILEDXFCONFIG_SECTION_DFX_CFG);
      if(sectionINI)
        {
          XSTRING key;         
          XSTRING value;
  
          while(sectionINI->GetNextKey(key, value))
            {
              XVARIANT variant;
              XSTRING  trash;                              
              int      type = 0;
  
              trash.AdjustSize(_MAXSTR);
  
              key.UnFormat(__L("$%s_%d"), trash.Get(), &type, NULL);
  
              GRPVECTORFILEDXFTEXTSECTION::GetVariableFromLine(trash.Get(), type, &value, variant);
                          
              if(variant.GetType() != XVARIANT_TYPE_NULL)   
                {
                  header->AddVariable(key.Get(), &variant);
                }
            }
        }
      
      //status = fileCFG->EndFile();
  
      fileCFG->GetFileINI()->XFILETXT::Close();
      fileCFG->DeleteAllRemarks();
    }
  
  #ifdef XTRACE_ACTIVE
  header->ShowTraceAllVariables();
  #endif
  
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPVECTORFILEDXFCONFIG::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILEDXFCONFIG::Clean()
{
  header    = NULL;

  pathFile.Empty();

  fileCFG   = NULL;
}

#pragma endregion

