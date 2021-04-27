//
// Created by Maxwell on 2021-01-18.
//

#ifndef VALKYRIE_CORE_ADT_HANDLE_HPP
#define VALKYRIE_CORE_ADT_HANDLE_HPP

#include <valkyrie/async/atomic.hpp>
#include <valkyrie/status/generic_code.hpp>
#include <valkyrie/traits.hpp>


namespace valkyrie{
  inline namespace concepts{
    /*template <typename H, typename T>
    concept Handle = requires(){};*/

    template <typename T>
    concept Trivial =
        std::is_trivially_copyable_v<T> &&
        std::is_trivially_destructible_v<T>;


    template <typename O, typename T>
    concept HandleOwnershipPolicy = requires(O& o, T pointer){
      { O::OwnsObject }      -> std::convertible_to<bool>;
      { O::IsCopyable }      -> std::convertible_to<bool>;
      { o.acquire(pointer) } -> std::convertible_to<T>;
      { o.release(pointer) } -> std::convertible_to<bool>;
    };
    template <typename D, typename T>
    concept HandleDestructionPolicy = requires(D& d, T pointer){
      { d.destroy(pointer) } noexcept;
    };
    template <typename N, typename T>
    concept HandleNullabilityPolicy = requires(N& n, T pointer){
      { N::IsNullable } -> std::convertible_to<bool>;
    } && (!N::IsNullable || requires(N& n, T pointer){
      { N::template NullHandle<T> } -> std::convertible_to<T>;
      { n.isNull(pointer) } -> std::convertible_to<bool>;
    });
  }

  struct EmptyBase{};
  template <typename T, typename Base = EmptyBase>
  class IntrusiveReferenceCount : public Base{
    mutable atomic<u32> refCount = 0;
  public:
    using Base::Base;

    T* incRefCount() const noexcept {
      ++refCount;
      return static_cast<T*>(const_cast<IntrusiveReferenceCount*>(this));
    }
    bool decRefCount() const noexcept {
      return !--refCount;
    }
  };

  class Object{
  public:


    constexpr virtual Object* acquire() const noexcept = 0;
    constexpr virtual bool    release() const noexcept = 0;


    virtual ~Object() = default;
  };

  struct SharedObjectControlBlock{
    atomic<u32> sharedCount;
    atomic<u32> weakCount;
    void* object;
    void(* const pDelete)(void* pObject, void* pUserData) noexcept;
    void*  const pUserData;
  };
}

namespace valkyrie::Policies{

  struct StrongRefCountedOwnership{
    VK_constant bool OwnsObject = true;
    VK_constant bool IsCopyable = true;
    template <typename T, typename Base>
    inline T*   acquire(IntrusiveReferenceCount<T, Base>* pointer) const noexcept {
      return pointer->incRefCount();
    }
    template <std::derived_from<Object> T>
    inline T*   acquire(T* pointer) const noexcept {
      return static_cast<T*>(pointer->acquire());
    }
    template <typename T, typename Base>
    inline bool release(IntrusiveReferenceCount<T, Base>* pointer) const noexcept {
      return pointer->decRefCount();
    }
    template <std::derived_from<Object> T>
    inline bool release(T* pointer) const noexcept {
      return pointer->release();
    }
  };
  struct UniqueOwnership{
    VK_constant bool OwnsObject = true;
    VK_constant bool IsCopyable = false;
    template <typename T>
    inline T*   acquire(T* pointer) const noexcept {
      return pointer;
    }
    inline bool release(const void* pointer) const noexcept {
      return true;
    }
  };
  struct BorrowedOwnership{
    VK_constant bool OwnsObject = false;
    VK_constant bool IsCopyable = true;
    template <typename T>
    inline T*   acquire(T* pointer) const noexcept {
      return pointer;
    }
    inline bool release(const void* pointer) const noexcept {
      return false;
    }
  };

  template <typename Alloc>
  struct AllocatorDestroy{
    [[no_unique_address]] Alloc allocator;

    template <typename ...Args> requires(ConstructibleFrom<Alloc, Args...>)
    AllocatorDestroy(Args&& ...args) noexcept : allocator(std::forward<Args>(args)...){}

    using pointer = typename std::allocator_traits<Alloc>::pointer;
    using value_type = std::remove_cv_t<typename std::pointer_traits<pointer>::element_type>;

    inline void destroy(pointer p) const noexcept{
      if (p) std::allocator_traits<Alloc>::destroy(allocator, p);
    }
  };

  struct DoNotDestroy{
    template <typename T>
    inline void destroy(T&&) const noexcept{}
  };
  struct DefaultDelete{
    template <typename T>
    inline void destroy(const T* pObject) const noexcept{
      delete pObject;
    }
  };
  struct DefaultArrayDelete{
    template <typename T>
    inline void destroy(const T* pObject) const noexcept {
      delete[] pObject;
    }
  };
  struct MethodDestroy{
    template <typename T>
    inline void destroy(T&& object) const noexcept {
      std::forward<T>(object).destroy();
    }
  };
  struct FreeFunctionDestroy{
  private:
    // WORKAROUND: If destroy(...) is implemented as a member function like normal,
    // lookup for the unscoped forward call to destroy(obj) will find the member
    // function, causing an infinite loop. Having destroy be a static member object
    // allows the exact same method call syntax to be used, but the forwarding call
    // to destroy will not find itself.
    struct IndirectDestroyer{
      template <typename T>
      inline void operator()(T&& object) const noexcept {
        destroy(std::forward<T>(object));
      }
    };
  public:
    inline constexpr static IndirectDestroyer destroy{};
  };

  struct NonNullable{
    VK_constant bool IsNullable = false;
    template <typename T>
    inline constexpr static T NullHandle{};
    inline static bool isNull(const void* pObject) noexcept {
      return !pObject;
    }
  };
  struct Nullable{
    VK_constant bool IsNullable = true;
    template <typename T>
    inline constexpr static T NullHandle{};
    inline static bool isNull(const void* pObject) noexcept {
      return !pObject;
    }
  };
}


namespace valkyrie{

  template <Trivial                    T,
            HandleOwnershipPolicy<T>   Ownership,
            HandleDestructionPolicy<T> Destruction,
            HandleNullabilityPolicy<T> Nullability>
  class BasicHandle{

  public:

    BasicHandle() requires(Nullability::IsNullable) = default;
    BasicHandle(std::nullptr_t) requires(Nullability::IsNullable)
        : ownership(),
          destructor(),
          nullability(),
          handle(nullability.NullHandle){
      VK_assert(nullability.isNull(handle));
    }

    BasicHandle(T handle, Ownership ownership = {}, Destruction destruction = {}, Nullability nullability = {}) noexcept
        : ownership(std::move(ownership)),
          destructor(std::move(destruction)),
          nullability(std::move(nullability)),
          handle(this->ownership.acquire(handle)){
      if constexpr (!Nullability::IsNullable) {
        if (this->nullability.isNull(this->handle))
          panic(generic_error(code::failed_assertion), VK_string("NonNullable Handle was initialized with a null handle"));
      }
    }


    BasicHandle(const BasicHandle& other) requires(Ownership::IsCopyable)
        : ownership(other.ownership),
          destructor(other.destructor),
          nullability(other.nullability),
          handle(ownership.acquire(other.handle)){}
    BasicHandle(BasicHandle&& other) noexcept
        : ownership(std::move(other.ownership)),
          destructor(std::move(other.destructor)),
          nullability(std::move(other.nullability)),
          handle(std::move(other.handle)){
      other.handle = nullability.NullHandle;
    }

    ~BasicHandle() {
      if (ownership.release(handle))
        destructor.destroy(handle);
    }

    BasicHandle& operator=(const BasicHandle& other) requires(Ownership::IsCopyable) {

      if (&other != this) {
        this->~BasicHandle();
        new (this) BasicHandle(other);
      }
      return *this;
    }
    BasicHandle& operator=(BasicHandle&& other) noexcept {
      this->~BasicHandle();
      new (this) BasicHandle(std::move(other));
      return *this;
    }



    decltype(auto) operator*() const noexcept {
      return *handle;
    }
    decltype(auto) operator->() const noexcept {
      return handle;
    }

    decltype(auto) get() const noexcept {
      return *handle;
    }

    T release() noexcept {
      T tmp = nullability.NullHandle;
      ownership.release(handle);
      std::swap(tmp, handle);
      return std::move(tmp);
    }


  public:

    [[no_unique_address]] Ownership   ownership;
    [[no_unique_address]] Destruction destructor;
    [[no_unique_address]] Nullability nullability;
    T                                 handle;
  };

  template <std::derived_from<Object> T,
            typename Alloc = std::allocator<T>>
  using SharedObject = BasicHandle<T*,
                                   Policies::StrongRefCountedOwnership,
                                   Policies::AllocatorDestroy<Alloc>,
                                   Policies::Nullable>;
  template <typename T>
  using Ref = BasicHandle<T*,
                          Policies::BorrowedOwnership,
                          Policies::DoNotDestroy,
                          Policies::NonNullable>;

  template <typename T>
  class OptionalHandle{

  };
}

#endif //VALKYRIE_CORE_ADT_HANDLE_HPP
