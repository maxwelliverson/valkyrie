//
// Created by Maxwell on 2020-11-10.
//

#ifndef VALKYRIE_ADT_FUNCTION_REF_HPP
#define VALKYRIE_ADT_FUNCTION_REF_HPP


#include <valkyrie/traits.hpp>


#include <tuple>
#include <utility>

namespace valkyrie{

  /*namespace detail{
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
      VK_constant PFN_indirect indirectLambda = [](void* p_user_data, Args&&... args) -> Ret{
        return static_cast<Ret>((*static_cast<F*>(p_user_data))(std::forward<Args>(args)...));
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
      VK_constant PFN_indirect indirectLambda = [](void* p_user_data, Args&&... args) noexcept -> Ret{
        return static_cast<Ret>((*static_cast<F*>(p_user_data))(std::forward<Args>(args)...));
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
        : pfn_indirect_function(traits::template indirectLambda<std::remove_reference_t<F>>),
          p_const_user_data(std::addressof(func)){}
    
    function_ref(PFN_direct directFunc) noexcept : pfn_indirect_function(nullptr), pfn_direct_function(directFunc){}
    
    template <typename F, typename V>
    function_ref(F&& func){}

    //function_ref(const function_ref& other) = default;
    //function_ref(function_ref&&) noexcept = default;
    ~function_ref() = default;


    template <typename ...Args> requires(std::invocable<T, Args...>)
    return_type operator()(Args&& ...args) const noexcept(traits::is_noexcept){
      if (!pfn_indirect_function)
        return pfn_direct_function(std::forward<Args>(args)...);
      return pfn_indirect_function(p_user_data, std::forward<Args>(args)...);
    }


  private:
    const PFN_indirect pfn_indirect_function;
    union{
      void*       p_user_data;
      const void* p_const_user_data;
      PFN_direct  pfn_direct_function;
    };
  };*/

  template <typename T, bool IsNoExcept = false>
  class function_ref;

  template <typename Ret, typename ...Args, bool IsNoexcept>
  class function_ref<Ret(Args...), IsNoexcept> {

  protected:
    using PFN_throwing_indirect = Ret(*)(void*, Args&&...);
    using PFN_throwing_direct   = Ret(*)(Args...);
    using PFN_nothrow_indirect = Ret(*)(void*, Args&&...) noexcept;
    using PFN_nothrow_direct   = Ret(*)(Args...) noexcept;

    VK_constant bool is_noexcept = IsNoexcept;

    using PFN_indirect = std::conditional_t<is_noexcept, PFN_nothrow_indirect, PFN_throwing_indirect>;
    using PFN_direct   = std::conditional_t<is_noexcept, PFN_nothrow_direct, PFN_throwing_direct>;

    template <typename F>
    VK_constant PFN_indirect indirect_fn = [](void* p_user_data, Args&& ... args) noexcept(is_noexcept) -> Ret {
      return (*static_cast<F*>(p_user_data))(std::forward<Args>(args)...);
    };


  public:
    using return_type  = Ret;
    using signature_type = std::remove_pointer_t<PFN_direct>;


    function_ref() = delete;

    template <not_same_as<function_ref> F> requires std::invocable<F, Args...>
    function_ref(F&& func) noexcept 
        : pfn_indirect_function(indirect_fn<remove_ref_t<F>>),
          p_user_data(const_cast<void*>(static_cast<const volatile void*>(std::addressof(func)))){}

    function_ref(PFN_direct directFunc) noexcept
        : pfn_indirect_function(nullptr),
          pfn_direct_function(directFunc){}

    function_ref(PFN_indirect indirectFunc, void* pUserData) noexcept
        : pfn_indirect_function(indirectFunc),
          p_user_data(pUserData){ }

    //function_ref(const function_ref& other) = default;
    //function_ref(function_ref&&) noexcept = default;
    ~function_ref() = default;


    template <typename ...OArgs> requires std::invocable<PFN_direct, OArgs...>
    return_type operator()(OArgs&& ...args) const noexcept(is_noexcept) {
      if (!pfn_indirect_function)
        return pfn_direct_function(static_cast<Args>(std::forward<OArgs>(args))...);
      return pfn_indirect_function(p_user_data, static_cast<Args>(std::forward<OArgs>(args))...);
    }





  private:
    const PFN_indirect pfn_indirect_function;
    union{
      void*       p_user_data;
      PFN_direct  pfn_direct_function;
    };
  };


  template <typename Ret, typename ...Args>
  class function_ref<Ret(Args...) noexcept> : public function_ref<Ret(Args...), true>{
  public:
    using function_ref<Ret(Args...), true>::function_ref;
  };


  template <typename Ret, typename C, typename ...Args, typename T> requires std::derived_from<remove_cv_t<T>, C>
  static function_ref<Ret(Args...)>          bind_to_ref(T& object, Ret(C::* mem_ptr)(Args...)) noexcept {
    static_assert(sizeof(mem_ptr) == sizeof(void*),
                  "creating function_ref objects out of pointers to member functions "
                  "of types using virtual inheritance is not allowed.");
    using PFN_indirect = Ret(*)(void*, Args...);
    PFN_indirect pfn_indirect =
        reinterpret_cast<PFN_indirect>(
        reinterpret_cast<u64>(mem_ptr));
    return { pfn_indirect, (void*)static_cast<const volatile C*>(std::addressof(object)) };
  }
  template <typename Ret, typename C, typename ...Args, typename T> requires std::derived_from<remove_cv_t<T>, C>
  static function_ref<Ret(Args...)>          bind_to_ref(const T& object, Ret(C::* mem_ptr)(Args...) const) noexcept {
    static_assert(sizeof(mem_ptr) == sizeof(void*),
                  "creating function_ref objects out of pointers to member functions "
                  "of types using virtual inheritance is not allowed.");
    using PFN_indirect = Ret(*)(void*, Args...);
    PFN_indirect pfn_indirect =
        reinterpret_cast<PFN_indirect>(
            reinterpret_cast<u64>(mem_ptr));
    return { pfn_indirect, (void*)static_cast<const C*>(std::addressof(object)) };
  }
  template <typename Ret, typename C, typename ...Args, typename T> requires std::derived_from<remove_cv_t<T>, C>
  static function_ref<Ret(Args...) noexcept> bind_to_ref(T& object, Ret(C::* mem_ptr)(Args...) noexcept) noexcept {
    static_assert(sizeof(mem_ptr) == sizeof(void*),
                  "creating function_ref objects out of pointers to member functions "
                  "of types using virtual inheritance is not allowed.");
    using PFN_indirect = Ret(*)(void*, Args...) noexcept;
    PFN_indirect pfn_indirect =
        reinterpret_cast<PFN_indirect>(
            reinterpret_cast<u64>(mem_ptr));
    return { pfn_indirect, (void*)static_cast<const volatile C*>(std::addressof(object)) };
  }
  template <typename Ret, typename C, typename ...Args, typename T> requires std::derived_from<remove_cv_t<T>, C>
  static function_ref<Ret(Args...) noexcept> bind_to_ref(const T& object, Ret(C::* mem_ptr)(Args...) const noexcept) noexcept {
    static_assert(sizeof(mem_ptr) == sizeof(void*),
                  "creating function_ref objects out of pointers to member functions "
                  "of types using virtual inheritance is not allowed.");
    using PFN_indirect = Ret(*)(void*, Args...) noexcept;
    PFN_indirect pfn_indirect =
        reinterpret_cast<PFN_indirect>(
            reinterpret_cast<u64>(mem_ptr));
    return { pfn_indirect, (void*)static_cast<const C*>(std::addressof(object)) };
  }
}

#endif//VALKYRIE_ADT_FUNCTION_REF_HPP
