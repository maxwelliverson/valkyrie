//
// Created by Maxwell on 2020-11-21.
//

#ifndef VALKYRIE_WINNT_ERROR_HPP
#define VALKYRIE_WINNT_ERROR_HPP

/*namespace valkyrie::Core{

  namespace System{
    class NTStatusDomain;
  }

  template <>
  struct Custom::Singleton<System::NTStatusDomain>{
    inline constexpr static StringView name = VK_raw_string(NTStatusDomain);
    inline constexpr static StringView scoped_name = VK_raw_string(valkyrie::Core::System::Detail::NTStatusDomain);
    inline constexpr static Uuid       uuid{"f96180e1-6f37-4ea1-be46-d8f086f335c1"};

    inline constexpr static const System::NTStatusDomain& get() noexcept;
  };

  namespace System{



    class NTStatusDomain    : public UniqueStatusDomain<NTStatusDomain>{
    public:
      using value_type = Win32::NativeNTStatus;
      using status_type = StatusCode<NTStatusDomain>;

      StringView name() const noexcept override { return VK_raw_string(NTSTATUS); }
    };
  }
}*/

#endif//VALKYRIE_WINNT_ERROR_HPP
