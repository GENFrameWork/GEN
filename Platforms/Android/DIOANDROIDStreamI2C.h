/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIOANDROIDStreamI2C.h
* 
* @class      DIOANDROIDSTREAMI2C
* @brief      ANDROID Data Input/Output Stream I2C class
* @ingroup    PLATFORM_ANDROID
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

#ifndef _DIOANDROIDSTREAMI2C_H_
#define _DIOANDROIDSTREAMI2C_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XFSMachine.h"

#include "DIOStreamI2C.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#define DIOANDROIDSTREAMI2C_MSG_TEN                         0x10
#define DIOANDROIDSTREAMI2C_MSG_RD                          0x01
#define DIOANDROIDSTREAMI2C_MSG_NOSTART                     0x4000
#define DIOANDROIDSTREAMI2C_MSG_REV_DIR_ADDR                0x2000
#define DIOANDROIDSTREAMI2C_MSG_SGIGNORE_NAK                0x1000
#define DIOANDROIDSTREAMI2C_MSG_NO_RD_ACK                   0x0800

#define DIOANDROIDSTREAMI2C_FUNC_I2C                        0x00000001
#define DIOANDROIDSTREAMI2C_FUNC_10BIT_ADDR                 0x00000002
#define DIOANDROIDSTREAMI2C_FUNC_PROTOCOL_MANGLING          0x00000004  // I2C_M_{REV_DIR_ADDR,NOSTART,..}
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_PEC                  0x00000008
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_BLOCK_PROC_CALL      0x00008000  // SMBus 2.0
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_QUICK                0x00010000
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_READ_BYTE            0x00020000
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_WRITE_BYTE           0x00040000
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_READ_BYTE_TICKET     0x00080000
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_WRITE_BYTE_TICKET    0x00100000
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_READ_WORD_TICKET     0x00200000
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_WRITE_WORD_TICKET    0x00400000
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_PROC_CALL            0x00800000
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_READ_BLOCK_TICKET    0x01000000
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_WRITE_BLOCK_TICKET   0x02000000
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_READ_I2C_BLOCK       0x04000000  // I2C-like block xfer
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_WRITE_I2C_BLOCK      0x08000000  // w/ 1-byte reg. addr.

#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_BYTE                 (DIOANDROIDSTREAMI2C_FUNC_SMBUS_READ_BYTE       | DIOANDROIDSTREAMI2C_FUNC_SMBUS_WRITE_BYTE)
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_BYTE_TICKET          (DIOANDROIDSTREAMI2C_FUNC_SMBUS_READ_BYTE_TICKET  | DIOANDROIDSTREAMI2C_FUNC_SMBUS_WRITE_BYTE_TICKET)
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_WORD_TICKET          (DIOANDROIDSTREAMI2C_FUNC_SMBUS_READ_WORD_TICKET  | DIOANDROIDSTREAMI2C_FUNC_SMBUS_WRITE_WORD_TICKET)
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_BLOCK_TICKET         (DIOANDROIDSTREAMI2C_FUNC_SMBUS_READ_BLOCK_TICKET | DIOANDROIDSTREAMI2C_FUNC_SMBUS_WRITE_BLOCK_TICKET)
#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_I2C_BLOCK            (DIOANDROIDSTREAMI2C_FUNC_SMBUS_READ_I2C_BLOCK  | DIOANDROIDSTREAMI2C_FUNC_SMBUS_WRITE_I2C_BLOCK)

#define DIOANDROIDSTREAMI2C_FUNC_SMBUS_HWPEC_CALC           DIOANDROIDSTREAMI2C_FUNC_SMBUS_PEC

#define DIOANDROIDSTREAMI2C_SMBUS_BLOCK_MAX                 32          // As specified in SMBus standard
#define DIOANDROIDSTREAMI2C_SMBUS_I2C_BLOCK_MAX             32          // Not specified but we use same structure

#define DIOANDROIDSTREAMI2C_SMBUS_READ                      1
#define DIOANDROIDSTREAMI2C_SMBUS_WRITE                     0

#define DIOANDROIDSTREAMI2C_SMBUS_QUICK                     0
#define DIOANDROIDSTREAMI2C_SMBUS_BYTE                      1
#define DIOANDROIDSTREAMI2C_SMBUS_BYTE_TICKET               2
#define DIOANDROIDSTREAMI2C_SMBUS_WORD_TICKET               3
#define DIOANDROIDSTREAMI2C_SMBUS_PROC_CALL                 4
#define DIOANDROIDSTREAMI2C_SMBUS_BLOCK_TICKET              5
#define DIOANDROIDSTREAMI2C_SMBUS_I2C_BLOCK_BROKEN          6
#define DIOANDROIDSTREAMI2C_SMBUS_BLOCK_PROC_CALL           7           // SMBus 2.0
#define DIOANDROIDSTREAMI2C_SMBUS_I2C_BLOCK_TICKET          8


#define DIOANDROIDSTREAMI2C_RETRIES                         0x0701      // number of times a device address should be polled when not acknowledging
#define DIOANDROIDSTREAMI2C_TIMEOUT                         0x0702      // set timeout in units of 10 ms

#define DIOANDROIDSTREAMI2C_SLAVE                           0x0703      // Use this slave address
#define DIOANDROIDSTREAMI2C_SLAVE_FORCE                     0x0706      // Use this slave address, even if it is already in use by a driver!
#define DIOANDROIDSTREAMI2C_TENBIT                          0x0704      // 0 for 7 bit addrs, != 0 for 10 bit

#define DIOANDROIDSTREAMI2C_FUNCS                           0x0705      // Get the adapter functionality mask

#define DIOANDROIDSTREAMI2C_RDWR                            0x0707      // Combined R/W transfer (one STOP only)

#define DIOANDROIDSTREAMI2C_PEC                             0x0708      // != 0 to use PEC with SMBus
#define DIOANDROIDSTREAMI2C_SMBUS                           0x0720      // SMBus transfer


typedef struct
{
  XWORD                                     addr;                       // slave address
  XWORD                                     flags;
  XWORD                                     size;                       // msg length
  XBYTE*                                    buffer;                     // pointer to msg data

} DIOANDROIDSTREAMI2C_MSG;


union DIOANDROIDSTREAMI2C_SMBUS_TICKET
{
  XBYTE                                     byte;
  XWORD                                     word;
  XBYTE                                     block[DIOANDROIDSTREAMI2C_SMBUS_BLOCK_MAX+2] ;  // block [0] is used for length + one more for PEC
};


typedef struct
{
  XBYTE                                     read_write;
  XBYTE                                     command;
  XDWORD                                    size;
  union DIOANDROIDSTREAMI2C_SMBUS_TICKET*   data;

} DIOANDROIDSTREAMI2C_SMBUS_IOCTL_TICKET;


typedef struct
{
  DIOANDROIDSTREAMI2C_MSG*                  msgs;
  XDWORD                                    nmsgs;

} DIOANDROIDSTREAMI2C_RDWR_IOCTL_TICKET;


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS



#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


class DIOANDROIDSTREAMI2C : public DIOSTREAMI2C
{
  public:
                                  DIOANDROIDSTREAMI2C                     ();
    virtual                      ~DIOANDROIDSTREAMI2C                     ();

    bool                          Open                                    ();
  //XDWORD                        Read                                    (XBYTE* buffer,XDWORD size);
    bool                          WaitToFilledReadingBuffer               (int filledto = DIOSTREAM_SOMETHINGTOREAD, int timeout = XTIMER_INFINITE);
    bool                          Close                                   ();

  private:

    void                          Clean                                   ();

    static void                   ThreadConnection                        (void* data);

    bool                          I2C_Read                                (XWORD address, XBYTE* buffer, XWORD size);
    bool                          I2C_Write                               (XWORD address, XBYTE* buffer, XWORD size);

    int                           handle;
    XWORD                         sizeread;
};


#pragma endregion


#endif

