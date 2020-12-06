//
// Created by Maxwell on 2020-11-16.
//

//#include <valkyrie/Core/Error/Status.hpp>

#include <valkyrie/Core/Config.hpp>

VK_msvc_warning(push)
VK_msvc_warning(disable:5104)
VK_msvc_warning(disable:5105)
#include <valkyrie/Core/Error/Stacktrace.hpp>

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

  std::ostream& operator<<(std::ostream& OS, Core::StringView string){
    return OS.write(string.c_str(), string.byteLength());
  }
  std::ostream& operator<<(std::ostream& OS, const Core::StringRef& string){
    return OS.write(string.c_str(), string.size());
  }
  std::ostream& operator<<(std::ostream& OS, Core::Severity severity){

#define severity_case(Value) case Core::Severity::Value: return OS << #Value;
    switch (severity) {
      VK_foreach(severity_case,
                 Debug,
                 Performance,
                 Success,
                 Info,
                 Warning,
                 Error,
                 InternalError,
                 Fatal)
      default:
        return OS << "Unknown";
    }
  }
  std::ostream& operator<<(std::ostream& OS, const Core::StatusCode<void>& status) {
    return OS << "{ Domain: " << status.domain().name() << ", Severity: " << status.severity() << ", Message: \"" << status.message() << "\" }";
  }
}

void Core::printStackTraceToStdErr() {
  std::cerr << boost::stacktrace::basic_stacktrace(2, 32) << std::endl;
}


VK_noreturn void Core::panic(const utf8 *pMessage) noexcept {
  std::cerr << "\n\nPANIC!\n\tMessage: " << (const char*)pMessage << std::endl;
  std::cerr << "\n\nStacktrace: \n\n" << boost::stacktrace::basic_stacktrace(3, 32) << std::endl;
  abort();
}
VK_noreturn void Core::panic(const StatusCode<void>& statusCode) noexcept {
  std::cerr << "\n\npanic( " << statusCode << " )" << std::endl;
  std::cerr << "\n\nStacktrace: \n\n" << boost::stacktrace::basic_stacktrace(3, 32) << std::endl;
  abort();
}
VK_noreturn void Core::panic(const StatusCode<void> &statusCode, const utf8 *pMessage) noexcept {
  std::cerr << "\n\nPANIC: " << statusCode;
  std::cerr << "\n\n" << (const char*)pMessage;
  std::cerr << "\n\nStacktrace: \n\n" << boost::stacktrace::basic_stacktrace(3, 32) << std::endl;
  abort();
}


void Core::StatusDomain::doThrowException(const StatusCode<void> &Code) const {
  std::stringstream msg;
  auto stacktrace = boost::stacktrace::basic_stacktrace(3, 32);
  msg << Code << "\n\n" << stacktrace << std::endl;
  throw std::runtime_error(msg.str());
}


Core::StringView Core::Detail::GenericDomain::name() const noexcept {
  return VK_string("Generic");
}
Core::StringRef Core::Detail::GenericDomain::doMessage(const StatusCode<void>& status) const noexcept {
  return VK_string("GenericDomain::doMessage not yet implemented...");
  switch (static_cast<const Core::GenericStatus&>(status).value()) {

  }
}
Core::Code Core::Detail::GenericDomain::doCode(const StatusCode<void>& status) const noexcept {
  return static_cast<const Core::GenericStatus&>(status).value();
}
bool Core::Detail::GenericDomain::doEquivalent(const StatusCode<void> & status, const StatusCode<void> & other) const noexcept {
  VK_assert(status.domain() == *this);
  return static_cast<const Core::GenericStatus&>(status).value() == other.generic();
}
bool Core::Detail::GenericDomain::doFailure(const StatusCode<void>& status) const noexcept {
  return static_cast<const Core::GenericStatus&>(status).value() < Code::Success;
}
