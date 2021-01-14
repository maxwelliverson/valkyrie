//
// Created by Maxwell on 2020-12-22.
//

#ifndef VALKYRIE_CORE_RESOURCE_HPP
#define VALKYRIE_CORE_RESOURCE_HPP



namespace valkyrie::Core{

  class Device{ };
  class DeviceQueue{ };

  class ResourceLifetime{ };
  class ResourceScope{ };
  class ResourceMemory{ };



  class Resource{ };
  class ResourceView{
  public:
    virtual Resource* getBackingResource() noexcept = 0;
  };

  class Image : public Resource{ };
  class Buffer : public Resource{  };


  class ImageView : public ResourceView{
  public:
    Image* getBackingResource() noexcept override = 0;
  };
  class BufferView : public ResourceView{
  public:
    Buffer* getBackingResource() noexcept override = 0;
  };
}


#endif//VALKYRIE_CORE_RESOURCE_HPP
