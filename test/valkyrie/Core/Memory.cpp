//
// Created by Maxwell on 2021-01-01.
//

#include <valkyrie/Core/Memory/SmallObjectAllocator.hpp>
#include <valkyrie/Core/Utility/Interval.hpp>

namespace Detail{

  class EmptyType{};


  template <typename T>
  class IntervalMapValueStorage{
    T value_;
  public:
    template <typename ...Args>
    IntervalMapValueStorage(Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>) : value_(std::forward<Args>(args)...){}

    using value_type = T;

    value_type & value() & noexcept {
      return value_;
    }
    const value_type & value() const & noexcept {
      return value_;
    }
    value_type & value() && noexcept {
      return std::move(value_);
    }
    const value_type && value() const && noexcept {
      return std::move(value_);
    }
  };
  template <typename T> requires(std::is_empty_v<T>)
  class IntervalMapValueStorage<T> : T{
  public:
    constexpr IntervalMapValueStorage() = default;

    using value_type = T;

    value_type & value() & noexcept {
      return *this;
    }
    const value_type & value() const & noexcept {
      return *this;
    }
    value_type & value() && noexcept {
      return std::move(*this);
    }
    const value_type && value() const && noexcept {
      return std::move(*this);
    }
  };

  template <typename Key, typename Value>
  class IntervalMapEntry : public IntervalMapValueStorage<Value>{
    Key key_;
  public:
    using key_type = Key;


    virtual ~IntervalMapEntry() = default;

    key_type& key() & noexcept {
      return key_;
    }
    const key_type& key() const & noexcept {
      return key_;
    }
    key_type&& key() && noexcept {
      return std::move(key_);
    }
    const key_type&& key() const && noexcept {
      return std::move(key_);
    }
  };
}


int main() {
  using valkyrie::Core::SmallObjectAllocator, valkyrie::Core::FixedSizeAllocator;

  SmallObjectAllocator<> allocator{};



}