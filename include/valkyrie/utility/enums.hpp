//
// Created by Maxwell on 2020-11-17.
//

#ifndef VALKYRIE_UTILITIES_ENUMS_HPP
#define VALKYRIE_UTILITIES_ENUMS_HPP


#include "bitflag.hpp"
#include "valkyrie/adt/string_view.hpp"
#include <valkyrie/ADT/flat_map.hpp>
#include <valkyrie/status/generic_code.hpp>


#include <span>

#define PP_VK_impl_DEFINE_ENUM_ENTRY_redirect(Enum) ::valkyrie::detail::EnumEntry{enum_type::Enum, VK_raw_string(Enum)}
#define PP_VK_impl_DEFINE_ENUM_ENTRY PP_VK_impl_DEFINE_ENUM_ENTRY_redirect

#define VK_enum_values(Type, ...)                                                          \
  using enum_type = Type;                                                                  \
  using underlying_type = std::underlying_type_t<enum_type>;                               \
  inline constexpr static std::string_view scoped_name{::valkyrie::detail::typeName<enum_type>()};                   \
  inline constexpr static std::string_view name{::valkyrie::detail::removeScopes(scoped_name)};                          \
  inline constexpr static bool isScoped = !std::convertible_to<enum_type, underlying_type>;\
  inline constexpr static ::valkyrie::detail::EnumEntryMap entries{ VK_for_each_delimit(PP_VK_impl_DEFINE_ENUM_ENTRY, VK_comma_delimiter, ##__VA_ARGS__) }


namespace valkyrie{

  namespace detail{
    template <typename T>
    struct valk_type_WRAPPER_t;
    inline constexpr static std::string_view valk_type_WRAPPER_name{"valk_type_WRAPPER_t<"};
    inline constexpr static std::string_view scope_operator_string{"::"};

    constexpr std::string_view removeScopes(std::string_view identifier) noexcept {
      size_t pos = identifier.find(scope_operator_string);
      while (pos != std::string_view::npos) {
        identifier.remove_prefix(pos + scope_operator_string.size());
        pos = identifier.find(scope_operator_string);
      }
      return identifier;
    }

    template <typename T>
    constexpr std::string_view typeName_redirect() noexcept {
      return VK_function_name;
    }

    template <typename T>
    constexpr std::string_view typeName() noexcept {
      std::string_view funcName = typeName_redirect<valk_type_WRAPPER_t<T>>();
      funcName.remove_prefix(funcName.find(valk_type_WRAPPER_name) + valk_type_WRAPPER_name.size());
      funcName.remove_suffix(funcName.size() - funcName.rfind(VK_if(VK_not(VK_compiler_clang)(">>")VK_else(">]"))));
      VK_if(VK_not(VK_compiler_clang)(funcName.remove_prefix(5)));
      return funcName;
    }
    template <bool IsScoped>
    constexpr std::u8string_view enumName(std::u8string_view typeName, std::u8string_view enumName) noexcept {
      if constexpr(IsScoped) {
        auto pos = enumName.rfind(typeName);
        if (pos != std::u8string_view::npos)
          enumName.remove_prefix(pos + typeName.size() + 2);
      }
      return enumName;
    }


    struct StatusEnumInfo{
      severity severity;
      string_view message;
      std::span<const code> generic{};
    };

    template <typename E>
    struct StatusEnumInitializer{
      E value;
      severity severity;
      string_view message;
      std::span<const code> generic;
    };




    template <typename E>
    struct EnumEntry{

      string_literal value;
      E             key;


      constexpr EnumEntry() noexcept = default;
      template <size_t N>
      constexpr EnumEntry(E val, const utf8(&name)[N]) noexcept
          : key(val), value{name}{}
      constexpr EnumEntry(E val, std::u8string_view name) noexcept
          : key(val), value{name}{}

      constexpr friend bool operator==(EnumEntry A, EnumEntry B) noexcept {
        return A.key == B.key;
      }
      constexpr friend bool operator==(EnumEntry A, E Val) noexcept {
        return A.key == Val;
      }
      constexpr friend std::strong_ordering operator<=>(EnumEntry A, EnumEntry B) noexcept {
        return A.key <=> B.key;
      }
      constexpr friend std::strong_ordering operator<=>(EnumEntry A, E Val) noexcept {
        return A.key <=> Val;
      }
    };


    template <typename E>
    struct StatusEnumEntry{
      E key;
      StatusEnumInfo value;

      constexpr StatusEnumEntry() noexcept = default;

      constexpr StatusEnumEntry(StatusEnumInitializer<E> init) noexcept
          : key(init.value), value{ .severity = init.severity, .message = init.message, .generic = init.generic }{}

      constexpr explicit StatusEnumEntry(E value, StatusEnumInfo&& info) noexcept
          : key(value), value{info}{}

      template <size_t N>
      constexpr StatusEnumEntry(severity severity, E value, const utf8(&message)[N]) noexcept
          : key(value), value{ .severity = severity, .message = message, .generic = {} }{}
      template <size_t N>
      constexpr StatusEnumEntry(severity severity, E value, const utf8(&message)[N], std::initializer_list<code> initList) noexcept
          : key(value), value{ severity, message, std::span<const code>{initList.begin(), initList.size()} }{}

      template <size_t N>
      constexpr StatusEnumEntry(E value, const utf8(&message)[N]) noexcept
          : key(value), value{ .severity = severity::error, .message = message, .generic = {} }{}
      template <size_t N>
      constexpr StatusEnumEntry(E value, const utf8(&message)[N], std::initializer_list<code> initList) noexcept
          : key(value), value{ .severity = (initList.size() ? get_default_severity(*initList.begin()) : severity::error), .message = message, .generic = std::span<const code>{initList.begin(), initList.end()} }{}

      constexpr friend bool operator==(StatusEnumEntry A, StatusEnumEntry B) noexcept {
        return A.key == B.key;
      }
      constexpr friend bool operator==(StatusEnumEntry A, E Val) noexcept {
        return A.key == Val;
      }
      constexpr friend std::strong_ordering operator<=>(StatusEnumEntry A, StatusEnumEntry B) noexcept {
        return A.key <=> B.key;
      }
      constexpr friend std::strong_ordering operator<=>(StatusEnumEntry A, E Val) noexcept {
        return A.key <=> Val;
      }
    };





    template <typename E, size_t N>
    EnumEntry(E, const utf8(&)[N]) -> EnumEntry<E>;

    template <typename E, size_t N>
    class EnumEntryMapContainer{
    public:
      using value_type = EnumEntry<E>;
      using size_type = u32;
      using difference_type = i32;
      using pointer = value_type*;
      using const_pointer = const value_type*;
      using reference = value_type&;
      using const_reference = const value_type&;

      using iterator = pointer;
      using const_iterator = const_pointer;
      using reverse_iterator = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;

      using allocator_type = void;

      /*constexpr EnumEntryMapContainer(const std::array<EnumEntry<E>, N>& array) noexcept
          : EnumEntryMapContainer{array.data(), std::make_index_sequence<N>{}}{}
      constexpr EnumEntryMapContainer(std::initializer_list<value_type> list) noexcept
          : EnumEntryMapContainer{list.begin(), std::make_index_sequence<N>{}}{}*/

      template <typename ...Args>
      constexpr EnumEntryMapContainer(Args&& ...args) noexcept
          : entries{ std::forward<Args>(args)... }{}

      constexpr size_type size() const noexcept { return N; }
      constexpr bool      empty() const noexcept { return !N; }

      constexpr reference operator[](size_type pos) noexcept { return entries[pos]; }
      constexpr const_reference operator[](size_type pos) const noexcept { return entries[pos]; }

      constexpr reference front() noexcept { return entries[0]; }
      constexpr const_reference front() const noexcept { return entries[0]; }
      constexpr reference back() noexcept { return entries[N - 1]; }
      constexpr const_reference back() const noexcept { return entries[N - 1]; }

      constexpr pointer data() noexcept { return entries; }
      constexpr const_pointer data() const noexcept { return entries; }

      constexpr iterator begin() noexcept { return entries; }
      constexpr const_iterator begin() const noexcept { return entries; }
      constexpr const_iterator cbegin() const noexcept { return entries; }

      constexpr iterator end() noexcept { return entries + N; }
      constexpr const_iterator end() const noexcept { return entries + N; }
      constexpr const_iterator cend() const noexcept { return entries + N; }

      constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
      constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
      constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

      constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
      constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
      constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

      constexpr iterator erase(const_iterator) noexcept { VK_unreachable; }
      constexpr iterator erase(const_iterator, const_iterator) noexcept { VK_unreachable; }


    private:
      template <size_t ...I>
      EnumEntryMapContainer(const value_type* arr, std::index_sequence<I...>) noexcept
          : entries{ arr[I]... }{}

      EnumEntry<E> entries[N];
    };

    template <typename E>
    class StatusEnumEntryMapContainer{
      using container_type = std::span<const StatusEnumEntry<E>>;
    public:
      using value_type = const StatusEnumEntry<E>;
      using size_type = u32;
      using difference_type = i32;
      using pointer = value_type*;
      using const_pointer = const value_type*;
      using reference = value_type&;
      using const_reference = const value_type&;

      using iterator = typename container_type::iterator;
      using const_iterator = iterator;
      using reverse_iterator = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;

      using allocator_type = void;


      constexpr StatusEnumEntryMapContainer(std::initializer_list<StatusEnumEntry<E>> list) noexcept
          : entries{ list }{}

      constexpr size_type size() const noexcept { return entries.size(); }
      constexpr bool      empty() const noexcept { return entries.empty(); }

      constexpr reference operator[](size_type pos) const noexcept { return entries[pos]; }

      constexpr reference front() const noexcept { return entries.front(); }
      constexpr reference back() const noexcept { return entries.back(); }

      constexpr pointer data() const noexcept { return entries.data(); }

      constexpr iterator begin() const noexcept { return entries.begin(); }
      constexpr const_iterator cbegin() const noexcept { return entries.begin(); }

      constexpr iterator end() const noexcept { return entries.end(); }
      constexpr const_iterator cend() const noexcept { return entries.end(); }

      constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
      constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

      constexpr reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }
      constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

      constexpr iterator erase(const_iterator) noexcept { VK_unreachable; }
      constexpr iterator erase(const_iterator, const_iterator) noexcept { VK_unreachable; }


    private:
      container_type entries;
    };

    template <typename E, size_t N>
    class EnumEntryMap{
      using container_type = flat_map<E, string_literal, EnumEntryMapContainer<E, N>>;
      using underlying_type = std::underlying_type_t<E>;
      using enum_type = E;
    public:

      template <typename ...Args> requires(exact_same_as<Args, EnumEntry<E>> && ...)
      constexpr EnumEntryMap(Args&& ...args) noexcept
          : entries{ guaranteeUnique, std::in_place, std::forward<Args>(args)... }{}

      constexpr string_literal operator[](E value) const noexcept {
        if (auto result = entries.find(value); result != entries.end())
          return result->value;
        return VK_empty_string;
      }

      constexpr bool hasNamedValue(underlying_type val) const noexcept {
        return entries.contains(static_cast<enum_type>(val));
      }

      constexpr std::optional<E> cast(underlying_type val) const noexcept {
        auto castVal = static_cast<enum_type>(val);
        if (entries.contains(castVal))
          return castVal;
        return std::nullopt;
      }

      constexpr const EnumEntry<E>* find(E value) const noexcept {
        if (auto result = entries.find(value); result == entries.end())
          return nullptr;
        else
          return &*result;
      }

    private:
      container_type entries;
    };

    template <typename E, typename ...Args>
    EnumEntryMap(EnumEntry<E>&&, Args&&...) -> EnumEntryMap<E, sizeof...(Args) + 1>;
  }

  template <typename E>
  class StatusEnumMap{
    using container_type = flat_map<E, detail::StatusEnumInfo, detail::StatusEnumEntryMapContainer<E>>;
    using underlying_type = std::underlying_type_t<E>;
    using enum_type = E;

    inline constexpr static detail::StatusEnumInfo missingInfo{severity::error, VK_raw_string(Unknown Error), {} };

  public:
    constexpr explicit StatusEnumMap(std::initializer_list<detail::StatusEnumEntry<E>> list) noexcept
    : entries{ guaranteeUnique, std::in_place, list }{}

    constexpr const detail::StatusEnumInfo& operator[](E value) const noexcept {
      if (auto result = entries.find(value); result != entries.end())
        return result->value;
      return missingInfo;
    }

  private:
    container_type entries;
  };




#define PP_VK_impl_STATUS_ENUM_PERIOD_severity next( .severity
#define PP_VK_impl_STATUS_ENUM_PERIOD_message  next( .message
#define PP_VK_impl_STATUS_ENUM_PERIOD_generic  finish( .generic

#define PP_VK_impl_STATUS_ENUM_10_next(a, ...) a, PP_VK_impl_STATUS_ENUM_FOR_EACH_9(__VA_ARGS__)
#define PP_VK_impl_STATUS_ENUM_9_next(a, ...) a, PP_VK_impl_STATUS_ENUM_FOR_EACH_8(__VA_ARGS__)
#define PP_VK_impl_STATUS_ENUM_8_next(a, ...) a, PP_VK_impl_STATUS_ENUM_FOR_EACH_7(__VA_ARGS__)
#define PP_VK_impl_STATUS_ENUM_7_next(a, ...) a, PP_VK_impl_STATUS_ENUM_FOR_EACH_6(__VA_ARGS__)
#define PP_VK_impl_STATUS_ENUM_6_next(a, ...) a, PP_VK_impl_STATUS_ENUM_FOR_EACH_5(__VA_ARGS__)
#define PP_VK_impl_STATUS_ENUM_5_next(a, ...) a, PP_VK_impl_STATUS_ENUM_FOR_EACH_4(__VA_ARGS__)
#define PP_VK_impl_STATUS_ENUM_4_next(a, ...) a, PP_VK_impl_STATUS_ENUM_FOR_EACH_3(__VA_ARGS__)
#define PP_VK_impl_STATUS_ENUM_3_next(a, ...) a, PP_VK_impl_STATUS_ENUM_FOR_EACH_2(__VA_ARGS__)
#define PP_VK_impl_STATUS_ENUM_2_next(a, ...) a, PP_VK_impl_STATUS_ENUM_FOR_EACH_1(__VA_ARGS__)
#define PP_VK_impl_STATUS_ENUM_1_next(a) a

#define PP_VK_impl_STATUS_ENUM_9_finish(...) __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_8_finish(...) __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_7_finish(...) __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_6_finish(...) __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_5_finish(...) __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_4_finish(...) __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_3_finish(...) __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_2_finish(...) __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_1_finish(...) __VA_ARGS__


#define PP_VK_impl_STATUS_ENUM_LAZY_10_redirect_2(a, ...) PP_VK_impl_STATUS_ENUM_10_##a, __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_LAZY_10_redirect PP_VK_impl_STATUS_ENUM_LAZY_10_redirect_2
#define PP_VK_impl_STATUS_ENUM_LAZY_10(...) PP_VK_impl_STATUS_ENUM_LAZY_10_redirect(__VA_ARGS__)

#define PP_VK_impl_STATUS_ENUM_LAZY_9_redirect_2(a, ...) PP_VK_impl_STATUS_ENUM_9_##a, __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_LAZY_9_redirect PP_VK_impl_STATUS_ENUM_LAZY_9_redirect_2
#define PP_VK_impl_STATUS_ENUM_LAZY_9(...) PP_VK_impl_STATUS_ENUM_LAZY_9_redirect(__VA_ARGS__)

#define PP_VK_impl_STATUS_ENUM_LAZY_8_redirect_2(a, ...) PP_VK_impl_STATUS_ENUM_8_##a, __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_LAZY_8_redirect PP_VK_impl_STATUS_ENUM_LAZY_8_redirect_2
#define PP_VK_impl_STATUS_ENUM_LAZY_8(...) PP_VK_impl_STATUS_ENUM_LAZY_8_redirect(__VA_ARGS__)

#define PP_VK_impl_STATUS_ENUM_LAZY_7_redirect_2(a, ...) PP_VK_impl_STATUS_ENUM_7_##a, __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_LAZY_7_redirect PP_VK_impl_STATUS_ENUM_LAZY_7_redirect_2
#define PP_VK_impl_STATUS_ENUM_LAZY_7(...) PP_VK_impl_STATUS_ENUM_LAZY_7_redirect(__VA_ARGS__)

#define PP_VK_impl_STATUS_ENUM_LAZY_6_redirect_2(a, ...) PP_VK_impl_STATUS_ENUM_6_##a, __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_LAZY_6_redirect PP_VK_impl_STATUS_ENUM_LAZY_6_redirect_2
#define PP_VK_impl_STATUS_ENUM_LAZY_6(...) PP_VK_impl_STATUS_ENUM_LAZY_6_redirect(__VA_ARGS__)

#define PP_VK_impl_STATUS_ENUM_LAZY_5_redirect_2(a, ...) PP_VK_impl_STATUS_ENUM_5_##a, __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_LAZY_5_redirect PP_VK_impl_STATUS_ENUM_LAZY_5_redirect_2
#define PP_VK_impl_STATUS_ENUM_LAZY_5(...) PP_VK_impl_STATUS_ENUM_LAZY_5_redirect(__VA_ARGS__)

#define PP_VK_impl_STATUS_ENUM_LAZY_4_redirect_2(a, ...) PP_VK_impl_STATUS_ENUM_4_##a, __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_LAZY_4_redirect PP_VK_impl_STATUS_ENUM_LAZY_4_redirect_2
#define PP_VK_impl_STATUS_ENUM_LAZY_4(...) PP_VK_impl_STATUS_ENUM_LAZY_4_redirect(__VA_ARGS__)

#define PP_VK_impl_STATUS_ENUM_LAZY_3_redirect_2(a, ...) PP_VK_impl_STATUS_ENUM_3_##a, __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_LAZY_3_redirect PP_VK_impl_STATUS_ENUM_LAZY_3_redirect_2
#define PP_VK_impl_STATUS_ENUM_LAZY_3(...) PP_VK_impl_STATUS_ENUM_LAZY_3_redirect(__VA_ARGS__)

#define PP_VK_impl_STATUS_ENUM_LAZY_2_redirect_2(a, ...) PP_VK_impl_STATUS_ENUM_2_##a, __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_LAZY_2_redirect PP_VK_impl_STATUS_ENUM_LAZY_2_redirect_2
#define PP_VK_impl_STATUS_ENUM_LAZY_2(...) PP_VK_impl_STATUS_ENUM_LAZY_2_redirect(__VA_ARGS__)

#define PP_VK_impl_STATUS_ENUM_LAZY_1_redirect_2(a, ...) PP_VK_impl_STATUS_ENUM_1_##a, __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_LAZY_1_redirect PP_VK_impl_STATUS_ENUM_LAZY_1_redirect_2
#define PP_VK_impl_STATUS_ENUM_LAZY_1(...) PP_VK_impl_STATUS_ENUM_LAZY_1_redirect(__VA_ARGS__)

#define PP_VK_impl_STATUS_ENUM_LAZY_0_redirect_2(a, ...) PP_VK_impl_STATUS_ENUM_0_##a, __VA_ARGS__
#define PP_VK_impl_STATUS_ENUM_LAZY_0_redirect PP_VK_impl_STATUS_ENUM_LAZY_0_redirect_2
#define PP_VK_impl_STATUS_ENUM_LAZY_0(...) PP_VK_impl_STATUS_ENUM_LAZY_0_redirect(__VA_ARGS__)



#define PP_VK_impl_STATUS_ENUM_MINI_FOR_EACH(...) VK_concat(PP_VK_impl_STATUS_ENUM_FOR_EACH, VK_count_args(__VA_ARGS__))(__VA_ARGS__)
#define PP_VK_impl_STATUS_ENUM_FOR_EACH_10(...) PP_VK_impl_STATUS_ENUM_LAZY_10(PP_VK_impl_STATUS_ENUM_PERIOD_##__VA_ARGS__))
#define PP_VK_impl_STATUS_ENUM_FOR_EACH_9(...)  PP_VK_impl_STATUS_ENUM_LAZY_9(PP_VK_impl_STATUS_ENUM_PERIOD_##__VA_ARGS__))
#define PP_VK_impl_STATUS_ENUM_FOR_EACH_8(...)  PP_VK_impl_STATUS_ENUM_LAZY_8(PP_VK_impl_STATUS_ENUM_PERIOD_##__VA_ARGS__))
#define PP_VK_impl_STATUS_ENUM_FOR_EACH_7(...)  PP_VK_impl_STATUS_ENUM_LAZY_7(PP_VK_impl_STATUS_ENUM_PERIOD_##__VA_ARGS__))
#define PP_VK_impl_STATUS_ENUM_FOR_EACH_6(...)  PP_VK_impl_STATUS_ENUM_LAZY_6(PP_VK_impl_STATUS_ENUM_PERIOD_##__VA_ARGS__))
#define PP_VK_impl_STATUS_ENUM_FOR_EACH_5(...)  PP_VK_impl_STATUS_ENUM_LAZY_5(PP_VK_impl_STATUS_ENUM_PERIOD_##__VA_ARGS__))
#define PP_VK_impl_STATUS_ENUM_FOR_EACH_4(...)  PP_VK_impl_STATUS_ENUM_LAZY_4(PP_VK_impl_STATUS_ENUM_PERIOD_##__VA_ARGS__))
#define PP_VK_impl_STATUS_ENUM_FOR_EACH_3(...)  PP_VK_impl_STATUS_ENUM_LAZY_3(PP_VK_impl_STATUS_ENUM_PERIOD_##__VA_ARGS__))
#define PP_VK_impl_STATUS_ENUM_FOR_EACH_2(...)  PP_VK_impl_STATUS_ENUM_LAZY_2(PP_VK_impl_STATUS_ENUM_PERIOD_##__VA_ARGS__))
#define PP_VK_impl_STATUS_ENUM_FOR_EACH_1(...)  PP_VK_impl_STATUS_ENUM_LAZY_1(PP_VK_impl_STATUS_ENUM_PERIOD_##__VA_ARGS__))
#define PP_VK_impl_STATUS_ENUM_FOR_EACH_0() {}



#define PP_VK_impl_PROCESS_STATUS_ENUM_INFO_redirect_2(val, ...) valkyrie::detail::StatusEnumEntry<enum_type>{ enum_type::val, valkyrie::detail::StatusEnumInfo{ PP_VK_impl_STATUS_ENUM_MINI_FOR_EACH(__VA_ARGS__) }}
#define PP_VK_impl_PROCESS_STATUS_ENUM_INFO_redirect(x) PP_VK_impl_PROCESS_STATUS_ENUM_INFO_redirect_2 x
#define PP_VK_impl_PROCESS_STATUS_ENUM_INFO PP_VK_impl_PROCESS_STATUS_ENUM_INFO_redirect


#define VK_status_enum_domain(name_, uuid_) inline constexpr static ::valkyrie::StringView domain_name{VK_raw_string(name_)}; inline constexpr static ::valkyrie::uuid domain_uuid{uuid_}
#define VK_status_enum_info(...) inline static const ::valkyrie::StatusEnumMap info{ VK_for_each_delimit(PP_VK_impl_PROCESS_STATUS_ENUM_INFO, VK_comma_delimiter, ##__VA_ARGS__) }

  /*template <>
  struct Custom::status_enum_type<ExampleEnum>{
    using enum_type = ExampleEnum;
    struct domain_type{
      using value_type = enum_type;
    };

    VK_status_enum_info(
        (A, message = "Alice"),

        (B, severity = severity::Debug, message = "Bob"),

        (C, message = "Charlie", generic = { code::done, code::insufficient_size })
    );
  };*/
}

#endif//VALKYRIE_UTILITIES_ENUMS_HPP
