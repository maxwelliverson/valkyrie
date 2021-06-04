//
// Created by maxwe on 2021-04-14.
//

#include <json/parser.h>
#include "internal.h"
#include "jsontypes.h"
#include "jsontables.h"

#include <intrin.h>
//#include <nmmintrin.h>

typedef json_u8_t  bitmask_x8;
typedef json_u16_t bitmask_x16;
typedef json_u32_t bitmask_x32;
typedef json_u64_t bitmask_x64;

typedef union u8_x16 {
  __m128i xmm;
} u8_x16;
typedef union u8_x32 {
  __m128i xmm[2];
  __m256i ymm;
} u8_x32;
typedef union u8_x64 {
  __m128i xmm[4];
  __m256i ymm[2];
  __m512i zmm;
} u8_x64;

typedef union u16_x8   {
  __m128i xmm;
} u16_x8;
typedef union u16_x16  {
  __m128i xmm[2];
  __m256i ymm;
} u16_x16;
typedef union u16_x32  {
  __m128i xmm[4];
  __m256i ymm[2];
  __m512i zmm;
} u16_x32;

typedef union u32_x4   {
  __m128i xmm;
} u32_x4;
typedef union u32_x8   {
  __m128i xmm[2];
  __m256i ymm;
} u32_x8;
typedef union u32_x16  {
  __m128i xmm[4];
  __m256i ymm[2];
  __m512i zmm;
} u32_x16;

typedef union u64_x2  {
  __m128i xmm;
} u64_x2;
typedef union u64_x4  {
  __m128i xmm[2];
  __m256i ymm;
} u64_x4;
typedef union u64_x8  {
  __m128i xmm[4];
  __m256i ymm[2];
  __m512i zmm;
} u64_x8;


#define JSON_vectorize JSON_forceinline static



JSON_vectorize u8_x16 splat_u8_x16(json_u8_t value) {
  return (u8_x16){ .xmm = _mm_set1_epi8((char)value) };
}
JSON_vectorize u8_x32 splat_u8_x32(json_u8_t value) {
  return (u8_x32){ .ymm = _mm256_set1_epi8((char)value) };
}
JSON_vectorize u8_x64 splat_u8_x64(json_u8_t value) {
  return (u8_x64){ .ymm = { splat_u8_x32(value).ymm, splat_u8_x32(value).ymm } };
}

JSON_vectorize u64_x2 splat_u64_x2(json_u64_t value) {
  return (u64_x2){ .xmm = _mm_set1_epi64x((long long)value) };
}


JSON_vectorize u8_x16 zero_u8_x16() {
  return (u8_x16){ .xmm = _mm_setzero_si128() };
}
JSON_vectorize u8_x32 zero_u8_x32() {
  return (u8_x32){ .ymm = _mm256_setzero_si256() };
}
JSON_vectorize u8_x64 zero_u8_x64() {
  return (u8_x64){ .ymm = { _mm256_setzero_si256(), _mm256_setzero_si256() } };
}
JSON_vectorize u8_x16 ones_u8_x16() {
  return splat_u8_x16(0xFF);
}
JSON_vectorize u8_x32 ones_u8_x32() {
  return splat_u8_x32(0xFF);
}
JSON_vectorize u8_x64 ones_u8_x64() {
  return splat_u8_x64(0xFF);
}


JSON_vectorize u8_x32 concat2_u8_x16(u8_x16 a, u8_x16 b) {
  return (u8_x32){ .xmm = { a.xmm, b.xmm } };
}
JSON_vectorize u8_x64 concat4_u8_x16(u8_x16 a, u8_x16 b, u8_x16 c, u8_x16 d) {
  return (u8_x64){ .xmm = { a.xmm, b.xmm, c.xmm, d.xmm } };
}
JSON_vectorize u8_x64 concat2_u8_x32(u8_x32 a, u8_x32 b) {
  return (u8_x64){ .ymm = { a.ymm, b.ymm } };
}




JSON_vectorize u8_x16 and_u8_x16(u8_x16 a, u8_x16 b) {
  return (u8_x16){ .xmm = _mm_and_si128(a.xmm, b.xmm) };
}
JSON_vectorize u8_x32 and_u8_x32(u8_x32 a, u8_x32 b) {
  return (u8_x32){ .ymm = _mm256_and_si256(a.ymm, b.ymm) };
}
JSON_vectorize u8_x64 and_u8_x64(u8_x64 a, u8_x64 b) {
  return concat2_u8_x32(and_u8_x32((u8_x32)a.ymm[0], (u8_x32)b.ymm[0]),
                        and_u8_x32((u8_x32)a.ymm[1], (u8_x32)b.ymm[1]));
}

JSON_vectorize u8_x16 or_u8_x16(u8_x16 a, u8_x16 b) {
  return (u8_x16){ .xmm = _mm_or_si128(a.xmm, b.xmm) };
}
JSON_vectorize u8_x32 or_u8_x32(u8_x32 a, u8_x32 b) {
  return (u8_x32){ .ymm = _mm256_or_si256(a.ymm, b.ymm) };
}
JSON_vectorize u8_x64 or_u8_x64(u8_x64 a, u8_x64 b) {
  return concat2_u8_x32(or_u8_x32((u8_x32)a.ymm[0], (u8_x32)b.ymm[0]),
                        or_u8_x32((u8_x32)a.ymm[1], (u8_x32)b.ymm[1]));
}

JSON_vectorize u8_x32 reduce_or_u8_x64(u8_x64 a) {
  return (u8_x32){ .ymm = _mm256_or_si256(a.ymm[0], a.ymm[1]) };
}

JSON_vectorize u8_x16 xor_u8_x16(u8_x16 a, u8_x16 b) {
  return (u8_x16){ .xmm = _mm_xor_si128(a.xmm, b.xmm) };
}
JSON_vectorize u8_x32 xor_u8_x32(u8_x32 a, u8_x32 b) {
  return (u8_x32){ .ymm = _mm256_xor_si256(a.ymm, b.ymm) };
}
JSON_vectorize u8_x64 xor_u8_x64(u8_x64 a, u8_x64 b) {
  return concat2_u8_x32(xor_u8_x32((u8_x32)a.ymm[0], (u8_x32)b.ymm[0]),
                        xor_u8_x32((u8_x32)a.ymm[1], (u8_x32)b.ymm[1]));
}

JSON_vectorize u8_x16 not_u8_x16(u8_x16 a) {
  return xor_u8_x16(a, ones_u8_x16());
}
JSON_vectorize u8_x32 not_u8_x32(u8_x32 a) {
  return xor_u8_x32(a, ones_u8_x32());
}
JSON_vectorize u8_x64 not_u8_x64(u8_x64 a) {
  return xor_u8_x64(a, ones_u8_x64());
}

JSON_vectorize u8_x32 andnot_u8_x32(u8_x32 a, u8_x32 b) {
  return (u8_x32){ .ymm = _mm256_andnot_si256(a.ymm, b.ymm) };
}


JSON_vectorize u8_x32 eq_u8_x32(u8_x32 a, u8_x32 b) {
  return (u8_x32){ .ymm = _mm256_cmpeq_epi8(a.ymm, b.ymm) };
}
JSON_vectorize u8_x64 eq_u8_x64(u8_x64 a, u8_x64 b) {
  return (u8_x64){ .ymm = {
                      _mm256_cmpeq_epi8(a.ymm[0], b.ymm[0]),
                      _mm256_cmpeq_epi8(a.ymm[1], b.ymm[1])
                  } };
}
JSON_vectorize u8_x32 lt_u8_x32(u8_x32 a, u8_x32 b) {
  return (u8_x32){ .ymm = _mm256_cmpgt_epi8(b.ymm, a.ymm) };
}
JSON_vectorize u8_x32 gt_u8_x32(u8_x32 a, u8_x32 b) {
  return (u8_x32){ .ymm = _mm256_cmpgt_epi8(a.ymm, b.ymm) };
}
JSON_vectorize u8_x32 neq_u8_x32(u8_x32 a, u8_x32 b) {
  return not_u8_x32(eq_u8_x32(a, b));
}


JSON_vectorize u8_x32 prev_u8_x32(u8_x32 a, u8_x32 b, int N) {
  return (u8_x32){ .ymm = _mm256_alignr_epi8(a.ymm, _mm256_permute2x128_si256(b.ymm, a.ymm, 0x21), 16 - N) };
}


JSON_forceinline json_bool_t test_any_u8_x32(u8_x32 mask) {
  return !_mm256_testz_si256(mask.ymm, mask.ymm);
}

JSON_forceinline bitmask_x16 bitmask_u8_x16(u8_x16 mask) {
  return (bitmask_x16)_mm_movemask_epi8(mask.xmm);
}
JSON_forceinline bitmask_x32 bitmask_u8_x32(u8_x32 mask) {
  return (bitmask_x32)_mm256_movemask_epi8(mask.ymm);
}
JSON_forceinline bitmask_x64 bitmask_u8_x64(u8_x64 mask) {
  const bitmask_x64 lo = bitmask_u8_x32((union u8_x32){ .ymm = mask.ymm[0] });
  const bitmask_x64 hi = bitmask_u8_x32((union u8_x32){ .ymm = mask.ymm[1] });
  return lo | (hi << 32);
}
JSON_forceinline bitmask_x8  bitmask_u32_x8(u32_x8 mask) {
  return (bitmask_x8)_mm256_movemask_ps(*(__m256*)&mask.ymm);
}
JSON_forceinline bitmask_x16 bitmask_u32_x16(u32_x16 mask) {
  const bitmask_x32 lo = bitmask_u32_x8((u32_x8){ .ymm = mask.ymm[0] });
  const bitmask_x32 hi = bitmask_u32_x8((u32_x8){ .ymm = mask.ymm[1] });
  return (bitmask_x16)(lo | (hi << 8));
}

#define bitmask_v(mask) \
  _Generic((mask), \
           u8_x16: bitmask_u8_x16, \
           u8_x32: bitmask_u8_x32, \
           u8_x64: bitmask_u8_x64, \
           u32_x8: bitmask_u32_x8, \
           u32_x16: bitmask_u32_x16)(mask)



JSON_forceinline u8_x32      shuffle_u8_x32(u8_x32 a, u8_x32 b) {
  return (u8_x32){ .ymm = _mm256_shuffle_epi8(a.ymm, b.ymm) };
}


JSON_vectorize u8_x32 add_u8_x32(u8_x32 a, u8_x32 b) {
  return (u8_x32){ .ymm = _mm256_add_epi8(a.ymm, b.ymm) };
}


JSON_forceinline u32_x4      set_u32_x4(json_u32_t a, json_u32_t b, json_u32_t c, json_u32_t d) {
  return (u32_x4){ .xmm = _mm_set_epi32(a, b, c, d) };
}
JSON_forceinline u32_x8      set_u32_x8(json_u32_t a, json_u32_t b, json_u32_t c, json_u32_t d, json_u32_t e, json_u32_t f, json_u32_t g, json_u32_t h) {
  return (u32_x8){ .ymm = _mm256_set_epi32(a, b, c, d, e, f, g, h) };
}
JSON_forceinline u64_x2      set_u64_x2(json_u64_t a, json_u64_t b) {
  return (u64_x2){ .xmm = _mm_set_epi64x(a, b) };
}
JSON_forceinline u64_x4      set_u64_x4(json_u64_t a, json_u64_t b, json_u64_t c, json_u64_t d) {
  return (u64_x4){ .ymm = _mm256_set_epi64x(a, b, c, d) };
}

JSON_forceinline u64_x2      clmul_u64_x2(u64_x2 a, u64_x2 b, json_u8_t value) {
  return (u64_x2){ .xmm = _mm_clmulepi64_si128(a.xmm, b.xmm, value) };
}
JSON_forceinline bitmask_x64 cvt_u64_x2(u64_x2 a) {
  return _mm_cvtsi128_si64(a.xmm);
}

JSON_vectorize u32_x4  blend_u32_x4(u32_x4 a, u32_x4 b, const int ctl) {
  return (u32_x4){ .xmm = _mm_blend_epi32(a.xmm, b.xmm, ctl) };
}
JSON_vectorize u32_x8  blend_u32_x8(u32_x8 a, u32_x8 b, const int ctl) {
  return (u32_x8){ .ymm = _mm256_blend_epi32(a.ymm, b.ymm, ctl) };
}

JSON_vectorize u16_x8  blend_u16_x8(u16_x8 a, u16_x8 b, const int ctl) {
  return (u16_x8){ .xmm = _mm_blend_epi16(a.xmm, b.xmm, ctl) };
}
JSON_vectorize u16_x16 blend_u16_x16(u16_x16 a, u16_x16 b, const int ctl) {
  return (u16_x16){ .ymm = _mm256_blend_epi16(a.ymm, b.ymm, ctl) };
}

JSON_vectorize u8_x32  blendv_u8_x32(u8_x32 a, u8_x32 b, u8_x32 mask) {
  return (u8_x32){ .ymm = _mm256_blendv_epi8(a.ymm, b.ymm, mask.ymm) };
}


JSON_vectorize u8_x16 min_u8_x16(u8_x16 a, u8_x16 b) {
  return (u8_x16){ .xmm = _mm_min_epu8(a.xmm, b.xmm) };
}
JSON_vectorize u8_x32 min_u8_x32(u8_x32 a, u8_x32 b) {
  return (u8_x32){ .ymm = _mm256_min_epu8(a.ymm, b.ymm) };
}
JSON_vectorize u8_x64 min_u8_x64(u8_x64 a, u8_x64 b) {
  return concat2_u8_x32((u8_x32){ .ymm = _mm256_min_epu8(a.ymm[0], b.ymm[0]) },
                        (u8_x32){ .ymm = _mm256_min_epu8(a.ymm[1], b.ymm[1])});
}

JSON_vectorize u8_x16 max_u8_x16(u8_x16 a, u8_x16 b) {
  return (u8_x16){ .xmm = _mm_max_epu8(a.xmm, b.xmm) };
}
JSON_vectorize u8_x32 max_u8_x32(u8_x32 a, u8_x32 b) {
  return (u8_x32){ .ymm = _mm256_max_epu8(a.ymm, b.ymm) };
}
JSON_vectorize u8_x64 max_u8_x64(u8_x64 a, u8_x64 b) {
  return concat2_u8_x32((u8_x32){ .ymm = _mm256_max_epu8(a.ymm[0], b.ymm[0]) },
                        (u8_x32){ .ymm = _mm256_max_epu8(a.ymm[1], b.ymm[1])});
}

JSON_vectorize u32_x4 gather_u32_x4(const void* addr, u32_x4 indices, int scale) {
  return (u32_x4){ .xmm = _mm_i32gather_epi32(addr, indices.xmm, scale) };
}
JSON_vectorize u32_x8 gather_u32_x8(const void* addr, u32_x8 indices, int scale) {
  return (u32_x8){ .ymm = _mm256_i32gather_epi32(addr, indices.ymm, scale) };
}
JSON_vectorize u32_x4 gather_u64_x2(const void* addr, u32_x4 indices, int scale) {
  return (u32_x4){ .xmm = _mm_i32gather_epi64(addr, indices.xmm, scale) };
}
JSON_vectorize u64_x4 gather_u64_x4(const void* addr, u32_x4 indices, int scale) {
  return (u64_x4){ .ymm = _mm256_i32gather_epi64(addr, indices.xmm, scale) };
}

JSON_vectorize u8_x32 load_halves_u8_x32(const void* addr_lo, const void* addr_hi) {
  return (u8_x32){ .ymm = _mm256_loadu2_m128i(addr_hi, addr_lo) };
}
JSON_vectorize void   store_halves_u8_x32(u8_x32 v, void* addr_lo, void* addr_hi) {
  _mm256_storeu2_m128i(addr_hi, addr_lo, v.ymm);
}

JSON_vectorize json_bool_t is_ascii_u8_x16(u8_x16 v) {
  return bitmask_u8_x16(v) == 0;
}
JSON_vectorize json_bool_t is_ascii_u8_x32(u8_x32 v) {
  return bitmask_u8_x32(v) == 0;
}
JSON_vectorize json_bool_t is_ascii_u8_x64(u8_x64 v) {
  return is_ascii_u8_x32(reduce_or_u8_x64(v));
}



JSON_forceinline bitmask_x64 prefix_xor(const bitmask_x64 bitmask) {
// There should be no such thing with a processor supporting avx2
// but not clmul.
  const u64_x2 ones = splat_u64_x2(~0ULL);
  const u64_x2 low_bitmask = set_u64_x2(0ULL, bitmask);
  const u64_x2 result = clmul_u64_x2(low_bitmask, ones, 0);
  return cvt_u64_x2(result);
}
JSON_forceinline void        compress_u8_x32(u8_x32 this, bitmask_x32 mask, json_u8_t* target) {


  const bitmask_x8 mask8[4] = {
      mask,
      mask >> 8,
      mask >> 16,
      mask >> 24
  };

  const u64_x4 shuffle_mask = set_u64_x4(thin_epi8_table__[mask8[3]],
                                         thin_epi8_table__[mask8[2]],
                                         thin_epi8_table__[mask8[1]],
                                         thin_epi8_table__[mask8[0]]);

  const u64_x4 sfl_mask_offset = set_u64_x4(0x1818181818181818ULL,
                                            0x1010101010101010ULL,
                                            0x0808080808080808ULL,
                                            0x0000000000000000ULL);

  const u8_x32 new_shuffle_mask = add_u8_x32(*(u8_x32*)&shuffle_mask, *(u8_x32*)&sfl_mask_offset);

  const u8_x32 shuffled = shuffle_u8_x32(this, new_shuffle_mask);

  int pop0 = bits_set_256_mul2_table__[mask8[0]];
  int pop2 = bits_set_256_mul2_table__[mask8[2]];

  const json_u64_t* const shuffle_combine_table = (const json_u64_t*)pshufb_combine_table__;

  const u8_x32 compact_mask = load_halves_u8_x32(shuffle_combine_table + pop0,
                                                 shuffle_combine_table + pop2);

  const u8_x32 almost_there = shuffle_u8_x32(shuffled, compact_mask);


  store_halves_u8_x32(almost_there, target, target + 16 - json_popcnt32(mask & 0xFFFF));
}
JSON_forceinline void        compress_u8_x64(u8_x64 this, bitmask_x64 mask, json_u8_t* target) {
  const bitmask_x32 mask_lo = (bitmask_x32)mask;
  const bitmask_x32 mask_hi = (bitmask_x32)(mask >> 32);
  compress_u8_x32((u8_x32){ .ymm = this.ymm[0] }, mask_lo, target);
  compress_u8_x32((u8_x32){ .ymm = this.ymm[1] }, mask_hi, target + 32 - json_popcnt32(mask_lo));
}


JSON_vectorize u8_x64 lookup_u8_x64(u8_x32 table, u8_x64 indices) {
  return concat2_u8_x32(shuffle_u8_x32(table, (u8_x32){ .ymm = indices.ymm[0] }),
                        shuffle_u8_x32(table, (u8_x32){ .ymm = indices.ymm[1] }));
}




const static union u8_x32 json_whitespace_table__ = {
    .ymm = {
        .m256i_u8 = {
            ' ',  100,  100, 100,  17,  100, 113,   2,
            100, '\t', '\n', 112, 100, '\r', 100, 100,
            ' ',  100,  100, 100,  17,  100, 113,   2,
            100, '\t', '\n', 112, 100, '\r', 100, 100
        }}
};
const static union u8_x32 json_op_table__ = {
    .ymm = {
        .m256i_u8 = {
            0, 0,   0,   0,   0,   0, 0, 0,
            0, 0, ':', '{', ',', '}', 0, 0,
            0, 0,   0,   0,   0,   0, 0, 0,
            0, 0, ':', '{', ',', '}', 0, 0
        }
    }
};

_Static_assert(sizeof(json_whitespace_table__) * 8 == 256, "");
_Static_assert(sizeof(json_op_table__) * 8 == 256, "");

struct json_character_block {
  json_u64_t whitespace; // ASCII white-space ('\r','\n','\t',' ')
  json_u64_t op; // structural characters (comma, colon, braces, brackets but not quotes)
};


JSON_vectorize struct json_character_block json_classify__(u8_x64 vec) {
  const bitmask_x64 whitespace = bitmask_u8_x64(eq_u8_x64(vec, lookup_u8_x64(json_whitespace_table__, vec)));
  const u8_x64 curl_vec = or_u8_x64(vec, splat_u8_x64(0x20));
  const bitmask_x64 op = bitmask_u8_x64(eq_u8_x64(curl_vec, lookup_u8_x64(json_op_table__, vec)));
  return (struct json_character_block){ .whitespace = whitespace, .op = op };
}



inline static json_cstring_t skip_whitespace__(json_cstring_t p, json_cstring_t end) {
  while (p != end && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t'))
    ++p;
  return p;
}



inline static json_cstring_t skip_whitespace_simd__(const char* p) {
  // Fast return for single non-whitespace
  if (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')
    ++p;
  else
    return p;

  // 16-byte align to the next boundary
  const char* nextAligned = (json_cstring_t)(((json_u64_t)p + 15) & ~15ULL);
  while (p != nextAligned)
    if (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')
      ++p;
    else
      return p;

  // The rest of string using SIMD
  static const char whitespace[16] = " \n\r\t";
  const __m128i w = _mm_loadu_si128((const __m128i *)(&whitespace[0]));

  for (;; p += 16) {
    const __m128i s = _mm_load_si128((const __m128i *)(p));
    const int r = _mm_cmpistri(w, s, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_LEAST_SIGNIFICANT | _SIDD_NEGATIVE_POLARITY);
    if (r != 16)    // some of characters is non-whitespace
      return p + r;
  }
}

inline static json_cstring_t skip_whitespace_simd_with_end__(const char* p, const char* end) {
  // Fast return for single non-whitespace
  if (p != end && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t'))
    ++p;
  else
    return p;

  // The middle of string using SIMD
  static const char whitespace[16] = " \n\r\t";
  const __m128i w = _mm_loadu_si128((const __m128i *)(&whitespace[0]));

  for (; p <= end - 16; p += 16) {
    const __m128i s = _mm_loadu_si128((const __m128i *)(p));
    const int r = _mm_cmpistri(w, s, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_LEAST_SIGNIFICANT | _SIDD_NEGATIVE_POLARITY);
    if (r != 16)    // some of characters is non-whitespace
      return p + r;
  }

  return skip_whitespace__(p, end);
}


JSON_forceinline void scan_copy_unescaped_string__(StringStream& is, StackStream<char>& os) {
const char* p = is.src_;

// Scan one by one until alignment (unaligned load may cross page boundary and cause crash)
const char* nextAligned = reinterpret_cast<const char*>((reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
while (p != nextAligned)
if (RAPIDJSON_UNLIKELY(*p == '\"') || RAPIDJSON_UNLIKELY(*p == '\\') || RAPIDJSON_UNLIKELY(static_cast<unsigned>(*p) < 0x20)) {
is.src_ = p;
return;
}
else
os.Put(*p++);

// The rest of string using SIMD
static const char dquote[16] = { '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"' };
static const char bslash[16] = { '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\' };
static const char space[16]  = { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F };
const __m128i dq = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&dquote[0]));
const __m128i bs = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&bslash[0]));
const __m128i sp = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&space[0]));

for (;; p += 16) {
const __m128i s = _mm_load_si128(reinterpret_cast<const __m128i *>(p));
const __m128i t1 = _mm_cmpeq_epi8(s, dq);
const __m128i t2 = _mm_cmpeq_epi8(s, bs);
const __m128i t3 = _mm_cmpeq_epi8(_mm_max_epu8(s, sp), sp); // s < 0x20 <=> max(s, 0x1F) == 0x1F
const __m128i x = _mm_or_si128(_mm_or_si128(t1, t2), t3);
unsigned short r = static_cast<unsigned short>(_mm_movemask_epi8(x));
if (RAPIDJSON_UNLIKELY(r != 0)) {   // some of characters is escaped
SizeType length;
#ifdef _MSC_VER         // Find the index of first escaped
unsigned long offset;
_BitScanForward(&offset, r);
length = offset;
#else
length = static_cast<SizeType>(__builtin_ffs(r) - 1);
#endif
if (length != 0) {
char* q = reinterpret_cast<char*>(os.Push(length));
for (size_t i = 0; i < length; i++)
q[i] = p[i];

p += length;
}
break;
}
_mm_storeu_si128(reinterpret_cast<__m128i *>(os.Push(16)), s);
}

is.src_ = p;
}
