//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_DETAIL_SMALL_FREE_LIST_HPP
#define VALKYRIE_MEMORY_DETAIL_SMALL_FREE_LIST_HPP

namespace valkyrie::detail{
  struct chunk_base
  {
    chunk_base* prev = this;
    chunk_base* next = this;

    unsigned char first_free = 0; // first free node for the linked list
    unsigned char capacity   = 0; // total number of free nodes available
    unsigned char no_nodes   = 0; // total number of nodes in memory

    chunk_base() noexcept = default;

    chunk_base(unsigned char no) noexcept : capacity(no), no_nodes(no)
    {
    }
  };

  struct chunk;

  // the same as free_memory_list but optimized for small node sizes
  // it is slower and does not support arrays
  // but has very small overhead
  // debug: allocate() and deallocate() mark memory as new and freed, respectively
  // node_size is increased via two times fence size and fence is put in front and after
  class small_free_memory_list
  {
  public:
    // minimum element size
    static constexpr u64 min_element_size = 1;
    // alignment
    static constexpr u64 min_element_alignment = 1;

    //=== constructor ===//
    small_free_memory_list(u64 node_size) noexcept;

    // does not own memory!
    small_free_memory_list(u64 node_size, void* mem,
                           u64 size) noexcept;

    small_free_memory_list(small_free_memory_list&& other) noexcept;

    ~small_free_memory_list() noexcept = default;

    small_free_memory_list& operator=(small_free_memory_list&& other) noexcept
    {
      small_free_memory_list tmp(std::move(other));
      swap(*this, tmp);
      return *this;
    }

    friend void swap(small_free_memory_list& a,
                     small_free_memory_list& b) noexcept;

    //=== insert/alloc/dealloc ===//
    // inserts new memory of given size into the free list
    // mem must be aligned for maximum alignment
    void insert(void* mem, u64 size) noexcept;

    // returns the usable size
    // i.e. how many memory will be actually inserted and usable on a call to insert()
    u64 usable_size(u64 size) const noexcept;

    // allocates a node big enough for the node size
    // pre: !empty()
    void* allocate() noexcept;

    // always returns nullptr, because array allocations are not supported
    void* allocate(u64) noexcept
    {
      return nullptr;
    }

    // deallocates the node previously allocated via allocate()
    void deallocate(void* node) noexcept;

    // forwards to insert()
    void deallocate(void* mem, u64 size) noexcept
    {
      insert(mem, size);
    }

    // hint for allocate() to be prepared to allocate n nodes
    // it searches for a chunk that has n nodes free
    // returns false, if there is none like that
    // never fails for n == 1 if not empty()
    // pre: capacity() >= n * node_size()
    bool find_chunk(u64 n) noexcept
    {
      return find_chunk_impl(n) != nullptr;
    }

    //=== getter ===//
    u64 node_size() const noexcept
    {
      return node_size_;
    }

    // the alignment of all nodes
    u64 alignment() const noexcept;

    // number of nodes remaining
    u64 capacity() const noexcept
    {
      return capacity_;
    }

    bool empty() const noexcept
    {
      return capacity_ == 0u;
    }

  private:
    u64 fence_size() const noexcept;

    chunk* find_chunk_impl(u64 n = 1) noexcept;
    chunk* find_chunk_impl(unsigned char* node, chunk_base* first,
                           chunk_base* last) noexcept;
    chunk* find_chunk_impl(unsigned char* node) noexcept;

    chunk_base  base_;
    u64 node_size_, capacity_;
    chunk_base *alloc_chunk_, *dealloc_chunk_;
  };

  // for some reason, this is required in order to define it
  void swap(small_free_memory_list& a, small_free_memory_list& b) noexcept;
}

#endif//VALKYRIE_MEMORY_DETAIL_SMALL_FREE_LIST_HPP
