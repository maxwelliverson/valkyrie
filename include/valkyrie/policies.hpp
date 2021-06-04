//
// Created by Maxwell on 2020-11-23.
//

#ifndef VALKYRIE_POLICIES_HPP
#define VALKYRIE_POLICIES_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie {

  class timeout;
  class deadline;
  enum class resource_scope : u32;

  /*enum class resource_scope : u32{
    thread,
    agent
    process,
    interprocess
  };*/
  
  
  namespace impl{
    struct dummy_extents{
      using index_type = i64;
    };
  }

  template <typename E>
  concept array_extents_p = requires{
    typename E::index_type;
    typename E::size_type;
  }
  && requires(E& e, const E& ce, typename E::index_type i, typename E::size_type n){
    { E::rank() }                -> same_as<typename E::size_type>;
    { E::rank_dynamic() }        -> same_as<typename E::size_type>;
    { E::is_static_extent(n) }   -> same_as<bool>;
    { E::total_static_extent() } -> same_as<typename E::size_type>;
    { ce.extent(0) }             -> same_as<typename E::index_type>;
    { ce.template extent<0>() }  -> same_as<typename E::index_type>;
  };

  template <typename M>
  concept array_mapping_p = requires(const M& map, size_t n){
    typename M::index_type;
    typename M::size_type;
    typename M::extents_type;
    requires constructible_from<M, const typename M::extents_type&>;
    typename M::template remap<typename M::extents_type>;
    M();
    M(map);
    { map.extents() }             -> same_as<typename M::extents_type>;
    { map.required_span_size() }  -> same_as<typename M::size_type>;
    { map.is_unique() }           -> convertible_to<bool>;
    { map.is_contiguous() }       -> convertible_to<bool>;
    { map.is_strided() }          -> convertible_to<bool>;
    { map.stride(n) }             -> convertible_to<typename M::index_type>;
    { M::is_always_unique() }     -> convertible_to<bool>;
    { M::is_always_contiguous() } -> convertible_to<bool>;
    { M::is_always_strided() }    -> convertible_to<bool>;
  };

  template <typename P, typename Ext>
  concept array_layout_p = requires{ 
    typename P::template mapping<Ext>; 
    requires array_mapping_p<typename P::template mapping<Ext>>;
  };

  template <typename P>
  concept array_accessor_p = requires {
    typename P::index_type;
    typename P::pointer;
    typename P::offset_policy;
    typename P::offset_policy::pointer;
    typename P::element_type;
    typename P::reference;
    typename P::template rebind<impl::dummy_extents>;
  } 
  && requires(P& accessor, typename P::pointer p, typename P::index_type i){
    { accessor.offset(p, i) } -> std::convertible_to<typename P::offset_policy::pointer>;
    { accessor.access(p, i) } -> std::convertible_to<typename P::reference>;
    { accessor.decay(p) }     -> std::convertible_to<typename P::pointer>;
  };
  
  
  

  template <typename P>
  concept resource_ownership_p = requires{ typename P::resource_type; } 
  && requires(P& ownership, typename P::resource_type& resource){
        { P::always_has_ownership() }                -> convertible_to<bool>;
        { P::always_has_unique_ownership() }         -> convertible_to<bool>;
        { P::always_has_shared_ownership() }         -> convertible_to<bool>;
        { ownership.has_ownership(resource) }        -> convertible_to<bool>;
        { ownership.has_unique_ownership(resource) } -> convertible_to<bool>;
        { ownership.has_shared_ownership(resource) } -> convertible_to<bool>;
        { ownership.scope(resource) }                -> same_as<resource_scope>;
        ownership.acquire(resource);
        ownership.release(resource);
  };
  
  
  
  template <typename P>
  concept channel_accessor_p = requires(P& accessor, const timeout& t, const deadline& d){

    { P::scope()                } -> same_as<resource_scope>;

    { P::absolute_max_writers() } -> extent_type<u64>;
    { P::absolute_max_readers() } -> extent_type<u64>;
    { accessor.max_writers() } -> convertible_to<u64>;
    { accessor.max_readers() } -> convertible_to<u64>;

      accessor.acquire_read_access();
    { accessor.try_acquire_read_access() } -> same_as<bool>;
    { accessor.acquire_read_access(t) } -> same_as<bool>;
    { accessor.acquire_read_access(d) } -> same_as<bool>;
      accessor.release_read_access();

      accessor.acquire_write_access();
    { accessor.try_acquire_write_access() } -> same_as<bool>;
    { accessor.acquire_write_access(t) } -> same_as<bool>;
    { accessor.acquire_write_access(d) } -> same_as<bool>;
      accessor.release_write_access();

  };

}


#endif//VALKYRIE_POLICIES_HPP
