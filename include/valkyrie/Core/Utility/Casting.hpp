//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_CASTING_HPP
#define VALKYRIE_CASTING_HPP

#include <valkyrie/Core/Traits.hpp>
#include <valkyrie/Core/Error/Assert.hpp>

#include <memory>

namespace valkyrie::Casting{

  template <typename From>
  struct Simplify{
    using type = From;

    inline static From& getValue(From& val) noexcept { return val; }
  };
  template <typename From>
  struct Simplify<const From>{
    using non_const_simple_type = typename Simplify<From>::type;
    using type = Meta::add_const_past_pointer_t<non_const_simple_type>;
    using return_type = std::conditional_t<std::is_pointer_v<type>, type, type&>;

    inline static return_type getValue(const From& val) noexcept {
      return Simplify<From>::getValue(const_cast<From&>(val));
    }
  };

  template <typename To, typename From>
  struct IsA{
    inline static bool call(const From& val) noexcept {
      return To::isTypeOf(&val);
    }
  };
  template <typename To, std::derived_from<To> From>
  struct IsA<To, From>{
    inline static bool call(const From& val) noexcept { return true; }
  };

  template <typename To, typename From>
  struct CastReturn;

  namespace Detail{


    template <typename To, typename From, typename Deleter>
    struct CastDeleter{
      struct type : Deleter{
        using Deleter::Deleter;

        using pointer = typename std::pointer_traits<typename Deleter::pointer>::template rebind<To>;

        inline void operator()(pointer p) noexcept(noexcept((*static_cast<Deleter*>(this)(p)))){
          (*static_cast<Deleter*>(this)(p));
        }
      };
    };





    template <typename To, typename From>
    struct IsAImplRedirect{
      inline static bool call(const From& val) noexcept {
        return Casting::IsA<To, From>::call(val);
      }
    };
    template <typename To, typename From>
    struct IsAImplRedirect<To, const From>{
      inline static bool call(const From& val) noexcept {
        return Casting::IsA<To, From>::call(val);
      }
    };
    template <typename To, typename From, typename Deleter>
    struct IsAImplRedirect<To, const std::unique_ptr<From, Deleter>>{
      inline static bool call(const std::unique_ptr<From, Deleter>& val) noexcept {
        return Casting::IsA<To, From>::call(*val);
      }
    };
    template <typename To, typename From>
    struct IsAImplRedirect<To, From*>{
      inline static bool call(const From* val) noexcept {
        return Casting::IsA<To, From>::call(*val);
      }
    };
    template <typename To, typename From>
    struct IsAImplRedirect<To, From* const>{
      inline static bool call(const From* val) noexcept {
        return Casting::IsA<To, From>::call(*val);
      }
    };
    template <typename To, typename From>
    struct IsAImplRedirect<To, const From*>{
      inline static bool call(const From* val) noexcept {
        return Casting::IsA<To, From>::call(*val);
      }
    };
    template <typename To, typename From>
    struct IsAImplRedirect<To, const From* const>{
      inline static bool call(const From* val) noexcept {
        return Casting::IsA<To, From>::call(*val);
      }
    };

    template <typename To, typename From, typename SimpleFrom>
    struct IsAImplWrap{
      inline static bool call(const From& from) noexcept {
        return IsAImplWrap<To,
                           SimpleFrom,
                           typename Simplify<SimpleFrom>::type>::
            call(Simplify<const From>::getValue(from));
      }
    };
    template <typename To, typename From>
    struct IsAImplWrap<To, From, From>{
      inline static bool call(const From& from) noexcept {
        return IsAImplRedirect<To, From>::call(from);
      }
    };


    template <typename To, typename From>
    struct CastReturnImpl{
      using type = To&;
    };
    template <typename To, typename From>
    struct CastReturnImpl<To, const From>{
      using type = const To&;
    };
    template <typename To, typename From>
    struct CastReturnImpl<To, From*>{
      using type = To*;
    };
    template <typename To, typename From>
    struct CastReturnImpl<To, const From*>{
      using type = const To*;
    };
    template <typename To, typename From>
    struct CastReturnImpl<To, From* const>{
      using type = To*;
    };
    template <typename To, typename From>
    struct CastReturnImpl<To, const From* const>{
      using type = const To*;
    };
    template <typename To, typename From, typename Deleter>
    struct CastReturnImpl<To, std::unique_ptr<From, Deleter>>{
      using type = std::unique_ptr<std::remove_pointer_t<typename CastReturnImpl<To, From*>::type>, typename CastDeleter<To, From, Deleter>::type>;
    };



    template <typename To, typename From, typename SimpleFrom>
    struct CastReturnImplWrap{
      using type = typename CastReturn<To, SimpleFrom>::type;
    };
    template <typename To, typename From>
    struct CastReturnImplWrap<To, From, From>{
      using type = typename CastReturnImpl<To, From>::type;
    };



  }

  template <typename To, typename From>
  struct CastReturn{
    using type = typename Detail::CastReturnImplWrap<To, From, typename Simplify<From>::type>;
  };

  template <typename To, typename From, typename SimpleFrom>
  struct CastConvertValue{
    inline static decltype(auto) call(From& from) noexcept {
      return CastConvertValue<To, SimpleFrom, typename Simplify<SimpleFrom>::type>::call(Simplify<From>::getValue(from));
    }
  };
  template <typename To, typename From>
  struct CastConvertValue<To, From, From>{
    inline static decltype(auto) call(const From& from) noexcept {
      return (typename CastReturn<To, From>::type)(const_cast<From&>(from));
    }
  };

  template <typename T>
  using simple_type_t = typename Simplify<T>::type;
  template <typename To, typename From>
  using cast_return_t = typename CastReturn<To, From>::type;

  template <typename T>
  concept SimpleType = ExactSameAs<T, simple_type_t<T>>;
  template <typename T>
  concept WrappedType = !ExactSameAs<T, simple_type_t<T>>;


  template <typename To, typename From>
  inline static decltype(auto) doConversion(From& from) noexcept {
    return CastConvertValue<To, From, simple_type_t<From>>::call(from);
  }

}

namespace valkyrie::Core{

  template <typename T> requires(Casting::SimpleType<std::remove_reference_t<T>>)
  VK_nodiscard inline bool isNull(T&& value) noexcept {
    if constexpr (std::is_pointer_v<std::remove_reference_t<T>>)
      return !std::forward<T>(value);
    else
      return false;
  }
  template <typename T>
  VK_nodiscard inline bool isNull(T&& value) noexcept {
    return ::valkyrie::Core::isNull(Casting::Simplify<std::remove_reference_t<T>>::getValue(std::forward<T>(value)));
  }

  template <typename To, typename From>
  VK_nodiscard inline bool isA(const From& from) noexcept {
    INVARIANT(not isNull(from));
    return Casting::Detail::IsAImplWrap<To, const From, Casting::simple_type_t<const From>>::call(from);
  }
  template <typename To, typename From>
  VK_nodiscard inline bool isAAndNonnull(const From& from) noexcept {
    if (isNull(from))
      return false;
    return isA<To>(from);
  }

  template <typename To, Casting::WrappedType From>
  VK_nodiscard inline decltype(auto) cast(const From& from) noexcept {
    INVARIANT(isA<To>(from));
    return Casting::doConversion<To>(from);
  }
  template <typename To, typename From>
  VK_nodiscard inline decltype(auto) cast(From& from) noexcept {
    INVARIANT(isA<To>(from));
    return Casting::doConversion<To>(from);
  }
  template <typename To, typename From>
  VK_nodiscard inline decltype(auto) cast(From* from) noexcept {
    INVARIANT(isA<To>(from));
    return Casting::doConversion<To>(from);
  }
  template <typename To, typename From, typename Deleter>
  VK_nodiscard inline decltype(auto) cast(std::unique_ptr<From, Deleter>&& from) noexcept {
    INVARIANT(isA<To>(from));
    using ret_type = Casting::cast_return_t<To, std::unique_ptr<From, Deleter>>;
    return ret_type(cast<To>(from.release()), from.get_deleter());
    //return Casting::doConversion<To>(std::move(from));
  }

  template <typename To, Casting::WrappedType From>
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


  template <typename To, Casting::WrappedType From>
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

  template <typename To, Casting::WrappedType From>
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


















  template <typename To, Concepts::PointerFwdRef From>
  inline constexpr static decltype(auto) ptr_cast(From&& from) noexcept
  requires(requires{ { pointer_traits<std::remove_reference_t<From>>::template cast<To>(std::forward<From>(from)) }; }) {
    return pointer_traits<std::remove_reference_t<From>>::template cast<To>(std::forward<From>(from));
  }

  template <typename To, Concepts::PointerFwdRef From> requires(Concepts::AliasedBy<typename pointer_traits<std::remove_reference_t<From>>::element_type, To>)
  inline constexpr static auto alias_cast(From&& from) noexcept {
    return Traits::Alias<To, typename pointer_traits<std::remove_reference_t<From>>::element_type>::alias(std::forward<From>(from));
  }
  template <typename To, Concepts::PointerFwdRef From> requires(Concepts::AliasedBy<To, typename pointer_traits<std::remove_reference_t<From>>::element_type>)
  inline constexpr static auto reify_cast(From&& from) noexcept {
    return Traits::Alias<typename pointer_traits<std::remove_reference_t<From>>::element_type, To>::reify(std::forward<From>(from));
  }
  template <typename To, Concepts::BitCastableTo<To> From>
  inline constexpr static To bit_cast(From&& from) noexcept {
    return std::bit_cast<To>(from);
  }







  /*template <typename T, typename U>
  requires(std::convertible_to<T*, U*> && sizeof(T) == sizeof(U) && alignof(T) == alignof(U))
  struct Traits::Alias<T, U>{
    template <Concepts::PointerFwdRef<T> From>
    inline constexpr static auto alias(From&& pValue) noexcept{
      return ptr_cast<U>(std::forward<From>(pValue));
    }
    template <Concepts::PointerFwdRef<U> From>
    inline constexpr static auto reify(From&& pValue) noexcept{
      return ptr_cast<T>(std::forward<From>(pValue));
    }
  };

  namespace Detail{
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

  template <typename Byte, typename T> requires(Detail::IsByteAlias<Byte>::value && !std::is_const_v<T>)
  struct Traits::Alias<Byte, T>{

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

    template <Concepts::PointerFwdRef<T> From>
    inline constexpr static auto alias(From&& pValue) noexcept{
      return byte_ptr_traits_t<From>::toPointer(*ptr_cast<Byte>(ptr_traits_t<From>::toAddress(std::forward<From>(pValue))));
    }
    template <Concepts::PointerFwdRef<Byte> From>
    inline constexpr static auto reify(From&& pValue) noexcept{
      return reified_ptr_traits_t<From>::toPointer(*ptr_cast<T>(ptr_traits_t<From>::toAddress(std::forward<From>(pValue))));
    }
  };
  template <typename Byte, typename T>
  struct Traits::Alias<const Byte, T>{
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

    template <Concepts::PointerFwdRef<T> From>
    inline constexpr static auto alias(From&& pValue) noexcept{
      return byte_ptr_traits_t<From>::toPointer(*ptr_cast<const Byte>(ptr_traits_t<From>::toAddress(std::forward<From>(pValue))));
    }
    template <Concepts::PointerFwdRef<const Byte> From>
    inline constexpr static auto reify(From&& pValue) noexcept{
      return reified_ptr_traits_t<From>::toPointer(*ptr_cast<const T>(ptr_traits_t<From>::toAddress(std::forward<From>(pValue))));
    }
  };*/









}

#endif//VALKYRIE_CASTING_HPP
