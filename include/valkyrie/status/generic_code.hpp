//
// Created by Maxwell on 2020-11-19.
//

#ifndef VALKYRIE_GENERIC_STATUS_CODE_HPP
#define VALKYRIE_GENERIC_STATUS_CODE_HPP


#include "severity.hpp"
#include "status_code.hpp"

namespace valkyrie{
  
  enum class code : i32 {

    bad_ordering                   = -71,

    unescaped_characters           = -70,
    missing_null_terminator         = -69,
    failed_parse                   = -68,


    bad_cast                       = -67,
    object_not_initialized          = -66,
    invalidated_state              = -65,
    bad_driver                     = -64,
    not_implemented                = -63,
    bad_alignment                  = -62,
    too_fragmented                 = -61,
    address_not_available           = -60,

    device_unrecoverable_state      = -59,
    device_not_found                = -58,
    device_unknown                 = -57,
    device_bad_address              = -56,
    device_bad_transfer             = -55,
    device_busy                    = -54,
    incompatible_device            = -53,
    device_out_of_memory             = -52,
    lost_device                    = -51,
    device_limit_reached            = -50,


    connection_limit_reached        = -49,
    already_connected              = -48,
    not_connected                  = -47,
    connection_reset               = -46,
    lost_connection                = -45,
    connection_aborted             = -44,
    connection_refused             = -43,
    network_unavailable            = -42,

    io_error                       = -41,

    bad_invariant                  = -40,
    failed_assertion               = -40,
    data_corruption                = -39,
    out_of_domain                   = -38,
    out_of_range                    = -37,
    bad_version                    = -35,
    interrupted                   = -34,
    invalid_format                 = -33,

    invalid_configuration          = -32,

    resource_is_empty               = -31,
    resource_out_of_date             = -30,
    resource_already_exists         = -29,
    resource_too_large              = -28,
    resource_limit_reached          = -27,
    resource_unexpected_type        = -26,
    incompatible_resource          = -25,
    invalid_resource               = -24,
    resource_destruction_failure    = -23,
    resource_unavailable           = -22,
    resource_not_found              = -21,
    resource_already_acquired       = -20,
    resource_not_owned              = -19,
    lost_resource                  = -18,


    initialization_failure         = -17,
    not_supported                  = -16,
    deadlock                      = -15,
    in_use                         = -14,
    recursion_depth                = -13,
    invalid_argument               = -12,
    insufficient_size              = -11,
    arithmetic_overflow            = -10,
    stack_overflow                 = -9,
    out_of_bounds_access             = -8,
    out_of_memory                   = -7,
    permission_denied              = -6,
    unrecoverable_state            = -5,
    bad_address                    = -4,
    external_error                 = -3,
    unhandled_error                = -2,
    unknown                       = -1,

    success                       = 0,

    busy                          = 1,
    not_ready                     = 2,
    waiting                       = 3,
    idle                          = 4,
    done                          = 5,
    set                           = 6,
    unset                         = 7,
    cancelled                     = 8,
    in_progress                    = 9,
    further_actions_required        = 10,
    timed_out                      = 11,
    end_of_file                     = 12,
    no_change                      = 13,
    changed                       = 14,
    unnecessary                   = 15,
    paused                        = 16
  };

  inline severity get_default_severity(code code) noexcept{


    switch (code) {

      case code::lost_device:
      case code::incompatible_device:
      case code::bad_version:
      case code::out_of_memory:
      case code::unrecoverable_state:
      case code::bad_address:
      case code::bad_driver:
      case code::device_unrecoverable_state:
      case code::device_bad_address:
      case code::device_out_of_memory:
      case code::data_corruption:
        return severity::fatal;


      case code::further_actions_required:
      case code::unnecessary:
      case code::resource_is_empty:
      case code::resource_out_of_date:
        return severity::warning;


      case code::success:
        return severity::success;

      case code::in_progress:
      case code::busy:
      case code::not_ready:
      case code::waiting:
      case code::idle:
      case code::done:
      case code::set:
      case code::unset:
      case code::cancelled:
      case code::timed_out:
      case code::end_of_file:
      case code::changed:
      case code::no_change:
      case code::in_use:
      case code::paused:
        return severity::info;



      case code::external_error:
      case code::not_implemented:
      case code::incompatible_resource:
      case code::initialization_failure:
      case code::resource_destruction_failure:
      case code::resource_unavailable:
      case code::resource_not_found:
      case code::resource_already_acquired:
      case code::resource_not_owned:
      case code::lost_resource:
      case code::invalid_resource:
      case code::permission_denied:
      case code::not_supported:
      case code::invalid_argument:
      case code::insufficient_size:
      case code::arithmetic_overflow:
      case code::invalid_configuration:
      case code::unescaped_characters:
      case code::missing_null_terminator:
      case code::failed_parse:
      case code::bad_cast:
      case code::bad_ordering:
      case code::interrupted:
      case code::invalidated_state:
      case code::too_fragmented:
      case code::address_not_available:
      case code::device_not_found:
      case code::device_unknown:
      case code::device_bad_transfer:
      case code::device_busy:
      case code::device_limit_reached:
      case code::connection_limit_reached:
      case code::already_connected:
      case code::not_connected:
      case code::connection_reset:
      case code::lost_connection:
      case code::connection_aborted:
      case code::connection_refused:
      case code::network_unavailable:
      case code::io_error:
      case code::out_of_domain:
      case code::out_of_range:
      case code::invalid_format:
      case code::resource_already_exists:
      case code::resource_too_large:
      case code::resource_limit_reached:
      case code::resource_unexpected_type:
        return severity::error;



      case code::object_not_initialized:
      case code::bad_alignment:
      case code::deadlock:
      case code::stack_overflow:
      case code::out_of_bounds_access:
      case code::recursion_depth:
      case code::unhandled_error:
      case code::failed_assertion:
      case code::unknown:
        return severity::internal_error;

      VK_no_default;
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
  /*inline generic_status make_status_code(code code) noexcept {
    return generic_status(*//*std::in_place, &generic_domain::get(), *//*code);
  }*/
}

/*
EACCES          code::permission_denied
*EADDRINUSE     code::address_in_use
*EADDRNOTAVAIL  code::address_not_available
EAGAIN          code::resource_unavailable
EALREADY       code::in_progress
**EBADE           Invalid exchange.
*EBADF          code::resource_does_not_exist
*EBADFD         code::file_corrupted
EBADMSG        code::invalid_resource
EBUSY          code::busy
ECANCELED      code::cancelled
**ECHILD        No child processes (POSIX.1-2001).
ECHRNG         code::out_of_domain
**ECOMM           .Communication error on send
*ECONNABORTED    code::connection_aborted
*ECONNREFUSED    code::connection_refused
*ECONNRESET      code::connection_reset
*EDEADLK         code::deadlock
**EDESTADDRREQ    Destination address required (POSIX.1-2001).
EDOM            code::out_of_domain
**EDQUOT          Disk quota exceeded (POSIX.1-2001).
*EEXIST          File exists (POSIX.1-2001).
EFAULT           code::bad_address
*EFBIG           code::file_too_large
**EHOSTDOWN       Host is down.
**EHOSTUNREACH    Host is unreachable (POSIX.1-2001).
**EHWPOISON       Memory page has hardware error.
**EIDRM           Identifier removed (POSIX.1-2001).
EILSEQ          code::invalid_format
EINPROGRESS     code::in_progress
**EINTR           Interrupted function call (POSIX.1-2001); see
EINVAL         code::invalid_argument
EIO            code::io_error
*EISCONN       code::socket_is_connected
EISDIR         code::resource_unexpected_type
*EKEYEXPIRED    code::resource_expired
*EKEYREJECTED   code::resource_rejected
*EKEYREVOKED    code::access_revoked
**EL2HLT          Level 2 halted.
**EL2NSYNC        Level 2 not synchronized.
**EL3HLT          Level 3 halted.
**EL3RST          Level 3 reset.
*ELIBACC        code::library_inaccessible
*ELIBBAD        code::library_corrupted
*ELIBMAX        code::too_many_libraries
**ELIBSCN         .lib section in a.out corrupted
**ELIBEXEC        Cannot exec a shared library directly.
**ELNRANGE        Link number out of range.
*ELOOP          code::recursion_limit_reached
**EMEDIUMTYPE     Wrong medium type.
EMFILE          code::resource_limit_reached
EMLINK          code::resource_limit_reached
*EMSGSIZE        code::message_too_long
**EMULTIHOP       Multihop attempted (POSIX.1-2001).
*ENAMETOOLONG    code::filename_too_long
*ENETDOWN        code::network_unavailable
*ENETRESET       code::network_unavailable
*ENETUNREACH     code::network_unavailable
ENFILE           code::resource_limit_reached
**ENOANO          No anode.
ENOBUFS         code::insufficient_size
*ENODATA        code::no_messages
*ENODEV         code::no_such_device
*ENOENT         code::resource_does_not_exist
*ENOEXEC        code::resource_not_executable
**ENOKEY          Required key not available.
**ENOLCK          No locks available (POSIX.1-2001).
**ENOLINK         Link has been severed (POSIX.1-2001).
**ENOMEDIUM       No medium found.
ENOMEM          code::out_of_memory
*ENOMSG         code::no_messages
*ENONET         code::network_unavailable
**ENOPKG          Package not installed.
**ENOPROTOOPT     Protocol not available (POSIX.1-2001).
*ENOSPC         code::device_out_of_memory
**ENOSR           No STREAM resources (POSIX.1 (XSI STREAMS option)).
ENOSTR          code::resource_unexpected_type
ENOSYS          code::not_implemented
**ENOTBLK         Block device required.
*ENOTCONN       code::not_connected
ENOTDIR       code::resource_unexpected_type
**ENOTEMPTY       Directory not empty (POSIX.1-2001).
ENOTRECOVERABLE code::unrecoverable_state
ENOTSOCK        code::resource_unexpected_type
ENOTSUP         code::operation_not_supported
ENOTTY          code::io_error
**ENOTUNIQ        Name not unique on network.
ENXIO           code::bad_address
EOPNOTSUPP      code::operation_not_supported
EOVERFLOW       code::arithmetic_overflow
**EOWNERDEAD      Owner died (POSIX.1-2008).
EPERM            code::permission_denied
**EPFNOSUPPORT    Protocol family not supported.
**EPIPE           Broken pipe (POSIX.1-2001).
**EPROTO          Protocol error (POSIX.1-2001).
**EPROTONOSUPPORT Protocol not supported (POSIX.1-2001).
EPROTOTYPE       code::resource_unexpected_type
ERANGE           code::out_of_range
**EREMCHG         Remote address changed.
**EREMOTE         Object is remote.
**EREMOTEIO       Remote I/O error.
**ERESTART        Interrupted system call should be restarted.
**ERFKILL         Operation not possible due to RF-kill.
**EROFS           Read-only filesystem (POSIX.1-2001).
**ESHUTDOWN       Cannot send after transport endpoint shutdown.
**ESPIPE          Invalid seek (POSIX.1-2001).
ESOCKTNOSUPPORT  code::resource_unexpected_type
ESRCH            code::resource_not_found
*ESTALE          code::resource_expired
**ESTRPIPE        Streams pipe error.
ETIME           code::timed_out
ETIMEDOUT       code::timed_out
ETOOMANYREFS    code::resource_limit_reached
ETXTBSY         code::busy
**EUCLEAN         Structure needs cleaning.
**EUNATCH         Protocol driver not attached.
EUSERS          code::resource_limit_reached
**EWOULDBLOCK     Operation would block (may be same value as EAGAIN)
**EXDEV           Improper link (POSIX.1-2001).
     * */

#endif //VALKYRIE_GENERIC_STATUS_CODE_HPP
