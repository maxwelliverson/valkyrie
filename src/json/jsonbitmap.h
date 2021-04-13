//
// Created by maxwe on 2021-04-12.
//

#ifndef VALKYRIE_INTERNAL_JSON_BITMAP_H
#define VALKYRIE_INTERNAL_JSON_BITMAP_H

#include "internal.h"


#define JSON_BITMAP_BITS_PER_FIELD_SHIFT 6
#define JSON_BITMAP_BITS_PER_FIELD (0x1 << JSON_BITMAP_BITS_PER_FIELD_SHIFT)
#define JSON_BITMAP_BIT_IN_FIELD_MASK (JSON_BITMAP_BITS_PER_FIELD - 1)
#define JSON_BITMAP_FIELD_FULL ((json_bitmap_field_t)-1)


JSON_BEGIN_C_NAMESPACE


typedef json_u64_t json_bitmap_field_t;
typedef json_u32_t json_bitmap_index_t;
typedef json_bitmap_field_t* json_bitmap_t;

inline static json_bitmap_index_t json_bitmap_index_create(json_u32_t fieldId, json_u32_t bitId) {
  return (fieldId << JSON_BITMAP_BITS_PER_FIELD_SHIFT) + bitId;
}
inline static json_u32_t          json_bitmap_index_field(json_bitmap_index_t bitmapIndex) {
  return bitmapIndex >> JSON_BITMAP_BITS_PER_FIELD_SHIFT;
}
inline static json_u32_t          json_bitmap_index_bit_in_field(json_bitmap_index_t bitmapIndex) {
  return bitmapIndex & JSON_BITMAP_BIT_IN_FIELD_MASK;
}


// Try to atomically claim a sequence of `count` bits in a single
// field at `idx` in `bitmap`. Returns `true` on success.
json_bool_t json_bitmap_try_find_claim_field(json_bitmap_t bitmap, size_t idx, size_t count, json_bitmap_index_t* bitmap_idx);

// Starts at idx, and wraps around to search in all `bitmap_fields` fields.
// For now, `count` can be at most MI_BITMAP_FIELD_BITS and will never cross fields.
json_bool_t json_bitmap_try_find_from_claim(json_bitmap_t bitmap, size_t bitmap_fields, size_t start_field_idx, size_t count, json_bitmap_index_t* bitmap_idx);

// Set `count` bits at `bitmap_idx` to 0 atomically
// Returns `true` if all `count` bits were 1 previously.
json_bool_t json_bitmap_unclaim(json_bitmap_t bitmap, size_t bitmap_fields, size_t count, json_bitmap_index_t bitmap_idx);

// Set `count` bits at `bitmap_idx` to 1 atomically
// Returns `true` if all `count` bits were 0 previously. `any_zero` is `true` if there was at least one zero bit.
json_bool_t json_bitmap_claim(json_bitmap_t bitmap, size_t bitmap_fields, size_t count, json_bitmap_index_t bitmap_idx, json_bool_t* any_zero);

json_bool_t json_bitmap_is_claimed(json_bitmap_t bitmap, size_t bitmap_fields, size_t count, json_bitmap_index_t bitmap_idx);
json_bool_t json_bitmap_is_any_claimed(json_bitmap_t bitmap, size_t bitmap_fields, size_t count, json_bitmap_index_t bitmap_idx);


JSON_END_C_NAMESPACE





#endif//VALKYRIE_INTERNAL_JSON_BITMAP_H
