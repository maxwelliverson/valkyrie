//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_CORE_ADT_FLAT_MAP_HPP
#define VALKYRIE_CORE_ADT_FLAT_MAP_HPP

#include "flat_set.hpp"

namespace valkyrie{
  template </*concepts::ordered*/typename Key, typename Value>
  struct FlatMapEntry{
    Key key{};
    Value value{};

    using key_t = in_param_t<Key>;
    using value_t = in_param_t<Value>;
    using entry_t = std::conditional_t<is_register_copyable<std::pair<Key, Value>>, FlatMapEntry, const FlatMapEntry&>;

    constexpr FlatMapEntry() noexcept = default;
    constexpr FlatMapEntry(const FlatMapEntry&) noexcept(std::is_nothrow_copy_constructible_v<Key> && std::is_nothrow_copy_constructible_v<Value>) = default;
    constexpr FlatMapEntry(FlatMapEntry&&) noexcept(std::is_nothrow_move_constructible_v<Key> && std::is_nothrow_move_constructible_v<Value>) = default;

    constexpr FlatMapEntry& operator=(const FlatMapEntry& other) noexcept(std::is_nothrow_copy_assignable_v<Key> && std::is_nothrow_copy_assignable_v<Value>) = default;
    constexpr FlatMapEntry& operator=(FlatMapEntry&& other) noexcept(std::is_nothrow_move_assignable_v<Key> && std::is_nothrow_move_assignable_v<Value>) = default;

    /*constexpr explicit FlatMapEntry(key_t key) noexcept(std::is_nothrow_copy_constructible_v<Key>)
        : key{key},
          value{}{}*/
    constexpr FlatMapEntry(key_t key, value_t value) noexcept(std::is_nothrow_copy_constructible_v<Key> && std::is_nothrow_copy_constructible_v<Value>)
        : key{key},
          value{value}{}


    template <typename ...Args>
    requires(constructible_from<Value, Args...> && !(sizeof...(Args) == 1 && (same_as<Args, value_t> || ...)))
    constexpr explicit FlatMapEntry(key_t key, Args&&...args) noexcept(
    std::is_nothrow_copy_constructible_v<Key> &&
    std::is_nothrow_constructible_v<Value, Args...>)
        : key{key},
          value{ std::forward<Args>(args)... }{}
    template <typename ...KArgs, typename ...VArgs>
    requires(constructible_from<Key, KArgs...> && constructible_from<Value, VArgs...>)
    constexpr FlatMapEntry(std::tuple<KArgs...>&& kargs, std::tuple<VArgs...>&& vargs) noexcept(
    std::is_nothrow_constructible_v<Key, KArgs...> &&
    std::is_nothrow_constructible_v<Value, VArgs...>)
        : key{std::make_from_tuple<Key>(std::move(kargs))},
          value{std::make_from_tuple<Value>(std::move(vargs))}{}
    template <typename K, typename V> requires(constructible_from<Key, K> && constructible_from<Value, V>)
    constexpr FlatMapEntry(K&& k, V&& v) noexcept(std::is_nothrow_constructible_v<Key, K> && std::is_nothrow_constructible_v<Value, V>)
        : key{std::forward<K>(k)},
          value{std::forward<V>(v)}{}


    constexpr void swap(FlatMapEntry& Other) noexcept(requires{
      { std::swap(key, Other.key) } noexcept;
      { std::swap(value, Other.value) } noexcept;
    }){
      std::swap(key, Other.key);
      std::swap(value, Other.value);
    }

    VK_nodiscard friend constexpr bool operator==(entry_t A, entry_t B) noexcept {
      return A.key == B.key;
    }
    VK_nodiscard friend constexpr auto operator<=>(entry_t A, entry_t B) noexcept {
      return A.key <=> B.key;
    }

    VK_nodiscard friend constexpr bool operator==(entry_t A, key_t K) noexcept {
      return A.key == K;
    }
    VK_nodiscard friend constexpr auto operator<=>(entry_t A, key_t K) noexcept {
      return A.key <=> K;
    }

    template <not_same_as_one_of<Key, FlatMapEntry> K> requires(requires(entry_t A, K&& k){ { A.key == std::forward<K>(k) } -> std::convertible_to<bool>; })
    VK_nodiscard friend constexpr bool operator==(entry_t A, K&& k) noexcept {
      return A.key == std::forward<K>(k);
    }
    template <not_same_as_one_of<Key, FlatMapEntry> K> requires(ordered_with<K, Key>)
    VK_nodiscard friend constexpr auto operator<=>(entry_t A, K&& k) noexcept {
      return A.key <=> std::forward<K>(k);
    }
  };

  template <typename K, typename V>
  FlatMapEntry(K&&, V&&) -> FlatMapEntry<std::remove_cvref_t<K>, std::remove_cvref_t<V>>;

  template <ordered Key,
            typename Value,
            allocator_c Alloc = default_allocator,
            u32 InlineEl = 4>
  class flat_map {

    using entry_t = FlatMapEntry<Key, Value>;

    using key_param_t = in_param_t<Key>;
    using value_param_t = in_param_t<Value>;
    using entry_param_t = in_param_t<entry_t>;

  public:

    using key_type = Key;
    using mapped_type = Value;
    using value_type = entry_t;
    using container_type = flat_set<value_type, Alloc, std::compare_three_way, InlineEl>;
    using size_type = typename container_type::size_type;
    using difference_type = typename container_type::difference_type;

    using reference = value_type&;
    using const_reference = const value_type&;
    using value_reference = Value&;
    using const_value_reference = const Value&;

    using allocator_type = typename container_type::allocator_type;

    using pointer = typename container_type::pointer;
    using const_pointer = typename container_type::const_pointer;

    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using reverse_iterator = typename container_type::reverse_iterator;
    using const_reverse_iterator = typename container_type::const_reverse_iterator;

    using maybe_iterator = typename container_type::maybe_iterator;
    using maybe_const_iterator = typename container_type::maybe_const_iterator;




    constexpr flat_map() = default;
    template <std::input_iterator I, std::sentinel_for<I> S> requires(std::convertible_to<std::iter_reference_t<I>, value_type>)
    constexpr flat_map(I&& first, S&& last)
        : m_set_( std::forward<I>(first), std::forward<S>(last) ){}
    constexpr flat_map(std::initializer_list<value_type> list) noexcept
        : m_set_{ list }{}
    template <typename ...Args> requires(constructible_from<Container, Args...>)
    constexpr explicit flat_map(std::in_place_t, Args&& ...args) noexcept
        : m_set_{  std::in_place, std::forward<Args>(args)... }{}
    template <std::input_iterator I, std::sentinel_for<I> S> requires(std::convertible_to<std::iter_reference_t<I>, value_type>)
    constexpr flat_map(GuaranteeUniqueTag, I&& first, S&& last)
        : m_set_(guaranteeUnique, std::forward<I>(first), std::forward<S>(last) ){}
    constexpr flat_map(GuaranteeUniqueTag, std::initializer_list<value_type> list) noexcept
        : m_set_{ guaranteeUnique, list }{}
    template <typename ...Args> requires(constructible_from<Container, Args...>)
    constexpr explicit flat_map(GuaranteeUniqueTag, std::in_place_t, Args&& ...args) noexcept
        : m_set_{ guaranteeUnique, std::in_place, std::forward<Args>(args)... }{}



    // Element Access

    constexpr pointer data() noexcept { return m_set_.data(); }
    constexpr const_pointer data() const noexcept { return m_set_.data(); }

    constexpr value_reference at(key_param_t key) {
      auto result = m_set_.find(key);
#if VK_exceptions_enabled
      if (result == m_set_.end())
        throw std::runtime_error("Invalid key used in flat_map::at");
#endif
      return result->value;
    }
    constexpr const_value_reference at(key_param_t key) const {
      auto result = m_set_.find(key);
#if VK_exceptions_enabled
      if (result == m_set_.end())
        throw std::runtime_error("Invalid key used in flat_map::at");
#endif
      return result->value;
    }
    constexpr value_reference operator[](key_param_t key) noexcept {
      return m_set_.find_or_emplace(key, key)->value;
    }


    // Iterators


    constexpr iterator        begin()       noexcept { return m_set_.begin(); }
    constexpr const_iterator  begin() const noexcept { return m_set_.begin(); }
    constexpr const_iterator cbegin() const noexcept { return m_set_.cbegin(); }

    constexpr iterator        end()       noexcept { return m_set_.end(); }
    constexpr const_iterator  end() const noexcept { return m_set_.end(); }
    constexpr const_iterator cend() const noexcept { return m_set_.cend(); }

    constexpr reverse_iterator        rbegin()       noexcept { return reverse_iterator(end()); }
    constexpr const_reverse_iterator  rbegin() const noexcept { return const_reverse_iterator(end()); }
    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

    constexpr reverse_iterator        rend()       noexcept { return reverse_iterator(begin()); }
    constexpr const_reverse_iterator  rend() const noexcept { return const_reverse_iterator(begin()); }
    constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }


    // Capacity

    constexpr bool empty() const noexcept { return m_set_.empty(); }
    constexpr size_type size() const noexcept { return m_set_.size(); }
    constexpr size_type max_size() const noexcept { return m_set_.max_size(); }


    // Modifiers

    constexpr void clear() noexcept { m_set_.clear(); }

    constexpr maybe_iterator insert(entry_param_t entry) {
      return m_set_.insert(entry);
    }
    constexpr maybe_iterator insert(value_type&& entry) requires(!is_register_copyable<value_type>) {
      return m_set_.insert(std::move(entry));
    }
    template <std::input_iterator I, std::sentinel_for<I> S> requires(std::convertible_to<std::iter_reference_t<I>, value_type>)
    constexpr void           insert(I&& beginIter, S&& endIter) noexcept {
      m_set_.insert(std::forward<I>(beginIter), std::forward<S>(endIter));
    }
    template <std::ranges::range Rng>  requires(std::convertible_to<std::ranges::range_reference_t<Rng>, value_type>)
    constexpr void           insert(Rng&& rng) noexcept {
      m_set_.insert(std::forward<Rng>(rng));
    }

    template <typename ...Args> requires(constructible_from<Value, Args...>)
    constexpr iterator emplace(key_param_t key, Args&& ...args) noexcept(std::is_nothrow_constructible_v<Value, Args...>) {
      return m_set_.find_or_emplace(key, key, std::forward<Args>(args)...);
    }


    /*template <typename ...Args> requires(constructible_from<Value, Args...>)
    constexpr iterator find_or_emplace(key_param_t key, Args&& ...args) noexcept {
      return m_set_.find_or_emplace(key, key, std::forward<Args>(args)...);
    }*/

    constexpr iterator erase(const_iterator pos) {
      return m_set_.erase(pos);
    }
    constexpr iterator erase(const_iterator from, const_iterator to) {
      return m_set_.erase(from, to);
    }
    constexpr bool erase(key_param_t key) {
      return m_set_.erase(key);
    }
    template <concepts::OrderedKeyFor<value_type> K>
    constexpr bool erase(const K& key) {
      return m_set_.erase(key);
    }

    constexpr void swap(flat_map & other) noexcept(requires{ { m_set_.swap(other.m_set_) } noexcept; }) {
      m_set_.swap(other.m_set_);
    }



    // Lookup

    constexpr size_type count(key_param_t key) const noexcept {
      return m_set_.contains(key);
    }
    template <concepts::OrderedKeyFor<value_type> K>
    constexpr size_type count(const K& key) const noexcept {
      return m_set_.contains(key);
    }

    constexpr iterator find(key_param_t key) noexcept {
      return m_set_.find(key);
    }
    constexpr const_iterator find(key_param_t key) const noexcept {
      return m_set_.find(key);
    }
    template <concepts::OrderedKeyFor<value_type> K>
    constexpr iterator find(const K& key) noexcept {
      return m_set_.find(key);
    }
    template <concepts::OrderedKeyFor<value_type> K>
    constexpr const_iterator find(const K& key) const noexcept {
      return m_set_.find(key);
    }

    constexpr bool contains(key_param_t key) const noexcept {
      return m_set_.contains(key);
    }
    template <concepts::OrderedKeyFor<value_type> K>
    constexpr bool contains(const K& key) const noexcept {
      return m_set_.contains(key);
    }

    constexpr iterator lower_bound(key_param_t key) noexcept {
      return m_set_.lower_bound(key);
    }
    constexpr const_iterator lower_bound(key_param_t key) const noexcept {
      return m_set_.lower_bound(key);
    }
    template <concepts::OrderedKeyFor<value_type> K>
    constexpr iterator lower_bound(const K& key) noexcept {
      return m_set_.lower_bound(key);
    }
    template <concepts::OrderedKeyFor<value_type> K>
    constexpr const_iterator lower_bound(const K& key) const noexcept {
      return m_set_.lower_bound(key);
    }

    constexpr iterator upper_bound(key_param_t key) noexcept {
      return m_set_.upper_bound(key);
    }
    constexpr const_iterator upper_bound(key_param_t key) const noexcept {
      return m_set_.upper_bound(key);
    }
    template <concepts::OrderedKeyFor<value_type> K>
    constexpr iterator upper_bound(const K& key) noexcept {
      return m_set_.upper_bound(key);
    }
    template <concepts::OrderedKeyFor<value_type> K>
    constexpr const_iterator upper_bound(const K& key) const noexcept {
      return m_set_.upper_bound(key);
    }


  private:

    container_type m_set_{};
  };
}

#endif //VALKYRIE_CORE_ADT_FLAT_MAP_HPP
