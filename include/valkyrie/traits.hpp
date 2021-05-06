//
// Created by maxwe on 2021-04-28.
//

#ifndef VALKYRIE_TRAITS_HPP
#define VALKYRIE_TRAITS_HPP


#include "meta.hpp"
#include "utility/uuid.hpp"
#include "utility/pow2.hpp"

#include <bit>
#include <ranges>


namespace valkyrie{

  class memory_block;
  template <typename>
  class status_code;
  template <typename Domain>
  class error_code;
  class status_domain;
  class generic_domain;
  template <typename E>
  class status_enum_domain;

  template </*concepts::enumerator*/ typename E>
  class bitflag;

  class string_view;

  enum class dynamic_t : u32;
  enum class severity  : i32;
  enum class code      : i32;

  struct root{};
  struct variable_size{};

  inline namespace concepts{

    template <typename T, typename U>
    concept same_as = meta::is_same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>::value;
    template <typename T, typename U>
    concept exact_same_as = meta::is_same_as<T, U>::value && meta::is_same_as<T, U>::value;


    template <typename T, typename U>
    concept not_exact_same_as = !exact_same_as<T, U>;
    template <typename T, typename U>
    concept not_same_as = !same_as<T, U> && not_exact_same_as<T, U>;

    template <typename T, typename ...U>
    concept same_as_one_of = (same_as<T, U> || ...);
    template <typename T, typename ...U>
    concept exact_same_as_one_of = (exact_same_as<T, U> || ...) && same_as_one_of<T, U...>;


    template <typename T, typename ...U>
    concept not_exact_same_as_one_of = (not_exact_same_as<T, U> && ...) && !exact_same_as_one_of<T, U...>;
    template <typename T, typename ...U>
    concept not_same_as_one_of = (not_same_as<T, U> && ...) && !same_as_one_of<T, U...> && not_exact_same_as_one_of<T, U...>;





    template <typename From, typename To>
    concept convertible_to = std::convertible_to<From, To>;
    template <typename T, typename ...Args>
    concept constructible_from = std::constructible_from<T, Args...>;



    template <typename From, typename To>
    concept pointer_castable_to    = requires(std::add_pointer_t<From> from,
                                              std::add_lvalue_reference_t<std::add_pointer_t<To>> to){
      to = static_cast<To*>(from);
    };

    template <typename From, typename To>
    concept pointer_convertible_to =
        pointer_castable_to<From, To> &&
        convertible_to<std::add_pointer_t<From>, std::add_pointer_t<To>> &&
        requires(std::add_pointer_t<From> from, std::add_lvalue_reference_t<std::add_pointer_t<To>> to){
      to = from;
    };

    template <typename P, typename T = const void>
    concept raw_pointer = std::is_pointer_v<P> && pointer_convertible_to<std::remove_pointer_t<P>, T>;
    template <typename P>
    concept member_pointer = meta::is_a_member_pointer<P>::value;



    /*template <typename T>
    concept enumerator = std::is_enum_v<T>;*/

    template <typename T, typename U>
    concept equality_comparable_with = requires(const T& t, const U& u){
      { t == u } -> exact_same_as<bool>;
      { u == t } -> exact_same_as<bool>;
      { t != u } -> exact_same_as<bool>;
      { u != t } -> exact_same_as<bool>;
    };
    template <typename T>
    concept equality_comparable = equality_comparable_with<T, T>;
    template <typename T, typename U>
    concept ordered_with = equality_comparable_with<T, U> && requires(const T& t, const U& u) {
      { t < u } -> std::convertible_to<bool>;
      { t > u } -> std::convertible_to<bool>;
      { t <= u } -> std::convertible_to<bool>;
      { t <= u } -> std::convertible_to<bool>;
      { u < t } -> std::convertible_to<bool>;
      { u > t } -> std::convertible_to<bool>;
      { u <= t } -> std::convertible_to<bool>;
      { u <= t } -> std::convertible_to<bool>;
    };
    template <typename T>
    concept ordered = ordered_with<T, T>;


    template <typename O, typename T, typename U = T>
    concept weak_order = requires(O&& order, const T& t, const U& u){
      { std::forward<O>(order)(t, u) } -> std::convertible_to<std::weak_ordering>;
    };




    template <typename Ext, typename Ind = size_t>
    concept extent_type = same_as<Ext, dynamic_t> || convertible_to<Ind, Ext>;
    template <typename Ext, typename Ind = size_t>
    concept strict_extent_type = same_as_one_of<Ext, dynamic_t, Ind> && extent_type<Ext, Ind>;




    template <typename T>
    concept is_register_copyable = std::conjunction_v<std::is_trivially_copyable<T>, std::is_trivially_destructible<T>> && sizeof(T) <= 16;
  }


  template <typename T>
  using in_param_t = std::conditional_t<is_register_copyable<T>, T, const T&>;
  template <typename T>
  using out_param_t = T&;


  namespace traits{
    template <typename T, typename U>
    struct alias : meta::template_no_default{};

    template <typename Ptr>
    struct pointer : meta::template_no_default{};

    template <typename E>
    struct bitflag_enum : meta::template_no_default{};

    template <typename E>
    struct status_enum : meta::template_no_default{};

    template <typename E>
    struct enumerator : meta::template_no_default{};

    template <typename E>
    struct custom_status : meta::template_no_default{};

    template <typename T>
    struct bitwise_movable : meta::template_no_default{};

    template <typename S>
    struct string : meta::template_no_default{};

    template <typename A>
    struct allocator : meta::template_no_default{};

    template <typename I>
    struct iterator : meta::template_no_default{};

    template <typename T>
    struct serializable : meta::template_no_default{};

    template <typename T>
    struct container : meta::template_no_default{};

    template <typename T>
    struct range : meta::template_no_default{};

    template <typename T>
    struct singleton : meta::template_no_default{};

    template <typename T>
    struct klass : meta::template_no_default{};

    template <typename T>
    struct numeric : meta::template_no_default{};

    template <typename T>
    struct domain : meta::template_no_default{};

    template <typename T>
    struct graph : meta::template_no_default{
      // using node_type = ...;
      // using edge_type = ...;
    };

    template <typename T>
    struct message : meta::template_no_default{};

    template <typename T>
    struct hash : meta::template_no_default{};


    namespace detail{
      namespace _inner{
        template <typename To>
        struct ref_cast_{
          template <typename From>
          inline To operator()(From&& from) const noexcept requires(requires{ static_cast<To>(std::forward<From>(from)); }) {
            return static_cast<To>(std::forward<From>(from));
          }
        };
        template <typename To>
        struct ref_cast_<To&>{
          VK_forceinline To& operator()(const To& to) const noexcept {
            return const_cast<To&>(to);
          }
          template <typename From>
          VK_forceinline To& operator()(const From& from) const noexcept requires(requires{ static_cast<To&>(const_cast<From&>(from)); }) {
            return static_cast<To&>(const_cast<From&>(from));
          }
          template <typename From>
          VK_forceinline To& operator()(From&& from) const noexcept requires(!requires{ static_cast<To&>(std::forward<From>(from)); } && requires{ (*this)(*from); }) {
            return (*this)(*from);
          }
          template <typename From>
          VK_forceinline To& operator()(From&& from) const noexcept requires(!std::is_lvalue_reference_v<From> && requires{ static_cast<To&>(std::move(from)); }){
            return static_cast<To&>(std::move(from));
          }
        };
        template <typename To>
        struct ref_cast_<To*>{
          VK_constant ref_cast_<To&> lvalue_cast{};
        public:
          template <pointer_castable_to<To> From>
          VK_forceinline To* operator()(const From* from) const noexcept {
            return static_cast<To*>(const_cast<From*>(from));
          }
          template <typename From>
          VK_forceinline To* operator()(From&& from) const noexcept
              requires(!raw_pointer<remove_ref_t<From>> && requires{ static_cast<To*>(std::forward<From>(from)); }){
            return static_cast<To*>(std::forward<From>(from));
          }
          template <typename From>
          VK_forceinline To* operator()(From&& from) const noexcept
              requires(!requires{ static_cast<To*>(std::forward<From>(from)); } && requires{ lvalue_cast(std::forward<From>(from)); }){
            return std::addressof(lvalue_cast(std::forward<From>(from)));
          }
        };
      }

      template <typename To>
      VK_constant _inner::ref_cast_<To> ref_cast{};




      template <typename T, template <typename> typename Trait>
      concept has_specialized = !requires{
        typename Trait<T>::invalid_specialization;
      };

#define VK_apply_if_nonempty(macro, ...) VK_if(VK_not(VK_pack_is_empty(__VA_ARGS__))(macro(__VA_ARGS__)))
#define VK_trait_init(val_) template <typename T, int N = val_> struct trait : trait<T, N - 1>{}
#define VK_trait_case(T, N, ...) template <typename T> VK_apply_if_nonempty(requires, __VA_ARGS__) struct trait<T, N>
#define VK_trait_case_match(params, args, N, ...) template < VK_unwrap(params) > VK_apply_if_nonempty(requires, __VA_ARGS__) struct trait< VK_unwrap(args), N >
#define VK_trait_fallback(T) VK_trait_case(T, 0)
#define VK_trait_no_fallback VK_trait_fallback(T) { u64 trait_failure_ = 0; }


      template <typename T>
      struct deferred_string_view{
        using type = string_view;
      };

      struct range{




        struct typedef_value_type{
          VK_trait_init(2);
          VK_trait_case(T, 2, requires{ typename traits::range<T>::value_type; }){
            using value_type = typename traits::range<T>::value_type;
          };
          VK_trait_case(T, 1, requires{ typename std::ranges::range_value_t<T>; }) {
            using value_type = typename std::ranges::range_value_t<T>;
          };
          VK_trait_no_fallback;
        };
        struct typedef_iterator_category{
          VK_trait_init(8);
          VK_trait_case(Rng, 8, requires{ typename traits::range<Rng>::iterator_category; }){
            using iterator_category = typename traits::range<Rng>::iterator_category;
          };
          VK_trait_case(Rng, 7, requires{ typename remove_cvref_t<Rng>::iterator_category; }){
            using iterator_category = typename remove_cvref_t<Rng>::iterator_category;
          };
          VK_trait_case(Rng, 6, std::ranges::contiguous_range<Rng>){
            using iterator_category = std::contiguous_iterator_tag;
          };
          VK_trait_case(Rng, 5, std::ranges::random_access_range<Rng>){
            using iterator_category = std::random_access_iterator_tag;
          };
          VK_trait_case(Rng, 4, std::ranges::bidirectional_range<Rng>){
            using iterator_category = std::bidirectional_iterator_tag;
          };
          VK_trait_case(Rng, 3, std::ranges::forward_range<Rng>){
            using iterator_category = std::forward_iterator_tag;
          };
          VK_trait_case(Rng, 2, std::ranges::input_range<Rng>){
            using iterator_category = std::input_iterator_tag;
          };
          VK_trait_case(Rng, 1, std::input_or_output_iterator<std::ranges::iterator_t<Rng>>){
            using iterator_category = std::output_iterator_tag;
          };

          VK_trait_no_fallback;
        };
        struct typedef_iterator{
          VK_trait_init(3);
          VK_trait_case(Rng, 3, requires{ typename traits::range<Rng>::iterator; }){
            using iterator = typename traits::range<Rng>::iterator;
          };
          VK_trait_case(Rng, 2, requires{ typename Rng::iterator; }){
            using iterator = typename Rng::iterator;
          };
          VK_trait_case(Rng, 1, requires{ typename std::ranges::iterator_t<Rng>; }){
            using iterator = typename std::ranges::iterator_t<Rng>;
          };
          VK_trait_no_fallback;
        };
        struct typedef_const_iterator{
          VK_trait_init(3);
          VK_trait_case(Rng, 3, requires{ typename traits::range<Rng>::const_iterator; }){
            using const_iterator = typename traits::range<Rng>::const_iterator;
          };
          VK_trait_case(Rng, 2, requires{ typename Rng::const_iterator; }){
            using const_iterator = typename Rng::const_iterator;
          };
          VK_trait_case(Rng, 1, requires{ typename std::ranges::iterator_t<add_const_t<Rng>>; }){
            using const_iterator = typename std::ranges::iterator_t<add_const_t<Rng>>;
          };
          VK_trait_no_fallback;
        };
        struct typedef_sentinel{
          VK_trait_init(3);
          VK_trait_case(Rng, 3, requires{ typename traits::range<Rng>::sentinel; }){
            using sentinel = typename traits::range<Rng>::sentinel;
          };
          VK_trait_case(Rng, 2, requires{ typename Rng::sentinel; }){
            using sentinel = typename Rng::sentinel;
          };
          VK_trait_case(Rng, 1, requires{ typename std::ranges::sentinel_t<Rng>; }){
            using sentinel = typename std::ranges::sentinel_t<Rng>;
          };
          VK_trait_no_fallback;
        };
        struct typedef_const_sentinel{
          VK_trait_init(3);
          VK_trait_case(Rng, 3, requires{ typename traits::range<Rng>::const_sentinel; }){
            using const_sentinel = typename traits::range<Rng>::const_sentinel;
          };
          VK_trait_case(Rng, 2, requires{ typename Rng::const_sentinel; }){
            using const_sentinel = typename Rng::const_sentinel;
          };
          VK_trait_case(Rng, 1, requires{ typename std::ranges::sentinel_t<add_const_t<Rng>>; }){
            using const_sentinel = typename std::ranges::sentinel_t<add_const_t<Rng>>;
          };
          VK_trait_no_fallback;
        };


        struct value_owns_elements{
          VK_trait_init(2);
          VK_trait_case(T, 2, requires{ { traits::range<remove_cvref_t<T>>::owns_elements } -> same_as<bool>; }){
            VK_constant bool owns_elements = traits::range<remove_cvref_t<T>>::owns_elements;
          };
          VK_trait_case(T, 1, std::ranges::viewable_range<remove_ref_t<T>>){
            VK_constant bool owns_elements = false;
          };
          VK_trait_fallback(T){
            VK_constant bool owns_elements = true;
          };
        };
        struct value_static_size{
          VK_trait_init(1);
          VK_trait_case(T, 1, requires{ traits::range<T>::static_size; }) {
            VK_constant auto static_size = traits::range<T>::static_size;
            VK_constant bool is_dynamic  = same_as<decltype(static_size), dynamic_t>;
          };
          VK_trait_fallback(T) {
            VK_constant auto static_size = dynamic_t{};
            VK_constant bool is_dynamic  = true;
          };
        };

        using trait_list = meta::list<typedef_value_type,
                                     value_static_size,
                                     typedef_iterator,
                                     typedef_const_iterator,
                                     typedef_sentinel,
                                     typedef_const_sentinel,
                                     typedef_iterator_category,
                                     value_owns_elements>;
      };
      struct pointer{
        // TODO: Add more pointer traits
        
        struct typedef_element_type{
          VK_trait_init(3);
          VK_trait_case(T, 3, requires{ typename traits::pointer<T>::element_type; }){
            using element_type = typename traits::pointer<T>::element_type;
          };
          VK_trait_case(T, 2, requires{ typename T::element_type; }){
            using element_type = typename T::element_type;
          };
          VK_trait_case(T, 1, requires{ typename std::pointer_traits<T>::element_type; }){
            using element_type = typename std::pointer_traits<T>::element_type;
          };
          VK_trait_no_fallback;
        };
        struct typedef_pointer{
          VK_trait_init(3);
          VK_trait_case(T, 3, requires{ typename traits::pointer<T>::pointer; }){
            using pointer = typename traits::pointer<T>::pointer;
          };
          VK_trait_case(T, 2, requires{ typename T::pointer; }){
            using pointer = typename T::pointer;
          };
          VK_trait_case(T, 1, requires{ typename std::pointer_traits<T>::pointer; }){
            using pointer = typename std::pointer_traits<T>::pointer;
          };
          VK_trait_fallback(T){
            using pointer = T;
          };
        };
        struct typedef_difference_type{
          VK_trait_init(3);
          VK_trait_case(T, 3, requires{ typename traits::pointer<T>::difference_type; }){
            using difference_type = typename traits::pointer<T>::difference_type;
          };
          VK_trait_case(T, 2, requires{ typename T::difference_type; }){
            using difference_type = typename T::difference_type;
          };
          VK_trait_case(T, 1, requires{ typename std::pointer_traits<T>::difference_type; }){
            using difference_type = typename std::pointer_traits<T>::difference_type;
          };
          VK_trait_no_fallback;
        };
        struct typedef_rebind{
          VK_trait_init(3);
          VK_trait_case(T, 3, requires{ typename traits::pointer<T>::template rebind<void>; }){
            template <typename U>
            using rebind = typename traits::pointer<T>::template rebind<U>;
          };
          VK_trait_case(T, 2, requires{ typename T::template rebind<void>; }){
            template <typename U>
            using rebind = typename T::template rebind<U>;
          };
          VK_trait_case(T, 1, requires{ typename std::pointer_traits<T>::template rebind<void>; }){
            template <typename U>
            using rebind = typename std::pointer_traits<T>::template rebind<U>;
          };
          VK_trait_no_fallback;
        };

        // pointer(element_type*)
        struct function_pointer_to{
          VK_trait_init(4);
          VK_trait_case(T, 4, requires(typename typedef_element_type::trait<T>::element_type* el){ { traits::pointer<T>::pointer_to(el) } -> same_as<T>; }){
            VK_forceinline static T pointer_to(typename typedef_element_type::trait<T>::element_type* el) noexcept {
              return valkyrie::traits::pointer<T>::pointer_to(el);
            }
          };
          VK_trait_case(T, 3, requires(typename typedef_element_type::trait<T>::element_type* el){ { T::pointer_to(el) } -> same_as<T>; }){
            VK_forceinline static T pointer_to(typename typedef_element_type::trait<T>::element_type* el) noexcept {
              return T::pointer_to(el);
            }
          };
          VK_trait_case(T, 2, requires(typename typedef_element_type::trait<T>::element_type* el){ { std::pointer_traits<T>::pointer_to(el) } -> same_as<T>; }){
            VK_forceinline static T pointer_to(typename typedef_element_type::trait<T>::element_type* el) noexcept {
              return std::pointer_traits<T>::pointer_to(el);
            }
          };
          VK_trait_case_match(typename T, T*, 1){
            VK_forceinline static T* pointer_to(T* p) noexcept {
              return p;
            }
          };
          VK_trait_no_fallback;
        };
        // element_type*(const pointer&)
        struct function_to_address{
          VK_trait_init(3);
          VK_trait_case(T, 3, requires(const T& ptr){ { traits::pointer<T>::to_address(ptr) } -> same_as<typename typedef_element_type::trait<T>::element_type*>; }){
            VK_forceinline static auto* to_address(const T& ptr) noexcept {
              return traits::pointer<T>::to_address(ptr);
            }
          };
          VK_trait_case(T, 2, requires(const T& ptr){ { T::to_address(ptr) } -> same_as<typename typedef_element_type::trait<T>::element_type*>; }){
            VK_forceinline static auto* to_address(const T& ptr) noexcept {
              return T::to_address(ptr);
            }
          };
          VK_trait_case(T, 1, requires(const T& ptr){ { std::to_address(ptr) } -> same_as<typename typedef_element_type::trait<T>::element_type*>; }){
            VK_forceinline static auto* to_address(const T& ptr) noexcept {
              return std::to_address(ptr);
            }
          };
          VK_trait_no_fallback;
        };


        struct integer_free_low_bits{
          VK_trait_init(3);
          VK_trait_case(T, 3, requires{ { traits::pointer<T>::free_low_bits } -> std::integral; }){
            VK_constant u32 free_low_bits = u32(traits::pointer<T>::free_low_bits);
          };
          VK_trait_case(T, 2, requires{ { T::free_low_bits } -> std::integral; }){
            VK_constant u32 free_low_bits = u32(T::free_low_bits);
          };
          VK_trait_case_match(typename T, T*, 1){
            VK_constant u32 free_low_bits = ilog2(alignof(T));
          };
          VK_trait_fallback(T){
            VK_constant u32 free_low_bits = 0;
          };
        };

        using trait_list = meta::list<typedef_element_type,
                                     typedef_pointer,
                                     typedef_difference_type,
                                     typedef_rebind,
                                     function_pointer_to,
                                     function_to_address,
                                     integer_free_low_bits>;
      };
      struct enumerator{
        struct string_name{
          VK_trait_init(3);
          VK_trait_case(T, 3, requires{ traits::enumerator<T>::name; }){
            VK_constant typename deferred_string_view<T>::type name = traits::enumerator<T>::name;
          };
          VK_trait_case(T, 2, requires{ traits::status_enum<T>::name; }){
            VK_constant typename deferred_string_view<T>::type name = traits::status_enum<T>::name;
          };
          VK_trait_case(T, 1, requires{ traits::bitflag_enum<T>::name; }){
            VK_constant typename deferred_string_view<T>::type name = traits::bitflag_enum<T>::name;
          };
          VK_trait_fallback(T){
            // TODO: automatic generation of type enum_traits<T>::name using VK_function_name hackery
            VK_constant typename deferred_string_view<T>::type name = "automatic generation of enum_traits<T>::name is not yet implemented";
          };
        };
        struct string_scoped_name{
          VK_trait_init(3);
          VK_trait_case(T, 3, requires{ traits::enumerator<T>::scoped_name; }){
            VK_constant typename deferred_string_view<T>::type scoped_name = traits::enumerator<T>::scoped_name;
          };
          VK_trait_case(T, 2, requires{ traits::status_enum<T>::scoped_name; }){
            VK_constant typename deferred_string_view<T>::type scoped_name = traits::status_enum<T>::scoped_name;
          };
          VK_trait_case(T, 1, requires{ traits::bitflag_enum<T>::scoped_name; }){
            VK_constant typename deferred_string_view<T>::type scoped_name = traits::bitflag_enum<T>::scoped_name;
          };
          VK_trait_fallback(T){
            // TODO: automatic generation of type enum_traits<T>::scoped_name using VK_function_name hackery
            VK_constant typename deferred_string_view<T>::type scoped_name = "automatic generation of class_traits<T>::scoped_name is not yet implemented";
          };
        };

        struct bool_is_bitflag{
          VK_trait_init(1);
          VK_trait_case(E, 1, !requires{ typename traits::bitflag_enum<E>::invalid_specialization; }){
            VK_constant bool is_bitflag = true;
          };
          VK_trait_fallback(E){
            VK_constant bool is_bitflag = false;
          };
        };
        struct bool_is_status_code{
          VK_trait_init(1);
          VK_trait_case(E, 1, !requires{ typename traits::status_enum<E>::invalid_specialization; }){
            VK_constant bool is_status_code = true;
          };
          VK_trait_fallback(E){
            VK_constant bool is_status_code = false;
          };
        };

        struct array_values{
          // TODO: Implement enum_traits<E>::values
          VK_trait_init(1);
          VK_trait_fallback(T){};
        };
        struct array_entries{
          // TODO: Implement enum_traits<E>::entries
          VK_trait_init(1);
          VK_trait_fallback(T){};
        };

        struct typedef_underlying_type{
          VK_trait_init(4);
          VK_trait_case(E, 4, requires{ typename traits::enumerator<E>::underlying_type; }){
            using underlying_type = typename traits::enumerator<E>::underlying_type;
          };
          VK_trait_case(E, 3, requires{ typename traits::status_enum<E>::underlying_type; }){
            using underlying_type = typename traits::status_enum<E>::underlying_type;
          };
          VK_trait_case(E, 2, requires{ typename traits::bitflag_enum<E>::underlying_type; }){
            using underlying_type = typename traits::bitflag_enum<E>::underlying_type;
          };
          VK_trait_case(E, 1, requires{ typename std::underlying_type<E>::type; }){
            using underlying_type = std::underlying_type_t<E>;
          };
          VK_trait_no_fallback;
        };
        struct typedef_status_domain{
          // Only for status_enum types
          VK_trait_init(2);
          VK_trait_case(E, 2, requires{ typename traits::status_enum<E>::status_domain; }){
            using status_domain = typename traits::status_enum<E>::status_domain;
          };
          VK_trait_case(E, 1, !requires{ typename traits::status_enum<E>::invalid_specialization; }){
            using status_domain = status_enum_domain<E>;
          };
          VK_trait_fallback(E){};
        };
        struct typedef_flag_type{
          // Only for bitflag types
          VK_trait_init(2);
          VK_trait_case(E, 2, requires{ typename traits::bitflag_enum<E>::flag_type; }){
            using flag_type = typename traits::bitflag_enum<E>::flag_type;
          };
          VK_trait_case(E, 1, !requires{ typename traits::bitflag_enum<E>::invalid_specialization; }){
            using flag_type = bitflag<E>;
          };
          VK_trait_fallback(E){};
        };


        using trait_list = meta::list<string_name,
                                     string_scoped_name,
                                     bool_is_bitflag,
                                     bool_is_status_code,
                                     array_values,
                                     array_entries,
                                     typedef_underlying_type,
                                     typedef_status_domain,
                                     typedef_flag_type>;
      };
      struct graph{

        struct typedef_node_type{
          VK_trait_init(2);
          VK_trait_case(G, 2, requires{ typename traits::graph<G>::node_type; }){
            using node_type = typename traits::graph<G>::node_type;
          };
          VK_trait_case(G, 1, requires{ typename G::node_type; }){
            using node_type = typename G::node_type;
          };
          VK_trait_no_fallback;
        };
        struct typedef_node_ref{
          VK_trait_init(3);
          VK_trait_case(G, 3, requires{ typename traits::graph<G>::node_ref; }){
            using node_ref = typename traits::graph<G>::node_ref;
          };
          VK_trait_case(G, 2, requires{ typename G::node_ref; }){
            using node_ref = typename G::node_ref;
          };
          VK_trait_case(G, 1, requires{ typename typedef_node_type::trait<G>::node_type; }){
            using node_ref = typename typedef_node_type::trait<G>::node_type*;
          };
          VK_trait_no_fallback;
        };

        struct typedef_edge_type{
          VK_trait_init(3);
          VK_trait_case(G, 3, requires{ typename traits::graph<G>::edge_type; }){
            using edge_type = typename traits::graph<G>::edge_type;
          };
          VK_trait_case(G, 2, requires{ typename G::edge_type; }){
            using edge_type = typename G::edge_type;
          };
          VK_trait_case(G, 1, requires{ typename typedef_node_type::trait<G>::node_type::edge_type; }){
            using edge_type = typename typedef_node_type::trait<G>::node_type::edge_type;
          };
          VK_trait_no_fallback;
        };
        struct typedef_edge_ref{
          VK_trait_init(3);
          VK_trait_case(G, 3, requires{ typename traits::graph<G>::edge_ref; }){
            using edge_ref = typename traits::graph<G>::edge_ref;
          };
          VK_trait_case(G, 2, requires{ typename G::edge_ref; }){
            using edge_ref = typename G::edge_ref;
          };
          VK_trait_case(G, 1, requires{ typename typedef_edge_type::trait<G>::edge_type; }){
            using edge_ref = typename typedef_edge_type::trait<G>::edge_type*;
          };
          VK_trait_no_fallback;
        };

        struct typedef_nodes_iterator{
          VK_trait_init(2);
          VK_trait_case(G, 2, requires{ typename traits::graph<G>::nodes_iterator; }){
            using nodes_iterator = typename traits::graph<G>::nodes_iterator;
          };
          VK_trait_case(G, 1, requires{ typename G::nodes_iterator; }){
            using nodes_iterator = typename G::nodes_iterator;
          };
          VK_trait_no_fallback;
        };
        struct typedef_child_iterator{
          VK_trait_init(2);
          VK_trait_case(G, 2, requires{ typename traits::graph<G>::child_iterator; }){
            using child_iterator = typename traits::graph<G>::child_iterator;
          };
          VK_trait_case(G, 1, requires{ typename G::child_iterator; }){
            using child_iterator = typename G::child_iterator;
          };
          VK_trait_no_fallback;
        };
        struct typedef_child_edge_iterator{
          VK_trait_init(2);
          VK_trait_case(G, 2, requires{ typename traits::graph<G>::child_edge_iterator; }){
            using child_edge_iterator = typename traits::graph<G>::child_edge_iterator;
          };
          VK_trait_case(G, 1, requires{ typename G::child_edge_iterator; }){
            using child_edge_iterator = typename G::child_edge_iterator;
          };
          VK_trait_no_fallback;
        };

        struct typedef_nodes_sentinel{
          VK_trait_init(2);
          VK_trait_case(G, 2, requires{ typename traits::graph<G>::nodes_sentinel; }){
            using nodes_sentinel = typename traits::graph<G>::nodes_sentinel;
          };
          VK_trait_case(G, 1, requires{ typename G::nodes_sentinel; }){
            using nodes_sentinel = typename G::nodes_sentinel;
          };
          VK_trait_fallback(G){
            using nodes_sentinel = typename typedef_nodes_iterator::trait<G>::nodes_iterator;
          };
        };
        struct typedef_child_sentinel{
          VK_trait_init(2);
          VK_trait_case(G, 2, requires{ typename traits::graph<G>::child_sentinel; }){
            using child_sentinel = typename traits::graph<G>::child_sentinel;
          };
          VK_trait_case(G, 1, requires{ typename G::child_sentinel; }){
            using child_sentinel = typename G::child_sentinel;
          };
          VK_trait_fallback(G){
            using child_sentinel = typename typedef_child_iterator::trait<G>::child_iterator;
          };
        };
        struct typedef_child_edge_sentinel{
          VK_trait_init(2);
          VK_trait_case(G, 2, requires{ typename traits::graph<G>::child_edge_sentinel; }){
            using child_edge_sentinel = typename traits::graph<G>::child_edge_sentinel;
          };
          VK_trait_case(G, 1, requires{ typename G::child_edge_sentinel; }){
            using child_edge_sentinel = typename G::child_edge_sentinel;
          };
          VK_trait_fallback(G){
            using child_edge_sentinel = typename typedef_child_edge_iterator::trait<G>::child_edge_iterator;
          };
        };


        // nodes_iterator(graph_type&)
        struct function_nodes_begin{
          VK_trait_init(2);
          VK_trait_case(G, 2, requires(G& graph){ traits::graph<G>::nodes_begin(graph); }){
            inline static decltype(auto) nodes_begin(G& graph) noexcept {
              using nodes_iterator = typename typedef_nodes_iterator::trait<G>::nodes_iterator;
              return nodes_iterator(traits::graph<G>::nodes_begin(graph));
            }
          };
          VK_trait_case(G, 1, requires(G& graph){ graph.nodes_begin(); }){
            inline static decltype(auto) nodes_begin(G& graph) noexcept {
              using nodes_iterator = typename typedef_nodes_iterator::trait<G>::nodes_iterator;
              return nodes_iterator(graph.nodes_begin());
            }
          };
          VK_trait_no_fallback;
        };
        // nodes_sentinel(graph_type&)
        struct function_nodes_end{
          VK_trait_init(2);
          VK_trait_case(G, 2, requires(G& graph){ traits::graph<G>::nodes_end(graph); }){
            inline static decltype(auto) nodes_end(G& graph) noexcept {
              using nodes_sentinel = typename typedef_nodes_sentinel::trait<G>::nodes_sentinel;
              return nodes_sentinel(traits::graph<G>::nodes_end(graph));
            }
          };
          VK_trait_case(G, 1, requires(G& graph){ graph.nodes_end(); }){
            inline static decltype(auto) nodes_end(G& graph) noexcept {
              using nodes_sentinel = typename typedef_nodes_sentinel::trait<G>::nodes_sentinel;
              return nodes_sentinel(graph.nodes_end());
            }
          };
          VK_trait_no_fallback;
        };


        // child_iterator(node_ref)
        struct function_child_begin{
          VK_trait_init(3);
          VK_trait_case(G, 3, requires(typename typedef_node_ref::trait<G>::node_ref node){ traits::graph<G>::child_begin(node); }){
            inline static decltype(auto) child_begin(typename typedef_node_ref::trait<G>::node_ref node) noexcept {
              using child_iterator = typename typedef_child_iterator::trait<G>::child_iterator;
              return child_iterator(traits::graph<G>::child_begin(node));
            }
          };
          VK_trait_case(G, 2, requires(typename typedef_node_ref::trait<G>::node_ref node){ node.child_begin(); }){
            inline static decltype(auto) child_begin(typename typedef_node_ref::trait<G>::node_ref node) noexcept {
              using child_iterator = typename typedef_child_iterator::trait<G>::child_iterator;
              return child_iterator(node.child_begin());
            }
          };
          VK_trait_case(G, 1, requires(typename typedef_node_ref::trait<G>::node_ref node){ node->child_begin(); }){
            inline static decltype(auto) child_begin(typename typedef_node_ref::trait<G>::node_ref node) noexcept {
              using child_iterator = typename typedef_child_iterator::trait<G>::child_iterator;
              return child_iterator(node->child_begin());
            }
          };
          VK_trait_no_fallback;
        };
        // child_sentinel(node_ref)
        struct function_child_end{
          VK_trait_init(3);
          VK_trait_case(G, 3, requires(typename typedef_node_ref::trait<G>::node_ref node){ traits::graph<G>::child_end(node); }){
            inline static decltype(auto) child_end(typename typedef_node_ref::trait<G>::node_ref node) noexcept {
              using child_sentinel = typename typedef_child_sentinel::trait<G>::child_sentinel;
              return child_sentinel(traits::graph<G>::child_end(node));
            }
          };
          VK_trait_case(G, 2, requires(typename typedef_node_ref::trait<G>::node_ref node){ node.child_end(); }){
            inline static decltype(auto) child_end(typename typedef_node_ref::trait<G>::node_ref node) noexcept {
              using child_sentinel = typename typedef_child_sentinel::trait<G>::child_sentinel;
              return child_sentinel(node.child_end());
            }
          };
          VK_trait_case(G, 1, requires(typename typedef_node_ref::trait<G>::node_ref node){ node->child_end(); }){
            inline static decltype(auto) child_end(typename typedef_node_ref::trait<G>::node_ref node) noexcept {
              using child_sentinel = typename typedef_child_sentinel::trait<G>::child_sentinel;
              return child_sentinel(node->child_end());
            }
          };
          VK_trait_no_fallback;
        };


        // child_edge_iterator(node_ref)
        struct function_child_edge_begin{
          VK_trait_init(3);
          VK_trait_case(G, 3, requires(typename typedef_node_ref::trait<G>::node_ref node){ traits::graph<G>::child_edge_begin(node); }){
            inline static decltype(auto) child_edge_begin(typename typedef_node_ref::trait<G>::node_ref node) noexcept {
              using child_edge_iterator = typename typedef_child_edge_iterator::trait<G>::child_edge_iterator;
              return child_edge_iterator(traits::graph<G>::child_edge_begin(node));
            }
          };
          VK_trait_case(G, 2, requires(typename typedef_node_ref::trait<G>::node_ref node){ node.child_edge_begin(); }){
            inline static decltype(auto) child_edge_begin(typename typedef_node_ref::trait<G>::node_ref node) noexcept {
              using child_edge_iterator = typename typedef_child_edge_iterator::trait<G>::child_edge_iterator;
              return child_edge_iterator(node.child_edge_begin());
            }
          };
          VK_trait_case(G, 1, requires(typename typedef_node_ref::trait<G>::node_ref node){ node->child_edge_begin(); }){
            inline static decltype(auto) child_edge_begin(typename typedef_node_ref::trait<G>::node_ref node) noexcept {
              using child_edge_iterator = typename typedef_child_edge_iterator::trait<G>::child_edge_iterator;
              return child_edge_iterator(node->child_edge_begin());
            }
          };
          VK_trait_no_fallback;
        };
        // child_edge_sentinel(node_ref)
        struct function_child_edge_end{
          VK_trait_init(3);
          VK_trait_case(G, 3, requires(typename typedef_node_ref::trait<G>::node_ref node){ traits::graph<G>::child_edge_end(node); }){
            inline static decltype(auto) child_edge_end(typename typedef_node_ref::trait<G>::node_ref node) noexcept {
              using child_edge_sentinel = typename typedef_child_edge_sentinel::trait<G>::child_edge_sentinel;
              return child_edge_sentinel(traits::graph<G>::child_edge_end(node));
            }
          };
          VK_trait_case(G, 2, requires(typename typedef_node_ref::trait<G>::node_ref node){ node.child_edge_end(); }){
            inline static decltype(auto) child_edge_end(typename typedef_node_ref::trait<G>::node_ref node) noexcept {
              using child_edge_sentinel = typename typedef_child_edge_sentinel::trait<G>::child_edge_sentinel;
              return child_edge_sentinel(node.child_edge_end());
            }
          };
          VK_trait_case(G, 1, requires(typename typedef_node_ref::trait<G>::node_ref node){ node->child_edge_end(); }){
            inline static decltype(auto) child_edge_end(typename typedef_node_ref::trait<G>::node_ref node) noexcept {
              using child_edge_sentinel = typename typedef_child_edge_sentinel::trait<G>::child_edge_sentinel;
              return child_edge_sentinel(node->child_edge_end());
            }
          };
          VK_trait_no_fallback;
        };




        // node_ref(const graph_type&)
        struct function_get_entry_node{
          VK_trait_init(2);
          VK_trait_case(G, 2, requires(const G& graph){ traits::graph<G>::get_entry_node(graph); }){
            inline static decltype(auto) get_entry_node(const G& graph) noexcept {
              using node_ref = typename typedef_node_ref::trait<G>::node_ref;
              return ref_cast<node_ref>(valkyrie::traits::graph<G>::get_entry_node(graph));
            }
          };
          VK_trait_case(G, 1, requires(const G& graph){ graph.entry_node(); }){
            inline static decltype(auto) get_entry_node(const G& graph) noexcept {
              using node_ref = typename typedef_node_ref::trait<G>::node_ref;
              return ref_cast<node_ref>(graph.entry_node());
            }
          };
          VK_trait_no_fallback;
        };

        // node_ref(edge_ref)
        struct function_edge_destination{
          VK_trait_init(2);
          VK_trait_case(G, 2, requires(typename typedef_edge_ref::trait<G>::edge_ref edge){ traits::graph<G>::edge_destination(edge); }){
            inline static decltype(auto) edge_destination(typename typedef_edge_ref::trait<G>::edge_ref edge) noexcept {
              using node_ref = typename typedef_node_ref::trait<G>::node_ref;
              return ref_cast<node_ref>(traits::graph<G>::edge_destination(edge));
            }
          };
          VK_trait_case(G, 1, requires(typename typedef_edge_ref::trait<G>::edge_ref edge){ edge.destination(); }){
            inline static decltype(auto) edge_destination(typename typedef_edge_ref::trait<G>::edge_ref edge) noexcept {
              using node_ref = typename typedef_node_ref::trait<G>::node_ref;
              return ref_cast<node_ref>(edge.destination());
            }
          };
          VK_trait_no_fallback;
        };

        // u64(const graph_type&)
        struct function_size{
          VK_trait_init(2);
          VK_trait_case(G, 2, requires(const G& graph){ traits::graph<G>::size(graph); }){
            inline static u64 size(const G& graph) noexcept {
              return valkyrie::traits::graph<G>::size(graph);
            }
          };
          VK_trait_case(G, 1, requires(const G& graph){ graph.size(); }){
            inline static u64 size(const G& graph) noexcept {
              return graph.size();
            }
          };
          VK_trait_no_fallback;
        };


        using trait_list = meta::list<typedef_node_type,
                                     typedef_node_ref,
                                     typedef_edge_type,
                                     typedef_edge_ref,
                                     typedef_nodes_iterator,
                                     typedef_nodes_sentinel,
                                     typedef_child_iterator,
                                     typedef_child_sentinel,
                                     typedef_child_edge_iterator,
                                     typedef_child_edge_sentinel,

                                     function_nodes_begin,
                                     function_nodes_end,
                                     function_child_begin,
                                     function_child_end,
                                     function_child_edge_begin,
                                     function_child_edge_end,
                                     function_edge_destination,
                                     function_size>;
      };
      struct numeric{
        struct bool_is_integral{
          VK_trait_init(1);
          VK_trait_case(T, 1, requires{ { traits::graph<T>::integral } -> same_as<bool>; }){
            VK_constant bool is_integral = traits::graph<T>::integral;
          };
          VK_trait_fallback(T){
            VK_constant bool is_integral = std::integral<T>;
          };
        };
        struct bool_is_floating_point{
          VK_trait_init(1);
          VK_trait_case(T, 1, requires{ { traits::graph<T>::floating_point } -> same_as<bool>; }){
            VK_constant bool is_floating_point = traits::graph<T>::floating_point;
          };
          VK_trait_fallback(T){
            VK_constant bool is_floating_point = std::floating_point<T>;
          };
        };
        struct bool_is_signed{
          VK_trait_init(3);
          VK_trait_case(T, 3, requires{ { traits::graph<T>::is_signed } -> same_as<bool>; }){
            VK_constant bool is_signed = traits::graph<T>::is_signed;
          };
          VK_trait_case(T, 2, bool_is_floating_point::trait<T>::is_floating_point){
            VK_constant bool is_signed = true;
          };
          VK_trait_case(T, 1, bool_is_integral::trait<T>::is_integral){
            VK_constant bool is_signed = std::signed_integral<T>;
          };
          VK_trait_no_fallback;
        };
        struct bool_is_bounded{
          VK_trait_init(2);
          VK_trait_case(T, 2, requires{ { traits::numeric<T>::is_bounded } -> same_as<bool>; }){
            VK_constant bool is_bounded = traits::numeric<T>::is_bounded;
          };
          VK_trait_case(T, 1, std::numeric_limits<T>::is_specialized){
            VK_constant bool is_bounded = std::numeric_limits<T>::is_bounded;
          };
          VK_trait_fallback(T){
            VK_constant bool is_bounded = true;
          };
        };
        struct bool_is_exact{
          VK_trait_init(2);
          VK_trait_case(T, 2, requires{ { traits::numeric<T>::is_exact } -> same_as<bool>; }){
            VK_constant bool is_exact = traits::numeric<T>::is_exact;
          };
          VK_trait_case(T, 1, std::numeric_limits<T>::is_specialized){
            VK_constant bool is_exact = std::numeric_limits<T>::is_exact;
          };
          VK_trait_fallback(T){
            VK_constant bool is_bounded = bool_is_integral::trait<T>::is_integral;
          };
        };
        struct size_bits{
          VK_trait_init(1);
          VK_trait_case(T, 1, requires{ traits::numeric<T>::bits; }){
            VK_constant decltype(auto) bits = traits::numeric<T>::bits;
          };
          VK_trait_fallback(T){
            VK_constant u64 bits = sizeof(T) * CHAR_BIT;
          };
        };

        using trait_list = meta::list<bool_is_integral,
                                     bool_is_floating_point,
                                     bool_is_signed,
                                     bool_is_bounded,
                                     bool_is_exact,
                                     size_bits>;
      };
      struct singleton{
        // const singleton_type&()

        struct function_instance;

        template <typename Trait, typename T>
        struct detect_is_fallback{
          using type = std::is_base_of<typename Trait::template fallback<T>, typename Trait::template trait<T>>;
        };

        struct bool_is_const{
          VK_trait_init(3);
          VK_trait_case(T, 3, requires{ { traits::singleton<T>::is_const } -> same_as<bool>; }){
            VK_constant bool is_const = traits::singleton<T>::is_const;
          };
          VK_trait_case(T, 2, requires{ { T::is_const } -> same_as<bool>; }){
            VK_constant bool is_const = T::is_const;
          };
          VK_trait_case(T, 1, !detect_is_fallback<function_instance, T>::type::value);
          VK_trait_fallback(T){
            VK_constant bool is_const = false;
          };
        };

        struct function_instance{
          VK_trait_init(3);
          VK_trait_case(T, 3, requires{ { traits::singleton<T>::instance } -> same_as<T>; }){
            VK_forceinline static decltype(auto) instance() noexcept {
              return traits::singleton<T>::instance;
            }
          };
          VK_trait_case(T, 2, requires{ { traits::singleton<T>::instance() } -> same_as<T>; }){
            VK_forceinline static decltype(auto) instance() noexcept {
              return traits::singleton<T>::instance();
            }
          };
          VK_trait_case(T, 1, requires{ { T::get() } -> same_as<T>; }){
            VK_forceinline static decltype(auto) instance() noexcept {
              return T::get();
            }
          };

          // A separate template is used so that the implementation of is_const can detect
          // whether or not the fallback implementation of instance() is in use.
          // note: This could also be accomplished with a static member variable called
          //       something along the lines of is_fallback, but that would introduce
          //       is_fallback as a static member variable of singleton_traits, which is
          //       undesirable. While this issue could be avoided by giving is_fallback
          //       private visibility and adding a friend declaration of bool_is_const to
          //       every case of the trait class, I find this a more elegant solution.
          template <typename T>
          struct fallback{
            // Fallback is NOT constexpr, to support all possible cases, as there is currently
            // no way to check whether or not any given expression can be evaluated in a
            // constant evaluated context
            VK_forceinline static decltype(auto) instance() noexcept {
              if constexpr ( bool_is_const::trait<T>::is_const ) {
                const static T instance_{};
                return static_cast<const T&>(instance_);
              }
              else {
                static T instance_{};
                return static_cast<T&>(instance_);
              }
            }
          };
          VK_trait_fallback(T) : fallback<T>{};
        };

        using trait_list = meta::list<function_instance,
                                      bool_is_const>;
      };

      template <typename T> requires(!singleton::detect_is_fallback<singleton::function_instance, T>::type::value)
      struct singleton::bool_is_const::trait<T, 1>{
        VK_constant bool is_const = std::is_const_v<remove_ref_t<decltype(singleton::function_instance::trait<T>::instance())>>;
      };

      struct klass{
        struct string_name{
          VK_trait_init(2);
          VK_trait_case(T, 2, requires{ traits::klass<T>::name; }){
            VK_constant typename deferred_string_view<T>::type name = traits::klass<T>::name;
          };
          VK_trait_case(T, 1, requires{ T::class_name; }){
            VK_constant typename deferred_string_view<T>::type name = T::class_name;
          };
          VK_trait_fallback(T){
            // TODO: automatic generation of type class_traits<T>::name using VK_function_name hackery
            VK_constant typename deferred_string_view<T>::type name = "automatic generation of class_traits<T>::name is not yet implemented";
          };
        };
        struct string_scoped_name{
          VK_trait_init(2);
          VK_trait_case(T, 2, requires{ traits::klass<T>::scoped_name; }){
            VK_constant typename deferred_string_view<T>::type scoped_name = traits::klass<T>::scoped_name;
          };
          VK_trait_case(T, 1, requires{ T::scoped_class_name; }){
            VK_constant typename deferred_string_view<T>::type scoped_name = T::scoped_class_name;
          };
          VK_trait_fallback(T){
            // TODO: automatic generation of class_traits<T>::scoped_name using VK_function_name hackery
            VK_constant typename deferred_string_view<T>::type scoped_name = "automatic generation of class_traits<T>::scoped_name is not yet implemented";
          };
        };

        struct uuid_id{
          VK_trait_init(2);
          VK_trait_case(T, 2, requires{ { traits::klass<T>::id } -> same_as<uuid>; }){
            VK_constant uuid id = traits::klass<T>::id;
          };
          VK_trait_case(T, 1, requires{ { T::class_id } -> same_as<uuid>; }){
            VK_constant uuid id = T::class_id;
          };
          VK_trait_no_fallback;
        };

        struct value_size{
          VK_trait_init(2);
          VK_trait_case(T, 2, requires{ { traits::klass<T>::size } -> extent_type; }){
            VK_constant auto size = traits::klass<T>::size;
            VK_constant bool has_variable_size = same_as<decltype(size), dynamic_t>;
          };
          VK_trait_case(T, 1, std::derived_from<T, variable_size>){
            VK_constant auto size = dynamic_t{};
            VK_constant bool has_variable_size = true;
          };
          VK_trait_fallback(T){
            VK_constant u64 size               = sizeof(T);
            VK_constant bool has_variable_size = false;
          };
        };

        // u64(const klass&)
        struct function_object_size{
          VK_trait_init(4);
          VK_trait_case(T, 4, requires(const T& obj){ { traits::klass<T>::object_size(obj) } -> convertible_to<u64>; }){
            VK_forceinline static u64 object_size(const T& obj) noexcept {
              return valkyrie::traits::klass<T>::object_size(obj);
            }
          };
          VK_trait_case(T, 3, requires(const T& obj){ { T::object_size(obj) } -> convertible_to<u64>; }){
            VK_forceinline static u64 object_size(const T& obj) noexcept {
              return T::object_size(obj);
            }
          };
          VK_trait_case(T, 2, requires(const T& obj){ { obj.object_size() } -> convertible_to<u64>; }){
            VK_forceinline static u64 object_size(const T& obj) noexcept {
              return obj.object_size();
            }
          };
          VK_trait_case(T, 1, !value_size::trait<T>::has_variable_size ) {
            VK_forceinline static u64 object_size(const T&) noexcept {
              return value_size::trait<T>::size;
            }
          };
          VK_trait_no_fallback;
        };

        struct integer_alignment{
          VK_trait_init(1);
          VK_trait_case(T, 1, requires{ traits::klass<T>::alignment; }){
            static_assert(numeric::bool_is_integral::trait<T>::is_integral,
                          "traits::klass<T>::alignment must be an integral value convertible to u64");
            VK_constant u64 alignment = traits::klass<T>::alignment;
          };
          VK_trait_fallback(T){
            VK_constant u64 alignment = alignof(T);
          };
        };

        struct typedef_supertype{
          VK_trait_init(2);
          VK_trait_case(T, 2, requires{ typename traits::klass<T>::supertype; }){
            using supertype = typename traits::klass<T>::supertype;
          };
          VK_trait_case(T, 1, requires{ typename T::supertype; }){
            using supertype = typename T::supertype;
          };
          VK_trait_fallback(T){
            using supertype = root;
          };
        };
        struct bool_is_polymorphic{
          VK_trait_init(2);
          VK_trait_case(T, 2, requires{ { traits::klass<T>::is_polymorphic } -> same_as<bool>; }){
            VK_constant bool is_polymorphic = traits::klass<T>::is_polymorphic;
          };
          VK_trait_case(T, 1, not_same_as<root, typename typedef_supertype::trait<T>::supertype>){
            VK_constant bool is_polymorphic = trait<typename typedef_supertype::trait<T>::supertype>::is_polymorphic;
          };
          VK_trait_fallback(T){
            VK_constant bool is_polymorphic = std::is_polymorphic_v<T>;
          };
        };
        struct bool_is_abstract{
          VK_trait_init(1);
          VK_trait_case(T, 1, requires{ { traits::klass<T>::is_abstract } -> same_as<bool>; }){
            VK_constant bool is_abstract = traits::klass<T>::is_abstract;
          };
          VK_trait_fallback(T){
            VK_constant bool is_abstract = std::is_abstract_v<T>;
          };
        };

        using trait_list = meta::list<string_name,
                                     string_scoped_name,
                                     uuid_id,
                                     function_object_size,
                                     integer_alignment,
                                     typedef_supertype,
                                     bool_is_polymorphic,
                                     bool_is_abstract>;
      };

      struct hash{
        struct function_call64{
          VK_trait_init(5);
          VK_trait_case(T, 5, requires(const T& val){ { traits::hash<T>::call64(val) } -> convertible_to<u64>; }){
            VK_constant u64 call64(in_param_t<T> val) noexcept {
              return traits::hash<T>::call64(val);
            }
          };
          VK_trait_case(T, 4, requires(const T& val){ { traits::hash<T>::call(val) } -> same_as<u64>; }){
            VK_constant u64 call64(in_param_t<T> val) noexcept {
              return traits::hash<T>::call(val);
            }
          };
          VK_trait_case(T, 3, requires(const T& val){ { val.hash64() } -> convertible_to<u64>; }){
            VK_constant u64 call64(in_param_t<T> val) noexcept {
              return val.hash64();
            }
          };
          VK_trait_case(T, 2, requires(const T& val){ { val.hash() } -> same_as<u64>; }){
            VK_constant u64 call64(in_param_t<T> val) noexcept {
              return val.hash();
            }
          };
          VK_trait_case(T, 1, requires(const T& val, std::hash<T> h){ { h(val) } -> convertible_to<u64>; }){
            VK_constant u64 call64(in_param_t<T> val) noexcept {
              return std::hash<T>{}(val);
            }
          };
          VK_trait_no_fallback;
        };
        struct function_call32{
          VK_trait_init(5);
          VK_trait_case(T, 5, requires(const T& val){ { traits::hash<T>::call32(val) } -> convertible_to<u32>; }){
            VK_constant u32 call32(in_param_t<T> val) noexcept {
              return traits::hash<T>::call32(val);
            }
          };
          VK_trait_case(T, 4, requires(const T& val){ { traits::hash<T>::call(val) } -> same_as<u32>; }){
            VK_constant u32 call32(in_param_t<T> val) noexcept {
              return traits::hash<T>::call(val);
            }
          };
          VK_trait_case(T, 3, requires(const T& val){ { val.hash32() } -> convertible_to<u32>; }){
            VK_constant u32 call32(in_param_t<T> val) noexcept {
              return val.hash32();
            }
          };
          VK_trait_case(T, 2, requires(const T& val){ { val.hash() } -> same_as<u32>; }){
            VK_constant u32 call32(in_param_t<T> val) noexcept {
              return val.hash();
            }
          };
          VK_trait_case(T, 1, requires(const T& val){ function_call64::trait<T>::call64(val); }){
            VK_constant u32 call32(in_param_t<T> val) noexcept {
              u64 tmp = function_call64::trait<T>::call64(val);
              return static_cast<u32>((tmp << 5) + tmp + (tmp >> 32));
            }
          };
          VK_trait_no_fallback;
        };

        using trait_list = meta::set<function_call64,
                                     function_call32>;
      };


      // TODO: Implement string traits_v2
      struct string{

        struct typedef_char_type{};


        struct bool_is_null_terminated{};


        // char_type*(const string&)
        struct function_data{};
        // u64(const string&)
        struct function_length{};

        using trait_list = meta::set<typedef_char_type,
                                     bool_is_null_terminated,
                                     function_data,
                                     function_length>;
      };
      struct iterator{
        struct typedef_pointer{
          VK_trait_init(3);
          VK_trait_case(T, 3, requires{ typename traits::iterator<T>::pointer; }){
            using pointer = typename traits::iterator<T>::pointer;
          };
          VK_trait_case(T, 2, requires{ typename T::pointer; }){
            using pointer = typename T::pointer;
          };
          VK_trait_case(T, 1, requires{ typename std::iterator_traits<T>::pointer; }){
            using pointer = typename std::iterator_traits<T>::pointer;
          };
          VK_trait_no_fallback;
        };
        struct typedef_reference{
          VK_trait_init(3);
          VK_trait_case(T, 3, requires{ typename traits::iterator<T>::reference; }){
            using reference = typename traits::iterator<T>::reference;
          };
          VK_trait_case(T, 2, requires{ typename T::reference; }){
            using reference = typename T::reference;
          };
          VK_trait_case(T, 1, requires{ typename std::iterator_traits<T>::reference; }){
            using reference = typename std::iterator_traits<T>::reference;
          };
          VK_trait_no_fallback;
        };
        struct typedef_value_type{
          VK_trait_init(3);
          VK_trait_case(T, 3, requires{ typename traits::iterator<T>::value_type; }){
            using value_type = typename traits::iterator<T>::value_type;
          };
          VK_trait_case(T, 2, requires{ typename T::value_type; }){
            using value_type = typename T::value_type;
          };
          VK_trait_case(T, 1, requires{ typename std::iterator_traits<T>::value_type; }){
            using value_type = typename std::iterator_traits<T>::value_type;
          };
          VK_trait_no_fallback;
        };
        struct typedef_category{
          VK_trait_init(5);
          VK_trait_case(T, 5, requires{ typename traits::iterator<T>::category; }){
            using category = typename traits::iterator<T>::category;
          };
          VK_trait_case(T, 4, requires{ typename T::iterator_concept; }){
            using category = typename T::iterator_concept;
          };
          VK_trait_case(T, 3, requires{ typename T::iterator_category; }){
            using category = typename T::iterator_category;
          };
          VK_trait_case(T, 2, requires{ typename std::iterator_traits<T>::iterator_concept; }){
            using category = typename std::iterator_traits<T>::iterator_concept;
          };
          VK_trait_case(T, 1, requires{ typename std::iterator_traits<T>::iterator_category; }){
            using category = typename std::iterator_traits<T>::iterator_category;
          };
          VK_trait_no_fallback;
        };
        struct typedef_difference_type{
          VK_trait_init(3);
          VK_trait_case(T, 3, requires{ typename traits::iterator<T>::difference_type; }){
            using difference_type = typename traits::iterator<T>::difference_type;
          };
          VK_trait_case(T, 2, requires{ typename T::difference_type; }){
            using difference_type = typename T::difference_type;
          };
          VK_trait_case(T, 1, requires{ typename std::iterator_traits<T>::difference_type; }){
            using difference_type = typename std::iterator_traits<T>::difference_type;
          };
          VK_trait_fallback(T){
            using difference_type = i64;
          };
        };

        using trait_list = meta::set<typedef_pointer,
                                     typedef_reference,
                                     typedef_value_type,
                                     typedef_category,
                                     typedef_difference_type>;
      };
      // TODO: Implement allocator traits_v2

      struct allocator{

        struct typedef_allocator_type{
          VK_trait_init(3);
          VK_trait_case(A, 3, requires{ typename traits::allocator<A>::allocator_type; }){
            using allocator_type = typename traits::allocator<A>::allocator_type;
          };
          VK_trait_case(A, 2, requires{ typename A::template rebind<char>::other; }){
            using allocator_type = std::decay_t<typename A::template rebind<char>::other>;
          };
          VK_trait_case_match((template <typename,typename...> typename Alloc, typename T, typename ...U),
                              Alloc<T, U...>,
                              1, requires{ typename Alloc<T, U...>::value_type; }){
            using allocator_type = std::decay_t<typename Alloc<T, U...>::value_type>;
          };
          VK_trait_fallback(A){
            using allocator_type = std::decay_t<A&>;
          };
        };

        template <typename A>
        using allocator_type_t = typename typedef_allocator_type::trait<A>::allocator_type;

        struct function_allocate_node{
          VK_trait_init(4);
          VK_trait_case(A, 4, requires(allocator_type_t<A>& alloc, u64 n){ { traits::allocator<A>::allocate_node(alloc, n, n) } -> exact_same_as<void*>; }){
            VK_nodiscard inline static void* allocate_node(allocator_type_t<A>& alloc, u64 size, u64 align) noexcept {
              return traits::allocator<A>::allocate_node(alloc, size, align);
            }
          };
          VK_trait_case(A, 3, requires(allocator_type_t<A>& alloc, u64 n){ { alloc.allocate_node(n, n) } -> exact_same_as<void*>; }){
            VK_nodiscard inline static void* allocate_node(allocator_type_t<A>& alloc, u64 size, u64 align) noexcept {
              return alloc.allocate_node(size, align);
            }
          };
          VK_trait_case(A, 2, requires(allocator_type_t<A>& alloc, u64 n){ static_cast<void*>(alloc.allocate(n, n)); }){
            VK_nodiscard inline static void* allocate_node(allocator_type_t<A>& alloc, u64 size, u64 align) noexcept {
              return static_cast<void*>(alloc.allocate(size, align));
            }
          };
          VK_trait_case(A, 1, requires(allocator_type_t<A>& alloc, u64 n){ static_cast<void*>(alloc.allocate(n)); }){
            VK_nodiscard inline static void* allocate_node(allocator_type_t<A>& alloc, u64 size, u64) noexcept {
              return static_cast<void*>(alloc.allocate(size));
            }
          };
          VK_trait_no_fallback;
        };
        struct function_deallocate_node{
          VK_trait_init(4);
          VK_trait_case(A, 4, requires(allocator_type_t<A>& alloc, void* p, u64 n){ traits::allocator<A>::deallocate_node(alloc, p, n, n); }){
            inline static void deallocate_node(allocator_type_t<A>& alloc, void* p, u64 size, u64 align) noexcept {
              traits::allocator<A>::deallocate_node(alloc, p, size, align);
            }
          };
          VK_trait_case(A, 3, requires(allocator_type_t<A>& alloc, void* p, u64 n){ alloc.deallocate_node(p, n, n); }){
            inline static void deallocate_node(allocator_type_t<A>& alloc, void* p, u64 size, u64 align) noexcept {
              alloc.deallocate_node(p, size, align);
            }
          };
          VK_trait_case(A, 2, requires(allocator_type_t<A>& alloc, char* p, u64 n){ alloc.deallocate(p, n, n); }){
            inline static void deallocate_node(allocator_type_t<A>& alloc, void* p, u64 size, u64 align) noexcept {
              alloc.deallocate(static_cast<char*>(p), size, align);
            }
          };
          VK_trait_case(A, 1, requires(allocator_type_t<A>& alloc, char* p, u64 n){ alloc.deallocate(p, n); }){
            inline static void deallocate_node(allocator_type_t<A>& alloc, void* p, u64 size, u64) noexcept {
              alloc.deallocate(static_cast<char*>(p), size);
            }
          };
          VK_trait_no_fallback;
        };
        struct function_allocate_array{
          VK_trait_init(2);
          VK_trait_case(A, 2, requires(allocator_type_t<A>& alloc, u64 n){ { traits::allocator<A>::allocate_array(alloc, n, n, n) } -> exact_same_as<void*>; }){
            inline static void* allocate_array(allocator_type_t<A>& alloc, u64 count, u64 size, u64 align) noexcept {
              return traits::allocator<A>::allocate_array(alloc, count, size, align);
            }
          };
          VK_trait_case(A, 1, requires(allocator_type_t<A>& alloc, u64 n){ { alloc.allocate_array(n, n, n) } -> exact_same_as<void*>; }){
            inline static void* allocate_array(allocator_type_t<A>& alloc, u64 count, u64 size, u64 align) noexcept {
              return alloc.allocate_array(count, size, align);
            }
          };
          VK_trait_fallback(A){
            inline static void*
            allocate_array(allocator_type_t<A>& alloc, u64 count, u64 size, u64 align) noexcept {
              return function_allocate_node::trait<A>::allocate_node(alloc, count * size, align);
            }
          };
        };
        struct function_deallocate_array{
          VK_trait_init(2);
          VK_trait_case(A, 2, requires(allocator_type_t<A>& alloc, void* p, u64 n){ traits::allocator<A>::deallocate_array(alloc, p, n, n, n); }){
            inline static void deallocate_array(allocator_type_t<A>& alloc, void* p, u64 count, u64 size, u64 align) noexcept {
              traits::allocator<A>::deallocate_array(alloc, p, count, size, align);
            }
          };
          VK_trait_case(A, 1, requires(allocator_type_t<A>& alloc, void* p, u64 n){ alloc.deallocate_array(p, n, n, n); }){
            inline static void
            deallocate_array(allocator_type_t<A>& alloc, void* p, u64 count, u64 size, u64 align) noexcept {
              alloc.deallocate_array(p, count, size, align);
            }
          };
          VK_trait_fallback(A){
            inline static void
            deallocate_array(allocator_type_t<A>& alloc, void* p, u64 count, u64 size, u64 align) noexcept {
              function_deallocate_node::trait<A>::deallocate_node(alloc, p, count * size, align);
            }
          };
        };
        struct function_try_allocate_node{
          VK_trait_init(2);
          VK_trait_case(A, 2, requires(allocator_type_t<A>& alloc, u64 n){ { traits::allocator<A>::try_allocate_node(alloc, n, n) } noexcept -> exact_same_as<void*>; }){
            inline static void* try_allocate_node(allocator_type_t<A>& alloc, u64 size, u64 align) noexcept {
              return traits::allocator<A>::try_allocate_node(alloc, size, align);
            }
          };
          VK_trait_case(A, 1, requires(allocator_type_t<A>& alloc, u64 n){ { alloc.try_allocate_node(n, n) } noexcept -> exact_same_as<void*>; }){
            inline static void* try_allocate_node(allocator_type_t<A>& alloc, u64 size, u64 align) noexcept {
              return alloc.try_allocate_node(size, align);
            }
          };
          VK_trait_fallback(A){};
        };
        struct function_try_deallocate_node{
          VK_trait_init(2);
          VK_trait_case(A, 2, requires(allocator_type_t<A>& alloc, void* p, u64 n){ { traits::allocator<A>::try_deallocate_node(alloc, p, n, n) } noexcept -> exact_same_as<bool>; }){
            inline static bool try_deallocate_node(allocator_type_t<A>& alloc, void* ptr, u64 size, u64 align) noexcept {
              return traits::allocator<A>::try_deallocate_node(alloc, ptr, size, align);
            }
          };
          VK_trait_case(A, 1, requires(allocator_type_t<A>& alloc, void* p, u64 n){ { alloc.try_deallocate_node(p, n, n) } noexcept -> exact_same_as<bool>; }){
            inline static bool try_deallocate_node(allocator_type_t<A>& alloc, void* ptr, u64 size, u64 align) noexcept {
              return alloc.try_deallocate_node(ptr, size, align);
            }
          };
          VK_trait_fallback(A){};
        };

        struct bool_is_composable{
          VK_trait_init(1);
          VK_trait_case(A, 1, requires{
                &function_try_allocate_node::trait<A>::try_allocate_node;
                &function_try_deallocate_node::trait<A>::try_deallocate_node;
          }){
            VK_constant bool is_composable = true;
          };
          VK_trait_fallback(A){
            VK_constant bool is_composable = false;
          };
        };

        struct function_try_allocate_array{
          VK_trait_init(3);
          VK_trait_case(A, 3, !bool_is_composable::trait<A>::is_composable){}; // not provided if try_allocate_node and try_deallocate_node are not both provided.
          VK_trait_case(A, 2, requires(allocator_type_t<A>& alloc, u64 n){ { traits::allocator<A>::try_allocate_array(alloc, n, n, n) } noexcept -> exact_same_as<void*>; }){
            inline static void* try_allocate_array(allocator_type_t<A>& alloc, u64 count, u64 size, u64 align) noexcept {
              return traits::allocator<A>::try_allocate_array(alloc, count, size, align);
            }
          };
          VK_trait_case(A, 1, requires(allocator_type_t<A>& alloc, u64 n){ { alloc.try_allocate_array(n, n, n) } noexcept -> exact_same_as<void*>; }){
            inline static void* try_allocate_array(allocator_type_t<A>& alloc, u64 count, u64 size, u64 align) noexcept {
              return alloc.try_allocate_array(count, size, align);
            }
          };
          VK_trait_fallback(A){
            inline static void* try_allocate_array(allocator_type_t<A>& alloc, u64 count, u64 size, u64 align) noexcept {
              return function_try_allocate_node::trait<A>::try_allocate_node(alloc, count * size, align);
            }
          };
        };
        struct function_try_deallocate_array{
          VK_trait_init(3);
          VK_trait_case(A, 3, !bool_is_composable::trait<A>::is_composable){}; // not provided if try_allocate_node and try_deallocate_node are not both provided.
          VK_trait_case(A, 2, requires(allocator_type_t<A>& alloc, void* p, u64 n){ { traits::allocator<A>::try_deallocate_array(alloc, p, n, n, n) } noexcept -> exact_same_as<bool>; }){
            inline static bool try_deallocate_array(allocator_type_t<A>& alloc, void* ptr, u64 count, u64 size, u64 align) noexcept {
              return traits::allocator<A>::try_deallocate_array(alloc, ptr, count, size, align);
            }
          };
          VK_trait_case(A, 1, requires(allocator_type_t<A>& alloc, void* p, u64 n){ { alloc.try_deallocate_array(p, n, n, n) } noexcept -> exact_same_as<bool>; }){
            inline static bool try_deallocate_array(allocator_type_t<A>& alloc, void* ptr, u64 count, u64 size, u64 align) noexcept {
              return alloc.try_deallocate_array(ptr, count, size, align);
            }
          };
          VK_trait_fallback(A){
            inline static bool try_deallocate_array(allocator_type_t<A>& alloc, void* ptr, u64 count, u64 size, u64 align) noexcept {
              return function_try_deallocate_node::trait<A>::try_deallocate_node(alloc, ptr, count * size, align);
            }
          };
        };

        struct function_max_node_size{
          VK_trait_init(3);
          VK_trait_case(A, 3, requires(const allocator_type_t<A>& alloc){ { traits::allocator<A>::max_node_size(alloc) } -> convertible_to<u64>; }){
            inline static u64 max_node_size(const allocator_type_t<A>& alloc) noexcept {
              return traits::allocator<A>::max_node_size(alloc);
            }
          };
          VK_trait_case(A, 2, requires(const allocator_type_t<A>& alloc){ { traits::allocator<A>::max_node_size() } -> convertible_to<u64>; }){
            inline static u64 max_node_size(const allocator_type_t<A>&) noexcept {
              return traits::allocator<A>::max_node_size();
            }
          };
          VK_trait_case(A, 1, requires(const allocator_type_t<A>& alloc){ { alloc.max_node_size() } -> convertible_to<u64>; }){
            inline static u64 max_node_size(const allocator_type_t<A>& alloc) noexcept {
              return alloc.max_node_size();
            }
          };
          VK_trait_fallback(A){
            inline static u64 max_node_size(const allocator_type_t<A>&) noexcept {
              return static_cast<u64>(-1);
            }
          };
        };
        struct function_max_array_size{
          VK_trait_init(3);
          VK_trait_case(A, 3, requires(const allocator_type_t<A>& alloc){ { traits::allocator<A>::max_array_size(alloc) } -> convertible_to<u64>; }){
            inline static u64 max_array_size(const allocator_type_t<A>& alloc) noexcept {
              return traits::allocator<A>::max_array_size(alloc);
            }
          };
          VK_trait_case(A, 2, requires(const allocator_type_t<A>& alloc){ { traits::allocator<A>::max_array_size() } -> convertible_to<u64>; }){
            inline static u64 max_array_size(const allocator_type_t<A>&) noexcept {
              return traits::allocator<A>::max_array_size();
            }
          };
          VK_trait_case(A, 1, requires(const allocator_type_t<A>& alloc){ { alloc.max_array_size() } -> convertible_to<u64>; }){
            inline static u64 max_array_size(const allocator_type_t<A>& alloc) noexcept {
              return alloc.max_array_size();
            }
          };
          VK_trait_fallback(A){
            inline static u64 max_array_size(const allocator_type_t<A>& alloc) noexcept {
              return function_max_node_size::trait<A>::max_node_size(alloc);
            }
          };
        };
        struct function_max_alignment{
          VK_trait_init(1);
          VK_trait_case(A, 3, requires(const allocator_type_t<A>& alloc){ { traits::allocator<A>::max_alignment(alloc) } -> convertible_to<u64>; }){
            inline static u64 max_alignment(const allocator_type_t<A>& alloc) noexcept {
              return traits::allocator<A>::max_alignment(alloc);
            }
          };
          VK_trait_case(A, 2, requires(const allocator_type_t<A>& alloc){ { traits::allocator<A>::max_alignment() } -> convertible_to<u64>; }){
            inline static u64 max_alignment(const allocator_type_t<A>&) noexcept {
              return traits::allocator<A>::max_alignment();
            }
          };
          VK_trait_case(A, 1, requires(const allocator_type_t<A>& alloc){
            { alloc.max_alignment() } -> convertible_to<u64>;
          }){
            inline static u64 max_alignment(const allocator_type_t<A>& alloc) noexcept {
              return alloc.max_alignment();
            }
          };
          VK_trait_fallback(A){
            inline static u64 max_alignment(const allocator_type_t<A>&) noexcept {
              return alignof(std::max_align_t);
            }
          };
        };

        struct bool_is_stateful{
          VK_trait_init(4);
          VK_trait_case(A, 4, requires{ { traits::allocator<A>::is_stateful } -> same_as<bool>; }){
            VK_constant bool is_stateful = traits::allocator<A>::is_stateful;
          };
          VK_trait_case(A, 3, requires{ { A::is_stateful } -> same_as<bool>; }){
            VK_constant bool is_stateful = A::is_stateful;
          };
          VK_trait_case(A, 2, std::is_empty_v<allocator_type_t<A>> && std::is_default_constructible_v<allocator_type_t<A>>){
            VK_constant bool is_stateful = false;
          };
          VK_trait_case(A, 1, !std::is_empty_v<allocator_type_t<A>>){
            VK_constant bool is_stateful = true;
          };
          VK_trait_no_fallback;
        };
        struct bool_is_shared{
          VK_trait_init(3);
          VK_trait_case(A, 3, !bool_is_stateful::trait<A>::is_stateful){
            VK_constant bool is_shared = false;
          };
          VK_trait_case(A, 2, requires{ { traits::allocator<A>::is_shared } -> exact_same_as<bool>; }){
            VK_constant bool is_shared = traits::allocator<A>::is_shared;
          };
          VK_trait_case(A, 1, requires{ { A::is_shared } -> exact_same_as<bool>; }){
            VK_constant bool is_shared = A::is_shared;
          };
          VK_trait_fallback(A){
            VK_constant bool is_shared = false;
          };
        };
        struct bool_is_thread_safe{
          VK_trait_init(2);
          VK_trait_case(A, 2, requires{ { traits::allocator<A>::is_thread_safe } -> same_as<bool>; }){
            VK_constant bool is_thread_safe = traits::allocator<A>::is_thread_safe;
          };
          VK_trait_case(A, 1, requires{ { A::is_thread_safe } -> same_as<bool>; }){
            VK_constant bool is_thread_safe = A::is_thread_safe;
          };
          VK_trait_fallback(A){
            VK_constant bool is_thread_safe = !bool_is_stateful::trait<A>::is_stateful;
          };
        };


        struct bool_propagate_on_container_copy_assignment{
          VK_trait_init(3);
          VK_trait_case(A, 3, requires{ { traits::allocator<A>::propagate_on_container_copy_assignment } -> same_as<bool>; }){
            VK_constant bool propagate_on_container_copy_assignment = traits::allocator<A>::propagate_on_container_copy_assignment;
          };
          VK_trait_case(A, 2, requires{ { A::propagate_on_container_copy_assignment } -> same_as<bool>; }){
            VK_constant bool propagate_on_container_copy_assignment = A::propagate_on_container_copy_assignment;
          };
          VK_trait_case(A, 1, requires{ { A::propagate_on_container_copy_assignment::value } -> same_as<bool>; }){
            VK_constant bool propagate_on_container_copy_assignment = A::propagate_on_container_copy_assignment::value;
          };
          VK_trait_fallback(A){
            VK_constant bool propagate_on_container_copy_assignment = bool_is_stateful::trait<A>::is_stateful;
          };
        };
        struct bool_propagate_on_container_move_assignment{
          VK_trait_init(3);
          VK_trait_case(A, 3, requires{ { traits::allocator<A>::propagate_on_container_move_assignment } -> same_as<bool>; }){
            VK_constant bool propagate_on_container_move_assignment = traits::allocator<A>::propagate_on_container_move_assignment;
          };
          VK_trait_case(A, 2, requires{ { A::propagate_on_container_move_assignment } -> same_as<bool>; }){
            VK_constant bool propagate_on_container_move_assignment = A::propagate_on_container_move_assignment;
          };
          VK_trait_case(A, 1, requires{ { A::propagate_on_container_move_assignment::value } -> same_as<bool>; }){
            VK_constant bool propagate_on_container_move_assignment = A::propagate_on_container_move_assignment::value;
          };
          VK_trait_fallback(A){
            VK_constant bool propagate_on_container_move_assignment = bool_is_stateful::trait<A>::is_stateful;
          };
        };
        struct bool_propagate_on_container_swap{
          VK_trait_init(3);
          VK_trait_case(A, 3, requires{ { traits::allocator<A>::propagate_on_container_swap } -> same_as<bool>; }){
            VK_constant bool propagate_on_container_swap = traits::allocator<A>::propagate_on_container_swap;
          };
          VK_trait_case(A, 2, requires{ { A::propagate_on_container_swap } -> same_as<bool>; }){
            VK_constant bool propagate_on_container_swap = A::propagate_on_container_swap;
          };
          VK_trait_case(A, 1, requires{ { A::propagate_on_container_swap::value } -> same_as<bool>; }){
            VK_constant bool propagate_on_container_swap = A::propagate_on_container_swap::value;
          };
          VK_trait_fallback(A){
            VK_constant bool propagate_on_container_swap = bool_is_stateful::trait<A>::is_stateful;
          };
        };

        struct function_select_on_container_copy_construction{
          VK_trait_init(2);
          VK_trait_case(A, 2, requires(const allocator_type_t<A>& alloc){
            { traits::allocator<A>::select_on_container_copy_construction(alloc) } -> same_as<allocator_type_t<A>>;
          }){
            inline static allocator_type_t<A> select_on_container_copy_construction(const allocator_type_t<A>& alloc) noexcept {
              return traits::allocator<A>::select_on_container_copy_construction(alloc);
            }
          };
          VK_trait_case(A, 1, requires(const allocator_type_t<A>& alloc){
            { alloc.select_on_container_copy_construction() } -> same_as<allocator_type_t<A>>;
          }){
            inline static allocator_type_t<A> select_on_container_copy_construction(const allocator_type_t<A>& alloc) noexcept {
              return alloc.select_on_container_copy_construction();
            }
          };
          VK_trait_fallback(A){
            inline static allocator_type_t<A> select_on_container_copy_construction(const allocator_type_t<A>& alloc) noexcept {
              return alloc;
            }
          };
        };


        using trait_list = meta::set<
            typedef_allocator_type,

            function_allocate_node,
            function_deallocate_node,
            function_allocate_array,
            function_deallocate_array,

            function_try_allocate_node,
            function_try_deallocate_node,
            bool_is_composable,
            function_try_allocate_array,
            function_try_deallocate_array,

            function_max_node_size,
            function_max_array_size,
            function_max_alignment,

            bool_is_stateful,
            bool_is_shared,
            bool_is_thread_safe,

            bool_propagate_on_container_copy_assignment,
            bool_propagate_on_container_move_assignment,
            bool_propagate_on_container_swap,
            function_select_on_container_copy_construction>;
      };

      // TODO: Implement container traits_v2
      struct container{

        struct typedef_element_type{};

        using trait_list = meta::set<typedef_element_type>;
      };
      // TODO: Implement agent traits_v2
      struct agent{
        struct typeset_consumes{};
        struct typeset_produces{};

        using trait_list = meta::set<typeset_consumes,
                                     typeset_produces>;
      };
      // TODO: Implement message traits_v2
      struct message{
        struct value_size{};

        using trait_list = meta::set<value_size>;
      };

      /*struct serializable{
        struct function_do_
      };*/


      template <typename SpecificTrait, typename T>
      concept satisfied_by = std::is_empty_v<typename SpecificTrait::template trait<T>>;

      struct success{};
      struct no_more_traits{};

      template <typename T, typename TraitList>
      struct do_trait_lookup;

      template <typename T, typename ...Traits>
      struct do_trait_lookup<T, meta::list<Traits...>> : Traits::template trait<T>...{};

      /*template <typename T, typelist List>
      struct instantiate{
        using type = success;
      };
      template <typename T, nonempty_typelist List>
      struct instantiate<T, List> {
        using type = typename meta::front_t<List>::template trait<T>;
      };

      template <typename T, typelist List>
      struct next_trait{
        using type = no_more_traits;
      };
      template <typename T, nonempty_typelist List>
      struct next_trait<T, List>{
        using type = do_trait_lookup<T, meta::pop_front_t<List>>;
      };

      template <typename T, typename TraitList>
      struct do_trait_lookup : instantiate<T, TraitList>::type,
                               next_trait<T, TraitList>::type{};*/

      template <typename List>
      struct trait_not_satisfied{ using failed_trait = meta::front_t<List>; };
      template <>
      struct trait_not_satisfied<meta::list<>>{ };

      template <typename T, typename TraitList>
      struct do_trait_concept_check;

      template <typename T, not_same_as<meta::empty_list> TraitList>
      requires(satisfied_by<meta::front_t<TraitList>, T>)
      struct do_trait_concept_check<T, TraitList> : do_trait_concept_check<T, meta::pop_front_t<TraitList>>{};

      template <typename T, typename TraitList>
      struct do_trait_concept_check : trait_not_satisfied<TraitList>{};


      template <typename TraitList>
      struct each_trait;
      template <typename ...Trait>
      struct each_trait<meta::list<Trait...>>{
        struct success{};
        template <typename T>
        struct failure{
          using failed_trait = T;
        };


        template <typename T>
        struct inherit : Trait::template trait<T> ... {};




        template <typename T, typename ...List>
        struct satisfied_;
        template <typename T, satisfied_by<T> Head, typename ...Tail>
        struct satisfied_<T, Head, Tail...> : satisfied_<T, Tail...>{};
        template <typename T, typename Head, typename ...Tail>
        struct satisfied_<T, Head, Tail...> : failure<Head>{};
        template <typename T>
        struct satisfied_<T>;

        template <typename T>
        using satisfied = satisfied_<T, Trait...>;
      };

      template <typename T, typename Trait>
      struct traits_impl : do_trait_lookup<T, typename Trait::trait_list>{};

      template <typename T, typename Trait>
      concept satisfies_trait = !requires{
        typename do_trait_concept_check<T, typename Trait::trait_list>::failed_trait;
      };
    }
  }

  inline namespace concepts{
    template <typename T>
    concept range_c = traits::detail::satisfies_trait<T, traits::detail::range>;
    template <typename T>
    concept pointer_c = traits::detail::satisfies_trait<T, traits::detail::pointer>;
    template <typename T>
    concept enumerator_c = traits::detail::satisfies_trait<T, traits::detail::enumerator>;
    template <typename T>
    concept graph_c = traits::detail::satisfies_trait<T, traits::detail::graph>;
    template <typename T>
    concept numeric_c = traits::detail::satisfies_trait<T, traits::detail::numeric>;
    template <typename T>
    concept integral_c = numeric_c<T> && traits::detail::numeric::bool_is_integral::trait<T>::is_integral;
    template <typename T>
    concept signed_integral_c = integral_c<T> && traits::detail::numeric::bool_is_signed::trait<T>::is_signed;
    template <typename T>
    concept unsigned_integral_c = integral_c<T> && !traits::detail::numeric::bool_is_signed::trait<T>::is_signed;
    template <typename T>
    concept floating_point_c = numeric_c<T> && traits::detail::numeric::bool_is_floating_point::trait<T>::is_floating_point;
    template <typename T>
    concept allocator_c = traits::detail::satisfies_trait<T, traits::detail::allocator>;
    template <typename T>
    concept iterator_c  = traits::detail::satisfies_trait<T, traits::detail::iterator>;
    template <typename T>
    concept string_c    = traits::detail::satisfies_trait<T, traits::detail::string>;
    template <typename T>
    concept singleton_c = traits::detail::satisfies_trait<T, traits::detail::singleton>;
    template <typename T>
    concept hashable_c = traits::detail::satisfies_trait<T, traits::detail::hash>;

    template <typename T>
    concept bitwise_movable = !requires{
      typename traits::bitwise_movable<T>::invalid_specialization;
    };
    template <typename From, typename To>
    concept bit_castable_to = sizeof(From) == sizeof(To) && bitwise_movable<To> && bitwise_movable<From>;
  }


  template <range_c R>
  using range_traits     = traits::detail::traits_impl<R, traits::detail::range>;
  template <pointer_c P>
  using pointer_traits   = traits::detail::traits_impl<P, traits::detail::pointer>;
  template <enumerator_c E>
  using enum_traits      = traits::detail::traits_impl<E, traits::detail::enumerator>;
  template <graph_c G>
  using graph_traits     = traits::detail::traits_impl<G, traits::detail::graph>;
  template <numeric_c N>
  using numeric_traits   = traits::detail::traits_impl<N, traits::detail::numeric>;
  template <allocator_c A>
  using allocator_traits = traits::detail::traits_impl<A, traits::detail::allocator>;
  template <iterator_c I>
  using iterator_traits  = traits::detail::traits_impl<I, traits::detail::iterator>;
  template <string_c S>
  using string_traits    = traits::detail::traits_impl<S, traits::detail::string>;
  template <singleton_c S>
  using singleton_traits = traits::detail::traits_impl<S, traits::detail::singleton>;

  template <hashable_c T>
  using hashable_traits  = traits::detail::traits_impl<T, traits::detail::hash>;






  namespace impl{
    template <typename T>
    struct protected_is_composable_test : T{
      meta::true_type test_is_composable() const requires(requires{ { this->is_composable() } -> same_as<bool>; });
      meta::false_type test_is_composable() const;
    };
  }






  inline namespace concepts{
    template <typename E>
    concept status_enum_c = enumerator_c<E> && enum_traits<E>::is_status_code;
    template <typename E>
    concept bitflag_c     = enumerator_c<E> && enum_traits<E>::is_bitflag;

    template <typename R>
    concept forward_range_c = range_c<R> &&
        std::derived_from<typename range_traits<R>::iterator_category, std::forward_iterator_tag>;
    template <typename R>
    concept bidirectional_range_c = forward_range_c<R> &&
                              std::derived_from<typename range_traits<R>::iterator_category, std::bidirectional_iterator_tag>;
    template <typename R>
    concept random_access_range_c = bidirectional_range_c<R> &&
                              std::derived_from<typename range_traits<R>::iterator_category, std::random_access_iterator_tag>;
    template <typename R>
    concept contiguous_range_c = random_access_range_c<R> &&
                              std::derived_from<typename range_traits<R>::iterator_category, std::contiguous_iterator_tag>;



    template <typename A>
    concept composable_allocator_c = allocator_c<A> && allocator_traits<A>::is_composable;

    template <typename T>
    concept block_allocator_c = requires(T& alloc, const T& const_alloc, memory_block block){
      { alloc.allocate_block() } -> same_as<memory_block>;
      alloc.deallocate_block(block);
      { const_alloc.next_block_size() } -> std::convertible_to<u64>;
    };

    template <typename T>
    concept stateful_allocator_c = allocator_c<T> && allocator_traits<T>::is_stateful;
    template <typename T>
    concept stateless_allocator_c = allocator_c<T> && !allocator_traits<T>::is_stateful;
    template <typename T>
    concept shared_allocator_c = stateful_allocator_c<T> && allocator_traits<T>::is_shared;

    template <typename T>
    concept memory_storage_policy_c = requires(T& policy, const T& cpolicy, const impl::protected_is_composable_test<T>& tester){
      typename T::allocator_type;
      { policy.get_allocator() } -> std::convertible_to<typename T::allocator_type&>;
      { cpolicy.get_allocator() } -> std::convertible_to<const typename T::allocator_type&>;
      { tester.test_is_composable() } -> exact_same_as<meta::true_type>;
    };
    template <typename T>
    concept segregatable_allocator_c = requires(T& seg, const T& cseg, u64 n){
      typename T::allocator_type;
      requires allocator_c<typename T::allocator_type>;
      { seg.get_allocator() } -> exact_same_as<typename T::allocator_type&>;
      { cseg.get_allocator() } -> exact_same_as<const typename T::allocator_type&>;
      { cseg.use_allocate_node(n, n) } -> same_as<bool>;
      { cseg.use_allocate_array(n, n, n) } -> same_as<bool>;
    };
    template <typename T>
    concept memory_tracker_c      = requires(T& t, void* p, u64 count, u64 size, u64 align){
      t.on_node_allocation(p, size, align);
      t.on_array_allocation(p, count, size, align);
      t.on_node_deallocation(p, size, align);
      t.on_array_deallocation(p, count, size, align);
    };
    template <typename T>
    concept memory_deep_tracker_c = memory_tracker_c<T> && requires(T& t, const memory_block& block){
      t.on_allocator_growth(block);
      t.on_allocator_shrinkage(block);
    };
  }




  template <typename T, size_t N>
  struct traits::range<T[N]>{
    VK_constant auto static_size = N;
  };
  template <typename T, size_t N>
  struct traits::range<std::array<T, N>>{
    VK_constant auto static_size = N;
  };

  template <typename T> requires(
      std::is_trivial_v<T> || 
      std::is_standard_layout_v<T> ||
      std::has_unique_object_representations_v<T>)
  struct traits::bitwise_movable<T>{ };


  template <>
  struct traits::status_enum<code>{

    VK_constant utf8 name[]        = u8"generic status code";
    VK_constant utf8 scoped_name[] = u8"valkyrie::code";


    using underlying_type = i32;
    using status_domain   = generic_domain;
  };
}


#define VK_extern_trait_instantiation(trait, ...) extern template struct ::valkyrie::traits::detail::traits_impl<__VA_ARGS__, ::valkyrie::traits::detail::trait>
#define VK_trait_instantiation(trait, ...) template struct ::valkyrie::traits::detail::traits_impl<__VA_ARGS__, ::valkyrie::traits::detail::trait>
#define VK_satisfies_subtrait(type, trait, subtrait) ::valkyrie::traits::detail::satisfied_by<::valkyrie::traits::detail:: trait :: subtrait, type>

#endif  //VALKYRIE_TRAITS_HPP
