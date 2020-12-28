//
// Created by Maxwell on 2020-12-04.
//


#include <valkyrie/Core/IO/Messages.hpp>

#include <windows.h>
#include <memoryapi.h>
//#include <sysinfoapi.h>
#include <dbghelp.h>


#include <iostream>






int main(){

}



/*
int main(){

  using byte = valkyrie::byte;

  using PFN_VirtualAlloc2 = void*(*)(HANDLE                 Process,
                                     PVOID                  BaseAddress,
                                     SIZE_T                 Size,
                                     ULONG                  AllocationType,
                                     ULONG                  PageProtection,
                                     MEM_EXTENDED_PARAMETER *ExtendedParameters,
                                     ULONG                  ParameterCount
  );
  using PFN_MapViewOfFile3 = void*(*)(HANDLE                 FileMapping,
                                      HANDLE                 Process,
                                      PVOID                  BaseAddress,
                                      ULONG64                Offset,
                                      SIZE_T                 ViewSize,
                                      ULONG                  AllocationType,
                                      ULONG                  PageProtection,
                                      MEM_EXTENDED_PARAMETER *ExtendedParameters,
                                      ULONG                  ParameterCount
  );

  PFN_VirtualAlloc2 pVirtualAlloc2;
  PFN_MapViewOfFile3 pMapViewOfFile3;
  HMODULE kernelDll = LoadLibrary("KernelBase.dll");
  pVirtualAlloc2 = (PFN_VirtualAlloc2)GetProcAddress(kernelDll, "VirtualAlloc2");
  pMapViewOfFile3 = (PFN_MapViewOfFile3)GetProcAddress(kernelDll, "MapViewOfFile3");


  byte* pAllocation;
  byte* pPlaceholderOne;
  byte* pPlaceholderTwo;
  byte* pViewOne;
  byte* pViewTwo;
  HANDLE hMappedFile;
  size_t bufferSize = 0x10'0000; // 1 MB
  SYSTEM_INFO sysInfo;

  GetSystemInfo(&sysInfo);

  pAllocation = (byte*)pVirtualAlloc2(nullptr,
                                         nullptr,
                                         4 * bufferSize,
                                         MEM_RESERVE | MEM_RESERVE_PLACEHOLDER,
                                         PAGE_NOACCESS,
                                         nullptr,
                                         0);

  pPlaceholderOne = pAllocation + bufferSize;
  pPlaceholderTwo = pPlaceholderOne + bufferSize;

  auto freeResultA = VirtualFree(pAllocation, bufferSize, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER);
  auto freeResultB = VirtualFree(pPlaceholderOne, bufferSize, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER);
  auto freeResultC = VirtualFree(pPlaceholderTwo, bufferSize, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER);

  hMappedFile = CreateFileMapping(
    INVALID_HANDLE_VALUE,
    nullptr,
    PAGE_READWRITE,
    0,
    bufferSize,
    nullptr
  );

  pViewOne = (byte*)pMapViewOfFile3(
    hMappedFile,
    nullptr,
    pPlaceholderOne,
    0,
    bufferSize,
    MEM_REPLACE_PLACEHOLDER,
    PAGE_READWRITE,
    nullptr,
    0
  );

  pViewTwo = (byte*)pMapViewOfFile3(
    hMappedFile,
    nullptr,
    pPlaceholderTwo,
    0,
    bufferSize,
    MEM_REPLACE_PLACEHOLDER,
    PAGE_READWRITE,
    nullptr,
    0
  );

  CloseHandle(hMappedFile);

  char* ringBuffer = (char*)pViewOne;
  std::memcpy(ringBuffer, "Hello world!", sizeof("Hello world!"));

  std::cout << (&ringBuffer[bufferSize]) << std::endl;

  FreeLibrary(kernelDll);

  UnmapViewOfFile(pViewOne);
  UnmapViewOfFile(pViewTwo);

  return 0;
}
*/


/*
BOOL CALLBACK EnumSymProc(
    PSYMBOL_INFO pSymInfo,
    ULONG SymbolSize,
    PVOID UserContext)
{
  UNREFERENCED_PARAMETER(UserContext);

  printf("%08X %4u %s\n",
         pSymInfo->Address, SymbolSize, pSymInfo->Name);
  return TRUE;
}

int main()
{
  HANDLE hProcess = GetCurrentProcess();
  DWORD64 BaseOfDll;
  const char *Mask = "Map*";
  BOOL status;

  status = SymInitialize(hProcess, "C:\\Windows\\System32", FALSE);
  if (status == FALSE)
  {
    return 1;
  }

  BaseOfDll = SymLoadModuleEx(hProcess,
                              NULL,
                              "KernelBase.dll",
                              NULL,
                              0,
                              0,
                              NULL,
                              0);

  if (BaseOfDll == 0)
  {
    auto message = valkyrie::Core::System::Win32::getLastError();
    SymCleanup(hProcess);
    panic(message);
    return 1;
  }

  if (SymEnumSymbols(hProcess,     // Process handle from SymInitialize.
                     BaseOfDll,   // Base address of module.
                     Mask,        // Name of symbols to match.
                     EnumSymProc, // Symbol handler procedure.
                     NULL))       // User context.
  {
    // SymEnumSymbols succeeded
  }
  else
  {
    // SymEnumSymbols failed
    printf("SymEnumSymbols failed: %d\n", GetLastError());
  }

  SymCleanup(hProcess);
  return 0;
}*/
