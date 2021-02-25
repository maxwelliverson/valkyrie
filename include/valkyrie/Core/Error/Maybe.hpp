//
// Created by Maxwell on 2020-11-21.
//

#ifndef VALKYRIE_MAYBE_HPP
#define VALKYRIE_MAYBE_HPP

#include <valkyrie/Core/Error/GenericCode.hpp>

namespace valkyrie::Core{

  namespace Detail{
    template <typename T, typename Domain>
    struct MaybeStorage{

      using status_type = StatusCode<Domain>;

      bool is_valid_ = false;
      union{
        StatusCode<Domain> status_{};
        T value_;
      };
      MaybeStorage() noexcept : status_{}{}
      template <typename ...Args> requires(std::is_constructible_v<status_type, Args...>)
      explicit MaybeStorage(Args&& ...args) noexcept
          : status_{ std::forward<Args>(args)... }{}
      ~MaybeStorage() {
        if constexpr (!(std::is_trivially_destructible_v<T> && std::is_trivially_destructible_v<StatusCode<Domain>>)) {
          if constexpr (std::is_trivially_destructible_v<T>) {
            if (!is_valid_)
              this->status_.~StatusCode<Domain>();
          }
          else if constexpr (std::is_trivially_destructible_v<StatusCode<Domain>>) {
            if (is_valid_)
              this->value_.~T();
          }
          else {
            if (is_valid_)
              this->value_.~T();
            else
              this->status_.~StatusCode<Domain>();
          }
        }
      }
    };
    template <typename T, typename Domain> requires(sizeof(T) <= std::max(sizeof(typename StatusCode<Domain>::value_type), 8ULL))
    struct MaybeStorage<T, Domain>{

      using status_type = StatusCode<Domain>;

      union{
        StatusCode<Domain> status_{};
        struct {
          u64 is_valid_ : 1;
          u64 : 63;
          T value_;
        };
      };

      /*MaybeStorage() noexcept(std::is_nothrow_default_constructible_v<T>) requires(std::default_initializable<T>)
          : is_valid_{true}, value_{}{}
      MaybeStorage(const MaybeStorage& other)
          : is_valid_{other.is_valid_}{
        if (is_valid_)
          new(&value_) T(other.value_);
        else
          new(&status_) status_type(other.status_);
      }
      MaybeStorage(MaybeStorage&& other) noexcept
          : is_valid_{other.is_valid_}{
        if (is_valid_) {
          new (&value_) T(std::move(other.value_));
        }
        else
          new(&status_) status_type(std::move(other.status_));
      }


      MaybeStorage(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>) requires(std::copy_constructible<T>)
          : is_valid_{true}, value_(value){}
      MaybeStorage(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) requires(std::move_constructible<T>)
          : is_valid_{true}, value_(std::move(value)){}
      MaybeStorage(const status_type& Status) noexcept
          : status_(Status){}
      MaybeStorage(status_type&& Status) noexcept
          : status_(std::move(Status)){}


      template <typename U, typename D2>
      requires(std::is_constructible_v<T, const U&> && std::is_constructible_v<status_type, const StatusCode<D2>&>)
      explicit(!std::convertible_to<const U&, T> || !std::convertible_to<const StatusCode<D2>&, status_type>)
      MaybeStorage(const MaybeStorage<U, D2>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
          : is_valid_{other.is_valid_}{
        if (is_valid_)
          new(&value_) T(other.value_);
        else
          new(&status_) status_type(other.status_);
      }

      template <typename U, typename D2>
      requires(std::is_constructible_v<T, U&&> && std::is_constructible_v<status_type, StatusCode<D2>&&>)
      explicit(!std::convertible_to<U&&, T> || !std::convertible_to<StatusCode<D2>&&, status_type>)
      MaybeStorage(MaybeStorage<U, D2>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
          : is_valid_{other.is_valid_}{
        if (is_valid_) {
          new (&value_) T(std::move(other.value_));
        }
        else
          new(&status_) status_type(std::move(other.status_));
      }


      template <typename ...Args>
      requires(std::is_constructible_v<T, Args...> && !(sizeof...(Args) == 1 && SameAsOneOf<T, Args...>))
      explicit(sizeof...(Args) == 1 && !(std::convertible_to<Args, T> && ...))
      MaybeStorage(Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
          : is_valid_(true), value_(std::forward<Args>(args)...){}

      template <typename ...Args>
      requires(std::is_constructible_v<status_type, Args...> && !(sizeof...(Args) == 1 && SameAsOneOf<status_type, Args...>))
      explicit(sizeof...(Args) == 1 && !(std::convertible_to<Args, status_type> && ...))
      MaybeStorage(Args&& ...args) noexcept(std::is_nothrow_constructible_v<status_type, Args...>)
          : status_(std::forward<Args>(args)...){}*/

      MaybeStorage() noexcept : status_{}{}
      template <typename ...Args> requires(std::is_constructible_v<status_type, Args...>)
      explicit MaybeStorage(Args&& ...args) noexcept
          : status_{ std::forward<Args>(args)... }{}
      ~MaybeStorage() {
        if constexpr (!(std::is_trivially_destructible_v<T> && std::is_trivially_destructible_v<StatusCode<Domain>>)) {
          if constexpr (std::is_trivially_destructible_v<T>) {
            if (!is_valid_)
              this->status_.~StatusCode<Domain>();
          }
          else if constexpr (std::is_trivially_destructible_v<StatusCode<Domain>>) {
            if (is_valid_)
              this->value_.~T();
          }
          else {
            if (is_valid_)
              this->value_.~T();
            else
              this->status_.~StatusCode<Domain>();
          }
        }
      }
    };
  }


  template <typename T, typename Domain = Erased<u64>>
  class Maybe : public Detail::MaybeStorage<T, Domain>{
    using Base = Detail::MaybeStorage<T, Domain>;

    void checkAccess() const VK_throws{
      if (!this->is_valid_)
        this->status_.throwException();
    }

  public:

    using value_type = T;
    using status_type = StatusCode<Domain>;

    Maybe() noexcept(std::is_nothrow_default_constructible_v<T>){
      this->status_.~status_type();
      this->is_valid_ = true;
      new (&this->value_) T{};
    }
    Maybe(const Maybe& other){
      if (other.is_valid_) {
        this->status_.~status_type();
        this->is_valid_ = true;
        new (&this->value_) T(other.value_);
      }
      else
        new(&this->status_) status_type(other.status_);
    }
    Maybe(Maybe&& other) noexcept{
      if (other.is_valid_) {
        this->status_.~status_type();
        this->is_valid_ = true;
        new (&this->value_) T(std::move(other.value_));
      }
      else
        new(&this->status_) status_type(std::move(other.status_));
    }


    /*Maybe(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>){
      this->status_.~status_type();
      this->is_valid_ = true;
      new(&this->value_) T{value};
    }
    Maybe(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) requires(std::move_constructible<T>){
      this->status_.~status_type();
      this->is_valid_ = true;
      new(&this->value_) T{std::move(value)};
    }
    Maybe(const status_type& Status) noexcept
        : Base{ Status }{}
    Maybe(status_type&& Status) noexcept
        : Base{ std::move(Status) }{}*/


    ~Maybe() = default;


    template <typename U, typename D2>
    requires(std::is_constructible_v<T, const U&> && std::is_constructible_v<status_type, const StatusCode<D2>&>)
    explicit(!std::convertible_to<const U&, T> || !std::convertible_to<const StatusCode<D2>&, status_type>)
    Maybe(const Maybe<U, D2>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>){
      if (other.is_valid_) {
        this->status_.~status_type();
        this->is_valid_ = true;
        new (&this->value_) T(other.value_);
      }
      else
        new(&this->status_) status_type(other.status_);
    }

    template <typename U, typename D2>
    requires(std::is_constructible_v<T, U&&> && std::is_constructible_v<status_type, StatusCode<D2>&&>)
    explicit(!std::convertible_to<U&&, T> || !std::convertible_to<StatusCode<D2>&&, status_type>)
    Maybe(Maybe<U, D2>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>){
      if (other.is_valid_) {
        this->status_.~status_type();
        this->is_valid_ = true;
        new (&this->value_) T(std::move(other.value_));
      }
      else
        new(&this->status_) status_type(std::move(other.status_));
    }


    template <NotSameAsOneOf<Maybe, status_type> V>
    requires(std::is_constructible_v<T, V>)
    explicit(!std::convertible_to<std::remove_reference_t<V>, T>)
    Maybe(V&& value) noexcept(std::is_nothrow_constructible_v<T, V>){
      this->status_.~status_type();
      this->is_valid_ = true;
      new(&this->value_) T{ std::forward<V>(value) };
    }

    template <NotSameAsOneOf<Maybe, T> S>
    requires(std::is_constructible_v<status_type, S>)
    explicit(!std::convertible_to<std::remove_reference_t<S>, status_type>)
    Maybe(S&& s) noexcept(std::is_nothrow_constructible_v<status_type, S>)
        : Base{ std::forward<S>(s) }{}

    template <typename ...Args>
    requires(std::is_constructible_v<T, Args...> && sizeof...(Args) > 1/* && !(sizeof...(Args) == 1 && SameAsOneOf<T, Args...>)*/)
    Maybe(Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>){
      this->status_.~status_type();
      this->is_valid_ = true;
      new(&this->value_) T{ std::forward<Args>(args)... };
    }

    template <typename ...Args>
    requires(std::is_constructible_v<StatusCode<Domain>, Args...> && sizeof...(Args) > 1/* && !(sizeof...(Args) == 1 && SameAsOneOf<status_type, Args...>)*/)
    Maybe(Args&& ...args) noexcept(std::is_nothrow_constructible_v<status_type, Args...>)
        : Base{ std::forward<Args>(args)... }{}





    Maybe& operator=(const Maybe& other) {
      Maybe tmp = std::move(*this);
      new(this) Maybe(other);
      return *this;
    }
    Maybe& operator=(Maybe&& other) noexcept {
      this->~Maybe();
      new(this) Maybe(std::move(other));
      return *this;
    }


    /*constexpr Maybe() noexcept(std::is_nothrow_default_constructible_v<T>) requires(std::default_initializable<T>)
        : Base{ .is_valid_ = true, .value_ = {}}{}
    constexpr Maybe(const Maybe& other) : Base{ .is_valid_ = other.is_valid_ }{
      if (this->is_valid_)
        new(&this->value_) T(other.value_);
      else
        new(&this->status_) status_type(other.status_);
    }
    constexpr Maybe(Maybe&& other) noexcept : Base{}{
      if (other.is_valid_) {
        this->is_valid_ = true;
        new (&this->value_) T(std::move(other.value_));
      }
      else
        new(&this->status_) status_type(std::move(other.status_));
    }


    constexpr Maybe(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>) requires(std::copy_constructible<T>)
        : Base{ .is_valid_ = true, .value_ = value }{}
    constexpr Maybe(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) requires(std::move_constructible<T>)
        : Base{ .is_valid_ = true, .value_ = std::move(value) }{}
    constexpr Maybe(const status_type& Status) noexcept
        : Base{ .status_ = Status.clone() }{}
    constexpr Maybe(status_type&& Status) noexcept
        : Base{ .status_ = std::move(Status) }{}


    constexpr Maybe& operator=(const Maybe& other) {
      if (this != &other) {
        this->~Maybe();
        new(this) Maybe(other);
      }
      return *this;
    }
    constexpr Maybe& operator=(Maybe&& other) noexcept {
      this->~Maybe();
      new(this) Maybe(std::move(other));
      return *this;
    }

    constexpr ~Maybe() = default;


    template <typename U, typename D2>
    requires(ConstructibleFrom<T, const U&> && ConstructibleFrom<status_type, const StatusCode<D2>&>)
    explicit(!std::convertible_to<const U&, T> || !std::convertible_to<const StatusCode<D2>&, status_type>)
    constexpr Maybe(const Maybe<U, D2>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        : Base{ .is_valid_ = other.is_valid_ }{
      if (hasValue())
        new(&this->value_) T(*other);
      else
        new(&this->status_) status_type(other.status_);
    }

    template <typename U, typename D2>
    requires(ConstructibleFrom<T, U&&> && ConstructibleFrom<status_type, StatusCode<D2>&&>)
    explicit(!std::convertible_to<U&&, T> || !std::convertible_to<StatusCode<D2>&&, status_type>)
    constexpr Maybe(Maybe<U, D2>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : Base{ .is_valid_ = other.is_valid_ }{
      if (hasValue())
        new(&this->value_) T(*std::move(other));
      else
        new(&this->status_) status_type(std::move(other.status_));
    }


    template <typename ...Args>
    requires(ConstructibleFrom<T, Args...> && !(sizeof...(Args) == 1 && SameAsOneOf<T, Args...>))
    explicit(sizeof...(Args) == 1 && !(std::convertible_to<Args, T> && ...))
    constexpr Maybe(Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : Base{ .is_valid_ = true, .value_ = T{ std::forward<Args>(args)... }}{}

    template <typename ...Args>
    requires(ConstructibleFrom<status_type, Args...> && !(sizeof...(Args) == 1 && SameAsOneOf<status_type, Args...>))
    explicit(sizeof...(Args) == 1 && !(std::convertible_to<Args, status_type> && ...))
    constexpr Maybe(Args&& ...args) noexcept(std::is_nothrow_constructible_v<status_type, Args...>)
        : Base{ .status_ = status_type{ std::forward<Args>(args)... }}{}*/

    explicit operator bool() const noexcept {
      return this->is_valid_;
    }

    bool hasValue() const noexcept {
      return this->is_valid_;
    }

    value_type take() {
      return std::move(this->value_);
    }

    value_type& value() & VK_throws {
      checkAccess();
      return this->value_;
    }
    const value_type& value() const & VK_throws {
      checkAccess();
      return this->value_;
    }
    value_type&& value() && VK_throws {
      checkAccess();
      return std::move(this->value_);
    }
    const value_type&& value() const && VK_throws {
      checkAccess();
      return std::move(this->value_);
    }

    value_type& operator*() & VK_throws {
      checkAccess();
      return this->value_;
    }
    const value_type& operator*() const & VK_throws {
      checkAccess();
      return this->value_;
    }
    value_type&& operator*() && VK_throws {
      checkAccess();
      return std::move(this->value_);
    }
    const value_type&& operator*() const && VK_throws {
      checkAccess();
      return std::move(this->value_);
    }

    status_type status() noexcept {
      if (this->is_valid_)
        return {};
      return std::move(this->status_);
    }
  };
}

#endif//VALKYRIE_MAYBE_HPP
