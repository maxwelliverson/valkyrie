//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_STATUS_CODE_HPP
#define VALKYRIE_STATUS_CODE_HPP

#include <valkyrie/utility/casting.hpp>
#include <valkyrie/adt/string_view.hpp>
#include <valkyrie/async/atomic.hpp>

namespace valkyrie{

  // Declaration

  template <typename>
  class erased;
  template <typename>
  class status_code;
  template <typename>
  class error_code;
  class status_domain;

  class generic_domain;
  template <typename E>
  class status_enum_domain;

  template <typename T, typename Base = status_domain>
  class unique_status_domain;

  enum class code : i32;
  enum class severity : i32;

  VK_noreturn void assertPanic(string_view msg) noexcept;
  VK_noreturn void panic(const utf8* pMessage) noexcept;
  VK_noreturn void panic(const status_code<void>& statusCode) noexcept;
  VK_noreturn void panic(const status_code<void>& statusCode, const utf8* pMessage) noexcept;

  inline severity getDefaultSeverity(code code) noexcept;











  // Implementation



  class status_domain{
    template <typename>
    friend class status_code;

    u64 guid_;

  protected:

    constexpr status_domain(const uuid& class_id) noexcept : guid_(toInteger(class_id)){}


    virtual bool doFailure(const status_code<void>& status) const noexcept = 0;
    virtual bool doEquivalent(const status_code<void>& statusA, const status_code<void>& statusB) const noexcept = 0;
    virtual code doCode(const status_code<void>& status) const noexcept = 0;
    virtual string_ref doMessage(const status_code<void>& status) const noexcept = 0;

    virtual severity doSeverity(const status_code<void>& status) const noexcept { return getDefaultSeverity(this->doCode(status)); }
    virtual void doErasedCopy(status_code<void>& To, const status_code<void>& From, size_t Bytes) const noexcept { std::memcpy(&To, &From, Bytes); }
    virtual void doErasedDestroy(status_code<void>& code, size_t Bytes) const noexcept { }

    VK_noreturn virtual void doThrowException(const status_code<void>& code) const VK_throws;

  public:

    using string_ref = string_ref;

    virtual string_ref name() const noexcept = 0;
    constexpr u64 id() const noexcept {
      return guid_;
    }

    VK_nodiscard friend bool operator==(const status_domain& A, const status_domain& B) noexcept{
      //return A.name() == B.name();
      return A.id() == B.id();
      //return A.guid() == B.guid();
    }
    VK_nodiscard friend std::strong_ordering operator<=>(const status_domain& A, const status_domain& B) noexcept {
      return A.id() <=> B.id();
    }
  };



  /*template <typename T, typename Base>
  class unique_status_domain : public Base{
  public:

    using domain_type = T;

    using Base::Base;


    const uuid& id() const noexcept override { return class_traits<T>::id; }
  };*/


  template <>
  class status_code<void>{
    template <typename>
    friend class status_code;
  public:
    using domain_type = void;
    using value_type = void;
    using string_ref = typename status_domain::string_ref;

  protected:
    const status_domain* Domain{nullptr};

    status_code() = default;
    status_code(const status_code &) = default;
    status_code(status_code &&) noexcept = default;
    status_code & operator=(const status_code &) = default;
    status_code & operator=(status_code &&) noexcept = default;
    ~status_code() = default;

    explicit status_code(const status_domain* Domain) noexcept : Domain(Domain){}
  public:
    VK_nodiscard VK_inline const status_domain& domain() const noexcept { return *Domain; }
    VK_nodiscard VK_inline bool empty() const noexcept { return !Domain; }

    VK_nodiscard string_ref message() const noexcept { return Domain ? Domain->doMessage(*this) : string_ref("(empty)"); }
    VK_nodiscard severity   severity() const noexcept { return empty() ? valkyrie::severity{} : Domain->doSeverity(*this); }
    VK_nodiscard bool       success() const noexcept { return !empty() && !Domain->doFailure(*this); }
    VK_nodiscard bool       failure() const noexcept { return !empty() && Domain->doFailure(*this); }
    VK_nodiscard code       generic() const noexcept { return empty() ? code{-1} : Domain->doCode(*this); }


    template <typename T>
    VK_nodiscard bool strictlyEquivalent(const status_code<T>& Other) const noexcept{
      if(empty() && Other.empty()) return true;
      if(!empty() && !Other.empty()) return Domain->doEquivalent(*this, Other);
      return false;
    }
    template <typename T, typename ...Args>
    VK_nodiscard bool strictlyEquivalent(T&& Val, Args&& ...args) const noexcept requires(requires{
      { makeStatusCode((T&&)Val, (Args&&)args...) } -> std::derived_from<status_code>;
    }){
      return this->strictlyEquivalent(makeStatusCode(std::forward<T>(Val), std::forward<Args>(args)...));
    }
    template <typename T>
    VK_nodiscard bool equivalent(const status_code<T>& Other) const noexcept{
      return strictlyEquivalent(Other) ||
             Other.strictlyEquivalent(*this) ||
             (Domain->doCode(*this) == Other.Domain->doCode(Other));
    }
    template <typename T, typename ...Args>
    VK_nodiscard bool equivalent(T&& Val, Args&& ...args) const noexcept requires(requires{
      { makeStatusCode((T&&)Val, (Args&&)args...) } -> std::derived_from<status_code>;
    }){
      return this->equivalent(makeStatusCode(std::forward<T>(Val), std::forward<Args>(args)...));
    }

    VK_noreturn void throwException() const VK_throws {
      Domain->doThrowException(*this);
      VK_if(VK_compiler_gcc(abort();))  // suppress buggy GCC warning
    }

    VK_nodiscard VK_gpu_inline explicit operator bool() const noexcept {
      return this->success();
    }

    template <typename T>
    VK_nodiscard bool operator==(T&& Val) const noexcept {
      return this->equivalent(std::forward<T>(Val));
    }

    /*template <typename T>
    VK_nodiscard friend bool operator==(const status_code& A, T&& Val) noexcept requires(requires{
      { A.equivalent(std::forward<T>(Val)) } noexcept;
    }){
      return A.equivalent(std::forward<T>(Val));
    }*/
  };

  namespace detail{
    template <typename Dom>
    struct GetDomainValueType{
      using domain_type = Dom;
      using value_type = typename domain_type::value_type;
    };
    template <typename ErasedType>
    struct GetDomainValueType<erased<ErasedType>>{
      using domain_type = status_domain;
      using value_type = ErasedType;
    };

    template <typename DomainT>
    class StatusStorage : public status_code<void>{
      using Base = status_code<void>;
    public:
      using domain_type = typename GetDomainValueType<DomainT>::domain_type;
      using value_type = typename GetDomainValueType<DomainT>::value_type;
      using string_ref = typename domain_type::string_ref;

      #ifndef NDEBUG
      static_assert(std::is_move_constructible_v<value_type> || std::is_copy_constructible_v<value_type>, "DomainType::value_type is neither move nor copy constructible!");
      static_assert(!std::is_default_constructible_v<value_type> || std::is_nothrow_default_constructible_v<value_type>, "DomainType::value_type is not nothrow default constructible!");
      static_assert(!std::is_move_constructible_v<value_type> || std::is_nothrow_move_constructible_v<value_type>, "DomainType::value_type is not nothrow move constructible!");
      static_assert(std::is_nothrow_destructible_v<value_type>, "DomainType::value_type is not nothrow destructible!");
      #endif

      VK_nodiscard const domain_type &domain() const noexcept { return *static_cast<const domain_type *>(Domain); }

      void clear() noexcept {
        Value.~value_type();
        Domain = nullptr;
        new(&Value) value_type();
      }

      value_type & value() & noexcept { return this->Value; }
      const value_type & value() const & noexcept { return this->Value; }
      value_type && value() && noexcept { return std::move(this->Value); }
      const value_type && value() const && noexcept { return std::move(this->Value); }

    protected:

      StatusStorage() noexcept = default;
      StatusStorage(const StatusStorage&) noexcept = default;
      StatusStorage(StatusStorage&& Other) noexcept
          : Base(static_cast<StatusStorage&&>(Other)),
            Value(static_cast<StatusStorage&&>(Other).Value){
        Other.Domain = nullptr;
      }
      template <typename ...Args> requires(ConstructibleFrom<value_type, Args...>)
      explicit StatusStorage(std::in_place_t, const domain_type * Domain, Args&& ...args) noexcept
          : Base(Domain),
            Value(std::forward<Args>(args)...){}


      StatusStorage& operator=(const StatusStorage&) = default;
      StatusStorage& operator=(StatusStorage&& Other) noexcept {
        this->~StatusStorage();
        new(this) StatusStorage(std::move(Other));
        return *this;
      }

      ~StatusStorage() noexcept = default;


      value_type Value;
    };

    template <typename T>
    struct GetStatusCodeBase{
      using type = StatusStorage<T>;
    };
    template <typename T> requires(requires{ typename T::template mixin<StatusStorage<T>>; } && std::derived_from<typename T::template mixin<StatusStorage<T>>, StatusStorage<T>>)
    struct GetStatusCodeBase<T>{
      using type = typename T::template mixin<StatusStorage<T>>;
    };

    template <typename T>
    using StatusCodeBase = typename GetStatusCodeBase<T>::type;

    template <typename T>
    concept ConstantDomain = requires{ { T::get() } -> std::convertible_to<const status_domain&>; };
  }

  template <typename Dom>
  class status_code : public detail::StatusCodeBase<Dom>{
    template <typename> friend class status_code;
    using Base = detail::StatusCodeBase<Dom>;

    using ThisType = status_code;

  public:

    //using domain_type = typename domain_traits<Dom>::domain_type;
    using domain_type = Dom;
    //using value_type = typename domain_traits<Dom>::value_type;
    using value_type = typename domain_type::value_type;
    using string_ref = string_ref;
    //using string_ref = typename domain_type::string_ref;


  protected:
    using Base::Base;

    inline constexpr static bool ConstantDomain = detail::ConstantDomain<domain_type>;

  public:
    status_code() = default;
    status_code(const status_code &) = default;
    status_code(status_code &&) noexcept = default;
    status_code & operator=(const status_code &) = default;
    status_code & operator=(status_code &&) noexcept = default;
    ~status_code() = default;

    status_code clone() const { return *this; }

    /*template <not_same_as<Dom> OtherDom> requires(ConstructibleFrom<value_type, const typename *//*domain_traits<*//*OtherDom*//*>*//*::value_type&>)
    status_code(const status_code<OtherDom>& other) noexcept : status_code(other.value()){}
    template <not_same_as<Dom> OtherDom> requires(ConstructibleFrom<value_type, typename *//*domain_traits<*//*OtherDom*//*>*//*::value_type&&>)
    status_code(status_code<OtherDom>&& other) noexcept : status_code(std::move(other).value()){}*/


    template <not_same_as_one_of<status_code, std::in_place_t> T, typename ...Args>
    status_code(T&& val, Args&& ...args) noexcept(noexcept(makeStatusCode((T&&)val, (Args&&)args...)))
        requires(requires{ { makeStatusCode(std::forward<T>(val), std::forward<Args>(args)...) } -> std::convertible_to<status_code>; })
        : status_code(makeStatusCode(std::forward<T>(val), std::forward<Args>(args)...)){}

    /*template <StatusEnum Enum>
    requires(ConstructibleFrom<ThisType, typename enum_traits<Enum>::status_type>)
    status_code(Enum&& E) noexcept(std::is_nothrow_constructible_v<ThisType, typename enum_traits<Enum>::status_type>)
        : status_code(typename enum_traits<Enum>::status_type(std::forward<Enum>(E))){}*/

    template <typename ...Args> requires(ConstantDomain && ConstructibleFrom<value_type, Args...>)
    explicit status_code(std::in_place_t, Args&& ...args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>)
        : Base(std::in_place, std::addressof(domain_type::get()),  std::forward<Args>(args)...){}

    template <typename T, typename ...Args> requires(ConstantDomain && ConstructibleFrom<value_type, std::initializer_list<T>, Args...>)
    explicit status_code(std::in_place_t, std::initializer_list<T> Il, Args&& ...args) noexcept(std::is_nothrow_constructible_v<value_type, std::initializer_list<T>, Args...>)
        : Base(std::in_place, std::addressof(domain_type::get()), Il, std::forward<Args>(args)...){}

    explicit(!status_enum_type<value_type>) status_code(const value_type & Other) noexcept(std::is_nothrow_copy_constructible_v<value_type>)
    requires(ConstantDomain && std::copy_constructible<value_type>)
        : Base(std::in_place, std::addressof(domain_type::get()), Other){}

    explicit(!status_enum_type<value_type>) status_code(value_type && Other) noexcept(std::is_nothrow_move_constructible_v<value_type>)
    requires(ConstantDomain && std::move_constructible<value_type>)
        : Base(std::in_place, std::addressof(domain_type::get()), std::move(Other)){}


    template <detail::safely_erasable_from<value_type> ErasedType>
    explicit status_code(const status_code<erased<ErasedType>>& Other)
    noexcept(std::is_nothrow_copy_constructible_v<value_type>)
    requires(ConstantDomain && std::move_constructible<value_type>)
        : status_code(erasure_cast<value_type>(Other.value())){
      VK_assert(this->domain() == Other.domain());
    }




    VK_nodiscard string_ref message() const noexcept { return this->Domain ? string_ref(this->Domain->doMessage(*this)) : string_ref("(empty)"); }
  };

  template <typename Arg, typename ...Args> requires(detail::has_make_status_code<Arg, Args...>)
  status_code(Arg&& arg, Args&& ...args) noexcept  -> status_code<typename decltype(makeStatusCode((Arg&&)arg, (Args&&)args...))::domain_type>;

  template <typename ErasedType>
  class status_code<erased<ErasedType>> : public detail::StatusCodeBase<erased<ErasedType>>{
    using Base = detail::StatusCodeBase<erased<ErasedType>>;

    using ThisType = status_code;

  public:
    using domain_type = void;
    using value_type = ErasedType;
    using typename Base::string_ref;

    status_code() = default;
    status_code(const status_code &) = delete;
    status_code(status_code &&) noexcept = default;
    status_code & operator=(const status_code &) = delete;
    status_code & operator=(status_code &&) noexcept = default;
    ~status_code(){
      if(this->Domain)
        this->Domain->doErasedDestroy(*this, sizeof(status_code));
    }

    status_code clone() const {
      if(!this->Domain)
        return {};
      status_code Copy;
      this->Domain->doErasedCopy(Copy, *this, sizeof(status_code));
      return std::move(Copy);
    }

    template <typename Dom>
    requires(detail::safely_erasable_from<value_type, typename Dom::value_type> &&
             std::is_trivially_copy_constructible_v<typename Dom::value_type>)
    status_code(const status_code<Dom>& Other) noexcept
        : Base(std::in_place, &Other.domain(), erasure_cast<value_type>(Other.value())){}

    template <typename Dom>
    requires(detail::safely_erasable_from<value_type, typename Dom::value_type> &&
             std::is_trivially_move_constructible_v<typename Dom::value_type>)
    status_code(status_code<Dom>&& Other) noexcept
        : Base(std::in_place, &Other.domain(), erasure_cast<value_type>(std::move(Other).value())){}


    template <not_same_as_one_of<ThisType, std::in_place_t> T, typename ...Args>
    status_code(T&& val, Args&& ...args) noexcept(noexcept(makeStatusCode((T&&)val, (Args&&)args...)))
        requires(requires{ { makeStatusCode((T&&)val, (Args&&)args...) } -> std::convertible_to<ThisType>; })
        : status_code(makeStatusCode(std::forward<T>(val), std::forward<Args>(args)...)){}

    template <status_enum_type Enum>
    requires(ConstructibleFrom<ThisType, typename enum_traits<Enum>::status_type>)
    status_code(Enum&& E) noexcept(std::is_nothrow_constructible_v<ThisType, typename enum_traits<Enum>::status_type>)
        : status_code(typename enum_traits<Enum>::status_type(std::forward<Enum>(E))){}


    value_type value() const noexcept { return this->Value; }
  };


  template <status_enum_type E>
  status_code(E) -> status_code<typename enum_traits<E>::status_domain>;

  template <typename Dom>
  class error_code : public status_code<Dom>{

    using Base = status_code<Dom>;
    using Base::success;

    using ThisType = error_code;

    inline void _checkIfSuccess() const;

  public:
    using typename Base::value_type;
    using typename Base::string_ref;

    error_code() = default;
    error_code(const error_code &) = default;
    error_code(error_code &&) noexcept = default;
    error_code & operator=(const error_code &) = default;
    error_code & operator=(error_code &&) noexcept = default;
    ~error_code() = default;


    explicit error_code(const Base& Other) noexcept(std::is_nothrow_copy_constructible_v<Base>) :
        Base(Other){
      _checkIfSuccess();
    }
    explicit error_code(Base&& Other) noexcept(std::is_nothrow_move_constructible_v<Base>) :
        Base(std::move(Other)){
      _checkIfSuccess();
    }


    template <not_same_as_one_of<ThisType, std::in_place_t> T, typename ...Args>
    error_code(T&& val, Args&& ...args) noexcept(noexcept(makeStatusCode((T&&)val, (Args&&)args...)))
        requires(requires{ { makeStatusCode((T&&)val, (Args&&)args...) } -> std::convertible_to<ThisType>; })
        : error_code(makeStatusCode(std::forward<T>(val), std::forward<Args>(args)...)){
      _checkIfSuccess();
    }

    template <status_enum_type Enum>
    requires(ConstructibleFrom<ThisType, typename enum_traits<Enum>::status_type>)
    error_code(Enum&& E) noexcept(std::is_nothrow_constructible_v<error_code, typename enum_traits<Enum>::status_type>)
        : error_code(typename enum_traits<Enum>::status_type(std::forward<Enum>(E))){
      _checkIfSuccess();
    }

    template <typename ...Args>
    explicit error_code(std::in_place_t, Args&& ...args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>)
    requires(ConstructibleFrom<Base, std::in_place_t, Args...>)
        : Base(std::in_place, std::forward<Args>(args)...){
      _checkIfSuccess();
    }

    template <typename T, typename ...Args>
    explicit error_code(std::in_place_t, std::initializer_list<T> Il, Args&& ...args) noexcept(std::is_nothrow_constructible_v<value_type, std::initializer_list<T>, Args...>)
    requires(ConstructibleFrom<Base, std::in_place_t, std::initializer_list<T>, Args...>)
        : Base(std::in_place, Il, std::forward<Args>(args)...){
      _checkIfSuccess();
    }

    explicit error_code(const value_type& Other) noexcept(std::is_nothrow_copy_constructible_v<value_type>)
    requires(std::is_nothrow_constructible_v<Base, const value_type&>)
        : Base(std::in_place, Other){
      _checkIfSuccess();
    }

    explicit error_code(value_type&& Other) noexcept(std::is_nothrow_move_constructible_v<value_type>)
    requires(std::is_nothrow_constructible_v<Base, value_type&&>)
        : Base(std::in_place, std::move(Other)){
      _checkIfSuccess();
    }


    template <detail::safely_erasable_from<value_type> ErasedType>
    explicit error_code(const status_code<erased<ErasedType>>& Other) noexcept(std::is_nothrow_copy_constructible_v<value_type>) : error_code(erasure_cast<value_type>(Other.value())) {
      VK_axiom(this->domain() == Other.domain());
      _checkIfSuccess();
    }


    VK_nodiscard bool success() const noexcept { return false; }
    VK_nodiscard const value_type& value() const & noexcept { return this->Value; }
  };
  template <typename ErasedType>
  class error_code<erased<ErasedType>> : public status_code<erased<ErasedType>>{
    using Base = status_code<erased<ErasedType>>;
    using Base::success;

    using ThisType = error_code;

    inline void _checkIfSuccess() const {
      if(Base::success())
        std::terminate(); //TODO: Replace with custom termination functions...
    }

  public:
    using typename Base::value_type;
    using typename Base::string_ref;

    error_code() = default;
    error_code(const error_code &) = default;
    error_code(error_code &&) noexcept = default;
    error_code & operator=(const error_code &) = default;
    error_code & operator=(error_code &&) noexcept = default;
    ~error_code() = default;

    explicit error_code(const Base& Other) noexcept(std::is_nothrow_copy_constructible_v<Base>) :
        Base(Other.clone()){
      _checkIfSuccess();
    }
    explicit error_code(Base&& Other) noexcept(std::is_nothrow_move_constructible_v<Base>) :
        Base(std::move(Other)){
      _checkIfSuccess();
    }

    template <typename Dom>
    requires(ConstructibleFrom<Base, const status_code<Dom>&>)
    error_code(const status_code<Dom>& Other) noexcept :
        Base(Other){
      _checkIfSuccess();
    }

    template <typename Dom>
    requires(ConstructibleFrom<Base, status_code<Dom>&&>)
    error_code(status_code<Dom>&& Other) noexcept :
        Base(std::move(Other)){
      _checkIfSuccess();
    }


    template <typename Dom>
    requires(ConstructibleFrom<Base, const status_code<Dom>&>)
    error_code(const error_code<Dom>& Other) noexcept :
        Base(static_cast<const status_code<Dom>&>(Other)){
      _checkIfSuccess();
    }

    template <typename Dom>
    requires(ConstructibleFrom<Base, status_code<Dom>&&>)
    error_code(error_code<Dom>&& Other) noexcept :
        Base(static_cast<status_code<Dom>&&>(Other)){
      _checkIfSuccess();
    }


    template <not_same_as_one_of<ThisType, std::in_place_t> T, typename ...Args>
    error_code(T&& val, Args&& ...args) noexcept(noexcept(makeStatusCode((T&&)val, (Args&&)args...)))
    requires(requires{ { makeStatusCode((T&&)val, (Args&&)args...) } -> std::convertible_to<ThisType>; })
        : error_code(makeStatusCode(std::forward<T>(val), std::forward<Args>(args)...)){
      _checkIfSuccess();
    }

    template <status_enum_type Enum>
    requires(ConstructibleFrom<ThisType, typename enum_traits<Enum>::status_type>)
    error_code(Enum&& E) noexcept(std::is_nothrow_constructible_v<error_code, typename enum_traits<Enum>::status_type>)
        : error_code(typename enum_traits<Enum>::status_type(std::forward<Enum>(E))){
      _checkIfSuccess();
    }


    VK_nodiscard bool success() const noexcept { return false; }
    VK_nodiscard const value_type& value() const & noexcept { return this->Value; }
  };


  using error         = error_code<erased<u64>>;
  using status        = status_code<erased<u64>>;

  template <typename Dom>
  inline void error_code<Dom>::_checkIfSuccess() const {
    if(Base::success())
      assertPanic("An error_code object was created from a non-failure status code. "
                  "Consider using status_code instead.");
  }



  /*template <typename ToDomain, typename FromDomain> requires(std::derived_from<ToDomain, FromDomain>)
  inline static status_code<ToDomain> domain_cast(const status_code<FromDomain>& from) noexcept {
    VK_assert(from.domain() == ToDomain());
    return *(const status_code<ToDomain>*)&from;
  }
  template <typename ToDomain, typename FromDomain> requires(std::derived_from<ToDomain, FromDomain>)
  inline static status_code<ToDomain> domain_cast(status_code<FromDomain>&& from) noexcept {
    VK_assert(from.domain() == ToDomain());
    return std::move(*(const status_code<ToDomain>*)&from);
  }*/
}

#if VK_system_windows
#include "system/win32.hpp"
#else
#include "system/posix.hpp"
#endif

#endif//VALKYRIE_STATUS_CODE_HPP
