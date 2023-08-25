//------------------------------------------------------------------------------------------
//  DIOLINUXSTREAMUARTLOCALENUMDEVICES.H
//
/**
// \class
//
//  LINUX Data Input/Output Stream UART Local Enum Devices class
//
//  ""
//  @version 02/01/2002
*/
//  GEN  Copyright (C).  All right reserved.
//------------------------------------------------------------------------------------------

#ifndef _DIOLINUXSTREAMUARTLOCALENUMDEVICES_H_
#define _DIOLINUXSTREAMUARTLOCALENUMDEVICES_H_

#if defined(DIO_ACTIVE) && defined(DIO_UART_ACTIVE)

//---- INCLUDES ----------------------------------------------------------------------------

#include "XString.h"

#include "DIOStreamUARTLocalEnumDevices.h"

//---- DEFINES & ENUMS  --------------------------------------------------------------------

//---- CLASS -------------------------------------------------------------------------------


class DIOLINUXSTREAMUARTLOCALENUMDEVICES :  public DIOSTREAMUARTLOCALENUMDEVICES
{
  public:
                          DIOLINUXSTREAMUARTLOCALENUMDEVICES          ();
    virtual              ~DIOLINUXSTREAMUARTLOCALENUMDEVICES          ();

    bool                  Search                                      ();
    bool                  IsDeviceAvailable                           (XCHAR* resource);
};


//---- INLINE FUNCTIONS --------------------------------------------------------------------

#endif

#endif



