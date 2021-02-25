//
// Created by Maxwell on 2021-02-22.
//

#ifndef VALKYRIE_CORE_ADT_RBTREE_HPP
#define VALKYRIE_CORE_ADT_RBTREE_HPP

#include <valkyrie/Core/Traits.hpp>
#include <valkyrie/Core/Memory/SmallObjectAllocator.hpp>


#include <limits>

namespace valkyrie::Core{
  template <typename T>
  class RBTree;

  class RBTreeBase{
  protected:

    class RBNodeBase{
      using Node = RBNodeBase;

    public:
      enum class Colour   : unsigned short{ Red,  Black };
      enum class Position : unsigned short{ Root, Left, Right };

    private:

      int             leftOffset   = 0;
      int             rightOffset  = 0;
      int             parentOffset = 0;
      Colour          colour       = Colour::Red;
      Position        position;

      inline const Node* doGetLeftChild() const noexcept {
        return reinterpret_cast<const Node*>((reinterpret_cast<const byte*>(this)) + leftOffset);
      }
      inline const Node* doGetRightChild() const noexcept {
        return reinterpret_cast<const Node*>((reinterpret_cast<const byte*>(this)) + rightOffset);
      }
      inline const Node* doGetParent() const noexcept {
        return reinterpret_cast<const Node*>((reinterpret_cast<const byte*>(this)) + parentOffset);
      }

      inline int getOffsetFrom(const Node* other) const noexcept {
        const ptrdiff_t nodeOffset = reinterpret_cast<const byte*>(this) - reinterpret_cast<const byte*>(other);
        VK_assert(nodeOffset <= std::numeric_limits<int>::max());
        VK_assert(std::numeric_limits<int>::min() <= nodeOffset);
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

      inline int safeLeftOffsetFrom(const Node* other) const noexcept {
        return leftOffset ? leftOffsetFrom(other): 0;
      }
      inline int safeRightOffsetFrom(const Node* other) const noexcept {
        return rightOffset ? rightOffsetFrom(other) : 0;
      }
      inline int safeParentOffsetFrom(const Node* other) const noexcept {
        return parentOffset ? parentOffsetFrom(other) : 0;
      }

    protected:

      explicit RBNodeBase() noexcept : position(Position::Root){}
      RBNodeBase(RBNodeBase* parent, Position pos) noexcept : position(pos){
        parent->setChild(this);
      }

      ~RBNodeBase();

      RBNodeBase& operator=(RBNodeBase&&) noexcept {
        // Implemented as a NOOP so that move assignment of the derived Node type does not modify any of the node's metadata in any way
        return *this;
      }

      inline void setParent(Node* other) noexcept {
        if (other)
          parentOffset = other->getOffsetFrom(this);
        else
          parentOffset = 0;
      }
      inline void setRightChild(Node* other) noexcept {
        if (other) {
          rightOffset = other->getOffsetFrom(this);
          other->setParent(this);
          other->position = Position::Right;
        } else
          rightOffset = 0;
      }
      inline void setLeftChild(Node* other) noexcept {
        if (other) {
          leftOffset = other->getOffsetFrom(this);
          other->setParent(this);
          other->position = Position::Left;
        } else
          leftOffset = 0;
      }

      // Deduce right/left from other->position
      inline void setChild(Node* other) noexcept {
        VK_assert(other->position != Position::Root);
        if (other->position == Position::Right){
          rightOffset = other->getOffsetFrom(this);
          other->setParent(this);
        } else {
          leftOffset = other->getOffsetFrom(this);
          other->setParent(this);
        }
      }
      inline void setChild(Node* other, Position pos) noexcept {
        VK_assert(pos != Position::Root);
        if (pos == Position::Right)
          setRightChild(other);
        else
          setLeftChild(other);
      }

      inline void makeRoot(RBTreeBase* pTree) noexcept {
        parentOffset = 0;
        position = Position::Root;
        pTree->pRoot = this;
      }



      inline void leftRotate(RBTreeBase* tree) noexcept {
        VK_assert(isBranch());

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
          VK_no_default;
        }
        setRightChild(y->getLeftChildOrNull());
        y->setLeftChild(this);
      }
      inline void rightRotate(RBTreeBase* tree) noexcept {
        VK_assert(isBranch());

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
          VK_no_default;
        }
        setLeftChild(y->getRightChildOrNull());
        y->setRightChild(this);
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
            if (y->isRed()) {
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
        tree->pRoot->setBlack();
      }
      template <typename NodeType>
      inline Node* doDelete(RBTreeBase* tree) noexcept {
        Node* removedNode;
        Node* subTree;

        if (leftOffset && rightOffset) {
          removedNode = getRightChild()->getMinimum();
          subTree     = removedNode->getRightChildOrNull();
          static_cast<NodeType&>(*this) = std::move(static_cast<NodeType&>(*removedNode));
          removedNode->getParent()->setChild(subTree, removedNode->position);
        } else {
          removedNode = this;
          subTree = leftOffset ? getLeftChild() : getRightChildOrNull();

          switch (position) {
            case Position::Root: [[unlikely]]
                                     tree->pRoot = subTree;
              if (subTree)
                subTree->position = position;
              break;
            case Position::Left:
              getParent()->setLeftChild(subTree);
              break;
            case Position::Right:
              getParent()->setRightChild(subTree);
              break;
            default:
              __assume(false);
              VK_assert(false && "Shouldn't be able to get here....");
          }
        }

        if (removedNode->isBlack())
          doPostDeleteRebalance(tree, subTree);

        return removedNode;
      }

      inline static void doPostDeleteRebalance(RBTreeBase* tree, Node* node) noexcept {

        Node* parent;
        Node* sibling;

        if ( tree->pRoot != node && node->isBlack() ) {

          parent = node->getParent();

          if ( node->position == Position::Left ) {
            sibling = parent->getRightChildOrNull();

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

            sibling = parent->getLeftChildOrNull();

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


    public:

      inline bool  isRoot()   const noexcept {
        VK_assert(position == Position::Root);
        return parentOffset == 0;
      }
      inline bool  isLeaf()   const noexcept {
        return leftOffset == 0 && rightOffset == 0;
      }
      inline bool  isBranch() const noexcept {
        return leftOffset != 0 && rightOffset != 0;
      }


      inline Node* getChild(Position pos) noexcept {
        if (pos == Position::Right)
          return getRightChild();
        return getLeftChild();
      }
      inline Node* getLeftChild() noexcept {
        VK_assert(leftOffset != 0);
        const auto* result = doGetLeftChild();
        VK_assert(result != this);
        VK_assert(result->position == Position::Left);
        return const_cast<Node*>(result);
      }
      inline Node* getRightChild() noexcept {
        VK_assert(rightOffset != 0);
        const auto* result = doGetRightChild();
        VK_assert(result != this);
        VK_assert(result->position == Position::Right);
        return const_cast<Node*>(result);
      }
      inline Node* getParent() noexcept {
        VK_assert(!isRoot());
        const auto* result = doGetParent();
        VK_assert(result != this);
        VK_assert((position == Position::Left && result->doGetLeftChild() == this) || (result->doGetRightChild() == this));
        return const_cast<Node*>(result);
      }


      inline Node* getChildOrNull(Position pos) noexcept {
        if (pos == Position::Right)
          return getRightChildOrNull();
        return getLeftChildOrNull();
      }
      inline Node* getLeftChildOrNull() noexcept {
        if (leftOffset)
          return getLeftChild();
        return nullptr;
      }
      inline Node* getRightChildOrNull() noexcept {
        if (rightOffset)
          return getRightChild();
        return nullptr;
      }
      inline Node* getParentOrNull() noexcept {
        if (parentOffset != 0)
          return getParent();
        return nullptr;
      }

      inline Node* getMaximum() noexcept {
        Node* x = this;
        while (x->rightOffset)
          x = x->getRightChild();
        return x;
      }
      inline Node* getMinimum() noexcept {
        Node* x = this;
        while (x->leftOffset)
          x = x->getLeftChild();
        return x;
      }
      inline Node* getSuccessor() noexcept {
        if (rightOffset)
          return getRightChild()->getMinimum();
        Node* x = this;
        Node* y;
        do {
          y = x;
          x = x->getParentOrNull();
        } while(x && y->position == Position::Right);
        return x;
      }
      inline Node* getPredecessor() noexcept{
        if (leftOffset)
          return getLeftChild()->getMaximum();
        Node* x = this;
        Node* y;
        do {
          y = x;
          x = x->getParentOrNull();
        } while(x && y->position == Position::Left);
        return x;
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


    RBNodeBase* pRoot;
    unsigned    blackDepth;
  };

  template <typename T>
  class RBTree : public RBTreeBase{
    class RBNode : public RBNodeBase{
      T value_;
    public:
      RBNode();
      template <typename ...Args> requires(ConstructibleFrom<T, Args...>)
      RBNode(std::in_place_t, Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
          : RBNodeBase(), value_(std::forward<Args>(args)...){}




      template <typename Key, typename F = decltype([](const Key& key, const T& t){ return key <=> t; })>
      RBNode* find(Key&& key, F&& eqFunc = {}) noexcept {
        auto result = eqFunc(key, value_);
        if (result == std::weak_ordering::equivalent)
          return this;

        auto* childNode = result == std::weak_ordering::less ? static_cast<RBNode*>(getLeftChildOrNull()) : static_cast<RBNode*>(getRightChildOrNull());
        if (!childNode)
          return childNode;
        return childNode->find(std::forward<Key>(key), std::forward<F>(eqFunc));
      }

      template <typename F>
      RBNode* insert(RBTree<T>* tree, RBNode* newNode, F&& func) noexcept {
        if (func(newNode->value_, value_)) {
          if (auto* leftChild = static_cast<RBNode*>(getLeftChildOrNull()))
            leftChild->insert(tree, newNode, std::forward<F>(func));
          else {
            setLeftChild(newNode);
            newNode->doInsert(tree);
          }
          return newNode;
        }
        else if (func(value_, newNode->value_)) {
          if (auto* rightChild = static_cast<RBNode*>(getRightChildOrNull()))
            rightChild->insert(tree, newNode, std::forward<F>(func));
          else {
            setRightChild(newNode);
            newNode->doInsert(tree);
          }
          return newNode;
        }

        return this;
      }
      RBNode* remove(RBTree<T>* tree) noexcept {
        return static_cast<RBNode*>(this->template doDelete<RBNode>(tree));
      }
    };


  };
}

#endif//VALKYRIE_CORE_ADT_RBTREE_HPP
