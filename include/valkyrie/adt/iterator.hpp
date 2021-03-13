//
// Created by maxwe on 2021-03-12.
//

#ifndef VALKYRIE_ADT_ITERATOR_HPP
#define VALKYRIE_ADT_ITERATOR_HPP

#include <iterator>
#include <utility>

namespace valkyrie{

  /// A range adaptor for a pair of iterators.
///
/// This just wraps two iterators into a range-compatible interface. Nothing
/// fancy at all.
  template <typename IteratorT>
  class iterator_range {
    IteratorT begin_iterator, end_iterator;

  public:
    //TODO: Add SFINAE to test that the Container's iterators match the range's
    //      iterators.
    template <typename Container>
    iterator_range(Container &&c)
    //TODO: Consider ADL/non-member begin/end calls.
        : begin_iterator(c.begin()), end_iterator(c.end()) {}
    iterator_range(IteratorT begin_iterator, IteratorT end_iterator)
        : begin_iterator(std::move(begin_iterator)),
          end_iterator(std::move(end_iterator)) {}

    IteratorT begin() const { return begin_iterator; }
    IteratorT end() const { return end_iterator; }
    bool empty() const { return begin_iterator == end_iterator; }
  };

/// Convenience function for iterating over sub-ranges.
///
/// This provides a bit of syntactic sugar to make using sub-ranges
/// in for loops a bit easier. Analogous to std::make_pair().
  template <class T> iterator_range<T> make_range(T x, T y) {
    return iterator_range<T>(std::move(x), std::move(y));
  }

  template <typename T> iterator_range<T> make_range(std::pair<T, T> p) {
    return iterator_range<T>(std::move(p.first), std::move(p.second));
  }

  template <typename DerivedT, typename IteratorCategoryT, typename T,
      typename DifferenceTypeT = std::ptrdiff_t, typename PointerT = T *,
      typename ReferenceT = T &>
  class iterator_facade_base
      : public std::iterator<IteratorCategoryT, T, DifferenceTypeT, PointerT,
          ReferenceT> {
  protected:
    enum {
      IsRandomAccess = std::is_base_of<std::random_access_iterator_tag,
          IteratorCategoryT>::value,
      IsBidirectional = std::is_base_of<std::bidirectional_iterator_tag,
          IteratorCategoryT>::value,
    };

    /// A proxy object for computing a reference via indirecting a copy of an
    /// iterator. This is used in APIs which need to produce a reference via
    /// indirection but for which the iterator object might be a temporary. The
    /// proxy preserves the iterator internally and exposes the indirected
    /// reference via a conversion operator.
    class ReferenceProxy {
      friend iterator_facade_base;

      DerivedT I;

      ReferenceProxy(DerivedT I) : I(std::move(I)) {}

    public:
      operator ReferenceT() const { return *I; }
    };

  public:
    DerivedT operator+(DifferenceTypeT n) const {
      static_assert(std::is_base_of<iterator_facade_base, DerivedT>::value,
                    "Must pass the derived type to this template!");
      static_assert(
          IsRandomAccess,
          "The '+' operator is only defined for random access iterators.");
      DerivedT tmp = *static_cast<const DerivedT *>(this);
      tmp += n;
      return tmp;
    }
    friend DerivedT operator+(DifferenceTypeT n, const DerivedT &i) {
      static_assert(
          IsRandomAccess,
          "The '+' operator is only defined for random access iterators.");
      return i + n;
    }
    DerivedT operator-(DifferenceTypeT n) const {
      static_assert(
          IsRandomAccess,
          "The '-' operator is only defined for random access iterators.");
      DerivedT tmp = *static_cast<const DerivedT *>(this);
      tmp -= n;
      return tmp;
    }

    DerivedT &operator++() {
      static_assert(std::is_base_of<iterator_facade_base, DerivedT>::value,
                    "Must pass the derived type to this template!");
      return static_cast<DerivedT *>(this)->operator+=(1);
    }
    DerivedT operator++(int) {
      DerivedT tmp = *static_cast<DerivedT *>(this);
      ++*static_cast<DerivedT *>(this);
      return tmp;
    }
    DerivedT &operator--() {
      static_assert(
          IsBidirectional,
          "The decrement operator is only defined for bidirectional iterators.");
      return static_cast<DerivedT *>(this)->operator-=(1);
    }
    DerivedT operator--(int) {
      static_assert(
          IsBidirectional,
          "The decrement operator is only defined for bidirectional iterators.");
      DerivedT tmp = *static_cast<DerivedT *>(this);
      --*static_cast<DerivedT *>(this);
      return tmp;
    }

    bool operator!=(const DerivedT &RHS) const {
      return !static_cast<const DerivedT *>(this)->operator==(RHS);
    }

    bool operator>(const DerivedT &RHS) const {
      static_assert(
          IsRandomAccess,
          "Relational operators are only defined for random access iterators.");
      return !static_cast<const DerivedT *>(this)->operator<(RHS) &&
             !static_cast<const DerivedT *>(this)->operator==(RHS);
    }
    bool operator<=(const DerivedT &RHS) const {
      static_assert(
          IsRandomAccess,
          "Relational operators are only defined for random access iterators.");
      return !static_cast<const DerivedT *>(this)->operator>(RHS);
    }
    bool operator>=(const DerivedT &RHS) const {
      static_assert(
          IsRandomAccess,
          "Relational operators are only defined for random access iterators.");
      return !static_cast<const DerivedT *>(this)->operator<(RHS);
    }

    PointerT operator->() { return &static_cast<DerivedT *>(this)->operator*(); }
    PointerT operator->() const {
      return &static_cast<const DerivedT *>(this)->operator*();
    }
    ReferenceProxy operator[](DifferenceTypeT n) {
      static_assert(IsRandomAccess,
                    "Subscripting is only defined for random access iterators.");
      return ReferenceProxy(static_cast<DerivedT *>(this)->operator+(n));
    }
    ReferenceProxy operator[](DifferenceTypeT n) const {
      static_assert(IsRandomAccess,
                    "Subscripting is only defined for random access iterators.");
      return ReferenceProxy(static_cast<const DerivedT *>(this)->operator+(n));
    }
  };

  /// CRTP base class for adapting an iterator to a different type.
///
/// This class can be used through CRTP to adapt one iterator into another.
/// Typically this is done through providing in the derived class a custom \c
/// operator* implementation. Other methods can be overridden as well.
  template <
      typename DerivedT, typename WrappedIteratorT,
      typename IteratorCategoryT =
      typename std::iterator_traits<WrappedIteratorT>::iterator_category,
      typename T = typename std::iterator_traits<WrappedIteratorT>::value_type,
      typename DifferenceTypeT =
      typename std::iterator_traits<WrappedIteratorT>::difference_type,
      typename PointerT = std::conditional_t<
                          std::is_same<T, typename std::iterator_traits<
                              WrappedIteratorT>::value_type>::value,
      typename std::iterator_traits<WrappedIteratorT>::pointer, T *>,
  typename ReferenceT = std::conditional_t<
                        std::is_same<T, typename std::iterator_traits<
                            WrappedIteratorT>::value_type>::value,
  typename std::iterator_traits<WrappedIteratorT>::reference, T &>>
  class iterator_adaptor_base
      : public iterator_facade_base<DerivedT, IteratorCategoryT, T,
          DifferenceTypeT, PointerT, ReferenceT> {
    using BaseT = typename iterator_adaptor_base::iterator_facade_base;

  protected:
    WrappedIteratorT I;

    iterator_adaptor_base() = default;

    explicit iterator_adaptor_base(WrappedIteratorT u) : I(std::move(u)) {
      static_assert(std::is_base_of<iterator_adaptor_base, DerivedT>::value,
                    "Must pass the derived type to this template!");
    }

    const WrappedIteratorT &wrapped() const { return I; }

  public:
    using difference_type = DifferenceTypeT;

    DerivedT &operator+=(difference_type n) {
      static_assert(
          BaseT::IsRandomAccess,
          "The '+=' operator is only defined for random access iterators.");
      I += n;
      return *static_cast<DerivedT *>(this);
    }
    DerivedT &operator-=(difference_type n) {
      static_assert(
          BaseT::IsRandomAccess,
          "The '-=' operator is only defined for random access iterators.");
      I -= n;
      return *static_cast<DerivedT *>(this);
    }
    using BaseT::operator-;
    difference_type operator-(const DerivedT &RHS) const {
      static_assert(
          BaseT::IsRandomAccess,
          "The '-' operator is only defined for random access iterators.");
      return I - RHS.I;
    }

    // We have to explicitly provide ++ and -- rather than letting the facade
    // forward to += because WrappedIteratorT might not support +=.
    using BaseT::operator++;
    DerivedT &operator++() {
      ++I;
      return *static_cast<DerivedT *>(this);
    }
    using BaseT::operator--;
    DerivedT &operator--() {
      static_assert(
          BaseT::IsBidirectional,
          "The decrement operator is only defined for bidirectional iterators.");
      --I;
      return *static_cast<DerivedT *>(this);
    }

    bool operator==(const DerivedT &RHS) const { return I == RHS.I; }
    bool operator<(const DerivedT &RHS) const {
      static_assert(
          BaseT::IsRandomAccess,
          "Relational operators are only defined for random access iterators.");
      return I < RHS.I;
    }

    ReferenceT operator*() const { return *I; }
  };

/// An iterator type that allows iterating over the pointees via some
/// other iterator.
///
/// The typical usage of this is to expose a type that iterates over Ts, but
/// which is implemented with some iterator over T*s:
///
/// \code
///   using iterator = pointee_iterator<SmallVectorImpl<T *>::iterator>;
/// \endcode
  template <typename WrappedIteratorT,
      typename T = std::remove_reference_t<decltype(
          **std::declval<WrappedIteratorT>())>>
  struct pointee_iterator
      : iterator_adaptor_base<
          pointee_iterator<WrappedIteratorT, T>, WrappedIteratorT,
          typename std::iterator_traits<WrappedIteratorT>::iterator_category,
          T> {
    pointee_iterator() = default;
    template <typename U>
    pointee_iterator(U &&u)
        : pointee_iterator::iterator_adaptor_base(std::forward<U &&>(u)) {}

    T &operator*() const { return **this->I; }
  };

  template <typename RangeT, typename WrappedIteratorT =
  decltype(std::begin(std::declval<RangeT>()))>
  iterator_range<pointee_iterator<WrappedIteratorT>>
  make_pointee_range(RangeT &&Range) {
    using PointeeIteratorT = pointee_iterator<WrappedIteratorT>;
    return make_range(PointeeIteratorT(std::begin(std::forward<RangeT>(Range))),
                      PointeeIteratorT(std::end(std::forward<RangeT>(Range))));
  }

  template <typename WrappedIteratorT,
      typename T = decltype(&*std::declval<WrappedIteratorT>())>
  class pointer_iterator
      : public iterator_adaptor_base<
          pointer_iterator<WrappedIteratorT, T>, WrappedIteratorT,
          typename std::iterator_traits<WrappedIteratorT>::iterator_category,
          T> {
    mutable T Ptr;

  public:
    pointer_iterator() = default;

    explicit pointer_iterator(WrappedIteratorT u)
        : pointer_iterator::iterator_adaptor_base(std::move(u)) {}

    T &operator*() { return Ptr = &*this->I; }
    const T &operator*() const { return Ptr = &*this->I; }
  };

  template <typename RangeT, typename WrappedIteratorT =
  decltype(std::begin(std::declval<RangeT>()))>
  iterator_range<pointer_iterator<WrappedIteratorT>>
  make_pointer_range(RangeT &&Range) {
    using PointerIteratorT = pointer_iterator<WrappedIteratorT>;
    return make_range(PointerIteratorT(std::begin(std::forward<RangeT>(Range))),
                      PointerIteratorT(std::end(std::forward<RangeT>(Range))));
  }

  template <typename WrappedIteratorT,
      typename T1 = std::remove_reference_t<decltype(
          **std::declval<WrappedIteratorT>())>,
  typename T2 = std::add_pointer_t<T1>>
  using raw_pointer_iterator =
  pointer_iterator<pointee_iterator<WrappedIteratorT, T1>, T2>;

// Wrapper iterator over iterator ItType, adding DataRef to the type of ItType,
// to create NodeRef = std::pair<InnerTypeOfItType, DataRef>.
  template <typename ItType, typename NodeRef, typename DataRef>
  class WrappedPairNodeDataIterator
      : public iterator_adaptor_base<
          WrappedPairNodeDataIterator<ItType, NodeRef, DataRef>, ItType,
          typename std::iterator_traits<ItType>::iterator_category, NodeRef,
          std::ptrdiff_t, NodeRef *, NodeRef &> {
    using BaseT = iterator_adaptor_base<
        WrappedPairNodeDataIterator, ItType,
        typename std::iterator_traits<ItType>::iterator_category, NodeRef,
        std::ptrdiff_t, NodeRef *, NodeRef &>;

    const DataRef DR;
    mutable NodeRef NR;

  public:
    WrappedPairNodeDataIterator(ItType Begin, const DataRef DR)
        : BaseT(Begin), DR(DR) {
      NR.first = DR;
    }

    NodeRef &operator*() const {
      NR.second = *this->I;
      return NR;
    }
  };


}

#endif//VALKYRIE_ADT_ITERATOR_HPP
