//
// Created by Maxwell on 2020-12-25.
//

#ifndef VALKYRIE_UTILITY_JSON_HPP
#define VALKYRIE_UTILITY_JSON_HPP

#include <valkyrie/adt/string.hpp>
#include <valkyrie/status/result.hpp>
#include <valkyrie/utility/bitflag.hpp>
#include <valkyrie/utility/filesystem.hpp>


namespace valkyrie{


  class json{
  public:
    enum class code {
      success = 0,              ///< No error
      insufficient_capacity,    ///< This parser can't support a document that big
      failed_allocation,        ///< Error allocating memory, most likely out of memory
      write_error,              ///< Something went wrong while writing to the tape (stage 2), this is a generic error
      depth_exceeded,           ///< Your document exceeds the user-specified depth limitation
      string_parsing_error,     ///< Problem while parsing a string
      true_parsing_error,       ///< Problem while parsing an atom starting with the letter 't'
      false_parsing_error,      ///< Problem while parsing an atom starting with the letter 'f'
      null_parsing_error,       ///< Problem while parsing an atom starting with the letter 'n'
      number_parsing_error,     ///< Problem while parsing a number
      utf8_encoding_error,      ///< the input is not valid UTF-8
      uninitialized,            ///< unknown error, or uninitialized document
      empty,                    ///< no structural element found
      unescaped_chars,          ///< found unescaped characters in a string.
      unclosed_string,          ///< missing quote at the end
      unsupported_architecture, ///< unsupported architecture
      bad_type,                 ///< JSON element has a different type than user expected
      number_out_of_range,      ///< JSON number does not fit in 64 bits
      index_out_of_bounds,      ///< JSON array index too large
      no_such_field,            ///< JSON field not found in object
      bad_file,                 ///< Error reading a file
      invalid_json_pointer,     ///< Invalid JSON pointer reference
      invalid_uri_fragment,     ///< Invalid URI fragment
      internal_error,           ///< indicative of a bug in simdjson
      parser_is_busy,           ///< parser is already in use.
      out_of_order_iteration,   ///< tried to iterate an array or object out of order
      insufficient_padding      ///< The JSON doesn't have enough padding for simdjson to safely parse it.
    };

  private:

    class json_status_domain final : public status_domain{
    public:

      using value_type = json::code;

      VK_constant uuid class_uuid{"55cece30-fc20-468c-acf0-1edbf22db493"};

      constexpr json_status_domain() : status_domain(class_uuid){}

      VK_nodiscard string_ref     name() const noexcept override;

      VK_nodiscard valkyrie::code do_generic_code(const status_code<void> &status) const noexcept override;
      VK_nodiscard bool           do_failure(const status_code<void> &status) const noexcept override;
      VK_nodiscard severity       do_severity(const status_code<void> &status) const noexcept override;
      VK_nodiscard string_ref     do_message(const status_code<void> &status) const noexcept override;
      VK_nodiscard bool           do_equivalent(const status_code<void> &statusA, const status_code<void> &statusB) const noexcept override;
      void                        do_throw_exception(const status_code<void> &code) const override;


      VK_nodiscard static const json_status_domain& get() noexcept;
    };


    class value{
    public:
    private:

      inline constexpr static u64 JsonValueSize = 16;

      u8 storage[JsonValueSize];
    };


  public:

    using status = status_code<json_status_domain>;
    using error  = error_code<json_status_domain>;
    template <typename T>
    using result = basic_result<T, json_status_domain, void>;




    json(const json&) = delete;
    json(json&& other) noexcept;



    static system_result<json> load(const fs::path& path) noexcept;
    static result<json>        parse(string_view jsonString) noexcept;








  private:

    json(const fs::path& path,   system_status& status) noexcept;
    json(string_view jsonString, status& status) noexcept;


    inline constexpr static u64 JsonDocumentSize = 24;

    u8 storage[JsonDocumentSize];
  };
}

#endif//VALKYRIE_UTILITY_JSON_HPP
