//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_UTILITY_CASTING_HPP
#define VALKYRIE_UTILITY_CASTING_HPP

//#include <valkyrie/status/assert.hpp>
#include <valkyrie/traits.hpp>

#include <memory>

namespace valkyrie::casting{

  template <typename From>
  struct simplify{
    using type = From;

    inline static From& getValue(From& val) noexcept { return val; }
  };
  template <typename From>
  struct simplify<const From>{
    using non_const_simple_type = typename simplify<From>::type;
    using type = add_const_past_pointer_t<non_const_simple_type>;
    using return_type = std::conditional_t<std::is_pointer_v<type>, type, type&>;

    inline static return_type getValue(const From& val) noexcept {
      return simplify<From>::getValue(const_cast<From&>(val));
    }
  };

  template <typename To, typename From>
  struct isa{
    inline static bool call(const From& val) noexcept {
      return To::isTypeOf(&val);
    }
  };
  template <typename To, std::derived_from<To> From>
  struct isa<To, From>{
    inline static bool call(const From& val) noexcept { return true; }
  };

  template <typename To, typename From>
  struct cast_return;

  namespace detail{


    template <typename To, typename From, typename Deleter>
    struct cast_deleter{
      struct type : Deleter{
        using Deleter::Deleter;

        using pointer = typename std::pointer_traits<typename Deleter::pointer>::template rebind<To>;

        inline void operator()(pointer p) noexcept(noexcept((*static_cast<Deleter*>(this)(p)))){
          (*static_cast<Deleter*>(this)(p));
        }
      };
    };





    template <typename To, typename From>
    struct isa_impl_redirect{
      inline static bool call(const From& val) noexcept {
        return casting::isa<To, From>::call(val);
      }
    };
    template <typename To, typename From>
    struct isa_impl_redirect<To, const From>{
      inline static bool call(const From& val) noexcept {
        return casting::isa<To, From>::call(val);
      }
    };
    template <typename To, typename From, typename Deleter>
    struct isa_impl_redirect<To, const std::unique_ptr<From, Deleter>>{
      inline static bool call(const std::unique_ptr<From, Deleter>& val) noexcept {
        return casting::isa<To, From>::call(*val);
      }
    };
    template <typename To, typename From>
    struct isa_impl_redirect<To, From*>{
      inline static bool call(const From* val) noexcept {
        return casting::isa<To, From>::call(*val);
      }
    };
    template <typename To, typename From>
    struct isa_impl_redirect<To, From* const>{
      inline static bool call(const From* val) noexcept {
        return casting::isa<To, From>::call(*val);
      }
    };
    template <typename To, typename From>
    struct isa_impl_redirect<To, const From*>{
      inline static bool call(const From* val) noexcept {
        return casting::isa<To, From>::call(*val);
      }
    };
    template <typename To, typename From>
    struct isa_impl_redirect<To, const From* const>{
      inline static bool call(const From* val) noexcept {
        return casting::isa<To, From>::call(*val);
      }
    };

    template <typename To, typename From, typename SimpleFrom>
    struct isa_impl_wrap{
      inline static bool call(const From& from) noexcept {
        return isa_impl_wrap<To,
                           SimpleFrom,
                           typename simplify<SimpleFrom>::type>::
            call(simplify<const From>::getValue(from));
      }
    };
    template <typename To, typename From>
    struct isa_impl_wrap<To, From, From>{
      inline static bool call(const From& from) noexcept {
        return isa_impl_redirect<To, From>::call(from);
      }
    };


    template <typename To, typename From>
    struct cast_return_impl{
      using type = To&;
    };
    template <typename To, typename From>
    struct cast_return_impl<To, const From>{
      using type = const To&;
    };
    template <typename To, typename From>
    struct cast_return_impl<To, From*>{
      using type = To*;
    };
    template <typename To, typename From>
    struct cast_return_impl<To, const From*>{
      using type = const To*;
    };
    template <typename To, typename From>
    struct cast_return_impl<To, From* const>{
      using type = To*;
    };
    template <typename To, typename From>
    struct cast_return_impl<To, const From* const>{
      using type = const To*;
    };
    template <typename To, typename From, typename Deleter>
    struct cast_return_impl<To, std::unique_ptr<From, Deleter>>{
      using type = std::unique_ptr<std::remove_pointer_t<typename cast_return_impl<To, From*>::type>, typename cast_deleter<To, From, Deleter>::type>;
    };



    template <typename To, typename From, typename SimpleFrom>
    struct cast_return_impl_wrap{
      using type = typename cast_return<To, SimpleFrom>::type;
    };
    template <typename To, typename From>
    struct cast_return_impl_wrap<To, From, From>{
      using type = typename cast_return_impl<To, From>::type;
    };
  }

  template <typename To, typename From>
  struct cast_return{
    using type = typename detail::cast_return_impl_wrap<To, From, typename simplify<From>::type>;
  };

  template <typename To, typename From, typename SimpleFrom>
  struct cast_convert_value{
    inline static decltype(auto) call(From& from) noexcept {
      return cast_convert_value<To, SimpleFrom, typename simplify<SimpleFrom>::type>::call(simplify<From>::getValue(from));
    }
  };
  template <typename To, typename From>
  struct cast_convert_value<To, From, From>{
    inline static decltype(auto) call(const From& from) noexcept {
      return (typename cast_return<To, From>::type)(const_cast<From&>(from));
    }
  };

  template <typename T>
  using simple_type_t = typename simplify<T>::type;
  template <typename To, typename From>
  using cast_return_t = typename cast_return<To, From>::type;

  template <typename T>
  concept simple_type = exact_same_as<T, simple_type_t<T>>;
  template <typename T>
  concept wrapped_type = !exact_same_as<T, simple_type_t<T>>;


  template <typename To, typename From>
  inline static decltype(auto) doConversion(From& from) noexcept {
    return cast_convert_value<To, From, simple_type_t<From>>::call(from);
  }

}

namespace valkyrie{

  template <typename T> requires(casting::simple_type<std::remove_reference_t<T>>)
  VK_nodiscard inline bool isNull(T&& value) noexcept {
    if constexpr (std::is_pointer_v<std::remove_reference_t<T>>)
      return !std::forward<T>(value);
    else
      return false;
  }
  template <typename T>
  VK_nodiscard inline bool isNull(T&& value) noexcept {
    return ::valkyrie::isNull(casting::simplify<std::remove_reference_t<T>>::getValue(std::forward<T>(value)));
  }

  template <typename To, typename From>
  VK_nodiscard inline bool isA(const From& from) noexcept {
    INVARIANT(not isNull(from));
    return casting::detail::isa_impl_wrap<To, const From, casting::simple_type_t<const From>>::call(from);
  }
  template <typename To, typename From>
  VK_nodiscard inline bool isAAndNonnull(const From& from) noexcept {
    if (isNull(from))
      return false;
    return isA<To>(from);
  }

  template <typename To, casting::wrapped_type From>
  VK_nodiscard inline decltype(auto) cast(const From& from) noexcept {
    INVARIANT(isA<To>(from));
    return casting::doConversion<To>(from);
  }
  template <typename To, typename From>
  VK_nodiscard inline decltype(auto) cast(From& from) noexcept {
    INVARIANT(isA<To>(from));
    return casting::doConversion<To>(from);
  }
  template <typename To, typename From>
  VK_nodiscard inline decltype(auto) cast(From* from) noexcept {
    INVARIANT(isA<To>(from));
    return casting::doConversion<To>(from);
  }
  template <typename To, typename From, typename Deleter>
  VK_nodiscard inline decltype(auto) cast(std::unique_ptr<From, Deleter>&& from) noexcept {
    INVARIANT(isA<To>(from));
    using ret_type = casting::cast_return_t<To, std::unique_ptr<From, Deleter>>;
    return ret_type(cast<To>(from.release()), from.get_deleter());
    //return casting::doConversion<To>(std::move(from));
  }

  template <typename To, casting::wrapped_type From>
  VK_nodiscard inline decltype(auto) cast_or_null(const From& from) noexcept {
    if (isNull(from))
      return nullptr;
    return cast<To>(from);
  }
  template <typename To, typename From>
  VK_nodiscard inline decltype(auto) cast_or_null(From& from) noexcept {
    if (isNull(from))
      return nullptr;
    return cast<To>(from);
  }
  template <typename To, typename From>
  VK_nodiscard inline decltype(auto) cast_or_null(From* from) noexcept {
    if (isNull(from))
      return nullptr;
    return cast<To>(from);
  }
  template <typename To, typename From, typename Deleter>
  VK_nodiscard inline decltype(auto) cast_or_null(std::unique_ptr<From, Deleter>&& from) noexcept {
    if (!from)
      return nullptr;
    return cast<To>(std::move(from));
  }


  template <typename To, casting::wrapped_type From>
  VK_nodiscard inline decltype(auto) dyn_cast(const From& from) noexcept {
    INVARIANT(not isNull(from));
    return isA<To>(from) ? cast<To>(from) : nullptr;
  }
  template <typename To, typename From>
  VK_nodiscard inline decltype(auto) dyn_cast(From& from) noexcept {
    INVARIANT(not isNull(from));
    return isA<To>(from) ? cast<To>(from) : nullptr;
  }
  template <typename To, typename From>
  VK_nodiscard inline decltype(auto) dyn_cast(From* from) noexcept {
    INVARIANT(not isNull(from));
    return isA<To>(from) ? cast<To>(from) : nullptr;
  }
  /*template <typename To, typename From, typename Deleter>
  inline decltype(auto) dyn_cast(std::unique_ptr<From, Deleter>&& from) noexcept {
    INVARIANT(not isNull(from));
    return isA<To>(from) ? cast<To>(std::move(from)) : nullptr;
  }*/

  template <typename To, casting::wrapped_type From>
  VK_nodiscard inline decltype(auto) dyn_cast_or_null(const From& from) noexcept {
    return (!isNull(from) && isA<To>(from)) ? cast<To>(from) : nullptr;
  }
  template <typename To, typename From>
  VK_nodiscard inline decltype(auto) dyn_cast_or_null(From& from) noexcept {
    return (!isNull(from) && isA<To>(from)) ? cast<To>(from) : nullptr;
  }
  template <typename To, typename From>
  VK_nodiscard inline decltype(auto) dyn_cast_or_null(From* from) noexcept {
    return (!isNull(from) && isA<To>(from)) ? cast<To>(from) : nullptr;
  }
  /*template <typename To, typename From, typename Deleter>
  inline decltype(auto) dyn_cast_or_null(std::unique_ptr<From, Deleter>&& from) noexcept {
    return (!isNull(from) && isA<To>(from)) ? cast<To>(std::move(from)) : nullptr;
  }*/


  template <typename To, typename From, typename Deleter>
  VK_nodiscard inline auto unique_dyn_cast(std::unique_ptr<From, Deleter>& from) noexcept {
    INVARIANT(not isNull(from));
    return isA<To>(from) ? cast<To>(std::move(from)) : nullptr;
  }
  template <typename To, typename From, typename Deleter>
  VK_nodiscard inline auto unique_dyn_cast(std::unique_ptr<From, Deleter>&& from) noexcept {
    return unique_dyn_cast<To>(from);
  }

  template <typename To, typename From, typename Deleter>
  VK_nodiscard inline auto unique_dyn_cast_or_null(std::unique_ptr<From, Deleter>& from) noexcept {
    return (!isNull(from) && isA<To>(from)) ? cast<To>(std::move(from)) : nullptr;
  }
  template <typename To, typename From, typename Deleter>
  VK_nodiscard inline auto unique_dyn_cast_or_null(std::unique_ptr<From, Deleter>&& from) noexcept {
    return unique_dyn_cast_or_null<To>(from);
  }




  namespace detail{

    template <typename To, typename From>
    concept static_castable_from = is_register_copyable<To> && is_register_copyable<From> && requires(const From& from){
      {static_cast<To>(from)} noexcept;
    };

    template <typename From, typename To>
    concept erasure_castable_to = bitwise_movable<From> || bitwise_movable<To>;
    template <typename From, typename To>
    concept static_erasure_castable_to = erasure_castable_to<From, To> && static_castable_from<To, From>;

    template <typename From, typename To>
    concept larger_than = sizeof(From) > sizeof(To);
    template <typename From, typename To>
    concept smaller_than = !larger_than<From, To> && sizeof(From) != sizeof(To);

    template <typename From, typename To>
    concept larger_erasure_castable_to = erasure_castable_to<From, To> && larger_than<From, To>;
    template <typename From, typename To>
    concept smaller_erasure_castable_to = erasure_castable_to<From, To> && smaller_than<From, To>;
    template <typename From, typename To>
    concept static_larger_erasure_castable_to = static_erasure_castable_to<From, To> && larger_than<From, To>;
    template <typename From, typename To>
    concept static_smaller_erasure_castable_to = static_erasure_castable_to<From, To> && smaller_than<From, To>;

    template <typename T>
    using underlying_type = typename std::conditional_t<enumerator<T>, std::underlying_type<T>, identity_type_t<T>>::type;

    template <typename OfSize, typename OfSign>
    using erasure_integer_type = std::conditional_t<std::signed_integral<underlying_type<OfSign>>, std::make_signed_t<underlying_type<OfSize>>, std::make_unsigned_t<underlying_type<OfSize>>>;

    template <typename ErasedType, size_t N>
    struct padded_erasure_object{
      static_assert(bitwise_movable<ErasedType>, "ErasedType must be BitCopyMovable");
      static_assert(alignof(ErasedType) <= sizeof(ErasedType), "ErasedType must not be over-aligned");

      ErasedType Val;
      char Padding[N];
      constexpr explicit padded_erasure_object(const ErasedType &V) noexcept :
          Val(V), Padding{}{}
    };

    template <typename T>
    struct status_sizer{
      void* Ptr;
      T Value;
    };

    template <typename To, typename From>
    concept safely_erasable_from = bitwise_movable<From> && (sizeof(status_sizer<From>) <= sizeof(status_sizer<To>));

    template <typename Arg, typename ...Args>
    concept has_make_status_code = requires(Arg&& arg, Args&& ...args){
      { makeStatusCode(std::forward<Arg>(arg), std::forward<Args>(args)...) } -> std::derived_from<status_code<void>>;
    };
  }




  template <typename To, detail::erasure_castable_to<To> From> requires(sizeof(To) == sizeof(From))
  To erasure_cast(const From& from) noexcept {
    return std::bit_cast<To>(from);
  }
  template <typename To, detail::static_larger_erasure_castable_to<To> From>
  To erasure_cast(const From& from) noexcept {
    return static_cast<To>(std::bit_cast<detail::erasure_integer_type<From, To>>(from));
  }
  template <typename To, detail::static_smaller_erasure_castable_to<To> From>
  To erasure_cast(const From& from) noexcept {
    return std::bit_cast<To>(static_cast<detail::erasure_integer_type<To, From>>(from));
  }
  template <typename To, detail::larger_erasure_castable_to<To> From>
  To erasure_cast(const From& from) noexcept {
    return std::bit_cast<detail::padded_erasure_object<To, sizeof(From) - sizeof(To)>>(from).Val;
  }
  template <typename To, detail::smaller_erasure_castable_to<To> From>
  To erasure_cast(const From& from) noexcept {
    return std::bit_cast<To>(detail::padded_erasure_object<From, sizeof(To) - sizeof(From)>{from});
  }






  template <typename To, concepts::pointer_fwd_ref From>
  inline constexpr static decltype(auto) ptr_cast(From&& from) noexcept
  requires(requires{ { pointer_traits<std::remove_reference_t<From>>::template cast<To>(std::forward<From>(from)) }; }) {
    return pointer_traits<std::remove_reference_t<From>>::template cast<To>(std::forward<From>(from));
  }

  template <typename To, concepts::pointer_fwd_ref From> requires(concepts::aliased_by<typename pointer_traits<std::remove_reference_t<From>>::element_type, To>)
  inline constexpr static auto alias_cast(From&& from) noexcept {
    return traits::Alias<To, typename pointer_traits<std::remove_reference_t<From>>::element_type>::alias(std::forward<From>(from));
  }
  template <typename To, concepts::pointer_fwd_ref From> requires(concepts::aliased_by<To, typename pointer_traits<std::remove_reference_t<From>>::element_type>)
  inline constexpr static auto reify_cast(From&& from) noexcept {
    return traits::Alias<typename pointer_traits<std::remove_reference_t<From>>::element_type, To>::reify(std::forward<From>(from));
  }
  template <typename To, concepts::bit_castable_to<To> From>
  inline constexpr static To bit_cast(From&& from) noexcept {
    return std::bit_cast<To>(from);
  }







  /*template <typename T, typename U>
  requires(std::convertible_to<T*, U*> && sizeof(T) == sizeof(U) && alignof(T) == alignof(U))
  struct traits::Alias<T, U>{
    template <concepts::pointer_fwd_ref<T> From>
    inline constexpr static auto alias(From&& pValue) noexcept{
      return ptr_cast<U>(std::forward<From>(pValue));
    }
    template <concepts::pointer_fwd_ref<U> From>
    inline constexpr static auto reify(From&& pValue) noexcept{
      return ptr_cast<T>(std::forward<From>(pValue));
    }
  };

  namespace detail{
    template <typename T>
    struct IsByteAlias : std::disjunction<std::is_same<T, std::byte>, *//*std::is_same<T, char>, *//*std::is_same<T, unsigned char>>{};

    template <typename To, typename From>
    inline constexpr static To* ptr_cast(From* ptr) noexcept{
      VK_consteval_block{
        return std::bit_cast<To*>(ptr);
      }
      VK_runtime_block{
        return static_cast<To*>(static_cast<void*>(ptr));
      }
    }
    template <typename To, typename From>
    inline constexpr static const To* ptr_cast(const From* ptr) noexcept{
      VK_consteval_block{
        return std::bit_cast<const To*>(ptr);
      }
      VK_runtime_block{
        return static_cast<const To*>(static_cast<const void*>(ptr));
      }
    }
  }

  template <typename Byte, typename T> requires(detail::IsByteAlias<Byte>::value && !std::is_const_v<T>)
  struct traits::Alias<Byte, T>{

  private:
    template <typename From>
    using ptr_traits_t = pointer_traits<std::remove_reference_t<From>>;
    template <typename From>
    using byte_ptr_t = typename ptr_traits_t<From>::template rebind<Byte>;
    template <typename From>
    using byte_ptr_traits_t = pointer_traits<byte_ptr_t<From>>;
    template <typename From>
    using reified_ptr_t = typename ptr_traits_t<From>::template rebind<T>;
    template <typename From>
    using reified_ptr_traits_t = pointer_traits<reified_ptr_t<From>>;
  public:

    template <concepts::pointer_fwd_ref<T> From>
    inline constexpr static auto alias(From&& pValue) noexcept{
      return byte_ptr_traits_t<From>::toPointer(*ptr_cast<Byte>(ptr_traits_t<From>::toAddress(std::forward<From>(pValue))));
    }
    template <concepts::pointer_fwd_ref<Byte> From>
    inline constexpr static auto reify(From&& pValue) noexcept{
      return reified_ptr_traits_t<From>::toPointer(*ptr_cast<T>(ptr_traits_t<From>::toAddress(std::forward<From>(pValue))));
    }
  };
  template <typename Byte, typename T>
  struct traits::Alias<const Byte, T>{
  private:
    template <typename From>
    using ptr_traits_t = pointer_traits<std::remove_reference_t<From>>;
    template <typename From>
    using byte_ptr_t = typename ptr_traits_t<From>::template rebind<const Byte>;
    template <typename From>
    using byte_ptr_traits_t = pointer_traits<byte_ptr_t<From>>;
    template <typename From>
    using reified_ptr_t = typename ptr_traits_t<From>::template rebind<const T>;
    template <typename From>
    using reified_ptr_traits_t = pointer_traits<reified_ptr_t<From>>;
  public:

    template <concepts::pointer_fwd_ref<T> From>
    inline constexpr static auto alias(From&& pValue) noexcept{
      return byte_ptr_traits_t<From>::toPointer(*ptr_cast<const Byte>(ptr_traits_t<From>::toAddress(std::forward<From>(pValue))));
    }
    template <concepts::pointer_fwd_ref<const Byte> From>
    inline constexpr static auto reify(From&& pValue) noexcept{
      return reified_ptr_traits_t<From>::toPointer(*ptr_cast<const T>(ptr_traits_t<From>::toAddress(std::forward<From>(pValue))));
    }
  };*/









}

#endif//VALKYRIE_UTILITY_CASTING_HPP
