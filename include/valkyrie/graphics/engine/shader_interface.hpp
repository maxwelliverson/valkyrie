//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_GRAPHICS_ENGINE_SHADER_INTERFACE_HPP
#define VALKYRIE_GRAPHICS_ENGINE_SHADER_INTERFACE_HPP

#include <valkyrie/adt/array_ref.hpp>
//#include <valkyrie/adt/small_vector.hpp>



namespace valkyrie::graphics::engine{


  class render_node_resource_in{};
  class render_node_resource_out{};

  class render_node_resource{
  public:
    render_node_resource_in  in;
    render_node_resource_out out;
  };

  class render_node_interface{
  public:
    virtual render_node_resource_out* out_node(size_t& size, size_t& align) const noexcept = 0;
    virtual render_node_resource_in*  in_node(size_t& size, size_t& align) const noexcept = 0;
  };


  class render_node{
  public:
    virtual render_node_interface& get_interface() const noexcept = 0;
  };

  /*class shader_interface;
  class shader_interface_node;
  class shader_interface_node_in;
  class shader_interface_node_out;



  class shader_interface_node{ };
  class shader_interface_node_in : public shader_interface_node{ };
  class shader_interface_node_out : public shader_interface_node{ };


  struct shader_interface_resource{
    shader_interface_node_in  in_node;
    shader_interface_node_out out_node;
  };


  class shader_interface{
  public:

    virtual array_ref<shader_interface_node_in>  get_in_nodes()  noexcept = 0;
    virtual array_ref<shader_interface_node_out> get_out_nodes() noexcept = 0;
  };*/
}

#endif //VALKYRIE_GRAPHICS_ENGINE_SHADER_INTERFACE_HPP
