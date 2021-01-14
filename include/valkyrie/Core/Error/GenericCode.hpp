//
// Created by Maxwell on 2020-11-19.
//

#ifndef VALKYRIE_GENERIC_STATUS_CODE_HPP
#define VALKYRIE_GENERIC_STATUS_CODE_HPP


#include <valkyrie/Core/Error/Status.hpp>
#include <valkyrie/Core/Error/Severity.hpp>

namespace valkyrie::Core{
  
  enum class Code : i32 {
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
    Unnecessary                   = 15
  };
  inline Severity getDefaultSeverity(Code code) noexcept{


    /*
EACCES          Code::PermissionDenied
*EADDRINUSE     Code::AddressInUse
*EADDRNOTAVAIL  Code::AddressNotAvailable
EAGAIN          Code::ResourceUnavailable
EALREADY       Code::InProgress
**EBADE           Invalid exchange.
*EBADF          Code::ResourceDoesNotExist
*EBADFD         Code::FileCorrupted
EBADMSG        Code::InvalidResource
EBUSY          Code::Busy
ECANCELED      Code::Cancelled
**ECHILD        No child processes (POSIX.1-2001).
ECHRNG         Code::OutOfDomain
**ECOMM           .Communication error on send
*ECONNABORTED    Code::ConnectionAborted
*ECONNREFUSED    Code::ConnectionRefused
*ECONNRESET      Code::ConnectionReset
*EDEADLK         Code::Deadlock
**EDESTADDRREQ    Destination address required (POSIX.1-2001).
EDOM            Code::OutOfDomain
**EDQUOT          Disk quota exceeded (POSIX.1-2001).
*EEXIST          File exists (POSIX.1-2001).
EFAULT           Code::BadAddress
*EFBIG           Code::FileTooLarge
**EHOSTDOWN       Host is down.
**EHOSTUNREACH    Host is unreachable (POSIX.1-2001).
**EHWPOISON       Memory page has hardware error.
**EIDRM           Identifier removed (POSIX.1-2001).
EILSEQ          Code::InvalidFormat
EINPROGRESS     Code::InProgress
**EINTR           Interrupted function call (POSIX.1-2001); see
EINVAL         Code::InvalidArgument
EIO            Code::IOError
*EISCONN       Code::SocketIsConnected
EISDIR         Code::ResourceUnexpectedType
*EKEYEXPIRED    Code::ResourceExpired
*EKEYREJECTED   Code::ResourceRejected
*EKEYREVOKED    Code::AccessRevoked
**EL2HLT          Level 2 halted.
**EL2NSYNC        Level 2 not synchronized.
**EL3HLT          Level 3 halted.
**EL3RST          Level 3 reset.
*ELIBACC        Code::LibraryInaccessible
*ELIBBAD        Code::LibraryCorrupted
*ELIBMAX        Code::TooManyLibraries
**ELIBSCN         .lib section in a.out corrupted
**ELIBEXEC        Cannot exec a shared library directly.
**ELNRANGE        Link number out of range.
*ELOOP          Code::RecursionLimitReached
**EMEDIUMTYPE     Wrong medium type.
EMFILE          Code::ResourceLimitReached
EMLINK          Code::ResourceLimitReached
*EMSGSIZE        Code::MessageTooLong
**EMULTIHOP       Multihop attempted (POSIX.1-2001).
*ENAMETOOLONG    Code::FilenameTooLong
*ENETDOWN        Code::NetworkUnavailable
*ENETRESET       Code::NetworkUnavailable
*ENETUNREACH     Code::NetworkUnavailable
ENFILE           Code::ResourceLimitReached
**ENOANO          No anode.
ENOBUFS         Code::InsufficientSize
*ENODATA        Code::NoMessages
*ENODEV         Code::NoSuchDevice
*ENOENT         Code::ResourceDoesNotExist
*ENOEXEC        Code::ResourceNotExecutable
**ENOKEY          Required key not available.
**ENOLCK          No locks available (POSIX.1-2001).
**ENOLINK         Link has been severed (POSIX.1-2001).
**ENOMEDIUM       No medium found.
ENOMEM          Code::OutOfMemory
*ENOMSG         Code::NoMessages
*ENONET         Code::NetworkUnavailable
**ENOPKG          Package not installed.
**ENOPROTOOPT     Protocol not available (POSIX.1-2001).
*ENOSPC         Code::DeviceOutOfMemory
**ENOSR           No STREAM resources (POSIX.1 (XSI STREAMS option)).
ENOSTR          Code::ResourceUnexpectedType
ENOSYS          Code::NotImplemented
**ENOTBLK         Block device required.
*ENOTCONN       Code::NotConnected
ENOTDIR       Code::ResourceUnexpectedType
**ENOTEMPTY       Directory not empty (POSIX.1-2001).
ENOTRECOVERABLE Code::UnrecoverableState
ENOTSOCK        Code::ResourceUnexpectedType
ENOTSUP         Code::OperationNotSupported
ENOTTY          Code::IOError
**ENOTUNIQ        Name not unique on network.
ENXIO           Code::BadAddress
EOPNOTSUPP      Code::OperationNotSupported
EOVERFLOW       Code::ArithmeticOverflow
**EOWNERDEAD      Owner died (POSIX.1-2008).
EPERM            Code::PermissionDenied
**EPFNOSUPPORT    Protocol family not supported.
**EPIPE           Broken pipe (POSIX.1-2001).
**EPROTO          Protocol error (POSIX.1-2001).
**EPROTONOSUPPORT Protocol not supported (POSIX.1-2001).
EPROTOTYPE       Code::ResourceUnexpectedType
ERANGE           Code::OutOfRange
**EREMCHG         Remote address changed.
**EREMOTE         Object is remote.
**EREMOTEIO       Remote I/O error.
**ERESTART        Interrupted system call should be restarted.
**ERFKILL         Operation not possible due to RF-kill.
**EROFS           Read-only filesystem (POSIX.1-2001).
**ESHUTDOWN       Cannot send after transport endpoint shutdown.
**ESPIPE          Invalid seek (POSIX.1-2001).
ESOCKTNOSUPPORT  Code::ResourceUnexpectedType
ESRCH            Code::ResourceNotFound
*ESTALE          Code::ResourceExpired
**ESTRPIPE        Streams pipe error.
ETIME           Code::TimedOut
ETIMEDOUT       Code::TimedOut
ETOOMANYREFS    Code::ResourceLimitReached
ETXTBSY         Code::Busy
**EUCLEAN         Structure needs cleaning.
**EUNATCH         Protocol driver not attached.
EUSERS          Code::ResourceLimitReached
**EWOULDBLOCK     Operation would block (may be same value as EAGAIN)
**EXDEV           Improper link (POSIX.1-2001).
     * */


    switch (code) {
      case Code::LostDevice:
      case Code::IncompatibleDevice:
      case Code::BadVersion:
      case Code::StackOverflow:
      case Code::OutOfBoundsAccess:
      case Code::OutOfMemory:
      case Code::UnrecoverableState:
      case Code::BadAddress:
      case Code::FailedAssertion:
        return Severity::Fatal;


      case Code::FurtherActionsRequired:
      case Code::Unnecessary:
        return Severity::Warning;


      case Code::Success:
        return Severity::Success;

      case Code::InProgress:
      case Code::Busy:
      case Code::NotReady:
      case Code::Waiting:
      case Code::Idle:
      case Code::Done:
      case Code::Set:
      case Code::Unset:
      case Code::Cancelled:
      case Code::TimedOut:
      case Code::EndOfFile:
      case Code::Changed:
      case Code::NoChange:
        return Severity::Info;



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

      case Code::ExternalError:
      case Code::NotImplemented:
      case Code::IncompatibleResource:
      case Code::InitializationFailure:
      case Code::ResourceDestructionFailure:
      case Code::ResourceUnavailable:
      case Code::ResourceNotFound:
      case Code::ResourceAlreadyAcquired:
      case Code::ResourceNotOwned:
      case Code::LostResource:
      case Code::InvalidResource:
      case Code::PermissionDenied:
      case Code::NotSupported:
      case Code::InvalidArgument:
      case Code::InsufficientSize:
      case Code::ArithmeticOverflow:
      case Code::InferenceFailure:
      case Code::InvalidConfiguration:
      case Code::Deadlock:
      case Code::Unknown:
      default:
        return Severity::Error;
    }
  }

  class GenericDomain final : public StatusDomain{
  public:
    using status_type = StatusCode<GenericDomain>;
    using error_type  = ErrorCode<GenericDomain>;
    using value_type  = Code;
    using domain_type = GenericDomain;

    inline constexpr static Uuid uuid{"4be5eed5-f1c2-4495-8630-0c5d8107d4a1"};

    constexpr GenericDomain() noexcept : StatusDomain(uuid){};
    ~GenericDomain() = default;

    VK_nodiscard StringRef name() const noexcept override;
    VK_nodiscard StringRef doMessage(const StatusCode<void>& Stat) const noexcept override;
    VK_nodiscard Code doCode(const StatusCode<void>& ) const noexcept override;
    VK_nodiscard bool doFailure(const StatusCode<void> &) const noexcept override;
    VK_nodiscard bool doEquivalent(const StatusCode<void>& , const StatusCode<void>&) const noexcept override;
    VK_noreturn  void doThrowException(const StatusCode<void>& code) const override;

    VK_nodiscard static constexpr const GenericDomain& get() noexcept;
  };

  using GenericError  = ErrorCode<GenericDomain>;
  using GenericStatus = StatusCode<GenericDomain>;

  namespace Detail{
    inline constexpr static Core::GenericDomain genericDomainInstance_{};
  }


  inline constexpr const GenericDomain& GenericDomain::get() noexcept {
    return Detail::genericDomainInstance_;
  }
  inline GenericStatus makeStatusCode(Code code) noexcept {
    return GenericStatus(std::in_place, code);
  }



}

#endif //VALKYRIE_GENERIC_STATUS_CODE_HPP
