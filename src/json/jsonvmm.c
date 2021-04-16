//
// Created by maxwe on 2021-04-07.
//

#include "jsonvmm.h"
#include "jsontypes.h"
#include "jsonmm.h"

#include <json/file.h>


#include <assert.h>
#include <intrin.h>


#define NOMINMAX
#include <windows.h>
#include <memoryapi.h>


#pragma intrinsic(__lzcnt, __lzcnt64, _BitScanReverse64)


struct vmem_file{
  json_file_t file;
  const char* name;
  HANDLE      handle;
  json_bool_t writeable;
};

#define WIDE_NAME_LEN 256
#define U64_HIGHEST_BIT ~((~0ULL) >> 1)
#define VMEM_BASE_ORDER 16

enum {
  PLACEHOLDER_STATE_FREE,
  PLACEHOLDER_STATE_SPLIT,
  PLACEHOLDER_STATE_COMMITTED,
  PLACEHOLDER_STATE_MAPPED
};


JSON_forceinline static json_u64_t bit_ceil__(json_u64_t value) {



  /*
     json_u64_t tmp = value - 1;

     while (value & tmp) {  // L0
       tmp |= value;        // L1
       tmp |= tmp >> 1;     // L2
       value = tmp + 1;     // L3
     }
     return value;
   *
   *
   *
   *
   * value =     01100010 10100100
   * tmp   =     01100010 10100011
   *
   * L0 while    01100010 10100000
   * L1 tmp      01100010 10100111
   *    tmp>>1   00110001 01010011
   * L2 tmp      01110011 11110111
   * L3 value    01110011 11111000
   * L0 while    01110011 11110000
   * L1 tmp      01110011 11111111
   *    tmp>>1   00111001 11111111
   * L2 tmp      01111011 11111111
   * L3 value    01111100 00000000
   * L0 while    01111000 00000000
   * L1 tmp      01111111 11111111
   *    tmp>>1   00111111 11111111
   * L2 tmp      01111111 11111111
   * L3 value    10000000 00000000
   * */

  if (!(value & (value - 1)))
    return value;
  return __ull_rshift(U64_HIGHEST_BIT, (json_i32_t)(__lzcnt64(value) - 1));
}
JSON_forceinline static json_u32_t pow2_log2__(json_u64_t value) {
  assert(value && !(value & (value - 1)));
  unsigned long index;
  unsigned char result = _BitScanReverse64(&index, value);
  assert(result != '\0');
  return index - 1;
}











json_status_t json_vmem_open_file(vmem_file_t* pFile, json_file_t file, const char* name) {
  SECURITY_ATTRIBUTES securityAttributes;
  securityAttributes.nLength              = sizeof(securityAttributes);
  securityAttributes.bInheritHandle       = TRUE;
  securityAttributes.lpSecurityDescriptor = NULL;

  HANDLE fileHandle = file == JSON_NULL_HANDLE ? NULL : json_file_get_native(file);

  (*pFile)->name = name;
  WCHAR wideName[WIDE_NAME_LEN];
  int wideNameLen = MultiByteToWideChar(CP_UTF8, 0, name, -1, wideName, WIDE_NAME_LEN);

  assert( wideNameLen != 0 );

  (*pFile)->handle = CreateFileMapping2(fileHandle,
                                        &securityAttributes,
                                        FILE_MAP_ALL_ACCESS,
                                        PAGE_READWRITE,
                                        0,
                                        file->maxSize,
                                        wideName,
                                        NULL,
                                        0);
  //TODO: actual implementation...
  return JSON_SUCCESS;
}
void          json_vmem_close_file(vmem_file_t file) {
  CloseHandle(json_file_get_native(file->file));
}

json_status_t json_vmem_map(void** pResult, vmem_manager_t allocator, vmem_file_t file, json_u64_t size, json_u64_t offset) {
  //TODO: claim pages from allocator to hold size bytes.
  // ...
  // ...
  void* page = NULL;
  *pResult = MapViewOfFile3(json_file_get_native(file->file),
                 GetCurrentProcess(),
                 page,
                 offset,
                 size,
                 MEM_REPLACE_PLACEHOLDER,
                 PAGE_READONLY,
                 NULL,
                 0);
  if ( !*pResult ) {
    DWORD Err = GetLastError();
    switch ( Err ) {
      case ERROR_OUTOFMEMORY:
      case ERROR_NOT_ENOUGH_MEMORY:
        return JSON_ERROR_SYSTEM_MEMORY;
      case ERROR_MAPPED_ALIGNMENT:
      case ERROR_USER_MAPPED_FILE:
        return JSON_ERROR_INTERNAL;
      default:
        return JSON_ERROR_UNKNOWN;
    }
  }
  return JSON_SUCCESS;
}
void          json_vmem_unmap(vmem_manager_t allocator, void* mappedFile, json_u64_t size) {
  BOOL result = UnmapViewOfFile2(GetCurrentProcess(), mappedFile, MEM_PRESERVE_PLACEHOLDER);
  assert(result);
}


vmem_placeholder_t vmem_placeholder_reserve(json_u64_t size, json_u64_t align) {
  MEM_ADDRESS_REQUIREMENTS addrReq;
  addrReq.Alignment             = align;
  addrReq.HighestEndingAddress  = NULL;
  addrReq.LowestStartingAddress = NULL;

  MEM_EXTENDED_PARAMETER extParam;
  extParam.Type = MemExtendedParameterAddressRequirements;
  return (vmem_placeholder_t)VirtualAlloc2(NULL,
                                            NULL,
                                            size,
                                            MEM_RESERVE_PLACEHOLDER | MEM_RESERVE,
                                            PAGE_NOACCESS,
                                            &extParam,
                                            1);
}
vmem_placeholder_t vmem_placeholder_split(vmem_placeholder_t placeholder, json_u64_t size) {
  const json_u64_t nextSize = size >> 1;
  VirtualFree(placeholder, nextSize, MEM_PRESERVE_PLACEHOLDER | MEM_RELEASE);
  return (vmem_placeholder_t)((json_u64_t)placeholder + nextSize);
}
vmem_placeholder_t vmem_placeholder_coalesce(vmem_placeholder_t placeholder, json_u64_t size) {
  const vmem_placeholder_t pairedPlaceholder = (vmem_placeholder_t)((json_u64_t)placeholder ^ size);
  const vmem_placeholder_t retPlaceholder = pairedPlaceholder < placeholder ? pairedPlaceholder : placeholder;
  VirtualFree(retPlaceholder, size, MEM_COALESCE_PLACEHOLDERS | MEM_RELEASE);
  return retPlaceholder;
}
void               vmem_placeholder_release(vmem_placeholder_t placeholder, json_u64_t size) {
  VirtualFree(placeholder, size, MEM_RELEASE);
}

void*              vmem_placeholder_commit(vmem_placeholder_t placeholder, json_u64_t size) {
  return VirtualAlloc2(GetCurrentProcess(),
                       placeholder,
                       size,
                       MEM_REPLACE_PLACEHOLDER,
                       PAGE_READWRITE,
                       NULL,
                       0);
}
vmem_placeholder_t vmem_placeholder_decommit(void* addr, json_u64_t size) {
  const vmem_placeholder_t placeholder = (vmem_placeholder_t)addr;
  BOOL result = VirtualFree(addr, size, MEM_PRESERVE_PLACEHOLDER);
  assert( result );
  return placeholder;
}

void*              vmem_placeholder_map(vmem_placeholder_t placeholder, vmem_file_t file, json_u64_t size, json_u64_t offset) {
  return MapViewOfFile3(json_file_get_native(file->file),
                        GetCurrentProcess(),
                        placeholder,
                        offset,
                        size,
                        MEM_REPLACE_PLACEHOLDER,
                        file->writeable ? PAGE_READWRITE : PAGE_READONLY,
                        NULL,
                        0);
}
vmem_placeholder_t vmem_placeholder_unmap(void* addr) {
  const vmem_placeholder_t placeholder = (vmem_placeholder_t)addr;
  UnmapViewOfFile2(GetCurrentProcess(), addr, MEM_PRESERVE_PLACEHOLDER);
  return placeholder;
}




json_status_t      vmem_manager_init(vmem_manager_t manager, json_u64_t addressSpaceSize) {
  json_fixed_allocator_init(&manager->splitNodePool, NULL, sizeof(struct vmem_split));
  const json_u32_t maxOrder = pow2_log2__(addressSpaceSize) - VMEM_BASE_ORDER;
  vmem_placeholder_t addressSpace = vmem_placeholder_reserve(addressSpaceSize, UINT_MAX + 1);
  manager->root.address  = addressSpace;
  manager->root.order    = maxOrder;
  manager->root.state    = PLACEHOLDER_STATE_FREE;
  manager->root.parent   = NULL;
  manager->root.children = NULL;
  return JSON_SUCCESS;
}
json_status_t      vmem_manager_cleanup(vmem_manager_t manager) {
  json_status_t result = vmem_release(manager);
  if ( result == JSON_SUCCESS )
    json_fixed_allocator_cleanup(&manager->splitNodePool);
  return result;
}


void*              vmem_alloc(vmem_manager_t manager, json_u64_t size) {
  const json_u64_t fullSize = bit_ceil__(size);
  const json_u64_t sizeOrder = pow2_log2__(fullSize) - VMEM_BASE_ORDER;


}
void               vmem_dealloc(vmem_manager_t manager, void* address, json_u64_t size) {
  const json_u64_t addrBits = (json_u64_t)address;

}




json_status_t json_vmem_manager_create(vmem_manager_t * pManager, json_u64_t virtualAddressSpaceSize);
json_status_t json_vmem_manager_destroy(vmem_manager_t manager);

json_u32_t    json_vmem_get_page_size(vmem_manager_t manager);
json_status_t json_vmem_get_base_page(vpage_t * pResultPage, void* address);

json_status_t json_vmem_reserve(vpage_t * pResultPage, vmem_manager_t manager, json_u64_t size);
json_status_t json_vmem_commit(vpage_t pageRange, json_u64_t size, json_u64_t offset);
json_status_t json_vmem_decommit(vpage_t pageRange);
json_status_t json_vmem_release(vpage_t pageRange);


