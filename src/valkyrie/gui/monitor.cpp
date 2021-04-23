//
// Created by Maxwell on 2020-11-30.
//

#include "dxgi.hpp"
#include "internal_structures.hpp"

#include <valkyrie/gui/monitor.hpp>

namespace {
  void fillGraphicsMode(valkyrie::GUI::Internal::GraphicsMode& mode, const DEVMODE& dev) noexcept {

    constexpr static auto AllFlags = DM_DISPLAYORIENTATION |
                                     DM_DISPLAYFREQUENCY |
                                     DM_DISPLAYFLAGS |
                                     //DM_LOGPIXELS |
                                     DM_BITSPERPEL |
                                     DM_PELSWIDTH |
                                     DM_PELSHEIGHT;

    assert((dev.dmFields & (AllFlags)) == AllFlags && "DEVMODE must have had all the specified Flags set");

    mode.displayFrequency = dev.dmDisplayFrequency;
    mode.pixelHeight = dev.dmPelsHeight;
    mode.pixelWidth  = dev.dmPelsWidth;
    mode.bitsPerPixel = dev.dmBitsPerPel;
    mode.interlaced  = dev.dmDisplayFlags & DM_INTERLACED;
  }
}

#if VK_debug_build
#include <iostream>
#include <array>

namespace{

  template <typename T>
  struct ObjectTraits;

  template <typename T>
  concept CustomObject = requires{
    { sizeof(ObjectTraits<std::remove_cvref_t<T>>) };
  };

  class IndentedOStream{
    inline constexpr static decltype(auto) indents = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
    std::ostream& OS;
    unsigned currentIndents = 0;

    void writeIndents() noexcept{
      OS << std::string_view{indents, currentIndents};
    }

  public:
    explicit IndentedOStream(std::ostream& OS) noexcept : OS(OS){
      this->OS << std::boolalpha;
    }

    IndentedOStream& startObject() noexcept {
      ++currentIndents;
      OS << "{ ";
      return *this;
    }
    IndentedOStream& endObject() noexcept {
      --currentIndents;
      newLine();
      OS << "}";
      return *this;
    }

    IndentedOStream& newLine() noexcept{
      OS << "\n";
      writeIndents();
      return *this;
    }
    IndentedOStream& flush() noexcept{
      OS << std::flush;
      return *this;
    }



    template <typename T> requires(!CustomObject<T> && requires(std::ostream& iOS, T&& t){
      { iOS << std::forward<T>(t) } -> valkyrie::exact_same_as<std::ostream&>;
    })
    friend IndentedOStream& operator<<(IndentedOStream& iOS, T&& t) {
      iOS.OS << std::forward<T>(t);
      return iOS;
    }
  };

  template <typename F>
  class FunctionRef;

  template <typename Ret, typename ...Args>
  class FunctionRef<Ret(Args...)>{
    using real_fn_ptr_t = Ret(*)(Args...);
    using fn_ptr_t = Ret(*)(const void* pFn, Args...);
    fn_ptr_t funcPtr;
    const void* pFunctor;
    template <typename F>
    inline static Ret erasedFunction(const void* pFunctor, Args&& ...args) noexcept {
      return (*static_cast<const F*>(pFunctor))(std::forward<Args>(args)...);
    }

  public:

    template <std::invocable<Args...> Fn> requires(std::convertible_to<std::invoke_result_t<Fn, Args...>, Ret>)
    constexpr FunctionRef(Fn&& func) noexcept : funcPtr(erasedFunction<std::remove_reference_t<Fn>>), pFunctor(std::addressof(func)){}
    template <std::convertible_to<fn_ptr_t> F>
    constexpr FunctionRef(F&& func) noexcept : funcPtr(std::forward<F>(func)), pFunctor(nullptr){}
    

    template <typename ...OArgs> requires(std::invocable<Ret(Args...), OArgs...>)
    Ret operator()(OArgs&&... args) const noexcept {
      return funcPtr(pFunctor, std::forward<OArgs>(args)...);
    }
  };

  template <typename T>
  struct ObjectMemberRef{
    std::string_view name;
    FunctionRef<IndentedOStream&(IndentedOStream&, const T&)> writeFunction;
  };

#define PP_VK_impl_UNWRAP_member(...) __VA_ARGS__



#define PP_VK_impl_INDENTED_WRITE_MEMBER_redirect_2(Type, member, ...) ObjectMemberRef<Type>{ \
    .name = #member, \
    .writeFunction = [](const void*, IndentedOStream& OS, const Type& value) -> IndentedOStream& { VK_if(VK_pack_is_empty(__VA_ARGS__)( return OS << value.member; )VK_else(__VA_ARGS__)) } \
  }
#define PP_VK_impl_INDENTED_WRITE_MEMBER_redirect PP_VK_impl_INDENTED_WRITE_MEMBER_redirect_2
#define PP_VK_impl_INDENTED_WRITE_MEMBER(...) PP_VK_impl_INDENTED_WRITE_MEMBER_redirect(__VA_ARGS__)

#define PP_VK_impl_INDENTED_WRITE(Type, mem_) PP_VK_impl_INDENTED_WRITE_MEMBER(Type, PP_VK_impl_UNWRAP_##mem_)
#define VK_write_object_json(Type, ...) template <> struct ObjectTraits<Type>{ \
    inline constexpr static ObjectMemberRef<Type> members[]{                      \
       VK_foreach_delimit(VK_bind_front(PP_VK_impl_INDENTED_WRITE, Type), VK_comma_delimiter, ##__VA_ARGS__) \
    };                    \
  };


  template <CustomObject T>
  inline IndentedOStream& operator<<(IndentedOStream& OS, T&& obj) noexcept {
    
    constexpr static size_t memberCount = std::size(ObjectTraits<std::remove_cvref_t<T>>::members);
    
    if constexpr (memberCount == 0) {
      return OS << "{ }";
    } else {
      OS.startObject();
      const T& value = obj;
      for (unsigned i = 0; i < (memberCount - 1); ++i) {
        const auto& mem = ObjectTraits<std::remove_cvref_t<T>>::members[i];
        OS.newLine() << mem.name << ": ";
        mem.writeFunction(OS, value) << ",";
      }
      const auto& mem = ObjectTraits<std::remove_cvref_t<T>>::members[memberCount - 1];
      OS.newLine() << mem.name << ": ";
      return mem.writeFunction(OS, value).endObject();
    }
  }


  using GraphicsMode = valkyrie::GUI::Internal::GraphicsMode;

  VK_write_object_json(GraphicsMode,
                         member(width, return OS << value.pixelWidth;),
                         member(height, return OS << value.pixelHeight;),
                         member(frequency,     return OS << value.displayFrequency << "Hz";)
                      );
  
  struct DisplayAdapter{
    const DISPLAY_DEVICE& dev;
    std::span<const unsigned> monitorIndices;
    DisplayAdapter(const DISPLAY_DEVICE& device, std::span<const unsigned> indices) noexcept : dev(device), monitorIndices(indices){}
  };
  

  
  

  void printDeviceFlags(IndentedOStream& OS, DWORD flags) noexcept {
    bool hasAtLeastOneFlag = false;
    OS << "[ ";
    if (flags & DISPLAY_DEVICE_ACTIVE) {
      OS << "Active | ";
      hasAtLeastOneFlag = true;
    }
    if (flags & DISPLAY_DEVICE_MIRRORING_DRIVER) {
      OS << "MirroringDriver | ";
      hasAtLeastOneFlag = true;
    }
    if (flags & DISPLAY_DEVICE_MODESPRUNED) {
      OS << "ModesPruned | ";
      hasAtLeastOneFlag = true;
    }
    if (flags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
      OS << "Primary | ";
      hasAtLeastOneFlag = true;
    }
    if (flags & DISPLAY_DEVICE_REMOVABLE) {
      OS << "Removable | ";
      hasAtLeastOneFlag = true;
    }
    if (flags & DISPLAY_DEVICE_VGA_COMPATIBLE) {
      OS << "VGACompatible | ";
      hasAtLeastOneFlag = true;
    }
    if (hasAtLeastOneFlag)
      OS << "\b\b";
    OS << "]";
  }
  void printDevMode(IndentedOStream& OS, const DEVMODE& dev) noexcept {

    constexpr static auto AllFlags = DM_POSITION |
                                     DM_DISPLAYFREQUENCY |
                                     DM_DISPLAYFLAGS |
                                     DM_DISPLAYFIXEDOUTPUT |
                                     DM_DISPLAYORIENTATION |
                                     DM_LOGPIXELS |
                                     DM_BITSPERPEL |
                                     DM_PELSWIDTH |
                                     DM_PELSHEIGHT |
                                     DM_DITHERTYPE;

    OS.startObject();
    if (dev.dmFields & DM_POSITION)
      OS.newLine() << "position: { x: " << dev.dmPosition.x << ", y: " << dev.dmPosition.y << " },";
    if (dev.dmFields & DM_DISPLAYORIENTATION) {
      OS.newLine() << "orientation: ";
      switch (dev.dmDisplayOrientation) {
        case DMDO_DEFAULT:
          OS << "{ degrees: 0 },";
          break;
        case DMDO_90:
          OS << "{ degrees: 90 },";
          break;
        case DMDO_180:
          OS << "{ degrees: 180 },";
          break;
        case DMDO_270:
          OS << "{ degrees: 270 },";
          break;
        default:
          OS << "UNKNOWN,";
      }
    }
    if (dev.dmFields & DM_DISPLAYFIXEDOUTPUT) {
      OS.newLine() << "fill_strategy: ";
      switch(dev.dmDisplayFixedOutput){
        case DMDFO_DEFAULT:
          OS << "\"Default\",";
          break;
        case DMDFO_CENTER:
          OS << "\"Center\",";
          break;
        case DMDFO_STRETCH:
          OS << "\"Stretch\",";
          break;
        default:
          OS << "UNKNOWN,";
      }
    }
    if (dev.dmFields & DM_LOGPIXELS)
      OS.newLine() << "dpi: " << dev.dmLogPixels << "px,";
    if (dev.dmFields & DM_BITSPERPEL)
      OS.newLine() << "bits_per_pixel: " << dev.dmBitsPerPel << "b,";
    if (dev.dmFields & DM_PELSWIDTH)
      OS.newLine() << "width: " << dev.dmPelsWidth << "px,";
    if (dev.dmFields & DM_PELSHEIGHT)
      OS.newLine() << "height: " << dev.dmPelsHeight << "px,";
    if (dev.dmFields & DM_DISPLAYFLAGS)
      OS.newLine() << "interlaced: " << bool(dev.dmDisplayFlags & DM_INTERLACED) << ",";
    if (dev.dmFields & DM_DISPLAYFREQUENCY)
      OS.newLine() << "frequency: " << dev.dmDisplayFrequency << " Hz,";
    if (dev.dmFields & DM_DITHERTYPE) {
      OS.newLine() << "dithering: ";
      switch (dev.dmDitherType) {
        case DMDITHER_NONE:
          OS << "\"None\",";
          break;
        case DMDITHER_COARSE:
          OS << "\"Coarse\",";
          break;
        case DMDITHER_FINE:
          OS << "\"Fine\",";
          break;
        case DMDITHER_LINEART:
          OS << "\"Lineart\",";
          break;
        case DMDITHER_GRAYSCALE:
          OS << "\"Grayscale\",";
          break;
        default:
          OS << "\"Other\",";
      }
    }
    if (dev.dmFields & AllFlags)
      OS << "\b";
    OS.endObject();
  }

  template <typename T>
  void printArray(IndentedOStream& OS, std::span<T> span) noexcept {
    if (span.empty()) {
      OS << "[]";
      return;
    }
    OS << "[ ";
    size_t lastIndex = span.size() - 1;
    for (size_t i = 0; i < lastIndex; ++i) {
      OS << span[i] << ", ";
    }
    OS << span.back() << " ]";
  }


  VK_write_object_json(DisplayAdapter,
                       member(path, return OS << value.dev.DeviceName;),
                       member(name, return OS << value.dev.DeviceString;),
                       member(flags, printDeviceFlags(OS, value.dev.StateFlags); return OS;),
                       member(id, return OS << value.dev.DeviceID;),
                       member(key, return OS << value.dev.DeviceKey;),
                       member(monitor_indices, printArray(OS, value.monitorIndices); return OS;)
  );
  VK_write_object_json(valkyrie::GUI::Internal::MonitorImpl,
                         member(name),
                         member(path),
                         member(current_mode, return OS << value.currentGraphicsMode;),
                         member(position, return OS << "{ x: " << value.position.x << ", y: " << value.position.y << " }";),
                         member(is_primary, return OS << value.primary;)//,
                         //member(valid_modes, printArray<const GraphicsMode>(OS, value.graphicsModes); return OS;)
                       );


  struct NewLineStr{};
  inline constexpr NewLineStr newline{};

  inline IndentedOStream& operator<<(IndentedOStream& OS, const NewLineStr&) noexcept {
    return OS.newLine();
  }
}

#else
namespace {
  class NullLog{};
  template <typename T>
  inline constexpr const NullLog& operator<<(const NullLog& null, T&&) noexcept { return null; }
  inline constexpr NullLog nullLog{};
  inline constexpr NullLog newline{};

  inline std::nullptr_t DisplayAdapter(...) noexcept {
    return nullptr;
  }

  template <typename T>
  void printArray(NullLog, std::span<T> span) noexcept{}
}
#endif


#define DEBUG_LOG VK_if(VK_debug_build(_iOstream)VK_else(nullLog))
#define CREATE_DEBUG_LOG VK_if(VK_debug_build(IndentedOStream _iOstream{std::cout};))



namespace valkyrie::GUI{



  class monitor::Impl : public Internal::MonitorImpl{};

  namespace {
    inline small_vector<Internal::MonitorImpl> systemMonitors{};
    inline small_vector<Internal::MonitorImpl*> systemMonitorPointers{};
    inline void   findMonitors(){

      static constexpr size_t BufferSize = 256;

      char descriptionBuffer[BufferSize];

      CREATE_DEBUG_LOG
      
      DISPLAY_DEVICE displayDevice{
        .cb = sizeof(DISPLAY_DEVICE)
      };

      u32 deviceIndex = 0;

      DEBUG_LOG << "display_adapters: [ ";

      while (EnumDisplayDevices(nullptr, deviceIndex, &displayDevice, 0)){
        
        u32 monitorIndex = 0;
        DEVMODE graphicsMode;
        graphicsMode.dmSize = sizeof(DEVMODE);
        graphicsMode.dmDriverExtra = 0;
        DISPLAY_DEVICE monitorDevice{
          .cb = sizeof(DISPLAY_DEVICE)
        };
        small_vector<u32, 4> monitorIndices{};


        while (EnumDisplayDevices(displayDevice.DeviceName, monitorIndex, &monitorDevice, 0)) {

          auto result = std::find_if(systemMonitors.cbegin(), systemMonitors.cend(), [&monitorDevice](const Internal::MonitorImpl& monitor){
            return monitor.path == monitorDevice.DeviceName;
          });
          
          u32 index = result - systemMonitors.begin();
          
          if (index == systemMonitors.size()) {
            Internal::MonitorImpl& newMonitor = systemMonitors.emplace_back();
            newMonitor.path = monitorDevice.DeviceName;
            newMonitor.refCount = 1;
            newMonitor.available = true;

            u32 graphicsModeIndex = 0;

            while (EnumDisplaySettingsEx(displayDevice.DeviceName, graphicsModeIndex, &graphicsMode, 0)) {
              auto& mode = newMonitor.graphicsModes.emplace_back();
              fillGraphicsMode(mode, graphicsMode);
              graphicsMode = {};
              graphicsMode.dmSize = sizeof(DEVMODE);
              graphicsMode.dmDriverExtra = 0;
              ++graphicsModeIndex;
            }

            auto currentResult = EnumDisplaySettingsEx(displayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &graphicsMode, 0);

            fillGraphicsMode(newMonitor.currentGraphicsMode, graphicsMode);


            assert(graphicsMode.dmFields & DM_POSITION);
            newMonitor.position.x = graphicsMode.dmPosition.x;
            newMonitor.position.y = graphicsMode.dmPosition.y;
            newMonitor.primary = displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE;
            newMonitor.handle  = MonitorFromPoint({ .x = newMonitor.position.x + 1, .y = newMonitor.position.y + 1 }, MONITOR_DEFAULTTONEAREST);

            PHYSICAL_MONITOR physicalMonitor;
            if (!GetPhysicalMonitorsFromHMONITOR(newMonitor.handle, 1, &physicalMonitor))
              panic(sys::win32::get_last_error());
            
            newMonitor.physicalMonitorHandle = physicalMonitor.hPhysicalMonitor;
            newMonitor.name.assign(monitorDevice.DeviceString, std::strlen(monitorDevice.DeviceString));
            /*mbstate_t mbstate{};
            size_t newSize;
            const wchar_t * descString = physicalMonitor.szPhysicalMonitorDescription;
            auto err = wcsrtombs_s(&newSize, descriptionBuffer, BufferSize, &descString, BufferSize, &mbstate);
            assert(!err);
            newMonitor.description.assign(descriptionBuffer, newSize);*/

            
            graphicsMode = {};
            graphicsMode.dmSize = sizeof(DEVMODE);
            graphicsMode.dmDriverExtra = 0;
          }
          monitorIndices.push_back(index);

          /*DEBUG_LOG << "name: \"" << monitorDevice.DeviceName << "\",\n\t\t";
          DEBUG_LOG << "\t\tdescription: \"" << monitorDevice.DeviceString << "\",\n\t\t";
          DEBUG_LOG << "flags: [ ";
          bool innerHasAtLeastOneFlag = false;
          if (monitorDevice.StateFlags & DISPLAY_DEVICE_ACTIVE) {
            DEBUG_LOG << "Active | ";
            innerHasAtLeastOneFlag = true;
          }
          if (monitorDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) {
            DEBUG_LOG << "MirroringDriver | ";
            innerHasAtLeastOneFlag = true;
          }
          if (monitorDevice.StateFlags & DISPLAY_DEVICE_MODESPRUNED) {
            DEBUG_LOG << "ModesPruned | ";
            innerHasAtLeastOneFlag = true;
          }
          if (monitorDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
            DEBUG_LOG << "Primary | ";
            innerHasAtLeastOneFlag = true;
          }
          if (monitorDevice.StateFlags & DISPLAY_DEVICE_REMOVABLE) {
            DEBUG_LOG << "Removable | ";
            innerHasAtLeastOneFlag = true;
          }
          if (monitorDevice.StateFlags & DISPLAY_DEVICE_VGA_COMPATIBLE) {
            DEBUG_LOG << "VGACompatible | ";
            innerHasAtLeastOneFlag = true;
          }
          if (innerHasAtLeastOneFlag)
            DEBUG_LOG << "\b\b";
          DEBUG_LOG << "],\n\t\t";
          DEBUG_LOG << "id: \"" << monitorDevice.DeviceID << "\",\n\t\t";
          DEBUG_LOG << "key: \"" << monitorDevice.DeviceKey << "\",\n\t\t";
          DEBUG_LOG << "current_display_mode: {"
          DEBUG_LOG << "display_modes: [ \n\t\t\t";*/

          ++monitorIndex;

        }

        DEBUG_LOG << DisplayAdapter(displayDevice, std::span{ monitorIndices.data(), monitorIndices.size() }) << ", ";
        
        ++deviceIndex;
      }
      DEBUG_LOG << "\b\b]";

      DEBUG_LOG << "\n\n";
      DEBUG_LOG << newline << "monitors: ";
      printArray<const Internal::MonitorImpl>(DEBUG_LOG, { systemMonitors.data(), systemMonitors.size() });

      for (auto& monitor : systemMonitors)
        systemMonitorPointers.push_back(&monitor);
    }
    inline std::atomic_flag hasLoaded{};
    inline void loadMonitors(){
      if (!hasLoaded.test_and_set()) {
        findMonitors();
      }
    }
  }


  bool monitor::isPrimary() const noexcept {
    return pImpl->primary;
  }
  u32 monitor::width() const noexcept {
    return pImpl->currentGraphicsMode.pixelWidth;
  }
  u32 monitor::height() const noexcept {
    return pImpl->currentGraphicsMode.pixelHeight;
  }
  graphics::Position2D<i32> monitor::position() const noexcept {
    return pImpl->position;
  }
  u32 monitor::frequency() const noexcept {
    return pImpl->currentGraphicsMode.displayFrequency;
  }

  std::string_view monitor::name() const noexcept {
    return pImpl->name;
  }
  std::string_view monitor::path() const noexcept {
    return pImpl->path;
  }

  monitor monitor::getPrimary() noexcept {
    loadMonitors();
    for (auto& monitor : systemMonitors) {
      if (monitor.primary) {
        return monitor(static_cast<Impl*>(&monitor));
      }
    }
    return monitor(nullptr);
  }
  std::span<monitor> monitor::enumerate() noexcept {
    loadMonitors();
    return { reinterpret_cast<monitor *>(systemMonitorPointers.data()), systemMonitorPointers.size() };
  }
}