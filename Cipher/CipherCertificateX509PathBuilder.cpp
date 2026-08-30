/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherCertificateX509PathBuilder.cpp
* @class      CIPHERCERTIFICATEX509PATHBUILDER
* @brief      X.509 certification path discovery, independent from validation policy
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"
#include "CipherCertificateX509PathBuilder.h"
#include "GEN_Control.h"

CIPHERCERTIFICATEX509PATHBUILDER::CIPHERCERTIFICATEX509PATHBUILDER() {}
CIPHERCERTIFICATEX509PATHBUILDER::~CIPHERCERTIFICATEX509PATHBUILDER() {}

void CIPHERCERTIFICATEX509PATHBUILDER::Path_Delete(XVECTOR<XBUFFER*>& path)
{
  path.DeleteContents();
  path.DeleteAll();
}

bool CIPHERCERTIFICATEX509PATHBUILDER::IsTrusted(CIPHERCERTIFICATEX509* certificate,
                                                  XVECTOR<CIPHERCERTIFICATEX509*>& roots)
{
  if(!certificate) return false;
  for(XDWORD c=0; c<roots.GetSize(); c++)
    {
      CIPHERCERTIFICATEX509* root = roots.Get(c);
      if(!root) continue;
      if(certificate->GetCertificateData()->Compare((*root->GetCertificateData()))) return true;
      if(certificate->GetIssuerData()->Compare((*root->GetSubjectData())) &&
         certificate->VerifySignature(root->GetPublicCipherKey())) return true;
    }
  return false;
}

bool CIPHERCERTIFICATEX509PATHBUILDER::Search(CIPHERCERTIFICATEX509* current,
                                               XVECTOR<CIPHERCERTIFICATEX509*>& candidates,
                                               XVECTOR<CIPHERCERTIFICATEX509*>& roots,
                                               XVECTOR<XDWORD>& selected,
                                               XVECTOR<XDWORD>& result,
                                               XDWORD maximumdepth)
{
  if(!current || !maximumdepth) return false;
  if(IsTrusted(current, roots))
    {
      result.DeleteAll();
      for(XDWORD c=0; c<selected.GetSize(); c++)
        {
          if(!result.Add(selected.Get(c))) return false;
        }
      return true;
    }
  if(selected.GetSize() + 1 >= maximumdepth) return false;

  for(XDWORD c=0; c<candidates.GetSize(); c++)
    {
      CIPHERCERTIFICATEX509* issuer = candidates.Get(c);
      if(!issuer || !issuer->IsCertificateAuthority() ||
         !current->GetIssuerData()->Compare((*issuer->GetSubjectData()))) continue;

      bool used = false;
      for(XDWORD n=0; n<selected.GetSize(); n++) if(selected.Get(n) == c) { used = true; break; }
      if(used || !current->VerifySignature(issuer->GetPublicCipherKey())) continue;

      selected.Add(c);
      if(Search(issuer, candidates, roots, selected, result, maximumdepth)) return true;
      selected.DeleteLast();
    }

  return false;
}

bool CIPHERCERTIFICATEX509PATHBUILDER::Build(XBUFFER& leaf,
                                              XVECTOR<XBUFFER*>* intermediates,
                                              XVECTOR<XBUFFER*>* trustedroots,
                                              XVECTOR<XBUFFER*>& path,
                                              XDWORD maximumdepth)
{
  Path_Delete(path);
  if(leaf.IsEmpty() || !trustedroots || trustedroots->IsEmpty() || !maximumdepth) return false;

  CIPHERCERTIFICATEX509 leafcertificate;
  XVECTOR<CIPHERCERTIFICATEX509*> candidates;
  XVECTOR<CIPHERCERTIFICATEX509*> roots;
  XVECTOR<XDWORD> selected;
  XVECTOR<XDWORD> result;
  bool status = leafcertificate.Decode(leaf);

  if(status && intermediates)
    for(XDWORD c=0; c<intermediates->GetSize(); c++)
      {
        XBUFFER* DER = intermediates->Get(c);
        if(!DER || DER->IsEmpty()) continue;

        CIPHERCERTIFICATEX509* certificate = GEN_NEW CIPHERCERTIFICATEX509();
        if(!certificate) { status = false; break; }

        // Path discovery must not fail because the peer supplied an unrelated or unsupported
        // alternate intermediate.  Only certificates that can actually participate in a path
        // become candidates; the selected path is validated separately afterwards.
        if(!certificate->Decode((*DER))) { GEN_DELETE certificate; continue; }
        if(!candidates.Add(certificate)) { GEN_DELETE certificate; status = false; break; }
      }

  if(status)
    for(XDWORD c=0; c<trustedroots->GetSize(); c++)
      {
        XBUFFER* DER = trustedroots->Get(c);
        if(!DER || DER->IsEmpty()) continue;

        CIPHERCERTIFICATEX509* certificate = GEN_NEW CIPHERCERTIFICATEX509();
        if(!certificate) { status = false; break; }

        // Native trust stores are heterogeneous.  An entry unsupported by GEN is unusable as
        // an anchor, but it must not invalidate every otherwise valid path in the same store.
        if(!certificate->Decode((*DER))) { GEN_DELETE certificate; continue; }
        if(!roots.Add(certificate)) { GEN_DELETE certificate; status = false; break; }
      }

  if(status) status = !roots.IsEmpty() && Search(&leafcertificate, candidates, roots, selected, result, maximumdepth);

  if(status)
    {
      XBUFFER* copy = GEN_NEW XBUFFER();
      if(!copy || !copy->Add(leaf) || !path.Add(copy)) { if(copy) GEN_DELETE copy; status = false; }
    }

  for(XDWORD c=0; status && c<result.GetSize(); c++)
    {
      CIPHERCERTIFICATEX509* certificate = candidates.Get(result.Get(c));
      XBUFFER* copy = certificate?GEN_NEW XBUFFER():NULL;
      if(!copy || !copy->Add((*certificate->GetCertificateData())) || !path.Add(copy))
        { if(copy) GEN_DELETE copy; status = false; }
    }

  candidates.DeleteContents(); candidates.DeleteAll();
  roots.DeleteContents(); roots.DeleteAll();
  if(!status) Path_Delete(path);
  return status;
}
