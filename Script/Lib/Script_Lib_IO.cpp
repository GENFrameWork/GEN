/**-------------------------------------------------------------------------------------------------------------------
*
* @file       Script_Lib_IO.cpp
*
* @class      SCRIPT_LIB_IO
* @brief      Script Library IO (input, output: Printf, Put, XTRACE_PRINTCOLOR, ...)
* @ingroup    SCRIPT
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XTrace.h"
#include "XFactory.h"
#include "XConsole.h"

#include "Script.h"
#include "Script_Lib_IO.h"

#include "XMemory_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         SCRIPT_LIB_IO::SCRIPT_LIB_IO()
* @brief      Constructor
* @ingroup    SCRIPT
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_LIB_IO::SCRIPT_LIB_IO() : SCRIPT_LIB(SCRIPT_LIB_NAME_IO)
{
  Clean();

  GEN_XFACTORY_CREATE(console, CreateConsole())
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         SCRIPT_LIB_IO::~SCRIPT_LIB_IO()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    SCRIPT
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_LIB_IO::~SCRIPT_LIB_IO()
{
  if(console) GEN_XFACTORY.DeleteConsole(console);

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool SCRIPT_LIB_IO::AddLibraryFunctions(SCRIPT* script)
* @brief      AddLibraryFunctions
* @ingroup    SCRIPT
*
* @param[in]  script :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_LIB_IO::AddLibraryFunctions(SCRIPT* script)
{
  if(!script) return false;

  this->script = script;

  script->AddLibraryFunction(this, __L("GetChar")                  , Call_GetChar);
  script->AddLibraryFunction(this, __L("PutChar")                  , Call_PutChar);

  script->AddLibraryFunction(this, __L("SPrintf")                  , Call_SPrintf);
  script->AddLibraryFunction(this, __L("Printf")                   , Call_Printf);

  script->AddLibraryFunction(this, __L("XTRACE_PRINTCOLOR")        , Call_XTRACE_PRINTCOLOR);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XCONSOLE* SCRIPT_LIB_IO::GetConsole()
* @brief      GetConsole
* @ingroup    SCRIPT
*
* @return     XCONSOLE* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XCONSOLE* SCRIPT_LIB_IO::GetConsole()
{
  if(!console) 
    {
      GEN_XFACTORY_CREATE(console, CreateConsole())
    }
    
  if(!console) return NULL;

  return console;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void SCRIPT_LIB_IO::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    SCRIPT
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void SCRIPT_LIB_IO::Clean()
{
  console = NULL;
}


/*--------------------------------------------------------------------------------------------------------------------*/
/* Library Functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void Call_GetChar(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
* @brief      Call_GetChar
* @ingroup    SCRIPT
*
* @param[in]  library :
* @param[in]  script :
* @param[in]  params :
* @param[in]  returnvalue :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void Call_GetChar(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
{
  if(!library)      return;
  if(!script)       return;
  if(!params)       return;
  if(!returnvalue)  return;

  XCONSOLE* console = ((SCRIPT_LIB_IO*)library)->GetConsole();
  if(!console) return;

  (*returnvalue) = console->GetChar();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void Call_PutChar(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
* @brief      Call_PutChar
* @ingroup    SCRIPT
*
* @param[in]  library :
* @param[in]  script :
* @param[in]  params :
* @param[in]  returnvalue :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void Call_PutChar(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
{
  if(!library)      return;
  if(!script)       return;
  if(!params)       return;
  if(!returnvalue)  return;

  returnvalue->Set();

  if(!params->GetSize())
    {
      script->HaveError(SCRIPT_ERRORCODE_INSUF_PARAMS);
      return;
    }

  XCHAR character = (*params->Get(0));

  XCONSOLE* console = ((SCRIPT_LIB_IO*)library)->GetConsole();
  if(!console) return;

  console->Printf(__L("%c"), character);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void Call_SPrintf(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
* @brief      Call_SPrintf
* @ingroup    SCRIPT
*
* @param[in]  library :
* @param[in]  script :
* @param[in]  params :
* @param[in]  returnvalue :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void Call_SPrintf(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
{
  if(!library)      return;
  if(!script)       return;
  if(!params)       return;
  if(!returnvalue)  return;

  returnvalue->Set();

  if(!params->GetSize())
    {
      script->HaveError(SCRIPT_ERRORCODE_INSUF_PARAMS);
      return;
    }

  XVARIANT  variantout = (*params->Get(0));
  XCHAR*    out = variantout;
  XSTRING  _out = out;

  XVARIANT  variantmask = (*params->Get(1));
  XCHAR*    mask = variantmask;

  XSTRING outstring;
  XSTRING string;

  int paramindex = 2;
  int c          = 0;

  while(mask[c])
    {
      switch(mask[c])
        {
          case '%' : {
                        #define MAXTEMPOSTR 32

                        XCHAR param[MAXTEMPOSTR];

                        int  nparam = 1;
                        bool end    = false;

                        memset(param,0,MAXTEMPOSTR*sizeof(XCHAR));
                        param[0] = '%';

                        c++;

                        switch(mask[c])
                              {
                                case __C('c')   :
                                case __C('C')   :
                                case __C('d')   :
                                case __C('i')   :
                                case __C('o')   :
                                case __C('u')   :
                                case __C('x')   :
                                case __C('X')   : { int value = 0;
                                                    library->GetParamConverted(params->Get(paramindex), value);
                                                    string.Format(param, value);
                                                    paramindex++;
                                                    end  = true;
                                                  }
                                                  break;

                                case __C('f')   : { float value = 0;
                                                    library->GetParamConverted(params->Get(paramindex), value);
                                                    string.Format(param, value);
                                                    paramindex++;
                                                    end  = true;
                                                  }
                                                  break;

                                case __C('g')   :
                                case __C('G')   :

                                case __C('e')   :
                                case __C('E')   :

                                case __C('n')   :
                                case __C('p')   : end = true;
                                                  break;

                                case __C('s')   :
                                case __C('S')   : { XVARIANT variantparam = (*params->Get(paramindex));
                                                    paramindex++;
                                                    string.Format(param,(XCHAR*)variantparam);
                                                    end = true;
                                                  }
                                                  break;

                                case __C('%')   : string = __L("%");
                                                  end = true;
                                                  break;

                                case __C('\0')  : end = true;
                                                  break;

                                      default   : break;
                              }

                      }
                      break;

            default : string.Set(mask[c]);
                      c++;
                      break;
        }

      outstring += string;
    }

  _out = outstring;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void Call_Printf(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
* @brief      Call_Printf
* @ingroup    SCRIPT
*
* @param[in]  library :
* @param[in]  script :
* @param[in]  params :
* @param[in]  returnvalue :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void Call_Printf(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
{
  if(!library)      return;
  if(!script)       return;
  if(!params)       return;
  if(!returnvalue)  return;

  returnvalue->Set();

  if(!params->GetSize())
    {
      script->HaveError(SCRIPT_ERRORCODE_INSUF_PARAMS);
      return;
    }

  XVARIANT  variantmask = (*params->Get(0));
  XCHAR*    mask = variantmask;
  XSTRING   outstring;
  XSTRING   string;

  int paramindex = 1;
  int c          = 0;

  if(!mask) return;

  while(mask[c])
    {
      switch(mask[c])
        {
          case '%' : {
                        #define MAXTEMPOSTR 32

                        XCHAR param[MAXTEMPOSTR];

                        int  nparam = 1;
                        bool end    = false;

                        memset(param,0,MAXTEMPOSTR*sizeof(XCHAR));
                        param[0] = '%';

                        c++;

                        do{ string.Empty();

                            param[nparam] = mask[c];
                            nparam++;

                            switch(mask[c])
                              {
                                case __C('c')   :
                                case __C('C')   :
                                case __C('d')   :
                                case __C('i')   :
                                case __C('o')   :
                                case __C('u')   :
                                case __C('x')   :
                                case __C('X')   : { int value = 0;
                                                    library->GetParamConverted(params->Get(paramindex), value);
                                                    string.Format(param, value);
                                                    paramindex++;
                                                    end  = true;
                                                  }
                                                  break;

                                case __C('f')   : { float value = 0;
                                                    library->GetParamConverted(params->Get(paramindex), value);
                                                    string.Format(param, value);
                                                    paramindex++;
                                                    end  = true;
                                                  }
                                                  break;

                                case __C('g')   :
                                case __C('G')   :

                                case __C('e')   :
                                case __C('E')   :

                                case __C('n')   :
                                case __C('p')   : end = true;
                                                  break;

                                case __C('s')   :
                                case __C('S')   : { XVARIANT variantparam = (*params->Get(paramindex));
                                                    paramindex++;
                                                    string.Format(param,(XCHAR*)variantparam);
                                                    end = true;
                                                  }
                                                  break;

                                case __C('%')   : string = __L("%");
                                                  end = true;
                                                  break;

                                case __C('\0')  : end = true;
                                                  break;

                                      default   : break;
                              }

                            c++;

                          } while(!end);
                      }
                      break;

            default : string.Set(mask[c]);
                      c++;
                      break;
        }

      outstring += string;
    }

  XCONSOLE* console = ((SCRIPT_LIB_IO*)library)->GetConsole();
  if(!console) return;

  console->Printf(outstring.Get());
}





/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void Call_XTRACE_PRINTCOLOR(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
* @brief      Call_XTRACE_PRINTCOLOR
* @ingroup    SCRIPT
*
* @param[in]  library :
* @param[in]  script :
* @param[in]  params :
* @param[in]  returnvalue :
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void Call_XTRACE_PRINTCOLOR(SCRIPT_LIB* library, SCRIPT* script, XVECTOR<XVARIANT*>* params, XVARIANT* returnvalue)
{
  if(!library)      return;
  if(!script)       return;
  if(!params)       return;
  if(!returnvalue)  return;

  returnvalue->Set();

  if(!params->GetSize())
    {
      script->HaveError(SCRIPT_ERRORCODE_INSUF_PARAMS);
      return;
    }

  // first param is the color
  XDWORD    color       = 0;
  library->GetParamConverted(params->Get(0), color);

  XVARIANT  variantmask = (*params->Get(1));
  XCHAR*    mask = variantmask;
  XSTRING   outstring;
  XSTRING   string;

  int paramindex = 2;
  int c          = 0;

  if(!mask) return;

  while(mask[c])
    {
      switch(mask[c])
        {
          case '%' : {
                        #define MAXTEMPOSTR 32

                        XCHAR param[MAXTEMPOSTR];

                        int  nparam = 1;
                        bool end    = false;

                        memset(param,0,MAXTEMPOSTR*sizeof(XCHAR));
                        param[0] = '%';

                        c++;

                        do{ string.Empty();

                            param[nparam] = mask[c];
                            nparam++;

                            switch(mask[c])
                              {
                                case __C('c')   :
                                case __C('C')   :
                                case __C('d')   :
                                case __C('i')   :
                                case __C('o')   :
                                case __C('u')   :
                                case __C('x')   :
                                case __C('X')   : { int value = 0;
                                                    library->GetParamConverted(params->Get(paramindex), value);
                                                    string.Format(param, value);
                                                    paramindex++;
                                                    end  = true;
                                                  }
                                                  break;

                                case __C('f')   : { float value = 0.0f;
                                                    library->GetParamConverted(params->Get(paramindex), value);
                                                    string.Format(param, value);
                                                    paramindex++;
                                                    end  = true;
                                                  }
                                                  break;

                                case __C('g')   :
                                case __C('G')   :

                                case __C('e')   :
                                case __C('E')   :

                                case __C('n')   :
                                case __C('p')   : end = true;
                                                  break;

                                case __C('s')   :
                                case __C('S')   : { XVARIANT variantparam = (*params->Get(paramindex));
                                                    paramindex++;
                                                    string.Format(param,(XCHAR*)variantparam);
                                                    end = true;
                                                  }
                                                  break;

                                case __C('%')   : string = __L("%");
                                                  end = true;
                                                  break;

                                case __C('\0')  : end = true;
                                                  break;

                                      default   : break;
                              }

                            c++;

                          } while(!end);
                      }
                      break;

            default : string.Set(mask[c]);
                      c++;
                      break;
        }

      outstring += string;
    }

 XTRACE_PRINTCOLOR(color, outstring.Get());
}



