//
// Created by maxwe on 2021-03-28.
//

#ifndef VALKYRIE_ADT_INTRUSIVE_LIST_HPP
#define VALKYRIE_ADT_INTRUSIVE_LIST_HPP

namespace valkyrie{

  namespace impl{
    class ilist_node_base{};
  }

  struct default_tag{};


  template <typename Derived, typename Tag = default_tag>
  class ilist_node : impl::ilist_node_base{};


}

#endif//VALKYRIE_ADT_INTRUSIVE_LIST_HPP
