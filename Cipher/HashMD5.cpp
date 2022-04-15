/*------------------------------------------------------------------------------------------
//  HASHMD5.CPP
//
//  Hash MD5 class
//
//  Author            : Abraham J. Velez
//  Date Of Creation  : 02/03/2013 12:07:38
//  Last Mofificacion :
//
//  GEN  Copyright (C).  All right reserved.
//----------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES --------------------------------------------------------------------------*/

#include <string.h>

#include "XBuffer.h"

#include "HashMD5.h"

#include "XMemory_Control.h"


/*---- DEFINES & ENUMS  ------------------------------------------------------------------*/

#define S11   7
#define S12   12
#define S13   17
#define S14   22
#define S21   5
#define S22   9
#define S23   14
#define S24   20
#define S31   4
#define S32   11
#define S33   16
#define S34   23
#define S41   6
#define S42   10
#define S43   15
#define S44   21


/*---- GENERAL VARIABLE ------------------------------------------------------------------*/


/*---- CLASS MEMBERS ---------------------------------------------------------------------*/



/*-------------------------------------------------------------------
//  HASHMD5::HASHMD5
*/
/**
//
//
//  ""
//  @version      02/03/2013 12:43:10
//
//  @return
*/
/*-----------------------------------------------------------------*/
HASHMD5::HASHMD5() : HASH()
{
  Clean();

  type = HASHTYPE_MD5;

  Ini();
}



/*-------------------------------------------------------------------
//  HASHMD5::~HASHMD5
*/
/**
//
//
//  ""
//  @version      02/03/2013 12:09:01
//
//  @return
//  */
/*-----------------------------------------------------------------*/
HASHMD5::~HASHMD5()
{
  Clean();
}



/*-------------------------------------------------------------------
//  HASHMD5::Do
*/
/**
//
//
//  ""
//  @version      03/03/2013 16:26:19
//
//  @return       bool :
//  @param        input :
//  @param        size :
*/
/*-----------------------------------------------------------------*/
bool HASHMD5::Do(XBYTE* input, XQWORD size)
{
  if(!result) return false;

  result->Delete();

  Ini();

  Update(input,size);

  End();

  result->Add(digest,16);

  return true;
}



/*-------------------------------------------------------------------
//  HASHMD5::GetDefaultSize
*/
/**
//
//
//  ""
//  @version      21/04/2013 0:55:35
//
//  @return       int :
//  */
/*-----------------------------------------------------------------*/
int HASHMD5::GetDefaultSize()
{
  return 16;
}



/*-------------------------------------------------------------------
//  HASHMD5::Clean
*/
/**
//
//
//  ""
//  @version      02/03/2013 12:09:56
//
//  @return       void :
//  */
/*-----------------------------------------------------------------*/
void HASHMD5::Clean()
{
  finalized=false;

  count[0] = 0;
  count[1] = 0;

  // load magic initialization constants.
  state[0] = 0x67452301;
  state[1] = 0xefcdab89;
  state[2] = 0x98badcfe;
  state[3] = 0x10325476;
}




/*-------------------------------------------------------------------
//  HASHMD5::Ini
*/
/**
//
//
//  ""
//  @version      02/03/2013 17:03:10
//
//  @return       void :
//  */
/*-----------------------------------------------------------------*/
void HASHMD5::Ini()
{
  finalized=false;

  count[0] = 0;
  count[1] = 0;

  // load magic initialization constants.
  state[0] = 0x67452301;
  state[1] = 0xefcdab89;
  state[2] = 0x98badcfe;
  state[3] = 0x10325476;
}




/*-------------------------------------------------------------------
//  HASHMD5::Update
*/
/**
//
//
//  ""
//  @version      02/03/2013 20:37:01
//
//  @return       void :
//  @param        buffer :
//  @param        size :
*/
/*-----------------------------------------------------------------*/
void HASHMD5::Update(XBYTE* input, XQWORD size)
{
  // compute number of bytes mod 64
  XDWORD index = count[0] / 8 % HASHMD5_BLOCKSIZE;

  // Update number of bits
  if ((count[0] += (XDWORD)(size << 3)) < (XDWORD)(size << 3)) count[1]++;

  count[1] += (XDWORD)(size >> 29);

  // number of bytes we need to fill in buffer
  XDWORD firstpart = 64 - index;

  XDWORD i;

  // transform as many times as possible.
  if(size >= firstpart)
    {
      // fill buffer first, transform
      memcpy(&buffer[index], input, firstpart);

      Transform(buffer);

    // transform chunks of HASHMD5_BLOCKSIZE (64 bytes)
    for(i = firstpart; i + HASHMD5_BLOCKSIZE <= (XDWORD)size; i += HASHMD5_BLOCKSIZE)
      {
        Transform(&input[i]);
      }

    index = 0;

  } else i = 0;

  // buffer remaining input
  memcpy(&buffer[index], &input[i], (XDWORD)(size-i));
}




/*-------------------------------------------------------------------
//  HASHMD5::Transform
*/
/**
//
//
//  ""
//  @version      02/03/2013 20:56:30
//
//  @return       void :
//  @param        block[HASHMD5_BLOCKSIZE] :
*/
/*-----------------------------------------------------------------*/
void HASHMD5::Transform(XBYTE block[HASHMD5_BLOCKSIZE])
{
  XDWORD a = state[0];
  XDWORD b = state[1];
  XDWORD c = state[2];
  XDWORD d = state[3];
  XDWORD x[16];

  Decode(x, block, HASHMD5_BLOCKSIZE);

  /* Round 1 */
  FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
  FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
  FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
  FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
  FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
  FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
  FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
  FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
  FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
  FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
  FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
  FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
  FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
  FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
  FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
  FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

  /* Round 2 */
  GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
  GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
  GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
  GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
  GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
  GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
  GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
  GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
  GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
  GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
  GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
  GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
  GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
  GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
  GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
  GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

  /* Round 3 */
  HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
  HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
  HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
  HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
  HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
  HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
  HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
  HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
  HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
  HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
  HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
  HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
  HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
  HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
  HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
  HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

  /* Round 4 */
  II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
  II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
  II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
  II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
  II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
  II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
  II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
  II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
  II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
  II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
  II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
  II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
  II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
  II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
  II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
  II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;

  // Zeroize sensitive information.
  memset(x, 0, sizeof(x));
}



/*-------------------------------------------------------------------
//  HASHMD5::Decode
*/
/**
//
//
//  ""
//  @version      02/03/2013 20:59:30
//
//  @return       void :
//  @param        output[] :
//  @param        input[] :
//  @param        size :
*/
/*-----------------------------------------------------------------*/
void HASHMD5::Decode(XDWORD* output, XBYTE* input, XDWORD size)
{
  for(XDWORD i=0, j=0; j<size; i++, j+=4)
    {
      output[i] = ((XDWORD)input[j]) | (((XDWORD)input[j+1]) << 8) | (((XDWORD)input[j+2]) << 16) | (((XDWORD)input[j+3]) << 24);
    }
}



/*-------------------------------------------------------------------
//  HASHMD5::Encode
*/
/**
//
//
//  ""
//  @version      02/03/2013 20:59:26
//
//  @return       void :
//  @param        output[] :
//  @param        input[] :
//  @param        size :
*/
/*-----------------------------------------------------------------*/
void HASHMD5::Encode(XBYTE* output, XDWORD* input, XDWORD size)
{
  for(XDWORD i=0, j=0; j<size; i++, j+= 4)
    {
      output[j] = input[i] & 0xff;
      output[j+1] = (input[i] >> 8) & 0xff;
      output[j+2] = (input[i] >> 16) & 0xff;
      output[j+3] = (input[i] >> 24) & 0xff;
    }
}




/*-------------------------------------------------------------------
//  HASHMD5::End
*/
/**
//
//
//  ""
//  @version      02/03/2013 20:50:06
//
//  @return       void :
//  */
/*-----------------------------------------------------------------*/
void HASHMD5::End()
{
  unsigned char padding[64] = { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                                     };

  if(finalized) return;

    // Save number of bits
  XBYTE bits[8];

  Encode(bits, count, 8);

    // pad out to 56 mod 64.
  XDWORD index = count[0] / 8 % 64;
  XDWORD padLen = (index < 56) ? (56 - index) : (120 - index);

  Update(padding, padLen);

    // Append size (before padding)
  Update(bits, 8);

   // Store state in digest
  Encode(digest, state, 16);

  // Zeroize sensitive information.
  memset(buffer, 0, sizeof buffer);
  memset(count, 0, sizeof count);

  finalized=true;
}




/*-------------------------------------------------------------------
//  XDWORD HASHMD5::F
*/
/**
//
//
//  ""
//  @version      02/03/2013 16:36:11
//
//  @return       inline :
//  @param        x :
//  @param        y :
//  @param        z :
*/
/*-----------------------------------------------------------------*/
inline XDWORD HASHMD5::F(XDWORD x, XDWORD y, XDWORD z)
{
  return (x&y) | (~x&z);
}



/*-------------------------------------------------------------------
//  XDWORD HASHMD5::G
*/
/**
//
//
//  ""
//  @version      02/03/2013 16:36:17
//
//  @return       inline :
//  @param        x :
//  @param        y :
//  @param        z :
*/
/*-----------------------------------------------------------------*/
inline XDWORD HASHMD5::G(XDWORD x, XDWORD y, XDWORD z)
{
  return (x&z) | (y&~z);
}



/*-------------------------------------------------------------------
//  XDWORD HASHMD5::H
*/
/**
//
//
//  ""
//  @version      02/03/2013 16:36:29
//
//  @return       inline :
//  @param        x :
//  @param        y :
//  @param        z :
*/
/*-----------------------------------------------------------------*/
inline XDWORD HASHMD5::H(XDWORD x, XDWORD y, XDWORD z)
{
  return x^y^z;
}




/*-------------------------------------------------------------------
//  XDWORD HASHMD5::I
*/
/**
//
//
//  ""
//  @version      02/03/2013 16:37:11
//
//  @return       inline :
//  @param        x :
//  @param        y :
//  @param        z :
*/
/*-----------------------------------------------------------------*/
inline XDWORD HASHMD5::I(XDWORD x, XDWORD y, XDWORD z)
{
  return y ^ (x | ~z);
}



/*-------------------------------------------------------------------
//  XDWORD HASHMD5::RotateLeft
*/
/**
//
//
//  ""
//  @version      02/03/2013 16:44:28
//
//  @return       inline :
//  @param        x :
//  @param        n :
*/
/*-----------------------------------------------------------------*/
inline XDWORD HASHMD5::RotateLeft(XDWORD x, int n)
{
  return (x << n) | (x >> (32-n));
}



/*-------------------------------------------------------------------
//  void HASHMD5::FF
*/
/**
//
//
//  ""
//  @version      02/03/2013 16:37:34
//
//  @return       inline :
//  @param        &a :
//  @param        b :
//  @param        c :
//  @param        d :
//  @param        x :
//  @param        s :
//  @param        ac :
*/
/*-----------------------------------------------------------------*/
inline void HASHMD5::FF(XDWORD &a, XDWORD b, XDWORD c, XDWORD d, XDWORD x, XDWORD s, XDWORD ac)
{
  a = RotateLeft(a+ F(b,c,d) + x + ac, s) + b;
}



/*-------------------------------------------------------------------
//  void HASHMD5::GG
*/
/**
//
//
//  ""
//  @version      02/03/2013 16:37:47
//
//  @return       inline :
//  @param        &a :
//  @param        b :
//  @param        c :
//  @param        d :
//  @param        x :
//  @param        s :
//  @param        ac :
*/
/*-----------------------------------------------------------------*/
inline void HASHMD5::GG(XDWORD &a, XDWORD b, XDWORD c, XDWORD d, XDWORD x, XDWORD s, XDWORD ac)
{
  a = RotateLeft(a + G(b,c,d) + x + ac, s) + b;
}



/*-------------------------------------------------------------------
//  void HASHMD5::HH
*/
/**
//
//
//  ""
//  @version      02/03/2013 16:38:06
//
//  @return       inline :
//  @param        &a :
//  @param        b :
//  @param        c :
//  @param        d :
//  @param        x :
//  @param        s :
//  @param        ac :
*/
/*-----------------------------------------------------------------*/
inline void HASHMD5::HH(XDWORD &a, XDWORD b, XDWORD c, XDWORD d, XDWORD x, XDWORD s, XDWORD ac)
{
  a = RotateLeft(a + H(b,c,d) + x + ac, s) + b;
}



/*-------------------------------------------------------------------
//  void HASHMD5::II
*/
/**
//
//
//  ""
//  @version      02/03/2013 16:38:12
//
//  @return       inline :
//  @param        &a :
//  @param        b :
//  @param        c :
//  @param        d :
//  @param        x :
//  @param        s :
//  @param        ac :
*/
/*-----------------------------------------------------------------*/
inline void HASHMD5::II(XDWORD &a, XDWORD b, XDWORD c, XDWORD d, XDWORD x, XDWORD s, XDWORD ac)
{
  a = RotateLeft(a + I(b,c,d) + x + ac, s) + b;
}


