//
// Created by Maxwell on 2020-11-16.
//

#include <valkyrie/status/generic_code.hpp>
#include <valkyrie/adt/string.hpp>
#include <valkyrie/status/assert.hpp>

VK_msvc_warning(push)
VK_msvc_warning(disable:5104)
VK_msvc_warning(disable:5105)
#include <valkyrie/status/stacktrace.hpp>

#include <boost/stacktrace.hpp>
#include <iostream>
#include <sstream>
VK_msvc_warning(pop)




using namespace valkyrie;

namespace {
  /*struct ScopeDescriptor{

  };
  struct TypeDescriptor{
    std::string name;

  };
  struct ParamInfo{
    std::string
  };*/
  /*struct FunctionInfo{
    std::string name;
  };
  struct FrameInfo{
    void* pFrame;
    FunctionInfo function;
    FrameInfo(void* pFrame) : pFrame(pFrame){

    }
  };*/

  /*std::ostream& operator<<(std::ostream& OS, const FrameInfo& frameInfo) {

  }*/

  std::ostream& operator<<(std::ostream& OS, string_view string){
    return OS.write(string.c_str(), string.size());
  }
  std::ostream& operator<<(std::ostream& OS, const string_ref& string){
    return OS.write(string.c_str(), string.size());
  }
  std::ostream& operator<<(std::ostream& OS, severity severity){

#define severity_case(Value) case severity::Value: return OS << #Value;
    switch (severity) {
      VK_foreach(severity_case,
                 debug,
                 performance,
                 success,
                 info,
                 warning,
                 error,
                 internal_error,
                 fatal)
      default:
        return OS << "Unknown";
    }
  }
  std::ostream& operator<<(std::ostream& OS, const status_code<void>& status) {
    return OS << "{ Domain: " << status.domain().name() << ", severity: " << status.severity() << ", Message: \"" << status.message() << "\" }";
  }
}

void valkyrie::printStackTraceToStdErr() {
  std::cerr << boost::stacktrace::basic_stacktrace(2, 32) << std::endl;
}

#if VK_debug_build
VK_noreturn void valkyrie::detail::_bad_invariant(utf8_string pMessage, utf8_string pFunction, utf8_string pFilename, int lineNum) VK_throws {

  std::stringstream outMsg;


  //std::cerr << "\n\n\n!!! BAD INVARIANT !!!";
  outMsg << "\n\tInvariant: \"" << (cstring)pMessage;
  outMsg << "\"\n\tCalling Function: " << (cstring)pFunction;
  outMsg << "\n\tSource Location: " << (cstring)pFilename << ":" << lineNum << "\n";
  panic(generic_error(code::BadInvariant), (utf8_string)outMsg.str().c_str());
  //std::cerr << "Stacktrace: \n\n" << boost::stacktrace::basic_stacktrace(4, 100) << std::endl;
  //abort();
}
#endif

VK_noreturn void valkyrie::panic(const utf8 *pMessage) noexcept {
  std::cerr << "\n\nPANIC!\n\tMessage: " << (const char*)pMessage << std::endl;
  std::cerr << "\n\nStacktrace: \n\n" << boost::stacktrace::basic_stacktrace(3, 32) << std::endl;
  abort();
}
VK_noreturn void valkyrie::panic(const status_code<void>& statusCode) noexcept {
  std::cerr << "\n\npanic( " << statusCode << " )" << std::endl;
  std::cerr << "\n\nStacktrace: \n\n" << boost::stacktrace::basic_stacktrace(3, 32) << std::endl;
  abort();
}
VK_noreturn void valkyrie::panic(const status_code<void> &statusCode, const utf8 *pMessage) noexcept {
  std::cerr << "\n\nPANIC: " << statusCode;
  std::cerr << "\n\n" << (const char*)pMessage;
  std::cerr << "\n\nStacktrace: \n\n" << boost::stacktrace::basic_stacktrace(3, 32) << std::endl;
  abort();
}


void status_domain::do_throw_exception(const status_code<void> &code) const {
  std::stringstream msg;
  auto stacktrace = boost::stacktrace::basic_stacktrace(3, 32);
  msg << code << "\n\n" << stacktrace << std::endl;
  VK_if(VK_exceptions_enabled(throw std::runtime_error(msg.str()))VK_else(panic(code, (utf8_string)msg.str().c_str())));
}


string_ref generic_domain::name() const noexcept {
  return VK_string("Generic");
}
string_ref generic_domain::do_message(const status_code<void>& status) const noexcept {
  return VK_string("generic_domain::do_message not yet implemented...");
  switch (static_cast<const generic_status&>(status).value()) {
  default:
    return "UNIMPLEMENTED GENERIC MESSAGES";
  }
}
code generic_domain::do_generic_code(const status_code<void>& status) const noexcept {
  return static_cast<const generic_status&>(status).value();
}
bool generic_domain::do_equivalent(const status_code<void> & status, const status_code<void> & other) const noexcept {
  VK_assert(status.domain() == *this);
  return static_cast<const generic_status&>(status).value() == other.generic();
}
bool generic_domain::do_failure(const status_code<void>& status) const noexcept {
  return static_cast<const generic_status&>(status).value() < code::Success;
}

VK_noreturn void generic_domain::do_throw_exception(const status_code<void> &code) const {
  auto&& msg = this->do_message(code);
  VK_if(VK_exceptions_enabled(throw std::runtime_error(std::string(msg.c_str(), msg.size())))VK_else(panic(code)));
}
