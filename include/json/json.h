//
// Created by Maxwell on 2021-02-13.
//

#ifndef VALKYRIE_JSON_VALUE_H
#define VALKYRIE_JSON_VALUE_H


#if !defined(_MSC_VER)
#define JSON_MAY_ALIAS __attribute__((__may_alias__))
#else
#define JSON_MAY_ALIAS
#endif


#if defined(__cplusplus)
#define JSON_BEGIN_C_NAMESPACE extern "C" {
#define JSON_END_C_NAMESPACE }
#else
#define JSON_BEGIN_C_NAMESPACE
#define JSON_END_C_NAMESPACE
#endif


#define JSON_GENERIC_VALUE_SIZE (8 + sizeof(void*))

#define JSON_BIG_INTEGER_BIT_COUNT 3328
#define JSON_BIG_INTEGER_CAPACITY (JSON_BIG_INTEGER_BIT_COUNT / sizeof(json_u64_t))
#define JSON_BIG_INTEGER_TYPE_BIT (sizeof(json_u64_t) * 8)

#define JSON_BIG_FLOAT_EXPONENT_SIZE 20
#define JSON_BIG_FLOAT_FRACTION_SIZE 236
#define JSON_BIG_FLOAT_EXPONENT_BIAS 0x3FFFF

JSON_BEGIN_C_NAMESPACE

// Generic Types

typedef struct JSON_MAY_ALIAS json_byte { unsigned char : 8; } json_byte_t;
typedef json_byte_t*                                           json_address_t;
typedef const json_byte_t*                                     json_const_address_t;





// Implementation Types

typedef struct json_symbol*            json_symbol_t;

typedef struct json_value_pool*        json_value_pool_t;
typedef struct json_object_pool*       json_object_pool_t;
typedef struct json_string_pool*       json_string_pool_t;
typedef struct json_string_pool_entry* json_string_pool_entry_t;
typedef struct json_context*           json_context_t;



//struct json_pooled_string;
struct json_string_ref;
struct json_inline_string;

struct json_big_integer;
struct json_big_float;
struct json_integer;
struct json_float;

typedef short              json_i16_t;
typedef unsigned short     json_u16_t;
typedef int                json_i32_t;
typedef unsigned           json_u32_t;
typedef long long          json_i64_t;
typedef unsigned long long json_u64_t;
typedef float              json_f32_t;
typedef double             json_f64_t;

typedef int                json_bool_t;
#define JSON_FALSE         0
#define JSON_TRUE          1

typedef json_u32_t         json_size_t;

typedef struct json_utf8 {
  union {
    struct {
      unsigned char                 : 3;
      unsigned char invalid_4_bytes : 1;
      unsigned char not_3_bytes     : 1;
      unsigned char not_2_bytes     : 1;
      unsigned char is_initial_byte : 1;
      unsigned char is_multibyte    : 1;
    }  meta;
    json_byte_t   byte;
  };
} json_utf8_t;




// api Types

struct json_document;


struct json_value;
struct json_member;
struct json_number;
struct json_object;
struct json_array;
struct json_string;



typedef enum json_type{
  JSON_NULL_TYPE           = 0x0,

  JSON_FALSE_TYPE          = 0x1,
  JSON_TRUE_TYPE           = 0x2,

  JSON_POOLED_STRING_TYPE  = 0x4,
  JSON_INLINE_STRING_TYPE  = 0x8,
  JSON_STRING_REF_TYPE     = 0x10,

  JSON_INT64_TYPE          = 0x20,
  JSON_UINT64_TYPE         = 0x40,
  JSON_FLOAT64_TYPE        = 0x80,
  JSON_BIG_INT_TYPE        = 0x100,
  JSON_BIG_FLOAT_TYPE      = 0x200,

  JSON_OBJECT_TYPE         = 0x400,

  JSON_ARRAY_TYPE          = 0x800,


  JSON_BOOLEAN_TYPE_MASK   = JSON_TRUE_TYPE | JSON_FALSE_TYPE,
  JSON_STRING_TYPE_MASK    = JSON_INLINE_STRING_TYPE | JSON_STRING_REF_TYPE | JSON_POOLED_STRING_TYPE,
  JSON_INTEGER_TYPE_MASK   = JSON_INT64_TYPE | JSON_UINT64_TYPE | JSON_BIG_INT_TYPE,
  JSON_FLOAT_TYPE_MASK     = JSON_FLOAT64_TYPE | JSON_BIG_FLOAT_TYPE,
  JSON_NUMBER_TYPE_MASK    = JSON_INTEGER_TYPE_MASK | JSON_FLOAT_TYPE_MASK
}              json_type_t;



typedef enum json_status{
  JSON_SUCCESS,
  JSON_ERROR_UNKNOWN,
  JSON_ERROR_POOL_OUT_OF_MEMORY,
  JSON_ERROR_POOL_CORRUPTED,
  JSON_ERROR_POOL_TOO_FRAGMENTED,
  JSON_ERROR_NOT_IMPLEMENTED,
  JSON_ERROR_COLLECTION_IS_EMPTY,
  JSON_ERROR_COLLECTION_IS_FULL,
  JSON_ERROR_INVALID_ARGUMENT
}            json_status_t;
typedef enum json_parse_status{
  JSON_PARSE_SUCCESS,

  JSON_PARSE_ERROR_DOCUMENT_EMPTY,
  JSON_PARSE_ERROR_DOCUMENT_ROOT_NOT_SINGULAR,
  JSON_PARSE_ERROR_INVALID_VALUE,

  JSON_PARSE_ERROR_OBJ_MISSING_NAME,
  JSON_PARSE_ERROR_OBJ_MISSING_COLON,
  JSON_PARSE_ERROR_OBJ_MISSING_COMMA_OR_CURLY_BRACKET,

  JSON_PARSE_ERROR_ARRAY_MISSING_COMMA_OR_SQUARE_BRACKET,

  JSON_PARSE_ERROR_STRING_UNICODE_ESCAPE_INVALID_HEX,
  JSON_PARSE_ERROR_STRING_UNICODE_INVALID_SURROGATE,
  JSON_PARSE_ERROR_STRING_INVALID_ESCAPE_SEQUENCE,
  JSON_PARSE_ERROR_STRING_MISSING_QUOTATION_MARK,
  JSON_PARSE_ERROR_STRING_INVALID_ENCODING,

  JSON_PARSE_ERROR_NUMBER_MISSING_FRACTION,
  JSON_PARSE_ERROR_NUMBER_MISSING_EXPONENT,

  JSON_PARSE_ERROR_TERMINATED,
  JSON_PARSE_ERROR_UNKNOWN_SYNTAX_ERROR
}      json_parse_status_t;
typedef enum json_validation_status{
  JSON_VALIDATION_SUCCESS,
  JSON_VALIDATION_ERROR_MULTIPLE_OF,
  JSON_VALIDATION_ERROR_MAXIMUM,
  JSON_VALIDATION_ERROR_EXCLUSIVE_MAXIMUM,
  JSON_VALIDATION_ERROR_MINIMUM,
  JSON_VALIDATION_ERROR_EXCLUSIVE_MINIMUM,

  JSON_VALIDATION_ERROR_MAX_LENGTH,
  JSON_VALIDATION_ERROR_MIN_LENGTH,
  JSON_VALIDATION_ERROR_PATTERN,

  JSON_VALIDATION_ERROR_MAX_ITEMS,
  JSON_VALIDATION_ERROR_MIN_ITEMS,
  JSON_VALIDATION_ERROR_UNIQUE_ITEMS,
  JSON_VALIDATION_ERROR_ADDITIONAL_ITEMS,

  JSON_VALIDATION_ERROR_MAX_PROPERTIES,
  JSON_VALIDATION_ERROR_MIN_PROPERTIES,
  JSON_VALIDATION_ERROR_REQUIRED,
  JSON_VALIDATION_ERROR_ADDITIONAL_PROPERTIES,
  JSON_VALIDATION_ERROR_PATTERN_PROPERTIES,
  JSON_VALIDATION_ERROR_DEPENDENCIES,

  JSON_VALIDATION_ERROR_ENUM,
  JSON_VALIDATION_ERROR_TYPE,

  JSON_VALIDATION_ERROR_ONE_OF,
  JSON_VALIDATION_ERROR_ONE_OF_MATCH,
  JSON_VALIDATION_ERROR_ALL_OF,
  JSON_VALIDATION_ERROR_ANY_OF,
  JSON_VALIDATION_ERROR_NOT
} json_validation_status_t;

typedef unsigned short json_type_flags_t;


typedef struct json_big_integer{
  json_u64_t  m_digits[JSON_BIG_INTEGER_CAPACITY];
  json_size_t m_size;
} json_big_integer_t;
typedef struct json_big_float{
  union{
    struct {
      json_u64_t : 0;
      json_u64_t : 0;
      json_u64_t : 0;
      json_u64_t : 0;
    } m_bits;
    struct {
      json_u64_t m_frac_lo_bits[3];
      json_u64_t m_frac_hi_bits : 44;
      json_i32_t m_exp_bits     : 20;
    };
  };
}   json_big_float_t;



typedef struct json_object* json_object_t;



/*typedef struct json_inline_string{
  json_utf8_t str[JSON_GENERIC_VALUE_SIZE];
} json_inline_string_t;
typedef struct json_pooled_string{

} json_pooled_string_t;*/
typedef struct json_string_ref   {
  json_size_t        length;
  const json_utf8_t* p_data;
} json_string_ref_t;
typedef struct json_value{
  union{
    struct {
      json_byte_t padding_[JSON_GENERIC_VALUE_SIZE - 2];
      json_type_flags_t flags;
    }         type;
    struct {
      json_u64_t    hint;
      json_object_t handle;
    }         m_object;
    struct {
      json_size_t        size;
      json_size_t        capacity;
      struct json_value* p_values;
    }         m_array;
    json_utf8_t          m_inline_string[JSON_GENERIC_VALUE_SIZE];
    struct {
      json_string_pool_t       pool;
      json_string_pool_entry_t entry;
    }         m_pooled_string;
    json_string_ref_t    m_string_ref;
    json_u32_t           m_u32;
    json_i32_t           m_i32;
    json_u64_t           m_u64;
    json_i64_t           m_i64;
    json_f64_t           m_f64;
    json_big_integer_t*  m_big_int;
    json_big_float_t  *  m_big_float;
  };
} json_value_t;
typedef struct json_array_ref{
  json_u64_t          length;
  const json_value_t* p_values;
} json_array_ref_t;


typedef struct json_member{
  json_value_t m_identifier;
  json_value_t m_value;
} json_member_t;
typedef struct json_document*   json_document_t;

/*typedef struct json_document {
  json_object_pool_t object_pool;
  json_object_t*     p_object;
} json_document_t;*/

#if defined(__cplusplus)
#define ASSERT_TYPE_SIZE(type, size) static_assert(sizeof(type) == (size), #type " is not " #size " bytes")

#else
#define ASSERT_TYPE_SIZE(type, size) _Static_assert(sizeof(type) == (size), #type " is not " #size " bytes")
#endif

ASSERT_TYPE_SIZE(json_byte_t, 1);
ASSERT_TYPE_SIZE(json_utf8_t, 1);
ASSERT_TYPE_SIZE(json_string_ref_t, 16);
ASSERT_TYPE_SIZE(json_value_t, 16);
ASSERT_TYPE_SIZE(json_member_t, 32);




typedef void(*PFN_json_alloc_notification)(void* pUserData /**< Pointer to user data specified in the json_allocator_setup_params object passed to json_create_allocator */,
                                           void* __restrict returnAddr,
                                           json_u64_t size);

typedef void(*PFN_json_realloc_notification)(void* pUserData,
                                             void* returnAddr,
                                             void* originalAddr,
                                             json_u64_t newSize,
                                             json_u64_t originalSize);

typedef void(*PFN_json_free_notification)(void* pUserData,
                                          void* addr,
                                          json_u64_t size);




typedef json_u32_t json_flags_t;


/**
 * @enum json_allocator_flags
 *
 *
 * @var JSON_ALLOCATOR_DEFAULT = 0x0
 * @var JSON_ALLOCATOR_CUSTOM = 0x1
 *
 * */
typedef enum json_context_flags{
  JSON_CONTEXT_DEFAULT_FLAGS = 0x0 /**< Default allocator flag */,
  JSON_CONTEXT_EXTERNALLY_SYNCHRONIZED = 0x1 /**< Disable internal synchronization, allocators created with this flag must be guaranteed to never be called concurrently by multiple threads. */
}  json_context_flags_t;
typedef enum json_document_flags{
  JSON_DOCUMENT_DEFAULT_FLAGS           = 0x0,
  JSON_DOCUMENT_EXTERNALLY_SYNCHRONIZED = 0x1,
  JSON_DOCUMENT_SHARED_MEMORY           = 0x2
} json_document_flags_t;

typedef struct json_context_setup_params{
  json_flags_t                  flags;

  PFN_json_alloc_notification   pfnAllocNotification;
  PFN_json_realloc_notification pfnReallocNotification;
  PFN_json_free_notification    pfnFreeNotification;
  void*                         pUserData;


} json_context_setup_params_t;













/*
 *
 * api Declarations
 *
 * */


/**
 * Creates a new allocator object with optional user hooks for custom allocation tracking.
 *
 * \returns On success, returns JSON_SUCCESS. Otherwise returns an error code.
 * \pre  The parameter pAllocator must point to an uninitialized variable
 * \post If JSON_SUCCESS is returned, pAllocator must point to a valid allocator handle. Otherwise pAllocator must point to a null handle.
 * */
json_status_t json_create_context(json_context_t* pContext                   /**< [out] Address where the new allocator will be returned */,
                                  const json_context_setup_params_t* pParams /**< [in]  Optional pointer allowing the allocator to be created with user defined hooks */);


json_status_t json_destroy_context(json_context_t context /**< [in] Handle to the object to be destroyed */);







json_status_t json_create_document(json_document_t* pDocument, json_context_t allocator, json_string_ref_t documentName);
json_status_t json_destroy_document(json_document_t document);













#define JSON_STRING_REF(...) ((json_string_ref_t){ .length = sizeof(__VA_ARGS__) - 1, .p_data = __VA_ARGS__ })


json_string_ref_t json_make_string_ref(const json_utf8_t* pString);

json_status_t     json_set_value_to_value(json_document_t document, json_object_t object,   json_string_ref_t key, json_value_t value);

json_status_t     json_set_value_to_object(json_document_t document, json_object_t object, json_string_ref_t key, const json_object_t* pNewValue);
json_status_t     json_set_value_to_array(json_document_t document, json_object_t object, json_string_ref_t key, json_array_ref_t arrayRef);




json_status_t     json_get_value(json_value_t* pMemberValue, json_document_t document, json_object_t* pObject, json_string_ref_t key);

json_status_t     json_get_member(json_member_t* pMember,    json_document_t document, json_object_t* pObject, json_string_ref_t key);

json_status_t     json_member_set_key  (json_document_t document, json_member_t* pMember, json_string_ref_t memberName);
json_status_t     json_member_set_value(json_document_t document, json_member_t* pMember, json_value_t value);








JSON_END_C_NAMESPACE

#endif//VALKYRIE_JSON_VALUE_H
