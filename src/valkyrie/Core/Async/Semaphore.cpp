//
// Created by Maxwell on 2020-11-18.
//

#include <valkyrie/Core/Error/System/Win32.hpp>
#include <valkyrie/Core/Async/Semaphore.hpp>

#if !VK_std_has_semaphore
#define NOMINMAX
#include <synchapi.h>
//#include <errhandlingapi.h>
#include <windows.h>
#include <intrin.h>

using namespace valkyrie;

namespace {

#define abortWin32Error() panic(System::Win32::getLastError())

  /*VK_noreturn void abortWindowsError(const char* pFunctionName, const char* pFileName, int LineNumber) noexcept {
    constexpr static size_t ErrorBufferSize = 4096;
    DWORD lastError = GetLastError();
    char errorMessageBuffer[ErrorBufferSize];

  }*/
}

void Core::Detail::Semaphore::acquire() noexcept {
  i64 compare = 0;
  if(!WaitOnAddress(&counter, &compare, sizeof(compare), std::numeric_limits<u32>::max())) VK_unlikely {
    abortWin32Error();
  }
  _InterlockedDecrement64(&counter);
}
void Core::Detail::Semaphore::release(i64 incValue) noexcept {

}
bool Core::Detail::Semaphore::tryAcquire() noexcept {}
bool Core::Detail::Semaphore::doTryAcquireFor(const std::chrono::seconds& s) noexcept {}
bool Core::Detail::Semaphore::doTryAcquireFor(const std::chrono::nanoseconds& ns) noexcept {}
bool Core::Detail::Semaphore::doTryAcquireFor(const std::chrono::microseconds& us) noexcept {}
bool Core::Detail::Semaphore::doTryAcquireFor(const std::chrono::milliseconds& ms) noexcept {}
bool Core::Detail::Semaphore::doTryAcquireUntil(const std::chrono::steady_clock::time_point &absTime) noexcept {}
bool Core::Detail::Semaphore::doTryAcquireUntil(const std::chrono::system_clock::time_point &absTime) noexcept {}


bool Core::CountingSemaphore<1>::tryAcquire() noexcept {}
void Core::CountingSemaphore<1>::release() noexcept {}
void Core::CountingSemaphore<1>::acquire() noexcept {}
bool Core::CountingSemaphore<1>::doTryAcquireUntil(const std::chrono::steady_clock::time_point &absTime) noexcept {}
bool Core::CountingSemaphore<1>::doTryAcquireUntil(const std::chrono::system_clock::time_point &absTime) noexcept {}
bool Core::CountingSemaphore<1>::doTryAcquireFor(const std::chrono::seconds &ns) noexcept {}
bool Core::CountingSemaphore<1>::doTryAcquireFor(const std::chrono::milliseconds &ns) noexcept {}
bool Core::CountingSemaphore<1>::doTryAcquireFor(const std::chrono::microseconds &ns) noexcept {}
bool Core::CountingSemaphore<1>::doTryAcquireFor(const std::chrono::nanoseconds &ns) noexcept {}
#endif