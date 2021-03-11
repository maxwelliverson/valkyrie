//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_STRING_VIEW_HPP
#define VALKYRIE_STRING_VIEW_HPP

#include <valkyrie/traits.hpp>

#include <iterator>
#include <string_view>
/*#include <new>*/

namespace valkyrie{

  //class String;
  template <size_t N>
  class small_string;
  template <size_t N>
  class static_string;
  class string_view;

  template <size_t N>
  class raw_string;

  class lazy_string;

  /*class Grapheme{
    utf8 byte[4];
  public:
    [[nodiscard]] u32 bytes() const noexcept {
      auto firstByte = (uint8_t)byte[0];
      if (!(firstByte & 0x80u)) [[likely]] return 1;
      if (!(firstByte & 0x20u)) return 2;
      if (!(firstByte & 0x10u)) return 3;
      return 4;
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept {
      return bool(byte[0]);
    }

    [[nodiscard]] friend bool operator==(Grapheme A, Grapheme B) noexcept{
      if (A.byte[0] != B.byte[0])
        return false;
      switch(A.bytes()) {
        case 4: if (A.byte[3] != B.byte[3]) return false;
          [[fallthrough]];
        case 3: if (A.byte[2] != B.byte[2]) return false;
          [[fallthrough]];
        case 2: if (A.byte[1] != B.byte[1]) return false;
          [[fallthrough]];
        case 1: return true;
        default:
          __assume(false);
      }
    }
    [[nodiscard]] friend std::strong_ordering operator<=>(Grapheme A, Grapheme B) noexcept{
      auto byte0Result = ((uint8_t)A.byte[0]) <=> ((uint8_t)B.byte[0]);
      if (byte0Result != std::strong_ordering::equal) return byte0Result;
      auto bytes = A.bytes();
      if (bytes == 1) [[likely]] return std::strong_ordering::equal;
      if (auto byte1Result = (uint8_t)A.byte[1] <=> (uint8_t)B.byte[1];
          bytes == 2 || byte1Result != std::strong_ordering::equal) return byte1Result;
      if (auto byte2Result = (uint8_t)A.byte[2] <=> (uint8_t)B.byte[2];
          bytes == 3 || byte2Result != std::strong_ordering::equal) return byte2Result;
      return (uint8_t)A.byte[3] <=> (uint8_t)B.byte[3];
    }
  };*/

  namespace detail{

    inline constexpr u32 offsetToNextCharacter(utf8 Char) noexcept{
      if (!(Char & 0x80u)) [[likely]] return 1;
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
    struct StringInfo<small_string<N>>{
      inline constexpr static const utf8* data(const small_string<N>& str) noexcept;
      inline constexpr static u32 byteLength(const small_string<N>& str) noexcept;
      inline constexpr static u32 length(const small_string<N>& str) noexcept;
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
  }

  class string_view {
    const utf8* pString = u8"";
    u32 byteLength_ = 0;
  public:

    using value_type = const utf8;
    using size_type = u32;
    using pointer = const utf8*;
    using cstring_type = const char*;

    //using reference = const Grapheme&;
    using reference = const utf8&;
    using iterator = detail::ConstStringIterator;
    using const_iterator = iterator;
    using sentinel = detail::ConstStringSentinel;
    using const_sentinel = sentinel;

    constexpr string_view() noexcept = default;
    constexpr string_view(const string_view &) noexcept = default;
    constexpr string_view(string_view &&) noexcept = default;

    constexpr string_view & operator=(const string_view &) noexcept = default;
    constexpr string_view & operator=(string_view &&) noexcept = default;

    /*constexpr */~string_view() noexcept = default;


    template <typename Str> requires(!std::same_as<string_view, std::remove_cvref_t<Str>> && requires(Str&& str){
      { detail::StringInfo<std::remove_cvref_t<Str>>::data((Str&&)str) } noexcept;
    })
    constexpr string_view(Str&& str) noexcept
        : pString(detail::StringInfo<std::remove_cvref_t<Str>>::data(std::forward<Str>(str))),
          byteLength_(detail::StringInfo<std::remove_cvref_t<Str>>::byteLength(std::forward<Str>(str))){}

    constexpr string_view(const utf8* pData, u32 stringLength) noexcept
        : pString(pData),
          byteLength_(stringLength){}


    [[nodiscard]] cstring_type c_str() const noexcept { return (cstring_type)pString; }

    [[nodiscard]] constexpr pointer data() const noexcept { return pString; }

    [[nodiscard]] constexpr u32 size() const noexcept { return byteLength_; }
    [[nodiscard]] constexpr u32 length() const noexcept { return byteLength_; }

    [[nodiscard]] constexpr u32 byteLength() const noexcept { return byteLength_; }

    [[nodiscard]] reference front() const noexcept { return *begin(); }
    [[nodiscard]] reference back() const noexcept { return *(pString + byteLength_ - 1); }

    [[nodiscard]] iterator begin()        const noexcept { return iterator(pString); }
    [[nodiscard]] const_iterator cbegin() const noexcept { return begin(); }
    [[nodiscard]] sentinel end()          const noexcept { return sentinel(pString + byteLength_); }
    [[nodiscard]] const_sentinel cend()   const noexcept { return end(); }

    [[nodiscard]] friend bool operator==(string_view A, string_view B) noexcept {
      return A.byteLength_ == B.byteLength_ &&
             detail::stringCompare(A.pString, B.pString, A.byteLength_) == std::strong_ordering::equal;
    }
    [[nodiscard]] friend std::strong_ordering operator<=>(string_view A, string_view B) noexcept {
      auto compResult = detail::stringCompare(A.pString, B.pString, std::min(A.byteLength_, B.byteLength_));
      if (compResult != std::strong_ordering::equal)
        return compResult;
      return A.byteLength_ <=> B.byteLength_;
    }
  };
  class mutable_string_view {
    utf8* pString = nullptr;
    u32   stringLength = 0;
  public:

    using value_type = utf8;
    using size_type = u32;
    using pointer = utf8*;
    using cstring_type = char*;

    using reference = utf8&;
    using const_reference = const utf8&;
    using iterator = detail::StringIterator;
    using const_iterator = detail::ConstStringIterator;
    using sentinel = detail::StringSentinel;
    using const_sentinel = detail::ConstStringSentinel;

    constexpr mutable_string_view() = default;
    constexpr mutable_string_view(utf8* pString, size_t N) noexcept
        : pString(pString), stringLength(N){}
    constexpr explicit mutable_string_view(utf8* pString) noexcept
        : pString(pString), stringLength(std::char_traits<utf8>::length(pString)){}

    VK_nodiscard cstring_type c_str() const noexcept { return (cstring_type)pString; }

    VK_nodiscard constexpr pointer data() const noexcept { return pString; }

    VK_nodiscard constexpr u32 size() const noexcept { return stringLength; }
    VK_nodiscard constexpr u32 length() const noexcept { return stringLength; }

    VK_nodiscard reference front() const noexcept { return *begin(); }
    VK_nodiscard reference back() const noexcept { return *(pString + stringLength - 1); }

    VK_nodiscard iterator        begin() const noexcept { return iterator(pString); }
    VK_nodiscard const_iterator cbegin() const noexcept { return const_iterator(pString); }

    VK_nodiscard sentinel        end()   const noexcept { return sentinel(pString + stringLength); }
    VK_nodiscard const_sentinel cend()   const noexcept { return const_sentinel(pString + stringLength); }

    VK_nodiscard friend bool operator==(mutable_string_view A, mutable_string_view B) noexcept {
      return A.stringLength == B.stringLength && std::char_traits<utf8>::compare(A.pString, B.pString, A.stringLength) == 0;
    }
    VK_nodiscard friend std::strong_ordering operator<=>(mutable_string_view A, mutable_string_view B) noexcept {

      //return std::lexicographical_compare_three_way(A.pString, A.pString + A.stringLength, B.pString, B.pString + B.stringLength);

      //return std::u8string_view{A.pString, A.stringLength} <=> std::u8string_view{B.pString, B.stringLength};

      const u32 minLength = std::min(A.stringLength, B.stringLength);
      for (u32 i = 0; i < minLength; ++i)
        if (auto compResult = A.pString[i] <=> B.pString[i]; compResult != std::strong_ordering::equal)
          return compResult;
      return A.stringLength <=> B.stringLength;


      //auto compResult = std::char_traits<utf8>::compare(A.pString, B.pString, std::min(A.stringLength, B.stringLength));
      /*if (compResult != std::strong_ordering::equal)
        return compResult;
      return A.stringLength <=> B.stringLength;*/
    }

    operator string_view() const noexcept {
      return {pString, stringLength};
    }
  };

  /*class StringReference{

    inline static constexpr utf8 EmptyString[1] = u8"";

  public:

    using value_type = const utf8;
    using size_type  = u32;
    using pointer = const utf8*;
    using iterator = pointer;
    using reference = const utf8&;

  protected:
    enum class ThunkOp{
      default_ctor,
      copy_ctor,
      move_ctor,
      copy_assign,
      move_assign,
      destruct,
      size,
      begin,
      end,
      callback
    };
    struct ThunkIn{
      union{
        byte* pThis;
        const byte* pConstThis;
      };
      ThunkOp op;
      union{
        byte* pOther;
        const byte* pConstOther;
        void* pUserData;
        const void* pConstUserData;
        struct {
          void(* pFunction)(byte* pStorage, void* pData);
          void* pData;
        } callback;
      };
    };
    union ThunkOut{
      pointer* ppString;
      u32*     pSize;
      i32*     pComp;
    };

    using ThunkFn = void (*)(ThunkIn in, ThunkOut out);

    inline constexpr static size_t erasedStorageSize = 4 * sizeof(void*);

    template <typename T>
    struct DefaultThunkHolder{
      inline constexpr static ThunkFn function = [](ThunkIn in, ThunkOut out) {
        switch (in.op) {
          case ThunkOp::default_ctor:
            VK_assert(in.pThis);
            new(in.pThis) T{};
            break;
            *//*case ThunkOp::callback:
              in.callback.pFunction(pTo, in.callback.pData);
              break;*//*
          case ThunkOp::copy_ctor:
            VK_assert(in.pThis && in.pConstOther);
            new(in.pThis) T{*(const T*)in.pConstOther};
            break;
          case ThunkOp::move_ctor:
            VK_assert(in.pThis && in.pOther);
            new(in.pThis) T{std::move(*(T*)in.pOther)};
            break;
          case ThunkOp::copy_assign:
            VK_assert(in.pThis &&in.pConstOther);
            (*(T*)in.pThis) = (*(const T*)in.pConstOther);
            break;
          case ThunkOp::move_assign:
            VK_assert(in.pThis &&in.pOther);
            (*(T*)in.pThis) = std::move(*(T*)in.pOther);
            break;
          case ThunkOp::destruct:
            VK_assert(in.pThis&&!in.pConstOther);
            ((T*)in.pThis)->~T();
            break;
          case ThunkOp::size:
            VK_assert(in.pConstThis && !in.pConstOther && out.pSize);
            if constexpr(requires(const T& str){ { std::size(str) } -> std::integral; })
              *out.pSize = static_cast<u32>(std::size(*(const T*)in.pConstThis));
            else if constexpr(requires(const T& str){ { size(str) } -> std::integral; })
              *out.pSize = static_cast<u32>(size(*(const T*)in.pConstThis));
            else
                static_assert(requires(const T& str){ { size(str) } -> std::integral; });
            break;
          case ThunkOp::begin:
            VK_assert(in.pConstThis && !in.pConstOther && out.ppString);
            if constexpr(requires(const T& str){ { std::begin(str) } -> std::random_access_iterator; })
              *out.ppString = (pointer)&*std::begin(*(const T*)in.pConstThis);
            else if constexpr(requires(const T& str){ { begin(str) } -> std::random_access_iterator; })
              *out.ppString = (pointer)&*begin(*(const T*)in.pConstThis);
            else
                static_assert(requires(const T& str){ { begin(str) } -> std::random_access_iterator; });
            break;
          case ThunkOp::end:
            VK_assert(in.pConstThis && !in.pConstOther && out.ppString);
            if constexpr(requires(const T& str){ { std::end(str) } -> std::sentinel_for<decltype(std::begin(str))>; })
              *out.ppString = (pointer)&*std::end(*(const T*)in.pConstThis);
            else if constexpr(requires(const T& str){ { end(str) } -> std::sentinel_for<decltype(begin(str))>; })
              *out.ppString = (pointer)&*end(*(const T*)in.pConstThis);
            else
                static_assert(requires(const T& str){ { end(str) } -> std::sentinel_for<decltype(begin(str))>; });
            break;
          VK_no_default;
        }
      };
    };

    inline static void emptyThunk(ThunkIn in, ThunkOut out) noexcept {
      switch (in.op) {
        case ThunkOp::size:
          VK_assert(out.pSize);
          *out.pSize = 0;
          break;
        case ThunkOp::begin:
        case ThunkOp::end:
          VK_assert(out.ppString);
          *out.ppString = EmptyString;
          break;
        default:
          (void)0;
      }
    }

    const ThunkFn thunk_fn_ = emptyThunk;
    byte storage_[erasedStorageSize];

  public:

    StringReference() noexcept = default;
    explicit StringReference(const char(&)[1]) noexcept{}
    StringReference(const utf8(&)[1]) noexcept{}

    template <size_t N>
    StringReference(const utf8(&string)[N]) noexcept : StringReference(string_view(string)){}
    template <size_t N>
    explicit StringReference(const char(&string)[N]) noexcept : StringReference(string_view(string)){}


    template <typename T>
    requires(sizeof(T) <= erasedStorageSize && std::semiregular<T> &&
             std::ranges::random_access_range<T> && same_as_one_of<std::ranges::range_value_t<T>, char, utf8>)
    StringReference(T&& string) noexcept : thunk_fn_(DefaultThunkHolder<std::remove_cvref_t<T>>::function){
      if constexpr (std::is_reference_v<T>)
        thunk_fn_({ .pThis = storage_, .op = ThunkOp::copy_ctor, .pConstOther = (const byte*)std::addressof(string) }, {});
      else
        thunk_fn_({ .pThis = storage_, .op = ThunkOp::move_ctor, .pOther      = (byte*)std::addressof(string) }, {});
    }


    StringReference(const StringReference& Other) : thunk_fn_(Other.thunk_fn_){
      thunk_fn_({ .pThis = storage_, .op = ThunkOp::copy_ctor, .pConstOther = Other.storage_ }, {});
    }
    StringReference(StringReference&& Other) noexcept : thunk_fn_(Other.thunk_fn_){
      thunk_fn_({ .pThis = storage_, .op = ThunkOp::move_ctor, .pOther = Other.storage_ }, {});
    }

    StringReference& operator=(const StringReference& other) VK_throws {

      if (this != &other) {
        if (thunk_fn_ == other.thunk_fn_) {
          thunk_fn_( {.pThis = storage_, .op = ThunkOp::copy_assign, .pConstOther = other.storage_ }, {});
        } else {
#if VK_exceptions_enabled
          StringReference tmp{std::move(*this)};
          this->~StringReference();
          try {
            new (this) StringReference(other);
          } catch (...) {
            new (this) StringReference{std::move(tmp)};
            throw;
          }
#else
          this->~StringReference();
          new (this) StringReference(other);
#endif
        }
      }
      return *this;
    }
    StringReference& operator=(StringReference&& other) noexcept {
      if (thunk_fn_ == other.thunk_fn_) {
        thunk_fn_({ .pThis = storage_, .op = ThunkOp::move_assign, .pOther = other.storage_ }, {});
      } else {
        this->~StringReference();
        new(this) StringReference(std::move(other));
      }
      return *this;
    }

    ~StringReference() noexcept {
      thunk_fn_({ .pThis = storage_, .op = ThunkOp::destruct }, {});
    }

  protected:

    explicit StringReference(ThunkFn thunkFunction) noexcept : thunk_fn_(thunkFunction){
      thunk_fn_({ .pThis = storage_, .op = ThunkOp::default_ctor }, {});
    }

  public:


    [[nodiscard]] bool empty() const noexcept {
      return !size();
    }
    [[nodiscard]] size_type size() const noexcept {
      u32 Size = 0;
      thunk_fn_({ .pConstThis = storage_, .op = ThunkOp::size }, { .pSize = &Size });
      return Size;
    }

    [[nodiscard]] const char* c_str() const noexcept {
      const char* pResult;
      thunk_fn_({ .pConstThis = storage_, .op = ThunkOp::begin }, { .ppString = (const utf8**)&pResult });
      return pResult;
    }

    [[nodiscard]] pointer data() const noexcept {
      pointer pResult;
      thunk_fn_({ .pConstThis = storage_, .op = ThunkOp::begin }, { .ppString = &pResult });
      return pResult;
    }

    [[nodiscard]] iterator begin() const noexcept { return data(); }
    [[nodiscard]] iterator cbegin() const noexcept { return data(); }

    [[nodiscard]] iterator end() const noexcept {
      pointer pResult;
      thunk_fn_({ .pConstThis = storage_, .op = ThunkOp::end }, { .ppString = &pResult });
      return pResult;
    }
    [[nodiscard]] iterator cend() const noexcept { return end(); }
  };*/

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
    [[nodiscard]] bool empty() const noexcept { return _begin == _end; }
    //! Returns the size of the string
    [[nodiscard]] size_type size() const noexcept { return _end - _begin; }
    //! Returns a null terminated C string
    [[nodiscard]] cstring_type c_str() const noexcept { return (cstring_type)_begin; }
    //! Returns a null terminated C string
    [[nodiscard]] const_pointer data() const noexcept { return _begin; }
    //! Returns the beginning of the string
    [[nodiscard]] iterator begin() noexcept { return _begin; }
    //! Returns the beginning of the string
    [[nodiscard]] const_iterator begin() const noexcept { return _begin; }
    //! Returns the beginning of the string
    [[nodiscard]] const_iterator cbegin() const noexcept { return _begin; }
    //! Returns the end of the string
    [[nodiscard]] iterator end() noexcept { return _end; }
    //! Returns the end of the string
    [[nodiscard]] const_iterator end() const noexcept { return _end; }
    //! Returns the end of the string
    [[nodiscard]] const_iterator cend() const noexcept { return _end; }
  };

  class string_literal : public string_view {
  public:
    template <size_t N>
    consteval string_literal(const char(&stringLiteral)[N]) : string_view(stringLiteral, N - 1){}
  };

  class twine{};

  /*class Twine{
    class ErasedValueType{

    };
    template <typename T>
    class ToStringPlaceholder{
      T* pValue;
    public:

    };
  public:

  };*/

}

#endif//VALKYRIE_STRING_VIEW_HPP
