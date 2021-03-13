//
// Created by maxwe on 2021-03-12.
//

#ifndef VALKYRIE_ADT_NOTHING_HPP
#define VALKYRIE_ADT_NOTHING_HPP

namespace valkyrie{
  enum class nothing_type{ nothing = 1 };
  inline constexpr static nothing_type nothing = nothing_type::nothing;
}

#endif//VALKYRIE_ADT_NOTHING_HPP
