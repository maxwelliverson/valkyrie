//
// Created by maxwe on 2021-03-28.
//

#ifndef VALKYRIE_ADT_INTRUSIVE_LIST_HPP
#define VALKYRIE_ADT_INTRUSIVE_LIST_HPP

#include <valkyrie/adt/tags.hpp>
#include <valkyrie/adt/iterator.hpp>

namespace valkyrie{

  namespace impl{

    inline constexpr static u64 NodeAlignment = 16;

    struct forward_ilist_node_base{
      forward_ilist_node_base* next = nullptr;

      inline bool is_list_element() const noexcept {
        return next != nullptr;
      }
    };
    struct ilist_node_base{
      ilist_node_base* next = nullptr;
      ilist_node_base* prev = nullptr;

      inline bool is_list_element() const noexcept {

        // Assertions that guarantee [ next == null iff prev == null]. We thus
        // only have to test one of the pointers to determine whether or not
        // this node has been inserted into a list.
        VK_assert_msg(!next || prev, "next is not null, but prev is");
        VK_assert_msg(next || !prev, "next is null, but prev is not");

        return next != nullptr;
      }
    };



    template <typename T>
    concept forward_ilist_node_type = std::derived_from<T, forward_ilist_node_base>;
    template <typename T>
    concept ilist_node_type         = std::derived_from<T, ilist_node_base>;


    struct alignas(NodeAlignment) forward_ilist_header : forward_ilist_node_base{
      forward_ilist_node_base* prev = nullptr;
      u64 length                    = 0;

      forward_ilist_header() : forward_ilist_node_base{ this }, prev(this), length(0){}
    };
    struct alignas(NodeAlignment) ilist_header         : ilist_node_base{
      u64 length = 0;

      ilist_header() noexcept : ilist_node_base{ this, this }, length(0){}
    };



    template <typename T, tag Tag>
    class forward_ilist_iterator;
    template <typename T, tag Tag>
    class ilist_iterator;
  }

  template <impl::forward_ilist_node_type T, tag Tag = default_tag>
  class forward_ilist;
  template <impl::ilist_node_type T,         tag Tag = default_tag>
  class ilist;


  template <typename Derived, tag ... Tags>
  class alignas(impl::NodeAlignment) ilist_node         : tagged_bases<impl::ilist_node_base, Tags...> {

    using base_type    = impl::ilist_node_base;
    using derived_type = Derived;


    using tagged_bases_type = tagged_bases<base_type, Tags...>;

    template <exact_same_as_one_of<Tags...> Tag>
    using tagged_base = tagged<base_type, Tag>;

    derived_type&        derived()       &  noexcept {
      return *static_cast<derived_type*>(this);
    }
    const derived_type&  derived() const &  noexcept {
      return *static_cast<const derived_type*>(this);
    }
    derived_type&&       derived()       && noexcept {
      return std::move(*static_cast<derived_type*>(this));
    }
    const derived_type&& derived() const && noexcept {
      return std::move(*static_cast<const derived_type*>(this));
    }

    template <typename T, tag Tag>
    friend class impl::ilist_iterator;
    template <impl::ilist_node_type T,         tag Tag = default_tag>
    friend class ilist;




  public:

    using inode_type   = ilist_node;

    template <exact_same_as_one_of<Tags...> Tag>
    static derived_type*           downcast(tagged<base_type, Tag>* base) noexcept {
      return static_cast<derived_type*>(static_cast<inode_type*>(static_cast<tagged_base<Tag>*>(base)));
    }
    template <exact_same_as_one_of<Tags...> Tag>
    static const derived_type*     downcast(const tagged<base_type, Tag>* base) noexcept {
      return static_cast<const derived_type*>(static_cast<const inode_type*>(static_cast<const tagged_base<Tag>*>(base)));
    }
    template <exact_same_as_one_of<Tags...> Tag>
    static tagged_base<Tag>*       upcast(derived_type* derived) noexcept {
      return static_cast<ilist_node*>(derived);
    }
    template <exact_same_as_one_of<Tags...> Tag>
    static const tagged_base<Tag>* upcast(const derived_type* derived) noexcept {
      return static_cast<const ilist_node*>(derived);
    }

    //using base_type::get;
  };
  template <typename Derived, tag ... Tags>
  class alignas(impl::NodeAlignment) forward_ilist_node : tagged_bases<impl::forward_ilist_node_base, Tags...> {

    using base_type    = impl::forward_ilist_node_base;
    using derived_type = Derived;


    using tagged_bases_type = tagged_bases<base_type, Tags...>;

    template <exact_same_as_one_of<Tags...> Tag>
    using tagged_base = tagged<base_type, Tag>;

    derived_type&        derived()       &  noexcept {
      return *static_cast<derived_type*>(this);
    }
    const derived_type&  derived() const &  noexcept {
      return *static_cast<const derived_type*>(this);
    }
    derived_type&&       derived()       && noexcept {
      return std::move(*static_cast<derived_type*>(this));
    }
    const derived_type&& derived() const && noexcept {
      return std::move(*static_cast<const derived_type*>(this));
    }

    template <typename T, tag Tag>
    friend class impl::forward_ilist_iterator;
    template <impl::forward_ilist_node_type T, tag Tag = default_tag>
    friend class forward_ilist;


    template <exact_same_as_one_of<Tags...> Tag>
    static derived_type*           downcast(tagged<base_type, Tag>* base) noexcept {
      return static_cast<derived_type*>(static_cast<forward_inode_type*>(static_cast<tagged_base<Tag>*>(base)));
    }
    template <exact_same_as_one_of<Tags...> Tag>
    static const derived_type*     downcast(const tagged<base_type, Tag>* base) noexcept {
      return static_cast<const derived_type*>(static_cast<const forward_inode_type *>(static_cast<const tagged_base<Tag>*>(base)));
    }
    template <exact_same_as_one_of<Tags...> Tag>
    static tagged_base<Tag>*       upcast(derived_type* derived) noexcept {
      return static_cast<forward_inode_type *>(derived);
    }
    template <exact_same_as_one_of<Tags...> Tag>
    static const tagged_base<Tag>* upcast(const derived_type* derived) noexcept {
      return static_cast<const forward_inode_type *>(derived);
    }


  public:



    using forward_inode_type = forward_ilist_node;

  };




  template <typename From, typename To>
  concept pointer_castable_to    = requires(From* from, To*& to){
    to = static_cast<To*>(from);
  };
  template <typename From, typename To>
  concept pointer_convertible_to = pointer_castable_to<From, To> && requires(From* from, To*& to){
    to = from;
  };






  namespace impl{

    /*struct forward_ilist_base{
      forward_ilist_header header;
      u64                  length;


    };

    struct ilist_base{
      ilist_header header;
      u64          length;
    };*/

    template <typename NodeBase>
    class ilist_iterator_base{

      using node_t = NodeBase;


    protected:
      inline constexpr static bool IsDoublyLinked = same_as<node_t, ilist_node_base>;
      inline constexpr static bool IsConst        = std::is_const_v<node_t>;



    public:
      using iterator_category = std::conditional_t<IsDoublyLinked, std::bidirectional_iterator_tag, std::forward_iterator_tag>;

    protected:

      using node_base_type = remove_cv_t<NodeBase>;


      explicit ilist_iterator_base(node_t* node) noexcept : node(node){}
      ilist_iterator_base(const ilist_iterator_base& other) = default;
      ilist_iterator_base(ilist_iterator_base<remove_const_t<node_t>> other) noexcept requires(IsConst) : node(other.node){}
      explicit ilist_iterator_base(ilist_iterator_base<add_const_t<node_t>> other) noexcept requires(!IsConst) : node(const_cast<node_t*>(other.node)){}

      node_t* node;

      void inc() noexcept {
        node = node->next;
      }
      void dec() noexcept requires(IsDoublyLinked) {
        node = node->prev;
      }

      inline void plus_eq(ptrdiff_t offset) noexcept {
        assert(offset >= 0);

        for (; offset; --offset)
          this->inc();
      }
      inline void minus_eq(ptrdiff_t offset) noexcept requires(IsDoublyLinked) {
        assert(offset >= 0);

        for (; offset; --offset)
          this->dec();
      }

    public:
      friend bool operator==(ilist_iterator_base a, ilist_iterator_base b) noexcept {
        return a.node == b.node;
      }
    };

    extern template class ilist_iterator_base<forward_ilist_node_base>;
    extern template class ilist_iterator_base<const forward_ilist_node_base>;
    extern template class ilist_iterator_base<ilist_node_base>;
    extern template class ilist_iterator_base<const ilist_node_base>;

    template <typename NodeBase, tag Tag>
    class tagged_ilist_iterator_base : public ilist_iterator_base<NodeBase>{

      using base = ilist_iterator_base<NodeBase>;
      using node_ptr_t = NodeBase*;

    protected:
      using tag_t            = Tag;
      using base_node_t      = typename base::node_base_type;
      using tagged_node_t    = tagged<base_node_t, tag_t>;

      using tagged_ptr_t     = copy_cv_t<tagged_node_t, NodeBase>*;

      tagged_ptr_t tagged_node() const noexcept {
        return static_cast<tagged_ptr_t>(this->node);
      }

      tagged_ilist_iterator_base(node_ptr_t node) noexcept : base(node){}
      tagged_ilist_iterator_base(tagged_ptr_t node) noexcept : base(node){}

      tagged_ilist_iterator_base(const tagged_ilist_iterator_base&) = default;

    public:

    };

    template <typename T, tag Tag>
    class forward_ilist_iterator : public tagged_ilist_iterator_base<copy_cv_t<forward_ilist_node_base, T>, Tag>,
                                   public iterator_facade_base<forward_ilist_iterator<T, Tag>, std::forward_iterator_tag, T>{

      using base_node_type = copy_cv_t<forward_ilist_node_base, T>;
      using base      = tagged_ilist_iterator_base<base_node_type, Tag>;
      using facade    = iterator_facade_base<forward_ilist_iterator<T, Tag>, std::forward_iterator_tag, T>;

      using node_type = typename T::forward_inode_type;
      using tagged_node_type = tagged<impl::forward_ilist_node_base, Tag>;

      using value_t = std::remove_cv_t<T>;

      inline static typename base::tagged_ptr_t to_base(T* node) noexcept {
        return node_type::template upcast<Tag>(node);
      }
      inline static T*                          to_derived(forward_ilist_node_base* node) noexcept {
        return static_cast<T*>(node_type::downcast(static_cast<typename base::tagged_ptr_t>(node)));
      }


      mutable base_node_type* prev_element_;

    public:

      forward_ilist_iterator() = default;

      forward_ilist_iterator(T* address) noexcept : base(forward_ilist_iterator::to_base(address)), prev_element_(nullptr){}
      forward_ilist_iterator(copy_cv_t<forward_ilist_header, T>& header) noexcept : base(&header),  prev_element_(header.prev){}

      template <pointer_castable_to<T> U>
      explicit(!pointer_convertible_to<U, T>)
      forward_ilist_iterator(const forward_ilist_iterator<U, Tag>& other) noexcept : base(static_cast<T*>(std::addressof(*other))), prev_element_(other.prev_element_){}




      /*inline forward_ilist_iterator& operator+=(ptrdiff_t offset) noexcept {
        assert( offset >= 0);
        for (; offset; --offset) this->inc();
        return *this;
      }*/
      inline forward_ilist_iterator& operator++() noexcept {
        prev_element_ = this->node;
        this->inc();
        return *this;
      }

      inline T& operator*() const noexcept {
        assert( this->node );
        return *forward_ilist_iterator::to_derived(this->node);
      }

      template <pointer_convertible_to<T> U, valid_tag_for<T> UTag>
      static forward_ilist_iterator convert(forward_ilist_iterator<U, UTag> other) noexcept {
        return std::addressof(*other);
      }
    };

    template <typename T, tag Tag>
    class ilist_iterator : public tagged_ilist_iterator_base<copy_cv_t<ilist_node_base, T>, Tag>,
                           public iterator_facade_base<ilist_iterator<T, Tag>, std::bidirectional_iterator_tag, T> {

      using base = tagged_ilist_iterator_base<copy_cv_t<ilist_node_base, T>, Tag>;
      using facade = iterator_facade_base<ilist_iterator<T, Tag>, std::bidirectional_iterator_tag, T>;
      using node_type = typename T::inode_type;
      using tagged_node_type = tagged<impl::forward_ilist_node_base, Tag>;

      inline static typename base::tagged_ptr_t to_base(T* node) noexcept {
        return node_type::template upcast<Tag>(node);
      }
      inline static T*                          to_derived(forward_ilist_node_base* node) noexcept {
        return static_cast<T*>(node_type::downcast(static_cast<typename base::tagged_ptr_t>(node)));
      }

    public:

      ilist_iterator() = default;
      ilist_iterator(copy_cv_t<ilist_header, T>& header) noexcept : base(&header){}
      ilist_iterator(T* address) noexcept : base(ilist_iterator::to_base(address)){}



      template <pointer_castable_to<T> U>
      explicit(!pointer_convertible_to<U, T>)
      ilist_iterator(const ilist_iterator<U, Tag>& other) noexcept : base(static_cast<T*>(std::addressof(*other))){}

      /*inline ilist_iterator& operator+=(ptrdiff_t offset) noexcept {
        if (offset < 0)
          this->minus_eq(-offset);
        else
          this->plus_eq(offset);
        return *this;
      }
      inline ilist_iterator& operator-=(ptrdiff_t offset) noexcept {
        if (offset < 0)
          this->plus_eq(-offset);
        else
          this->minus_eq(offset);
        return *this;
      }*/

      inline ilist_iterator& operator++() noexcept {
        this->inc();
        return *this;
      }
      inline ilist_iterator& operator--() noexcept {
        this->dec();
        return *this;
      }

      inline T& operator*() const noexcept {
        return *static_cast<T*>(static_cast<tagged_node_type*>(this->node));
      }

      template <pointer_convertible_to<T> U, valid_tag_for<T> UTag>
      static ilist_iterator convert(ilist_iterator<U, UTag> other) noexcept {
        return std::addressof(*other);
      }
    };
  }


  template <impl::forward_ilist_node_type T, tag Tag>
  class forward_ilist{


    using node_base_type = typename T::forward_inode_type;

  public:

    using iterator        = impl::forward_ilist_iterator<T, Tag>;
    using const_iterator  = impl::forward_ilist_iterator<const T, Tag>;
    using reference       = typename iterator::reference;
    using const_reference = typename const_iterator::reference;
    using pointer         = typename iterator::pointer;
    using const_pointer   = typename const_iterator::pointer;
    using value_type      = typename iterator::value_type;


    forward_ilist() = default;



    reference front() noexcept {

    }



    iterator        begin()       noexcept {
      return ++end();
    }
    const_iterator  begin() const noexcept {
      return ++end();
    }
    const_iterator cbegin() const noexcept {
      return ++cend();
    }


    iterator        end()       noexcept {
      return iterator(header);
    }
    const_iterator  end() const noexcept {
      return const_iterator(header);
    }
    const_iterator cend() const noexcept {
      return const_iterator(header);
    }


  private:
    impl::forward_ilist_header header;
  };


  template <impl::ilist_node_type T,         tag Tag>
  class ilist{

    inline void assert_invariants() const noexcept {

#if VK_debug_build
      const bool len                = header.length;
      const bool next_equals_header = header.next == &header;
      const bool prev_equals_header = header.prev == &header;
      const bool next_equals_prev   = header.next == header.prev;
#endif

      VK_assert_msg(header.next != nullptr, "header.next is null");
      VK_assert_msg(header.prev != nullptr, "header.prev is null");

      VK_assert_msg(len || next_equals_header, "header.length is 0, but header.next does not point to itself");
      VK_assert_msg(len || prev_equals_header, "header.length is 0, but header.prev does not point to itself");
      VK_assert_msg(!len || !next_equals_header, "header.length is not 0, but header.next points to itself");
      VK_assert_msg(!len || !prev_equals_header, "header.length is not 0, but header.prev points to itself");
      VK_assert_msg((len != 1) || next_equals_prev, "header.length is 1, but header.next is not equal to header.prev");
      VK_assert_msg((len == 1) || !next_equals_prev, "header.length is not 1, but header.next is equal to header.prev");
    }

    using node_base_type = typename T::inode_type;


    inline void insert_after(impl::forward_ilist_node_base* node, T* new_node) noexcept {
      VK_assert(node != nullptr);
      auto* const base_node = node_base_type::template upcast<Tag>(new_node);
      VK_assert(!base_node->is_list_element());

      base_node->next = node->next;
      node->next = base_node;

      VK_assert(base_node->is_list_element());
    }
    inline T*   remove_after(impl::forward_ilist_node_base* node) noexcept {
      VK_assert_msg(!empty(), "remove_after was called on an empty list");
      VK_assert(node != nullptr);
      auto* const next = node->next;

      VK_assert(next->is_list_element());

      T* ret = static_cast<T*>(node_base_type::downcast(static_cast<tagged<impl::forward_ilist_node_base, Tag>*>(next)));
      node->next = next->next;
      next->next = nullptr;

      VK_assert(!next->is_list_element());

      return ret;
    }


  public:

    using iterator               = impl::forward_ilist_iterator<T, Tag>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_iterator         = impl::forward_ilist_iterator<const T, Tag>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reference              = typename iterator::reference;
    using const_reference        = typename const_iterator::reference;
    using pointer                = typename iterator::pointer;
    using const_pointer          = typename const_iterator::pointer;
    using value_type             = typename iterator::value_type;
    using size_type              = u64;




    ilist() = default;


    /* [ modifiers ] */

    void push_front(T* element) noexcept {
      insert_after(&header, element);
      ++header.length;
      assert_invariants();
    }
    void push_back(T* element) noexcept {

      ++header.length;
      assert_invariants();
    }
    T*   pop_front() noexcept {

      if (empty())
        return nullptr;

      T* retval;

      --header.length;
      assert_invariants();
    }
    T*   pop_back() noexcept {

      if (empty())
        return nullptr;

      --header.length;
      assert_invariants();
    }

    void insert(const_iterator after_element, T* element)   noexcept {


      ++header.length;
      assert_invariants();
    }
    T*   remove(const_iterator after_element)               noexcept {



      --header.length;
      assert_invariants();
    }




    /* [ observers ] */

    size_type       size() const noexcept {
      //VK_assert();

      return header.length;
    }
    bool            empty() const noexcept {
      return header.length == 0;
    }




    /* [ accessors ] */

    reference       front()       noexcept {
      VK_assert(not empty());
      return *begin();
    }
    const_reference front() const noexcept {
      VK_assert(not empty());
      return *begin();
    }
    reference       back()        noexcept {
      VK_assert(not empty());
      return *--end();
    }
    const_reference back()  const noexcept {
      VK_assert(not empty());
      return *--end();
    }




    /* [ iterators ] */

    iterator        begin()       noexcept {
      return ++end();
    }
    const_iterator  begin() const noexcept {
      return ++end();
    }
    const_iterator cbegin() const noexcept {
      return ++cend();
    }

    iterator        end()       noexcept {
      return iterator(header);
    }
    const_iterator  end() const noexcept {
      return const_iterator(header);
    }
    const_iterator cend() const noexcept {
      return const_iterator(header);
    }


    reverse_iterator        rbegin()       noexcept {
      return reverse_iterator(end());
    }
    const_reverse_iterator  rbegin() const noexcept {
      return const_reverse_iterator(end());
    }
    const_reverse_iterator crbegin() const noexcept {
      return const_reverse_iterator(cend());
    }


    reverse_iterator        rend()       noexcept {
      return reverse_iterator(begin());
    }
    const_reverse_iterator  rend() const noexcept {
      return const_reverse_iterator(begin());
    }
    const_reverse_iterator crend() const noexcept {
      return const_reverse_iterator(cbegin());
    }

  private:

    impl::ilist_header header;
  };



}

#endif//VALKYRIE_ADT_INTRUSIVE_LIST_HPP
