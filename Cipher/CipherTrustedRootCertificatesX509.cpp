/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       CipherTrustedRootCertificatesX509.cpp
* 
* @class      CIPHERTRUSTEDROOTCERTIFICATES
* @brief      Cipher Trusted Root Certificates X.509 class
* @ingroup    CIPHER
* 
* @copyright  EndoraSoft. All rights reserved.
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

#include "GEN_Defines.h"



/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "CipherTrustedRootCertificatesX509.h"

#include "XFileTXT.h"

#include "CipherCertificateX509.h"
#include "CipherKey.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

#include "cacert.h"
//XCHAR*  trustedrootcertificatesX509[] = { __L("")  };
                       


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         CIPHERTRUSTPROVIDERX509::CIPHERTRUSTPROVIDERX509()
* @brief      Constructor of class
* @ingroup    CIPHER
*
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERTRUSTPROVIDERX509::CIPHERTRUSTPROVIDERX509()
{
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         CIPHERTRUSTPROVIDERX509::~CIPHERTRUSTPROVIDERX509()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    CIPHER
*
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERTRUSTPROVIDERX509::~CIPHERTRUSTPROVIDERX509()
{
  Roots_Delete();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<XBUFFER*>* CIPHERTRUSTPROVIDERX509::GetRoots()
* @brief      Get the trust anchors loaded by the provider
* @ingroup    CIPHER
*
* @return     XVECTOR<XBUFFER*>* : Pointer to the loaded trust anchors.
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XBUFFER*>* CIPHERTRUSTPROVIDERX509::GetRoots()
{
  return &roots;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERTRUSTPROVIDERX509::Root_Add(XBUFFER& root)
* @brief      Add a copied DER trust anchor to the provider
* @ingroup    CIPHER
*
* @param[in]  root : DER certificate to copy.
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERTRUSTPROVIDERX509::Root_Add(XBUFFER& root)
{
  XBUFFER* copy;

  if(root.IsEmpty()) return false;

  copy = GEN_NEW XBUFFER();
  if(!copy) return false;

  if(!copy->Add(root) || !roots.Add(copy))
    {
      GEN_DELETE copy;
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERTRUSTPROVIDERX509::Roots_Delete()
* @brief      Delete all trust anchors loaded by the provider
* @ingroup    CIPHER
*
* @return     bool : true if the operation is successful; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERTRUSTPROVIDERX509::Roots_Delete()
{
  roots.DeleteContents();
  roots.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         CIPHERTRUSTPROVIDERX509GEN::CIPHERTRUSTPROVIDERX509GEN()
* @brief      Constructor of class
* @ingroup    CIPHER
*
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERTRUSTPROVIDERX509GEN::CIPHERTRUSTPROVIDERX509GEN()
{
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         CIPHERTRUSTPROVIDERX509GEN::~CIPHERTRUSTPROVIDERX509GEN()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    CIPHER
*
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERTRUSTPROVIDERX509GEN::~CIPHERTRUSTPROVIDERX509GEN()
{
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool CIPHERTRUSTPROVIDERX509GEN::Load()
* @brief      Load the supported trust anchors from the embedded GEN CA bundle
* @ingroup    CIPHER
*
* @return     bool : true if at least one trust anchor is loaded; otherwise false.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERTRUSTPROVIDERX509GEN::Load()
{
  CIPHERTRUSTEDROOTCERTIFICATESX509 defaultroots;

  Roots_Delete();

  if(!defaultroots.Certificates_Decode()) return false;

  for(XDWORD c=0; c<defaultroots.Certificates_GetAll()->GetSize(); c++)
    {
      XBUFFER* rootDER = defaultroots.Certificates_GetAll()->Get(c);

      if(rootDER)
        {
          CIPHERCERTIFICATEX509 root;

          if(root.Decode((*rootDER)) && root.IsCertificateAuthority() && root.GetPublicCipherKey() &&
             ((root.GetPublicCipherKey()->GetType() == CIPHERKEYTYPE_RSA_PUBLIC) ||
              (root.GetPublicCipherKey()->GetType() == CIPHERKEYTYPE_ECDSA_SECP256R1_PUBLIC) ||
              (root.GetPublicCipherKey()->GetType() == CIPHERKEYTYPE_ECDSA_SECP384R1_PUBLIC) ||
              (root.GetPublicCipherKey()->GetType() == CIPHERKEYTYPE_ECDSA_SECP521R1_PUBLIC)))
            {
              if(!Root_Add((*rootDER)))
                {
                  Roots_Delete();
                  return false;
                }
            }
        }
    }

  return !GetRoots()->IsEmpty();
}



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
      XSTRING* line = GEN_NEW XSTRING();
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
* @note       VIRTUAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
CIPHERTRUSTEDROOTCERTIFICATESX509::~CIPHERTRUSTEDROOTCERTIFICATESX509()
{
  Certificates_DeleteAll();
  DeleteAllLines();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERTRUSTEDROOTCERTIFICATESX509::ReadFromFile(XPATH* pathnamefile)
* @brief      read from file
* @ingroup    CIPHER
* 
* @param[in]  pathnamefile : Pathnamefile pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
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

  Certificates_DeleteAll();
    
  xfiletxt = GEN_NEW XFILETXT();
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
              XSTRING* line = GEN_NEW XSTRING();
              if(line)
                {
                  line->Set(xfiletxt->GetLine(c)->Get()); 
                  lines.Add(line);                  
                }
            }                                      
        }

      xfiletxt->Close();   
    }

  GEN_DELETE xfiletxt;

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERTRUSTEDROOTCERTIFICATESX509::GenerateEmbeddedHeadere(XPATH* originpathnamefile, XPATH* targetpathnamefile)
* @brief      generate embedded headere
* @ingroup    CIPHER
* 
* @param[in]  originpathnamefile : Originpathnamefile pointer to use.
* @param[in]  targetpathnamefile : Targetpathnamefile pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
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
    
  originxfiletxt = GEN_NEW XFILETXT();
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
          targetxfiletxt = GEN_NEW XFILETXT();
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

  GEN_DELETE originxfiletxt;

  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XSTRING*>* CIPHERTRUSTEDROOTCERTIFICATESX509::GetLines()
* @brief      get lines
* @ingroup    CIPHER
* 
* @return     XVECTOR<XSTRING*>* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XSTRING*>* CIPHERTRUSTEDROOTCERTIFICATESX509::GetLines()
{
  return &lines;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERTRUSTEDROOTCERTIFICATESX509::DeleteAllLines()
* @brief      GEN_DELETE all lines
* @ingroup    CIPHER
* 
* @return     bool : true if the operation is successful; otherwise false.
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
* @fn         bool CIPHERTRUSTEDROOTCERTIFICATESX509::Certificates_Decode()
* @brief      Decode every PEM certificate into an owned DER buffer
* @ingroup    CIPHER
* 
* @return     bool : true if at least one complete certificate is decoded; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERTRUSTEDROOTCERTIFICATESX509::Certificates_Decode()
{
  XSTRING certificatebase64;
  bool    iscertificate = false;

  Certificates_DeleteAll();

  for(XDWORD c=0; c<lines.GetSize(); c++)
    {
      XSTRING* line = lines.Get(c);
      if(!line) continue;

      if(line->Find(__L("-----BEGIN CERTIFICATE-----"), true) != XSTRING_NOTFOUND)
        {
          if(iscertificate)
            {
              Certificates_DeleteAll();
              return false;
            }

          certificatebase64.Empty();
          iscertificate = true;
          continue;
        }

      if(line->Find(__L("-----END CERTIFICATE-----"), true) != XSTRING_NOTFOUND)
        {
          XBUFFER* certificate;

          if(!iscertificate || certificatebase64.IsEmpty())
            {
              Certificates_DeleteAll();
              return false;
            }

          certificate = GEN_NEW XBUFFER();
          if(!certificate)
            {
              Certificates_DeleteAll();
              return false;
            }

          if(!certificate->ConvertFromBase64(certificatebase64) || certificate->IsEmpty() || !certificates.Add(certificate))
            {
              GEN_DELETE certificate;
              Certificates_DeleteAll();
              return false;
            }

          certificatebase64.Empty();
          iscertificate = false;
          continue;
        }

      if(iscertificate && !line->IsEmpty()) certificatebase64.Add(line->Get());
    }

  if(iscertificate || certificates.IsEmpty())
    {
      Certificates_DeleteAll();
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVECTOR<XBUFFER*>* CIPHERTRUSTEDROOTCERTIFICATESX509::Certificates_GetAll()
* @brief      Get every decoded DER trust anchor
* @ingroup    CIPHER
* 
* @return     XVECTOR<XBUFFER*>* : Pointer to the owned certificate list.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<XBUFFER*>* CIPHERTRUSTEDROOTCERTIFICATESX509::Certificates_GetAll()
{
  return &certificates;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool CIPHERTRUSTEDROOTCERTIFICATESX509::Certificates_DeleteAll()
* @brief      Delete every decoded DER trust anchor
* @ingroup    CIPHER
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool CIPHERTRUSTEDROOTCERTIFICATESX509::Certificates_DeleteAll()
{
  certificates.DeleteContents();
  certificates.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void CIPHERTRUSTEDROOTCERTIFICATESX509::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    CIPHER
* 
* --------------------------------------------------------------------------------------------------------------------*/
void CIPHERTRUSTEDROOTCERTIFICATESX509::Clean()
{

}





