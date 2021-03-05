//
// Created by Maxwell on 2020-11-10.
//

#ifndef VALKYRIE_UTILITY_FUNCTION_REF_HPP
#define VALKYRIE_UTILITY_FUNCTION_REF_HPP


#include <valkyrie/preprocessor.hpp>


#include <tuple>

namespace valkyrie{

  namespace detail{
    template <typename F>
    struct FunctionTypeTraits;
    template <typename Ret, typename ...Args>
    struct FunctionTypeTraits<Ret(Args...)>{
      VK_constant bool is_noexcept = false;
      VK_constant size_t arg_count = sizeof...(Args);

      template <typename F>
      VK_constant bool invocable      = std::invocable<F, Args...> && (std::convertible_to<std::invoke_result_t<F, Args...>, Ret> || (std::is_void_v<Ret> && std::is_void_v<std::invoke_result_t<F, Args...>>));


      using PFN_indirect = Ret(*)(void*, Args&&...);
      using PFN_direct   = Ret(*)(Args...);

      template <typename F>
      VK_constant PFN_indirect indirectLambda = [](void* pUserData, Args&&... args) -> Ret{
        return static_cast<Ret>((*static_cast<F*>(pUserData))(std::forward<Args>(args)...));
      };



      using return_type = Ret;
      template <size_t N>
      using arg_type = std::tuple_element_t<N, std::tuple<Args...>>;


    };
    template <typename Ret, typename ...Args>
    struct FunctionTypeTraits<Ret(Args...) noexcept> : FunctionTypeTraits<Ret(Args...)>{
      VK_constant bool is_noexcept = true;

      using PFN_indirect = Ret(*)(void*, Args&&...) noexcept;
      using PFN_direct   = Ret(*)(Args...) noexcept;

      template <typename F>
      VK_constant PFN_indirect indirectLambda = [](void* pUserData, Args&&... args) noexcept -> Ret{
        return static_cast<Ret>((*static_cast<F*>(pUserData))(std::forward<Args>(args)...));
      };


    };
  }




  template <typename T>
  class function_ref {
    using traits = detail::FunctionTypeTraits<T>;
    using PFN_indirect = typename traits::PFN_indirect;
    using PFN_direct   = typename traits::PFN_direct;




  public:
    using return_type  = typename traits::return_type;


    function_ref() = delete;

    template <not_same_as<function_ref> F> requires(traits::template invocable<F>)
        function_ref(F&& func) noexcept
        : pIndirectFunction(traits::template indirectLambda<std::remove_reference_t<F>>),
          pConstUserData(std::addressof(func)){}
    /*template <not_same_as<function_ref> F> requires(traits::template invocable<const F>)
    function_ref(const F& func) noexcept
        : pIndirectFunction(traits::template indirectLambda<const F>),
          pConstUserData(std::addressof(func)){}*/
    function_ref(PFN_direct directFunc) noexcept : pIndirectFunction(nullptr), pDirectFunction(directFunc){}

    //function_ref(const function_ref& other) = default;
    //function_ref(function_ref&&) noexcept = default;
    ~function_ref() = default;


    template <typename ...Args> requires(std::invocable<T, Args...>)
    return_type operator()(Args&& ...args) const noexcept(traits::is_noexcept){
      if (!pIndirectFunction)
        return pDirectFunction(std::forward<Args>(args)...);
      return pIndirectFunction(pUserData, std::forward<Args>(args)...);
    }


  private:
    const PFN_indirect pIndirectFunction;
    union{
      void*       pUserData;
      const void* pConstUserData;
      PFN_direct  pDirectFunction;
    };
  };




}

#endif//VALKYRIE_UTILITY_FUNCTION_REF_HPP
