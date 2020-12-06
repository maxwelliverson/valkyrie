//
// Created by Maxwell on 2020-11-21.
//

#include <valkyrie/Core/Config.hpp>

#if VK_compiler_msvc

#include <valkyrie/Core/Error/System/Windows.hpp>

#include <atomic>

namespace {
  using Code = valkyrie::Core::Code;

  inline Code translateWin32CodeToGeneric(const uint32_t dwordValue) noexcept {
    switch (dwordValue) {
      case 0x1: return Code::NotImplemented;
      case 0x2:
      case 0x3: return Code::ResourceNotFound;
      case 0x4: return Code::ResourceLimitReached;
      case 0x5: return Code::PermissionDenied;
      case 0x6: return Code::InvalidResource;
      case 0x8: return Code::OutOfMemory;
      case 0xb: return Code::InvalidFormat;
      case 0xc: return Code::PermissionDenied;
      case 0xe: return Code::OutOfMemory;
      case 0xf: return Code::DeviceNotFound;
      case 0x10:
      //case 0x11: return EXDEV;
      case 0x13: return Code::PermissionDenied;
      case 0x14: return Code::DeviceNotFound;
      case 0x15: return Code::DeviceBusy;
      case 0x19: return Code::IOError;
      case 0x1d:
      case 0x1e: return Code::DeviceBadTransfer;
      case 0x1f: return Code::ExternalError;
      case 0x20: return Code::PermissionDenied;
      //case 0x21: return ENOLCK;
      case 0x26: return Code::EndOfFile;
      case 0x27: return Code::DeviceOutOfMemory;
      case 0x37: return Code::LostDevice;
      case 0x38: return Code::ResourceLimitReached;
      case 0x3C: return Code::IncompatibleDevice;
      case 0x41: return Code::ConnectionRefused;
      case 0x44:
      case 0x45: return Code::ResourceLimitReached;
      case 0x47: return Code::ConnectionLimitReached;
      case 0x50: return Code::ResourceAlreadyExists;
      case 0x52: return Code::PermissionDenied;
      case 0x57: return Code::InvalidArgument;
      case 0x6e: return Code::IOError;
      case 0x6f: return Code::ResourceTooLarge;
      case 0x70: return Code::DeviceOutOfMemory;
      case 0x78: return Code::NotImplemented;
      case 0x79: return Code::TimedOut;
      case 0x7A: return Code::InsufficientSize;
      case 0x7b:
      case 0x7c: return Code::InvalidArgument;
      case 0x7e:
      case 0x7f: return Code::ResourceNotFound;
      case 0x83: return Code::InvalidArgument;
      case 0x8e: return Code::DeviceBusy;
      //case 0x91: return Code::NotEmpty;
      case 0x94: return Code::ResourceUnavailable;
      case 0x9a: return Code::ResourceTooLarge;
      case 0x9b: return Code::ResourceLimitReached;
      case 0x9f:
      case 0xa0: return Code::InvalidArgument;
      case 0xa4: return Code::ResourceLimitReached;
      case 0xaa: return Code::Busy;
      case 0xb7: return Code::ResourceAlreadyExists;
      case 0xbb: return Code::ResourceNotFound;
      //case 0xd4: return ENOLCK;
      case 0x10b: return Code::InvalidArgument;
      case 0x120: return Code::ResourceNotOwned;
      case 0x12b: return Code::FurtherActionsRequired;
      case 0x1e7: return Code::BadAddress;
      case 0x216: return Code::ArithmeticOverflow;
      case 0x217: return Code::AlreadyConnected;
      case 0x225: return Code::BadAlignment;
      case 0x23b: return Code::PermissionDenied;
      case 0x23e: return Code::UnhandledError;
      case 0x23f: return Code::InitializationFailure;
      case 0x246:
      case 0x247: return Code::InvalidFormat;
      case 0x257: return Code::StackOverflow;
      case 0x25d: return Code::DataCorruption;
      case 0x287: return Code::InitializationFailure;
      case 0x29c: return Code::FailedAssertion;
      case 0x2b0: return Code::UnhandledError;
      case 0x2c6: return Code::Done;
      case 0x2c7: return Code::Waiting;
      case 0x30b: return Code::DataCorruption;
      case 0x3e3: return Code::Cancelled;
      case 0x3e6: return Code::PermissionDenied;
      case 0x3e9: return Code::StackOverflow;
      case 0x3ec: return Code::InvalidArgument;
      //case 0x3f3:
      //case 0x3f4:
      //case 0x3f5: return EIO;
      case 0x4c7: return Code::Cancelled;
      case 0x4cd: return Code::NotConnected;
      case 0x4ce: return Code::AlreadyConnected;
      case 0x4cf: return Code::NetworkUnavailable;
      case 0x4d3:
      case 0x4d4: return Code::ConnectionAborted;
      case 0x4d5: return Code::Busy;
      case 0x4d6: return Code::ConnectionLimitReached;
      case 0x7d0: return Code::InvalidFormat;
      case 0x961:
      case 0x964: return Code::DeviceBusy;
      case 0x2714: return Code::Interrupted;
      case 0x2719: return Code::ResourceNotFound;
      case 0x271d: return Code::PermissionDenied;
      case 0x271e: return Code::BadAddress;
      case 0x2726: return Code::InvalidArgument;
      case 0x2728: return Code::ResourceLimitReached;
      case 0x2733: return Code::Busy;
      case 0x2734:
      case 0x2735: return Code::InProgress;
      case 0x2736: return Code::ResourceUnexpectedType;
      //case 0x2737: return EDESTADDRREQ;
      case 0x2738: return Code::ResourceTooLarge;
      //case 0x2739: return EPROTOTYPE;
      //case 0x273a: return ENOPROTOOPT;
      case 0x273b:
      case 0x273d:
      case 0x273f: return Code::NotSupported;
      case 0x2740:
      case 0x2741: return Code::AddressNotAvailable;
      case 0x2742:
      case 0x2743:
      case 0x2744: return Code::NetworkUnavailable;
      case 0x2745: return Code::ConnectionAborted;
      case 0x2746: return Code::ConnectionReset;
      case 0x2747: return Code::InsufficientSize;
      case 0x2748: return Code::AlreadyConnected;
      case 0x2749: return Code::NotConnected;
      case 0x274c: return Code::TimedOut;
      case 0x274d: return Code::ConnectionRefused;
      case 0x274f: return Code::ResourceTooLarge;
      //case 0x2751: return EHOSTUNREACH;
      case 0x3BC3: return Code::DeviceNotFound;
      default:
        return Code::Unknown;
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
  inline valkyrie::Core::Code translatePosixCodeToGeneric(const int posixError) noexcept {
    switch(posixError) {
      case EACCES: return Code::PermissionDenied;
      case EADDRINUSE:
      case EADDRNOTAVAIL: return Code::AddressNotAvailable;
      case EAGAIN:  return Code::ResourceUnavailable;
      case EALREADY: return Code::InProgress;
      case EBADF: return Code::ResourceNotFound;
      case EBADMSG: return Code::InvalidResource;
      case EBUSY: return Code::Busy;
      case ECANCELED: return Code::Cancelled;
      case ECHILD: return Code::ResourceNotFound;
      case ECONNABORTED: return Code::ConnectionAborted;
      case ECONNREFUSED: return Code::ConnectionRefused;
      case ECONNRESET:   return Code::ConnectionReset;
      case EDEADLK:      return Code::Deadlock;
      //**EDESTADDRREQ    Destination address required (POSIX.1-2001).
      case EDOM: return Code::OutOfDomain;
      case EEXIST: return Code::ResourceAlreadyExists;
      case EFAULT: return Code::BadAddress;

      //**EHOSTUNREACH    Host is unreachable (POSIX.1-2001)
      //**EIDRM           Identifier removed (POSIX.1-2001)
      case EILSEQ: return Code::InvalidFormat;
      case EINPROGRESS: return Code::InProgress;
      case EINTR:  return Code::Interrupted;
      case EINVAL: return Code::InvalidArgument;
      case EIO:    return Code::IOError;
      case EISCONN: return Code::AlreadyConnected;
      case EISDIR: return Code::ResourceUnexpectedType;
      case ELOOP:  return Code::RecursionDepth;
      case EMFILE:
      case EMLINK: return Code::ResourceLimitReached;
      case EFBIG:
      case EMSGSIZE:
      case ENAMETOOLONG: return Code::ResourceTooLarge;
      case ENETDOWN:
      case ENETRESET:
      case ENETUNREACH: return Code::NetworkUnavailable;
      case ENFILE: return Code::ResourceLimitReached;
      case ENOBUFS: return Code::InsufficientSize;
      //*case ENODATA: return Code::NoMessages;
      case ENODEV: return Code::DeviceNotFound;
      case ENOENT: return Code::ResourceNotFound;
      //*case ENOEXEC: return Code::ResourceNotExecutable;
      //**ENOLCK          No locks available (POSIX.1-2001).
      //**ENOLINK         Link has been severed (POSIX.1-2001).
      case ENOMEM: return Code::OutOfMemory;
      //*case ENOMSG: return Code::NoMessages;
      //**ENOPROTOOPT     Protocol not available (POSIX.1-2001).
      case ENOSPC: return Code::DeviceOutOfMemory;
      //**ENOSR           No STREAM resources (POSIX.1 (XSI STREAMS option)).
      case ENOSTR: return Code::ResourceUnexpectedType;
      case ENOSYS: return Code::NotImplemented;
      case ENOTCONN: return Code::NotConnected;
      case ENOTDIR: return Code::ResourceUnexpectedType;
      //**ENOTEMPTY       Directory not empty (POSIX.1-2001).
      case ENOTRECOVERABLE: return Code::UnrecoverableState;
      case ENOTSOCK: return Code::ResourceUnexpectedType;
      case ENOTSUP: return Code::NotSupported;
      case ENOTTY: return Code::IOError;
      case ENXIO: return Code::BadAddress;
      case EOPNOTSUPP: return Code::NotSupported;
      case EOVERFLOW: return Code::ArithmeticOverflow;
      //**EOWNERDEAD      Owner died (POSIX.1-2008).
      case EPERM: return Code::PermissionDenied;
      //**EPIPE           Broken pipe (POSIX.1-2001).
      //**EPROTO          Protocol error (POSIX.1-2001).
      //**EPROTONOSUPPORT Protocol not supported (POSIX.1-2001).
      case EPROTOTYPE: return Code::ResourceUnexpectedType;
      case ERANGE: return Code::OutOfRange;
        //**ESPIPE          Invalid seek (POSIX.1-2001).
      case ESRCH: return Code::ResourceNotFound;
      case ETIME:
      case ETIMEDOUT: return Code::TimedOut;
      case ETXTBSY: return Code::Busy;
      //**EWOULDBLOCK     Operation would block (may be same value as EAGAIN)
      //**EXDEV           Improper link (POSIX.1-2001).
      default:
        return Code::Unknown;
    }
  }
}

struct valkyrie::Core::System::Win32StatusDomain::RefCountedWin32String::payload{

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

valkyrie::Core::System::Win32StatusDomain::RefCountedWin32String::payload *
valkyrie::Core::System::Win32StatusDomain::RefCountedWin32String::makeNewPayload(const u32 messageId) noexcept {
  u32 stringLength = 16;
  auto* pPayload = new payload(stringLength);
  while (!pPayload->setMessage(messageId, stringLength))
    pPayload = std::launder(new(pPayload, stringLength) payload);
  return pPayload;
}
void valkyrie::Core::System::Win32StatusDomain::RefCountedWin32String::refCountThunk(base_type *pTo, const base_type *pFrom, _thunk_op Op) noexcept {
  VK_assert(pTo);
  auto* pPayload = static_cast<payload*>(static_cast<const RefCountedWin32String*>(pTo)->_state[0]);
  if (pPayload) {
    switch(Op) {
      case _thunk_op::copy:
        VK_assert(pFrom);
        pPayload->refCount.fetch_add(1, std::memory_order_relaxed);
        break;
      case _thunk_op::move:
        VK_assert(pFrom);
        ((RefCountedWin32String*)pFrom)->_state[0] = nullptr;
        break;
      case _thunk_op::destruct:
        if (!pPayload->refCount.fetch_sub(1, std::memory_order_release))
          delete pPayload;
        break;
      VK_no_default;
    }
  }
}
valkyrie::Core::System::Win32StatusDomain::RefCountedWin32String::RefCountedWin32String(payload *pPayload) noexcept
    : base_type(pPayload->stringBuffer, pPayload->stringLength, pPayload, nullptr, nullptr, refCountThunk) { }

valkyrie::Core::Severity valkyrie::Core::System::Win32StatusDomain::doSeverity(const StatusCode<void> &status) const noexcept{
  return Win32::NativeHResult(HRESULT_FROM_WIN32(static_cast<const status_type&>(status).value())).severity();
}
valkyrie::Core::Code valkyrie::Core::System::Win32StatusDomain::doCode(const StatusCode<void>& status) const noexcept {
  return translateWin32CodeToGeneric(static_cast<const status_type&>(status).value());
}
bool valkyrie::Core::System::Win32StatusDomain::doFailure(const StatusCode<void> &status) const noexcept{
  return HRESULT_FROM_WIN32(static_cast<const status_type&>(status).value()) < 0;
}
bool valkyrie::Core::System::Win32StatusDomain::doEquivalent(const StatusCode<void>& A, const StatusCode<void>& B) const noexcept {
  return translateWin32CodeToGeneric(static_cast<const status_type&>(A).value()) == B.generic();
}
const valkyrie::Core::Uuid & valkyrie::Core::System::Win32StatusDomain::id() const noexcept {
  //return class_traits<Win32StatusDomain>::uuid;
  //return class_traits<Win32StatusDomain>::uuid;
  return Traits::Singleton<Win32StatusDomain>::uuid;
}

valkyrie::Core::System::Win32::Status valkyrie::Core::System::Win32::getLastError() noexcept {
  return Status{ GetLastError() };
}

#else
#endif
