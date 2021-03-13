//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_TRAITS_HPP
#define VALKYRIE_TRAITS_HPP

#include "meta.hpp"
#include "utility/uuid.hpp"
#include "status/severity.hpp"

#include <bit>
#include <ranges>

#define VK_trait(name_, decl_, given_, ...) struct name_ {                              \
    template <size_t, PP_VK_impl_TRAIT_PARAMS_##given_>                                   \
    struct Trait;                                                                         \
    template < PP_VK_impl_TRAIT_PARAMS_##given_ >                                         \
    struct Trait<0, PP_VK_impl_TRAIT_ARGS_##given_ > : meta::fallback{};                   \
    PP_VK_impl_TRAIT_BODY_MACRO(PP_VK_impl_TRAIT_BODY_MACRO_##decl_, PP_VK_impl_TRAIT_BIND_##given_, ##__VA_ARGS__)\
    };
#define VK_valid_typename(...) requires{ typename __VA_ARGS__; }
#define VK_complete_type(...) requires{ typename __VA_ARGS__; { sizeof(typename __VA_ARGS__) }; }




namespace valkyrie{

  template <typename>
  class status_code;
  template <typename Domain>
  class error_code;
  class status_domain;
  class generic_domain;
  template <typename E>
  class status_enum_domain;

  inline namespace concepts{
    template <typename T, typename U>
    concept same_as = meta::is_same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>::value;
    template <typename T, typename U>
    concept exact_same_as = meta::is_same_as<T, U>::value && meta::is_same_as<T, U>::value;



    template <typename T, typename U>
    concept not_exact_same_as = !exact_same_as<T, U>;
    template <typename T, typename U>
    concept not_same_as = !same_as<T, U> && not_exact_same_as<T, U>;

    template <typename T, typename ...U>
    concept same_as_one_of = (same_as<T, U> || ...);
    template <typename T, typename ...U>
    concept exact_same_as_one_of = (exact_same_as<T, U> || ...) && same_as_one_of<T, U...>;


    template <typename T, typename ...U>
    concept not_exact_same_as_one_of = (not_exact_same_as<T, U> && ...) && !exact_same_as_one_of<T, U...>;
    template <typename T, typename ...U>
    concept not_same_as_one_of = (not_same_as<T, U> && ...) && !same_as_one_of<T, U...> && not_exact_same_as_one_of<T, U...>;



    template <typename P, typename T = const void>
    concept raw_pointer = std::is_pointer_v<P> && std::convertible_to<P, T*>;
    template <typename T>
    concept enumerator = std::is_enum_v<T>;
    template <typename TT, typename ...T>
    concept specialized = requires{
      typename TT::template instantiate<T...>;
      sizeof(typename TT::template instantiate<T...>);
    } && !requires{
      typename TT::template instantiate<T...>::invalid_specialization;
    };

    template <typename T, typename U>
    concept equality_comparable_with = requires(const T& t, const U& u){
      { t == u } -> exact_same_as<bool>;
      { u == t } -> exact_same_as<bool>;
      { t != u } -> exact_same_as<bool>;
      { u != t } -> exact_same_as<bool>;
    };
    template <typename T>
    concept equality_comparable = equality_comparable_with<T, T>;
    template <typename T, typename U>
    concept ordered_with = equality_comparable_with<T, U> && requires(const T& t, const U& u) {
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
    concept ordered = ordered_with<T, T>;

    template <typename O, typename T, typename U = T>
    concept weak_order = requires(O&& order, const T& t, const U& u){
      { std::forward<O>(order)(t, u) } -> std::convertible_to<std::weak_ordering>;
    };
  }
  
  namespace traits{
    template <typename T>
    struct static_array : meta::template_no_default{};

    template <typename T>
    struct DynamicArray : meta::template_no_default{};

    template <typename T>
    struct View : meta::template_no_default{};

    template <typename T, typename U>
    struct Alias : meta::template_no_default{};

    template <typename Ptr>
    struct Pointer : meta::template_no_default{};

    template <typename E>
    struct Enum : meta::template_no_default{};

    template <typename E>
    struct BitFlagEnum : meta::template_no_default{};

    template <typename E>
    struct StatusEnum : meta::template_no_default{};

    template <typename E>
    struct Status : meta::template_no_default{};

    template <typename E>
    struct Error : meta::template_no_default{};

    template <typename T>
    struct BitwiseMovable : meta::template_no_default{};

    template <typename Str>
    struct String : meta::template_no_default{};

    template <typename A>
    struct Allocator : meta::template_no_default{};

    template <typename I>
    struct Iterator : meta::template_no_default{};

    template <typename T>
    struct Serializable : meta::template_no_default{};

    template <typename T>
    struct Container : meta::template_no_default{};

    template <typename T>
    struct Range : meta::template_no_default{};

    template <typename T>
    struct Singleton : meta::template_no_default{};

    template <typename T>
    struct Class : meta::template_no_default{};

    template <typename T>
    struct Numeric : meta::template_no_default{};

    template <typename T>
    struct Domain : meta::template_no_default{};

    template <typename T>
    struct Graph : meta::template_no_default{
      // using node_type = ...;
      // using edge_type = ...;
    };
    
    namespace detail{

      template <typename T>
      inline constexpr static const T& referenceTo(const T* pVal) noexcept {
        return *pVal;
      }




      template <typename T, size_t N, typename ...Args>
      struct probe_initial_case{
        inline constexpr static size_t value = N - 1;
        using type = typename T::template Trait<N - 1, Args...>;
      };
      template <typename T, size_t N, typename ...Args> requires(VK_instantiable_with(T::template Trait, N, Args...))
      struct probe_initial_case<T, N, Args...> : probe_initial_case<T, N + 1, Args...>{};

      template <typename TraitType, typename ...Args>
      using probe = typename probe_initial_case<TraitType, 1, Args...>::type;

      template <typename Trait, typename ...T>
      concept satisfied_by = !std::derived_from<probe<Trait, T...>, meta::fallback>;
      template <typename T, typename Trait>
      concept satisfies = satisfied_by<Trait, T>;


      namespace Array{
        VK_trait(ElementType,
                   alias(element_type),
                     given(typename A),
                       typename(traits::static_array<A>::element_type),
                       typename(traits::DynamicArray<A>::element_type),
                       typename(std::ranges::range_value_t<A>)
        );

        VK_trait(StaticSize,
                 let(staticSize),
                 given(typename A),
                 if(VK_complete_type(traits::static_array<A>))(traits::static_array<A>::size)
        );
      }

      namespace Pointer{
        VK_trait(ElementType,
                 alias(element_type),
                 given(typename P),
                 typename (traits::Pointer<P>::element_type),
                 typename (std::pointer_traits<P>::element_type)
        );
        VK_trait(Reference,
                 alias(reference),
                 given(typename P),
                 typename(traits::Pointer<P>::reference),
                 if(requires(P&& p){ { *std::forward<P>(p) }; }) decltype(*std::declval<P>())
        );
        VK_trait(DifferenceType,
                 alias(difference_type),
                 given(typename P),
                 typename(traits::Pointer<P>::difference_type),
                 if(requires(P& a, P& b){ { a - b }; }) decltype(std::declval<P>() - std::declval<P>()),
                 typename(std::pointer_traits<P>::difference_type)
        );
      }

      namespace Status{
        VK_trait(DomainType,
                 alias(domain_type),
                   given(typename S),
                     typename( traits::Status<S>::domain_type ),
                     typename( traits::Error<S>::domain_type ),
                     if (requires(const S& status){ { status.domain() }; }) std::remove_cvref_t<decltype(std::declval<const S&>().domain())>
        );
        VK_trait(ValueType,
                 alias(value_type),
                 given(typename S),
                 typename(traits::Status<S>::value_type),
                 typename(traits::Error<S>::value_type),
                 if (satisfied_by<DomainType, S> && requires{ typename probe<DomainType, S>::domain_type::value_type; }) (typename probe<DomainType, S>::domain_type::value_type),
                 if (requires(const S& status){ { status.value() }; }) std::remove_cvref_t<decltype(std::declval<const S&>().value())>
        );
      }

      namespace Enum{

        VK_trait(Name,
                 let(name),
                 given(typename E),
                 member_value(name, traits::BitFlagEnum<E>),
                 member_value(name, traits::StatusEnum<E>),
                 member_value(name, traits::Enum<E>)
        );

        VK_trait(ScopedName,
                 let(scoped_name),
                   given(typename E),
                     member_value(scoped_name, traits::BitFlagEnum<E>),
                     member_value(scoped_name, traits::StatusEnum<E>),
                     member_value(scoped_name, traits::Enum<E>),
                     member_value(name, probe<Name, E>)
        );




        VK_trait(Type,
                 alias(enum_type),
                   given(typename E),
                     member_type(enum_type, traits::BitFlagEnum<E>),
                     member_type(bit_type, traits::BitFlagEnum<E>),
                     member_type(enum_type, traits::StatusEnum<E>),
                     member_type(enum_type, traits::Enum<E>),
                     if(std::is_enum_v<E>) E
        );

        VK_trait(UnderlyingType,
                 alias(underlying_type),
                   given(typename E),
                     member_type(underlying_type, traits::BitFlagEnum<E>),
                     member_type(underlying_type, traits::StatusEnum<E>),
                     member_type(underlying_type, traits::Enum<E>),
                     member_type(type, std::underlying_type<E>)
        );

        VK_trait(Values,
                 let(values),
                   given(typename E),
                     member_value(values, traits::Enum<E>)/*,
                     member_value(flags, traits::BitFlagEnum<E>)*/
        );

        VK_trait(Entries,
                 let(entries),
                   given(typename E),
                     if (VK_instantiable_with(traits::Enum, E) && requires(E val){ { traits::Enum<E>::entries[val] }; }) traits::Enum<E>::entries,
                     if (VK_instantiable_with(traits::BitFlagEnum, E) && requires(E val){ { traits::BitFlagEnum<E>::entries[val] }; }) traits::BitFlagEnum<E>::entries
        );

        VK_trait(IsBitFlag,
                 let(is_bit_flag),
                   given(typename E),
                     otherwise (VK_instantiable_with(traits::BitFlagEnum, E))
        );

        VK_trait(status_domain,
                 alias(domain_type),
                   given(typename E),
                     member_type(domain_type, traits::StatusEnum<E>),
                     member_type(status_type::domain_type, traits::StatusEnum<E>),
                     if (VK_instantiable_with(traits::StatusEnum, E)) status_enum_domain<E>
        );

        VK_trait(StatusType,
                 alias(status_type),
                   given(typename E),
                     member_type(status_type, traits::StatusEnum<E>),
                     if (requires{ typename probe<status_domain, E>::domain_type; typename probe<status_domain, E>::domain_type::status_type; })(typename probe<status_domain, E>::domain_type::status_type),
                     if (satisfies<E, status_domain>) (status_code<typename probe<status_domain, E>::domain_type>),
                     if (requires(E e){ { makeStatusCode(e) } -> satisfies<Status::ValueType>; }) decltype(makeStatusCode(std::declval<E>()))
        );

        VK_trait(StatusInfo,
                 let(status_info),
                   given(typename E),
                     if (requires(E e){
                       { traits::StatusEnum<E>::info[e].message };
                       { traits::StatusEnum<E>::info[e].generic };
                       { traits::StatusEnum<E>::info[e].severity } -> concepts::same_as<severity>;
                     }) referenceTo(&traits::StatusEnum<E>::info)
        );



        VK_trait(IsStatusEnum,
                 let(is_status_enum),
                   given(typename E),
                     otherwise (satisfied_by<StatusType, E> && satisfied_by<status_domain, E>/* && satisfied_by<StatusInfo, E>*/)
        );



        VK_trait(Flags,
                 let(bits),
                   given(typename E),
                     member_value(bits, traits::BitFlagEnum<E>)
        );

        VK_trait(FlagType,
                 alias(flag_type),
                   given(typename E),
                     member_type(flag_type, traits::BitFlagEnum<E>),
                     member_type(type::flag_type, meta::identity<E>)
        );
      }

      namespace Container{
        VK_trait(Pointer,
                 alias(pointer),
                   given(typename C),
                     typename(traits::Container<C>::pointer),
                     typename (C::pointer),
                     if (requires(C& c){ { std::ranges::data(c) }; }) decltype(std::ranges::data(std::declval<C&>()))
        );
        VK_trait(IsStatic,
                 let(isStatic),
                   given(typename C),
                     if (VK_instantiable_with(traits::Container, C) && requires{ { traits::Container<C>::isStatic } -> std::convertible_to<bool>; }) traits::Container<C>::isStatic,
                     if (VK_instantiable_with(std::tuple_size, C) && requires{ { std::tuple_size_v<C> } -> std::convertible_to<size_t>; }) true,
                     if (VK_instantiable_with(traits::static_array, C)) true
        );
      }

      namespace Class{
        VK_trait(Name,
                 let(name),
                   given(typename C),
                     if (VK_instantiable_with(traits::Class, C) && requires{ { traits::Class<C>::name }; }) traits::Class<C>::name,
                     if (VK_instantiable_with(traits::Singleton, C) && requires{ { traits::Singleton<C>::name }; } ) traits::Singleton<C>::name
        );
        VK_trait(Uuid,
                 let(id),
                   given(typename C),
                     if (VK_instantiable_with(traits::Singleton, C) && requires{ { traits::Singleton<C>::class_id } -> same_as<uuid>; } ) traits::Singleton<C>::class_id,
                     if (VK_instantiable_with(traits::Class, C) && requires{ { traits::Class<C>::class_id } -> same_as<uuid>; }) traits::Class<C>::class_id,
                     if (requires{ { C::class_id } -> same_as<uuid>; }) C::class_id
        );
        VK_trait(Info,
                 let(info),
                   given(typename C),
                     if (VK_instantiable_with(traits::Class, C) && requires{ { traits::Class<C>::info }; }) traits::Class<C>::info,
                     if (VK_instantiable_with(traits::Singleton, C) && requires{ { traits::Singleton<C>::info }; } ) traits::Singleton<C>::info
        );
        VK_trait(Size,
                 let(size),
                   given(typename C),
                     if (VK_instantiable_with(traits::Class, C) && requires{ { traits::Class<C>::size } -> std::convertible_to<size_t>; }  && !requires{ { sizeof(C) }; }) traits::Class<C>::size,
                     otherwise( sizeof(C) )
        );
        VK_trait(Alignment,
                 let(alignment),
                   given(typename C),
                     if (VK_instantiable_with(traits::Class, C) && requires{ { traits::Class<C>::alignment } -> std::convertible_to<size_t>; }  && !requires{ { alignof(C) }; }) traits::Class<C>::alignment,
                     otherwise( alignof(C) )
        );

        /*VK_trait(Instance,
                 let(instance),
                   given(typename C),
                     if (VK_instantiable_with(traits::Singleton, C) && requires{ { traits::Singleton<C>::instance } -> std::convertible_to<const C&>; }) traits::Singleton<C>::instance,
                     if (VK_instantiable_with(traits::Singleton, C) && requires{ { traits::Singleton<C>::get() } -> std::convertible_to<const C&>; }) traits::Singleton<C>::get(),
                     if (requires{ { C::instance } -> std::convertible_to<const C&>; } ) C::instance,
                     if (requires{ { C::get() } -> std::convertible_to<const C&>; } ) C::get(),
                     if (VK_instantiable_with(traits::Singleton, C)) C{}
        );*/

        VK_trait(IsSingleton,
                 let(isSingleton),
                   given(typename C),
                     if (VK_instantiable_with(traits::Singleton, C)) true//,
                     //otherwise(requires{ { std::addressof(C::get()) } -> std::convertible_to<const C*>; }) /* && satisfied_by<Instance, C>*/
        );
      }

      namespace Domain{
        VK_trait(DomainType,
                 alias(domain_type),
                   given(typename D),
                     member_type(domain_type, traits::Domain<D>),
                     if (std::convertible_to<const D*, const status_domain*>) D,
                     member_type(domain_type, D),
                     otherwise(D)
        );
        VK_trait(StatusType,
                 alias(status_type),
                   given(typename D),
                     member_type(status_type, traits::Domain<D>),
                     member_type(status_type, D),
                     if (std::convertible_to<const D*, const status_domain*>) status_code<D>
        );
        VK_trait(ErrorType,
                 alias(error_type),
                   given(typename D),
                     member_type(error_type, traits::Domain<D>),
                     member_type(error_type, D),
                     if (std::convertible_to<const D*, const status_domain*>) error_code<D>
        );
        VK_trait(ValueType,
                 alias(value_type),
                   given(typename D),
                     member_type(value_type, traits::Domain<D>),
                     member_type(value_type, D)
        );
        VK_trait(IsConstant,
                 let(is_constant),
                   given(typename D),
                     member_value(is_constant, traits::Domain<D>),
                     if (satisfies<D, Class::IsSingleton>) true
        );
      }

      namespace Numeric{
        VK_trait(IsIntegral,
                   let(is_integral),
                     given(typename N),
                       member_value(integral, traits::Numeric<N>),
                       if (std::integral<N>) true
        );
        VK_trait(IsFloatingPoint,
                   let(is_floating_point),
                     given(typename N),
                     member_value(floating_point, traits::Numeric<N>),
                     if (std::floating_point<N>) true
        );
        VK_trait(IsSigned,
                 let(is_signed),
                   given(typename N),
                     member_value(is_signed, traits::Numeric<N>),
                     if (std::floating_point<N>) true,
                     otherwise (std::signed_integral<N>)
        );
        VK_trait(IsBounded,
                 let(is_bounded),
                   given(typename N),
                     member_value(is_bounded, traits::Numeric<N>),
                     if (std::numeric_limits<N>::is_specialized) std::numeric_limits<N>::is_bounded,
                     otherwise (true)
        );
        VK_trait(IsExact,
                 let(is_exact),
                   given(typename N),
                     member_value(is_exact, traits::Numeric<N>),
                     if (std::numeric_limits<N>::is_exact) std::numeric_limits<N>::is_exact,
                     otherwise (probe<IsIntegral, N>::is_integral)
        );
        VK_trait(Bits,
                 let(bits),
                   given(typename N),
                     member_value(bits, traits::Numeric<N>),
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
                     if (satisfies<A, BaseType> && satisfies<typename probe<BaseType, A>::base_type, RootBaseType>)(typename probe<BaseType, A>::base_type),
                     if (satisfies<A, MessageType> && satisfies<A, StatusType>) A
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
      template <typename P> requires(requires{ typename traits::Pointer<P>::template rebind<void>; })
      struct PointerRebind<P>{
        template <typename U>
        using rebind = typename traits::Pointer<P>::template rebind<U>;
      };


      template <typename P>
      struct PointerToAddressBackup{};
      template <typename P> requires(requires(P& p){ { std::to_address(p) } -> concepts::raw_pointer; })
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
      template <typename P> requires(requires(P& p){ { traits::Pointer<P>::toAddress(p) } -> concepts::raw_pointer; })
      struct PointerToAddress<P>{
        inline constexpr static auto* toAddress(P& p) noexcept {
          return traits::Pointer<P>::toAddress(p);
        }
      };


      template <typename P>
      struct PointerToPointerBackup{};
      template <typename P> requires(requires(typename probe<Pointer::ElementType, P>::element_type& t){ { std::pointer_traits<P>::to_pointer(t) } -> exact_same_as<P>; })
      struct PointerToPointerBackup<P>{
        inline constexpr static P toPointer(typename probe<Pointer::ElementType, P>::element_type& t) noexcept {
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
      template <typename P> requires(requires(typename probe<Pointer::ElementType, P>::element_type& t){ { traits::Pointer<P>::toPointer(t) } -> exact_same_as<P>; })
      struct PointerToPointer<P>{
        inline constexpr static P toPointer(typename probe<Pointer::ElementType, P>::element_type& t) noexcept {
          return traits::Pointer<P>::toPointer(t);
        }
      };


      template <typename T>
      struct PointerCastBackup{};
      template <typename P> requires(requires(const P& p){ { static_cast<typename PointerRebind<P>::template rebind<copy_cv_t<void, typename probe<Pointer::ElementType, P>::element_type>>>(p) }; })
      struct PointerCastBackup<P>{
      private:
        template <typename To>
        using cast_result_t = typename PointerRebind<P>::template rebind<copy_cv_t<To, typename probe<Pointer::ElementType, P>::element_type>>;
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
        { traits::Pointer<P>::template cast<void>(ptr) } ->
        exact_same_as<typename PointerRebind<P>::template rebind<
            copy_cv_t<void, typename probe<Pointer::ElementType, P>::element_type>>>;
      })
      struct PointerCast<P>{
        template <typename To>
        inline constexpr static auto cast(const P& ptr) noexcept {
          return traits::Pointer<P>::template cast<To>(ptr);
        }
      };

      template <typename T>
      struct ArrayInfo;
      template <typename T> requires( requires(T& value) {
        {traits::DynamicArray<T>::data(value) };
        { traits::DynamicArray<T>::size(value) } -> std::convertible_to<size_t>;
      })
      struct ArrayInfo<T> : public probe<Array::ElementType, T>{
        inline constexpr static bool isDynamic = true;
        inline constexpr static decltype(auto) getData(T& value) noexcept {
          return traits::DynamicArray<T>::data(value);
        }
        inline constexpr static decltype(auto) getData(const T& value) noexcept {
          return traits::DynamicArray<T>::data(value);
        }
        inline constexpr static decltype(auto) getSize(const T& value) noexcept {
          return traits::DynamicArray<T>::size(value);
        }
      };
      template <typename T> requires( requires(T& value) {
        { traits::static_array<T>::data(value) };
        { traits::static_array<T>::size } -> std::convertible_to<size_t>;
      })
      struct ArrayInfo<T> : public probe<Array::ElementType, T>{
        inline constexpr static bool isDynamic = false;
        inline constexpr static size_t size = traits::static_array<T>::size;
        inline constexpr static decltype(auto) getData(T& value) noexcept {
          return traits::static_array<T>::data(value);
        }
        inline constexpr static decltype(auto) getData(const T& value) noexcept {
          return traits::static_array<T>::data(value);
        }
        inline constexpr static decltype(auto) getSize(const T& value) noexcept {
          return traits::static_array<T>::size;
        }
      };
      template <typename T> requires(requires { sizeof(ArrayInfo<T>); })
      struct ArrayInfo<T&> : ArrayInfo<T>{};
      template <typename T> requires(requires { sizeof(ArrayInfo<T>); })
      struct ArrayInfo<T&&> : ArrayInfo<T>{};
      template <typename T> requires(sizeof(traits::View<T>) > 0 && requires { sizeof(ArrayInfo<T>); })
      struct ArrayInfo<const T> : ArrayInfo<T>{ };


      template <typename E>
      struct EnumInfo
          : probe<Enum::Type, E>,
            probe<Enum::UnderlyingType, E>,
            probe<Enum::Values, E>,
            probe<Enum::FlagType,  E>,
            probe<Enum::IsStatusEnum, E>,
            probe<Enum::StatusType, E>,
            probe<Enum::status_domain, E>,
            probe<Enum::StatusInfo, E>,
            probe<Enum::ScopedName, E>,
            probe<Enum::Name, E>{
        //inline constexpr static bool isScoped = std::convertible_to<typename probe<Enum::Type, E>::enum_type, typename probe<Enum::UnderlyingType, E>::underlying_type>;
        //inline constexpr static bool isBitFlag = VK_instantiable_with(traits::BitFlagEnum, E);
      };



      template <typename T>
      struct PointerInfo :
          probe<Pointer::ElementType, T>,
          //PointerElementType<T>,
          PointerRebind<T>,
          probe<Pointer::Reference, T>,
          //PointerReference<T>,
          probe<Pointer::DifferenceType, T>,
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
          : probe<Domain::DomainType, D>,
            probe<Domain::StatusType, D>,
            probe<Domain::ErrorType, D>,
            probe<Domain::ValueType, D>,
            probe<Domain::IsConstant, D>{};

      template <typename C>
      struct ClassInfo :
          probe<Class::Name, C>,
          probe<Class::Uuid, C>,
          //probe<Class::Info, C>,
          probe<Class::Size, C>,
          probe<Class::Alignment, C>,
          probe<Class::IsSingleton, C>/*,
          probe<Class::Instance, C>*/{};


      template <typename N>
      struct NumericInfo
          : probe<Numeric::IsIntegral, N>,
            probe<Numeric::IsFloatingPoint, N>,
            probe<Numeric::IsSigned, N>,
            probe<Numeric::IsBounded, N>,
            probe<Numeric::IsExact, N>,
            probe<Numeric::Bits, N>{};

      template <typename A>
      struct AgentInfo
          : probe<Agent::BaseType, A>,
            probe<Agent::RootBaseType, A>,
            probe<Agent::MessageType, A>,
            probe<Agent::StatusType, A>{};
    }
  }
  
  inline namespace concepts{
    template <typename P, typename T = const void>
    concept pointer = raw_pointer<P> || (requires(P&& p){
      typename traits::detail::PointerInfo<P>::element_type;
      typename traits::detail::PointerInfo<P>::difference_type;
      typename traits::detail::PointerInfo<P>::template rebind<void>;
    } && (exact_same_as<T, const void> ||
          requires(P& p){
            { *p } -> exact_same_as<typename traits::detail::PointerInfo<P>::reference>;
            { traits::detail::PointerInfo<P>::template cast<void>(p) };
            { p.operator->() };
          }));

    template <typename P, typename T = const void>
    concept pointer_fwd_ref = pointer<std::remove_reference_t<P>, T>;

    template <typename T, typename U>
    concept aliased_by = requires{
      typename traits::Alias<U, T>;
    } && requires(T* pTarget, U* pAliased){
      { traits::Alias<U, T>::reify(pAliased) } -> exact_same_as<T*>;
      { traits::Alias<U, T>::alias(pTarget) }  -> exact_same_as<U*>;
    } && ((sizeof(T) % sizeof(U)) == 0);

    template <typename P, typename T>
    concept array_pointer = pointer<P> && aliased_by<typename traits::detail::PointerInfo<P>::element_type, T>;



    template <typename T>
    concept bitwise_movable = requires{
      sizeof(traits::BitwiseMovable<T>);
    };

    template <typename T>
    concept is_register_copyable = std::conjunction_v<std::is_trivially_copyable<T>, std::is_trivially_destructible<T>> && sizeof(T) <= 16;


    template <typename A, typename T = byte>
    concept contiguous_range_type = requires(A&& a){
      typename traits::detail::ArrayInfo<A>::element_type;
      { traits::detail::ArrayInfo<A>::getData(std::forward<A>(a)) };
      { traits::detail::ArrayInfo<A>::getSize(std::forward<A>(a)) } -> std::convertible_to<size_t>;
    } && aliased_by<typename traits::detail::ArrayInfo<A>::element_type, T>;
    template <typename A, typename T = byte>
    concept dynamic_memory_span_type = contiguous_range_type<A, T> && traits::detail::ArrayInfo<A>::isDynamic;
    template <typename A, typename T = byte>
    concept static_memory_span_type = contiguous_range_type<A, T> && !traits::detail::ArrayInfo<A>::isDynamic
                               && requires{
      { traits::detail::ArrayInfo<A>::size } -> std::convertible_to<size_t>;
    };

    template <typename From, typename To>
    concept bit_castable_to = sizeof(From) == sizeof(To) && bitwise_movable<To> && bitwise_movable<From>;


    /*template <typename T>
    concept Status = *//*VK_instantiable_with(traits::Status, T)*//*true;

    template <typename T>
    concept Error = *//*VK_instantiable_with(traits::Error, T)*//*true;*/

    template <typename T>
    concept status_enum_type = VK_instantiable_with(traits::StatusEnum, T) &&
                         requires(const T& t){
                           typename traits::detail::EnumInfo<T>::status_type;
                           typename traits::detail::EnumInfo<T>::domain_type;
                         };

    template <typename C, typename T = void>
    concept container_type = requires(C& lv, const C& clv){
      typename traits::detail::ContainerInfo<C>;
      typename traits::detail::ContainerInfo<C>::value_type;
      typename traits::detail::ContainerInfo<C>::reference;
      typename traits::detail::ContainerInfo<C>::const_reference;
      typename traits::detail::ContainerInfo<C>::pointer;
      typename traits::detail::ContainerInfo<C>::const_pointer;
      typename traits::detail::ContainerInfo<C>::iterator;
      typename traits::detail::ContainerInfo<C>::const_iterator;
      { traits::detail::ContainerInfo<C>::isDynamic } -> same_as<bool>;
    };

    template <typename R, typename T = void>
    concept range_type     = VK_instantiable_with(traits::DynamicArray, R) || VK_instantiable_with(traits::static_array, R);

    template <typename S>
    concept string_type    = VK_instantiable_with(traits::String, S);

    template <typename I, typename T = void>
    concept iterator_type  = VK_instantiable_with(traits::Iterator, I) && (exact_same_as<T, void> || (requires{ typename traits::detail::IteratorInfo<I>::reference_type; } && std::convertible_to<typename traits::detail::IteratorInfo<I>::reference_type, std::add_lvalue_reference_t<T>>));

    template <typename A, typename T = void>
    concept allocator_type = requires(A& alloc, size_t n){
      { alloc.allocate(n) } -> pointer<T>;
    } && requires(A& alloc, size_t n, decltype(alloc.allocate(n)) p){
      { alloc.deallocate(p) };
    };

    template <typename T>
    concept bitflag_enum_type = VK_instantiable_with(traits::BitFlagEnum, T) && enumerator<T>;

    template <typename T>
    concept class_type = std::is_class_v<T>;

    template <typename T>
    concept integral_type = requires{
      { traits::detail::NumericInfo<T>::is_integral };
    } && traits::detail::NumericInfo<T>::is_integral;
    template <typename T>
    concept floating_point_type = requires{
      { traits::detail::NumericInfo<T>::is_floating_point };
    } && traits::detail::NumericInfo<T>::is_floating_point;
    template <typename T>
    concept numeric_type = integral_type<T> || floating_point_type<T> || VK_instantiable_with(traits::Numeric, T);

    template <typename T>
    concept Domain = true;/*requires{
      //typename traits::detail::DomainInfo<T>::domain_type;
      typename traits::detail::DomainInfo<T>::status_type;
      //typename traits::detail::DomainInfo<T>::error_type;
      typename traits::detail::DomainInfo<T>::value_type;
      { traits::detail::DomainInfo<T>::is_constant } -> std::convertible_to<bool>;
    };*/

    template <typename T>
    concept AgentLike = requires{
      typename traits::detail::AgentInfo<T>::base_type;
      typename traits::detail::AgentInfo<T>::root_base_type;
      typename traits::detail::AgentInfo<T>::message_type;
      typename traits::detail::AgentInfo<T>::status_type;
    };
    template <typename T, typename ...Args>
    concept ConstructibleFrom = std::constructible_from<T, Args...> ||
                               requires(Args&& ...args){
                                 { T{ std::forward<Args>(args)... } };
                               };
  }
  
  template <container_type T>
  using container_traits = traits::detail::ContainerInfo<T>;
  template <range_type T>
  using array_traits     = traits::detail::ArrayInfo<T>;
  template <pointer T>
  using pointer_traits   = traits::detail::PointerInfo<T>;
  template <enumerator T>
  using enum_traits      = traits::detail::EnumInfo<T>;
  template <string_type T>
  using string_traits    = traits::detail::StringInfo<T>;
  template <iterator_type T>
  using iterator_traits  = traits::detail::IteratorInfo<T>;/*
  template <allocator_type T>
  using allocator_traits = traits::detail::AllocatorInfo<T>;*/
  template <class_type C>
  using class_traits     = traits::detail::ClassInfo<C>;
  template <numeric_type N>
  using numeric_traits   = traits::detail::NumericInfo<N>;
  template <Domain D>
  using domain_traits    = traits::detail::DomainInfo<D>;
  template <AgentLike A>
  using agent_traits     = traits::detail::AgentInfo<A>;


  template <typename T>
  using param_t = std::conditional_t<is_register_copyable<T>, T, const T&>;
  template <typename T>
  using in_register_t = std::conditional_t<is_register_copyable<T>, T, const T&>;


  template <contiguous_range_type Rng>
  inline constexpr static auto* rangeData(Rng&& rng) noexcept {
    return array_traits<Rng>::getData(std::forward<Rng>(rng));
  }
  template <contiguous_range_type Rng>
  inline constexpr static size_t rangeSize(Rng&& rng) noexcept {
    return array_traits<Rng>::getSize(std::forward<Rng>(rng));
  }
}


template <typename T>
struct valkyrie::traits::Pointer<T*>{
  using element_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  template <typename U>
  using rebind = copy_cv_t<U, T>*;
};

template <typename T> requires(std::is_trivially_copyable_v<T> && std::is_trivially_destructible_v<T>)
struct valkyrie::traits::BitwiseMovable<T>{};


#endif//VALKYRIE_TRAITS_HPP
