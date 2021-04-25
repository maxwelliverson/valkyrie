//
// Created by Maxwell on 2021-02-11.
//

#ifndef VALKYRIE_ERROR_ASSERT_HPP
#define VALKYRIE_ERROR_ASSERT_HPP

#include <valkyrie/preprocessor.hpp>
#include <valkyrie/primitives.hpp>

#if VK_debug_build

namespace valkyrie::detail{
  VK_noreturn void _bad_invariant(utf8_string pMsg, utf8_string pFunction, utf8_string pFilename, int lineNumber) VK_throws;
}

#define VK_invariant_msg(expr, msg) do { \
    const bool _invariant_RESULT = expr; \
    VK_assume(_invariant_RESULT);                \
    (void)(_invariant_RESULT || (::valkyrie::detail::_bad_invariant(VK_string(msg), (utf8_string)VK_function_name, (utf8_string)VK_filename, VK_line_number), true)); \
  }while(false)
#else
#define VK_invariant_msg(expr, msg) ((void)(VK_assume(expr), 0))
#endif

#define VK_invariant(expr) VK_invariant_msg(expr, #expr)


#endif //VALKYRIE_ERROR_ASSERT_HPP

