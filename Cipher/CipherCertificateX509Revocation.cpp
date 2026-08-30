/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherCertificateX509Revocation.cpp
* @brief      Signed OCSP/CRL validation primitives for X.509 consumers
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/
#include "GEN_Defines.h"
#include "CipherCertificateX509Revocation.h"
#include <string.h>
#include "XFactory.h"
#include "GEN_Control.h"

struct CIPHERREV_DER { XBYTE tag; const XBYTE* encoded; XDWORD encodedsize; const XBYTE* data; XDWORD size; };
class CIPHERREV_READER
{
  public: CIPHERREV_READER(const XBYTE* p,XDWORD n):data(p),size(n),pos(0){}
  bool Read(CIPHERREV_DER& item)
  {
    if(!data||pos>=size) return false; XDWORD start=pos; item.tag=data[pos++];
    if((item.tag&0x1F)==0x1F || pos>=size) return false; XDWORD length=data[pos++];
    if(length&0x80) { XDWORD n=length&0x7F; length=0; if(!n||n>4||n>size-pos)return false; for(XDWORD c=0;c<n;c++)length=(length<<8)|data[pos++]; }
    if(length>size-pos)return false; item.encoded=data+start; item.data=data+pos; item.size=length; pos+=length; item.encodedsize=pos-start; return true;
  }
  bool End(){return pos==size;}
  private: const XBYTE* data; XDWORD size,pos;
};

static bool CIPHERREV_OID(CIPHERREV_DER& item,const XBYTE* oid,XDWORD size){return item.tag==0x06&&item.size==size&&!memcmp(item.data,oid,size);}
static CIPHERCERTIFICATEX509_ALGORITHM_TYPE CIPHERREV_Algorithm(CIPHERREV_DER& algorithm)
{
  static const XBYTE r256[]={0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x0B};
  static const XBYTE r384[]={0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x0C};
  static const XBYTE r512[]={0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x0D};
  static const XBYTE e256[]={0x2A,0x86,0x48,0xCE,0x3D,0x04,0x03,0x02};
  static const XBYTE e384[]={0x2A,0x86,0x48,0xCE,0x3D,0x04,0x03,0x03};
  static const XBYTE e512[]={0x2A,0x86,0x48,0xCE,0x3D,0x04,0x03,0x04};
  if(algorithm.tag!=0x30)return CIPHERCERTIFICATEX509_ALGORITHM_TYPE_UNKNOWN;
  CIPHERREV_READER r(algorithm.data,algorithm.size); CIPHERREV_DER oid;
  if(!r.Read(oid))return CIPHERCERTIFICATEX509_ALGORITHM_TYPE_UNKNOWN;
  if(CIPHERREV_OID(oid,r256,sizeof(r256)))return CIPHERCERTIFICATEX509_ALGORITHM_TYPE_SHA256WITHRSAENCRYPTION;
  if(CIPHERREV_OID(oid,r384,sizeof(r384)))return CIPHERCERTIFICATEX509_ALGORITHM_TYPE_SHA384WITHRSAENCRYPTION;
  if(CIPHERREV_OID(oid,r512,sizeof(r512)))return CIPHERCERTIFICATEX509_ALGORITHM_TYPE_SHA512WITHRSAENCRYPTION;
  if(CIPHERREV_OID(oid,e256,sizeof(e256)))return CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA256;
  if(CIPHERREV_OID(oid,e384,sizeof(e384)))return CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA384;
  if(CIPHERREV_OID(oid,e512,sizeof(e512)))return CIPHERCERTIFICATEX509_ALGORITHM_TYPE_ECDSAWITHSHA512;
  return CIPHERCERTIFICATEX509_ALGORITHM_TYPE_UNKNOWN;
}
static bool CIPHERREV_SerialEqual(CIPHERREV_DER& serial,XBUFFER* expected)
{
  if(serial.tag!=0x02||!expected)return false; const XBYTE* a=serial.data; XDWORD as=serial.size; XBYTE* b=expected->Get(); XDWORD bs=expected->GetSize();
  while(as>1&&!a[0]){a++;as--;} while(bs>1&&!b[0]){b++;bs--;} return as==bs&&!memcmp(a,b,as);
}
static bool CIPHERREV_Verify(CIPHERREV_DER& tbs,CIPHERREV_DER& algorithm,CIPHERREV_DER& bits,CIPHERCERTIFICATEX509& issuer)
{
  if(bits.tag!=0x03||bits.size<2||bits.data[0])return false;
  CIPHERCERTIFICATEX509_ALGORITHM_TYPE type=CIPHERREV_Algorithm(algorithm); if(type==CIPHERCERTIFICATEX509_ALGORITHM_TYPE_UNKNOWN)return false;
  XBUFFER data,signature; data.Add((XBYTE*)tbs.encoded,tbs.encodedsize); signature.Add((XBYTE*)bits.data+1,bits.size-1);
  return CIPHERCERTIFICATEX509::VerifyDataSignature(issuer.GetPublicCipherKey(),type,data,signature);
}

static bool CIPHERREV_Time(CIPHERREV_DER& item,XDATETIME& datetime)
{
  if((item.tag!=0x17&&item.tag!=0x18)||item.size!=(item.tag==0x17?13:15)||item.data[item.size-1]!='Z')return false;
  for(XDWORD c=0;c<item.size-1;c++)if(item.data[c]<'0'||item.data[c]>'9')return false;int index=0,year=0;
  if(item.tag==0x17){year=(item.data[0]-'0')*10+item.data[1]-'0';year+=(year>=50)?1900:2000;index=2;}
  else{year=(item.data[0]-'0')*1000+(item.data[1]-'0')*100+(item.data[2]-'0')*10+item.data[3]-'0';index=4;}
  datetime.SetYear(year);datetime.SetMonth((item.data[index]-'0')*10+item.data[index+1]-'0');datetime.SetDay((item.data[index+2]-'0')*10+item.data[index+3]-'0');
  datetime.SetHours((item.data[index+4]-'0')*10+item.data[index+5]-'0');datetime.SetMinutes((item.data[index+6]-'0')*10+item.data[index+7]-'0');datetime.SetSeconds((item.data[index+8]-'0')*10+item.data[index+9]-'0');datetime.SetMilliSeconds(0);datetime.SetIsLocal(false);return datetime.IsValidDate();
}
static bool CIPHERREV_TimeWindow(CIPHERREV_DER& from,CIPHERREV_DER& until)
{
  XDATETIME first,last;XDATETIME* now=GEN_XFACTORY.CreateDateTime();if(!now)return false;bool status=CIPHERREV_Time(from,first)&&CIPHERREV_Time(until,last)&&now->Read(false);
  if(status){XQWORD value=now->GetSeconsFromDate();status=value>=first.GetSeconsFromDate()&&value<=last.GetSeconsFromDate();}GEN_XFACTORY.DeleteDateTime(now);return status;
}

CIPHERCERTIFICATEX509REVOCATION_RESULT CIPHERCERTIFICATEX509REVOCATION::ValidateCRL(XBUFFER& CRL,CIPHERCERTIFICATEX509& certificate,CIPHERCERTIFICATEX509& issuer)
{
  CIPHERREV_READER outer(CRL.Get(),CRL.GetSize()); CIPHERREV_DER sequence,tbs,algorithm,bits;
  if(!outer.Read(sequence)||!outer.End()||sequence.tag!=0x30)return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;
  CIPHERREV_READER top(sequence.data,sequence.size); if(!top.Read(tbs)||!top.Read(algorithm)||!top.Read(bits)||!top.End()||tbs.tag!=0x30||!CIPHERREV_Verify(tbs,algorithm,bits,issuer))return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;
  CIPHERREV_READER r(tbs.data,tbs.size); CIPHERREV_DER item,inneralg,issuername;
  if(!r.Read(item))return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID; if(item.tag==0x02){if(!r.Read(item))return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;}
  inneralg=item; if(!r.Read(issuername)||issuername.tag!=0x30||issuername.encodedsize!=issuer.GetSubjectData()->GetSize()||memcmp(issuername.encoded,issuer.GetSubjectData()->Get(),issuername.encodedsize))return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;
  CIPHERREV_DER thisupdate,nextupdate;
  if(!r.Read(thisupdate)||((thisupdate.tag!=0x17)&&(thisupdate.tag!=0x18))||!r.Read(nextupdate)||
     ((nextupdate.tag!=0x17)&&(nextupdate.tag!=0x18))||!CIPHERREV_TimeWindow(thisupdate,nextupdate))return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;
  if(r.End())return CIPHERCERTIFICATEX509REVOCATION_RESULT_GOOD;if(!r.Read(item))return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;
  if(item.tag!=0x30)return CIPHERCERTIFICATEX509REVOCATION_RESULT_GOOD;
  CIPHERREV_READER revoked(item.data,item.size);
  while(!revoked.End()) { CIPHERREV_DER entry,serial; if(!revoked.Read(entry)||entry.tag!=0x30)return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID; CIPHERREV_READER er(entry.data,entry.size); if(!er.Read(serial))return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID; if(CIPHERREV_SerialEqual(serial,certificate.GetSerial()))return CIPHERCERTIFICATEX509REVOCATION_RESULT_REVOKED; }
  return CIPHERCERTIFICATEX509REVOCATION_RESULT_GOOD;
}

CIPHERCERTIFICATEX509REVOCATION_RESULT CIPHERCERTIFICATEX509REVOCATION::ValidateOCSP(XBUFFER& response,CIPHERCERTIFICATEX509& certificate,CIPHERCERTIFICATEX509& issuer)
{
  static const XBYTE basicOID[]={0x2B,0x06,0x01,0x05,0x05,0x07,0x30,0x01,0x01};
  CIPHERREV_READER ro(response.Get(),response.GetSize()); CIPHERREV_DER seq,status,rb;
  if(!ro.Read(seq)||!ro.End()||seq.tag!=0x30)return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID; CIPHERREV_READER r(seq.data,seq.size);
  if(!r.Read(status)||status.tag!=0x0A||status.size!=1||status.data[0]!=0||!r.Read(rb)||rb.tag!=0xA0)return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;
  CIPHERREV_READER rbr(rb.data,rb.size); CIPHERREV_DER rbs,oid,octets; if(!rbr.Read(rbs)||!rbr.End()||rbs.tag!=0x30)return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID; CIPHERREV_READER rr(rbs.data,rbs.size);
  if(!rr.Read(oid)||!CIPHERREV_OID(oid,basicOID,sizeof(basicOID))||!rr.Read(octets)||octets.tag!=0x04||!rr.End())return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;
  CIPHERREV_READER br(octets.data,octets.size); CIPHERREV_DER basic,tbs,algorithm,bits; if(!br.Read(basic)||!br.End()||basic.tag!=0x30)return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID; CIPHERREV_READER b(basic.data,basic.size);
  if(!b.Read(tbs)||!b.Read(algorithm)||!b.Read(bits)||tbs.tag!=0x30||!CIPHERREV_Verify(tbs,algorithm,bits,issuer))return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;
  CIPHERREV_READER tr(tbs.data,tbs.size); CIPHERREV_DER item;
  if(!tr.Read(item))return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID; if(item.tag==0xA0){if(!tr.Read(item))return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;}
  if((item.tag!=0xA1)&&(item.tag!=0xA2))return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;
  if(!tr.Read(item)||item.tag!=0x18||!tr.Read(item)||item.tag!=0x30)return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;
  CIPHERREV_READER responses(item.data,item.size);
  while(!responses.End())
    {
      CIPHERREV_DER single,certid,certstatus; if(!responses.Read(single)||single.tag!=0x30)return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID; CIPHERREV_READER sr(single.data,single.size);
      if(!sr.Read(certid)||certid.tag!=0x30)return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID; CIPHERREV_READER cr(certid.data,certid.size); CIPHERREV_DER ignored,serial;
      if(!cr.Read(ignored)||!cr.Read(ignored)||!cr.Read(ignored)||!cr.Read(serial)||!cr.End())return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;
      if(!sr.Read(certstatus))return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;
      CIPHERREV_DER thisupdate,nextwrapper,nextupdate;
      if(!sr.Read(thisupdate)||thisupdate.tag!=0x18||!sr.Read(nextwrapper)||nextwrapper.tag!=0xA0)return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;
      CIPHERREV_READER nr(nextwrapper.data,nextwrapper.size);if(!nr.Read(nextupdate)||!nr.End()||nextupdate.tag!=0x18||!CIPHERREV_TimeWindow(thisupdate,nextupdate))return CIPHERCERTIFICATEX509REVOCATION_RESULT_INVALID;
      if(CIPHERREV_SerialEqual(serial,certificate.GetSerial()))
        { if(certstatus.tag==0x80)return CIPHERCERTIFICATEX509REVOCATION_RESULT_GOOD; if(certstatus.tag==0xA1)return CIPHERCERTIFICATEX509REVOCATION_RESULT_REVOKED; return CIPHERCERTIFICATEX509REVOCATION_RESULT_UNKNOWN; }
    }
  return CIPHERCERTIFICATEX509REVOCATION_RESULT_UNKNOWN;
}
