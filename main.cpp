#include <iostream>

#include "vk_features.h"
#include "vk_object.h"

class MyEngine : public Vk::Engine{
public:
  MyEngine() : Vk::Engine(){
    this->name_ = "MyEngine";
    this->version_ = Vk::Version(1, 2, 154);
    this->api_version_ = Vk::Version(1, 2, 154);
    std::initializer_list<const char*> layers{
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_LUNARG_api_dump"
    };
    std::initializer_list<const char*> instanceExtensions{
        "VK_KHR_get_physical_device_properties2",
        //"VK_KHR_get_display_properties_2",
        "VK_KHR_get_surface_capabilities2",
        "VK_EXT_debug_utils",
        "VK_KHR_surface",
        //"VK_KHR_display",
        "VK_KHR_win32_surface",
        "VK_EXT_validation_features"
    };
    this->enabledLayers_.insert(layers);
    this->instanceExtensions_.insert(instanceExtensions);
  }
};
class MyApplication : public Vk::Application{
  Vk::Version version_;
  MyEngine engine_;
  std::string name_;
public:

  explicit MyApplication(std::string_view name_) : version_(0, 1, 0), engine_(), name_(name_){}

  [[nodiscard]] std::string_view name() const noexcept override{
    return name_;
  }
  [[nodiscard]] const MyEngine & engine() const noexcept override{
    return engine_;
  }
  [[nodiscard]] Vk::Version version() const noexcept override {
    return version_;
  }

  std::span<const Vk::UniqueString> enabledLayers() const noexcept override {
    return engine_.enabledLayers();
  }
  std::span<const Vk::UniqueString> enabledInstanceExtensions() const noexcept override {
    return engine_.instanceExtensions();
  }
};

/*struct ExampleStruct{
  VK_feature(
    addOp,
      property bool isValid,
      property string opName,
      property type(uint32_t) opCount,
      property group(
        string left,
        string right,
        bool supportsThird
      ) operands
  );
};*/

int main() {
  /*ExampleStruct exampleStruct;
  static_assert(Vk::serializable<decltype(exampleStruct.addOp)>);
  static_assert(Vk::string_like<typename decltype(exampleStruct.addOp.opName)::value_type>);
  std::cout << serialize(exampleStruct.addOp) << std::endl;
  exampleStruct.addOp.enable();
  exampleStruct.addOp.operands.left = "3";
  exampleStruct.addOp.operands.right = "24";
  exampleStruct.addOp.opName = "addition";
  exampleStruct.addOp.opCount = 235;
  std::cout << serialize(exampleStruct.addOp) << std::endl;*/

  MyApplication application("Maxwell's Test Application");
  Vk::Debugger debugger;
  debugger.enable(Vk::DebugPrintf);
  debugger.enable(Vk::BestPracticesValidation);
  debugger.enable(Vk::GpuAssistedValidation);

  application.init(debugger);

  auto&& physicalDevice = application.getPhysicalDevice();



  return 0;
}
