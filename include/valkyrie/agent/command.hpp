//
// Created by maxwe on 2021-03-20.
//

#ifndef VALKYRIE_COMMAND_HPP
#define VALKYRIE_COMMAND_HPP

#include <valkyrie/memory/default_allocator.hpp>

#include <utility>

namespace valkyrie{
  template <typename Alloc, typename ...T>
  class basic_command{

    using allocator_type = typename allocator_traits<Alloc>::allocator_type;

    class impl{
    public:
      virtual ~impl() = default;
      virtual void send_command(T&&...) noexcept = 0;
    };
    template <typename Sender, typename Msg>
    class derived_impl : public impl{
      Sender sender;
    public:
      void send_command(T&&... args) noexcept override{
        sender.template send<Msg>(std::forward<T>(args)...);
      }
    };

    impl* pImpl;
    [[no_unique_address]] allocator_type alloc;





  public:

  };

  template <typename ...Args>
  using command = basic_command<default_allocator, Args...>;
}

#endif//VALKYRIE_COMMAND_HPP
