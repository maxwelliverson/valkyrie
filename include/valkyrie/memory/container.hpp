//
// Created by maxwe on 2021-03-12.
//

#ifndef VALKYRIE_MEMORY_CONTAINER_HPP
#define VALKYRIE_MEMORY_CONTAINER_HPP

#include <functional>
#include <utility>

#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <scoped_allocator>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "std_allocator.hpp"
#include "threading.hpp"

namespace valkyrie
{
    /// \ingroup adapter
    /// @{

    /// Alias template for an STL container that uses a certain
    /// \concept{concept_rawallocator,RawAllocator}. It is just a shorthand for a passing in the \c
    /// RawAllocator wrapped in a \ref foonathan::memory::std_allocator.
    template <typename T, class RawAllocator>
    using vector = std::vector<T, std_allocator<T, RawAllocator>>;

    /// Same as above but uses \c std::scoped_allocator_adaptor so the allocator is inherited by all
    /// nested containers.
    template <typename T, class RawAllocator>
    using vector_scoped_alloc = std::vector<T, std::scoped_allocator_adaptor<std_allocator<T, RawAllocator>>>;

    /// \copydoc vector
    template <typename T, class RawAllocator>
    using deque = std::deque<T, std_allocator<T, RawAllocator>>;
    /// \copydoc vector_scoped_alloc
    template <typename T, class RawAllocator>
    using deque_scoped_alloc = std::deque<T, std::scoped_allocator_adaptor<std_allocator<T, RawAllocator>>>;

    /// \copydoc vector
    template <typename T, class RawAllocator>
    using list = std::list<T, std_allocator<T, RawAllocator>>;
    /// \copydoc vector_scoped_alloc
    template <typename T, class RawAllocator>
    using list_scoped_alloc = std::list<T, std::scoped_allocator_adaptor<std_allocator<T, RawAllocator>>>;

    /// \copydoc vector
    template <typename T, class RawAllocator>
    using forward_list = std::forward_list<T, std_allocator<T, RawAllocator>>;
    /// \copydoc vector_scoped_alloc
    template <typename T, class RawAllocator>
    using forward_list_scoped_alloc = std::forward_list<T, std::scoped_allocator_adaptor<std_allocator<T, RawAllocator>>>;

    /// \copydoc vector
    template <typename T, class RawAllocator>
    using set =  std::set<T, std::less<T>, std_allocator<T, RawAllocator>>;
    /// \copydoc vector_scoped_alloc
    template <typename T, class RawAllocator>
    using set_scoped_alloc = std::set<T, std::less<T>,
        std::scoped_allocator_adaptor<std_allocator<T, RawAllocator>>>;

    /// \copydoc vector
    template <typename T, class RawAllocator>
    using multiset = std::multiset<T, std::less<T>, std_allocator<T, RawAllocator>>;
    /// \copydoc vector_scoped_alloc
    template <typename T, class RawAllocator>
    using multiset_scoped_alloc = std::multiset<T, std::less<T>,
        std::scoped_allocator_adaptor<std_allocator<T, RawAllocator>>>;

    /// \copydoc vector
    template <typename Key, typename Value, class RawAllocator>
    using map = std::map<Key, Value, std::less<Key>,
        std_allocator<std::pair<const Key, Value>, RawAllocator>>;
    /// \copydoc vector_scoped_alloc
    template <typename Key, typename Value, class RawAllocator>
    using map_scoped_alloc = std::map<Key, Value, std::less<Key>,
        std::scoped_allocator_adaptor<
        std_allocator<std::pair<const Key, Value>, RawAllocator>>>;

    /// \copydoc vector
    template <typename Key, typename Value, class RawAllocator>
    using multimap = std::multimap<Key, Value, std::less<Key>,
        std_allocator<std::pair<const Key, Value>, RawAllocator>>;
    /// \copydoc vector_scoped_alloc
    template <typename Key, typename Value, class RawAllocator>
    using multimap_scoped_alloc = std::multimap<Key, Value, std::less<Key>,
        std::scoped_allocator_adaptor<
        std_allocator<std::pair<const Key, Value>, RawAllocator>>>;

    /// \copydoc vector
    template <typename T, class RawAllocator>
    using unordered_set = std::unordered_set<T, std::hash<T>, std::equal_to<T>, std_allocator<T, RawAllocator>>;
    /// \copydoc vector_scoped_alloc
    template <typename T, class RawAllocator>
    using unordered_set_scoped_alloc = std::unordered_set<T, std::hash<T>, std::equal_to<T>,
        std::scoped_allocator_adaptor<std_allocator<T, RawAllocator>>>;

    /// \copydoc vector
    template <typename T, class RawAllocator>
    using unordered_multiset = std::unordered_multiset<T, std::hash<T>, std::equal_to<T>,
        std_allocator<T, RawAllocator>>;
    /// \copydoc vector_scoped_alloc
    template <typename T, class RawAllocator>
    using unordered_multiset_scoped_alloc = std::unordered_multiset<T, std::hash<T>, std::equal_to<T>,
        std::scoped_allocator_adaptor<std_allocator<T, RawAllocator>>>;

    /// \copydoc vector
    template <typename Key, typename Value, class RawAllocator>
    using unordered_map = std::unordered_map<Key, Value, std::hash<Key>, std::equal_to<Key>,
        std_allocator<std::pair<const Key, Value>, RawAllocator>>;
    /// \copydoc vector_scoped_alloc
    template <typename Key, typename Value, class RawAllocator>
    using unordered_map_scoped_alloc = std::unordered_map<Key, Value, std::hash<Key>, std::equal_to<Key>,
        std::scoped_allocator_adaptor<
        std_allocator<std::pair<const Key, Value>, RawAllocator>>>;

    /// \copydoc vector
    template <typename Key, typename Value, class RawAllocator>
    using unordered_multimap = std::unordered_multimap<Key, Value, std::hash<Key>, std::equal_to<Key>,
        std_allocator<std::pair<const Key, Value>, RawAllocator>>;
    /// \copydoc vector_scoped_alloc
    template <typename Key, typename Value, class RawAllocator>
    using unordered_multimap_scoped_alloc = std::unordered_multimap<Key, Value, std::hash<Key>, std::equal_to<Key>,
        std::scoped_allocator_adaptor<
        std_allocator<std::pair<const Key, Value>, RawAllocator>>>;

    /// \copydoc vector
    template <typename T, class RawAllocator>
    using stack = std::stack<T, deque<T, RawAllocator>>;
    /// \copydoc vector_scoped_alloc
    template <typename T, class RawAllocator>
    using stack_scoped_alloc = std::stack<T, deque_scoped_alloc<T, RawAllocator>>;

    /// \copydoc vector
    template <typename T, class RawAllocator>
    using queue = std::queue<T, deque<T, RawAllocator>>;
    /// \copydoc vector_scoped_alloc
    template <typename T, class RawAllocator>
    using queue_scoped_alloc = std::queue<T, deque_scoped_alloc<T, RawAllocator>>;

    /// \copydoc vector
    template <typename T, class RawAllocator>
    using priority_queue = std::priority_queue<T, deque<T, RawAllocator>>;
    /// \copydoc vector_scoped_alloc
    template <typename T, class RawAllocator>
    using priority_queue_scoped_alloc = std::priority_queue<T, deque_scoped_alloc<T, RawAllocator>>;

    /// \copydoc vector
    template <class RawAllocator>
    using string = std::basic_string<char, std::char_traits<char>, std_allocator<char, RawAllocator>>;
    /// @}

    /// @{
    /// Convenience function to create a container adapter using a certain
    /// \concept{concept_rawallocator,RawAllocator}. \returns An empty adapter with an
    /// implementation container using a reference to a given allocator. \ingroup adapter
    template <typename T, class RawAllocator, class Container = deque<T, RawAllocator>>
    std::stack<T, Container> make_stack(RawAllocator& allocator)
    {
      return std::stack<T, Container>{Container(allocator)};
    }

    /// \copydoc make_stack
    template <typename T, class RawAllocator, class Container = deque<T, RawAllocator>>
    std::queue<T, Container> make_queue(RawAllocator& allocator)
    {
      return std::queue<T, Container>{Container(allocator)};
    }

    /// \copydoc make_stack
    template <typename T, class RawAllocator, class Container = deque<T, RawAllocator>,
        class Compare = std::less<T>>
    std::priority_queue<T, Container, Compare> make_priority_queue(RawAllocator& allocator,
                                                                   Compare       comp = {})
    {
      return std::priority_queue<T, Container, Compare>{detail::move(comp),
                                                        Container(allocator)};
    }
    /// @}
}

#endif//VALKYRIE_MEMORY_CONTAINER_HPP
