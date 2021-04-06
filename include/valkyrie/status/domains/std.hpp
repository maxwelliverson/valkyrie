//
// Created by maxwe on 2021-04-05.
//

#ifndef VALKYRIE_STATUS_DOMAINS_STD_HPP
#define VALKYRIE_STATUS_DOMAINS_STD_HPP

#include "../status_code.hpp"


#include <system_error>


namespace valkyrie{
  class std_error_category_domain : public status_domain{


    struct value_type_{
      u32  id;
      i32  code;
    };

  public:

    using value_type = value_type_;

    static value_type      to_value_type(std::error_code std_err) noexcept;
    static std::error_code to_error_code(value_type val) noexcept;

    constexpr static uuid class_id{"b3faed85-9813-49c2-bd80-3a4e6c4e29d7"};

    constexpr std_error_category_domain() : status_domain(class_id){}


    string_ref name() const noexcept override;

    string_ref do_message(const status_code<void> &status) const noexcept override;
    bool do_equivalent(const status_code<void> &statusA,
                       const status_code<void> &statusB) const noexcept override;
    severity do_severity(const status_code<void> &status) const noexcept override;
    code do_generic_code(const status_code<void> &status) const noexcept override;
    bool do_failure(const status_code<void> &status) const noexcept override;
    VK_noreturn void do_throw_exception(const status_code<void> &code) const override;


    inline static const std_error_category_domain& get() noexcept {
      constexpr static std_error_category_domain instance{};
      return instance;
    }
  };

  using std_status = status_code<std_error_category_domain>;
  using std_error  = error_code<std_error_category_domain>;
}

valkyrie::std_status make_status_code(std::error_code err) noexcept;

#endif//VALKYRIE_STATUS_DOMAINS_STD_HPP
