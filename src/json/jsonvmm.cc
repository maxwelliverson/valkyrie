//
// Created by maxwe on 2021-04-07.
//

#include "jsonmm.h"


#define NOMINMAX
#include <windows.h>
#include <memoryapi.h>


typedef struct json_virtual_page {
  json_byte_t data[];
} json_virtual_page;
typedef struct json_virtual_page_desc{
  json_virtual_page_t pageAddress;
  json_size_t         contiguousPageCount;
  json_bool_t         isCommitted;
} json_virtual_page_desc;
typedef struct json_virtual_page_manager{

  json_generic_stack_t    pageStack;

  json_virtual_page_desc* pageDescriptors;
  json_size_t             descriptorCount;
  json_u64_t              pageSize;
} json_virtual_page_manager;


extern "C"