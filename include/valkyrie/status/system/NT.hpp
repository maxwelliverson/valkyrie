//
// Created by Maxwell on 2020-11-21.
//

#ifndef VALKYRIE_WINNT_ERROR_HPP
#define VALKYRIE_WINNT_ERROR_HPP

/*namespace valkyrie{

  namespace system{
    class NTStatusDomain;
  }

  template <>
  struct Custom::Singleton<system::NTStatusDomain>{
    inline constexpr static string_view name = VK_raw_string(NTStatusDomain);
    inline constexpr static string_view scoped_name = VK_raw_string(valkyrie::system::detail::NTStatusDomain);
    inline constexpr static uuid       class_id{"f96180e1-6f37-4ea1-be46-d8f086f335c1"};

    inline constexpr static const system::NTStatusDomain& get() noexcept;
  };

  namespace system{



    class NTStatusDomain    : public unique_status_domain<NTStatusDomain>{
    public:
      using value_type = win32::NativeNTStatus;
      using status_type = status_code<NTStatusDomain>;

      string_view name() const noexcept override { return VK_raw_string(NTSTATUS); }
    };
  }
}*/

#endif//VALKYRIE_WINNT_ERROR_HPP
