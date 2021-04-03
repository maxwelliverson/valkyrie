//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ADT_TEMP_ARRAY_HPP
#define VALKYRIE_ADT_TEMP_ARRAY_HPP

#include <valkyrie/adt/array_ref.hpp>
#include <valkyrie/adt/ptr.hpp>
#include <valkyrie/memory/default_allocator.hpp>


namespace valkyrie{

  namespace detail{
    template <typename RawAlloc, typename Type = reference_type_t<RawAlloc>>
    struct allocator_state_param{
      using type = const typename allocator_traits<RawAlloc>::allocator_type&;
    };
    template <typename RawAlloc>
    struct allocator_state_param<RawAlloc, reference_stateful>{
      using type = typename allocator_traits<RawAlloc>::allocator_type&;
    };
  }

  template <typename T, raw_allocator Alloc = default_allocator, typename Ptr = borrowed_ptr<T>>
  class temp_array {

    //using alloc_state_t = typename allocator_traits<Alloc>::allocator_type;
    using alloc_param_t = typename detail::allocator_state_param<Alloc>::type;

  public:

    using allocator_type  = allocator_reference<Alloc>;

    using size_type       = size_t;
    using difference_type = i64;
    using value_type      = T;
    using pointer         = Ptr;
    using reference       = typename pointer::reference;
    using const_pointer   = ptr_rebind_t<const value_type, pointer>;
    using const_reference = typename const_pointer::reference;

    using iterator = pointer;
    using const_iterator = const_pointer;
    using sentinel = iterator;
    using const_sentinel = const_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_sentinel = reverse_iterator;
    using const_reverse_sentinel = const_reverse_iterator;

    temp_array() = default;
    temp_array(const temp_array &) = delete;
    temp_array(temp_array && other) noexcept
        : allocator_(std::move(other.allocator_)), ptr_(other.ptr_), size_(other.size_){
      other.ptr_ = nullptr;
      other.size_ = 0; // not strictly necessary?
    }

    explicit temp_array(size_t length_) requires(!stateful_allocator<Alloc>) : temp_array(length_, {}){}
    temp_array(size_type length_, )
    temp_array(size_t length_, alloc_param_t allocator)
        : allocator_(std::move(allocator)), ptr_(), size_(length_){
      if constexpr (!std::is_trivially_default_constructible_v<T>) {
        for (size_t i = 0; i < length_; ++i) {
          alloc_traits::construct(allocator_, ptr_ + i);
        }
      }
    }

    ~temp_array() {
      if (ptr_) {
        if constexpr (!std::is_trivially_destructible_v<T>) {
          for (size_t i = 0; i < size_; ++i)
            std::allocator_traits<Alloc>::destroy(allocator_, ptr_ + i);
        }
        std::allocator_traits<Alloc>::deallocate(allocator_, ptr_, size_);
      }
    }



    reference operator[](size_type offset) const noexcept {
      VK_assert(offset < size_);
      return ptr_[offset];
    }


    size_type size() const noexcept {
      return size_;
    }
    pointer data() const noexcept {
      return ptr_;
    }


    iterator  begin() const noexcept {
      return ptr_;
    }
    const_iterator cbegin() const noexcept {
      return ptr_;
    }

    sentinel end() const noexcept {
      return ptr_ + size_;
    }
    const_sentinel cend() const noexcept {
      return end();
    }

    reverse_iterator  rbegin() const noexcept {
      return reverse_iterator(end());
    }
    const_reverse_iterator crbegin() const noexcept {
      return const_reverse_iterator(cend());
    }

    reverse_sentinel rend() const noexcept {
      return reverse_sentinel(begin());
    }
    const_reverse_sentinel crend() const noexcept {
      return const_reverse_sentinel(cbegin());
    }


  private:
    [[no_unique_address]] allocator_type allocator_{};
    pointer                              ptr_  = nullptr;
    size_type                            size_ = 0;
  };
}

#endif //VALKYRIE_ADT_TEMP_ARRAY_HPP
