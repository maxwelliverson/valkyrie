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
      forward_ilist_node_base* next;
    };
    struct ilist_node_base{
      ilist_node_base* next;
      ilist_node_base* prev;
    };

    template <typename T>
    concept forward_ilist_node_type = std::derived_from<T, forward_ilist_node_base>;
    template <typename T>
    concept ilist_node_type         = std::derived_from<T, ilist_node_base>;


    template <forward_ilist_node_type T, tag Tag>
    class forward_ilist_iterator;
    template <ilist_node_type T, tag Tag>
    class ilist_iterator;
  }






  template <typename Derived, tag ... Tags>
  class alignas(impl::NodeAlignment) ilist_node         : tagged_bases<impl::ilist_node_base, Tags...>{

    using base_type    = tagged_bases<impl::ilist_node_base, Tags...>;
    using derived_type = Derived;

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

    template <impl::forward_ilist_node_type T, tag Tag>
    friend class impl::ilist_iterator;

  public:

    using inode_type = ilist_node;
    //using base_type::get;
  };
  template <typename Derived, tag ... Tags>
  class alignas(impl::NodeAlignment) forward_ilist_node : tagged_bases<impl::forward_ilist_node_base, Tags...> {

    using base_type    = tagged_bases<impl::forward_ilist_node_base, Tags...>;
    using derived_type = Derived;

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

    template <impl::forward_ilist_node_type T, tag Tag>
    friend class impl::forward_ilist_iterator;

  public:

    using forward_inode_type = forward_ilist_node;
  };



  namespace impl{
    class forward_ilist{
      forward_ilist_node_base header;
      u64                     length;


    };

    class ilist{
      ilist_node_base header;
      u64             length;
    };

    class forward_ilist_iterator_base{
    protected:

      explicit forward_ilist_iterator_base(forward_ilist_node_base* node) noexcept : node(node){}

      forward_ilist_node_base* node;


      void inc() noexcept {
        node = node->next;
      }

      friend bool operator==(forward_ilist_iterator_base a, forward_ilist_iterator_base b) noexcept {
        return a.node == b.node;
      }

    public:

      using iterator_category = std::forward_iterator_tag;
    };
    class ilist_iterator_base{


    protected:

      explicit ilist_iterator_base(ilist_node_base* node) noexcept : node(node){}

      ilist_node_base* node;

      void inc() noexcept {
        node = node->next;
      }
      void dec() noexcept {
        node = node->prev;
      }

      inline void plus_eq(ptrdiff_t offset) noexcept {
        assert(offset >= 0);

        for (; offset; --offset)
          this->inc();
      }
      inline void minus_eq(ptrdiff_t offset) noexcept {
        assert(offset >= 0);

        for (; offset; --offset)
          this->dec();
      }

      friend bool operator==(ilist_iterator_base a, ilist_iterator_base b) noexcept {
        return a.node == b.node;
      }

    public:

      using iterator_category = std::bidirectional_iterator_tag;
    };

    template <forward_ilist_node_type T, tag Tag>
    class forward_ilist_iterator : public forward_ilist_iterator_base,
                                   public iterator_facade_base<forward_ilist_iterator<T, Tag>, std::forward_iterator_tag, T>{
      using node_type = typename T::forward_inode_type;
      using tagged_node_type = tagged<impl::forward_ilist_node_base, Tag>;

      static forward_ilist_node_base* cast_to_base(T* node) noexcept {
        static_cast<node_type*>();
      }


    public:

      forward_ilist_iterator(T* address) noexcept : forward_ilist_iterator_base(cast_to_base(address)){}

      inline forward_ilist_iterator& operator+=(ptrdiff_t offset) noexcept {
        assert( offset >= 0);
        for (; offset; --offset) this->inc();
        return *this;
      }
      inline forward_ilist_iterator& operator++() noexcept {
        this->inc();
        return *this;
      }

      inline T& operator*() const noexcept {
        assert( this->node );
        return *static_cast<T*>(static_cast<tagged_node_type*>(this->node));
      }
    };

    template <ilist_node_type T, tag Tag>
    class ilist_iterator : public ilist_iterator_base,
                           public iterator_facade_base<ilist_iterator<T, Tag>, std::bidirectional_iterator_tag, T>{
      using node_type = typename T::inode_type;
      using tagged_node_type = tagged<impl::forward_ilist_node_base, Tag>;

    public:

      ilist_iterator(T* address) noexcept : ilist_iterator_base(){}

      inline ilist_iterator& operator+=(ptrdiff_t offset) noexcept {
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
      }

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
    };

  }


  template <impl::forward_ilist_node_type T, tag Tag = default_tag>
  class forward_ilist : impl::forward_ilist{


  public:

    using iterator       = impl::forward_ilist_iterator<T, Tag>;
    using const_iterator = impl::forward_ilist_iterator<const T, Tag>;
    using reference      = T&;
    using pointer        = T*;

    using value_type     = T;

  };
  template <impl::ilist_node_type T,         tag Tag = default_tag>
  class ilist : impl::ilist{

  public:

    using iterator       = impl::ilist_iterator<T, Tag>;
    using const_iterator = impl::ilist_iterator<const T, Tag>;
    using reference      = T&;
    using pointer        = T*;

    using value_type     = T;

  };



}

#endif//VALKYRIE_ADT_INTRUSIVE_LIST_HPP
