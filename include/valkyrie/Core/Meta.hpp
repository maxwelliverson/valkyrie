//
// Created by Maxwell on 2020-11-23.
//

#ifndef VALKYRIE_META_HPP
#define VALKYRIE_META_HPP

#include <type_traits>

namespace valkyrie::Meta{


  struct TemplateNoDefault{ using invalid_specialization = void; };


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
  } && std::is_void_v<typename T::IsAValue>;

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




  template <typename To, typename From>
  struct CopyCv{
    using type = To;
  };
  template <typename To, typename From>
  struct CopyCv<To, const From>{
    using type = const typename CopyCv<To, From>::type;
  };
  template <typename To, typename From>
  struct CopyCv<To, volatile From>{
    using type = volatile typename CopyCv<To, From>::type;
  };


  template <typename To, typename From>
  struct CopyRef{
    using type = To;
  };
  template <typename To, typename From>
  struct CopyRef<To, From&>{
    using type = To&;
  };
  template <typename To, typename From>
  struct CopyRef<To, From&&>{
    using type = To&&;
  };

  template <typename To, typename From>
  struct CopyCvRef{
    using type = To;
  };
  template <typename To, typename From> requires(!std::is_same_v<To, typename CopyRef<typename CopyCv<To, std::remove_reference_t<From>>::type, From>::type>)
  struct CopyCvRef<To, From>{
    using type = typename CopyRef<typename CopyCv<To, std::remove_reference_t<From>>::type, From>::type;
  };


  template <typename To, typename From>
  using copy_cv_t = typename CopyCv<To, From>::type;
  template <typename To, typename From>
  using copy_ref_t = typename CopyRef<To, From>::type;
  template <typename To, typename From>
  using copy_cvref_t = typename CopyCvRef<To, From>::type;
}

#define VK_wrap(...) decltype(::valkyrie::Meta::wrap_<__VA_ARGS__>(::valkyrie::Meta::Overload<2>{}))
#define VK_instantiable_with(...) Concepts::Specialized<VK_foreach_delimit(VK_wrap, VK_comma_delimiter, ##__VA_ARGS__)>

#endif//VALKYRIE_META_HPP
