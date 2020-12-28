//
// Created by Maxwell on 2020-12-22.
//

#ifndef VALKYRIE_CORE_RESOURCE_HPP
#define VALKYRIE_CORE_RESOURCE_HPP



namespace valkyrie::Core{

  class Device{};

  class ResourceLifetime{};
  class ResourceScope{};
  class ResourceMemory{};



  class Resource{};
  class ResourceView{};

  template <typename PixelType>
  class Image{};
  template <typename ElementType>
  class Buffer{};


  class ImageView{};
  class BufferView{};
}


#endif//VALKYRIE_CORE_RESOURCE_HPP
