/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XBER.h
*
* @class      XBER
* @brief      eXtended Basic Encoding Rules (BER) encoding class (https://en.wikipedia.org/wiki/X.690)
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

#ifndef _XBER_H_
#define _XBER_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XBase.h"
#include "XVector.h"
#include "XString.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define XBERTYPE_EXTENDED                 0x01
#define XBERTYPE_INTEGER                  0x02
#define XBERTYPE_BITSTRING                0x03
#define XBERTYPE_OCTETSTRING              0x04
#define XBERTYPE_NULL                     0x05
#define XBERTYPE_OID                      0x06
#define XBERTYPE_SEQOFSEQ                 0x10
#define XBERTYPE_SETOFSET                 0x11
#define XBERTYPE_PRINTABLESTRING          0x13
#define XBERTYPE_IA5STRING                0x16
#define XBERTYPE_UTCTIME                  0x17

#define XBERTYPE_ISUNIVERSAL              0x00
#define XBERTYPE_ISCONSTRUCTED            0x20
#define XBERTYPE_ISAPPLICATION            0x40
#define XBERTYPE_ISCONTEXTSPECIFIC        0x80
#define XBERTYPE_ISPRIVATE                0xC0



/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class XFACTORY;
class XBUFFER;

class XBER
{
  public:
                              XBER                      ();
    virtual                  ~XBER                      ();

    XBYTE                     GetType                   ();
    XDWORD                    GetSize                   ();
    XBUFFER*                  GetData                   ();

    bool                      GetDump                   (XBUFFER& xbuffer, bool notheader = false);
    bool                      SetFromDump               (XBUFFER& xbuffer);

    bool                      SetType                   (XBYTE type);
    bool                      SetSize                   (XDWORD size);

    bool                      SetNULL                   ();

    bool                      SetINTEGER                (XDWORD integer);
    bool                      SetINTEGER                (XBUFFER& xbufferdata);

    bool                      SetBITSTRING              (XBUFFER& databuffer);
    bool                      SetBITSTRING              (XBYTE* buffer, XDWORD size);

    bool                      SetOCTETSTRING            (XCHAR* string);
    bool                      SetOCTETSTRING            (XBUFFER& databuffer);
    bool                      SetOCTETSTRING            (XBYTE* buffer, XDWORD size);

    bool                      SetOID                    (XCHAR* OIDstring);

    bool                      Copy                      (XBER* newber);

    bool                      Sequence_AddCommandTo     (XBYTE command, XBER& xber);
    virtual bool              Sequence_AddTo            (XBER& xber);
    XDWORD                    Sequence_GetSize          ();
    bool                      Sequence_DeleteAll        ();

  protected:

    XBYTE                     type;
    XDWORD                    size;
    XBUFFER*                  data;
    XVECTOR<XBER*>            sequence;

  private:

    void                      Clean                     ();

    bool                      CodeBigNumber             (XDWORD number,XBUFFER& data);
    bool                      CodeSize                  (XDWORD number,XBUFFER& data);

};


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/

#endif


