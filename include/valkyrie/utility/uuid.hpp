//
// Created by Maxwell on 2020-11-18.
//

#ifndef VALKYRIE_UTILITY_UUID_HPP
#define VALKYRIE_UTILITY_UUID_HPP


#include <valkyrie/preprocessor.hpp>
#include <valkyrie/primitives.hpp>

#include <bit>
#include <compare>
#include <cstring>
#include <utility>


namespace valkyrie{


  VK_msvc_warning(push)
  VK_msvc_warning(disable:4715)
  class uuid {
    inline constexpr static u32 UuidSize = 16;
    inline constexpr static u32 UuidStringLength = 36;
    byte data_[UuidSize];

    inline constexpr static byte parseChar(char C) noexcept {
      switch(C){
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          return byte(C - '0');
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
          return byte(C - 'W');
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
          return byte(C - '7');
        default:
          VK_constexpr_assert(C == '0' && "Invalid character found during UUID parsing.");
          VK_runtime_block VK_unreachable;
      }
    }
    inline constexpr static byte parseByteAt(const char* pString, u32 pos) noexcept {
      return (parseChar(pString[pos]) << 4) | parseChar(pString[pos + 1]);
    }
    inline constexpr static utf8 parseHalfByte(u8 Byte) noexcept {

#define VK_UUID_BYTE_CASE(b) case 0x0##b: case 0x##b##0: return VK_raw_string(b)[0]

      switch (Byte) {
        case 0x00: return u8'0';
        VK_UUID_BYTE_CASE(1);
        VK_UUID_BYTE_CASE(2);
        VK_UUID_BYTE_CASE(3);
        VK_UUID_BYTE_CASE(4);
        VK_UUID_BYTE_CASE(5);
        VK_UUID_BYTE_CASE(6);
        VK_UUID_BYTE_CASE(7);
        VK_UUID_BYTE_CASE(8);
        VK_UUID_BYTE_CASE(9);
        VK_UUID_BYTE_CASE(a);
        VK_UUID_BYTE_CASE(b);
        VK_UUID_BYTE_CASE(c);
        VK_UUID_BYTE_CASE(d);
        VK_UUID_BYTE_CASE(e);
        VK_UUID_BYTE_CASE(f);
        default:
          VK_constexpr_assert(Byte == '0' && "Invalid character found during UUID parsing.");
          VK_runtime_block VK_unreachable;
      }

#undef VK_UUID_BYTE_CASE
    }
    inline constexpr static void writeByte(utf8* pBuffer, byte Byte) noexcept {
      pBuffer[0] = parseHalfByte(u8(Byte) & 0x0F);
      pBuffer[1] = parseHalfByte(u8(Byte) & 0xF0);
    }

    inline constexpr static void parseUuid(byte(&data)[UuidSize], const char* pString) noexcept {
      u32 i = 0;
      u32 j = 0;

#define VK_UUID_PARSE_UNTIL_BYTE(n) for (; i < n; ++i, j += 2) data[i] = (parseChar(pString[j]) << 4) | parseChar(pString[j + 1])
#define VK_UUID_PARSE_HYPHEN() VK_assert(pString[j] == '-'); ++j

      VK_UUID_PARSE_UNTIL_BYTE(4);
      VK_UUID_PARSE_HYPHEN();
      VK_UUID_PARSE_UNTIL_BYTE(6);
      VK_UUID_PARSE_HYPHEN();
      VK_UUID_PARSE_UNTIL_BYTE(8);
      VK_UUID_PARSE_HYPHEN();
      VK_UUID_PARSE_UNTIL_BYTE(10);
      VK_UUID_PARSE_HYPHEN();
      VK_UUID_PARSE_UNTIL_BYTE(16);

#undef VK_UUID_PARSE_HYPHEN
#undef VK_UUID_PARSE_UNTIL_BYTE
    }

    template <typename T, size_t N, size_t ...I>
    constexpr explicit uuid(const T(&array)[N], std::index_sequence<I...>) noexcept : data_{ static_cast<byte>(array[I]) ... }{}

    template <size_t N, u32 ...I>
    constexpr explicit uuid(const char(&literal)[N], std::integer_sequence<u32, I...>) noexcept : data_{ parseByteAt(literal, I)... }{}

    constexpr uuid() = default;

  public:

    static uuid generate() noexcept;

    // Conversion for Windows GUID struct
    constexpr explicit uuid(unsigned long dataA, u16 dataB, u16 dataC, const u8(&dataD)[8]) noexcept
        : data_{ byte(dataA & 0xFF), byte((dataA >> 8) & 0xFF), byte((dataA >> 16) & 0xFF), byte((dataA >> 24) & 0xFF),
                 byte(dataB & 0xFF), byte((dataB >> 8) & 0xFF),
                 byte(dataC & 0xFF), byte((dataC >> 8) & 0xFF),
                 byte(dataD[0]), byte(dataD[1]), byte(dataD[2]), byte(dataD[3]), byte(dataD[4]), byte(dataD[5]), byte(dataD[6]), byte(dataD[7]) }{}

    template <typename T> requires(sizeof(T) == sizeof(byte))
    constexpr explicit uuid(const T(&a)[16]) noexcept          : uuid(a,       std::make_index_sequence<16>{}){}
    constexpr explicit uuid(const char(&literal)[37]) noexcept : uuid(literal, std::integer_sequence<u32, 0, 2, 4, 6, 9,  11, 14, 16, 19, 21, 24, 26, 28, 30, 32, 34>{}){}
    constexpr explicit uuid(const char(&literal)[39]) noexcept : uuid(literal, std::integer_sequence<u32, 1, 3, 5, 7, 10, 12, 15, 17, 20, 22, 25, 27, 29, 31, 33, 35>{}){}


    /*constexpr explicit uuid(const char(&literal)[37]) noexcept {
      parseUuid(data_, literal);
    }
    constexpr explicit uuid(const char(&literal)[39]) noexcept{
      VK_assert(literal[0] == '{');
      VK_assert(literal[37] == '}');
      parseUuid(data_, &literal[1]);
    }*/

    friend constexpr utf8* writeToBuffer(utf8* pBuffer, u32 bufferLength, const uuid& uuid) noexcept {
      if (bufferLength < 36)
        return nullptr;
      u32 i = 0;
#define VK_UUID_WRITE_UNTIL_BYTE(...) for (; i < __VA_ARGS__; ++i, pBuffer += 2) uuid::writeByte(pBuffer, uuid.data_[i])
#define VK_UUID_WRITE_HYPHEN() *pBuffer++ = u8'-'

      VK_UUID_WRITE_UNTIL_BYTE(4);
      VK_UUID_WRITE_HYPHEN();
      VK_UUID_WRITE_UNTIL_BYTE(6);
      VK_UUID_WRITE_HYPHEN();
      VK_UUID_WRITE_UNTIL_BYTE(8);
      VK_UUID_WRITE_HYPHEN();
      VK_UUID_WRITE_UNTIL_BYTE(10);
      VK_UUID_WRITE_HYPHEN();
      VK_UUID_WRITE_UNTIL_BYTE(16);

#undef VK_UUID_WRITE_UNTIL_BYTE
#undef VK_UUID_WRITE_HYPHEN

      return pBuffer;
    }


    friend constexpr uuid operator^(const uuid& A, const uuid& B) noexcept {
      uuid result;
      for (u32 i = 0; i < UuidSize; ++i)
        result.data_[i] = A.data_[i] ^ B.data_[i];
      return result;
    }

    friend constexpr bool operator==(const uuid& A, const uuid& B) noexcept {
      VK_consteval_block {
        for (u32 i = 0; i < UuidSize; ++i)
          if (A.data_[i] != B.data_[i])
            return false;
        return true;
      }
      VK_runtime_block {
        return *reinterpret_cast<const std::pair<u64, u64>*>(&A) == *reinterpret_cast<const std::pair<u64, u64>*>(&B);
        //return !std::memcmp(A.data_, B.data_, UuidSize);
      }
    }
    friend constexpr std::strong_ordering operator<=>(const uuid& A, const uuid& B) noexcept {
      VK_consteval_block {
        for (u32 i = 0; i < UuidSize; ++i)
          if (auto result = A.data_[i] <=> B.data_[i]; result != std::strong_ordering::equal)
            return result;
        return std::strong_ordering::equal;
      }
      VK_runtime_block{
        //return *reinterpret_cast<const std::pair<u64, u64>*>(&A) <=> *reinterpret_cast<const std::pair<u64, u64>*>(&B);
        return std::memcmp(A.data_, B.data_, UuidSize) <=> 0;
      }
    }

    friend constexpr u64 toInteger(const uuid& uuid) noexcept {
      VK_consteval_block{
        u64 result = 0;
        for (u32 i = 0; i < UuidSize / 2; ++i)
          result |= (std::to_integer<u64>(uuid.data_[i]) << (i * CHAR_BIT));
        for (u32 i = 0; i < UuidSize / 2; ++i)
          result ^= (std::to_integer<u64>(uuid.data_[i + (UuidSize / 2)]) << (i * CHAR_BIT));
        return result;
      }
      VK_runtime_block{
        const auto& u64data = reinterpret_cast<const u64(&)[2]>(uuid.data_);
        return u64data[0] ^ u64data[1];
      }
    }
  };
  VK_msvc_warning(pop)


  /*class UuidGenerator{
    uuid state;
  public:
    UuidGenerator();
    explicit UuidGenerator(uuid seed);

    uuid next() noexcept;
  };*/


/*#define VK_uuid(uuid) ::valkyrie::uuid(std::in_place, VK_stringify({uuid}))

  uuid thisUuid{"2390a52c-3180-4129-8552-a506545cf6de"};

  static_assert(VK_uuid(2390a52c-3180-4129-8552-a506545cf6de) == uuid{"2390a52c-3180-4129-8552-a506545cf6de"});*/

}

template <>
struct std::hash<valkyrie::uuid>{
  inline valkyrie::u64 operator()(const valkyrie::uuid& uuid) const noexcept {
    return toInteger(uuid);
  }
};

#endif//VALKYRIE_UTILITY_UUID_HPP
