//
// Created by maxwe on 2021-03-04.
//

#ifndef VALKYRIE_ERROR_RESULT_HPP
#define VALKYRIE_ERROR_RESULT_HPP

#include "generic_code.hpp"
#include "status_code.hpp"

#include <optional>

namespace valkyrie{

  inline constexpr static struct return_status_t{} return_status;
  inline constexpr static struct return_value_t{} return_value;

  template <typename T, typename Dom, typename NoValuePolicy>
  class basic_result;

  template <typename T, typename ADom, typename ANVP, typename BDom, typename BNVP>
  bool operator==(const basic_result<T, ADom, ANVP>&, const basic_result<T, BDom, BNVP>&) noexcept;

  namespace detail{

    template <typename T, typename Dom>
    struct basic_result_constants{
      using status_type = status_code<Dom>;
      using value_type  = T;

      inline constexpr static bool is_trivially_default_constructable = std::is_trivially_default_constructible_v<value_type>;
      inline constexpr static bool is_default_constructable           = std::is_default_constructible_v<value_type>;



      inline constexpr static bool is_trivially_copy_constructable    = std::is_trivially_copy_constructible_v<value_type> && std::is_trivially_copy_constructible_v<status_type>;
      inline constexpr static bool is_trivially_move_constructable    = std::is_trivially_move_constructible_v<value_type> && std::is_trivially_move_constructible_v<status_type>;
      inline constexpr static bool is_trivially_copy_assignable       = std::is_trivially_copy_assignable_v<value_type> && std::is_trivially_copy_assignable_v<status_type>;
      inline constexpr static bool is_trivially_move_assignable       = std::is_trivially_move_assignable_v<value_type> && std::is_trivially_move_assignable_v<status_type>;

      inline constexpr static bool is_copy_constructable              = std::is_copy_constructible_v<value_type> && std::is_copy_constructible_v<status_type>;
      inline constexpr static bool is_move_constructable              = std::is_move_constructible_v<value_type> && std::is_move_constructible_v<status_type>;
      inline constexpr static bool is_copy_assignable                 = std::is_copy_assignable_v<value_type> && std::is_copy_assignable_v<status_type>;
      inline constexpr static bool is_move_assignable                 = std::is_move_assignable_v<value_type> && std::is_move_assignable_v<status_type>;

      inline constexpr static bool is_trivially_destructible          = std::is_trivially_destructible_v<value_type> && std::is_trivially_destructible_v<status_type>;
      inline constexpr static bool is_destructible                    = std::is_destructible_v<value_type> && std::is_destructible_v<status_type>;
    };



    template <typename T, typename Dom>
    struct basic_result_storage : basic_result_constants<T, Dom>{

      using constants   = basic_result_constants<T, Dom>;
      using value_type  = typename constants::value_type;
      using status_type = typename constants::status_type;

      union{
        value_type  value_{};
        status_type status_;
        u8          default_init_;
      };
      bool          has_value_ = true;

      constexpr basic_result_storage() requires(constants::is_default_constructable) = default;
      constexpr basic_result_storage(const basic_result_storage&) requires(constants::is_trivially_copy_constructable) = default;
      template <typename U, typename ODom> requires(std::constructible_from<value_type, const U&> && std::constructible_from<status_type, const status_code<ODom>&>)
      constexpr basic_result_storage(const basic_result_storage<U, ODom>& other)
          : has_value_(other.has_value_), default_init_{}{
        if ( has_value_ )
          new (&value_) value_type(other.value_);
        else
          new (&status_) status_type(other.status_);
      }
      constexpr basic_result_storage(basic_result_storage&&) noexcept requires(constants::is_trivially_move_constructable) = default;
      template <typename U, typename ODom> requires(std::constructible_from<value_type, U&&> && std::constructible_from<status_type, status_code<ODom>&&>)
      constexpr basic_result_storage(basic_result_storage<U, ODom>&& other)
          : has_value_(other.has_value_), default_init_{}{
        if ( has_value_ )
          new (&value_) value_type(std::move(other.value_));
        else
          new (&status_) status_type(std::move(other.status_));
      }


      constexpr basic_result_storage& operator=(const basic_result_storage&) requires(constants::is_trivially_copy_assignable) = default;
      template <typename U, typename ODom> requires(std::assignable_from<value_type&, const U&> && std::assignable_from<status_type&, const status_code<ODom>&>)
      constexpr basic_result_storage& operator=(const basic_result_storage<U, ODom>& other){
#if VK_exceptions_enabled
        basic_result_storage tmp{std::move(*this)};
        try {
          this->~basic_result_storage();
          new(this) basic_result_storage(other);
        } catch(...) {
          new(this) basic_result_storage(std::move(tmp));
          throw;
        }
#else
        this->~basic_result_storage();
        new(this) basic_result_storage(other);
#endif
        return *this;
      }
      constexpr basic_result_storage& operator=(basic_result_storage&&) noexcept requires(constants::is_trivially_move_assignable) = default;
      template <typename U, typename ODom> requires(std::assignable_from<value_type&, U&&> && std::assignable_from<status_type&, status_code<ODom>&&>)
      constexpr basic_result_storage& operator=(basic_result_storage<U, ODom>&& other) noexcept {
        this->~basic_result_storage();
        new(this) basic_result_storage(std::move(other));
        return *this;
      }


      //constexpr ~basic_result_storage() requires(constants::is_trivially_destructible) = default;
      constexpr ~basic_result_storage() {
        if constexpr (!constants::is_trivially_destructible) {
          if ( has_value_ ) {
            if constexpr ( !std::is_trivially_destructible_v<value_type> )
              value_.~value_type();
          }
          else if constexpr ( !std::is_trivially_destructible_v<status_type> )
            status_.~status_type();
        }
        /*if ( has_value_ )
          value_.~value_type();
        else
          status_.~status_type();*/
      }


      template <typename ...Args> requires(std::constructible_from<value_type, Args...>)
      constexpr /*explicit*/ basic_result_storage(return_value_t, Args&& ...args)
      noexcept(std::is_nothrow_constructible_v<value_type, Args...>)
          : value_(std::forward<Args>(args)...), has_value_(true){}
      template <typename ...Args> requires(std::constructible_from<status_type, Args...>)
      constexpr /*explicit*/ basic_result_storage(return_status_t, Args&& ...args)
      noexcept(std::is_nothrow_constructible_v<status_type, Args...>)
          : status_(std::forward<Args>(args)...), has_value_(false){}
    };
    template <typename T, typename Dom> requires(sizeof(T) <= sizeof(typename status_code<Dom>::value_type))
    struct basic_result_storage<T, Dom> : basic_result_constants<T, Dom>{

      using constants = basic_result_constants<T, Dom>;
      using value_type = typename constants::value_type;
      using status_type = typename constants::status_type;


      union{
        struct {
          u64        has_value_ : 1 = true;
          u64                   : 63;
          union {
            value_type value_{};
            u8         default_init_;
          };
        };
        status_type  status_;
      };

      constexpr basic_result_storage() requires(constants::is_default_constructable) = default;
      constexpr basic_result_storage(const basic_result_storage&) requires(constants::is_trivially_copy_constructable) = default;
      template <typename U, typename ODom> requires(std::constructible_from<value_type, const U&> && std::constructible_from<status_type, const status_code<ODom>&>)
      constexpr basic_result_storage(const basic_result_storage<U, ODom>& other)
          : has_value_(other.has_value_), default_init_{}{
        if ( has_value_ )
          new (&value_) value_type(other.value_);
        else
          new (&status_) status_type(other.status_);
      }
      constexpr basic_result_storage(basic_result_storage&&) noexcept requires(constants::is_trivially_move_constructable) = default;
      template <typename U, typename ODom> requires(std::constructible_from<value_type, U&&> && std::constructible_from<status_type, status_code<ODom>&&>)
      constexpr basic_result_storage(basic_result_storage<U, ODom>&& other)
          : has_value_(other.has_value_), default_init_{}{
        if ( has_value_ )
          new (&value_) value_type(std::move(other.value_));
        else
          new (&status_) status_type(std::move(other.status_));
      }


      constexpr basic_result_storage& operator=(const basic_result_storage&) requires(constants::is_trivially_copy_assignable) = default;
      template <typename U, typename ODom> requires(std::assignable_from<value_type&, const U&> && std::assignable_from<status_type&, const status_code<ODom>&>)
      constexpr basic_result_storage& operator=(const basic_result_storage<U, ODom>& other){
#if VK_exceptions_enabled
        basic_result_storage tmp{std::move(*this)};
        try {
          this->~basic_result_storage();
          new(this) basic_result_storage(other);
        } catch(...) {
          new(this) basic_result_storage(std::move(tmp));
          throw;
        }
#else
        this->~basic_result_storage();
        new(this) basic_result_storage(other);
#endif
        return *this;
      }
      constexpr basic_result_storage& operator=(basic_result_storage&&) noexcept requires(constants::is_trivially_move_assignable) = default;
      template <typename U, typename ODom> requires(std::assignable_from<value_type&, U&&> && std::assignable_from<status_type&, status_code<ODom>&&>)
      constexpr basic_result_storage& operator=(basic_result_storage<U, ODom>&& other) noexcept {
        this->~basic_result_storage();
        new(this) basic_result_storage(std::move(other));
        return *this;
      }


      //constexpr ~basic_result_storage() requires(constants::is_trivially_destructible) = default;
      constexpr ~basic_result_storage() {
        if constexpr (!constants::is_trivially_destructible) {
          if ( has_value_ ) {
            if constexpr ( !std::is_trivially_destructible_v<value_type> )
              value_.~value_type();
          }
          else if constexpr ( !std::is_trivially_destructible_v<status_type> )
            status_.~status_type();
        }
        /*if ( has_value_ )
          value_.~value_type();
        else
          status_.~status_type();*/
      }


      template <typename ...Args> requires(std::constructible_from<value_type, Args...>)
      constexpr explicit basic_result_storage(return_value_t, Args&& ...args)
      noexcept(std::is_nothrow_constructible_v<value_type, Args...>)
          : value_(std::forward<Args>(args)...), has_value_(true){}
      template <typename ...Args> requires(std::constructible_from<status_type, Args...>)
      constexpr explicit basic_result_storage(return_status_t, Args&& ...args)
      noexcept(std::is_nothrow_constructible_v<status_type, Args...>)
          : status_(std::forward<Args>(args)...){}
    };


    template <typename T, typename Policy>
    struct no_value_policy_storage : public Policy{};
    template <typename T>
    struct no_value_policy_storage<T, void>{
      static std::optional<T> try_handle(const status_code<void>& status) noexcept {
        return std::nullopt;
      }
      static void             on_access(const status_code<void>& status) {
        status.throw_exception();
        //auto msg = status.message();
        //panic(generic_error(code::UnhandledError), msg.data());
      }
      static void             on_destruction(const status_code<void>& status) noexcept {
        if ( status.severity() == severity::fatal) {
          status.throw_exception();
          //panic(status);
        }
      }
    };
  }

  template <typename T, typename Dom, typename NoValuePolicy>
  class basic_result : detail::basic_result_storage<T, Dom>, detail::no_value_policy_storage<T, NoValuePolicy>{
    using base_type = detail::basic_result_storage<T, Dom>;
    using policy = detail::no_value_policy_storage<T, NoValuePolicy>;

    void handle_missing_value_access(){
      if (!has_value()) {
        if (auto result = policy::try_handle(this->status_))
          *this = basic_result(std::move(result.value()));
        else
          policy::on_access(this->status_);
      }
    }
    void handle_missing_value_access() const {
      if (!has_value())
        policy::on_access(this->status_);
    }


  public:

    using value_type = typename base_type::value_type;
    using status_type = typename base_type::status_type;

    using base_type::base_type;

    template <typename ...Args> requires(std::constructible_from<value_type, Args...> && !std::constructible_from<status_type, Args...>)
    basic_result(Args&& ...args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>)
        : base_type(return_value, std::forward<Args>(args)...){}
    template <typename ...Args> requires(!std::constructible_from<value_type, Args...> && std::constructible_from<status_type, Args...>)
    basic_result(Args&& ...args) noexcept(std::is_nothrow_constructible_v<status_type, Args...>)
        : base_type(return_status, std::forward<Args>(args)...){}


    ~basic_result() {
      if (!has_value()) {
        policy::on_destruction(this->status_);
      }
    }

    VK_nodiscard value_type& value() & {
      handle_missing_value_access();
      VK_assert(has_value());
      return this->value_;
    }
    VK_nodiscard const value_type& value() const & {
      handle_missing_value_access();
      VK_assert(has_value());
      return this->value_;
    }
    VK_nodiscard value_type&& value() && {
      handle_missing_value_access();
      VK_assert(has_value());
      return std::move(this->value_);
    }
    VK_nodiscard const value_type&& value() const && {
      handle_missing_value_access();
      VK_assert(has_value());
      return std::move(this->value_);
    }

    VK_nodiscard status_type status() const noexcept {
      status_type tmp{};
      if ( !has_value() )
        swap(tmp, this->status_);
      return std::move(tmp);
    }


    VK_nodiscard constexpr bool has_value() const noexcept {
      return this->has_value_;
    }
    VK_nodiscard constexpr explicit operator bool () const noexcept {
      return has_value();
    }

    template <typename ADom, typename ANVP, typename BDom, typename BNVP>
    friend bool operator==(const basic_result<T, ADom, ANVP>& a, const basic_result<T, BDom, BNVP>& b) noexcept;
  };

  template <typename T, typename ADom, typename ANVP, typename BDom, typename BNVP>
  bool operator==(const basic_result<T, ADom, ANVP>& a, const basic_result<T, BDom, BNVP>& b) noexcept{
    if ( a.has_value() )
      return b.has_value() && a.value_ == b.value_;
    return !b.has_value() && a.status_.strictly_equivalent(b.status_);
  }





  template <typename T, typename Dom = erased<u64>>
  using result = basic_result<T, Dom, void>;
  template <typename T>
  using system_result = result<T, typename system_status::domain_type>;
}

#endif //VALKYRIE_ERROR_RESULT_HPP
