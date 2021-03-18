//
// Created by Maxwell on 2020-11-18.
//

#ifndef VALKYRIE_WIN32_ERROR_HPP
#define VALKYRIE_WIN32_ERROR_HPP

#include <valkyrie/status/status_code.hpp>

namespace valkyrie{
  namespace sys{
    class win32_status_domain;
  }


  template <>
  struct traits::Singleton<sys::win32_status_domain>{
    inline constexpr static string_view name = VK_raw_string(win32_status_domain);
    inline constexpr static string_view scoped_name = VK_raw_string(valkyrie::sys::detail::win32_status_domain);
    inline constexpr static uuid       class_id{"cf4339da-2b4a-49f9-b66e-8f32eb61d2c9"};

    inline constexpr static const sys::win32_status_domain& get() noexcept;
  };


  namespace sys{
    class win32_status_domain : public status_domain{
    public:

      using domain_type = win32_status_domain;
      using status_type = status_code<win32_status_domain>;
      using error_type  = error_code<win32_status_domain>;
      using value_type = u32;

      inline constexpr static uuid class_id{"cf4339da-2b4a-49f9-b66e-8f32eb61d2c9"};

    private:
      class ref_counted_win32_string : public string_ref{
        using base_type = string_ref;

      protected:
        struct payload;

        static payload* makeNewPayload(u32 messageId) noexcept;
        static void refCountThunk(base_type* pTo, const base_type* pFrom, _thunk_op Op) noexcept;

        explicit ref_counted_win32_string(payload* pPayload) noexcept;

      public:
        ref_counted_win32_string(u32 errorValue) noexcept : ref_counted_win32_string(makeNewPayload(errorValue)){}
      };
    public:
      //using string_ref = ref_counted_win32_string;

      constexpr win32_status_domain() noexcept : status_domain(class_id){}

      string_ref name() const noexcept override { return VK_raw_string(Win32); }
      string_ref do_message(const status_code<void>& status) const noexcept override{
        VK_assert(status.domain() == *this);
        return ref_counted_win32_string(static_cast<const status_code<win32_status_domain>&>(status).value());
      }
      code do_generic_code(const status_code<void> &) const noexcept override;
      severity do_severity(const status_code<void>& status) const noexcept override;
      bool do_failure(const status_code<void>& status) const noexcept override;
      bool do_equivalent(const status_code<void>& A, const status_code<void>& B) const noexcept override;

      constexpr static const win32_status_domain& get() noexcept;
    };


    namespace detail{
      inline constexpr static win32_status_domain win32_status_domain{};
    }



    namespace win32{
      using status = status_code<win32_status_domain>;
      using error = error_code<win32_status_domain>;

      status getLastError() noexcept;
    }
  }
  using system_status = sys::win32::status;
  using system_error  = sys::win32::error;
}

constexpr const valkyrie::sys::win32_status_domain & valkyrie::traits::Singleton<valkyrie::sys::win32_status_domain>::get() noexcept {
  return valkyrie::sys::detail::win32_status_domain;
}
constexpr const valkyrie::sys::win32_status_domain & valkyrie::sys::win32_status_domain::get() noexcept {
  return valkyrie::sys::detail::win32_status_domain;
}


#endif//VALKYRIE_WIN32_ERROR_HPP
