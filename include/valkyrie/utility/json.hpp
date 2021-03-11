//
// Created by Maxwell on 2020-12-25.
//

#ifndef VALKYRIE_UTILITY_JSON_HPP
#define VALKYRIE_UTILITY_JSON_HPP

#include <valkyrie/adt/string.hpp>
#include <valkyrie/async/atomic.hpp>
#include <valkyrie/utility/bitflag.hpp>

#include <bit>
#include <deque>
#include <memory>
#include <memory_resource>
#include <optional>
#include <vector>

namespace valkyrie{


  namespace json{
    /*enum class type{
      Null,
      True,
      False,
      String,
      Number,
      Object,
      Array
    };*/
    enum class type_flags{
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

    using type = valkyrie::bitflag<type_flags>;

    class object;
    class symbol;
    class value;

    class symbol_pool;
    class document;

    class object{
      class member{
        const symbol* key;

      };
    };
  }






  class symbol_pool{
    struct symbol{
      atomic<u32> refCount;
      u32         length;
      utf8        buffer[];
    };
    std::pmr::unsynchronized_pool_resource memoryResource;
  };

  class json_file;
  class json_object;


  class json_key{
    string identifier_;
  };
  class json_value{
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
      mutable_string_view key()       noexcept { }
      string_view key() const noexcept { }
      JsonValue& value()       noexcept { }
      const JsonValue& value() const noexcept{
        return const_cast<const JsonValue&>(const_cast<Entry*>(this)->value());
      }
    };
  };
}

#endif//VALKYRIE_UTILITY_JSON_HPP
