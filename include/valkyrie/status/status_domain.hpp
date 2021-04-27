//
// Created by maxwe on 2021-04-26.
//

#ifndef VALKYRIE_STATUS_STATUS_DOMAIN_HPP
#define VALKYRIE_STATUS_STATUS_DOMAIN_HPP

#include <valkyrie/utility/uuid.hpp>
#include <valkyrie/adt/string_view.hpp>

namespace valkyrie{

  template <typename>
  class status_code;
  template <typename>
  class error_code;
  class status_domain;

  enum class code : i32;
  enum class severity : i32;


  inline severity get_default_severity(code code) noexcept;

  class status_domain{
    template <typename>
    friend class status_code;

    u64 guid_;

  protected:

    constexpr status_domain(const uuid& class_id) noexcept : guid_(toInteger(class_id)){}


    virtual bool do_failure(const status_code<void>& status) const noexcept = 0;
    virtual bool do_equivalent(const status_code<void>& statusA, const status_code<void>& statusB) const noexcept = 0;
    virtual code do_generic_code(const status_code<void>& status) const noexcept = 0;
    virtual string_ref do_message(const status_code<void>& status) const noexcept = 0;

    virtual severity do_severity(const status_code<void>& status) const noexcept { return get_default_severity(this->do_generic_code(status)); }
    virtual void do_erased_copy(status_code<void>& To, const status_code<void>& From, size_t Bytes) const noexcept { std::memcpy(&To, &From, Bytes); }
    virtual void do_erased_destroy(status_code<void>& code, size_t Bytes) const noexcept { }

    VK_noreturn virtual void do_throw_exception(const status_code<void>& code) const VK_throws;

  public:

    using string_ref = string_ref;

    virtual string_ref name() const noexcept = 0;
    constexpr u64 id() const noexcept {
      return guid_;
    }

    VK_nodiscard friend bool operator==(const status_domain& A, const status_domain& B) noexcept{
      //return A.name() == B.name();
      return A.id() == B.id();
      //return A.guid() == B.guid();
    }
    VK_nodiscard friend std::strong_ordering operator<=>(const status_domain& A, const status_domain& B) noexcept {
      return A.id() <=> B.id();
    }
  };
}

#endif  //VALKYRIE_STATUS_STATUS_DOMAIN_HPP
