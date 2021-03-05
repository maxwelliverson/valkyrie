//
// Created by Maxwell on 2020-11-23.
//

#ifndef VALKYRIE_META_HPP
#define VALKYRIE_META_HPP

#include <type_traits>

namespace valkyrie{
  
  namespace meta{
    
    struct template_no_default{ using invalid_specialization = void; };



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




    template <typename A, typename B>
    struct is_same_as : false_type {};
    template <typename A>
    struct is_same_as<A, A> : true_type {};
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
  using add_const_past_pointer_t = typename meta::add_const_past_pointer<T>::type;
}

#define VK_wrap(...) decltype(::valkyrie::meta::detail::wrap_<__VA_ARGS__>(::valkyrie::meta::overload<2>{}))
#define VK_instantiable_with(...) concepts::specialized<VK_foreach_delimit(VK_wrap, VK_comma_delimiter, ##__VA_ARGS__)>

#endif//VALKYRIE_META_HPP
