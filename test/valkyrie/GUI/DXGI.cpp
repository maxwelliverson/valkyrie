//
// Created by Maxwell on 2020-12-03.
//

#include "../../../src/valkyrie/GUI/DXGI.hpp"


#include <iostream>

namespace dxgi = valkyrie::GUI::Internal::DXGI;

using valkyrie::Core::panic;

int main() {
  auto factory = *dxgi::createFactory();
  auto adapter = *factory.getAdapter(0);
  std::cout << "DXGI Adapter: " << adapter.getName() << std::endl;
  std::cout << "VRAM: " << adapter.dedicatedVideoMemory() << std::endl;
  std::cout << "SysRAM: " << adapter.dedicatedSystemMemory() << std::endl;
  std::cout << "Shared: " << adapter.sharedSystemMemory() << std::endl;
  std::cout << "Last Error: " << adapter.getLastError().message().c_str() << std::endl;


  auto output = *adapter.getMonitor(0);
  auto outputLuminance = output.getLuminance();
  auto ffLuminance = output.getFullFrameLuminance();

  std::cout << "\nDXGI Monitor: " << output.getName() << "\n";
  std::cout << "Luminance: { min: " << outputLuminance.min << ", max: " << outputLuminance.max << " }\n";
  std::cout << "FFLuminance: { min: " << ffLuminance.min << ", max: " << ffLuminance.max << " }\n";

  /*if (!LoadLibrary("SOMELIBRARYLMAO"))
    panic(valkyrie::Core::System::Win32::getLastError());*/
}