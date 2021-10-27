/*------------------------------------------------------------------------------------------
//  DIOLINUXSTREAMSPI.H
*/
/**
// \class
//
//  Data Input/Output Stream SPI class
//
//  @author  Abraham J. Velez
//  @version 20/09/2012 14:31:53
*/
/*  GEN  Copyright (C).  All right reserved.
//----------------------------------------------------------------------------------------*/

#ifndef _DIOLINUXSTREAMSPI_H_
#define _DIOLINUXSTREAMSPI_H_


/*---- INCLUDES --------------------------------------------------------------------------*/

#include "XBuffer.h"
#include "XLINUXThread.h"

#include "DIOStreamSPI.h"


/*---- DEFINES & ENUMS  ------------------------------------------------------------------*/



/*---- CLASS -----------------------------------------------------------------------------*/

class XBUFFER;
class XFACTORY;


class DIOLINUXSTREAMSPI : public DIOSTREAMSPI
{
  public:
                                  DIOLINUXSTREAMSPI                   ();
    virtual                      ~DIOLINUXSTREAMSPI                   ();

    bool                          Open                                ();
    bool                          Close                               ();
    bool                          TransferBuffer                      (XBYTE* bufferread, XBYTE* bufferwrite, XDWORD size);

    void                          Sleep                               (int count);

  private:

    void                          Clean                               ();

    int                           handle;
};


//---- INLINE FUNCTIONS --------------------------------------------------------------------

#endif

