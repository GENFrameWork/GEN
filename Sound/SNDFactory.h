/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       SNDFactory.h
* 
* @class      SNDFACTORY
* @brief      Sound Factory class
* @ingroup    SOUND
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


#ifndef _SNDFACTORY_H_
#define _SNDFACTORY_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XPath.h"
#include "XSubject.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class SNDITEM;
class SNDPLAYCFG;

class SNDFACTORY : public XSUBJECT
{
  public:
                                      SNDFACTORY                  ();
    virtual                          ~SNDFACTORY                  ();

    static bool                       GetIsInstanced              ();
    static SNDFACTORY&                GetInstance                 ();
    static bool                       SetInstance                 (SNDFACTORY* instance);
    static bool                       DelInstance                 ();
    
    virtual bool                      Ini                         ();    
    virtual bool  		    						Update										  ();    
    virtual bool                      End                         ();         
        
    virtual float                     Volume_Get                  ();
    virtual bool                      Volume_Set                  (float volume);    

    virtual bool                      Sound_Play                  (SNDITEM* item, SNDPLAYCFG* playCFG = NULL, XDWORD ntimestoplay = 1);
    virtual bool                      Sound_Pause                 (SNDITEM* item);    
    virtual bool                      Sound_Stop                  (SNDITEM* item);
    virtual bool                      Sound_StopAll               ();  
    virtual bool                      Sound_WaitToEnd             (SNDITEM* item, int maxtimeout = -1);  
    virtual bool                      Sound_WaitAllToEnd          (int maxtimeout = -1);  
 
    SNDITEM*                          CreateItem                  (XCHAR* path);  
    SNDITEM*                          CreateItem                  (XPATH& xpath);     
    SNDITEM*                          CreateItem                  (XDWORD frecuency, XDWORD duration = 500);  

    XVECTOR<SNDITEM*>*                GetItems                    ();
    bool                              DeleteAllItems              ();
    
    bool                              Sound_IsAnyPlaying          ();
    bool                              Sound_IsAnyActive           ();
                
  private:

    void                              Clean                       ();

    static SNDFACTORY*                instance;
    XVECTOR<SNDITEM*>                 sounditems;
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif


