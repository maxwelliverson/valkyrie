//
// Created by Maxwell on 2020-11-19.
//

#ifndef VALKYRIE_GENERIC_STATUS_CODE_HPP
#define VALKYRIE_GENERIC_STATUS_CODE_HPP


#include "severity.hpp"
#include "status_code.hpp"

namespace valkyrie{
  
  enum class code : i32 {
    BadCast                       = -67,
    ObjectNotInitialized          = -66,
    InvalidatedState              = -65,
    BadDriver                     = -64,
    NotImplemented                = -63,
    BadAlignment                  = -62,
    TooFragmented                 = -61,
    AddressNotAvailable           = -60,

    DeviceUnrecoverableState      = -59,
    DeviceNotFound                = -58,
    DeviceUnknown                 = -57,
    DeviceBadAddress              = -56,
    DeviceBadTransfer             = -55,
    DeviceBusy                    = -54,
    IncompatibleDevice            = -53,
    DeviceOutOfMemory             = -52,
    LostDevice                    = -51,
    DeviceLimitReached            = -50,


    ConnectionLimitReached        = -49,
    AlreadyConnected              = -48,
    NotConnected                  = -47,
    ConnectionReset               = -46,
    LostConnection                = -45,
    ConnectionAborted             = -44,
    ConnectionRefused             = -43,
    NetworkUnavailable            = -42,

    IOError                       = -41,

    BadInvariant                  = -40,
    FailedAssertion               = -40,
    DataCorruption                = -39,
    OutOfDomain                   = -38,
    OutOfRange                    = -37,
    BadVersion                    = -35,
    Interrupted                   = -34,
    InvalidFormat                 = -33,
    InferenceFailure              = -32,
    InvalidConfiguration          = -31,

    ResourceOutOfDate             = -30,
    ResourceAlreadyExists         = -29,
    ResourceTooLarge              = -28,
    ResourceLimitReached          = -27,
    ResourceUnexpectedType        = -26,
    IncompatibleResource          = -25,
    InvalidResource               = -24,
    ResourceDestructionFailure    = -23,
    ResourceUnavailable           = -22,
    ResourceNotFound              = -21,
    ResourceAlreadyAcquired       = -20,
    ResourceNotOwned              = -19,
    LostResource                  = -18,


    InitializationFailure         = -17,
    NotSupported                  = -16,
    Deadlock                      = -15,
    InUse                         = -14,
    RecursionDepth                = -13,
    InvalidArgument               = -12,
    InsufficientSize              = -11,
    ArithmeticOverflow            = -10,
    StackOverflow                 = -9,
    OutOfBoundsAccess             = -8,
    OutOfMemory                   = -7,
    PermissionDenied              = -6,
    UnrecoverableState            = -5,
    BadAddress                    = -4,
    ExternalError                 = -3,
    UnhandledError                = -2,
    Unknown                       = -1,

    Success                       = 0,

    Busy                          = 1,
    NotReady                      = 2,
    Waiting                       = 3,
    Idle                          = 4,
    Done                          = 5,
    Set                           = 6,
    Unset                         = 7,
    Cancelled                     = 8,
    InProgress                    = 9,
    FurtherActionsRequired        = 10,
    TimedOut                      = 11,
    EndOfFile                     = 12,
    NoChange                      = 13,
    Changed                       = 14,
    Unnecessary                   = 15,
    Paused                        = 16
  };

  inline severity get_default_severity(code code) noexcept{





    switch (code) {
      case code::LostDevice:
      case code::IncompatibleDevice:
      case code::BadVersion:
      case code::StackOverflow:
      case code::OutOfBoundsAccess:
      case code::OutOfMemory:
      case code::UnrecoverableState:
      case code::BadAddress:
      case code::FailedAssertion:
        return severity::fatal;


      case code::FurtherActionsRequired:
      case code::Unnecessary:
        return severity::warning;


      case code::Success:
        return severity::success;

      case code::InProgress:
      case code::Busy:
      case code::NotReady:
      case code::Waiting:
      case code::Idle:
      case code::Done:
      case code::Set:
      case code::Unset:
      case code::Cancelled:
      case code::TimedOut:
      case code::EndOfFile:
      case code::Changed:
      case code::NoChange:
        return severity::info;



        //InUse                         = -14,
        //RecursionDepth                = -13,
        //BadAlignment                  = -62,
        //TooFragmented                 = -61,
        //AddressNotAvailable           = -60,

        //DeviceUnrecoverableState      = -59,
        //DeviceNotFound                = -58,
        //DeviceUnknown                 = -57,
        //DeviceBadAddress              = -56,
        //DeviceBadTransfer             = -55,
        //DeviceBusy                    = -54,
        //DeviceOutOfMemory             = -52,
        //DeviceLimitReached            = -50,


        //ConnectionLimitReached        = -49,
        //AlreadyConnected              = -48,
        //NotConnected                  = -47,
        //ConnectionReset               = -46,
        //LostConnection                = -45,
        //ConnectionAborted             = -44,
        //ConnectionRefused             = -43,
        //NetworkUnavailable            = -42,

        //IOError                       = -41,

      case code::ExternalError:
      case code::NotImplemented:
      case code::IncompatibleResource:
      case code::InitializationFailure:
      case code::ResourceDestructionFailure:
      case code::ResourceUnavailable:
      case code::ResourceNotFound:
      case code::ResourceAlreadyAcquired:
      case code::ResourceNotOwned:
      case code::LostResource:
      case code::InvalidResource:
      case code::PermissionDenied:
      case code::NotSupported:
      case code::InvalidArgument:
      case code::InsufficientSize:
      case code::ArithmeticOverflow:
      case code::InferenceFailure:
      case code::InvalidConfiguration:
      case code::Deadlock:
      case code::Unknown:
      default:
        return severity::error;
    }
  }

  class generic_domain final : public status_domain{
  public:
    using status_type = status_code<generic_domain>;
    using error_type  = error_code<generic_domain>;
    using value_type  = code;
    using domain_type = generic_domain;

    inline constexpr static uuid class_id{"4be5eed5-f1c2-4495-8630-0c5d8107d4a1"};

    constexpr generic_domain() noexcept : status_domain(class_id){};
    ~generic_domain() = default;

    VK_nodiscard string_ref name() const noexcept override;
    VK_nodiscard string_ref do_message(const status_code<void>& Stat) const noexcept override;
    VK_nodiscard code do_generic_code(const status_code<void>& ) const noexcept override;
    VK_nodiscard bool do_failure(const status_code<void> &) const noexcept override;
    VK_nodiscard bool do_equivalent(const status_code<void>& , const status_code<void>&) const noexcept override;
    VK_noreturn  void do_throw_exception(const status_code<void>& code) const override;

    VK_nodiscard static constexpr const generic_domain& get() noexcept;
  };

  using generic_error  = error_code<generic_domain>;
  using generic_status = status_code<generic_domain>;

  namespace detail{
    inline constexpr static generic_domain genericDomainInstance_{};
  }


  inline constexpr const generic_domain& generic_domain::get() noexcept {
    return detail::genericDomainInstance_;
  }
  inline generic_status makeStatusCode(code code) noexcept {
    return generic_status(std::in_place, code);
  }



}

/*
EACCES          code::PermissionDenied
*EADDRINUSE     code::AddressInUse
*EADDRNOTAVAIL  code::AddressNotAvailable
EAGAIN          code::ResourceUnavailable
EALREADY       code::InProgress
**EBADE           Invalid exchange.
*EBADF          code::ResourceDoesNotExist
*EBADFD         code::FileCorrupted
EBADMSG        code::InvalidResource
EBUSY          code::Busy
ECANCELED      code::Cancelled
**ECHILD        No child processes (POSIX.1-2001).
ECHRNG         code::OutOfDomain
**ECOMM           .Communication error on send
*ECONNABORTED    code::ConnectionAborted
*ECONNREFUSED    code::ConnectionRefused
*ECONNRESET      code::ConnectionReset
*EDEADLK         code::Deadlock
**EDESTADDRREQ    Destination address required (POSIX.1-2001).
EDOM            code::OutOfDomain
**EDQUOT          Disk quota exceeded (POSIX.1-2001).
*EEXIST          File exists (POSIX.1-2001).
EFAULT           code::BadAddress
*EFBIG           code::FileTooLarge
**EHOSTDOWN       Host is down.
**EHOSTUNREACH    Host is unreachable (POSIX.1-2001).
**EHWPOISON       Memory page has hardware error.
**EIDRM           Identifier removed (POSIX.1-2001).
EILSEQ          code::InvalidFormat
EINPROGRESS     code::InProgress
**EINTR           Interrupted function call (POSIX.1-2001); see
EINVAL         code::InvalidArgument
EIO            code::IOError
*EISCONN       code::SocketIsConnected
EISDIR         code::ResourceUnexpectedType
*EKEYEXPIRED    code::ResourceExpired
*EKEYREJECTED   code::ResourceRejected
*EKEYREVOKED    code::AccessRevoked
**EL2HLT          Level 2 halted.
**EL2NSYNC        Level 2 not synchronized.
**EL3HLT          Level 3 halted.
**EL3RST          Level 3 reset.
*ELIBACC        code::LibraryInaccessible
*ELIBBAD        code::LibraryCorrupted
*ELIBMAX        code::TooManyLibraries
**ELIBSCN         .lib section in a.out corrupted
**ELIBEXEC        Cannot exec a shared library directly.
**ELNRANGE        Link number out of range.
*ELOOP          code::RecursionLimitReached
**EMEDIUMTYPE     Wrong medium type.
EMFILE          code::ResourceLimitReached
EMLINK          code::ResourceLimitReached
*EMSGSIZE        code::MessageTooLong
**EMULTIHOP       Multihop attempted (POSIX.1-2001).
*ENAMETOOLONG    code::FilenameTooLong
*ENETDOWN        code::NetworkUnavailable
*ENETRESET       code::NetworkUnavailable
*ENETUNREACH     code::NetworkUnavailable
ENFILE           code::ResourceLimitReached
**ENOANO          No anode.
ENOBUFS         code::InsufficientSize
*ENODATA        code::NoMessages
*ENODEV         code::NoSuchDevice
*ENOENT         code::ResourceDoesNotExist
*ENOEXEC        code::ResourceNotExecutable
**ENOKEY          Required key not available.
**ENOLCK          No locks available (POSIX.1-2001).
**ENOLINK         Link has been severed (POSIX.1-2001).
**ENOMEDIUM       No medium found.
ENOMEM          code::OutOfMemory
*ENOMSG         code::NoMessages
*ENONET         code::NetworkUnavailable
**ENOPKG          Package not installed.
**ENOPROTOOPT     Protocol not available (POSIX.1-2001).
*ENOSPC         code::DeviceOutOfMemory
**ENOSR           No STREAM resources (POSIX.1 (XSI STREAMS option)).
ENOSTR          code::ResourceUnexpectedType
ENOSYS          code::NotImplemented
**ENOTBLK         Block device required.
*ENOTCONN       code::NotConnected
ENOTDIR       code::ResourceUnexpectedType
**ENOTEMPTY       Directory not empty (POSIX.1-2001).
ENOTRECOVERABLE code::UnrecoverableState
ENOTSOCK        code::ResourceUnexpectedType
ENOTSUP         code::OperationNotSupported
ENOTTY          code::IOError
**ENOTUNIQ        Name not unique on network.
ENXIO           code::BadAddress
EOPNOTSUPP      code::OperationNotSupported
EOVERFLOW       code::ArithmeticOverflow
**EOWNERDEAD      Owner died (POSIX.1-2008).
EPERM            code::PermissionDenied
**EPFNOSUPPORT    Protocol family not supported.
**EPIPE           Broken pipe (POSIX.1-2001).
**EPROTO          Protocol error (POSIX.1-2001).
**EPROTONOSUPPORT Protocol not supported (POSIX.1-2001).
EPROTOTYPE       code::ResourceUnexpectedType
ERANGE           code::OutOfRange
**EREMCHG         Remote address changed.
**EREMOTE         Object is remote.
**EREMOTEIO       Remote I/O error.
**ERESTART        Interrupted system call should be restarted.
**ERFKILL         Operation not possible due to RF-kill.
**EROFS           Read-only filesystem (POSIX.1-2001).
**ESHUTDOWN       Cannot send after transport endpoint shutdown.
**ESPIPE          Invalid seek (POSIX.1-2001).
ESOCKTNOSUPPORT  code::ResourceUnexpectedType
ESRCH            code::ResourceNotFound
*ESTALE          code::ResourceExpired
**ESTRPIPE        Streams pipe error.
ETIME           code::TimedOut
ETIMEDOUT       code::TimedOut
ETOOMANYREFS    code::ResourceLimitReached
ETXTBSY         code::Busy
**EUCLEAN         Structure needs cleaning.
**EUNATCH         Protocol driver not attached.
EUSERS          code::ResourceLimitReached
**EWOULDBLOCK     Operation would block (may be same value as EAGAIN)
**EXDEV           Improper link (POSIX.1-2001).
     * */

#endif //VALKYRIE_GENERIC_STATUS_CODE_HPP
