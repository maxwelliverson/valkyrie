//
// Created by Maxwell on 2020-12-25.
//

#include <valkyrie/utility/json.hpp>


#if VK_exceptions_enabled
#include <exception>
#endif


using namespace valkyrie;


inline json::status make_status_code(json::code code) noexcept {
  return json::status(std::in_place, code);
}


#if VK_exceptions_enabled
namespace {
  class json_exception final : public std::exception {

  public:
    /**
   * Create an exception from a simdjson error code.
   * @param error The error code
   */
    json_exception(json::code error) noexcept : _error{error} {}
    /** The error message */
    const char *what() const noexcept override {
      if (!_msg)
        _msg.emplace(make_status_code(_error).message());
      return _msg->c_str();
    }
    /** The error code */
    json::code error() const noexcept { return _error; }

  private:
    /** The error code that was used */
    mutable std::optional<string_ref> _msg;
    json::code _error;
  };
}
#endif


const json::json_status_domain& json::json_status_domain::get() noexcept {
  static constexpr json_status_domain instance{};
  return instance;
}
string_ref json::json_status_domain::name() const noexcept {
  return VK_string("JSON");
}
string_ref json::json_status_domain::do_message(const status_code<void> &status) const noexcept {
  static constexpr string_view messages_table[]{
      "Success",
      "This parser can't support a document that big",
      "Error allocating memory, we're most likely out of memory",
      "The JSON document has an improper structure: missing or superfluous commas, braces, missing keys, etc.",
      "The JSON document was too deep (too many nested objects and arrays)",
      "Problem while parsing a string",
      "Problem while parsing an atom starting with the letter 't'",
      "Problem while parsing an atom starting with the letter 'f'",
      "Problem while parsing an atom starting with the letter 'n'",
      "Problem while parsing a number",
      "The input is not valid UTF-8",
      "Uninitialized",
      "Empty: no JSON found",
      "Within strings, some characters must be escaped, we found unescaped characters",
      "Found a string that is missing a closing quotation mark.",
      "simdjson does not have an implementation supported by this CPU architecture (perhaps it's a non-SIMD CPU?).",
      "The JSON element does not have the requested type.",
      "The JSON number is too large or too small to fit within the requested type.",
      "Attempted to access an element of a JSON array that is beyond its length.",
      "The JSON field referenced does not exist in this object.",
      "Error reading the file.",
      "Invalid JSON pointer syntax.",
      "Invalid URI fragment syntax.",
      "Unexpected error, consider reporting this problem as you may have found a bug in simdjson",
      "Cannot parse a new document while a document is still in use.",
      "Objects and arrays can only be iterated when they are first encountered.",
      "simdjson requires the input JSON string to have at least SIMDJSON_PADDING extra bytes allocated, beyond the string's length."
  };
  return messages_table[static_cast<std::underlying_type_t<json::code>>(static_cast<const json::status&>(status).value())];
}
severity json::json_status_domain::do_severity(const status_code<void> &status) const noexcept {
  switch (static_cast<const json::status&>(status).value()) {
    case json::code::success:
      return severity::success;

    case json::code::failed_allocation:
    case json::code::internal_error:
    case json::code::unsupported_architecture:
      return severity::fatal;

    default:
      return severity::error;
  }
}
valkyrie::code json::json_status_domain::do_generic_code(const status_code<void> &status) const noexcept {
  using vc = valkyrie::code;
  constexpr static vc code_table[]{
      vc::Success,
      vc::ResourceTooLarge,
      vc::OutOfMemory,
      vc::InvalidatedState,
      vc::ResourceLimitReached,
      vc::FailedParse,
      vc::FailedParse,
      vc::FailedParse,
      vc::FailedParse,
      vc::FailedParse,
      vc::InvalidFormat,
      vc::ObjectNotInitialized,
      vc::ResourceIsEmpty,
      vc::UnescapedCharacters,
      vc::FailedParse,
      vc::NotSupported,
      vc::ResourceUnexpectedType,
      vc::OutOfRange,
      vc::OutOfBoundsAccess,
      vc::ResourceNotFound,
      vc::IOError,
      vc::InvalidFormat,
      vc::InvalidFormat,
      vc::Unknown,
      vc::Busy,
      vc::BadOrdering,
      vc::InsufficientSize
  };

  return code_table[static_cast<std::underlying_type_t<json::code>>(static_cast<const json::status&>(status).value())];

}
bool json::json_status_domain::do_failure(const status_code<void> &status) const noexcept {
  return static_cast<const json::status&>(status).value() != json::code::success;
}
void json::json_status_domain::do_throw_exception(const status_code<void> &code) const {
#if VK_exceptions_enabled
  throw json_exception(static_cast<const json::status&>(code).value());
#else
  panic(code);
#endif
}
bool json::json_status_domain::do_equivalent(const status_code<void> &statusA, const status_code<void> &statusB) const noexcept {
  VK_assert(statusA.domain() == *this);
  return (statusB.domain() == *this) && static_cast<const json::status&>(statusA).value() == static_cast<const json::status&>(statusB).value();
}







/// SimdJSON


#include "simdjson.hpp"


json::json(const fs::path &path,   system_status &status) noexcept { }
json::json(string_view jsonString, status &status) noexcept {
  simdjson::dom::parser parser;
  auto* doc = new (storage) simdjson::dom::document();
  auto result = parser.parse_into_document(*doc, jsonString.c_str(), jsonString.length());
  status = json::status(static_cast<json::code>(static_cast<std::underlying_type_t<simdjson::error_code>>(result.error())));
}
json::json(json&& other) noexcept { }

system_result<json> json::load(const fs::path &path) noexcept {
  system_status loadResult;
  json document(path, loadResult);
  if ( loadResult.failure() )
    return std::move(loadResult);
  return std::move(document);
}
json::result<json>  json::parse(string_view jsonString) noexcept {
  status parseResult;
  json document(jsonString, parseResult);
  if ( parseResult.failure() )
    return std::move(parseResult);
  return std::move(document);
}






