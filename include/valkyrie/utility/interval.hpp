//
// Created by Maxwell on 2020-11-13.
//

#ifndef VALKYRIE_UTILITY_INTERVAL_HPP
#define VALKYRIE_UTILITY_INTERVAL_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie{
  template <typename T>
  using param_t = std::conditional_t<is_register_copyable<T>, T, const T&>;

  template <typename Comp, typename T>
  concept order_on = requires(Comp&& comp, const T& a, const T& b){
    { std::forward<Comp>(comp)(a, b) } -> std::convertible_to<std::partial_ordering>;
  };

  inline constexpr auto DefaultOrder = []<typename T, typename U>(T&& A, U&& B){ return std::forward<T>(A) <=> std::forward<U>(B); };

  template <auto Value>
  struct LowerBound;
  template <typename T>
  struct Mixin;
  template <auto Value>
  struct UpperBound;
  template <auto minValue, auto maxValue>
  struct Bounds;



  namespace _detail{
    enum IntervalLocation{
      EntirelyLessThan,
      IntersectionLessThan,
      Equals,
      ContainedWithin,
      EntirelyContains,
      IntersectionGreaterThan,
      EntirelyGreaterThan
    };

    template <typename T, typename Opt>
    class IntervalStorage;
    template <typename T>
    class IntervalStorage<T, void> {
      using param_t = param_t<T>;
    public:
      constexpr IntervalStorage() = default;
      constexpr IntervalStorage(param_t minValue, param_t maxValue) noexcept : min(minValue), max(maxValue){}

      T min;
      T max;
    };
    template <typename T, std::convertible_to<T> U, U Value>
    class IntervalStorage<T, LowerBound<Value>>{
      using param_t = param_t<T>;
    public:
      constexpr IntervalStorage() = default;
      constexpr IntervalStorage(param_t maxValue) noexcept : max(maxValue){}

      constexpr static T min = Value;
      T max;
    };
    template <typename T, std::convertible_to<T> U, U Value>
    class IntervalStorage<T, UpperBound<Value>>{
      using param_t = param_t<T>;
    public:
      constexpr IntervalStorage() = default;
      constexpr IntervalStorage(param_t minValue) noexcept : min(minValue){}

      T min;
      constexpr static T max = Value;
    };
    template <typename T, std::convertible_to<T> U, std::convertible_to<T> V, U MinVal, V MaxVal>
    class IntervalStorage<T, Bounds<MinVal, MaxVal>>{
      using param_t = param_t<T>;
    public:
      constexpr IntervalStorage() = default;

      constexpr static T min = MinVal;
      constexpr static T max = MaxVal;
    };
    template <typename T, typename U>
    class IntervalStorage<T, Mixin<U>> : public U{};

    template <typename T, typename ...Opt>
    class IntervalMixin : public IntervalStorage<T, Opt> ... {
    public:
      using IntervalStorage<T, Opt>::IntervalStorage...;
    };
    template <typename T>
    class IntervalMixin<T> : public IntervalStorage<T, void> {
    public:
      using IntervalStorage<T, void>::IntervalStorage;
    };
  }

  template <typename T, typename ...Opt>
  class interval : public _detail::IntervalMixin<T, Opt...> {
    using param_t = param_t<T>;
    using base_t = _detail::IntervalMixin<T, Opt...>;
  public:
    using base_t::base_t;

    [[nodiscard]] inline constexpr size_t size() const noexcept requires(requires{
      { this->max - this->min } -> std::convertible_to<size_t>;
    }){
      return this->max - this->min;
    }

    [[nodiscard]] inline constexpr bool contains(param_t Value) const noexcept{
      return this->min <= Value && Value < this->max;
    }
    template <std::predicate<param_t, param_t> Compare>
    [[nodiscard]] inline constexpr bool contains(param_t Value, Compare&& Comp) const noexcept{
      return !Comp(Value, this->min) && Comp(Value, this->max);
    }
    template <order_on<T> Compare>
    [[nodiscard]] inline constexpr bool contains(param_t Value, Compare&& Comp) const noexcept{
      return (Comp(Value, this->min) != std::partial_ordering::greater) && (Comp(Value, this->max) == std::partial_ordering::less);
    }

    [[nodiscard]] inline constexpr _detail::IntervalLocation relativeLocation(const interval & Other) const noexcept{
      auto Lower = this->min <=> Other.min;
      auto Upper = this->max <=> Other.max;
      if (Lower == Upper) VK_likely {
        if (Lower == std::partial_ordering::less)
          return this->max < Other.min ? _detail::EntirelyLessThan : _detail::IntersectionLessThan;
        if (Lower == std::partial_ordering::greater)
          return Other.max < this->min ? _detail::EntirelyGreaterThan : _detail::IntersectionGreaterThan;
        return _detail::Equals;
      }

      if (Lower == std::partial_ordering::less || Upper == std::partial_ordering::greater)
        return _detail::EntirelyContains;
      return _detail::ContainedWithin;
    }
    template <std::predicate<param_t, param_t> Compare>
    [[nodiscard]] inline constexpr _detail::IntervalLocation relativeLocation(const interval & Other, Compare&& Comp) const noexcept{
      if (Comp(this->min, Other.min)) {
        if (Comp(this->max, Other.min))
          return _detail::EntirelyLessThan;
        if (Comp(this->max, Other.max))
          return _detail::IntersectionLessThan;
        return _detail::EntirelyContains;
      }
      if (Comp(Other.max, this->max)) {
        if (Comp(Other.max, this->min))
          return _detail::EntirelyGreaterThan;
        if (Comp(Other.min, this->min))
          return _detail::IntersectionGreaterThan;
        return _detail::EntirelyContains;
      }
      VK_unlikely
      if (Comp(Other.min, this->min) || Comp(this->max, Other.max))
        return _detail::ContainedWithin;
      return _detail::Equals;
    }
    template <order_on<T> Compare>
    [[nodiscard]] inline constexpr _detail::IntervalLocation relativeLocation(const interval & Other, Compare&& Comp) const noexcept{
      auto Lower = Comp(this->min, Other.min);
      auto Upper = Comp(this->max, Other.max);
      if (Lower == Upper) VK_likely {
        if (Lower == std::partial_ordering::less)
          return (Comp(this->max, Other.min) == std::partial_ordering::less) ? _detail::EntirelyLessThan : _detail::IntersectionLessThan;
        if (Lower == std::partial_ordering::greater)
          return (Comp(Other.max, this->min) == std::partial_ordering::less) ? _detail::EntirelyGreaterThan : _detail::IntersectionGreaterThan;
        return _detail::Equals;
      }

      if (Lower == std::partial_ordering::less || Upper == std::partial_ordering::greater)
        return _detail::EntirelyContains;
      return _detail::ContainedWithin;
    }

    [[nodiscard]] inline constexpr interval intersect(const interval & Other) const noexcept{
      switch(this->relativeLocation(Other)) {
        case _detail::Equals:
        case _detail::ContainedWithin:
          return *this;
        case _detail::EntirelyContains:
          return Other;
        case _detail::IntersectionLessThan:
          return interval{Other.min, this->max};
        case _detail::IntersectionGreaterThan:
          return interval{this->min, Other.max};
        case _detail::EntirelyGreaterThan:
        case _detail::EntirelyLessThan:
          return {};
        default:
          __assume(0);
      }
    }
    template <std::predicate<param_t, param_t> Compare>
    [[nodiscard]] inline constexpr interval intersect(const interval & Other, Compare&& Comp) const noexcept{
      return this->intersect(Other, std::forward<Compare>(Comp));
    }
    template <order_on<T> Compare>
    [[nodiscard]] inline constexpr interval intersect(const interval & Other, Compare&& Comp) const noexcept{
      return this->intersect(Other, std::forward<Compare>(Comp));
    }


    [[nodiscard]] inline constexpr std::partial_ordering compare(const interval & Other) const noexcept{
      return compareInternal(this->relativeLocation(Other));
    }
    template <std::predicate<param_t, param_t> Compare>
    [[nodiscard]] inline constexpr std::partial_ordering compare(const interval & Other, Compare&& Comp) const noexcept{
      return compareInternal(this->relativeLocation(Other, std::forward<Compare>(Comp)));
    }
    template <order_on<T> Compare>
    [[nodiscard]] inline constexpr std::partial_ordering compare(const interval & Other, Compare&& Comp) const noexcept{
      return compareInternal(this->relativeLocation(Other, std::forward<Compare>(Comp)));
    }


    constexpr explicit operator bool() const noexcept{
      return this->min != this->max;
    }



    friend constexpr bool operator==(const interval & A, const interval & B) noexcept = default;
    friend constexpr std::partial_ordering operator<=>(const interval & A, const interval & B) noexcept{
      return compareInternal(A.relativeLocation(B));
    }

  private:

    [[nodiscard]] inline static constexpr std::partial_ordering compareInternal(_detail::IntervalLocation Loc) noexcept{
      switch(Loc){
        case _detail::EntirelyLessThan:
        case _detail::IntersectionLessThan:
          return std::partial_ordering::less;
        case _detail::Equals:
          return std::partial_ordering::equivalent;
        case _detail::IntersectionGreaterThan:
        case _detail::EntirelyGreaterThan:
          return std::partial_ordering::greater;
        case _detail::ContainedWithin:
        case _detail::EntirelyContains:
          return std::partial_ordering::unordered;
        default:
          __assume(0);
      }
    }
  };

  template <typename T, typename U>
  interval(T, U) -> interval<std::common_type_t<T, U>>;

  template <typename T>
  using LimitMax = interval<T, LowerBound<std::numeric_limits<T>::min()>>;
  template <typename T>
  using LimitMin = interval<T, UpperBound<std::numeric_limits<T>::max()>>;
  template <typename T>
  using LimitRange = interval<T>;

  using MaxU32 = LimitMax<u32>;
  using MinU32 = LimitMin<u32>;
  using RangeU32 = LimitRange<u32>;
  using MaxI32 = LimitMax<i32>;
  using MinI32 = LimitMin<i32>;
  using RangeI32 = LimitRange<i32>;
  using MaxU64 = LimitMax<u64>;
  using MinU64 = LimitMin<u64>;
  using MaxF32 = interval<f32, LowerBound<0>>;
  using RangeU64 = LimitRange<u64>;
  using RangeF32 = LimitRange<f32>;

}

#endif //VALKYRIE_UTILITY_INTERVAL_HPP
