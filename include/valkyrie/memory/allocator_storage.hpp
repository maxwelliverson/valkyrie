//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_ALLOCATOR_STORAGE_HPP
#define VALKYRIE_MEMORY_ALLOCATOR_STORAGE_HPP



#include <valkyrie/adt/locked_ptr.hpp>

#include <new>

namespace valkyrie {
  /*namespace detail {
    template <class Alloc>
    void *try_allocate_node(std::true_type, Alloc &alloc, u64 size, u64 alignment) noexcept {
      return composable_allocator_traits<Alloc>::try_allocate_node(alloc, size,
                                                                   alignment);
    }

    template <class Alloc>
    void *try_allocate_array(std::true_type, Alloc &alloc, u64 count, u64 size, u64 alignment) noexcept {
      return composable_allocator_traits<Alloc>::try_allocate_array(alloc, count, size,
                                                                    alignment);
    }

    template <class Alloc>
    bool try_deallocate_node(std::true_type, Alloc &alloc, void *ptr, u64 size, u64 alignment) noexcept {
      return composable_allocator_traits<Alloc>::try_deallocate_node(alloc, ptr, size,
                                                                     alignment);
    }

    template <class Alloc>
    bool try_deallocate_array(std::true_type, Alloc &alloc, void *ptr, u64 count, u64 size, u64 alignment) noexcept {
      return composable_allocator_traits<Alloc>::try_deallocate_array(alloc, ptr, count,
                                                                      size, alignment);
    }

    template <class Alloc>
    void *try_allocate_node(std::false_type, Alloc &, u64, u64) noexcept {

      VK_unreachable;
      return nullptr;
    }

    template <class Alloc>
    void *try_allocate_array(std::false_type, Alloc &, u64, u64, u64) noexcept {
      VK_unreachable;
      return nullptr;
    }

    template <class Alloc>
    bool try_deallocate_node(std::false_type, Alloc &, void *, u64, u64) noexcept {
      VK_unreachable;
      return false;
    }

    template <class Alloc>
    bool try_deallocate_array(std::false_type, Alloc &, void *, u64, u64, u64) noexcept {
      VK_unreachable;
      return false;
    }
  }*/  // namespace detail
  
  
  namespace impl{
    template <memory_storage_policy_c Pol, mutex_c Mtx>
    using mutex_actual_type = std::conditional_t<
        allocator_traits<typename Pol::allocator_type>::is_thread_safe,
        noop_mutex,
        Mtx>;
  }

  /// A \concept{concept_rawallocator,RawAllocator} that stores another allocator.
  /// The \concept{concept_storagepolicy,StoragePolicy} defines the allocator type being stored and how it is stored.
  /// The \c Mutex controls synchronization of the access.
  /// \ingroup storage
  template <memory_storage_policy_c StoragePolicy,
            mutex_c Mutex = mutex>
  class VK_empty_bases allocator_storage : StoragePolicy, impl::mutex_actual_type<StoragePolicy, Mutex> {
  public:
    using allocator_type         = typename StoragePolicy::allocator_type;
    using storage_policy         = StoragePolicy;
    using mutex_type             = impl::mutex_actual_type<StoragePolicy, Mutex>;

    template <memory_storage_policy_c, mutex_c>
    friend class allocator_storage;

  private:
    using trt          = allocator_traits<allocator_type>;


  public:

    VK_constant bool is_stateful           = trt::is_stateful;
    VK_constant bool is_always_composable  = trt::is_composable;
    VK_constant bool is_always_thread_safe = trt::is_thread_safe || not_same_as<Mutex, noop_mutex>;



    /// \effects Creates it by default-constructing the \c StoragePolicy.
    /// \requires The \c StoragePolicy must be default-constructible.
    /// \notes The default constructor may create an invalid allocator storage not associated with any allocator.
    /// If that is the case, it must not be used.
    allocator_storage() = default;

    /// \effects Creates it by passing it an allocator.
    /// The allocator will be forwarded to the \c StoragePolicy, it decides whether it will be moved, its address stored or something else.
    /// \requires The expression <tt>new storage_policy(std::forward<Alloc>(alloc))</tt> must be well-formed,
    /// otherwise this constructor does not participate in overload resolution.
    template <typename Alloc>
    allocator_storage(Alloc &&alloc) requires(!pointer_convertible_to<remove_cvref_t<Alloc>, allocator_storage>
        /*&& constructible_from<storage_policy, Alloc>*/)
        : storage_policy(std::forward<Alloc>(alloc)),
          mutex_type(){ }

    /// \effects Creates it by passing it another \c allocator_storage with a different \c StoragePolicy but the same \c Mutex type.
    /// Initializes it with the result of \c other.get_allocator().
    /// \requires The expression <tt>new storage_policy(other.get_allocator())</tt> must be well-formed,
    /// otherwise this constructor does not participate in overload resolution.
    template <class OtherPolicy>
    allocator_storage(const allocator_storage<OtherPolicy, Mutex> &other) requires(requires { storage_policy(other.get_allocator()); })
        : storage_policy(other.get_allocator()),
          mutex_type() { }

    /// @{
    /// \effects Moves the \c allocator_storage object.
    /// A moved-out \c allocator_storage object must still store a valid allocator object.
    allocator_storage(allocator_storage &&other) noexcept
        : storage_policy(std::move(other)),
          mutex_type(std::move(other.mutex)) { }

    allocator_storage &operator=(allocator_storage &&other) noexcept {
      storage_policy::operator=(std::move(other));
      mutex_type::operator=(std::move(other));
      return *this;
    }
    /// @}

    /// @{
    /// \effects Copies the \c allocator_storage object.
    /// \requires The \c StoragePolicy must be copyable.
    allocator_storage(const allocator_storage & other) : storage_policy(other), mutex_type(){}
    allocator_storage& operator=(const allocator_storage& other) {
      if (std::addressof(other) == this)
        return *this;
      this->~allocator_storage();
      new((void*)this) allocator_storage(other);
      return *this;
    }
    /// @}

    /// @{
    /// \effects Calls the function on the stored allocator.
    /// The \c Mutex will be locked during the operation.
    void*   allocate_node(u64 size, u64 alignment) {
      VK_scope_lock(get_mutex(), write_access);
      return trt::allocate_node(get_allocator(), size, alignment);
    }
    void  deallocate_node(void *ptr, u64 size, u64 alignment) noexcept {
      VK_scope_lock(get_mutex(), write_access);
      trt::deallocate_node(get_allocator(), ptr, size, alignment);
    }

    void*   allocate_array(u64 count, u64 size, u64 alignment) {
      VK_scope_lock(get_mutex(), write_access);
      return trt::allocate_array(get_allocator(), count, size, alignment);
    }
    void* reallocate_array(void* ptr, u64 new_count, u64 old_count, u64 size, u64 align) noexcept {
      VK_scope_lock(get_mutex(), write_access);
      return trt::reallocate_array(get_allocator(), ptr, new_count, old_count, size, align);
    }
    void  deallocate_array(void *ptr, u64 count, u64 size, u64 alignment) noexcept {
      VK_scope_lock(get_mutex(), write_access);
      trt::deallocate_array(get_allocator(), ptr, count, size, alignment);
    }

    VK_nodiscard u64 max_node_size() const noexcept {
      VK_scope_lock(get_mutex(), read_access);
      return trt::max_node_size(get_allocator());
    }
    VK_nodiscard u64 max_array_size() const noexcept {
      VK_scope_lock(get_mutex(), read_access);
      return trt::max_array_size(get_allocator());
    }
    VK_nodiscard u64 max_alignment() const noexcept {
      VK_scope_lock(get_mutex(), read_access);
      return trt::max_alignment(get_allocator());
    }
    /// @}


    /// @{
    /// \effects Calls the function on the stored composable allocator.
    /// The \c Mutex will be locked during the operation.
    /// \requires The allocator must be composable,
    /// i.e. \ref is_composable() must return `true`.
    /// \note This check is done at compile-time where possible,
    /// and at runtime in the case of type-erased storage.
    VK_nodiscard void* try_allocate_node(u64 size, u64 alignment) noexcept {
      VK_assert(is_composable());
      VK_scope_lock(get_mutex(), write_access);
      auto &&alloc = get_allocator();
      return trt::try_allocate_node(alloc, size, alignment);
    }
    VK_nodiscard void* try_allocate_array(u64 count, u64 size, u64 alignment) noexcept {
      VK_assert(is_composable());
      VK_scope_lock(get_mutex(), write_access);
      auto &&alloc = get_allocator();
      return trt::try_allocate_array(alloc, count, size, alignment);
    }

    VK_nodiscard bool try_deallocate_node(void *ptr, u64 size, u64 alignment) noexcept {
      VK_assert(is_composable());
      VK_scope_lock(get_mutex(), write_access);
      auto &&alloc = get_allocator();
      return trt::try_deallocate_node(alloc, ptr, size, alignment);
    }
    VK_nodiscard bool try_deallocate_array(void *ptr, u64 count, u64 size, u64 alignment) noexcept {
      VK_assert(is_composable());
      VK_scope_lock(get_mutex(), write_access);
      auto &&alloc = get_allocator();
      return trt::try_deallocate_array(alloc, ptr, count, size, alignment);
    }
    /// @}


    /// @{
    /// \effects Forwards to the \c StoragePolicy.
    /// \returns Returns a reference to the stored allocator.
    /// \note This does not lock the \c Mutex.
    decltype(auto) get_allocator()       noexcept {
      return storage_policy::get_allocator();
    }
    decltype(auto) get_allocator() const noexcept {
      return storage_policy::get_allocator();
    }
    /// @}



    /// @{
    /// \returns A proxy object that acts like a pointer to the stored allocator.
    /// It cannot be reassigned to point to another allocator object and only moving is supported, which is destructive.
    /// As long as the proxy object lives and is not moved from, the \c Mutex will be kept locked.
    auto lock()       noexcept {
      return locked_ptr(std::addressof(get_allocator()), get_mutex());
    }
    auto lock() const noexcept {
      return locked_ptr(std::addressof(get_allocator()), get_mutex());
    }
    /// @}.

    /// \returns Whether or not the stored allocator is composable,
    /// that is you can use the composing functions.
    /// \note Due to type-erased allocators,
    /// this function can not be `constexpr`.
    VK_nodiscard bool is_composable() const noexcept {
      return StoragePolicy::is_composable();
    }


    VK_constant bool propagate_on_container_copy_assignment = trt::propagate_on_container_copy_assignment;
    VK_constant bool propagate_on_container_move_assignment = trt::propagate_on_container_move_assignment;
    VK_constant bool propagate_on_container_swap            = trt::propagate_on_container_swap;

    template <typename Pol> requires std::assignable_from<storage_policy&, typename Pol::allocator_type&>
    void container_copy_assign(const allocator_storage<Pol, Mutex>& other) noexcept {
      if constexpr ( propagate_on_container_copy_assignment ) {
        storage_policy::operator=(other.get_allocator());
      }
    }
    template <typename Pol> requires std::assignable_from<storage_policy&, typename Pol::allocator_type&&>
    void container_move_assign(allocator_storage<Pol, Mutex>&& other) noexcept {
      if constexpr ( propagate_on_container_move_assignment ) {
        storage_policy::operator=(std::move(other.get_allocator()));
      }
    }
    template <typename Pol> requires std::swappable_with<storage_policy&, Pol&>
    void container_swap(allocator_storage<Pol, Mutex>& other) noexcept {
      if constexpr ( propagate_on_container_swap ) {
        std::ranges::swap(static_cast<storage_policy&>(*this), static_cast<Pol&>(*this));
      }
    }

    /*VK_nodiscard allocator_storage select_on_container_copy_construction() const noexcept {
      return allocator_storage(trt::select_on_container_copy_construction(get_allocator()));
    }*/


    template <typename T>
    inline void on_new_object(T* p) noexcept {
      trt::on_new_object(get_allocator(), p);
    }
    template <typename T>
    inline void on_new_array(T* p, u64 size) noexcept {
      trt::on_new_array(get_allocator(), p, size);
    }
    template <typename T>
    inline void on_delete_object(T* p) noexcept {
      trt::on_delete_object(get_allocator(), p);
    }
    template <typename T>
    inline void on_delete_array(T* p, u64 size) noexcept {
      trt::on_delete_array(get_allocator(), p, size);
    }


    inline void copy_bytes(void* to, const void* from, u64 bytes) noexcept {
      trt::copy_bytes(get_allocator(), to, from, bytes);
    }
    inline void move_bytes(void* to, const void* from, u64 bytes) noexcept {
      trt::move_bytes(get_allocator(), to, from, bytes);
    }
    inline void  set_bytes(void* to, int value,        u64 bytes) noexcept {
      trt::set_bytes(get_allocator(), to, value, bytes);
    }
    inline void zero_bytes(void* to,                   u64 bytes) noexcept {
      trt::zero_bytes(get_allocator(), to, bytes);
    }

  protected:

    decltype(auto) get_mutex() const noexcept {
      return const_cast<mutex_type&>(static_cast<const mutex_type&>(*this));
    }
  };

  /// A \concept{concept_storagepolicy,StoragePolicy} that stores the allocator directly.
  /// It embeds the allocator inside it, i.e. moving the storage policy will move the allocator.
  /// \ingroup storage
  template <class RawAllocator>
  class direct_storage : allocator_traits<RawAllocator>::allocator_type {
    static_assert(!std::is_same<RawAllocator, any_allocator>::value,
                  "cannot type-erase in direct_storage");

  public:
    using allocator_type = typename allocator_traits<RawAllocator>::allocator_type;

    /// \effects Creates it by default-constructing the allocator.
    /// \requires The \c RawAllcoator must be default constructible.
    direct_storage() = default;

    /// \effects Creates it by moving in an allocator object.
    direct_storage(allocator_type &&allocator) noexcept
        : allocator_type(std::move(allocator)) {
    }

    /// @{
    /// \effects Moves the \c direct_storage object.
    /// This will move the stored allocator.
    direct_storage(direct_storage &&other) noexcept : allocator_type(std::move(other)) {}

    direct_storage &operator=(direct_storage &&other) noexcept {
      allocator_type::operator=(std::move(other));
      return *this;
    }
    /// @}

    /// @{
    /// \returns A (\c const) reference to the stored allocator.
    allocator_type &get_allocator() noexcept {
      return *this;
    }

    const allocator_type &get_allocator() const noexcept {
      return *this;
    }
    /// @}

  protected:
    ~direct_storage() noexcept = default;

    bool is_composable() const noexcept {
      return composable_allocator_c<allocator_type>;
    }
  };

  /// An alias template for \ref allocator_storage using the \ref direct_storage policy without a mutex.
  /// It has the effect of giving any \concept{concept_rawallocator,RawAllocator} the interface with all member functions,
  /// avoiding the need to wrap it inside the \ref allocator_traits.
  /// \ingroup storage
  template <class RawAllocator>
  using allocator_adapter = allocator_storage<direct_storage<RawAllocator>, noop_mutex>;

  /// \returns A new \ref allocator_adapter object created by forwarding to the constructor.
  /// \relates allocator_adapter
  template <class RawAllocator>
  auto make_allocator_adapter(RawAllocator &&allocator) noexcept
      -> allocator_adapter<std::decay_t<RawAllocator>> {
    return {std::forward<RawAllocator>(allocator)};
  }

  /// An alias template for \ref allocator_storage using the \ref direct_storage policy with a mutex.
  /// It has a similar effect as \ref allocator_adapter but performs synchronization.
  /// The \c Mutex will default to \c std::mutex if threading is supported,
  /// otherwise there is no default.
  /// \ingroup storage
  template <class RawAllocator, class Mutex = mutex>
  using thread_safe_allocator = allocator_storage<direct_storage<RawAllocator>, Mutex>;

  template <class RawAllocator>
  auto make_thread_safe_allocator(RawAllocator &&allocator)
      -> thread_safe_allocator<std::decay_t<RawAllocator>> {
    return std::forward<RawAllocator>(allocator);
  }

  /// \returns A new \ref thread_safe_allocator object created by forwarding to the constructor,
  /// specifying a certain mutex type.
  /// \requires It requires threading support from the implementation.
  /// \relates thread_safe_allocator
  template <class Mutex, class RawAllocator>
  auto make_thread_safe_allocator(RawAllocator &&allocator)
      -> thread_safe_allocator<typename std::decay<RawAllocator>::type, Mutex> {
    return std::forward<RawAllocator>(allocator);
  }


  /*namespace detail
{
  struct reference_stateful
  {
  };
  struct reference_stateless
  {
  };
  struct reference_shared
  {
  };

  reference_stateful  reference_type(std::true_type stateful, std::false_type shared);
  reference_stateless reference_type(std::false_type stateful, std::true_type shared);
  reference_stateless reference_type(std::false_type stateful, std::false_type shared);
  reference_shared    reference_type(std::true_type stateful, std::true_type shared);

  template <class RawAllocator, class Tag>
  class reference_storage_impl;

  // reference to stateful: stores a pointer to an allocator
  template <class RawAllocator>
  class reference_storage_impl<RawAllocator, reference_stateful>
  {
  protected:
    reference_storage_impl() noexcept : alloc_(nullptr) {}

    reference_storage_impl(RawAllocator& allocator) noexcept : alloc_(&allocator) {}

    bool is_valid() const noexcept
    {
      return alloc_ != nullptr;
    }

    RawAllocator& get_allocator() const noexcept
    {
      VK_assert(alloc_ != nullptr);
      return *alloc_;
    }

  private:
    RawAllocator* alloc_;
  };

  // reference to stateless: store in static storage
  template <class RawAllocator>
  class reference_storage_impl<RawAllocator, reference_stateless>
  {
  protected:
    reference_storage_impl() noexcept = default;

    reference_storage_impl(const RawAllocator&) noexcept {}

    bool is_valid() const noexcept
    {
      return true;
    }

    RawAllocator& get_allocator() const noexcept
    {
      static RawAllocator alloc;
      return alloc;
    }
  };

  // reference to shared: stores RawAllocator directly
  template <class RawAllocator>
  class reference_storage_impl<RawAllocator, reference_shared>
  {
  protected:
    reference_storage_impl() noexcept = default;

    reference_storage_impl(const RawAllocator& alloc) noexcept : alloc_(alloc) {}

    bool is_valid() const noexcept
    {
      return true;
    }

    RawAllocator& get_allocator() const noexcept
    {
      return alloc_;
    }

  private:
    mutable RawAllocator alloc_;
  };


  template <typename Alloc>
  using reference_type_t = decltype(detail::reference_type(typename allocator_traits<Alloc>::is_stateful{}, is_shared_allocator<Alloc>{}));
} // namespace detail*/


  namespace impl {
    template <allocator_c RawAllocator>
    class reference_storage {
    protected:
      static_assert(stateless_allocator_c<RawAllocator>);

      reference_storage() noexcept = default;

      reference_storage(const RawAllocator &) noexcept {}

      VK_nodiscard bool is_valid() const noexcept {
        return true;
      }

      RawAllocator &get_allocator() const noexcept {
        static RawAllocator alloc;
        return alloc;
      }
    };

    // reference to stateful: stores a pointer to an allocator
    template <typename RawAllocator> requires stateful_allocator_c<RawAllocator>
    class reference_storage<RawAllocator> {
    protected:
      reference_storage() noexcept : alloc_(nullptr) {}

      reference_storage(RawAllocator &allocator) noexcept : alloc_(&allocator) {}

      VK_nodiscard bool is_valid() const noexcept {
        return alloc_ != nullptr;
      }

      VK_nodiscard RawAllocator& get_allocator() const noexcept {
        VK_assert(alloc_ != nullptr);
        return *alloc_;
      }

    private:
      RawAllocator* alloc_;
    };

    // reference to shared: stores RawAllocator directly
    template <typename RawAllocator> requires shared_allocator_c<RawAllocator>
    class reference_storage<RawAllocator> {
    protected:
      reference_storage() noexcept = default;

      reference_storage(const RawAllocator &alloc) noexcept : alloc_(alloc) {}

      VK_nodiscard bool is_valid() const noexcept {
        return true;
      }

      VK_nodiscard RawAllocator& get_allocator() const noexcept {
        return alloc_;
      }

    private:
      mutable RawAllocator alloc_;
    };
  }  // namespace impl


  /// A \concept{concept_storagepolicy,StoragePolicy} that stores a reference to an allocator.
  /// For stateful allocators it only stores a pointer to an allocator object and copying/moving only copies the pointer.
  /// For stateless allocators it does not store anything, an allocator will be constructed as needed.
  /// For allocators that are already shared (determined through \ref is_shared_allocator) it will store the allocator type directly.
  /// \note It does not take ownership over the allocator in the stateful case, the user has to ensure that the allocator object stays valid.
  /// In the other cases the lifetime does not matter.
  /// \ingroup storage
  template <class RawAllocator>
  class reference_storage : impl::reference_storage<RawAllocator> {
    using storage = impl::reference_storage<RawAllocator>;

  public:
    using allocator_type = typename allocator_traits<RawAllocator>::allocator_type;

    /// Default constructor.
    /// \effects If the allocator is stateless, this has no effect and the object is usable as an allocator.
    /// If the allocator is stateful, creates an invalid reference without any associated allocator.
    /// Then it must not be used.
    /// If the allocator is shared, default constructs the shared allocator.
    /// If the shared allocator does not have a default constructor, this constructor is ill-formed.
    reference_storage() noexcept = default;

    /// \effects Creates it from a stateless or shared allocator.
    /// It will not store anything, only creates the allocator as needed.
    /// \requires The \c RawAllocator is stateless or shared.
    reference_storage(const allocator_type &alloc) noexcept : storage(alloc) {}

    /// \effects Creates it from a reference to a stateful allocator.
    /// It will store a pointer to this allocator object.
    /// \note The user has to take care that the lifetime of the reference does not exceed the allocator lifetime.
    reference_storage(allocator_type &alloc) noexcept : storage(alloc) {}

    /// @{
    /// \effects Copies the \c allocator_reference object.
    /// Only copies the pointer to it in the stateful case.
    reference_storage(const reference_storage &) noexcept = default;
    reference_storage &operator=(const reference_storage &) noexcept = default;
    /// @}

    /// \returns Whether or not the reference is valid.
    /// It is only invalid, if it was created by the default constructor and the allocator is stateful.
    explicit operator bool() const noexcept {
      return storage::is_valid();
    }

    /// \returns Returns a reference to the allocator.
    /// \requires The reference must be valid.
    allocator_type &get_allocator() const noexcept {
      return storage::get_allocator();
    }

    ~reference_storage() noexcept = default;

  protected:
    VK_nodiscard bool is_composable() const noexcept {
      return composable_allocator_c<allocator_type>;
    }
  };

  /// Specialization of the class template \ref reference_storage that is type-erased.
  /// It is triggered by the tag type \ref any_allocator.
  /// The specialization can store a reference to any allocator type.
  /// \ingroup storage
  template <>
  class reference_storage<any_allocator> {
    class base_allocator {
    public:
      VK_constant bool is_stateful                     = true;

      virtual ~base_allocator()                        = default;

      virtual void clone(void *storage) const noexcept = 0;

      void *allocate_node(u64 size, u64 alignment) {
        return allocate_impl(1, size, alignment);
      }
      void *allocate_array(u64 count, u64 size, u64 alignment) {
        return allocate_impl(count, size, alignment);
      }
      void deallocate_node(void *node, u64 size, u64 alignment) noexcept {
        deallocate_impl(node, 1, size, alignment);
      }
      void deallocate_array(void *array, u64 count, u64 size, u64 alignment) noexcept {
        deallocate_impl(array, count, size, alignment);
      }

      void *try_allocate_node(u64 size, u64 alignment) noexcept {
        return try_allocate_impl(1, size, alignment);
      }
      void *try_allocate_array(u64 count, u64 size, u64 alignment) noexcept {
        return try_allocate_impl(count, size, alignment);
      }
      bool try_deallocate_node(void *node, u64 size, u64 alignment) noexcept {
        return try_deallocate_impl(node, 1, size, alignment);
      }
      bool try_deallocate_array(void *array, u64 count, u64 size, u64 alignment) noexcept {
        return try_deallocate_impl(array, count, size, alignment);
      }

      // count 1 means node
      virtual void *allocate_impl(u64 count, u64 size, u64 alignment)                          = 0;
      virtual void deallocate_impl(void *ptr, u64 count, u64 size, u64 alignment) noexcept     = 0;
      virtual void *try_allocate_impl(u64 count, u64 size, u64 alignment) noexcept             = 0;
      virtual bool try_deallocate_impl(void *ptr, u64 count, u64 size, u64 alignment) noexcept = 0;

      VK_nodiscard u64 max_node_size() const {
        return max(query::node_size);
      }
      VK_nodiscard u64 max_array_size() const {
        return max(query::array_size);
      }
      VK_nodiscard u64 max_alignment() const {
        return max(query::alignment);
      }

      VK_nodiscard virtual bool is_composable() const noexcept = 0;

    protected:
      enum class query {
        node_size,
        array_size,
        alignment
      };

      VK_nodiscard virtual u64 max(query q) const = 0;
    };

  public:
    using allocator_type = base_allocator;

    /// \effects Creates it from a reference to any stateful \concept{concept_rawallocator,RawAllocator}.
    /// It will store a pointer to this allocator object.
    /// \note The user has to take care that the lifetime of the reference does not exceed the allocator lifetime.
    template <typename RawAllocator>
    reference_storage(RawAllocator &alloc) noexcept {
      static_assert(sizeof(basic_allocator<RawAllocator>) <= sizeof(basic_allocator<default_instantiation>),
                    "requires all instantiations to have certain maximum size");
      ::new (static_cast<void *>(&storage_)) basic_allocator<RawAllocator>(alloc);
    }

    // \effects Creates it from any stateless \concept{concept_rawallocator,RawAllocator}.
    /// It will not store anything, only creates the allocator as needed.
    /// \requires The \c RawAllocator is stateless.
    template <stateless_allocator_c RawAllocator>
    reference_storage(const RawAllocator &alloc) noexcept {
      static_assert(sizeof(basic_allocator<RawAllocator>) <= sizeof(basic_allocator<default_instantiation>),
                    "requires all instantiations to have certain maximum size");
      ::new (static_cast<void *>(&storage_)) basic_allocator<RawAllocator>(alloc);
    }

    /// \effects Creates it from the internal base class for the type-erasure.
    /// Has the same effect as if the actual stored allocator were passed to the other constructor overloads.
    /// \note This constructor is used internally to avoid double-nesting.
    reference_storage(const base_allocator &alloc) noexcept {
      alloc.clone(&storage_);
    }

    /// @{
    /// \effects Copies the \c reference_storage object.
    /// It only copies the pointer to the allocator.
    reference_storage(const reference_storage &other) noexcept {
      other.get_allocator().clone(&storage_);
    }

    reference_storage &operator=(const reference_storage &other) noexcept {
      if ( this != &other )
        return *this;

      get_allocator().~allocator_type();
      other.get_allocator().clone(&storage_);
      return *this;
    }
    /// @}



    /// \returns A reference to the allocator.
    /// The actual type is implementation-defined since it is the base class used in the type-erasure,
    /// but it provides the full \concept{concept_rawallocator,RawAllocator} member functions.
    /// \note There is no way to access any custom member functions of the allocator type.
    allocator_type &get_allocator() const noexcept {
      auto mem = static_cast<void *>(&storage_);
      return *static_cast<base_allocator *>(mem);
    }

  protected:
    ~reference_storage() noexcept {
      get_allocator().~allocator_type();
    }

    VK_nodiscard bool is_composable() const noexcept {
      return get_allocator().is_composable();
    }

  private:
    template <class RawAllocator>
    class basic_allocator : public base_allocator, private impl::reference_storage<RawAllocator> {
      using trt           = allocator_traits<RawAllocator>;
      using allocator_type_t = typename trt::allocator_type;
      using storage          = impl::reference_storage<RawAllocator>;

    public:
      // non stateful
      basic_allocator(const RawAllocator &alloc) noexcept : storage(alloc) {}

      // stateful
      basic_allocator(RawAllocator &alloc) noexcept : storage(alloc) {}

    private:
      VK_nodiscard allocator_type_t& get() const noexcept {
        return storage::get_allocator();
      }

      void clone(void *storage) const noexcept override {
        ::new (storage) basic_allocator(get());
      }

      void *allocate_impl(u64 count, u64 size, u64 alignment) override {
        auto &&alloc = get();
        if (count == 1u)
          return trt::allocate_node(alloc, size, alignment);
        else
          return trt::allocate_array(alloc, count, size, alignment);
      }

      void deallocate_impl(void *ptr, u64 count, u64 size, u64 alignment) noexcept override {
        auto &&alloc = get();
        if (count == 1u)
          trt::deallocate_node(alloc, ptr, size, alignment);
        else
          trt::deallocate_array(alloc, ptr, count, size, alignment);
      }

      void *try_allocate_impl(u64 count, u64 size, u64 alignment) noexcept override {
        if constexpr ( trt::is_composable ) {
          auto &&alloc = get();
          if (count == 1u)
            return trt::try_allocate_node(alloc, size, alignment);
          return trt::try_allocate_array(alloc, count, size, alignment);
        }
        VK_unreachable;
        return nullptr;
      }

      bool try_deallocate_impl(void *ptr, u64 count, u64 size, u64 alignment) noexcept override {
        if constexpr ( trt::is_composable ) {
          auto &&alloc = get();
          if (count == 1u)
            return trt::try_deallocate_node(alloc, ptr, size, alignment);
          return trt::try_deallocate_array(alloc, ptr, count, size, alignment);
        }
        VK_unreachable;
        return false;
      }

      VK_nodiscard bool is_composable() const noexcept override {
        return trt::is_composable;
      }

      VK_nodiscard u64 max(query q) const override {
        auto &&alloc = get();
        if (q == query::node_size)
          return trt::max_node_size(alloc);
        else if (q == query::array_size)
          return trt::max_array_size(alloc);
        return trt::max_alignment(alloc);
      }
    };

    // use a stateful instantiation to determine size and alignment
    // base_allocator is stateful
    using default_instantiation = basic_allocator<base_allocator>;
    alignas(default_instantiation) mutable char storage_[sizeof(default_instantiation)];
  };

  /// An alias template for \ref allocator_storage using the \ref reference_storage policy.
  /// It will store a reference to the given allocator type. The tag type \ref any_allocator enables type-erasure.
  /// Wrap the allocator in a \ref thread_safe_allocator if you want thread safety.
  /// \ingroup storage
  template <class RawAllocator>
  using allocator_reference = allocator_storage<reference_storage<RawAllocator>, noop_mutex>;

  /// \returns A new \ref allocator_reference object by forwarding the allocator to the constructor.
  /// \relates allocator_reference
  template <class RawAllocator>
  auto make_allocator_reference(RawAllocator &&allocator) noexcept
      -> allocator_reference<std::decay_t<RawAllocator>> {
    return {std::forward<RawAllocator>(allocator)};
  }

  /// An alias for the \ref reference_storage specialization using type-erasure.
  /// \ingroup storage
  using any_reference_storage = reference_storage<any_allocator>;

  /// An alias for \ref allocator_storage using the \ref any_reference_storage.
  /// It will store a reference to any \concept{concept_rawallocator,RawAllocator}.
  /// This is the same as passing the tag type \ref any_allocator to the alias \ref allocator_reference.
  /// Wrap the allocator in a \ref thread_safe_allocator if you want thread safety.
  /// \ingroup storage
  using any_allocator_reference = allocator_storage<any_reference_storage, noop_mutex>;


  static_assert(memory_storage_policy_c<any_reference_storage>);



  /// \returns A new \ref any_allocator_reference object by forwarding the allocator to the constructor.
  /// \relates any_allocator_reference
  template <typename RawAllocator> requires allocator_c<remove_cvref_t<RawAllocator>>
  any_allocator_reference make_any_allocator_reference(RawAllocator &&allocator) noexcept {
    return any_allocator_reference(std::forward<RawAllocator>(allocator));
  }
}  // namespace valkyrie

#endif  //VALKYRIE_MEMORY_ALLOCATOR_STORAGE_HPP
