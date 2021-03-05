//
// Created by Maxwell on 2020-11-19.
//

#ifndef VALKYRIE_POSIX_ERROR_HPP
#define VALKYRIE_POSIX_ERROR_HPP

#include <valkyrie/status/status_code.hpp>

namespace valkyrie::sys{

  class posix_status;
  class posix_error;

  namespace detail{
    class posix_status_domain : public status_domain{
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

      inline constexpr static StringView domain_name{VK_string("POSIX errno")};
      inline constexpr static uuid             class_id{"d5add6cc-d38f-4377-8ed4-c012f1b604ef"};

      using value_type = value_container;
      using status_type = posix_status;
      using error_type = posix_error;

      inline StringView name() const noexcept override{
        return domain_name;
      }
      inline const uuid & id() const noexcept override{
        return class_id;
      }

      string_ref doMessage(const status_code<void>& status) const noexcept override;
      Severity doSeverity(const status_code<void>& status) const noexcept override;
      //void doThrowException(const status_code<void>& code) const override;
      code doCode(const status_code<void>& status) const noexcept override;
      bool doFailure(const status_code<void>& status) const noexcept override;
      bool doEquivalent(const status_code<void>& statusA, const status_code<void>& statusB) const noexcept override;

      inline constexpr static const posix_status_domain& get() noexcept;
    };
    inline constexpr static posix_status_domain posixDomainInstance{};
    inline constexpr const posix_status_domain & posix_status_domain::get() noexcept {
      return posixDomainInstance;
    }
  }

  class posix_status : public status_code<detail::posix_status_domain>{
    using Base_ = status_code<detail::posix_status_domain>;

    using Base_::value;
  public:

    using value_type = int;

    posix_status() noexcept = default;
    explicit posix_status(int errValue) noexcept : Base_(std::in_place, errValue){}
    posix_status(int errValue, int sysCall) : Base_(std::in_place, errValue, sysCall){}

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
  class posix_error  : public error_code<detail::posix_status_domain>{
    using Base_ = error_code<detail::posix_status_domain>;

    using Base_::value;
  public:

    using value_type = int;

    using Base_::Base_;
    posix_error() noexcept : Base_(std::in_place, errno){}
    explicit posix_error(int errValue) noexcept : Base_(std::in_place, errValue){}
    posix_error(int errValue, int sysCall) : Base_(std::in_place, errValue, sysCall){}

    const value_type& value() const & noexcept{
      return Base_::value().posixCode;
    }
  };

  inline posix_status posixErrno() noexcept{
    return posix_status(errno);
  }
}

#if VK_not(VK_system_windows)
namespace valkyrie{
  using system_status = sys::posix_status;
  using system_error = sys::posix_error;
}
#endif

#endif//VALKYRIE_POSIX_ERROR_HPP
