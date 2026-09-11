/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       Script_Cache.cpp
* 
* @class      SCRIPT_CACHE
* @brief      Script Cache class
* @ingroup    SCRIPT
* 
* @copyright  EndoraSoft. All rights reserved.
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"



/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "Script_Cache.h"

#include "XBuffer.h"
#include "XDir.h"

#include "HashCRC32.h"

#include "Script.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

SCRIPT_CACHE*  SCRIPT_CACHE::instance = NULL;



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_CACHE::GetIsInstanced()
* @brief      Get is instanced
* @ingroup    SCRIPT
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_CACHE::GetIsInstanced()
{
  return instance!=NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SCRIPT_CACHE& SCRIPT_CACHE::GetInstance()
* @brief      Get instance
* @ingroup    SCRIPT
* 
* @return     SCRIPT_CACHE& : Reference to the requested object.
* 
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_CACHE& SCRIPT_CACHE::GetInstance()
{
  if(!instance) instance = GEN_NEW SCRIPT_CACHE();
  return (*instance);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_CACHE::DelInstance()
* @brief      Del instance
* @ingroup    SCRIPT
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_CACHE::DelInstance()
{
  if(!instance) return false;

  GEN_DELETE instance;
  instance = NULL;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD SCRIPT_CACHE::GenerateID(XSTRING& stringID)
* @brief      Generate ID
* @ingroup    SCRIPT
* 
* @param[in]  stringID : String ID value.
* 
* @return     XDWORD : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD SCRIPT_CACHE::GenerateID(XSTRING& stringID)
{
  HASHCRC32 crc32;
  XBUFFER   buffer;

  buffer.Add(stringID);

  crc32.Do(buffer);

  XDWORD CRC32 = crc32.GetResultCRC32();

  return CRC32;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_CACHE::GenerateListKey(XVECTOR<XSTRING*>* namescripts, XSTRING& stringID)
* @brief      Generate a canonical cache key for an ordered list of scripts
* @ingroup    SCRIPT
* 
* @param[in]  namescripts : Script names in execution order.
* @param[out] stringID : Canonical cache key.
* 
* @return     bool : true if the key was generated; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_CACHE::GenerateListKey(XVECTOR<XSTRING*>* namescripts, XSTRING& stringID)
{
  stringID.Empty();

  if(!namescripts)
    {
      return false;
    }

  if(!namescripts->GetSize())
    {
      return false;
    }

  stringID.Format(__L("bundle|%u"), namescripts->GetSize());

  for(XDWORD c=0; c<namescripts->GetSize(); c++)
    {
      XSTRING* namescript = namescripts->Get(c);
      if(!namescript)
        {
          stringID.Empty();
          return false;
        }

      XSTRING namescriptnormalized;

      namescriptnormalized = namescript->Get();
      SCRIPT::EliminateExtraChars(&namescriptnormalized);

      if(namescriptnormalized.IsEmpty())
        {
          stringID.Empty();
          return false;
        }

      XPATH xpathscript;
      if(!SCRIPT::ResolvePathInScriptsRoot(namescriptnormalized.Get(), xpathscript))
        {
          stringID.Empty();
          return false;
        }

      stringID.AddFormat(__L("|%u:"), xpathscript.GetSize());
      stringID += xpathscript.Get();
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_CACHE::Cache_Add(XDWORD ID, XSTRING* script)
* @brief      Cache add
* @ingroup    SCRIPT
* 
* @param[in]  ID : Identifier to use.
* @param[in]  script : Script pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_CACHE::Cache_Add(XDWORD ID, XSTRING* script)
{
  if(!ID)
    {
      return false;
    }

  if(!script)
    {
      return false;
    }

  if(SCRIPT_CACHE::Cache_Get(ID))
    {
      return Cache_Set(ID, script);
    }

  CacheKey_Del(ID);

  XSTRING* _script;   

  _script = GEN_NEW XSTRING();

  if(!_script)
    {
      GEN_DELETE _script;
      return false;
    }
  
  (*_script) = script->Get();

  if(!cache.Add(ID, _script))
    {
      GEN_DELETE _script;
      return false;
    }
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_CACHE::Cache_Add(XDWORD ID, XSTRING* script, XSTRING& stringID)
* @brief      Cache add with collision verification key
* @ingroup    SCRIPT
* 
* @param[in]  ID : Identifier to use.
* @param[in]  script : Script pointer to use.
* @param[in]  stringID : Canonical key associated with the identifier.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_CACHE::Cache_Add(XDWORD ID, XSTRING* script, XSTRING& stringID)
{
  if(stringID.IsEmpty())
    {
      return false;
    }

  if(!Cache_Add(ID, script))
    {
      return false;
    }

  if(!CacheKey_Set(ID, stringID))
    {
      Cache_Del(ID);
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* SCRIPT_CACHE::Cache_Get(XDWORD ID, int* index)
* @brief      Cache get
* @ingroup    SCRIPT
* 
* @param[in]  ID : Identifier to use.
* @param[in]  index : Index value.
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* SCRIPT_CACHE::Cache_Get(XDWORD ID, int* index)
{
  if(!ID)
    {
      return NULL;
    }

  if(index)
    {
      (*index) = -1;
    }

  for(int c=0; c<cache.GetSize(); c++)
    {
      XDWORD _ID = cache.GetKey(c);
      if(_ID)
        {
          if(ID == _ID)
            {
              XSTRING* script = cache.GetElement(c);
              if(index && script)
                {
                  (*index) = c;
                }

              return script;    
            }         
        }
    }

  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XSTRING* SCRIPT_CACHE::Cache_Get(XDWORD ID, XSTRING& stringID, int* index)
* @brief      Cache get with collision verification
* @ingroup    SCRIPT
* 
* @param[in]  ID : Identifier to use.
* @param[in]  stringID : Canonical key associated with the identifier.
* @param[in]  index : Index value.
* 
* @return     XSTRING* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XSTRING* SCRIPT_CACHE::Cache_Get(XDWORD ID, XSTRING& stringID, int* index)
{
  XSTRING* script = Cache_Get(ID, index);
  if(!script)
    {
      return NULL;
    }

  XSTRING* cachekey = cachekeys.Get(ID);
  if(!cachekey || cachekey->Compare(stringID))
    {
      if(index) (*index) = SCRIPT_CACHE_NOTFOUND;
      return NULL;
    }

  return script;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_CACHE::Cache_Set(XDWORD ID, XSTRING* script)
* @brief      Cache set
* @ingroup    SCRIPT
* 
* @param[in]  ID : Identifier to use.
* @param[in]  script : Script pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_CACHE::Cache_Set(XDWORD ID, XSTRING* script)
{
  if(!ID)
    {
      return false;
    }

  if(!script)
    {
      return false;
    }

  int index = 0;

  if(!SCRIPT_CACHE::Cache_Get(ID, &index))
    {
      return false;
    }

  if(index == SCRIPT_CACHE_NOTFOUND)
    {
      return false;
    }

  XSTRING* _script = cache.GetElement(index);
  if(!_script)
    {
      return false;    
    }

  (*_script) = script->Get();
  CacheKey_Del(ID);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_CACHE::CacheKey_Set(XDWORD ID, XSTRING& stringID)
* @brief      Set the canonical key associated with a cache identifier
* @ingroup    SCRIPT
* 
* @param[in]  ID : Identifier to use.
* @param[in]  stringID : Canonical key.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_CACHE::CacheKey_Set(XDWORD ID, XSTRING& stringID)
{
  if(!ID || stringID.IsEmpty())
    {
      return false;
    }

  XSTRING* cachekey = cachekeys.Get(ID);
  if(cachekey)
    {
      (*cachekey) = stringID.Get();
      return true;
    }

  cachekey = GEN_NEW XSTRING();
  if(!cachekey)
    {
      return false;
    }

  (*cachekey) = stringID.Get();

  if(!cachekeys.Add(ID, cachekey))
    {
      GEN_DELETE cachekey;
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_CACHE::CacheKey_Del(XDWORD ID)
* @brief      Delete the canonical key associated with a cache identifier
* @ingroup    SCRIPT
* 
* @param[in]  ID : Identifier to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_CACHE::CacheKey_Del(XDWORD ID)
{
  XSTRING* cachekey = cachekeys.Get(ID);
  if(!cachekey)
    {
      return false;
    }

  if(!cachekeys.Delete(ID))
    {
      return false;
    }

  GEN_DELETE cachekey;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_CACHE::Cache_Del(XDWORD ID)
* @brief      Cache del
* @ingroup    SCRIPT
* 
* @param[in]  ID : Identifier to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_CACHE::Cache_Del(XDWORD ID)
{
  if(!ID)
    {
      return false;
    }
  
  int index = SCRIPT_CACHE_NOTFOUND;

  XSTRING* script = SCRIPT_CACHE::Cache_Get(ID, &index);
  if(!script || index == SCRIPT_CACHE_NOTFOUND)
    {
      return false;
    }

  if(!cache.Delete(ID))
    {
      return false;
    }

  GEN_DELETE script;
  CacheKey_Del(ID);

  return true;
}
    
    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XMAP<XDWORD, XSTRING*>* SCRIPT_CACHE::Cache_GetAll()
* @brief      Cache get
* @ingroup    SCRIPT
* 
* @return     XMAP<XDWORD, XSTRING*>* : Pointer to the requested string; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
XMAP<XDWORD, XSTRING*>* SCRIPT_CACHE::Cache_GetAll()
{
  return &cache;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_CACHE::Cache_DelAll()
* @brief      Cache del all
* @ingroup    SCRIPT
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_CACHE::Cache_DelAll()
{
  if(!cache.GetSize() && !cachekeys.GetSize())
    {
      return false;
    }

  cache.DeleteElementContents();
  cache.DeleteAll();

  cachekeys.DeleteElementContents();
  cachekeys.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_CACHE::Cache_AllDirectory(XPATH& xpath)
* @brief      Cache all directory
* @ingroup    SCRIPT
* 
* @param[in]  xpath : Path to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_CACHE::Cache_AllDirectory(XPATH& xpath)
{
  XDIR*         xdir;
  XDIRELEMENT   element;
 
  xdir = GEN_XFACTORY.Create_Dir();
  if(!xdir) 
    {
      return false;
    }
      
  if(xdir->FirstSearch(xpath.Get(), __L("*"), &element))
    {
      do{ SCRIPT_TYPE type = SCRIPT::GetTypeByExtension(element.GetNameFile()->Get());
          if(type != SCRIPT_TYPE_UNKNOWN)
            {  
              SCRIPT* script = SCRIPT::Create(element.GetNameFile()->Get());
              if(script) 
                {
                  XPATH xpathnamefile;

                  xpathnamefile = xpath.Get();
                  xpathnamefile.Slash_Add();  
                  xpathnamefile.Add(element.GetNameFile()->Get());
                    
                  script->Load(xpathnamefile);                                      
                } 

               GEN_DELETE script;
            }
           
        } while(xdir->NextSearch(&element));
    }            
    
  GEN_XFACTORY.Delete_Dir(xdir);

  return true;
}
 

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool SCRIPT_CACHE::Cache_AllList(XVECTOR<XSTRING*>* listscripts)
* @brief      Cache all list
* @ingroup    SCRIPT
* 
* @param[in]  listscripts : Listscripts pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool SCRIPT_CACHE::Cache_AllList(XVECTOR<XSTRING*>* listscripts)
{
  if(!listscripts) 
    {
      return false;
    }

  bool status = false;
                                                                                                     
  for(XDWORD c=0; c<listscripts->GetSize(); c++)
    {  
      XSTRING* linescripts = listscripts->Get(c);    
      if(linescripts)
        {
          if(!linescripts->IsEmpty())
            {
              XVECTOR<XSTRING*> namescripts;

              linescripts->Split(__C(','), namescripts);

              bool pathsvalid = namescripts.GetSize()?true:false;

              for(XDWORD d=0; d<namescripts.GetSize() && pathsvalid; d++)
                {
                  XSTRING* nametovalidate = namescripts.Get(d);
                  XPATH    validatedpath;

                  if(!nametovalidate || !SCRIPT::EliminateExtraChars(nametovalidate) ||
                     !SCRIPT::ResolvePathInScriptsRoot(nametovalidate->Get(), validatedpath))
                    {
                      pathsvalid = false;
                    }
                }

              if(!pathsvalid) status = false;

              XSTRING* namescript = pathsvalid?namescripts.Get(0):NULL;
              if(namescript)
                {
                  SCRIPT* script = SCRIPT::Create(namescript->Get());
                  if(script) 
                    {
                      XDWORD      ID = 0;
                      bool        incache = false;
                      XSTRING     cachekey;
                      bool        cachekeyvalid = GEN_SCRIPT_CACHE.GenerateListKey(&namescripts, cachekey);

                      if(cachekeyvalid)
                        {
                          ID = GEN_SCRIPT_CACHE.GenerateID(cachekey);
                        }

                      #ifdef SCRIPT_CACHE_ACTIVE

                      XSTRING* _script = cachekeyvalid?GEN_SCRIPT_CACHE.Cache_Get(ID, cachekey):NULL;
                      if(_script)
                        {                         
                          (*script->GetScript()) += _script->Get();      

                          incache = true;
                          status  = !_script->IsEmpty();
                        }

                      #endif

                      if(!incache)
                        {
                          for(XDWORD d=0; d<namescripts.GetSize(); d++)
                            {  
                              namescript = namescripts.Get(d);
                              if(namescript)
                                {                          
                                  XPATH xpath;   

                                  if(!SCRIPT::ResolvePathInScriptsRoot(namescript->Get(), xpath))
                                    {
                                      status = false;
                                      break;
                                    }

                                  status = script->LoadAdd(xpath);
                                  if(!status)  
                                    {
                                      break;
                                    }  
                                }
                            }  
                        }
          
                      #ifdef SCRIPT_CACHE_ACTIVE
                      if(status && !incache && cachekeyvalid)
                        {                          
                          GEN_SCRIPT_CACHE.Cache_Add(ID, script->GetScript(), cachekey);
                        }
                      #endif                                          
                      
                      GEN_DELETE script;
                      script = NULL;

                    }                    
                }

              namescripts.DeleteContents();
              namescripts.DeleteAll();              
            
            } 
        } 
    }
  
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SCRIPT_CACHE::SCRIPT_CACHE()
* @brief      Constructor of class
* @ingroup    SCRIPT
* 
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_CACHE::SCRIPT_CACHE()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         SCRIPT_CACHE::~SCRIPT_CACHE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    SCRIPT
* 
* --------------------------------------------------------------------------------------------------------------------*/
SCRIPT_CACHE::~SCRIPT_CACHE()
{
  Cache_DelAll();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void SCRIPT_CACHE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    SCRIPT
* 
* --------------------------------------------------------------------------------------------------------------------*/
void SCRIPT_CACHE::Clean()
{

}
