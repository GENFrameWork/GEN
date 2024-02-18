/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       ANDROIDJNI.cpp
* 
* @class      ANDROIDJNI
* @brief      ANDROID JNI Interface class
* @ingroup    PLATFORM_ANDROID
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

/*---- PRECOMPILATION INCLUDES ----------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "ANDROIDJNI.h"

#include "android_native_app_glue.h"

#include "XTrace.h"

//#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

JavaVM*   ANDROIDJNI::currentjavaVM;
jint      ANDROIDJNI::currentjavaversion;
jobject   ANDROIDJNI::ClassLoader;
jmethodID ANDROIDJNI::FindClassMethod;

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         static void JavaEnvDestructor(void*)
* @brief      oid JavaEnvDestructor
* @ingroup    PLATFORM_ANDROID
* 
* @param[in]  void* : 
* 
* @return     static : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
static void JavaEnvDestructor(void*)
{
  ANDROIDJNI::DetachJNIEnv();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void ANDROIDJNI::InitializeJNI(android_app* application, jint javaversion)
* @brief      InitializeJNI
* @ingroup    PLATFORM_ANDROID
* 
* @param[in]  application : 
* @param[in]  javaversion : 
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void ANDROIDJNI::InitializeJNI(android_app* application, jint javaversion)
{
  currentjavaVM       = application->activity->vm;
  currentjavaversion  = javaversion;

  JNIEnv* env = ANDROIDJNI::GetJNIEnv();
  if(!env)
    {
      return;
    }

  jobject   mainactivity  = application->activity->clazz;
  
  jclass    MainClass             = env->GetObjectClass(mainactivity);
  jclass    classClass            = env->FindClass("java/lang/Class");
  jclass    classLoaderClass      = env->FindClass("java/lang/ClassLoader");
  jmethodID getClassLoaderMethod  = env->GetMethodID(classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
  jobject   classLoader           = env->CallObjectMethod(MainClass, getClassLoaderMethod);

  ClassLoader = env->NewGlobalRef(classLoader);

  FindClassMethod = env->GetMethodID(classLoaderClass, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         JNIEnv* ANDROIDJNI::GetJNIEnv()
* @brief      GetJNIEnv
* @ingroup    PLATFORM_ANDROID
* 
* @return     JNIEnv* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
JNIEnv* ANDROIDJNI::GetJNIEnv()
{
  static XDWORD threadslot = 0;
  if(!threadslot)
    {
      pthread_key_create((pthread_key_t*)&threadslot, &JavaEnvDestructor);
    }

  JNIEnv* env = NULL;

  if(!currentjavaVM)
    {
      XTRACE_PRINTCOLOR(XTRACE_COLOR_RED, __L("[ANDROID] No Current Java VM"));
    }

  jint getenvresult = currentjavaVM->GetEnv((void**)&env, currentjavaversion);
  if(getenvresult == JNI_EDETACHED)
    {
      jint attachresult = currentjavaVM->AttachCurrentThread(&env, NULL);

      if(attachresult == JNI_ERR)
        {
          return NULL;
        }
    }
   else 
    {
      if(getenvresult != JNI_OK)
        {
          return NULL;
        }
    }

  return env;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void ANDROIDJNI::DetachJNIEnv()
* @brief      DetachJNIEnv
* @ingroup    PLATFORM_ANDROID
* 
* @return     void : does not return anything. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void ANDROIDJNI::DetachJNIEnv()
{
  currentjavaVM->DetachCurrentThread();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         jclass ANDROIDJNI::FindJNIClass(const XSTRING & name)
* @brief      FindJNIClass
* @ingroup    PLATFORM_ANDROID
* 
* @param[in]  XSTRING & name : 
* 
* @return     jclass : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
jclass ANDROIDJNI::FindJNIClass(const XSTRING & name)
{
  char* cname = NULL;

  name.CreateOEM(cname);

  return ANDROIDJNI::FindJNIClass(cname);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         jclass ANDROIDJNI::FindJNIClass(XSTRING* name)
* @brief      FindJNIClass
* @ingroup    PLATFORM_ANDROID
* 
* @param[in]  name : 
* 
* @return     jclass : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
jclass ANDROIDJNI::FindJNIClass(XSTRING* name)
{
  return ANDROIDJNI::FindJNIClass(*name);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         jclass ANDROIDJNI::FindJNIClass(XCHAR* name)
* @brief      FindJNIClass
* @ingroup    PLATFORM_ANDROID
* 
* @param[in]  name : 
* 
* @return     jclass : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
jclass ANDROIDJNI::FindJNIClass(XCHAR* name)
{
  return ANDROIDJNI::FindJNIClass(XSTRING(name));
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         jclass ANDROIDJNI::FindJNIClass(const char* name)
* @brief      FindJNIClass
* @ingroup    PLATFORM_ANDROID
* 
* @param[in]  char* name : 
* 
* @return     jclass : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
jclass ANDROIDJNI::FindJNIClass(const char* name)
{
  JNIEnv* env = ANDROIDJNI::GetJNIEnv();
  if(!env)
    {
      return NULL;
    }

  jstring ClassNameObj = env->NewStringUTF(name);
  jclass FoundClass = static_cast<jclass>(env->CallObjectMethod(ClassLoader, FindClassMethod, ClassNameObj));
  ANDROIDJNI::CheckJavaException();
  env->DeleteLocalRef(ClassNameObj);

  return FoundClass;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         jclass ANDROIDJNI::FindJNIEnvClassWrapper(const char* name)
* @brief      FindJNIEnvClassWrapper
* @ingroup    PLATFORM_ANDROID
* 
* @param[in]  char* name : 
* 
* @return     jclass : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
jclass ANDROIDJNI::FindJNIEnvClassWrapper(const char* name)
{
  JNIEnv* env = ANDROIDJNI::GetJNIEnv();
  if(!env)
    {
      return NULL;
    }

  return env->FindClass(name);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool ANDROIDJNI::CheckJavaException()
* @brief      CheckJavaException
* @ingroup    PLATFORM_ANDROID
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool ANDROIDJNI::CheckJavaException()
{
  JNIEnv* env = ANDROIDJNI::GetJNIEnv();
  if(!env)
    {
      return true;
    }

  if(env->ExceptionCheck())
    {
      env->ExceptionDescribe();

      env->ExceptionClear();
      return true;
    }

  return false;
}


#pragma endregion