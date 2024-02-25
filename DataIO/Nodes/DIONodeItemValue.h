/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIONodeItemValue.h
* 
* @class      DIONODEITEMVALUE
* @brief      Data Input/Output Node Item Value class
* @ingroup    DATAIO
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

#ifndef _DIONODEITEMVALUE_H_
#define _DIONODEITEMVALUE_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XVariant.h"
#include "XSerializable.h"

#include "DIONodeItemValueUnitFormat.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS

enum DIONODEITEMVALUE_MODE
{
  DIONODEITEMVALUE_MODE_NONE              = 0 ,
  DIONODEITEMVALUE_MODE_READ              = 1 ,
  DIONODEITEMVALUE_MODE_WRITE             = 2 ,
  DIONODEITEMVALUE_MODE_READWRITE         = 3 
};


enum DIONODEITEMVALUE_TYPE
{
  DIONODEITEMVALUE_TYPE_UNKNOWN           = 0 ,

  DIONODEITEMVALUE_TYPE_TEMPERATURE           ,
  DIONODEITEMVALUE_TYPE_HUMIDITY              ,
  DIONODEITEMVALUE_TYPE_MOTIONDETECT          ,
  DIONODEITEMVALUE_TYPE_LIGHTSWITCH           ,
  DIONODEITEMVALUE_TYPE_LIGHTLEVEL            ,

  DIONODEITEMVALUE_TYPE_OWNER                 ,
};

#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS

class XDATETIME;

class DIONODEITEMVALUE : public XSERIALIZABLE
{
  public:
                                        DIONODEITEMVALUE          ();
    virtual                            ~DIONODEITEMVALUE          ();

    XDWORD                              GetType                   ();
    void                                SetType                   (XDWORD type);

    virtual bool                        GetDescription            (XSTRING& description);

    DIONODEITEMVALUE_MODE               GetMode                   ();
    void                                SetMode                   (DIONODEITEMVALUE_MODE mode);
    void                                GetModeString             (XSTRING& modestring);

    XVARIANT*                           GetDateTimeLastUpdate     ();
    XVARIANT*                           GetValue                  ();
    XVARIANT*                           GetMinValue               ();
    XVARIANT*                           GetMaxValue               ();
    XVARIANT*                           GetDifferenceForChange    ();

    bool                                ValueHasChanged           ();
    void                                SetValueHasChanged        (bool haschanged);

    DIONODEITEMVALUEUNITFORMAT*         GetUnitFormat             ();

    bool                                ReadUpdateDateTime        ();  

    virtual bool                        Serialize                 ();                                          
    virtual bool                        Deserialize               ();

  private:

    void                                Clean                     ();

    XDWORD                              type;

    DIONODEITEMVALUE_MODE               mode;
    XVARIANT                            datetimelastupdate;
    XVARIANT                            value; 
    XVARIANT                            minvalue;
    XVARIANT                            maxvalue;    
    XVARIANT                            differenceforchange;
    bool                                haschanged;
    DIONODEITEMVALUEUNITFORMAT          unitformat;
    
    XDATETIME*                          datetime;
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

