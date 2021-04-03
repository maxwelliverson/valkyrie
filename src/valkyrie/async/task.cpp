//
// Created by Maxwell on 2021-04-02.
//

#include <valkyrie/async/task.hpp>


#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <processthreadsapi.h>
#include <synchapi.h>


using namespace valkyrie;

void autonomous_task::yield() {
  SwitchToThread();
}


std::pair<void*, unsigned> autonomous_task::launchThread_(size_t stackSize, PFN_threadProc threadProc, void* pParams) noexcept {
  SECURITY_ATTRIBUTES secAttr{
      .nLength = sizeof(SECURITY_ATTRIBUTES),
      .lpSecurityDescriptor = nullptr,
      .bInheritHandle = true
  };
  std::pair<void*, unsigned> results;

  results.first = reinterpret_cast<void*>(_beginthreadex(
      &secAttr,
      static_cast<unsigned>(stackSize),
      threadProc,
      pParams,
      0,
      &results.second));

  return results;
}

void                       autonomous_task::setTaskDesc_(void* handle, autonomous_task* pTask) noexcept {

  static constexpr int DescBufferLength = 4096 * 2;
  wchar_t descriptionBuffer[DescBufferLength];

  auto desc = pTask->get_description();
  int result = MultiByteToWideChar(CP_UTF8,
                                   0,
                                   desc.c_str(),
                                   (int)desc.size(),
                                   descriptionBuffer,
                                   DescBufferLength);
  assert(result);
  descriptionBuffer[result] = L'\0';

  auto descResult = SetThreadDescription(handle, descriptionBuffer);

  assert(SUCCEEDED(descResult));
}