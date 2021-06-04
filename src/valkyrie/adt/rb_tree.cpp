//
// Created by maxwe on 2021-05-07.
//

#include <valkyrie/adt/rb_tree.hpp>


using namespace valkyrie;

void impl::rb_node_base::set_root(const rb_tree_base* tree) noexcept {
  parent_ = tree->null();
  position_ = position::root;
}


void impl::rb_tree_base::post_insert_rebalance(rb_node_base *node) noexcept {
  rb_node_base* x = node;
  rb_node_base* y;
  rb_node_base* p;
  rb_node_base* pp;
  rb_node_base* _p;
  rb_node_base* _pp;

  while (!x->is_root() && (p = x->get_parent())->is_red()) {
    pp = p->get_parent();
    if (p->is_left()) {
      y = pp->get_right_child();
      if (y->is_red()) {
        p->set_black();
        y->set_black();
        pp->set_red();
        x = pp;
      }
      else {
        if (x->is_right()) {
          x = p;
          left_rotate(x);
        }
        _p = x->get_parent();
        _pp = _p->get_parent();
        _p->set_black();
        _pp->set_red();
        right_rotate(_pp);
      }
    }
    else {
      y = pp->get_left_child();
      if ( y->is_red() ) {
        p->set_black();
        y->set_black();
        pp->set_red();
        x = pp;
      }
      else {
        if (x->is_left()) {
          x = p;
          right_rotate(x);
        }
        _p = x->get_parent();
        _pp = _p->get_parent();
        _p->set_black();
        _pp->set_red();
        left_rotate(_pp);
      }
    }
  }
  if ( get_root()->is_red() )
    get_root()->set_black();
}
VK_nodiscard impl::rb_node_base* impl::rb_tree_base::delete_node(rb_node_base *node) noexcept {

  rb_node_base *x, *y;

  if ( is_null(node->get_left_child()) ) {
    y = node;
    x = node->get_right_child();
  }
  else {
    if ( is_null(node->get_right_child()) ) {
      y = node;
      x = node->get_left_child();
    }
    else {
      y = get_successor(node);
      x = y->get_left_child();
      if ( is_null(x) )
        x = y->get_right_child();
    }
  }

  switch ( y->get_position() ) {
    case rb_node_base::position::root:
      set_root(x);
      break;
    case rb_node_base::position::left:
      y->get_parent()->set_left_child(x);
      break;
    case rb_node_base::position::right:
      y->get_parent()->set_right_child(x);
      break;
    VK_no_default;
  }

  return y;
}
void impl::rb_tree_base::post_delete_rebalance(rb_node_base *node) noexcept {

  rb_node_base* x = node;
  rb_node_base* w;

  while ( x->is_black() ) {
    switch ( x->get_position() ) {
      case rb_node_base::position::root:
        return;
      case rb_node_base::position::left:

        w = x->get_parent()->get_right_child();
        if ( w->is_red() ) {
          w->set_black();
          x->get_parent()->set_red();
          left_rotate(x->get_parent());
          w = x->get_parent()->get_right_child();
        }
        if ( w->get_right_child()->is_black() ) {
          
          w->set_red();
          
          if ( w->get_left_child()->is_black() ) {
            x = x->get_parent();
            break;
          }
          
          w->get_left_child()->set_black();
          right_rotate(w);
          w = x->get_parent()->get_right_child();
        }
        
        w->set_colour(x->get_parent()->get_colour());
        x->get_parent()->set_black();
        w->get_right_child()->set_black();
        left_rotate(x->get_parent());
        x = get_root();
        break;


      case rb_node_base::position::right:

        w = x->get_parent()->get_left_child();
        if ( w->is_red() ) {
          w->set_black();
          x->get_parent()->set_red();
          right_rotate(x->get_parent());
          w = x->get_parent()->get_left_child();
        }
        if ( w->get_left_child()->is_black() ) {

          w->set_red();

          if ( w->get_right_child()->is_black() ) {
            x = x->get_parent();
            break;
          }

          w->get_right_child()->set_black();
          left_rotate(w);
          w = x->get_parent()->get_left_child();
        }

        w->set_colour(x->get_parent()->get_colour());
        x->get_parent()->set_black();
        w->get_left_child()->set_black();
        right_rotate(x->get_parent());
        x = get_root();
        break;
      VK_no_default;
    }
  }
  
  x->set_black();
}


VK_nodiscard impl::rb_node_base* impl::rb_tree_base::get_maximum(rb_node_base* node) const noexcept {

  rb_node_base* x = node;
  rb_node_base* y = x->get_right_child();

  while ( !is_null(y) ) {
    x = y;
    y = y->get_right_child();
  }

  return x;
}
VK_nodiscard impl::rb_node_base* impl::rb_tree_base::get_minimum(rb_node_base* node) const noexcept {

  rb_node_base* x = node;
  rb_node_base* y = x->get_left_child();

  while ( !is_null(y) ) {
    x = y;
    y = y->get_left_child();
  }

  return x;

}
VK_nodiscard impl::rb_node_base* impl::rb_tree_base::get_successor(rb_node_base* node) const noexcept {

  rb_node_base* x = node;
  rb_node_base* y;

  if ( !is_null(x->get_right_child()) )
    return get_minimum(x->get_right_child());

  do {
    y = x;
    x = x->get_parent();
  } while (!is_null(x) && y->is_right());
  return x;
}
VK_nodiscard impl::rb_node_base* impl::rb_tree_base::get_predecessor(rb_node_base* node) const noexcept {
  rb_node_base* x = node;
  rb_node_base* y;

  if ( !is_null(x->get_left_child()) )
    return get_maximum(x->get_left_child());

  do {
    y = x;
    x = x->get_parent();
  } while (!is_null(x) && y->is_left());
  return x;
}

void impl::rb_tree_base::left_rotate(rb_node_base *node) noexcept {

  rb_node_base* y = node->get_right_child();

  switch (node->get_position()) {
    case rb_node_base::position::root:
      set_root(y);
      break;
    case rb_node_base::position::left:
      node->get_parent()->set_left_child(y);
      break;
    case rb_node_base::position::right:
      node->get_parent()->set_right_child(y);
      break;
    VK_no_default;
  }
  node->set_right_child(y->get_left_child());
  y->set_left_child(node);
}
void impl::rb_tree_base::right_rotate(rb_node_base *node) noexcept {
  auto* y = node->get_left_child();

  switch (node->get_position()) {
    case rb_node_base::position::root:
      set_root(y);
      break;
    case rb_node_base::position::left:
      node->get_parent()->set_left_child(y);
      break;
    case rb_node_base::position::right:
      node->get_parent()->set_right_child(y);
      break;
    VK_no_default;
  }
  node->set_left_child(y->get_right_child());
  y->set_right_child(node);
}


#if 0
template <typename T, weak_order<const T&, const T&> Order = std::compare_three_way>
class RBTree;

class RBTreeBase{
protected:

  class RBNodeBase{
    using Node = RBNodeBase;

  public:
    enum class Colour   : u8 { Red,  Black };
    enum class Position : u8 { Root, Left, Right };



  private:

    int             leftOffset   = 0;
    int             rightOffset  = 0;
    int             parentOffset = 0;
    u16             chunkHint;
    Colour          colour       = Colour::Red;
    Position        position;

    inline const Node* doGetLeftChild() const noexcept {
      return reinterpret_cast<const Node*>((reinterpret_cast<const std::byte*>(this)) + leftOffset);
    }
    inline const Node* doGetRightChild() const noexcept {
      return reinterpret_cast<const Node*>((reinterpret_cast<const std::byte*>(this)) + rightOffset);
    }
    inline const Node* doGetParent() const noexcept {
      return reinterpret_cast<const Node*>((reinterpret_cast<const std::byte*>(this)) + parentOffset);
    }

    inline int getOffsetFrom(const Node* other) const noexcept {
      const i64 nodeOffset = reinterpret_cast<const std::byte*>(this) - reinterpret_cast<const std::byte*>(other);
      assert(nodeOffset <= std::numeric_limits<int>::max());
      assert(std::numeric_limits<int>::min() <= nodeOffset);
      return static_cast<int>(nodeOffset);
    }

    inline int leftOffsetFrom(const Node* other) const noexcept {
      return leftOffset + getOffsetFrom(other);
    }
    inline int rightOffsetFrom(const Node* other) const noexcept {
      return rightOffset + getOffsetFrom(other);
    }
    inline int parentOffsetFrom(const Node* other) const noexcept {
      return parentOffset + getOffsetFrom(other);
    }

  protected:

    explicit RBNodeBase(ChunkHint chunkHint) noexcept
        : chunkHint(u16(chunkHint)), colour(Colour::Black), position(Position::Root){}
    RBNodeBase(RBNodeBase* parent, ChunkHint chunkHint, Position pos) noexcept
        : chunkHint(u16(chunkHint)), position(pos){
      parent->setChild(this);
    }

    ~RBNodeBase() = default;

    RBNodeBase& operator=(RBNodeBase&&) noexcept {
      // Implemented as a NOOP so that move assignment of the derived Node type does not modify any of the node's metadata in any way
      return *this;
    }

    inline void setParent(Node* other) noexcept {
      parentOffset = other->getOffsetFrom(this);
    }
    inline void setRightChild(Node* other) noexcept {
      rightOffset = other->getOffsetFrom(this);
      other->setParent(this);
      other->position = Position::Right;
    }
    inline void setLeftChild(Node* other) noexcept {
      leftOffset = other->getOffsetFrom(this);
      other->setParent(this);
      other->position = Position::Left;
    }

    // Deduce right/left from other->position
    inline void setChild(Node* other) noexcept {
      assert(other->position != Position::Root);

      if (other->position == Position::Right){
        rightOffset = other->getOffsetFrom(this);
        other->setParent(this);
      } else {
        leftOffset = other->getOffsetFrom(this);
        other->setParent(this);
      }
    }
    inline void setChild(Node* other, Position pos) noexcept {
      assert(pos != Position::Root);
      if (pos == Position::Right)
        setRightChild(other);
      else
        setLeftChild(other);
    }


    inline void leftRotate(RBTreeBase* tree) noexcept {
      assert(isBranch());

      auto* y = getRightChild();
      switch (position) {
        case Position::Root:
          y->makeRoot(tree);
          break;
        case Position::Left:
          getParent()->setLeftChild(y);
          break;
        case Position::Right:
          getParent()->setRightChild(y);
          break;
        default:
          __assume(false);
          assert(false && "Node::position contained an invalid value");
      }
      setRightChild(y->getLeftChild());
      y->setLeftChild(this);
    }
    inline void rightRotate(RBTreeBase* tree) noexcept {
      assert(isBranch());

      auto* y = getLeftChild();

      switch (position) {
        case Position::Root:
          y->makeRoot(tree);
          break;
        case Position::Left:
          getParent()->setLeftChild(y);
          break;
        case Position::Right:
          getParent()->setRightChild(y);
          break;
        default:
          __assume(false);
          assert(false && "Node::position contained an invalid value");
      }
      setLeftChild(y->getRightChild());
      y->setRightChild(this);
    }
    inline void rotate(RBTreeBase* tree, Position pos) noexcept {
      if (pos == Position::Left)
        leftRotate(tree);
      else
        rightRotate(tree);
    }
    inline void reverseRotate(RBTreeBase* tree, Position pos) noexcept {
      if (pos == Position::Left)
        rightRotate(tree);
      else
        leftRotate(tree);
    }

    inline void  doInsert(RBTreeBase* tree) noexcept {

      Node* x = this;
      Node* y;
      Node* p;
      Node* pp;

      while (!x->isRoot() && (p = x->getParent())->isRed()) {
        pp = p->getParent();
        if (p->position == Position::Left) {
          y = pp->getRightChild();
          if (y->isRed()) {
            p->setBlack();
            y->setBlack();
            pp->setRed();
            x = pp;
          }
          else {
            if (x->position == Position::Right) {
              x = p;
              x->leftRotate(tree);
            }
            Node* _p = x->getParent();
            Node* _pp = _p->getParent();
            _p->setBlack();
            _pp->setRed();
            _pp->rightRotate(tree);
          }
        }
        else {
          y = pp->getLeftChild();
          if ( y->isRed() ) {
            p->setBlack();
            y->setBlack();
            pp->setRed();
            x = pp;
          }
          else {
            if (x->position == Position::Left) {
              x = p;
              x->rightRotate(tree);
            }
            Node* _p = x->getParent();
            Node* _pp = _p->getParent();
            _p->setBlack();
            _pp->setRed();
            _pp->leftRotate(tree);
          }
        }
      }
      if ( tree->pRoot->isRed() ) {
        ++tree->blackDepth;
        tree->pRoot->setBlack();
      }
    }
    /*inline Node* doDelete(RBTreeBase* tree, void(* pfnMoveAssign)(void* to, void* from) noexcept) noexcept {
      Node* removedNode;
      Node* subTree;
      Node* subTreeParent;
      Position subTreePos;

      if ( !(tree->isNull(getRightChild()) || tree->isNull(getLeftChild())) ) {
        removedNode = getSuccessor();
        pfnMoveAssign(this, removedNode);
      } else {
        if ( isRoot() ) {

        }
      }

      if ( tree->isNull(getRightChild()) ) {
        subTreePos = Position::Left;
      }
      else {
        if ( !tree->isNull(getLeftChild()) ) {

        }
      }

      subTreePos = rightOffset ? Position::Right : Position::Left;;

      if ( leftOffset && rightOffset ) {
        removedNode = getSuccessor();
        pfnMoveAssign(this, removedNode);
      } else {
        if ( isRoot() ) {
          getChild(subTreePos)->makeRoot(tree);
          return this;
        }
        removedNode = this;
      }

      subTreeParent = removedNode->getParent();
      subTree = removedNode->getChild(subTreePos);
      subTreeParent->setChild(subTree, subTreePos);

      *//*if (leftOffset && rightOffset) {
          removedNode = getRightChild()->getMinimum();
          subTree     = removedNode->getRightChild();
          subTreePos  = Position::Right;
          subTreeParent = removedNode->getParent();

          subTreeParent->setChild(subTree, removedNode->position);
        } else {
          removedNode = this;
          subTreePos = leftOffset ? Position::Left : Position::Right;
          subTreeParent = getParent();
          subTree = getChild(subTreePos);
          if (isRoot()) {
            tree->pRoot = subTree;
            if (subTree)
              subTree->position = Position::Root;
          } else {
            subTreeParent->setChild(subTree, position);
          }

        }*//*

        if (removedNode->isBlack())
          doPostDeleteRebalance(tree, subTreeParent, subTreePos);

        return removedNode;
      }*/
    inline static void doPostDeleteRebalance(RBTreeBase* tree, Node* node) noexcept {

      Node* parent;
      Node* sibling;

      if ( tree->pRoot != node && node->isBlack() ) {

        parent = node->getParent();

        if ( node->position == Position::Left ) {
          sibling = parent->getRightChild();

          if ( sibling->isRed() ) {
            sibling->setBlack();
            parent->setRed();
            parent->leftRotate(tree);
            parent = node->getParent();
            sibling = parent->getRightChild();
          }

          if ( sibling->getRightChild()->isBlack() ) {
            if ( sibling->getLeftChild()->isBlack() ) {
              sibling->setRed();
              return doPostDeleteRebalance(tree, parent);
            }
            sibling->getLeftChild()->setBlack();
            sibling->setRed();
            sibling->rightRotate(tree);
            sibling = parent->getRightChild();
          }

          sibling->colour = parent->colour;
          parent->setBlack();
          sibling->getRightChild()->setBlack();
          parent->leftRotate(tree);
        }
        else {

          sibling = parent->getLeftChild();

          if ( sibling->isRed() ) {
            sibling->setBlack();
            parent->setRed();
            parent->rightRotate(tree);
            parent = node->getParent();
            sibling = parent->getLeftChild();
          }

          if ( sibling->getLeftChild()->isBlack() ) {
            if ( sibling->getRightChild()->isBlack() ) {
              sibling->setRed();
              return doPostDeleteRebalance(tree, parent);
            }
            sibling->getRightChild()->setBlack();
            sibling->setRed();
            sibling->leftRotate(tree);
            sibling = parent->getLeftChild();
          }

          sibling->colour = parent->colour;
          parent->setBlack();
          sibling->getLeftChild()->setBlack();
          parent->rightRotate(tree);
        }

        tree->pRoot->setBlack();
      }
      else
        node->setBlack();
    }
    inline static void doPostDeleteRebalance(RBTreeBase* tree, Node* parent, Position pos) noexcept {

      __assume(pos != Position::Root);

      Node* subTree = parent->getChild(pos);
      Node* sibling = parent->getSibling(pos);
      Node* subRoot = parent;

      if ( subTree->isBlackOrNull() ) {

        if ( sibling->isRed() ) {
          sibling->setBlack();
          parent->setRed();
          subRoot = sibling;
          parent->rotate(tree, pos);
          sibling = parent->getSibling(pos);
          //parent =
        }


      }

      if ( !parent ) {
        if ( tree->pRoot )
          tree->pRoot->setBlack();
      } else {
        subTree = parent->getChild(pos);
        sibling = parent->getSibling(pos);

        if ( !subTree || subTree->isBlack() ) {

          if ( sibling->isRed() ) {
            sibling->setBlack();
            parent->setRed();
            parent->rotate(tree, pos);
            //parent = node->getParent();
            sibling = parent->getSibling(pos);
          }

          if ( sibling->getRightChild()->isBlack() ) {
            if ( sibling->getLeftChild()->isBlack() ) {
              sibling->setRed();
              return doPostDeleteRebalance(tree, subRoot, pos);
            }
            sibling->getChild(pos)->setBlack();
            sibling->setRed();
            sibling->reverseRotate(tree, pos);
            sibling = parent->getSibling(pos);
          }

          sibling->colour = parent->colour;
          parent->setBlack();
          sibling->getSibling(pos)->setBlack();
          parent->rotate(tree, pos);
        }



      }

      if ( parent && (!(subTree = parent->getChild(subTreePos)) || subTree->isBlack()) ) {

        //parent = node->getParent();

        if ( node->position == Position::Left ) {
          sibling = parent->getRightChild();

          if ( sibling->isRed() ) {
            sibling->setBlack();
            parent->setRed();
            parent->leftRotate(tree);
            parent = node->getParent();
            sibling = parent->getRightChild();
          }

          if ( sibling->getRightChild()->isBlack() ) {
            if ( sibling->getLeftChild()->isBlack() ) {
              sibling->setRed();
              return doPostDeleteRebalance(tree, parent);
            }
            sibling->getLeftChild()->setBlack();
            sibling->setRed();
            sibling->rightRotate(tree);
            sibling = parent->getRightChild();
          }

          sibling->colour = parent->colour;
          parent->setBlack();
          sibling->getRightChild()->setBlack();
          parent->leftRotate(tree);
        }
        else {

          sibling = parent->getLeftChild();

          if ( sibling->isRed() ) {
            sibling->setBlack();
            parent->setRed();
            parent->rightRotate(tree);
            parent = node->getParent();
            sibling = parent->getLeftChild();
          }

          if ( sibling->getLeftChild()->isBlack() ) {
            if ( sibling->getRightChild()->isBlack() ) {
              sibling->setRed();
              return doPostDeleteRebalance(tree, parent);
            }
            sibling->getRightChild()->setBlack();
            sibling->setRed();
            sibling->leftRotate(tree);
            sibling = parent->getLeftChild();
          }

          sibling->colour = parent->colour;
          parent->setBlack();
          sibling->getLeftChild()->setBlack();
          parent->rightRotate(tree);
        }

        tree->pRoot->setBlack();
      }
      else
        node->setBlack();
    }

    inline Node* doDelete(RBTreeBase* tree, void(* pfnMoveAssign)(void* to, void* from) noexcept) noexcept {
      switch ( position ) {
        case Position::Root:
          tree->
        case Position::Right:
        case Position::Left:
          VK_no_default;
      }
    }
    inline Node* doLeftDelete(RBTreeBase* tree, void(* pfnMoveAssign)(void* to, void* from) noexcept) noexcept {

      Node* removedNode;

      if ( !tree->isNull(getRightChild())) {
        removedNode = getSuccessor(tree);
        pfnMoveAssign(this, removedNode);
      }
    }
    inline Node* doRightDelete(RBTreeBase* tree, void(* pfnMoveAssign)(void* to, void* from) noexcept) noexcept {
      if ( !tree->isNull(getLeftChild()) )
    }
    inline static void doPostDeleteRebalanceLeft(RBTreeBase* tree, Node* parent) noexcept {

      if ( !parent ) {
        if ( tree->pRoot ) {
          tree->pRoot->setBlack();
          return;
        }
      }


      Node* node    = parent->getLeftChild();
      Node* sibling = parent->getRightChild();
      Node* subRoot = parent;

      if ( node->isBlackOrNull() ) {

        if ( sibling->isRed() ) {
          sibling->setBlack();
          parent->setRed();
          subRoot = sibling;
          parent->leftRotate(tree);
          sibling = parent->getRightChild();
        }


      }

    }
    inline static void doPostDeleteRebalanceRight(RBTreeBase* tree, Node* parent) noexcept {}
  public:

    explicit RBNodeBase(std::nullptr_t, ChunkHint hint) noexcept
        : chunkHint(static_cast<u16>(hint)), colour(Colour::Black), position(Position::Root){}

    [[nodiscard]] inline bool  isRoot()   const noexcept {
      //assert(position == Position::Root);
      return parentOffset == 0;
    }
    [[nodiscard]] inline bool  isLeaf()   const noexcept {
      return leftOffset == 0 && rightOffset == 0;
    }
    [[nodiscard]] inline bool  isBranch() const noexcept {
      return leftOffset != 0 && rightOffset != 0;
    }


    /*[[nodiscard]] inline Node* getLeftChild() const noexcept {
      assert(leftOffset != 0);
      const auto* result = doGetLeftChild();
      assert(result != this);
      assert(result->position == Position::Left);
      return const_cast<Node*>(result);
    }
    [[nodiscard]] inline Node* getRightChild() const noexcept {
      assert(rightOffset != 0);
      const auto* result = doGetRightChild();
      assert(result != this);
      assert(result->position == Position::Right);
      return const_cast<Node*>(result);
    }
    [[nodiscard]] inline Node* getParent() const noexcept {
      assert(!isRoot());
      const auto* result = doGetParent();
      assert(result != this);
      assert((position == Position::Left && result->doGetLeftChild() == this) || (result->doGetRightChild() == this));
      return const_cast<Node*>(result);
    }*/


    [[nodiscard]] inline Node* getChild(Position pos) const noexcept {
      if (pos == Position::Right)
        return getRightChild();
      return getLeftChild();
    }
    [[nodiscard]] inline Node* getSibling(Position pos) const noexcept {
      if (pos == Position::Left)
        return getRightChild();
      return getLeftChild();
    }
    [[nodiscard]] inline Node* getLeftChild() const noexcept {
      /*if (leftOffset)
        return const_cast<Node*>(doGetLeftChild());
      return nullptr;*/
      return const_cast<Node*>(doGetLeftChild());
    }
    [[nodiscard]] inline Node* getRightChild() const noexcept {
      /*if (rightOffset)
        return const_cast<Node*>(doGetRightChild());
      return nullptr;*/
      return const_cast<Node*>(doGetRightChild());
    }
    [[nodiscard]] inline Node* getParent() const noexcept {
      /*if (parentOffset != 0)
        return const_cast<Node*>(doGetParent());
      return nullptr;*/
      return const_cast<Node*>(doGetParent());
    }

    [[nodiscard]] inline Node* getMaximum() const noexcept {

      if (this_is_null())
        return nullptr;

      const Node* x = this;
      while (x->rightOffset)
        x = x->getRightChild();
      return const_cast<Node*>(x);
    }
    [[nodiscard]] inline Node* getMinimum() const noexcept {

      if (this_is_null())
        return nullptr;

      const Node* x = this;

      while (x->leftOffset)
        x = x->getLeftChild();
      return const_cast<Node*>(x);
    }
    [[nodiscard]] inline Node* getSuccessor() const noexcept {

      if (this_is_null())
        return nullptr;

      if (rightOffset)
        return getRightChild()->getMinimum();
      const Node* x = this;
      const Node* y;
      do {
        y = x;
        x = x->getParent();
      } while(x && y->position == Position::Right);
      return const_cast<Node*>(x);
    }
    [[nodiscard]] inline Node* getPredecessor() const noexcept{

      if (this_is_null())
        return nullptr;

      if (leftOffset)
        return getLeftChild()->getMaximum();
      const Node* x = this;
      const Node* y;
      do {
        y = x;
        x = x->getParent();
      } while(x && y->position == Position::Left);
      return const_cast<Node*>(x);
    }

    [[nodiscard]] inline Node* getMaximum(RBTreeBase* tree) const noexcept {


      const Node* x = this;
      const Node* y = x->getRightChild();

      while ( !tree->isNull(y) ) {
        x = y;
        y = y->getRightChild();
      }
      return const_cast<Node*>(x);
    }
    [[nodiscard]] inline Node* getMinimum(RBTreeBase* tree) const noexcept {

      const Node* x = this;
      const Node* y = x->getLeftChild();

      while ( !tree->isNull(y) ) {
        x = y;
        y = y->getLeftChild();
      }
      return const_cast<Node*>(x);
    }
    [[nodiscard]] inline Node* getSuccessor(RBTreeBase* tree) const noexcept {

      if (const Node* rightChild = getRightChild(); !tree->isNull(rightChild) )
        return rightChild->getMinimum();

      const Node* x = this;
      const Node* y;
      do {
        y = x;
        x = x->getParent();
      } while( !tree->isNull(x) && y->position == Position::Right);
      return const_cast<Node*>(x);
    }
    [[nodiscard]] inline Node* getPredecessor(RBTreeBase* tree) const noexcept{

      if (const Node* leftChild = getLeftChild(); !tree->isNull(leftChild) )
        return leftChild->getMaximum();

      const Node* x = this;
      const Node* y;
      do {
        y = x;
        x = x->getParent();
      } while( !tree->isNull(x) && y->position == Position::Left);
      return const_cast<Node*>(x);
    }

    inline void  setRed() noexcept {
      colour = Colour::Red;
    }
    inline void  setBlack() noexcept {
      colour = Colour::Black;
    }

    inline void makeRoot(RBTreeBase* pTree) noexcept {
      pTree->pRoot = this;
      position = Position::Root;
      setParent(pTree->pNull);
    }

    [[nodiscard]] inline bool  isRed() const noexcept {
      return colour == Colour::Red;
    }
    [[nodiscard]] inline bool  isBlack() const noexcept {
      return colour == Colour::Black;
    }

    [[nodiscard]] inline ChunkHint getAllocHint() const noexcept {
      return ChunkHint(chunkHint);
    }

    [[nodiscard]] inline std::pair<u32, bool> checkTreeInvariants(const RBTreeBase* tree) const noexcept{

      if ( tree->isNull(this) )
        return { 0, true };

      auto [ leftDepth,  leftSuccess ]  = getLeftChild()->checkTreeInvariants(tree);
      auto [ rightDepth, rightSuccess ] = getRightChild()->checkTreeInvariants(tree);

      if ( !leftSuccess || !rightSuccess ) {
        assert( false );
        return { 0, false };
      }

      if ( leftDepth != rightDepth ) {
        assert( false );
        return { 0, false };
      }

      if (isRed()) {
        if ( getLeftChild()->isRed() ) {
          assert( false );
          return { 0, false };
        }
        if ( getRightChild()->isRed() ) {
          assert( false );
          return { 0, false };
        }
        return { leftDepth, true };
      }

      return { leftDepth + 1, true };
    }
  };


  RBNodeBase* pRoot;
  RBNodeBase* pNull;
  u32         blackDepth;
  u32         nodeCount;


  [[nodiscard]] inline bool isNull(const RBNodeBase* node) const noexcept {
    return node == pNull;
  }

public:

  [[nodiscard]] bool checkInvariants() const noexcept {
    if ( !pRoot )
      return true;

    if ( pRoot->isRed() ) {
      assert( false );
      return false;
    }
    auto [ treeDepth, success ] = pRoot->checkTreeInvariants(this);

    if ( !success )
      return false;

    if ( treeDepth != blackDepth ) {
      assert( false );
      return false;
    }

    return true;
  }

};

template <typename T, weak_order<const T&, const T&> Order>
class RBTree : public RBTreeBase{
  class RBNode : public RBNodeBase{

  public:

    T value_;


    explicit RBNode(ChunkHint hint) : RBNodeBase(hint), value_(){}

    template <typename ...Args> requires(std::constructible_from<T, Args...>)
    explicit RBNode(ChunkHint hint, Args&& ...args)
    noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : RBNodeBase(hint),
    value_(std::forward<Args>(args)...){}
    template <typename ...Args> requires(std::constructible_from<T, Args...>)
    RBNode(RBNode* parentNode, ChunkHint hint, Position pos, Args&& ...args)
    noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : RBNodeBase(parentNode, hint, pos),
    value_(std::forward<Args>(args)...){}

    RBNode(RBNode&& other) noexcept = default;
    RBNode& operator=(RBNode&& other) noexcept = default;


    template <typename F>
    RBNode* find(const RBTree<T>* tree, F&& eqFunc) noexcept {

      if (tree->isNull(this))
        return this;

      auto result = eqFunc(value_);
      if (result == std::weak_ordering::equivalent)
        return this;

      return (result == std::weak_ordering::less ?
              leftChild() :
              rightChild()
      )->find(tree, std::forward<F>(eqFunc));
    }

    template <typename F, typename ...Args>
    std::pair<RBNode*, bool> insert(RBTree<T>* tree, F&& func, Args&& ...args) noexcept {

      auto result = func(value_);

      if (result == std::weak_ordering::equivalent)
        return { this, false };

      auto pos = result == std::weak_ordering::less ? Position::Left : Position::Right;
      auto* childNode = child(pos);
      if ( tree->isNull(childNode) ) {
        childNode = tree->newNode(this, pos, std::forward<Args>(args)...);
        assert( childNode );
        childNode->doInsert(tree);
        return { childNode, true };
      }
      else
        return childNode->insert(tree, std::forward<F>(func), std::forward<Args>(args)...);
    }

    [[nodiscard]] RBNode* remove(RBTree<T>* tree) noexcept {
      return static_cast<RBNode*>(this->doDelete(tree, [](void* to, void* from) noexcept {
          (*static_cast<RBNode*>(to)) = std::move(*static_cast<RBNode*>(from));
      }));
    }


    RBNode* successor() const noexcept {
      return static_cast<RBNode*>(this->getSuccessor());
    }
    RBNode* predecessor() const noexcept {
      return static_cast<RBNode*>(this->getPredecessor());
    }
    RBNode* minimum() const noexcept {
      return static_cast<RBNode*>(this->getMinimum());
    }
    RBNode* maximum() const noexcept {
      return static_cast<RBNode*>(this->getMaximum());
    }

    RBNode* leftChild() const noexcept {
      return static_cast<RBNode*>(getLeftChild());
    }
    RBNode* rightChild() const noexcept {
      return static_cast<RBNode*>(getRightChild());
    }
    RBNode* child(Position pos) const noexcept {
      return static_cast<RBNode*>(getChild(pos));
    }

  };

  template <typename QualifiedType>
  class NodeIterator{


    inline void inc() noexcept {
      if ( *this )
        node = node->successor();
      else
        node = tree->root()->minimum();
    }
    inline void dec() noexcept {
      if ( *this )
        node = node->predecessor();
      else
        node = tree->root()->maximum();
    }

  public:

    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = QualifiedType;
    using reference  = value_type&;
    using const_reference = const value_type&;
    using pointer    = value_type*;
    using const_pointer = const value_type*;

    using tree_type = std::conditional_t<std::is_const_v<value_type>, const RBTree, RBTree>;
    using node_type = std::conditional_t<std::is_const_v<value_type>, const RBNode, RBNode>;

    NodeIterator() = default;
    NodeIterator(tree_type* tree, node_type* node) noexcept
        : tree(tree), node(node){}

    template <typename OtherT> requires(std::assignable_from<QualifiedType*, OtherT*>)
    NodeIterator(const NodeIterator<OtherT>& other) noexcept
        : tree(other.tree), node(other.node){}


    reference operator*() const noexcept  {
      assert( *this );
      return node->value_;
    }
    pointer   operator->() const noexcept {
      assert( *this );
      return std::addressof(node->value_);
    }

    NodeIterator& operator++() noexcept {
      this->inc();
      return *this;
    }
    NodeIterator  operator++(int) noexcept {
      NodeIterator copy = *this;
      this->inc();
      return copy;
    }
    NodeIterator& operator--() noexcept {
      this->dec();
      return *this;
    }
    NodeIterator  operator--(int) noexcept {
      NodeIterator copy = *this;
      this->dec();
      return copy;
    }


    explicit operator bool() const noexcept {
      return !tree->isNull(node);
    }

    friend bool                  operator==(NodeIterator a, NodeIterator b) noexcept {
      return a.node == b.node;
    }
    friend std::partial_ordering operator<=>(NodeIterator a, NodeIterator b) noexcept {
      if (a.tree != b.tree)
        return std::partial_ordering::unordered;
      if ( !(a && b) ) {
        if ( a )
          return std::partial_ordering::less;
        if ( b )
          return std::partial_ordering::greater;
        return std::partial_ordering::equivalent;
      }
      return a.tree->getOrder()(a.node->value_, b.node->value_);
    }

    tree_type* tree = nullptr;
    node_type* node = nullptr;
  };




  template <typename ...Args> requires(std::constructible_from<T, Args...>)
  RBNode* newNode(Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
      auto&& [addr, allocHint] = nodeAllocator.allocateWithHint();
      return new(addr) RBNode(allocHint, std::forward<Args>(args)...);
      //return new(nodeAllocator.)
  }
  template <typename ...Args> requires(std::constructible_from<T, Args...>)
  RBNode* newNode(RBNode* parentNode, typename RBNode::Position pos, Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
      auto&& [addr, allocHint] = nodeAllocator.allocateWithHint();
      return new(addr) RBNode(parentNode, allocHint, pos, std::forward<Args>(args)...);
      //return new(nodeAllocator.)
  }

  void deleteNode(RBNode* pNode) noexcept {
    pNode->~RBNode();
    nodeAllocator.deallocate(pNode, pNode->getAllocHint());
  }

  void destroyTree() noexcept {
    destroySubTree(root());
    nodeAllocator.release();
  }
  void destroySubTree(RBNode* subRoot) noexcept {
    if (subRoot != pNull) {
      destroySubTree(subRoot->leftChild());
      destroySubTree(subRoot->rightChild());
      subRoot->~RBNode();
    }
  }

  void init() noexcept {
    ChunkHint nullHint;
    std::tie((void*&)pNull, nullHint) = nodeAllocator.allocateWithHint();
    ::new(pNull) RBNodeBase(nullptr, nullHint);
    pRoot = pNull;
  }

  /*template <typename F>
  void invokeForEachNode(F&& func) noexcept {

  }*/

  /*template <typename Ret, typename ...Args>
  Ret invokeOnRoot(Ret(RBNode::* pfnOp)(Args...), std::type_identity_t<Args>... args) noexcept {
    return std::forward<Ret>(())
  }
  template <typename Ret, typename ...Args>
  Ret invokeOnRoot(Ret(RBNode::* pfnOp)(Args...) const, std::type_identity_t<Args>... args) noexcept {

  }
  template <typename Ret, typename ...Args>
  Ret invokeOnRoot(Ret(RBNode::* pfnOp)(Args...) const, std::type_identity_t<Args>... args) const noexcept {

  }*/

  [[nodiscard]] RBNode* root() const noexcept {
    return static_cast<RBNode*>(pRoot);
  }
  [[nodiscard]] RBNode* null() const noexcept {
    return static_cast<RBNode*>(pNull);
  }


  template <typename F>
  inline bool doErase(F&& func) noexcept {

    RBNode* result = root()->find(this, std::forward<F>(func));

    if ( isNull(result) )
      return false;

    deleteNode(result->remove(this));
    return true;
  }


public:

  using value_type = T;
  using allocator_type = FixedSizeAllocator<meta::FixedChunkSize<RBNode>::value>;
  using iterator = NodeIterator<T>;
  using const_iterator = NodeIterator<const T>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;


  RBTree(Order order = {})
      : RBTreeBase(),
        nodeAllocator(std::pmr::get_default_resource()),
        order(order){
    init();
  }

  RBTree(const RBTree& other);

  explicit RBTree(std::pmr::memory_resource* pMemory, Order order = {}) noexcept
      : RBTreeBase(),
  nodeAllocator(pMemory),
  order(order){
    init();
  }


  ~RBTree() {
    destroyTree();
  }



  [[nodiscard]] size_t          size() const noexcept {
    return nodeCount;
  }


  [[nodiscard]] iterator        begin()       noexcept {
    return iterator{ this, root()->minimum() };
  }
  [[nodiscard]] const_iterator  begin() const noexcept {
    return const_iterator{ this, root()->minimum() };
  }
  [[nodiscard]] const_iterator cbegin() const noexcept {
    return this->begin();
  }


  [[nodiscard]] iterator        end()       noexcept {
    return iterator{ this, null() };
  }
  [[nodiscard]] const_iterator  end() const noexcept {
    return const_iterator{ this, null() };
  }
  [[nodiscard]] const_iterator cend() const noexcept {
    return this->end();
  }


  [[nodiscard]] reverse_iterator        rbegin()       noexcept {
    return reverse_iterator(this->end());
  }
  [[nodiscard]] const_reverse_iterator  rbegin() const noexcept {
    return const_reverse_iterator(this->end());
  }
  [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(this->end());
  }

  [[nodiscard]] reverse_iterator        rend()       noexcept {
    return reverse_iterator(this->begin());
  }
  [[nodiscard]] const_reverse_iterator  rend() const noexcept {
    return const_reverse_iterator(this->begin());
  }
  [[nodiscard]] const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(this->cbegin());
  }


  const_iterator find(const T& value) const noexcept {
    return const_iterator{ this, root()->find(this, [&](const T& other) noexcept { return order(value, other); }) };
  }
  template <typename Key, std::strict_weak_order<const Key&, const T&> KeyOrder = Order>
  const_iterator find(const Key& key, KeyOrder&& keyOrder = {}) const noexcept {
    return const_iterator{ this, root()->find(this, [&](const T& other) noexcept { return keyOrder(key, other); }) };
  }

  std::pair<iterator, bool> insert(const T& value) noexcept {
    if ( isNull(root()) ) {
      newNode(value)->makeRoot(this);
      nodeCount = 1;
      blackDepth = 1;
      return { begin(), true };
    }
    std::pair<iterator, bool> result;
    RBNode* pResultNode;
    std::tie(pResultNode, result.second) = root()->insert(this, [&](const T& other) noexcept {
        return order(value, other);
    }, value);

    nodeCount += result.second;

    expensive_assert(checkInvariants());

    result.first = iterator(this, pResultNode);
    return result;
  }
  bool                      remove(const T& value) noexcept {
    bool result = this->doErase([&](const T& other){ return order(value, other); });
    this->nodeCount -= result;
    expensive_assert(checkInvariants());
    return result;
  }

  const Order& getOrder() const noexcept {
    return order;
  }


private:
  allocator_type              nodeAllocator;
  [[no_unique_address]] Order order;
};



class AddressIntervalTree{
  class Node {
  public:
    enum class Colour   : unsigned short{ Red,  Black };
    enum class Position : unsigned short{ Root, Left, Right };

  private:
    AddressInterval interval;
    int             leftOffset;
    int             rightOffset;
    int             parentOffset;
    Colour          colour;
    Position        position;

    inline const Node* doGetLeftChild() const noexcept {
      return reinterpret_cast<const Node*>((reinterpret_cast<const std::byte*>(this)) + leftOffset);
    }
    inline const Node* doGetRightChild() const noexcept {
      return reinterpret_cast<const Node*>((reinterpret_cast<const std::byte*>(this)) + rightOffset);
    }
    inline const Node* doGetParent() const noexcept {
      return reinterpret_cast<const Node*>((reinterpret_cast<const std::byte*>(this)) + parentOffset);
    }

    inline int getOffsetFrom(const Node* other) const noexcept {
      const i64 nodeOffset = reinterpret_cast<const std::byte*>(this) - reinterpret_cast<const std::byte*>(other);
      assert(nodeOffset <= std::numeric_limits<int>::max());
      assert(std::numeric_limits<int>::min() <= nodeOffset);
      return static_cast<int>(nodeOffset);
    }

    inline void setParent(Node* other) noexcept {
      parentOffset = other->getOffsetFrom(this);
    }
    inline void setRightChild(Node* other) noexcept {
      rightOffset = other->getOffsetFrom(this);
      other->setParent(this);
    }
    inline void setLeftChild(Node* other) noexcept {
      leftOffset = other->getOffsetFrom(this);
      other->setParent(this);
    }

    inline void makeRoot(AddressIntervalTree* pTree) noexcept {
      parentOffset = 0;
      position = Position::Root;
      pTree->setRoot(this);
    }

    inline int leftOffsetFrom(const Node* other) const noexcept {
      return leftOffset + getOffsetFrom(other);
    }
    inline int rightOffsetFrom(const Node* other) const noexcept {
      return rightOffset + getOffsetFrom(other);
    }
    inline int parentOffsetFrom(const Node* other) const noexcept {
      return parentOffset + getOffsetFrom(other);
    }

  public:

    inline void leftRotate(AddressIntervalTree* pTree) noexcept {
      assert(isBranch());
      assert(getRightChild()->isBranch());

      auto* y = getRightChild();
      setRightChild(y->getLeftChild());
      switch (position) {
        case Position::Root:
          y->makeRoot(pTree);
          break;
        case Position::Left:
          getParent()->setLeftChild(y);
          break;
        case Position::Right:
          getParent()->setRightChild(y);
          break;
        default:
          __assume(false);
          assert(false && "Node::position contained an invalid value");
      }
      y->setLeftChild(this);
    }
    inline void rightRotate(AddressIntervalTree* pTree) noexcept {
      assert(isBranch());
      assert(getLeftChild()->isBranch());

      auto* y = getLeftChild();
      setLeftChild(y->getRightChild());
      switch (position) {
        case Position::Root:
          y->makeRoot(pTree);
          break;
        case Position::Left:
          getParent()->setLeftChild(y);
          break;
        case Position::Right:
          getParent()->setRightChild(y);
          break;
        default:
          __assume(false);
          assert(false && "Node::position contained an invalid value");
      }
      y->setRightChild(this);
    }


    inline bool  isRoot()   const noexcept {
      assert(position == Position::Root);
      return parentOffset == 0;
    }
    inline bool  isLeaf()   const noexcept {
      return leftOffset == 0 && rightOffset == 0;
    }
    inline bool  isBranch() const noexcept {
      return leftOffset != 0 && rightOffset != 0;
    }

    inline Node* getLeftChild() noexcept {
      //assert(isBranch());
      const auto* result = doGetLeftChild();
      assert(result != this);
      assert(result->position == Position::Left);
      return const_cast<Node*>(result);
    }
    inline Node* getRightChild() noexcept {
      //assert(isBranch());
      const auto* result = doGetRightChild();
      assert(result != this);
      assert(result->position == Position::Right);
      return const_cast<Node*>(result);
    }
    inline Node* getParent() noexcept {
      assert(!isRoot());
      const auto* result = doGetParent();
      assert(result != this);
      assert((position == Position::Left && result->doGetLeftChild() == this) || (result->doGetRightChild() == this));
      return const_cast<Node*>(result);
    }

    inline void  setRed() noexcept {
      colour = Colour::Red;
    }
    inline void  setBlack() noexcept {
      colour = Colour::Black;
    }

    inline bool  isRed() const noexcept {
      return colour == Colour::Red;
    }
    inline bool  isBlack() const noexcept {
      return colour == Colour::Black;
    }
  };
  Node* pRoot;
  json_fixed_size_allocator nodeAllocator;
  json_u32_t blackDepth;

  inline void setRoot(Node* pNewRoot) noexcept {
    // Unsafe, should only be called by member functions of AddressIntervalTree::Node
    pRoot = pNewRoot;
  }

  static_assert(sizeof(Node) == 32);

  inline void doInsert(Node* node) noexcept {

  }


public:

};
#endif