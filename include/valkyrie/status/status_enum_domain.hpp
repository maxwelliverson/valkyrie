//
// Created by Maxwell on 2020-12-30.
//

#ifndef VALKYRIE_STATUS_ENUM_DOMAIN_HPP
#define VALKYRIE_STATUS_ENUM_DOMAIN_HPP

#include <valkyrie/adt/array_ref.hpp>
#include <valkyrie/status/generic_code.hpp>

namespace valkyrie{
  template <typename E>
  class status_enum_domain : public status_domain {
    template <typename> friend class status_code;
    //using status_traits = status_traits<typename traits::status_type>;
    using status_type = status_code<status_enum_domain>;

    using self = status_enum_domain;


    inline static status_type& cast(status_code<void>& status) noexcept { return static_cast<status_type&>(status); }
    inline static const status_type& cast(const status_code<void>& status) noexcept { return static_cast<const status_type&>(status); }

    inline static const auto& info(const status_code<void>& status) noexcept {
      return traits::StatusEnum<E>::info[self::cast(status).value()];
      //return enum_traits<E>::status_info[self::cast(status).value()];
    }
    inline static array_ref<const code, dynamic, sizeof(code)> codeMappings(const status_code<void>& status) noexcept {
      return info(status).generic;
    }

  public:

    inline constexpr static uuid class_id = traits::StatusEnum<E>::domain_uuid;

    using value_type = E;
    using domain_type = status_enum_domain;

    constexpr status_enum_domain() noexcept : status_domain(traits::StatusEnum<E>::domain_uuid){}
    ~status_enum_domain() = default;

    VK_nodiscard string_ref doMessage(const status_code<void>& status) const noexcept override {
      auto msg = self::info(status).message;
      return string_ref(msg.data(), msg.size());
    }
    VK_nodiscard code doCode(const status_code<void>& status) const noexcept override {
      VK_axiom(status.domain() == *this);
      if (auto mappings = codeMappings(status); mappings.empty())
        return code::Unknown;
      else
        return mappings.front();
    }
    VK_nodiscard bool doFailure(const status_code<void>& status) const noexcept override {
      for (code code : codeMappings(status))
        if (code == code::Success)
          return false;
      return true;
    }
    bool doEquivalent(const status_code<void> &A, const status_code<void> &B) const noexcept override {
      VK_axiom(A.domain() == *this);
      if (B.domain() == *this)
        return cast(A).value() == cast(B).value();
      if (B.domain() == generic_domain::get()) {
        const auto& genB = static_cast<const status_code<generic_domain>&>(B);
        for (code ec : codeMappings(A))
          if (ec == genB.value())
            return true;
      }
      return false;
    }
    //VK_if(VK_exceptions_enabled(VK_noreturn void doThrowException(const status_code<void> &code) const override {  }))

    VK_nodiscard inline static constexpr const status_enum_domain<E>& get() noexcept;
    VK_nodiscard string_ref name() const noexcept override { return traits::StatusEnum<E>::domain_name; }
  };

  namespace detail{


    template <typename E>
    inline constexpr static status_enum_domain<E> statusEnumDomainInstance{};

  }

  template <typename E>
  inline constexpr const status_enum_domain<E>& status_enum_domain<E>::get() noexcept {
    return detail::statusEnumDomainInstance<E>;
  }
}

#endif//VALKYRIE_STATUS_ENUM_DOMAIN_HPP
