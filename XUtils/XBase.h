/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XBase.h
*
* @class      XBASE
* @brief      Basic functions and defines
* @ingroup    XUTILS
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

#ifndef _XBASE_H_
#define _XBASE_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#ifndef XBYTE
typedef unsigned char         XBYTE;
#endif

#ifndef XWORD
typedef unsigned short        XWORD;
#endif

#ifndef XDWORD
typedef unsigned int          XDWORD;
#endif

#ifndef XQWORD
typedef unsigned long long    XQWORD;
#endif


#ifndef XBYTESIG
typedef signed char           XBYTESIG;
#endif

#ifndef XWORDSIG
typedef signed short          XWORDSIG;
#endif

#ifndef XDWORDSIG
typedef signed int            XDWORDSIG;
#endif

#ifndef XQWORDSIG
typedef signed long long      XQWORDSIG;
#endif

#ifndef XREAL
typedef double                XREAL;
#endif

#ifndef POINTER
typedef void*                 POINTER;
#endif


#ifndef NULL
#define NULL                  0
#endif

#ifndef TRUE
#define TRUE                  1
#endif

#ifndef FALSE
#define FALSE                 0
#endif


#define NOTFOUND             -1

#define ISEQUAL               0
#define ISGREATER             1
#define ISLESS               -1

#define COMMENT SLASH(/)
#define SLASH(s) /##s

#ifndef __MAX
#define __MAX(a,b)            (((a)>(b))?(a):(b))
#endif

#ifndef __MIN
#define __MIN(a,b)            (((a)<(b))?(a):(b))
#endif

#ifdef MICROCONTROLLER
#define _MAXSTR               32
#else
#define _MAXSTR               256
#endif
#define _MAXPATH              _MAXSTR
#define _MAXBUFFER            _MAXSTR

#define PI                    3.141592654f

#ifndef MPI
#define MPI                   1.570796327f
#endif

#define EPSILON               0.00000000001f

#define DEGREE2RADIAN(c)      ((c*PI)/180.0f)//((c / 2.0f / 180.0f) * PI)
#define RADIAN2DEGREE(c)      ((c * 180.0f) / PI)

#define TRUNCATE(number)      (number - fmod(number, 0.01))

#define SWAPWORD(data)        data = SwapWORD(data);
#define SWAPDWORD(data)       data = SwapDWORD(data);
#define SWAPQWORD(data)       data = SwapQWORD(data);


#ifdef APPMODE_API_LIB_ACTIVE

  #if defined(WINDOWS)
    //  Microsoft    
    #ifdef APPMODE_API_LIB_EXPORT_ACTIVE
      #define GEN_API_LIB __declspec(dllexport)  
    #else  
      #define GEN_API_LIB __declspec(dllimport)
    #endif  

  #elif defined(LINUX)
    //  GCC
    #include <dlfcn.h> 
    #ifdef APPMODE_API_LIB_EXPORT_ACTIVE
      #define GEN_API_LIB __attribute__((visibility("default")))
    #else
      #define GEN_API_LIB
    #endif  
 
  #else
    //  do nothing and hope for the best?
    #define GEN_API_LIB
  
  #endif
#else
  #define GEN_API_LIB
#endif


typedef bool (*PROGRESS_FUNCTION)(void);
 

#ifdef _MSC_VER    
  #define  TYPEDEF_STRUCT_ALIGNMENT_BYTE_INI                                            \
                                                      _Pragma("pack(push)")             \
                                                      _Pragma("pack(1)")                \
                                                      typedef struct
#endif  


#ifdef __GNUC__                                                          
  #define  TYPEDEF_STRUCT_ALIGNMENT_BYTE_INI          typedef struct __attribute__ ((__packed__))
#endif
 

#ifdef _MSC_VER    
 #define  TYPEDEF_STRUCT_ALIGNMENT_BYTE_END           _Pragma("pack(pop)")                                                          
#else
 #define  TYPEDEF_STRUCT_ALIGNMENT_BYTE_END
#endif


#define GEN_MODULE_EXEC   __FILE__
#define GEN_LINE_EXEC     __LINE__

#ifdef COMPILER_MSVC
  #define GEN_FUNCTION_EXEC  __FUNCSIG__
#else
  #ifdef COMPILER_GCC
    #define GEN_FUNCTION_EXEC  __PRETTY_FUNCTION__
  #else
    #ifdef COMPILER_CLANG)
      #define GEN_FUNCTION_EXEC  __PRETTY_FUNCTION__
    #else 
      #define GEN_FUNCTION_EXEC  ""  
    #endif
  #endif 
#endif 


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XBASE
{
  public:
                     XBASE   ();
    virtual         ~XBASE   ();

  protected:

  private:

    void             Clean   ();
};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

double  Truncate          (double value);
double  Fraction          (double value);

float   RoundOff          (float value , XBYTE prec);
double  RoundOff          (double value, XBYTE prec);

XBYTE   High_WORD         (XWORD  data);
XWORD   High_DWORD        (XDWORD data);
XDWORD  High_QWORD        (XQWORD data);

XBYTE   Low_WORD          (XWORD  data);
XWORD   Low_DWORD         (XDWORD data);
XDWORD  Low_QWORD         (XQWORD data);

XWORD   SwapWORD          (XWORD  data);
XDWORD  SwapDWORD         (XDWORD data);
XQWORD  SwapQWORD         (XQWORD data);

int     InvertSign        (int number);

float   RoundFloat        (float x);

XQWORD  DWORDToBCD        (XDWORD dword);
XDWORD  BCDToDWORD        (XQWORD bcd);

XBYTE   RotateBitLeft     (XBYTE byte);
XBYTE   RotateBitRight    (XBYTE byte);
XBYTE   RotateBitReverse  (XBYTE byte);

#endif

