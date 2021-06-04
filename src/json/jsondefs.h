//
// Created by maxwe on 2021-04-09.
//

#ifndef VALKYRIE_INTERNAL_JSON_DEFS_H
#define VALKYRIE_INTERNAL_JSON_DEFS_H

#include <limits.h>


#if defined(__BYTE_ORDER__)
# define JSON_BIG_ENDIAN    __ORDER_BIG_ENDIAN__
# define JSON_LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__
# define JSON_TARGET_ENDIAN __BYTE_ORDER__
# define JSON_IS_BIG_ENDIAN (JSON_TARGET_ENDIAN == JSON_BIG_ENDIAN)
# define JSON_IS_LITTLE_ENDIAN (JSON_TARGET_ENDIAN == JSON_LITTLE_ENDIAN)
#else
// Cross your fingers and pray you're compiling for a little endian processor
// This is almost always a safe bet.
# define JSON_BIG_ENDIAN    0xDABBED
# define JSON_LITTLE_ENDIAN 0xBA5ED
# define JSON_TARGET_ENDIAN JSON_LITTLE_ENDIAN
# define JSON_IS_BIG_ENDIAN 0
# define JSON_IS_LITTLE_ENDIAN 1
# endif






#if defined(__GNUC__) || defined(__clang__)
# define JSON_assume_aligned(p, align) __builtin_assume_aligned(p, align)
#else
# define JSON_assume_aligned(p, align) p
#endif







#define JSON_FIXED_SIZE_ALLOCATOR_COUNT 59
#define JSON_SMALL_OBJECT_MAX_SIZE      (UCHAR_MAX + 1)



#if !defined(JSON_MEM_SECURE)
#define JSON_MEM_SECURE 0
#endif

// Define JSON_DEBUG for debug mode
// #define JSON_DEBUG 1  // basic assertion checks and statistics, check double free, corrupted free list, and invalid pointer free.
// #define JSON_DEBUG 2  // + internal assertion checks
// #define JSON_DEBUG 3  // + extensive internal invariant checking (cmake -DJSON_DEBUG_FULL=ON)
#if !defined(JSON_DEBUG)
#if !defined(NDEBUG) || defined(_DEBUG)
#define JSON_DEBUG 2
#else
#define JSON_DEBUG 0
#endif
#endif

// Reserve extra padding at the end of each block to be more resilient against heap block overflows.
// The padding can detect byte-precise buffer overflow on free.
#if !defined(JSON_MEM_PADDING) && (JSON_DEBUG>=1)
#define JSON_MEM_PADDING  1
#endif


// Encoded free lists allow detection of corrupted free lists
// and can detect buffer overflows, modify after free, and double `free`s.
#if (JSON_MEM_SECURE>=3 || JSON_DEBUG>=1 || JSON_MEM_PADDING > 0)
#define JSON_MEM_ENCODE_FREELIST  1
#endif


#if (JSON_DEBUG>0)
#define json_trace_message(...)  json_trace_message__(__VA_ARGS__)
#else
#define json_trace_message(...)
#endif



#define JSON_GENERIC_VALUE_SIZE (8 + sizeof(void*))
#define JSON_GENERIC_VALUE_ALIGNMENT 16
#define JSON_INLINE_STRING_MAX_LENGTH (JSON_GENERIC_VALUE_SIZE - 2)


#define JSON_BIG_INTEGER_BIT_COUNT 3328
#define JSON_BIG_INTEGER_CAPACITY (JSON_BIG_INTEGER_BIT_COUNT / sizeof(json_u64_t))
#define JSON_BIG_INTEGER_TYPE_BIT (sizeof(json_u64_t) * 8)


#define JSON_BIG_FLOAT_EXPONENT_SIZE 20
#define JSON_BIG_FLOAT_FRACTION_SIZE 236
#define JSON_BIG_FLOAT_EXPONENT_BIAS 0x3FFFF





#define JSON_INTPTR_SHIFT 3

#define JSON_INTPTR_SIZE  (1<<JSON_INTPTR_SHIFT)
#define JSON_INTPTR_BITS  (JSON_INTPTR_SIZE*8)

#define KiB     ((size_t)1024)
#define MiB     (KiB*KiB)
#define GiB     (MiB*KiB)

#define JSON_SMALL_WSIZE_MAX  (128)
#define JSON_SMALL_SIZE_MAX   (JSON_SMALL_WSIZE_MAX*sizeof(void*))


#define JSON_SMALL_PAGE_SHIFT               (13 + JSON_INTPTR_SHIFT)      // 64kb
#define JSON_MEDIUM_PAGE_SHIFT              ( 3 + JSON_SMALL_PAGE_SHIFT)  // 512kb
#define JSON_LARGE_PAGE_SHIFT               ( 3 + JSON_MEDIUM_PAGE_SHIFT) // 4mb
#define JSON_SEGMENT_SHIFT                  ( JSON_LARGE_PAGE_SHIFT)      // 4mb


// Derived constants
#define JSON_SEGMENT_SIZE                   (1UL<<JSON_SEGMENT_SHIFT)
#define JSON_SEGMENT_MASK                   ((uintptr_t)JSON_SEGMENT_SIZE - 1)

#define JSON_SMALL_PAGE_SIZE                (1UL<<JSON_SMALL_PAGE_SHIFT)
#define JSON_MEDIUM_PAGE_SIZE               (1UL<<JSON_MEDIUM_PAGE_SHIFT)
#define JSON_LARGE_PAGE_SIZE                (1UL<<JSON_LARGE_PAGE_SHIFT)

#define JSON_SMALL_PAGES_PER_SEGMENT        (JSON_SEGMENT_SIZE/JSON_SMALL_PAGE_SIZE)
#define JSON_MEDIUM_PAGES_PER_SEGMENT       (JSON_SEGMENT_SIZE/JSON_MEDIUM_PAGE_SIZE)
#define JSON_LARGE_PAGES_PER_SEGMENT        (JSON_SEGMENT_SIZE/JSON_LARGE_PAGE_SIZE)

// The max object size are checked to not waste more than 12.5% internally over the page sizes.
// (Except for large pages since huge objects are allocated in 4MiB chunks)
#define JSON_SMALL_OBJ_SIZE_MAX             (JSON_SMALL_PAGE_SIZE/4)   // 16kb
#define JSON_MEDIUM_OBJ_SIZE_MAX            (JSON_MEDIUM_PAGE_SIZE/4)  // 128kb
#define JSON_LARGE_OBJ_SIZE_MAX             (JSON_LARGE_PAGE_SIZE/2)   // 2mb
#define JSON_LARGE_OBJ_WSIZE_MAX            (JSON_LARGE_OBJ_SIZE_MAX/JSON_INTPTR_SIZE)
#define JSON_HUGE_OBJ_SIZE_MAX              (2*JSON_INTPTR_SIZE*JSON_SEGMENT_SIZE)        // (must match JSON_REGION_MAX_ALLOC_SIZE in memory.c)

// Maximum number of size classes. (spaced exponentially in 12.5% increments)
#define JSON_BIN_HUGE  (73U)

#define JSON_HUGE_BLOCK_SIZE   ((json_u32_t)JSON_HUGE_OBJ_SIZE_MAX)


#if defined(__GNUC__) || defined(__clang__)
#define JSON_unlikely(x)     __builtin_expect((x),0)
#define JSON_likely(x)       __builtin_expect((x),1)
#else
#define JSON_unlikely(x)     (x)
#define JSON_likely(x)       (x)
#endif

#ifndef __has_builtin
#define __has_builtin(x)  0
#endif


#if (JSON_DEBUG)
// use our own assertion to print without memory allocation
void    json_assert_fail__(const char* assertion, const char* fname, unsigned int line, const char* func );
#define json_assert(expr)     ((expr) ? (void)0 : json_assert_fail__("Failed Assertion: ( " #expr " ) evaluated to false.",__FILE__,__LINE__,__func__))
#define json_assert_msg(expr, msg)     ((expr) ? (void)0 : json_assert_fail__(msg ": (" #expr ") evaluated to false.",__FILE__,__LINE__,__func__))
#else
#define json_assert(x)
#define json_assert_msg(expr, msg)
#endif

#if (JSON_DEBUG>1)
#define json_assert_internal    json_assert
#define json_assert_msg_internal json_assert_msg
#else
#define json_assert_internal(x)
#define json_assert_msg_internal(x, msg)
#endif

#if (JSON_DEBUG>2)
#define json_assert_expensive   json_assert
#define json_assert_expensive_msg json_assert
#else
#define json_assert_expensive(x)
#define json_assert_expensive_msg(x, msg)
#endif


#define UNUSED(x)     (void)(x)
#if (JSON_DEBUG>0)
#define UNUSED_RELEASE(x)
#else
#define UNUSED_RELEASE(x)  UNUSED(x)
#endif


#define JSON_INIT4(x)   x(),x(),x(),x()
#define JSON_INIT8(x)   JSON_INIT4(x),JSON_INIT4(x)
#define JSON_INIT16(x)  JSON_INIT8(x),JSON_INIT8(x)
#define JSON_INIT32(x)  JSON_INIT16(x),JSON_INIT16(x)
#define JSON_INIT64(x)  JSON_INIT32(x),JSON_INIT32(x)
#define JSON_INIT128(x) JSON_INIT64(x),JSON_INIT64(x)
#define JSON_INIT256(x) JSON_INIT128(x),JSON_INIT128(x)




#endif//VALKYRIE_INTERNAL_JSON_DEFS_H
