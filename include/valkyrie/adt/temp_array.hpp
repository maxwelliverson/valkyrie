//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ADT_TEMP_ARRAY_HPP
#define VALKYRIE_ADT_TEMP_ARRAY_HPP

#include <valkyrie/adt/array_ref.hpp>
#include <valkyrie/adt/ptr.hpp>
#include <valkyrie/memory/default_allocator.hpp>


namespace valkyrie{

  template <typename T,
            allocator_c Alloc = default_allocator,
            typename Ptr = T*/*borrowed_ptr<T>*/>
  class temp_array {

    using alloc_traits = allocator_traits<Alloc>;

  public:

    using allocator_type  = typename alloc_traits::allocator_type;

    using size_type              = size_t;
    using difference_type        = i64;
    using value_type             = T;
    using pointer                = Ptr;
    using reference              = decltype(*std::declval<pointer>());
    using const_pointer          = ptr_rebind_t<const value_type, pointer>;
    using const_reference        = decltype(*std::declval<const_pointer>());

    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using sentinel               = iterator;
    using const_sentinel         = const_iterator;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_sentinel       = reverse_iterator;
    using const_reverse_sentinel = const_reverse_iterator;

    temp_array() = default;
    temp_array(const temp_array &) = delete;
    temp_array(temp_array && other) noexcept
        : allocator_(std::move(other.allocator_)),
          ptr_(other.ptr_),
          size_(other.size_){
      other.ptr_ = nullptr;
      other.size_ = 0; // not strictly necessary?
    }

    explicit temp_array(size_type length_) requires(std::is_default_constructible_v<allocator_type>)
        : temp_array(length_, allocator_type{}){}

    temp_array(size_type length, const allocator_type& allocator)
        : allocator_(allocator),
          ptr_(static_cast<pointer>(alloc_traits::allocate_array(allocator_, length, sizeof(value_type), alignof(value_type)))),
          size_(length) {
      if constexpr (!std::is_trivially_default_constructible_v<T>) {
        for (size_type i = 0; i < length; ++i) {
          alloc_traits::construct_object(allocator_, ptr_ + i);
        }
      }
    }
    temp_array(size_type length, allocator_type&& allocator)
        : allocator_(std::move(allocator)),
          ptr_(static_cast<pointer>(alloc_traits::allocate_array(allocator_, length, sizeof(value_type), alignof(value_type)))),
          size_(length){
      if constexpr (!std::is_trivially_default_constructible_v<T>) {
        for (size_t i = 0; i < length; ++i) {
          alloc_traits::construct_object(allocator_, ptr_ + i);
        }
      }
    }

    ~temp_array() {
      if (ptr_) {
        if constexpr (!std::is_trivially_destructible_v<T>) {
          for (size_t i = 0; i < size_; ++i)
            alloc_traits::destroy_object(allocator_, ptr_ + i);
        }
        alloc_traits::deallocate_array(allocator_, ptr_, size_, sizeof(T), alignof(T));
      }
    }



    VK_nodiscard reference front() const noexcept {
      return ptr_[0];
    }
    VK_nodiscard reference back() const noexcept {
      return ptr_[size_ - 1];
    }
    VK_nodiscard reference operator[](size_type offset) const noexcept {
      VK_assert(offset < size_);
      return ptr_[offset];
    }


    VK_nodiscard size_type size() const noexcept {
      return size_;
    }
    VK_nodiscard bool empty() const noexcept {
      return size_ == 0;
    }
    VK_nodiscard pointer data() const noexcept {
      return ptr_;
    }


    VK_nodiscard iterator  begin() const noexcept {
      return ptr_;
    }
    VK_nodiscard const_iterator cbegin() const noexcept {
      return ptr_;
    }

    VK_nodiscard sentinel end() const noexcept {
      return ptr_ + size_;
    }
    VK_nodiscard const_sentinel cend() const noexcept {
      return end();
    }

    VK_nodiscard reverse_iterator  rbegin() const noexcept {
      return reverse_iterator(end());
    }
    VK_nodiscard const_reverse_iterator crbegin() const noexcept {
      return const_reverse_iterator(cend());
    }

    VK_nodiscard reverse_sentinel rend() const noexcept {
      return reverse_sentinel(begin());
    }
    VK_nodiscard const_reverse_sentinel crend() const noexcept {
      return const_reverse_sentinel(cbegin());
    }


    void swap(temp_array& other) noexcept {
      if constexpr (alloc_traits::propagate_on_container_swap) {
        allocator_type tmp = std::move(allocator_);
        allocator_ = std::move(other.allocator_);
        other.allocator_ = std::move(tmp);
      }
      std::swap(ptr_, other.ptr_);
      std::swap(size_, other.size_);
    }

  private:
    [[no_unique_address]] allocator_type allocator_{};
                          pointer        ptr_  = nullptr;
                          size_type      size_ = 0;
  };
}

#endif //VALKYRIE_ADT_TEMP_ARRAY_HPP
