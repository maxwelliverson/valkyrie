//
// Created by Maxwell on 2020-11-07.
//

#ifndef VULKAN_WRAPPER_VK_FEATURES_H
#define VULKAN_WRAPPER_VK_FEATURES_H

#include <string>
#include <sstream>
#include <string_view>
#include <concepts>
#include <iostream>
#include <charconv>
#include <system_error>
#include <boost/container/small_vector.hpp>
#include <boost/static_string.hpp>

#include "vk_pp.h"
#include "vk_util.h"


namespace Vk{

  struct Serializable{};

  class String;

  class FeatureBase : public Serializable{
  protected:
    bool enabled;

  public:

    constexpr FeatureBase() = default;
    constexpr FeatureBase(bool enabled) noexcept : enabled(enabled){}

    constexpr void enable() noexcept {
      enabled = true;
    }
    constexpr void disable() noexcept {
      enabled = false;
    }

    constexpr explicit operator bool() const noexcept {
      return enabled;
    }
  };
  template <typename F>
  class Feature : public FeatureBase{
    friend std::string serialize(const Feature& feature) noexcept{
      std::string tabs;
      return F::properties.serializeWith(static_cast<const F*>(std::addressof(feature)), tabs);
    }
    template<auto... Components>
    friend struct ComponentSet;

  protected:
    inline constexpr static bool isAPropertyGroup = false;

  public:
    F& operator=(bool Value) noexcept {
      this->enabled = Value;
      return *(F*)this;
    }
  };
  template <typename Feat, typename Comp>
  class FeatureProperty{};
  template <typename Feat, typename Property>
  class Subfeature : public FeatureProperty<Feat, Property>, public Feature<Property>{};
  template <typename Feat, typename Property, typename ValueType>
  class FeaturePropertyValue : public FeatureProperty<Feat, Property>{
    using param_t = ::Vk::param_t<ValueType>;
  protected:
    ValueType value_;
  public:
    using value_type = ValueType;

    constexpr Property& operator=(param_t Val) noexcept {
      value_ = Val;
      return static_cast<Property&>(*this);
    }

    constexpr param_t value() const noexcept {
      return value_;
    }

    constexpr operator param_t() const noexcept {
      return value();
    }
  };
  template <typename Feat, typename Property>
  class FeaturePropertyValue<Feat, Property, String> : public FeatureProperty<Feat, Property>{
  public:
    using value_type = boost::static_strings::static_string<256>;
  private:
    using param_t = const value_type&;
  protected:
    value_type value_;
  public:


    template <size_t N> requires(N <= 256)
    constexpr Property& operator=(StringLiteral<N> literal) noexcept{
      value_.assign(literal.data(), literal.size());
    }
    constexpr Property& operator=(param_t Val) noexcept {
      value_ = Val;
      return static_cast<Property&>(*this);
    }

    constexpr param_t value() const noexcept {
      return value_;
    }

    constexpr operator param_t() const noexcept {
      return value();
    }
  };
  template <typename Feat, typename Property, typename ValueType> requires(std::is_class_v<ValueType>)
  class FeaturePropertyValue<Feat, Property, ValueType> : public FeatureProperty<Feat, Property>, public ValueType{
    using param_t = ::Vk::param_t<ValueType>;
  public:
    using value_type = ValueType;

    constexpr Property& operator=(param_t Val) noexcept {
      (*(ValueType*)this) = Val;
      return static_cast<Property&>(*this);
    }

    constexpr param_t value() const noexcept {
      return (*(const ValueType*)this);
    }
/*
    constexpr operator param_t() const noexcept {
      return value();
    }*/
  };
  template <typename Feat, typename Property>
  class FeaturePropertyGroup : public FeatureProperty<Feat, Property>, public Serializable{
  public:
    friend std::string serialize(const FeaturePropertyGroup<Feat, Property>& feature) noexcept{
      std::string tabs;
      return Property::properties.serializeWith(static_cast<const Property*>(std::addressof(feature)), tabs);
    }

  protected:
    template<auto... Components>
    friend struct ComponentSet;
    inline constexpr static bool isAPropertyGroup = true;
  };

  template <typename Group>
  class PropertyGroup : public Serializable{
  public:
    using property_group_t = Group;
    friend std::string serialize(const PropertyGroup& feature) noexcept{
      std::string tabs;
      return Group::properties.serializeWith(static_cast<const Group*>(std::addressof(feature)), tabs);
    }
  protected:
    template<auto... Components>
    friend struct ComponentSet;
    inline constexpr static bool isAPropertyGroup = true;
  };

  template<typename T>
  concept string_like = requires(const T &str) {
    { std::data(str) }
    ->std::convertible_to<const char *>;
    { std::size(str) }
    ->std::convertible_to<size_t>;
  };
  template <typename T>
  concept serializable = std::derived_from<std::remove_cvref_t<T>, Serializable>;

  namespace detail {
    template<auto... Components>
    struct ComponentSet;

    template<auto... Comps>
    struct LastComponent;
    template<auto Head, auto... Tail>
    struct LastComponent<Head, Tail...> : LastComponent<Tail...> {};
    template<auto Head>
    struct LastComponent<Head> {
      inline constexpr static auto value = Head;
    };
    template <auto ...Comp>
    struct PruneLastComponent;
    enum class DiscardThis{};
    inline constexpr static DiscardThis discardThisValue{};
    template <auto ...Comp>
    struct PrunedComponents {
      template <auto Value>
      using push_front = PrunedComponents<Value, Comp...>;
      using type = ComponentSet<Comp...>;
    };
    template <auto Head, auto Neck, auto ...Tail>
    struct PruneLastComponent<Head, Neck, Tail...>{
      using type = typename PruneLastComponent<Neck, Tail...>::type::template push_front<Head>;
    };
    template <auto Head>
    struct PruneLastComponent<Head>{
      using type = PrunedComponents<Head>;
    };
    template <>
    struct PruneLastComponent<discardThisValue>{
      using type = PrunedComponents<>;
    };
    template <>
    struct PruneLastComponent<>{
      using type = PrunedComponents<>;
    };



    namespace detail{
      template <typename F, typename Prop>
      std::true_type isAPropGroup(const FeaturePropertyGroup<F, Prop>*);
      template <typename Group>
      std::true_type isAPropGroup(const PropertyGroup<Group>*);
      std::false_type isAPropGroup(...);
    }




    template<auto... Components>
    struct ComponentSet{
    private:
      inline constexpr static decltype(auto) lastComponent = LastComponent<Components...>::value;

      template <auto pComponent>
      static void endl(std::stringstream &out) noexcept{
        if constexpr (!std::same_as<decltype(pComponent), decltype(lastComponent)>) {
          out << ',';
        } else if (pComponent != lastComponent) {
          out << ',';
        }
        out << "\n";
      }

      template <string_like Val>
      static void printValue(const Val& value, std::stringstream &out) noexcept{
        out << '\"' << std::string_view{std::data(value), std::size(value)} << '\"';
      }
      template <typename Val>
      static void printValue(const Val& value, std::stringstream &out) noexcept{
        out << value;
      }

      template<auto pComponent, typename F>
      static void serializeComponent(const F *pFeature, std::stringstream &out, std::string &tabDepth) noexcept{
        auto &&component = pFeature->*pComponent;
        using component_type = std::remove_cvref_t<decltype(component)>;
        using value_type = typename component_type::value_type;
        out << tabDepth << component.property_field_name << ": ";
        printValue(component.value(), out);
        endl<pComponent>(out);
      }
      template<auto pComponent, typename F>
      static void serializeComponent(const F *pFeature, std::stringstream &out, std::string &tabDepth) noexcept requires(requires{
        { pFeature->*pComponent } -> serializable;
      }){
        auto &&component = pFeature->*pComponent;
        out << component.properties.serializeWith(std::addressof(component), tabDepth);
        endl<pComponent>(out);
      }

    public:

      constexpr ComponentSet() noexcept = default;

      template <typename F>
      static std::string serializeWith(const F *pFeature, std::string &tabDepth) noexcept {
        std::stringstream out;
        out << std::boolalpha;
        out << tabDepth << F::property_field_name << ": {\n";
        tabDepth.push_back('\t');
        if constexpr(!decltype(detail::isAPropGroup(pFeature))::value){
          out << tabDepth << "enabled: " << (bool)*pFeature << ",\n";
        }
        ((serializeComponent<Components>(pFeature, out, tabDepth)), ...);
        tabDepth.pop_back();
        out << tabDepth << '}';
        return out.str();
      }
    };
    template <>
    struct ComponentSet<>{
      template <typename F>
      static std::string serializeWith(const F *pFeature, std::string &tabDepth) noexcept {
        std::stringstream out;
        out << std::boolalpha;
        out << tabDepth << F::property_field_name << ": {\n";
        tabDepth.push_back('\t');
        out << tabDepth << "enabled: " << *pFeature << "\n";
        tabDepth.pop_back();
        out << tabDepth << '}';
        return out.str();
      }
    };
    template <auto ...Comp>
    using component_set_dropped_end_t = typename PruneLastComponent<Comp...>::type::type;
    static_assert(std::same_as<component_set_dropped_end_t<>, ComponentSet<>>);
    static_assert(std::same_as<component_set_dropped_end_t<discardThisValue>, ComponentSet<>>);
    static_assert(std::same_as<component_set_dropped_end_t<0>, ComponentSet<0>>);
    static_assert(std::same_as<component_set_dropped_end_t<0, discardThisValue>, ComponentSet<0>>);
  }
  /*class object;

  struct read_result{
    std::span<const char> buffer;
    std::error_code error;
  };
  struct write_result{
    std::span<char> buffer;
    std::error_code error;
  };

  class object_descriptor {
  public:

    virtual void doDefaultInit(std::byte* bufferView) const noexcept = 0;
    virtual void doCopyConstruct(std::byte* toBuffer, const std::byte* fromBuffer) const noexcept = 0;

    virtual std::span<const object_descriptor * const> doSubobjectDescriptors() const noexcept = 0;
    virtual std::string_view doTypeName() const noexcept = 0;
    virtual void doWrite(const std::byte* objectBuffer, write_result& result) const noexcept = 0;
    virtual void doRead(std::byte* objectBuffer, read_result& result) const noexcept = 0;
    [[nodiscard]] virtual bool   doIsPrimitive() const noexcept = 0;

    [[nodiscard]] virtual size_t doSize() const noexcept = 0;
    [[nodiscard]] virtual size_t doAlignment() const noexcept = 0;
  };

  class object_registry;

  class object{
    class instance;
    friend class object_registry;

    struct subobject{
      class reference;
      class const_reference;
      class iterator;
      class const_iterator;
      class sentinel;
      class const_sentinel;
      class range;

      class reference{
        friend class const_reference;

        const object_descriptor * pType;
        std::span<std::byte> bufferSpan;

      public:
        reference(const object_descriptor * pType, std::byte* pBuffer) noexcept : pType(pType), bufferSpan(pBuffer, this->pType->doSize()){ }

        template <string_like S>
        reference& operator=(S&& str) {
          if (auto error = pType->doRead(bufferSpan, {std::data(str), std::size(str)}))
            throw std::system_error(error);
          return *this;
        }
      };
      class const_reference{
        const object_descriptor * pType;
        std::span<const std::byte> bufferSpan;


      public:
        const_reference(const object_descriptor * pType, const std::byte* pBuffer) noexcept : pType(pType), bufferSpan(pBuffer, this->pType->doSize()){ }
        const_reference(const reference& ref) noexcept : pType(ref.pType), bufferSpan(ref.bufferSpan){}
      };
      class iterator{

        friend class const_iterator;
        friend class sentinel;

        std::span<const object_descriptor * const> subobjectTypes;
        std::byte* pBuffer;

      public:

        explicit iterator(instance* pObject) noexcept
            : subobjectTypes(pObject->pType->doSubobjectDescriptors()),
              pBuffer(pObject->buffer){}

        [[nodiscard]] reference operator*() const noexcept{
          return reference(subobjectTypes.front(), pBuffer);
        }

        iterator& operator++() noexcept {
          pBuffer += subobjectTypes.front()->doSize();
          subobjectTypes = subobjectTypes.last(subobjectTypes.size() - 1);
          return *this;
        }
        [[nodiscard("Should only ever use post-increment with expressed purpose; "
                    "discarding the return value is almost always indicative of bad usage."
                    "Consider using pre-increment instead.")]]
        iterator operator++(int) noexcept{
          iterator copy = *this;
          ++*this;
          return copy;
        }
      };
      class const_iterator{

        friend class const_sentinel;

        std::span<const object_descriptor * const> subobjectTypes;
        const std::byte* pBuffer;

      public:

        explicit const_iterator(const instance* pObject) noexcept
            : subobjectTypes(pObject->pType->doSubobjectDescriptors()),
              pBuffer(pObject->buffer){}
        const_iterator(const iterator& iter) noexcept
            : subobjectTypes(iter.subobjectTypes),
              pBuffer(iter.pBuffer){}

        [[nodiscard]] const_reference operator*() const noexcept{
          return const_reference(subobjectTypes.front(), pBuffer);
        }

        const_iterator& operator++() noexcept {
          pBuffer += subobjectTypes.front()->doSize();
          subobjectTypes = subobjectTypes.last(subobjectTypes.size() - 1);
          return *this;
        }
        [[nodiscard("Should only ever use post-increment with expressed purpose; "
        "discarding the return value is almost always indicative of bad usage."
        "Consider using pre-increment instead.")]]
        const_iterator operator++(int) noexcept{
          const_iterator copy = *this;
          ++*this;
          return copy;
        }
      };
      class sentinel{

        friend class const_sentinel;

        instance* pObject;
      public:
        explicit sentinel(instance* pObject) noexcept : pObject(pObject){}

        friend bool operator==(const iterator& iter, sentinel sent) noexcept{
          return iter.subobjectTypes.empty();
        }
      };
      class const_sentinel{
        const instance* pObject;
      public:
        explicit const_sentinel(const instance* pObject) noexcept : pObject(pObject){}
        const_sentinel(sentinel sent) noexcept : pObject(sent.pObject){}

        friend bool operator==(const const_iterator& iter, sentinel sent) noexcept{
          return iter.subobjectTypes.empty();
        }
      };
      class range{
        instance* pObject;
      public:
        explicit range(instance* pObject) noexcept : pObject(pObject){}

        [[nodiscard]] iterator begin() const noexcept {
          return iterator(pObject);
        }
        [[nodiscard]] const_iterator cbegin() const noexcept {
          return const_iterator(pObject);
        }
        [[nodiscard]] sentinel end() const noexcept {
          return sentinel(pObject);
        }
        [[nodiscard]] const_sentinel cend() const noexcept {
          return const_sentinel(pObject);
        }
      };
      class const_range{
        const instance* pObject;
      public:
        explicit const_range(const instance* pObject) noexcept : pObject(pObject){}

        [[nodiscard]] const_iterator begin() const noexcept {
          return const_iterator(pObject);
        }
        [[nodiscard]] const_iterator cbegin() const noexcept {
          return const_iterator(pObject);
        }
        [[nodiscard]] const_sentinel end() const noexcept {
          return const_sentinel(pObject);
        }
        [[nodiscard]] const_sentinel cend() const noexcept {
          return const_sentinel(pObject);
        }
      };
    };

    class instance{

      friend struct subobject;

      const object_descriptor * pType;
      std::byte buffer[];
    public:

      explicit instance(const object_descriptor * pType) noexcept : pType(pType){
        this->pType->doDefaultInit(buffer);
      }
    };

    instance* pInstance;

  public:


    [[nodiscard]] subobject::range subobjects() noexcept {
      return subobject::range(pInstance);
    }
    [[nodiscard]] subobject::const_range subobjects() const noexcept {
      return subobject::const_range(pInstance);
    }
  };

  class object_identifier{
    using string_chunk = boost::static_strings::static_string<16>;
    using chunk_vector = boost::container::small_vector<string_chunk, 8>;

    chunk_vector chunks_;

  };

  class object_registry{
    class key : public boost::static_strings::static_string<16>{
      using base_ = boost::static_strings::static_string<16>;
    public:
      using base_::base_;
    };
    class key_ref{
      std::string_view key_ref_;
    public:
      template <string_like S>
      explicit key_ref(const S& str) noexcept
          : key_ref_(std::data(str), std::size(str)){}

      friend bool operator==(const key_ref& ref, const key& k) noexcept {
        return ref.key_ref_ == std::string_view{k.data(), k.size()};
      }
      friend std::strong_ordering operator<=>(const key_ref& ref, const key& k) noexcept {
        switch (ref.key_ref_.compare(std::string_view(k.data(), k.size()))) {
          case -1:
            return std::strong_ordering::less;
          case 0:
            return std::strong_ordering::equal;
          case 1:
            return std::strong_ordering::greater;
          default:
            __assume(0);
        }
      }
    };

    std::unordered_map<key, std::unique_ptr<object::instance>> registry_;

  public:

  };

  class primitive_object_descriptor : public object_descriptor{
  public:
    [[nodiscard]] std::span<const object_descriptor * const> doSubobjectDescriptors() const noexcept final {
      return {};
    }
    [[nodiscard]] bool doIsPrimitive() const noexcept final{
      return true;
    }
  };

  namespace detail{
    template <typename T>
    struct primitive_type_name;

    template <>
    struct primitive_type_name<char>{
      inline constexpr static StringLiteral name{"char"};
    };
    template <>
    struct primitive_type_name<bool>{
      inline constexpr static StringLiteral name{"bool"};
    };

    template <>
    struct primitive_type_name<int8_t>{
      inline constexpr static StringLiteral name{"i8"};
    };
    template <>
    struct primitive_type_name<uint8_t>{
      inline constexpr static StringLiteral name{"u8"};
    };
    template <>
    struct primitive_type_name<int16_t>{
      inline constexpr static StringLiteral name{"i16"};
    };
    template <>
    struct primitive_type_name<uint16_t>{
      inline constexpr static StringLiteral name{"u16"};
    };
    template <>
    struct primitive_type_name<int32_t>{
      inline constexpr static StringLiteral name{"i32"};
    };
    template <>
    struct primitive_type_name<uint32_t>{
      inline constexpr static StringLiteral name{"u32"};
    };
    template <>
    struct primitive_type_name<int64_t>{
      inline constexpr static StringLiteral name{"i64"};
    };
    template <>
    struct primitive_type_name<uint64_t>{
      inline constexpr static StringLiteral name{"u64"};
    };

    template <>
    struct primitive_type_name<float>{
      inline constexpr static StringLiteral name{"f32"};
    };
    template <>
    struct primitive_type_name<double>{
      inline constexpr static StringLiteral name{"f64"};
    };
  };

  template <std::integral I>
  class integral_object_descriptor final : public primitive_object_descriptor{
  public:
    void doDefaultInit(std::byte* bufferView) const noexcept override{
      std::memset(bufferView, 0, sizeof(I));
    }
    void doCopyConstruct(std::byte* toBuffer, const std::byte *fromBuffer) const noexcept override{
      std::memcpy(toBuffer, fromBuffer, sizeof(I));
    }

    void doRead(std::byte* objectBuffer, read_result& result) const noexcept override{
      auto&& [last, err] = std::from_chars(result.buffer.begin(), result.buffer.end(), *(I*)objectBuffer);
      if (err) {
        result.error = std::make_error_code(err);
      }
      else {
        result.buffer = {last, result.buffer.end()};
      }
    }
    void doWrite(const std::byte* objectBuffer, write_result& result) const noexcept override{
      auto&& [last, err] = std::to_chars(result.buffer.begin(), result.buffer.end(), *(I*)objectBuffer);
      if (err) {
        result.error = std::make_error_code(err);
      }
      else {
        result.buffer = {last, result.buffer.end()};
      }
    }

    [[nodiscard]] std::string_view doTypeName() const noexcept override{
      return {detail::primitive_type_name<I>::name.data(), detail::primitive_type_name<I>::name.size()};
    }

    [[nodiscard]] size_t doSize() const noexcept override{
      return sizeof(I);
    }
    [[nodiscard]] size_t doAlignment() const noexcept final {
      return alignof(I);
    }
  };
  template <std::floating_point F>
  class floating_point_object_descriptor final : public primitive_object_descriptor{
  public:
    void doDefaultInit(std::byte* bufferView) const noexcept override{
      std::memset(bufferView, 0, sizeof(F));
    }
    void doCopyConstruct(std::byte* toBuffer, const std::byte *fromBuffer) const noexcept override{
      std::memcpy(toBuffer, fromBuffer, sizeof(F));
    }

    void doRead(std::byte* objectBuffer, read_result& result) const noexcept override{
      auto&& [last, err] = std::from_chars(result.buffer.begin(), result.buffer.end(), *(F*)objectBuffer);
      if (err) {
        result.error = std::make_error_code(err);
      }
      else {
        result.buffer = {last, result.buffer.end()};
      }
    }
    void doWrite(const std::byte* objectBuffer, write_result& result) const noexcept override{
      auto&& [last, err] = std::to_chars(result.buffer.begin(), result.buffer.end(), *(F*)objectBuffer);
      if (err) {
        result.error = std::make_error_code(err);
      }
      else {
        result.buffer = {last, result.buffer.end()};
      }
    }

    [[nodiscard]] std::string_view doTypeName() const noexcept override{
      return {detail::primitive_type_name<F>::name.data(), detail::primitive_type_name<F>::name.size()};
    }
    [[nodiscard]] size_t doSize() const noexcept override{
      return sizeof(F);
    }
    [[nodiscard]] size_t doAlignment() const noexcept final {
      return alignof(F);
    }
  };
  class string_object_descriptor final : public primitive_object_descriptor{
  public:

    using value_type = boost::static_strings::static_string<32>;

    void doDefaultInit(std::byte* bufferView) const noexcept override{
      new(bufferView) value_type();
    }
    void doCopyConstruct(std::byte* toBuffer, const std::byte *fromBuffer) const noexcept override{
      new(toBuffer) value_type(*(const value_type*)fromBuffer);
    }

    void doRead(std::byte* objectBuffer, read_result& result) const noexcept override {
      try {
        (*(value_type*)objectBuffer).assign(result.buffer.data(), result.buffer.size());
      } catch (std::length_error& exception){
        result.error = std::make_error_code(std::errc::value_too_large);
      }
    }
    void doWrite(const std::byte* objectBuffer, write_result& result) const noexcept override{
      const value_type& string = *(const value_type*)objectBuffer;
      if (result.buffer.size() > string.size()) {
        result.error = std::make_error_code(std::errc::value_too_large);
      }
      else {
        try {
          (*(value_type*)objectBuffer).assign(result.buffer.data(), result.buffer.size());
        } catch (std::length_error& exception){
          result.error = std::make_error_code(std::errc::value_too_large);
        }
      }
    }

    [[nodiscard]] std::string_view doTypeName() const noexcept override{
      return "string";
    }
    [[nodiscard]] size_t doSize() const noexcept override{
      return sizeof(value_type);
    }
    [[nodiscard]] size_t doAlignment() const noexcept final {
      return alignof(value_type);
    }
  };
  class bool_object_descriptor final : public primitive_object_descriptor{
  public:
    using value_type = bool;

    void doDefaultInit(std::byte* bufferView) const noexcept override{
      new(bufferView) value_type(false);
    }
    void doCopyConstruct(std::byte* toBuffer, const std::byte *fromBuffer) const noexcept override{
      new(toBuffer) value_type(*(const value_type*)fromBuffer);
    }

    void doRead(std::byte* objectBuffer, read_result& result) const noexcept override {
      (*(value_type*)objectBuffer) = *(bool*)result.buffer.data();
      result.buffer = result.buffer.last(result.buffer.size() - 1);
    }
    void doWrite(const std::byte* objectBuffer, write_result& result) const noexcept override{
      *(bool*)result.buffer.data() = *(const value_type*)objectBuffer;
      result.buffer = result.buffer.last(result.buffer.size() - 1);
    }

    [[nodiscard]] std::string_view doTypeName() const noexcept override{
      return {detail::primitive_type_name<bool>::name.data(), detail::primitive_type_name<bool>::name.size()};
    }
    [[nodiscard]] size_t doSize() const noexcept override{
      return sizeof(value_type);
    }
    [[nodiscard]] size_t doAlignment() const noexcept final {
      return alignof(value_type);
    }
  };
  class char_object_descriptor final : public primitive_object_descriptor{
  public:
    using value_type = char;

    void doDefaultInit(std::byte* bufferView) const noexcept override{
      new(bufferView) value_type(' ');
    }
    void doCopyConstruct(std::byte* toBuffer, const std::byte *fromBuffer) const noexcept override{
      new(toBuffer) value_type(*(const value_type*)fromBuffer);
    }

    void doRead(std::byte* objectBuffer, read_result& result) const noexcept override {
      (*(value_type*)objectBuffer) = *(char*)result.buffer.data();
      result.buffer = result.buffer.last(result.buffer.size() - 1);
    }
    void doWrite(const std::byte* objectBuffer, write_result& result) const noexcept override{
      *(char*)result.buffer.data() = *(const value_type*)objectBuffer;
      result.buffer = result.buffer.last(result.buffer.size() - 1);
    }

    [[nodiscard]] std::string_view doTypeName() const noexcept override{
      return {detail::primitive_type_name<char>::name.data(), detail::primitive_type_name<char>::name.size()};
    }
    [[nodiscard]] size_t doSize() const noexcept override{
      return sizeof(value_type);
    }
    [[nodiscard]] size_t doAlignment() const noexcept final {
      return alignof(value_type);
    }
  };

  class array_object_descriptor final : public primitive_object_descriptor{
    const object_descriptor* pDescriptor;
    boost::static_string<32> typeName;
    size_t arraySize;
  public:

    array_object_descriptor(const object_descriptor* pDescriptor, size_t arraySize) noexcept :
        pDescriptor(pDescriptor), typeName(), arraySize(arraySize){
      auto&& subTypeName = this->pDescriptor->doTypeName();
      typeName.assign(subTypeName.data(), subTypeName.size());
      typeName += '[';
      typeName += boost::static_strings::to_static_string(arraySize);
      typeName += ']';
    }

    void doDefaultInit(std::byte* bufferView) const noexcept override{
      size_t Size = pDescriptor->doSize();
      for (size_t I = 0; I < arraySize; ++I)
        pDescriptor->doDefaultInit(bufferView + (I * Size));
    }
    void doCopyConstruct(std::byte* toBuffer, const std::byte *fromBuffer) const noexcept override{
      size_t Size = pDescriptor->doSize();
      for (size_t I = 0; I < arraySize; ++I)
        pDescriptor->doCopyConstruct(toBuffer + (I * Size), fromBuffer + (I * Size));
    }

    void doRead(std::byte* objectBuffer, read_result& result) const noexcept override {

      (*(value_type*)objectBuffer) = *(char*)result.buffer.data();
      result.buffer = result.buffer.last(result.buffer.size() - 1);
    }
    void doWrite(const std::byte* objectBuffer, write_result& result) const noexcept override{
      *(char*)result.buffer.data() = *(const value_type*)objectBuffer;
      result.buffer = result.buffer.last(result.buffer.size() - 1);
    }

    [[nodiscard]] std::string_view doTypeName() const noexcept override{
      return {typeName.data(), typeName.size()};
    }
    [[nodiscard]] size_t doSize() const noexcept override{
      return pDescriptor->doSize() * arraySize;
    }
    [[nodiscard]] size_t doAlignment() const noexcept final {
      return pDescriptor->doAlignment();
    }
  };

  class subobject_descriptor final : public object_descriptor{
    const object_descriptor* pDescriptor;
    boost::static_string<32> memberName;
    size_t offsetFromParent;

  public:
    subobject_descriptor(const object_descriptor* pDescriptor, std::string_view name, size_t offset) noexcept :
        pDescriptor(pDescriptor), memberName(name.data(), name.size()), offsetFromParent(offset){}

    void doDefaultInit(std::byte* bufferView) const noexcept final {
      pDescriptor->doDefaultInit(bufferView + offsetFromParent);
    }
    void doCopyConstruct(std::byte* toBuffer, const std::byte* fromBuffer) const noexcept final {
      pDescriptor->doCopyConstruct(toBuffer + offsetFromParent, fromBuffer + offsetFromParent);
    }

    [[nodiscard]] std::span<const object_descriptor * const> doSubobjectDescriptors() const noexcept final {
      return pDescriptor->doSubobjectDescriptors();
    }
    [[nodiscard]] std::string_view doTypeName() const noexcept final {
      return pDescriptor->doTypeName();
    }
    void doWrite(const std::byte* objectBuffer, write_result& result) const noexcept final {
      pDescriptor->doWrite(objectBuffer + offsetFromParent, result);
    }
    void doRead(std::byte* objectBuffer, read_result& result) const noexcept final {
      pDescriptor->doRead(objectBuffer + offsetFromParent, result);
    }
    [[nodiscard]] bool   doIsPrimitive() const noexcept final {
      return pDescriptor->doIsPrimitive();
    }
    [[nodiscard]] size_t doSize() const noexcept final {
      return pDescriptor->doSize();
    }
    [[nodiscard]] size_t doAlignment() const noexcept override{
      return pDescriptor->doAlignment();
    }

    [[nodiscard]] std::string_view getName() const noexcept{
      return {memberName.data(), memberName.size()};
    }
    void setName(std::string_view newName) noexcept {
      memberName.assign(newName.data(), newName.size());
    }

    [[nodiscard]] size_t getOffset() const noexcept {
      return offsetFromParent;
    }
  };

  class compound_object_descriptor final : public object_descriptor{
    boost::container::small_vector<subobject_descriptor, 8> memberObjects;
    boost::container::small_vector<const subobject_descriptor*, 8> memberObjectPtrVector;
    size_t Size;
    size_t Alignment;

    inline void alignTo(size_t alignment) noexcept {
      Size += static_cast<bool>(Size % alignment) * ((~Size & (alignment - 1)) + 1);
      assert(!(Size % alignment));
    }
  public:

    compound_object_descriptor(std::initializer_list<std::pair<const object_descriptor*, std::string_view>> list) noexcept :
        memberObjects{}, memberObjectPtrVector{}, Size(0), Alignment(1){
      memberObjects.reserve(list.size());
      memberObjectPtrVector.reserve(list.size());
      for (auto&& [obj_ptr, name] : list) {
        size_t memberAlignment = obj_ptr->doAlignment();
        alignTo(memberAlignment);
        auto& result = memberObjects.emplace_back(obj_ptr, name, Size);
        memberObjectPtrVector.push_back(&result);
        Alignment = std::max(Alignment, memberAlignment);
        Size += obj_ptr->doSize();
      }
      alignTo(Alignment);
    }


    void doDefaultInit(std::byte* bufferView) const noexcept final {
      for (const auto& subobject : memberObjects) {
        subobject.doDefaultInit(bufferView);
      }
    }
    void doCopyConstruct(std::byte* toBuffer, const std::byte* fromBuffer) const noexcept final {
      for (const auto& subobject : memberObjects) {
        subobject.doCopyConstruct(toBuffer, fromBuffer);
      }
    }

    void doWrite(const std::byte *objectBuffer, write_result &result) const noexcept override{
      for (const auto& subobject : memberObjects) {
        if (result.error)
          break;
        subobject.doWrite(objectBuffer, result);
      }
    }
    void doRead(std::byte *objectBuffer, read_result &result) const noexcept override{
      for (const auto& subobject : memberObjects) {
        if (result.error)
          break;
        subobject.doRead(objectBuffer, result);
      }
    }

    [[nodiscard]] std::span<const object_descriptor *const> doSubobjectDescriptors() const noexcept final {
      return {(const object_descriptor * const *)memberObjectPtrVector.data(), memberObjectPtrVector.size()};
    }

    [[nodiscard]] bool   doIsPrimitive() const noexcept final {
      return false;
    }
    [[nodiscard]] size_t doSize() const noexcept final {
      return Size;
    }
    [[nodiscard]] size_t doAlignment() const noexcept final{
      return Alignment;
    }
  };



  class ObjectType;
  class ObjectPrototype;
  class ObjectReference{
    const ObjectType* pObjectType;
    std::span<std::byte> rawBytes;
  };
  class ObjectIterable{
    ObjectReference* pObjectReference;
  };

  class ObjectType{
  public:
    virtual size_t totalSize() const noexcept = 0;

  };



  class ObjectHeader{

  };

  class ObjectPrototype{};


  class Json{};

  class Serializer;
  class PropertyReference;

  class PropertyType{
  public:
    virtual std::error_code read(PropertyReference& property, std::span<const char> value) const noexcept = 0;
    virtual std::error_code write(const PropertyReference& property, std::span<char> buffer) const noexcept = 0;
  };

  class PropertyReference{
    const PropertyType* pPropertyType;
    void* pProperty;
  };*/
  /*class PropertyReference{
  public:
    template <detail::string_like S>
    PropertyReference& operator=(const S& stringLike) noexcept{

    }
  };*/
}
#define PP_VK_impl_EXPAND_DISPATCH_first_concat_redirect_2(x, y) x##y
#define PP_VK_impl_EXPAND_DISPATCH_first_concat_redirect PP_VK_impl_EXPAND_DISPATCH_first_concat_redirect_2
#define PP_VK_impl_EXPAND_DISPATCH_first_concat(x, y) PP_VK_impl_EXPAND_DISPATCH_first_concat_redirect(x, y)

#define PP_VK_impl_EXPAND_DISPATCH_0_concat_redirect_2(x, y) x##y
#define PP_VK_impl_EXPAND_DISPATCH_0_concat_redirect PP_VK_impl_EXPAND_DISPATCH_0_concat_redirect_2
#define PP_VK_impl_EXPAND_DISPATCH_0_concat(x, y) PP_VK_impl_EXPAND_DISPATCH_0_concat_redirect(x, y)

#define PP_VK_impl_EXPAND_DISPATCH_1_concat_redirect_2(x, y) x##y
#define PP_VK_impl_EXPAND_DISPATCH_1_concat_redirect PP_VK_impl_EXPAND_DISPATCH_1_concat_redirect_2
#define PP_VK_impl_EXPAND_DISPATCH_1_concat(x, y) PP_VK_impl_EXPAND_DISPATCH_1_concat_redirect(x, y)

#define PP_VK_impl_EXPAND_DISPATCH_2_concat_redirect_2(x, y) x##y
#define PP_VK_impl_EXPAND_DISPATCH_2_concat_redirect PP_VK_impl_EXPAND_DISPATCH_2_concat_redirect_2
#define PP_VK_impl_EXPAND_DISPATCH_2_concat(x, y) PP_VK_impl_EXPAND_DISPATCH_2_concat_redirect(x, y)

#define PP_VK_impl_EXPAND_DISPATCH_3_concat_redirect_2(x, y) x##y
#define PP_VK_impl_EXPAND_DISPATCH_3_concat_redirect PP_VK_impl_EXPAND_DISPATCH_3_concat_redirect_2
#define PP_VK_impl_EXPAND_DISPATCH_3_concat(x, y) PP_VK_impl_EXPAND_DISPATCH_3_concat_redirect(x, y)

#define PP_VK_impl_EXPAND_DISPATCH_4_concat_redirect_2(x, y) x##y
#define PP_VK_impl_EXPAND_DISPATCH_4_concat_redirect PP_VK_impl_EXPAND_DISPATCH_4_concat_redirect_2
#define PP_VK_impl_EXPAND_DISPATCH_4_concat(x, y) PP_VK_impl_EXPAND_DISPATCH_4_concat_redirect(x, y)

#define PP_VK_impl_EXPAND_DISPATCH_5_concat_redirect_2(x, y) x##y
#define PP_VK_impl_EXPAND_DISPATCH_5_concat_redirect PP_VK_impl_EXPAND_DISPATCH_5_concat_redirect_2
#define PP_VK_impl_EXPAND_DISPATCH_5_concat(x, y) PP_VK_impl_EXPAND_DISPATCH_5_concat_redirect(x, y)



#define PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_DISPATCH_first_concat(PP_VK_impl_EXPAND_first_, VK_type_##x) )
#define PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_DISPATCH_0_concat(PP_VK_impl_EXPAND_0_, VK_type_##x) )
#define PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_DISPATCH_1_concat(PP_VK_impl_EXPAND_1_, VK_type_##x) )
#define PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_DISPATCH_2_concat(PP_VK_impl_EXPAND_2_, VK_type_##x) )
#define PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_DISPATCH_3_concat(PP_VK_impl_EXPAND_3_, VK_type_##x) )
#define PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_DISPATCH_4_concat(PP_VK_impl_EXPAND_4_, VK_type_##x) )
#define PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_DISPATCH_5_concat(PP_VK_impl_EXPAND_5_, VK_type_##x) )


#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_20(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_19(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_19(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_18(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_18(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_17(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_17(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_16(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_16(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_15(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_15(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_14(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_14(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_13(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_13(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_12(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_12(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_11(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_11(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_10(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_10(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_9(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_9(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_8(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_8(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_7(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_7(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_6(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_6(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_5(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_5(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_4(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_4(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_3(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_3(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_2(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_2(x, ...) PP_VK_impl_EXPAND_DISPATCH_first(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_first_1(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_1(x) PP_VK_impl_EXPAND_DISPATCH_first(x)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_first_0()


#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_20(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_19(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_19(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_18(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_18(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_17(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_17(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_16(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_16(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_15(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_15(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_14(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_14(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_13(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_13(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_12(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_12(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_11(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_11(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_10(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_10(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_9(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_9(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_8(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_8(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_7(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_7(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_6(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_6(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_5(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_5(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_4(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_4(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_3(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_3(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_2(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_2(x, ...) PP_VK_impl_EXPAND_DISPATCH_0(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_0_1(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_1(x) PP_VK_impl_EXPAND_DISPATCH_0(x)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_0_0()

#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_20(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_19(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_19(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_18(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_18(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_17(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_17(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_16(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_16(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_15(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_15(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_14(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_14(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_13(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_13(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_12(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_12(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_11(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_11(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_10(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_10(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_9(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_9(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_8(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_8(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_7(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_7(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_6(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_6(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_5(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_5(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_4(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_4(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_3(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_3(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_2(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_2(x, ...) PP_VK_impl_EXPAND_DISPATCH_1(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_1_1(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_1(x) PP_VK_impl_EXPAND_DISPATCH_1(x)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_1_0()

#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_20(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_19(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_19(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_18(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_18(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_17(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_17(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_16(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_16(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_15(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_15(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_14(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_14(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_13(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_13(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_12(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_12(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_11(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_11(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_10(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_10(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_9(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_9(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_8(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_8(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_7(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_7(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_6(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_6(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_5(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_5(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_4(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_4(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_3(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_3(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_2(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_2(x, ...) PP_VK_impl_EXPAND_DISPATCH_2(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_2_1(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_1(x) PP_VK_impl_EXPAND_DISPATCH_2(x)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_2_0()

#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_20(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_19(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_19(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_18(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_18(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_17(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_17(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_16(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_16(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_15(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_15(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_14(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_14(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_13(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_13(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_12(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_12(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_11(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_11(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_10(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_10(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_9(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_9(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_8(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_8(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_7(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_7(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_6(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_6(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_5(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_5(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_4(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_4(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_3(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_3(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_2(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_2(x, ...) PP_VK_impl_EXPAND_DISPATCH_3(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_3_1(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_1(x) PP_VK_impl_EXPAND_DISPATCH_3(x)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_3_0()

#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_20(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_19(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_19(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_18(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_18(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_17(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_17(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_16(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_16(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_15(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_15(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_14(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_14(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_13(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_13(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_12(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_12(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_11(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_11(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_10(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_10(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_9(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_9(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_8(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_8(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_7(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_7(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_6(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_6(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_5(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_5(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_4(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_4(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_3(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_3(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_2(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_2(x, ...) PP_VK_impl_EXPAND_DISPATCH_4(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_4_1(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_1(x) PP_VK_impl_EXPAND_DISPATCH_4(x)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_4_0()

#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_20(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_19(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_19(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_18(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_18(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_17(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_17(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_16(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_16(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_15(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_15(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_14(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_14(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_13(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_13(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_12(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_12(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_11(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_11(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_10(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_10(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_9(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_9(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_8(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_8(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_7(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_7(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_6(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_6(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_5(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_5(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_4(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_4(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_3(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_3(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_2(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_2(x, ...) PP_VK_impl_EXPAND_DISPATCH_5(x) PP_VK_impl_EXPAND_EACH_IN_GROUP_5_1(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_1(x) PP_VK_impl_EXPAND_DISPATCH_5(x)
#define PP_VK_impl_EXPAND_EACH_IN_GROUP_5_0()

#define PP_VK_impl_EXPAND_EACH_first_concat_redirect_concat_redirect(x, y) x##y
#define PP_VK_impl_EXPAND_EACH_first_concat_redirect_concat PP_VK_impl_EXPAND_EACH_first_concat_redirect_concat_redirect
#define PP_VK_impl_EXPAND_EACH_first_concat_redirect(suffix_, ...) PP_VK_impl_EXPAND_EACH_first_concat_redirect_concat(PP_VK_impl_EXPAND_EACH_IN_GROUP_first, suffix_)(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_first_concat PP_VK_impl_EXPAND_EACH_first_concat_redirect
#define PP_VK_impl_EXPAND_EACH_first(...) PP_VK_impl_EXPAND_EACH_first_concat(VK_count_args(__VA_ARGS__), ##__VA_ARGS__)


#define PP_VK_impl_EXPAND_EACH_0_concat_redirect_concat_redirect(x, y) x##y
#define PP_VK_impl_EXPAND_EACH_0_concat_redirect_concat PP_VK_impl_EXPAND_EACH_0_concat_redirect_concat_redirect
#define PP_VK_impl_EXPAND_EACH_0_concat_redirect(suffix_, ...) PP_VK_impl_EXPAND_EACH_0_concat_redirect_concat(PP_VK_impl_EXPAND_EACH_IN_GROUP_0, suffix_)(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_0_concat PP_VK_impl_EXPAND_EACH_0_concat_redirect
#define PP_VK_impl_EXPAND_EACH_0(...) PP_VK_impl_EXPAND_EACH_0_concat(VK_count_args(__VA_ARGS__), ##__VA_ARGS__)

#define PP_VK_impl_EXPAND_EACH_1_concat_redirect_concat_redirect(x, y) x##y
#define PP_VK_impl_EXPAND_EACH_1_concat_redirect_concat PP_VK_impl_EXPAND_EACH_1_concat_redirect_concat_redirect
#define PP_VK_impl_EXPAND_EACH_1_concat_redirect(suffix_, ...) PP_VK_impl_EXPAND_EACH_1_concat_redirect_concat(PP_VK_impl_EXPAND_EACH_IN_GROUP_1, suffix_)(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_1_concat PP_VK_impl_EXPAND_EACH_1_concat_redirect
#define PP_VK_impl_EXPAND_EACH_1(...) PP_VK_impl_EXPAND_EACH_1_concat(VK_count_args(__VA_ARGS__), ##__VA_ARGS__)

#define PP_VK_impl_EXPAND_EACH_2_concat_redirect_concat_redirect(x, y) x##y
#define PP_VK_impl_EXPAND_EACH_2_concat_redirect_concat PP_VK_impl_EXPAND_EACH_2_concat_redirect_concat_redirect
#define PP_VK_impl_EXPAND_EACH_2_concat_redirect(suffix_, ...) PP_VK_impl_EXPAND_EACH_2_concat_redirect_concat(PP_VK_impl_EXPAND_EACH_IN_GROUP_2, suffix_)(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_2_concat PP_VK_impl_EXPAND_EACH_2_concat_redirect
#define PP_VK_impl_EXPAND_EACH_2(...) PP_VK_impl_EXPAND_EACH_2_concat(VK_count_args(__VA_ARGS__), ##__VA_ARGS__)

#define PP_VK_impl_EXPAND_EACH_3_concat_redirect_concat_redirect(x, y) x##y
#define PP_VK_impl_EXPAND_EACH_3_concat_redirect_concat PP_VK_impl_EXPAND_EACH_3_concat_redirect_concat_redirect
#define PP_VK_impl_EXPAND_EACH_3_concat_redirect(suffix_, ...) PP_VK_impl_EXPAND_EACH_3_concat_redirect_concat(PP_VK_impl_EXPAND_EACH_IN_GROUP_3, suffix_)(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_3_concat PP_VK_impl_EXPAND_EACH_3_concat_redirect
#define PP_VK_impl_EXPAND_EACH_3(...) PP_VK_impl_EXPAND_EACH_3_concat(VK_count_args(__VA_ARGS__), ##__VA_ARGS__)

#define PP_VK_impl_EXPAND_EACH_4_concat_redirect_concat_redirect(x, y) x##y
#define PP_VK_impl_EXPAND_EACH_4_concat_redirect_concat PP_VK_impl_EXPAND_EACH_4_concat_redirect_concat_redirect
#define PP_VK_impl_EXPAND_EACH_4_concat_redirect(suffix_, ...) PP_VK_impl_EXPAND_EACH_4_concat_redirect_concat(PP_VK_impl_EXPAND_EACH_IN_GROUP_4, suffix_)(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_4_concat PP_VK_impl_EXPAND_EACH_4_concat_redirect
#define PP_VK_impl_EXPAND_EACH_4(...) PP_VK_impl_EXPAND_EACH_4_concat(VK_count_args(__VA_ARGS__), ##__VA_ARGS__)

#define PP_VK_impl_EXPAND_EACH_5_concat_redirect_concat_redirect(x, y) x##y
#define PP_VK_impl_EXPAND_EACH_5_concat_redirect_concat PP_VK_impl_EXPAND_EACH_5_concat_redirect_concat_redirect
#define PP_VK_impl_EXPAND_EACH_5_concat_redirect(suffix_, ...) PP_VK_impl_EXPAND_EACH_5_concat_redirect_concat(PP_VK_impl_EXPAND_EACH_IN_GROUP_5, suffix_)(__VA_ARGS__)
#define PP_VK_impl_EXPAND_EACH_5_concat PP_VK_impl_EXPAND_EACH_5_concat_redirect
#define PP_VK_impl_EXPAND_EACH_5(...) PP_VK_impl_EXPAND_EACH_5_concat(VK_count_args(__VA_ARGS__), ##__VA_ARGS__)

#define PP_VK_impl_PROPERTY_LIST(...) inline static constexpr ::Vk::detail::component_set_dropped_end_t<VK_for_each(PP_VK_impl_MAKE_PROPERTY_GROUP_FIELD_LIST, ##__VA_ARGS__) ::Vk::detail::discardThisValue> properties;

#define PP_VK_impl_EXPAND_PROPERTY(type_, name_) private: struct name_##PROPERTY : ::Vk::FeaturePropertyValue<property_group_t, name_##PROPERTY, VK_unwrap(type_)>{ \
    VK_decl_member_name(name_);                                                                                                                                                              \
    using ::Vk::FeaturePropertyValue<property_group_t, name_##PROPERTY, VK_unwrap(type_)>::operator=;\
  }; public: name_##PROPERTY name_;


#define PP_VK_impl_EXPAND_first_property(type_, name_) PP_VK_impl_EXPAND_PROPERTY(type_, name_)
#define PP_VK_impl_EXPAND_first_feature(body_, name_) private: struct name_##FEATURE : ::Vk::Feature<name_##FEATURE>{ \
  VK_decl_member_name(name_);                                                                                   \
  private:                                                                                                      \
  using property_group_t = name_##FEATURE;                                                                      \
  public:                                                                                                       \
  using ::Vk::Feature<name_##FEATURE>::operator=;                                                               \
  PP_VK_impl_EXPAND_EACH_0 body_                                                                                \
  PP_VK_impl_PROPERTY_LIST body_                                                                                \
  }; public: name_##FEATURE name_;
#define PP_VK_impl_EXPAND_first_group(body_, name_) private: struct name_##PROPERTY_GROUP : ::Vk::FeaturePropertyGroup<property_group_t, name_##PROPERTY_GROUP>{ \
    VK_decl_member_name(name_);                                                                                                                            \
  private:                                                                                                                                                 \
    using property_group_t = name_##PROPERTY_GROUP;                                                                                                        \
  public:                                                                                                                                                  \
    PP_VK_impl_EXPAND_EACH_0 body_                                                                                                                  \
    PP_VK_impl_PROPERTY_LIST body_                                                                                                                  \
  }; public: name_##PROPERTY_GROUP name_;



#define PP_VK_impl_EXPAND_0_property(type_, name_) PP_VK_impl_EXPAND_PROPERTY(type_, name_)
#define PP_VK_impl_EXPAND_0_feature(body_, name_) private: struct name_##FEATURE : ::Vk::Feature<name_##FEATURE>{ \
  VK_decl_member_name(name_);                                                                                   \
  private:                                                                                                      \
  using property_group_t = name_##FEATURE;                                                                      \
  public:                                                                                                       \
  using ::Vk::Feature<name_##FEATURE>::operator=;                                                               \
  PP_VK_impl_EXPAND_EACH_1 body_                                                                                                                  \
    PP_VK_impl_PROPERTY_LIST body_                                                                         \
  }; public: name_##FEATURE name_;
#define PP_VK_impl_EXPAND_0_group(body_, name_) private: struct name_##PROPERTY_GROUP : ::Vk::FeaturePropertyGroup<property_group_t, name_##PROPERTY_GROUP>{ \
    VK_decl_member_name(name_);                                                                                                                            \
  private:                                                                                                                                                 \
    using property_group_t = name_##PROPERTY_GROUP;                                                                                                        \
  public:                                                                                                                                                  \
    PP_VK_impl_EXPAND_EACH_1 body_                                                                                                                  \
    PP_VK_impl_PROPERTY_LIST body_                                                                                                                  \
  }; public: name_##PROPERTY_GROUP name_;

#define PP_VK_impl_EXPAND_1_property(type_, name_) PP_VK_impl_EXPAND_PROPERTY(type_, name_)
#define PP_VK_impl_EXPAND_1_feature(body_, name_) private: struct name_##FEATURE : ::Vk::Feature<name_##FEATURE>{ \
  VK_decl_member_name(name_);                                                                                   \
  private:                                                                                                      \
  using property_group_t = name_##FEATURE;                                                                      \
  public:                                                                                                       \
  using ::Vk::Feature<name_##FEATURE>::operator=;                                                               \
  PP_VK_impl_EXPAND_EACH_2 body_                                                                                                                  \
    PP_VK_impl_PROPERTY_LIST body_                                                                         \
  }; public: name_##FEATURE name_;
#define PP_VK_impl_EXPAND_1_group(body_, name_) private: struct name_##PROPERTY_GROUP : ::Vk::FeaturePropertyGroup<property_group_t, name_##PROPERTY_GROUP>{ \
    VK_decl_member_name(name_);                                                                                                                            \
  private:                                                                                                                                                 \
    using property_group_t = name_##PROPERTY_GROUP;                                                                                                        \
  public:                                                                                                                                                  \
    PP_VK_impl_EXPAND_EACH_2 body_                                                                                                                  \
    PP_VK_impl_PROPERTY_LIST body_                                                                                                                 \
  }; public: name_##PROPERTY_GROUP name_;

#define PP_VK_impl_EXPAND_2_property(type_, name_) PP_VK_impl_EXPAND_PROPERTY(type_, name_)
#define PP_VK_impl_EXPAND_2_feature(body_, name_) private: struct name_##FEATURE : ::Vk::Feature<name_##FEATURE>{ \
  VK_decl_member_name(name_);                                                                                   \
  private:                                                                                                      \
  using property_group_t = name_##FEATURE;                                                                      \
  public:                                                                                                       \
  using ::Vk::Feature<name_##FEATURE>::operator=;                                                               \
  PP_VK_impl_EXPAND_EACH_3 body_                                                                                                                  \
    PP_VK_impl_PROPERTY_LIST body_                                                                        \
  }; public: name_##FEATURE name_;
#define PP_VK_impl_EXPAND_2_group(body_, name_) private: struct name_##PROPERTY_GROUP : ::Vk::FeaturePropertyGroup<property_group_t, name_##PROPERTY_GROUP>{ \
    VK_decl_member_name(name_);                                                                                                                            \
  private:                                                                                                                                                 \
    using property_group_t = name_##PROPERTY_GROUP;                                                                                                        \
  public:                                                                                                                                                  \
    PP_VK_impl_EXPAND_EACH_3 body_                                                                                                                  \
    PP_VK_impl_PROPERTY_LIST body_                                                                                                                \
  }; public: name_##PROPERTY_GROUP name_;

#define PP_VK_impl_EXPAND_3_property(type_, name_) PP_VK_impl_EXPAND_PROPERTY(type_, name_)
#define PP_VK_impl_EXPAND_3_feature(body_, name_) private: struct name_##FEATURE : ::Vk::Feature<name_##FEATURE>{ \
  VK_decl_member_name(name_);                                                                                   \
  private:                                                                                                      \
  using property_group_t = name_##FEATURE;                                                                      \
  public:                                                                                                       \
  using ::Vk::Feature<name_##FEATURE>::operator=;                                                               \
  PP_VK_impl_EXPAND_EACH_4 body_                                                                                                                  \
    PP_VK_impl_PROPERTY_LIST body_                                                                       \
  }; public: name_##FEATURE name_;
#define PP_VK_impl_EXPAND_3_group(body_, name_) private: struct name_##PROPERTY_GROUP : ::Vk::FeaturePropertyGroup<property_group_t, name_##PROPERTY_GROUP>{ \
    VK_decl_member_name(name_);                                                                                                                            \
  private:                                                                                                                                                 \
    using property_group_t = name_##PROPERTY_GROUP;                                                                                                        \
  public:                                                                                                                                                  \
    PP_VK_impl_EXPAND_EACH_4 body_                                                                                                                  \
    PP_VK_impl_PROPERTY_LIST body_                                                                                                                 \
  }; public: name_##PROPERTY_GROUP name_;

#define PP_VK_impl_EXPAND_4_property(type_, name_) PP_VK_impl_EXPAND_PROPERTY(type_, name_)
#define PP_VK_impl_EXPAND_4_feature(body_, name_) private: struct name_##FEATURE : ::Vk::Feature<name_##FEATURE>{ \
  VK_decl_member_name(name_);                                                                                   \
  private:                                                                                                      \
  using property_group_t = name_##FEATURE;                                                                      \
  public:                                                                                                       \
  using ::Vk::Feature<name_##FEATURE>::operator=;                                                               \
  PP_VK_impl_EXPAND_EACH_5 body_                                                                                                                  \
    PP_VK_impl_PROPERTY_LIST body_                                                                         \
  }; public: name_##FEATURE name_;
#define PP_VK_impl_EXPAND_4_group(body_, name_) private: struct name_##PROPERTY_GROUP : ::Vk::FeaturePropertyGroup<property_group_t, name_##PROPERTY_GROUP>{ \
    VK_decl_member_name(name_);                                                                                                                            \
  private:                                                                                                                                                 \
    using property_group_t = name_##PROPERTY_GROUP;                                                                                                        \
  public:                                                                                                                                                  \
    PP_VK_impl_EXPAND_EACH_5 body_                                                                                                                  \
    PP_VK_impl_PROPERTY_LIST body_                                                                                                                  \
  }; public: name_##PROPERTY_GROUP name_;

#define PP_VK_impl_EXPAND_5_property(type_, value_) PP_VK_impl_EXPAND_PROPERTY(VK_type_##value_)
/*#define PP_VK_impl_EXPAND_5_feature(name_, ...) private: struct name_##FEATURE : ::Vk::Feature<name_##FEATURE>{ \
  VK_decl_member_name(name_);                                                                                   \
  private:                                                                                                      \
  using property_group_t = name_##FEATURE;                                                                      \
  public:                                                                                                       \
  using ::Vk::Feature<name_##FEATURE>::operator=;                                                               \
  PP_VK_impl_EXPAND_EACH_5(__VA_ARGS__)                                                                         \
  PP_VK_impl_PROPERTY_LIST(__VA_ARGS__)                                                                         \
  }; public: name_##FEATURE name_;
#define PP_VK_impl_EXPAND_5_group(name_, ...) private: struct name_##PROPERTY_GROUP : ::Vk::FeaturePropertyGroup<property_group_t, name_##PROPERTY_GROUP>{ \
    VK_decl_member_name(name_);                                                                                                                            \
  private:                                                                                                                                                 \
    using property_group_t = name_##PROPERTY_GROUP;                                                                                                        \
  public:                                                                                                                                                  \
    PP_VK_impl_EXPAND_EACH_5(__VA_ARGS__)                                                                                                                  \
    PP_VK_impl_PROPERTY_LIST(__VA_ARGS__)                                                                                                                  \
  }; public: name_##PROPERTY_GROUP name_;*/

#define VK_property_group(name_, ...) private: struct name_##PROPERTY_GROUP : ::Vk::PropertyGroup<name_##PROPERTY_GROUP>{ \
    VK_decl_member_name(name_);                                                                                                                        \
    private:                                                                                                                      \
    using property_group_t = name_##PROPERTY_GROUP;                                                                                                                        \
    public:                                                                                                                      \
    PP_VK_impl_EXPAND_EACH_first(__VA_ARGS__)                                                                             \
    PP_VK_impl_PROPERTY_LIST(__VA_ARGS__)\
  }; public: name_##PROPERTY_GROUP name_


/*#define PP_VK_impl_DEFINE_PROPERTY_GROUP_BODY_redirect(...) __VA_ARGS__
#define PP_VK_impl_DEFINE_PROPERTY_GROUP_BODY PP_VK_impl_DEFINE_PROPERTY_GROUP_BODY_redirect

#define PP_VK_impl_DEFINE_FEATURE_BODY_redirect(...) __VA_ARGS__
#define PP_VK_impl_DEFINE_FEATURE_BODY PP_VK_impl_DEFINE_FEATURE_BODY_redirect

#define PP_VK_impl_DEFINE_PROPERTY_GROUP_redirect(name_, ...) private: struct name_##PROPERTY_GROUP : ::Vk::PropertyGroup<name_##PROPERTY_GROUP>{ \
    VK_decl_member_name(name_);                                                                                                   \
    using property_group_t = name_##PROPERTY_GROUP;                                                                                                                              \
    PP_VK_impl_DEFINE_PROPERTY_GROUP_BODY __VA_ARGS__ ) }; public: name_##PROPERTY_GROUP name_

#define PP_VK_impl_DEFINE_FEATURE_redirect(name_, ...) private: struct name_##FEATURE : ::Vk::Feature<name_##FEATURE>{ \
  VK_decl_member_name(name_);                                                                                          \
  using property_group_t = name_##FEATURE;                                                                                                                     \
  using ::Vk::Feature<name_##FEATURE>::operator=;\
  PP_VK_impl_DEFINE_FEATURE_BODY __VA_ARGS__ ) \
  }; public: name_##FEATURE name_*/

/*
#define PP_VK_impl_TAKE_ADDRESS_OF_PROPERTY_redirect_2(type_, name_) &property_group_t::name_,
#define PP_VK_impl_TAKE_ADDRESS_OF_PROPERTY_redirect PP_VK_impl_TAKE_ADDRESS_OF_PROPERTY_redirect_2
#define PP_VK_impl_TAKE_ADDRESS_OF_PROPERTY(value_) PP_VK_impl_TAKE_ADDRESS_OF_PROPERTY_redirect(value_)
#define PP_VK_impl_TAKE_ADDRESS_property(value_) PP_VK_impl_TAKE_ADDRESS_OF_PROPERTY(VK_type_##value_)
#define PP_VK_impl_TAKE_ADDRESS_group(name_, ...) &property_group_t::name_,
#define PP_VK_impl_TAKE_ADDRESS_feature(name_, ...) &property_group_t::name_,

#define PP_VK_impl_DEFINE_PROPERTY_GROUP PP_VK_impl_DEFINE_PROPERTY_GROUP_redirect
#define PP_VK_impl_DEFINE_FEATURE_concat(a, b) a##b
#define PP_VK_impl_DEFINE_FEATURE PP_VK_impl_DEFINE_FEATURE_concat(PP_VK_impl_DEFINE_, FEATURE_redirect)
*/

/*#define VK_property_group(name_, ...) PP_VK_impl_DEFINE_PROPERTY_GROUP ( name_,
#define VK_define_feature(name_) PP_VK_impl_DEFINE_FEATURE ( name_,*/

/*#define PP_VK_impl_TAKE_ADDRESS_redirect(value_) PP_VK_impl_TAKE_ADDRESS_##value_
#define PP_VK_impl_TAKE_ADDRESS PP_VK_impl_TAKE_ADDRESS_redirect*/

//#define PP_VK_impl_TAKE_ADDRESS(value_) PP_VK_impl_TAKE_ADDRESS_##value_



#define PP_VK_impl_DEFINE_PROPERTY_WITH_TYPE_MACRO_redirect_2(type_, name_) private: struct name_##PROPERTY : ::Vk::FeaturePropertyValue<property_group_t, name_##PROPERTY, VK_unwrap(type_)>{ \
    VK_decl_member_name(name_);                                                                                                                                                              \
    using ::Vk::FeaturePropertyValue<property_group_t, name_##PROPERTY, VK_unwrap(type_)>::operator=;\
  }; public: name_##PROPERTY name_
#define PP_VK_impl_DEFINE_PROPERTY_WITH_TYPE_MACRO_redirect PP_VK_impl_DEFINE_PROPERTY_WITH_TYPE_MACRO_redirect_2
#define PP_VK_impl_DEFINE_PROPERTY_WITH_TYPE_MACRO(value_) VK_concat_inner(PP_VK_impl_DEFINE_PROPERTY_WITH_, TYPE_MACRO_redirect)(value_)
#define PP_VK_impl_DEFINE_PROPERTY_WITH_type(...) PP_VK_impl_DEFINE_PROPERTY_WITH_TYPE_MACRO ( (__VA_ARGS__) ,


#define VK_type_group(...) group( (__VA_ARGS__) ,
#define VK_type_feature(...) feature( (__VA_ARGS__) ,
#define VK_type_type(...) property( (__VA_ARGS__) ,
#define VK_typedef(...) VK_type_type(__VA_ARGS__)


#define PP_VK_impl_PROPERTY_DISPATCH(macro_, val_) VK_concat(macro_, VK_type_##val_) )

#define PP_VK_impl_MAKE_PROPERTY_GROUP_FIELD_LIST_redirect(val_) PP_VK_impl_PROPERTY_DISPATCH(PP_VK_impl_TAKE_MEMBER_ADDRESS_, val_)
#define PP_VK_impl_MAKE_PROPERTY_GROUP_FIELD_LIST PP_VK_impl_MAKE_PROPERTY_GROUP_FIELD_LIST_redirect
#define PP_VK_impl_TAKE_MEMBER_ADDRESS_redirect(name_) &property_group_t::name_,
#define PP_VK_impl_TAKE_MEMBER_ADDRESS_group(body_, name_) PP_VK_impl_TAKE_MEMBER_ADDRESS_redirect(name_)
#define PP_VK_impl_TAKE_MEMBER_ADDRESS_feature(body_, name_) PP_VK_impl_TAKE_MEMBER_ADDRESS_redirect(name_)
#define PP_VK_impl_TAKE_MEMBER_ADDRESS_property(body_, name_) PP_VK_impl_TAKE_MEMBER_ADDRESS_redirect(name_)



#define VK_decl_member_name(name_) inline static constexpr ::Vk::StringLiteral property_field_name{#name_}



#define PP_VK_impl_PROPERTY_GROUP_MEMBER_bool_redirect_2(name_) private: struct name_##PROPERTY : ::Vk::FeaturePropertyValue<feature_t, name_##PROPERTY, bool>{ \
    inline constexpr static Vk::StringLiteral name{#name_};                                                                                                \
    using ::Vk::FeaturePropertyValue<feature_t, name_##PROPERTY, bool>::operator=;                                                                         \
  }; public: name_##PROPERTY name_
#define PP_VK_impl_PROPERTY_GROUP_MEMBER_bool_redirect PP_VK_impl_PROPERTY_GROUP_MEMBER_bool_redirect_2
#define PP_VK_impl_PROPERTY_GROUP_MEMBER_bool PP_VK_impl_PROPERTY_GROUP_MEMBER_bool_redirect (
#define PP_VK_impl_PROPERTY_GROUP_MEMBER_string_redirect_2(name_) private: struct name_##PROPERTY : ::Vk::FeaturePropertyValue<feature_t, name_##PROPERTY, Vk::String>{ \
    using ::Vk::FeaturePropertyValue<feature_t, name_##PROPERTY, Vk::String>::operator=;                                                                   \
    inline constexpr static Vk::StringLiteral name{#name_};                                                                                                \
  }; public: name_##PROPERTY name_
#define PP_VK_impl_PROPERTY_GROUP_MEMBER_string_redirect PP_VK_impl_PROPERTY_GROUP_MEMBER_string_redirect_2
#define PP_VK_impl_PROPERTY_GROUP_MEMBER_string PP_VK_impl_PROPERTY_GROUP_MEMBER_string_redirect (

#define PP_VK_impl_PROPERTY_GROUP_MEMBER_type_redirect_2(type_, name_) private: struct name_##PROPERTY : ::Vk::FeaturePropertyValue<feature_t, name_##PROPERTY, VK_unwrap(type_)>{ \
    inline constexpr static Vk::StringLiteral name{#name_};                                                                                                             \
    using ::Vk::FeaturePropertyValue<feature_t, name_##PROPERTY, VK_unwrap(type_)>::operator=;\
  }; public: name_##PROPERTY name_
#define PP_VK_impl_PROPERTY_GROUP_MEMBER_type_redirect PP_VK_impl_PROPERTY_GROUP_MEMBER_type_redirect_2
#define PP_VK_impl_PROPERTY_GROUP_MEMBER_type(...) PP_VK_impl_PROPERTY_GROUP_MEMBER_type_redirect ( (__VA_ARGS__),



#define PP_VK_impl_PROPERTY_GROUP_MEMBER_group_redirect_2(members_, name_) private: struct name_##PROPERTY_GROUP : ::Vk::FeaturePropertyGroup<feature_t, name_##PROPERTY_GROUP>{ \
  inline constexpr static Vk::StringLiteral name{#name_};                                                                                                                              \
  private:                                                                                                                                                                             \
    using feature_t = name_##PROPERTY_GROUP;                                                                                                                                           \
  public:                                                                                                                                                                              \
    PP_VK_impl_PROPERTY_GROUP_MEMBERS members_                                                                                                                                         \
                                                                                                                                                                                       \
    inline constexpr static ::Vk::detail::ComponentSet<PP_VK_impl_PROPERTY_GROUP_COLLECT members_> properties{};                                                                       \
  }; public: name_##PROPERTY_GROUP name_
#define PP_VK_impl_PROPERTY_GROUP_MEMBER_group_redirect PP_VK_impl_PROPERTY_GROUP_MEMBER_group_redirect_2
#define PP_VK_impl_PROPERTY_GROUP_MEMBER_group(...) PP_VK_impl_PROPERTY_GROUP_MEMBER_group_redirect ( (__VA_ARGS__),




#define PP_VK_impl_PROPERTY_GROUP_PROCESS_redirect(decl) PP_VK_impl_PROPERTY_GROUP_MEMBER_##decl );
#define PP_VK_impl_PROPERTY_GROUP_PROCESS PP_VK_impl_PROPERTY_GROUP_PROCESS_redirect

#define PP_VK_impl_PROPERTY_GROUP_MEMBERS_redirect(...) VK_concat_inner(PP_VK_impl_PROPERTY_GROUP_FOR_EACH, VK_count_args(__VA_ARGS__))(__VA_ARGS__)
#define PP_VK_impl_PROPERTY_GROUP_MEMBERS PP_VK_impl_PROPERTY_GROUP_MEMBERS_redirect

#define PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS_redirect(prop) &feature_t::PP_VK_impl_FEATURE_DISCARD_##prop
#define PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS_redirect

#define PP_VK_impl_PROPERTY_GROUP_COLLECT_redirect(...) VK_concat_inner(PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH, VK_count_args(__VA_ARGS__))(__VA_ARGS__)
#define PP_VK_impl_PROPERTY_GROUP_COLLECT PP_VK_impl_PROPERTY_GROUP_COLLECT_redirect

#define PP_VK_impl_PROPERTY_GROUP_FOR_EACH_10(x, ...) PP_VK_impl_PROPERTY_GROUP_PROCESS(x) PP_VK_impl_PROPERTY_GROUP_FOR_EACH_9(__VA_ARGS__)
#define PP_VK_impl_PROPERTY_GROUP_FOR_EACH_9(x, ...) PP_VK_impl_PROPERTY_GROUP_PROCESS(x) PP_VK_impl_PROPERTY_GROUP_FOR_EACH_8(__VA_ARGS__)
#define PP_VK_impl_PROPERTY_GROUP_FOR_EACH_8(x, ...) PP_VK_impl_PROPERTY_GROUP_PROCESS(x) PP_VK_impl_PROPERTY_GROUP_FOR_EACH_7(__VA_ARGS__)
#define PP_VK_impl_PROPERTY_GROUP_FOR_EACH_7(x, ...) PP_VK_impl_PROPERTY_GROUP_PROCESS(x) PP_VK_impl_PROPERTY_GROUP_FOR_EACH_6(__VA_ARGS__)
#define PP_VK_impl_PROPERTY_GROUP_FOR_EACH_6(x, ...) PP_VK_impl_PROPERTY_GROUP_PROCESS(x) PP_VK_impl_PROPERTY_GROUP_FOR_EACH_5(__VA_ARGS__)
#define PP_VK_impl_PROPERTY_GROUP_FOR_EACH_5(x, ...) PP_VK_impl_PROPERTY_GROUP_PROCESS(x) PP_VK_impl_PROPERTY_GROUP_FOR_EACH_4(__VA_ARGS__)
#define PP_VK_impl_PROPERTY_GROUP_FOR_EACH_4(x, y, z, w) PP_VK_impl_PROPERTY_GROUP_PROCESS(x) PP_VK_impl_PROPERTY_GROUP_FOR_EACH_3(y, z, w)
#define PP_VK_impl_PROPERTY_GROUP_FOR_EACH_3(x, y, z) PP_VK_impl_PROPERTY_GROUP_PROCESS(x) PP_VK_impl_PROPERTY_GROUP_PROCESS(y) PP_VK_impl_PROPERTY_GROUP_PROCESS(z)
#define PP_VK_impl_PROPERTY_GROUP_FOR_EACH_2(x, y) PP_VK_impl_PROPERTY_GROUP_PROCESS(x) PP_VK_impl_PROPERTY_GROUP_PROCESS(y)
#define PP_VK_impl_PROPERTY_GROUP_FOR_EACH_1(x) PP_VK_impl_PROPERTY_GROUP_PROCESS(x)
#define PP_VK_impl_PROPERTY_GROUP_FOR_EACH_0()

#define PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_10(x, ...) PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS(x), PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_9(__VA_ARGS__)
#define PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_9(x, ...) PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS(x), PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_8(__VA_ARGS__)
#define PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_8(x, ...) PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS(x), PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_7(__VA_ARGS__)
#define PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_7(x, ...) PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS(x), PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_6(__VA_ARGS__)
#define PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_6(x, ...) PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS(x), PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_5(__VA_ARGS__)
#define PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_5(x, ...) PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS(x), PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_4(__VA_ARGS__)
#define PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_4(x, y, z, w) PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS(x), PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_3(y, z, w)
#define PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_3(x, y, z) PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS(x), PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS(y), PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS(z)
#define PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_2(x, y) PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS(x), PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS(y)
#define PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_1(x) PP_VK_impl_PROPERTY_GROUP_COLLECT_MEMBERS(x)
#define PP_VK_impl_PROPERTY_GROUP_COLLECT_EACH_0()


#define PP_VK_impl_DEFINE_FEATURE_PROPERTY_bool_redirect(name_) private: struct name_##PROPERTY : ::Vk::FeaturePropertyValue<feature_t, name_##PROPERTY, bool>{ \
    inline constexpr static Vk::StringLiteral name{#name_};                                                                                                \
    using ::Vk::FeaturePropertyValue<feature_t, name_##PROPERTY, bool>::operator=;                                                                                                                                                              \
  }; public: name_##PROPERTY name_
#define PP_VK_impl_DEFINE_FEATURE_PROPERTY_string_redirect(name_) private: struct name_##PROPERTY : ::Vk::FeaturePropertyValue<feature_t, name_##PROPERTY, Vk::String>{ \
    inline constexpr static Vk::StringLiteral name{#name_};                                                                                                             \
    using ::Vk::FeaturePropertyValue<feature_t, name_##PROPERTY, Vk::String>::operator=;\
  }; public: name_##PROPERTY name_
#define PP_VK_impl_DEFINE_FEATURE_PROPERTY_group_redirect(members_, name_) private: struct name_##PROPERTY_GROUP : ::Vk::FeaturePropertyGroup<feature_t, name_##PROPERTY_GROUP>{ \
    inline constexpr static Vk::StringLiteral name{#name_};                                                                                                                            \
  private:                                                                                                                                                                        \
    using feature_t = name_##PROPERTY_GROUP;                                                                                                                                \
  public:\
    PP_VK_impl_PROPERTY_GROUP_MEMBERS members_ \
                                                                                                                                                                            \
    inline constexpr static ::Vk::detail::ComponentSet<PP_VK_impl_PROPERTY_GROUP_COLLECT members_> properties{};                                                            \
  }; public: name_##PROPERTY_GROUP name_
#define PP_VK_impl_DEFINE_FEATURE_PROPERTY_type_redirect(type_, name_) private: struct name_##PROPERTY : ::Vk::FeaturePropertyValue<feature_t, name_##PROPERTY, VK_unwrap(type_)>{ \
    inline constexpr static Vk::StringLiteral name{#name_};                                                                                                             \
    using ::Vk::FeaturePropertyValue<feature_t, name_##PROPERTY, VK_unwrap(type_)>::operator=;\
  }; public: name_##PROPERTY name_
#define PP_VK_impl_FEATURE_PROPERTY_feature(...) NOOOOOOO WHY MUST YOU BE LIKE THIS >:c

#define PP_VK_impl_DEFINE_FEATURE_PROPERTY_bool VK_concat_inner(PP_VK_impl_DEFINE_, FEATURE_PROPERTY_bool_redirect)
#define PP_VK_impl_DEFINE_FEATURE_PROPERTY_string VK_concat_inner(PP_VK_impl_DEFINE_, FEATURE_PROPERTY_string_redirect)
#define PP_VK_impl_DEFINE_FEATURE_PROPERTY_group VK_concat_inner(PP_VK_impl_DEFINE_, FEATURE_PROPERTY_group_redirect)
#define PP_VK_impl_DEFINE_FEATURE_PROPERTY_type VK_concat_inner(PP_VK_impl_DEFINE_, FEATURE_PROPERTY_type_redirect)
#define PP_VK_impl_FEATURE_PROPERTY_bool PP_VK_impl_DEFINE_FEATURE_PROPERTY_bool (
#define PP_VK_impl_FEATURE_PROPERTY_string PP_VK_impl_DEFINE_FEATURE_PROPERTY_string (
#define PP_VK_impl_FEATURE_PROPERTY_type(...) PP_VK_impl_DEFINE_FEATURE_PROPERTY_type ( (__VA_ARGS__),
#define PP_VK_impl_FEATURE_PROPERTY_group(...) PP_VK_impl_DEFINE_FEATURE_PROPERTY_group ( (__VA_ARGS__),
#define PP_VK_impl_FEATURE_DISCARD_property
#define PP_VK_impl_FEATURE_DISCARD_bool
#define PP_VK_impl_FEATURE_DISCARD_string
#define PP_VK_impl_FEATURE_DISCARD_type(...)
#define PP_VK_impl_FEATURE_DISCARD_group(...)


#define PP_VK_impl_FEATURE_COLLECT_EACH_10(x, ...) PP_VK_impl_FEATURE_COLLECT_MEMBERS(x), PP_VK_impl_FEATURE_COLLECT_EACH_9(__VA_ARGS__)
#define PP_VK_impl_FEATURE_COLLECT_EACH_9(x, ...) PP_VK_impl_FEATURE_COLLECT_MEMBERS(x), PP_VK_impl_FEATURE_COLLECT_EACH_8(__VA_ARGS__)
#define PP_VK_impl_FEATURE_COLLECT_EACH_8(x, ...) PP_VK_impl_FEATURE_COLLECT_MEMBERS(x), PP_VK_impl_FEATURE_COLLECT_EACH_7(__VA_ARGS__)
#define PP_VK_impl_FEATURE_COLLECT_EACH_7(x, ...) PP_VK_impl_FEATURE_COLLECT_MEMBERS(x), PP_VK_impl_FEATURE_COLLECT_EACH_6(__VA_ARGS__)
#define PP_VK_impl_FEATURE_COLLECT_EACH_6(x, ...) PP_VK_impl_FEATURE_COLLECT_MEMBERS(x), PP_VK_impl_FEATURE_COLLECT_EACH_5(__VA_ARGS__)
#define PP_VK_impl_FEATURE_COLLECT_EACH_5(x, ...) PP_VK_impl_FEATURE_COLLECT_MEMBERS(x), PP_VK_impl_FEATURE_COLLECT_EACH_4(__VA_ARGS__)
#define PP_VK_impl_FEATURE_COLLECT_EACH_4(x, y, z, w) PP_VK_impl_FEATURE_COLLECT_MEMBERS(x), PP_VK_impl_FEATURE_COLLECT_EACH_3(y, z, w)
#define PP_VK_impl_FEATURE_COLLECT_EACH_3(x, y, z) PP_VK_impl_FEATURE_COLLECT_MEMBERS(x), PP_VK_impl_FEATURE_COLLECT_MEMBERS(y), PP_VK_impl_FEATURE_COLLECT_MEMBERS(z)
#define PP_VK_impl_FEATURE_COLLECT_EACH_2(x, y) PP_VK_impl_FEATURE_COLLECT_MEMBERS(x), PP_VK_impl_FEATURE_COLLECT_MEMBERS(y)
#define PP_VK_impl_FEATURE_COLLECT_EACH_1(x) PP_VK_impl_FEATURE_COLLECT_MEMBERS(x)
#define PP_VK_impl_FEATURE_COLLECT_EACH_0()

#define PP_VK_impl_FEATURE_COLLECT_INNER_MEMBER_redirect_2(member) &feature_t::PP_VK_impl_FEATURE_DISCARD_##member
#define PP_VK_impl_FEATURE_COLLECT_INNER_MEMBER_redirect PP_VK_impl_FEATURE_COLLECT_INNER_MEMBER_redirect_2
#define PP_VK_impl_FEATURE_COLLECT_INNER_MEMBER VK_concat_inner(PP_VK_impl_FEATURE_COLLECT_, INNER_MEMBER_redirect)

#define PP_VK_impl_FEATURE_DISCARD_OUTER_property PP_VK_impl_FEATURE_COLLECT_INNER_MEMBER (

#define PP_VK_impl_FEATURE_COLLECT_MEMBERS_redirect(prop) PP_VK_impl_FEATURE_DISCARD_OUTER_##prop)
#define PP_VK_impl_FEATURE_COLLECT_MEMBERS PP_VK_impl_FEATURE_COLLECT_MEMBERS_redirect

#define PP_VK_impl_FEATURE_PROPERTY_PROCESS_redirect_2(prop) VK_concat(PP_VK_impl_FEATURE_PROPERTY_, PP_VK_impl_FEATURE_DISCARD_##prop));
#define PP_VK_impl_FEATURE_PROPERTY_PROCESS_redirect PP_VK_impl_FEATURE_PROPERTY_PROCESS_redirect_2
#define PP_VK_impl_FEATURE_PROPERTY_PROCESS PP_VK_impl_FEATURE_PROPERTY_PROCESS_redirect

#define PP_VK_impl_FEATURE_COLLECT_redirect(...) VK_concat_inner(PP_VK_impl_FEATURE_COLLECT_EACH, VK_count_args(__VA_ARGS__))(__VA_ARGS__)
#define PP_VK_impl_FEATURE_COLLECT PP_VK_impl_FEATURE_COLLECT_redirect

#define VK_feature(name_, ...) private: struct name_##FEATURE : ::Vk::Feature<name_##FEATURE> {  \
    private:                                                                                     \
      using feature_t = name_##FEATURE;                                                          \
    public:                                                                                      \
      using ::Vk::Feature<name_##FEATURE>::operator=;                                            \
      VK_for_each(PP_VK_impl_FEATURE_PROPERTY_PROCESS, __VA_ARGS__)                              \
                                                                                                 \
    inline constexpr static Vk::StringLiteral name{#name_};                                                 \
    inline constexpr static ::Vk::detail::ComponentSet<PP_VK_impl_FEATURE_COLLECT(__VA_ARGS__)> properties{};\
  }; public: name_##FEATURE name_

#endif//VULKAN_WRAPPER_VK_FEATURES_H
