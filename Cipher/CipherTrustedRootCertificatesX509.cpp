/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       CipherTrustedRootCertificatesX509.cpp
* 
* @class      CIPHERTRUSTEDROOTCERTIFICATES
* @brief      Cipher Trusted Root Certificates X.509 class
* @ingroup    CIPHER
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
#pragma region PRECOMPILATION_DEFINE_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "CipherTrustedRootCertificatesX509.h"

#include "XFileTXT.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#include "cacert.h"
//XCHAR*  trustedrootcertificatesX509[] = { __L("")  };
                       
#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERTRUSTEDROOTCERTIFICATESX509::CIPHERTRUSTEDROOTCERTIFICATESX509()
* @brief      Constructor of class
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERTRUSTEDROOTCERTIFICATESX509::CIPHERTRUSTEDROOTCERTIFICATESX509()
{
  Clean();

  for(XDWORD c=0; c<(sizeof(trustedrootcertificates)/sizeof(XCHAR*)); c++)
    {
      XSTRING* line = new XSTRING();
      if(line)
        {
          line->Set(trustedrootcertificates[c]);
          lines.Add(line);  
        }
    }  
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         CIPHERTRUSTEDROOTCERTIFICATESX509::~CIPHERTRUSTEDROOTCERTIFICATESX509()
* @brief      Destructor of class
* @ingroup    CIPHER
* @note       VIRTUAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERTRUSTEDROOTCERTIFICATESX509::~CIPHERTRUSTEDROOTCERTIFICATESX509()
{
  DeleteAllLines();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERTRUSTEDROOTCERTIFICATESX509::ReadFromFile(XPATH* namefile)
* @brief      read from file
* @ingroup    CIPHER
* 
* @param[in]  namefile : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERTRUSTEDROOTCERTIFICATESX509::ReadFromFile(XPATH* pathnamefile)
{
  XFILETXT* xfiletxt;
  bool      status = false;  

  if(!pathnamefile)
    {
      return false;
    }
    
  xfiletxt = new XFILETXT();
  if(!xfiletxt) 
    {
      return false;
    }

  status = xfiletxt->Open((*pathnamefile), true);
  if(status)   
    { 
      status = xfiletxt->ReadAllFile();
      if(status)
        {      
          DeleteAllLines(); 
    
          for(XDWORD c=0; c<xfiletxt->GetNLines(); c++)
            {
              XSTRING* line = new XSTRING();
              if(line)
                {
                  line->Set(xfiletxt->GetLine(c)->Get()); 
                  lines.Add(line);                  
                }
            }                                      
        }

      xfiletxt->Close();   
    }

  delete xfiletxt;

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERTRUSTEDROOTCERTIFICATESX509::GenerateEmbeddedHeadere(XPATH* originpathnamefile, XPATH* targetpathnamefile)
* @brief      generate embedded headere
* @ingroup    CIPHER
* 
* @param[in]  originpathnamefile : 
* @param[in]  targetpathnamefile : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERTRUSTEDROOTCERTIFICATESX509::GenerateEmbeddedHeadere(XPATH* originpathnamefile, XPATH* targetpathnamefile)
{
  XFILETXT* originxfiletxt;
  XFILETXT* targetxfiletxt;
  bool      status = false;  

  if(!originpathnamefile)
    {
      return false;
    }

  if(!targetpathnamefile)
    {
      return false;
    }
    
  originxfiletxt = new XFILETXT();
  if(!originxfiletxt) 
    {
      return false;
    }

  status = originxfiletxt->Open((*originpathnamefile), true);
  if(status)   
    { 
      status = originxfiletxt->ReadAllFile();
      if(status)
        {              
          targetxfiletxt = new XFILETXT();
          if(targetxfiletxt) 
            {
              status = targetxfiletxt->Create((*targetpathnamefile));
              if(status)   
                { 
                  XSTRING line;     

                  for(XDWORD c=0; c<originxfiletxt->GetNLines(); c++)
                    {
                      line.Empty();
  
                      if(!c)
                        {
                          line.AddFormat(__L("XCHAR*  trustedrootcertificates[] =              { __L(\"%s\")  ,"), originxfiletxt->GetLine(c)->Get()); 
                        }
                       else
                        {
                          line.AddFormat(__L("                                                   __L(\"%s\")  ,"), originxfiletxt->GetLine(c)->Get());  
                        }

                      targetxfiletxt->AddLine(line);                                                                            
                    } 

                  line.Empty();
                  line.AddFormat(__L("                                                  };"));  
                  targetxfiletxt->AddLine(line);                                                                            

                  status = targetxfiletxt->WriteAllFile();

                  targetxfiletxt->Close(); 
                }
            }                                     
        }

      originxfiletxt->Close();   
    }

  delete originxfiletxt;

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XSTRING*>* CIPHERTRUSTEDROOTCERTIFICATESX509::GetLines()
* @brief      get lines
* @ingroup    CIPHER
* 
* @return     XVECTOR<XSTRING*>* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XSTRING*>* CIPHERTRUSTEDROOTCERTIFICATESX509::GetLines()
{
  return &lines;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERTRUSTEDROOTCERTIFICATESX509::DeleteAllLines()
* @brief      delete all lines
* @ingroup    CIPHER
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERTRUSTEDROOTCERTIFICATESX509::DeleteAllLines()
{
  if(lines.IsEmpty())
    {
      return false;
    }

  lines.DeleteContents();
  lines.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERTRUSTEDROOTCERTIFICATESX509::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @ingroup    CIPHER
* @note       INTERNAL
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERTRUSTEDROOTCERTIFICATESX509::Clean()
{

}


#pragma endregion






