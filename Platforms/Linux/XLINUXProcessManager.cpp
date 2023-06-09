/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XLINUXProcessManager.cpp
*
* @class      XLINUXPROCESSMANAGER
* @brief      eXtended LINUX process management class
* @ingroup    PLATFORM_LINUX
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include "XLINUXFactory.h"

#include "XBuffer.h"
#include "XFile.h"
#include "XTrace.h"
#include "XString.h"
#include "XPath.h"

#include "XLINUXProcessManager.h"

#include "XMemory_Control.h"

/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/

/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XLINUXPROCESSMANAGER::XLINUXPROCESSMANAGER()
* @brief      Constructor
* @ingroup    PLATFORM_LINUX
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XLINUXPROCESSMANAGER::XLINUXPROCESSMANAGER(): XPROCESSMANAGER()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XLINUXPROCESSMANAGER::~XLINUXPROCESSMANAGER()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    PLATFORM_LINUX
*
* @return     Does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
XLINUXPROCESSMANAGER::~XLINUXPROCESSMANAGER()
{
   Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXPROCESSMANAGER::MakeSystemCommand(XCHAR* command)
* @brief      MakeSystemCommand
* @ingroup    PLATFORM_LINUX
*
* @param[in]  command : 
*
* @return     bool : true if is succesful. 
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXPROCESSMANAGER::MakeSystemCommand(XCHAR* command)
{
  XSTRING   _command;
  bool      status = false;

  _command = command;
  
  XBUFFER cmdchar;
  
  _command.ConvertToASCII(cmdchar);  
  //printf(" -- %s\n", cmdchar.GetPtrChar());
  if(system(cmdchar.GetPtrChar()) == -1) status = false; else status = true;
    
  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXPROCESSMANAGER::MakeCommand(XCHAR* command, XSTRING& out, int* returncode)
* @brief      MakeCommand
* @ingroup    PLATFORM_LINUX
*
* @param[in]  command :
* @param[in]  out :
* @param[in]  returncode :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXPROCESSMANAGER::MakeCommand(XCHAR* command, XSTRING* out, int* returncode)
{
  XSTRING _command;
  char    buffer[256];
  FILE*   pipe;
  bool    status = true;

  _command = command;

  XBUFFER cmdchar;
  
  _command.ConvertToASCII(cmdchar);   
  pipe = popen(cmdchar.GetPtrChar(), "rt" );
  if(pipe == NULL) status = false;
  
  if(!status) return false;

  if(out)
    {
      while(fgets(buffer, 256, pipe))
        {
          (*out) += buffer;
        }
    }

  // Close pipe and print return value of pPipe.
  if(feof(pipe))
    {
      if(returncode) (*returncode) =  pclose(pipe);
    }
   else
    {
      return false;
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXPROCESSMANAGER::ExecuteApplication(XCHAR* applicationpath, XCHAR* params, XSTRING* in, XSTRING* out, int* returncode)
* @brief      ExecuteApplication
* @ingroup    PLATFORM_LINUX
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
bool XLINUXPROCESSMANAGER::ExecuteApplication(XCHAR* applicationpath, XCHAR* params, XSTRING* in, XSTRING* out, int* returncode)
{ 
  #define PIPE_READ   0
  #define PIPE_WRITE  1

  if(!applicationpath) return false;

  pid_t pID;
  bool  exist  = false;
  bool  status = false;
  int   stdinpipe[2]  = { 0 , 0 };
  int   stdoutpipe[2] = { 0 , 0 };
  int   nresult = 0;

  if(pipe(stdinpipe) < 0)
    {
      return -1;
    }

  if(pipe(stdoutpipe) < 0)
    {
      close(stdinpipe[PIPE_READ]);
      close(stdinpipe[PIPE_WRITE]);

      return -1;
    }


  XFILE* GEN_XFACTORY_CREATE(xfile, Create_File())
  if(xfile)
    {
      exist = xfile->Open(applicationpath);
      xfile->Close();
    }

  GEN_XFACTORY.Delete_File(xfile);

  if(!exist) return false;

  //-----------------------------------------
  // exec attributtes

  XSTRING cmd;

  cmd  = __L("chmod 775 ");
  cmd += applicationpath;


  XBUFFER charstr;
  
  cmd.ConvertToASCII(charstr);
  status = system(charstr.GetPtrChar());
  
  //if(status == -1) return false;

  //------------------------------------------

  XSTRING _params;

  cmd = applicationpath;

  _params.Add(cmd);
  if(params)
    {
      _params.Add(" ");
      _params.Add(params);
    }

  #define MAXNPARAMS  20

  char* param[MAXNPARAMS] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                              NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
                            };
  int   start             = 0;
  bool  endfound          = false;

  for(int c=0; c<MAXNPARAMS; c++)
    {
      XSTRING string;
      int     found = _params.Find(__L(" "), true, start);

      if(found == XSTRING_NOTFOUND)
        {
          _params.Copy(start, string);
          endfound = true;
        }
       else
        {
          _params.Copy(start, found, string);
          start = found+1;
        }
            
      if(endfound) break;
    }

  pID = fork();

  switch(pID)
    {
      case -1 : { // failed to create child

                  close(stdinpipe[PIPE_READ]);
                  close(stdinpipe[PIPE_WRITE]);

                  close(stdoutpipe[PIPE_READ]);
                  close(stdoutpipe[PIPE_WRITE]);
                }
                break;

      case  0 : { // child continues here

                  // redirect stdin
                  if(dup2(stdinpipe[PIPE_READ], STDIN_FILENO)    == -1)
                    {
                      exit(errno);
                    }

                    // redirect stdout
                  if(dup2(stdoutpipe[PIPE_WRITE], STDOUT_FILENO) == -1)
                    {
                      exit(errno);
                    }

                  // redirect stderr
                  if(dup2(stdoutpipe[PIPE_WRITE], STDERR_FILENO) == -1)
                    {
                      exit(errno);
                    }

                  // all these are for use by parent only
                  close(stdinpipe[PIPE_READ]);
                  close(stdinpipe[PIPE_WRITE]);
                  close(stdoutpipe[PIPE_READ]);
                  close(stdoutpipe[PIPE_WRITE]);

                  // run child process image
                  // replace this with any exec* function find easier to use ("man exec")

                  // Child
                  
                  XBUFFER charstr;
                  
                  cmd.ConvertToASCII(charstr);                                
                  int result = execl(charstr.GetPtrChar() , param[ 0], param[ 1], param[ 2], param[ 3], param[ 4], param[ 5], param[ 6], param[ 7], param[ 8], param[ 9]
                                                          , param[10], param[11], param[12], param[13], param[14], param[15], param[16], param[17], param[18], param[19]
                                                          , NULL);
                  if(result == -1)
                    {
                      //XTRACE_PRINTCOLOR(4,__L("Error en EXEC: %d"), errno);
                    }
                   else
                    {
                      if(returncode) (*returncode) = result;
                      status = true;
                    }
                  
                  exit(result);
                }
                break;

      default : { char nchar;

                  //parent continues here

                  // close unused file descriptors, these are for child only
                  close(stdinpipe[PIPE_READ]);
                  close(stdoutpipe[PIPE_WRITE]);     

                  if(in)
                    {
                      if(in->GetSize())
                        {
                          XBUFFER charstr;
                          
                          (*in).ConvertToASCII(charstr);                           
                          write(stdinpipe[PIPE_WRITE], charstr.Get(), in->GetSize()); 
                        }
                    }

                  if(out)
                    {                   
                      // Just a char by char read here, you can change it accordingly
                      while(read(stdoutpipe[PIPE_READ], &nchar, 1) == 1)
                        {
                          out->Add(nchar);              
                        }
                    }
                  
                  // done with these in this example program, you would normally keep these
                  // open of course as long as you want to talk to the child
                  close(stdinpipe[PIPE_WRITE]);
                  close(stdoutpipe[PIPE_READ]);
                }

                status = true;
                break;
    }

  for(int c=0; c<MAXNPARAMS; c++)
    {
      delete [] param[c];
    }

  return status;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XLINUXPROCESSMANAGER::IsApplicationRunning(XCHAR* applicationname, XDWORD* ID)
* @brief      Is Application Running
* @ingroup    PLATFORM_LINUX
*
* @param[in]  applicationname : application name
* @param[out] ID : ID of the application in the SO
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool XLINUXPROCESSMANAGER::IsApplicationRunning(XCHAR* applicationname, XDWORD* ID)
{
  DIR*            dir;
  struct dirent*  ent;
  char*           endptr;
  char            buf[512];

  if(!(dir = opendir("/proc"))) return -1;

  while((ent = readdir(dir)) != NULL)
    {
      // if endptr is not a null character, the directory is not entirely numeric, so ignore it
      long lpid = strtol(ent->d_name, &endptr, 10);
      if(*endptr != '\0')
        {
          continue;
        }

      // try to open the cmdline file
      snprintf(buf, sizeof(buf), "/proc/%ld/status", lpid);
      FILE* fp = fopen(buf, "r");
      if(fp)
        {
          if(fgets(buf, sizeof(buf), fp) != NULL)
            {
              // check the first token in the file, the program name
              char* first = strtok(buf, " ");

              XPATH nameall;

              nameall = first;
              nameall.SetOnlyNamefile();
              nameall.DeleteCharacter(__C(' ') , XSTRINGCONTEXT_FROM_FIRST);
              nameall.DeleteCharacter(__C('\t'), XSTRINGCONTEXT_FROM_FIRST);
              nameall.DeleteCharacter(__C('\n'), XSTRINGCONTEXT_TO_END);
              nameall.DeleteCharacter(__C('\r'), XSTRINGCONTEXT_TO_END);

              if(!nameall.Compare(applicationname, false))
                {
                  if(ID) (*ID) = (XDWORD)lpid;

                  fclose(fp);
                  closedir(dir);

                  return true;

                }
            }

          fclose(fp);
        }
    }

  closedir(dir);


  return false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XLINUXPROCESSMANAGER::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    PLATFORM_LINUX
*
* @return     void : does not return anything.
*
* --------------------------------------------------------------------------------------------------------------------*/
void XLINUXPROCESSMANAGER::Clean()
{

}


