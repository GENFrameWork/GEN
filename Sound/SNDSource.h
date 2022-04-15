/**-------------------------------------------------------------------------------------------------------------------
*
* @file       SNDSource.h
*
* @class      SNDSOURCE
* @brief      Sound source class
* @ingroup    SOUND
*
* @copyright  GEN Group. All right reserved.
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

#ifndef _SNDSOURCE_H_
#define _SNDSOURCE_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XBase.h"

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


class SNDELEMENT;
class SNDINSTANCE;

class SNDSOURCE
{
  public:

                                    SNDSOURCE                           ();
    virtual                        ~SNDSOURCE                           ();

    void                            SetInstance                         (SNDINSTANCE* instance);
    SNDINSTANCE*                    GetInstance                         ();
    bool                            IsInstancePlaying                   ();

    virtual void                    SetElement                          (SNDELEMENT* element);
    virtual SNDELEMENT*             GetElement                          ();
   
    virtual void                    Stop                                ();
    virtual void                    Pause                               ();
    virtual void                    UnPause                             ();

    virtual void                    SetLoop                             (bool loop);

    virtual bool                    IsPLaying                           ();
    virtual bool                    IsStopped                           ();
    virtual bool                    IsPaused                            ();

    virtual void                    SetVolume                           (float volume);
    virtual float                   GetVolume                           ();

    virtual void                    SetPitch                            (float pitch);
    virtual float                   GetPitch                            ();

    
    virtual void                    SetSecondsOffset                    (float seconds);
    virtual void                    SetSamplesOffset                    (int samples);
   
  protected:

    SNDINSTANCE*                    instance;
    SNDELEMENT*                     element;
    
  private:

    void                            Clean                               ();
    
};



/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif








/*------------------------------------------------------------------------------------------
//  SNDSOURCE.H
*/
/**
// \file
//
//  base class for sources
//
//  ""
//
//  Date Of Creation  : 11/11/2015 10:02:29
//  Last Modification :
*/
/*  GEN  Copyright (C).  All right reserved.
//----------------------------------------------------------------------------------------*/

#ifndef _SNDSOURCE_H_
#define _SNDSOURCE_H_


/*---- INCLUDES --------------------------------------------------------------------------*/


/*---- DEFINES & ENUMS  ------------------------------------------------------------------*/


/*---- CLASS -----------------------------------------------------------------------------*/



/*---- INLINE FUNCTIONS ------------------------------------------------------------------*/

#endif

