//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_CORE_ADT_TEMP_ARRAY_HPP
#define VALKYRIE_CORE_ADT_TEMP_ARRAY_HPP

#include <valkyrie/Core/ADT/ArrayRef.hpp>

#include <memory>
#include <utility>

namespace valkyrie::Core{

  template <typename T, typename Alloc = std::allocator<T>>
  class TempArray{

    using alloc_traits = std::allocator_traits<Alloc>;

  public:

    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using value_type = T;
    using pointer = typename alloc_traits::pointer;
    using reference = decltype(*std::declval<pointer>());
    using const_pointer = typename alloc_traits::const_pointer;
    using const_reference = decltype(*std::declval<const_pointer>());

    using iterator = pointer;
    using const_iterator = const_pointer;
    using sentinel = iterator;
    using const_sentinel = const_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_sentinel = reverse_iterator;
    using const_reverse_sentinel = const_reverse_iterator;

    TempArray() = default;
    TempArray(const TempArray&) = delete;
    TempArray(TempArray&& other) noexcept : allocator_(std::move(other.allocator_)), ptr_(other.ptr_), size_(other.size_){
      other.ptr_ = nullptr;
      other.size_ = 0; // not strictly necessary?
    }

    explicit TempArray(size_t length_) : TempArray(length_, {}){}
    TempArray(size_t length_, const Alloc& allocator) : allocator_(allocator), ptr_(alloc_traits::allocate(allocator_, length_)), size_(length_){
      if constexpr (!std::is_trivially_default_constructible_v<T>) {
        for (size_t i = 0; i < length_; ++i) {
          alloc_traits::construct(allocator_, ptr_ + i);
        }
      }
    }

    ~TempArray() {
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
    [[no_unique_address]] Alloc allocator_{};
    pointer                     ptr_ = nullptr;
    size_type                   size_ = 0;
  };



}

#endif //VALKYRIE_CORE_ADT_TEMP_ARRAY_HPP
