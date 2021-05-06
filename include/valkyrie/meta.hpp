//
// Created by Maxwell on 2020-11-23.
//

#ifndef VALKYRIE_META_HPP
#define VALKYRIE_META_HPP

#include <valkyrie/preprocessor.hpp>
#include <valkyrie/primitives.hpp>

#include <concepts>
#include <bit>

namespace valkyrie{
  
  namespace meta{
    
    struct template_no_default{ using invalid_specialization = void; };


    template <typename ...>
    struct list{};



    template <typename T>
    struct identity{
      using type = T;
    };

    struct fallback{};

    template <size_t N>
    struct overload : overload<N - 1>{};
    template <>
    struct overload<0> : fallback{};

    struct template_wrapper{ using is_a_template = void; };

    template <decltype(auto) Val>
    struct value_wrapper{
      using is_a_value = void;
      inline constexpr static decltype(auto) value = Val;
    };






    template <typename T>
    concept wrapped_value = requires{
      typename T::is_a_value;
    } && std::is_void_v<typename T::is_a_value>;

    template <wrapped_value T>
    inline constexpr static decltype(auto) unwrapValue = T::value;

    template <template <typename...> typename TT>
    struct t_template : template_wrapper{
      template <typename ...Args> requires(requires{ typename TT<Args...>; })
      using instantiate = TT<Args...>;
    };
    template <template <decltype(auto)...> typename TT>
    struct v_template : template_wrapper{
      template <wrapped_value ...Args> requires(requires{ typename TT<unwrapValue<Args>...>; })
      using instantiate = TT<unwrapValue<Args>...>;
    };
    template <template <typename, decltype(auto)...> typename TT>
    struct tv_template : template_wrapper{
      template <typename T, wrapped_value ...Args> requires(requires{ typename TT<T, unwrapValue<Args>...>; })
      using instantiate = TT<T, unwrapValue<Args>...>;
    };
    template <template <decltype(auto), typename...> typename TT>
    struct vt_template : template_wrapper{
      template <wrapped_value V, typename...Args> requires(requires{ typename TT<unwrapValue<V>, Args...>; })
      using instantiate = TT<unwrapValue<V>, Args...>;
    };
    template <template <typename, typename, decltype(auto)...> typename TT>
    struct ttv_template : template_wrapper{
      template <typename T, typename U, wrapped_value ...Args> requires(requires{ typename TT<T, U, unwrapValue<Args>...>; })
      using instantiate = TT<T, U, unwrapValue<Args>...>;
    };
    template <template <decltype(auto), decltype(auto), typename...> typename TT>
    struct vvt_template : template_wrapper{
      template <wrapped_value V, wrapped_value U, typename...Args> requires(requires{ typename TT<unwrapValue<V>, unwrapValue<U>, Args...>; })
      using instantiate = TT<unwrapValue<V>, unwrapValue<U>, Args...>;
    };
    template <template <decltype(auto), typename, decltype(auto)...> typename TT>
    struct vtv_template : template_wrapper{
      template <wrapped_value T, typename U, wrapped_value ...Args> requires(requires{ typename TT<unwrapValue<T>, U, unwrapValue<Args>...>; })
      using instantiate = TT<unwrapValue<T>, U, unwrapValue<Args>...>;
    };
    template <template <typename, decltype(auto), typename...> typename TT>
    struct tvt_template : template_wrapper{
      template <typename T, wrapped_value U, typename...Args> requires(requires{ typename TT<T, unwrapValue<U>, Args...>; })
      using instantiate = TT<T, unwrapValue<U>, Args...>;
    };
    
    namespace detail {

      template<template<typename...> typename TT>
      t_template<TT> wrap_(overload<2>) noexcept;
      template<template<decltype(auto)...> typename TT>
      v_template<TT> wrap_(overload<2>) noexcept;
      template<template<typename, decltype(auto)...> typename TT>
      tv_template<TT> wrap_(overload<1>) noexcept;
      template<template<decltype(auto), typename, typename...> typename TT>
      vt_template<TT> wrap_(overload<1>) noexcept;
      template<template<typename, typename, decltype(auto)...> typename TT>
      ttv_template<TT> wrap_(overload<0>) noexcept;
      template<template<decltype(auto), decltype(auto), typename...> typename TT>
      vvt_template<TT> wrap_(overload<0>) noexcept;
      template<template<decltype(auto), typename, decltype(auto)...> typename TT>
      vtv_template<TT> wrap_(overload<0>) noexcept;
      template<template<typename, decltype(auto), typename...> typename TT>
      tvt_template<TT> wrap_(overload<0>) noexcept;

      template<typename T>
      T wrap_(overload<2>) noexcept;
      template<decltype(auto) Val>
      value_wrapper<Val> wrap_(overload<2>) noexcept;


    }


    template <typename T>
    struct add_const_past_pointer{
      using type = const T;
    };
    template <typename T>
    struct add_const_past_pointer<T*>{
      using type = const T*;
    };




    template <typename To, typename From>
    struct copy_cv{
      using type = To;
    };
    template <typename To, typename From>
    struct copy_cv<To, const From>{
      using type = const typename copy_cv<To, From>::type;
    };
    template <typename To, typename From>
    struct copy_cv<To, volatile From>{
      using type = volatile typename copy_cv<To, From>::type;
    };


    template <typename To, typename From>
    struct copy_ref{
      using type = To;
    };
    template <typename To, typename From>
    struct copy_ref<To, From&>{
      using type = To&;
    };
    template <typename To, typename From>
    struct copy_ref<To, From&&>{
      using type = To&&;
    };

    template <typename To, typename From>
    struct copy_cv_ref{
      using type = To;
    };
    template <typename To, typename From> requires(!std::is_same_v<To, typename copy_ref<typename copy_cv<To, std::remove_reference_t<From>>::type, From>::type>)
    struct copy_cv_ref<To, From>{
      using type = typename copy_ref<typename copy_cv<To, std::remove_reference_t<From>>::type, From>::type;
    };

    template <bool B>
    using boolean_constant = value_wrapper<B>;

    using false_type = boolean_constant<false>;
    using true_type  = boolean_constant<true>;


    template <typename T>
    struct is_typelist : false_type{};
    template <typename ...T>
    struct is_typelist<list<T...>> : true_type{};


    template <typename A, typename B>
    struct is_same_as : false_type {};
    template <typename A>
    struct is_same_as<A, A> : true_type {};


    template <typename T>
    struct is_a_member_pointer : false_type {};
    template <typename T, typename C>
    struct is_a_member_pointer<T C::*> : true_type {};

    template <typename T>
    struct member_pointer_pointee;
    template <typename T, typename C>
    struct member_pointer_pointee<T C::*>{
      using type = T;
    };

    template <typename T>
    struct member_pointer_class;
    template <typename T, typename C>
    struct member_pointer_class<T C::*>{
      using type = C;
    };
  }

  template <typename T>
  using identity_type_t = typename meta::identity<T>::type;

  template <typename To, typename From>
  using copy_cv_t = typename meta::copy_cv<To, From>::type;
  template <typename To, typename From>
  using copy_ref_t = typename meta::copy_ref<To, From>::type;
  template <typename To, typename From>
  using copy_cvref_t = typename meta::copy_cv_ref<To, From>::type;


  template <typename T>
  using add_const_t = std::add_const_t<T>;
  template <typename T>
  using add_volatile_t = std::add_volatile_t<T>;
  template <typename T>
  using add_cv_t = add_const_t<add_volatile_t<T>>;


  template <typename T>
  using remove_const_t = std::remove_const_t<T>;
  template <typename T>
  using remove_volatile_t = std::remove_volatile_t<T>;
  template <typename T>
  using remove_cv_t = std::remove_cv_t<T>;

  template <typename T>
  using remove_ref_t = std::remove_reference_t<T>;

  template <typename T>
  using remove_cvref_t = std::remove_cvref_t<T>;

  template <typename T>
  using add_const_past_pointer_t = typename meta::add_const_past_pointer<T>::type;


  template <typename P> requires meta::is_a_member_pointer<P>::value
  using member_pointer_pointee_t = typename meta::member_pointer_pointee<P>::type;
  template <typename P> requires meta::is_a_member_pointer<P>::value
  using member_pointer_class_t = typename meta::member_pointer_class<P>::type;


  inline namespace concepts{
    template <typename T>
    concept typelist = meta::is_typelist<T>::value;
    template <typename T>
    concept nonempty_typelist = typelist<T> && !meta::is_same_as<T, meta::list<>>::value;
  }


  namespace meta{

    using empty_list = list<>;

    namespace impl{

      template <typename>
      struct front;
      template <typename T, typename ...U>
      struct front<list<T, U...>> {
        using type = T;
      };
      template <typename>
      struct back;
      template <typename T>
      struct back<list<T>>{
        using type = T;
      };
      template <typename T, typename U, typename ...V>
      struct back<list<T, U, V...>> : back<list<U, V...>>{};

      template <typename L, typename T>
      struct push_front;
      template <typename ...U, typename T>
      struct push_front<list<U...>, T>{
        using type = list<T, U...>;
      };
      template <typename L, typename T>
      struct push_back;
      template <typename ...U, typename T>
      struct push_back<list<U...>, T>{
        using type = list<U..., T>;
      };
      template <typename L>
      struct pop_front;
      template <typename T, typename ...U>
      struct pop_front<list<T, U...>>{
        using type = list<U...>;
      };
      template <typename>
      struct pop_back;
      template <typename T>
      struct pop_back<list<T>>{
        using type = empty_list;
      };
      template <typename T, typename U, typename ...V>
      struct pop_back<list<T, U, V...>>{
        using type = typename push_front<typename pop_back<list<U, V...>>::type, T>::type;
      };


      template <typename L, typename T>
      struct contains;
      template <typename T, typename ...U>
      struct contains<list<T, U...>, T> : true_type{};
      template <typename T, typename U, typename ...V>
      struct contains<list<U, V...>, T> : contains<list<V...>, T>{};
      template <typename T>
      struct contains<empty_list, T> : false_type{};


      template <typename A, typename B>
      struct append;
      template <typename ...T, typename ...U>
      struct append<list<T...>, list<U...>>{
        using type = list<T..., U...>;
      };
    }

    template <nonempty_typelist L>
    using front_t = typename impl::front<L>::type;
    template <nonempty_typelist L>
    using back_t  = typename impl::back<L>::type;

    template <nonempty_typelist L>
    using pop_front_t = typename impl::pop_front<L>::type;
    template <nonempty_typelist L>
    using pop_back_t  = typename impl::pop_back<L>::type;

    template <typelist L, typename T>
    using push_front_t = typename impl::push_front<L, T>::type;
    template <typelist L, typename T>
    using push_back_t = typename impl::push_back<L, T>::type;

    template <typelist A, typelist B>
    using append_list_t = typename impl::append<A, B>::type;

    template <typename T, typename U>
    concept typelist_containing = nonempty_typelist<T> && impl::contains<T, U>::value;

    namespace impl{
      template <typelist In, typelist Out>
      struct unique;
      template <typelist Out>
      struct unique<empty_list, Out>{
        using type = Out;
      };

      template <nonempty_typelist In, typelist_containing<front_t<In>> Out>
      struct unique<In, Out> : unique<pop_front_t<In>, Out>{};
      template <nonempty_typelist In, typelist Out>
      struct unique<In, Out> : unique<pop_front_t<In>, push_back_t<Out, front_t<In>>>{};
    }

    template <typelist L>
    using unique_list_t = typename impl::unique<L, empty_list>::type;

    template <typename L>
    concept typeset = typelist<L> && is_same_as<L, unique_list_t<L>>::value;

    static_assert(is_same_as<unique_list_t<list<int, int, double, float, int>>, list<int, double, float>>::value);


    template <typeset A, typeset B>
    using set_union_t = unique_list_t<append_list_t<A, B>>;



    template <typename ...T> requires(typeset<list<T...>>)
    using set = list<T...>;

    using empty_set = set<>;

    namespace impl{

      template <typeset A>
      struct set_order;
      template <typename ...T>
      struct set_order<list<T...>> : value_wrapper<sizeof...(T)>{};

      static_assert(set_order<empty_set>::value == 0);

      /*// is A a strict subset of B? (ie. is every element of A an element of B,
      template <typeset A, typeset B>
      struct is_strict_subset_of;
      // if order(A) >= order(B), A cannot be a strict subset of B and it's a waste of time to cross reference elements.
      template <typeset A, typeset B> requires(set_order<A>::value >= set_order<B>::value)
      struct is_strict_subset_of<A, B> : false_type{};
      template <typeset B>
      struct is_strict_subset_of<empty_set, B> : boolean_constant<!is_same_as<empty_set, B>::value>{};
      template <>


      // is A a strict superset of B?
      template <typeset A, typeset B>
      struct is_strict_superset_of : is_strict_subset_of<B, A>{};


      // checks whether A is a superset of B
      template <typeset A, typeset B>
      struct is_superset_of;
      template <typeset B>
      struct is_superset_of<empty_set, B> : is_same_as<B, empty_set>{};
      template <typeset A, typeset B>
      struct is_subset_of{};*/

      template <typename A, typename B, typeset Out = empty_set>
      struct intersection_set;

      template <typeset Out>
      struct intersection_set<empty_set, empty_set, Out>{
        using type = Out;
      };
      template <typename A, typeset Out>
      struct intersection_set<A, empty_set, Out>{
        using type = Out;
      };
      template <typename B, typeset Out>
      struct intersection_set<empty_set, B, Out>{
        using type = Out;
      };


      template <typename A, typelist_containing<front_t<A>> B, typeset Out>
      struct intersection_set<A, B, Out> : intersection_set<pop_front_t<A>, B, push_back_t<Out, front_t<A>>>{};
      template <typename A, typename B, typeset Out>
      struct intersection_set : intersection_set<pop_front_t<A>, B, Out>{};


      template <typename A, typename B, typeset Out = empty_set>
      struct difference_set;

      template <typeset Out>
      struct difference_set<empty_set, empty_set, Out>{
        using type = Out;
      };
      template <typename A, typeset Out>
      struct difference_set<A, empty_set, Out>{
        using type = Out;
      };
      template <typename B, typeset Out>
      struct difference_set<empty_set, B, Out>{
        using type = Out;
      };

      template <typename A, typelist_containing<front_t<A>> B, typeset Out>
      struct difference_set<A, B, Out> : difference_set<pop_front_t<A>, B, Out>{};
      template <typename A, typename B, typeset Out>
      struct difference_set : difference_set<pop_front_t<A>, B, push_back_t<Out, front_t<A>>>{};
    }

    template <typeset A>
    inline constexpr static size_t set_order = impl::set_order<A>::value;


    template <typeset A, typeset B>
    using set_intersection_t = typename impl::intersection_set<A, B, empty_set>::type;
    template <typeset A, typeset B>
    using set_difference_t = typename impl::difference_set<A, B, empty_set>::type;
    template <typeset A, typeset B>
    using set_symmetric_difference_t = set_difference_t<set_union_t<A, B>, set_intersection_t<A, B>>;



    template <typename A, typename B>
    concept set_equivalent_to =
        typeset<A> &&
        typeset<B> &&
            (set_order<A> == set_order<B>) &&
        is_same_as<set_symmetric_difference_t<A, B>, empty_set>::value;

    template <typename A, typename B>
    concept strict_subset_of =
        typeset<A> &&
        typeset<B> &&
        (set_order<A> < set_order<B>) &&
        is_same_as<set_difference_t<A, B>, empty_set>::value;
    template <typename A, typename B>
    concept strict_superset_of = strict_subset_of<B, A>;

    template <typename A, typename B>
    concept subset_of = strict_subset_of<A, B> || set_equivalent_to<A, B>;
    template <typename A, typename B>
    concept superset_of = strict_superset_of<A, B> || set_equivalent_to<A, B>;


    static_assert(subset_of<set<int, float>, set<int, float>>);
    static_assert(subset_of<set<int, float>, set<float, int, double>>);
    static_assert(set_equivalent_to<set<int, float, double>, set<double, int, float>>);
    static_assert(strict_subset_of<set<int>, set<float, double, int>>);

  }







  namespace impl{

    template <typename T>
    struct strip_unbounded_array{
      using type = T;
    };
    template <typename T>
    struct strip_unbounded_array<T[]>{
      using type = T;
    };

    template <auto MemPtr>
    struct get_member_offset;
    template <typename Member, typename Class, Member Class::* MemPtr>
    struct get_member_offset<MemPtr>{

      using member_type = std::conditional_t<std::is_unbounded_array_v<Member>, std::remove_extent_t<Member>, Member>;

      // Multiply by two to allow for GCC style trailing array members
      inline constexpr static size_t MemberArraySize = (sizeof(Class) / sizeof(member_type)) * 2;

      union{
        char        init{};
        member_type member[MemberArraySize];
        Class       object;
      };

      constexpr get_member_offset() noexcept : init(0){}

      constexpr size_t calculate() const noexcept {
        const member_type* result_addr = nullptr;
        if constexpr ( std::is_unbounded_array_v<Member> ) {
          result_addr = std::addressof(object)->*MemPtr;
        }
        else {
          result_addr = std::addressof(std::addressof(object)->*MemPtr);
        }
        size_t I = 0;
        for (;I < MemberArraySize; ++I) {
          if (member + I == result_addr)
            return I * sizeof(member_type);
        }

        // should be unreachable
        return size_t(-1);
      }
    };
  }

  template <auto Ptr>
  inline constexpr size_t offset_of() noexcept {
    /*using class_type         = member_pointer_class_t<decltype(Ptr)>;
    using member_type        = member_pointer_pointee_t<decltype(Ptr)>;
    using normal_member_type = std::conditional_t<std::is_unbounded_array_v<member_type>,
                                                  std::remove_extent_t<member_type>,
                                                  member_type>;

    union{
      char               init;
      normal_member_type member_array[(sizeof(class_type) / sizeof(normal_member_type)) * 2];
      class_type         object;
    } u{ .init = 0 };

    const normal_member_type* result_addr = nullptr;
    if constexpr ( std::is_unbounded_array_v<member_type> ) {
      result_addr = std::addressof(u.object)->*Ptr;
    }
    else {
      result_addr = std::addressof(std::addressof(u.object)->*Ptr);
    }
    size_t I = 0;
    for (; I < (sizeof(class_type) / sizeof(normal_member_type)) * 2; ++I) {
      if (u.member_array + I == result_addr)
        return I * sizeof(normal_member_type);
    }

    return u64(-1);*/
    impl::get_member_offset<Ptr> str_{};
    return str_.calculate();
  }
}

#define VK_offsetof(type, member) ::valkyrie::offset_of<&type::member>()
#define VK_wrap(...) decltype(::valkyrie::meta::detail::wrap_<__VA_ARGS__>(::valkyrie::meta::overload<2>{}))
#define VK_instantiable_with(...) concepts::specialized<VK_foreach_delimit(VK_wrap, VK_comma_delimiter, ##__VA_ARGS__)>

#endif//VALKYRIE_META_HPP
