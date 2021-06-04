//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_ADT_STRING_HPP
#define VALKYRIE_ADT_STRING_HPP

#include "string_view.hpp"

#include <valkyrie/memory/allocators/std_allocator.hpp>
#include <valkyrie/memory/default_allocator.hpp>

#include <string>

namespace valkyrie{




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
    template <valkyrie::concepts::String Str>
    String(Str&& str) : String(string_traits<Str>::data(str), string_traits<Str>::byteLength(str)){}




  };*/

  template <allocator_c Alloc = default_allocator>
  using basic_string = std::basic_string<utf8, std::char_traits<utf8>, std_allocator<utf8, Alloc>>;

  using string = basic_string<>;

}


#endif //VALKYRIE_ADT_STRING_HPP
