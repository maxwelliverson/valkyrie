//
// Created by Maxwell on 2020-12-30.
//

#ifndef VALKYRIE_VALIDATION_DOMAIN_HPP
#define VALKYRIE_VALIDATION_DOMAIN_HPP

#include <valkyrie/status/status_code.hpp>


namespace valkyrie{
  // A value returned with this domain must be explicitly checked or suppressed, otherwise it will panic upon destruction.
  template <typename Dom>
  class status_validation_domain final : public Dom{
    static_assert(std::derived_from<Dom, status_domain>);
    static_assert(sizeof(typename Dom::value_type) <= sizeof(u32));

    inline std::u8string getName() const noexcept{
      StringView domainName{Dom::name()};
      std::u8string stdDomainName{domainName.data(), domainName.size()};
      stdDomainName += VK_string(" (Validation)");
      return std::move(stdDomainName);
    }

  public:

    using status_type = status_code<status_validation_domain>;
    using error_type  = error_code<status_validation_domain>;


    status_validation_domain() noexcept = default;

    template <typename T>
    struct mixin : T{

      using typename T::value_type;

      constexpr mixin() noexcept(std::is_nothrow_constructible_v<T>) : T{}, isChecked(true){}
      constexpr mixin(const mixin& other) = delete;
      constexpr mixin(mixin&& other) noexcept
          : T{std::move(other)}, isChecked(other.isChecked){
        other.isChecked = true;
      }

      /*template <typename ...Args> requires(ConstructibleFrom<T, Args...>)
      constexpr mixin(Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
          : T{std::forward<Args>(args)...},
            isChecked(T::success()){}*/

      using T::T;

      mixin& operator=(const mixin& other) = delete;
      mixin& operator=(mixin&& other) noexcept {
        T::operator=(std::move(other));
        isChecked = other.isChecked;
        other.suppress();
        return *this;
      }

      ~mixin() {
        if (!isChecked)
          panic(*this);
      }

      value_type& value() & noexcept {
        isChecked = true;
        return T::value();
      }
      const value_type& value() const & noexcept {
        isChecked = true;
        return T::value();
      }
      value_type&& value() && noexcept {
        isChecked = true;
        return std::move(T::value());
      }
      const value_type&& value() const && noexcept {
        isChecked = true;
        return std::move(T::value());
      }

      void clear() noexcept {
        T::clear();
        suppress();
      }
      void suppress() const noexcept {
        isChecked = true;
      }
      bool checked() const noexcept {
        return isChecked;
      }

    private:
      friend class status_validation_domain;
      mutable bool isChecked = T::success();
    };

    void doErasedCopy(status_code<void>& To, const status_code<void>& From, size_t Bytes) const noexcept override {
      VK_assert(From.domain() == *this);
      VK_assert(Bytes == 16);
      std::memcpy(&To, &From, Bytes);
      static_cast<const status_code<status_validation_domain<Dom>>&>(From).suppress();
    }
    void doErasedDestroy(status_code<void>& code, size_t Bytes) const noexcept override {
      auto& code = static_cast<status_code<status_validation_domain<Dom>>&>(code);
      if (!code.checked())
        panic(code);
    }

    VK_nodiscard string_ref name() const noexcept override {
      const static std::u8string domain_name = getName();
      return StringView(domain_name.data(), (u32)domain_name.size());
    }
    VK_nodiscard bool doEquivalent(const status_code<void>& statusA, const status_code<void>& statusB) const noexcept override {
      using domain_type = status_code<Dom>;
      VK_assert(statusA.domain() == *this);
      if (statusB.domain() == *this)
        return static_cast<const status_type&>(statusA).value() == static_cast<const status_type&>(statusB).value();
      if (statusB.domain().id() == Dom{}.id()) // should be class_traits<Dom>::class_id but that isn't working???
        return static_cast<const status_type&>(statusA).value() ==
               static_cast<const domain_type&>(statusB).value();
      return false;
    }
    VK_nodiscard string_ref doMessage(const status_code<void>& status) const noexcept override {
      VK_assert(status.domain() == *this);
      static_cast<const mixin<detail::StatusStorage<status_validation_domain>>&>(status).isChecked = true;
      return Dom::doMessage(status);
    }
    VK_nodiscard code doCode(const status_code<void>& status) const noexcept override {
      VK_assert(status.domain() == *this);
      static_cast<const mixin<detail::StatusStorage<status_validation_domain>>&>(status).isChecked = true;
      return Dom::doCode(status);
    }
    VK_nodiscard bool doFailure(const status_code<void>& status) const noexcept override {
      VK_assert(status.domain() == *this);
      static_cast<const mixin<detail::StatusStorage<status_validation_domain>>&>(status).isChecked = true;
      return Dom::doFailure(status);
    }

    VK_nodiscard static const status_validation_domain& get() noexcept{
      const static status_validation_domain instance{};
      return instance;
    }
  };
}

#endif//VALKYRIE_VALIDATION_DOMAIN_HPP
