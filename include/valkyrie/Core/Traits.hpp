//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_TRAITS_HPP
#define VALKYRIE_TRAITS_HPP

#include <valkyrie/Core/Meta.hpp>
#include <valkyrie/Core/Error/Severity.hpp>
#include <valkyrie/Core/Utility/Uuid.hpp>

#include <ranges>
#include <bit>

#define VK_trait(name_, decl_, given_, ...) struct name_ {                              \
    template <size_t, PP_VK_impl_TRAIT_PARAMS_##given_>                                   \
    struct Trait;                                                                         \
    template < PP_VK_impl_TRAIT_PARAMS_##given_ >                                         \
    struct Trait<0, PP_VK_impl_TRAIT_ARGS_##given_ > : Meta::Fallback{};                   \
    PP_VK_impl_TRAIT_BODY_MACRO(PP_VK_impl_TRAIT_BODY_MACRO_##decl_, PP_VK_impl_TRAIT_BIND_##given_, ##__VA_ARGS__)\
    };
#define VK_valid_typename(...) requires{ typename __VA_ARGS__; }
#define VK_complete_type(...) requires{ typename __VA_ARGS__; { sizeof(typename __VA_ARGS__) }; }




namespace valkyrie{

  namespace Core{
    template <typename>
    class StatusCode;
    template <typename Domain>
    class ErrorCode;
    class StatusDomain;
    class GenericDomain;
    template <typename E>
    class StatusEnumDomain;
  }

  inline namespace Concepts{
    template <typename T, typename U>
    concept SameAs = std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;
    template <typename T, typename U>
    concept ExactSameAs = std::same_as<T, U> && SameAs<T, U>;



    template <typename T, typename U>
    concept NotExactSameAs = !ExactSameAs<T, U>;
    template <typename T, typename U>
    concept NotSameAs = !SameAs<T, U> && NotExactSameAs<T, U>;

    template <typename T, typename ...U>
    concept SameAsOneOf = (SameAs<T, U> || ...);
    template <typename T, typename ...U>
    concept ExactSameAsOneOf = (ExactSameAs<T, U> || ...) && SameAsOneOf<T, U...>;


    template <typename T, typename ...U>
    concept NotExactSameAsOneOf = (NotExactSameAs<T, U> && ...) && !ExactSameAsOneOf<T, U...>;
    template <typename T, typename ...U>
    concept NotSameAsOneOf = (NotSameAs<T, U> && ...) && !SameAsOneOf<T, U...> && NotExactSameAsOneOf<T, U...>;



    template <typename P, typename T = const void>
    concept RawPointer = std::is_pointer_v<P> && std::convertible_to<P, T*>;
    template <typename T>
    concept Enum = std::is_enum_v<T>;
    template <typename TT, typename ...T>
    concept Specialized = requires{
      typename TT::template Instantiate<T...>;
      sizeof(typename TT::template Instantiate<T...>);
    } && !requires{
      typename TT::template Instantiate<T...>::invalid_specialization;
    };
    template <typename T, typename U>
    concept EqualityComparableWith = requires(const T& t, const U& u){
      { t == u } -> std::same_as<bool>;
      { u == t } -> std::same_as<bool>;
      { t != u } -> std::same_as<bool>;
      { u != t } -> std::same_as<bool>;
    };
    template <typename T>
    concept EqualityComparable = EqualityComparableWith<T, T>;
    template <typename T, typename U>
    concept OrderedWith = EqualityComparableWith<T, U> && requires(const T& t, const U& u) {
      { t < u } -> std::convertible_to<bool>;
      { t > u } -> std::convertible_to<bool>;
      { t <= u } -> std::convertible_to<bool>;
      { t <= u } -> std::convertible_to<bool>;
      { u < t } -> std::convertible_to<bool>;
      { u > t } -> std::convertible_to<bool>;
      { u <= t } -> std::convertible_to<bool>;
      { u <= t } -> std::convertible_to<bool>;
    };
    template <typename T>
    concept Ordered = OrderedWith<T, T>;
  }
  
  namespace Traits{
    template <typename T>
    struct StaticArray : Meta::TemplateNoDefault{};

    template <typename T>
    struct DynamicArray : Meta::TemplateNoDefault{};

    template <typename T>
    struct View : Meta::TemplateNoDefault{};

    template <typename T, typename U>
    struct Alias : Meta::TemplateNoDefault{};

    template <typename Ptr>
    struct Pointer : Meta::TemplateNoDefault{};

    template <typename E>
    struct Enum : Meta::TemplateNoDefault{};

    template <typename E>
    struct BitFlagEnum : Meta::TemplateNoDefault{};

    template <typename E>
    struct StatusEnum : Meta::TemplateNoDefault{};

    template <typename E>
    struct Status : Meta::TemplateNoDefault{};

    template <typename E>
    struct Error : Meta::TemplateNoDefault{};

    template <typename T>
    struct BitwiseMovable : Meta::TemplateNoDefault{};

    template <typename To, typename From>
    struct BitwiseCastable : Meta::TemplateNoDefault{};

    template <typename Str>
    struct String : Meta::TemplateNoDefault{};

    template <typename A>
    struct Allocator : Meta::TemplateNoDefault{};

    template <typename I>
    struct Iterator : Meta::TemplateNoDefault{};

    template <typename T>
    struct Serializable : Meta::TemplateNoDefault{};

    template <typename T>
    struct Container : Meta::TemplateNoDefault{};

    template <typename T>
    struct Range : Meta::TemplateNoDefault{};

    template <typename T>
    struct Singleton : Meta::TemplateNoDefault{};

    template <typename T>
    struct Class : Meta::TemplateNoDefault{};

    template <typename T>
    struct Numeric : Meta::TemplateNoDefault{};

    template <typename T>
    struct Domain : Meta::TemplateNoDefault{};

    template <typename T>
    struct Graph : Meta::TemplateNoDefault{
      // using node_type = ...;
      // using edge_type = ...;
    };
    
    namespace Detail{

      template <typename T>
      inline constexpr static const T& referenceTo(const T* pVal) noexcept {
        return *pVal;
      }




      template <typename T, size_t N, typename ...Args>
      struct ProbeInitialCase{
        inline constexpr static size_t value = N - 1;
        using type = typename T::template Trait<N - 1, Args...>;
      };
      template <typename T, size_t N, typename ...Args> requires(VK_instantiable_with(T::template Trait, N, Args...))
      struct ProbeInitialCase<T, N, Args...> : ProbeInitialCase<T, N + 1, Args...>{};

      template <typename TraitType, typename ...Args>
      using Probe = typename ProbeInitialCase<TraitType, 1, Args...>::type;

      template <typename Trait, typename ...T>
      concept SatisfiedBy = !std::derived_from<Probe<Trait, T...>, Meta::Fallback>;
      template <typename T, typename Trait>
      concept Satisfies = SatisfiedBy<Trait, T>;


      namespace Array{
        VK_trait(ElementType,
                   alias(element_type),
                     given(typename A),
                       typename(Traits::StaticArray<A>::element_type),
                       typename(Traits::DynamicArray<A>::element_type),
                       typename(std::ranges::range_value_t<A>)
        );

        VK_trait(StaticSize,
                 let(staticSize),
                 given(typename A),
                 if(VK_complete_type(Traits::StaticArray<A>))(Traits::StaticArray<A>::size)
        );
      }

      namespace Pointer{
        VK_trait(ElementType,
                 alias(element_type),
                 given(typename P),
                 typename (Traits::Pointer<P>::element_type),
                 typename (std::pointer_traits<P>::element_type)
        );
        VK_trait(Reference,
                 alias(reference),
                 given(typename P),
                 typename(Traits::Pointer<P>::reference),
                 if(requires(P&& p){ { *std::forward<P>(p) }; }) decltype(*std::declval<P>())
        );
        VK_trait(DifferenceType,
                 alias(difference_type),
                 given(typename P),
                 typename(Traits::Pointer<P>::difference_type),
                 if(requires(P& a, P& b){ { a - b }; }) decltype(std::declval<P>() - std::declval<P>()),
                 typename(std::pointer_traits<P>::difference_type)
        );
      }

      namespace Status{
        VK_trait(DomainType,
                 alias(domain_type),
                   given(typename S),
                     typename( Traits::Status<S>::domain_type ),
                     typename( Traits::Error<S>::domain_type ),
                     if (requires(const S& status){ { status.domain() }; }) std::remove_cvref_t<decltype(std::declval<const S&>().domain())>
        );
        VK_trait(ValueType,
                 alias(value_type),
                 given(typename S),
                 typename(Traits::Status<S>::value_type),
                 typename(Traits::Error<S>::value_type),
                 if (SatisfiedBy<DomainType, S> && requires{ typename Probe<DomainType, S>::domain_type::value_type; }) (typename Probe<DomainType, S>::domain_type::value_type),
                 if (requires(const S& status){ { status.value() }; }) std::remove_cvref_t<decltype(std::declval<const S&>().value())>
        );
      }

      namespace Enum{

        VK_trait(Name,
                 let(name),
                 given(typename E),
                 member_value(name, Traits::BitFlagEnum<E>),
                 member_value(name, Traits::StatusEnum<E>),
                 member_value(name, Traits::Enum<E>)
        );

        VK_trait(ScopedName,
                 let(scoped_name),
                   given(typename E),
                     member_value(scoped_name, Traits::BitFlagEnum<E>),
                     member_value(scoped_name, Traits::StatusEnum<E>),
                     member_value(scoped_name, Traits::Enum<E>),
                     member_value(name, Probe<Name, E>)
        );




        VK_trait(Type,
                 alias(enum_type),
                   given(typename E),
                     member_type(enum_type, Traits::BitFlagEnum<E>),
                     member_type(bit_type, Traits::BitFlagEnum<E>),
                     member_type(enum_type, Traits::StatusEnum<E>),
                     member_type(enum_type, Traits::Enum<E>),
                     if(std::is_enum_v<E>) E
        );

        VK_trait(UnderlyingType,
                 alias(underlying_type),
                   given(typename E),
                     member_type(underlying_type, Traits::BitFlagEnum<E>),
                     member_type(underlying_type, Traits::StatusEnum<E>),
                     member_type(underlying_type, Traits::Enum<E>),
                     member_type(type, std::underlying_type<E>)
        );

        VK_trait(Values,
                 let(values),
                   given(typename E),
                     member_value(values, Traits::Enum<E>)/*,
                     member_value(flags, Traits::BitFlagEnum<E>)*/
        );

        VK_trait(Entries,
                 let(entries),
                   given(typename E),
                     if (VK_instantiable_with(Traits::Enum, E) && requires(E val){ { Traits::Enum<E>::entries[val] }; }) Traits::Enum<E>::entries,
                     if (VK_instantiable_with(Traits::BitFlagEnum, E) && requires(E val){ { Traits::BitFlagEnum<E>::entries[val] }; }) Traits::BitFlagEnum<E>::entries
        );

        VK_trait(IsBitFlag,
                 let(is_bit_flag),
                   given(typename E),
                     otherwise (VK_instantiable_with(Traits::BitFlagEnum, E))
        );

        VK_trait(StatusDomain,
                 alias(domain_type),
                   given(typename E),
                     member_type(domain_type, Traits::StatusEnum<E>),
                     member_type(status_type::domain_type, Traits::StatusEnum<E>),
                     if (VK_instantiable_with(Traits::StatusEnum, E)) Core::StatusEnumDomain<E>
        );

        VK_trait(StatusType,
                 alias(status_type),
                   given(typename E),
                     member_type(status_type, Traits::StatusEnum<E>),
                     if (requires{ typename Probe<StatusDomain, E>::domain_type; typename Probe<StatusDomain, E>::domain_type::status_type; })(typename Probe<StatusDomain, E>::domain_type::status_type),
                     if (Satisfies<E, StatusDomain>) (Core::StatusCode<typename Probe<StatusDomain, E>::domain_type>),
                     if (requires(E e){ { makeStatusCode(e) } -> Satisfies<Status::ValueType>; }) decltype(makeStatusCode(std::declval<E>()))
        );

        VK_trait(StatusInfo,
                 let(status_info),
                   given(typename E),
                     if (requires(E e){
                       { Traits::StatusEnum<E>::info[e].message };
                       { Traits::StatusEnum<E>::info[e].generic };
                       { Traits::StatusEnum<E>::info[e].severity } -> Concepts::SameAs<Core::Severity>;
                     }) referenceTo(&Traits::StatusEnum<E>::info)
        );



        VK_trait(IsStatusEnum,
                 let(is_status_enum),
                   given(typename E),
                     otherwise (SatisfiedBy<StatusType, E> && SatisfiedBy<StatusDomain, E>/* && SatisfiedBy<StatusInfo, E>*/)
        );



        VK_trait(Flags,
                 let(bits),
                   given(typename E),
                     member_value(bits, Traits::BitFlagEnum<E>)
        );

        VK_trait(FlagType,
                 alias(flag_type),
                   given(typename E),
                     member_type(flag_type, Traits::BitFlagEnum<E>),
                     member_type(type::flag_type, Meta::Identity<E>)
        );
      }

      namespace Container{
        VK_trait(Pointer,
                 alias(pointer),
                   given(typename C),
                     typename(Traits::Container<C>::pointer),
                     typename (C::pointer),
                     if (requires(C& c){ { std::ranges::data(c) }; }) decltype(std::ranges::data(std::declval<C&>()))
        );
        VK_trait(IsStatic,
                 let(isStatic),
                   given(typename C),
                     if (VK_instantiable_with(Traits::Container, C) && requires{ { Traits::Container<C>::isStatic } -> std::convertible_to<bool>; }) Traits::Container<C>::isStatic,
                     if (VK_instantiable_with(std::tuple_size, C) && requires{ { std::tuple_size_v<C> } -> std::convertible_to<size_t>; }) true,
                     if (VK_instantiable_with(Traits::StaticArray, C)) true
        );
      }

      namespace Class{
        VK_trait(Name,
                 let(name),
                   given(typename C),
                     if (VK_instantiable_with(Traits::Class, C) && requires{ { Traits::Class<C>::name }; }) Traits::Class<C>::name,
                     if (VK_instantiable_with(Traits::Singleton, C) && requires{ { Traits::Singleton<C>::name }; } ) Traits::Singleton<C>::name
        );
        VK_trait(Uuid,
                 let(uuid),
                   given(typename C),
                     if (VK_instantiable_with(Traits::Singleton, C) && requires{ { Traits::Singleton<C>::uuid } -> SameAs<Core::Uuid>; } ) Traits::Singleton<C>::uuid,
                     if (VK_instantiable_with(Traits::Class, C) && requires{ { Traits::Class<C>::uuid } -> SameAs<Core::Uuid>; }) Traits::Class<C>::uuid,
                     if (requires{ { C::uuid } -> SameAs<Core::Uuid>; }) C::uuid
        );
        VK_trait(Info,
                 let(info),
                   given(typename C),
                     if (VK_instantiable_with(Traits::Class, C) && requires{ { Traits::Class<C>::info }; }) Traits::Class<C>::info,
                     if (VK_instantiable_with(Traits::Singleton, C) && requires{ { Traits::Singleton<C>::info }; } ) Traits::Singleton<C>::info
        );
        VK_trait(Size,
                 let(size),
                   given(typename C),
                     if (VK_instantiable_with(Traits::Class, C) && requires{ { Traits::Class<C>::size } -> std::convertible_to<size_t>; }  && !requires{ { sizeof(C) }; }) Traits::Class<C>::size,
                     otherwise( sizeof(C) )
        );
        VK_trait(Alignment,
                 let(alignment),
                   given(typename C),
                     if (VK_instantiable_with(Traits::Class, C) && requires{ { Traits::Class<C>::alignment } -> std::convertible_to<size_t>; }  && !requires{ { alignof(C) }; }) Traits::Class<C>::alignment,
                     otherwise( alignof(C) )
        );

        /*VK_trait(Instance,
                 let(instance),
                   given(typename C),
                     if (VK_instantiable_with(Traits::Singleton, C) && requires{ { Traits::Singleton<C>::instance } -> std::convertible_to<const C&>; }) Traits::Singleton<C>::instance,
                     if (VK_instantiable_with(Traits::Singleton, C) && requires{ { Traits::Singleton<C>::get() } -> std::convertible_to<const C&>; }) Traits::Singleton<C>::get(),
                     if (requires{ { C::instance } -> std::convertible_to<const C&>; } ) C::instance,
                     if (requires{ { C::get() } -> std::convertible_to<const C&>; } ) C::get(),
                     if (VK_instantiable_with(Traits::Singleton, C)) C{}
        );*/

        VK_trait(IsSingleton,
                 let(isSingleton),
                   given(typename C),
                     if (VK_instantiable_with(Traits::Singleton, C)) true//,
                     //otherwise(requires{ { std::addressof(C::get()) } -> std::convertible_to<const C*>; }) /* && SatisfiedBy<Instance, C>*/
        );
      }

      namespace Domain{
        VK_trait(DomainType,
                 alias(domain_type),
                   given(typename D),
                     member_type(domain_type, Traits::Domain<D>),
                     if (std::convertible_to<const D*, const Core::StatusDomain*>) D,
                     member_type(domain_type, D),
                     otherwise(D)
        );
        VK_trait(StatusType,
                 alias(status_type),
                   given(typename D),
                     member_type(status_type, Traits::Domain<D>),
                     member_type(status_type, D),
                     if (std::convertible_to<const D*, const Core::StatusDomain*>) Core::StatusCode<D>
        );
        VK_trait(ErrorType,
                 alias(error_type),
                   given(typename D),
                     member_type(error_type, Traits::Domain<D>),
                     member_type(error_type, D),
                     if (std::convertible_to<const D*, const Core::StatusDomain*>) Core::ErrorCode<D>
        );
        VK_trait(ValueType,
                 alias(value_type),
                   given(typename D),
                     member_type(value_type, Traits::Domain<D>),
                     member_type(value_type, D)
        );
        VK_trait(IsConstant,
                 let(is_constant),
                   given(typename D),
                     member_value(is_constant, Traits::Domain<D>),
                     if (Satisfies<D, Class::IsSingleton>) true
        );
      }

      namespace Numeric{
        VK_trait(IsIntegral,
                   let(is_integral),
                     given(typename N),
                       member_value(integral, Traits::Numeric<N>),
                       if (std::integral<N>) true
        );
        VK_trait(IsFloatingPoint,
                   let(is_floating_point),
                     given(typename N),
                     member_value(floating_point, Traits::Numeric<N>),
                     if (std::floating_point<N>) true
        );
        VK_trait(IsSigned,
                 let(is_signed),
                   given(typename N),
                     member_value(is_signed, Traits::Numeric<N>),
                     if (std::floating_point<N>) true,
                     otherwise (std::signed_integral<N>)
        );
        VK_trait(IsBounded,
                 let(is_bounded),
                   given(typename N),
                     member_value(is_bounded, Traits::Numeric<N>),
                     if (std::numeric_limits<N>::is_specialized) std::numeric_limits<N>::is_bounded,
                     otherwise (true)
        );
        VK_trait(IsExact,
                 let(is_exact),
                   given(typename N),
                     member_value(is_exact, Traits::Numeric<N>),
                     if (std::numeric_limits<N>::is_exact) std::numeric_limits<N>::is_exact,
                     otherwise (Probe<IsIntegral, N>::is_integral)
        );
        VK_trait(Bits,
                 let(bits),
                   given(typename N),
                     member_value(bits, Traits::Numeric<N>),
                     otherwise (sizeof(N) * CHAR_BIT)
        );
      }

      namespace Agent{
        VK_trait(BaseType,
                 alias(base_type),
                   given(typename A),
                     member_type(base_type, A)
                 );
        VK_trait(MessageType,
                 alias(message_type),
                   given(typename A),
                     member_type(message_type, A)
                 );
        VK_trait(StatusType,
                 alias(status_type),
                   given(typename A),
                     member_type(status_type, A)
                 );
        VK_trait(RootBaseType,
                 alias(root_base_type),
                   given(typename A),
                     if (Satisfies<A, BaseType> && Satisfies<typename Probe<BaseType, A>::base_type, RootBaseType>)(typename Probe<BaseType, A>::base_type),
                     if (Satisfies<A, MessageType> && Satisfies<A, StatusType>) A
                 );
      }

      template <typename P>
      struct PointerRebindBackup{};
      template <typename P> requires(requires{ typename std::pointer_traits<P>::template rebind<void>; })
      struct PointerRebindBackup<P>{
        template <typename U>
        using rebind = typename std::pointer_traits<P>::template rebind<U>;
      };

      template <typename P>
      struct PointerRebind : PointerRebindBackup<P>{};
      template <typename P> requires(requires{ typename Traits::Pointer<P>::template rebind<void>; })
      struct PointerRebind<P>{
        template <typename U>
        using rebind = typename Traits::Pointer<P>::template rebind<U>;
      };


      template <typename P>
      struct PointerToAddressBackup{};
      template <typename P> requires(requires(P& p){ { std::to_address(p) } -> Concepts::RawPointer; })
      struct PointerToAddressBackup<P>{
        inline constexpr static auto* toAddress(P& p) noexcept {
          return std::to_address(p);
        }
      };
      template <typename T>
      struct PointerToAddressBackup<T*>{
        inline constexpr static T* toAddress(T* t) noexcept {
          return t;
        }
      };

      template <typename P>
      struct PointerToAddress : PointerToAddressBackup<P>{};
      template <typename P> requires(requires(P& p){ { Traits::Pointer<P>::toAddress(p) } -> Concepts::RawPointer; })
      struct PointerToAddress<P>{
        inline constexpr static auto* toAddress(P& p) noexcept {
          return Traits::Pointer<P>::toAddress(p);
        }
      };


      template <typename P>
      struct PointerToPointerBackup{};
      template <typename P> requires(requires(typename Probe<Pointer::ElementType, P>::element_type& t){ { std::pointer_traits<P>::to_pointer(t) } -> std::same_as<P>; })
      struct PointerToPointerBackup<P>{
        inline constexpr static P toPointer(typename Probe<Pointer::ElementType, P>::element_type& t) noexcept {
          return std::pointer_traits<P>::to_pointer(t);
        }
      };
      template <typename T>
      struct PointerToPointerBackup<T*>{
        inline constexpr static T* toPointer(T& t) noexcept {
          return std::addressof(t);
        }
      };

      template <typename T>
      struct PointerToPointer : PointerToPointerBackup<T>{};
      template <typename P> requires(requires(typename Probe<Pointer::ElementType, P>::element_type& t){ { Traits::Pointer<P>::toPointer(t) } -> std::same_as<P>; })
      struct PointerToPointer<P>{
        inline constexpr static P toPointer(typename Probe<Pointer::ElementType, P>::element_type& t) noexcept {
          return Traits::Pointer<P>::toPointer(t);
        }
      };


      template <typename T>
      struct PointerCastBackup{};
      template <typename P> requires(requires(const P& p){ { static_cast<typename PointerRebind<P>::template rebind<Meta::copy_cv_t<void, typename Probe<Pointer::ElementType, P>::element_type>>>(p) }; })
      struct PointerCastBackup<P>{
      private:
        template <typename To>
        using cast_result_t = typename PointerRebind<P>::template rebind<Meta::copy_cv_t<To, typename Probe<Pointer::ElementType, P>::element_type>>;
      public:
        template <typename To>
        inline constexpr static cast_result_t<To> cast(const P& ptr) noexcept {
          return static_cast<cast_result_t<To>>(ptr);
        }
      };

      template <typename P>
      struct PointerCast : PointerCastBackup<P>{};
      template <typename P>
      requires(requires(const P& ptr){
        { Traits::Pointer<P>::template cast<void>(ptr) } ->
        std::same_as<typename PointerRebind<P>::template rebind<
            Meta::copy_cv_t<void, typename Probe<Pointer::ElementType, P>::element_type>>>;
      })
      struct PointerCast<P>{
        template <typename To>
        inline constexpr static auto cast(const P& ptr) noexcept {
          return Traits::Pointer<P>::template cast<To>(ptr);
        }
      };

      template <typename T>
      struct ArrayInfo;
      template <typename T> requires( requires(T& value) {
        {Traits::DynamicArray<T>::data(value) };
        { Traits::DynamicArray<T>::size(value) } -> std::convertible_to<size_t>;
      })
      struct ArrayInfo<T> : public Probe<Array::ElementType, T>{
        inline constexpr static bool isDynamic = true;
        inline constexpr static decltype(auto) getData(T& value) noexcept {
          return Traits::DynamicArray<T>::data(value);
        }
        inline constexpr static decltype(auto) getData(const T& value) noexcept {
          return Traits::DynamicArray<T>::data(value);
        }
        inline constexpr static decltype(auto) getSize(const T& value) noexcept {
          return Traits::DynamicArray<T>::size(value);
        }
      };
      template <typename T> requires( requires(T& value) {
        { Traits::StaticArray<T>::data(value) };
        { Traits::StaticArray<T>::size } -> std::convertible_to<size_t>;
      })
      struct ArrayInfo<T> : public Probe<Array::ElementType, T>{
        inline constexpr static bool isDynamic = false;
        inline constexpr static size_t size = Traits::StaticArray<T>::size;
        inline constexpr static decltype(auto) getData(T& value) noexcept {
          return Traits::StaticArray<T>::data(value);
        }
        inline constexpr static decltype(auto) getData(const T& value) noexcept {
          return Traits::StaticArray<T>::data(value);
        }
        inline constexpr static decltype(auto) getSize(const T& value) noexcept {
          return Traits::StaticArray<T>::size;
        }
      };
      template <typename T> requires(requires { sizeof(ArrayInfo<T>); })
      struct ArrayInfo<T&> : ArrayInfo<T>{};
      template <typename T> requires(requires { sizeof(ArrayInfo<T>); })
      struct ArrayInfo<T&&> : ArrayInfo<T>{};
      template <typename T> requires(sizeof(Traits::View<T>) > 0 && requires { sizeof(ArrayInfo<T>); })
      struct ArrayInfo<const T> : ArrayInfo<T>{ };


      template <typename E>
      struct EnumInfo
          : Probe<Enum::Type, E>,
            Probe<Enum::UnderlyingType, E>,
            Probe<Enum::Values, E>,
            Probe<Enum::FlagType,  E>,
            Probe<Enum::IsStatusEnum, E>,
            Probe<Enum::StatusType, E>,
            Probe<Enum::StatusDomain, E>,
            Probe<Enum::StatusInfo, E>,
            Probe<Enum::ScopedName, E>,
            Probe<Enum::Name, E>{
        //inline constexpr static bool isScoped = std::convertible_to<typename Probe<Enum::Type, E>::enum_type, typename Probe<Enum::UnderlyingType, E>::underlying_type>;
        //inline constexpr static bool isBitFlag = VK_instantiable_with(Traits::BitFlagEnum, E);
      };



      template <typename T>
      struct PointerInfo :
          Probe<Pointer::ElementType, T>,
          //PointerElementType<T>,
          PointerRebind<T>,
          Probe<Pointer::Reference, T>,
          //PointerReference<T>,
          Probe<Pointer::DifferenceType, T>,
          //PointerDifferenceType<T>,
          PointerToPointer<T>,
          PointerToAddress<T>,
          PointerCast<T>{};


      template <typename S>
      struct StringInfo;

      template <typename I>
      struct IteratorInfo;

      template <typename A>
      struct AllocatorInfo;

      /*template <typename E>
      struct StatusInfo;*/

      template <typename E>
      struct ContainerInfo;

      template <typename D>
      struct DomainInfo
          : Probe<Domain::DomainType, D>,
            Probe<Domain::StatusType, D>,
            Probe<Domain::ErrorType, D>,
            Probe<Domain::ValueType, D>,
            Probe<Domain::IsConstant, D>{};

      template <typename C>
      struct ClassInfo :
          Probe<Class::Name, C>,
          Probe<Class::Uuid, C>,
          //Probe<Class::Info, C>,
          Probe<Class::Size, C>,
          Probe<Class::Alignment, C>,
          Probe<Class::IsSingleton, C>/*,
          Probe<Class::Instance, C>*/{};


      template <typename N>
      struct NumericInfo
          : Probe<Numeric::IsIntegral, N>,
            Probe<Numeric::IsFloatingPoint, N>,
            Probe<Numeric::IsSigned, N>,
            Probe<Numeric::IsBounded, N>,
            Probe<Numeric::IsExact, N>,
            Probe<Numeric::Bits, N>{};

      template <typename A>
      struct AgentInfo
          : Probe<Agent::BaseType, A>,
            Probe<Agent::RootBaseType, A>,
            Probe<Agent::MessageType, A>,
            Probe<Agent::StatusType, A>{};
    }
  }
  
  inline namespace Concepts{
    template <typename P, typename T = const void>
    concept Pointer = RawPointer<P> || requires(P&& p){
      typename Traits::Detail::PointerInfo<P>::element_type;
      typename Traits::Detail::PointerInfo<P>::difference_type;
      typename Traits::Detail::PointerInfo<P>::template rebind<void>;
    } && (std::same_as<T, const void> ||
          requires(P& p){
            { *p } -> std::same_as<typename Traits::Detail::PointerInfo<P>::reference>;
            { Traits::Detail::PointerInfo<P>::template cast<void>(p) };
            { p.operator->() };
          });

    template <typename P, typename T = const void>
    concept PointerFwdRef = Pointer<std::remove_reference_t<P>, T>;

    template <typename T, typename U>
    concept AliasedBy = requires{
      typename Traits::Alias<U, T>;
    } && requires(T* pTarget, U* pAliased){
      { Traits::Alias<U, T>::reify(pAliased) } -> std::same_as<T*>;
      { Traits::Alias<U, T>::alias(pTarget) }  -> std::same_as<U*>;
    } && ((sizeof(T) % sizeof(U)) == 0);

    template <typename P, typename T>
    concept ArrayPointer = Pointer<P> && AliasedBy<typename Traits::Detail::PointerInfo<P>::element_type, T>;



    template <typename T>
    concept BitwiseMovable = requires{
      sizeof(Traits::BitwiseMovable<T>);
    };

    template <typename T>
    concept IsRegisterCopyable = std::conjunction_v<std::is_trivially_copyable<T>, std::is_trivially_destructible<T>> && sizeof(T) <= 16;


    template <typename A, typename T = std::byte>
    concept ContiguousRange = requires(A&& a){
      typename Traits::Detail::ArrayInfo<A>::element_type;
      { Traits::Detail::ArrayInfo<A>::getData(std::forward<A>(a)) };
      { Traits::Detail::ArrayInfo<A>::getSize(std::forward<A>(a)) } -> std::convertible_to<size_t>;
    } && AliasedBy<typename Traits::Detail::ArrayInfo<A>::element_type, T>;
    template <typename A, typename T = std::byte>
    concept DynamicMemorySpan = ContiguousRange<A, T> && Traits::Detail::ArrayInfo<A>::isDynamic;
    template <typename A, typename T = std::byte>
    concept StaticMemorySpan = ContiguousRange<A, T> && !Traits::Detail::ArrayInfo<A>::isDynamic
                               && requires{
      { Traits::Detail::ArrayInfo<A>::size } -> std::convertible_to<size_t>;
    };

    /*template <typename To, typename From>
    concept BitCastable = sizeof(From) == sizeof(To) && BitwiseMovable<To> && BitwiseMovable<From>;*/
    template <typename From, typename To>
    concept BitCastableTo = sizeof(From) == sizeof(To) && BitwiseMovable<To> && BitwiseMovable<From>;


    template <typename T>
    concept Status = /*VK_instantiable_with(Traits::Status, T)*/true;

    template <typename T>
    concept Error = /*VK_instantiable_with(Traits::Error, T)*/true;

    template <typename T>
    concept StatusEnum = VK_instantiable_with(Traits::StatusEnum, T) &&
                         requires(const T& t){
                           typename Traits::Detail::EnumInfo<T>::status_type;
                           typename Traits::Detail::EnumInfo<T>::domain_type;
                         } && Status<typename Traits::Detail::EnumInfo<T>::status_type>;

    template <typename C, typename T = void>
    concept Container = requires(C& lv, const C& clv){
      typename Traits::Detail::ContainerInfo<C>;
      typename Traits::Detail::ContainerInfo<C>::value_type;
      typename Traits::Detail::ContainerInfo<C>::reference;
      typename Traits::Detail::ContainerInfo<C>::const_reference;
      typename Traits::Detail::ContainerInfo<C>::pointer;
      typename Traits::Detail::ContainerInfo<C>::const_pointer;
      typename Traits::Detail::ContainerInfo<C>::iterator;
      typename Traits::Detail::ContainerInfo<C>::const_iterator;
      { Traits::Detail::ContainerInfo<C>::isDynamic } -> SameAs<bool>;
    };

    template <typename R, typename T = void>
    concept Range     = VK_instantiable_with(Traits::DynamicArray, R) || VK_instantiable_with(Traits::StaticArray, R);

    template <typename S>
    concept String    = VK_instantiable_with(Traits::String, S);

    template <typename I, typename T = void>
    concept Iterator  = VK_instantiable_with(Traits::Iterator, I) && (ExactSameAs<T, void> || (requires{ typename Traits::Detail::IteratorInfo<I>::reference_type; } && std::convertible_to<typename Traits::Detail::IteratorInfo<I>::reference_type, std::add_lvalue_reference_t<T>>));

    template <typename A, typename T = void>
    concept Allocator = requires(A& alloc, size_t n){
      { alloc.allocate(n) } -> Pointer<T>;
    } && requires(A& alloc, size_t n, decltype(alloc.allocate(n)) p){
      { alloc.deallocate(p) };
    };

    template <typename T>
    concept BitFlagEnum = VK_instantiable_with(Traits::BitFlagEnum, T) && Enum<T>;

    template <typename T>
    concept Class = std::is_class_v<T>;

    template <typename T>
    concept Integral = requires{
      { Traits::Detail::NumericInfo<T>::is_integral };
    } && Traits::Detail::NumericInfo<T>::is_integral;
    template <typename T>
    concept FloatingPoint = requires{
      { Traits::Detail::NumericInfo<T>::is_floating_point };
    } && Traits::Detail::NumericInfo<T>::is_floating_point;
    template <typename T>
    concept Numeric = Integral<T> || FloatingPoint<T> || VK_instantiable_with(Traits::Numeric, T);

    template <typename T>
    concept Domain = true;/*requires{
      //typename Traits::Detail::DomainInfo<T>::domain_type;
      typename Traits::Detail::DomainInfo<T>::status_type;
      //typename Traits::Detail::DomainInfo<T>::error_type;
      typename Traits::Detail::DomainInfo<T>::value_type;
      { Traits::Detail::DomainInfo<T>::is_constant } -> std::convertible_to<bool>;
    };*/

    template <typename T>
    concept AgentLike = requires{
      typename Traits::Detail::AgentInfo<T>::base_type;
      typename Traits::Detail::AgentInfo<T>::root_base_type;
      typename Traits::Detail::AgentInfo<T>::message_type;
      typename Traits::Detail::AgentInfo<T>::status_type;
    };
    template <typename T, typename ...Args>
    concept ConstructibleFrom = std::constructible_from<T, Args...> ||
                               requires(Args&& ...args){
                                 { T{ std::forward<Args>(args)... } };
                               };
  }
  
  template <Container T>
  using container_traits = Traits::Detail::ContainerInfo<T>;
  template <Range T>
  using array_traits     = Traits::Detail::ArrayInfo<T>;
  template <Pointer T>
  using pointer_traits   = Traits::Detail::PointerInfo<T>;
  template <Enum T>
  using enum_traits      = Traits::Detail::EnumInfo<T>;
  template <String T>
  using string_traits    = Traits::Detail::StringInfo<T>;
  template <Iterator T>
  using iterator_traits  = Traits::Detail::IteratorInfo<T>;
  template <Allocator T>
  using allocator_traits = Traits::Detail::AllocatorInfo<T>;
  /*template <Status T>
  using status_traits    = Traits::Detail::StatusInfo<T>;*/
  template <Class C>
  using class_traits     = Traits::Detail::ClassInfo<C>;
  template <Numeric N>
  using numeric_traits   = Traits::Detail::NumericInfo<N>;
  template <Domain D>
  using domain_traits    = Traits::Detail::DomainInfo<D>;
  template <AgentLike A>
  using agent_traits     = Traits::Detail::AgentInfo<A>;


  template <typename T>
  using param_t = std::conditional_t<Concepts::IsRegisterCopyable<T>, T, const T&>;
  template <typename T>
  using in_register_t = std::conditional_t<Concepts::IsRegisterCopyable<T>, T, const T&>;


  template <Concepts::ContiguousRange Rng>
  inline constexpr static auto* rangeData(Rng&& rng) noexcept {
    return array_traits<Rng>::getData(std::forward<Rng>(rng));
  }
  template <Concepts::ContiguousRange Rng>
  inline constexpr static size_t rangeSize(Rng&& rng) noexcept {
    return array_traits<Rng>::getSize(std::forward<Rng>(rng));
  }
}

template <typename T>
struct valkyrie::Traits::Pointer<T*>{
  using element_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  template <typename U>
  using rebind = Meta::copy_cv_t<U, T>*;
};

template <valkyrie::BitwiseMovable To, valkyrie::BitwiseMovable From> requires(sizeof(To) == sizeof(From))
struct valkyrie::Traits::BitwiseCastable<To, From>{};


template <typename T> requires(std::is_trivially_copyable_v<T> && std::is_trivially_destructible_v<T>)
struct valkyrie::Traits::BitwiseMovable<T>{};



/*namespace valkyrie::Core{

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
  
  struct Disable{ using invalid_specialization = void; };
  
  namespace Meta{

    template <typename T>
    struct Identity{
      using type = T;
    };

    struct Fallback{};

    template <size_t N>
    struct Overload : Overload<N - 1>{};
    template <>
    struct Overload<0> : Fallback{};
    
    struct TemplateWrapper{ using IsATemplate = void; };
    
    template <decltype(auto) Val>
    struct ValueWrapper{
      using IsAValue = void;
      inline constexpr static decltype(auto) value = Val;
    };
    
    template <typename T>
    concept WrappedValue = requires{
      typename T::IsAValue;
    } && std::same_as<typename T::IsAValue, void>;
    
    template <WrappedValue T>
    inline constexpr static decltype(auto) unwrapValue = T::value;
    
    template <template <typename...> typename TT>
    struct TemplateT : TemplateWrapper{
      template <typename ...Args> requires(requires{ typename TT<Args...>; })
      using Instantiate = TT<Args...>;
    };
    template <template <decltype(auto)...> typename TT>
    struct TemplateV : TemplateWrapper{
      template <WrappedValue ...Args> requires(requires{ typename TT<unwrapValue<Args>...>; })
      using Instantiate = TT<unwrapValue<Args>...>;
    };
    template <template <typename, decltype(auto)...> typename TT>
    struct TemplateTV : TemplateWrapper{
      template <typename T, WrappedValue ...Args> requires(requires{ typename TT<T, unwrapValue<Args>...>; })
      using Instantiate = TT<T, unwrapValue<Args>...>;
    };
    template <template <decltype(auto), typename...> typename TT>
    struct TemplateVT : TemplateWrapper{
      template <WrappedValue V, typename...Args> requires(requires{ typename TT<unwrapValue<V>, Args...>; })
      using Instantiate = TT<unwrapValue<V>, Args...>;
    };
    template <template <typename, typename, decltype(auto)...> typename TT>
    struct TemplateTTV : TemplateWrapper{
      template <typename T, typename U, WrappedValue ...Args> requires(requires{ typename TT<T, U, unwrapValue<Args>...>; })
      using Instantiate = TT<T, U, unwrapValue<Args>...>;
    };
    template <template <decltype(auto), decltype(auto), typename...> typename TT>
    struct TemplateVVT : TemplateWrapper{
      template <WrappedValue V, WrappedValue U, typename...Args> requires(requires{ typename TT<unwrapValue<V>, unwrapValue<U>, Args...>; })
      using Instantiate = TT<unwrapValue<V>, unwrapValue<U>, Args...>;
    };
    template <template <decltype(auto), typename, decltype(auto)...> typename TT>
    struct TemplateVTV : TemplateWrapper{
      template <WrappedValue T, typename U, WrappedValue ...Args> requires(requires{ typename TT<unwrapValue<T>, U, unwrapValue<Args>...>; })
      using Instantiate = TT<unwrapValue<T>, U, unwrapValue<Args>...>;
    };
    template <template <typename, decltype(auto), typename...> typename TT>
    struct TemplateTVT : TemplateWrapper{
      template <typename T, WrappedValue U, typename...Args> requires(requires{ typename TT<T, unwrapValue<U>, Args...>; })
      using Instantiate = TT<T, unwrapValue<U>, Args...>;
    };
    
    template <template <typename...> typename TT>
    TemplateT<TT> wrap_(Overload<2>) noexcept;
    template <template <decltype(auto)...> typename TT>
    TemplateV<TT> wrap_(Overload<2>) noexcept;
    template <template <typename, decltype(auto)...> typename TT>
    TemplateTV<TT> wrap_(Overload<1>) noexcept;
    template <template <decltype(auto), typename, typename...> typename TT>
    TemplateVT<TT> wrap_(Overload<1>) noexcept;
    template <template <typename, typename, decltype(auto)...> typename TT>
    TemplateTTV<TT> wrap_(Overload<0>) noexcept;
    template <template <decltype(auto), decltype(auto), typename...> typename TT>
    TemplateVVT<TT> wrap_(Overload<0>) noexcept;
    template <template <decltype(auto), typename, decltype(auto)...> typename TT>
    TemplateVTV<TT> wrap_(Overload<0>) noexcept;
    template <template <typename, decltype(auto), typename...> typename TT>
    TemplateTVT<TT> wrap_(Overload<0>) noexcept;
    
    template <typename T>
    T wrap_(Overload<2>) noexcept;
    template <decltype(auto) Val>
    ValueWrapper<Val> wrap_(Overload<2>) noexcept;

    template <typename Tmp, typename ...T>
    struct InstantiateTemplate{};
    template <typename Tmp, typename ...T> requires(requires{ typename Tmp::template Instantiate<T...>; })
    struct InstantiateTemplate<Tmp, T...>{
      using type = typename Tmp::template Instantiate<T...>;
    };

    template <typename Tmp, typename ...T>
    struct InstantiateRedirected : InstantiateTemplate<Tmp, T...>{
      template <typename U>
      using push_front = InstantiateRedirected<Tmp, U, T...>;
    };

    template <typename Tmp, typename ...T>
    struct InstantiateWithoutLast;
    template <typename Tmp, typename Head, typename ...Tail>
    struct InstantiateWithoutLast<Tmp, Head, Tail...>{
      using type = typename InstantiateWithoutLast<Tmp, Tail...>::type::template push_front<Head>;
    };
    template <typename Tmp, typename Tail>
    struct InstantiateWithoutLast<Tmp, Tail>{
      using type = InstantiateRedirected<Tmp>;
    };
    template <typename Tmp>
    struct InstantiateWithoutLast<Tmp>{
      using type = InstantiateRedirected<Tmp>;
    };

    template <typename Tmp, typename ...T>
    concept IsSpecializedWithoutLast = requires{
      typename InstantiateWithoutLast<Tmp, T...>::type;
      typename InstantiateWithoutLast<Tmp, T...>::type::type;
      sizeof(typename InstantiateWithoutLast<Tmp, T...>::type::type);
    } && !requires{
      typename InstantiateWithoutLast<Tmp, T...>::type::type::invalid_specialization;
    };
  }

#define VK_wrap(...) decltype(::valkyrie::Core::Meta::wrap_<__VA_ARGS__>(::valkyrie::Core::Meta::Overload<2>{}))
#define PP_VK_impl_WRAP_ALL_redirect(...) VK_wrap(__VA_ARGS__) ,
#define PP_VK_impl_WRAP_ALL PP_VK_impl_WRAP_ALL_redirect
  
  namespace Concepts{

    template <typename T, typename U>
    concept SameAs = std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;
    template <typename T, typename U>
    concept ExactSameAs = std::same_as<T, U> && SameAs<T, U>;



    template <typename T, typename U>
    concept NotExactSameAs = !ExactSameAs<T, U>;
    template <typename T, typename U>
    concept NotSameAs = !SameAs<T, U> && NotExactSameAs<T, U>;

    template <typename T, typename ...U>
    concept SameAsOneOf = (SameAs<T, U> || ...);
    template <typename T, typename ...U>
    concept ExactSameAsOneOf = (ExactSameAs<T, U> || ...) && SameAsOneOf<T, U...>;


    template <typename T, typename ...U>
    concept NotExactSameAsOneOf = (NotExactSameAs<T, U> && ...) && !ExactSameAsOneOf<T, U...>;
    template <typename T, typename ...U>
    concept NotSameAsOneOf = (NotSameAs<T, U> && ...) && !SameAsOneOf<T, U...> && NotExactSameAsOneOf<T, U...>;



    template <typename P, typename T = const void>
    concept RawPointer = std::is_pointer_v<P> && std::convertible_to<P, T*>;
    template <typename T>
    concept Enum = std::is_enum_v<T>;
    template <typename TT, typename ...T>
    concept Specialized = requires{
      typename TT::template Instantiate<T...>;
      sizeof(typename TT::template Instantiate<T...>);
    } && !requires{
      typename TT::template Instantiate<T...>::invalid_specialization;
    };
    template <typename T, typename U>
    concept EqualityComparableWith = requires(const T& t, const U& u){
      { t == u } -> std::same_as<bool>;
      { u == t } -> std::same_as<bool>;
      { t != u } -> std::same_as<bool>;
      { u != t } -> std::same_as<bool>;
    };
    template <typename T>
    concept EqualityComparable = EqualityComparableWith<T, T>;
    template <typename T, typename U>
    concept OrderedWith = EqualityComparableWith<T, U> && requires(const T& t, const U& u) {
      { t < u } -> std::same_as<bool>;
      { t > u } -> std::same_as<bool>;
      { t <= u } -> std::same_as<bool>;
      { t <= u } -> std::same_as<bool>;
      { u < t } -> std::same_as<bool>;
      { u > t } -> std::same_as<bool>;
      { u <= t } -> std::same_as<bool>;
      { u <= t } -> std::same_as<bool>;
    };
    template <typename T>
    concept Ordered = OrderedWith<T, T>;
  }

#define PP_VK_impl_WRAP_IGNORE_INDEX(i, x) VK_wrap(x)
#define InstantiableWith(...) Meta::IsSpecializedWithoutLast<VK_for_each(PP_VK_impl_WRAP_ALL, ##__VA_ARGS__) void>
#define InstantiableWith_(...) Concepts::Specialized<VK_invoke_foreach_delimit(PP_VK_impl_WRAP_IGNORE_INDEX, VK_comma_delimiter, ##__VA_ARGS__)>

}*/

/*#define VALKYRIE_TRAITS_INCLUDED_FOR_CONCEPTS

#include <valkyrie/Core/Concepts.hpp> // Part 2*/





#endif//VALKYRIE_TRAITS_HPP
