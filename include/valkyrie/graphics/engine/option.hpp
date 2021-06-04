//
// Created by Maxwell on 2020-11-15.
//

#ifndef VALKYRIE_GRAPHICS_OPTION_HPP
#define VALKYRIE_GRAPHICS_OPTION_HPP


#include <valkyrie/adt/array_ref.hpp>
#include <valkyrie/adt/flat_map.hpp>
#include <valkyrie/adt/temp_array.hpp>
#include <valkyrie/adt/string.hpp>
#include <valkyrie/adt/interval.hpp>
#include <valkyrie/utility/version.hpp>


namespace valkyrie::graphics::engine{

  class option;
  class feature;
  class enumerated_setting;
  class instance_extension;
  class device_extension;
  class configuration{
    struct InternalState;
    InternalState* pState;
  public:

  };

  template <std::derived_from<option> O>
  auto dependencies(const O& option) noexcept;


  class option {
    virtual option* doGetDependencies(u32& arraySize) const noexcept = 0;
  public:

    virtual bool    doIsSupported()                   const noexcept = 0;

    template <std::derived_from<option> O>
    friend auto dependencies(const O& option) noexcept;
  };

  template <std::derived_from<option> O>
  inline auto dependencies(const O& option) noexcept {
    u32 depSize = 0;
    auto* pDeps = option.doGetDependencies(depSize);
    return array_ref{pDeps, depSize};
  }

  class enumeration{
    flat_map<u32, string> entries;
  };
  class flags{
    flat_map<u32, string> entries;
  };
  class group{};


  template <typename>
  class field;
  template <>
  class field<void> : public option {
    string_view name_;
  protected:
    constexpr field() = default;
    constexpr explicit field(string_view name) noexcept : name_(name){}
  public:
    string_view name() const noexcept {
      return {name_.data(), static_cast<u32>(name_.size())};
    }
    option * doGetDependencies(u32& arraySize) const noexcept override;
    bool     doIsSupported()     const noexcept override;
  };
  template <>
  class field<string> : public field<void>{
  public:
    field() = default;
  };
  template <>
  class field<bool> : public field<void>{
  public:
    field() = default;
  };



  template <typename T, typename ...Opt>
  class field<interval<T, Opt...>> : public field<void>{
    interval<T, Opt...> value_;
  public:
    constexpr field() = default;
    constexpr field(string_view name) noexcept : field<void>(name), value_(){}

    constexpr auto& value() noexcept {
      return value_;
    }
    constexpr const auto& value() const noexcept {
      return value_;
    }
  };

  class enumerated_field : public field<void>{
    enumeration enumeration;

  protected:
    enumerated_field();
  };
  
  template <enumerator_c E>
  class field<E> : public enumerated_field{
  public:
    field() = default;
  };

  class bitflag_field : public field<void>{};

  template <bitflag_c E>
  class field<E> : public bitflag_field{
  public:
    field() = default;
  };
  template <>
  class field<group> : public field<void>{
    flat_set<field<void>*> enabledFields;
    flat_set<field<void>*> disabledFields;
    vector<field<void>*>   members;
  public:
    field() = default;
  };

  class enabled_option : public option{
    string_view name_;
    bool isSupported_ = false;
    bool isEnabled_ = false;
    virtual status doSetEnabled(bool enabled) noexcept = 0;
  protected:
    constexpr explicit enabled_option(string_view name) noexcept : name_(name){}
  public:
    inline string_view name() const noexcept {
      return {name_.data(), u32(name_.size())};
    }
    inline status enable(bool b = true)  noexcept {

      if (!doIsSupported())
        return code::not_supported;

      auto result = this->doSetEnabled(b);

      if (result.success())
        isEnabled_ = b;

      return std::move(result);
    }
    inline status disable() noexcept {
      auto result = this->doSetEnabled(false);

      if (result.success())
        isEnabled_ = false;

      return std::move(result);
    }

    inline void setSupport(bool value) noexcept {
      isSupported_ = value;
    }
    bool    doIsSupported()     const noexcept override {
      return isSupported_;
    }
  };

  class feature : public enabled_option{

    status doSetEnabled(bool enabled) noexcept override;

  public:

    constexpr feature(string_view name) noexcept : enabled_option(name){}

    enabled_option* doGetDependencies(u32& arraySize) const noexcept override;

  };

  class extension : public enabled_option{
    version specVersion_;

    status doSetEnabled(bool enabled) noexcept override {

      for (extension& dep : dependencies(*this)){
        auto result = dep.enable(enabled);
        if (result.failure())
          return std::move(result);
      }

      return code::success;
    }

  protected:
    constexpr extension(string_view name, version specVersion) noexcept
        : enabled_option(name), specVersion_(specVersion){}
  public:
    inline version version() const noexcept {
      return specVersion_;
    }

    extension* doGetDependencies(u32& arraySize) const noexcept override = 0;

    virtual array_ref<feature>     features() noexcept;
    virtual array_ref<field<void>> properties() noexcept;
  };





  class instance_extension : public extension{
  public:
    using extension::extension;
  };
  class device_extension : public extension{
  public:
    using extension::extension;
  };

  class layer : public option{
    string_view name_;
    version specVersion_;
    version implVersion_;
    small_vector<extension*, 2> enabledExtensions_;
  public:
    inline string_view name() const noexcept {
      return name_;
    }
    inline version implVersion() const noexcept {
      return implVersion_;
    }
    inline version specVersion() const noexcept {
      return specVersion_;
    }

  };
}


#endif//VALKYRIE_GRAPHICS_OPTION_HPP
