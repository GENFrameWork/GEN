/**-------------------------------------------------------------------------------------------------------------------
* @file       CipherEd25519.cpp
* @class      CIPHERED25519
* @brief      Ed25519 signature algorithm (RFC 8032)
* @ingroup    CIPHER
* --------------------------------------------------------------------------------------------------------------------*/
#include "GEN_Defines.h"

#include "CipherEd25519.h"
#include "Cipher.h"

#include "XFactory.h"
#include "XRand.h"
#include "XMPInteger.h"
#include "HashSHA2.h"

#include "GEN_Control.h"

namespace
{
  static void SecureErase(void* data,XDWORD size)
  {
    volatile XBYTE* bytes=(volatile XBYTE*)data;
    for(XDWORD c=0;c<size;c++) bytes[c]=0;
  }

  struct EDPOINT
  {
    XMPINTEGER X;
    XMPINTEGER Y;
    XMPINTEGER Z;
    XMPINTEGER T;
  };

  static bool Const(XMPINTEGER& v, const XCHAR* text) { return v.SetFromString(16, (XCHAR*)text); }

  static bool ModMul(XMPINTEGER& r,XMPINTEGER& a,XMPINTEGER& b,XMPINTEGER& p)
  { XMPINTEGER t; return t.Multiplication(&a,&b) && r.Module(&r,&t,&p); }
  static bool ModAdd(XMPINTEGER& r,XMPINTEGER& a,XMPINTEGER& b,XMPINTEGER& p)
  { XMPINTEGER t; return t.AdditionSigned(&a,&b) && r.Module(&r,&t,&p); }
  static bool ModSub(XMPINTEGER& r,XMPINTEGER& a,XMPINTEGER& b,XMPINTEGER& p)
  { XMPINTEGER t; return t.SubtractionSigned(&a,&b) && r.Module(&r,&t,&p); }
  static bool ModDouble(XMPINTEGER& r,XMPINTEGER& a,XMPINTEGER& p)
  { XMPINTEGER t; return t.Multiplication(&a,2) && r.Module(&r,&t,&p); }

  static bool ImportLE(XMPINTEGER& v,const XBYTE* data,XDWORD size)
  {
    XBYTE temp[64]; if(!data||size>sizeof(temp))return false; for(XDWORD i=0;i<size;i++)temp[i]=data[size-1-i];
    bool s=v.ImportFromBinary(temp,size);SecureErase(temp,sizeof(temp));return s;
  }
  static bool ExportLE(XMPINTEGER& v,XBYTE* data,XDWORD size)
  {
    XBYTE temp[64]; if(!data||size>sizeof(temp))return false;memset(temp,0,sizeof(temp));
    bool status=v.ExportToBinary(temp,size);
    if(status) for(XDWORD i=0;i<size;i++)data[i]=temp[size-1-i];
    SecureErase(temp,sizeof(temp));return status;
  }

  static bool Hash512(const XBYTE* a,XDWORD as,const XBYTE* b,XDWORD bs,const XBYTE* c,XDWORD cs,XBYTE out[64])
  {
    XSECUREBUFFER in;if(as&&!in.Add((XBYTE*)a,as))return false;if(bs&&!in.Add((XBYTE*)b,bs))return false;if(cs&&!in.Add((XBYTE*)c,cs))return false;
    HASHSHA2 hash(HASHSHA2TYPE_512);if(!hash.Do(in)||!hash.GetResult()||hash.GetResult()->GetSize()!=64)return false;memcpy(out,hash.GetResult()->Get(),64);return true;
  }

  static bool InitConstants(XMPINTEGER& p,XMPINTEGER& l,XMPINTEGER& d,XMPINTEGER& bx,XMPINTEGER& by,XMPINTEGER& sqrtm1)
  {
    return Const(p,__L("7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFED")) &&
           Const(l,__L("1000000000000000000000000000000014DEF9DEA2F79CD65812631A5CF5D3ED")) &&
           Const(d,__L("52036CEE2B6FFE738CC740797779E89800700A4D4141D8AB75EB4DCA135978A3")) &&
           Const(bx,__L("216936D3CD6E53FEC0A4E231FDD6DC5C692CC7609525A7B2C9562D608F25D51A")) &&
           Const(by,__L("6666666666666666666666666666666666666666666666666666666666666658")) &&
           Const(sqrtm1,__L("2B8324804FC1DF0B2B4D00993DFBD7A72F431806AD2FE478C4EE1B274A0EA0B0"));
  }

  static bool PointIdentity(EDPOINT& p)
  { return p.X.LeftSet(0)&&p.Y.LeftSet(1)&&p.Z.LeftSet(1)&&p.T.LeftSet(0); }

  static bool PointAffine(EDPOINT& p,XMPINTEGER& x,XMPINTEGER& y,XMPINTEGER& prime)
  {
    XMPINTEGER zi;if(!zi.ModularInverse(&p.Z,&prime)||!ModMul(x,p.X,zi,prime)||!ModMul(y,p.Y,zi,prime))return false;return true;
  }

  static bool PointFromAffine(EDPOINT& p,XMPINTEGER& x,XMPINTEGER& y,XMPINTEGER& prime)
  { return p.X.CopyFrom(&x)&&p.Y.CopyFrom(&y)&&p.Z.LeftSet(1)&&ModMul(p.T,x,y,prime); }

  static bool PointAdd(EDPOINT& r,EDPOINT& p1,EDPOINT& p2,XMPINTEGER& prime,XMPINTEGER& d)
  {
    XMPINTEGER ymx1,ymx2,ypx1,ypx2,A,B,C,D,E,F,G,H,t1,t2;
    if(!ModSub(ymx1,p1.Y,p1.X,prime)||!ModSub(ymx2,p2.Y,p2.X,prime)||!ModMul(A,ymx1,ymx2,prime)||
       !ModAdd(ypx1,p1.Y,p1.X,prime)||!ModAdd(ypx2,p2.Y,p2.X,prime)||!ModMul(B,ypx1,ypx2,prime)||
       !ModMul(t1,p1.T,p2.T,prime)||!ModMul(t2,d,t1,prime)||!ModDouble(C,t2,prime)||
       !ModMul(t1,p1.Z,p2.Z,prime)||!ModDouble(D,t1,prime)||!ModSub(E,B,A,prime)||!ModSub(F,D,C,prime)||
       !ModAdd(G,D,C,prime)||!ModAdd(H,B,A,prime)||!ModMul(r.X,E,F,prime)||!ModMul(r.Y,G,H,prime)||
       !ModMul(r.T,E,H,prime)||!ModMul(r.Z,F,G,prime)) return false;return true;
  }

  static bool PointDouble(EDPOINT& r,EDPOINT& p,XMPINTEGER& prime)
  {
    XMPINTEGER A,B,C,D,E,F,G,H,t1,t2;
    if(!ModMul(A,p.X,p.X,prime)||!ModMul(B,p.Y,p.Y,prime)||!ModMul(t1,p.Z,p.Z,prime)||!ModDouble(C,t1,prime)||
       !D.SubtractionSigned(&prime,&A)||!D.Module(&D,&D,&prime)||!ModAdd(t1,p.X,p.Y,prime)||!ModMul(t2,t1,t1,prime)||
       !ModSub(t1,t2,A,prime)||!ModSub(E,t1,B,prime)||!ModAdd(G,D,B,prime)||!ModSub(F,G,C,prime)||!ModSub(H,D,B,prime)||
       !ModMul(r.X,E,F,prime)||!ModMul(r.Y,G,H,prime)||!ModMul(r.T,E,H,prime)||!ModMul(r.Z,F,G,prime))return false;return true;
  }

  // Fixed number of iterations for every scalar multiplication over Ed25519's ~255-bit field/order, so that
  // neither the loop trip count nor the addition step depend on the (potentially secret) scalar's bit pattern
  // or bit-length: both leak timing information that can be used to recover a static private key or a per-
  // signature nonce (see e.g. the "Minerva"-class attacks against non-constant-time Ed25519/ECDSA scalar mul).
  static const XDWORD CIPHERED25519_SCALARBITS = 256;

  static bool IntegerSelect(XMPINTEGER& result,XMPINTEGER& value0,XMPINTEGER& value1,XBYTE select1,XDWORD fixedlimbs)
  {
    if(!fixedlimbs||!value0.Grow(fixedlimbs)||!value1.Grow(fixedlimbs)||!result.Grow(fixedlimbs))return false;

    XLIMB mask=(XLIMB)0-(XLIMB)(select1&1);
    for(XDWORD c=0;c<fixedlimbs;c++) result.GetLimbs()[c]=(value0.GetLimbs()[c]&~mask)|(value1.GetLimbs()[c]&mask);
    result.SetSign(1);

    return true;
  }

  static bool PointSelect(EDPOINT& result,EDPOINT& point0,EDPOINT& point1,XBYTE select1,XDWORD fixedlimbs)
  {
    return IntegerSelect(result.X,point0.X,point1.X,select1,fixedlimbs)&&IntegerSelect(result.Y,point0.Y,point1.Y,select1,fixedlimbs)&&
           IntegerSelect(result.Z,point0.Z,point1.Z,select1,fixedlimbs)&&IntegerSelect(result.T,point0.T,point1.T,select1,fixedlimbs);
  }

  static bool PointMul(EDPOINT& r,XMPINTEGER& scalar,EDPOINT& point,XMPINTEGER& prime,XMPINTEGER& d)
  {
    const XDWORD fixedlimbs=XMPINTEGER_BITSTOLIMBS(CIPHERED25519_SCALARBITS);

    // Force the scalar to the fixed width up front: GetBit() below returns 0 without touching the limb array
    // once the requested position is beyond the operand's current allocation, and that early-return is itself
    // a (data-independent-looking but allocation-size-dependent) branch. Pre-growing removes it from the loop.
    if(!scalar.Grow(fixedlimbs))return false;

    EDPOINT acc,base,added,selected;
    if(!PointIdentity(acc)||!base.X.CopyFrom(&point.X)||!base.Y.CopyFrom(&point.Y)||!base.Z.CopyFrom(&point.Z)||!base.T.CopyFrom(&point.T))return false;

    for(XDWORD i=0;i<CIPHERED25519_SCALARBITS;i++)
      {
        // acc = acc + base is always computed; the branch only decides which of the two results (acc unchanged,
        // or acc+base) is kept, via a constant-time limb-wise select instead of a scalar-bit-dependent 'if'.
        if(!PointAdd(added,acc,base,prime,d))return false;
        if(!PointSelect(selected,acc,added,(XBYTE)scalar.GetBit(i),fixedlimbs))return false;
        acc.X.Swap(&selected.X);acc.Y.Swap(&selected.Y);acc.Z.Swap(&selected.Z);acc.T.Swap(&selected.T);

        // base = 2*base is unconditional (right-to-left double-and-add): no secret-dependent branch here.
        if(!PointDouble(added,base,prime))return false;
        base.X.Swap(&added.X);base.Y.Swap(&added.Y);base.Z.Swap(&added.Z);base.T.Swap(&added.T);
      }

    r.X.Swap(&acc.X);r.Y.Swap(&acc.Y);r.Z.Swap(&acc.Z);r.T.Swap(&acc.T);return true;
  }

  static bool PointEncode(EDPOINT& p,XBYTE out[32],XMPINTEGER& prime)
  {
    XMPINTEGER x,y;if(!PointAffine(p,x,y,prime)||!ExportLE(y,out,32))return false; if(x.GetBit(0))out[31]|=0x80;else out[31]&=0x7f;return true;
  }

  static bool PointDecode(const XBYTE in[32],EDPOINT& point,XMPINTEGER& prime,XMPINTEGER& d,XMPINTEGER& sqrtm1)
  {
    XBYTE enc[32];memcpy(enc,in,32);int sign=(enc[31]>>7)&1;enc[31]&=0x7f;XMPINTEGER y;if(!ImportLE(y,enc,32)||y.CompareSignedValues(prime)>=0)return false;
    XMPINTEGER y2,u,v,vinv,x2,exp,x,check,rr;
    if(!ModMul(y2,y,y,prime)||!u.SubtractionSigned(&y2,1)||!u.Module(&u,&u,&prime)||!ModMul(v,d,y2,prime)||!v.AdditionSigned(&v,1)||!v.Module(&v,&v,&prime)||!vinv.ModularInverse(&v,&prime)||!ModMul(x2,u,vinv,prime))return false;
    if(!exp.CopyFrom(&prime)||!exp.AdditionSigned(&exp,3)||!exp.RightShift(3)||!x.CopyFrom(&x2)||!x.SlidingWindowExponentiation(&x,&exp,&prime,&rr)||!ModMul(check,x,x,prime))return false;
    if(check.CompareSignedValues(x2)!=0){if(!ModMul(x,x,sqrtm1,prime)||!ModMul(check,x,x,prime)||check.CompareSignedValues(x2)!=0)return false;}
    if(x.CompareSignedValues(0)==0&&sign)return false;if(x.GetBit(0)!=sign){XMPINTEGER nx;if(!ModSub(nx,prime,x,prime))return false;x.Swap(&nx);}
    return PointFromAffine(point,x,y,prime);
  }

  static bool PointIsPrimeOrder(EDPOINT& point,XMPINTEGER& order,XMPINTEGER& prime,XMPINTEGER& d,bool rejectidentity)
  {
    EDPOINT multiple;
    XBYTE encoded[32];
    XBYTE identity[32]={1};

    if(rejectidentity)
      {
        if(!PointEncode(point,encoded,prime)) return false;
        if(CIPHER::CompareConstantTime(encoded,identity,sizeof(encoded))) return false;
      }

    bool status=PointMul(multiple,order,point,prime,d) && PointEncode(multiple,encoded,prime) &&
                CIPHER::CompareConstantTime(encoded,identity,sizeof(encoded));
    SecureErase(encoded,sizeof(encoded));
    return status;
  }

  static bool ScalarFromLEMod(XMPINTEGER& s,const XBYTE* data,XDWORD size,XMPINTEGER& l)
  { XMPINTEGER t;if(!ImportLE(t,data,size))return false;return s.Module(&s,&t,&l); }

  static bool PublicFromSeed(const XBYTE seed[32],XBYTE publickey[32])
  {
    XMPINTEGER p,l,d,bx,by,sqrtm1,a;XBYTE h[64]={0};EDPOINT B,A;bool status=false;
    do
      {
        if(!InitConstants(p,l,d,bx,by,sqrtm1)||!Hash512(seed,32,NULL,0,NULL,0,h)) break;
        h[0]&=248;h[31]&=63;h[31]|=64;
        if(!ImportLE(a,h,32)||!PointFromAffine(B,bx,by,p)||!PointMul(A,a,B,p,d)||!PointEncode(A,publickey,p)) break;
        status=true;
      }
    while(false);
    SecureErase(h,sizeof(h));return status;
  }
}

CIPHERED25519::CIPHERED25519(){Clean();}
CIPHERED25519::~CIPHERED25519(){Clean();}

bool CIPHERED25519::Random(XBYTE* data,XDWORD size)
{
  if(!data||!size)return false;XRAND* xrand=GEN_XFACTORY.CreateRand();if(!xrand)return false;bool status=false;if(xrand->Ini()&&xrand->IsCryptographicallySecure())status=xrand->Generate(data,size);GEN_XFACTORY.DeleteRand(xrand);return status;
}

bool CIPHERED25519::KeyPair_Create(XBUFFER& privatekey,XBUFFER& publickey)
{
  XBYTE seed[32]={0},pub[32]={0};privatekey.SecureDelete();publickey.Delete();bool status=Random(seed,sizeof(seed))&&PublicFromSeed(seed,pub)&&privatekey.Add(seed,32)&&publickey.Add(pub,32);if(!status){privatekey.SecureDelete();publickey.Delete();}SecureErase(seed,sizeof(seed));SecureErase(pub,sizeof(pub));return status;
}

bool CIPHERED25519::PublicKey_Create(XBUFFER& privatekey,XBUFFER& publickey)
{
  if(privatekey.GetSize()!=32)return false;XBYTE pub[32]={0};publickey.Delete();bool status=PublicFromSeed(privatekey.Get(),pub)&&publickey.Add(pub,32);if(!status)publickey.Delete();SecureErase(pub,sizeof(pub));return status;
}

bool CIPHERED25519::PublicKey_IsValid(XBUFFER& publickey)
{
  if(publickey.GetSize()!=CIPHERED25519_PUBLICKEYSIZE)return false;
  XMPINTEGER p,l,d,bx,by,sqrtm1;EDPOINT point;XBYTE canonical[32]={0};
  bool status=InitConstants(p,l,d,bx,by,sqrtm1)&&PointDecode(publickey.Get(),point,p,d,sqrtm1)&&
              PointEncode(point,canonical,p)&&CIPHER::CompareConstantTime(canonical,publickey.Get(),sizeof(canonical))&&
              PointIsPrimeOrder(point,l,p,d,true);
  SecureErase(canonical,sizeof(canonical));return status;
}

bool CIPHERED25519::Sign(XBUFFER& privatekey,XBUFFER& publickey,XBUFFER& input,XBUFFER& signature)
{
  signature.Delete();if(privatekey.GetSize()!=32||publickey.GetSize()!=32||!PublicKey_IsValid(publickey))return false;
  XBYTE derived[32]={0},h[64]={0},rh[64]={0},kh[64]={0},Renc[32]={0},Senc[32]={0};
  XMPINTEGER p,l,d,bx,by,sqrtm1,a,r,k,S,tmp;EDPOINT B,R;bool status=false;
  do
    {
      if(!PublicFromSeed(privatekey.Get(),derived)||!CIPHER::CompareConstantTime(derived,publickey.Get(),32)||
         !InitConstants(p,l,d,bx,by,sqrtm1)||!Hash512(privatekey.Get(),32,NULL,0,NULL,0,h)) break;
      h[0]&=248;h[31]&=63;h[31]|=64;
      if(!ImportLE(a,h,32)||!Hash512(h+32,32,input.Get(),input.GetSize(),NULL,0,rh)||!ScalarFromLEMod(r,rh,64,l)||
         !PointFromAffine(B,bx,by,p)||!PointMul(R,r,B,p,d)||!PointEncode(R,Renc,p)||
         !Hash512(Renc,32,publickey.Get(),32,input.Get(),input.GetSize(),kh)||!ScalarFromLEMod(k,kh,64,l)||
         !ModMul(tmp,k,a,l)||!ModAdd(S,r,tmp,l)||!ExportLE(S,Senc,32)) break;
      status=signature.Add(Renc,32)&&signature.Add(Senc,32);
    }
  while(false);
  if(!status)signature.Delete();SecureErase(derived,sizeof(derived));SecureErase(h,sizeof(h));SecureErase(rh,sizeof(rh));SecureErase(kh,sizeof(kh));SecureErase(Renc,sizeof(Renc));SecureErase(Senc,sizeof(Senc));return status;
}

bool CIPHERED25519::Verify(XBUFFER& publickey,XBUFFER& input,XBUFFER& signature)
{
  if(publickey.GetSize()!=32||signature.GetSize()!=64||!PublicKey_IsValid(publickey))return false;
  XMPINTEGER p,l,d,bx,by,sqrtm1,S,k;EDPOINT A,R,B,SB,kA,rhs;XBYTE kh[64]={0},left[32]={0},right[32]={0},canonicalR[32]={0};bool status=false;
  do
    {
      if(!InitConstants(p,l,d,bx,by,sqrtm1)||!ImportLE(S,signature.Get()+32,32)||S.CompareSignedValues(l)>=0||
         !PointDecode(publickey.Get(),A,p,d,sqrtm1)||!PointDecode(signature.Get(),R,p,d,sqrtm1)||
         !PointEncode(R,canonicalR,p)||!CIPHER::CompareConstantTime(canonicalR,signature.Get(),sizeof(canonicalR))||
         !PointIsPrimeOrder(R,l,p,d,false)||!PointFromAffine(B,bx,by,p)||
         !Hash512(signature.Get(),32,publickey.Get(),32,input.Get(),input.GetSize(),kh)||!ScalarFromLEMod(k,kh,64,l)||
         !PointMul(SB,S,B,p,d)||!PointMul(kA,k,A,p,d)||!PointAdd(rhs,R,kA,p,d)||
         !PointEncode(SB,left,p)||!PointEncode(rhs,right,p)) break;
      status=CIPHER::CompareConstantTime(left,right,32);
    }
  while(false);
  SecureErase(kh,sizeof(kh));SecureErase(left,sizeof(left));SecureErase(right,sizeof(right));SecureErase(canonicalR,sizeof(canonicalR));return status;
}

void CIPHERED25519::Clean(){}
