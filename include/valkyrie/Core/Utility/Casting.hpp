//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_CASTING_HPP
#define VALKYRIE_CASTING_HPP

#include <valkyrie/Core/Traits.hpp>

namespace valkyrie::Core{
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

  template <typename T, typename U>
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
    struct IsByteAlias : std::disjunction<std::is_same<T, std::byte>, /*std::is_same<T, char>, */std::is_same<T, unsigned char>>{};

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
  };
}

#endif//VALKYRIE_CASTING_HPP
