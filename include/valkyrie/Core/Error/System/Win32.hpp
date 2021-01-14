//
// Created by Maxwell on 2020-11-18.
//

#ifndef VALKYRIE_WIN32_ERROR_HPP
#define VALKYRIE_WIN32_ERROR_HPP

#include <valkyrie/Core/Error/Status.hpp>

namespace valkyrie{
  namespace Core::System{
    class Win32StatusDomain;
  }


  template <>
  struct Traits::Singleton<Core::System::Win32StatusDomain>{
    inline constexpr static Core::StringView name = VK_raw_string(Win32StatusDomain);
    inline constexpr static Core::StringView scoped_name = VK_raw_string(valkyrie::Core::System::Detail::Win32StatusDomain);
    inline constexpr static Core::Uuid       uuid{"cf4339da-2b4a-49f9-b66e-8f32eb61d2c9"};

    inline constexpr static const Core::System::Win32StatusDomain& get() noexcept;
  };


  namespace Core{
    namespace System{
      class Win32StatusDomain : public StatusDomain{
      public:

        using domain_type = Win32StatusDomain;
        using status_type = StatusCode<Win32StatusDomain>;
        using error_type  = ErrorCode<Win32StatusDomain>;
        using value_type = u32;

        inline constexpr static Uuid uuid{"cf4339da-2b4a-49f9-b66e-8f32eb61d2c9"};

      private:
        class RefCountedWin32String : public StringRef{
          using base_type = StringRef;

        protected:
          struct payload;

          static payload* makeNewPayload(u32 messageId) noexcept;
          static void refCountThunk(base_type* pTo, const base_type* pFrom, _thunk_op Op) noexcept;

          explicit RefCountedWin32String(payload* pPayload) noexcept;

        public:
          RefCountedWin32String(u32 errorValue) noexcept : RefCountedWin32String(makeNewPayload(errorValue)){}
        };
      public:
        using string_ref = RefCountedWin32String;

        constexpr Win32StatusDomain() noexcept : StatusDomain(uuid){}

        StringRef name() const noexcept override { return VK_raw_string(Win32); }
        StringRef doMessage(const StatusCode<void>& status) const noexcept override{
          VK_assert(status.domain() == *this);
          return string_ref(static_cast<const StatusCode<Win32StatusDomain>&>(status).value());
        }
        Code doCode(const StatusCode<void> &) const noexcept override;
        Severity doSeverity(const StatusCode<void>& status) const noexcept override;
        bool doFailure(const StatusCode<void>& status) const noexcept override;
        bool doEquivalent(const StatusCode<void>& A, const StatusCode<void>& B) const noexcept override;

        constexpr static const Win32StatusDomain& get() noexcept;
      };


      namespace Detail{
        inline constexpr static Win32StatusDomain win32StatusDomain{};
      }



      namespace Win32{
        using Status = StatusCode<Win32StatusDomain>;
        using Error  = ErrorCode<Win32StatusDomain>;

        Status getLastError() noexcept;
      }
    }
    using SystemStatus = System::Win32::Status;
    using SystemError  = System::Win32::Error;
  }
}

constexpr const valkyrie::Core::System::Win32StatusDomain & valkyrie::Traits::Singleton<valkyrie::Core::System::Win32StatusDomain>::get() noexcept {
  return valkyrie::Core::System::Detail::win32StatusDomain;
}
constexpr const valkyrie::Core::System::Win32StatusDomain & valkyrie::Core::System::Win32StatusDomain::get() noexcept {
  return valkyrie::Core::System::Detail::win32StatusDomain;
}


#endif//VALKYRIE_WIN32_ERROR_HPP
