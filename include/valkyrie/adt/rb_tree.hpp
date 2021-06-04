//
// Created by Maxwell on 2021-02-22.
//

#ifndef VALKYRIE_CORE_ADT_RBTREE_HPP
#define VALKYRIE_CORE_ADT_RBTREE_HPP

#include <valkyrie/traits.hpp>
#include <valkyrie/utility/ordering.hpp>
#include <valkyrie/memory/new_delete.hpp>


#include <limits>

namespace valkyrie{


  template <typename T,
            weak_order<const T&> Order = std::compare_three_way,
            allocator_c Alloc = default_allocator>
  class basic_rb_tree;



  namespace impl{

    class rb_tree_base;

    class rb_node_base{
    public:
      enum class colour   : u8 { black, red };
      enum class position : u8 { root, left, right };

    private:


      rb_node_base* leftChild_;
      rb_node_base* rightChild_;
      rb_node_base* parent_;
      colour        colour_;
      position      position_;


    protected:

      rb_node_base() noexcept : colour_(colour::red){}

    public:


      position get_position() const noexcept {
        return position_;
      }
      colour   get_colour() const noexcept {
        return colour_;
      }


      void set_right_child(rb_node_base* node) noexcept {
        node->position_ = position::right;
        node->parent_ = this;
        rightChild_ = node;
      }
      void set_left_child(rb_node_base* node) noexcept {
        node->position_ = position::left;
        node->parent_ = this;
        leftChild_ = node;
      }


      rb_node_base* get_parent() const noexcept { return parent_; }
      rb_node_base* get_left_child() const noexcept { return leftChild_; }
      rb_node_base* get_right_child() const noexcept { return rightChild_; }


      bool is_red() const noexcept { return colour_ == colour::red; }
      bool is_black() const noexcept { return colour_ == colour::black; }
      bool is_root() const noexcept { return position_ == position::root; }
      bool is_left() const noexcept { return position_ == position::left; }
      bool is_right() const noexcept { return position_ == position::right; }

      void set_red() noexcept { colour_ = colour::red; }
      void set_black() noexcept { colour_ = colour::black; }
      void set_colour(colour c) noexcept { colour_ = c; }

      void set_root(const rb_tree_base* tree) noexcept;

    };

    class rb_tree_base{
      friend class rb_node_base;
    protected:

      mutable rb_node_base nullNode;
      rb_node_base*        pRootNode;
      u32                  nodeCount;


      rb_node_base*              null() const noexcept {
        return &nullNode;
      }
      bool                       is_null(rb_node_base* node) const noexcept {
        return node == null();
      }
      rb_node_base*              get_root() const noexcept { return pRootNode; }
      void                       set_root(rb_node_base* node) noexcept {
        pRootNode = node;
        node->set_root(this);
      }


      void                       left_rotate(rb_node_base* node) noexcept;
      void                       right_rotate(rb_node_base* node) noexcept;

      VK_nodiscard rb_node_base* get_maximum(rb_node_base* node) const noexcept;
      VK_nodiscard rb_node_base* get_minimum(rb_node_base* node) const noexcept;
      VK_nodiscard rb_node_base* get_successor(rb_node_base* node) const noexcept;
      VK_nodiscard rb_node_base* get_predecessor(rb_node_base* node) const noexcept;

      void                       post_insert_rebalance(rb_node_base* node) noexcept;
      VK_nodiscard rb_node_base* delete_node(rb_node_base* node) noexcept;
      void                       post_delete_rebalance(rb_node_base* node) noexcept;

    public:

      VK_nodiscard bool empty() const noexcept {
        VK_assert( ( size() > 0 ) ^ is_null(get_root()) );
        return is_null(get_root());
      }
      VK_nodiscard u32  size() const noexcept {
        return nodeCount;
      }
    };




  }

  template <typename T, weak_order<const T&> Order, allocator_c Alloc>
  class basic_rb_tree : protected impl::rb_tree_base{
    class rb_node : public impl::rb_node_base{
      T value_;

    public:

      rb_node(T&& value) noexcept
          : impl::rb_node_base(), value_(std::move(value)){}

      T&       get()       noexcept { return value_; }
      const T& get() const noexcept { return value_; }
    };

    using alloc_state = typename allocator_traits<Alloc>::allocator_type;

    rb_node* root_node() const noexcept {
      return static_cast<rb_node*>(this->get_root());
    }

    auto do_order(const rb_node& a, const rb_node& b) const noexcept {
      return order(a.get(), b.get());
    }


    bool do_insert(T&& val, rb_node*& resultNode) noexcept {

      if ( empty() ) {
        auto* new_node = new_object<rb_node>(std::move(val));
        set_root(new_node);
        new_node->set_black();
        ++nodeCount;
        resultNode = new_node;
        return true;
      }

      rb_node* x = root_node();
      rb_node* new_node;
      weak_ordering o;

      do {
        o = order(val, x->get());

        if ( o == weak_ordering::equivalent ) VK_unlikely {
          resultNode = x;
          return false;
        }

        if ( o == weak_ordering::less ) {
          if ( is_null(x->get_left_child()) ) {
            new_node   = new_object<rb_node>(allocator, std::move(val));
            x->set_left_child(new_node);
            break;
          }

          x = static_cast<rb_node*>(x->get_left_child());
        }
        else {
          if ( is_null(x->get_right_child()) ) {
            new_node   = new_object<rb_node>(allocator, std::move(val));
            x->set_right_child(new_node);
            break;
          }

          x = static_cast<rb_node*>(x->get_right_child());
        }
      } while ( true );


      this->post_insert_rebalance(new_node);

      ++nodeCount;
      resultNode = new_node;
      return true;
    }
    void do_remove(rb_node* node) noexcept {
      VK_assert(size() > 0);
      rb_node* rem_node = static_cast<rb_node*>(delete_node(node));
      if ( rem_node != node )
        node->get() = std::move(rem_node->get());
      post_delete_rebalance(rem_node);
      delete_object(allocator, rem_node);
    }

  public:




  private:

    [[no_unique_address]] alloc_state allocator;
    [[no_unique_address]] Order       order;
  };


  template <typename T, allocator_c Alloc = default_allocator>
  using rb_tree = basic_rb_tree<T, std::compare_three_way, Alloc>;
}

#endif//VALKYRIE_CORE_ADT_RBTREE_HPP
