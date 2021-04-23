//
// Created by Maxwell on 2020-11-18.
//

#include <valkyrie/async/semaphore.hpp>
#include <valkyrie/status/system/win32.hpp>

#if !VK_std_has_semaphore
#define NOMINMAX
#include <synchapi.h>
//#include <errhandlingapi.h>
#include <Windows.h>
#include <intrin.h>

using namespace valkyrie;

namespace {

#define abortWin32Error() panic(sys::win32::get_last_error())

  /*VK_noreturn void abortWindowsError(const char* pFunctionName, const char* pFileName, int LineNumber) noexcept {
    constexpr static size_t ErrorBufferSize = 4096;
    DWORD lastError = GetLastError();
    char errorMessageBuffer[ErrorBufferSize];

  }*/
}

void detail::semaphore::acquire() noexcept {
  i64 compare = 0;
  if(!WaitOnAddress(&counter, &compare, sizeof(compare), std::numeric_limits<u32>::max())) VK_unlikely {
    abortWin32Error();
  }
  _InterlockedDecrement64(&counter);
}
void detail::semaphore::release(i64 incValue) noexcept {

}
bool detail::semaphore::tryAcquire() noexcept {}
bool detail::semaphore::doTryAcquireFor(const std::chrono::seconds& s) noexcept {}
bool detail::semaphore::doTryAcquireFor(const std::chrono::nanoseconds& ns) noexcept {}
bool detail::semaphore::doTryAcquireFor(const std::chrono::microseconds& us) noexcept {}
bool detail::semaphore::doTryAcquireFor(const std::chrono::milliseconds& ms) noexcept {}
bool detail::semaphore::doTryAcquireUntil(const std::chrono::steady_clock::time_point &absTime) noexcept {}
bool detail::semaphore::doTryAcquireUntil(const std::chrono::system_clock::time_point &absTime) noexcept {}


bool counting_semaphore<1>::tryAcquire() noexcept {}
void counting_semaphore<1>::release() noexcept {}
void counting_semaphore<1>::acquire() noexcept {}
bool counting_semaphore<1>::doTryAcquireUntil(const std::chrono::steady_clock::time_point &absTime) noexcept {}
bool counting_semaphore<1>::doTryAcquireUntil(const std::chrono::system_clock::time_point &absTime) noexcept {}
bool counting_semaphore<1>::doTryAcquireFor(const std::chrono::seconds &ns) noexcept {}
bool counting_semaphore<1>::doTryAcquireFor(const std::chrono::milliseconds &ns) noexcept {}
bool counting_semaphore<1>::doTryAcquireFor(const std::chrono::microseconds &ns) noexcept {}
bool counting_semaphore<1>::doTryAcquireFor(const std::chrono::nanoseconds &ns) noexcept {}
#endif