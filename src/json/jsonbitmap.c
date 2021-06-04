//
// Created by maxwe on 2021-04-12.
//

#include "jsonbitmap.h"


inline static json_u64_t json_bitmap_mask(json_u32_t count, json_u32_t bitId) {
  json_assert_internal(count + bitId <= JSON_BITMAP_BITS_PER_FIELD);
  json_assert_internal(count > 0);
  return (((json_u64_t)0x1 << count) - 1) << bitId;
}

typedef unsigned long long u64;
typedef unsigned           u32;

inline static json_u32_t fastlog2(json_u64_t value){
  const static json_u32_t magic_shift  = 0b00111010;
  const static json_u64_t magic_number = 0x03147259A7ABB7E1ULL;
  const static json_u32_t magic_results[64] = {
       0,  1, 59,  2, 60,  8,  3, 14,
      61, 20,  9, 23,  4, 29, 15, 35,
      62, 12, 21, 33, 10, 41, 24, 43,
       5, 26, 30, 49, 16, 45, 36, 52,
      63, 58,  7, 13, 19, 22, 28, 34,
      11, 32, 40, 42, 25, 48, 44, 51,
      57,  6, 18, 27, 31, 39, 47, 50,
      56, 17, 38, 46, 55, 37, 54, 53
  };

  return magic_results[((value & (json_u64_t)-value) * magic_number) << magic_shift];
}


// Returns `true` if all `count` bits were 1. `any_ones` is `true` if there was at least one bit set to one.
inline static json_bool_t json_bitmap_is_claimedx(json_bitmap_t bitmap, size_t bitmap_fields, size_t count, json_bitmap_index_t bitmap_idx, json_bool_t* any_ones) {
  const size_t idx     = json_bitmap_index_field(bitmap_idx);
  const size_t bitidx  = json_bitmap_index_bit_in_field(bitmap_idx);
  const uintptr_t mask = json_bitmap_mask(count, bitidx);
  json_assert_internal(bitmap_fields > idx); 
  UNUSED(bitmap_fields);
  const json_u64_t field = bitmap[idx];
  if (any_ones != NULL) 
    *any_ones = ((field & mask) != 0);
  return ((field & mask) == mask);
}

#if defined (JSON_HAVE_FAST_BITSCAN)
inline static json_u64_t json_initial_bit__(json_u64_t map) {
  return json_ctz(~map);
}
#else
inline static json_u64_t json_initial_bit__(json_u64_t) {
  return 0;
}
#endif



json_bool_t json_bitmap_try_find_claim_field(json_bitmap_t bitmap, size_t idx, const size_t count, json_bitmap_index_t* bitmap_idx) {
  json_assert_internal(bitmap_idx != NULL);
  json_assert_internal(count <= JSON_BITMAP_BITS_PER_FIELD);
  json_u64_t* const field = &bitmap[idx];
  json_u64_t map          = *field;
  if ( map == JSON_BITMAP_FIELD_FULL )
    return false; // short cut

  // search for 0-bit sequence of length count
  const uintptr_t mask = json_bitmap_mask(count, 0);
  const size_t    bitidx_max = JSON_BITMAP_BITS_PER_FIELD - count;


  size_t bitidx = json_initial_bit__(map);
  uintptr_t m = (mask << bitidx);     // invariant: m == mask shifted by bitidx

  // scan linearly for a free range of zero bits
  while (bitidx <= bitidx_max) {
    const uintptr_t mapm = map & m;
    if (mapm == 0) {  // are the mask bits free at bitidx?
      json_assert_internal((m >> bitidx) == mask); // no overflow?
      const uintptr_t newmap = map | m;
      json_assert_internal((newmap^map) >> bitidx == mask);

      *field = newmap;
      *bitmap_idx = json_bitmap_index_create(idx, bitidx);
      return true;
      /*json_assert_internal((newmap^map) >> bitidx == mask);
      if (!json_atomic_cas_weak_acq_rel(field, &map, newmap)) {  // TODO: use strong cas here?
        // no success, another thread claimed concurrently.. keep going (with updated `map`)
        continue;
      }
      else {
        // success, we claimed the bits!
        *bitmap_idx = json_bitmap_index_create(idx, bitidx);
        return true;
      }*/
    }
    else {
      // on to the next bit range
#ifdef JSON_HAVE_FAST_BITSCAN
      const size_t shift = (count == 1 ? 1 : json_bsr(mapm) - bitidx + 1);
      json_assert_internal(shift > 0 && shift <= count);
#else
      const size_t shift = 1;
#endif
      bitidx += shift;
      m <<= shift;
    }
  }
  // no bits found
  return false;
}

json_bool_t json_bitmap_try_find_from_claim(json_bitmap_t bitmap, const size_t bitmap_fields, const size_t start_field_idx, const size_t count, json_bitmap_index_t* bitmap_idx) {
  size_t idx = start_field_idx;
  for (size_t visited = 0; visited < bitmap_fields; visited++, idx++) {
    if (idx >= bitmap_fields) idx = 0; // wrap
    if (json_bitmap_try_find_claim_field(bitmap, idx, count, bitmap_idx)) {
      return true;
    }
  }
  return false;
}

json_bool_t json_bitmap_unclaim(json_bitmap_t bitmap, size_t bitmap_fields, size_t count, json_bitmap_index_t bitmap_idx)  {
  const size_t idx = json_bitmap_index_field(bitmap_idx);
  const size_t bitidx = json_bitmap_index_bit_in_field(bitmap_idx);
  const uintptr_t mask = json_bitmap_mask(count, bitidx);
  json_assert_internal(bitmap_fields > idx); UNUSED(bitmap_fields);
  // json_assert_internal((bitmap[idx] & mask) == mask);
  const json_u64_t prev = bitmap[idx];
  bitmap[idx] &= ~mask;
  //uintptr_t prev = json_atomic_and_acq_rel(&bitmap[idx], ~mask);
  return ((prev & mask) == mask);
}

json_bool_t json_bitmap_claim(json_bitmap_t bitmap, size_t bitmap_fields, size_t count, json_bitmap_index_t bitmap_idx, json_bool_t* any_zero)  {
  const size_t idx = json_bitmap_index_field(bitmap_idx);
  const size_t bitidx = json_bitmap_index_bit_in_field(bitmap_idx);
  const uintptr_t mask = json_bitmap_mask(count, bitidx);
      json_assert_internal(bitmap_fields > idx); UNUSED(bitmap_fields);
  //json_assert_internal(any_zero != NULL || (bitmap[idx] & mask) == 0);
  const json_u64_t prev = bitmap[idx];
  bitmap[idx] |= mask;
  if (any_zero != NULL)
    *any_zero = ((prev & mask) != mask);
  return ((prev & mask) == 0);
}

json_bool_t json_bitmap_is_claimed(json_bitmap_t bitmap, size_t bitmap_fields, size_t count, json_bitmap_index_t bitmap_idx) {
  return json_bitmap_is_claimedx(bitmap, bitmap_fields, count, bitmap_idx, NULL);
}
json_bool_t json_bitmap_is_any_claimed(json_bitmap_t bitmap, size_t bitmap_fields, size_t count, json_bitmap_index_t bitmap_idx)  {
  json_bool_t any_ones;
  json_bitmap_is_claimedx(bitmap, bitmap_fields, count, bitmap_idx, &any_ones);
  return any_ones;
}