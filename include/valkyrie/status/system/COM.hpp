//
// Created by Maxwell on 2020-11-21.
//

#ifndef VALKYRIE_COM_ERROR_HPP
#define VALKYRIE_COM_ERROR_HPP

/*#include <winerror.h>

#include <comdef.h>

namespace valkyrie{

  namespace system{
    class COMStatusDomain;
  }

  template <>
  struct Custom::Singleton<system::COMStatusDomain>{
    inline constexpr static string_view name = VK_raw_string(COMStatusDomain);
    inline constexpr static string_view scoped_name = VK_raw_string(valkyrie::system::detail::COMStatusDomain);
    inline constexpr static uuid       class_id{"061a735a-82ff-4e06-aff3-a2484869ff4f"};

    inline constexpr static const system::COMStatusDomain& get() noexcept;
  };


  namespace system{
    class COMStatusDomain   : public unique_status_domain<COMStatusDomain>{
    public:
      using value_type = win32::NativeHResult;
      using status_type = status_code<NTStatusDomain>;

      string_view name() const noexcept override { return VK_raw_string(COM HRESULT); }
      string_ref doMessage(const status_code<void>& status) const noexcept override {

      }
    };
  }
}*/

#endif//VALKYRIE_COM_ERROR_HPP
