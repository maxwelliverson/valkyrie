//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_BITFLAGS_HPP
#define VALKYRIE_BITFLAGS_HPP

#include <valkyrie/Core/Traits.hpp>

namespace valkyrie::Core{

  template </*Concepts::Enum*/ typename E>
  class BitFlags {
    using UnderlyingType = std::underlying_type_t<E>;

  public:
    constexpr BitFlags() noexcept = default;
    constexpr BitFlags(const BitFlags&) noexcept = default;
    constexpr BitFlags(BitFlags&&) noexcept = default;

    constexpr BitFlags(E enumValue) noexcept : m_bits_(static_cast<UnderlyingType>(enumValue)) {}
    constexpr explicit BitFlags(UnderlyingType intValue) noexcept : m_bits_(intValue){}


    constexpr BitFlags& operator=(const BitFlags&) noexcept = default;
    constexpr BitFlags& operator=(BitFlags&&) noexcept = default;

    constexpr void set(BitFlags other) noexcept {
      m_bits_ |= other.m_bits_;
    }
    constexpr void unset(BitFlags other) noexcept {
      m_bits_ &= ~other.m_bits_;
    }
    constexpr void assign(BitFlags other, bool condition){
      m_bits_ = (m_bits_ & ~other.m_bits_) | ((~(((std::make_unsigned_t<UnderlyingType>)condition) - 1)) & other.m_bits_);
    }
    constexpr bool test(BitFlags other) noexcept {
      return (m_bits_ & other.m_bits_) == other.m_bits_;
    }
    constexpr bool testAny(BitFlags other) noexcept {
      return m_bits_ & other.m_bits_;
    }

    constexpr BitFlags& operator&=(const BitFlags& Other) noexcept {
      m_bits_ &= Other.m_bits_;
      return *this;
    }
    constexpr BitFlags& operator|=(const BitFlags& Other) noexcept {
      m_bits_ |= Other.m_bits_;
      return *this;
    }
    constexpr BitFlags& operator^=(const BitFlags& Other) noexcept {
      m_bits_ ^= Other.m_bits_;
      return *this;
    }


    constexpr operator E() const noexcept { return static_cast<E>(m_bits_); }
    constexpr explicit operator UnderlyingType() const noexcept {
      return m_bits_;
    }
    constexpr explicit operator bool() const noexcept { return m_bits_; }


    friend constexpr BitFlags operator~(const BitFlags& Flags) noexcept { return BitFlags{ ~Flags.m_bits_ }; }
    friend constexpr BitFlags operator&(const BitFlags& A, const BitFlags& B) noexcept { return BitFlags{ A.m_bits_ & B.m_bits_ }; }
    friend constexpr BitFlags operator|(const BitFlags& A, const BitFlags& B) noexcept { return BitFlags{ A.m_bits_ | B.m_bits_ }; }
    friend constexpr BitFlags operator^(const BitFlags& A, const BitFlags& B) noexcept { return BitFlags{ A.m_bits_ ^ B.m_bits_ }; }


  private:
    UnderlyingType m_bits_;
  };
  
  template <typename Derived, Enum E>
  class CRTPBitFlags{
    template <typename, Enum>
    friend class CRTPBitFlags;

    inline constexpr Derived& derived() noexcept {
      return *static_cast<Derived*>(this);
    }
    inline constexpr const Derived& derived() const noexcept {
      return *static_cast<const Derived*>(this);
    }

    class Iterator;
    class Sentinel;
    class ConstSentinel;
    class ConstIterator;

    class Reference{

      friend class Iterator;
      friend class CRTPBitFlags;

      using underlying_type = typename enum_traits<E>::underlying_type;

      underlying_type&      bits;
      const underlying_type mask;

      constexpr Reference(underlying_type* pBits, underlying_type mask) noexcept
          : bits(*pBits), mask(mask){}

    public:

      Reference() = delete;
      Reference(const Reference&) = delete;


      constexpr Reference& operator=(const Reference& other) {
        set(bool(other));
        return *this;
      }
      constexpr Reference& operator=(bool B) noexcept {
        set(B);
        return *this;
      }

      constexpr void set() const noexcept{
        bits |= mask;
      }
      constexpr void unset() const noexcept {
        bits &= ~mask;
      }
      constexpr void set(bool B) const noexcept {
        bits = (~mask & bits) | (~(((underlying_type)B) - 1) & mask);
      }

      constexpr operator Derived() const noexcept {
        return Derived{ mask & bits };
      }
      constexpr explicit operator bool() const noexcept {
        return bool(mask & bits);
      }
    };

    class Sentinel{
      friend class CRTPBitFlags;
      friend class Iterator;
      friend class ConstIterator;

      using underlying_type = typename enum_traits<E>::underlying_type;

      underlying_type* pBits;

      constexpr Sentinel(underlying_type* pBits) noexcept : pBits(pBits){}
    };
    class ConstSentinel{
      friend class CRTPBitFlags;
      friend class Iterator;
      friend class ConstIterator;

      using underlying_type = typename enum_traits<E>::underlying_type;

      const underlying_type* pBits;

      constexpr ConstSentinel(const underlying_type* pBits) noexcept : pBits(pBits){}
    };

    class Iterator{
      friend class CRTPBitFlags;
      using underlying_type = typename enum_traits<E>::underlying_type;

      underlying_type* pBits;
      underlying_type  mask = *pBits;

      constexpr explicit Iterator(underlying_type* pBits) noexcept
          : pBits(pBits){}

    public:
      Iterator(std::nullptr_t) = delete;
      constexpr Iterator(const Iterator&) = default;

      constexpr Reference operator*() const noexcept {
        return {pBits, mask & ~(mask - 1)};
      }
      constexpr Iterator& operator++() noexcept {
        mask &= (mask - 1);
      }
      constexpr Iterator  operator++(int) noexcept {
        Iterator copy = *this;
        mask &= (mask - 1);
        return copy;
      }


      constexpr bool operator==(Sentinel) const noexcept {
        return !mask;
      }
      friend constexpr bool operator==(Iterator A, Iterator B) noexcept {
        return A.pBits == B.pBits && A.mask == B.mask;
      }

      constexpr std::partial_ordering operator<=>(Sentinel S) const noexcept {
        if (pBits != S.pBits)
          return std::partial_ordering::unordered;
        return static_cast<underlying_type>(0) <=> mask;
      }
      friend constexpr std::partial_ordering operator<=>(Iterator A, Iterator B) noexcept {
        if (A.pBits != B.pBits)
          return std::partial_ordering::unordered;
        return B.mask <=> A.mask;
      }
    };
    class ConstIterator{
      friend class CRTPBitFlags;
      using underlying_type = typename enum_traits<E>::underlying_type;

      const underlying_type* pBits;
      underlying_type  mask = *pBits;
    public:

    };


  public:

    using underlying_type = std::underlying_type_t<E>;
    using bit_type =        E;
    using flag_type =       Derived;

    using iterator = Iterator;
    using const_iterator = ConstIterator;
    using sentinel = Sentinel;
    using const_sentinel = ConstSentinel;
    using reference = Reference;
    using const_reference = flag_type;

    constexpr CRTPBitFlags() noexcept = default;
    constexpr CRTPBitFlags(const CRTPBitFlags&) noexcept = default;
    constexpr CRTPBitFlags(CRTPBitFlags&&) noexcept = default;

    constexpr CRTPBitFlags(E enumValue) noexcept : m_bits_(static_cast<underlying_type>(enumValue)) {}
    constexpr explicit CRTPBitFlags(underlying_type intValue) noexcept : m_bits_(intValue){}


    constexpr CRTPBitFlags& operator=(const CRTPBitFlags&) noexcept = default;
    constexpr CRTPBitFlags& operator=(CRTPBitFlags&&) noexcept = default;


    constexpr reference       operator[](CRTPBitFlags other) noexcept {
      return { &m_bits_, other.m_bits_ };
    }
    constexpr const_reference operator[](CRTPBitFlags other) const noexcept {
      return { m_bits_ & other.m_bits_ };
    }

    constexpr flag_type& operator&=(const CRTPBitFlags& Other) noexcept {
      m_bits_ &= Other.m_bits_;
      return *this;
    }
    constexpr flag_type& operator|=(const CRTPBitFlags& Other) noexcept {
      m_bits_ |= Other.m_bits_;
      return *this;
    }
    constexpr flag_type& operator^=(const CRTPBitFlags& Other) noexcept {
      m_bits_ ^= Other.m_bits_;
      return *this;
    }


    constexpr operator E() const noexcept { return static_cast<E>(m_bits_); }
    constexpr explicit operator underlying_type() const noexcept {
      return m_bits_;
    }
    constexpr explicit operator bool() const noexcept { return m_bits_; }


    constexpr iterator       begin()        noexcept {
      return { &m_bits_ };
    }
    constexpr const_iterator begin()  const noexcept {
      return { &m_bits_ };
    }
    constexpr const_iterator cbegin() const noexcept {
      return { &m_bits_ };
    }


    constexpr sentinel       end()        noexcept {
      return { &m_bits_ };
    }
    constexpr const_sentinel end()  const noexcept {
      return { &m_bits_ };
    }
    constexpr const_sentinel cend() const noexcept {
      return { &m_bits_ };
    }



    friend constexpr flag_type operator~(const CRTPBitFlags& Flags) noexcept {
      return flag_type{ ~Flags.m_bits_ };
    }
    friend constexpr flag_type operator&(const CRTPBitFlags& A, const CRTPBitFlags& B) noexcept { return flag_type{ A.m_bits_ & B.m_bits_ }; }
    friend constexpr flag_type operator|(const CRTPBitFlags& A, const CRTPBitFlags& B) noexcept { return flag_type{ A.m_bits_ | B.m_bits_ }; }
    friend constexpr flag_type operator^(const CRTPBitFlags& A, const CRTPBitFlags& B) noexcept { return flag_type{ A.m_bits_ ^ B.m_bits_ }; }

  protected:
    underlying_type m_bits_;
  };
}

#endif//VALKYRIE_BITFLAGS_HPP
