//
// Created by Maxwell on 2021-02-22.
//

#ifndef VALKYRIE_CORE_ADT_RBTREE_HPP
#define VALKYRIE_CORE_ADT_RBTREE_HPP

#include <valkyrie/traits.hpp>
#include <valkyrie/memory/default_allocator.hpp>


#include <limits>

namespace valkyrie{


  template <typename T,
            weak_order<const T&> Order = std::compare_three_way,
            allocator_c Alloc = default_allocator>
  class basic_rb_tree;

  template <typename T, allocator_c Alloc = default_allocator>
  using rb_tree = basic_rb_tree<T, std::compare_three_way, Alloc>;

  namespace impl{

    class rb_tree_base;

    class rb_node_base{
    public:
      enum class colour   : u8 { black, red };
      enum class position : u8 { root, left, right };

      rb_node_base* leftChild_;
      rb_node_base* rightChild_;
      rb_node_base* parent_;
      colour        colour_;
      position      position_;


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
      void insert(rb_tree_base* tree) noexcept;

      VK_nodiscard rb_node_base* get_maximum(rb_tree_base* tree) const noexcept;
      VK_nodiscard rb_node_base* get_minimum(rb_tree_base* tree) const noexcept;
      VK_nodiscard rb_node_base* get_successor(rb_tree_base* tree) const noexcept;
      VK_nodiscard rb_node_base* get_predeccessor(rb_tree_base* tree) const noexcept;



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
      void set_root() noexcept { position_ = position::root; }
    };

    class rb_tree_base{
    protected:

      mutable rb_node_base  nullNode;
      rb_node_base* pRootNode;
      u32           nodeCount;


      rb_node_base* null() const noexcept {
        return &nullNode;
      }
      rb_node_base* get_root() const noexcept { return pRootNode; }
      void          set_root(rb_node_base* node) noexcept {
        pRootNode = node;
        node->parent_ = null();
        node->position_ = rb_node_base::position::root;
      }
      rb_node_base* left_rotate(rb_node_base* node) noexcept;
      rb_node_base* right_rotate(rb_node_base* node) noexcept;
    };




  }

  template <typename T, weak_order<const T&> Order, allocator_c Alloc>
  class basic_rb_tree : protected impl::rb_tree_base{
    class rb_node : public rb_node_base{
      T value_;
    };

    using alloc_state = typename allocator_traits<Alloc>::allocator_type;
  };

}

#endif//VALKYRIE_CORE_ADT_RBTREE_HPP
