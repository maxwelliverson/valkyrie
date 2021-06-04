//
// Created by maxwe on 2021-05-25.
//

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>


#pragma comment(lib, "psapi")




void printError() {
  DWORD err = GetLastError();
  TCHAR errString[1 << 14];
  size_t errStringLen = sizeof(errString) / sizeof(TCHAR);

  auto result = FormatMessage(
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS |
      (FORMAT_MESSAGE_MAX_WIDTH_MASK & 80),
                nullptr,
                err,
                0,
                errString,
                errStringLen,
                nullptr);
  printf("ERROR 0x%lX: %s\n\n", err, errString);
}



// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

void PrintModules( DWORD processID )
{
  HMODULE hMods[1024];
  TCHAR  processName[1024];
  HANDLE hProcess;
  DWORD cbNeeded;
  unsigned int i;
  unsigned int procNameLength;

  // Print the process identifier.

  printf( "\nProcess ID: %lu\n", processID );

  // Get a handle to the process.

  hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                          PROCESS_VM_READ,
                          FALSE, processID );
  if (NULL == hProcess) {
    if ( processID != 0 )
      printError();
    return;
  }


  procNameLength = GetProcessImageFileName(hProcess, processName, sizeof(processName) / sizeof(TCHAR));

  if ( procNameLength == 0 ) {
    printError();
    goto ret;
  }


  processName[procNameLength + 1] = '\0';

  printf("%s\n", processName);

  // Get a list of all the modules in this process.

  if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
  {
    for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
    {
      TCHAR szModName[MAX_PATH];
      MODULEINFO info;

      // Get the full path to the module's file.

      if ( GetModuleFileNameEx( hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
      {
        // Print the module name and handle value.

        printf( TEXT("\t%s (0x%016llX)\n"), szModName, (unsigned long long)hMods[i] );

        if ( GetModuleInformation(hProcess, hMods[i], &info, sizeof(MODULEINFO))) {

          TCHAR memInfo[32];
          if ( info.SizeOfImage >= (1024 * 1024)) {
            sprintf(memInfo, "%.4f MiB", (float)info.SizeOfImage / (1024 * 1024));
          }
          else if ( info.SizeOfImage > 1024) {
            sprintf(memInfo, "%.4f KiB", (float)info.SizeOfImage / float(1024));
          }
          else {
            sprintf(memInfo, "%u bytes", (unsigned int)info.SizeOfImage);
          }

          unsigned long long base = (unsigned long long)info.lpBaseOfDll;

          printf("\t\tLocation: (0x%llx - 0x%llx)\n"
                 "\t\tSize:     %s\n"
                 "\t\tEntry Point: 0x%llx\n", base, base + info.SizeOfImage, memInfo, (unsigned long long)info.EntryPoint);

        }


      }

    }
  }

  printf("\n");

  // Release the handle to the process.

  ret:
  CloseHandle( hProcess );
}

int main( void )
{

  DWORD aProcesses[1024];
  DWORD cbNeeded;
  DWORD cProcesses;
  unsigned int i;

  /*if (!SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
    printError();
    return 1;
  }*/

  // Get the list of process identifiers.

  if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    return 1;

  // Calculate how many process identifiers were returned.

  cProcesses = cbNeeded / sizeof(DWORD);

  // Print the names of the modules for each process.

  for ( i = 0; i < cProcesses; i++ )
  {
    PrintModules( aProcesses[i] );
  }

  return 0;
}
