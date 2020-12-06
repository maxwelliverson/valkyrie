//
// Created by Maxwell on 2020-11-21.
//

#ifndef VALKYRIE_COM_ERROR_HPP
#define VALKYRIE_COM_ERROR_HPP

/*#include <winerror.h>

#include <comdef.h>

namespace valkyrie::Core{

  namespace System{
    class COMStatusDomain;
  }

  template <>
  struct Custom::Singleton<System::COMStatusDomain>{
    inline constexpr static StringView name = VK_raw_string(COMStatusDomain);
    inline constexpr static StringView scoped_name = VK_raw_string(valkyrie::Core::System::Detail::COMStatusDomain);
    inline constexpr static Uuid       uuid{"061a735a-82ff-4e06-aff3-a2484869ff4f"};

    inline constexpr static const System::COMStatusDomain& get() noexcept;
  };


  namespace System{
    class COMStatusDomain   : public UniqueStatusDomain<COMStatusDomain>{
    public:
      using value_type = Win32::NativeHResult;
      using status_type = StatusCode<NTStatusDomain>;

      StringView name() const noexcept override { return VK_raw_string(COM HRESULT); }
      Core::StringRef doMessage(const StatusCode<void>& status) const noexcept override {

      }
    };
  }
}*/

#endif//VALKYRIE_COM_ERROR_HPP
