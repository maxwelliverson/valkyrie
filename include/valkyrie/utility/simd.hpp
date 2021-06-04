//
// Created by maxwe on 2021-04-19.
//

#ifndef VALKYRIE_ADT_SIMD_HPP
#define VALKYRIE_ADT_SIMD_HPP

#include <valkyrie/preprocessor.hpp>
#include <valkyrie/primitives.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cfloat>
#include <new>


#define PP_VK_impl_SIMD_SUPPORTS_none   true
#define PP_VK_impl_SIMD_SUPPORTS_sse    true
#define PP_VK_impl_SIMD_SUPPORTS_sse2   true
#define PP_VK_impl_SIMD_SUPPORTS_sse3   true
#define PP_VK_impl_SIMD_SUPPORTS_ssse3  true
#define PP_VK_impl_SIMD_SUPPORTS_sse4_1 true
#define PP_VK_impl_SIMD_SUPPORTS_sse4_2 true
#define PP_VK_impl_SIMD_SUPPORTS_avx    true
#define PP_VK_impl_SIMD_SUPPORTS_avx2   true
#define PP_VK_impl_SIMD_SUPPORTS_avx512f false
#define PP_VK_impl_SIMD_SUPPORTS_avx512v false

#define VK_simd_supports(flag) PP_VK_impl_SIMD_SUPPORTS_##flag

#define PP_VK_impl_TAKE_FIRST_ARG_redirect(x, ...) x
#define PP_VK_impl_TAKE_FIRST_ARG(...) PP_VK_impl_TAKE_FIRST_ARG_redirect(__VA_ARGS__)

#define VK_first_arg(x) PP_VK_impl_TAKE_FIRST_ARG(VK_unwrap(x))

#define PP_VK_impl_SIMD_UNWRAP_PARAM_redirect(index, type) VK_first_arg(type) const _##index
#define PP_VK_impl_SIMD_UNWRAP_PARAM PP_VK_impl_SIMD_UNWRAP_PARAM_redirect
#define PP_VK_impl_SIMD_UNWRAP_PARAMS(...) VK_foreach_indexed_delimit(PP_VK_impl_SIMD_UNWRAP_PARAM, VK_comma_delimiter, ##__VA_ARGS__)

#define PP_VK_impl_SIMD_UNWRAP_REG(t) . t##reg
#define PP_VK_impl_SIMD_UNWRAP_ARG_redirect_2(type, ...) VK_if(VK_not(VK_pack_is_empty(__VA_ARGS__))( PP_VK_impl_SIMD_UNWRAP_REG(__VA_ARGS__)))
#define PP_VK_impl_SIMD_UNWRAP_ARG_redirect(index, type) _##index PP_VK_impl_SIMD_UNWRAP_ARG_redirect_2 type
#define PP_VK_impl_SIMD_UNWRAP_ARG PP_VK_impl_SIMD_UNWRAP_ARG_redirect
#define PP_VK_impl_SIMD_UNWRAP_ARGS(...) VK_foreach_indexed_delimit(PP_VK_impl_SIMD_UNWRAP_ARG, VK_comma_delimiter, ##__VA_ARGS__)
#define PP_VK_impl_SIMD_UNWRAP_ARG_LO_redirect_2(type, ...) VK_if(VK_not(VK_pack_is_empty(__VA_ARGS__))( .reg_lo ))
#define PP_VK_impl_SIMD_UNWRAP_ARG_LO_redirect(index, type) _##index PP_VK_impl_SIMD_UNWRAP_ARG_LO_redirect_2 type
#define PP_VK_impl_SIMD_UNWRAP_ARG_LO PP_VK_impl_SIMD_UNWRAP_ARG_LO_redirect
#define PP_VK_impl_SIMD_UNWRAP_ARGS_LO(...) VK_foreach_indexed_delimit(PP_VK_impl_SIMD_UNWRAP_ARG_LO, VK_comma_delimiter, ##__VA_ARGS__)
#define PP_VK_impl_SIMD_UNWRAP_ARG_HI_redirect_2(type, ...) VK_if(VK_not(VK_pack_is_empty(__VA_ARGS__))( .reg_hi ))
#define PP_VK_impl_SIMD_UNWRAP_ARG_HI_redirect(index, type) _##index PP_VK_impl_SIMD_UNWRAP_ARG_HI_redirect_2 type
#define PP_VK_impl_SIMD_UNWRAP_ARG_HI PP_VK_impl_SIMD_UNWRAP_ARG_HI_redirect
#define PP_VK_impl_SIMD_UNWRAP_ARGS_HI(...) VK_foreach_indexed_delimit(PP_VK_impl_SIMD_UNWRAP_ARG_HI, VK_comma_delimiter, ##__VA_ARGS__)

#define PP_VK_impl_SIMD_RETURN_TYPE_redirect_2(...) VK_if(VK_not(VK_pack_is_empty(__VA_ARGS__))( VK_first_arg((__VA_ARGS__)) )VK_else( void ))
#define PP_VK_impl_SIMD_RETURN_TYPE_redirect(...) PP_VK_impl_SIMD_RETURN_TYPE_redirect_2(__VA_ARGS__)
#define PP_VK_impl_SIMD_RETURN_TYPE(x) PP_VK_impl_SIMD_RETURN_TYPE_redirect(VK_unwrap(x))


#define PP_VK_impl_SIMD_UNWRAP_RETURN_TYPE_INIT_2(ret, t, ...) return { PP_VK_impl_SIMD_UNWRAP_REG(t) = __VA_ARGS__ }
#define PP_VK_impl_SIMD_UNWRAP_RETURN_TYPE_INIT_1(ret, ...) return __VA_ARGS__
#define PP_VK_impl_SIMD_UNWRAP_RETURN_TYPE_INIT_0(...) __VA_ARGS__
#define PP_VK_impl_SIMD_UNWRAP_RET_TY_INIT_redirect(index, ...) VK_concat(PP_VK_impl_SIMD_UNWRAP_RETURN_TYPE_INIT_, index)(__VA_ARGS__)
#define PP_VK_impl_SIMD_UNWRAP_RET_TY_INIT PP_VK_impl_SIMD_UNWRAP_RET_TY_INIT_redirect
#define PP_VK_impl_SIMD_UNWRAP_RETURN_TYPE_INIT(ret, ...) PP_VK_impl_SIMD_UNWRAP_RET_TY_INIT(VK_tuple_size(VK_unwrap(ret)), VK_unwrap(ret), ##__VA_ARGS__)

#define PP_VK_impl_SIMD_FLAG_SUPPORTED_BODY(name, ret_type, params, intrin) \
  VK_forceinline VK_first_arg(ret_type) name(PP_VK_impl_SIMD_UNWRAP_PARAMS params) noexcept { \
    PP_VK_impl_SIMD_UNWRAP_RETURN_TYPE_INIT(ret_type, intrin(PP_VK_impl_SIMD_UNWRAP_ARGS params)); \
  }

#define PP_VK_impl_SIMD_FALLBACK_BODY(name, ret_type, params) \
  VK_forceinline VK_first_arg(ret_type) name(PP_VK_impl_SIMD_UNWRAP_PARAMS params) noexcept { \
    return { .reg_lo = name(PP_VK_impl_SIMD_UNWRAP_ARGS_LO params), .reg_hi = name(PP_VK_impl_SIMD_UNWRAP_ARGS_HI params) }; \
  }

#define PP_VK_impl_SIMD_BODY(name, ret_type, params, intrin, flag) \
  VK_if((VK_simd_supports(flag))(PP_VK_impl_SIMD_FLAG_SUPPORTED_BODY(name, ret_type, params, intrin))VK_else(PP_VK_impl_SIMD_FALLBACK_BODY(name, ret_type, params)))

#define VK_simd_function PP_VK_impl_SIMD_BODY


#if !defined(_WIN32)
// strcasecmp, strncasecmp
#include <strings.h>
#endif

#include <intrin.h>
#include <mmintrin.h>
#include <immintrin.h>






namespace valkyrie{
  
  
  
  

  namespace simd{
    namespace impl{

#define VK_scalar_binary_op(name, bits, type, func) \
  VK_forceinline base##bits name(base##bits a, base##bits b) noexcept { \
    return { . type##reg = func(a. type##reg, b. type##reg) };          \
  }

/*#define VK_simd_binary_x2_op(name, bits, type, flag, intrin) \
  VK_forceinline base##bits name(const base##bits& a, const base##bits& b) noexcept { \
    if ( cpu.has_##flag )                                                                       \
      return { . type##reg = intrin ( a. type##reg, b. type##reg ) };                           \
    return { .reg_lo = name(a.reg_lo, b.reg_lo), .reg_hi = name(a.reg_hi, b.reg_hi) };          \
  }*/
#define VK_simd_binary_bit_op(name, bits, type, intrin, ...) \
  VK_forceinline base##bits name(const base##bits& a, const base##bits& b) noexcept { \
    if ( cpu.has_##flag )                                                                       \
      return { . type##reg = intrin ( a. type##reg, b. type##reg ) };                           \
    return { .reg_lo = name(a.reg_lo, b.reg_lo), .reg_hi = name(a.reg_hi, b.reg_hi) };          \
  }

#define VK_simd_binary_bit_op_fallback(name, bits, type, func, ...) \
  VK_forceinline base##bits name(const base##bits& a, const base##bits b) noexcept {\
    return { .reg_lo = name(a.reg_lo, b.reg_lo), .reg_hi = name(a.reg_hi, b.reg_hi) };          \
  }
      
      
      
#define VK_binary_operator_alias(name, op) \
  template <typename T>        \
  VK_forceinline auto name(T a, T b) noexcept {      \
    if constexpr ( std::is_same_v<decltype(a op b), bool> ) \
      return a op b;                         \
    else return T(a op b); \
  }

#define VK_ivec(bits) (base##bits, i)
#define VK_fvec(bits) (base##bits, f)
#define VK_dvec(bits) (base##bits, d)
#define VK_uvec(bits) (base##bits, u)
#define VK_void ()


      // Forward-declared so they can be used by splat and friends.
      template<typename Child>
      struct base {
        __m256i value;

// Zero constructor
        VK_forceinline base() : value{__m256i()} {}

// Conversion from SIMD register
        VK_forceinline base(const __m256i _value) : value(_value) {}

// Conversion to SIMD register
        VK_forceinline operator const __m256i&() const { return this->value; }
        VK_forceinline operator __m256i&() { return this->value; }

// Bit operations
        VK_forceinline Child operator|(const Child other) const { return _mm256_or_si256(*this, other); }
        VK_forceinline Child operator&(const Child other) const { return _mm256_and_si256(*this, other); }
        VK_forceinline Child operator^(const Child other) const { return _mm256_xor_si256(*this, other); }
        VK_forceinline Child bit_andnot(const Child other) const { return _mm256_andnot_si256(other, *this); }
        VK_forceinline Child& operator|=(const Child other) { auto this_cast = static_cast<Child*>(this); *this_cast = *this_cast | other; return *this_cast; }
        VK_forceinline Child& operator&=(const Child other) { auto this_cast = static_cast<Child*>(this); *this_cast = *this_cast & other; return *this_cast; }
        VK_forceinline Child& operator^=(const Child other) { auto this_cast = static_cast<Child*>(this); *this_cast = *this_cast ^ other; return *this_cast; }
      };

// Forward-declared so they can be used by splat and friends.
      template<typename T>
      struct simd8;

      template<typename T, typename Mask=simd8<bool>>
      struct base8: base<simd8<T>> {
        typedef uint32_t bitmask_t;
        typedef uint64_t bitmask2_t;

        VK_forceinline base8() : base<simd8<T>>() {}
        VK_forceinline base8(const __m256i _value) : base<simd8<T>>(_value) {}

        VK_forceinline friend Mask operator==(const simd8<T>& a, const simd8<T>& b) noexcept {
          return _mm256_cmpeq_epi8(a, b);
        }

        static const int SIZE = sizeof(base<T>::value);

        template<int N=1>
        VK_forceinline simd8<T> prev(const simd8<T> prev_chunk) const {
          return _mm256_alignr_epi8(*this, _mm256_permute2x128_si256(prev_chunk, *this, 0x21), 16 - N);
        }
      };

// SIMD byte mask type (returned by things like eq and gt)
      template<>
      struct simd8<bool>: base8<bool> {
        static VK_forceinline simd8<bool> splat(bool _value) { return _mm256_set1_epi8(uint8_t(-(!!_value))); }

        VK_forceinline simd8<bool>() : base8() {}
        VK_forceinline simd8<bool>(const __m256i _value) : base8<bool>(_value) {}
// Splat constructor
        VK_forceinline simd8<bool>(bool _value) : base8<bool>(splat(_value)) {}

        VK_forceinline int to_bitmask() const { return _mm256_movemask_epi8(*this); }
        VK_forceinline bool any() const { return !_mm256_testz_si256(*this, *this); }
        VK_forceinline simd8<bool> operator~() const { return *this ^ true; }
      };

      template<typename T>
      struct base8_numeric: base8<T> {
        static VK_forceinline simd8<T> splat(T _value) { return _mm256_set1_epi8(_value); }
        static VK_forceinline simd8<T> zero() { return _mm256_setzero_si256(); }
        static VK_forceinline simd8<T> load(const T values[32]) {
          return _mm256_loadu_si256(reinterpret_cast<const __m256i *>(values));
        }
// Repeat 16 values as many times as necessary (usually for lookup tables)
        static VK_forceinline simd8<T> repeat_16(
            T v0,  T v1,  T v2,  T v3,  T v4,  T v5,  T v6,  T v7,
            T v8,  T v9,  T v10, T v11, T v12, T v13, T v14, T v15
        ) {
          return simd8<T>(
              v0, v1, v2, v3, v4, v5, v6, v7,
                  v8, v9, v10,v11,v12,v13,v14,v15,
                  v0, v1, v2, v3, v4, v5, v6, v7,
                  v8, v9, v10,v11,v12,v13,v14,v15
          );
        }

        VK_forceinline base8_numeric() : base8<T>() {}
        VK_forceinline base8_numeric(const __m256i _value) : base8<T>(_value) {}

// Store to array
        VK_forceinline void store(T dst[32]) const { return _mm256_storeu_si256(reinterpret_cast<__m256i *>(dst), *this); }

// Addition/subtraction are the same for signed and unsigned
        VK_forceinline simd8<T> operator+(const simd8<T> other) const { return _mm256_add_epi8(*this, other); }
        VK_forceinline simd8<T> operator-(const simd8<T> other) const { return _mm256_sub_epi8(*this, other); }
        VK_forceinline simd8<T>& operator+=(const simd8<T> other) { *this = *this + other; return *static_cast<simd8<T>*>(this); }
        VK_forceinline simd8<T>& operator-=(const simd8<T> other) { *this = *this - other; return *static_cast<simd8<T>*>(this); }

// Override to distinguish from bool version
        VK_forceinline simd8<T> operator~() const { return *this ^ 0xFFu; }

// Perform a lookup assuming the value is between 0 and 16 (undefined behavior for out of range values)
        template<typename L>
        VK_forceinline simd8<L> lookup_16(simd8<L> lookup_table) const {
          return _mm256_shuffle_epi8(lookup_table, *this);
        }

// Copies to 'output" all bytes corresponding to a 0 in the mask (interpreted as a bitset).
// Passing a 0 value for mask would be equivalent to writing out every byte to output.
// Only the first 32 - count_ones(mask) bytes of the result are significant but 32 bytes
// get written.
// Design consideration: it seems like a function with the
// signature simd8<L> compress(uint32_t mask) would be
// sensible, but the AVX ISA makes this kind of approach difficult.
        template<typename L>
        VK_forceinline void compress(uint32_t mask, L * output) const {
          using internal::thintable_epi8;
          using internal::BitsSetTable256mul2;
          using internal::pshufb_combine_table;
// this particular implementation was inspired by work done by @animetosho
// we do it in four steps, first 8 bytes and then second 8 bytes...
          uint8_t mask1 = uint8_t(mask); // least significant 8 bits
          uint8_t mask2 = uint8_t(mask >> 8); // second least significant 8 bits
          uint8_t mask3 = uint8_t(mask >> 16); // ...
          uint8_t mask4 = uint8_t(mask >> 24); // ...
// next line just loads the 64-bit values thintable_epi8[mask1] and
// thintable_epi8[mask2] into a 128-bit register, using only
// two instructions on most compilers.
          __m256i shufmask =  _mm256_set_epi64x(thintable_epi8[mask4], thintable_epi8[mask3],
                                                thintable_epi8[mask2], thintable_epi8[mask1]);
// we increment by 0x08 the second half of the mask and so forth
          shufmask =
              _mm256_add_epi8(shufmask, _mm256_set_epi32(0x18181818, 0x18181818,
                                                         0x10101010, 0x10101010, 0x08080808, 0x08080808, 0, 0));
// this is the version "nearly pruned"
          __m256i pruned = _mm256_shuffle_epi8(*this, shufmask);
// we still need to put the  pieces back together.
// we compute the popcount of the first words:
          int pop1 = BitsSetTable256mul2[mask1];
          int pop3 = BitsSetTable256mul2[mask3];

// then load the corresponding mask
// could be done with _mm256_loadu2_m128i but many standard libraries omit this intrinsic.
          __m256i v256 = _mm256_castsi128_si256(
              _mm_loadu_si128(reinterpret_cast<const __m128i *>(pshufb_combine_table + pop1 * 8)));
          __m256i compactmask = _mm256_insertf128_si256(v256,
                                                        _mm_loadu_si128(reinterpret_cast<const __m128i *>(pshufb_combine_table + pop3 * 8)), 1);
          __m256i almostthere =  _mm256_shuffle_epi8(pruned, compactmask);
// We just need to write out the result.
// This is the tricky bit that is hard to do
// if we want to return a SIMD register, since there
// is no single-instruction approach to recombine
// the two 128-bit lanes with an offset.
          __m128i v128;
          v128 = _mm256_castsi256_si128(almostthere);
          _mm_storeu_si128( reinterpret_cast<__m128i *>(output), v128);
          v128 = _mm256_extractf128_si256(almostthere, 1);
          _mm_storeu_si128( reinterpret_cast<__m128i *>(output + 16 - count_ones(mask & 0xFFFF)), v128);
        }

        template<typename L>
        VK_forceinline simd8<L> lookup_16(
            L replace0,  L replace1,  L replace2,  L replace3,
            L replace4,  L replace5,  L replace6,  L replace7,
            L replace8,  L replace9,  L replace10, L replace11,
            L replace12, L replace13, L replace14, L replace15) const {
          return lookup_16(simd8<L>::repeat_16(
              replace0,  replace1,  replace2,  replace3,
              replace4,  replace5,  replace6,  replace7,
              replace8,  replace9,  replace10, replace11,
              replace12, replace13, replace14, replace15
          ));
        }
      };

// Signed bytes
      template<>
      struct simd8<int8_t> : base8_numeric<int8_t> {
        VK_forceinline simd8() : base8_numeric<int8_t>() {}
        VK_forceinline simd8(const __m256i _value) : base8_numeric<int8_t>(_value) {}
// Splat constructor
        VK_forceinline simd8(int8_t _value) : simd8(splat(_value)) {}
// Array constructor
        VK_forceinline simd8(const int8_t values[32]) : simd8(load(values)) {}
// Member-by-member initialization
        VK_forceinline simd8(
            int8_t v0,  int8_t v1,  int8_t v2,  int8_t v3,  int8_t v4,  int8_t v5,  int8_t v6,  int8_t v7,
            int8_t v8,  int8_t v9,  int8_t v10, int8_t v11, int8_t v12, int8_t v13, int8_t v14, int8_t v15,
            int8_t v16, int8_t v17, int8_t v18, int8_t v19, int8_t v20, int8_t v21, int8_t v22, int8_t v23,
            int8_t v24, int8_t v25, int8_t v26, int8_t v27, int8_t v28, int8_t v29, int8_t v30, int8_t v31
        ) : simd8(_mm256_setr_epi8(
            v0, v1, v2, v3, v4, v5, v6, v7,
            v8, v9, v10,v11,v12,v13,v14,v15,
            v16,v17,v18,v19,v20,v21,v22,v23,
            v24,v25,v26,v27,v28,v29,v30,v31
        )) {}
// Repeat 16 values as many times as necessary (usually for lookup tables)
        VK_forceinline static simd8<int8_t> repeat_16(
            int8_t v0,  int8_t v1,  int8_t v2,  int8_t v3,  int8_t v4,  int8_t v5,  int8_t v6,  int8_t v7,
            int8_t v8,  int8_t v9,  int8_t v10, int8_t v11, int8_t v12, int8_t v13, int8_t v14, int8_t v15
        ) {
          return simd8<int8_t>(
              v0, v1, v2, v3, v4, v5, v6, v7,
              v8, v9, v10,v11,v12,v13,v14,v15,
              v0, v1, v2, v3, v4, v5, v6, v7,
              v8, v9, v10,v11,v12,v13,v14,v15
          );
        }

// Order-sensitive comparisons
        VK_forceinline simd8<int8_t> max_val(const simd8<int8_t> other) const { return _mm256_max_epi8(*this, other); }
        VK_forceinline simd8<int8_t> min_val(const simd8<int8_t> other) const { return _mm256_min_epi8(*this, other); }
        VK_forceinline simd8<bool> operator>(const simd8<int8_t> other) const { return _mm256_cmpgt_epi8(*this, other); }
        VK_forceinline simd8<bool> operator<(const simd8<int8_t> other) const { return _mm256_cmpgt_epi8(other, *this); }
      };

// Unsigned bytes
      template<>
      struct simd8<uint8_t>: base8_numeric<uint8_t> {
        VK_forceinline simd8() : base8_numeric<uint8_t>() {}
        VK_forceinline simd8(const __m256i _value) : base8_numeric<uint8_t>(_value) {}
// Splat constructor
        VK_forceinline simd8(uint8_t _value) : simd8(splat(_value)) {}
// Array constructor
        VK_forceinline simd8(const uint8_t values[32]) : simd8(load(values)) {}
// Member-by-member initialization
        VK_forceinline simd8(
            uint8_t v0,  uint8_t v1,  uint8_t v2,  uint8_t v3,  uint8_t v4,  uint8_t v5,  uint8_t v6,  uint8_t v7,
            uint8_t v8,  uint8_t v9,  uint8_t v10, uint8_t v11, uint8_t v12, uint8_t v13, uint8_t v14, uint8_t v15,
            uint8_t v16, uint8_t v17, uint8_t v18, uint8_t v19, uint8_t v20, uint8_t v21, uint8_t v22, uint8_t v23,
            uint8_t v24, uint8_t v25, uint8_t v26, uint8_t v27, uint8_t v28, uint8_t v29, uint8_t v30, uint8_t v31
        ) : simd8(_mm256_setr_epi8(
            v0, v1, v2, v3, v4, v5, v6, v7,
            v8, v9, v10,v11,v12,v13,v14,v15,
            v16,v17,v18,v19,v20,v21,v22,v23,
            v24,v25,v26,v27,v28,v29,v30,v31
        )) {}
// Repeat 16 values as many times as necessary (usually for lookup tables)
        VK_forceinline static simd8<uint8_t> repeat_16(
            uint8_t v0,  uint8_t v1,  uint8_t v2,  uint8_t v3,  uint8_t v4,  uint8_t v5,  uint8_t v6,  uint8_t v7,
            uint8_t v8,  uint8_t v9,  uint8_t v10, uint8_t v11, uint8_t v12, uint8_t v13, uint8_t v14, uint8_t v15
        ) {
          return simd8<uint8_t>(
              v0, v1, v2, v3, v4, v5, v6, v7,
              v8, v9, v10,v11,v12,v13,v14,v15,
              v0, v1, v2, v3, v4, v5, v6, v7,
              v8, v9, v10,v11,v12,v13,v14,v15
          );
        }

// Saturated math
        VK_forceinline simd8<uint8_t> saturating_add(const simd8<uint8_t> other) const { return _mm256_adds_epu8(*this, other); }
        VK_forceinline simd8<uint8_t> saturating_sub(const simd8<uint8_t> other) const { return _mm256_subs_epu8(*this, other); }

// Order-specific operations
        VK_forceinline simd8<uint8_t> max_val(const simd8<uint8_t> other) const { return _mm256_max_epu8(*this, other); }
        VK_forceinline simd8<uint8_t> min_val(const simd8<uint8_t> other) const { return _mm256_min_epu8(other, *this); }
// Same as >, but only guarantees true is nonzero (< guarantees true = -1)
        VK_forceinline simd8<uint8_t> gt_bits(const simd8<uint8_t> other) const { return this->saturating_sub(other); }
// Same as <, but only guarantees true is nonzero (< guarantees true = -1)
        VK_forceinline simd8<uint8_t> lt_bits(const simd8<uint8_t> other) const { return other.saturating_sub(*this); }
        VK_forceinline simd8<bool> operator<=(const simd8<uint8_t> other) const { return other.max_val(*this) == other; }
        VK_forceinline simd8<bool> operator>=(const simd8<uint8_t> other) const { return other.min_val(*this) == other; }
        VK_forceinline simd8<bool> operator>(const simd8<uint8_t> other) const { return this->gt_bits(other).any_bits_set(); }
        VK_forceinline simd8<bool> operator<(const simd8<uint8_t> other) const { return this->lt_bits(other).any_bits_set(); }

// Bit-specific operations
        VK_forceinline simd8<bool> bits_not_set() const { return *this == uint8_t(0); }
        VK_forceinline simd8<bool> bits_not_set(simd8<uint8_t> bits) const { return (*this & bits).bits_not_set(); }
        VK_forceinline simd8<bool> any_bits_set() const { return ~this->bits_not_set(); }
        VK_forceinline simd8<bool> any_bits_set(simd8<uint8_t> bits) const { return ~this->bits_not_set(bits); }
        VK_forceinline bool is_ascii() const { return _mm256_movemask_epi8(*this) == 0; }
        VK_forceinline bool bits_not_set_anywhere() const { return _mm256_testz_si256(*this, *this); }
        VK_forceinline bool any_bits_set_anywhere() const { return !bits_not_set_anywhere(); }
        VK_forceinline bool bits_not_set_anywhere(simd8<uint8_t> bits) const { return _mm256_testz_si256(*this, bits); }
        VK_forceinline bool any_bits_set_anywhere(simd8<uint8_t> bits) const { return !bits_not_set_anywhere(bits); }
        template<int N>
        VK_forceinline simd8<uint8_t> shr() const { return simd8<uint8_t>(_mm256_srli_epi16(*this, N)) & uint8_t(0xFFu >> N); }
        template<int N>
        VK_forceinline simd8<uint8_t> shl() const { return simd8<uint8_t>(_mm256_slli_epi16(*this, N)) & uint8_t(0xFFu << N); }
// Get one of the bits and make a bitmask out of it.
// e.g. value.get_bit<7>() gets the high bit
        template<int N>
        VK_forceinline int get_bit() const { return _mm256_movemask_epi8(_mm256_slli_epi16(*this, 7-N)); }
      };

      template<typename T>
      struct simd8x64 {
        static constexpr int NUM_CHUNKS = 64 / sizeof(simd8<T>);
        static_assert(NUM_CHUNKS == 2, "Haswell kernel should use two registers per 64-byte block.");
        const simd8<T> chunks[NUM_CHUNKS];

        simd8x64(const simd8x64<T>& o) = delete; // no copy allowed
        simd8x64<T>& operator=(const simd8<T>& other) = delete; // no assignment allowed
        simd8x64() = delete; // no default constructor allowed

        VK_forceinline simd8x64(const simd8<T> chunk0, const simd8<T> chunk1) : chunks{chunk0, chunk1} {}
        VK_forceinline simd8x64(const T ptr[64]) : chunks{simd8<T>::load(ptr), simd8<T>::load(ptr+32)} {}

        VK_forceinline void compress(uint64_t mask, T * output) const {
          uint32_t mask1 = uint32_t(mask);
          uint32_t mask2 = uint32_t(mask >> 32);
          this->chunks[0].compress(mask1, output);
          this->chunks[1].compress(mask2, output + 32 - count_ones(mask1));
        }

        VK_forceinline void store(T ptr[64]) const {
          this->chunks[0].store(ptr+sizeof(simd8<T>)*0);
          this->chunks[1].store(ptr+sizeof(simd8<T>)*1);
        }

        VK_forceinline uint64_t to_bitmask() const {
          uint64_t r_lo = uint32_t(this->chunks[0].to_bitmask());
          uint64_t r_hi =                       this->chunks[1].to_bitmask();
          return r_lo | (r_hi << 32);
        }

        VK_forceinline simd8<T> reduce_or() const {
          return this->chunks[0] | this->chunks[1];
        }

        VK_forceinline simd8x64<T> bit_or(const T m) const {
          const simd8<T> mask = simd8<T>::splat(m);
          return simd8x64<T>(
              this->chunks[0] | mask,
              this->chunks[1] | mask
          );
        }

        VK_forceinline uint64_t eq(const T m) const {
          const simd8<T> mask = simd8<T>::splat(m);
          return  simd8x64<bool>(
              this->chunks[0] == mask,
              this->chunks[1] == mask
          ).to_bitmask();
        }

        VK_forceinline uint64_t eq(const simd8x64<uint8_t> &other) const {
          return  simd8x64<bool>(
              this->chunks[0] == other.chunks[0],
              this->chunks[1] == other.chunks[1]
          ).to_bitmask();
        }

        VK_forceinline uint64_t lteq(const T m) const {
          const simd8<T> mask = simd8<T>::splat(m);
          return  simd8x64<bool>(
              this->chunks[0] <= mask,
              this->chunks[1] <= mask
          ).to_bitmask();
        }
      }; // struct simd8x64<T>


      union base16{
          u16 freg; // TODO: replace with f16 type when implemented
          u16 ureg;
          i16 ireg;
      };
      union base32{
        f32 freg;
        u32 ureg;
        i32 ireg;
        struct {
          base16 reg_lo;
          base16 reg_hi;
        };
      };
      union base64{

          f32 freg[2];
          f64 dreg;
          u64 ureg;
          i64 ireg;
          struct {
            base32 reg_lo;
            base32 reg_hi;
          };
      };
      union base128{

          __m128  freg;
          __m128i ireg;
          __m128i ureg;
          __m128d dreg;

          struct {
            base64 reg_lo;
            base64 reg_hi;
          };

      };
      union base256{

          __m256  freg;
          __m256i ireg;
          __m256i ureg;
          __m256d dreg;

          struct {
            base128 reg_lo;
            base128 reg_hi;
          };

      };
      union base512{

          __m512  freg;
          __m512i ireg;
          __m512i ureg;
          __m512d dreg;
          struct {
            base256 reg_lo;
            base256 reg_hi;
          };

      };


      template <size_t N>
      struct select_base;
      template <>
      struct select_base<2>{
        using type = base16;
      };
      template <>
      struct select_base<4>{
        using type = base32;
      };
      template <>
      struct select_base<8>{
        using type = base64;
      };
      template <>
      struct select_base<16>{
        using type = base128;
      };
      template <>
      struct select_base<32>{
        using type = base256;
      };
      template <>
      struct select_base<64>{
        using type = base512;
      };

      template <typename T, size_t N>
      using select_base_t = typename select_base<sizeof(T) * N>::type;

      template <size_t TSize, size_t xN, bool IsFloatingPoint>
      class erased_base : public select_base<TSize * xN>::type {
        using base = typename select_base<TSize * xN>::type;
      public:
        constexpr erased_base() = default;
        constexpr erased_base(const base& vec) noexcept : base(vec){ }
      };




      VK_binary_operator_alias(_bitand, &)
      VK_binary_operator_alias(_bitor,  |)
      VK_binary_operator_alias(_xor,    ^)
      VK_binary_operator_alias(_and,    &&)
      VK_binary_operator_alias(_or,     ||)
      VK_binary_operator_alias(_plus,   +)
      VK_binary_operator_alias(_sub,    -)
      VK_binary_operator_alias(_mul,    *)
      VK_binary_operator_alias(_div,    /)
      VK_binary_operator_alias(_mod,    %)
      VK_binary_operator_alias(_eq,     ==)
      VK_binary_operator_alias(_neq,    !=)
      VK_binary_operator_alias(_lt,     <)
      VK_binary_operator_alias(_gt,     >)
      VK_binary_operator_alias(_lt_eq,  <=)
      VK_binary_operator_alias(_gt_eq,  >=)
      VK_binary_operator_alias(_lshift, <<)
      VK_binary_operator_alias(_rshift, >>)


      VK_forceinline base16 bit_and(const base16 a, const base16 b) noexcept {
        return { .ureg = static_cast<u16>(a.ureg & b.ureg) };
      }
      VK_forceinline base32 bit_and(const base32 a, const base32 b) noexcept {
        return { .ureg = a.ureg & b.ureg };
      }
      VK_forceinline base64 bit_and(const base64 a, const base64 b) noexcept {
        return { .ureg = a.ureg & b.ureg };
      }

      VK_forceinline base16 bit_andnot(const base16 a, const base16 b) noexcept {
        return { .ureg = static_cast<u16>(~a.ureg & b.ureg) };
      }
      VK_forceinline base32 bit_andnot(const base32 a, const base32 b) noexcept {
        return { .ureg = ~a.ureg & b.ureg };
      }
      VK_forceinline base64 bit_andnot(const base64 a, const base64 b) noexcept {
        return { .ureg = ~a.ureg & b.ureg };
      }

      VK_forceinline base16 bit_or(const base16 a, const base16 b) noexcept {
        return { .ureg = static_cast<u16>(a.ureg | b.ureg) };
      }
      VK_forceinline base32 bit_or(const base32 a, const base32 b) noexcept {
        return { .ureg = a.ureg | b.ureg };
      }
      VK_forceinline base64 bit_or(const base64 a, const base64 b) noexcept {
        return { .ureg = a.ureg | b.ureg };
      }

      VK_forceinline base16 bit_xor(const base16 a, const base16 b) noexcept {
        return { .ureg = static_cast<u16>(a.ureg ^ b.ureg) };
      }
      VK_forceinline base32 bit_xor(const base32 a, const base32 b) noexcept {
        return { .ureg = a.ureg ^ b.ureg };
      }
      VK_forceinline base64 bit_xor(const base64 a, const base64 b) noexcept {
        return { .ureg = a.ureg ^ b.ureg };
      }



      VK_simd_function(bit_and,    VK_uvec(128), (VK_uvec(128), VK_uvec(128)), _mm_and_si128,    sse2)
      VK_simd_function(bit_and,    VK_uvec(256), (VK_uvec(256), VK_uvec(256)), _mm256_and_si256, avx2)
      VK_simd_function(bit_and,    VK_uvec(512), (VK_uvec(512), VK_uvec(512)), _mm512_and_si512, avx512f)

      VK_simd_function(bit_andnot, VK_uvec(128), (VK_uvec(128), VK_uvec(128)), _mm_andnot_si128,    sse2)
      VK_simd_function(bit_andnot, VK_uvec(256), (VK_uvec(256), VK_uvec(256)), _mm256_andnot_si256, avx2)
      VK_simd_function(bit_andnot, VK_uvec(512), (VK_uvec(512), VK_uvec(512)), _mm512_andnot_si512, avx512f)

      VK_simd_function(bit_or,     VK_uvec(128), (VK_uvec(128), VK_uvec(128)), _mm_or_si128,    sse2)
      VK_simd_function(bit_or,     VK_uvec(256), (VK_uvec(256), VK_uvec(256)), _mm256_or_si256, avx2)
      VK_simd_function(bit_or,     VK_uvec(512), (VK_uvec(512), VK_uvec(512)), _mm512_or_si512, avx512f)

      VK_simd_function(bit_xor,    VK_uvec(128), (VK_uvec(128), VK_uvec(128)), _mm_xor_si128,    sse2)
      VK_simd_function(bit_xor,    VK_uvec(256), (VK_uvec(256), VK_uvec(256)), _mm256_xor_si256, avx2)
      VK_simd_function(bit_xor,    VK_uvec(512), (VK_uvec(512), VK_uvec(512)), _mm512_xor_si512, avx512f)


      VK_simd_function(load, VK_void, ((base128*, i), (const void*)), _mm_load_si128, sse2)





      VK_simd_binary_bit_op(iadd_x2, 128, i, sse2, _mm_add_epi64)











      template <typename T>
      class x2  : select_base_t<T, 2>{
        using base = select_base_t<T, 2>;
      public:

      };
      template <typename T>
      class x4  : select_base_t<T, 4>{ };
      template <typename T>
      class x8  : select_base_t<T, 8>{ };
      template <typename T>
      class x16 : select_base_t<T, 16>{ };
      template <typename T>
      class x32 : select_base_t<T, 32>{ };
      template <typename T>
      class x64 : select_base_t<T, 64>{ };
    }

    using boolx2  = impl::x2<bool>;
    using boolx4  = impl::x4<bool>;
    using boolx8  = impl::x8<bool>;
    using boolx16 = impl::x16<bool>;
    using boolx32 = impl::x32<bool>;
    using boolx64 = impl::x64<bool>;

    using charx2  = impl::x2<char>;
    using charx4  = impl::x4<char>;
    using charx8  = impl::x8<char>;
    using charx16 = impl::x16<char>;
    using charx32 = impl::x32<char>;
    using charx64 = impl::x64<char>;

    using utf8x2  = impl::x2<utf8>;
    using utf8x4  = impl::x4<utf8>;
    using utf8x8  = impl::x8<utf8>;
    using utf8x16 = impl::x16<utf8>;
    using utf8x32 = impl::x32<utf8>;
    using utf8x64 = impl::x64<utf8>;

    using i8x2  = impl::x2<i8>;
    using i8x4  = impl::x4<i8>;
    using i8x8  = impl::x8<i8>;
    using i8x16 = impl::x16<i8>;
    using i8x32 = impl::x32<i8>;
    using i8x64 = impl::x64<i8>;

    using u8x2  = impl::x2<u8>;
    using u8x4  = impl::x4<u8>;
    using u8x8  = impl::x8<u8>;
    using u8x16 = impl::x16<u8>;
    using u8x32 = impl::x32<u8>;
    using u8x64 = impl::x64<u8>;


    // TODO: Implement f16 (half) floating point type
#if 0
    using f16x2  = impl::x2<f16>;
    using f16x4  = impl::x4<f16>;
    using f16x8  = impl::x8<f16>;
    using f16x16 = impl::x16<f16>;
    using f16x32 = impl::x32<f16>;
#endif

    using i16x2  = impl::x2<i16>;
    using i16x4  = impl::x4<i16>;
    using i16x8  = impl::x8<i16>;
    using i16x16 = impl::x16<i16>;
    using i16x32 = impl::x32<i16>;

    using u16x2  = impl::x2<u16>;
    using u16x4  = impl::x4<u16>;
    using u16x8  = impl::x8<u16>;
    using u16x16 = impl::x16<u16>;
    using u16x32 = impl::x32<u16>;

    using utf16x2  = impl::x2<utf16>;
    using utf16x4  = impl::x4<utf16>;
    using utf16x8  = impl::x8<utf16>;
    using utf16x16 = impl::x16<utf16>;
    using utf16x32 = impl::x32<utf16>;


    using wcharx2  = impl::x2<wchar_t>;
    using wcharx4  = impl::x4<wchar_t>;
    using wcharx8  = impl::x8<wchar_t>;
    using wcharx16 = impl::x16<wchar_t>;
#if VK_system_windows
    using wcharx32 = impl::x32<wchar_t>;
#endif



    using i32x2  = impl::x2<i32>;
    using i32x4  = impl::x4<i32>;
    using i32x8  = impl::x8<i32>;
    using i32x16 = impl::x16<i32>;

    using u32x2  = impl::x2<u32>;
    using u32x4  = impl::x4<u32>;
    using u32x8  = impl::x8<u32>;
    using u32x16 = impl::x16<u32>;

    using utf32x2  = impl::x2<utf32>;
    using utf32x4  = impl::x4<utf32>;
    using utf32x8  = impl::x8<utf32>;
    using utf32x16 = impl::x16<utf32>;

    using f32x2  = impl::x2<f32>;
    using f32x4  = impl::x4<f32>;
    using f32x8  = impl::x8<f32>;
    using f32x16 = impl::x16<f32>;



    using i64x2  = impl::x2<i64>;
    using i64x4  = impl::x4<i64>;
    using i64x8  = impl::x8<i64>;

    using u64x2  = impl::x2<u64>;
    using u64x4  = impl::x4<u64>;
    using u64x8  = impl::x8<u64>;

    using f64x2  = impl::x2<f64>;
    using f64x4  = impl::x4<f64>;
    using f64x8  = impl::x8<f64>;

  }








  namespace impl{
    template<typename Child>
    struct base256 {
      __m256i value;

// Zero constructor
      VK_forceinline base() : value{__m256i()} {}

// Conversion from SIMD register
      VK_forceinline base(const __m256i _value) : value(_value) {}

// Conversion to SIMD register
      VK_forceinline operator const __m256i&() const { return this->value; }
      VK_forceinline operator __m256i&() { return this->value; }

// Bit operations
      VK_forceinline Child operator|(const Child other) const { return _mm256_or_si256(*this, other); }
      VK_forceinline Child operator&(const Child other) const { return _mm256_and_si256(*this, other); }
      VK_forceinline Child operator^(const Child other) const { return _mm256_xor_si256(*this, other); }
      VK_forceinline Child bit_andnot(const Child other) const { return _mm256_andnot_si256(other, *this); }
      VK_forceinline Child& operator|=(const Child other) { auto this_cast = static_cast<Child*>(this); *this_cast = *this_cast | other; return *this_cast; }
      VK_forceinline Child& operator&=(const Child other) { auto this_cast = static_cast<Child*>(this); *this_cast = *this_cast & other; return *this_cast; }
      VK_forceinline Child& operator^=(const Child other) { auto this_cast = static_cast<Child*>(this); *this_cast = *this_cast ^ other; return *this_cast; }
    };
    template<typename Child>
    struct base128 {
      __m128i value;

// Zero constructor
      VK_forceinline base() : value{__m256i()} {}

// Conversion from SIMD register
      VK_forceinline base(const __m256i _value) : value(_value) {}

// Conversion to SIMD register
      VK_forceinline operator const __m256i&() const { return this->value; }
      VK_forceinline operator __m256i&() { return this->value; }

// Bit operations
      VK_forceinline Child operator|(const Child other) const { return _mm_or_si128(*this, other); }
      VK_forceinline Child operator&(const Child other) const { return _mm_and_si128(*this, other); }
      VK_forceinline Child operator^(const Child other) const { return _mm_xor_si128(*this, other); }
      VK_forceinline Child bit_andnot(const Child other) const { return _mm_andnot_si128(other, *this); }
      VK_forceinline Child& operator|=(const Child other) { auto this_cast = static_cast<Child*>(this); *this_cast = *this_cast | other; return *this_cast; }
      VK_forceinline Child& operator&=(const Child other) { auto this_cast = static_cast<Child*>(this); *this_cast = *this_cast & other; return *this_cast; }
      VK_forceinline Child& operator^=(const Child other) { auto this_cast = static_cast<Child*>(this); *this_cast = *this_cast ^ other; return *this_cast; }
    };
    template<typename T, typename Mask=simd8<bool>>
    struct base8: base<simd8<T>> {
    typedef uint32_t bitmask_t;
    typedef uint64_t bitmask2_t;

    VK_forceinline base8() : base<simd8<T>>() {}
    VK_forceinline base8(const __m256i _value) : base<simd8<T>>(_value) {}

  VK_forceinline Mask operator==(const simd8<T> other) const { return _mm256_cmpeq_epi8(*this, other); }

  static const int SIZE = sizeof(base<T>::value);

  template<int N=1>
  VK_forceinline simd8<T> prev(const simd8<T> prev_chunk) const {
    return _mm256_alignr_epi8(*this, _mm256_permute2x128_si256(prev_chunk, *this, 0x21), 16 - N);
  }
};
  }
  


#if defined(__x86_64__) || defined(_M_AMD64)


  template<>
  struct simd8<bool>: impl::base8<bool> {
    static VK_forceinline simd8<bool> splat(bool _value) { return _mm256_set1_epi8(uint8_t(-(!!_value))); }

    VK_forceinline simd8<bool>() : base8() {}
    VK_forceinline simd8<bool>(const __m256i _value) : base8<bool>(_value) {}
// Splat constructor
    VK_forceinline simd8<bool>(bool _value) : base8<bool>(splat(_value)) {}

    VK_forceinline int to_bitmask() const { return _mm256_movemask_epi8(*this); }
    VK_forceinline bool any() const { return !_mm256_testz_si256(*this, *this); }
    VK_forceinline simd8<bool> operator~() const { return *this ^ true; }
  };


  template<typename T>
  struct base8_numeric: impl::base8<T> {
    static VK_forceinline simd8<T> splat(T _value) { return _mm256_set1_epi8(_value); }
    static VK_forceinline simd8<T> zero() { return _mm256_setzero_si256(); }
    static VK_forceinline simd8<T> load(const T values[32]) {
      return _mm256_loadu_si256(reinterpret_cast<const __m256i *>(values));
    }
// Repeat 16 values as many times as necessary (usually for lookup tables)
    static VK_forceinline simd8<T> repeat_16(
        T v0,  T v1,  T v2,  T v3,  T v4,  T v5,  T v6,  T v7,
        T v8,  T v9,  T v10, T v11, T v12, T v13, T v14, T v15
    ) {
      return simd8<T>(
          v0, v1, v2, v3, v4, v5, v6, v7,
              v8, v9, v10,v11,v12,v13,v14,v15,
              v0, v1, v2, v3, v4, v5, v6, v7,
              v8, v9, v10,v11,v12,v13,v14,v15
      );
    }

    VK_forceinline base8_numeric() : base8<T>() {}
    VK_forceinline base8_numeric(const __m256i _value) : base8<T>(_value) {}

// Store to array
    VK_forceinline void store(T dst[32]) const { return _mm256_storeu_si256(reinterpret_cast<__m256i *>(dst), *this); }

// Addition/subtraction are the same for signed and unsigned
    VK_forceinline simd8<T> operator+(const simd8<T> other) const { return _mm256_add_epi8(*this, other); }
    VK_forceinline simd8<T> operator-(const simd8<T> other) const { return _mm256_sub_epi8(*this, other); }
    VK_forceinline simd8<T>& operator+=(const simd8<T> other) { *this = *this + other; return *static_cast<simd8<T>*>(this); }
    VK_forceinline simd8<T>& operator-=(const simd8<T> other) { *this = *this - other; return *static_cast<simd8<T>*>(this); }

// Override to distinguish from bool version
    VK_forceinline simd8<T> operator~() const { return *this ^ 0xFFu; }

// Perform a lookup assuming the value is between 0 and 16 (undefined behavior for out of range values)
    template<typename L>
    VK_forceinline simd8<L> lookup_16(simd8<L> lookup_table) const {
      return _mm256_shuffle_epi8(lookup_table, *this);
    }

// Copies to 'output" all bytes corresponding to a 0 in the mask (interpreted as a bitset).
// Passing a 0 value for mask would be equivalent to writing out every byte to output.
// Only the first 32 - count_ones(mask) bytes of the result are significant but 32 bytes
// get written.
// Design consideration: it seems like a function with the
// signature simd8<L> compress(uint32_t mask) would be
// sensible, but the AVX ISA makes this kind of approach difficult.
    template<typename L>
    VK_forceinline void compress(uint32_t mask, L * output) const {
      using internal::thintable_epi8;
      using internal::BitsSetTable256mul2;
      using internal::pshufb_combine_table;
// this particular implementation was inspired by work done by @animetosho
// we do it in four steps, first 8 bytes and then second 8 bytes...
      uint8_t mask1 = uint8_t(mask); // least significant 8 bits
      uint8_t mask2 = uint8_t(mask >> 8); // second least significant 8 bits
      uint8_t mask3 = uint8_t(mask >> 16); // ...
      uint8_t mask4 = uint8_t(mask >> 24); // ...
// next line just loads the 64-bit values thintable_epi8[mask1] and
// thintable_epi8[mask2] into a 128-bit register, using only
// two instructions on most compilers.
      __m256i shufmask =  _mm256_set_epi64x(thintable_epi8[mask4], thintable_epi8[mask3],
                                            thintable_epi8[mask2], thintable_epi8[mask1]);
// we increment by 0x08 the second half of the mask and so forth
      shufmask =
          _mm256_add_epi8(shufmask, _mm256_set_epi32(0x18181818, 0x18181818,
                                                     0x10101010, 0x10101010, 0x08080808, 0x08080808, 0, 0));
// this is the version "nearly pruned"
      __m256i pruned = _mm256_shuffle_epi8(*this, shufmask);
// we still need to put the  pieces back together.
// we compute the popcount of the first words:
      int pop1 = BitsSetTable256mul2[mask1];
      int pop3 = BitsSetTable256mul2[mask3];

// then load the corresponding mask
// could be done with _mm256_loadu2_m128i but many standard libraries omit this intrinsic.
      __m256i v256 = _mm256_castsi128_si256(
          _mm_loadu_si128(reinterpret_cast<const __m128i *>(pshufb_combine_table + pop1 * 8)));
      __m256i compactmask = _mm256_insertf128_si256(v256,
                                                    _mm_loadu_si128(reinterpret_cast<const __m128i *>(pshufb_combine_table + pop3 * 8)), 1);
      __m256i almostthere =  _mm256_shuffle_epi8(pruned, compactmask);
// We just need to write out the result.
// This is the tricky bit that is hard to do
// if we want to return a SIMD register, since there
// is no single-instruction approach to recombine
// the two 128-bit lanes with an offset.
      __m128i v128;
      v128 = _mm256_castsi256_si128(almostthere);
      _mm_storeu_si128( reinterpret_cast<__m128i *>(output), v128);
      v128 = _mm256_extractf128_si256(almostthere, 1);
      _mm_storeu_si128( reinterpret_cast<__m128i *>(output + 16 - count_ones(mask & 0xFFFF)), v128);
    }

    template<typename L>
    VK_forceinline simd8<L> lookup_16(
        L replace0,  L replace1,  L replace2,  L replace3,
        L replace4,  L replace5,  L replace6,  L replace7,
        L replace8,  L replace9,  L replace10, L replace11,
        L replace12, L replace13, L replace14, L replace15) const {
      return lookup_16(simd8<L>::repeat_16(
          replace0,  replace1,  replace2,  replace3,
          replace4,  replace5,  replace6,  replace7,
          replace8,  replace9,  replace10, replace11,
          replace12, replace13, replace14, replace15
      ));
    }
  };

  // Signed bytes
  template<>
  struct simd8<int8_t> : base8_numeric<int8_t> {
    VK_forceinline simd8() : base8_numeric<int8_t>() {}
    VK_forceinline simd8(const __m256i _value) : base8_numeric<int8_t>(_value) {}
// Splat constructor
    VK_forceinline simd8(int8_t _value) : simd8(splat(_value)) {}
// Array constructor
    VK_forceinline simd8(const int8_t values[32]) : simd8(load(values)) {}
// Member-by-member initialization
    VK_forceinline simd8(
        int8_t v0,  int8_t v1,  int8_t v2,  int8_t v3,  int8_t v4,  int8_t v5,  int8_t v6,  int8_t v7,
        int8_t v8,  int8_t v9,  int8_t v10, int8_t v11, int8_t v12, int8_t v13, int8_t v14, int8_t v15,
        int8_t v16, int8_t v17, int8_t v18, int8_t v19, int8_t v20, int8_t v21, int8_t v22, int8_t v23,
        int8_t v24, int8_t v25, int8_t v26, int8_t v27, int8_t v28, int8_t v29, int8_t v30, int8_t v31
    ) : simd8(_mm256_setr_epi8(
        v0, v1, v2, v3, v4, v5, v6, v7,
        v8, v9, v10,v11,v12,v13,v14,v15,
        v16,v17,v18,v19,v20,v21,v22,v23,
        v24,v25,v26,v27,v28,v29,v30,v31
    )) {}
// Repeat 16 values as many times as necessary (usually for lookup tables)
    VK_forceinline static simd8<int8_t> repeat_16(
        int8_t v0,  int8_t v1,  int8_t v2,  int8_t v3,  int8_t v4,  int8_t v5,  int8_t v6,  int8_t v7,
        int8_t v8,  int8_t v9,  int8_t v10, int8_t v11, int8_t v12, int8_t v13, int8_t v14, int8_t v15
    ) {
      return simd8<int8_t>(
          v0, v1, v2, v3, v4, v5, v6, v7,
          v8, v9, v10,v11,v12,v13,v14,v15,
          v0, v1, v2, v3, v4, v5, v6, v7,
          v8, v9, v10,v11,v12,v13,v14,v15
      );
    }

// Order-sensitive comparisons
    VK_forceinline simd8<int8_t> max_val(const simd8<int8_t> other) const { return _mm256_max_epi8(*this, other); }
    VK_forceinline simd8<int8_t> min_val(const simd8<int8_t> other) const { return _mm256_min_epi8(*this, other); }
    VK_forceinline simd8<bool> operator>(const simd8<int8_t> other) const { return _mm256_cmpgt_epi8(*this, other); }
    VK_forceinline simd8<bool> operator<(const simd8<int8_t> other) const { return _mm256_cmpgt_epi8(other, *this); }
  };

// Unsigned bytes
  template<>
  struct simd8<uint8_t>: base8_numeric<uint8_t> {
    VK_forceinline simd8() : base8_numeric<uint8_t>() {}
    VK_forceinline simd8(const __m256i _value) : base8_numeric<uint8_t>(_value) {}
// Splat constructor
    VK_forceinline simd8(uint8_t _value) : simd8(splat(_value)) {}
// Array constructor
    VK_forceinline simd8(const uint8_t values[32]) : simd8(load(values)) {}
// Member-by-member initialization
    VK_forceinline simd8(
        uint8_t v0,  uint8_t v1,  uint8_t v2,  uint8_t v3,  uint8_t v4,  uint8_t v5,  uint8_t v6,  uint8_t v7,
        uint8_t v8,  uint8_t v9,  uint8_t v10, uint8_t v11, uint8_t v12, uint8_t v13, uint8_t v14, uint8_t v15,
        uint8_t v16, uint8_t v17, uint8_t v18, uint8_t v19, uint8_t v20, uint8_t v21, uint8_t v22, uint8_t v23,
        uint8_t v24, uint8_t v25, uint8_t v26, uint8_t v27, uint8_t v28, uint8_t v29, uint8_t v30, uint8_t v31
    ) : simd8(_mm256_setr_epi8(
        v0, v1, v2, v3, v4, v5, v6, v7,
        v8, v9, v10,v11,v12,v13,v14,v15,
        v16,v17,v18,v19,v20,v21,v22,v23,
        v24,v25,v26,v27,v28,v29,v30,v31
    )) {}
// Repeat 16 values as many times as necessary (usually for lookup tables)
    VK_forceinline static simd8<uint8_t> repeat_16(
        uint8_t v0,  uint8_t v1,  uint8_t v2,  uint8_t v3,  uint8_t v4,  uint8_t v5,  uint8_t v6,  uint8_t v7,
        uint8_t v8,  uint8_t v9,  uint8_t v10, uint8_t v11, uint8_t v12, uint8_t v13, uint8_t v14, uint8_t v15
    ) {
      return simd8<uint8_t>(
          v0, v1, v2, v3, v4, v5, v6, v7,
          v8, v9, v10,v11,v12,v13,v14,v15,
          v0, v1, v2, v3, v4, v5, v6, v7,
          v8, v9, v10,v11,v12,v13,v14,v15
      );
    }

// Saturated math
    VK_forceinline simd8<uint8_t> saturating_add(const simd8<uint8_t> other) const { return _mm256_adds_epu8(*this, other); }
    VK_forceinline simd8<uint8_t> saturating_sub(const simd8<uint8_t> other) const { return _mm256_subs_epu8(*this, other); }

// Order-specific operations
    VK_forceinline simd8<uint8_t> max_val(const simd8<uint8_t> other) const { return _mm256_max_epu8(*this, other); }
    VK_forceinline simd8<uint8_t> min_val(const simd8<uint8_t> other) const { return _mm256_min_epu8(other, *this); }
// Same as >, but only guarantees true is nonzero (< guarantees true = -1)
    VK_forceinline simd8<uint8_t> gt_bits(const simd8<uint8_t> other) const { return this->saturating_sub(other); }
// Same as <, but only guarantees true is nonzero (< guarantees true = -1)
    VK_forceinline simd8<uint8_t> lt_bits(const simd8<uint8_t> other) const { return other.saturating_sub(*this); }
    VK_forceinline simd8<bool> operator<=(const simd8<uint8_t> other) const { return other.max_val(*this) == other; }
    VK_forceinline simd8<bool> operator>=(const simd8<uint8_t> other) const { return other.min_val(*this) == other; }
    VK_forceinline simd8<bool> operator>(const simd8<uint8_t> other) const { return this->gt_bits(other).any_bits_set(); }
    VK_forceinline simd8<bool> operator<(const simd8<uint8_t> other) const { return this->lt_bits(other).any_bits_set(); }

// Bit-specific operations
    VK_forceinline simd8<bool> bits_not_set() const { return *this == uint8_t(0); }
    VK_forceinline simd8<bool> bits_not_set(simd8<uint8_t> bits) const { return (*this & bits).bits_not_set(); }
    VK_forceinline simd8<bool> any_bits_set() const { return ~this->bits_not_set(); }
    VK_forceinline simd8<bool> any_bits_set(simd8<uint8_t> bits) const { return ~this->bits_not_set(bits); }
    VK_forceinline bool is_ascii() const { return _mm256_movemask_epi8(*this) == 0; }
    VK_forceinline bool bits_not_set_anywhere() const { return _mm256_testz_si256(*this, *this); }
    VK_forceinline bool any_bits_set_anywhere() const { return !bits_not_set_anywhere(); }
    VK_forceinline bool bits_not_set_anywhere(simd8<uint8_t> bits) const { return _mm256_testz_si256(*this, bits); }
    VK_forceinline bool any_bits_set_anywhere(simd8<uint8_t> bits) const { return !bits_not_set_anywhere(bits); }
    template<int N>
    VK_forceinline simd8<uint8_t> shr() const { return simd8<uint8_t>(_mm256_srli_epi16(*this, N)) & uint8_t(0xFFu >> N); }
    template<int N>
    VK_forceinline simd8<uint8_t> shl() const { return simd8<uint8_t>(_mm256_slli_epi16(*this, N)) & uint8_t(0xFFu << N); }
// Get one of the bits and make a bitmask out of it.
// e.g. value.get_bit<7>() gets the high bit
    template<int N>
    VK_forceinline int get_bit() const { return _mm256_movemask_epi8(_mm256_slli_epi16(*this, 7-N)); }
  };

  template<typename T>
  struct simd8x64 {
    static constexpr int NUM_CHUNKS = 64 / sizeof(simd8<T>);
    static_assert(NUM_CHUNKS == 2, "Haswell kernel should use two registers per 64-byte block.");
    const simd8<T> chunks[NUM_CHUNKS];

    simd8x64(const simd8x64<T>& o) = delete; // no copy allowed
    simd8x64<T>& operator=(const simd8<T>& other) = delete; // no assignment allowed
    simd8x64() = delete; // no default constructor allowed

    VK_forceinline simd8x64(const simd8<T> chunk0, const simd8<T> chunk1) : chunks{chunk0, chunk1} {}
    VK_forceinline simd8x64(const T ptr[64]) : chunks{simd8<T>::load(ptr), simd8<T>::load(ptr+32)} {}

    VK_forceinline void compress(uint64_t mask, T * output) const {
      uint32_t mask1 = uint32_t(mask);
      uint32_t mask2 = uint32_t(mask >> 32);
      this->chunks[0].compress(mask1, output);
      this->chunks[1].compress(mask2, output + 32 - count_ones(mask1));
    }

    VK_forceinline void store(T ptr[64]) const {
      this->chunks[0].store(ptr+sizeof(simd8<T>)*0);
      this->chunks[1].store(ptr+sizeof(simd8<T>)*1);
    }

    VK_forceinline uint64_t to_bitmask() const {
      uint64_t r_lo = uint32_t(this->chunks[0].to_bitmask());
      uint64_t r_hi =                       this->chunks[1].to_bitmask();
      return r_lo | (r_hi << 32);
    }

    VK_forceinline simd8<T> reduce_or() const {
      return this->chunks[0] | this->chunks[1];
    }

    VK_forceinline simd8x64<T> bit_or(const T m) const {
      const simd8<T> mask = simd8<T>::splat(m);
      return simd8x64<T>(
          this->chunks[0] | mask,
              this->chunks[1] | mask
      );
    }

    VK_forceinline uint64_t eq(const T m) const {
      const simd8<T> mask = simd8<T>::splat(m);
      return  simd8x64<bool>(
          this->chunks[0] == mask,
              this->chunks[1] == mask
      ).to_bitmask();
    }

    VK_forceinline uint64_t eq(const simd8x64<uint8_t> &other) const {
      return  simd8x64<bool>(
          this->chunks[0] == other.chunks[0],
              this->chunks[1] == other.chunks[1]
      ).to_bitmask();
    }

    VK_forceinline uint64_t lteq(const T m) const {
      const simd8<T> mask = simd8<T>::splat(m);
      return  simd8x64<bool>(
          this->chunks[0] <= mask,
              this->chunks[1] <= mask
      ).to_bitmask();
    }
  }; // struct simd8x64<T>



#elif defined(__aarch64__) || defined(_M_ARM64)




#elif defined(__PPC64__) || defined(_M_PPC64)




#endif







}

#endif  //VALKYRIE_ADT_SIMD_HPP
