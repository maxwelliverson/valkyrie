//
// Created by maxwe on 2021-04-14.
//

#include <json/parser.h>
#include "jsontypes.h"


#include <nmmintrin.h>




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
