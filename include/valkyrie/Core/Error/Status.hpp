//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_STATUS_HPP
#define VALKYRIE_STATUS_HPP

#include <valkyrie/Core/Config.hpp>
#include <valkyrie/Core/Error/GenericCode.hpp>
#include <valkyrie/Core/Error/Severity.hpp>
#include <valkyrie/Core/Utility/Span.hpp>
#include <valkyrie/Core/Utility/StringView.hpp>
#include <valkyrie/Core/Utility/Uuid.hpp>

#include <atomic>
#include <utility>
#include <span>


namespace valkyrie::Core{

  // Declaration

  template <typename>
  class Erased;
  template <typename>
  class StatusCode;
  template <typename Domain>
  class ErrorCode;
  class StatusDomain;


  namespace Detail{
    class GenericDomain;
    template <typename E>
    class StatusEnumDomain;
  }

  template <typename T, typename Base = StatusDomain>
  class UniqueStatusDomain;

  using Error         = ErrorCode<Erased<u64>>;
  using Status        = StatusCode<Erased<u64>>;
  using GenericError  = ErrorCode<Detail::GenericDomain>;
  using GenericStatus = StatusCode<Detail::GenericDomain>;





  VK_noreturn void panic(const utf8* pMessage) noexcept;
  VK_noreturn void panic(const StatusCode<void>& statusCode) noexcept;
  VK_noreturn void panic(const StatusCode<void>& statusCode, const utf8* pMessage) noexcept;

  VK_nodiscard GenericStatus makeStatusCode(Code code) noexcept;









  // Implementation



  class StatusDomain{
    template <typename>
    friend class StatusCode;

  protected:
    virtual bool doFailure(const StatusCode<void>&) const noexcept = 0;
    virtual bool doEquivalent(const StatusCode<void>&, const StatusCode<void>&) const noexcept = 0;
    virtual Code doCode(const StatusCode<void>&) const noexcept = 0;
    virtual StringRef doMessage(const StatusCode<void>&) const noexcept = 0;
    virtual Severity  doSeverity(const StatusCode<void>& status) const noexcept { return getDefaultSeverity(this->doCode(status)); }
    virtual void doErasedCopy(StatusCode<void>& To, const StatusCode<void>& From, size_t Bytes) const noexcept { std::memcpy(&To, &From, Bytes); }
    virtual void doErasedDestroy(StatusCode<void>& Code, size_t Bytes) const noexcept { }

    VK_if(VK_not(VK_exceptions_enabled)(inline)) virtual void doThrowException(const StatusCode<void>& Code) const VK_if(VK_exceptions_enabled(;)VK_else( noexcept { panic(Code); }))

  public:

    using string_ref = Core::StringRef;

    virtual StringView name() const noexcept = 0;
    virtual const Uuid& id() const noexcept = 0;



    /*[[nodiscard]] u64 id() const noexcept{
      auto&& Name = this->name();
      return Util::hash(std::span{Name.data(), Name.size()});
    }*/

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

    class GenericDomain final : public StatusDomain{
    public:
      using value_type = Code;
      using domain_type = GenericDomain;

      constexpr GenericDomain() = default;
      ~GenericDomain() = default;

      VK_nodiscard StringView name() const noexcept override;
      VK_nodiscard StringRef doMessage(const StatusCode<void>& Stat) const noexcept override;
      VK_nodiscard Code doCode(const StatusCode<void>& ) const noexcept override;
      VK_nodiscard bool doFailure(const StatusCode<void> &) const noexcept override;
      VK_nodiscard bool doEquivalent(const StatusCode<void>& , const StatusCode<void>&) const noexcept override;
      VK_noreturn void doThrowException(const StatusCode<void> &Code) const override;

      VK_nodiscard static constexpr const GenericDomain& get() noexcept;
      VK_nodiscard const Uuid& id() const noexcept override;
    };

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
  }




  template <typename To, Detail::ErasureCastableTo<To> From> requires(sizeof(To) == sizeof(From))
  constexpr To erasure_cast(const From& from) noexcept {
    return bit_cast<To>(from);
  }
  template <typename To, Detail::StaticLargerErasureCastableTo<To> From>
  constexpr To erasure_cast(const From& from) noexcept {
    return static_cast<To>(bit_cast<Detail::ErasureIntegerType<From, To>>(from));
  }
  template <typename To, Detail::StaticSmallerErasureCastableTo<To> From>
  constexpr To erasure_cast(const From& from) noexcept {
    return bit_cast<To>(static_cast<Detail::ErasureIntegerType<To, From>>(from));
  }
  template <typename To, Detail::LargerErasureCastableTo<To> From>
  constexpr To erasure_cast(const From& from) noexcept {
    return bit_cast<Detail::PaddedErasureObject<To, sizeof(From) - sizeof(To)>>(from).Val;
  }
  template <typename To, Detail::SmallerErasureCastableTo<To> From>
  constexpr To erasure_cast(const From& from) noexcept {
    return bit_cast<To>(Detail::PaddedErasureObject<From, sizeof(To) - sizeof(From)>{from});
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

    constexpr explicit StatusCode(const StatusDomain* Domain) noexcept : Domain(Domain){}
  public:
    VK_nodiscard VK_inline constexpr const StatusDomain& domain() const noexcept { return *Domain; }
    VK_nodiscard VK_inline constexpr bool empty() const noexcept { return !Domain; }

    VK_nodiscard string_ref message() const noexcept { return Domain ? Domain->doMessage(*this) : string_ref("(empty)"); }
    VK_nodiscard Severity   severity() const noexcept { return empty() ? Severity{} : Domain->doSeverity(*this); }
    VK_nodiscard bool       success() const noexcept { return !empty() && !Domain->doFailure(*this); }
    VK_nodiscard bool       failure() const noexcept { return !empty() && Domain->doFailure(*this); }
    VK_nodiscard Code       generic() const noexcept { return empty() ? Code::Unknown : Domain->doCode(*this); }


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
    template <typename Domain>
    struct GetDomainValueType{
      using domain_type = Domain;
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

      VK_nodiscard constexpr const domain_type &domain() const noexcept { return *static_cast<const domain_type *>(Domain); }

      constexpr void clear() noexcept {
        Value.~value_type();
        Domain = nullptr;
        new(&Value) value_type();
      }

      constexpr value_type & value() & noexcept { return this->Value; }
      constexpr const value_type & value() const & noexcept { return this->Value; }
      constexpr value_type && value() && noexcept { return std::move(this->Value); }
      constexpr const value_type && value() const && noexcept { return std::move(this->Value); }

    protected:

      StatusStorage() noexcept = default;
      StatusStorage(const StatusStorage&) noexcept = default;
      constexpr StatusStorage(StatusStorage&& Other) noexcept
          : Base(static_cast<StatusStorage&&>(Other)),
            Value(static_cast<StatusStorage&&>(Other).Value){
        Other.Domain = nullptr;
      }
      template <typename ...Args> requires(std::constructible_from<value_type, Args...>)
      constexpr explicit StatusStorage(std::in_place_t, const domain_type * Domain, Args&& ...args) noexcept
          : Base(Domain),
            Value(std::forward<Args>(args)...){}


      StatusStorage& operator=(const StatusStorage&) = default;
      constexpr StatusStorage& operator=(StatusStorage&& Other) noexcept {
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

    using domain_type = Dom;
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

    constexpr StatusCode clone() const { return *this; }

    template <NotSameAsOneOf<StatusCode, std::in_place_t> T, typename ...Args>
    constexpr StatusCode(T&& val, Args&& ...args) noexcept(noexcept(makeStatusCode((T&&)val, (Args&&)args...)))
        requires(requires{ { makeStatusCode(std::forward<T>(val), std::forward<Args>(args)...) } -> std::convertible_to<StatusCode>; })
        : StatusCode(makeStatusCode(std::forward<T>(val), std::forward<Args>(args)...)){}

    template <StatusEnum Enum>
    requires(std::constructible_from<ThisType, typename enum_traits<Enum>::status_type>)
    constexpr StatusCode(Enum&& E) noexcept(std::is_nothrow_constructible_v<ThisType, typename enum_traits<Enum>::status_type>) 
        : StatusCode(typename enum_traits<Enum>::status_type(std::forward<Enum>(E))){}

    template <typename ...Args> requires(ConstantDomain && std::constructible_from<value_type, Args...>)
    constexpr explicit StatusCode(std::in_place_t, Args&& ...args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>)
        : Base(std::in_place, std::addressof(domain_type::get()),  std::forward<Args>(args)...){}

    template <typename T, typename ...Args> requires(ConstantDomain && std::constructible_from<value_type, std::initializer_list<T>, Args...>)
    constexpr explicit StatusCode(std::in_place_t, std::initializer_list<T> Il, Args&& ...args) noexcept(std::is_nothrow_constructible_v<value_type, std::initializer_list<T>, Args...>)
        : Base(std::in_place, std::addressof(domain_type::get()), Il, std::forward<Args>(args)...){}

    constexpr explicit StatusCode(const value_type & Other) noexcept(std::is_nothrow_copy_constructible_v<value_type>)
    requires(ConstantDomain && std::copy_constructible<value_type>)
        : Base(std::in_place, std::addressof(domain_type::get()), Other){}

    constexpr explicit StatusCode(value_type && Other) noexcept(std::is_nothrow_move_constructible_v<value_type>)
    requires(ConstantDomain && std::move_constructible<value_type>)
        : Base(std::in_place, std::addressof(domain_type::get()), std::move(Other)){}


    template <Detail::SafelyErasableFrom<value_type> ErasedType>
    constexpr explicit StatusCode(const StatusCode<Erased<ErasedType>>& Other) noexcept(std::is_nothrow_copy_constructible_v<value_type>) requires(std::constructible_from<ThisType, value_type>)
        : StatusCode(erasure_cast<value_type>(Other.value())){
      VK_axiom(this->domain() == Other.domain())
    }

      VK_nodiscard string_ref message() const noexcept { return this->Domain ? this->Domain->doMessage(*this) : string_ref("(empty)"); }
    };

  template <typename Arg, typename ...Args> requires(requires(Arg&& arg, Args&& ...args){
    { makeStatusCode((Arg&&)arg, (Args&&)args...) } -> std::derived_from<StatusCode<void>>;
  }) StatusCode(Arg&& arg, Args&& ...args) noexcept  -> StatusCode<typename decltype(makeStatusCode((Arg&&)arg, (Args&&)args...))::DomainType>;

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
    constexpr StatusCode(const StatusCode<Dom>& Other) noexcept
        : Base(std::in_place, &Other.domain(), erasure_cast<value_type>(Other.value())){}

    template <typename Dom>
    requires(Detail::SafelyErasableFrom<value_type, typename Dom::value_type> &&
             std::is_trivially_move_constructible_v<typename Dom::value_type>)
    constexpr StatusCode(StatusCode<Dom>&& Other) noexcept
        : Base(std::in_place, &Other.domain(), erasure_cast<value_type>(std::move(Other).value())){}


    template <NotSameAsOneOf<ThisType, std::in_place_t> T, typename ...Args>
    constexpr StatusCode(T&& val, Args&& ...args) noexcept(noexcept(makeStatusCode((T&&)val, (Args&&)args...)))
        requires(requires{ { makeStatusCode((T&&)val, (Args&&)args...) } -> std::convertible_to<ThisType>; })
        : StatusCode(makeStatusCode(std::forward<T>(val), std::forward<Args>(args)...)){}

    template <StatusEnum Enum>
    requires(std::constructible_from<ThisType, typename enum_traits<Enum>::status_type>)
    constexpr StatusCode(Enum&& E) noexcept(std::is_nothrow_constructible_v<ThisType, typename enum_traits<Enum>::status_type>)
        : StatusCode(typename enum_traits<Enum>::status_type(std::forward<Enum>(E))){}


    constexpr value_type value() const noexcept { return this->Value; }
  };


  template <StatusEnum E>
  StatusCode(E) -> StatusCode<typename enum_traits<E>::status_domain>;

  template <typename Dom>
  class ErrorCode : public StatusCode<Dom>{

    using Base = StatusCode<Dom>;
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
        Base(Other){
      _checkIfSuccess();
    }
    explicit ErrorCode(Base&& Other) noexcept(std::is_nothrow_move_constructible_v<Base>) :
        Base(std::move(Other)){
      _checkIfSuccess();
    }


    template <NotSameAsOneOf<ThisType, std::in_place_t> T, typename ...Args>
    constexpr ErrorCode(T&& val, Args&& ...args) noexcept(noexcept(makeStatusCode((T&&)val, (Args&&)args...)))
        requires(requires{ { makeStatusCode((T&&)val, (Args&&)args...) } -> std::convertible_to<ThisType>; })
        : ErrorCode(makeStatusCode(std::forward<T>(val), std::forward<Args>(args)...)){
      _checkIfSuccess();
    }

    template <StatusEnum Enum>
    requires(std::constructible_from<ThisType, typename enum_traits<Enum>::status_type>)
    constexpr ErrorCode(Enum&& E) noexcept(std::is_nothrow_constructible_v<ErrorCode, typename enum_traits<Enum>::status_type>)
        : ErrorCode(typename enum_traits<Enum>::status_type(std::forward<Enum>(E))){
      _checkIfSuccess();
    }

    template <typename ...Args>
    constexpr explicit ErrorCode(std::in_place_t, Args&& ...args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>)
    requires(std::constructible_from<Base, std::in_place_t, Args...>)
        : Base(std::in_place, std::forward<Args>(args)...){
      _checkIfSuccess();
    }

    template <typename T, typename ...Args>
    constexpr explicit ErrorCode(std::in_place_t, std::initializer_list<T> Il, Args&& ...args) noexcept(std::is_nothrow_constructible_v<value_type, std::initializer_list<T>, Args...>)
    requires(std::constructible_from<Base, std::in_place_t, std::initializer_list<T>, Args...>)
        : Base(std::in_place, Il, std::forward<Args>(args)...){
      _checkIfSuccess();
    }

    constexpr explicit ErrorCode(const value_type& Other) noexcept(std::is_nothrow_copy_constructible_v<value_type>)
    requires(std::is_nothrow_constructible_v<Base, const value_type&>)
        : Base(std::in_place, Other){
      _checkIfSuccess();
    }

    constexpr explicit ErrorCode(value_type&& Other) noexcept(std::is_nothrow_move_constructible_v<value_type>)
    requires(std::is_nothrow_constructible_v<Base, value_type&&>)
        : Base(std::in_place, std::move(Other)){
      _checkIfSuccess();
    }


    template <Detail::SafelyErasableFrom<value_type> ErasedType>
    constexpr explicit ErrorCode(const StatusCode<Erased<ErasedType>>& Other) noexcept(std::is_nothrow_copy_constructible_v<value_type>) : ErrorCode(erasure_cast<value_type>(Other.value())) {
      VK_axiom(this->domain() == Other.domain());
      _checkIfSuccess();
    }


    VK_nodiscard constexpr bool success() const noexcept { return false; }
    VK_nodiscard constexpr const value_type& value() const & noexcept { return this->Value; }
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
    requires(std::constructible_from<Base, const StatusCode<Dom>&>)
    constexpr ErrorCode(const StatusCode<Dom>& Other) noexcept :
        Base(Other){
      _checkIfSuccess();
    }

    template <typename Dom>
    requires(std::constructible_from<Base, StatusCode<Dom>&&>)
    constexpr ErrorCode(StatusCode<Dom>&& Other) noexcept :
        Base(std::move(Other)){
      _checkIfSuccess();
    }


    template <typename Dom>
    requires(std::constructible_from<Base, const StatusCode<Dom>&>)
    constexpr ErrorCode(const ErrorCode<Dom>& Other) noexcept :
        Base(static_cast<const StatusCode<Dom>&>(Other)){
      _checkIfSuccess();
    }

    template <typename Dom>
    requires(std::constructible_from<Base, StatusCode<Dom>&&>)
    constexpr ErrorCode(ErrorCode<Dom>&& Other) noexcept :
        Base(static_cast<StatusCode<Dom>&&>(Other)){
      _checkIfSuccess();
    }


    template <NotSameAsOneOf<ThisType, std::in_place_t> T, typename ...Args>
    constexpr ErrorCode(T&& val, Args&& ...args) noexcept(noexcept(makeStatusCode((T&&)val, (Args&&)args...)))
    requires(requires{ { makeStatusCode((T&&)val, (Args&&)args...) } -> std::convertible_to<ThisType>; })
        : ErrorCode(makeStatusCode(std::forward<T>(val), std::forward<Args>(args)...)){
      _checkIfSuccess();
    }

    template <StatusEnum Enum>
    requires(std::constructible_from<ThisType, typename enum_traits<Enum>::status_type>)
    constexpr ErrorCode(Enum&& E) noexcept(std::is_nothrow_constructible_v<ErrorCode, typename enum_traits<Enum>::status_type>)
        : ErrorCode(typename enum_traits<Enum>::status_type(std::forward<Enum>(E))){
      _checkIfSuccess();
    }


    VK_nodiscard constexpr bool success() const noexcept { return false; }
    VK_nodiscard constexpr const value_type& value() const & noexcept { return this->Value; }
  };


  // A value returned with this domain must be explicitly checked or suppressed, otherwise it will panic.
  template <typename Dom>
  class StatusValidationDomain : public Dom{
    static_assert(std::derived_from<Dom, StatusDomain>);
    static_assert(sizeof(typename Dom::value_type) <= sizeof(u32));
  public:

    constexpr StatusValidationDomain() noexcept = default;

    template <typename T>
    struct mixin;

    void doErasedCopy(StatusCode<void>& To, const StatusCode<void>& From, size_t Bytes) const noexcept override {
      VK_assert(From.domain() == *this);
      VK_assert(Bytes == 16);
      std::memcpy(&To, &From, Bytes);
      static_cast<const StatusCode<StatusValidationDomain<Dom>>&>(From).suppress();
    }
    void doErasedDestroy(StatusCode<void>& Code, size_t Bytes) const noexcept override {
      auto& code = static_cast<StatusCode<StatusValidationDomain<Dom>>&>(Code);
      if (!code.checked())
        panic(code);
    }

    template <typename T>
    struct mixin : T{

      using typename T::value_type;

      constexpr mixin() noexcept(std::is_nothrow_constructible_v<T>) : T{}, isChecked(true){}
      constexpr mixin(const mixin& other) = delete;
      constexpr mixin(mixin&& other) noexcept
          : T{std::move(other)}, isChecked(other.isChecked){
        other.isChecked = true;
      }
      template <typename ...Args> requires(std::constructible_from<T, Args...>)
      constexpr mixin(Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
          : T{std::forward<Args>(args)...},
            isChecked(T::success()){}

      constexpr mixin& operator=(const mixin& other) = delete;
      constexpr mixin& operator=(mixin&& other) noexcept {
        T::operator=(std::move(other));
        isChecked = other.isChecked;
        other.suppress();
        return *this;
      }

      ~mixin() {
        if (!isChecked)
          panic(*this);
      }

      constexpr value_type& value() & noexcept {
        isChecked = true;
        return T::value();
      }
      constexpr const value_type& value() const & noexcept {
        isChecked = true;
        return T::value();
      }
      constexpr value_type&& value() && noexcept {
        isChecked = true;
        return std::move(T::value());
      }
      constexpr const value_type&& value() const && noexcept {
        isChecked = true;
        return std::move(T::value());
      }

      constexpr void clear() noexcept {
        T::clear();
        suppress();
      }
      constexpr void suppress() const noexcept {
        isChecked = true;
      }
      constexpr bool checked() const noexcept {
        return isChecked;
      }

    private:
      mutable bool isChecked;
    };
  };


  template <typename StatusCode_>
  class IndirectDomain : public StatusDomain{
    template <typename> friend class StatusCode;
    using Base_ = StatusDomain;
    using domain_type = typename StatusCode_::domain_type;


    inline constexpr static domain_type _domain{};


    using inner_value_type = typename StatusCode_::value_type;

    struct RefCountedStatus : StatusCode_{

      template <typename ...Args> requires(std::constructible_from<StatusCode_, Args...>)
      RefCountedStatus(Args&& ...args)
          : StatusCode_(std::forward<Args>(args)...), refCount_(){}
      RefCountedStatus(const RefCountedStatus&) = delete;
      RefCountedStatus& operator=(const RefCountedStatus&) = delete;

      void     acquireRef() const noexcept {
        //refCount_.fetch_add(1, std::memory_order_relaxed);
        ++refCount_;
      }
      uint32_t releaseRef() const noexcept {
        return --refCount_;
      }


    private:
      mutable std::atomic_uint32_t refCount_{0};
    };



  public:

    inline constexpr static Uuid uuid = Uuid{"c557643b-40a4-4866-9ec8-61b4b744ff7b"} ^ class_traits<domain_type>::uuid;

    template <typename T>
    struct mixin : T{

    protected:
      mixin() = default;
      template <typename ...Args> requires(std::constructible_from<StatusCode_, Args...>)
      mixin(std::in_place_t, const IndirectDomain* pDomain, Args&& ...args)
          : T{std::in_place, pDomain, new RefCountedStatus(std::forward<Args>(args)...)}{}
      template <typename ...Args> requires(std::constructible_from<inner_value_type, Args...>)
      mixin(std::in_place_t, const IndirectDomain* pDomain, Args&& ...args)
          : T{std::in_place, pDomain, new RefCountedStatus(std::in_place, std::forward<Args>(args)...)}{}
      mixin(const mixin& other)     : T(other){
        incRef();
      }
      mixin(mixin&& other) noexcept : T(std::move(other)){
        other.ptr() = nullptr;
      }
      mixin& operator=(const mixin& other) {
        if (ptr() != other.ptr()) {
          decRef();
          T::value() = other.ptr();
          incRef();
        }
        return *this;
      }
      mixin& operator=(mixin&& other) noexcept {
        T::value() = other.ptr();
        other.ptr() = nullptr;
        return *this;
      }
      ~mixin() {
        decRef();
      }

    public:
      void clear() {
        decRef();
        T::clear();
      }

      constexpr inner_value_type& value() & noexcept {
        return T::value()->value();
      }
      constexpr const inner_value_type& value() const & noexcept {
        return T::value()->value();
      }
      constexpr inner_value_type&& value() && noexcept {
        return std::move(T::value()->value());
      }
      constexpr const inner_value_type&& value() const && noexcept {
        return std::move(T::value()->value());
      }

    protected:
      constexpr auto*& ptr() noexcept {
        return T::value();
      }
      constexpr const auto*& ptr() const noexcept {
        return T::value();
      }


      void incRef() {
        if (T::value())
          T::value()->acquireRef();
      }
      void decRef() {
        auto* pValue = T::value();
        if (pValue && !pValue->releaseRef()) {
          delete pValue;
        }
      }

    private:
      using T::clear;
      using T::value;

      template <typename>
      friend class IndirectDomain;
    };

    using value_type = RefCountedStatus*;

    using Base_::string_ref;

  private:
    using code_type = StatusCode<IndirectDomain>;
    using redirected_type = StatusCode_;

    redirected_type&       redirect(StatusCode<void>& code) noexcept {
      return *static_cast<Detail::StatusStorage<IndirectDomain>&>(static_cast<code_type&>(code)).value();
    }
    const redirected_type& redirect(const StatusCode<void>& code) noexcept {
      return *static_cast<const Detail::StatusStorage<IndirectDomain>&>(static_cast<const code_type&>(code)).value();
    }
  public:

    constexpr IndirectDomain() noexcept = default;

    StringView name() const noexcept override{
      return _domain.path();
    }
  protected:
    bool doFailure(const StatusCode<void>& code) const noexcept override{
      VK_assert(code.domain() == *this);
      return _domain.doFailure(redirect(code));
    }
    Severity doSeverity(const StatusCode<void>& code) const noexcept override{
      VK_assert(code.domain() == *this);
      return _domain.doSeverity(redirect(code));
    }
    Code doCode(const StatusCode<void>& code) const noexcept override{
      VK_assert(code.domain() == *this);
      return _domain.doCode(redirect(code));
    }
    string_ref doMessage(const StatusCode<void>& code) const noexcept override{
      VK_assert(code.domain() == *this);
      return _domain.doMessage(redirect(code));
    }
    bool doEquivalent(const StatusCode<void> & codeA, const StatusCode<void> & codeB) const noexcept override{
      VK_assert(codeA.domain() == *this);
      return _domain.doEquivalent(redirect(codeA), codeB);
    }
    VK_noreturn void doThrowException(const StatusCode<void>& code) const VK_throws override{
      VK_assert(code.domain() == *this);
      _domain.doThrowException(redirect(code));
    }
    void doErasedCopy(StatusCode<void>& To, const StatusCode<void>& From, size_t Bytes) const noexcept override{
      VK_assert(From.domain() == *this);
      VK_assert(Bytes == 16);
      new(&To) code_type(static_cast<const code_type&>(From));
    }
    void doErasedDestroy(StatusCode<void> & code, size_t Bytes) const noexcept override{
      VK_assert(code.domain() == *this);
      VK_assert(Bytes == 16);
      static_cast<code_type&>(code).~code_type();
    }

    inline constexpr static const IndirectDomain& get() noexcept;
  };

  namespace Detail{
    template <typename E>
    class StatusEnumDomain : public StatusDomain {
      template <typename> friend class Core::StatusCode;
      using traits = enum_traits<E>;
      //using status_traits = Core::status_traits<typename traits::status_type>;
      using status_type = StatusCode<StatusEnumDomain>;

      using self = StatusEnumDomain;


      inline static status_type& cast(StatusCode<void>& status) noexcept { return static_cast<status_type&>(status); }
      inline static const status_type& cast(const StatusCode<void>& status) noexcept { return static_cast<const status_type&>(status); }

      inline static const auto& info(const StatusCode<void>& status) noexcept {
        return traits::info[self::cast(status).value()];
      }
      inline static std::span<const Code> codeMappings(const StatusCode<void>& status) noexcept {
        return info(status).generic;
      }

    public:
      using value_type = E;
      using domain_type = StatusEnumDomain;

      constexpr StatusEnumDomain() = default;
      ~StatusEnumDomain() = default;

      VK_nodiscard StringRef doMessage(const StatusCode<void>& status) const noexcept override {
        return StringRef(self::info(status).message);
      }
      VK_nodiscard Code doCode(const StatusCode<void>& status) const noexcept override {
        VK_axiom(status.domain() == *this);
        if (auto mappings = codeMappings(status); mappings.empty())
          return Code::Unknown;
        else
          return mappings.front();
      }
      VK_nodiscard bool doFailure(const StatusCode<void>& status) const noexcept override {
        for (Code code : codeMappings(status))
          if (code == Code::Success)
            return false;
        return true;
      }
      VK_nodiscard bool doEquivalent(const StatusCode<void>& A, const StatusCode<void>& B) const noexcept override {
        VK_axiom(A.domain() == *this);
        if (B.domain() == *this)
          return cast(A).value() == cast(B).value();
        if (B.domain() == GenericDomain::get()) {
          const auto& genB = static_cast<const GenericStatus&>(B);
          for (Code ec : codeMappings(A))
            if (ec == genB)
              return true;
        }
        return false;
      }
      //VK_if(VK_exceptions_enabled(VK_noreturn void doThrowException(const StatusCode<void> &Code) const override {  }))

      VK_nodiscard inline static constexpr const StatusEnumDomain<E>& get() noexcept;
      VK_nodiscard const Uuid& id() const noexcept override { return traits::domain_uuid; }
      VK_nodiscard StringView name() const noexcept override { return traits::domain_name; }
    };

    template <typename StatusCode_>
    inline constexpr static IndirectDomain<StatusCode_> indirectDomainInstance{};

    template <typename E>
    inline constexpr static StatusEnumDomain<E> statusEnumDomainInstance{};
    template <typename E>
    inline constexpr const StatusEnumDomain<E>& StatusEnumDomain<E>::get() noexcept {
      return statusEnumDomainInstance<E>;
    }
  }

  template <typename StatusCode_>
  inline constexpr const IndirectDomain<StatusCode_>& IndirectDomain<StatusCode_>::get() noexcept {
    return Detail::indirectDomainInstance<StatusCode_>;
  }
}

namespace valkyrie{
  template <typename Dom>
  struct Traits::Status<Core::StatusCode<Dom>>{
    using domain_type = Dom;
    //using value_type = typename domain_type::value_type;
  };
  template <typename Dom>
  struct Traits::Status<Core::StatusCode<Core::Erased<Dom>>>{
    using domain_type = Core::StatusDomain;
    using value_type = Dom;
  };
  template <>
  struct Traits::Status<Core::StatusCode<void>>{
    using domain_type = Core::StatusDomain;
  };
  template <>
  struct Traits::Singleton<Core::Detail::GenericDomain>{
    inline constexpr static Core::StringView name{VK_raw_string(GenericDomain)};
    inline constexpr static Core::Uuid       uuid{"4be5eed5-f1c2-4495-8630-0c5d8107d4a1"};
    inline constexpr static const Core::Detail::GenericDomain& get() noexcept;
  };
}



#define PP_VK_impl_SYSTEM_ERROR_CODE_HEADER_FILE <valkyrie/Core/Error/System/VK_if(VK_system_windows(Win32)VK_else(Posix)).hpp>

#include PP_VK_impl_SYSTEM_ERROR_CODE_HEADER_FILE

#endif//VALKYRIE_STATUS_HPP
