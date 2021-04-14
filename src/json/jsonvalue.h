//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_JSON_INTERNAL_VALUE_H
#define VALKYRIE_JSON_INTERNAL_VALUE_H

#include <json/value.h>

JSON_BEGIN_C_NAMESPACE

/**
 *    0x00  =>  00 | 0 0 0 0 0 0  =  null
 *
 *    0x01  =>  00 | 0 0 0 0 0 1  =  false
 *    0x41  =>  01 | 0 0 0 0 0 1  =  true
 *
 *    0x02  =>  00 | 0 0 0 0 1 0  =  symbol
 *    0x42  =>  01 | 0 0 0 0 1 0  =  inline string
 *    0x82  =>  10 | 0 0 0 0 1 0  =  string ref
 *
 *
 *    0x04  =>  00 | 0 0 0 1 0 0  =  int64
 *    0x44  =>  01 | 0 0 0 1 0 0  =  uint64
 *    0x84  =>  10 | 0 0 0 1 0 0  =  big int
 *
 *    0x08  =>  00 | 0 0 1 0 0 0  =  float64
 *    0x48  =>  01 | 0 0 1 0 0 0  =  big float
 *
 *    0x10  =>  00 | 0 1 0 0 0 0  =  object
 *
 *    0x20  =>  00 | 1 0 0 0 0 0  =  array
 *
 * */




//ASSERT_TYPE_SIZE(json_value_t, JSON_GENERIC_VALUE_SIZE);



JSON_END_C_NAMESPACE

#endif//VALKYRIE_JSON_INTERNAL_VALUE_H
