//
// Created by maxwe on 2021-05-15.
//

#ifndef VALKYRIE_SMALL_STRING_HPP
#define VALKYRIE_SMALL_STRING_HPP

#include "small_vector.hpp"

namespace valkyrie{

  template <u64 N, allocator_c Alloc = default_allocator>
  class small_string : public small_array<utf8, N, Alloc>{

    using base = small_array<utf8, N, Alloc>;
  public:
    using base::base;


    /// Default ctor - Initialize to empty.
    small_string() = default;

    /// Initialize from a string_view.
    small_string(string_view S) : small_array<utf8, N, Alloc>(S.begin(), S.end()) {}

    /// Initialize by concatenating a list of string_views.
    small_string(std::initializer_list<string_view> Refs) : small_array<utf8, N, Alloc>() {
      this->append(Refs);
    }

    /// Initialize with a range.
    template<typename ItTy>
    small_string(ItTy S, ItTy E) : small_array<utf8, N, Alloc>(S, E) {}

    /// @}
    /// @name String Assignment
    /// @{

    using small_array<utf8, N, Alloc>::assign;

    /// Assign from a string_view.
    void assign(string_view RHS) {
      any_small_array<utf8, Alloc>::assign(RHS.begin(), RHS.end());
    }

    /// Assign from a list of string_views.
    void assign(std::initializer_list<string_view> Refs) {
      this->clear();
      append(Refs);
    }

    /// @}
    /// @name String Concatenation
    /// @{

    using small_array<utf8, N, Alloc>::append;

    /// Append from a string_view.
    void append(string_view RHS) {
      any_small_array<utf8, Alloc>::append(RHS.begin(), RHS.end());
    }

    /// Append from a list of string_views.
    void append(std::initializer_list<string_view> Refs) {
      u64 SizeNeeded = this->size();
      for (const string_view &Ref : Refs)
        SizeNeeded += Ref.size();
      this->reserve(SizeNeeded);
      auto CurEnd = this->end();
      for (const string_view &Ref : Refs) {
        this->uninitialized_copy(Ref.begin(), Ref.end(), CurEnd);
        CurEnd += Ref.size();
      }
      this->set_size(SizeNeeded);
    }

    /// @}
    /// @name String Comparison
    /// @{

    /// Check for string equality.  This is more efficient than compare() when
    /// the relative ordering of inequal strings isn't needed.
    VK_nodiscard bool equals(string_view RHS) const {
      return str().equals(RHS);
    }

    /// Check for string equality, ignoring case.
    VK_nodiscard bool equals_lower(string_view RHS) const {
      return str().equals_lower(RHS);
    }

    /// Compare two strings; the result is -1, 0, or 1 if this string is
    /// lexicographically less than, equal to, or greater than the \p RHS.
    VK_nodiscard int compare(string_view RHS) const {
      return str().compare(RHS);
    }

    /*/// compare_lower - Compare two strings, ignoring case.
    VK_nodiscard int compare_lower(string_view RHS) const {
      return str().compare_lower(RHS);
    }

    /// compare_numeric - Compare two strings, treating sequences of digits as
    /// numbers.
    VK_nodiscard int compare_numeric(string_view RHS) const {
      return str().compare_numeric(RHS);
    }*/

    /// @}
    /// @name String Predicates
    /// @{

    /// startswith - Check if this string starts with the given \p Prefix.
    VK_nodiscard bool starts_with(string_view Prefix) const {
      return str().starts_with(Prefix);
    }

    /// endswith - Check if this string ends with the given \p Suffix.
    VK_nodiscard bool ends_with(string_view Suffix) const {
      return str().ends_with(Suffix);
    }

    /// @}
    /// @name String Searching
    /// @{

    /// find - Search for the first character \p C in the string.
    ///
    /// \return - The index of the first occurrence of \p C, or npos if not
    /// found.
    VK_nodiscard u64 find(char C, u64 From = 0) const {
      return str().find(C, From);
    }

    /// Search for the first string \p Str in the string.
    ///
    /// \returns The index of the first occurrence of \p Str, or npos if not
    /// found.
    VK_nodiscard u64 find(string_view Str, u64 From = 0) const {
      return str().find(Str, From);
    }

    /// Search for the last character \p C in the string.
    ///
    /// \returns The index of the last occurrence of \p C, or npos if not
    /// found.
    VK_nodiscard u64 rfind(char C, u64 From = string_view::npos) const {
      return str().rfind(C, From);
    }

    /// Search for the last string \p Str in the string.
    ///
    /// \returns The index of the last occurrence of \p Str, or npos if not
    /// found.
    VK_nodiscard u64 rfind(string_view Str) const {
      return str().rfind(Str);
    }

    /// Find the first character in the string that is \p C, or npos if not
    /// found. Same as find.
    VK_nodiscard u64 find_first_of(char C, u64 From = 0) const {
      return str().find_first_of(C, From);
    }

    /// Find the first character in the string that is in \p Chars, or npos if
    /// not found.
    ///
    /// Complexity: O(size() + Chars.size())
    VK_nodiscard u64 find_first_of(string_view Chars, u64 From = 0) const {
      return str().find_first_of(Chars, From);
    }

    /// Find the first character in the string that is not \p C or npos if not
    /// found.
    VK_nodiscard u64 find_first_not_of(char C, u64 From = 0) const {
      return str().find_first_not_of(C, From);
    }

    /// Find the first character in the string that is not in the string
    /// \p Chars, or npos if not found.
    ///
    /// Complexity: O(size() + Chars.size())
    VK_nodiscard u64 find_first_not_of(string_view Chars, u64 From = 0) const {
      return str().find_first_not_of(Chars, From);
    }

    /// Find the last character in the string that is \p C, or npos if not
    /// found.
    VK_nodiscard u64 find_last_of(char C, u64 From = string_view::npos) const {
      return str().find_last_of(C, From);
    }

    /// Find the last character in the string that is in \p C, or npos if not
    /// found.
    ///
    /// Complexity: O(size() + Chars.size())
    VK_nodiscard u64 find_last_of(string_view Chars, u64 From = string_view::npos) const {
      return str().find_last_of(Chars, From);
    }

    /// @}
    /// @name Helpful Algorithms
    /// @{

    /// Return the number of occurrences of \p C in the string.
    VK_nodiscard u64 count(char C) const {
      return str().count(C);
    }

    /// Return the number of non-overlapped occurrences of \p Str in the
    /// string.
    VK_nodiscard u64 count(string_view Str) const {
      return str().count(Str);
    }

    /// @}
    /// @name Substring Operations
    /// @{

    /// Return a reference to the substring from [Start, Start + N).
    ///
    /// \param Start The index of the starting character in the substring; if
    /// the index is npos or greater than the length of the string then the
    /// empty substring will be returned.
    ///
    /// \param N The number of characters to included in the substring. If \p N
    /// exceeds the number of characters remaining in the string, the string
    /// suffix (starting with \p Start) will be returned.
    VK_nodiscard string_view substr(u64 Start, u64 N = string_view::npos) const {
      return str().substr(Start, N);
    }

    /// Return a reference to the substring from [Start, End).
    ///
    /// \param Start The index of the starting character in the substring; if
    /// the index is npos or greater than the length of the string then the
    /// empty substring will be returned.
    ///
    /// \param End The index following the last character to include in the
    /// substring. If this is npos, or less than \p Start, or exceeds the
    /// number of characters remaining in the string, the string suffix
    /// (starting with \p Start) will be returned.
    VK_nodiscard string_view slice(u64 Start, u64 End) const {
      return str().slice(Start, End);
    }

    // Extra methods.

    /// Explicit conversion to string_view.
    VK_nodiscard string_view str() const { return string_view(this->data(), this->size()); }

    // TODO: Make this const, if it's safe...
    const char* c_str() {
      this->push_back(0);
      this->pop_back();
      return this->data();
    }

    /// Implicit conversion to string_view.
    operator string_view() const { return str(); }

    explicit operator std::string() const {
      return std::string(this->data(), this->size());
    }

    // Extra operators.
    small_string &operator=(string_view RHS) {
      this->assign(RHS);
      return *this;
    }

    small_string &operator+=(string_view RHS) {
      this->append(RHS.begin(), RHS.end());
      return *this;
    }
    small_string &operator+=(char C) {
      this->push_back(C);
      return *this;
    }
    
  };
  template <allocator_c Alloc = default_allocator>
  using any_small_string = any_small_array<utf8, Alloc>;
}

#endif  //VALKYRIE_SMALL_STRING_HPP
