//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_CORE_ADT_FLAT_SET_HPP
#define VALKYRIE_CORE_ADT_FLAT_SET_HPP

#include "small_array.hpp"
#include <valkyrie/status/maybe.hpp>
#include <valkyrie/traits.hpp>


#include <algorithm>


namespace valkyrie{

  inline namespace concepts{
    template <typename T, typename U>
    concept KeyFor = not_same_as<T, U> && equality_comparable_with<T, U>;
    template <typename T, typename U>
    concept OrderedKeyFor = !std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>> && ordered_with<T, U>;
  }

  inline constexpr static struct GuaranteeUniqueTag{} guaranteeUnique;

  template </*concepts::ordered*/typename T, typename Container = small_array<T>>
  class flat_set {
    using in_param_t = param_t<T>;
    using container_type = Container;
  public:
    using value_type = T;
    using allocator_type = typename container_type::allocator_type;
    using size_type = typename container_type::size_type;
    using difference_type = typename container_type::difference_type;

    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using pointer = typename container_type::pointer;
    using const_pointer = typename container_type::const_pointer;

    using maybe_iterator = std::optional<iterator>;
    using maybe_const_iterator = std::optional<const_iterator>;

    constexpr flat_set() = default;
    template <std::input_iterator I, std::sentinel_for<I> S> requires(std::convertible_to<std::iter_reference_t<I>, value_type>)
    constexpr flat_set(I&& first, S&& last) : m_set_(std::forward<I>(first), std::forward<S>(last)){
      priv_prune_();
    }
    constexpr flat_set(std::initializer_list<T> List) : m_set_(List) {
      priv_prune_();
    }
    template <typename ...Args> requires(ConstructibleFrom<container_type, Args...>)
    constexpr explicit flat_set(std::in_place_t, Args&& ...args) noexcept
        : m_set_{ std::forward<Args>(args)... }{
      priv_prune_();
    }
    template <std::input_iterator I, std::sentinel_for<I> S> requires(std::convertible_to<std::iter_reference_t<I>, value_type>)
    constexpr flat_set(GuaranteeUniqueTag, I&& first, S&& last) : m_set_(std::forward<I>(first), std::forward<S>(last)){
      priv_sort_();
    }
    constexpr flat_set(GuaranteeUniqueTag, std::initializer_list<T> List) : m_set_(List) {
      priv_sort_();
    }
    template <typename ...Args> requires(ConstructibleFrom<container_type, Args...>)
    constexpr explicit flat_set(GuaranteeUniqueTag, std::in_place_t, Args&& ...args) noexcept
        : m_set_{ std::forward<Args>(args)... }{
      priv_sort_();
    }


    constexpr flat_set & operator&=(const flat_set & Other) noexcept {
      container_type result;
      std::set_intersection(m_set_.begin(), m_set_.end(), Other.m_set_.begin(), Other.m_set_.end(), std::back_inserter(result));
      //std::ranges::set_intersection(m_set_, Other.m_set_, std::back_inserter(result));
      m_set_.swap(result);
      return *this;
    }
    constexpr flat_set & operator|=(const flat_set & Other) noexcept {
      container_type result;
      std::set_union(m_set_.begin(), m_set_.end(), Other.m_set_.begin(), Other.m_set_.end(), std::back_inserter(result));
      //std::ranges::set_union(m_set_, Other.m_set_, std::back_inserter(result));
      m_set_.swap(result);
      return *this;
    }
    constexpr flat_set & operator^=(const flat_set & Other) noexcept {
      container_type result;
      std::set_symmetric_difference(m_set_.begin(), m_set_.end(), Other.m_set_.begin(), Other.m_set_.end(), std::back_inserter(result));
      //std::ranges::set_symmetric_difference(m_set_, Other.m_set_, std::back_inserter(result));
      m_set_.swap(result);
      return *this;
    }





    constexpr bool empty() const noexcept { return m_set_.empty(); }
    constexpr size_type size() const noexcept { return m_set_.size(); }
    constexpr size_type max_size() const noexcept { return m_set_.max_size(); }

    constexpr pointer data() noexcept { return m_set_.data(); }
    constexpr const_pointer data() const noexcept { return m_set_.data(); }

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

    constexpr size_type count(in_param_t Value) const noexcept {
      return this->contains(Value);
    }
    template <concepts::OrderedKeyFor<T> Key>
    constexpr size_type count(const Key& Value) const noexcept {
      return this->contains(Value);
    }


    constexpr bool contains(in_param_t Value) const noexcept {
      return std::binary_search(m_set_.begin(), m_set_.end(), Value);
      //return std::ranges::binary_search(m_set_, Value);
    }
    template <concepts::OrderedKeyFor<T> Key>
    constexpr bool contains(const Key& Value) const noexcept {
      return std::binary_search(m_set_.begin(), m_set_.end(), Value);
    }


    constexpr iterator       find(in_param_t Value) noexcept {
      iterator result = this->lower_bound(Value);
      if (result == end() || *result != Value) return end();
      return result;
    }
    constexpr const_iterator find(in_param_t Value) const noexcept {
      const_iterator result = this->lower_bound(Value);
      if (result == cend() || *result != Value) return cend();
      return result;
    }
    template <concepts::OrderedKeyFor<T> Key>
    constexpr iterator       find(const Key& Value) noexcept {
      iterator result = this->lower_bound(Value);
      if (result == end() || *result != Value) return end();
      return result;
    }
    template <concepts::OrderedKeyFor<T> Key>
    constexpr const_iterator find(const Key& Value) const noexcept {
      const_iterator result = this->lower_bound(Value);
      if (result == cend() || *result != Value) return cend();
      return result;
    }


    constexpr maybe_iterator insert(in_param_t Value) noexcept {
      //iterator iter = std::ranges::lower_bound(m_set_, Value);
      iterator iter = std::lower_bound(m_set_.begin(), m_set_.end(), Value);
      if (iter != end() && *iter == Value) return std::nullopt;
      return m_set_.insert(iter, Value);
    }
    constexpr maybe_iterator insert(value_type&& Value) noexcept requires(!is_register_copyable<value_type>) {
      //iterator iter = std::ranges::lower_bound(m_set_, Value);
      iterator iter = std::lower_bound(m_set_.begin(), m_set_.end(), Value);
      if (iter != end() && *iter == Value) return std::nullopt;
      return m_set_.insert(iter, std::move(Value));
    }
    template <std::input_iterator I, std::sentinel_for<I> S> requires(std::convertible_to<std::iter_reference_t<I>, T>)
    constexpr void           insert(I&& first, S&& last) noexcept {
      while(first != last) {
        m_set_.push_back(*first);
        ++first;
      }
      //m_set_.insert(first, last);
      priv_prune_();
    }
    template <std::ranges::range Rng>  requires(std::convertible_to<std::ranges::range_reference_t<Rng>, T>)
    constexpr void           insert(Rng&& range) noexcept {
      this->insert(std::ranges::begin(range), std::ranges::end(range));
    }


    constexpr iterator erase(const_iterator at) {
      return m_set_.erase(at);
    }
    constexpr iterator erase(const_iterator from, const_iterator to) noexcept {
      return m_set_.erase(from, to);
    }
    constexpr bool     erase(in_param_t Val) noexcept {
      iterator position = this->find(Val);
      if (position == end()) return false;
      m_set_.erase(position);
      return true;
    }
    template <concepts::OrderedKeyFor<T> Key>
    constexpr bool     erase(const Key& Val) noexcept {
      iterator position = this->find(Val);
      if (position == end()) return false;
      m_set_.erase(position);
      return true;
    }

    template <typename ...Args> requires(ConstructibleFrom<T, Args...>)
    constexpr maybe_iterator emplace(Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
      auto& result = m_set_.emplace_back(std::forward<Args>(args)...);
      auto post_size = m_set_.size();
      priv_prune_();
      if (m_set_.size() < post_size)
        return std::nullopt;
      return iterator(std::addressof(result));
    }
    template <ordered_with<T> Key, typename ...Args>
    constexpr iterator find_or_emplace(const Key& key, Args&& ...args)
    noexcept(std::is_nothrow_constructible_v<T, Args...>) {
      iterator bound = this->lower_bound(key);
      if (bound != this->end() && *bound == key)
        return bound;
      return m_set_.emplace(bound, std::forward<Args>(args)...);
    }

    constexpr void clear() noexcept { m_set_.clear(); }





    constexpr iterator       lower_bound(in_param_t Val) noexcept {
      return std::lower_bound(m_set_.begin(), m_set_.end(), Val);
    }
    constexpr const_iterator lower_bound(in_param_t Val) const noexcept {
      return std::lower_bound(m_set_.begin(), m_set_.end(), Val);
    }
    template <concepts::OrderedKeyFor<T> Key>
    constexpr iterator       lower_bound(const Key& Val) noexcept {
      return std::lower_bound(m_set_.begin(), m_set_.end(), Val);
    }
    template <concepts::OrderedKeyFor<T> Key>
    constexpr const_iterator lower_bound(const Key& Val) const noexcept {
      return std::lower_bound(m_set_.begin(), m_set_.end(), Val);
    }


    constexpr iterator       upper_bound(in_param_t Val) noexcept {
      return std::upper_bound(m_set_.begin(), m_set_.end(), Val);
    }
    constexpr const_iterator upper_bound(in_param_t Val) const noexcept {
      return std::upper_bound(m_set_.begin(), m_set_.end(), Val);
    }
    template <concepts::OrderedKeyFor<T> Key>
    constexpr iterator       upper_bound(const Key& Val) noexcept {
      return std::upper_bound(m_set_.begin(), m_set_.end(), Val);
    }
    template <concepts::OrderedKeyFor<T> Key>
    constexpr const_iterator upper_bound(const Key& Val) const noexcept {
      return std::upper_bound(m_set_.begin(), m_set_.end(), Val);
    }


    constexpr void swap(flat_set & Other) noexcept {
      m_set_.swap(Other.m_set_);
    }


    constexpr const container_type& container() const noexcept { return m_set_; }
    constexpr allocator_type get_allocator() const noexcept { return m_set_.get_allocator(); }




    friend constexpr flat_set operator&(const flat_set & A, const flat_set & B) noexcept {
      flat_set C;
      std::set_intersection(A.m_set_.front(), A.m_set_.end(), B.m_set_.begin(), B.m_set_.end(), std::back_inserter(C.m_set_));
      C.priv_prune_();
      return std::move(C);
    }
    friend constexpr flat_set operator|(const flat_set & A, const flat_set & B) noexcept {
      flat_set C;
      std::set_union(A.m_set_.front(), A.m_set_.end(), B.m_set_.begin(), B.m_set_.end(), std::back_inserter(C.m_set_));
      C.priv_prune_();
      return std::move(C);
    }
    friend constexpr flat_set operator^(const flat_set & A, const flat_set & B) noexcept {
      flat_set C;
      std::set_symmetric_difference(A.m_set_.front(), A.m_set_.end(), B.m_set_.begin(), B.m_set_.end(), std::back_inserter(C.m_set_));
      C.priv_prune_();
      return std::move(C);
    }


    friend constexpr bool operator==(const flat_set & A, const flat_set & B) noexcept {
      return std::ranges::equal(A, B);
    }
    friend constexpr auto operator<=>(const flat_set & A, const flat_set & B) noexcept {
      auto&& [iterA, iterB] = std::ranges::mismatch(A, B);
      if (iterA == A.end() || iterB == B.end()) return A.size() <=> B.size();
      return *iterA <=> *iterB;
    }

  private:
    inline constexpr bool checkIsSorted() const noexcept {

      if (empty()) return true;

      const const_iterator last = end();

      for (const_iterator first = begin(), next = first + 1; next != last; ++first, ++next) {
        if (!(*first < *next))
          return false;
      }
      return true;
    }

    inline constexpr void priv_sort_() noexcept {
      std::sort(m_set_.begin(), m_set_.end(), [](in_param_t A, in_param_t B){ return A < B; });
      VK_constexpr_assert(checkIsSorted());
    }
    inline constexpr void priv_sort_() noexcept requires(std::is_const_v<value_type>){
      VK_constexpr_assert(checkIsSorted());
    }
    inline constexpr void priv_unique_() noexcept {
      m_set_.erase(std::unique(m_set_.begin(), m_set_.end()), m_set_.end());
    }
    inline constexpr void priv_prune_() noexcept {
      priv_sort_();
      priv_unique_();
    }

    container_type m_set_{};
  };
}

#endif //VALKYRIE_CORE_ADT_FLAT_SET_HPP
