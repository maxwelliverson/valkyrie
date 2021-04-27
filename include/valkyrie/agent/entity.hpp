//
// Created by maxwe on 2021-03-04.
//

#ifndef VALKYRIE_AGENT_ENTITY_HPP
#define VALKYRIE_AGENT_ENTITY_HPP

namespace valkyrie{


  class sender;
  class receiver;



  class entity{

  };







  /*class entity_base{

  public:

    virtual ~entity_base() = default;

    virtual sender*   acquire_sender()            noexcept = 0;
    virtual receiver* acquire_receiver()          noexcept = 0;
    virtual void      release_sender(sender*)     noexcept = 0;
    virtual void      release_receiver(receiver*) noexcept = 0;
  };


  template <typename Agent, typename Backend>
  class entity{

  };*/


}

#endif//VALKYRIE_AGENT_ENTITY_HPP
