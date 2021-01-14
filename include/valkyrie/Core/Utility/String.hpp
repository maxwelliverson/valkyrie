//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_CORE_ADT_STRING_HPP
#define VALKYRIE_CORE_ADT_STRING_HPP

#include <valkyrie/Core/Utility/StringView.hpp>

#include <string>

namespace valkyrie::Core{




  /*class String{
    struct BigStorage{
      u64 isBig : 1;
      u64 capacity : 63;
      u32 length;
      u32 byteLength;
      utf8* data;
    };
    inline constexpr static u32 inlineSmallBufferSize = sizeof(BigStorage) - 2;
    inline constexpr static u32 smallBufferMaxSize = inlineSmallBufferSize - 1;
    struct SmallStorage{
      u8 isBig : 1 = false;
      u8 length : 7 = 0;
      u8 byteLength = 0;
      utf8 stringData[inlineSmallBufferSize]{};
    };
    union{
      BigStorage big;
      SmallStorage small{};
      u8 isBig : 1;
    };

    inline String(utf8* pData, size_t byteLength) {}

  public:
    using value_type = utf8;
    using pointer = utf8*;


    constexpr String() noexcept = default;
    template <valkyrie::Concepts::String Str>
    String(Str&& str) : String(string_traits<Str>::data(str), string_traits<Str>::byteLength(str)){}




  };*/

  using String = std::u8string;
}

/*template <>
struct valkyrie::Traits::String<>{};*/


#endif //VALKYRIE_CORE_ADT_STRING_HPP
