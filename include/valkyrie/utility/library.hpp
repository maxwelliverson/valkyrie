//
// Created by maxwe on 2021-03-04.
//

#ifndef VALKYRIE_UTILITY_LIBRARY_HPP
#define VALKYRIE_UTILITY_LIBRARY_HPP

#include <valkyrie/adt/string.hpp>
#include <valkyrie/adt/string_view.hpp>
#include <valkyrie/status/result.hpp>

namespace valkyrie{
  class library{
    void*  handle;
    string name;

    explicit library(string_view libName, status& opResult);

    struct polymorphic_function{
      void(*pFunction)();

      template <typename Ret, typename ...Args>
      explicit operator function_ptr<Ret, Args...>() const noexcept {
        return static_cast<function_ptr<Ret, Args...>>(pFunction);
      }
      template <typename Ret, typename ...Args>
      explicit operator noreturn_function_ptr<Ret, Args...>() const noexcept {
        return static_cast<noreturn_function_ptr<Ret, Args...>>(pFunction);
      }
    };

  public:

    static system_result<library> open(string_view libName) noexcept;


    polymorphic_function load(string_view functionName) noexcept;

  };
}

#endif //VALKYRIE_UTILITY_LIBRARY_HPP
