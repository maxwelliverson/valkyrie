//
// Created by Maxwell on 2020-11-21.
//

#ifndef VALKYRIE_STATUS_MAYBE_HPP
#define VALKYRIE_STATUS_MAYBE_HPP

#include "generic_code.hpp"

namespace valkyrie{

  namespace detail{
    template <typename T, typename Domain>
    struct maybe_storage{

      using status_type = status_code<Domain>;

      bool is_valid_ = false;
      union{
        status_code<Domain> status_{};
        T value_;
      };
      maybe_storage() noexcept : status_{}{}
      template <typename ...Args> requires(std::is_constructible_v<status_type, Args...>)
      explicit maybe_storage(Args&& ...args) noexcept
          : status_{ std::forward<Args>(args)... }{}
      ~maybe_storage() {
        if constexpr (!(std::is_trivially_destructible_v<T> && std::is_trivially_destructible_v<status_code<Domain>>)) {
          if constexpr (std::is_trivially_destructible_v<T>) {
            if (!is_valid_)
              this->status_.~status_code<Domain>();
          }
          else if constexpr (std::is_trivially_destructible_v<status_code<Domain>>) {
            if (is_valid_)
              this->value_.~T();
          }
          else {
            if (is_valid_)
              this->value_.~T();
            else
              this->status_.~status_code<Domain>();
          }
        }
      }
    };
    template <typename T, typename Domain> requires(sizeof(T) <= std::max(sizeof(typename status_code<Domain>::value_type), 8ULL))
    struct maybe_storage<T, Domain>{

      using status_type = status_code<Domain>;

      union{
        status_code<Domain> status_{};
        struct {
          u64 is_valid_ : 1;
          u64 : 63;
          T value_;
        };
      };

      /*maybe_storage() noexcept(std::is_nothrow_default_constructible_v<T>) requires(std::default_initializable<T>)
          : is_valid_{true}, value_{}{}
      maybe_storage(const maybe_storage& other)
          : is_valid_{other.is_valid_}{
        if (is_valid_)
          new(&value_) T(other.value_);
        else
          new(&status_) status_type(other.status_);
      }
      maybe_storage(maybe_storage&& other) noexcept
          : is_valid_{other.is_valid_}{
        if (is_valid_) {
          new (&value_) T(std::move(other.value_));
        }
        else
          new(&status_) status_type(std::move(other.status_));
      }


      maybe_storage(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>) requires(std::copy_constructible<T>)
          : is_valid_{true}, value_(value){}
      maybe_storage(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) requires(std::move_constructible<T>)
          : is_valid_{true}, value_(std::move(value)){}
      maybe_storage(const status_type& status) noexcept
          : status_(status){}
      maybe_storage(status_type&& status) noexcept
          : status_(std::move(status)){}


      template <typename U, typename D2>
      requires(std::is_constructible_v<T, const U&> && std::is_constructible_v<status_type, const status_code<D2>&>)
      explicit(!std::convertible_to<const U&, T> || !std::convertible_to<const status_code<D2>&, status_type>)
      maybe_storage(const maybe_storage<U, D2>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
          : is_valid_{other.is_valid_}{
        if (is_valid_)
          new(&value_) T(other.value_);
        else
          new(&status_) status_type(other.status_);
      }

      template <typename U, typename D2>
      requires(std::is_constructible_v<T, U&&> && std::is_constructible_v<status_type, status_code<D2>&&>)
      explicit(!std::convertible_to<U&&, T> || !std::convertible_to<status_code<D2>&&, status_type>)
      maybe_storage(maybe_storage<U, D2>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
          : is_valid_{other.is_valid_}{
        if (is_valid_) {
          new (&value_) T(std::move(other.value_));
        }
        else
          new(&status_) status_type(std::move(other.status_));
      }


      template <typename ...Args>
      requires(std::is_constructible_v<T, Args...> && !(sizeof...(Args) == 1 && same_as_one_of<T, Args...>))
      explicit(sizeof...(Args) == 1 && !(std::convertible_to<Args, T> && ...))
      maybe_storage(Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
          : is_valid_(true), value_(std::forward<Args>(args)...){}

      template <typename ...Args>
      requires(std::is_constructible_v<status_type, Args...> && !(sizeof...(Args) == 1 && same_as_one_of<status_type, Args...>))
      explicit(sizeof...(Args) == 1 && !(std::convertible_to<Args, status_type> && ...))
      maybe_storage(Args&& ...args) noexcept(std::is_nothrow_constructible_v<status_type, Args...>)
          : status_(std::forward<Args>(args)...){}*/

      maybe_storage() noexcept : status_{}{}
      template <typename ...Args> requires(std::is_constructible_v<status_type, Args...>)
      explicit maybe_storage(Args&& ...args) noexcept
          : status_{ std::forward<Args>(args)... }{}
      ~maybe_storage() {
        if constexpr (!(std::is_trivially_destructible_v<T> && std::is_trivially_destructible_v<status_code<Domain>>)) {
          if constexpr (std::is_trivially_destructible_v<T>) {
            if (!is_valid_)
              this->status_.~status_code<Domain>();
          }
          else if constexpr (std::is_trivially_destructible_v<status_code<Domain>>) {
            if (is_valid_)
              this->value_.~T();
          }
          else {
            if (is_valid_)
              this->value_.~T();
            else
              this->status_.~status_code<Domain>();
          }
        }
      }
    };
  }


  template <typename T, typename Domain = erased<u64>>
  class maybe : public detail::maybe_storage<T, Domain>{
    using Base = detail::maybe_storage<T, Domain>;

    void checkAccess() const VK_throws{
      if (!this->is_valid_)
        this->status_.throwException();
    }

  public:

    using value_type = T;
    using status_type = status_code<Domain>;

    maybe() noexcept(std::is_nothrow_default_constructible_v<T>){
      this->status_.~status_type();
      this->is_valid_ = true;
      new (&this->value_) T{};
    }
    maybe(const maybe & other){
      if (other.is_valid_) {
        this->status_.~status_type();
        this->is_valid_ = true;
        new (&this->value_) T(other.value_);
      }
      else
        new(&this->status_) status_type(other.status_);
    }
    maybe(maybe && other) noexcept{
      if (other.is_valid_) {
        this->status_.~status_type();
        this->is_valid_ = true;
        new (&this->value_) T(std::move(other.value_));
      }
      else
        new(&this->status_) status_type(std::move(other.status_));
    }


    /*maybe(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>){
      this->status_.~status_type();
      this->is_valid_ = true;
      new(&this->value_) T{value};
    }
    maybe(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) requires(std::move_constructible<T>){
      this->status_.~status_type();
      this->is_valid_ = true;
      new(&this->value_) T{std::move(value)};
    }
    maybe(const status_type& status) noexcept
        : Base{ status }{}
    maybe(status_type&& status) noexcept
        : Base{ std::move(status) }{}*/


    ~maybe() = default;


    template <typename U, typename D2>
    requires(std::is_constructible_v<T, const U&> && std::is_constructible_v<status_type, const status_code<D2>&>)
    explicit(!std::convertible_to<const U&, T> || !std::convertible_to<const status_code<D2>&, status_type>)
        maybe(const maybe<U, D2>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>){
      if (other.is_valid_) {
        this->status_.~status_type();
        this->is_valid_ = true;
        new (&this->value_) T(other.value_);
      }
      else
        new(&this->status_) status_type(other.status_);
    }

    template <typename U, typename D2>
    requires(std::is_constructible_v<T, U&&> && std::is_constructible_v<status_type, status_code<D2>&&>)
    explicit(!std::convertible_to<U&&, T> || !std::convertible_to<status_code<D2>&&, status_type>)
        maybe(maybe<U, D2>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>){
      if (other.is_valid_) {
        this->status_.~status_type();
        this->is_valid_ = true;
        new (&this->value_) T(std::move(other.value_));
      }
      else
        new(&this->status_) status_type(std::move(other.status_));
    }


    template <not_same_as_one_of<maybe, status_type> V>
    requires(std::is_constructible_v<T, V>)
    explicit(!std::convertible_to<std::remove_reference_t<V>, T>)
        maybe(V&& value) noexcept(std::is_nothrow_constructible_v<T, V>){
      this->status_.~status_type();
      this->is_valid_ = true;
      new(&this->value_) T{ std::forward<V>(value) };
    }

    template <not_same_as_one_of<maybe, T> S>
    requires(std::is_constructible_v<status_type, S>)
    explicit(!std::convertible_to<std::remove_reference_t<S>, status_type>)
        maybe(S&& s) noexcept(std::is_nothrow_constructible_v<status_type, S>)
        : Base{ std::forward<S>(s) }{}

    template <typename ...Args>
    requires(std::is_constructible_v<T, Args...> && sizeof...(Args) > 1/* && !(sizeof...(Args) == 1 && same_as_one_of<T, Args...>)*/)
        maybe(Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>){
      this->status_.~status_type();
      this->is_valid_ = true;
      new(&this->value_) T{ std::forward<Args>(args)... };
    }

    template <typename ...Args>
    requires(std::is_constructible_v<status_code<Domain>, Args...> && sizeof...(Args) > 1/* && !(sizeof...(Args) == 1 && same_as_one_of<status_type, Args...>)*/)
        maybe(Args&& ...args) noexcept(std::is_nothrow_constructible_v<status_type, Args...>)
        : Base{ std::forward<Args>(args)... }{}


    maybe & operator=(const maybe & other) {
      maybe tmp = std::move(*this);
      new(this) maybe(other);
      return *this;
    }
    maybe & operator=(maybe && other) noexcept {
      this->~maybe();
      new(this) maybe(std::move(other));
      return *this;
    }


    /*constexpr maybe() noexcept(std::is_nothrow_default_constructible_v<T>) requires(std::default_initializable<T>)
        : Base{ .is_valid_ = true, .value_ = {}}{}
    constexpr maybe(const maybe& other) : Base{ .is_valid_ = other.is_valid_ }{
      if (this->is_valid_)
        new(&this->value_) T(other.value_);
      else
        new(&this->status_) status_type(other.status_);
    }
    constexpr maybe(maybe&& other) noexcept : Base{}{
      if (other.is_valid_) {
        this->is_valid_ = true;
        new (&this->value_) T(std::move(other.value_));
      }
      else
        new(&this->status_) status_type(std::move(other.status_));
    }


    constexpr maybe(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>) requires(std::copy_constructible<T>)
        : Base{ .is_valid_ = true, .value_ = value }{}
    constexpr maybe(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) requires(std::move_constructible<T>)
        : Base{ .is_valid_ = true, .value_ = std::move(value) }{}
    constexpr maybe(const status_type& status) noexcept
        : Base{ .status_ = status.clone() }{}
    constexpr maybe(status_type&& status) noexcept
        : Base{ .status_ = std::move(status) }{}


    constexpr maybe& operator=(const maybe& other) {
      if (this != &other) {
        this->maybe();
        new(this) maybe(other);
      }
      return *this;
    }
    constexpr maybe& operator=(maybe&& other) noexcept {
      this->maybe();
      new(this) maybe(std::move(other));
      return *this;
    }

    constexpr maybe() = default;


    template <typename U, typename D2>
    requires(ConstructibleFrom<T, const U&> && ConstructibleFrom<status_type, const status_code<D2>&>)
    explicit(!std::convertible_to<const U&, T> || !std::convertible_to<const status_code<D2>&, status_type>)
    constexpr maybe(const maybe<U, D2>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        : Base{ .is_valid_ = other.is_valid_ }{
      if (hasValue())
        new(&this->value_) T(*other);
      else
        new(&this->status_) status_type(other.status_);
    }

    template <typename U, typename D2>
    requires(ConstructibleFrom<T, U&&> && ConstructibleFrom<status_type, status_code<D2>&&>)
    explicit(!std::convertible_to<U&&, T> || !std::convertible_to<status_code<D2>&&, status_type>)
    constexpr maybe(maybe<U, D2>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : Base{ .is_valid_ = other.is_valid_ }{
      if (hasValue())
        new(&this->value_) T(*std::move(other));
      else
        new(&this->status_) status_type(std::move(other.status_));
    }


    template <typename ...Args>
    requires(ConstructibleFrom<T, Args...> && !(sizeof...(Args) == 1 && same_as_one_of<T, Args...>))
    explicit(sizeof...(Args) == 1 && !(std::convertible_to<Args, T> && ...))
    constexpr maybe(Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : Base{ .is_valid_ = true, .value_ = T{ std::forward<Args>(args)... }}{}

    template <typename ...Args>
    requires(ConstructibleFrom<status_type, Args...> && !(sizeof...(Args) == 1 && same_as_one_of<status_type, Args...>))
    explicit(sizeof...(Args) == 1 && !(std::convertible_to<Args, status_type> && ...))
    constexpr maybe(Args&& ...args) noexcept(std::is_nothrow_constructible_v<status_type, Args...>)
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

#endif//VALKYRIE_STATUS_MAYBE_HPP
