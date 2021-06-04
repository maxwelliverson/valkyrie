//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_BITFLAG_HPP
#define VALKYRIE_BITFLAG_HPP

#include <valkyrie/traits.hpp>
#include <valkyrie/adt/iterator.hpp>

namespace valkyrie{

  /*template <*//*concepts::enumerator*//* typename E>
  class bitflag {
    using underlying_type = std::underlying_type_t<E>;

  public:
    constexpr bitflag() noexcept = default;
    constexpr bitflag(const bitflag &) noexcept = default;
    constexpr bitflag(bitflag &&) noexcept = default;

    constexpr bitflag(E enumValue) noexcept : m_bits_(static_cast<underlying_type>(enumValue)) {}
    constexpr explicit bitflag(underlying_type intValue) noexcept : m_bits_(intValue){}


    constexpr bitflag & operator=(const bitflag &) noexcept = default;
    constexpr bitflag & operator=(bitflag &&) noexcept = default;

    constexpr void set(bitflag other) noexcept {
      m_bits_ |= other.m_bits_;
    }
    constexpr void unset(bitflag other) noexcept {
      m_bits_ &= ~other.m_bits_;
    }
    constexpr void assign(bitflag other, bool condition){
      m_bits_ = (m_bits_ & ~other.m_bits_) | ((~(((std::make_unsigned_t<underlying_type>)condition) - 1)) & other.m_bits_);
    }
    constexpr bool test(bitflag other) noexcept {
      return (m_bits_ & other.m_bits_) == other.m_bits_;
    }
    constexpr bool testAny(bitflag other) noexcept {
      return m_bits_ & other.m_bits_;
    }

    constexpr bitflag& operator&=(const bitflag & Other) noexcept {
      m_bits_ &= Other.m_bits_;
      return *this;
    }
    constexpr bitflag& operator|=(const bitflag & Other) noexcept {
      m_bits_ |= Other.m_bits_;
      return *this;
    }
    constexpr bitflag& operator^=(const bitflag & Other) noexcept {
      m_bits_ ^= Other.m_bits_;
      return *this;
    }


    constexpr operator E() const noexcept { return static_cast<E>(m_bits_); }
    constexpr explicit operator underlying_type() const noexcept {
      return m_bits_;
    }
    constexpr explicit operator bool() const noexcept { return m_bits_; }


    friend constexpr bitflag operator~(const bitflag & Flags) noexcept { return bitflag{ ~Flags.m_bits_ }; }
    friend constexpr bitflag operator&(const bitflag & A, const bitflag & B) noexcept { return bitflag{ A.m_bits_ & B.m_bits_ }; }
    friend constexpr bitflag operator|(const bitflag & A, const bitflag & B) noexcept { return bitflag{ A.m_bits_ | B.m_bits_ }; }
    friend constexpr bitflag operator^(const bitflag & A, const bitflag & B) noexcept { return bitflag{ A.m_bits_ ^ B.m_bits_ }; }


  private:
    underlying_type m_bits_;
  };*/


  namespace impl{
    /*template <enumerator_c E>
    class bitflag_iterator_base{
    public:

      using iterator_category = std::forward_iterator_tag;
      using value_type        = E;
      using difference_type   = i64;

    protected:

      explicit bitflag_iterator_base(underlying_type bits) noexcept : m_bits(bits){}

      using underlying_type = typename enum_traits<E>::underlying_type;
      using signed_int_type = std::make_signed_t<underlying_type>;
      using enum_type       = E;

      inline underlying_type get_low_bit() const noexcept {
        const underlying_type tmp_bits = m_bits & m_bitmask;
        return tmp_bits & static_cast<underlying_type>(-static_cast<signed_int_type>(tmp_bits));
      }

      underlying_type m_bits       = 0;
      underlying_type m_bitmask    = static_cast<underlying_type>(-1);
    };

    template <enumerator_c E>
    class bitflag_sentinel{
    public:
      
    };

    template <enumerator_c E>
    class bitflag_iterator : public bitflag_iterator_base<E>{
      using underlying_type = typename enum_traits<E>::underlying_type;
      using enum_type       = E;

      void inc() noexcept {
        const auto low_bit = this->get_low_bit();
        this->m_bitmask = (~(low_bit - 1)) << 1;
      }

    public:

      bitflag_iterator() = default;
      explicit bitflag_iterator(underlying_type bits) noexcept : m_bits(bits){}

      bitflag_iterator& operator++()    noexcept {
        inc();
        return *this;
      }
      bitflag_iterator  operator++(int) noexcept {
        bitflag_iterator tmp = *this;
        inc();
        return tmp;
      }

      enum_type operator*() const noexcept {
        return static_cast<enum_type>(this->get_low_bit());
      }
      
      friend bool operator!=(bitflag_iterator iter, bitflag_sentinel<E> sent) noexcept {
        return static_cast<bool>(iter.get_low_bit());
      }
    };*/

    template <typename E>
    class bitflag_iterator;
    template <typename E>
    class bitflag_const_iterator;
    template <typename E>
    class bitflag_sentinel;
    template <typename E>
    class bitflag_const_sentinel;

    template <typename E>
    class bitflag_reference{

      using underlying_type = typename enum_traits<E>::underlying_type;

      underlying_type&      bits;
      const underlying_type mask;

    public:

      bitflag_reference() = delete;
      bitflag_reference(const bitflag_reference&) = delete;

      constexpr bitflag_reference(underlying_type* pBits, underlying_type mask) noexcept
          : bits(*pBits), mask(mask){}


      constexpr bitflag_reference& operator=(const bitflag_reference& other) {
        set(bool(other));
        return *this;
      }
      constexpr bitflag_reference& operator=(bool B) noexcept {
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

      constexpr explicit operator bool() const noexcept {
        return bool(mask & bits);
      }

      constexpr operator bitflags<E>() const noexcept {
        return bitflags<E>{ mask & bits };
      }
    };

    template <typename E>
    class bitflag_const_sentinel{
      template <typename>
      friend class bitflag_const_iterator;

      using underlying_type = typename enum_traits<E>::underlying_type;

      const underlying_type bits;

    public:
      constexpr explicit bitflag_const_sentinel(const underlying_type bits) noexcept : bits(bits){}
    };
    template <typename E>
    class bitflag_sentinel{
      template <typename>
      friend class bitflag_iterator;

      using underlying_type = typename enum_traits<E>::underlying_type;

      underlying_type* pBits;

    public:
      constexpr explicit bitflag_sentinel(underlying_type* pBits) noexcept
          : pBits(pBits){}

      constexpr operator bitflag_const_sentinel<E>() const noexcept {
        return bitflag_const_sentinel<E>{*pBits};
      }
    };


    template <typename E>
    class bitflag_const_iterator{
      template <typename>
      friend class valkyrie::bitflags;
      using underlying_type = typename enum_traits<E>::underlying_type;
      using enum_type       = E;

      underlying_type bits;
      underlying_type mask = bits;

    public:

      bitflag_const_iterator() = default;

      bitflag_const_iterator(std::nullptr_t) = delete;
      constexpr bitflag_const_iterator(const bitflag_const_iterator&) = default;

      constexpr explicit bitflag_const_iterator(underlying_type bits) noexcept
          : bits(bits){}

      constexpr enum_type operator*() const noexcept {
        return bits & (mask & -mask);
      }
      constexpr bitflag_const_iterator& operator++() noexcept {
        mask &= (mask - 1);
        return *this;
      }
      constexpr bitflag_const_iterator  operator++(int) noexcept {
        bitflag_const_iterator copy = *this;
        mask &= (mask - 1);
        return copy;
      }


      constexpr bool operator==(bitflag_const_sentinel<E>) const noexcept {
        return !mask;
      }
      friend constexpr bool operator==(bitflag_const_iterator A, bitflag_const_iterator B) noexcept {
        return A.bits == B.bits && A.mask == B.mask;
      }

      constexpr std::partial_ordering operator<=>(bitflag_const_sentinel<E> S) const noexcept {
        if (bits != S.bits) VK_unlikely
          return std::partial_ordering::unordered;
        return static_cast<underlying_type>(0) <=> mask;
      }
      friend constexpr std::partial_ordering operator<=>(bitflag_const_iterator A, bitflag_const_iterator B) noexcept {
        if (A.bits != B.bits) VK_unlikely
          return std::partial_ordering::unordered;
        return B.mask <=> A.mask;
      }
    };
    template <typename E>
    class bitflag_iterator{
      template <typename>
      friend class valkyrie::bitflags;
      using underlying_type = typename enum_traits<E>::underlying_type;

      underlying_type* pBits;
      underlying_type  mask = *pBits;

    public:

      bitflag_iterator() = default;

      bitflag_iterator(std::nullptr_t) = delete;
      constexpr bitflag_iterator(const bitflag_iterator&) = default;

      constexpr explicit bitflag_iterator(underlying_type* pBits) noexcept
          : pBits(pBits){}

      constexpr bitflag_reference<E> operator*() const noexcept {
        return { pBits, mask & -mask };
      }
      constexpr bitflag_iterator& operator++() noexcept {
        mask &= (mask - 1);
        return *this;
      }
      constexpr bitflag_iterator  operator++(int) noexcept {
        bitflag_iterator copy = *this;
        mask &= (mask - 1);
        return copy;
      }


      constexpr operator bitflag_const_iterator<E>() const noexcept {
        return bitflag_const_iterator<E>{ *pBits };
      }


      constexpr bool operator==(bitflag_sentinel<E>) const noexcept {
        return !mask;
      }
      friend constexpr bool operator==(bitflag_iterator A, bitflag_iterator B) noexcept {
        return A.pBits == B.pBits && A.mask == B.mask;
      }

      constexpr std::partial_ordering operator<=>(bitflag_sentinel<E> S) const noexcept {
        if (pBits != S.pBits)
          return std::partial_ordering::unordered;
        return static_cast<underlying_type>(0) <=> mask;
      }
      friend constexpr std::partial_ordering operator<=>(bitflag_iterator A, bitflag_iterator B) noexcept {
        if (A.pBits != B.pBits)
          return std::partial_ordering::unordered;
        return B.mask <=> A.mask;
      }
    };
  }

  template <enumerator_c E>
  class bitflags<E>{
    template <typename>
    friend class bitflags;

  public:

    using underlying_type = typename enum_traits<E>::underlying_type;
    using bit_type =        E;
    using flag_type =       bitflags;

    using iterator        = impl::bitflag_iterator<bit_type>;
    using const_iterator  = impl::bitflag_const_iterator<bit_type>;
    using sentinel        = impl::bitflag_sentinel<bit_type>;
    using const_sentinel  = impl::bitflag_const_sentinel<bit_type>;
    using reference       = impl::bitflag_reference<bit_type>;
    using const_reference = flag_type;

    constexpr bitflags() noexcept = default;
    constexpr bitflags(const bitflags&) noexcept = default;
    constexpr bitflags(bitflags&&) noexcept = default;
    constexpr bitflags& operator=(const bitflags&) noexcept = default;
    constexpr bitflags& operator=(bitflags&&) noexcept = default;

    constexpr bitflags(bit_type bit) noexcept : m_bits_(static_cast<underlying_type>(bit)) {}
    constexpr explicit bitflags(underlying_type flags) noexcept : m_bits_(flags){}



    constexpr void set(bitflags other) noexcept {
      m_bits_ |= other.m_bits_;
    }
    constexpr void unset(bitflags other) noexcept {
      m_bits_ &= ~other.m_bits_;
    }
    constexpr void assign(bitflags other, bool condition){
      m_bits_ = (m_bits_ & ~other.m_bits_) | ((~(((std::make_unsigned_t<underlying_type>)condition) - 1)) & other.m_bits_);
    }
    constexpr bool test_all(bitflags other) noexcept {
      return (m_bits_ & other.m_bits_) == other.m_bits_;
    }
    constexpr bool test_any(bitflags other) noexcept {
      return m_bits_ & other.m_bits_;
    }



    constexpr reference       operator[](bitflags other)       noexcept {
      return { &m_bits_, other.m_bits_ };
    }
    constexpr const_reference operator[](bitflags other) const noexcept {
      return { m_bits_ & other.m_bits_ };
    }

    constexpr flag_type& operator&=(bitflags other) noexcept {
      m_bits_ &= other.m_bits_;
      return *this;
    }
    constexpr flag_type& operator|=(bitflags other) noexcept {
      m_bits_ |= other.m_bits_;
      return *this;
    }
    constexpr flag_type& operator^=(bitflags other) noexcept {
      m_bits_ ^= other.m_bits_;
      return *this;
    }


    constexpr operator bit_type() const noexcept { return static_cast<bit_type>(m_bits_); }
    constexpr explicit operator underlying_type() const noexcept {
      return m_bits_;
    }
    constexpr explicit operator bool() const noexcept { return m_bits_; }


    constexpr iterator       begin()        noexcept {
      return { &m_bits_ };
    }
    constexpr const_iterator begin()  const noexcept {
      return { m_bits_ };
    }
    constexpr const_iterator cbegin() const noexcept {
      return { m_bits_ };
    }


    constexpr sentinel       end()        noexcept {
      return { &m_bits_ };
    }
    constexpr const_sentinel end()  const noexcept {
      return { m_bits_ };
    }
    constexpr const_sentinel cend() const noexcept {
      return { m_bits_ };
    }



    friend constexpr flag_type operator~(const bitflags Flags) noexcept {
      return flag_type{ ~Flags.m_bits_ };
    }
    friend constexpr flag_type operator&(const bitflags A, const bitflags B) noexcept { return flag_type{ A.m_bits_ & B.m_bits_ }; }
    friend constexpr flag_type operator|(const bitflags A, const bitflags B) noexcept { return flag_type{ A.m_bits_ | B.m_bits_ }; }
    friend constexpr flag_type operator^(const bitflags A, const bitflags B) noexcept { return flag_type{ A.m_bits_ ^ B.m_bits_ }; }

  protected:
    underlying_type m_bits_;
  };
}

#endif//VALKYRIE_BITFLAG_HPP
