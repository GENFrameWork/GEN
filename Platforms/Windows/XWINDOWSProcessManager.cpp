/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       XWINDOWSProcessManager.cpp
* 
* @class      XWINDOWSPROCESSMANAGER
* @brief      eXtended WINDOWS process manager class
* @ingroup    PLATFORM_WINDOWS
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

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "XWINDOWSProcessManager.h"

#include <tlhelp32.h>
#include <psapi.h> 
#include <tchar.h>
#include <process.h>

#include "XFactory.h"
#include "XFile.h"
#include "XSleep.h"
#include "XTrace.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE


#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XWINDOWSPROCESSMANAGER::XWINDOWSPROCESSMANAGER()
* @brief      Constructor of class
* @ingroup    PLATFORM_WINDOWS
* 
* --------------------------------------------------------------------------------------------------------------------*/
XWINDOWSPROCESSMANAGER::XWINDOWSPROCESSMANAGER() : XPROCESSMANAGER()
{
  Clean();

  xmutexgetsrunninglist = GEN_XFACTORY.Create_Mutex();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWINDOWSPROCESSMANAGER::~XWINDOWSPROCESSMANAGER()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    PLATFORM_WINDOWS
*
* --------------------------------------------------------------------------------------------------------------------*/
XWINDOWSPROCESSMANAGER::~XWINDOWSPROCESSMANAGER()
{
  if(xmutexgetsrunninglist)
    {
       GEN_XFACTORY.Delete_Mutex(xmutexgetsrunninglist);
    }

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSPROCESSMANAGER::MakeSystemCommand(XCHAR* command)
* @brief      Make system command
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  command : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSPROCESSMANAGER::MakeSystemCommand(XCHAR* command)
{
  return (_wsystem(command) == -1)?false:true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSPROCESSMANAGER::MakeCommand(XCHAR* command, XSTRING& out, int* returncode)
* @brief      Make command
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  command :
* @param[in]  out :
* @param[in]  returncode :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSPROCESSMANAGER::MakeCommand(XCHAR* command, XBUFFER* out, int* returncode)
{
  XBYTE  buffer[_MAXBUFFER];
  FILE*  pipe;

  if((pipe = _wpopen(command, __L("rt") )) == NULL) return false;

  if(out)
    {
      memset(buffer, 0, 256);
          
      while(fgets((char*)buffer, _MAXBUFFER, pipe))
        {          
          out->Add((XBYTE*)buffer, SizeBufferASCII(buffer, _MAXBUFFER));
          memset(buffer, 0, _MAXBUFFER);
        }
    }

  if(feof(pipe))
    {
      if(returncode) (*returncode) =  _pclose(pipe);
    }
   else
    {
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XWINDOWSPROCESSMANAGER::OpenURL(XCHAR* url)
* @brief      Open URL
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  url : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSPROCESSMANAGER::OpenURL(XCHAR* url)
{
  //HINSTANCE handle = ShellExecute(NULL, L"open", url ,NULL, NULL, SW_SHOWNORMAL); 
  //if((int)handle <= 32) return false;

  ShellExecute(NULL, L"open", url ,NULL, NULL, SW_SHOWNORMAL); 

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XWINDOWSPROCESSMANAGER::Application_Execute(XCHAR* applicationpath, XCHAR* params, XBUFFER* in, XBUFFER* out, int* returncode)
* @brief      Application execute
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  applicationpath : 
* @param[in]  params : 
* @param[in]  in : 
* @param[in]  out : 
* @param[in]  returncode : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSPROCESSMANAGER::Application_Execute(XCHAR* applicationpath, XCHAR* params, XBUFFER* in, XBUFFER* out, int* returncode)
{
  if(!applicationpath)
    {
      return false;
    }

  if(!applicationpath[0])
    {
      return false;
    }

  #define CMDLINE_SIZE  (10*1024)
  #define OUTBUF_SIZE   (10*1024*1024)
  
  PROCESS_INFORMATION processinfo;
  STARTUPINFOA        startupinfo;
  SECURITY_ATTRIBUTES saattr;
  XBYTE*              cmdline             = NULL;
  XBYTE*              outbuftmp           = NULL;
  DWORD               bytes_read;
  DWORD               exitcode;
  HANDLE              stdhandle_out_read  = NULL;
  HANDLE              stdhandle_out_write = NULL;
  HANDLE              stdhandle_in_read   = NULL;
  HANDLE              stdhandle_in_write  = NULL;
  XSTRING             _command;
  bool                status = false;

  cmdline    = new XBYTE[CMDLINE_SIZE];
  outbuftmp  = new XBYTE[OUTBUF_SIZE];
  
  if(cmdline && outbuftmp)
    {      
      _command.Format(__L("\"%s\""), applicationpath);
      
      if(params) _command.AddFormat(__L(" %s"), params);
      
      XBUFFER charstr;
      
      _command.ConvertToASCII(charstr);
      memcpy(cmdline, charstr.GetPtrChar(), _command.GetSize()+1);
      
      memset(&saattr, 0, sizeof(saattr));
      saattr.nLength              = sizeof(SECURITY_ATTRIBUTES);
      saattr.bInheritHandle       = TRUE;
      saattr.lpSecurityDescriptor = NULL;

      // Create a pipe for the child process's STDOUT.
      if(!CreatePipe(&stdhandle_out_read, &stdhandle_out_write, &saattr, 0))  return false;
      // Ensure the read handle to the pipe for STDOUT is not inherited.
      if(!SetHandleInformation(stdhandle_out_read, HANDLE_FLAG_INHERIT, 0))   return false;

      // Create a pipe for the child process's STDIN.
      if(!CreatePipe(&stdhandle_in_read, &stdhandle_in_write, &saattr, 0))    return false;
      // Ensure the read handle to the pipe for STDIN is not inherited.
      if(!SetHandleInformation(stdhandle_in_write, HANDLE_FLAG_INHERIT, 0))   return false;

      ZeroMemory(&processinfo, sizeof(PROCESS_INFORMATION));

      ZeroMemory(&startupinfo, sizeof(STARTUPINFO));
      startupinfo.cb          = sizeof(startupinfo);
      startupinfo.hStdError   = stdhandle_out_write;
      startupinfo.hStdOutput  = stdhandle_out_write;
      startupinfo.hStdInput   = stdhandle_in_read;    // GetStdHandle(STD_INPUT_HANDLE);
      startupinfo.dwFlags    |= STARTF_USESTDHANDLES;

      if(CreateProcessA(NULL, (char*)cmdline, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startupinfo, &processinfo))
        {
          CloseHandle(stdhandle_out_write);
          
          if(in)
            {
              if(in->GetSize())
                {
                  DWORD bytes_write;

                  WriteFile(stdhandle_in_write, in->Get(), in->GetSize(), &bytes_write, NULL);
                  
                  FlushFileBuffers(stdhandle_in_write);
                }
            }

          CloseHandle(stdhandle_in_write);

          if(out)
            {
              memset(outbuftmp, 0, OUTBUF_SIZE);

              for(;;)
                {
                  if(!ReadFile(stdhandle_out_read, outbuftmp, OUTBUF_SIZE, &bytes_read, NULL))
                    {
                      break;
                    }

                  if(bytes_read > 0)
                    {                    
                      out->Add(outbuftmp, bytes_read);
                    }
                }             
            }

          if(WaitForSingleObject(processinfo.hProcess, (out?INFINITE:500)) == WAIT_OBJECT_0)
            {
              if(out)
                {
                  if(GetExitCodeProcess(processinfo.hProcess, &exitcode))
                    {
                      if(returncode) (*returncode) = exitcode;
                    }

                  CloseHandle(processinfo.hProcess);
                  CloseHandle(processinfo.hThread);
                }

              status = true;
            }
        }
    }

  if(cmdline) 
    {
      delete [] cmdline;
    }

  if(outbuftmp)
    {
      delete [] outbuftmp;
    }
  
  return status;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XWINDOWSPROCESSMANAGER::Application_Execute(XBUFFER* applicationpath, XBUFFER* params, XBUFFER* in, XBUFFER* out, int* returncode)
* @brief      application  execute
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  applicationpath : 
* @param[in]  params : 
* @param[in]  in : 
* @param[in]  out : 
* @param[in]  returncode : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSPROCESSMANAGER::Application_Execute(XBUFFER* applicationpath, XBUFFER* params, XBUFFER* in, XBUFFER* out, int* returncode)
{
  if(!applicationpath)
    {
      return false;
    }

  if(!applicationpath->GetSize())
    {
      return false;
    }

  #define OUTBUF_SIZE   (10*1024*1024)

  PROCESS_INFORMATION processinfo;
  STARTUPINFOW        startupinfo;
  SECURITY_ATTRIBUTES saattr;
  XSTRING             cmdline;
  XBYTE*              outbuftmp           = NULL;
  DWORD               bytes_read;
  DWORD               exitcode;
  HANDLE              stdhandle_out_read  = NULL;
  HANDLE              stdhandle_out_write = NULL;
  HANDLE              stdhandle_in_read   = NULL;
  HANDLE              stdhandle_in_write  = NULL;
  bool                status = false;

  outbuftmp  = new XBYTE[OUTBUF_SIZE];
  
  if(outbuftmp)
    {      
      XSTRING _params;

      cmdline.ConvertFromUTF8((*applicationpath));
      _params.ConvertFromUTF8((*params));

      cmdline.AddFormat(__L(" %s"), _params.Get());
      
      memset(&saattr, 0, sizeof(saattr));
      saattr.nLength              = sizeof(SECURITY_ATTRIBUTES);
      saattr.bInheritHandle       = TRUE;
      saattr.lpSecurityDescriptor = NULL;

      // Create a pipe for the child process's STDOUT.
      if(!CreatePipe(&stdhandle_out_read, &stdhandle_out_write, &saattr, 0))  return false;
      // Ensure the read handle to the pipe for STDOUT is not inherited.
      if(!SetHandleInformation(stdhandle_out_read, HANDLE_FLAG_INHERIT, 0))   return false;

      // Create a pipe for the child process's STDIN.
      if(!CreatePipe(&stdhandle_in_read, &stdhandle_in_write, &saattr, 0))    return false;
      // Ensure the read handle to the pipe for STDIN is not inherited.
      if(!SetHandleInformation(stdhandle_in_write, HANDLE_FLAG_INHERIT, 0))   return false;

      ZeroMemory(&processinfo, sizeof(PROCESS_INFORMATION));

      ZeroMemory(&startupinfo, sizeof(STARTUPINFO));
      startupinfo.cb          = sizeof(startupinfo);
      startupinfo.hStdError   = stdhandle_out_write;
      startupinfo.hStdOutput  = stdhandle_out_write;
      startupinfo.hStdInput   = stdhandle_in_read;    // GetStdHandle(STD_INPUT_HANDLE);
      startupinfo.dwFlags    |= STARTF_USESTDHANDLES;

      if(CreateProcessW(NULL, (LPWSTR)cmdline.Get(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startupinfo, &processinfo))
        {
          CloseHandle(stdhandle_out_write);
          
          if(in)
            {
              if(in->GetSize())
                {
                  DWORD bytes_write;

                  WriteFile(stdhandle_in_write, in->Get(), in->GetSize(), &bytes_write, NULL);
                  
                  FlushFileBuffers(stdhandle_in_write);
                }
            }

          CloseHandle(stdhandle_in_write);

          if(out)
            {
              memset(outbuftmp, 0, OUTBUF_SIZE);

              for(;;)
                {
                  if(!ReadFile(stdhandle_out_read, outbuftmp, OUTBUF_SIZE, &bytes_read, NULL))
                    {
                      break;
                    }

                  if(bytes_read > 0)
                    {                    
                      out->Add(outbuftmp, bytes_read);
                    }
                }             
            }

          if(WaitForSingleObject(processinfo.hProcess, (out?INFINITE:500)) == WAIT_OBJECT_0)
            {
              if(out)
                {
                  if(GetExitCodeProcess(processinfo.hProcess, &exitcode))
                    {
                      if(returncode) (*returncode) = exitcode;
                    }

                  CloseHandle(processinfo.hProcess);
                  CloseHandle(processinfo.hThread);
                }

              status = true;
            }
        }
    }

  if(outbuftmp)
    {
      delete [] outbuftmp;
    }
  
  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSPROCESSMANAGER::Application_IsRunning(XCHAR* applicationname, XDWORD* ID)
* @brief      Application is running
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  applicationname : application name
* @param[out] ID : ID of the application by SO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSPROCESSMANAGER::Application_IsRunning(XCHAR* applicationname, XDWORD* ID)
{
   if(!applicationname)
    {
      return false;
    }

  if(!applicationname[0])
    {
      return false;
    }

  XVECTOR<XPROCESS*>  applist;
  bool                status = GEN_XPROCESSMANAGER.Application_GetRunningList(applist);
  bool                exists = false;

  if(status)
    {
      for(XDWORD c=0; c<applist.GetSize(); c++)
        {
          XSTRING nameapp;
          XSTRING _applicationname = applicationname;

          nameapp  = applist.Get(c)->GetName()->Get();

          if(_applicationname.Find(nameapp, true) != XSTRING_NOTFOUND)
            {
              if(ID) (*ID) = applist.Get(c)->GetID();
              exists = true;
            }

        }
    }
    
  applist.DeleteContents();
  applist.DeleteAll();

  return exists;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XWINDOWSPROCESSMANAGER::Application_GetRunningList(XVECTOR<XPROCESS*>& applist, bool onlywithvalidwindow)
* @brief      Application get running list
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  applist : 
* @param[in]  onlywithvalidwindow : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSPROCESSMANAGER::Application_GetRunningList(XVECTOR<XPROCESS*>& applist, bool onlywithvalidwindow)
{
  PROCESSENTRY32 processentry;
  HANDLE         snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  
  if(snapshot == INVALID_HANDLE_VALUE) 
    {
      return false;
    }

  if(xmutexgetsrunninglist)
    {   
      xmutexgetsrunninglist->Lock();
    }
  
  applist.DeleteContents();
  applist.DeleteAll();

  memset(&processentry, 0, sizeof(PROCESSENTRY32));
  processentry.dwSize = sizeof(PROCESSENTRY32);

  if(Process32First(snapshot, &processentry))
    {      
      do{ XPROCESS* xprocess = new XPROCESS();
          if(xprocess)
            {
              HANDLE  processhandle = NULL;
              TCHAR   pathfilename[MAX_PATH];

              xprocess->SetID((XDWORD)processentry.th32ProcessID); 
              xprocess->GetName()->Set(processentry.szExeFile); 
              
              processhandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processentry.th32ProcessID);
              if(processhandle != NULL) 
                {
                  GetModuleFileNameEx(processhandle, NULL, pathfilename, MAX_PATH);

                  xprocess->GetPath()->Set(pathfilename);

                  CloseHandle(processhandle);  
                }
              
              applist.Add(xprocess);               
            }

          memset(&processentry, 0, sizeof(PROCESSENTRY32));
          processentry.dwSize = sizeof(PROCESSENTRY32);

        } while(Process32Next(snapshot, &processentry));
    }  

  CloseHandle(snapshot);  

  GetChildProcesses(applist);

  MAPWINPROCESS mapofwinprocess;
  GetMapOfWinProcess(mapofwinprocess);

  
  XDWORD c = 0;   

  do{ XPROCESS* xprocess = applist.Get(c);
      if(xprocess)
        {
          if(!xprocess->GetWindowHandle())      
            {              
              for(XDWORD d=0; d<mapofwinprocess.GetSize(); d++)
                {
                  HWND    hwnd      = mapofwinprocess.GetKey(d);
                  XDWORD  processID = mapofwinprocess.GetElement(d);
                  
                  for(XDWORD e=0; e<xprocess->GetProcessIDs()->GetSize(); e++)
                    {                   
                      if(processID == xprocess->GetProcessIDs()->Get(e))
                        {                       
                          XSTRING     title;
                          GRPRECTINT  rect;                          
         
                          GetWindowPropertys(hwnd, title, rect);                          

                          if(!xprocess->GetWindowHandle())                                  
                            {                                                           
                              xprocess->SetWindowHandle((void*)hwnd);         
                              xprocess->GetWindowTitle()->Set(title.Get());
                              xprocess->GetWindowRect()->CopyFrom(&rect);
                              xprocess->SetWindowTitleHeight(GetWindowTitleHeight(hwnd));
                              xprocess->SetWindowBorderWidth(GetWindowBorderWidth(hwnd));                              
                            }
                           else
                            {                                                                    
                              XPROCESS* newxprocess = new XPROCESS();
                              if(newxprocess)
                                {
                                  newxprocess->CopyFrom((*xprocess));  
                                                              
                                  newxprocess->SetWindowHandle((void*)hwnd);         
                                  newxprocess->GetWindowTitle()->Set(title.Get());
                                  newxprocess->GetWindowRect()->CopyFrom(&rect);
                                  newxprocess->SetWindowTitleHeight(GetWindowTitleHeight(hwnd));
                                  newxprocess->SetWindowBorderWidth(GetWindowBorderWidth(hwnd));

                                  applist.Add(newxprocess);                                  
                                }                                                                                                                                                  
                            } 

                          break; 
                        } 
                    }                  
                }
            }
        }
      
      c++;

    } while(c<applist.GetSize());          

  mapofwinprocess.DeleteAll();

  //------------------------------------------------------------------------

  if(onlywithvalidwindow)
    {
      XDWORD c = 0;

      do{ XPROCESS* xprocess = applist.Get(c);
          if(xprocess)
            {
              if(!xprocess->GetWindowHandle()           || 
                  xprocess->GetWindowTitle()->IsEmpty() ||          
                  xprocess->GetWindowRect()->IsEmpty())
                {
                  applist.Delete(xprocess);
                  delete xprocess;
                }
               else
                {
                  c++;  
                } 
            } 
           else
            {
              break;
            }

        } while(c<applist.GetSize());          
    }

  //------------------------------------------------------------------------
  /*
  #ifdef XTRACE_ACTIVE
  for(XDWORD c = 0; c < applist.GetSize(); c++)
    {
      XPROCESS* process = applist.Get(c);
      if(process)
        {
          XTRACE_PRINTCOLOR(XTRACE_COLOR_BLUE, __L("ID: %08X, %s, %s, %08X, %s, [%d,%d,%d,%d]") , process->GetID()
                                                                                                , process->GetName()->Get()
                                                                                                , process->GetPath()->Get()                                                                                                
                                                                                                , process->GetWindowHandle()
                                                                                                , process->GetWindowTitle()->Get()
                                                                                                , process->GetWindowRect()->x1
                                                                                                , process->GetWindowRect()->y1
                                                                                                , process->GetWindowRect()->x2
                                                                                                , process->GetWindowRect()->y2
                                                                                                );
        }
    }  
  #endif  
  */
  //------------------------------------------------------------------------

  if(xmutexgetsrunninglist)
    {   
      xmutexgetsrunninglist->UnLock();
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XWINDOWSPROCESSMANAGER::Application_Terminate(XDWORD processID, XDWORD exitcode)
* @brief      Application terminate
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  processID : 
* @param[in]  exitcode : 
* 
* @return     bool : true if is succesful. 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSPROCESSMANAGER::Application_Terminate(XDWORD processID, XDWORD  exitcode)
{
  DWORD   desiredaccess   = PROCESS_TERMINATE;
  bool    inherithandle   = FALSE;
  HANDLE  hprocess        = NULL;

  hprocess = OpenProcess(desiredaccess, inherithandle, processID);
  if(hprocess == NULL) return false;

  BOOL result = TerminateProcess(hprocess, exitcode);

  CloseHandle(hprocess);

  return (result?true:false);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XWINDOWSPROCESSMANAGER::GetWindowPropertys(HWND hwnd, XSTRING& title, GRPRECTINT& rect)
* @brief      Get window propertys
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  hwnd : 
* @param[in]  title : 
* @param[in]  rect : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSPROCESSMANAGER::GetWindowPropertys(HWND hwnd, XSTRING& title, GRPRECTINT& rect)
{           
  title.Empty(); 

  int sizetitlewindow = GetWindowTextLength(hwnd);                                   
  if(!sizetitlewindow)
    { 
      return false;
    }
                  
  title.AdjustSize(sizetitlewindow+1);
  int size = GetWindowText(hwnd, title.Get(), sizetitlewindow + 1);
  title.AdjustSize();

  if(!size) 
    {
      title.Empty(); 
      return false;
    }

  RECT wrect;

  if(GetWindowRect(hwnd, &wrect))
    {
      rect.x1 = wrect.left;
      rect.y1 = wrect.top;
      rect.x2 = wrect.right;
      rect.y2 = wrect.bottom;
    }
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int XWINDOWSPROCESSMANAGER::GetWindowTitleHeight(HWND hwnd)
* @brief      Get window title height
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  hwnd : 
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int XWINDOWSPROCESSMANAGER::GetWindowTitleHeight(HWND hwnd) 
{
  RECT window_rectangle; 
  RECT client_rectangle;
  int  height;
  int  width;
    
  GetWindowRect(hwnd, &window_rectangle);
  GetClientRect(hwnd, &client_rectangle);

  height = (window_rectangle.bottom - window_rectangle.top) - (client_rectangle.bottom - client_rectangle.top);
  width  = (window_rectangle.right - window_rectangle.left) - (client_rectangle.right - client_rectangle.left);
    
  return height - (width/2);
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int XWINDOWSPROCESSMANAGER::GetWindowBorderWidth(HWND hwnd)
* @brief      Get window border width
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  hwnd : 
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int XWINDOWSPROCESSMANAGER::GetWindowBorderWidth(HWND hwnd) 
{
  RECT window_rectangle; 
  RECT client_rectangle;

  GetWindowRect(hwnd, &window_rectangle);
  GetClientRect(hwnd, &client_rectangle);

  return ((window_rectangle.right - window_rectangle.left) - client_rectangle.right) / 2; 
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XWINDOWSPROCESSMANAGER::GetChildProcesses(XVECTOR<XPROCESS*>& applist)
* @brief      Get child processes
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  applist : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSPROCESSMANAGER::GetChildProcesses(XVECTOR<XPROCESS*>& applist) 
{
  HANDLE threadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if(threadSnapshot == INVALID_HANDLE_VALUE) 
    {
      return false;
    }

  THREADENTRY32 threadEntry;
  threadEntry.dwSize = sizeof(THREADENTRY32);

  if(!Thread32First(threadSnapshot, &threadEntry)) 
    {
      CloseHandle(threadSnapshot);
      return false;
    }

  do { XPROCESS* mainprocess = Application_GetProcessByID(threadEntry.th32OwnerProcessID, applist);
       if(mainprocess) 
         {
           if(mainprocess->GetProcessIDs()->IsEmpty())
             {
               mainprocess->GetProcessIDs()->Add(threadEntry.th32OwnerProcessID); 
             }

           mainprocess->GetProcessIDs()->Add(threadEntry.th32ThreadID);
         }

      } while (Thread32Next(threadSnapshot, &threadEntry));

  CloseHandle(threadSnapshot);

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         BOOL CALLBACK XWINDOWSPROCESSMANAGER::EnumWindowsProc(HWND hwnd, LPARAM lparam)
* @brief      Enum windows proc
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  hwnd : 
* @param[in]  lparam : 
* 
* @return     BOOL : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
BOOL CALLBACK XWINDOWSPROCESSMANAGER::EnumWindowsProc(HWND hwnd, LPARAM lparam)
{
  MAPWINPROCESS* mapofwinprocess = (MAPWINPROCESS*)lparam;
  if(!mapofwinprocess)
    {
      return FALSE;
    }
    
  DWORD processID;  

  GetWindowThreadProcessId(hwnd, &processID);

  if(processID)
    {
      mapofwinprocess->Add(hwnd, processID);
    }
    
  return TRUE;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool XWINDOWSPROCESSMANAGER::GetMapOfWinProcess(MAPWINPROCESS& mapofwinprocess)
* @brief      Get map of win process
* @ingroup    PLATFORM_WINDOWS
* 
* @param[in]  mapofwinprocess : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSPROCESSMANAGER::GetMapOfWinProcess(MAPWINPROCESS& mapofwinprocess)
{  
  mapofwinprocess.DeleteAll();

  BOOL result = EnumWindows(EnumWindowsProc, (LPARAM)(&mapofwinprocess));
  
  return result?true:false;
 }


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSPROCESSMANAGER::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    PLATFORM_WINDOWS
*
* --------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSPROCESSMANAGER::Clean()
{
  xmutexgetsrunninglist = NULL;
}

#pragma endregion
