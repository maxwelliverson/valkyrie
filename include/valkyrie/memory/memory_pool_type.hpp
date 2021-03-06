//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_MEMORY_POOL_TYPE_HPP
#define VALKYRIE_MEMORY_MEMORY_POOL_TYPE_HPP

namespace valkyrie{
  /// Tag type defining a memory pool optimized for nodes.
  /// It does not support array allocations that great and may trigger a growth even if there is enough memory.
  /// But it is the fastest pool type.
  /// \ingroup allocator
  struct node_pool : FOONATHAN_EBO(std::true_type)
      {
          using type = detail::node_free_memory_list;
      };

  /// Tag type defining a memory pool optimized for arrays.
  /// It keeps the nodes oredered inside the free list and searches the list for an appropriate memory block.
  /// Array allocations are still pretty slow, if the array gets big enough it can get slower than \c new.
  /// Node allocations are still fast, unless there is deallocation in random order.
  /// \note Use this tag type only if you really need to have a memory pool!
  /// \ingroup allocator
  struct array_pool : FOONATHAN_EBO(std::true_type)
      {
          using type = detail::array_free_memory_list;
      };

  /// Tag type defining a memory pool optimized for small nodes.
  /// The free list is intrusive and thus requires that each node has at least the size of a pointer.
  /// This tag type does not have this requirement and thus allows zero-memory-overhead allocations of small nodes.
  /// It is a little bit slower than \ref node_pool and does not support arrays.
  /// \ingroup allocator
  struct small_node_pool : FOONATHAN_EBO(std::false_type)
      {
          using type = detail::small_free_memory_list;
      };
}

#endif//VALKYRIE_MEMORY_MEMORY_POOL_TYPE_HPP
