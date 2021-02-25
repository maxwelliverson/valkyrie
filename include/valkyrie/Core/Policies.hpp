//
// Created by Maxwell on 2020-11-23.
//

#ifndef VALKYRIE_POLICIES_HPP
#define VALKYRIE_POLICIES_HPP

#include <valkyrie/Core/Traits.hpp>

namespace valkyrie::Core {

  enum class Scope{
    Thread,
    Process,
    Interprocess
  };

  namespace Detail{
    template <typename M>
    concept MappingPolicy = requires(const M& map, size_t n){
      typename M::index_type;
      typename M::extents_type;
      typename M::template remap<Extents<4>>;
      M();
      M(map);
      { map.extents() }           -> SameAs<typename M::extents_type>;
      { map.requiredSpanSize() }  -> SameAs<typename M::index_type>;
      { map.isUnique() }          -> std::convertible_to<bool>;
      { map.isContiguous() }      -> std::convertible_to<bool>;
      { map.isStrided() }         -> std::convertible_to<bool>;
      { map.stride(n) }           -> std::convertible_to<typename M::index_type>;
      { M::isAlwaysUnique() }     -> std::convertible_to<bool>;
      { M::isAlwaysContiguous() } -> std::convertible_to<bool>;
      { M::isAlwaysStrided() }    -> std::convertible_to<bool>;
    } && requires(const typename M::extents_type& ext){
      M(ext);
    };

    struct AnyExtents{
      using index_type = ptrdiff_t;
    };
  }

  namespace Policy{

    template <typename P>
    concept Ownership = requires(P& ownership){
      typename P::resource_type;
      { P::ownershipIsConstant() }       -> std::convertible_to<bool>;
      { P::uniqueOwnershipIsConstant() } -> std::convertible_to<bool>;
      { P::sharedOwnershipIsConstant() } -> std::convertible_to<bool>;
    } && requires(P& ownership, typename P::resource_type& resource){
      { ownership.hasOwnership(resource) }       -> std::convertible_to<bool>;
      { ownership.hasUniqueOwnership(resource) } -> std::convertible_to<bool>;
      { ownership.hasSharedOwnership(resource) } -> std::convertible_to<bool>;
      { ownership.resourceScope(resource) }      -> SameAs<Scope>;
      ownership.acquire(resource);
      ownership.release(resource);
    };
    template <typename P, typename Ext>
    concept Layout    = requires{ typename P::template mapping<Ext>; } && Detail::MappingPolicy<typename P::template mapping<Ext>>;
    template <typename P>
    concept Accessor  = requires {
      typename P::index_type;
      typename P::offset_policy;
      typename P::element_type;
      typename P::reference;
      typename P::pointer;
      typename P::template rebind<Detail::AnyExtents>;
    } && requires(P& accessor, typename P::pointer p, typename P::index_type i){
      { accessor.offset(p, i) } -> std::convertible_to<typename P::offset_policy::pointer>;
      { accessor.access(p, i) } -> std::convertible_to<typename P::reference>;
      { accessor.decay(p) }     -> std::convertible_to<typename P::pointer>;
    };
    template <typename P>
    concept Locking   = true;


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
}


#endif//VALKYRIE_POLICIES_HPP
