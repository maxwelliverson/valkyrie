//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_GRAPHICS_ENGINE_ATTACHMENT_RESOURCE_HPP
#define VALKYRIE_GRAPHICS_ENGINE_ATTACHMENT_RESOURCE_HPP

#include <valkyrie/ADT/array_ref.hpp>
#include <valkyrie/ADT/small_array.hpp>



namespace valkyrie::graphics::engine{


  class AttachmentTypeDescription{
  public:
    bool compatibleWith(const AttachmentTypeDescription& other) const noexcept;
  };

  class AttachmentDescription{
  public:
    const AttachmentTypeDescription& type() const noexcept;
  };
  class InterfaceParameter {
  public:
    bool isConnected() const noexcept;
  };
  class InterfaceParameterIn{

  };
  class InterfaceParameterOut{

  };

  class AttachmentResource{
    AttachmentTypeDescription type_;

  public:
    bool satisfies(const AttachmentDescription& other) const noexcept {
      return type_.compatibleWith(other.type());
    }
  };
}


#endif // VALKYRIE_GRAPHICS_ENGINE_ATTACHMENT_RESOURCE_HPP
