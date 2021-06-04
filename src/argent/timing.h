//
// Created by maxwe on 2021-05-23.
//

#ifndef ARGENT_INTERNAL_TIMING_H
#define ARGENT_INTERNAL_TIMING_H

#include "internal.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <time.h>

AGT_BEGIN_C_NAMESPACE

// timeout units: 100ns, ie. QueryPerformanceFrequency
// 346DC5D63886594BULL
// 0011 0100 0110 1101
// 1100 0101 1101 0110
// 0011 1000 1000 0110
// 0101 1001 0100 1011

#define TIMEOUT_UNITS_PER_MS 10000

typedef agt_u64_t agt_deadline_t;


AGT_forceinline agt_deadline_t agt_current_time() {
  agt_u64_t current_time;
  QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
  return current_time;
}

AGT_forceinline agt_deadline_t agt_deadline_from_timeout(agt_u64_t timeout) {
  return agt_current_time() + timeout;
}


AGT_forceinline agt_u32_t deadline_max_timeout_ms(agt_deadline_t deadline) {
  if ( deadline < 2 )
    return (agt_u32_t)((agt_u32_t)0 - (agt_u32_t)deadline);
  return (deadline - agt_current_time()) / TIMEOUT_UNITS_PER_MS;
}
AGT_forceinline bool      is_past_deadline(agt_deadline_t deadline) {
  return agt_current_time() >= deadline;
}

AGT_forceinline void      yield_processor() {
  YieldProcessor();
}


AGT_END_C_NAMESPACE

#endif  //ARGENT_INTERNAL_TIMING_H
