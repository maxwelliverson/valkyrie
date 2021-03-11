//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_DETAIL_FREE_LIST_ARRAY_HPP
#define VALKYRIE_MEMORY_DETAIL_FREE_LIST_ARRAY_HPP

namespace valkyrie::detail{
  // an array of free_memory_list types
  // indexed via size, AccessPolicy does necessary conversions
  // requires trivial destructible FreeList type
  template <class FreeList, class AccessPolicy>
  class free_list_array
  {
    // not supported on GCC 4.7
    //static_assert(std::is_trivially_destructible<FreeList>::value,
    //            "free list must be trivially destructible");
  public:
    // creates sufficient elements to support up to given maximum node size
    // all lists are initially empty
    // actual number is calculated via policy
    // memory is taken from fixed_memory_stack, it must be sufficient
    free_list_array(fixed_memory_stack& stack, const char* end,
                    u64 max_node_size) noexcept
        : no_elements_(AccessPolicy::index_from_size(max_node_size) - min_size_index + 1)
        {
            array_ = static_cast<FreeList*>(
                stack.allocate(end, no_elements_ * sizeof(FreeList), alignof(FreeList)));
        VK_assert_MSG(array_, "insufficient memory for free lists");
        for (u64 i = 0u; i != no_elements_; ++i)
        {
          auto node_size = AccessPolicy::size_from_index(i + min_size_index);
          ::new (static_cast<void*>(array_ + i)) FreeList(node_size);
        }
        }

    // move constructor, does not actually move the elements, just the pointer
    free_list_array(free_list_array&& other) noexcept
        : array_(other.array_), no_elements_(other.no_elements_)
    {
      other.array_       = nullptr;
      other.no_elements_ = 0u;
    }

    // destructor, does nothing, list must be trivially destructible!
    ~free_list_array() noexcept = default;

    free_list_array& operator=(free_list_array&& other) noexcept
    {
      array_       = other.array_;
      no_elements_ = other.no_elements_;

      other.array_       = nullptr;
      other.no_elements_ = 0u;
      return *this;
    }

    // access free list for given size
    FreeList& get(u64 node_size) const noexcept
    {
      auto i = AccessPolicy::index_from_size(node_size);
      if (i < min_size_index)
        i = min_size_index;
      return array_[i - min_size_index];
    }

    // number of free lists
    u64 size() const noexcept
    {
      return no_elements_;
    }

    // maximum supported node size
    u64 max_node_size() const noexcept
    {
      return AccessPolicy::size_from_index(no_elements_ + min_size_index - 1);
    }

  private:
    static const u64 min_size_index;

    FreeList*   array_;
    u64 no_elements_;
  };

  template <class FL, class AP>
  const u64 free_list_array<FL, AP>::min_size_index =
      AP::index_from_size(FL::min_element_size);

  // AccessPolicy that maps size to indices 1:1
  // creates a free list for each size!
  struct identity_access_policy
  {
    static u64 index_from_size(u64 size) noexcept
    {
      return size;
    }

    static u64 size_from_index(u64 index) noexcept
    {
      return index;
    }
  };

  // AccessPolicy that maps sizes to the integral log2
  // this creates more nodes and never wastes more than half the size
  struct log2_access_policy
  {
    static u64 index_from_size(u64 size) noexcept;
    static u64 size_from_index(u64 index) noexcept;
  };
}

#endif//VALKYRIE_MEMORY_DETAIL_FREE_LIST_ARRAY_HPP
