//
// Created by Maxwell on 2020-12-30.
//

#ifndef VALKYRIE_STATUS_ENUM_DOMAIN_HPP
#define VALKYRIE_STATUS_ENUM_DOMAIN_HPP

#include <valkyrie/Core/ADT/ArrayRef.hpp.hpp>
#include <valkyrie/Core/Error/GenericCode.hpp>

namespace valkyrie::Core{
  template <typename E>
  class StatusEnumDomain : public StatusDomain {
    template <typename> friend class Core::StatusCode;
    //using status_traits = Core::status_traits<typename traits::status_type>;
    using status_type = StatusCode<StatusEnumDomain>;

    using self = StatusEnumDomain;


    inline static status_type& cast(StatusCode<void>& status) noexcept { return static_cast<status_type&>(status); }
    inline static const status_type& cast(const StatusCode<void>& status) noexcept { return static_cast<const status_type&>(status); }

    inline static const auto& info(const StatusCode<void>& status) noexcept {
      return Traits::StatusEnum<E>::info[self::cast(status).value()];
      //return enum_traits<E>::status_info[self::cast(status).value()];
    }
    inline static ArrayRef<const Code, dynamicExtent, sizeof(Code)> codeMappings(const StatusCode<void>& status) noexcept {
      return info(status).generic;
    }

  public:

    inline constexpr static Uuid uuid = Traits::StatusEnum<E>::domain_uuid;

    using value_type = E;
    using domain_type = StatusEnumDomain;

    constexpr StatusEnumDomain() noexcept : StatusDomain(Traits::StatusEnum<E>::domain_uuid){}
    ~StatusEnumDomain() = default;

    VK_nodiscard StringRef doMessage(const StatusCode<void>& status) const noexcept override {
      auto msg = self::info(status).message;
      return StringRef(msg.data(), msg.size());
    }
    VK_nodiscard Code doCode(const StatusCode<void>& status) const noexcept override {
      VK_axiom(status.domain() == *this);
      if (auto mappings = codeMappings(status); mappings.empty())
        return Code::Unknown;
      else
        return mappings.front();
    }
    VK_nodiscard bool doFailure(const StatusCode<void>& status) const noexcept override {
      for (Code code : codeMappings(status))
        if (code == Code::Success)
          return false;
      return true;
    }
    bool doEquivalent(const StatusCode<void> &A, const StatusCode<void> &B) const noexcept override {
      VK_axiom(A.domain() == *this);
      if (B.domain() == *this)
        return cast(A).value() == cast(B).value();
      if (B.domain() == GenericDomain::get()) {
        const auto& genB = static_cast<const StatusCode<GenericDomain>&>(B);
        for (Code ec : codeMappings(A))
          if (ec == genB.value())
            return true;
      }
      return false;
    }
    //VK_if(VK_exceptions_enabled(VK_noreturn void doThrowException(const StatusCode<void> &Code) const override {  }))

    VK_nodiscard inline static constexpr const StatusEnumDomain<E>& get() noexcept;
    VK_nodiscard StringRef name() const noexcept override { return Traits::StatusEnum<E>::domain_name; }
  };

  namespace Detail{


    template <typename E>
    inline constexpr static StatusEnumDomain<E> statusEnumDomainInstance{};

  }

  template <typename E>
  inline constexpr const StatusEnumDomain<E>& StatusEnumDomain<E>::get() noexcept {
    return Detail::statusEnumDomainInstance<E>;
  }
}

#endif//VALKYRIE_STATUS_ENUM_DOMAIN_HPP
