/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XLINUXThread.cpp
*
* @class      XLINUXTHREAD
* @brief      eXtended LINUX Thread and Mutex class
* @ingroup    PLATFORM_LINUX
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

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>

#include "XTrace.h"

#include "XLINUXThread.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XLINUXMUTEX::XLINUXMUTEX()
* @brief      Constructor
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XLINUXMUTEX::XLINUXMUTEX()
{
  Clean();

  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

  if(!pthread_mutex_init(&mutex, &attr)) init = true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XLINUXMUTEX::~XLINUXMUTEX()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XLINUXMUTEX::~XLINUXMUTEX()
{
  if(init)
    {
      pthread_mutex_destroy(&mutex);
      init = false;
    }

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXMUTEX::Lock()
* @brief      Lock
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XLINUXMUTEX::Lock()
{
  if(!init)                      return false;

  if(pthread_mutex_lock(&mutex)) return false;

  islock = true;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXMUTEX::UnLock()
* @brief      UnLock
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XLINUXMUTEX::UnLock()
{
  if(!init)                        return false;
  if(pthread_mutex_unlock(&mutex)) return false;

  islock = false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XLINUXMUTEX::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XLINUXMUTEX::Clean()
{
  init = false;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* LINUX THREAD class
*
*---------------------------------------------------------------------------------------------------------------------*/




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XLINUXTHREAD::XLINUXTHREAD(XTHREADGROUPID groupID, XCHAR* ID, XTHREADFUNCTION function, void* data)
* @brief      Constructor
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  groupID : group ID
* @param[in]  ID : thread ID
* @param[in]  function : callbak fuction
* @param[in]  param : param of the callback
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XLINUXTHREAD::XLINUXTHREAD(XTHREADGROUPID groupID, XCHAR* ID, XTHREADFUNCTION function, void* param): XTHREAD(groupID, ID, function, param)
{
  Clean();
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XLINUXTHREAD::~XLINUXTHREAD()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XLINUXTHREAD::~XLINUXTHREAD()
{
  End();

  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXTHREAD::Ini(bool run)
* @brief      Ini
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  run : if run the thread directly
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XLINUXTHREAD::Ini(bool run)
{
  statusfunc = XTHREADSTATUS_STOP;

  pthread_attr_t attr;

  if(!stacksize) SetStackSize(1024*1024);

  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, stacksize);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  int _status = pthread_create(&thhandle, &attr, Callback, this);
  if(!_status)
    {
      XSTRING_CREATEOEM(ID, charstr)
      if(ID.GetSize()>=16) charstr[15] = 0;
      //pthread_setname_np(thhandle, charstr);
      XSTRING_DELETEOEM(ID, charstr)

      if(run)
        {
          if(!Run(true)) return false;
        }
       else
        {
          statusfunc = XTHREADSTATUS_STOP;
        }

      pthread_attr_destroy(&attr);
    }
   else
    {
      thhandle = 0;
      // If pthread_create() fails, no new thread is created and the contents of the location referenced by thread are UNDEFINED.
    }

  return (!_status)?true:false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXTHREAD::Wait(int miliseconds)
* @brief      Wait
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  miliseconds :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XLINUXTHREAD::Wait(int miliseconds)
{
  usleep(1000*(miliseconds?miliseconds:1));

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXTHREAD::End()
* @brief      End
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XLINUXTHREAD::End()
{
  if(statusfunc == XTHREADSTATUS_END) return false;

  statusfunc = XTHREADSTATUS_EXIT;

  WaitToEnd();

  if(thhandle)
    {
      pthread_join(thhandle, NULL);
      thhandle = 0;
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XLINUXTHREAD::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XLINUXTHREAD::Clean()
{
  thhandle = 0;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void* XLINUXTHREAD::Callback(void* thread)
* @brief      Callback
* @ingroup    PLATFORM_LINUX
*
* @author     Abraham J. Velez
* @date       01/03/2016 12:00
*
* @param[in]  thread : thread class
*
* @return     void* : NULL always
*
*---------------------------------------------------------------------------------------------------------------------*/
void* XLINUXTHREAD::Callback(void* thread)
{
  XLINUXTHREAD*    th   = NULL;
  XTHREADFUNCTION  func = NULL;
  void*            param;

  th = (XLINUXTHREAD*)thread;
  if(!th) return NULL;

  func = (XTHREADFUNCTION)th->GetFunction();
  if(!func) return NULL;

  param = th->GetParam();

  th->gotofunction = true;

  if((th->priority != XTHREADPRIORITY_UNKNOWN) &&
     (th->priority != XTHREADPRIORITY_NORMAL))
    {
      struct sched_param param;    // scheduling priority

      int policy = SCHED_FIFO;     // scheduling policy

      switch(th->priority)
        {
          #ifdef SCHED_IDLE
          case XTHREADPRIORITY_LOW      : policy = SCHED_IDLE;
                                          param.sched_priority = sched_get_priority_min(policy);
                                          break;
          #endif

          case XTHREADPRIORITY_HIGH     : policy = SCHED_FIFO;
                                          param.sched_priority = sched_get_priority_max(policy)/2;
                                          break;

          case XTHREADPRIORITY_REALTIME : policy = SCHED_FIFO;
                                          param.sched_priority = sched_get_priority_max(policy);
                                          break;
        }

      pthread_setschedparam(th->thhandle, policy, &param);
    }

  while(1)
    {
      if(!th) break;

      if(th->statusfunc == XTHREADSTATUS_EXIT)  break;
      if(th->statusfunc == XTHREADSTATUS_END)   break;

      if(th->statusfunc == XTHREADSTATUS_RUN)
        {
          if(func)
            {
              th->isinfunction = true;
              func(param);
              th->isinfunction = false;

            } else break;
        }

      if(!th) break;

      if(th->waityield) usleep(th->waityield*1000);
    }

  if(th) th->statusfunc = XTHREADSTATUS_END;

  pthread_exit(0);

  return NULL;
}


