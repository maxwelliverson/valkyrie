//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_DETAIL_EBO_STORAGE_HPP
#define VALKYRIE_MEMORY_DETAIL_EBO_STORAGE_HPP

namespace valkyrie::detail{
  template <int Tag, typename T>
  class ebo_storage : T
  {
  protected:
    ebo_storage(const T& t) : T(t) {}

    ebo_storage(T&& t) noexcept(std::is_nothrow_move_constructible<T>::value)
        : T(detail::move(t))
        {
        }

    T& get() noexcept
    {
      return *this;
    }

    const T& get() const noexcept
    {
      return *this;
    }
  };
}

#endif//VALKYRIE_MEMORY_DETAIL_EBO_STORAGE_HPP
