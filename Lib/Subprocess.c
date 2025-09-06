// clang-format off
///*///
#include<stdio.h>
#include<windows.h>

struct Subprocess{String commandLine, PROCESS_INFORMATION pi};

namespace Subprocess

c_function _clear_pi()
    ZeroMemory(&this->pi, sizeof(this->pi));
endc_function

function __init__(commandLine : str)
    this.commandLine.__init__(commandLine)
    this._clear_pi()      
endfunction  

c_function run() -> int:
    // Start subprocess (non-blocking)
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    if (!CreateProcessA(
        NULL,                  // No module name (use command line)
        this->commandLine.arr, // Command line
        NULL,                  // Process handle not inheritable
        NULL,                  // Thread handle not inheritable
        FALSE,                 // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE,    // Create in new console window
        NULL,                  // Use parent's environment block
        NULL,                  // Use parent's starting directory 
        &si,                   // Pointer to STARTUPINFO structure
        &this->pi))             // Pointer to PROCESS_INFORMATION structure
    {
        fprintf(stderr, "CreateProcess failed (%lu).\n", GetLastError());
        return -1;
    }

    // success, running in background
    return 0; 
endc_function

c_function wait() -> int:
    if (this->pi.hProcess == NULL) {
        fprintf(stderr, "No process to wait for.\n");
        return -1;
    }

    // Wait until child process exits
    WaitForSingleObject(this->pi.hProcess, INFINITE);

    DWORD exitCode;
    if (!GetExitCodeProcess(this->pi.hProcess, &exitCode)) {
        fprintf(stderr, "GetExitCodeProcess failed (%lu).\n", GetLastError());
        exitCode = (DWORD)-1;
    }

    CloseHandle(this->pi.hProcess);
    CloseHandle(this->pi.hThread);

    this->pi.hProcess = NULL;
    this->pi.hThread = NULL;

    return (int)exitCode;
endc_function

c_function run_and_detach() -> int:
  STARTUPINFOA si;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);

  // Clear any old process information before creating a new one.
  Subprocess_clear_pi(this);

  if (!CreateProcessA(NULL,     // No module name (use command line)
    this->commandLine.arr,      // Command line
    NULL,                       // Process handle not inheritable
    NULL,                       // Thread handle not inheritable
    FALSE,                      // Set handle inheritance to FALSE
    CREATE_NEW_CONSOLE,         // Create in new console window
    NULL,                       // Use parent's environment block
    NULL,                       // Use parent's starting directory
    &si,                        // Pointer to STARTUPINFO structure
    &this->pi)) 
  {
    fprintf(stderr, "CreateProcess failed (%lu).\n", GetLastError());
    return -1;
  }

  // Immediately close the handles. The child process will continue to run.
  // This prevents handle leaks and detaches the child from the parent.
  CloseHandle(this->pi.hProcess);
  CloseHandle(this->pi.hThread);

  // Clear the PI structure again so a subsequent call to wait() will fail gracefully.
  Subprocess_clear_pi(this);

  return 0;
endc_function
endnamespace
///*///
