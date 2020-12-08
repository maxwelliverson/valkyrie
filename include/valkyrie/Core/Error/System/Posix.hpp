//
// Created by Maxwell on 2020-11-19.
//

#ifndef VALKYRIE_POSIX_ERROR_HPP
#define VALKYRIE_POSIX_ERROR_HPP

#include <valkyrie/Core/Error/Status.hpp>

namespace valkyrie::Core::System{

  class PosixStatus;
  class PosixError;

  namespace Detail{
    class PosixStatusDomain : public StatusDomain{
      struct value_container{
        int posixCode;
        int sysCallOrigin;

        value_container() = default;
        explicit value_container(int posixCode) noexcept : posixCode(posixCode), sysCallOrigin(0){}
        explicit value_container(int posixCode, int sysCallOrigin) noexcept : posixCode(posixCode), sysCallOrigin(sysCallOrigin){}

        inline bool equivalent(value_container other) const noexcept {
          return posixCode == other.posixCode;
        }
      };
    public:

      inline constexpr static Core::StringView domain_name{VK_string("POSIX errno")};
      inline constexpr static Uuid             uuid{"d5add6cc-d38f-4377-8ed4-c012f1b604ef"};

      using value_type = value_container;
      using status_type = PosixStatus;
      using error_type = PosixError;

      inline StringView name() const noexcept override{
        return domain_name;
      }
      inline const Uuid & id() const noexcept override{
        return uuid;
      }

      StringRef doMessage(const StatusCode<void>& status) const noexcept override;
      Severity doSeverity(const StatusCode<void>& status) const noexcept override;
      //void doThrowException(const StatusCode<void>& code) const override;
      Code doCode(const StatusCode<void>& status) const noexcept override;
      bool doFailure(const StatusCode<void>& status) const noexcept override;
      bool doEquivalent(const StatusCode<void>& statusA, const StatusCode<void>& statusB) const noexcept override;

      inline constexpr static const PosixStatusDomain& get() noexcept;
    };
    inline constexpr static PosixStatusDomain posixDomainInstance{};
    inline constexpr const PosixStatusDomain & PosixStatusDomain::get() noexcept {
      return posixDomainInstance;
    }
  }

  class PosixStatus : public StatusCode<Detail::PosixStatusDomain>{
    using Base_ = StatusCode<Detail::PosixStatusDomain>;

    using Base_::value;
  public:

    using value_type = int;

    PosixStatus() noexcept = default;
    explicit PosixStatus(int errValue) noexcept : Base_(std::in_place, errValue){}
    PosixStatus(int errValue, int sysCall) : Base_(std::in_place, errValue, sysCall){}

    value_type& value() & noexcept {
      return Base_::value().posixCode;
    }
    const value_type& value() const & noexcept{
      return Base_::value().posixCode;
    }
    value_type&& value() && noexcept{
      return static_cast<Base_&&>(*this).value().posixCode;
    }
    const value_type&& value() const && noexcept{
      return static_cast<const Base_&&>(*this).value().posixCode;
    }
  };
  class PosixError  : public ErrorCode<Detail::PosixStatusDomain>{
    using Base_ = ErrorCode<Detail::PosixStatusDomain>;

    using Base_::value;
  public:

    using value_type = int;

    using Base_::Base_;
    PosixError() noexcept : Base_(std::in_place, errno){}
    explicit PosixError(int errValue) noexcept : Base_(std::in_place, errValue){}
    PosixError(int errValue, int sysCall) : Base_(std::in_place, errValue, sysCall){}

    const value_type& value() const & noexcept{
      return Base_::value().posixCode;
    }
  };

  inline PosixStatus posixErrno() noexcept{
    return PosixStatus(errno);
  }
}

#if VK_not(VK_system_windows)
namespace valkyrie::Core{
  using SystemStatus = System::PosixStatus;
  using SystemError = System::PosixError;
}
#endif

#endif//VALKYRIE_POSIX_ERROR_HPP
