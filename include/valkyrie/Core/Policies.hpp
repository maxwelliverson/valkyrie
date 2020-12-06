//
// Created by Maxwell on 2020-11-23.
//

#ifndef VALKYRIE_POLICIES_HPP
#define VALKYRIE_POLICIES_HPP

#include <valkyrie/Core/Traits.hpp>

namespace valkyrie::Policies{
  struct Ownership{};
  struct Locking{};

  enum class Scope{
    Thread,
    Process,
    Interprocess
  };

  /*struct OneToMany : Ownership{
    VK_constant Scope    scope   = Scope::Process;
    VK_constant Capacity writers = Capacity::One;
    VK_constant Capacity readers = Capacity::Many;
  };
  struct ManyToOne : Ownership{
    VK_constant Scope    scope   = Scope::Process;
    VK_constant Capacity writers = Capacity::Many;
    VK_constant Capacity readers = Capacity::One;
  };
  struct OneToOne  : Ownership{
    VK_constant Scope    scope   = Scope::Process;
    VK_constant Capacity writers = Capacity::One;
    VK_constant Capacity readers = Capacity::One;
  };
  struct ManyToMany : Ownership{
    VK_constant Scope    scope   = Scope::Process;
    VK_constant Capacity writers = Capacity::Many;
    VK_constant Capacity readers = Capacity::Many;
  };*/
}


#endif//VALKYRIE_POLICIES_HPP
