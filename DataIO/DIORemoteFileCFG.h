/**-------------------------------------------------------------------------------------------------------------------
*
* @file       DIORemoteFileCFG.h
*
* @class      DIOREMOTEFILECFG
* @brief      Data Input/Output Remote File CFG class
* @ingroup    DATAIO
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @copyright  Copyright(c) 2008 - 2016 GEN Group.
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
*---------------------------------------------------------------------------------------------------------------------*/

#ifndef _DIOREMOTEFILECFG_H_
#define _DIOREMOTEFILECFG_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XFileCFG.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

#define DIOREMOTEFILECFG_PREFIXNAMEFILE    __L("remote_")

#define DIOREMOTEFILECFG_SECTIONGENERAL    __L("general")
#define DIOREMOTEFILECFG_URLREMOTECFG      __L("urlremotecfg")

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/

class DIOURL;
class DIOWEBCLIENT;

class DIOREMOTEFILECFG : public XFILECFG
{
  public:
                                        DIOREMOTEFILECFG          (XCHAR* namefile = NULL);
    virtual                            ~DIOREMOTEFILECFG          ();

    virtual bool                        Ini                       ();
    XSTRING*                            GetURLRemoteCFG           ();

  protected:

    XSTRING                             URLremoteCFG;

  private:

    void                                Clean                     ();

    DIOWEBCLIENT*                       webclient;
};


/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif
