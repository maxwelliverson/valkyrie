//
// Created by maxwe on 2021-03-04.
//

#ifndef VALKYRIE_ERROR_RESULT_HPP
#define VALKYRIE_ERROR_RESULT_HPP

#include "status_code.hpp"

namespace valkyrie{

  template <typename T, typename EC, typename NoValuePolicy>
  class basic_result;

  template <typename T, typename Dom = generic_domain>
  using result = basic_result<T, Dom, void>;

  template <typename T>
  using system_result = result<T, typename system_status::domain_type>;
}

#endif //VALKYRIE_ERROR_RESULT_HPP
