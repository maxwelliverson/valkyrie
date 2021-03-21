//
// Created by maxwe on 2021-03-19.
//

#ifndef VALKYRIE_EVENT_LOOP_HPP
#define VALKYRIE_EVENT_LOOP_HPP

namespace valkyrie{

  class mailbox;

  /*class event_loop{
    enum class flags {
      busy,
      lazy
    };
    flags flags_;
    mailbox* mailbox_;

  };*/

  namespace detail{
    class event_loop_base{

    public:
      void pause();
    };
  }

  template <typename Entity>
  class event_loop{

  };


}

#endif//VALKYRIE_EVENT_LOOP_HPP
