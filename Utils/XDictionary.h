/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XDictionary.h
*
* @class      XDICTIONARY
* @brief      eXtended Diccionary class
* @ingroup    UTILS
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

#ifndef _XDICTIONARY_H_
#define _XDICTIONARY_H_

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <XList.h>

/*---- DEFINES & ENUMS  ----------------------------------------------------------------------------------------------*/

/*---- CLASS ---------------------------------------------------------------------------------------------------------*/


template<class T>
class XDICTIONARYENTRY
{
  public:

    XSTRING             key;
    T                   value;
};



template<class T>
class XDICTIONARY
{
  public:

                                      XDICTIONARY                 ()
                                      {
                                        Clean();
                                      }

                                     ~XDICTIONARY                 ()
                                      {
                                        entries.DeleteContents();
                                        entries.DeleteAll();

                                        Clean();
                                      }

    T&                                operator []                 (int n)
                                      {
                                        return entries[n];
                                      }

    T&                                operator []                 (XCHAR* key)
                                      {
                                        for(XDWORD e=0; e<entries.GetSize(); e++)
                                          {
                                            if(entries.Get(e)->key == key)  return entries.Get(e)->value;
                                          }

                                         entries.Add(new XDICTIONARYENTRY<T>);

                                         ntries.GetLast()->key    = key;
                                         entries.GetLast()->value = T();

                                         return entries.GetLast()->value;
                                      }

    XLIST<XDICTIONARYENTRY<T>*>       entries;

  private:

    void                              Clean         ()
                                      {

                                      }
};



/*---- INLINE FUNCTIONS ----------------------------------------------------------------------------------------------*/

#endif







/*---- INLINE FUNCTIONS ------------------------------------------------------------------*/

#endif