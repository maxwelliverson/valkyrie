//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_INTERNAL_JSON_NUMBER_H
#define VALKYRIE_INTERNAL_JSON_NUMBER_H

#include <json/value.h>

JSON_BEGIN_C_NAMESPACE


struct json_big_integer{
  json_u64_t  digits[JSON_BIG_INTEGER_CAPACITY];
  json_size_t size;
};
struct json_big_float{
  union{
    struct {
      json_u64_t : 64;
      json_u64_t : 64;
      json_u64_t : 64;
      json_u64_t : 64;
    } bits;
    struct {
      json_u64_t frac_lo_bits[3];
      json_u64_t frac_hi_bits : 44;
      json_i32_t exp_bits     : 20;
    };
  };
};



JSON_END_C_NAMESPACE


#endif//VALKYRIE_INTERNAL_JSON_NUMBER_H
