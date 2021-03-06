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
      case EACCES: return code::PermissionDenied;
      case EADDRINUSE:
      case EADDRNOTAVAIL: return code::AddressNotAvailable;
      case EAGAIN:  return code::ResourceUnavailable;
      case EALREADY: return code::InProgress;
      case EBADF: return code::ResourceNotFound;
      case EBADMSG: return code::InvalidResource;
      case EBUSY: return code::Busy;
      case ECANCELED: return code::Cancelled;
      case ECHILD: return code::ResourceNotFound;
      case ECONNABORTED: return code::ConnectionAborted;
      case ECONNREFUSED: return code::ConnectionRefused;
      case ECONNRESET:   return code::ConnectionReset;
      case EDEADLK:      return code::Deadlock;
        //**EDESTADDRREQ    Destination address required (POSIX.1-2001).
      case EDOM: return code::OutOfDomain;
      case EEXIST: return code::ResourceAlreadyExists;
      case EFAULT: return code::BadAddress;

        //**EHOSTUNREACH    Host is unreachable (POSIX.1-2001)
        //**EIDRM           Identifier removed (POSIX.1-2001)
      case EILSEQ: return code::InvalidFormat;
      case EINPROGRESS: return code::InProgress;
      case EINTR:  return code::Interrupted;
      case EINVAL: return code::InvalidArgument;
      case EIO:    return code::IOError;
      case EISCONN: return code::AlreadyConnected;
      case EISDIR: return code::ResourceUnexpectedType;
      case ELOOP:  return code::RecursionDepth;
      case EMFILE:
      case EMLINK: return code::ResourceLimitReached;
      case EFBIG:
      case EMSGSIZE:
      case ENAMETOOLONG: return code::ResourceTooLarge;
      case ENETDOWN:
      case ENETRESET:
      case ENETUNREACH: return code::NetworkUnavailable;
      case ENFILE: return code::ResourceLimitReached;
      case ENOBUFS: return code::InsufficientSize;
        //*case ENODATA: return code::NoMessages;
      case ENODEV: return code::DeviceNotFound;
      case ENOENT: return code::ResourceNotFound;
        //*case ENOEXEC: return code::ResourceNotExecutable;
        //**ENOLCK          No locks available (POSIX.1-2001).
        //**ENOLINK         Link has been severed (POSIX.1-2001).
      case ENOMEM: return code::OutOfMemory;
        //*case ENOMSG: return code::NoMessages;
        //**ENOPROTOOPT     Protocol not available (POSIX.1-2001).
      case ENOSPC: return code::DeviceOutOfMemory;
        //**ENOSR           No STREAM resources (POSIX.1 (XSI STREAMS option)).
      case ENOSTR: return code::ResourceUnexpectedType;
      case ENOSYS: return code::NotImplemented;
      case ENOTCONN: return code::NotConnected;
      case ENOTDIR: return code::ResourceUnexpectedType;
        //**ENOTEMPTY       Directory not empty (POSIX.1-2001).
      case ENOTRECOVERABLE: return code::UnrecoverableState;
      case ENOTSOCK: return code::ResourceUnexpectedType;
      case ENOTSUP: return code::NotSupported;
      case ENOTTY: return code::IOError;
      case ENXIO: return code::BadAddress;
      case EOVERFLOW: return code::ArithmeticOverflow;
        //**EOWNERDEAD      Owner died (POSIX.1-2008).
      case EPERM: return code::PermissionDenied;
        //**EPIPE           Broken pipe (POSIX.1-2001).
        //**EPROTO          Protocol error (POSIX.1-2001).
        //**EPROTONOSUPPORT Protocol not supported (POSIX.1-2001).
      case EPROTOTYPE: return code::ResourceUnexpectedType;
      case ERANGE: return code::OutOfRange;
        //**ESPIPE          Invalid seek (POSIX.1-2001).
      case ESRCH: return code::ResourceNotFound;
      case ETIME:
      case ETIMEDOUT: return code::TimedOut;
      case ETXTBSY: return code::Busy;
        //**EWOULDBLOCK     Operation would block (may be same value as EAGAIN)
        //**EXDEV           Improper link (POSIX.1-2001).
      default:
        return code::Unknown;
    }
  }
}


#if VK_compiler_msvc

#include <valkyrie/status/System/windows.hpp>

#include <atomic>

namespace {
  inline code translateWin32CodeToGeneric(const uint32_t dwordValue) noexcept {
    switch (dwordValue) {
      case 0x1: return code::NotImplemented;
      case 0x2:
      case 0x3: return code::ResourceNotFound;
      case 0x4: return code::ResourceLimitReached;
      case 0x5: return code::PermissionDenied;
      case 0x6: return code::InvalidResource;
      case 0x8: return code::OutOfMemory;
      case 0xb: return code::InvalidFormat;
      case 0xc: return code::PermissionDenied;
      case 0xe: return code::OutOfMemory;
      case 0xf: return code::DeviceNotFound;
      case 0x10:
      //case 0x11: return EXDEV;
      case 0x13: return code::PermissionDenied;
      case 0x14: return code::DeviceNotFound;
      case 0x15: return code::DeviceBusy;
      case 0x19: return code::IOError;
      case 0x1d:
      case 0x1e: return code::DeviceBadTransfer;
      case 0x1f: return code::ExternalError;
      case 0x20: return code::PermissionDenied;
      //case 0x21: return ENOLCK;
      case 0x26: return code::EndOfFile;
      case 0x27: return code::DeviceOutOfMemory;
      case 0x37: return code::LostDevice;
      case 0x38: return code::ResourceLimitReached;
      case 0x3C: return code::IncompatibleDevice;
      case 0x41: return code::ConnectionRefused;
      case 0x44:
      case 0x45: return code::ResourceLimitReached;
      case 0x47: return code::ConnectionLimitReached;
      case 0x50: return code::ResourceAlreadyExists;
      case 0x52: return code::PermissionDenied;
      case 0x57: return code::InvalidArgument;
      case 0x6e: return code::IOError;
      case 0x6f: return code::ResourceTooLarge;
      case 0x70: return code::DeviceOutOfMemory;
      case 0x78: return code::NotImplemented;
      case 0x79: return code::TimedOut;
      case 0x7A: return code::InsufficientSize;
      case 0x7b:
      case 0x7c: return code::InvalidArgument;
      case 0x7e:
      case 0x7f: return code::ResourceNotFound;
      case 0x83: return code::InvalidArgument;
      case 0x8e: return code::DeviceBusy;
      //case 0x91: return code::NotEmpty;
      case 0x94: return code::ResourceUnavailable;
      case 0x9a: return code::ResourceTooLarge;
      case 0x9b: return code::ResourceLimitReached;
      case 0x9f:
      case 0xa0: return code::InvalidArgument;
      case 0xa4: return code::ResourceLimitReached;
      case 0xaa: return code::Busy;
      case 0xb7: return code::ResourceAlreadyExists;
      case 0xbb: return code::ResourceNotFound;
      //case 0xd4: return ENOLCK;
      case 0x10b: return code::InvalidArgument;
      case 0x120: return code::ResourceNotOwned;
      case 0x12b: return code::FurtherActionsRequired;
      case 0x1e7: return code::BadAddress;
      case 0x216: return code::ArithmeticOverflow;
      case 0x217: return code::AlreadyConnected;
      case 0x225: return code::BadAlignment;
      case 0x23b: return code::PermissionDenied;
      case 0x23e: return code::UnhandledError;
      case 0x23f: return code::InitializationFailure;
      case 0x246:
      case 0x247: return code::InvalidFormat;
      case 0x257: return code::StackOverflow;
      case 0x25d: return code::DataCorruption;
      case 0x287: return code::InitializationFailure;
      case 0x29c: return code::FailedAssertion;
      case 0x2b0: return code::UnhandledError;
      case 0x2c6: return code::Done;
      case 0x2c7: return code::Waiting;
      case 0x30b: return code::DataCorruption;
      case 0x3e3: return code::Cancelled;
      case 0x3e6: return code::PermissionDenied;
      case 0x3e9: return code::StackOverflow;
      case 0x3ec: return code::InvalidArgument;
      //case 0x3f3:
      //case 0x3f4:
      //case 0x3f5: return EIO;
      case 0x4c7: return code::Cancelled;
      case 0x4cd: return code::NotConnected;
      case 0x4ce: return code::AlreadyConnected;
      case 0x4cf: return code::NetworkUnavailable;
      case 0x4d3:
      case 0x4d4: return code::ConnectionAborted;
      case 0x4d5: return code::Busy;
      case 0x4d6: return code::ConnectionLimitReached;
      case 0x7d0: return code::InvalidFormat;
      case 0x961:
      case 0x964: return code::DeviceBusy;
      case 0x2714: return code::Interrupted;
      case 0x2719: return code::ResourceNotFound;
      case 0x271d: return code::PermissionDenied;
      case 0x271e: return code::BadAddress;
      case 0x2726: return code::InvalidArgument;
      case 0x2728: return code::ResourceLimitReached;
      case 0x2733: return code::Busy;
      case 0x2734:
      case 0x2735: return code::InProgress;
      case 0x2736: return code::ResourceUnexpectedType;
      //case 0x2737: return EDESTADDRREQ;
      case 0x2738: return code::ResourceTooLarge;
      //case 0x2739: return EPROTOTYPE;
      //case 0x273a: return ENOPROTOOPT;
      case 0x273b:
      case 0x273d:
      case 0x273f: return code::NotSupported;
      case 0x2740:
      case 0x2741: return code::AddressNotAvailable;
      case 0x2742:
      case 0x2743:
      case 0x2744: return code::NetworkUnavailable;
      case 0x2745: return code::ConnectionAborted;
      case 0x2746: return code::ConnectionReset;
      case 0x2747: return code::InsufficientSize;
      case 0x2748: return code::AlreadyConnected;
      case 0x2749: return code::NotConnected;
      case 0x274c: return code::TimedOut;
      case 0x274d: return code::ConnectionRefused;
      case 0x274f: return code::ResourceTooLarge;
      //case 0x2751: return EHOSTUNREACH;
      case 0x3BC3: return code::DeviceNotFound;
      default:
        return code::Unknown;
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

valkyrie::severity valkyrie::sys::win32_status_domain::doSeverity(const status_code<void> &status) const noexcept{
  return win32::NativeHResult(HRESULT_FROM_WIN32(static_cast<const status_type&>(status).value())).severity();
}
valkyrie::code valkyrie::sys::win32_status_domain::doCode(const status_code<void>& status) const noexcept {
  return translateWin32CodeToGeneric(static_cast<const status_type&>(status).value());
}
bool valkyrie::sys::win32_status_domain::doFailure(const status_code<void> &status) const noexcept{
  return HRESULT_FROM_WIN32(static_cast<const status_type&>(status).value()) < 0;
}
bool valkyrie::sys::win32_status_domain::doEquivalent(const status_code<void>& A, const status_code<void>& B) const noexcept {
  return translateWin32CodeToGeneric(static_cast<const status_type&>(A).value()) == B.generic();
}

valkyrie::sys::win32::status valkyrie::sys::win32::getLastError() noexcept {
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
