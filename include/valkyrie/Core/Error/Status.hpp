//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_STATUS_HPP
#define VALKYRIE_STATUS_HPP

/*#include <valkyrie/Core/Error/GenericCode.hpp>*/
#include <valkyrie/Core/Utility/StringView.hpp>

#include <atomic>
#include <utility>


namespace valkyrie::Core{

  // Declaration

  template <typename>
  class Erased;
  template <typename>
  class StatusCode;
  template <typename>
  class ErrorCode;
  class StatusDomain;

  class GenericDomain;
  template <typename E>
  class StatusEnumDomain;

  template <typename T, typename Base = StatusDomain>
  class UniqueStatusDomain;

  enum class Code : i32;
  enum class Severity : i32;

  VK_noreturn void assertPanic(StringView msg) noexcept;
  VK_noreturn void panic(const utf8* pMessage) noexcept;
  VK_noreturn void panic(const StatusCode<void>& statusCode) noexcept;
  VK_noreturn void panic(const StatusCode<void>& statusCode, const utf8* pMessage) noexcept;

  inline Severity getDefaultSeverity(Code code) noexcept;











  // Implementation



  class StatusDomain{
    template <typename>
    friend class StatusCode;

    u64 guid_;

  protected:

    constexpr StatusDomain(const Uuid& uuid) noexcept : guid_(toInteger(uuid)){}


    virtual bool doFailure(const StatusCode<void>& status) const noexcept = 0;
    virtual bool doEquivalent(const StatusCode<void>& statusA, const StatusCode<void>& statusB) const noexcept = 0;
    virtual Code doCode(const StatusCode<void>& status) const noexcept = 0;
    virtual StringRef doMessage(const StatusCode<void>& status) const noexcept = 0;

    virtual Severity  doSeverity(const StatusCode<void>& status) const noexcept { return getDefaultSeverity(this->doCode(status)); }
    virtual void doErasedCopy(StatusCode<void>& To, const StatusCode<void>& From, size_t Bytes) const noexcept { std::memcpy(&To, &From, Bytes); }
    virtual void doErasedDestroy(StatusCode<void>& Code, size_t Bytes) const noexcept { }

    VK_noreturn virtual void doThrowException(const StatusCode<void>& code) const VK_throws;

  public:

    using string_ref = Core::StringRef;

    virtual string_ref name() const noexcept = 0;
    constexpr u64 id() const noexcept {
      return guid_;
    }

    VK_nodiscard friend bool operator==(const StatusDomain& A, const StatusDomain& B) noexcept{
      //return A.name() == B.name();
      return A.id() == B.id();
      //return A.guid() == B.guid();
    }
    VK_nodiscard friend std::strong_ordering operator<=>(const StatusDomain& A, const StatusDomain& B) noexcept {
      return A.id() <=> B.id();
    }
  };



  /*template <typename T, typename Base>
  class UniqueStatusDomain : public Base{
  public:

    using domain_type = T;

    using Base::Base;


    const Uuid& id() const noexcept override { return class_traits<T>::uuid; }
  };*/


  namespace Detail{

    template <typename T>
    struct identity_type{
      using type = T;
    };

    template <typename To, typename From>
    concept StaticCastableFrom = IsRegisterCopyable<To> && IsRegisterCopyable<From> && requires(const From& from){
      {static_cast<To>(from)} noexcept;
    };

    template <typename From, typename To>
    concept ErasureCastableTo = BitwiseMovable<From> || BitwiseMovable<To>;
    template <typename From, typename To>
    concept StaticErasureCastableTo = ErasureCastableTo<From, To> && StaticCastableFrom<To, From>;

    template <typename From, typename To>
    concept LargerThan = sizeof(From) > sizeof(To);
    template <typename From, typename To>
    concept SmallerThan = !LargerThan<From, To> && sizeof(From) != sizeof(To);

    template <typename From, typename To>
    concept LargerErasureCastableTo = ErasureCastableTo<From, To> && LargerThan<From, To>;
    template <typename From, typename To>
    concept SmallerErasureCastableTo = ErasureCastableTo<From, To> && SmallerThan<From, To>;
    template <typename From, typename To>
    concept StaticLargerErasureCastableTo = StaticErasureCastableTo<From, To> && LargerThan<From, To>;
    template <typename From, typename To>
    concept StaticSmallerErasureCastableTo = StaticErasureCastableTo<From, To> && SmallerThan<From, To>;

    template <typename T>
    using UnderlyingType = typename std::conditional_t<Enum<T>, std::underlying_type<T>, identity_type<T>>::type;

    template <typename OfSize, typename OfSign>
    using ErasureIntegerType = std::conditional_t<std::signed_integral<UnderlyingType<OfSign>>, std::make_signed_t<UnderlyingType<OfSize>>, std::make_unsigned_t<UnderlyingType<OfSize>>>;

    template <typename ErasedType, size_t N>
    struct PaddedErasureObject{
      static_assert(BitwiseMovable<ErasedType>, "ErasedType must be BitCopyMovable");
      static_assert(alignof(ErasedType) <= sizeof(ErasedType), "ErasedType must not be over-aligned");

      ErasedType Val;
      char Padding[N];
      constexpr explicit PaddedErasureObject(const ErasedType &V) noexcept :
          Val(V), Padding{}{}
    };

    template <typename T>
    struct StatusSizer{
      void* Ptr;
      T Value;
    };

    template <typename To, typename From>
    concept SafelyErasableFrom = BitwiseMovable<From> && (sizeof(StatusSizer<From>) <= sizeof(StatusSizer<To>));

    template <typename Arg, typename ...Args>
    concept HasMakeStatusCode = requires(Arg&& arg, Args&& ...args){
      { makeStatusCode(std::forward<Arg>(arg), std::forward<Args>(args)...) } -> std::derived_from<StatusCode<void>>;
    };
  }




  template <typename To, Detail::ErasureCastableTo<To> From> requires(sizeof(To) == sizeof(From))
  To erasure_cast(const From& from) noexcept {
    return std::bit_cast<To>(from);
  }
  template <typename To, Detail::StaticLargerErasureCastableTo<To> From>
  To erasure_cast(const From& from) noexcept {
    return static_cast<To>(std::bit_cast<Detail::ErasureIntegerType<From, To>>(from));
  }
  template <typename To, Detail::StaticSmallerErasureCastableTo<To> From>
  To erasure_cast(const From& from) noexcept {
    return std::bit_cast<To>(static_cast<Detail::ErasureIntegerType<To, From>>(from));
  }
  template <typename To, Detail::LargerErasureCastableTo<To> From>
  To erasure_cast(const From& from) noexcept {
    return std::bit_cast<Detail::PaddedErasureObject<To, sizeof(From) - sizeof(To)>>(from).Val;
  }
  template <typename To, Detail::SmallerErasureCastableTo<To> From>
  To erasure_cast(const From& from) noexcept {
    return std::bit_cast<To>(Detail::PaddedErasureObject<From, sizeof(To) - sizeof(From)>{from});
  }



  template <>
  class StatusCode<void>{
    template <typename>
    friend class StatusCode;
  public:
    using domain_type = void;
    using value_type = void;
    using string_ref = typename StatusDomain::string_ref;

  protected:
    const StatusDomain* Domain{nullptr};

    StatusCode() = default;
    StatusCode(const StatusCode &) = default;
    StatusCode(StatusCode &&) noexcept = default;
    StatusCode & operator=(const StatusCode &) = default;
    StatusCode & operator=(StatusCode &&) noexcept = default;
    ~StatusCode() = default;

    explicit StatusCode(const StatusDomain* Domain) noexcept : Domain(Domain){}
  public:
    VK_nodiscard VK_inline const StatusDomain& domain() const noexcept { return *Domain; }
    VK_nodiscard VK_inline bool empty() const noexcept { return !Domain; }

    VK_nodiscard string_ref message() const noexcept { return Domain ? Domain->doMessage(*this) : string_ref("(empty)"); }
    VK_nodiscard Severity   severity() const noexcept { return empty() ? Severity{} : Domain->doSeverity(*this); }
    VK_nodiscard bool       success() const noexcept { return !empty() && !Domain->doFailure(*this); }
    VK_nodiscard bool       failure() const noexcept { return !empty() && Domain->doFailure(*this); }
    VK_nodiscard Code       generic() const noexcept { return empty() ? Code{-1} : Domain->doCode(*this); }


    template <typename T>
    VK_nodiscard bool strictlyEquivalent(const StatusCode<T>& Other) const noexcept{
      if(empty() && Other.empty()) return true;
      if(!empty() && !Other.empty()) return Domain->doEquivalent(*this, Other);
      return false;
    }
    template <typename T, typename ...Args>
    VK_nodiscard bool strictlyEquivalent(T&& Val, Args&& ...args) const noexcept requires(requires{
      { makeStatusCode((T&&)Val, (Args&&)args...) } -> std::derived_from<StatusCode>;
    }){
      return this->strictlyEquivalent(makeStatusCode(std::forward<T>(Val), std::forward<Args>(args)...));
    }
    template <typename T>
    VK_nodiscard bool equivalent(const StatusCode<T>& Other) const noexcept{
      return strictlyEquivalent(Other) ||
             Other.strictlyEquivalent(*this) ||
             (Domain->doCode(*this) == Other.Domain->doCode(Other));
    }
    template <typename T, typename ...Args>
    VK_nodiscard bool equivalent(T&& Val, Args&& ...args) const noexcept requires(requires{
      { makeStatusCode((T&&)Val, (Args&&)args...) } -> std::derived_from<StatusCode>;
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
    VK_nodiscard friend bool operator==(const StatusCode& A, T&& Val) noexcept requires(requires{
      { A.equivalent(std::forward<T>(Val)) } noexcept;
    }){
      return A.equivalent(std::forward<T>(Val));
    }*/
  };

  namespace Detail{
    template <typename Dom>
    struct GetDomainValueType{
      using domain_type = Dom;
      using value_type = typename domain_type::value_type;
    };
    template <typename ErasedType>
    struct GetDomainValueType<Erased<ErasedType>>{
      using domain_type = StatusDomain;
      using value_type = ErasedType;
    };

    template <typename DomainT>
    class StatusStorage : public StatusCode<void>{
      using Base = StatusCode<void>;
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
    concept ConstantDomain = requires{ { T::get() } -> std::convertible_to<const StatusDomain&>; };
  }

  template <typename Dom>
  class StatusCode : public Detail::StatusCodeBase<Dom>{
    template <typename> friend class StatusCode;
    using Base = Detail::StatusCodeBase<Dom>;

    using ThisType = StatusCode;

  public:

    //using domain_type = typename domain_traits<Dom>::domain_type;
    using domain_type = Dom;
    //using value_type = typename domain_traits<Dom>::value_type;
    using value_type = typename domain_type::value_type;
    using string_ref = typename domain_type::string_ref;

  protected:
    using Base::Base;

    inline constexpr static bool ConstantDomain = Detail::ConstantDomain<domain_type>;

  public:
    StatusCode() = default;
    StatusCode(const StatusCode &) = default;
    StatusCode(StatusCode &&) noexcept = default;
    StatusCode & operator=(const StatusCode &) = default;
    StatusCode & operator=(StatusCode &&) noexcept = default;
    ~StatusCode() = default;

    StatusCode clone() const { return *this; }

    /*template <NotSameAs<Dom> OtherDom> requires(ConstructibleFrom<value_type, const typename *//*domain_traits<*//*OtherDom*//*>*//*::value_type&>)
    StatusCode(const StatusCode<OtherDom>& other) noexcept : StatusCode(other.value()){}
    template <NotSameAs<Dom> OtherDom> requires(ConstructibleFrom<value_type, typename *//*domain_traits<*//*OtherDom*//*>*//*::value_type&&>)
    StatusCode(StatusCode<OtherDom>&& other) noexcept : StatusCode(std::move(other).value()){}*/


    template <NotSameAsOneOf<StatusCode, std::in_place_t> T, typename ...Args>
    StatusCode(T&& val, Args&& ...args) noexcept(noexcept(makeStatusCode((T&&)val, (Args&&)args...)))
        requires(requires{ { makeStatusCode(std::forward<T>(val), std::forward<Args>(args)...) } -> std::convertible_to<StatusCode>; })
        : StatusCode(makeStatusCode(std::forward<T>(val), std::forward<Args>(args)...)){}

    /*template <StatusEnum Enum>
    requires(ConstructibleFrom<ThisType, typename enum_traits<Enum>::status_type>)
    StatusCode(Enum&& E) noexcept(std::is_nothrow_constructible_v<ThisType, typename enum_traits<Enum>::status_type>)
        : StatusCode(typename enum_traits<Enum>::status_type(std::forward<Enum>(E))){}*/

    template <typename ...Args> requires(ConstantDomain && ConstructibleFrom<value_type, Args...>)
    explicit StatusCode(std::in_place_t, Args&& ...args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>)
        : Base(std::in_place, std::addressof(domain_type::get()),  std::forward<Args>(args)...){}

    template <typename T, typename ...Args> requires(ConstantDomain && ConstructibleFrom<value_type, std::initializer_list<T>, Args...>)
    explicit StatusCode(std::in_place_t, std::initializer_list<T> Il, Args&& ...args) noexcept(std::is_nothrow_constructible_v<value_type, std::initializer_list<T>, Args...>)
        : Base(std::in_place, std::addressof(domain_type::get()), Il, std::forward<Args>(args)...){}

    explicit(!StatusEnum<value_type>) StatusCode(const value_type & Other) noexcept(std::is_nothrow_copy_constructible_v<value_type>)
    requires(ConstantDomain && std::copy_constructible<value_type>)
        : Base(std::in_place, std::addressof(domain_type::get()), Other){}

    explicit(!StatusEnum<value_type>) StatusCode(value_type && Other) noexcept(std::is_nothrow_move_constructible_v<value_type>)
    requires(ConstantDomain && std::move_constructible<value_type>)
        : Base(std::in_place, std::addressof(domain_type::get()), std::move(Other)){}


    template <Detail::SafelyErasableFrom<value_type> ErasedType>
    explicit StatusCode(const StatusCode<Erased<ErasedType>>& Other)
    noexcept(std::is_nothrow_copy_constructible_v<value_type>)
    requires(ConstantDomain && std::move_constructible<value_type>)
        : StatusCode(erasure_cast<value_type>(Other.value())){
      VK_assert(this->domain() == Other.domain());
    }




    VK_nodiscard string_ref message() const noexcept { return this->Domain ? this->Domain->doMessage(*this) : string_ref("(empty)"); }
  };

  template <typename Arg, typename ...Args> requires(Detail::HasMakeStatusCode<Arg, Args...>)
  StatusCode(Arg&& arg, Args&& ...args) noexcept  -> StatusCode<typename decltype(makeStatusCode((Arg&&)arg, (Args&&)args...))::domain_type>;

  template <typename ErasedType>
  class StatusCode<Erased<ErasedType>> : public Detail::StatusCodeBase<Erased<ErasedType>>{
    using Base = Detail::StatusCodeBase<Erased<ErasedType>>;

    using ThisType = StatusCode;

  public:
    using domain_type = void;
    using value_type = ErasedType;
    using typename Base::string_ref;

    StatusCode() = default;
    StatusCode(const StatusCode &) = delete;
    StatusCode(StatusCode &&) noexcept = default;
    StatusCode & operator=(const StatusCode &) = delete;
    StatusCode & operator=(StatusCode &&) noexcept = default;
    ~StatusCode(){
      if(this->Domain)
        this->Domain->doErasedDestroy(*this, sizeof(StatusCode));
    }

    StatusCode clone() const {
      if(!this->Domain)
        return {};
      StatusCode Copy;
      this->Domain->doErasedCopy(Copy, *this, sizeof(StatusCode));
      return std::move(Copy);
    }

    template <typename Dom>
    requires(Detail::SafelyErasableFrom<value_type, typename Dom::value_type> &&
             std::is_trivially_copy_constructible_v<typename Dom::value_type>)
    StatusCode(const StatusCode<Dom>& Other) noexcept
        : Base(std::in_place, &Other.domain(), erasure_cast<value_type>(Other.value())){}

    template <typename Dom>
    requires(Detail::SafelyErasableFrom<value_type, typename Dom::value_type> &&
             std::is_trivially_move_constructible_v<typename Dom::value_type>)
    StatusCode(StatusCode<Dom>&& Other) noexcept
        : Base(std::in_place, &Other.domain(), erasure_cast<value_type>(std::move(Other).value())){}


    template <NotSameAsOneOf<ThisType, std::in_place_t> T, typename ...Args>
    StatusCode(T&& val, Args&& ...args) noexcept(noexcept(makeStatusCode((T&&)val, (Args&&)args...)))
        requires(requires{ { makeStatusCode((T&&)val, (Args&&)args...) } -> std::convertible_to<ThisType>; })
        : StatusCode(makeStatusCode(std::forward<T>(val), std::forward<Args>(args)...)){}

    template <StatusEnum Enum>
    requires(ConstructibleFrom<ThisType, typename enum_traits<Enum>::status_type>)
    StatusCode(Enum&& E) noexcept(std::is_nothrow_constructible_v<ThisType, typename enum_traits<Enum>::status_type>)
        : StatusCode(typename enum_traits<Enum>::status_type(std::forward<Enum>(E))){}


    value_type value() const noexcept { return this->Value; }
  };


  template <StatusEnum E>
  StatusCode(E) -> StatusCode<typename enum_traits<E>::status_domain>;

  template <typename Dom>
  class ErrorCode : public StatusCode<Dom>{

    using Base = StatusCode<Dom>;
    using Base::success;

    using ThisType = ErrorCode;

    inline void _checkIfSuccess() const;

  public:
    using typename Base::value_type;
    using typename Base::string_ref;

    ErrorCode() = default;
    ErrorCode(const ErrorCode &) = default;
    ErrorCode(ErrorCode &&) noexcept = default;
    ErrorCode & operator=(const ErrorCode &) = default;
    ErrorCode & operator=(ErrorCode &&) noexcept = default;
    ~ErrorCode() = default;


    explicit ErrorCode(const Base& Other) noexcept(std::is_nothrow_copy_constructible_v<Base>) :
        Base(Other){
      _checkIfSuccess();
    }
    explicit ErrorCode(Base&& Other) noexcept(std::is_nothrow_move_constructible_v<Base>) :
        Base(std::move(Other)){
      _checkIfSuccess();
    }


    template <NotSameAsOneOf<ThisType, std::in_place_t> T, typename ...Args>
    ErrorCode(T&& val, Args&& ...args) noexcept(noexcept(makeStatusCode((T&&)val, (Args&&)args...)))
        requires(requires{ { makeStatusCode((T&&)val, (Args&&)args...) } -> std::convertible_to<ThisType>; })
        : ErrorCode(makeStatusCode(std::forward<T>(val), std::forward<Args>(args)...)){
      _checkIfSuccess();
    }

    template <StatusEnum Enum>
    requires(ConstructibleFrom<ThisType, typename enum_traits<Enum>::status_type>)
    ErrorCode(Enum&& E) noexcept(std::is_nothrow_constructible_v<ErrorCode, typename enum_traits<Enum>::status_type>)
        : ErrorCode(typename enum_traits<Enum>::status_type(std::forward<Enum>(E))){
      _checkIfSuccess();
    }

    template <typename ...Args>
    explicit ErrorCode(std::in_place_t, Args&& ...args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>)
    requires(ConstructibleFrom<Base, std::in_place_t, Args...>)
        : Base(std::in_place, std::forward<Args>(args)...){
      _checkIfSuccess();
    }

    template <typename T, typename ...Args>
    explicit ErrorCode(std::in_place_t, std::initializer_list<T> Il, Args&& ...args) noexcept(std::is_nothrow_constructible_v<value_type, std::initializer_list<T>, Args...>)
    requires(ConstructibleFrom<Base, std::in_place_t, std::initializer_list<T>, Args...>)
        : Base(std::in_place, Il, std::forward<Args>(args)...){
      _checkIfSuccess();
    }

    explicit ErrorCode(const value_type& Other) noexcept(std::is_nothrow_copy_constructible_v<value_type>)
    requires(std::is_nothrow_constructible_v<Base, const value_type&>)
        : Base(std::in_place, Other){
      _checkIfSuccess();
    }

    explicit ErrorCode(value_type&& Other) noexcept(std::is_nothrow_move_constructible_v<value_type>)
    requires(std::is_nothrow_constructible_v<Base, value_type&&>)
        : Base(std::in_place, std::move(Other)){
      _checkIfSuccess();
    }


    template <Detail::SafelyErasableFrom<value_type> ErasedType>
    explicit ErrorCode(const StatusCode<Erased<ErasedType>>& Other) noexcept(std::is_nothrow_copy_constructible_v<value_type>) : ErrorCode(erasure_cast<value_type>(Other.value())) {
      VK_axiom(this->domain() == Other.domain());
      _checkIfSuccess();
    }


    VK_nodiscard bool success() const noexcept { return false; }
    VK_nodiscard const value_type& value() const & noexcept { return this->Value; }
  };
  template <typename ErasedType>
  class ErrorCode<Erased<ErasedType>> : public StatusCode<Erased<ErasedType>>{
    using Base = StatusCode<Erased<ErasedType>>;
    using Base::success;

    using ThisType = ErrorCode;

    inline void _checkIfSuccess() const {
      if(Base::success())
        std::terminate(); //TODO: Replace with custom termination functions...
    }

  public:
    using typename Base::value_type;
    using typename Base::string_ref;

    ErrorCode() = default;
    ErrorCode(const ErrorCode &) = default;
    ErrorCode(ErrorCode &&) noexcept = default;
    ErrorCode & operator=(const ErrorCode &) = default;
    ErrorCode & operator=(ErrorCode &&) noexcept = default;
    ~ErrorCode() = default;

    explicit ErrorCode(const Base& Other) noexcept(std::is_nothrow_copy_constructible_v<Base>) :
        Base(Other.clone()){
      _checkIfSuccess();
    }
    explicit ErrorCode(Base&& Other) noexcept(std::is_nothrow_move_constructible_v<Base>) :
        Base(std::move(Other)){
      _checkIfSuccess();
    }

    template <typename Dom>
    requires(ConstructibleFrom<Base, const StatusCode<Dom>&>)
    ErrorCode(const StatusCode<Dom>& Other) noexcept :
        Base(Other){
      _checkIfSuccess();
    }

    template <typename Dom>
    requires(ConstructibleFrom<Base, StatusCode<Dom>&&>)
    ErrorCode(StatusCode<Dom>&& Other) noexcept :
        Base(std::move(Other)){
      _checkIfSuccess();
    }


    template <typename Dom>
    requires(ConstructibleFrom<Base, const StatusCode<Dom>&>)
    ErrorCode(const ErrorCode<Dom>& Other) noexcept :
        Base(static_cast<const StatusCode<Dom>&>(Other)){
      _checkIfSuccess();
    }

    template <typename Dom>
    requires(ConstructibleFrom<Base, StatusCode<Dom>&&>)
    ErrorCode(ErrorCode<Dom>&& Other) noexcept :
        Base(static_cast<StatusCode<Dom>&&>(Other)){
      _checkIfSuccess();
    }


    template <NotSameAsOneOf<ThisType, std::in_place_t> T, typename ...Args>
    ErrorCode(T&& val, Args&& ...args) noexcept(noexcept(makeStatusCode((T&&)val, (Args&&)args...)))
    requires(requires{ { makeStatusCode((T&&)val, (Args&&)args...) } -> std::convertible_to<ThisType>; })
        : ErrorCode(makeStatusCode(std::forward<T>(val), std::forward<Args>(args)...)){
      _checkIfSuccess();
    }

    template <StatusEnum Enum>
    requires(ConstructibleFrom<ThisType, typename enum_traits<Enum>::status_type>)
    ErrorCode(Enum&& E) noexcept(std::is_nothrow_constructible_v<ErrorCode, typename enum_traits<Enum>::status_type>)
        : ErrorCode(typename enum_traits<Enum>::status_type(std::forward<Enum>(E))){
      _checkIfSuccess();
    }


    VK_nodiscard bool success() const noexcept { return false; }
    VK_nodiscard const value_type& value() const & noexcept { return this->Value; }
  };


  using Error         = ErrorCode<Erased<u64>>;
  using Status        = StatusCode<Erased<u64>>;

  template <typename Dom>
  inline void ErrorCode<Dom>::_checkIfSuccess() const {
    if(Base::success())
      assertPanic("An ErrorCode object was created from a non-failure status code. "
                  "Consider using StatusCode instead.");
  }
}

/*namespace valkyrie{

  *//*template <typename ErasedType>
  struct Traits::Domain<Core::Erased<ErasedType>>{
    using domain_type = Core::StatusDomain;
    using value_type  = ErasedType;
    using status_type = Core::StatusCode<Core::Erased<ErasedType>>;
    using error_type  = Core::ErrorCode<Core::Erased<ErasedType>>;
  };
  template <>
  struct Traits::Domain<Core::StatusDomain> : Meta::TemplateNoDefault{};*//*
  *//*template <>
  struct Traits::Domain<Core::GenericDomain>{
    using domain_type = Core::GenericDomain;
    using value_type  = Core::Code;
    using status_type = Core::StatusCode<domain_type>;
    using error_type  = Core::ErrorCode<domain_type>;
  };
  template <StatusEnum E>
  struct Traits::Domain<Core::StatusEnumDomain<E>>{
    using domain_type = Core::StatusEnumDomain<E>;
    using value_type  = E;
    using status_type = Core::StatusCode<domain_type>;
    using error_type  = Core::ErrorCode<domain_type>;
  };
  template <typename Dom>
  struct Traits::Domain<Core::IndirectDomain<Dom>>{
    using domain_type = Core::IndirectDomain<Dom>;
    using value_type  = typename domain_traits<Dom>::value_type*;
  };
  template <typename Dom>
  struct Traits::Domain<Core::StatusValidationDomain<Dom>>{
    using domain_type = Core::StatusValidationDomain<Dom>;
    using value_type  = typename domain_traits<Dom>::value_type;
  };*//*




  *//*template <typename Dom>
  struct Traits::Status<Core::StatusCode<Dom>>{
  private:
    using traits = domain_traits<Dom>;
  public:

    using domain_type = typename traits::domain_type;
    using value_type  = typename traits::value_type;
    using error_type  = typename traits::error_type;
  };
  template <typename Dom>
  struct Traits::Error<Core::ErrorCode<Dom>>{
  private:
    using traits = domain_traits<Dom>;
  public:

    using domain_type = typename traits::domain_type;
    using value_type  = typename traits::value_type;
    using status_type  = typename traits::status_type;
  };

  template <>
  struct Traits::Status<Core::StatusCode<void>>{
    using domain_type = Core::StatusDomain;
  };*//*



  *//*template <>
  struct Traits::Singleton<Core::GenericDomain>{
    inline constexpr static Core::StringView name{VK_raw_string(GenericDomain)};
    inline constexpr static Core::Uuid       uuid{"4be5eed5-f1c2-4495-8630-0c5d8107d4a1"};
    inline constexpr static Core::GenericDomain instance{};
  };
  template <typename Dom>
  struct Traits::Singleton<Core::StatusValidationDomain<Dom>>{
    inline constexpr static Core::Uuid uuid = Core::Uuid{"b8af8d60-1fa7-4ad8-b032-5cf0aa9728e5"} ^ class_traits<Dom>::uuid;
  };*//*

  namespace Core{

  }
}*/


#if VK_system_windows
#include <valkyrie/Core/Error/System/Win32.hpp>
#else
#include <valkyrie/Core/Error/System/Posix.hpp>
#endif

#endif//VALKYRIE_STATUS_HPP
