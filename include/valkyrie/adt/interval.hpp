//
// Created by Maxwell on 2020-11-13.
//

#ifndef VALKYRIE_ADT_INTERVAL_HPP
#define VALKYRIE_ADT_INTERVAL_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie{

  template <typename Comp, typename T>
  concept order_on = requires(Comp&& comp, const T& a, const T& b){
    { comp(a, b) } -> std::convertible_to<std::partial_ordering>;
  };

  enum class interval_order{
    less,
    partial_less,
    equal,
    contained_by,
    contains,
    partial_greater,
    greater
  };

  namespace impl{

    template <auto Value>
    struct interval_lower_bound;
    template <typename T>
    struct interval_mixin;
    template <auto Value>
    struct interval_upper_bound;
    template <auto minValue, auto maxValue>
    struct interval_bounds;

    template <typename T, typename Opt>
    class interval_storage;
    template <typename T>
    class interval_storage<T, void> {
      using param_t = in_param_t<T>;
    public:
      constexpr interval_storage() = default;
      constexpr interval_storage(param_t minValue, param_t maxValue) noexcept : min(minValue), max(maxValue){}

      T min;
      T max;
    };
    template <typename T, std::convertible_to<T> U, U Value>
    class interval_storage<T, interval_lower_bound<Value>>{
      using param_t = in_param_t<T>;
    public:
      constexpr interval_storage() = default;
      constexpr interval_storage(param_t maxValue) noexcept : max(maxValue){}

      constexpr static T min = Value;
      T max;
    };
    template <typename T, std::convertible_to<T> U, U Value>
    class interval_storage<T, interval_upper_bound<Value>>{
      using param_t = in_param_t<T>;
    public:
      constexpr interval_storage() = default;
      constexpr interval_storage(param_t minValue) noexcept : min(minValue){}

      T min;
      constexpr static T max = Value;
    };
    template <typename T, std::convertible_to<T> U, std::convertible_to<T> V, U MinVal, V MaxVal>
    class interval_storage<T, interval_bounds<MinVal, MaxVal>>{
      using param_t = in_param_t<T>;
    public:
      constexpr interval_storage() = default;

      constexpr static T min = MinVal;
      constexpr static T max = MaxVal;
    };
    template <typename T, typename U>
    class interval_storage<T, interval_mixin<U>> : public U{};

    template <typename T, typename ...Opt>
    class interval_base : public interval_storage<T, Opt> ... {
    public:
      using interval_storage<T, Opt>::interval_storage...;
    };
    template <typename T>
    class interval_base<T> : public interval_storage<T, void> {
    public:
      using interval_storage<T, void>::interval_storage;
    };
  }

  template <typename From, typename To = From>
  class interval_projection;

  template <typename T, typename ...Opt>
  class interval : public impl::interval_base<T, Opt...> {
    using param_t = in_param_t<T>;
    //using interval_param_t = in_param_t<interval>;
    using base_t = impl::interval_base<T, Opt...>;
  public:
    using base_t::base_t;

    VK_nodiscard inline constexpr size_t size() const noexcept requires(requires{
      { this->max - this->min } -> std::convertible_to<size_t>;
    }){
      return this->max - this->min;
    }

    VK_nodiscard inline constexpr bool contains(param_t Value) const noexcept{
      return this->min <= Value && Value < this->max;
    }
    template <std::predicate<param_t, param_t> Compare>
    VK_nodiscard inline constexpr bool contains(param_t Value, Compare&& Comp) const noexcept{
      return !Comp(Value, this->min) && Comp(Value, this->max);
    }
    template <weak_order<T> Compare>
    VK_nodiscard inline constexpr bool contains(param_t Value, Compare&& Comp) const noexcept{
      return (Comp(Value, this->min) != std::partial_ordering::greater) && (Comp(Value, this->max) == std::partial_ordering::less);
    }

    template <typename ...OMix>
    VK_nodiscard inline constexpr interval_order order(const interval<T, OMix...>& other) const noexcept{
      auto Lower = this->min <=> other.min;
      auto Upper = this->max <=> other.max;
      if (Lower == Upper) VK_likely {
        if (Lower == std::partial_ordering::less)
          return this->max < other.min ? interval_order::less : interval_order::partial_less;
        if (Lower == std::partial_ordering::greater)
          return other.max < this->min ? interval_order::greater : interval_order::partial_greater;
        return interval_order::equal;
      }

      if (Lower == std::partial_ordering::less || Upper == std::partial_ordering::greater)
        return interval_order::contains;
      return interval_order::contained_by;
    }
    template <std::predicate<param_t, param_t> Compare, typename ...OMix>
    VK_nodiscard inline constexpr interval_order order(const interval<T, OMix...>& other, Compare&& Comp) const noexcept{
      if (Comp(this->min, other.min)) {
        if (Comp(this->max, other.min))
          return interval_order::less;
        if (Comp(this->max, other.max))
          return interval_order::partial_less;
        return interval_order::contains;
      }
      if (Comp(other.max, this->max)) {
        if (Comp(other.max, this->min))
          return interval_order::greater;
        if (Comp(other.min, this->min))
          return interval_order::partial_greater;
        return interval_order::contains;
      }
      VK_unlikely
      if (Comp(other.min, this->min) || Comp(this->max, other.max))
        return interval_order::contained_by;
      return interval_order::equal;
    }
    template <weak_order<T> Compare, typename ...OMix>
    VK_nodiscard inline constexpr interval_order order(const interval<T, OMix...>& other, Compare&& Comp) const noexcept{
      auto Lower = Comp(this->min, other.min);
      auto Upper = Comp(this->max, other.max);
      if (Lower == Upper) VK_likely {
        if (Lower == std::partial_ordering::less)
          return (Comp(this->max, other.min) == std::partial_ordering::less) ? interval_order::less : interval_order::partial_less;
        if (Lower == std::partial_ordering::greater)
          return (Comp(other.max, this->min) == std::partial_ordering::less) ? interval_order::greater : interval_order::partial_greater;
        return interval_order::equal;
      }

      if (Lower == std::partial_ordering::less || Upper == std::partial_ordering::greater)
        return interval_order::contains;
      return interval_order::contained_by;
    }

    template <typename ...OMix>
    VK_nodiscard inline constexpr interval intersect(const interval<T, OMix...>& other) const noexcept{
      return intersect_internal(other, this->order(other));
    }
    template <std::predicate<param_t, param_t> Compare, typename ...OMix>
    VK_nodiscard inline constexpr interval intersect(const interval<T, OMix...>& other, Compare&& comp) const noexcept{
      return intersect_internal(other, this->order(other, std::forward<Compare>(comp)));
    }
    template <weak_order<T> Compare, typename ...OMix>
    VK_nodiscard inline constexpr interval intersect(const interval<T, OMix...>& other, Compare&& comp) const noexcept{
      return intersect_internal(other, this->order(other, std::forward<Compare>(comp)));
    }


    template <typename ...OMix>
    VK_nodiscard inline constexpr std::partial_ordering compare(const interval<T, OMix...>& other) const noexcept{
      return compare_internal(this->order(other));
    }
    template <std::predicate<param_t, param_t> Compare, typename ...OMix>
    VK_nodiscard inline constexpr std::partial_ordering compare(const interval<T, OMix...>& other, Compare&& Comp) const noexcept{
      return compare_internal(this->order(other, std::forward<Compare>(Comp)));
    }
    template <weak_order<T> Compare, typename ...OMix>
    VK_nodiscard inline constexpr std::partial_ordering compare(const interval<T, OMix...>& other, Compare&& Comp) const noexcept{
      return compare_internal(this->order(other, std::forward<Compare>(Comp)));
    }


    constexpr explicit operator bool() const noexcept{
      return this->min != this->max;
    }
    
    template <typename ...OMix>
    constexpr bool operator==(const interval<T, OMix...>& B) const noexcept {
      return this->max == B.max && this->min == B.min;
    }
    template <typename ...OMix>
    constexpr std::partial_ordering operator<=>(const interval<T, OMix...>& B) const noexcept{
      return compare_internal(this->order(B));
    }

  private:
    
    template <typename ...OOpts>
    VK_nodiscard inline constexpr interval intersect_internal(const interval<T, OOpts...>& other, interval_order order) const noexcept {
      switch(order) {
        case interval_order::equal:
        case interval_order::contained_by:
          return *this;
        case interval_order::contains:
          return { other.min, other.max };
        case interval_order::partial_less:
          return interval{ other.min, this->max };
        case interval_order::partial_greater:
          return interval{ this->min, other.max };
        case interval_order::greater:
        case interval_order::less:
          return {};
        VK_no_default;
      }
    }
    VK_nodiscard VK_constant std::partial_ordering compare_internal(interval_order Loc) noexcept{
      switch(Loc){
        case interval_order::less:
        case interval_order::partial_less:
          return std::partial_ordering::less;
        case interval_order::equal:
          return std::partial_ordering::equivalent;
        case interval_order::partial_greater:
        case interval_order::greater:
          return std::partial_ordering::greater;
        case interval_order::contained_by:
        case interval_order::contains:
          return std::partial_ordering::unordered;
        VK_no_default;
      }
    }
  };

  template <typename T, typename U>
  interval(T, U) -> interval<std::common_type_t<T, U>>;


  template <typename From, typename To>
  class interval_projection {
    using from_value_t    = From;
    using to_value_t      = To;

    using intermediate_t  = std::conditional_t<
        same_as_one_of<f32, from_value_t, to_value_t> && not_same_as_one_of<f64, from_value_t, to_value_t>,
        f32,
        f64>;

    constexpr interval_projection(intermediate_t coeff, intermediate_t off) noexcept
        : coefficient(coeff), offset(off){}

  public:

    template <typename ...FOpt, typename ...TOpt>
    constexpr interval_projection(const interval<From, FOpt...>& from, const interval<To, TOpt...>& to) noexcept
        : coefficient(intermediate_t(to.max - to.min) / intermediate_t(from.max - from.min)),
          offset(to.min - (from.min * coefficient)){}


    constexpr to_value_t operator()(in_param_t<from_value_t> from) const noexcept {
      return static_cast<to_value_t>(from * coefficient + offset);
    }

    constexpr interval_projection<To, From> invert() const noexcept {
      return interval_projection<To, From>{ 1 / coefficient, -offset / coefficient };
    }

  private:

    intermediate_t  coefficient;
    intermediate_t  offset;
  };

  template <typename From, typename ...FOpt, typename To, typename ...TOpt>
  interval_projection(const interval<From, FOpt...>&, const interval<To, TOpt...>&)
      -> interval_projection<From, To>;

  
  template <typename T, convertible_to<T> auto LowerBound = std::numeric_limits<T>::min(), typename ...Mixins>
  using upper_bound = interval<T, impl::interval_lower_bound<LowerBound>, impl::interval_mixin<Mixins>...>;
  template <typename T, convertible_to<T> auto UpperBound = std::numeric_limits<T>::max(), typename ...Mixins>
  using lower_bound = interval<T, impl::interval_upper_bound<UpperBound>, impl::interval_mixin<Mixins>...>;

  template <auto LowerBound, auto UpperBound, typename ...Mixins>
  using static_interval = interval<
      std::common_type_t<decltype(LowerBound), decltype(UpperBound)>,
      impl::interval_bounds<LowerBound, UpperBound>,
      impl::interval_mixin<Mixins>...>;


  template <typename To, typename From, typename ...TOpt, typename ...FOpt>
  VK_constant To project(in_param_t<From> from,
                         const interval<From, FOpt...>& fInterval,
                         const interval<To, TOpt...>& tInterval) noexcept {
    return interval_projection{fInterval, tInterval}(from);
  }


  static_assert(17 == project(4, static_interval<-2, 8>{}, static_interval<14, 19>{}));
  

  /*template <typename T>
  using LimitRange = interval<T>;*/
  /*using MaxU32 = LimitMax<u32>;
  using MinU32 = LimitMin<u32>;
  using RangeU32 = LimitRange<u32>;
  using MaxI32 = LimitMax<i32>;
  using MinI32 = LimitMin<i32>;
  using RangeI32 = LimitRange<i32>;
  using MaxU64 = LimitMax<u64>;
  using MinU64 = LimitMin<u64>;
  using MaxF32 = interval<f32, interval_lower_bound<0>>;
  using RangeU64 = LimitRange<u64>;
  using RangeF32 = LimitRange<f32>;*/
}

#endif //VALKYRIE_ADT_INTERVAL_HPP
