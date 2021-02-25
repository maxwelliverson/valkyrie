//
// Created by Maxwell on 2020-12-25.
//

#ifndef VALKYRIE_JSON_HPP
#define VALKYRIE_JSON_HPP

#include <valkyrie/Core/Utility/String.hpp>
#include <valkyrie/Core/Async/Atomic.hpp>

#include <memory>
#include <memory_resource>
#include <optional>
#include <deque>
#include <bit>
#include <vector>

namespace valkyrie::Core{


  namespace Json{
    enum class Type{
      Null,
      True,
      False,
      String,
      Number,
      Object,
      Array
    };
    enum class TypeFlags{
      Null         = 0x0000,
      True         = 0x0001,
      False        = 0x0002,
      PooledString = 0x0004,
      ConstString  = 0x0008,
      SmallString  = 0x0010,
      Int32        = 0x0020,
      UInt32       = 0x0040,
      Int64        = 0x0080,
      UInt64       = 0x0100,
      BigInt       = 0x0200,
      Float64      = 0x0400,
      Float32      = 0x0800,
      BigFloat     = 0x1000,
      Object       = 0x2000,
      Array        = 0x4000
    };
  }






  class StringPool{
    struct String{
      Atomic<u32> refCount;
      u32         length;
      utf8        buffer[];
    };
    std::pmr::unsynchronized_pool_resource memoryResource;
  };

  class JsonFile;
  class JsonObject;


  class JsonKey{
    String identifier_;
  };
  class JsonValue{
    JsonType type_;
  public:
  };


  class JsonObjectDescriptor{

  };

  class JsonIterator{
    JsonObject* pObject;
  };


  class JsonNull    : public JsonValue{};
  class JsonString  : public JsonValue{
    std::pmr::u8string value_;
  };
  class JsonBoolean : public JsonValue{
    bool value_;
  };
  class JsonNumber  : public JsonValue{
    enum { INTEGER, FLOAT } type;
    union{
      int i_value_;
      float f_value_;
    };
  };
  class JsonArray   : public JsonValue{

  };
  class JsonObject  : public JsonValue{
    JsonObjectDescriptor* pDescriptor;
    class Entry{
    public:
      MutableStringView key()       noexcept { }
      StringView        key() const noexcept { }
      JsonValue& value()       noexcept { }
      const JsonValue& value() const noexcept{
        return const_cast<const JsonValue&>(const_cast<Entry*>(this)->value());
      }
    };
  };
}

#endif//VALKYRIE_JSON_HPP
