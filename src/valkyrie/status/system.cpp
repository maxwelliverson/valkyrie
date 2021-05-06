//
// Created by Maxwell on 2020-11-21.
//

#include <valkyrie/preprocessor.hpp>

#include <valkyrie/status/generic_code.hpp>

#include <cerrno>


namespace {
  using namespace valkyrie;

  using code = code;

  inline code translatePosixCodeToGeneric(const int posixError) noexcept {
    switch(posixError) {
      case EACCES: return code::permission_denied;
      case EADDRINUSE:
      case EADDRNOTAVAIL: return code::address_not_available;
      case EAGAIN:  return code::resource_unavailable;
      case EALREADY: return code::in_progress;
      case EBADF: return code::resource_not_found;
      case EBADMSG: return code::invalid_resource;
      case EBUSY: return code::busy;
      case ECANCELED: return code::cancelled;
      case ECHILD: return code::resource_not_found;
      case ECONNABORTED: return code::connection_aborted;
      case ECONNREFUSED: return code::connection_refused;
      case ECONNRESET:   return code::connection_reset;
      case EDEADLK:      return code::deadlock;
        //**EDESTADDRREQ    Destination address required (POSIX.1-2001).
      case EDOM: return code::out_of_domain;
      case EEXIST: return code::resource_already_exists;
      case EFAULT: return code::bad_address;

        //**EHOSTUNREACH    Host is unreachable (POSIX.1-2001)
        //**EIDRM           Identifier removed (POSIX.1-2001)
      case EILSEQ: return code::invalid_format;
      case EINPROGRESS: return code::in_progress;
      case EINTR:  return code::interrupted;
      case EINVAL: return code::invalid_argument;
      case EIO:    return code::io_error;
      case EISCONN: return code::already_connected;
      case EISDIR: return code::resource_unexpected_type;
      case ELOOP:  return code::recursion_depth;
      case EMFILE:
      case EMLINK: return code::resource_limit_reached;
      case EFBIG:
      case EMSGSIZE:
      case ENAMETOOLONG: return code::resource_too_large;
      case ENETDOWN:
      case ENETRESET:
      case ENETUNREACH: return code::network_unavailable;
      case ENFILE: return code::resource_limit_reached;
      case ENOBUFS: return code::insufficient_size;
        //*case ENODATA: return code::no_messages;
      case ENODEV: return code::device_not_found;
      case ENOENT: return code::resource_not_found;
        //*case ENOEXEC: return code::resource_not_executable;
        //**ENOLCK          No locks available (POSIX.1-2001).
        //**ENOLINK         Link has been severed (POSIX.1-2001).
      case ENOMEM: return code::out_of_memory;
        //*case ENOMSG: return code::no_messages;
        //**ENOPROTOOPT     Protocol not available (POSIX.1-2001).
      case ENOSPC: return code::device_out_of_memory;
        //**ENOSR           No STREAM resources (POSIX.1 (XSI STREAMS option)).
      case ENOSTR: return code::resource_unexpected_type;
      case ENOSYS: return code::not_implemented;
      case ENOTCONN: return code::not_connected;
      case ENOTDIR: return code::resource_unexpected_type;
        //**ENOTEMPTY       Directory not empty (POSIX.1-2001).
      case ENOTRECOVERABLE: return code::unrecoverable_state;
      case ENOTSOCK: return code::resource_unexpected_type;
      case ENOTSUP: return code::not_supported;
      case ENOTTY: return code::io_error;
      case ENXIO: return code::bad_address;
      case EOVERFLOW: return code::arithmetic_overflow;
        //**EOWNERDEAD      Owner died (POSIX.1-2008).
      case EPERM: return code::permission_denied;
        //**EPIPE           Broken pipe (POSIX.1-2001).
        //**EPROTO          Protocol error (POSIX.1-2001).
        //**EPROTONOSUPPORT Protocol not supported (POSIX.1-2001).
      case EPROTOTYPE: return code::resource_unexpected_type;
      case ERANGE: return code::out_of_range;
        //**ESPIPE          Invalid seek (POSIX.1-2001).
      case ESRCH: return code::resource_not_found;
      case ETIME:
      case ETIMEDOUT: return code::timed_out;
      case ETXTBSY: return code::busy;
        //**EWOULDBLOCK     Operation would block (may be same value as EAGAIN)
        //**EXDEV           Improper link (POSIX.1-2001).
      default:
        return code::unknown;
    }
  }
}


#if VK_compiler_msvc

#include <valkyrie/status/system/windows.hpp>

#include <atomic>

namespace {
  inline code translateWin32CodeToGeneric(const uint32_t dwordValue) noexcept {
    switch (dwordValue) {
      case 0x1: return code::not_implemented;
      case 0x2:
      case 0x3: return code::resource_not_found;
      case 0x4: return code::resource_limit_reached;
      case 0x5: return code::permission_denied;
      case 0x6: return code::invalid_resource;
      case 0x8: return code::out_of_memory;
      case 0xb: return code::invalid_format;
      case 0xc: return code::permission_denied;
      case 0xe: return code::out_of_memory;
      case 0xf: return code::device_not_found;
      case 0x10:
      //case 0x11: return EXDEV;
      case 0x13: return code::permission_denied;
      case 0x14: return code::device_not_found;
      case 0x15: return code::device_busy;
      case 0x19: return code::io_error;
      case 0x1d:
      case 0x1e: return code::device_bad_transfer;
      case 0x1f: return code::external_error;
      case 0x20: return code::permission_denied;
      //case 0x21: return ENOLCK;
      case 0x26: return code::end_of_file;
      case 0x27: return code::device_out_of_memory;
      case 0x37: return code::lost_device;
      case 0x38: return code::resource_limit_reached;
      case 0x3C: return code::incompatible_device;
      case 0x41: return code::connection_refused;
      case 0x44:
      case 0x45: return code::resource_limit_reached;
      case 0x47: return code::connection_limit_reached;
      case 0x50: return code::resource_already_exists;
      case 0x52: return code::permission_denied;
      case 0x57: return code::invalid_argument;
      case 0x6e: return code::io_error;
      case 0x6f: return code::resource_too_large;
      case 0x70: return code::device_out_of_memory;
      case 0x78: return code::not_implemented;
      case 0x79: return code::timed_out;
      case 0x7A: return code::insufficient_size;
      case 0x7b:
      case 0x7c: return code::invalid_argument;
      case 0x7e:
      case 0x7f: return code::resource_not_found;
      case 0x83: return code::invalid_argument;
      case 0x8e: return code::device_busy;
      //case 0x91: return code::not_empty;
      case 0x94: return code::resource_unavailable;
      case 0x9a: return code::resource_too_large;
      case 0x9b: return code::resource_limit_reached;
      case 0x9f:
      case 0xa0: return code::invalid_argument;
      case 0xa4: return code::resource_limit_reached;
      case 0xaa: return code::busy;
      case 0xb7: return code::resource_already_exists;
      case 0xbb: return code::resource_not_found;
      //case 0xd4: return ENOLCK;
      case 0x10b: return code::invalid_argument;
      case 0x120: return code::resource_not_owned;
      case 0x12b: return code::further_actions_required;
      case 0x1e7: return code::bad_address;
      case 0x216: return code::arithmetic_overflow;
      case 0x217: return code::already_connected;
      case 0x225: return code::bad_alignment;
      case 0x23b: return code::permission_denied;
      case 0x23e: return code::unhandled_error;
      case 0x23f: return code::initialization_failure;
      case 0x246:
      case 0x247: return code::invalid_format;
      case 0x257: return code::stack_overflow;
      case 0x25d: return code::data_corruption;
      case 0x287: return code::initialization_failure;
      case 0x29c: return code::failed_assertion;
      case 0x2b0: return code::unhandled_error;
      case 0x2c6: return code::done;
      case 0x2c7: return code::waiting;
      case 0x30b: return code::data_corruption;
      case 0x3e3: return code::cancelled;
      case 0x3e6: return code::permission_denied;
      case 0x3e9: return code::stack_overflow;
      case 0x3ec: return code::invalid_argument;
      //case 0x3f3:
      //case 0x3f4:
      //case 0x3f5: return EIO;
      case 0x4c7: return code::cancelled;
      case 0x4cd: return code::not_connected;
      case 0x4ce: return code::already_connected;
      case 0x4cf: return code::network_unavailable;
      case 0x4d3:
      case 0x4d4: return code::connection_aborted;
      case 0x4d5: return code::busy;
      case 0x4d6: return code::connection_limit_reached;
      case 0x7d0: return code::invalid_format;
      case 0x961:
      case 0x964: return code::device_busy;
      case 0x2714: return code::interrupted;
      case 0x2719: return code::resource_not_found;
      case 0x271d: return code::permission_denied;
      case 0x271e: return code::bad_address;
      case 0x2726: return code::invalid_argument;
      case 0x2728: return code::resource_limit_reached;
      case 0x2733: return code::busy;
      case 0x2734:
      case 0x2735: return code::in_progress;
      case 0x2736: return code::resource_unexpected_type;
      //case 0x2737: return EDESTADDRREQ;
      case 0x2738: return code::resource_too_large;
      //case 0x2739: return EPROTOTYPE;
      //case 0x273a: return ENOPROTOOPT;
      case 0x273b:
      case 0x273d:
      case 0x273f: return code::not_supported;
      case 0x2740:
      case 0x2741: return code::address_not_available;
      case 0x2742:
      case 0x2743:
      case 0x2744: return code::network_unavailable;
      case 0x2745: return code::connection_aborted;
      case 0x2746: return code::connection_reset;
      case 0x2747: return code::insufficient_size;
      case 0x2748: return code::already_connected;
      case 0x2749: return code::not_connected;
      case 0x274c: return code::timed_out;
      case 0x274d: return code::connection_refused;
      case 0x274f: return code::resource_too_large;
      //case 0x2751: return EHOSTUNREACH;
      case 0x3BC3: return code::device_not_found;
      default:
        return code::unknown;
    }
  }
  inline int translateWin32StatusToPosixCode(const uint32_t dwordValue) noexcept {
    switch (dwordValue) {
      case 0x1: return ENOSYS;
      case 0x2:
      case 0x3: return ENOENT;
      case 0x4: return EMFILE;
      case 0x5: return EACCES;
      case 0x6: return EINVAL;
      case 0x8: return ENOMEM;
      case 0xc: return EACCES;
      case 0xe: return ENOMEM;
      case 0xf: return ENODEV;
      case 0x10: return EACCES;
      case 0x11: return EXDEV;
      case 0x13: return EACCES;
      case 0x14: return ENODEV;
      case 0x15: return EAGAIN;
      case 0x19:
      case 0x1d:
      case 0x1e: return EIO;
      case 0x20: return EACCES;
      case 0x21: return ENOLCK;
      case 0x27: return ENOSPC;
      case 0x37: return ENODEV;
      case 0x50: return EEXIST;
      case 0x52: return EACCES;
      case 0x57: return EINVAL;
      case 0x6e: return EIO;
      case 0x6f: return ENAMETOOLONG;
      case 0x70: return ENOSPC;
      case 0x7b:
      case 0x83: return EINVAL;
      case 0x8e: return EBUSY;
      case 0x91: return ENOTEMPTY;
      case 0xaa: return EBUSY;
      case 0xb7: return EEXIST;
      case 0xd4: return ENOLCK;
      case 0x10b: return EINVAL;
      case 0x3e3: return ECANCELED;
      case 0x3e6: return EACCES;
      case 0x3f3:
      case 0x3f4:
      case 0x3f5: return EIO;
      case 0x4d5: return EAGAIN;
      case 0x961:
      case 0x964: return EBUSY;
      case 0x2714: return EINTR;
      case 0x2719: return EBADF;
      case 0x271d: return EACCES;
      case 0x271e: return EFAULT;
      case 0x2726: return EINVAL;
      case 0x2728: return EMFILE;
      case 0x2733: return EWOULDBLOCK;
      case 0x2734: return EINPROGRESS;
      case 0x2735: return EALREADY;
      case 0x2736: return ENOTSOCK;
      case 0x2737: return EDESTADDRREQ;
      case 0x2738: return EMSGSIZE;
      case 0x2739: return EPROTOTYPE;
      case 0x273a: return ENOPROTOOPT;
      case 0x273b: return EPROTONOSUPPORT;
      case 0x273d: return EOPNOTSUPP;
      case 0x273f: return EAFNOSUPPORT;
      case 0x2740: return EADDRINUSE;
      case 0x2741: return EADDRNOTAVAIL;
      case 0x2742: return ENETDOWN;
      case 0x2743: return ENETUNREACH;
      case 0x2744: return ENETRESET;
      case 0x2745: return ECONNABORTED;
      case 0x2746: return ECONNRESET;
      case 0x2747: return ENOBUFS;
      case 0x2748: return EISCONN;
      case 0x2749: return ENOTCONN;
      case 0x274c: return ETIMEDOUT;
      case 0x274d: return ECONNREFUSED;
      case 0x274f: return ENAMETOOLONG;
      case 0x2751: return EHOSTUNREACH;
      default:
        return -1;
    }
  }
}

struct valkyrie::sys::win32_status_domain::ref_counted_win32_string::payload{

  inline static constexpr u32 initialStringLength = 16;
  inline static constexpr u32 stringSizeGrowthRate = 2;

  std::atomic_uint32_t refCount = 1;
  u32 stringLength = 0;
  char stringBuffer[];

  payload() = default;
  explicit payload(u32& initialLength){
    initialLength = initialStringLength;
  }

  inline bool setMessage(DWORD messageId, u32 strLength) noexcept {
    stringLength = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 nullptr,
                                 messageId,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 stringBuffer,
                                 strLength,
                                 nullptr);
    if (stringLength) {
      if (std::string_view(&stringBuffer[stringLength - 2], 2) == "\r\n") {
        stringLength -= 2;
        stringBuffer[stringLength] = '\0';
      }
      return true;
    }
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
      std::exit(GetLastError());
    return false;
  }

  inline static void* operator new(size_t, payload* pPayload, u32& strLength) noexcept {
    strLength *= stringSizeGrowthRate;
    return std::realloc(pPayload, offsetof(payload, stringBuffer) + strLength);
  }

  static void* operator new(size_t) noexcept {
    return std::malloc(offsetof(payload, stringBuffer) + initialStringLength);
  }
  static void operator delete(void* pPayload) noexcept {
    std::free(pPayload);
  }
};

valkyrie::sys::win32_status_domain::ref_counted_win32_string::payload *
valkyrie::sys::win32_status_domain::ref_counted_win32_string::makeNewPayload(const u32 messageId) noexcept {
  u32 stringLength = 16;
  auto* pPayload = new payload(stringLength);
  while (!pPayload->setMessage(messageId, stringLength))
    pPayload = std::launder(new(pPayload, stringLength) payload);
  return pPayload;
}
void valkyrie::sys::win32_status_domain::ref_counted_win32_string::refCountThunk(base_type *pTo, const base_type *pFrom, _thunk_op Op) noexcept {
  VK_assert(pTo);
  auto* pPayload = static_cast<payload*>(static_cast<const ref_counted_win32_string*>(pTo)->_state[0]);
  if (pPayload) {
    switch(Op) {
      case _thunk_op::copy:
        VK_assert(pFrom);
        pPayload->refCount.fetch_add(1, std::memory_order_relaxed);
        break;
      case _thunk_op::move:
        VK_assert(pFrom);
        ((ref_counted_win32_string*)pFrom)->_state[0] = nullptr;
        break;
      case _thunk_op::destruct:
        if (!pPayload->refCount.fetch_sub(1, std::memory_order_release))
          delete pPayload;
        break;
      VK_no_default;
    }
  }
}
valkyrie::sys::win32_status_domain::ref_counted_win32_string::ref_counted_win32_string(payload *pPayload) noexcept
    : base_type(pPayload->stringBuffer, pPayload->stringLength, pPayload, nullptr, nullptr, refCountThunk) { }

valkyrie::severity valkyrie::sys::win32_status_domain::do_severity(const status_code<void> &status) const noexcept{
  return win32::NativeHResult(HRESULT_FROM_WIN32(static_cast<const status_type&>(status).value())).severity();
}
valkyrie::code valkyrie::sys::win32_status_domain::do_generic_code(const status_code<void>& status) const noexcept {
  return translateWin32CodeToGeneric(static_cast<const status_type&>(status).value());
}
bool valkyrie::sys::win32_status_domain::do_failure(const status_code<void> &status) const noexcept{
  return HRESULT_FROM_WIN32(static_cast<const status_type&>(status).value()) < 0;
}
bool valkyrie::sys::win32_status_domain::do_equivalent(const status_code<void>& A, const status_code<void>& B) const noexcept {
  return translateWin32CodeToGeneric(static_cast<const status_type&>(A).value()) == B.generic();
}

valkyrie::sys::win32::status valkyrie::sys::win32::get_last_error() noexcept {
  return status{ GetLastError() };
}

#endif

/*#include <valkyrie/status/system/Posix.hpp>
#include <valkyrie/adt/flat_map.hpp>

#include <shared_mutex>

namespace {

  namespace system = valkyrie::system;



  class PosixErrorMessageTable{
    flat_map<int, flat_map<int, const utf8*>> table_;
    mutable std::shared_mutex mutex;
  public:

    explicit PosixErrorMessageTable();


  };
}

code system::detail::posix_status_domain::doCode(const status_code<void> &status) const noexcept {
  VK_assert(status.domain() == *this);
  return translatePosixCodeToGeneric(static_cast<const status_type&>(status).value());
}
bool system::detail::posix_status_domain::doEquivalent(const status_code<void> &statusA, const status_code<void> &statusB) const noexcept {
  VK_assert(statusA.domain() == *this);
  if (statusB.domain() == *this)
    return static_cast<const status_type&>(statusA).value() == static_cast<const status_type&>(statusB).value();
#if VK_system_windows
  else if (statusB.domain() == system::win32_status_domain())
    return static_cast<const status_type&>(statusA).value() ==
           translateWin32StatusToPosixCode(static_cast<const system::win32::status&>(statusB).value());
#endif
  return false;
}
bool system::detail::posix_status_domain::doFailure(const status_code<void> &status) const noexcept {
  VK_assert(status.domain() == *this);
  return static_cast<const status_type&>(status).value();
}
string_ref system::detail::posix_status_domain::doMessage(const status_code<void> &status) const noexcept {}
severity system::detail::posix_status_domain::doSeverity(const status_code<void> &status) const noexcept {}*/
