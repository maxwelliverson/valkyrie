//
// Created by maxwe on 2021-03-31.
//

#ifndef VALKYRIE_ADT_PTR_HPP
#define VALKYRIE_ADT_PTR_HPP

#include <valkyrie/traits.hpp>
#include <valkyrie/utility/ordering.hpp>
#include <valkyrie/utility/casting.hpp>


VK_diagnostic_push
VK_clang_ignore_warning(unknown-attributes)


namespace valkyrie{

  template <typename Ptr>
  using ptr_element_t    = typename std::pointer_traits<Ptr>::element_type;
  template <typename T, typename Ptr>
  using ptr_rebind_t     = typename std::pointer_traits<Ptr>::template rebind<T>;
  template <typename Ptr>
  using ptr_difference_t = typename std::pointer_traits<Ptr>::difference_type;
  template <typename Ptr> requires requires{ typename Ptr::pointer; }
  using inner_pointer_t  = typename Ptr::pointer;





  namespace impl{

    template <auto MemFnPtr, typename = meta::overload<1>>
    struct get_function_ptr;

    template <typename Ret, typename ...Args, Ret(*FnPtr)(Args...) noexcept>
    struct get_function_ptr<FnPtr, meta::overload<1>>{
      inline constexpr static auto value = FnPtr;
    };
    template <typename Ret, typename Class, typename ...Args, Ret(Class::* MemFn)(Args...) noexcept>
    struct get_function_ptr<MemFn, meta::overload<1>>{
      static Ret value(Class* VK_param(nonnull) p, Args... args) noexcept {
        return (p->*MemFn)(std::forward<Args>(args)...);
      }
    };
    template <typename Ret, typename Class, typename ...Args, Ret(Class::* MemFn)(Args...) const noexcept>
    struct get_function_ptr<MemFn, meta::overload<1>>{
      static Ret value(const Class* VK_param(nonnull) p, Args... args) noexcept {
        return (p->*MemFn)(std::forward<Args>(args)...);
      }
    };

    template <typename Ret, typename ...Args, Ret(*FnPtr)(Args...)>
    struct get_function_ptr<FnPtr, meta::overload<0>>{
      inline constexpr static auto value = FnPtr;
    };
    template <typename Ret, typename Class, typename ...Args, Ret(Class::* MemFn)(Args...)>
    struct get_function_ptr<MemFn, meta::overload<0>>{
      static Ret value(Class* VK_param(nonnull) p, Args... args) {
        return (p->*MemFn)(std::forward<Args>(args)...);
      }
    };
    template <typename Ret, typename Class, typename ...Args, Ret(Class::* MemFn)(Args...) const>
    struct get_function_ptr<MemFn, meta::overload<0>>{
      static Ret value(const Class* VK_param(nonnull) p, Args... args) {
        return (p->*MemFn)(std::forward<Args>(args)...);
      }
    };

    template <auto Fn>
    struct get_function_ptr<Fn, meta::overload<1>> : get_function_ptr<Fn, meta::overload<0>>{};





    //template <auto Fn>




    template <typename Derived,
              typename Ptr,
              typename RefType,
              typename DiffType>
    struct ptr_typedefs;

    template <typename Derived,
              typename DiffType>
    class ptr_arithmetic;

    template <typename Derived,
        typename Ordering>
    class ptr_ordering;

    template <typename Derived,
              typename Ptr,
              typename RefType,
              typename DiffType>
    class ptr_deref;

    template <typename Derived,
              std::default_initializable Ptr>
    class ptr_storage;








    template <typename DiffType>
    struct param_difference_type{
      using type = in_param_t<DiffType>;
    };
    template <same_as<void> DiffType>
    struct param_difference_type<DiffType>{
      using type = ptrdiff_t;
    };


    template <typename Fn>
    using function_ptr = Fn*;

    template <typename Derived, typename Ptr>
    concept has_on_acquire = requires(function_ptr<Ptr(Derived* VK_param(nonnull), in_param_t<Ptr>) noexcept>& fn){
      fn = get_function_ptr<&Derived::on_acquire>::value;
    };
    template <typename Derived>
    concept has_on_release = requires(function_ptr<void(Derived* VK_param(nonnull)) noexcept>& fn){
      fn = get_function_ptr<&Derived::on_release>::value;
    };



    template <typename Derived,
              typename Ptr,
              typename Ref,
              typename DiffType>
    class ptr_deref {

      using derived_type = Derived;
      inline const derived_type& derived() const noexcept {
        return *static_cast<const derived_type*>(this);
      }


      using diff_param_t = typename impl::param_difference_type<DiffType>::type;

    public:

      /*using element_type    = ptr_element_t<Ptr>;
      using value_type      = remove_cv_t<element_type>;
      using reference       = Ref;*/


      Ref operator*()  const noexcept {
        auto result = derived().get();
        VK_assert_msg(result, "null pointer access detected");
        return *result;
      }
      Ptr operator->() const noexcept /*requires std::is_compound_v<remove_cv_t<ptr_element_t<Ptr>>>*/ {
        auto result = derived().get();
        VK_assert_msg(result, "null pointer access detected");
        return result;
      }

      Ref operator[](diff_param_t offset) const requires same_as<DiffType, void> {
        return *(derived() + offset);
      }
    };

    template <typename Derived,
              std::default_initializable Ptr>
    class ptr_storage{

      using derived_type = Derived;

      inline derived_type&       derived()       noexcept {
        return *static_cast<derived_type*>(this);
      }
      inline const derived_type& derived() const noexcept {
        return *static_cast<const derived_type*>(this);
      }

      using ptr_param_t = in_param_t<Ptr>;

      using PFN_on_acquire = Ptr(*)(Derived* VK_param(nonnull), ptr_param_t) /*noexcept*/;
      using PFN_on_release = void(*)(Derived* VK_param(nonnull)) /*noexcept*/;

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

      /*template <PFN_on_release = &Derived::on_release>
      static auto get_on_release(meta::overload<1>) -> meta::true_type;
      static auto get_on_release(meta::fallback)    -> meta::false_type;

      template <PFN_on_acquire = &Derived::on_acquire>
      static auto get_on_acquire(meta::overload<1>) -> meta::true_type;
      static auto get_on_acquire(meta::fallback)    -> meta::false_type;*/


      //inline constexpr static bool IsTrivialRelease          = is_trivial_release<&Derived::on_release>();
      //inline constexpr static bool IsTrivialAcquire          = is_trivial_acquire<&Derived::on_acquire>();
      inline constexpr static bool IsTrivialRelease          = !has_on_release<Derived>;
      inline constexpr static bool IsTrivialAcquire          = !has_on_acquire<Derived, Ptr>;
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
        do_release();
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

      inline void swap(ptr_storage& other) noexcept {
        derived_type der(std::move(derived()));
        auto* const  other_p = std::addressof(other.derived());

        new (this) derived_type(std::move(*other_p));
        new (other_p) derived_type(std::move(der));
      }


      explicit operator bool() const noexcept {
        return static_cast<bool>(derived().get());
      }


      friend void swap(ptr_storage& a, ptr_storage& b) noexcept {
        a.swap(b);
      }

      friend bool operator== (const ptr_storage& a, const ptr_storage& b) noexcept {
        return a.derived().get() == b.derived().get();
      }
    private:

      inline void do_release()              noexcept requires(!IsTrivialRelease) {
        constexpr static PFN_on_release on_release_ = get_function_ptr<&Derived::on_release>::value;
        on_release_(static_cast<derived_type*>(this));
      }
      inline Ptr  do_acquire(ptr_param_t p) noexcept requires(!IsTrivialAcquire) {
        constexpr static PFN_on_acquire on_acquire_ = get_function_ptr<&Derived::on_acquire>::value;
        return on_acquire_(static_cast<derived_type*>(this), p);
      }

      Ptr ptr_;
    };

    template <typename Derived,
              typename Ordering>
    class ptr_ordering{

      using derived_type = Derived;

      inline       derived_type& derived()       noexcept {
        return *static_cast<derived_type*>(this);
      }
      inline const derived_type& derived() const noexcept {
        return *static_cast<const derived_type*>(this);
      }

    public:

      using ordering_type = Ordering;


      friend ordering_type operator<=>(const ptr_ordering& a, const ptr_ordering& b) noexcept {

        const auto a_ = a.derived().get(), b_ = b.derived().get();

        if constexpr ( std::three_way_comparable<std::remove_cv_t<decltype(a_)>> )
          return a_ <=> b_;
        else {
          if ( a_ < b_ )
            return std::strong_ordering::less;
          else if ( b_ < a_ )
            return std::strong_ordering::greater;
          return std::strong_ordering::equal;
        }
      }

    };





    template <typename Derived,
              typename DiffType>
    class ptr_arithmetic{

      using derived_type = Derived;

      inline derived_type&       derived()       noexcept {
        return *static_cast<derived_type*>(this);
      }
      inline const derived_type& derived() const noexcept {
        return *static_cast<const derived_type*>(this);
      }

      using diff_param_t = in_param_t<DiffType>;

    public:

      using difference_type = DiffType;


      derived_type& operator++()    noexcept {
        derived_type& This = derived();
        auto&& p = This.get();
        This.reset(++p);
        return This;
      }
      derived_type  operator++(int) noexcept {
        derived_type& This = derived();
        derived_type copy = This;
        ++This;
        return copy;
      }
      derived_type& operator--()    noexcept {
        derived_type& This = derived();
        This.reset(--This.get());
        return This;
      }
      derived_type  operator--(int) noexcept {
        derived_type& This = derived();
        derived_type copy = This;
        --This;
        return copy;
      }

      derived_type& operator+=(diff_param_t offset) noexcept {
        derived_type& This = derived();
        This.reset(This.get() + offset);
        return This;
      }
      derived_type& operator-=(diff_param_t offset) noexcept {

        derived_type& This = derived();

        if constexpr ( std::signed_integral<difference_type> )
          return (This += (-offset));
        else {
          This.reset(This.get() - offset);
          return This;
        }
      }

      friend derived_type operator+(const ptr_arithmetic& a, diff_param_t diff) noexcept {
        derived_type result = a.derived();
        result += diff;
        return result;
      }
      friend derived_type operator-(const ptr_arithmetic& a, diff_param_t diff) noexcept {
        derived_type result = a.derived();
        result -= diff;
        return result;
      }
      friend derived_type operator-(diff_param_t diff, const ptr_arithmetic& a) noexcept = delete;

      friend difference_type operator-(const ptr_arithmetic& a, const ptr_arithmetic& b) noexcept {
        return a.derived().get() - b.derived().get();
      }
    };



    // Base dispatch

    template <typename Derived, typename Ptr, typename Ref, typename Diff>
    class ptr_deref_base : public ptr_deref<Derived, Ptr, Ref, Diff>{};
    template <typename Derived, typename Ptr, same_as<void> Ref, typename Diff>
    class ptr_deref_base<Derived, Ptr, Ref, Diff>{};


    template <typename Derived, typename Ordering>
    struct ptr_ordering_base : ptr_ordering<Derived, Ordering>{};
    template <typename Derived>
    struct ptr_ordering_base<Derived, not_ordered>{};


    template <typename Derived, typename DiffType>
    struct ptr_arithmetic_base : ptr_arithmetic<Derived, DiffType> {};
    template <typename Derived, same_as<void> DiffType>
    struct ptr_arithmetic_base<Derived, DiffType> {};


    // Typedefs/Meta Utilities


    template <typename Ref>
    struct ptr_reference_value_type;
    template <same_as<void> Ref>
    struct ptr_reference_value_type<Ref>{
      using type = Ref;
    };

    template <typename Ref> requires requires(const Ref& ref){ ref.get(); }
    struct ptr_reference_value_type<Ref>{
      using type = typename ptr_reference_value_type<decltype(std::declval<const Ref&>().get())>::type;
    };
    template <typename T>
    struct ptr_reference_value_type<T&>{
      using type = T;
    };


    template <typename Derived,
        typename Ptr,
        typename RefType,
        typename DiffType>
    struct ptr_typedefs{
      using pointer       = Derived;
      using inner_pointer = Ptr;
      using element_type  = typename ptr_reference_value_type<RefType>::type;
      using value_type    = remove_cv_t<element_type>;
    };


    // Default types...


    template <typename Ptr, typename RefType, typename = meta::overload<1>>
    struct ptr_default_difference_type{};
    template <typename Ptr, typename RefType, size_t N> requires ( N > 0)
    struct ptr_default_difference_type<Ptr, RefType, meta::overload<N>> : ptr_default_difference_type<Ptr, RefType, meta::overload<N - 1>>{};

    template <typename Ptr, typename Ref>
    struct ptr_default_difference_type<Ptr, Ref, meta::overload<0>>{
      using type = void;
    };
    template <typename Ptr, typename RefType> requires requires(const Ptr& p){
      p - p;
    }
    struct ptr_default_difference_type<Ptr, RefType, meta::overload<1>>{
      using type = decltype(std::declval<const Ptr&>() - std::declval<const Ptr&>());
    };


    template <typename Ptr, typename = meta::overload<1>>
    struct ptr_default_ordering_type : ptr_default_ordering_type<Ptr, meta::overload<0>>{};
    template <typename Ptr>
    struct ptr_default_ordering_type<Ptr, meta::overload<0>>{
      using type = not_ordered;
    };
    template <typename Ptr> requires requires(const Ptr& a){ a < a; a == a; }
    struct ptr_default_ordering_type<Ptr, meta::overload<0>>{
      using type = strong_ordering;
    };
    template <typename Ptr>
    requires requires(const Ptr& a){
      typename std::compare_three_way_result_t<const Ptr&, const Ptr&>;
      a < a;
      a == a;
    }
    struct ptr_default_ordering_type<Ptr, meta::overload<1>>{
      using type = std::compare_three_way_result_t<const Ptr&, const Ptr&>;
    };



    template <typename Ptr, typename = meta::overload<2>>
    struct ptr_default_reference_type;
    template <typename Ptr, size_t N> requires (N > 0)
    struct ptr_default_reference_type<Ptr, meta::overload<N>>
        : ptr_default_reference_type<Ptr, meta::overload<N - 1>>{};


    template <typename Ptr> requires requires{ typename ptr_element_t<Ptr>; }
    struct ptr_default_reference_type<Ptr, meta::overload<0>>{
      using type = ptr_element_t<Ptr>&;
    };
    template <typename Ptr> requires requires{
      typename ptr_element_t<Ptr>;
      requires same_as<ptr_element_t<Ptr>, void>;
    }
    struct ptr_default_reference_type<Ptr, meta::overload<1>>{
      using type = ptr_element_t<Ptr>;
    };
    template <typename Ptr> requires requires(const Ptr& a){
      *a;
      typename ptr_reference_value_type<decltype(*a)>::type;
    }
    struct ptr_default_reference_type<Ptr, meta::overload<2>>{
      using type = decltype(*std::declval<const Ptr&>());
    };


    template <typename Ptr>
    using default_ptr_reference_t  = typename ptr_default_reference_type<Ptr>::type;
    template <typename Ptr>
    using default_ptr_ordering_t   = typename ptr_default_ordering_type<Ptr>::type;
    template <typename Ptr, typename RefType>
    using default_ptr_difference_t = typename ptr_default_difference_type<Ptr, RefType>::type;
  }


  /**
   * A CRTP based complete (yet customizable) implementation of most facilities needed for a
   * smart pointer type.
   *
   * @tparam Derived  The actual type inheriting from ptr_facade.
   *                  Derived must otherwise define the following
   *                  methods:
   *                    - Ptr  Derived::get() const
   *                    - Ptr  Derived::release()
   *                    - void Derived::reset()
   *                    - void Derived::reset(Ptr)
   * @tparam Ptr      The underlying pointer type, must either be a
   *                  raw pointer or another smart pointer.
   * @tparam RefType  The result type of dereference ops. Must be one of
   *                    - an lvalue reference
   *                    - a reference wrapper type: must have a method RefType::get() that returns a reference type
   *                    - void: in this case, dereference ops are not provided
   * @tparam Ordering The result type of a three way comparison ops. Must be one of
   *                    - strong_ordering
   *                    - weak_ordering
   *                    - partial_ordering
   *                    - not_ordered       (in this case, none of the ordered relational operators are provided)
   * @tparam DiffType The offset type of arithmetic ops. Does not need to
   *                  be signed (or even integral) so long as it has well
   *                  defined arithmetic operations with objects of type
   *                  Ptr. This restriction is not necessary if the following
   *                  operators are overridden by Derived:
   *                    - operator++()
   *                    - operator--()
   *                    - operator+=(DiffType)
   *                    - operator-(Derived, Derived)
   *                  DiffType may also be void, in which case arithmetic
   *                  ops are not provided.
   *                  The subscript operator, operator[](DiffType), is
   *                  also provided if and only if RefType is not void.
   * */
  template <typename Derived,
            typename Ptr,
            typename RefType  = impl::default_ptr_reference_t<Ptr>,
            typename Ordering = impl::default_ptr_ordering_t<Ptr>,
            typename DiffType = impl::default_ptr_difference_t<Ptr, RefType>>
  class ptr_facade : public impl::ptr_typedefs<Derived, Ptr, RefType, DiffType>,
                     public impl::ptr_deref_base<Derived, Ptr, RefType, DiffType>,
                     public impl::ptr_arithmetic_base<Derived, DiffType>,
                     public impl::ptr_ordering_base<Derived, Ordering>{};



  /**
   * An extension of ptr_facade that also provides storage, access,
   * and optional lifetime tracking.
   *
   * @tparam Derived  The actual type inheriting from ptr_base.
   *                  Unlike ptr_facade, Derived doesn't need to
   *                  define any extra methods for full functionality,
   *                  but two optional hooks are provided for
   *                  lifetime tracking. It is valid to only provide
   *                  one hook but not the other.
   *                    - Ptr on_acquire(in_param_t<Ptr>) noexcept
   *                      OR
   *                      static Ptr on_acquire(Derived*, in_param_t<Ptr>) noexcept
   *                    - void on_release() noexcept
   *                      OR
   *                      static void on_release(Derived*) noexcept
   *
   * @tparam Ptr      See ptr_facade
   * @tparam RefType  See ptr_facade
   * @tparam Ordering See ptr_facade
   * @tparam DiffType See ptr_facade
   *
   * */
  template <typename Derived,
            typename Ptr,
            typename RefType  = typename impl::ptr_default_reference_type<Ptr>::type,
            typename Ordering = typename impl::ptr_default_ordering_type<Ptr>::type,
            typename DiffType = typename impl::ptr_default_difference_type<Ptr, RefType>::type>
  class ptr_base : public impl::ptr_storage<Derived, Ptr>,
                   public ptr_facade<Derived, Ptr, RefType, Ordering, DiffType>{
    using storage_base = impl::ptr_storage<Derived, Ptr>;
  public:
    using storage_base::storage_base;
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


    operator T* VK_param(nullable) () const noexcept {
      return this->get();
    }
  };

  template <typename T, typename Deleter = std::default_delete<T>>
  class unique_ptr : public ptr_base<unique_ptr<T>, T*>{
    using base = ptr_base<unique_ptr, T*>;
  public:
    using base::base;

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;



    void on_release() noexcept {
      if (auto p = this->get())
        deleter(p);
    }

  private:
    [[no_unique_address]] Deleter deleter{};
  };

  /*struct HelloTest{
    int hi;
    HelloTest* next;

    int member_func(float) noexcept;
    static int static_func(HelloTest*, float);
  };

  void hello(borrowed_ptr<HelloTest> ptr) {
    auto& p = ++ptr;
    assert( ptr < p );
  }
  void hello(borrowed_ptr<void> ptr) {
    assert( (ptr <=> ptr) == strong_ordering::equal);
  }

  void test(){
    using fn_ptr = int(*)(HelloTest*, float);
    fn_ptr a = impl::get_function_ptr<&HelloTest::static_func>::value;
    fn_ptr b = impl::get_function_ptr<&HelloTest::member_func>::value;
  }*/


#define VK_nonnull VK_if(VK_compiler_clang(_Nonnull))
#define VK_assume_nonnull(ptr) VK_if(VK_compiler_msvc(__assume((ptr) != nullptr))VK_else())


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
    using ptr_param_t = in_param_t<pointer>;
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

/*template <typename T>
struct std::pointer_traits<valkyrie::borrowed_ptr<T>>{

  //using pointer


};*/

VK_diagnostic_pop

#endif//VALKYRIE_ADT_PTR_HPP
