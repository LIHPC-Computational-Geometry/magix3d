// Gmsh - Copyright (C) 1997-2013 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@geuz.org>.

// This file contains a bunch of functions that depend on OS-dependent
// features and/or system calls

// these are available on all OSes
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <math.h>

#if !defined(WIN32) || defined(__CYGWIN__)
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#if defined(WIN32)
#include <windows.h>
#include <process.h>
#include <io.h>
#include <direct.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#endif

#if defined(__APPLE__)
#define RUSAGE_SELF      0
#define RUSAGE_CHILDREN -1
#endif

#include "GmshMessage.h"

const char *GetEnvironmentVar(const char *var)
{
#if !defined(WIN32)
  return getenv(var);
#else
  const char *tmp = getenv(var);
  // Don't accept top dir or anything partially expanded like
  // c:\Documents and Settings\%USERPROFILE%, etc.
  if(!tmp || !strcmp(tmp, "/") || strstr(tmp, "%") || strstr(tmp, "$"))
    return 0;
  else
    return tmp;
#endif
}

double GetTimeInSeconds()
{
#if !defined(WIN32) || defined(__CYGWIN__)
  struct timeval tp;
  gettimeofday(&tp, (struct timezone *)0);
  double t = (double)tp.tv_sec + 1.e-6 * (double)tp.tv_usec;
  return t;
#else
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  double t =  1.e-7 * 4294967296. * (double)ft.dwHighDateTime +
              1.e-7 * (double)ft.dwLowDateTime;
  return t;
#endif
}

void SleepInSeconds(double s)
{
#if !defined(WIN32) || defined(__CYGWIN__)
  usleep((long)(1.e6 * s));
#else
  Sleep((long)(1.e3 * s));
#endif
}

static void GetResources(double *s, long *mem)
{
#if !defined(WIN32) || defined(__CYGWIN__)
  static struct rusage r;
  getrusage(RUSAGE_SELF, &r);
  *s = (double)r.ru_utime.tv_sec + 1.e-6 * (double)r.ru_utime.tv_usec;
  *mem = (long)r.ru_maxrss;
#else
  FILETIME creation, exit, kernel, user;
  if(GetProcessTimes(GetCurrentProcess(), &creation, &exit, &kernel, &user)){
    *s = 1.e-7 * 4294967296. * (double)user.dwHighDateTime +
         1.e-7 * (double)user.dwLowDateTime;
  }
  *mem = 0;
#endif
}

void CheckResources()
{
#if !defined (WIN32) || defined(__CYGWIN__)
  static struct rlimit r;

  getrlimit(RLIMIT_STACK, &r);

  // Try to get at least 16 MB of stack. Running with too small a stack
  // can cause crashes in the recursive calls (e.g. for tet
  // classification in 3D Delaunay)
  if(r.rlim_cur < 16 * 1024 * 1024){
    Msg::Info("Increasing process stack size (%d kB < 16 MB)", r.rlim_cur / 1024);
    r.rlim_cur = r.rlim_max;
    setrlimit(RLIMIT_STACK, &r);
  }
#endif
}

double Cpu()
{
  long mem = 0;
  double s = 0.;
  GetResources(&s, &mem);
  return s;
}

long GetMemoryUsage()
{
  long mem = 0;
  double s = 0.;
  GetResources(&s, &mem);
  return mem;
}

int GetProcessId()
{
#if !defined(WIN32) || defined(__CYGWIN__)
  return getpid();
#else
  return _getpid();
#endif
}

std::string GetHostName()
{
  char host[256];
  gethostname(host, sizeof(host));
  return std::string(host);
}

int UnlinkFile(const std::string &fileName)
{
#if !defined(WIN32) || defined(__CYGWIN__)
  return unlink(fileName.c_str());
#else
  return _unlink(fileName.c_str());
#endif
}

int StatFile(const std::string &fileName)
{
#if !defined(WIN32) || defined(__CYGWIN__)
  struct stat buf;
  int ret = stat(fileName.c_str(), &buf);
  // could get file modification time from buf
#else
  struct _stat buf;
  int ret = _stat(fileName.c_str(), &buf);
#endif
  return ret;
}

int CreateDirectory(const std::string &dirName)
{
#if !defined(WIN32) || defined(__CYGWIN__)
  if(mkdir(dirName.c_str(), 0777))
    return 0;
#else
  if(_mkdir(dirName.c_str()))
    return 0;
#endif
  return 1;
}

int KillProcess(int pid)
{
#if !defined(WIN32) || defined(__CYGWIN__)
  if(kill(pid, 9))
    return 0;
#else
  HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
  if(!TerminateProcess(hProc, 0)){
    CloseHandle(hProc);
    return 0;
  }
#endif
  return 1;
}

int SystemCall(const std::string &command, bool blocking)
{
#if defined(WIN32)
  STARTUPINFO suInfo;
  PROCESS_INFORMATION prInfo;
  memset(&suInfo, 0, sizeof(suInfo));
  suInfo.cb = sizeof(suInfo);
  Msg::Info("Calling '%s'", command.c_str());
  if(blocking){
    CreateProcess(NULL, (char*)command.c_str(), NULL, NULL, FALSE,
                  NORMAL_PRIORITY_CLASS, NULL, NULL,
                  &suInfo, &prInfo);
    // wait until child process exits.
    WaitForSingleObject(prInfo.hProcess, INFINITE);
    // close process and thread handles.
    CloseHandle(prInfo.hProcess);
    CloseHandle(prInfo.hThread);
  }
  else{
    // DETACHED_PROCESS removes the console (useful if the program to launch is
    // a console-mode exe)
    CreateProcess(NULL, (char*)command.c_str(), NULL, NULL, FALSE,
                  NORMAL_PRIORITY_CLASS|DETACHED_PROCESS, NULL, NULL,
                  &suInfo, &prInfo);
  }
  return 0;
#else
  if(!system(NULL)) {
    Msg::Error("Could not find /bin/sh: aborting system call");
    return 1;
  }
  std::string cmd(command);
  if(!blocking) cmd += " &";
  Msg::Info("Calling '%s'", cmd.c_str());
  system(cmd.c_str());
  return 0;
#endif
}

std::string GetCurrentWorkdir()
{
  char path[1024];
#if defined(WIN32)
  if(!_getcwd(path, sizeof(path))) return "";
#else
  if(!getcwd(path, sizeof(path))) return "";
#endif
  std::string str(path);
  // match the convention of SplitFileName that delivers directory path
  // ending with a directory separator
#if defined(WIN32)
  str.append("\\");
#else
  str.append("/");
#endif
  return str;
}

void RedirectIOToConsole()
{
#if defined(WIN32) && !defined(__CYGWIN__)
  // Win32 GUI apps do not write to the DOS console; make it work again by
  // attaching to parent console, which allows to use the DOS shell to work
  // with Gmsh on the command line (without this hack, you need to either use
  // a better shell (e.g. bash), or compile a /subsystem:console version
  if(!AttachConsole(ATTACH_PARENT_PROCESS)) return;
  // redirect unbuffered stdout, stdin and stderr to the console
  intptr_t lStdHandle = (intptr_t)GetStdHandle(STD_OUTPUT_HANDLE);
  int hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  FILE *fp = _fdopen(hConHandle, "w");
  *stdout = *fp;
  setvbuf(stdout, NULL, _IONBF, 0);
  lStdHandle = (intptr_t)GetStdHandle(STD_INPUT_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "r");
  *stdin = *fp;
  setvbuf(stdin, NULL, _IONBF, 0);
  lStdHandle = (intptr_t)GetStdHandle(STD_ERROR_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "w");
  *stderr = *fp;
  setvbuf(stderr, NULL, _IONBF, 0);
  // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to
  // console as well
  std::ios::sync_with_stdio();
#endif
}
