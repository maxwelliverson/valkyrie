//
// Created by Maxwell on 2021-02-11.
//

#ifndef VALKYRIE_CORE_ERROR_ASSERT_HPP
#define VALKYRIE_CORE_ERROR_ASSERT_HPP

#include <valkyrie/Core/Error/GenericCode.hpp>


#endif //VALKYRIE_CORE_ERROR_ASSERT_HPP

// Past include guards...



#if !defined(INVARIANT)
#if VK_debug_build

namespace valkyrie::Core::Detail{
  VK_noreturn void badInvariant(utf8_string pMsg, utf8_string pFunction, utf8_string pFilename, int lineNumber) noexcept;
}

#define INVARIANT(...) do{ \
  if (!(__VA_ARGS__)) ::valkyrie::Core::Detail::badInvariant(VK_raw_string(__VA_ARGS__), (utf8_string)VK_function_name, (utf8_string)VK_filename, VK_line_number); }while(false)
#else
#define INVARIANT(...) ((void)0)
#endif
#endif