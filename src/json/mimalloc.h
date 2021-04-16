//
// Created by maxwe on 2021-04-10.
//

#ifndef VALKYRIE_JSON_MIMALLOC_H
#define VALKYRIE_JSON_MIMALLOC_H

#include <json/core.h>
#include "internal.h"


JSON_BEGIN_C_NAMESPACE


struct json_heap;
typedef struct json_heap json_heap_t;


// ------------------------------------------------------
// Standard malloc interface
// ------------------------------------------------------

JSON_nodiscard JSON_api JSON_restricted void* json_mem_malloc(size_t size)               JSON_noexcept JSON_malloc JSON_alloc_size(1);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_calloc(size_t count, size_t size) JSON_noexcept JSON_malloc JSON_alloc_size(1,2);
JSON_nodiscard JSON_api               void* json_mem_realloc(void* p, size_t newsize)  JSON_noexcept JSON_alloc_size(2);
JSON_api                              void* json_mem_expand(void* p, size_t newsize)   JSON_noexcept JSON_alloc_size(2);

JSON_api                              void  json_mem_free(void* p)                     JSON_noexcept;

JSON_nodiscard JSON_api JSON_restricted char* json_mem_strdup(const char* s) JSON_noexcept JSON_malloc; 
JSON_nodiscard JSON_api JSON_restricted char* json_mem_strndup(const char* s, size_t n) JSON_noexcept JSON_malloc;
JSON_nodiscard JSON_api JSON_restricted char* json_mem_realpath(const char* fname, char* resolved_name) JSON_noexcept JSON_malloc;

// ------------------------------------------------------
// Extended functionality
// ------------------------------------------------------
#define MI_SMALL_WSIZE_MAX  (128)
#define MI_SMALL_SIZE_MAX   (MI_SMALL_WSIZE_MAX*sizeof(void*))

JSON_nodiscard JSON_api JSON_restricted void* json_mem_malloc_small(size_t size) JSON_noexcept JSON_malloc JSON_alloc_size(1);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_zalloc_small(size_t size) JSON_noexcept JSON_malloc JSON_alloc_size(1);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_zalloc(size_t size)       JSON_noexcept JSON_malloc JSON_alloc_size(1);

JSON_nodiscard JSON_api JSON_restricted void* json_mem_mallocn(size_t count, size_t size) JSON_noexcept JSON_malloc JSON_alloc_size(1,2);
JSON_nodiscard JSON_api void* json_mem_reallocn(void* p, size_t count, size_t size)        JSON_noexcept JSON_alloc_size(2,3);
JSON_nodiscard JSON_api void* json_mem_reallocf(void* p, size_t newsize)                   JSON_noexcept JSON_alloc_size(2);

JSON_nodiscard JSON_api size_t json_mem_usable_size(const void* p) JSON_noexcept;
JSON_nodiscard JSON_api size_t json_mem_good_size(size_t size)     JSON_noexcept;


// ------------------------------------------------------
// Internals
// ------------------------------------------------------

typedef void (JSON_cdecl json_mem_deferred_free_fun)(json_bool_t force, unsigned long long heartbeat, void* arg);
JSON_api void json_mem_register_deferred_free(json_mem_deferred_free_fun* deferred_free, void* arg) JSON_noexcept;

typedef void (JSON_cdecl json_mem_output_fun)(const char* msg, void* arg);
JSON_api void json_mem_register_output(json_mem_output_fun* out, void* arg) JSON_noexcept;

typedef void (JSON_cdecl json_mem_error_fun)(int err, void* arg);
JSON_api void json_mem_register_error(json_mem_error_fun* fun, void* arg);

JSON_api void json_mem_collect(json_bool_t force)    JSON_noexcept;
JSON_api int  json_mem_version(void)          JSON_noexcept;
JSON_api void json_mem_stats_reset(void)      JSON_noexcept;
JSON_api void json_mem_stats_merge(void)      JSON_noexcept;
JSON_api void json_mem_stats_print(void* out) JSON_noexcept;  // backward compatibility: `out` is ignored and should be NULL
JSON_api void json_mem_stats_print_out(json_mem_output_fun* out, void* arg) JSON_noexcept;

JSON_api void json_mem_process_init(void)     JSON_noexcept;
JSON_api void json_mem_thread_init(void)      JSON_noexcept;
JSON_api void json_mem_thread_done(void)      JSON_noexcept;
JSON_api void json_mem_thread_stats_print_out(json_mem_output_fun* out, void* arg) JSON_noexcept;

JSON_api void json_mem_process_info(size_t* elapsed_msecs, size_t* user_msecs, size_t* system_msecs,
                                    size_t* current_rss, size_t* peak_rss,
                                    size_t* current_commit, size_t* peak_commit, size_t* page_faults) JSON_noexcept;

// -------------------------------------------------------------------------------------
// Aligned allocation
// Note that `alignment` always follows `size` for consistency with unaligned
// allocation, but unfortunately this differs from `posix_memalign` and `aligned_alloc`.
// -------------------------------------------------------------------------------------

JSON_nodiscard JSON_api JSON_restricted void* json_mem_malloc_aligned(size_t size, size_t alignment) JSON_noexcept JSON_malloc JSON_alloc_size(1) JSON_alloc_align(2);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_malloc_aligned_at(size_t size, size_t alignment, size_t offset) JSON_noexcept JSON_malloc JSON_alloc_size(1);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_zalloc_aligned(size_t size, size_t alignment) JSON_noexcept JSON_malloc JSON_alloc_size(1) JSON_alloc_align(2);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_zalloc_aligned_at(size_t size, size_t alignment, size_t offset) JSON_noexcept JSON_malloc JSON_alloc_size(1);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_calloc_aligned(size_t count, size_t size, size_t alignment) JSON_noexcept JSON_malloc JSON_alloc_size(1,2) JSON_alloc_align(3);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_calloc_aligned_at(size_t count, size_t size, size_t alignment, size_t offset) JSON_noexcept JSON_malloc JSON_alloc_size(1,2);
JSON_nodiscard JSON_api               void* json_mem_realloc_aligned(void* p, size_t newsize, size_t alignment) JSON_noexcept JSON_alloc_size(2) JSON_alloc_align(3);
JSON_nodiscard JSON_api               void* json_mem_realloc_aligned_at(void* p, size_t newsize, size_t alignment, size_t offset) JSON_noexcept JSON_alloc_size(2);


// -------------------------------------------------------------------------------------
// Heaps: first-class, but can only allocate from the same thread that created it.
// -------------------------------------------------------------------------------------


JSON_nodiscard JSON_api json_heap_t* json_mem_heap_new(void);
JSON_api void       json_mem_heap_delete(json_heap_t* heap);
JSON_api void       json_mem_heap_destroy(json_heap_t* heap);
JSON_api json_heap_t* json_mem_heap_set_default(json_heap_t* heap);
JSON_api json_heap_t* json_mem_heap_get_default(void);
JSON_api json_heap_t* json_mem_heap_get_backing(void);
JSON_api void       json_mem_heap_collect(json_heap_t* heap, json_bool_t force) JSON_noexcept;

JSON_nodiscard JSON_api JSON_restricted void* json_mem_heap_malloc(json_heap_t* heap, size_t size) JSON_noexcept JSON_malloc JSON_alloc_size(2);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_heap_zalloc(json_heap_t* heap, size_t size) JSON_noexcept JSON_malloc JSON_alloc_size(2);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_heap_calloc(json_heap_t* heap, size_t count, size_t size) JSON_noexcept JSON_malloc JSON_alloc_size(2, 3);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_heap_mallocn(json_heap_t* heap, size_t count, size_t size) JSON_noexcept JSON_malloc JSON_alloc_size(2, 3);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_heap_malloc_small(json_heap_t* heap, size_t size) JSON_noexcept JSON_malloc JSON_alloc_size(2);

JSON_nodiscard JSON_api void* json_mem_heap_realloc(json_heap_t* heap, void* p, size_t newsize)              JSON_noexcept JSON_alloc_size(3);
JSON_nodiscard JSON_api void* json_mem_heap_reallocn(json_heap_t* heap, void* p, size_t count, size_t size)  JSON_noexcept JSON_alloc_size(3,4);
JSON_nodiscard JSON_api void* json_mem_heap_reallocf(json_heap_t* heap, void* p, size_t newsize)             JSON_noexcept JSON_alloc_size(3);

JSON_nodiscard JSON_api JSON_restricted char* json_mem_heap_strdup(json_heap_t* heap, const char* s)            JSON_noexcept JSON_malloc;
JSON_nodiscard JSON_api JSON_restricted char* json_mem_heap_strndup(json_heap_t* heap, const char* s, size_t n) JSON_noexcept JSON_malloc;
JSON_nodiscard JSON_api JSON_restricted char* json_mem_heap_realpath(json_heap_t* heap, const char* fname, char* resolved_name) JSON_noexcept JSON_malloc;

JSON_nodiscard JSON_api JSON_restricted void* json_mem_heap_malloc_aligned(json_heap_t* heap, size_t size, size_t alignment) JSON_noexcept JSON_malloc JSON_alloc_size(2) JSON_alloc_align(3);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_heap_malloc_aligned_at(json_heap_t* heap, size_t size, size_t alignment, size_t offset) JSON_noexcept JSON_malloc JSON_alloc_size(2);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_heap_zalloc_aligned(json_heap_t* heap, size_t size, size_t alignment) JSON_noexcept JSON_malloc JSON_alloc_size(2) JSON_alloc_align(3);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_heap_zalloc_aligned_at(json_heap_t* heap, size_t size, size_t alignment, size_t offset) JSON_noexcept JSON_malloc JSON_alloc_size(2);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_heap_calloc_aligned(json_heap_t* heap, size_t count, size_t size, size_t alignment) JSON_noexcept JSON_malloc JSON_alloc_size(2, 3) JSON_alloc_align(4);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_heap_calloc_aligned_at(json_heap_t* heap, size_t count, size_t size, size_t alignment, size_t offset) JSON_noexcept JSON_malloc JSON_alloc_size(2, 3);
JSON_nodiscard JSON_api               void* json_mem_heap_realloc_aligned(json_heap_t* heap, void* p, size_t newsize, size_t alignment) JSON_noexcept JSON_alloc_size(3) JSON_alloc_align(4);
JSON_nodiscard JSON_api               void* json_mem_heap_realloc_aligned_at(json_heap_t* heap, void* p, size_t newsize, size_t alignment, size_t offset) JSON_noexcept JSON_alloc_size(3);


// --------------------------------------------------------------------------------
// Zero initialized re-allocation.
// Only valid on memory that was originally allocated with zero initialization too.
// e.g. `json_mem_calloc`, `json_mem_zalloc`, `json_mem_zalloc_aligned` etc.
// see <https://github.com/microsoft/mimalloc/issues/63#issuecomment-508272992>
// --------------------------------------------------------------------------------

JSON_nodiscard JSON_api void* json_mem_rezalloc(void* p, size_t newsize)                JSON_noexcept JSON_alloc_size(2);
JSON_nodiscard JSON_api void* json_mem_recalloc(void* p, size_t newcount, size_t size)  JSON_noexcept JSON_alloc_size(2,3);

JSON_nodiscard JSON_api void* json_mem_rezalloc_aligned(void* p, size_t newsize, size_t alignment) JSON_noexcept JSON_alloc_size(2) JSON_alloc_align(3);
JSON_nodiscard JSON_api void* json_mem_rezalloc_aligned_at(void* p, size_t newsize, size_t alignment, size_t offset) JSON_noexcept JSON_alloc_size(2);
JSON_nodiscard JSON_api void* json_mem_recalloc_aligned(void* p, size_t newcount, size_t size, size_t alignment) JSON_noexcept JSON_alloc_size(2,3) JSON_alloc_align(4);
JSON_nodiscard JSON_api void* json_mem_recalloc_aligned_at(void* p, size_t newcount, size_t size, size_t alignment, size_t offset) JSON_noexcept JSON_alloc_size(2,3);

JSON_nodiscard JSON_api void* json_mem_heap_rezalloc(json_heap_t* heap, void* p, size_t newsize)                JSON_noexcept JSON_alloc_size(3);
JSON_nodiscard JSON_api void* json_mem_heap_recalloc(json_heap_t* heap, void* p, size_t newcount, size_t size)  JSON_noexcept JSON_alloc_size(3,4);

JSON_nodiscard JSON_api void* json_mem_heap_rezalloc_aligned(json_heap_t* heap, void* p, size_t newsize, size_t alignment) JSON_noexcept JSON_alloc_size(3) JSON_alloc_align(4);
JSON_nodiscard JSON_api void* json_mem_heap_rezalloc_aligned_at(json_heap_t* heap, void* p, size_t newsize, size_t alignment, size_t offset) JSON_noexcept JSON_alloc_size(3);
JSON_nodiscard JSON_api void* json_mem_heap_recalloc_aligned(json_heap_t* heap, void* p, size_t newcount, size_t size, size_t alignment) JSON_noexcept JSON_alloc_size(3,4) JSON_alloc_align(5);
JSON_nodiscard JSON_api void* json_mem_heap_recalloc_aligned_at(json_heap_t* heap, void* p, size_t newcount, size_t size, size_t alignment, size_t offset) JSON_noexcept JSON_alloc_size(3,4);


// ------------------------------------------------------
// Analysis
// ------------------------------------------------------

JSON_api json_bool_t json_mem_heap_contains_block(json_heap_t* heap, const void* p);
JSON_api json_bool_t json_mem_heap_check_owned(json_heap_t* heap, const void* p);
JSON_api json_bool_t json_mem_check_owned(const void* p);

// An area of heap space contains blocks of a single size.
typedef struct json_mem_heap_area_s {
  void*  blocks;      // start of the area containing heap blocks
  size_t reserved;    // bytes reserved for this area (virtual)
  size_t committed;   // current available bytes for this area
  size_t used;        // bytes in use by allocated blocks
  size_t block_size;  // size in bytes of each block
} json_mem_heap_area_t;

typedef json_bool_t(JSON_cdecl json_mem_block_visit_fun)(const json_heap_t* heap, const json_mem_heap_area_t* area, void* block, size_t block_size, void* arg);

JSON_api json_bool_t json_mem_heap_visit_blocks(const json_heap_t* heap, json_bool_t visit_all_blocks, json_mem_block_visit_fun* visitor, void* arg);

// Experimental
JSON_nodiscard JSON_api json_bool_t json_mem_is_in_heap_region(const void* p) JSON_noexcept;
JSON_nodiscard JSON_api json_bool_t json_mem_is_redirected(void) JSON_noexcept;

JSON_api int json_mem_reserve_huge_os_pages_interleave(size_t pages, size_t numa_nodes, size_t timeout_msecs) JSON_noexcept;
JSON_api int json_mem_reserve_huge_os_pages_at(size_t pages, int numa_node, size_t timeout_msecs) JSON_noexcept;

JSON_api int         json_mem_reserve_os_memory(size_t size, json_bool_t commit, json_bool_t allow_large) JSON_noexcept;
JSON_api json_bool_t json_mem_manage_os_memory(void* start, size_t size, json_bool_t is_committed, json_bool_t is_large, json_bool_t is_zero, int numa_node) JSON_noexcept;


// deprecated
JSON_api int  json_mem_reserve_huge_os_pages(size_t pages, double max_secs, size_t* pages_reserved) JSON_noexcept;


// ------------------------------------------------------
// Convenience
// ------------------------------------------------------

#define json_mem_malloc_tp(tp)                ((tp*)json_mem_malloc(sizeof(tp)))
#define json_mem_zalloc_tp(tp)                ((tp*)json_mem_zalloc(sizeof(tp)))
#define json_mem_calloc_tp(tp,n)              ((tp*)json_mem_calloc(n,sizeof(tp)))
#define json_mem_mallocn_tp(tp,n)             ((tp*)json_mem_mallocn(n,sizeof(tp)))
#define json_mem_reallocn_tp(p,tp,n)          ((tp*)json_mem_reallocn(p,n,sizeof(tp)))
#define json_mem_recalloc_tp(p,tp,n)          ((tp*)json_mem_recalloc(p,n,sizeof(tp)))

#define json_mem_heap_malloc_tp(hp,tp)        ((tp*)json_mem_heap_malloc(hp,sizeof(tp)))
#define json_mem_heap_zalloc_tp(hp,tp)        ((tp*)json_mem_heap_zalloc(hp,sizeof(tp)))
#define json_mem_heap_calloc_tp(hp,tp,n)      ((tp*)json_mem_heap_calloc(hp,n,sizeof(tp)))
#define json_mem_heap_mallocn_tp(hp,tp,n)     ((tp*)json_mem_heap_mallocn(hp,n,sizeof(tp)))
#define json_mem_heap_reallocn_tp(hp,p,tp,n)  ((tp*)json_mem_heap_reallocn(hp,p,n,sizeof(tp)))
#define json_mem_heap_recalloc_tp(hp,p,tp,n)  ((tp*)json_mem_heap_recalloc(hp,p,n,sizeof(tp)))


// ------------------------------------------------------
// Options, all `false` by default
// ------------------------------------------------------

typedef enum json_mem_option_e {
  // stable options
  json_mem_option_show_errors,
  json_mem_option_show_stats,
  json_mem_option_verbose,
  // the following options are experimental
  json_mem_option_eager_commit,
  json_mem_option_eager_region_commit,
  json_mem_option_reset_decommits,
  json_mem_option_large_os_pages,         // implies eager commit
  json_mem_option_reserve_huge_os_pages,
  json_mem_option_reserve_os_memory,
  json_mem_option_segment_cache,
  json_mem_option_page_reset,
  json_mem_option_abandoned_page_reset,
  json_mem_option_segment_reset,
  json_mem_option_eager_commit_delay,
  json_mem_option_reset_delay,
  json_mem_option_use_numa_nodes,
  json_mem_option_limit_os_alloc,
  json_mem_option_os_tag,
  json_mem_option_max_errors,
  json_mem_option_max_warnings,
  _json_option_last
} json_mem_option_t;


JSON_nodiscard JSON_api json_bool_t json_mem_option_is_enabled(json_mem_option_t option);
JSON_api void                       json_mem_option_enable(json_mem_option_t option);
JSON_api void                       json_mem_option_disable(json_mem_option_t option);
JSON_api void                       json_mem_option_set_enabled(json_mem_option_t option, json_bool_t enable);
JSON_api void                       json_mem_option_set_enabled_default(json_mem_option_t option, json_bool_t enable);

JSON_nodiscard JSON_api long        json_mem_option_get(json_mem_option_t option);
JSON_api void                       json_mem_option_set(json_mem_option_t option, long value);
JSON_api void                       json_mem_option_set_default(json_mem_option_t option, long value);


// -------------------------------------------------------------------------------------------------------
// "json_mem" prefixed implementations of various posix, Unix, Windows, and C++ allocation functions.
// (This can be convenient when providing overrides of these functions as done in `mimalloc-override.h`.)
// note: we use `json_mem_cfree` as "checked free" and it checks if the pointer is in our heap before free-ing.
// -------------------------------------------------------------------------------------------------------

JSON_api void  json_mem_cfree(void* p) JSON_noexcept;
JSON_api void* json_mem__expand(void* p, size_t newsize) JSON_noexcept;
JSON_nodiscard JSON_api size_t json_mem_malloc_size(const void* p)        JSON_noexcept;
JSON_nodiscard JSON_api size_t json_mem_malloc_usable_size(const void *p) JSON_noexcept;

JSON_api int json_mem_posix_memalign(void** p, size_t alignment, size_t size)   JSON_noexcept;
JSON_nodiscard JSON_api JSON_restricted void* json_mem_memalign(size_t alignment, size_t size) JSON_noexcept JSON_malloc JSON_alloc_size(2) JSON_alloc_align(1);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_valloc(size_t size)  JSON_noexcept JSON_malloc JSON_alloc_size(1);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_pvalloc(size_t size) JSON_noexcept JSON_malloc JSON_alloc_size(1);
JSON_nodiscard JSON_api JSON_restricted void* json_mem_aligned_alloc(size_t alignment, size_t size) JSON_noexcept JSON_malloc JSON_alloc_size(2) JSON_alloc_align(1);

JSON_nodiscard JSON_api void* json_mem_reallocarray(void* p, size_t count, size_t size) JSON_noexcept JSON_alloc_size(2,3);
JSON_nodiscard JSON_api void* json_mem_aligned_recalloc(void* p, size_t newcount, size_t size, size_t alignment) JSON_noexcept;
JSON_nodiscard JSON_api void* json_mem_aligned_offset_recalloc(void* p, size_t newcount, size_t size, size_t alignment, size_t offset) JSON_noexcept;

JSON_nodiscard JSON_api JSON_restricted unsigned short* json_mem_wcsdup(const unsigned short* s) JSON_noexcept JSON_malloc;
JSON_nodiscard JSON_api JSON_restricted unsigned char*  json_mem_mbsdup(const unsigned char* s)  JSON_noexcept JSON_malloc;
JSON_api int json_mem_dupenv_s(char** buf, size_t* size, const char* name)                      JSON_noexcept;
JSON_api int json_mem_wdupenv_s(unsigned short** buf, size_t* size, const unsigned short* name) JSON_noexcept;

JSON_api void json_mem_free_size(void* p, size_t size)                           JSON_noexcept;
JSON_api void json_mem_free_size_aligned(void* p, size_t size, size_t alignment) JSON_noexcept;
JSON_api void json_mem_free_aligned(void* p, size_t alignment)                   JSON_noexcept;




#pragma region Type Definitions

// The free lists use encoded next fields
// (Only actually encodes when JSON_ENCODED_FREELIST is defined.)
typedef uintptr_t json_encoded_t;

// free lists contain blocks
typedef struct json_block {
  json_encoded_t next;
} json_block_t;


// The delayed flags are used for efficient multi-threaded free-ing
typedef enum json_delayed_e {
  JSON_USE_DELAYED_FREE   = 0, // push on the owning heap thread delayed list
  JSON_DELAYED_FREEING    = 1, // temporary: another thread is accessing the owning heap
  JSON_NO_DELAYED_FREE    = 2, // optimize: push on page local thread free queue if another block is already in the heap thread delayed free list
  JSON_NEVER_DELAYED_FREE = 3  // sticky, only resets on page reclaim
} json_delayed_t;


// The `in_full` and `has_aligned` page flags are put in a union to efficiently
// test if both are false (`full_aligned == 0`) in the `json_free` routine.
#if !JSON_TSAN
typedef union json_page_flags {
  json_u8_t full_aligned;
  struct {
    json_u8_t in_full : 1;
    json_u8_t has_aligned : 1;
  } x;
} json_page_flags_t;
#else
// under thread sanitizer, use a byte for each flag to suppress warning, issue #130
typedef union json_page_flags {
  json_u16_t full_aligned;
  struct {
    json_u8_t in_full;
    json_u8_t has_aligned;
  } x;
} json_page_flags_t;
#endif

// Thread free list.
// We use the bottom 2 bits of the pointer for json_delayed_t flags
typedef uintptr_t json_thread_free_t;

// A page contains blocks of one specific size (`block_size`).
// Each page has three list of free blocks:
// `free` for blocks that can be allocated,
// `local_free` for freed blocks that are not yet available to `json_malloc`
// `thread_free` for freed blocks by other threads
// The `local_free` and `thread_free` lists are migrated to the `free` list
// when it is exhausted. The separate `local_free` list is necessary to
// implement a monotonic heartbeat. The `thread_free` list is needed for
// avoiding atomic operations in the common case.
//
//
// `used - |thread_free|` == actual blocks that are in use (alive)
// `used - |thread_free| + |free| + |local_free| == capacity`
//
// We don't count `freed` (as |free|) but use `used` to reduce
// the number of memory accesses in the `json_page_all_free` function(s).
//
// Notes:
// - Access is optimized for `json_free` and `json_page_alloc` (in `alloc.c`)
// - Using `json_u16_t` does not seem to slow things down
// - The size is 8 words on 64-bit which helps the page index calculations
//   (and 10 words on 32-bit, and encoded free lists add 2 words. Sizes 10
//    and 12 are still good for address calculation)
// - To limit the structure size, the `xblock_size` is 32-bits only; for
//   blocks > JSON_HUGE_BLOCK_SIZE the size is determined from the segment page size
// - `thread_free` uses the bottom bits as a delayed-free flags to optimize
//   concurrent frees where only the first concurrent free adds to the owning
//   heap `thread_delayed_free` list (see `alloc.c:json_free_block_mt`).
//   The invariant is that no-delayed-free is only set if there is
//   at least one block that will be added, or as already been added, to
//   the owning heap `thread_delayed_free` list. This guarantees that pages
//   will be freed correctly even if only other threads free blocks.
typedef struct json_page {
  // "owned" by the segment
  json_u8_t               segment_idx;       // index in the segment `pages` array, `page == &segment->pages[page->segment_idx]`
  json_u8_t               segment_in_use:1;  // `true` if the segment allocated this page
  json_u8_t               is_reset:1;        // `true` if the page memory was reset
  json_u8_t               is_committed:1;    // `true` if the page virtual memory is committed
  json_u8_t               is_zero_init:1;    // `true` if the page was zero initialized

  // layout like this to optimize access in `json_malloc` and `json_free`
  json_u16_t              capacity;          // number of blocks committed, must be the first field, see `segment.c:page_clear`
  json_u16_t              reserved;          // number of blocks reserved in memory
  json_page_flags_t       flags;             // `in_full` and `has_aligned` flags (8 bits)
  json_u8_t               is_zero:1;         // `true` if the blocks in the free list are zero initialized
  json_u8_t               retire_expire:7;   // expiration count for retired blocks

  json_block_t*           free;              // list of available free blocks (`malloc` allocates from this list)
#ifdef JSON_MEM_ENCODE_FREELIST
  uintptr_t             keys[2];           // two random keys to encode the free lists (see `_json_block_next`)
#endif
  json_u32_t              used;              // number of blocks in use (including blocks in `local_free` and `thread_free`)
  json_u32_t              xblock_size;       // size available in each block (always `>0`)

  json_block_t*           local_free;        // list of deferred free blocks by this thread (migrates to `free`)
  _Atomic(json_thread_free_t) xthread_free;  // list of deferred free blocks freed by other threads
  _Atomic(uintptr_t)        xheap;

  struct json_page*     next;              // next page owned by this thread with the same `block_size`
  struct json_page*     prev;              // previous page owned by this thread with the same `block_size`
} json_page_t;



typedef enum json_page_kind_e {
  JSON_PAGE_SMALL,    // small blocks go into 64kb pages inside a segment
  JSON_PAGE_MEDIUM,   // medium blocks go into 512kb pages inside a segment
  JSON_PAGE_LARGE,    // larger blocks go into a single page spanning a whole segment
  JSON_PAGE_HUGE      // huge blocks (>512kb) are put into a single page in a segment of the exact size (but still 2mb aligned)
} json_page_kind_t;

// Segments are large allocated memory blocks (2mb on 64 bit) from
// the OS. Inside segments we allocated fixed size _pages_ that
// contain blocks.
typedef struct json_segment {
  // memory fields
  size_t               memid;            // id for the os-level memory manager
  json_bool_t                 mem_is_pinned;    // `true` if we cannot decommit/reset/protect in this memory (i.e. when allocated using large OS pages)
  json_bool_t                 mem_is_committed; // `true` if the whole segment is eagerly committed

  // segment fields
  _Atomic(struct json_segment*) abandoned_next;
  struct json_segment* next;             // must be the first segment field after abandoned_next -- see `segment.c:segment_init`
  struct json_segment* prev;

  size_t               abandoned;        // abandoned pages (i.e. the original owning thread stopped) (`abandoned <= used`)
  size_t               abandoned_visits; // count how often this segment is visited in the abandoned list (to force reclaim it it is too long)

  size_t               used;             // count of pages in use (`used <= capacity`)
  size_t               capacity;         // count of available pages (`#free + used`)
  size_t               segment_size;     // for huge pages this may be different from `JSON_SEGMENT_SIZE`
  size_t               segment_info_size;// space we are using from the first page for segment meta-data and possible guard pages.
  uintptr_t            cookie;           // verify addresses in secure mode: `_json_ptr_cookie(segment) == segment->cookie`

  // layout like this to optimize access in `json_free`
  size_t               page_shift;       // `1 << page_shift` == the page sizes == `page->block_size * page->reserved` (unless the first page, then `-segment_info_size`).
  _Atomic(uintptr_t)   thread_id;        // unique id of the thread owning this segment
  json_page_kind_t       page_kind;        // kind of pages: small, large, or huge
  json_page_t            pages[1];         // up to `JSON_SMALL_PAGES_PER_SEGMENT` pages
} json_segment_t;


// ------------------------------------------------------
// Heaps
// Provide first-class heaps to allocate from.
// A heap just owns a set of pages for allocation and
// can only be allocate/reallocate from the thread that created it.
// Freeing blocks can be done from any thread though.
// Per thread, the segments are shared among its heaps.
// Per thread, there is always a default heap that is
// used for allocation; it is initialized to statically
// point to an empty heap to avoid initialization checks
// in the fast path.
// ------------------------------------------------------

// Thread local data
typedef struct json_tld json_tld_t;

// Pages of a certain block size are held in a queue.
typedef struct json_page_queue {
  json_page_t* first;
  json_page_t* last;
  size_t     block_size;
} json_page_queue_t;

#define JSON_BIN_FULL  (JSON_BIN_HUGE+1)

// Random context
typedef struct json_random_cxt {
  json_u32_t input[16];
  json_u32_t output[16];
  int      output_available;
} json_random_ctx_t;


// In debug mode there is a padding stucture at the end of the blocks to check for buffer overflows
#if (JSON_MEM_PADDING)
typedef struct json_padding {
  json_u32_t canary; // encoded block value to check validity of the padding (in case of overflow)
  json_u32_t delta;  // padding bytes before the block. (json_usable_size(p) - delta == exact allocated bytes)
} json_padding_t;
#define JSON_PADDING_SIZE   (sizeof(json_padding_t))
#define JSON_PADDING_WSIZE  ((JSON_PADDING_SIZE + JSON_INTPTR_SIZE - 1) / JSON_INTPTR_SIZE)
#else
#define JSON_PADDING_SIZE   0
#define JSON_PADDING_WSIZE  0
#endif

#define JSON_PAGES_DIRECT   (JSON_SMALL_WSIZE_MAX + JSON_PADDING_WSIZE + 1)


// A heap owns a set of pages.
struct json_heap {
  json_tld_t*             tld;
  json_page_t*            pages_free_direct[JSON_PAGES_DIRECT];// optimize: array where every entry points a page with possibly free blocks in the corresponding queue for that size.
  json_page_queue_t       pages[JSON_BIN_FULL + 1];            // queue of pages for each size class (or "bin")
  _Atomic(json_block_t*)  thread_delayed_free;
  uintptr_t               thread_id;                           // thread this heap belongs too
  uintptr_t               cookie;                              // random cookie to verify pointers (see `_json_ptr_cookie`)
  uintptr_t               keys[2];                             // two random keys used to encode the `thread_delayed_free` list
  json_random_ctx_t       random;                              // random number context used for secure allocation
  size_t                  page_count;                          // total number of pages in the `pages` queues.
  size_t                  page_retired_min;                    // smallest retired index (retired pages are fully free, but still in the page queues)
  size_t                  page_retired_max;                    // largest retired index into the `pages` array.
  json_heap_t*            next;                                // list of heaps per thread
  json_bool_t             no_reclaim;                          // `true` if this heap should not reclaim abandoned pages
};




// ------------------------------------------------------
// Debug
// ------------------------------------------------------

#define JSON_DEBUG_UNINIT     (0xD0)
#define JSON_DEBUG_FREED      (0xDF)
#define JSON_DEBUG_PADDING    (0xDE)

typedef enum json_mem_debug_pattern{
  JSON_MEM_DEBUG_PATTERN_UNINIT  = 0xD0,
  JSON_MEM_DEBUG_PATTERN_FREED   = 0xDF,
  JSON_MEM_DEBUG_PATTERN_PADDING = 0xDE
} json_mem_debug_pattern_t;



// ------------------------------------------------------
// Statistics
// ------------------------------------------------------

#ifndef JSON_COLLECT_MEM_STATS
#if (JSON_DEBUG>0)
#define JSON_COLLECT_MEM_STATS 2
#else
#define JSON_COLLECT_MEM_STATS 0
#endif
#endif

typedef struct json_stat_count {
  json_u64_t allocated;
  json_u64_t freed;
  json_u64_t peak;
  json_u64_t current;
} json_stat_count_t;
typedef struct json_stat_counter {
  json_u64_t total;
  json_u64_t count;
} json_stat_counter_t;
typedef struct json_stats {
  json_stat_count_t segments;
  json_stat_count_t pages;
  json_stat_count_t reserved;
  json_stat_count_t committed;
  json_stat_count_t reset;
  json_stat_count_t page_committed;
  json_stat_count_t segments_abandoned;
  json_stat_count_t pages_abandoned;
  json_stat_count_t threads;
  json_stat_count_t normal;
  json_stat_count_t huge;
  json_stat_count_t giant;
  json_stat_count_t malloc;
  json_stat_count_t segments_cache;
  json_stat_counter_t pages_extended;
  json_stat_counter_t mmap_calls;
  json_stat_counter_t commit_calls;
  json_stat_counter_t page_no_retire;
  json_stat_counter_t searches;
  json_stat_counter_t normal_count;
  json_stat_counter_t huge_count;
  json_stat_counter_t giant_count;
#if JSON_COLLECT_MEM_STATS>1
  json_stat_count_t normal_bins[JSON_BIN_HUGE+1];
#endif
} json_stats_t;


void json_stat_increase_(json_stat_count_t* stat, size_t amount);
void json_stat_decrease_(json_stat_count_t* stat, size_t amount);
void json_stat_counter_increase_(json_stat_counter_t* stat, size_t amount);

#if (JSON_COLLECT_MEM_STATS)
#define json_stat_increase(stat,amount)         json_stat_increase_( &(stat), amount)
#define json_stat_decrease(stat,amount)         json_stat_decrease_( &(stat), amount)
#define json_stat_counter_increase(stat,amount) json_stat_counter_increase_( &(stat), amount)
#else
#define json_stat_increase(stat,amount)         (void)0
#define json_stat_decrease(stat,amount)         (void)0
#define json_stat_counter_increase(stat,amount) (void)0
#endif

#define json_heap_stat_counter_increase(heap,stat,amount)  json_stat_counter_increase( (heap)->tld->stats.stat, amount)
#define json_heap_stat_increase(heap,stat,amount)  json_stat_increase( (heap)->tld->stats.stat, amount)
#define json_heap_stat_decrease(heap,stat,amount)  json_stat_decrease( (heap)->tld->stats.stat, amount)

// ------------------------------------------------------
// Thread Local data
// ------------------------------------------------------

typedef json_u64_t  json_msecs_t;

// Queue of segments
typedef struct json_segment_queue {
  json_segment_t* first;
  json_segment_t* last;
} json_segment_queue_t;

// OS thread local data
typedef struct json_os_tld {
  size_t                region_idx;   // start point for next allocation
  json_stats_t*           stats;        // points to tld stats
} json_os_tld_t;

// Segments thread local data
typedef struct json_segments_tld {
  json_segment_queue_t  small_free;   // queue of segments with free small pages
  json_segment_queue_t  medium_free;  // queue of segments with free medium pages
  json_page_queue_t     pages_reset;  // queue of freed pages that can be reset
  size_t              count;        // current number of segments;
  size_t              peak_count;   // peak number of segments
  size_t              current_size; // current size of all segments
  size_t              peak_size;    // peak size of all segments
  size_t              cache_count;  // number of segments in the cache
  size_t              cache_size;   // total size of all segments in the cache
  json_segment_t*       cache;        // (small) cache of segments
  json_stats_t*         stats;        // points to tld stats
  json_os_tld_t*        os;           // points to os stats
} json_segments_tld_t;

// Thread local data
struct json_tld {
  unsigned long long    heartbeat;     // monotonic heartbeat count
  json_bool_t           recurse;       // true if deferred was called; used to prevent infinite recursion.
  json_heap_t*          heap_backing;  // backing heap of this thread (cannot be deleted)
  json_heap_t*          heaps;         // list of heaps in this thread (so we can abandon all when the thread terminates)
  json_segments_tld_t   segments;      // segment tld
  json_os_tld_t         os;            // os tld
  json_stats_t          stats;         // statistics
};


#pragma endregion

JSON_END_C_NAMESPACE



#endif//VALKYRIE_JSON_MIMALLOC_H
