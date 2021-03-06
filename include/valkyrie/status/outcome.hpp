//
// Created by maxwe on 2021-03-04.
//

#ifndef VALKYRIE_ERROR_OUTCOME_HPP
#define VALKYRIE_ERROR_OUTCOME_HPP



namespace valkyrie{

  template <typename T, typename EC, typename EP, typename NoValuePolicy>
  class basic_outcome;

  template <typename T, typename EC, typename EP>
  using outcome = basic_outcome<T, EC, EP, void>;


}

#endif //VALKYRIE_ERROR_OUTCOME_HPP
