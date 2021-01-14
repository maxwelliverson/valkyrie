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



  enum class JsonType{
    Null,
    String,
    Boolean,
    Number,
    Object,
    Array
  };


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
