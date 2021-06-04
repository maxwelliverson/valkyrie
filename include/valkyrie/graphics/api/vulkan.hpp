//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_GRAPHICS_API_VULKAN_HPP
#define VALKYRIE_GRAPHICS_API_VULKAN_HPP

#include <valkyrie/adt/interval.hpp>
#include "enums.hpp"
//#include <valkyrie/status/result.hpp>
#include <valkyrie/utility/shapes.hpp>
#include <valkyrie/extended_primitives.hpp>

#include <format>
#include <source_location>
#include <concepts>
#include <tuple>

namespace valkyrie::graphics::api{

  using native_handle_t = void*;
  using    api_handle_t = const void*;



  /*class vulkan_status_domain final : public status_domain{
    inline static result enumValue(const status_code<void>& status) noexcept {
      return static_cast<const status_code<vulkan_status_domain>&>(status).value();
      //VK_assert();
    }
  public:
    using value_type = result;

    constexpr vulkan_status_domain() noexcept : status_domain(uuid("8ad355cf-d68b-4712-beb6-7e0938c9f835")){}


    string_ref name() const noexcept override{
      return VK_string("Vulkan api");
    }

    code do_generic_code(const status_code<void> &status) const noexcept override {}
    string_ref do_message(const status_code<void> &status) const noexcept override {}
    bool do_failure(const status_code<void> &status) const noexcept override {}
    severity do_severity(const status_code<void> &status) const noexcept override {}
    bool do_equivalent(const status_code<void> &statusA, const status_code<void> &statusB) const noexcept override {}

    inline static const vulkan_status_domain& get() noexcept {
      constexpr static vulkan_status_domain domainInstance{};
      return domainInstance;
    }
  };*/

  /*using status = status_code<vulkan_status_domain>;
  using error  = error_code<vulkan_status_domain>;
  template <typename T, typename NoValuePolicy = void>
  using basic_result = valkyrie::basic_result<T, vulkan_status_domain, NoValuePolicy>;
  template <typename T>
  using result = basic_result<T>;*/


  class instance;
  class physical_device;
  class device;
  class queue;
  class semaphore;
  class command_buffer;
  class fence;
  class device_memory;
  class buffer;
  class image;
  class event;
  class query_pool;
  class buffer_view;
  class image_view;
  class shader_module;
  class pipeline_cache;
  class pipeline_layout;
  class render_pass;
  class pipeline;
  class descriptor_set_layout;
  class sampler;
  class descriptor_pool;
  class descriptor_set;
  class framebuffer;
  class command_pool;
  class sampler_ycbcr_conversion;
  class descriptor_update_template;
  class surface;
  class swapchain;
  class display;
  class display_mode;
  class debug_report_callback;
  class debug_utils_messenger;
  class acceleration_structure;
  class validation_cache;
  class performance_configuration;
  class deferred_operation;
  class indirect_commands_layout;
  class private_data_slot;







  namespace impl{
    class vulkan_type{};
    class vulkan_resource : public vulkan_type{};



    class chainable {};

    class chain_root : public chainable{};
    class chain_link : public chainable{};

    template <typename T>
    class extends;



    template <typename List>
    List extract_list_type(const extends<List>&);
    template <std::derived_from<chainable> T>
    using extract_list_t = decltype(extract_list_type(std::declval<const T&>()));

    template <typename T, typename Base>
    concept valid_extension_of = (std::derived_from<Base, chain_root> && meta::typelist_containing<extract_list_t<T>, Base>) ||
        (std::derived_from<Base, chain_link> && nonempty_typelist<meta::set_intersection_t<extract_list_t<T>, extract_list_t<Base>>>);

    template <>
    class extends<meta::empty_set> : public chain_root{};
    template <typename List>
    class extends : public chain_link {
      static_assert(nonempty_typelist<List>);
      static_assert(meta::typeset<List>);
    };



    template <typename T>
    struct structure_type_enum;
    template <typename T>
    struct object_type_enum;

    template <typename T>
    VK_constant structure_type structure_type_v = structure_type_enum<T>::value;
    template <typename T>
    VK_constant object_type    object_type_v = object_type_enum<T>::value;

    class vulkan_structure   : public vulkan_type{
      structure_type sType;
    protected:
      constexpr explicit vulkan_structure(structure_type type) noexcept : sType(type){}

      constexpr bool same_type_as(const vulkan_structure& other) const noexcept {
        return sType == other.sType;
      }
      constexpr bool same_type_as(structure_type otherType) const noexcept {
        return sType == otherType;
      }
    };
    class base_in_structure  : public vulkan_structure{
      mutable const base_in_structure* pNext = nullptr;
    protected:
      constexpr base_in_structure(structure_type sType) noexcept : vulkan_structure(sType){}

      constexpr void unsafe_link(base_in_structure& other) const noexcept {
        if ( pNext != nullptr )
          pNext->unsafe_link(other);
        else
          pNext = &other;
      }
      constexpr bool safe_link(base_in_structure& other) const noexcept {
        if ( this->same_type_as(other) )
          return false;
        if ( pNext != nullptr )
          return pNext->safe_link(other);
        pNext = &other;
        return true;
      }
      constexpr const base_in_structure* safe_unlink(const base_in_structure& parent, structure_type sType) const noexcept {
        if ( this->same_type_as(sType) ) {
          parent.pNext = std::exchange(pNext, nullptr);
          return this;
        }
        if ( pNext != nullptr )
          return pNext->safe_unlink(*this, sType);
        return nullptr;
      }
    };
    class base_out_structure : public vulkan_structure{
      base_out_structure* pNext = nullptr;
    protected:
      constexpr base_out_structure(structure_type sType) noexcept : vulkan_structure(sType){}

      constexpr void unsafe_link(base_out_structure& other) noexcept {
        if ( pNext != nullptr )
          pNext->unsafe_link(other);
        else
          pNext = &other;
      }

      constexpr bool safe_link(base_out_structure& other) noexcept {
        if ( this->same_type_as(other) )
          return false;
        if ( pNext != nullptr )
          return pNext->safe_link(other);
        pNext = &other;
        return true;
      }
      constexpr base_out_structure* safe_unlink(base_out_structure& parent, structure_type sType) noexcept {
        if ( this->same_type_as(sType) ) {
          parent.pNext = std::exchange(pNext, nullptr);
          return this;
        }
        if ( pNext != nullptr )
          return pNext->safe_unlink(*this, sType);
        return nullptr;
      }
    };
  }

  class object   : public impl::vulkan_type{};
  class basic    : public impl::vulkan_type{};


  template <typename Derived, typename ...Extends>
  class in_structure : public impl::base_in_structure, public impl::extends<meta::set<Extends...>>{
  public:
    constexpr in_structure() noexcept : impl::base_in_structure(impl::structure_type_v<Derived>){}

    template <impl::valid_extension_of<Derived> T>
    bool link(T& value) noexcept {
      return impl::base_in_structure::safe_link((impl::base_in_structure&)value);
    }
    template <impl::valid_extension_of<Derived> T>
    void unsafe_link(T& value) noexcept {
      impl::base_in_structure::unsafe_link((impl::base_in_structure&)value);
    }
    template <impl::valid_extension_of<Derived> T>
    T*   unlink() noexcept {
      return const_cast<T*>(static_cast<const T*>(impl::base_in_structure::safe_unlink(*this, impl::structure_type_v<Derived>)));
    }
  };
  template <typename Derived, typename ...Extends>
  class out_structure : public impl::base_out_structure, public impl::extends<meta::set<Extends...>>{
  public:
    constexpr out_structure() noexcept : impl::base_out_structure(impl::structure_type_v<Derived>){}

    template <impl::valid_extension_of<Derived> T>
    bool link(T& value) noexcept {
      return impl::base_out_structure::safe_link((impl::base_out_structure&)value);
    }
    template <impl::valid_extension_of<Derived> T>
    void unsafe_link(T& value) noexcept {
      impl::base_out_structure::unsafe_link((impl::base_out_structure&)value);
    }
    template <impl::valid_extension_of<Derived> T>
    T*   unlink() noexcept {
      return static_cast<T*>(impl::base_out_structure::safe_unlink(*this, impl::structure_type_v<Derived>));
    }
  };


  class instance_object        : public object{ };
  class physical_device_object : public object{ };
  class device_object          : public object{ };


#define VK_declare_struct(type) \
  struct type; \
  template <> struct impl::structure_type_enum<type>{ \
    VK_constant structure_type value = structure_type::type;  \
  }


  struct component_mapping{
    component_swizzle r = component_swizzle::identity;
    component_swizzle g = component_swizzle::identity;
    component_swizzle b = component_swizzle::identity;
    component_swizzle a = component_swizzle::identity;
  };
  struct viewport : Box2D<f32>{
    interval<f32> depth;
  };
  struct stencil_op_state{
    stencil_op failOp;
    stencil_op passOp;
    stencil_op depthFailOp;
    compare_op compareOp;
    u32       compareMask;
    u32       writeMask;
    u32       reference;
  };
  struct shading_rate_palette{
    u32                               entryCount;
    const shading_rate_palette_entry* pEntries;
  };
  struct sample_location : position2d<f32>{};
  struct coarse_sample_location{
    position2d<u32> pixel;
    u32             sample;
  };
  struct coarse_sample_order_custom {
    shading_rate_palette_entry    shadingRate;
    u32                           sampleCount;
    u32                           sampleLocationCount;
    const coarse_sample_location* pSampleLocations;
  };

#define VK_define_in_struct(type, ...) struct type  : in_structure<type, ##__VA_ARGS__>
#define VK_define_out_struct(type, ...) struct type : out_structure<type, ##__VA_ARGS__>
}



#endif//VALKYRIE_GRAPHICS_API_VULKAN_HPP
