//
// Created by Maxwell on 2021-03-28.
//

#ifndef VALKYRIE_ADT_TAGS_HPP
#define VALKYRIE_ADT_TAGS_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie{
  namespace impl{
    struct tag_base{
      inline static void on_construction(void* obj) noexcept {}
      inline static void on_copy_construction(void* to, const void* from) noexcept {}
      inline static void on_move_construction(void* to, void* from) noexcept {}
      inline static void on_copy_assignment(void* to, const void* from) noexcept {}
      inline static void on_move_assignment(void* to, void* from) noexcept {}
      inline static void on_destruction(void* obj) noexcept {}
    };
  }

#define VK_define_tag(name) struct VK_empty_bases name : ::valkyrie::impl::tag_base{}

  template <typename T>
  concept tag = std::derived_from<T, impl::tag_base> && requires(T* to, T* from, const T* cfrom){
    T::on_construction(to);
    T::on_copy_construction(to, cfrom);
    T::on_move_construction(to, from);
    T::on_copy_assignment(to, cfrom);
    T::on_move_assignment(to, from);
    T::on_destruction(to);
  };


  VK_define_tag(default_tag);


  template <typename T, tag Tag>
  class tagged       : public T, public Tag{
    
    inline constexpr static bool default_constructible         = std::conjunction_v<std::is_default_constructible<T>, std::is_default_constructible<Tag>>;
    inline constexpr static bool copy_constructible            = std::conjunction_v<std::is_copy_constructible<T>, std::is_copy_constructible<Tag>>;
    inline constexpr static bool move_constructible            = std::conjunction_v<std::is_move_constructible<T>, std::is_move_constructible<Tag>>;
    inline constexpr static bool copy_assignable               = std::conjunction_v<std::is_copy_assignable<T>, std::is_copy_assignable<Tag>>;
    inline constexpr static bool move_assignable               = std::conjunction_v<std::is_move_assignable<T>, std::is_move_assignable<Tag>>;

    inline constexpr static bool nothrow_default_constructible = std::conjunction_v<std::is_nothrow_default_constructible<T>, std::is_nothrow_default_constructible<Tag>>;
    inline constexpr static bool nothrow_copy_constructible    = std::conjunction_v<std::is_nothrow_copy_constructible<T>, std::is_nothrow_copy_constructible<Tag>>;
    inline constexpr static bool nothrow_move_constructible    = std::conjunction_v<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_constructible<Tag>>;
    inline constexpr static bool nothrow_copy_assignable       = std::conjunction_v<std::is_nothrow_copy_assignable<T>, std::is_nothrow_copy_assignable<Tag>>;
    inline constexpr static bool nothrow_move_assignable       = std::conjunction_v<std::is_nothrow_move_assignable<T>, std::is_nothrow_move_assignable<Tag>>;

  
    template <typename ...Args>
    inline constexpr static bool constructible_with            = std::conjunction_v<std::is_default_constructible<Tag>, std::is_constructible<T, Args...>>;
    template <typename ...Args>
    inline constexpr static bool nothrow_constructible_with    = std::conjunction_v<std::is_nothrow_default_constructible<Tag>, std::is_nothrow_constructible<T, Args...>>;

    inline T&          get_value() & noexcept {
      return *this;
    }
    inline const T&    get_value() const & noexcept {
      return *this;
    }
    inline T&&         get_value() && noexcept {
      return std::move(*this);
    }
    inline const T&&   get_value() const && noexcept {
      return std::move(*this);
    }

    inline Tag&        get_tag() & noexcept {
      return *this;
    }
    inline const Tag&  get_tag() const & noexcept {
      return *this;
    }
    inline Tag&&       get_tag() && noexcept {
      return std::move(*this);
    }
    inline const Tag&& get_tag() const && noexcept {
      return std::move(*this);
    }
    
  public:

    using base_type = T;
    using tag_type  = Tag;

    tagged()                    noexcept(nothrow_default_constructible) requires(default_constructible)
        : T(), tag_type(){
      tag_type::on_construction(this);
    }
    tagged(const tagged& other) noexcept(nothrow_copy_constructible)    requires(copy_constructible)
        : T(other.get_value()), tag_type(other.get_tag()){
      tag_type::on_copy_construction(this, std::addressof(other));
    }
    tagged(tagged&& other)      noexcept(nothrow_move_constructible)    requires(move_constructible)
        : T(std::move(other.get_value())), tag_type(std::move(other.get_tag())){
      tag_type::on_move_construction(this, std::addressof(other));
    }

    template <typename ...Args>
    explicit tagged(std::in_place_t, Args&& ...args) noexcept(nothrow_constructible_with<Args...>)  requires(constructible_with<Args...>)
        : base_type(std::forward<Args>(args)...), tag_type(){
      tag_type::on_construction(this);
    }

    tagged& operator=(const tagged& other) noexcept(nothrow_copy_assignable) requires(copy_assignable) {
      get_value() = other.get_value();
      get_tag() = other.get_tag();
      return *this;
    }
    tagged& operator=(tagged&& other)      noexcept(nothrow_move_assignable) requires(move_assignable) {
      get_value() = std::move(other.get_value());
      get_tag()   = std::move(other.get_tag());
      return *this;
    }

    ~tagged() {
      tag_type::on_destruction(this);
    }




    inline base_type* cast_to_base(impl::tag_base* tag_) const noexcept {
      return tag_ == std::addressof(get_tag()) ? const_cast<tagged*>(this) : nullptr;
    }
    inline tag_type*  cast_to_tag(base_type* base_) const noexcept {
      return (base_ == std::addressof(get_value())) ? const_cast<tagged*>(this) : nullptr;
    }
  };

  template <typename T, tag ... Tags> requires(meta::typeset<meta::list<Tags...>>)
  class tagged_bases : private tagged<T, Tags> ... {

    using tag_base = std::remove_reference_t<std::common_reference_t<Tags&...>>;

    static_assert(std::derived_from<tag_base, impl::tag_base>);


    inline constexpr static u64 TypeSize = std::is_empty_v<T> ? 0 : sizeof(T);

  public:
    using base_type = T;

    tagged_bases() = default;
    tagged_bases(const tagged_bases&)     = default;
    tagged_bases(tagged_bases&&) noexcept = default;

    tagged_bases& operator=(const tagged_bases&) = default;
    tagged_bases& operator=(tagged_bases&&) noexcept = default;

    ~tagged_bases() = default;




    template <same_as_one_of<Tags...> Tag>
    Tag*             get_tag()           noexcept {
      return this;
      //return static_cast<tagged<T, Tag>*>(this);
    }
    template <same_as_one_of<Tags...> Tag>
    const Tag*       get_tag()     const noexcept {
      return this;
      //return static_cast<tagged<T, Tag>*>(this);
    }

    tag_base*        get_tag(T* p) const noexcept {

    }


    template <same_as_one_of<Tags...> Tag>
    base_type*       get()       noexcept {
      return static_cast<tagged<T, Tag>*>(this);
    }
    template <same_as_one_of<Tags...> Tag>
    const base_type* get() const noexcept {
      return static_cast<const tagged<T, Tag>*>(this);
    }


    template <same_as_one_of<Tags...> Tag>
    base_type*       get(Tag* t)            const noexcept {
      return static_cast<tagged<T, Tag>*>(this)->cast_to_base(t);
    }
    base_type*       get(impl::tag_base* t) const noexcept {

      const auto pt = reinterpret_cast<byte*>(t);
      const auto this_addr = reinterpret_cast<const byte*>(this);
      const auto next_addr = this_addr + sizeof(tagged_bases);

      return (this_addr < pt && pt <= next_addr) ? reinterpret_cast<base_type*>(pt - TypeSize) : nullptr;
    }
  };
}

#endif//VALKYRIE_ADT_TAGS_HPP
