/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XSerializationMethod.h
* 
* @class      XSERIALIZATIONMETHOD
* @brief      eXtended Utils Serialization class
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

#ifndef _XSERIALIZATIONMETHOD_H_
#define _XSERIALIZATIONMETHOD_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XString.h"

#pragma endregion


/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/
#pragma region DEFINES_ENUMS


#pragma endregion


/*---- CLASS ---------------------------------------------------------------------------------------------------------*/
#pragma region CLASS


class XBUFFER;
class XVARIANT;       


class XSERIALIZATIONMETHOD
{
  public:
                                   XSERIALIZATIONMETHOD ();
    virtual                       ~XSERIALIZATIONMETHOD ();

    virtual bool                   Add                  (bool var, XCHAR* name = NULL); 
    virtual bool                   Add                  (char var, XCHAR* name = NULL);  
    virtual bool                   Add                  (int var, XCHAR* name = NULL);  
    virtual bool                   Add                  (float var, XCHAR* name = NULL);  
    virtual bool                   Add                  (double var, XCHAR* name = NULL); 
    virtual bool                   Add                  (long var, XCHAR* name = NULL); 
    virtual bool                   Add                  (long long var, XCHAR* name = NULL);  

    virtual bool                   Add                  (XBYTE var, XCHAR* name = NULL);  
    virtual bool                   Add                  (XWORD var, XCHAR* name = NULL);  
    virtual bool                   Add                  (XDWORD var, XCHAR* name = NULL); 
    virtual bool                   Add                  (XQWORD var, XCHAR* name = NULL); 
    
    virtual bool                   Add                  (XSTRING* var, XCHAR* name = NULL);       
    virtual bool                   Add                  (XBUFFER* var, XCHAR* name = NULL);
    virtual bool                   Add                  (XVARIANT* var, XCHAR* name = NULL);       

    virtual bool                   AddStruct            (XCHAR* name = NULL, bool open = false);
    virtual bool                   AddArray             (XDWORD nelements, XCHAR* name = NULL, bool open = false);

    virtual bool                   Extract              (bool& var, XCHAR* name = NULL); 
    virtual bool                   Extract              (char& var, XCHAR* name = NULL);  
    virtual bool                   Extract              (int& var, XCHAR* name = NULL);  
    virtual bool                   Extract              (float& var, XCHAR* name = NULL);  
    virtual bool                   Extract              (double& var, XCHAR* name = NULL); 
    virtual bool                   Extract              (long& var, XCHAR* name = NULL); 
    virtual bool                   Extract              (long long& var, XCHAR* name = NULL);  

    virtual bool                   Extract              (XBYTE& var, XCHAR* name = NULL);  
    virtual bool                   Extract              (XWORD& var, XCHAR* name = NULL);  
    virtual bool                   Extract              (XDWORD& var, XCHAR* name = NULL); 
    virtual bool                   Extract              (XQWORD& var, XCHAR* name = NULL); 
    
    virtual bool                   Extract              (XSTRING& var, XCHAR* name = NULL);       
    virtual bool                   Extract              (XBUFFER& var, XCHAR* name = NULL);
    virtual bool                   Extract              (XVARIANT& var, XCHAR* name = NULL);

    virtual bool                   ExtractStruct        (XCHAR* name = NULL);
    virtual bool                   ExtractArray         (XDWORD nelements, XCHAR* name = NULL);
    
  private:

    void                           Clean                ();   
};


#pragma endregion


/*---- INLINE FUNCTIONS + PROTOTYPES ---------------------------------------------------------------------------------*/
#pragma region FUNCTIONS_PROTOTYPES


#pragma endregion


#endif

