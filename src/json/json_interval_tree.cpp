//
// Created by Maxwell on 2021-02-18.
//

#include <json/json.h>
#include "jsonmm.h"

#include <any>
#include <bit>
#include <bitset>
#include <cassert>
#include <compare>
#include <cstddef>
#include <limits>
#include <memory_resource>
#include <optional>
#include <utility>
#include <string>
#include <typeindex>
#include <concurrent_unordered_map.h>

namespace json{

  using u32 = unsigned;
  using u8  = unsigned char;
  using u64 = unsigned long long;
  using std::byte;


  enum class ChunkHint : u32{};

  enum class Corruption{
    None,
    BlocksAvailableOutOfBounds,
    NextFreeBlockOutOfBounds,
    NextIndexOutOfBounds,
    EarlyLoopInBlockList,
    BadLoopLength,
    BadBookkeeping
  };

  /*struct Variable{
    const char* name;
    std::any    value;
  };
  class Invariant{

    using print_fn = void(*)(std::stringstream& out, const std::any& obj);

    template <typename T>
    inline constexpr static auto typed_print_fn = [](std::stringstream& out, const std::any& obj){
      if constexpr (requires(const T& t){ out << t; }) {
        out << *any_cast<T>(&obj);
      }
      else {
        out << std::to_string(*any_cast<T>(&obj));
      }
    };

  public:
    std::string                statement;
    std::optional<std::string> scope;
    std::vector<Variable>      variables;
    bool                       isValid;

    inline static concurrency::concurrent_unordered_map<std::type_index, print_fn> printingFunctions{};

    template <typename T>
    void insertVariable(const char* varName, T&& value) noexcept {
      using RealT = std::remove_cvref_t<T>;
      printingFunctions.insert({ std::type_index(typeid(RealT)), typed_print_fn<RealT> });
      variables.push_back({ varName, std::forward<T>(value) });
    }
  };
  class FunctionInvariants{
    std::string functionName;
    std::vector<Variable>      variables;
  };
  class ClassInvariants{

    using print_fn = void(*)(std::stringstream& out, const std::any& obj);

    template <typename T>
    inline constexpr static auto typed_print_fn = [](std::stringstream& out, const std::any& obj){
      if constexpr (requires(const T& t){ out << t; }) {
        out << *any_cast<T>(&obj);
      }
      else {
        out << std::to_string(*any_cast<T>(&obj));
      }
    };


    std::vector<std::pair<const char*, bool>> invariants;
    std::string                               className;
    std::vector<Variable>                     variables;

    inline static concurrency::concurrent_unordered_map<std::type_index, print_fn> printingFunctions{};
  public:
    template <typename T>
    void insertVariable(const char* varName, T&& value) noexcept {
      using RealT = std::remove_cvref_t<T>;
      printingFunctions.insert({ std::type_index(typeid(RealT)), typed_print_fn<RealT> });
      variables.push_back({ varName, std::forward<T>(value) });
    }
  };*/



  template <size_t BlockSize, u8 BlocksPerChunk = u8(256)>
  class FixedSizeAllocator{

    static_assert(BlockSize && !(BlockSize & (BlockSize - 1)),
                  "FixedSizeAllocator<BlockSize, BlocksPerChunk> -> BlockSize must be a power of 2");


    inline constexpr static size_t ChunkMemorySize = BlockSize * BlocksPerChunk;
    inline constexpr static size_t ChunkMemoryAlignment = std::min(BlockSize, alignof(std::max_align_t));
    inline constexpr static size_t BlockSizeLog2 = std::countr_zero(BlockSize);

    struct Chunk{

      u8    availableBlocks = BlocksPerChunk;
      u8    nextFreeBlock   = 0;
      byte* pMemory         = nullptr;

      void init(std::pmr::memory_resource* allocator) noexcept {
        pMemory = static_cast<std::byte*>(allocator->allocate(ChunkMemorySize, ChunkMemoryAlignment));
        u8 i{0x00};
        byte* p = pMemory;
        for(; i < BlocksPerChunk; p += BlockSize)
          *p = byte(++i);
      }
      void destroy(std::pmr::memory_resource* allocator) noexcept {

        assert(this->empty());

        allocator->deallocate(pMemory, ChunkMemorySize, ChunkMemoryAlignment);
        pMemory = nullptr;
      }

      Chunk(){
        this->init(std::pmr::get_default_resource());
      }
      explicit Chunk(std::pmr::memory_resource* pResource) {
        this->init(pResource);
      }

      Chunk(const Chunk&) = delete;
      Chunk(Chunk&& other) noexcept : availableBlocks(0), nextFreeBlock(0), pMemory(nullptr){
        std::swap(availableBlocks, other.availableBlocks);
        std::swap(nextFreeBlock, other.nextFreeBlock);
        std::swap(pMemory, other.pMemory);
      }

      Chunk& operator=(const Chunk&) = delete;
      Chunk& operator=(Chunk&& other) noexcept {
        this->~ChunkHandle();
        new(this) Chunk(std::move(other));
        return *this;
      }

      ~Chunk(){
        assert(this->empty());

        if (pMemory)
          destroy(std::pmr::get_default_resource());

        //assert(!pMemory);
        /*if (pChunk) {
          allocator.deallocate(pChunk, ChunkAllocationSize);
        }*/
      }

      void* allocate() noexcept {
        if (full())
          return nullptr;
        auto* pResult = reinterpret_cast<u8*>(&pMemory[nextFreeBlock * BlockSize]);
        nextFreeBlock = *pResult;
        --availableBlocks;
        return pResult;
      }
      void  deallocate(void* p) noexcept {
        assert(contains(p));
        assert(!empty());
        auto* pRelease = static_cast<byte*>(p);
        assert((pRelease - pMemory) % BlockSize == 0);
        *pRelease = std::byte{nextFreeBlock};
        nextFreeBlock = static_cast<u8>((pRelease - pMemory) >> BlockSizeLog2);
        assert(nextFreeBlock == (pRelease - pMemory) >> BlockSizeLog2);
        ++availableBlocks;
      }
      bool  contains(void* p) const noexcept {
        const byte* const pComp = static_cast<const byte*>(p);
        return pMemory <= pComp && pComp < (pMemory + ChunkMemorySize);
      }
      bool  full() const noexcept {
        return !availableBlocks;
      }
      bool  empty() const noexcept {
        return availableBlocks == BlocksPerChunk;
      }

      Corruption checkCorruption() const noexcept {
        if (BlocksPerChunk < availableBlocks)
          return Corruption::BlocksAvailableOutOfBounds;

        if ( full() )
          return Corruption::None;

        if ( BlocksPerChunk <= nextFreeBlock )
          return Corruption::NextFreeBlockOutOfBounds;

        return checkDataCorruption();
      }
      Corruption checkDataCorruption() const noexcept {
        u8                          index = nextFreeBlock;
        std::bitset<BlocksPerChunk> foundBlocks;
        byte*                       nextBlock = nullptr;

        for (unsigned char count; ; ) {
          nextBlock = pMemory + (index * BlockSize);
          foundBlocks.set(index);
          ++count;
          if (count >= availableBlocks)
            break;

          index = u8(*nextBlock);
          if (BlocksPerChunk <= index)
            return Corruption::NextIndexOutOfBounds;
          if ( foundBlocks.test(index) )
            return Corruption::EarlyLoopInBlockList;
        }
        if ( foundBlocks.count() != availableBlocks )
          return Corruption::BadLoopLength;


        return Corruption::None;
      }
    };

    using ChunkVector = std::vector<Chunk>;

    ChunkVector chunks{};

    mutable Chunk* allocChunk   = nullptr;
    mutable Chunk* deallocChunk = nullptr;
    mutable Chunk* emptyChunk   = nullptr;

    std::pmr::memory_resource* pChunkResource = std::pmr::get_default_resource();

    /*ChunkHandle findChunk(void* p) const noexcept {
      if ( chunks.empty() )
        return nullptr;

      assert(deallocChunk != InvalidChunk);

      Chunk * lo = &chunks[deallocChunk];
      Chunk * hi = &chunks[deallocChunk + 1];
      const Chunk * loBound = &chunks.front();
      const Chunk * hiBound = &chunks.back() + 1;

      // Special case: deallocChunk_ is the last in the array
      if (hi == hiBound)
        hi = nullptr;

      for (;;)
      {
        if (lo)
        {
          if ( lo->contains( p ) )
            return lo;
          if ( lo == loBound ) {
            lo = nullptr;
            if ( !hi )
              break;
          }
          else
            --lo;
        }
        if (hi) {
          if ( hi->contains( p ) )
            return hi;
          if ( ++hi == hiBound )
          {
            hi = nullptr;
            if ( !lo )
              break;
          }
        }
      }

      return nullptr;
    }*/

    Chunk* findDeallocChunk(void* p) const noexcept {
      if ( chunks.empty() )
        return nullptr;

      assert(deallocChunk != nullptr);

      Chunk* lo = deallocChunk;
      Chunk* hi = deallocChunk + 1;
      const Chunk* loBound = &*chunks.cbegin();
      const Chunk* hiBound = &*chunks.cend();

      // Special case: deallocChunk_ is the last in the array
      if (hi == hiBound)
        hi = nullptr;

      for (;;)
      {
        if ( lo )
        {
          if ( lo->contains( p ) )
            return lo;
          if ( lo == loBound ) {
            lo = nullptr;
            if ( !hi )
              break;
          }
          else
            --lo;
        }
        if ( hi ) {
          if ( hi->contains( p ) )
            return hi;
          if ( ++hi == hiBound )
          {
            hi = nullptr;
            if ( !lo )
              break;
          }
        }
      }

      return nullptr;
    }
    size_t countEmptyChunks() const noexcept {
#if defined(VK_debug_memory)
      return std::ranges::count(chunks, [](const Chunk& chunk){ return chunk.empty(); });
#else
      return emptyChunk != nullptr;
#endif
    }
    void doDeallocate(void* p) noexcept {
      // Show that deallocChunk_ really owns the block at address p.
      assert( deallocChunk != nullptr);
      assert( deallocChunk->contains( p ) );
      // Either of the next two assertions may fail if somebody tries to
      // delete the same block twice.
      assert( emptyChunk != deallocChunk );
      assert( !deallocChunk->empty() );
      // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
      assert( ( emptyChunk == nullptr ) || emptyChunk->empty() );

      // call into the chunk, will adjust the inner list but won't release memory
      deallocChunk->deallocate(p);

      if ( deallocChunk->empty() ) {
        assert( emptyChunk != deallocChunk );

        // deallocChunk_ is empty, but a Chunk is only released if there are 2
        // empty chunks.  Since emptyChunk_ may only point to a previously
        // cleared Chunk, if it points to something else besides deallocChunk_,
        // then FixedAllocator currently has 2 empty Chunks.
        if ( emptyChunk ) {
          // If last Chunk is empty, just change what deallocChunk_
          // points to, and release the last.  Otherwise, swap an empty
          // Chunk with the last, and then release it.
          auto* poppedChunk = this->popEmptyChunk();

          if ( ( allocChunk == poppedChunk ) || emptyChunk->full() )
            allocChunk = deallocChunk;
        }
        emptyChunk = deallocChunk;
      }

      // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
      assert( ( emptyChunk == nullptr ) || emptyChunk->empty() );
    }

    Chunk* popEmptyChunk() noexcept {

      assert( !chunks.empty() );
      assert( emptyChunk && emptyChunk->empty() );

      Chunk* lastChunk = &chunks.back();

      if ( !lastChunk->empty() || lastChunk != emptyChunk )
        std::swap( *emptyChunk, *lastChunk );

      assert( lastChunk->empty() );

      lastChunk->destroy(pChunkResource);
      chunks.pop_back();

      return lastChunk;
    }


    inline static constexpr u32 InvalidChunkIndex = static_cast<u32>(-1);

  public:
    FixedSizeAllocator() = default;
    FixedSizeAllocator(std::pmr::memory_resource* pResource) noexcept : pChunkResource(pResource){}
    /*FixedSizeAllocator(std::pmr::memory_resource* pChunkVectorResource, std::pmr::memory_resource* pChunkResource) noexcept
        : chunks(0, pChunkVectorResource),
          pChunkResource(pChunkResource){}*/

    u32 ownedChunkIndex(void* p) const noexcept {
      for (u32 i = 0; i < chunks.size(); ++i)
        if (chunks[i].contains(p))
          return i;
      return InvalidChunkIndex;
    }

    bool tryDeallocate(void* p) noexcept {
      auto foundChunk = findDeallocChunk(p);
      if ( !foundChunk )
        return false;
      deallocChunk = foundChunk;
      doDeallocate(p);
      assert(countEmptyChunks() < 2);
      return true;
    }

    size_t trimEmptyChunk() {
      // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
      assert( ( !emptyChunk ) || emptyChunk->empty() );
      if ( !emptyChunk )
        return 0;

      // If emptyChunk_ points to valid Chunk, then chunk list is not empty.
      assert( !chunks.empty() );
      // And there should be exactly 1 empty Chunk.
      assert( 1 == countEmptyChunks() );

      auto poppedChunk = this->popEmptyChunk();

      if ( chunks.empty() ) {
        allocChunk = nullptr;
        deallocChunk = nullptr;
      }
      else {
        if ( deallocChunk == poppedChunk ) {
          deallocChunk = &chunks.front();
          assert( !deallocChunk->empty() );
        }
        if ( allocChunk == poppedChunk ) {
          allocChunk = &chunks.back();
          assert( !allocChunk->empty() );
        }
      }

      emptyChunk = nullptr;
      assert( 0 == countEmptyChunks() );

      return ChunkMemorySize;
    }
    size_t trimChunkList() {
      if ( chunks.empty() ) {
        assert( !allocChunk );
        assert( !deallocChunk );
      }


      const size_t startCapacity = chunks.capacity();
      chunks.shrink_to_fit();

      return (startCapacity - chunks.capacity()) * sizeof(Chunk);
    }


    void* allocate() noexcept {
      ChunkHint discardHint;
      return this->allocate(&discardHint);
    }
    void* allocate(ChunkHint* pChunkHint) noexcept {
      assert( !chunks.empty() );
      assert( pChunkHint );
      assert(deallocChunk < chunks.size());
      assert(allocChunk < chunks.size());
      assert( countEmptyChunks() < 2 );

      if ( !allocChunk || allocChunk->full()) {
        auto i         = chunks.begin();
        const auto end = chunks.end();
        for (;; ++i){
          if (i == end) {
            assert( !emptyChunk );
            chunks.emplace_back(pChunkResource);
            allocChunk   = &chunks.back();
            deallocChunk = &chunks.front();
            break;
          }
          if (!i->full()) {
            allocChunk = &*i;
            break;
          }
        }
      }
      assert( allocChunk );
      assert( !allocChunk->full() );

      if (allocChunk == emptyChunk)
        emptyChunk = nullptr;

      *pChunkHint = ChunkHint(allocChunk - &chunks.front());

      return allocChunk->allocate();
    }
    void* allocateWithHint(ChunkHint chunkHint) noexcept {
      assert(u32(chunkHint) < chunks.size());
      allocChunk = &chunks[u32(chunkHint)];
      ChunkHint discardHint;
      return this->allocate(&discardHint);
    }
    void* allocateWithHint(ChunkHint chunkHint, ChunkHint* pChunkHint) noexcept {
      assert(u32(chunkHint) < chunks.size());
      allocChunk = &chunks[u32(chunkHint)];
      return this->allocate(pChunkHint);
    }

    void  deallocate(void* p) noexcept {
      assert( countEmptyChunks() < 2 );

      auto foundChunk = findDeallocChunk( p );
      assert( foundChunk->contains( p ) );

      deallocChunk = foundChunk;
      doDeallocate( p );
      assert( countEmptyChunks() < 2 );
    }
    void  deallocate(void* p, ChunkHint chunk) noexcept {

      assert( countEmptyChunks() < 2 );

      assert( u32(chunk) < chunks.size() );

      auto foundChunk = &chunks[u32(chunk)];
      assert( foundChunk->contains(p) );

      deallocChunk = foundChunk;
      doDeallocate(p);
      assert( countEmptyChunks() < 2 );
    }


    Corruption checkCorruption(){
      const bool   isEmpty = chunks.empty();
      auto         begin   = chunks.begin();
      auto         end     = chunks.end();
      const size_t emptyChunkCount = countEmptyChunks();

      /*struct Variable{
        std::string name;
        std::string value;
      };

      struct Invariant{
        std::string statement;
        std::vector<Variable> variables;
      };

      struct AllocatorState{
        std::vector<Corruption> perChunk;
        std::vector<Invariant>  invariants;
      } state;

      state.perChunk.reserve(chunks.size());*/


      if ( isEmpty ) {
        if ( begin != end ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( emptyChunkCount ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( deallocChunk ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( allocChunk ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( emptyChunk ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
      }
      else {
        const Chunk * front = &chunks.front();
        const Chunk * back  = &chunks.back();
        if ( begin >= end ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( back < deallocChunk ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( back < allocChunk ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( front > deallocChunk ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( front > allocChunk ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }

        switch ( emptyChunkCount ) {
          case 0:
            if ( emptyChunk ) {
              assert( false );
              return Corruption::BadBookkeeping;
            }
            break;
          case 1:
            if ( !emptyChunk ) {
              assert( false );
              return Corruption::BadBookkeeping;
            }
            if ( back < emptyChunk ) {
              assert( false );
              return Corruption::BadBookkeeping;
            }
            if ( front > emptyChunk ) {
              assert( false );
              return Corruption::BadBookkeeping;
            }
            if ( !emptyChunk->empty() ) {
              assert( false );
              return Corruption::BadBookkeeping;
            }
            break;
          default:
            assert(false);
            return Corruption::BadBookkeeping;
        }

        for (const Chunk& chunk : chunks ) {
          if (auto result = chunk.checkCorruption(); result != Corruption::None)
            return result;
        }
      }
      return Corruption::None;
    }
  };
  
  struct AddressInterval{
    void* min;
    void* max;
  };

  namespace meta{
    template <size_t N>
    struct CeilLog2 : std::integral_constant<size_t, CeilLog2<(N | (N - 1)) + 1>::value>{};
    template <size_t N> requires(!(N & (N - 1)))
    struct CeilLog2<N> : std::integral_constant<size_t, N>{};

    template <typename T>
    struct FixedChunkSize : public CeilLog2<sizeof(T)>{};
  }



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
        return reinterpret_cast<const Node*>((reinterpret_cast<const std::byte*>(this)) + leftOffset);
      }
      inline const Node* doGetRightChild() const noexcept {
        return reinterpret_cast<const Node*>((reinterpret_cast<const std::byte*>(this)) + rightOffset);
      }
      inline const Node* doGetParent() const noexcept {
        return reinterpret_cast<const Node*>((reinterpret_cast<const std::byte*>(this)) + parentOffset);
      }

      inline int getOffsetFrom(const Node* other) const noexcept {
        const ptrdiff_t nodeOffset = reinterpret_cast<const std::byte*>(this) - reinterpret_cast<const std::byte*>(other);
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

      inline void makeRoot(RBTreeBase* pTree) noexcept {
        parentOffset = 0;
        position = Position::Root;
        pTree->pRoot = this;
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
        setRightChild(y->getLeftChildOrNull());
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
              assert(false && "Shouldn't be able to get here....");
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
        assert(position == Position::Root);
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
        assert(leftOffset != 0);
        const auto* result = doGetLeftChild();
        assert(result != this);
        assert(result->position == Position::Left);
        return const_cast<Node*>(result);
      }
      inline Node* getRightChild() noexcept {
        assert(rightOffset != 0);
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
      RBNode() : RBNodeBase(), value_(){}

      template <typename ...Args> requires(std::constructible_from<T, Args...>)
      RBNode(std::in_place_t, Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>) : RBNodeBase(), value_(std::forward<Args>(args)...){}

      RBNode(RBNode&& other) noexcept = default;
      RBNode& operator=(RBNode&& other) noexcept = default;


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

    FixedSizeAllocator<meta::FixedChunkSize<T>::value> nodeAllocator;


  public:
    RBTree() : RBTreeBase(){

    }

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
        const ptrdiff_t nodeOffset = reinterpret_cast<const std::byte*>(this) - reinterpret_cast<const std::byte*>(other);
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
        assert(isBranch());
        const auto* result = doGetLeftChild();
        assert(result != this);
        assert(result->position == Position::Left);
        return const_cast<Node*>(result);
      }
      inline Node* getRightChild() noexcept {
        assert(isBranch());
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
}


JSON_BEGIN_C_NAMESPACE









JSON_END_C_NAMESPACE


