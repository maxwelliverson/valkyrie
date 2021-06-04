//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_TRACKING_HPP
#define VALKYRIE_MEMORY_TRACKING_HPP


#include <valkyrie/traits.hpp>

#include "detail/utility.hpp"
#include "memory_arena.hpp"

namespace valkyrie {
  namespace detail {

    template <typename Alloc,
              typename Tracker> requires(
        memory_tracker_c<Tracker>       &&
        requires(Alloc& a, Tracker* t){ a.get_allocator().set_tracker(t); })
    inline decltype(auto) set_tracker(Alloc& alloc, Tracker* tracker) noexcept {
      return alloc.get_allocator().set_tracker(tracker);
    }
    template <typename Alloc, typename Tracker>
    inline void set_tracker(Alloc&, Tracker*) noexcept { }


    // used with deeply_tracked_allocator
    template<class Tracker, class BlockAllocator>
    class deeply_tracked_block_allocator : BlockAllocator {
    public:
      template<typename... Args>
      deeply_tracked_block_allocator(u64 block_size, Args &&...args)
          : BlockAllocator(block_size, std::forward<Args>(args)...), tracker_(nullptr) { }

      memory_block allocate_block() {
        auto block = BlockAllocator::allocate_block();
        if (tracker_)// on first call tracker_ is nullptr
          tracker_->on_allocator_growth(block.memory, block.size);
        return block;
      }

      void deallocate_block(memory_block block) noexcept {
        if (tracker_)// on last call tracker_ is nullptr again
          tracker_->on_allocator_shrinking(block.memory, block.size);
        BlockAllocator::deallocate_block(block);
      }

      u64 next_block_size() const noexcept {
        return BlockAllocator::next_block_size();
      }

      void set_tracker(Tracker *tracker) noexcept {
        tracker_ = tracker;
      }

    private:
      Tracker *tracker_;
    };
  }// namespace detail

  /// A \concept{concept_blockallocator,BlockAllocator} adapter that tracks another allocator using a \concept{concept_tracker,tracker}.
  /// It wraps another \concept{concept_blockallocator,BlockAllocator} and calls the tracker function before forwarding to it.
  /// The class can then be used anywhere a \concept{concept_blockallocator,BlockAllocator} is required and the memory usage will be tracked.<br>
  /// It will only call the <tt>on_allocator_growth()</tt> and <tt>on_allocator_shrinking()</tt> tracking functions,
  /// since a \concept{concept_blockallocator,BlockAllocator} is normally used inside higher allocators only.
  /// \ingroup adapter
  template<memory_deep_tracker_c Tracker, block_or_raw_allocator_c BlockOrRawAllocator>
  class tracked_block_allocator : Tracker, make_block_allocator_t<BlockOrRawAllocator> {
  public:
    using allocator_type = make_block_allocator_t<BlockOrRawAllocator>;
    using tracker = Tracker;

    /// @{
    /// \effects Creates it by giving it a \concept{concept_tracker,tracker} and the tracked \concept{concept_rawallocator,RawAllocator}.
    /// It will embed both objects.
    explicit tracked_block_allocator(tracker t = {}) noexcept : tracker(std::move(t)) {}

    tracked_block_allocator(tracker t, allocator_type &&alloc) noexcept
        : tracker(std::move(t)), allocator_type(std::move(alloc)) { }
    /// @}

    /// \effects Creates it in the form required by the concept.
    /// The allocator will be constructed using \c block_size and \c args.
    template<typename... Args>
    tracked_block_allocator(u64 block_size, tracker t, Args &&...args)
        : tracker(std::move(t)), allocator_type(block_size, std::forward<Args>(args)...) { }

    /// \effects Calls <tt>Tracker::on_allocator_growth()</tt> after forwarding to the allocator.
    /// \returns The block as the returned by the allocator.
    memory_block allocate_block() {
      auto block = allocator_type::allocate_block();
      this->on_allocator_growth(block.memory, block.size);
      return block;
    }

    /// \effects Calls <tt>Tracker::on_allocator_shrinking()</tt> and forwards to the allocator.
    void deallocate_block(memory_block block) noexcept {
      this->on_allocator_shrinking(block.memory, block.size);
      allocator_type::deallocate_block(block);
    }

    /// \returns The next block size as returned by the allocator.
    u64 next_block_size() const noexcept {
      return allocator_type::next_block_size();
    }

    /// @{
    /// \returns A (const) reference to the used allocator.
    allocator_type &get_allocator() noexcept {
      return *this;
    }

    const allocator_type &get_allocator() const noexcept {
      return *this;
    }
    /// @}

    /// @{
    /// \returns A (const) reference to the tracker.
    tracker &get_tracker() noexcept {
      return *this;
    }

    const tracker &get_tracker() const noexcept {
      return *this;
    }
    /// @}
  };

  /// Similar to \ref tracked_block_allocator, but shares the tracker with the higher level allocator.
  /// This allows tracking both (de-)allocations and growth with one tracker.
  /// \note Due to implementation reasons, it cannot track growth and shrinking in the constructor/destructor of the higher level allocator.
  /// \ingroup adapter
  template<memory_deep_tracker_c Tracker, block_or_raw_allocator_c Alloc>
  using deeply_tracked_block_allocator = detail::deeply_tracked_block_allocator<Tracker, make_block_allocator_t<Alloc>>;

  /// A \concept{concept_rawallocator,RawAllocator} adapter that tracks another allocator using a \concept{concept_tracker,tracker}.
  /// It wraps another \concept{concept_rawallocator,RawAllocator} and calls the tracker function before forwarding to it.
  /// The class can then be used anywhere a \concept{concept_rawallocator,RawAllocator} is required and the memory usage will be tracked.<br>
  /// If the \concept{concept_rawallocator,RawAllocator} uses \ref deeply_tracked_block_allocator as \concept{concept_blockallocator,BlockAllocator},
  /// it will also track growth and shrinking of the allocator.
  /// \ingroup adapter
  template<class Tracker, allocator_c RawAllocator>
  class tracked_allocator : Tracker, allocator_traits<RawAllocator>::allocator_type {
    using trt = allocator_traits<RawAllocator>;

  public:
    using allocator_type = typename allocator_traits<RawAllocator>::allocator_type;
    using tracker = Tracker;

    VK_constant bool is_stateful = trt::is_stateful || !std::is_empty_v<Tracker>;
    VK_constant bool is_composable = trt::is_composable;

    /// @{
    /// \effects Creates it by giving it a \concept{concept_tracker,tracker} and the tracked \concept{concept_rawallocator,RawAllocator}.
    /// It will embed both objects.
    /// \note This will never call the <tt>Tracker::on_allocator_growth()</tt> function.
    explicit tracked_allocator(tracker t = {}) noexcept 
        : tracked_allocator(std::move(t), allocator_type{}) { }

    tracked_allocator(tracker t, allocator_type &&allocator) noexcept
        : tracker(std::move(t)), 
          allocator_type(std::move(allocator)) { 
      detail::set_tracker(get_allocator(), &get_tracker());
    }
    /// @}

    /// \effects Destroys both tracker and allocator.
    /// \note This will never call the <tt>Tracker::on_allocator_shrinking()</tt> function.
    ~tracked_allocator() noexcept {
      detail::set_tracker(get_allocator(), static_cast<tracker *>(nullptr));
    }

    /// @{
    /// \effects Moving moves both the tracker and the allocator.
    tracked_allocator(tracked_allocator &&other) noexcept
        : tracker(std::move(other)), 
          allocator_type(std::move(other)) {
      detail::set_tracker(get_allocator(), &get_tracker());
    }

    tracked_allocator &operator=(tracked_allocator &&other) noexcept {
      tracker::operator=(std::move(other));
      allocator_type::operator=(std::move(other));
      detail::set_tracker(get_allocator(), &get_tracker());
      return *this;
    }
    /// @}

    /// \effects Calls <tt>Tracker::on_node_allocation()</tt> and forwards to the allocator.
    /// If a growth occurs and the allocator is deeply tracked, also calls <tt>Tracker::on_allocator_growth()</tt>.
    /// \returns The result of <tt>allocate_node()</tt>
    void *allocate_node(u64 size, u64 alignment) {
      auto mem = trt::allocate_node(get_allocator(), size, alignment);
      this->on_node_allocation(mem, size, alignment);
      return mem;
    }

    /// \effects Calls the composable node allocation function.
    /// If allocation was successful, also calls `Tracker::on_node_allocation()`.
    /// \returns The result of `try_allocate_node()`.
    void *try_allocate_node(u64 size, u64 alignment) noexcept requires(is_composable) {
      auto mem = trt::try_allocate_node(get_allocator(), size, alignment);
      if (mem)
        this->on_node_allocation(mem, size, alignment);
      return mem;
    }

    /// \effects Calls <tt>Tracker::on_array_allocation()</tt> and forwards to the allocator.
    /// If a growth occurs and the allocator is deeply tracked, also calls <tt>Tracker::on_allocator_growth()</tt>.
    /// \returns The result of <tt>allocate_array()</tt>
    void *allocate_array(u64 count, u64 size, u64 alignment) {
      auto mem = trt::allocate_array(get_allocator(), count, size, alignment);
      this->on_array_allocation(mem, count, size, alignment);
      return mem;
    }

    /// \effects Calls the composable array allocation function.
    /// If allocation was succesful, also calls `Tracker::on_array_allocation()`.
    /// \returns The result of `try_allocate_array()`.
    void *try_allocate_array(u64 count, u64 size, u64 alignment) noexcept requires(is_composable) {
      auto mem = trt::try_allocate_array(get_allocator(), count, size, alignment);
      if (mem)
        this->on_array_allocation(mem, count, size, alignment);
      return mem;
    }

    /// \effects Calls <tt>Tracker::on_node_deallocation()</tt> and forwards to the allocator's <tt>deallocate_node()</tt>.
    /// If shrinking occurs and the allocator is deeply tracked, also calls <tt>Tracker::on_allocator_shrinking()</tt>.
    void deallocate_node(void *ptr, u64 size, u64 alignment) noexcept {
      this->on_node_deallocation(ptr, size, alignment);
      trt::deallocate_node(get_allocator(), ptr, size, alignment);
    }

    /// \effects Calls the composable node deallocation function.
    /// If it was succesful, also calls `Tracker::on_node_deallocation()`.
    /// \returns The result of `try_deallocate_node()`.
    bool try_deallocate_node(void *ptr, u64 size, u64 alignment) noexcept requires(is_composable) {
      auto res = trt::try_deallocate_node(get_allocator(), ptr, size, alignment);
      if (res)
        this->on_node_deallocation(ptr, size, alignment);
      return res;
    }

    /// \effects Calls <tt>Tracker::on_array_deallocation()</tt> and forwards to the allocator's <tt>deallocate_array()</tt>.
    /// If shrinking occurs and the allocator is deeply tracked, also calls <tt>Tracker::on_allocator_shrinking()</tt>.
    void deallocate_array(void *ptr, u64 count, u64 size, u64 alignment) noexcept {
      this->on_array_deallocation(ptr, count, size, alignment);
      trt::deallocate_array(get_allocator(), ptr, count, size, alignment);
    }

    /// \effects Calls the composable array deallocation function.
    /// If it was succesful, also calls `Tracker::on_array_deallocation()`.
    /// \returns The result of `try_deallocate_array()`.
    bool try_deallocate_array(void *ptr, u64 count, u64 size, u64 alignment) noexcept requires(is_composable){
      auto res = trt::try_deallocate_array(ptr, count, size, alignment);
      if (res)
        this->on_array_deallocation(ptr, count, size, alignment);
      return res;
    }

    /// @{
    /// \returns The result of the corresponding function on the wrapped allocator.
    u64 max_node_size() const {
      return trt::max_node_size(get_allocator());
    }
    u64 max_array_size() const {
      return trt::max_array_size(get_allocator());
    }
    u64 max_alignment() const {
      return trt::max_alignment(get_allocator());
    }
    /// @}

    /// @{
    /// \returns A (\c const) reference to the wrapped allocator.
    allocator_type &get_allocator() noexcept {
      return *this;
    }

    const allocator_type &get_allocator() const noexcept {
      return *this;
    }
    /// @}

    /// @{
    /// \returns A (\c const) reference to the tracker.
    tracker &get_tracker() noexcept {
      return *this;
    }

    const tracker &get_tracker() const noexcept {
      return *this;
    }
    /// @}
  };

  /// \effects Takes a \concept{concept_rawallocator,RawAllocator} and wraps it with a \concept{concept_tracker,tracker}.
  /// \returns A \ref tracked_allocator with the corresponding parameters forwarded to the constructor.
  /// \relates tracked_allocator
  template <typename Tracker, typename RawAllocator>
  requires(memory_tracker_c<Tracker> && allocator_c<remove_cvref_t<RawAllocator>>)
  auto make_tracked_allocator(Tracker t, RawAllocator &&alloc) {
    return tracked_allocator<Tracker, remove_cvref_t<RawAllocator>>{std::move(t), std::move(alloc)};
  }

  namespace impl {
    template<class RawAllocator, class BlockAllocator>
    struct rebind_block_allocator;

    template<template <typename...> typename RawAllocator, typename... Args, typename OtherBlockAllocator>
    struct rebind_block_allocator<RawAllocator<Args...>, OtherBlockAllocator> {
      using type = RawAllocator<std::conditional_t<block_or_raw_allocator_c<Args>, OtherBlockAllocator, Args>...>;
    };

    template<typename Tracker, typename Alloc>
    using deeply_tracked_block_allocator_for =
        deeply_tracked_block_allocator<Tracker, typename Alloc::allocator_type>;

    template<typename Tracker, typename Alloc>
    using rebound_allocator = typename rebind_block_allocator<Alloc, deeply_tracked_block_allocator_for<Tracker, Alloc>>::type;
  }// namespace detail

  /// A \ref tracked_allocator that has rebound any \concept{concept_blockallocator,BlockAllocator} to the corresponding \ref deeply_tracked_block_allocator.
  /// This makes it a deeply tracked allocator.<br>
  /// It replaces each template argument of the given \concept{concept_rawallocator,RawAllocator} for which \ref is_block_allocator or \ref is_raw_allocator is \c true with a \ref deeply_tracked_block_allocator.
  /// \ingroup adapter
  template<class Tracker, class RawAllocator>
  using deeply_tracked_allocator = tracked_allocator<Tracker, impl::rebound_allocator<Tracker, RawAllocator>>;

  /// \effects Takes a \concept{concept_rawallocator,RawAllocator} and deeply wraps it with a \concept{concept_tracker,tracker}.
  /// \returns A \ref deeply_tracked_allocator with the corresponding parameters forwarded to the constructor.
  /// \relates deeply_tracked_allocator
  template<typename RawAllocator, typename Tracker, typename... Args>
  requires(allocator_c<RawAllocator> && memory_deep_tracker_c<Tracker> && constructible_from<>)
  auto make_deeply_tracked_allocator(Tracker t, Args &&...args) -> deeply_tracked_allocator<Tracker, RawAllocator> {
    return deeply_tracked_allocator<Tracker, RawAllocator>(std::move(t), {std::forward<Args>(args)...});
  }
}// namespace valkyrie

#endif//VALKYRIE_MEMORY_TRACKING_HPP
