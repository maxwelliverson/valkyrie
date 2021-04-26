//
// Created by maxwe on 2021-04-25.
//

#ifndef VALKYRIE_AGENT_DETAIL_MAILBOX_TRAITS_HPP
#define VALKYRIE_AGENT_DETAIL_MAILBOX_TRAITS_HPP

#include <valkyrie/constants.hpp>
#include <valkyrie/traits.hpp>
#include <valkyrie/async/semaphore.hpp>

#include "agent_concepts.hpp"

namespace valkyrie::impl{

  template <std::unsigned_integral U>
  VK_constant bool high_bit_is_set(U value) noexcept {
    return value >> ((sizeof(U) * CHAR_BIT) - 1);
  }

  template <u64 Count>
  struct mailbox_semaphore{
    using type = counting_semaphore<Count>;
  };
  template <u64 Count> requires (high_bit_is_set(Count))
  struct mailbox_semaphore<Count>{
    using type = noop_semaphore;
  };

  template <u64 Count>
  using mailbox_semaphore_t = typename mailbox_semaphore<Count>::type;



  template <typename T>
  struct mailbox_message_size{
    VK_constant bool message_size_is_dynamic = false;
    VK_constant u64 message_size = T::message_size;
  };
  template <typename T> requires(same_as<decltype(T::message_size), dynamic_t>)
  struct mailbox_message_size<T>{
    VK_constant bool message_size_is_dynamic = true;
  };

  // Writer traits

  template <typename T>
  struct mailbox_max_writers{
    VK_constant u64  max_writers         = std::numeric_limits<u64>::max();
  };
  template <typename T> requires(requires{ { T::max_writers } -> std::convertible_to<u64>; })
  struct mailbox_max_writers<T>{
    VK_constant u64  max_writers         = T::max_writers;
  };
  template <typename T> requires(requires{ T::single_writer; } || requires{ typename T::single_writer; })
  struct mailbox_max_writers<T>{
    VK_constant u64  max_writers         = 1;
  };

  template <typename T>
  struct mailbox_is_write_coherent{
    VK_constant bool is_write_coherent = mailbox_max_writers<T>::max_writers == 1;
  };
  template <typename T> requires(requires{ { T::is_write_coherent } -> same_as<bool>; })
  struct mailbox_is_write_coherent<T>{
    VK_constant bool is_write_coherent = T::is_write_coherent;
  };

  template <typename T>
  struct mailbox_writer_semaphore{
    using writer_semaphore_type = mailbox_semaphore_t<mailbox_max_writers<T>::max_writers>;
  };
  template <typename T> requires(requires{ typename T::writer_semaphore_type; })
  struct mailbox_writer_semaphore<T>{
    using writer_semaphore_type = typename T::writer_semaphore_type;
  };


  // Reader Traits

  template <typename T>
  struct mailbox_max_readers{
    VK_constant u64  max_readers         = 1;
  };
  template <typename T> requires(requires{ { T::max_readers } -> std::convertible_to<u64>; })
  struct mailbox_max_readers<T>{
    VK_constant u64  max_readers         = T::max_readers;
  };
  template <typename T> requires(requires{ T::many_readers; } || requires{ typename T::many_readers; })
  struct mailbox_max_readers<T>{
    VK_constant u64  max_readers         = std::numeric_limits<u64>::max();
  };

  template <typename T>
  struct mailbox_is_read_coherent{
    VK_constant bool is_read_coherent = true;
  };
  template <typename T> requires(requires{ { T::is_read_coherent } -> same_as<bool>; })
  struct mailbox_is_read_coherent<T>{
    VK_constant bool is_read_coherent = T::is_read_coherent;
  };

  template <typename T>
  struct mailbox_reader_semaphore{
    using reader_semaphore_type = mailbox_semaphore_t<mailbox_max_readers<T>::max_readers>;
  };
  template <typename T> requires(requires{ typename T::reader_semaphore_type; })
  struct mailbox_reader_semaphore<T>{
    using reader_semaphore_type = typename T::reader_semaphore_type;
  };



  template <typename T>
  struct mailbox_is_shared{
    VK_constant bool is_shared = false;
  };
  template <typename T> requires requires{ { T::is_shared } -> same_as<bool>; }
  struct mailbox_is_shared<T>{
    VK_constant bool is_shared = T::is_shared;
  };


  template <typename T>
  struct mailbox_traits
      : mailbox_message_size<T>,
        mailbox_max_writers<T>,
        mailbox_is_write_coherent<T>,
        mailbox_writer_semaphore<T>,
        mailbox_max_readers<T>,
        mailbox_is_read_coherent<T>,
        mailbox_reader_semaphore<T>,
        mailbox_is_shared<T>{ };
}

#endif  //VALKYRIE_AGENT_DETAIL_MAILBOX_TRAITS_HPP
