/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPFactory.h
* 
* @class      GRPFACTORY
* @brief      Graphics Factory class
* @ingroup    GRAPHIC
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

#ifndef _GRPFACTORY_H_
#define _GRPFACTORY_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "GRPProperties.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

#define GRPFACTORY_GENERATECANVAS(newcanvas, canvas)    if(newcanvas)                               \
                                                          {                                         \
                                                            newcanvas->CopyPropertysFrom(screen);   \
                                                            newcanvas->CreateBuffers();             \
                                                            canvas = (GRPCANVAS*)newcanvas;         \
                                                          }

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class GRPSCREEN;
class GRPCONTEXT;
class GRPCANVAS;
class GRPBITMAP;
class GRPDESKTOPMANAGER;

class GRPFACTORY
{
  public:
                                      GRPFACTORY                ();
    virtual                          ~GRPFACTORY                ();

    static bool                       GetIsInstanced            ();
    static GRPFACTORY&                GetInstance               ();
    static bool                       SetInstance               (GRPFACTORY* instance);
    static bool                       DelInstance               ();

    virtual GRPSCREEN*                CreateScreen              ();
    virtual bool                      DeleteScreen              (GRPSCREEN* screen);

    #ifdef GRP_OPENGL_ACTIVE
    virtual GRPCONTEXT*               CreateContext             ();
    virtual bool                      DeleteContext             (GRPCONTEXT* context);
    #endif

    GRPCANVAS*                        CreateCanvas              (GRPPROPERTIES* properties);
    bool                              DeleteCanvas              (GRPCANVAS* canvas);

    GRPBITMAP*                        CreateBitmap              (int width, int height, GRPPROPERTYMODE mode, bool isbufferinverse = false);
    bool                              DeleteBitmap              (GRPBITMAP* bitmap);

    #ifdef GRP_DESKTOPMANAGER_ACTIVE
    virtual GRPDESKTOPMANAGER*        CreateDesktopManager      ();
    virtual bool                      DeleteDesktopManager      (GRPDESKTOPMANAGER* desktopmanager);
    #endif

  private:

    void                              Clean                     ();

    static GRPFACTORY*                instance;
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

