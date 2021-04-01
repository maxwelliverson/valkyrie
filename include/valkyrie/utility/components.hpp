//
// Created by maxwe on 2021-03-31.
//

#ifndef VALKYRIE_UTILITY_COMPONENTS_HPP
#define VALKYRIE_UTILITY_COMPONENTS_HPP

#include <valkyrie/adt/intrusive_list.hpp>
#include <valkyrie/adt/ptr.hpp>

namespace valkyrie{

  class component_impl{
  public:

  };


  /**
   *
   * Example:
   *
   * class frame_operation : public component<frame_operation>{
   *   class interface{
   *     public:
   *
   *   };
   *
   *
   * };
   *
   *
   *
   *
   *
   *
   *
   *
   * */

  class object{

  };

  class component_interface{
    u64 id_;

  protected:
    constexpr component_interface(const uuid& id) noexcept : id_(to_integer(id)){}
  public:

    virtual u32 acquire(object*) noexcept = 0;
    virtual u32 release(object*) noexcept = 0;

    constexpr u64 id() const noexcept { return id_; }

    friend constexpr bool operator==(const component_interface& a, const component_interface& b) noexcept {
      return a.id_ == b.id_;
    }
  };

  template <typename Der>
  class component : public component_interface{

  };

  template <typename Der, typename ...Comps>
  class implements : public forward_ilist_node<Der, Comps...>{

  };


  template <typename Comp>
  class component_ptr : public ptr_facade<component_ptr<Comp>, const Comp*>{

    static const Comp* instance() noexcept {
      constexpr static Comp comp{};
      return std::addressof(comp);
    }



  public:

    component_ptr() noexcept : comp(instance()){}

    component_ptr(const component_ptr&)     = default;
    component_ptr(component_ptr&&) noexcept = default;




  private:
    const Comp* comp;
  };

  VK_define_tag(component_tag);

  template <typename Comp>
  class component_list{

    struct tag : component_tag, ptr_{};

  public:
  private:
    forward_ilist<>
  };

}

#endif//VALKYRIE_UTILITY_COMPONENTS_HPP
