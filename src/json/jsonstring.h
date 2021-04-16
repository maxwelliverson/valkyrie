//
// Created by Maxwell on 2021-02-16.
//

#ifndef VALKYRIE_INTERNAL_JSON_STRING_H
#define VALKYRIE_INTERNAL_JSON_STRING_H

#include <json/string.h>

#include "internal_fwd.h"


#include <immintrin.h>


JSON_BEGIN_C_NAMESPACE

// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#define UTF8_ACCEPT 0
#define UTF8_REJECT 12

static const json_u8_t utf8decode_table__[] = {
    // The first part of the table maps bytes to character classes that
    // to reduce the size of the transition table and create bitmasks.
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

    // The second part is a transition table that maps a combination
    // of a state of the automaton and a character class to a state.
    0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
    12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
    12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
    12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
    12,36,12,12,12,12,12,12,12,12,12,12,
};

/*static const JSON_cache_aligned json_u8_t utf8encode_table__[] = {
    // First half is the right shift bit value
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 18, 17, 16, 15, 14,
    12, 11, 10,  9,  8,  6,  5,  4,
     0,  0,  0,  0,  0,  0,  0,  0,
    // Second half is the appropriate mask
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8,
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xE0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // Left Shift
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 3, 3, 3, 3, 3,

};*/

JSON_forceinline static json_u32_t json_utf8_decode_next__(json_u32_t* JSON_restrict state,
                                                           json_u32_t* JSON_restrict codep,
                                                           json_u32_t byte) {
  json_u32_t type = utf8decode_table__[byte];

  *codep = (*state != UTF8_ACCEPT) ?
           (byte & 0x3fu) | (*codep << 6) :
           (0xff >> type) & (byte);

  *state = utf8decode_table__[256 + *state + type];
  return *state;
}

JSON_forceinline static json_u32_t json_utf8_decode_next8__(json_u32_t                initialState,
                                                            json_u32_t* JSON_restrict pCodepoints,
                                                            json_u32_t* JSON_restrict pCodepointsWritten,
                                                            const json_char_t*        pBytes) {
  static const json_i32_t utf8decode8_table[] = {
      // The first part of the table maps bytes to character classes that
      // to reduce the size of the transition table and create bitmasks.
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
       1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
       7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
       8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
      10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

      // The second part is a transition table that maps a combination
      // of a state of the automaton and a character class to a state.
       0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
      12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
      12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
      12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
      12,36,12,12,12,12,12,12,12,12,12,12
  };

#define AVX_MASK_256(name, val64) const static __m256i name = { .m256i_u64 = { val64, val64, val64, val64 } }
#define AVX_MASK_128(name, val64) const static __m128i name = { .m128i_u64 = { val64, val64 } }

#define BYTE_MASK_VAL64      0x00FF00FF00FF00FFULL
#define LOW_6BITS_MASK_VAL64 0x003F003F003F003FULL
#define OFFSET_256_VAL64     0x0100010001000100ULL

  AVX_MASK_256(U32LowerByteMask, BYTE_MASK_VAL64);
  AVX_MASK_256(Low6BitMask,      LOW_6BITS_MASK_VAL64);
  AVX_MASK_256(Offset256,        OFFSET_256_VAL64);
  AVX_MASK_256(AcceptState,      0);

  const static __m256i ZeroHigh4BytesMask  = { .m256i_i32 = { -1, -1, -1, -1, -1, -1, -1,  0 } };
  const static __m256i ShiftRight4Bytes    = { .m256i_u32 = {  1,  2,  3,  4,  5,  6,  7,  7 } };
  const static __m256i ZeroHigh8BytesMask  = { .m256i_i64 = { -1, -1, -1, 0 } };
  const static __m256i ShiftRight8Bytes    = { .m256i_u32 = {  2,  3,  4,  5,  6,  7,  7,  7 } };
  const static __m256i ZeroHigh12BytesMask = { .m256i_i32 = { -1, -1, -1, -1, -1,  0,  0,  0 } };
  const static __m256i ShiftRight12Bytes   = { .m256i_u32 = {  3,  4,  5,  6,  7,  7,  7,  7 } };


#define m256_shift_nbytes(val, n) _mm256_and_si256(_mm256_and_si256(_mm256_permutevar8x32_epi32(val, ShiftRight##n##Bytes), ZeroHigh##n##BytesMask), mbCharMaskV)

#define m256_accept_state(c, type) _mm256_and_si256(_mm256_srav_epi32(U32LowerByteMask, type), c)
#define m256_reject_state(c, codepoint) _mm256_or_si256(_mm256_slli_epi32(codepoint, 6), sixBitCharV)
#define m256_update_codepoints(codepoint, state, c) codepoint = _mm256_blendv_epi8(acceptStateV, m256_reject_state(c, codepoint), _mm256_cmpeq_epi32(state, AcceptState))
#define m256_update_state(state) state = _mm256_i32gather_epi32(utf8decode8_table,_mm256_add_epi32(_mm256_add_epi32(state, typeV), Offset256), 1)

  __m256i stateV;
  __m256i codepointsV;
  __m256i tmpCodePointsV;

  //__m256i rejectStateV;


  // Initial State

  const __m256i     charU32V     = _mm256_cvtepu8_epi32(_mm_loadu_epi64(pBytes));
  const __m256i     mbCharMaskV  = _mm256_cmpgt_epi32(charU32V, Low6BitMask);
  const __m256i     sixBitCharV  = _mm256_and_si256(charU32V, Low6BitMask);
  const __m256i     typeV        = _mm256_i32gather_epi32(utf8decode8_table, charU32V, 1);
  const __m256i     acceptStateV = m256_accept_state(charU32V, typeV);

  const json_bool_t isAllAscii  = *(const json_u64_t*)pBytes;

  stateV = (__m256i){ .m256i_u32 = { initialState } };
  codepointsV = (__m256i){ .m256i_u32 = { *pCodepoints } };

  // Round 1

  m256_update_codepoints(codepointsV, stateV, charU32V);
  m256_update_state(stateV);

  if ( isAllAscii ) {
    *pCodepointsWritten = 8;
    goto writeResults;
  }

  tmpCodePointsV =  m256_shift_nbytes(codepointsV, 4);


  /* *codep = (*state != UTF8_ACCEPT) ?
             (byte & 0x3fu) | (*codep << 6) :
             (0xff >> type) & (byte);

  *state = utf8decode_table__[256 + *state + type];
  return *state;*/


  // Round 2






writeResults:


  json_u32_t codePoints = ...;


  _mm256_storeu_si256((__m256i*)pCodepoints, codepointsV);


  *pCodepointsWritten += codePoints;
  *pBytesConsumed = ...;

  return codePoints == 8 ? UTF8_ACCEPT : stateV.m256i_u32[codePoints];
}

/**
 * 1 Byte:  00000000 00000000 00000000 00000000  ....  00000000 00000000 00000000 01111111
 * 2 Byte:  00000000 00000000 00000000 10000000  ....  00000000 00000000 00000111 11111111
 * 3 Byte:  00000000 00000000 00001000 00000000  ....  00000000 00000000 11111111 11111111
 * 4 Byte:  00000000 00000001 00000000 00000000  ....  00000000 00010000 11111111 11111111
 *
 *
 *
 * */

#if JSON_IS_LITTLE_ENDIAN
JSON_forceinline static json_u32_t json_utf8_encode_next__(json_u32_t*  JSON_restrict state,
                                                           json_char_t* JSON_restrict byte,
                                                           json_u32_t codepoint) {
  //json_assert(codepoint < 0x00110000);
  json_u32_t shf = json_clz32(codepoint);

}
JSON_forceinline static json_u32_t json_utf8_encode_next8__(json_u32_t*  JSON_restrict state,
                                                            json_char_t* JSON_restrict byte,
                                                            json_u32_t*  codepoints) {
  //json_assert(codepoint < 0x00110000);
  //json_u32_t shf = json_clz32(codepoint);


}
#else
JSON_forceinline static json_u32_t json_utf8_encode_next__() {

}
#endif






inline static json_u32_t inline_string_get_length__(const char* inlineString) {
  return JSON_INLINE_STRING_MAX_LENGTH - inlineString[JSON_INLINE_STRING_MAX_LENGTH];
}
inline static void       inline_string_set_length__(char* inlineString, json_u32_t length) {
  json_assert_internal(length <= JSON_INLINE_STRING_MAX_LENGTH);
  inlineString[JSON_INLINE_STRING_MAX_LENGTH] = JSON_INLINE_STRING_MAX_LENGTH - (char)length;
}


json_status_t  symbol_registry_init(json_symbol_registry_t registry, json_u32_t initSize, json_internal_allocator_t allocator);
void           symbol_registry_cleanup(json_symbol_registry_t registry);

json_symbol_t* symbol_registry_find(json_symbol_registry_t registry, const char* buffer, json_u64_t length);
json_symbol_t  symbol_registry_register_symbol(json_symbol_registry_t registry, const char* string, json_u64_t length);
json_symbol_t  symbol_registry_remove(json_symbol_registry_t registry, const char* string, json_u64_t length);
void           symbol_registry_remove_symbol(json_symbol_registry_t registry, json_symbol_t symbol);
json_bool_t    symbol_registry_erase(json_symbol_registry_t registry, const char* string, json_u64_t length);
void           symbol_registry_erase_symbol(json_symbol_registry_t registry, json_symbol_t symbol);
void           symbol_registry_clear(json_symbol_registry_t registry);



JSON_END_C_NAMESPACE


#endif //VALKYRIE_INTERNAL_JSON_STRING_H
