//
// Created by Maxwell on 2020-11-30.
//

#ifndef VALKYRIE_MEMORY_ADDRESS_HPP
#define VALKYRIE_MEMORY_ADDRESS_HPP

#include <valkyrie/utility/shapes.hpp>
#include <valkyrie/status/result.hpp>
#include <valkyrie/utility/bitflag.hpp>

namespace valkyrie{

  class Agent;

  class AddressSpace;
  class SystemAddressSpace;


  template <typename Domain>
  class MemoryAddress;
  template <typename Domain>
  class MemoryBuffer;

  using Address = MemoryAddress<erased<u64>>;
  using Buffer  = MemoryBuffer<erased<u64>>;

  namespace detail{
    enum class MemoryAddressBits{
      ReadAccess  = 0x1,
      WriteAccess = 0x2,
      DeviceLocal = 0x4,
      Managed     = 0x8,
      Mapped      = 0x10,
      Virtual     = 0x20,
      Physical    = 0x40,
      Null        = 0x80
    };
  }

  using MemoryAddressFlags = BitFlags<detail::MemoryAddressBits>;

  namespace Memory{
    inline constexpr static MemoryAddressFlags ReadAccess { detail::MemoryAddressBits::ReadAccess  };
    inline constexpr static MemoryAddressFlags WriteAccess{ detail::MemoryAddressBits::WriteAccess };
    inline constexpr static MemoryAddressFlags DeviceLocal{ detail::MemoryAddressBits::DeviceLocal };
    inline constexpr static MemoryAddressFlags Managed    { detail::MemoryAddressBits::Managed     };
    inline constexpr static MemoryAddressFlags Mapped     { detail::MemoryAddressBits::Mapped      };
    inline constexpr static MemoryAddressFlags Virtual    { detail::MemoryAddressBits::Virtual     };
    inline constexpr static MemoryAddressFlags Physical   { detail::MemoryAddressBits::Physical    };
    inline constexpr static MemoryAddressFlags Null       { detail::MemoryAddressBits::Null        };
    inline constexpr static MemoryAddressFlags ReadWriteAccess = ReadAccess | WriteAccess;
  }



  class AddressSpace{
    template <typename> friend class MemoryAddress;
    template <typename> friend class MemoryBuffer;
  public:
    using value_type      = void*;
    using difference_type = i64; // ie. index_type
    using size_type       = u64; // ie. extent_type
    using status_type     = status;

  protected:

    using       address_ref    = MemoryAddress<void>&;
    using const_address_ref    = const MemoryAddress<void>&;
    using       extent_ref     = void*;
    using const_extent_ref     = const void*;
    using       difference_ref = void*;
    using const_difference_ref = const void*;
    using       status_ref     = status_code<void>&;
    using const_status_ref     = const status_code<void>&;


    // [ operations ]

    virtual void doMemCopy(status_ref result, address_ref dst, const_address_ref src, const_extent_ref extent) const noexcept = 0;
    virtual void doMemMove(status_ref result, address_ref dst, const_address_ref src, const_extent_ref extent) const noexcept = 0;
    virtual void doMemSet( status_ref result, address_ref dst, byte value,            const_extent_ref extent) const noexcept = 0;

    virtual void doGetDifference(status_ref result, difference_ref diff, const_address_ref addressA, const_address_ref addressB) const noexcept = 0;
    virtual void doIndex(status_ref result, address_ref dst, const_address_ref src, const_difference_ref offset) const noexcept = 0;
    virtual std::partial_ordering doCompare(const_address_ref addrA, const_address_ref addrB) const noexcept = 0;


    // [ erased operations ]

    virtual void doErasedDestroy(status_ref result, address_ref addr, size_t bytes) const noexcept { }
    virtual void doErasedCopy(status_ref result, address_ref addrDst, const_address_ref addrSrc, size_t bytes) const noexcept { }


    // [ property queries ]

    virtual StringView   doGetName() const noexcept = 0;
    virtual const uuid&        doGetUuid() const noexcept = 0;
    virtual u64                doGetPointerBits(const_address_ref addr) const noexcept = 0;
    virtual MemoryAddressFlags doGetFlags(const_address_ref address, const Agent* pAgent) const noexcept = 0;
    virtual void               doGetBudget(status_ref result, extent_ref extent) const noexcept = 0;
    virtual void               doGetNullPointer(address_ref nullAddr) const noexcept = 0;



  public:

    VK_nodiscard StringView name() const noexcept {
      return doGetName();
    }
    VK_nodiscard const uuid&      id()   const noexcept {
      return doGetUuid();
    }

    friend bool operator==(const AddressSpace& A, const AddressSpace& B) noexcept {
      return A.id() == B.id();
    }
  };

  namespace detail{
    class NullAddressSpace : public AddressSpace{
    public:

      inline constexpr static uuid class_id{"a0e47cda-193a-4d65-b33b-f74139dcc098"};

      using value_type = std::nullptr_t;
      using status_type = generic_status;


      StringView doGetName() const noexcept override {
        return VK_string("Null Address Space");
      }
      const uuid & doGetUuid() const noexcept override {
        return class_id;
      }

      void doMemSet(status_ref result, address_ref dst, byte value, const_extent_ref extent) const noexcept override;
      void doMemMove(status_ref result, address_ref dst, const_address_ref src, const_extent_ref extent) const noexcept override;
      void doMemCopy(status_ref result, address_ref dst, const_address_ref src, const_extent_ref extent) const noexcept override;
      void doGetDifference(status_ref result, difference_ref diff, const_address_ref addressA, const_address_ref addressB) const noexcept override;
      u64  doGetPointerBits(const_address_ref addr) const noexcept override;
      void doIndex(status_ref result, address_ref dst, const_address_ref src, const_difference_ref offset) const noexcept override;
      void doGetNullPointer(address_ref nullAddr) const noexcept override;
      void doGetBudget(status_ref result, extent_ref extent) const noexcept override;
      MemoryAddressFlags doGetFlags(const_address_ref address, const Agent* pAgentID) const noexcept override;
      std::partial_ordering doCompare(const_address_ref addrA, const_address_ref addrB) const noexcept override;
    };
    const NullAddressSpace& getNullAddressSpace() noexcept;
    inline static const NullAddressSpace* getDefaultAddressSpace() noexcept {
      return &getNullAddressSpace();
    }
  }

  template <>
  class MemoryAddress<void>{
  protected:
    const AddressSpace* pAddressSpace = detail::getDefaultAddressSpace();

    MemoryAddress() = default;
    MemoryAddress(const MemoryAddress&) = default;
    MemoryAddress(MemoryAddress&&) noexcept = default;
    MemoryAddress& operator=(const MemoryAddress&) = default;
    MemoryAddress& operator=(MemoryAddress&&) noexcept = default;
    ~MemoryAddress() = default;

    MemoryAddress(const AddressSpace* pAddressSpace) noexcept : pAddressSpace(pAddressSpace){}

  public:

    VK_nodiscard MemoryAddressFlags flags() const noexcept {
      return pAddressSpace->doGetFlags(*this, nullptr);
    }
    VK_nodiscard MemoryAddressFlags flags(const Agent& agent) const noexcept {
      return pAddressSpace->doGetFlags(*this, &agent);
    }

    VK_nodiscard bool canBeReadFrom() const noexcept {
      return flags().test(Memory::ReadAccess);
    }
    VK_nodiscard bool canBeWrittenTo() const noexcept {
      return flags().test(Memory::WriteAccess);
    }
    VK_nodiscard bool canBeReadFromBy(const Agent& agent) const noexcept {
      return flags(agent).test(Memory::ReadAccess);
    }
    VK_nodiscard bool canBeWrittenToBy(const Agent& agent) const noexcept {
      return flags(agent).test(Memory::WriteAccess);
    }

    VK_nodiscard const AddressSpace& addressSpace() const noexcept {
      return *pAddressSpace;
    }

    VK_nodiscard u64                 rawBits() const noexcept {
      return pAddressSpace->doGetPointerBits(*this);
    }

    template <typename Dom>
    VK_nodiscard std::partial_ordering compare(const MemoryAddress<Dom>& other) const noexcept{
      return pAddressSpace->doCompare(*this, other);
    }

    VK_nodiscard explicit operator bool() const noexcept {
      return !pAddressSpace->doGetFlags(*this, nullptr).test(Memory::Null);
    }


    friend bool operator==(const MemoryAddress& A, std::nullptr_t) noexcept{
      return A.flags().test(Memory::Null);
    }
    friend bool operator==(const MemoryAddress& A, const MemoryAddress& B) noexcept {
      return A.compare(B) == std::partial_ordering::equivalent;
    }
    friend std::partial_ordering operator<=>(const MemoryAddress& A, const MemoryAddress& B) noexcept {
      return A.compare(B);
    }
  };


  namespace detail{

    template <typename ErasedValueType, typename ErasedSizeType, typename ErasedDifferenceType, typename ErasedStatusType>
    struct ErasedSpace;


    template <typename Domain>
    struct GetAddressSpaceTypedefs;

    template <typename ErasedValueType, typename ErasedSizeType, typename ErasedDifferenceType, typename ErasedStatusType>
    struct GetAddressSpaceTypedefs<ErasedSpace<ErasedValueType, ErasedSizeType, ErasedDifferenceType, ErasedStatusType>>{
      using address_space_type = AddressSpace;
      using value_type         = ErasedSizeType;
      using size_type          = ErasedSizeType;
      using difference_type    = ErasedDifferenceType;
      using status_type        = ErasedStatusType;
    };

    template <typename Domain>
    struct GetAddressSpaceTypedefs{
      using address_space_type = Domain;
      using value_type = typename address_space_type::value_type;
      using status_type = typename address_space_type::status_type;
      using size_type = typename address_space_type::size_type;
      using difference_type = typename address_space_type::difference_type;
    };



    template <typename Domain>
    class MemoryAddressStorage : public MemoryAddress<void>{
      using types = GetAddressSpaceTypedefs<Domain>;
      using base = MemoryAddress<void>;
    public:

      using address_space_type = typename types::address_space_type;
      using value_type = typename types::value_type;
      using size_type  = typename types::size_type;
      using difference_type = typename types::difference_type;
      using status_type = typename types::status_type;


      value_type& value() & noexcept {
        return value_;
      }
      const value_type& value() const & noexcept {
        return value_;
      }
      value_type&& value() && noexcept {
        return std::move(value_);
      }
      const value_type&& value() const && noexcept {
        return std::move(value_);
      }


      const address_space_type& addressSpace() const noexcept {
        return static_cast<const address_space_type&>(MemoryAddress<void>::addressSpace());
      }





    protected:
      MemoryAddressStorage() = default;
      MemoryAddressStorage(const MemoryAddressStorage&) = default;
      MemoryAddressStorage(MemoryAddressStorage&& other) noexcept
          : base(other.pAddressSpace), value_(std::move(other.value_)){
        other.pAddressSpace = detail::getDefaultAddressSpace();
      }
      MemoryAddressStorage& operator=(const MemoryAddressStorage&) = default;
      MemoryAddressStorage& operator=(MemoryAddressStorage&& other) noexcept {
        this->~MemoryAddressStorage();
        new(this) MemoryAddressStorage(std::move(other));
        return *this;
      }

      template <typename ...Args> requires(ConstructibleFrom<value_type, Args...>)
      explicit MemoryAddressStorage(std::in_place_t, const address_space_type* pAddressSpace, Args&& ...args) noexcept
          : base(pAddressSpace), value_{ std::forward<Args>(args)... }{}

      ~MemoryAddressStorage() = default;

      value_type value_;
    };

    template <typename Domain>
    struct GetMemoryAddressBase{
      using type = MemoryAddressStorage<Domain>;
    };
    template <typename Domain>
    requires(requires{ typename Domain::template mixin<MemoryAddressStorage<Domain>>; } &&
             std::derived_from<typename Domain::template mixin<MemoryAddressStorage<Domain>>, MemoryAddressStorage<Domain>>)
    struct GetMemoryAddressBase<Domain>{
      using type = typename Domain::template mixin<MemoryAddressStorage<Domain>>;
    };

    template <typename T>
    concept ConstantAddressSpace = requires{ { T::get() } -> std::convertible_to<const T&>; };
  }

  template <typename Domain>
  class MemoryAddress : public detail::GetMemoryAddressBase<Domain>::type{

    using Base_ = typename detail::GetMemoryAddressBase<Domain>::type;
    using ThisType = MemoryAddress;

  protected:

    using Base_::Base_;

    inline constexpr static bool ConstantAddressSpace = detail::ConstantAddressSpace<typename Base_::address_space_type>;

    template <typename T>
    using result = Maybe<T, typename Base_::status_type::domain_type>;

  public:
    using address_space_type = typename Base_::address_space_type;
    using value_type = typename Base_::value_type;
    using size_type = typename Base_::size_type;
    using difference_type = typename Base_::difference_type;
    using status_type = typename Base_::status_type;



    MemoryAddress() = default;
    MemoryAddress(const MemoryAddress&) = default;
    MemoryAddress(MemoryAddress&&) noexcept = default;
    MemoryAddress& operator=(const MemoryAddress&) = default;
    MemoryAddress& operator=(MemoryAddress&&) noexcept = default;
    ~MemoryAddress() = default;



    MemoryAddress clone() const {
      return *this;
    }



    template <not_same_as_one_of<ThisType, std::in_place_t> T, typename ...Args>
    MemoryAddress(T&& t, Args&& ...args) noexcept(noexcept(makeAddress((T&&)t, (Args&&)args...)))
    requires(requires{ { makeAddress((T&&)t, (Args&&)args...) } -> std::convertible_to<ThisType>; })
        : MemoryAddress(makeAddress(std::forward<T>(t), std::forward<Args>(args)...)) { }

    explicit MemoryAddress(const value_type& value)
        noexcept(std::is_nothrow_copy_constructible_v<value_type>)
        requires(ConstantAddressSpace && std::copy_constructible<value_type>)
        : Base_{ std::in_place,  address_space_type::get(), value }{}
    explicit MemoryAddress(value_type&& value)
        noexcept(std::is_nothrow_move_constructible_v<value_type>)
        requires(ConstantAddressSpace && std::move_constructible<value_type>)
        : Base_{ std::in_place,  address_space_type::get(), std::move(value) }{}


    template <detail::safely_erasable_from<value_type> V, typename S, typename D, typename Stat>
    explicit MemoryAddress(const MemoryAddress<detail::ErasedSpace<V, S, D, Stat>>& erasedSpace)
        noexcept(std::is_nothrow_copy_constructible_v<value_type>)
        requires(ConstructibleFrom<ThisType, value_type>) {}


    template <typename Other>
    result<difference_type> distance(const MemoryAddress<Other>& other) const noexcept {
      status_type result;
      difference_type value;
      this->pAddressSpace->doGetDifference(result, std::addressof(value), *this, other);
      if (result.failure())
        return std::move(result);
      return std::move(value);
    }
  };

  template <typename Domain>
  class MemoryBuffer{
  public:
    using value_type      = typename Domain::value_type;
    using size_type       = typename Domain::size_type;
    using difference_type = typename Domain::difference_type;
    using status_type     = typename Domain::status_type;





  private:
    MemoryAddress<Domain> address_;
    size_type             size_;
  };


  class SystemAddressSpace : public AddressSpace{
  public:

    inline constexpr static uuid class_id{"6cc65ad7-e56b-48a2-9562-6f7cb860a656"};


    using status_type = generic_status;
    using address_type = MemoryAddress<SystemAddressSpace>;

    StringView   doGetName() const noexcept override {
      return VK_string("system Memory (Default)");
    }
    const uuid&        doGetUuid() const noexcept override {
      return class_id;
    }

    void doMemCopy(status_ref result, address_ref dst, const_address_ref src, const_extent_ref extent) const noexcept override {
      VK_assert(dst.addressSpace() == *this);
      if (src.addressSpace() == *this) {
        std::memcpy(static_cast<address_type &>(dst).value(), static_cast<const address_type&>(src).value(), *static_cast<const size_type*>(extent));
        static_cast<status_type&>(result) = code::success;
      }
      //|| src.canBeReadFrom()
      else {
        static_cast<status_type&>(result) = code::invalid_argument;
      }
    }
    void doMemMove(status_ref result, address_ref dst, const_address_ref src, const_extent_ref extent) const noexcept override {
      VK_assert(dst.addressSpace() == *this);
      if (src.addressSpace() == *this) {
        std::memmove(static_cast<address_type &>(dst).value(), static_cast<const address_type&>(src).value(), *static_cast<const size_type*>(extent));
        static_cast<status_type&>(result) = code::success;
      }
        //|| src.canBeReadFrom()
      else {
        static_cast<status_type&>(result) = code::invalid_argument;
      }
    }
    void doMemSet(status_ref result, address_ref dst, byte value, const_extent_ref extent) const noexcept override {
      VK_assert(dst.addressSpace() == *this);
      std::memset(static_cast<address_type &>(dst).value(), std::to_integer<int>(value), *static_cast<const size_type*>(extent));
      static_cast<status_type&>(result) = code::success;
    }

    u64 doGetPointerBits(const_address_ref addr) const noexcept override{
      return reinterpret_cast<u64>(static_cast<const address_type&>(addr).value());
    }

    void doGetDifference(status_ref result, difference_ref diff, const_address_ref addressA, const_address_ref addressB) const noexcept override {
      VK_assert(addressA.addressSpace() == *this);
      if (addressB.addressSpace() == *this) {
        *static_cast<difference_type*>(diff) =
            (const byte*)static_cast<const address_type&>(addressB).value() -
            (const byte*)static_cast<const address_type&>(addressA).value();
        static_cast<status_type&>(result) = code::success;
      }
      else if (addressB.flags().test(Memory::DeviceLocal | Memory::Mapped)){
        *static_cast<difference_type*>(diff) = addressB.rawBits() - this->doGetPointerBits(addressA);
        static_cast<status_type&>(result) = code::success;
      }
      else
        static_cast<status_type&>(result) = code::invalid_argument;
    }

    void                  doIndex(status_ref result, address_ref dst, const_address_ref src, const_difference_ref offset) const noexcept override {
      VK_assert(src.addressSpace() == *this);
      VK_assert(dst.addressSpace() == *this);
      static_cast<address_type&>(dst).value() = (void*)(this->doGetPointerBits(src) + *static_cast<const difference_type*>(offset));
      static_cast<status_type&>(result) = code::success;
    }
    std::partial_ordering doCompare(const_address_ref addressA, const_address_ref addressB) const noexcept override {
      VK_axiom(addressA.addressSpace() == *this);
      if (addressB.flags().test(Memory::DeviceLocal | Memory::Mapped))
        return this->doGetPointerBits(addressA) <=> addressB.rawBits();
      return std::partial_ordering::unordered;
    }

    void doGetBudget(status_ref result, extent_ref extent) const noexcept override;
    void doGetNullPointer(address_ref nullAddr) const noexcept override {
      VK_assert(nullAddr.addressSpace() == *this);
      static_cast<address_type&>(nullAddr).value() = nullptr;
    }
    MemoryAddressFlags doGetFlags(const_address_ref address, const Agent *pAgent) const noexcept override;
  };

  /*template <>
  class MemoryBuffer<void>{
  protected:
    const AddressSpace* pAddressSpace;


  };*/
  /*template <typename ErasedType>
  class MemoryAddress<erased<ErasedType>>{

  };*/
  /*template <typename ErasedType>
  class MemoryBuffer<erased<ErasedType>>{

  };*/
}

#endif//VALKYRIE_MEMORY_ADDRESS_HPP
