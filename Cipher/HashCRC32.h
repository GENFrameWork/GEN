/*------------------------------------------------------------------------------------------
//  HASHCRC32.H
*/
/**
// \class
//
//  Hash CRC 32 bits Class
//
//  ""
//  @version 02/03/2013 12:02:40
*/
/*  GEN  Copyright (C).  All right reserved.
//----------------------------------------------------------------------------------------*/

#ifndef _HASHCRC32_H_
#define _HASHCRC32_H_


/*---- INCLUDES --------------------------------------------------------------------------*/

#include "Hash.h"

/*---- DEFINES & ENUMS  ------------------------------------------------------------------*/


/*---- CLASS -----------------------------------------------------------------------------*/


class HASHCRC32 : public HASH
{
  public:
                        HASHCRC32               ();
    virtual            ~HASHCRC32               ();

    using               HASH::Do;
    bool                Do                      (XBYTE* input, XQWORD size);

    bool                ResetResult             ();

    int                 GetDefaultSize          ();

    XBUFFER*            GetResult               ();
    XBYTE*              GetResult               (XDWORD& resultsize);

    XDWORD              GetResultCRC32          ();

  private:

    void                Clean                   ();

    void                Ini                     (void);
    XDWORD              Update                  (XDWORD CRC32, XBYTE c);

    XDWORD              table[256];
    XBUFFER*            resulttmp;
};


/*---- INLINE FUNCTIONS ------------------------------------------------------------------*/

#endif

