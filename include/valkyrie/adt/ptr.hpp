//
// Created by maxwe on 2021-03-31.
//

#ifndef VALKYRIE_ADT_PTR_HPP
#define VALKYRIE_ADT_PTR_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie{

  template <typename Ptr>
  using ptr_element_t    = typename std::pointer_traits<Ptr>::element_type;
  template <typename T, typename Ptr>
  using ptr_rebind_t     = typename std::pointer_traits<Ptr>::template rebind<T>;
  template <typename Ptr>
  using ptr_difference_t = typename std::pointer_traits<Ptr>::difference_type;
  template <typename Ptr> requires requires{ typename Ptr::pointer; }
  using inner_pointer_t  = typename Ptr::pointer;

  template <typename Derived, typename Ptr>
  class ptr_facade{

    using derived_type = Derived;

    inline derived_type&       derived()       noexcept {
      return *static_cast<derived_type*>(this);
    }
    inline const derived_type& derived() const noexcept {
      return *static_cast<const derived_type*>(this);
    }

  protected:



  public:

    using pointer         = Ptr;
    using element_type    = ptr_element_t<Ptr>;
    using value_type      = remove_cv_t<element_type>;
    using difference_type = ptr_difference_t<Ptr>;

    //using reference       = decltype(*std::declval<pointer>());



    pointer release() noexcept requires(!std::is_final_v<derived_type>) {
      struct protected_access : public derived_type {};

      pointer p = derived().get();
      derived().on_release();
      return p;
    }


    void swap(ptr_facade& other) noexcept {
      derived_type tmp = std::move(other.derived());
      new (&other) derived_type(std::move(derived()));
      new (this)   derived_type(std::move(tmp));
    }


    decltype(auto) operator*()  const noexcept {
      auto&& result = derived().operator->();
      VK_assert_msg(result, "null pointer access detected");
      return *std::forward<decltype(result)>(result);
    }
    pointer        operator->() const noexcept {
      auto result = derived().get();
      VK_assert_msg(result, "null pointer access detected");
      return std::move(result);
    }

    explicit operator bool() const noexcept {
      return static_cast<bool>(derived().get());
    }

    friend bool operator== (const ptr_facade& a, const ptr_facade& b) noexcept {
      return a.derived().get() == b.derived().get();
    }
    friend auto operator<=>(const ptr_facade& a, const ptr_facade& b) noexcept
        requires(std::totally_ordered<pointer>) {

      const auto a_ = a.derived().get(), b_ = b.derived().get();

      if constexpr (std::three_way_comparable<pointer>)
        return a_ <=> b_;
      else {
        if ( a_ < b_ )
          return std::strong_ordering::less;
        else if ( b_ < a_ )
          return std::strong_ordering::greater;
        return std::strong_ordering::equal;
      }
    }

    friend auto operator-(const ptr_facade& a, const ptr_facade& b) noexcept -> decltype(a.operator->() - b.operator->()) {
      return a.operator->() - b.operator->();
    }

    friend void swap(ptr_facade& a, ptr_facade& b) noexcept {
      a.swap(b);
    }


    // Private Hooks: would ideally be protected, but need to be public such that
    // the member functions of this class can access them without needing to be
    // explicitly declared a friend of the deriving class
  protected:
    void _do_release()        noexcept { }
    void _do_acquire(pointer) noexcept { }
  };

  template <typename Derived, std::default_initializable Ptr>
  class ptr_storage{

    using derived_type = Derived;

    inline derived_type&       derived()       noexcept {
      return *static_cast<derived_type*>(this);
    }
    inline const derived_type& derived() const noexcept {
      return *static_cast<const derived_type*>(this);
    }

    using ptr_param_t = param_t<Ptr>;

    using PFN_on_acquire = Ptr(*)(Derived*, ptr_param_t) noexcept;
    using PFN_on_release = void(*)(Derived*) noexcept;

    /*template <auto pfn_on_release>
    consteval bool is_trivial_release() {
      return false;
    }
    template <same_as<PFN_on_release> auto pfn_on_release>
    consteval bool is_trivial_release() {
      return pfn_on_release == &ptr_storage::on_release;
    }

    template <auto>
    consteval bool is_trivial_acquire() {
      return false;
    }
    template <same_as<PFN_on_acquire> auto pfn_on_acquire>
    consteval bool is_trivial_acquire() {
      return pfn_on_acquire == &ptr_storage::on_acquire;
    }*/

    template <PFN_on_release = &Derived::on_release>
    static auto get_on_release(meta::overload<1>) -> meta::true_type;
    static auto get_on_release(meta::fallback)    -> meta::false_type;

    template <PFN_on_acquire = &Derived::on_acquire>
    static auto get_on_acquire(meta::overload<1>) -> meta::true_type;
    static auto get_on_acquire(meta::fallback)    -> meta::false_type;


    //inline constexpr static bool IsTrivialRelease          = is_trivial_release<&Derived::on_release>();
    //inline constexpr static bool IsTrivialAcquire          = is_trivial_acquire<&Derived::on_acquire>();
    inline constexpr static bool IsTrivialRelease          = !decltype(get_on_release(meta::overload<1>{}))::value;
    inline constexpr static bool IsTrivialAcquire          = !decltype(get_on_acquire(meta::overload<1>{}))::value;
    inline constexpr static bool IsTrivial                 = IsTrivialAcquire && IsTrivialRelease;
    inline constexpr static bool IsNullable                = std::constructible_from<Ptr, std::nullptr_t>;

    inline constexpr static bool IsTrivialAcqNonTrivialRel = IsTrivialAcquire && !IsTrivialRelease;
    inline constexpr static bool IsNonTrivialAcqTrivialRel = !IsTrivialAcquire && IsTrivialRelease;
    inline constexpr static bool IsNonTrivial              = !IsTrivialAcquire && !IsTrivialRelease;


  public:


    ptr_storage() = default;
    ptr_storage(std::nullptr_t) noexcept requires(IsNullable) : ptr_(nullptr){}

    // Default implementations if on_acquire/on_release operations are not overridden
    ptr_storage(const ptr_storage& other)     requires(IsTrivialAcquire) = default;
    ptr_storage(ptr_storage&& other) noexcept requires(IsTrivial)        = default;

    ptr_storage(ptr_param_t ptr) noexcept requires(IsTrivialAcquire) : ptr_(ptr){}

    ~ptr_storage() requires(IsTrivialRelease) = default;

    ptr_storage& operator=(const ptr_storage& other)     requires(IsTrivialAcquire) = default;
    ptr_storage& operator=(ptr_storage&& other) noexcept requires(IsTrivial) = default;


    // Non-default implementations if only on_acquire ops are overridden

    ptr_storage(const ptr_storage& other) : ptr_(do_acquire(other.ptr_)){}
    ptr_storage(ptr_storage&& other) noexcept requires(IsNonTrivialAcqTrivialRel) : ptr_(do_acquire()) {}

    ~ptr_storage() {
      this->do_release();
    }



    inline Ptr  get() const noexcept {
      return ptr_;
    }
    inline Ptr  release() noexcept {
      if constexpr (IsTrivialRelease)
        return ptr_;
      else {
        Ptr p = ptr_;
        do_release();
        return p;
      }
    }
    inline void reset(ptr_param_t p = {}) noexcept {

      if constexpr (!IsTrivialRelease)
        do_release();

      if constexpr (IsTrivialAcquire)
        ptr_ = p;
      else
        ptr_ = do_acquire(p);
    }


  private:

    inline void do_release()              noexcept requires(!IsTrivialRelease) {
      constexpr static PFN_on_release on_release_ = &Derived::on_release;
      on_release_(static_cast<derived_type*>(this));
    }
    inline Ptr  do_acquire(ptr_param_t p) noexcept requires(!IsTrivialAcquire) {
      constexpr static PFN_on_acquire on_acquire_ = &Derived::on_acquire;
      return on_acquire_(static_cast<derived_type*>(this), p);
    }


    Ptr ptr_;
  };


  template <typename Derived, typename Ptr>
  class ptr_base : public ptr_storage<Derived, Ptr>, public ptr_facade<Derived, Ptr>{
    using storage = ptr_storage<Derived, Ptr>;
  public:

    using storage::storage;
  };


  // Essentially provides a thin wrapper over a primitive pointer,
  // allowing unified semantics between raw and fancy pointers
  template <typename T>
  class borrowed_ptr : public ptr_base<borrowed_ptr<T>, T*>{

    using base = ptr_base<borrowed_ptr<T>, T*>;

    template <typename>
    friend class borrowed_ptr;

  public:

    using base::base;

    template <pointer_convertible_to<T> U>
    borrowed_ptr(const borrowed_ptr<U> other) noexcept : base(other.get()){ }


    operator T*() const noexcept {
      return this->get();
    }
  };


#define VK_nonnull VK_if(VK_compiler_clang(_Nonnull))
#define VK_assume_nonnull(ptr) VK_if(VK_compiler_msvc(__assume(ptr != nullptr))VK_else())


  namespace impl{
    template <typename Ptr>
    struct attach_nonnull_to_ptr{
      using type = ptr_rebind_t<typename attach_nonnull_to_ptr<inner_pointer_t<Ptr>>::type, Ptr>;
    };
    template <typename T>
    struct attach_nonnull_to_ptr<T*>{
      using type = T* VK_nonnull;
    };
  }

  template <typename Ptr>
  class nonnull  : public ptr_base<nonnull<Ptr>, typename impl::attach_nonnull_to_ptr<Ptr>::type>{
    using base = ptr_base<nonnull<Ptr>, typename impl::attach_nonnull_to_ptr<Ptr>::type>;
  public:

    using pointer = typename base::pointer;

  private:
    using ptr_param_t = param_t<pointer>;
  public:

    nonnull()               = delete;
    nonnull(std::nullptr_t) = delete;

    using base::base;

    pointer on_acquire(ptr_param_t p) noexcept {
      VK_assert_msg( p != nullptr, "a null value was assigned to a pointer of type nonnull<>");
      VK_assume_nonnull( p );
      return p;
    }
    void    on_release() noexcept {
      VK_assert_msg( this->get() != nullptr, "tried to release() a null nonnull<> pointer" );
      VK_assume_nonnull( this->get() );
    }


    operator pointer() const noexcept {
      return this->get();
    }

    explicit operator bool() const noexcept {
      return true;
    }

    friend bool operator==(const nonnull& a, std::nullptr_t) noexcept { return false; }
  };

}

#endif//VALKYRIE_ADT_PTR_HPP
