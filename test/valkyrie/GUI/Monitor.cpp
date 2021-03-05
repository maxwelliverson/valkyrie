//
// Created by Maxwell on 2020-11-30.
//

#include <valkyrie/GUI/monitor.hpp>
#include <valkyrie/utility/uuid.hpp>

#include <iostream>


using namespace valkyrie::GUI;

int main(){
  auto monitors = monitor::enumerate();
  std::cout << "\n\n\n\n ======[ int main() ]======\n\n";
  for (monitor monitor : monitors)
    std::cout << monitor.name() << ": " << "{ w: " << monitor.width() << ", h: " << monitor.height() << ", fps: " << monitor.frequency() << " }\n";
  /*CUdevice device;
  CUcontext ctx;
  CUuuid   uuid;

  cuInit(0);
  cuDeviceGet(&device, 0);
  cuDevicePrimaryCtxRetain(&ctx, device);
  cuDeviceGetUuid(&uuid, device);
  cuDevicePrimaryCtxRelease(device);

  char cudaUuidBuffer[39]{ '{' };
  char vulkanUuidBuffer[39]{ '{' };

  VkApplicationInfo appInfo{
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = nullptr,
    .pApplicationName = "App",
    .applicationVersion = 0,
    .pEngineName = "Engine",
    .engineVersion = 0,
    .apiVersion = VK_API_VERSION_1_2
  };

  VkInstanceCreateInfo instanceCreateInfo{
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .pApplicationInfo = &appInfo,
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = nullptr,
    .enabledExtensionCount = 0,
    .ppEnabledExtensionNames = nullptr
  };

  VkInstance instance;
  VkResult instanceResult = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);

  VkPhysicalDevice vkDevice;
  uint32_t physicalDeviceCount = 1;
  VkResult physDeviceResult = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, &vkDevice);

  VkPhysicalDeviceIDProperties idProperties{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES,
      .pNext = nullptr
  };
  VkPhysicalDeviceProperties2 deviceProperties2{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
      .pNext = &idProperties
  };
  VkPhysicalDeviceDriverProperties driver;

  vkGetPhysicalDeviceProperties2(vkDevice, &deviceProperties2);




  auto bufferEnd = (char*)writeToBuffer((char8_t*)&cudaUuidBuffer[1], sizeof(cudaUuidBuffer), *reinterpret_cast<const valkyrie::uuid*>(&uuid));
  bufferEnd[0] = '}';
  auto cudaUuid = std::string_view(cudaUuidBuffer, bufferEnd - cudaUuidBuffer + 1);
  bufferEnd = (char*)writeToBuffer((char8_t*)&vulkanUuidBuffer[1], sizeof(vulkanUuidBuffer), *reinterpret_cast<const valkyrie::uuid*>(&idProperties.deviceUUID));
  bufferEnd[0] = '}';
  auto vulkanUuid = std::string_view(vulkanUuidBuffer, bufferEnd - vulkanUuidBuffer + 1);

  std::cout << "\n\nCuda Device ID: " << cudaUuid;
  std::cout << "\nVulkan Device ID: " << vulkanUuid << std::endl;*/
}