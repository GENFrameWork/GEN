/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherCertificateX509PathBuilder.cpp
* @class      CIPHERCERTIFICATEX509PATHBUILDER
* @brief      X.509 certification path discovery, independent from validation policy
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"
#include "CipherCertificateX509PathBuilder.h"
#include "GEN_Control.h"


static bool CIPHERCERTIFICATEX509PATHBUILDER_IssuerIdentifierMatches(CIPHERCERTIFICATEX509* certificate,
                                                                       CIPHERCERTIFICATEX509* issuer)
{
  if(!certificate || !issuer) return false;

  if(certificate->HasAuthorityKeyIdentifier() && issuer->HasSubjectKeyIdentifier())
    {
      XBUFFER* authority = certificate->GetAuthorityKeyIdentifier();
      XBUFFER* subject   = issuer->GetSubjectKeyIdentifier();
      if(!authority || !subject || !authority->Compare((*subject))) return false;
    }

  return true;
}

CIPHERCERTIFICATEX509PATHBUILDER::CIPHERCERTIFICATEX509PATHBUILDER() {}
CIPHERCERTIFICATEX509PATHBUILDER::~CIPHERCERTIFICATEX509PATHBUILDER() {}

void CIPHERCERTIFICATEX509PATHBUILDER::Path_Delete(XVECTOR<XBUFFER*>& path)
{
  path.DeleteContents();
  path.DeleteAll();
}

void CIPHERCERTIFICATEX509PATHBUILDER::Paths_Delete(XVECTOR<XVECTOR<XBUFFER*>*>& paths)
{
  for(XDWORD c=0; c<paths.GetSize(); c++)
    {
      XVECTOR<XBUFFER*>* path = paths.Get(c);
      if(!path) continue;
      Path_Delete((*path));
      GEN_DELETE path;
    }

  paths.DeleteAll();
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
         CIPHERCERTIFICATEX509PATHBUILDER_IssuerIdentifierMatches(certificate, root) &&
         certificate->VerifySignature(root->GetPublicCipherKey())) return true;
    }
  return false;
}

bool CIPHERCERTIFICATEX509PATHBUILDER::SearchAll(CIPHERCERTIFICATEX509* current,
                                                  XVECTOR<CIPHERCERTIFICATEX509*>& candidates,
                                                  XVECTOR<CIPHERCERTIFICATEX509*>& roots,
                                                  XVECTOR<XDWORD>& selected,
                                                  XVECTOR<XVECTOR<XDWORD>*>& results,
                                                  XDWORD maximumdepth,
                                                  XDWORD maximumpaths,
                                                  XDWORD& searchednodes)
{
  if(!current || !maximumdepth || !maximumpaths) return false;
  if(results.GetSize() >= maximumpaths) return true;
  if(searchednodes >= CIPHERCERTIFICATEX509PATHBUILDER_MAX_SEARCH_NODES) return true;
  searchednodes++;

  if(IsTrusted(current, roots))
    {
      XVECTOR<XDWORD>* result = GEN_NEW XVECTOR<XDWORD>();
      if(!result) return false;

      for(XDWORD c=0; c<selected.GetSize(); c++)
        {
          if(!result->Add(selected.Get(c)))
            {
              GEN_DELETE result;
              return false;
            }
        }

      if(!results.Add(result))
        {
          GEN_DELETE result;
          return false;
        }

      if(results.GetSize() >= maximumpaths) return true;
    }

  // Reaching one trust anchor is a valid result, not the end of the search.  A cross-signed
  // version of that CA may lead to another anchor whose constraints and policy are acceptable.
  if(selected.GetSize() + 1 >= maximumdepth) return true;

  for(XDWORD c=0; c<candidates.GetSize(); c++)
    {
      CIPHERCERTIFICATEX509* issuer = candidates.Get(c);
      if(!issuer || !issuer->IsCertificateAuthority() ||
         !current->GetIssuerData()->Compare((*issuer->GetSubjectData())) ||
         !CIPHERCERTIFICATEX509PATHBUILDER_IssuerIdentifierMatches(current, issuer)) continue;

      bool used = false;
      if(current->GetCertificateData()->Compare((*issuer->GetCertificateData()))) used = true;

      for(XDWORD n=0; !used && n<selected.GetSize(); n++)
        {
          CIPHERCERTIFICATEX509* selectedcertificate = candidates.Get(selected.Get(n));
          if((selected.Get(n) == c) ||
             (selectedcertificate && selectedcertificate->GetCertificateData()->Compare((*issuer->GetCertificateData()))))
            {
              used = true;
            }
        }

      if(used || !current->VerifySignature(issuer->GetPublicCipherKey())) continue;

      if(!selected.Add(c)) return false;
      bool status = SearchAll(issuer, candidates, roots, selected, results, maximumdepth, maximumpaths, searchednodes);
      selected.DeleteLast();

      if(!status) return false;
      if(results.GetSize() >= maximumpaths) return true;
    }

  return true;
}

bool CIPHERCERTIFICATEX509PATHBUILDER::Build(XBUFFER& leaf,
                                              XVECTOR<XBUFFER*>* intermediates,
                                              XVECTOR<XBUFFER*>* trustedroots,
                                              XVECTOR<XBUFFER*>& path,
                                              XDWORD maximumdepth)
{
  Path_Delete(path);
  XVECTOR<XVECTOR<XBUFFER*>*> paths;

  if(!BuildAll(leaf, intermediates, trustedroots, paths, maximumdepth, 1)) return false;

  XVECTOR<XBUFFER*>* firstpath = paths.Get(0);
  bool status = firstpath != NULL;

  for(XDWORD c=0; status && c<firstpath->GetSize(); c++)
    {
      XBUFFER* source = firstpath->Get(c);
      XBUFFER* copy   = source?GEN_NEW XBUFFER():NULL;

      if(!copy || !copy->Add((*source)) || !path.Add(copy))
        {
          if(copy) GEN_DELETE copy;
          status = false;
        }
    }

  Paths_Delete(paths);
  if(!status) Path_Delete(path);
  return status;
}


bool CIPHERCERTIFICATEX509PATHBUILDER::BuildAll(XBUFFER& leaf,
                                                 XVECTOR<XBUFFER*>* intermediates,
                                                 XVECTOR<XBUFFER*>* trustedroots,
                                                 XVECTOR<XVECTOR<XBUFFER*>*>& paths,
                                                 XDWORD maximumdepth,
                                                 XDWORD maximumpaths)
{
  Paths_Delete(paths);
  if(leaf.IsEmpty() || !trustedroots || trustedroots->IsEmpty() || !maximumdepth || !maximumpaths) return false;

  CIPHERCERTIFICATEX509 leafcertificate;
  XVECTOR<CIPHERCERTIFICATEX509*> candidates;
  XVECTOR<CIPHERCERTIFICATEX509*> roots;
  XVECTOR<XDWORD> selected;
  XVECTOR<XVECTOR<XDWORD>*> results;
  XDWORD searchednodes = 0;
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

        bool duplicate = false;
        for(XDWORD d=0; d<candidates.GetSize(); d++)
          {
            CIPHERCERTIFICATEX509* existing = candidates.Get(d);
            if(existing && certificate->GetCertificateData()->Compare((*existing->GetCertificateData())))
              {
                duplicate = true;
                break;
              }
          }

        if(duplicate) { GEN_DELETE certificate; continue; }
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

        bool duplicate = false;
        for(XDWORD d=0; d<roots.GetSize(); d++)
          {
            CIPHERCERTIFICATEX509* existing = roots.Get(d);
            if(existing && certificate->GetCertificateData()->Compare((*existing->GetCertificateData())))
              {
                duplicate = true;
                break;
              }
          }

        if(duplicate) { GEN_DELETE certificate; continue; }
        if(!roots.Add(certificate)) { GEN_DELETE certificate; status = false; break; }
      }

  if(status) status = !roots.IsEmpty() && SearchAll(&leafcertificate, candidates, roots, selected, results,
                                                    maximumdepth, maximumpaths, searchednodes) && !results.IsEmpty();

  for(XDWORD c=0; status && c<results.GetSize(); c++)
    {
      XVECTOR<XDWORD>* result = results.Get(c);
      XVECTOR<XBUFFER*>* path = result?GEN_NEW XVECTOR<XBUFFER*>():NULL;

      if(!path) { status = false; break; }

      XBUFFER* leafcopy = GEN_NEW XBUFFER();
      if(!leafcopy || !leafcopy->Add(leaf) || !path->Add(leafcopy))
        {
          if(leafcopy) GEN_DELETE leafcopy;
          Path_Delete((*path));
          GEN_DELETE path;
          status = false;
          break;
        }

      for(XDWORD d=0; status && d<result->GetSize(); d++)
        {
          CIPHERCERTIFICATEX509* certificate = candidates.Get(result->Get(d));
          XBUFFER* copy = certificate?GEN_NEW XBUFFER():NULL;
          if(!copy || !copy->Add((*certificate->GetCertificateData())) || !path->Add(copy))
            {
              if(copy) GEN_DELETE copy;
              status = false;
            }
        }

      if(status && !paths.Add(path)) status = false;
      if(!status)
        {
          Path_Delete((*path));
          GEN_DELETE path;
        }
    }

  results.DeleteContents(); results.DeleteAll();
  candidates.DeleteContents(); candidates.DeleteAll();
  roots.DeleteContents(); roots.DeleteAll();
  if(!status) Paths_Delete(paths);
  return status && !paths.IsEmpty();
}
