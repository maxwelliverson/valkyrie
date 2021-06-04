//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_STRING_VIEW_HPP
#define VALKYRIE_STRING_VIEW_HPP

//#include <valkyrie/traits.hpp>
#include <valkyrie/utility/hash.hpp>

#include <iterator>
#include <string>
#include <string_view>
/*#include <new>*/

#include <intrin.h>

namespace valkyrie{

  //class String;
  template <size_t N>
  class static_string;
  class string_view;

  template <size_t N>
  class raw_string;

  class lazy_string;


  namespace detail{

    inline constexpr u32 offsetToNextCharacter(utf8 Char) noexcept{
      if (!(Char & 0x80u)) VK_likely return 1;
      if (!(Char & 0x20u)) return 2;
      if (!(Char & 0x10u)) return 3;
      return 4;
    }
    inline constexpr size_t stringLength(const utf8* pString) noexcept{
      //return __builtin_u8strlen(pString);
      return std::char_traits<utf8>::length(pString);
    }
    inline constexpr u32 stringGraphemeLength(VK_param(in) const utf8* pString) noexcept{
      u32 graphemeCount = 0;
      while (*pString) {
        pString += offsetToNextCharacter(*pString);
        ++graphemeCount;
      }
      return graphemeCount;
    }
    inline constexpr std::pair<u32, u32> stringBytesAndGraphemeCount(const utf8* pString){
      u32 byteCount = 0;
      u32 graphemeCount = 0;
      while(*pString) {
        auto nextOffset = offsetToNextCharacter(*pString);
        byteCount += nextOffset;
        ++graphemeCount;
        pString += nextOffset;
      }
      return {byteCount, graphemeCount};
    }
    inline constexpr std::strong_ordering stringCompare(const utf8* pStringA, const utf8* pStringB, size_t count) noexcept{
      auto result = std::char_traits<utf8>::compare(pStringA, pStringB, count);
      if (result < 0)
        return std::strong_ordering::less;
      if (result > 0)
        return std::strong_ordering::greater;
      return std::strong_ordering::equal;
    }
    inline constexpr void stringAssign(utf8& to, const utf8& from) noexcept {
      std::char_traits<utf8>::assign(to, from);
    }
    inline constexpr utf8* stringAssign(utf8* to, utf8 ch, const size_t count) noexcept {
      /*return std::char_traits<utf8>::assign(to, ch, count);*/
      for (size_t i = 0; i < count; ++i)
        to[i] = ch;
      return to;
    }
    /*inline constexpr utf8* stringMove(utf8* dest, const utf8* src, size_t count ) noexcept {
      return std::char_traits<utf8>::move(dest, src, count);
    }*/
    inline constexpr utf8* stringCopy(utf8* dest, const utf8* src, size_t count ) noexcept {
      //return std::char_traits<utf8>::copy(dest, src, count);
      for (size_t i = 0; i < count; ++i)
        dest[i] = src[i];
      return dest;
    }
    inline constexpr const utf8* stringFind(const utf8* pString, size_t count, const utf8& ch ) noexcept {
      return std::char_traits<utf8>::find(pString, count, ch);
    }

    template <typename S>
    struct StringInfo;
    /*template <>
    struct StringInfo<String>{
      inline constexpr static const utf8* data(const String& str) noexcept;
      inline constexpr static u32 byteLength(const String& str) noexcept;
      inline constexpr static u32 length(const String& str) noexcept;
    };*/
    template <>
    struct StringInfo<string_view>{
      inline constexpr static const utf8* data(string_view str) noexcept;
      inline constexpr static u32 byteLength(string_view str) noexcept;
      inline constexpr static u32 length(string_view str) noexcept;
    };
    template <size_t N>
    struct StringInfo<static_string<N>>{
      inline constexpr static const utf8* data(const static_string<N>& str) noexcept;
      inline constexpr static u32 byteLength(const static_string<N>& str) noexcept;
      inline constexpr static u32 length(const static_string<N>& str) noexcept;
    };
    template <size_t N>
    struct StringInfo<raw_string<N>>{
      inline constexpr static const utf8* data(const raw_string<N>& str) noexcept;
      inline constexpr static u32 byteLength(const raw_string<N>& str) noexcept;
      inline constexpr static u32 length(const raw_string<N>& str) noexcept;
    };
    template <>
    struct StringInfo<lazy_string>{
      inline constexpr static const utf8* data(const lazy_string& str) noexcept;
      inline constexpr static u32 byteLength(const lazy_string& str) noexcept;
      inline constexpr static u32 length(const lazy_string& str) noexcept;
    };

    template <>
    struct StringInfo<utf8*>{
      inline constexpr static const utf8* data(const utf8* str) noexcept{
        return str;
      }
      inline constexpr static u32 byteLength(const utf8* str) noexcept{
        return stringLength(str);
      }
      inline constexpr static u32 length(const utf8* str) noexcept{
        return stringGraphemeLength(str);
      }
    };
    template <>
    struct StringInfo<char*>{
      inline constexpr static const utf8* data(const char* str) noexcept{
        return (const utf8*)str;
      }
      inline constexpr static u32 byteLength(const char* str) noexcept{
        return std::char_traits<char>::length(str);
      }
      inline constexpr static u32 length(const char* str) noexcept{
        return std::char_traits<char>::length(str);
      }
    };
    template <size_t N>
    struct StringInfo<utf8[N]>{
      inline constexpr static const utf8* data(const utf8(&str)[N]) noexcept{
        return str;
      }
      inline constexpr static u32 byteLength(const utf8(&str)[N]) noexcept{
        return N - 1;
      }
      inline constexpr static u32 length(const utf8(&str)[N]) noexcept{
        return stringGraphemeLength(str);
      }
    };
    template <size_t N>
    struct StringInfo<char[N]>{
      inline constexpr static const utf8* data(const char(&str)[N]) noexcept{
        return (const utf8*)str;
      }
      inline constexpr static u32 byteLength(const char(&str)[N]) noexcept{
        return N - 1;
      }
      inline constexpr static u32 length(const char(&str)[N]) noexcept{
        return N - 1;
      }
    };

    template <typename T>
    struct StringInfo<const T> : StringInfo<T>{};
    template <typename T>
    struct StringInfo<const T*> : StringInfo<T*>{};

    /*class StringSentinel;
    class ConstStringIterator;
    class ConstStringSentinel;


    class StringIterator{

      friend class StringSentinel;
      friend class ConstStringIterator;
      friend class ConstStringSentinel;

      union{
        utf8* pString;
        Grapheme* pGraphemeString;
      };

    public:
      constexpr explicit StringIterator(utf8* pString) noexcept : pString(pString){}

      Grapheme& operator*() const noexcept{
        return *pGraphemeString;
      }
      StringIterator& operator++() noexcept{
        pString += pGraphemeString->bytes();
        return *this;
      }
      StringIterator operator++(int) noexcept{
        auto copy = *this;
        ++*this;
        return copy;
      }
      StringIterator& operator--() noexcept {
        do {
          --pString;
        } while((*pString & 0xC0u) == 0x40u);
        return *this;
      }
      StringIterator operator--(int) noexcept{
        auto copy = *this;
        --*this;
        return copy;
      }
    };
    class ConstStringIterator{

      friend class StringSentinel;
      friend class ConstStringSentinel;

      union{
        const utf8* pString;
        const Grapheme* pGraphemeString;
      };

    public:
      constexpr explicit ConstStringIterator(const utf8* pString) noexcept : pString(pString){}
      constexpr ConstStringIterator(StringIterator iter) noexcept : pString(iter.pString){}

      const Grapheme& operator*() const noexcept{
        return *pGraphemeString;
      }
      ConstStringIterator& operator++() noexcept {
        pString += pGraphemeString->bytes();
        return *this;
      }
      ConstStringIterator operator++(int) noexcept{
        auto copy = *this;
        ++*this;
        return copy;
      }
      ConstStringIterator& operator--() noexcept {
        do {
          --pString;
        } while((*pString & 0xC0u) == 0x40u);
        return *this;
      }
      ConstStringIterator operator--(int) noexcept{
        auto copy = *this;
        --*this;
        return copy;
      }
    };


    class StringSentinel{

      friend class ConstStringSentinel;

      utf8* pStringEnd;
    public:
      constexpr explicit StringSentinel(utf8* pStringEnd) noexcept : pStringEnd(pStringEnd){}

      friend bool operator==(StringIterator Iter, StringSentinel Sent) noexcept{
        return Iter.pString == Sent.pStringEnd;
      }
      friend bool operator==(ConstStringIterator Iter, StringSentinel Sent) noexcept{
        return Iter.pString == Sent.pStringEnd;
      }
      friend std::strong_ordering operator<=>(StringIterator Iter, StringSentinel Sent) noexcept{
        return Iter.pString <=> Sent.pStringEnd;
      }
      friend std::strong_ordering operator<=>(ConstStringIterator Iter, StringSentinel Sent) noexcept{
        return Iter.pString <=> Sent.pStringEnd;
      }
    };
    class ConstStringSentinel{
      const utf8* pStringEnd;
    public:
      constexpr explicit ConstStringSentinel(const utf8* pStringEnd) noexcept : pStringEnd(pStringEnd){}

      friend bool operator==(StringIterator Iter, ConstStringSentinel Sent) noexcept{
        return Iter.pString == Sent.pStringEnd;
      }
      friend bool operator==(ConstStringIterator Iter, ConstStringSentinel Sent) noexcept{
        return Iter.pString == Sent.pStringEnd;
      }
      friend std::strong_ordering operator<=>(StringIterator Iter, ConstStringSentinel Sent) noexcept{
        return Iter.pString <=> Sent.pStringEnd;
      }
      friend std::strong_ordering operator<=>(ConstStringIterator Iter, ConstStringSentinel Sent) noexcept{
        return Iter.pString <=> Sent.pStringEnd;
      }
    };*/

    using StringIterator = utf8*;
    using ConstStringIterator = const utf8*;
    using StringSentinel = utf8*;
    using ConstStringSentinel = const utf8*;



#if defined(_AMD64_) || defined(_M_X64)

    class string_bitmap{
    public:

      string_bitmap() noexcept : bits(_mm256_setzero_si256()){}


    private:
      __m256i bits;
    };

    template <size_t Step>
    class string_chunk;



    template <>
    class string_chunk<16>{
    public:

      VK_constant u64 chunk_size = 16;
      using chunk_type = __m128i;

      string_chunk() = default;
      string_chunk(const chunk_type& c, int len) noexcept
          : chunk(c), length_(len){}

      void fill(utf8_string str) noexcept {
        chunk = _mm_loadu_si128((const __m128i* __unaligned)str);
        length_ = chunk_size;
      }
      void fill(utf8_string str, u64 len) noexcept {
        VK_assert(len <= chunk_size);
        __m128i tmp_simd = _mm_setzero_si128();
        std::memcpy(&tmp_simd, str, len);
        chunk = _mm_load_si128(&tmp_simd);
        length_ = len;
      }

      const chunk_type& reg()    const noexcept {
        return chunk;
      }
      int               length() const noexcept {
        return length_;
      }

      VK_nodiscard u32 find(const string_chunk<16>& other) const noexcept {

      }

      VK_nodiscard u32 find_first_of(const string_chunk<16>& other) const noexcept {
        return static_cast<u32>(_mm_cmpestri(other.reg(), other.length(), reg(), length(), 0));
      }
      VK_nodiscard u32 find_first_not_of(const string_chunk<16>& other) const noexcept {
        return static_cast<u32>(_mm_cmpestri(other.reg(), other.length(), reg(), length(), _SIDD_MASKED_NEGATIVE_POLARITY));
      }
      VK_nodiscard u32 find_last_of(const string_chunk<16>& other) const noexcept {
        return static_cast<u32>(_mm_cmpestri(other.reg(), other.length(), reg(), length(), _SIDD_MOST_SIGNIFICANT | _SIDD_MASKED_NEGATIVE_POLARITY));
      }
      VK_nodiscard u32 find_last_not_of(const string_chunk<16>& other) const noexcept {
        return static_cast<u32>(_mm_cmpestri(other.reg(), other.length(), reg(), length(), _SIDD_MOST_SIGNIFICANT));
      }

      VK_nodiscard u32 mask_each_from(const string_chunk<16>& other) const noexcept {
        chunk_type result = _mm_cmpestrm(other.reg(), other.length(), reg(), length(), 0);
        return _mm_extract_epi16(result, 0);
      }
      VK_nodiscard u32 mask_each_not_from(const string_chunk<16>& other) const noexcept {
        chunk_type result = _mm_cmpestrm(other.reg(), other.length(), reg(), length(), _SIDD_MASKED_NEGATIVE_POLARITY);
        return _mm_extract_epi16(result, 0);
      }

    private:
      chunk_type chunk;
      int        length_;
    };
    template <>
    class string_chunk<32>{
    public:

      VK_constant u64 chunk_size = 32;
      using chunk_type = __m256i;

      string_chunk() = default;
      string_chunk(const string_chunk<16>& a, const string_chunk<16>& b) noexcept
          : chunk(_mm256_set_m128i(a.reg(), b.reg())), length_(a.length() + b.length()){
        VK_assert(a.length() == 16);
      }

      void fill(utf8_string str) noexcept {
        chunk = _mm256_loadu_si256((const __m256i* __unaligned)str);
        length_ = chunk_size;
      }
      void fill(utf8_string str, u64 len) noexcept {
        VK_assert(len <= chunk_size);
        __m256i tmp_simd = _mm256_setzero_si256();
        std::memcpy(&tmp_simd, str, len);
        chunk = _mm256_load_si256(&tmp_simd);
        length_ = len;
      }

      string_chunk<16> lo() const noexcept {
        return { _mm256_extractf128_si256(chunk, 0), std::min(length_, 16) };
      }
      string_chunk<16> hi() const noexcept {
        return { _mm256_extractf128_si256(chunk, 0), std::max(length_ - 16, 0) };
      }

      const chunk_type& reg() const noexcept {
        return chunk;
      }
      int               length() const noexcept {
        return length_;
      }

    private:
      chunk_type chunk;
      int     length_;
    };

    template <size_t Step>
    class string_chunk_reverse_iterator;

    class string_chunk_sentinel{};
    template <size_t Step>
    class string_chunk_iterator{

      void update() const noexcept {
        if ( needs_update ) {
          utf8_string string = end - remaining;
          if ( remaining >= Step )
            chunk.fill(string);
          else
            chunk.fill(string, remaining);
          needs_update = false;
        }
      }

    public:

      VK_constant u64 chunk_size = Step;
      using chunk_type = string_chunk<chunk_size>;

      string_chunk_iterator() = default;
      template <size_t N>
      string_chunk_iterator(const string_chunk_iterator<N>& other) noexcept
          : end(other.end), remaining(other.remaining), chunk(){ }
      string_chunk_iterator(utf8_string string, u64 size) noexcept
          : chunk(), end(string + size), remaining(static_cast<i32>(size)){ }
      template <size_t N>
      explicit string_chunk_iterator(const string_chunk_reverse_iterator<N>& iter) noexcept;

      string_chunk_iterator& operator++() noexcept {
        remaining -= Step;
        needs_update = true;
        return *this;
      }
      string_chunk_iterator  operator++(int) noexcept {
        string_chunk_iterator tmp(*this);
        remaining -= Step;
        needs_update = true;
        return tmp;
      }

      chunk_type& operator*() const noexcept {
        update();
        return chunk;
      }


      void add_offset(u64 offset) noexcept {
        remaining -= offset;
        needs_update = true;
      }


      friend bool operator==(const string_chunk_iterator& iter, string_chunk_sentinel) noexcept {
        return iter.remaining <= 0;
      }

    private:

      template <size_t>
      friend class string_chunk_reverse_iterator;
      template <size_t>
      friend class string_chunk_iterator;


      VK_constant u64 StepHiMask = ~(Step - 1);

      utf8_string        end;
      i32                remaining = 0;
      mutable bool       needs_update = true;
      mutable chunk_type chunk;
    };
    class string_chunk_reverse_sentinel{};
    template <size_t Step>
    class string_chunk_reverse_iterator{

      void update() const noexcept {
        if ( needs_update ) {
          if ( remaining >= Step )
            chunk.fill(begin + remaining - Step);
          else
            chunk.fill(begin, remaining);
          needs_update = false;
        }
      }

    public:

      VK_constant u64 chunk_size = Step;
      using chunk_type = string_chunk<chunk_size>;

      string_chunk_reverse_iterator() = default;
      template <size_t N>
      string_chunk_reverse_iterator(const string_chunk_reverse_iterator<N>& other) noexcept
          : begin(other.begin), remaining(other.remaining), chunk(){ }
      string_chunk_reverse_iterator(utf8_string string, u64 size) noexcept
          : begin(string), remaining(static_cast<i32>(size)), chunk(){ }

      template <size_t N>
      explicit string_chunk_reverse_iterator(const string_chunk_iterator<N>& iter) noexcept
          : begin(iter.end - iter.remaining), remaining(iter.remaining), chunk(){ }

      string_chunk_reverse_iterator& operator++() noexcept {
        remaining -= Step;
        needs_update = true;
        return *this;
      }
      string_chunk_reverse_iterator  operator++(int) noexcept {
        string_chunk_reverse_iterator tmp(*this);
        remaining -= Step;
        needs_update = true;
        return tmp;
      }

      chunk_type& operator*() const noexcept {
        update();
        return chunk;
      }

      void add_offset(u64 offset) noexcept {
        remaining -= offset;
        needs_update = true;
      }

      friend bool operator==(const string_chunk_reverse_iterator& iter, string_chunk_reverse_sentinel) noexcept {
        return iter.remaining <= 0;
      }

    private:
      template <size_t>
      friend class string_chunk_iterator;
      template <size_t>
      friend class string_chunk_reverse_iterator;

      VK_constant u64 StepHiMask = ~(Step - 1);

      utf8_string        begin;
      i32                remaining = 0;
      mutable bool       needs_update = true;
      mutable chunk_type chunk;
    };

    template <size_t N>
    template <size_t M>
    string_chunk_iterator<N>::string_chunk_iterator(const string_chunk_reverse_iterator<M>& iter) noexcept
        : end(iter.begin + iter.remaining), remaining(iter.remaining), chunk(){}

    template <size_t Step>
    class string_chunk_range;
    template <size_t Step>
    class string_chunk_reverse_range;

    template <size_t Step>
    class string_chunk_range{
    public:

      VK_constant u64 chunk_size = Step;

      using value_type = string_chunk<chunk_size>;

      using iterator = string_chunk_iterator<Step>;
      using sentinel = string_chunk_sentinel;
      using reverse_iterator = string_chunk_reverse_iterator<Step>;
      using reverse_sentinel = string_chunk_reverse_sentinel;

      string_chunk_range() = default;
      string_chunk_range(utf8_string str, u64 length) noexcept
          : string(str), length(length){}

      VK_nodiscard iterator begin() const noexcept {
        return iterator(string, length);
      }
      VK_nodiscard sentinel end() const noexcept {
        return sentinel();
      }

      VK_nodiscard reverse_iterator rbegin() const noexcept {
        return reverse_iterator(string, length);
      }
      VK_nodiscard reverse_sentinel rend() const noexcept {
        return reverse_sentinel();
      }

      VK_nodiscard string_chunk_reverse_range<Step> reverse() const noexcept;

      VK_nodiscard u64 size() const noexcept {
        constexpr static u64 offset = chunk_size - 1;
        constexpr static u64 mask   = ~offset;
        return ((length + offset) & mask) / chunk_size;
      }

    private:
      utf8_string string;
      u64         length = 0;
    };

    template <size_t Step>
    class string_chunk_reverse_range{
    public:

      VK_constant u64 chunk_size = Step;

      using value_type = string_chunk<chunk_size>;

      using iterator = string_chunk_reverse_iterator<Step>;
      using sentinel = string_chunk_reverse_sentinel;
      using reverse_iterator = string_chunk_iterator<Step>;
      using reverse_sentinel = string_chunk_sentinel;

      string_chunk_reverse_range() = default;
      string_chunk_reverse_range(utf8_string str, u64 length) noexcept
          : string(str), length(length){}

      VK_nodiscard iterator begin() const noexcept {
        return iterator(string, length);
      }
      VK_nodiscard sentinel end() const noexcept {
        return sentinel();
      }

      VK_nodiscard reverse_iterator rbegin() const noexcept {
        return reverse_iterator(string, length);
      }
      VK_nodiscard reverse_sentinel rend() const noexcept {
        return reverse_sentinel();
      }

      VK_nodiscard string_chunk_range<chunk_size> reverse() const noexcept {
        return { string, length };
      }

      VK_nodiscard u64 size() const noexcept {
        constexpr static u64 offset = chunk_size - 1;
        constexpr static u64 mask   = ~offset;
        return ((length + offset) & mask) / chunk_size;
      }

    private:
      utf8_string string;
      u64         length = 0;
    };

    template <size_t Step>
    string_chunk_reverse_range<Step> string_chunk_range<Step>::reverse() const noexcept {
      return { string, length };
    }


    namespace string_ops {
      enum class type : u8{
        u8  = _SIDD_UBYTE_OPS,
        u16 = _SIDD_UWORD_OPS,
        i8  = _SIDD_SBYTE_OPS,
        i16 = _SIDD_SWORD_OPS
      };
      enum class cmp  : u8{
        equal_any     = _SIDD_CMP_EQUAL_ANY,
        ranges        = _SIDD_CMP_RANGES,
        equal_each    = _SIDD_CMP_EQUAL_EACH,
        equal_ordered = _SIDD_CMP_EQUAL_ORDERED
      };
      enum class parity : u8{
        positive = _SIDD_POSITIVE_POLARITY,
        masked_positive = _SIDD_MASKED_POSITIVE_POLARITY,
        negative = _SIDD_NEGATIVE_POLARITY,
        masked_negative = _SIDD_MASKED_NEGATIVE_POLARITY
      };
      enum class index : u8 {
        lsb = _SIDD_LEAST_SIGNIFICANT,
        msb = _SIDD_MOST_SIGNIFICANT
      };
      enum class mask : u8 {
        bit  = _SIDD_BIT_MASK,
        unit = _SIDD_UNIT_MASK
      };

      template <typename ...Opts>
      VK_forceinline static u32 cmpistri(const string_chunk<16>& a, const string_chunk<16>& b, Opts ...opts) noexcept {
        const int options_mask = (0 & ... & (static_cast<int>(opts)));
        return static_cast<u32>(_mm_cmpistri(a.reg(), b.reg(), options_mask));
      }
      template <typename ...Opts>
      VK_forceinline static u32 cmpestri(const string_chunk<16>& a, const string_chunk<16>& b, Opts ...opts) noexcept {
        const int options_mask = (0 & ... & (static_cast<int>(opts)));
        return static_cast<u32>(_mm_cmpestri(a.reg(), a.length(), b.reg(), b.length(), options_mask));
      }
      template <typename ...Opts>
      VK_forceinline static u64 cmpistri(const string_chunk<32>& a, const string_chunk<32>& b, Opts ...opts) noexcept {
        const int options_mask = (0 & ... &  (static_cast<int>(opts)));
        const u64 lo_mask = static_cast<u32>(_mm_cmpistri(a.lo().reg(), b.lo().reg(), options_mask));
        const u64 hi_mask = static_cast<u32>(_mm_cmpistri(a.hi().reg(), b.hi().reg(), options_mask));
        return lo_mask | (hi_mask << 32);
      }
      template <typename ...Opts>
      VK_forceinline static u64 cmpestri(const string_chunk<32>& a, const string_chunk<32>& b, Opts ...opts) noexcept {
        const int options_mask = (0 & ... &  (static_cast<int>(opts)));
        const auto a_lo = a.lo();
        const auto a_hi = a.hi();
        const auto b_lo = b.lo();
        const auto b_hi = b.hi();
        const u64 lo_mask = static_cast<u32>(_mm_cmpestri(a_lo.reg(), a_lo.length(), b_lo.reg(), b_lo.length(), options_mask));
        const u64 hi_mask = static_cast<u32>(_mm_cmpestri(a_hi.reg(), a_hi.length(), b_hi.reg(), b_hi.length(), options_mask));
        return lo_mask | (hi_mask << 32);
      }

      VK_nodiscard static u64 find_first_of(utf8_string string, u64 length, utf8_string coll, u64 coll_len) noexcept {
        string_chunk_range<16> haystack{string, length};
        string_chunk_range<16> needles{coll, coll_len};


        u64 offset = 0;

        for ( const auto& chunk : haystack ) {
          for ( const auto& needle : needles ) {
            if (auto result = chunk.find_first_of(needle); result != 16) {
              return offset + result;
            }
          }
          offset += chunk.length();
        }

        return (u64)-1;
      }
      VK_nodiscard static u64 find_last_of(utf8_string string, u64 length, utf8_string coll, u64 coll_len) noexcept {
        string_chunk_range<16> haystack{string, length};
        string_chunk_range<16> needles{coll, coll_len};


        u64 offset = 0;

        for ( const auto& chunk : haystack.reverse() ) {
          for ( const auto& needle : needles ) {
            if (auto result = chunk.find_last_of(needle); result != 16) {
              return offset + result;
            }
          }
          offset += chunk.length();
        }

        return (u64)-1;
      }
    };


#endif
  }

  class string_view {
    union{
      const utf8* u8;
      const char* asc;
    } ptr = { .u8 = u8"" };
    u64        length_ = 0;
  public:

    using value_type = const utf8;
    using size_type = u64;
    using pointer = const utf8*;
    using cstring_type = const char*;

    //using reference = const Grapheme&;
    using reference = const utf8&;
    using iterator = detail::ConstStringIterator;
    using const_iterator = iterator;
    using sentinel = detail::ConstStringSentinel;
    using const_sentinel = sentinel;


    VK_constant size_type npos = std::numeric_limits<size_type>::max();



    constexpr string_view() noexcept = default;
    constexpr string_view(const string_view &) noexcept = default;
    constexpr string_view(string_view &&) noexcept = default;

    constexpr string_view & operator=(const string_view &) noexcept = default;
    constexpr string_view & operator=(string_view &&) noexcept = default;

    /*constexpr */~string_view() noexcept = default;

    template <std::ranges::contiguous_range Rng> requires(same_as_one_of<std::ranges::range_value_t<Rng>, utf8, char>)
    constexpr string_view(Rng& rng) noexcept
        : string_view(std::ranges::data(rng), std::ranges::size(rng)) { }

    template <size_t N>
    constexpr string_view(const utf8(&str)[N]) noexcept
        : string_view(static_cast<utf8_string>(str), N - 1){}

    template <size_t N>
    constexpr string_view(const char(&str)[N]) noexcept
        : string_view(static_cast<cstring>(str), N - 1){}

    constexpr string_view(utf8_string c_str) noexcept
        : string_view(c_str, std::char_traits<utf8>::length(c_str)){}
    constexpr string_view(cstring c_str) noexcept
        : string_view(c_str, std::char_traits<char>::length(c_str)){}

    constexpr string_view(utf8_string data, u64 stringLength) noexcept
        : ptr{ .u8 = data },
          length_(stringLength){}
    constexpr string_view(cstring data, u64 stringLength) noexcept
        : ptr{ .asc = data },
          length_(stringLength){}




    // [ string_view.capacity ]

    VK_nodiscard constexpr size_type size() const noexcept { return length_; }
    VK_nodiscard constexpr size_type length() const noexcept { return length_; }
    VK_nodiscard constexpr size_type max_size() const noexcept { return std::numeric_limits<i64>::max(); }
    VK_nodiscard constexpr bool      empty() const noexcept { return !length_; }

    // [ string_view.accessors ]

    VK_nodiscard constexpr pointer data() const noexcept { return ptr.u8; }
    VK_nodiscard constexpr cstring_type c_str() const noexcept { return ptr.asc; }
    VK_nodiscard constexpr reference front() const noexcept { return *begin(); }
    VK_nodiscard constexpr reference back() const noexcept { return *(ptr.u8 + length_ - 1); }
    VK_nodiscard constexpr reference operator[](size_type pos) const noexcept {
      VK_assert( pos < size() );
      return ptr.u8[pos];
    }


    // [ string_view.iterators ]

    VK_nodiscard constexpr iterator begin()        const noexcept { return iterator(ptr.u8); }
    VK_nodiscard constexpr const_iterator cbegin() const noexcept { return begin(); }
    VK_nodiscard constexpr sentinel end()          const noexcept { return sentinel(ptr.u8 + length_); }
    VK_nodiscard constexpr const_sentinel cend()   const noexcept { return end(); }



    // [ string_view.modifiers ]

    constexpr void remove_prefix(size_type n) noexcept {
      VK_constexpr_assert( n <= size() );
      ptr.u8 += n;
      length_ -= n;
    }
    constexpr void remove_suffix(size_type n) noexcept {
      VK_constexpr_assert( n <= size() );
      length_ -= n;
    }
    constexpr void swap(string_view& other) noexcept {
      std::swap(ptr.u8, other.ptr.u8);
      std::swap(length_, other.length_);
    }



    // [ string_view.operations ]
    
    constexpr size_type copy(utf8* dest, size_type count, size_type pos = 0) const noexcept {
      VK_constexpr_assert(pos <= size());
      const size_type min_count = std::min(count, size() - pos);
      VK_consteval_block {
        for (size_type i = 0; i < min_count; ++i)
          dest[i] = ptr.u8[pos + i];
      }
      VK_runtime_block {
        if ( min_count > 0 ) VK_likely
              std::memcpy(dest, data() + pos, min_count);
      }
      return min_count;
    }
    VK_nodiscard constexpr string_view substr(size_type pos = 0, size_type count = npos) const noexcept {
      VK_constexpr_assert(pos <= size());
      return string_view(data() + pos, std::min(count, size() - pos));
    }
    VK_nodiscard constexpr string_view slice(size_type from, size_type to) const noexcept {
      return string_view(ptr.u8 + from, to - from);
    }

    VK_nodiscard constexpr int compare(string_view sv) const noexcept {
      return std::char_traits<utf8>::compare(data(), sv.data(), std::min(size(), sv.size()));
    }
    VK_nodiscard constexpr int compare(size_type pos, size_type count, string_view sv) const noexcept {
      return substr(pos, count).compare(sv);
    }
    VK_nodiscard constexpr int compare(size_type pos, size_type count, string_view sv, size_type pos2, size_type count2) const noexcept {
      return substr(pos, count).compare(sv.substr(pos2, count2));
    }
    VK_nodiscard constexpr int compare(utf8_string str) const noexcept {
      return compare(string_view(str));
    }
    VK_nodiscard constexpr int compare(size_type pos, size_type count, utf8_string str) const noexcept {
      return substr(pos, count).compare(string_view(str));
    }
    VK_nodiscard constexpr int compare(size_type pos, size_type count, utf8_string str, size_type pos2, size_type count2) const noexcept {
      return substr(pos, count).compare(string_view(str + pos2, count2));
    }

    VK_nodiscard constexpr bool starts_with(string_view sv) const noexcept {
      return substr(0, sv.size()) == sv;
    }
    VK_nodiscard constexpr bool starts_with(utf8 c) const noexcept {
      return !empty() && front() == c;
    }
    VK_nodiscard constexpr bool starts_with(utf8_string str) const noexcept {
      return starts_with(string_view(str));
    }

    VK_nodiscard constexpr bool ends_with(string_view sv) const noexcept {
      return size() >= sv.size() && substr(size() - sv.size()) == sv;
    }
    VK_nodiscard constexpr bool ends_with(utf8 c) const noexcept {
      return !empty() && back() == c;
    }
    VK_nodiscard constexpr bool ends_with(utf8_string str) const noexcept {
      return ends_with(string_view(str));
    }


    VK_nodiscard constexpr bool contains(string_view sv) const noexcept {
      return this->find(sv) != npos;
    }
    VK_nodiscard constexpr bool contains(utf8 c) const noexcept {
      return this->find(c) != npos;
    }
    VK_nodiscard constexpr bool contains(utf8_string str) const noexcept {
      return this->find(str) != npos;
    }


    // [ string_view.searching ]

    VK_nodiscard constexpr size_type find(string_view sv) const noexcept {
      if ( sv.empty() )
        return 0;
      if ( size() >= sv.size() ) {
        utf8_string first = std::char_traits<utf8>::find(begin(), size(), sv.front());
        while ( first ) {
          if ( (end() - first) < sv.size() )
            return npos;
          if ( string_view(first, sv.size()) == sv )
            return first - begin();
          first = std::char_traits<utf8>::find(first, end() - first, sv.front());
        }
      }
      return npos;
    }
    VK_nodiscard constexpr size_type find(string_view sv, size_type pos) const noexcept {
      return substr(pos).find(sv);
    }
    VK_nodiscard constexpr size_type find(utf8 c) const noexcept {
      if ( utf8_string pos = std::char_traits<utf8>::find(begin(), size(), c) )
        return pos - begin();
      return npos;
    }
    VK_nodiscard constexpr size_type find(utf8 c, size_type pos) const noexcept {
      return substr(pos).find(c);
    }
    VK_nodiscard constexpr size_type find(utf8_string str) const noexcept {
      return find(string_view(str));
    }
    VK_nodiscard constexpr size_type find(utf8_string str, size_type pos) const noexcept {
      return substr(pos).find(string_view(str));
    }
    VK_nodiscard constexpr size_type find(utf8_string str, size_type pos, size_type count) const noexcept {
      return substr(pos).find(string_view(str, count));
    }

    VK_nodiscard constexpr size_type rfind(string_view sv, size_type pos = npos) const noexcept {
      if ( sv.empty() )
        return 0;
      if ( size() >= sv.size() ) {
        for (const_iterator current = begin() + std::min(pos, size() - sv.size()); ; --current ) {
          if ( *current == sv.front() && string_view((utf8_string)current, sv.size()) == sv )
            return current - begin();
          if ( current == begin() )
            break;
        }
      }
      return npos;
    }
    VK_nodiscard constexpr size_type rfind(utf8 c, size_type pos = npos) const noexcept {
      if ( !empty() ) {
        for (const_iterator current = begin() + std::min(pos, size() - 1); ; --current ) {
          if ( *current == c )
            return current - begin();
          if ( current == begin() )
            break;
        }
      }
      return npos;
    }
    VK_nodiscard constexpr size_type rfind(utf8_string str, size_type pos = npos) const noexcept {
      return rfind(string_view(str), pos);
    }
    VK_nodiscard constexpr size_type rfind(utf8_string str, size_type pos, size_type count) const noexcept {
      return rfind(string_view(str, count), pos);
    }


    VK_nodiscard constexpr size_type find_first_of(string_view sv, size_type pos = 0) const noexcept {
      for ( size_type i = pos; i < size(); ++i ) {
        if ( sv.contains((*this)[i]) )
          return i;
      }
      return npos;
    }
    VK_nodiscard constexpr size_type find_first_of(utf8 c, size_type pos = 0) const noexcept {
      for ( size_type i = pos; i < size(); ++i ) {
        if ( (*this)[i] == c )
          return i;
      }
      return npos;
    }
    VK_nodiscard constexpr size_type find_first_of(utf8_string str, size_type pos = 0) const noexcept {
      return find_first_of(string_view(str), pos);
    }
    VK_nodiscard constexpr size_type find_first_of(utf8_string str, size_type pos, size_type count) const noexcept {
      return find_first_of(string_view(str, count), pos);
    }

    VK_nodiscard constexpr size_type find_last_of(string_view sv, size_type pos = npos) const noexcept {
      for ( size_type i = std::min(size() - 1, pos); i > 0; --i ) {
        if ( sv.contains((*this)[i]) )
          return i;
      }
      return npos;
    }
    VK_nodiscard constexpr size_type find_last_of(utf8 c, size_type pos = npos) const noexcept {
      for ( size_type i = std::min(size() - 1, pos); i > 0; --i ) {
        if ( (*this)[i] == c )
          return i;
      }
      return npos;
    }
    VK_nodiscard constexpr size_type find_last_of(utf8_string str, size_type pos = npos) const noexcept {
      return find_last_of(string_view(str), pos);
    }
    VK_nodiscard constexpr size_type find_last_of(utf8_string str, size_type pos, size_type count) const noexcept {
      return find_last_of(string_view(str, count), pos);
    }

    VK_nodiscard constexpr size_type find_first_not_of(string_view sv, size_type pos = 0) const noexcept {
      for ( size_type i = pos; i < size(); ++i ) {
        if ( !sv.contains((*this)[i]) )
          return i;
      }
      return npos;
    }
    VK_nodiscard constexpr size_type find_first_not_of(utf8 c, size_type pos = 0) const noexcept {
      for ( size_type i = pos; i < size(); ++i ) {
        if ( (*this)[i] != c )
          return i;
      }
      return npos;
    }
    VK_nodiscard constexpr size_type find_first_not_of(utf8_string str, size_type pos = 0) const noexcept {
      return find_first_not_of(string_view(str), pos);
    }
    VK_nodiscard constexpr size_type find_first_not_of(utf8_string str, size_type pos, size_type count) const noexcept {
      return find_first_not_of(string_view(str, count), pos);
    }

    VK_nodiscard constexpr size_type find_last_not_of(string_view sv, size_type pos = npos) const noexcept {
      for ( size_type i = std::min(size() - 1, pos); i > 0; --i ) {
        if ( !sv.contains((*this)[i]) )
          return i;
      }
      return npos;
    }
    VK_nodiscard constexpr size_type find_last_not_of(utf8 c, size_type pos = npos) const noexcept {
      for ( size_type i = std::min(size() - 1, pos); i > 0; --i ) {
        if ( (*this)[i] != c )
          return i;
      }
      return npos;
    }
    VK_nodiscard constexpr size_type find_last_not_of(utf8_string str, size_type pos = npos) const noexcept {
      return find_last_not_of(string_view(str), pos);
    }
    VK_nodiscard constexpr size_type find_last_not_of(utf8_string str, size_type pos, size_type count) const noexcept {
      return find_last_not_of(string_view(str, count), pos);
    }


    VK_nodiscard constexpr friend bool operator==(string_view A, string_view B) noexcept {
      return A.length_ == B.length_ &&
             detail::stringCompare(A.ptr.u8, B.ptr.u8, A.length_) == std::strong_ordering::equal;
    }
    VK_nodiscard constexpr friend std::strong_ordering operator<=>(string_view A, string_view B) noexcept {
      auto compResult = detail::stringCompare(A.ptr.u8, B.ptr.u8, std::min(A.length_, B.length_));
      if (compResult != std::strong_ordering::equal)
        return compResult;
      return A.length_ <=> B.length_;
    }
  };
  class mutable_string_view {

    inline static utf8 EmptyString = u8'\0';

    union{
      utf8* u8;
      char* asc;
    } ptr = { .u8 = &EmptyString };
    u64        length_ = 0;
  public:

    using value_type = utf8;
    using size_type = u64;
    using pointer = utf8*;
    using cstring_type = char*;

    //using reference = const Grapheme&;
    using reference = utf8&;
    using iterator = detail::StringIterator;
    using const_iterator = detail::ConstStringIterator;
    using sentinel = detail::ConstStringSentinel;
    using const_sentinel = detail::ConstStringSentinel;

    constexpr mutable_string_view() noexcept = default;
    constexpr mutable_string_view(const mutable_string_view &) noexcept = default;
    constexpr mutable_string_view(mutable_string_view &&) noexcept = default;

    constexpr mutable_string_view & operator=(const mutable_string_view &) noexcept = default;
    constexpr mutable_string_view & operator=(mutable_string_view &&) noexcept = default;

    /*constexpr */~mutable_string_view() noexcept = default;

    template <contiguous_range_c Rng> requires(exact_same_as_one_of<std::ranges::range_value_t<Rng>, utf8, char>)
    constexpr mutable_string_view(Rng& rng) noexcept
        : mutable_string_view(std::ranges::data(rng), std::ranges::size(rng)) { }

    template <size_t N>
    constexpr mutable_string_view(utf8(&str)[N]) noexcept
        : mutable_string_view(static_cast<utf8*>(str), N - 1){}

    template <size_t N>
    constexpr mutable_string_view(char(&str)[N]) noexcept
        : mutable_string_view(static_cast<char*>(str), N - 1){}

    constexpr mutable_string_view(utf8* c_str) noexcept
        : mutable_string_view(c_str, std::char_traits<utf8>::length(c_str)){}
    constexpr mutable_string_view(char* c_str) noexcept
        : mutable_string_view(c_str, std::char_traits<char>::length(c_str)){}

    constexpr mutable_string_view(utf8* data, u64 stringLength) noexcept
        : ptr{ .u8 = data },
          length_(stringLength){}
    constexpr mutable_string_view(char* data, u64 stringLength) noexcept
        : ptr{ .asc = data },
          length_(stringLength){}


    VK_nodiscard constexpr cstring_type c_str() const noexcept { return ptr.asc; }

    VK_nodiscard constexpr pointer data() const noexcept { return ptr.u8; }

    VK_nodiscard constexpr u64 size() const noexcept { return length_; }
    VK_nodiscard constexpr u64 length() const noexcept { return length_; }

    VK_nodiscard reference front() const noexcept { return *begin(); }
    VK_nodiscard reference back() const noexcept { return *(ptr.u8 + length_ - 1); }

    VK_nodiscard iterator begin()        const noexcept { return iterator(ptr.u8); }
    VK_nodiscard const_iterator cbegin() const noexcept { return begin(); }
    VK_nodiscard sentinel end()          const noexcept { return sentinel(ptr.u8 + length_); }
    VK_nodiscard const_sentinel cend()   const noexcept { return end(); }

    VK_nodiscard friend bool operator==(mutable_string_view A, mutable_string_view B) noexcept {
      return A.length_ == B.length_ &&
             detail::stringCompare(A.ptr.u8, B.ptr.u8, A.length_) == std::strong_ordering::equal;
    }
    VK_nodiscard friend std::strong_ordering operator<=>(mutable_string_view A, mutable_string_view B) noexcept {
      auto compResult = detail::stringCompare(A.ptr.u8, B.ptr.u8, std::min(A.length_, B.length_));
      if (compResult != std::strong_ordering::equal)
        return compResult;
      return A.length_ <=> B.length_;
    }
  };

  class string_ref {
  public:
    using value_type = const utf8;
    using size_type = u32;
    using pointer = const utf8 *;
    using const_pointer = const utf8 *;
    using iterator = const utf8 *;
    using const_iterator = const utf8 *;

    using cstring_type = const char*;

  protected:

    enum class _thunk_op {
      copy,
      move,
      destruct
    };

    using _thunk_spec = void (*)(string_ref *dest, const string_ref *src, _thunk_op op);
#ifndef NDEBUG
  private:
    static void _checking_string_thunk(string_ref *dest, const string_ref *src, _thunk_op /*unused*/) noexcept
    {
      (void) dest;
      (void) src;
      VK_assert(dest->_thunk == _checking_string_thunk);                   // NOLINT
      VK_assert(src == nullptr || src->_thunk == _checking_string_thunk);  // NOLINT
      // do nothing
    }

  protected:
#endif
    //! Pointers to beginning and end of character range
    pointer _begin{}, _end{};
    //! Three `void*` of state
    void *_state[3]{};  // at least the size of a shared_ptr
    //! Handler for when operations occur
    const _thunk_spec _thunk{nullptr};

    constexpr explicit string_ref(_thunk_spec thunk) noexcept : _thunk(thunk) {}

  public:
    template <size_t N>
    string_ref(const char(&cString)[N]) noexcept :
        string_ref((const utf8*)cString, N - 1){}
    template <size_t N>
    string_ref(const utf8(&cString)[N]) noexcept :
        string_ref(cString, N - 1){}

    string_ref(string_view sv) noexcept : string_ref(sv.data(), sv.length()){}

    explicit string_ref(const utf8 *str, size_type len = static_cast<size_type>(-1), void *state0 = nullptr, void *state1 = nullptr, void *state2 = nullptr,
#ifndef NDEBUG
                       _thunk_spec thunk = _checking_string_thunk
#else
        _thunk_spec thunk = nullptr
#endif
    ) noexcept :
        _begin(str),
        _end(str + (len == static_cast<size_type>(-1) ? detail::stringLength(str) : len)),  // NOLINT
        _state{state0, state1, state2},
        _thunk(thunk) { }

    //! Construct from a C string literal
    explicit string_ref(const char *str, size_type len = static_cast<size_type>(-1), void *state0 = nullptr, void *state1 = nullptr, void *state2 = nullptr,
#ifndef NDEBUG
                                 _thunk_spec thunk = _checking_string_thunk
#else
        _thunk_spec thunk = nullptr
#endif
    ) noexcept : string_ref((const utf8*)str, len, state0, state1, state2, thunk){ }
    //! Copy construct the derived implementation.
    string_ref(const string_ref &o) :
        _begin(o._begin),
        _end(o._end),
        _state{o._state[0], o._state[1], o._state[2]},
        _thunk(o._thunk) {
      if (_thunk != nullptr) {
        _thunk(this, &o, _thunk_op::copy);
      }
    }
    //! Move construct the derived implementation.
    string_ref(string_ref &&o) noexcept :
        _begin(o._begin),
        _end(o._end),
        _state{o._state[0], o._state[1], o._state[2]},
        _thunk(o._thunk) {
      if(_thunk != nullptr)
      {
        _thunk(this, &o, _thunk_op::move);
      }
    }
    //! Copy assignment
    string_ref &operator=(const string_ref &o)
    {
      if(this != &o)
      {
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)
        string_ref temp(static_cast<string_ref &&>(*this));
        this->~string_ref();
        try
        {
          new(this) string_ref(o);  // may throw
        }
        catch(...)
        {
          new(this) string_ref(static_cast<string_ref &&>(temp));
          throw;
        }
#else
        this->~string_ref();
        new(this) string_ref(o);
#endif
      }
      return *this;
    }
    //! Move assignment
    string_ref &operator=(string_ref &&o) noexcept {
      if (this != &o) {
        this->~string_ref();
        new(this) string_ref(static_cast<string_ref &&>(o));
      }
      return *this;
    }
    //! Destruction
    ~string_ref() {
      if (_thunk != nullptr) {
        _thunk(this, nullptr, _thunk_op::destruct);
      }
      _begin = _end = nullptr;
    }

    //! Returns whether the reference is empty or not
    VK_nodiscard bool empty() const noexcept { return _begin == _end; }
    //! Returns the size of the string
    VK_nodiscard size_type size() const noexcept { return _end - _begin; }
    //! Returns a null terminated C string
    VK_nodiscard cstring_type c_str() const noexcept { return (cstring_type)_begin; }
    //! Returns a null terminated C string
    VK_nodiscard const_pointer data() const noexcept { return _begin; }
    //! Returns the beginning of the string
    VK_nodiscard iterator begin() noexcept { return _begin; }
    //! Returns the beginning of the string
    VK_nodiscard const_iterator begin() const noexcept { return _begin; }
    //! Returns the beginning of the string
    VK_nodiscard const_iterator cbegin() const noexcept { return _begin; }
    //! Returns the end of the string
    VK_nodiscard iterator end() noexcept { return _end; }
    //! Returns the end of the string
    VK_nodiscard const_iterator end() const noexcept { return _end; }
    //! Returns the end of the string
    VK_nodiscard const_iterator cend() const noexcept { return _end; }
  };

  class string_literal : public string_view {
  public:
    template <size_t N>
    consteval string_literal(const char(&stringLiteral)[N]) : string_view(stringLiteral, N - 1){}
  };





  template <>
  struct traits::hash<string_view>{
    VK_constant u64 call64(string_view sv) noexcept {
      return impl::hash_append_bytes64(sv);
    }
    VK_constant u32 call32(string_view sv) noexcept {
      return impl::hash_append_bytes32(sv);
    }
  };

}

#endif//VALKYRIE_STRING_VIEW_HPP
