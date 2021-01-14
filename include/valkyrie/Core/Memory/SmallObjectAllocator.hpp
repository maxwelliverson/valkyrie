//
// Created by Maxwell on 2020-12-19.
//

#ifndef VALKYRIE_SMALL_OBJECT_ALLOCATOR_HPP
#define VALKYRIE_SMALL_OBJECT_ALLOCATOR_HPP

#include <valkyrie/Core/Config.hpp>
#include <valkyrie/Core/Types.hpp>

#include <memory>
#include <memory_resource>
#include <optional>
#include <deque>
#include <bit>
#include <vector>
#include <ranges>
#include <algorithm>


namespace valkyrie::Core{

  inline constexpr static u32    InvalidChunk = static_cast<u32>(-1);


  template <size_t BlockSize, u8 BlocksPerChunk = u8(256)>
  class FixedSizeAllocator{

    static_assert(BlockSize && !(BlockSize & (BlockSize - 1)),
                  "FixedSizeAllocator<BlockSize, BlocksPerChunk> -> BlockSize must be a power of 2");


    inline constexpr static size_t ChunkMemorySize = BlockSize * BlocksPerChunk;
    inline constexpr static size_t ChunkMemoryAlignment = std::min(BlockSize, alignof(std::max_align_t));
    inline constexpr static size_t BlockSizeLog2 = std::countr_zero(BlockSize);

    struct Chunk{
      u8   availableBlocks;
      u8   nextFreeBlock;
      alignas(ChunkMemoryAlignment) byte memory[];
    };


    inline constexpr static size_t ChunkAllocationSize = offsetof(Chunk, memory) + ChunkMemorySize;


    struct ChunkHandle{
      std::pmr::polymorphic_allocator<byte> allocator;
      Chunk* pChunk;

      void init() noexcept {
        pChunk->availableBlocks = BlocksPerChunk;
        pChunk->nextFreeBlock = 0;
        u8 i{0x00};
        byte* p = pChunk->memory;
        for(; i < BlocksPerChunk; p += BlockSize)
          *p = byte(++i);
      }

      ChunkHandle()
          : allocator(),
            pChunk(new(allocator.allocate(ChunkAllocationSize)) Chunk){
        init();
      }
      ChunkHandle(std::nullptr_t) noexcept : allocator(), pChunk(nullptr){}
      ChunkHandle(std::pmr::memory_resource* pResource)
          : allocator(pResource),
            pChunk(new(allocator.allocate(ChunkAllocationSize)) Chunk) {
        init();
      }

      ChunkHandle(const ChunkHandle&) = delete;
      ChunkHandle(ChunkHandle&& other) noexcept : allocator(other.allocator.resource()), pChunk(other.pChunk){
        other.pChunk = nullptr;
      }

      ChunkHandle& operator=(const ChunkHandle&) = delete;
      ChunkHandle& operator=(ChunkHandle&& other) noexcept {
        this->~ChunkHandle();
        new(this) ChunkHandle(std::move(other));
        return *this;
      }

      ~ChunkHandle(){
        if (pChunk) {
          allocator.deallocate(pChunk, ChunkAllocationSize);
        }
      }

      void* allocate() const noexcept {
        if (full())
          return nullptr;
        auto* pResult = &pChunk->memory[pChunk->nextFreeBlock * BlockSize];
        pChunk->nextFreeBlock = *pResult;
        --pChunk->availableBlocks;
        return pResult;
      }
      void  deallocate(void* pMemory) const noexcept {
        VK_assert(contains(pMemory));
        VK_assert(!empty());
        auto* pRelease = static_cast<byte*>(pMemory);
        VK_assert((pRelease - &pChunk->memory[0]) % BlockSize == 0);
        *pRelease = pChunk->nextFreeBlock;
        pChunk->nextFreeBlock = static_cast<u8>((pRelease - &pChunk->memory[0]) >> BlockSizeLog2);
        VK_assert(pChunk->nextFreeBlock == (pRelease - &pChunk->memory[0]) >> BlockSizeLog2);
        ++pChunk->availableBlocks;
      }
      bool  contains(void* p) const noexcept {
        const byte* const pComp = static_cast<const byte*>(p);
        return &pChunk->memory[0] <= pComp && pComp < &pChunk->memory[ChunkMemorySize];
      }
      bool  full() const noexcept {
        return !pChunk->availableBlocks;
      }
      bool  empty() const noexcept {
        return pChunk->availableBlocks == BlocksPerChunk;
      }
    };

    using ChunkVector = std::pmr::vector<ChunkHandle>;

    ChunkVector chunks{};
    mutable u32 allocChunk   = InvalidChunk;
    mutable u32 deallocChunk = InvalidChunk;
    mutable u32 emptyChunk   = InvalidChunk;
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

    u32 findDeallocChunk(void* p) const noexcept {
      if ( chunks.empty() )
        return InvalidChunk;

      VK_assert(deallocChunk != InvalidChunk);

      u32 lo = deallocChunk;
      u32 hi = deallocChunk + 1;
      const u32 loBound = 0;
      const u32 hiBound = chunks.size();

      // Special case: deallocChunk_ is the last in the array
      if (hi == hiBound)
        hi = InvalidChunk;

      for (;;)
      {
        if (lo != InvalidChunk)
        {
          if ( chunks[lo]->contains( p ) )
            return lo;
          if ( lo == loBound ) {
            lo = InvalidChunk;
            if ( hi == InvalidChunk )
              break;
          }
          else
            --lo;
        }
        if (hi != InvalidChunk) {
          if ( chunks[hi]->contains( p ) )
            return hi;
          if ( ++hi == hiBound )
          {
            hi = InvalidChunk;
            if ( lo != InvalidChunk )
              break;
          }
        }
      }

      return InvalidChunk;
    }
    size_t countEmptyChunks() const noexcept {
#if defined(VK_debug_memory)
      return std::ranges::count(chunks, [](const ChunkHandle& chunk){ return chunk.empty(); });
#else
      return emptyChunk != InvalidChunk;
#endif
    }
    void doDeallocate(void* p) noexcept {
      // Show that deallocChunk_ really owns the block at address p.
      VK_assert( chunks[deallocChunk].contains( p ) );
      // Either of the next two assertions may fail if somebody tries to
      // delete the same block twice.
      VK_assert( emptyChunk != deallocChunk );
      VK_assert( !chunks[deallocChunk].empty() );
      // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
      VK_assert( ( InvalidChunk == emptyChunk ) || chunks[emptyChunk].empty() );

      // call into the chunk, will adjust the inner list but won't release memory
      chunks[deallocChunk].deallocate(p);

      if ( chunks[deallocChunk].empty() )
      {
        VK_assert( emptyChunk != deallocChunk );

        // deallocChunk_ is empty, but a Chunk is only released if there are 2
        // empty chunks.  Since emptyChunk_ may only point to a previously
        // cleared Chunk, if it points to something else besides deallocChunk_,
        // then FixedAllocator currently has 2 empty Chunks.
        if ( emptyChunk != InvalidChunk )
        {
          // If last Chunk is empty, just change what deallocChunk_
          // points to, and release the last.  Otherwise, swap an empty
          // Chunk with the last, and then release it.
          const u32 lastChunk = u32(chunks.size() - 1);
          if ( lastChunk == deallocChunk )
            deallocChunk = emptyChunk;
          else if ( lastChunk != emptyChunk )
            std::swap( chunks[emptyChunk], chunks[lastChunk] );
          VK_assert( chunks[lastChunk].empty() );
          chunks.pop_back();
          if ( ( allocChunk == lastChunk ) || chunks[emptyChunk].full() )
            allocChunk = deallocChunk;
        }
        emptyChunk = deallocChunk;
      }

      // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
      VK_assert( ( emptyChunk == InvalidChunk ) || chunks[emptyChunk].empty() );
    }

  public:
    FixedSizeAllocator() = default;
    FixedSizeAllocator(std::pmr::memory_resource* pChunkVectorResource) noexcept : chunks(0, pChunkVectorResource){}
    FixedSizeAllocator(std::pmr::memory_resource* pChunkVectorResource, std::pmr::memory_resource* pChunkResource) noexcept
        : chunks(0, pChunkVectorResource),
          pChunkResource(pChunkResource){}

    void                       setChunkResource(std::pmr::memory_resource* pResource) noexcept {
      pChunkResource = pResource;
    }
    std::pmr::memory_resource* getChunkResource() const noexcept {
      return pChunkResource;
    }

    u32 ownedChunkIndex(void* p) const noexcept {
      for (u32 i = 0; i < chunks.size(); ++i)
        if (chunks[i].contains(p))
          return i;
      return InvalidChunk;
    }

    bool tryDeallocate(void* p) noexcept {
      auto foundChunk = findDeallocChunk(p);
      if (foundChunk == InvalidChunk)
        return false;
      deallocChunk = foundChunk;
      doDeallocate(p);
      VK_assert(countEmptyChunks() < 2);
      return true;
    }

    bool trimEmptyChunk() {
      // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
      VK_assert( ( emptyChunk == InvalidChunk ) || chunks[emptyChunk]->empty() );
      if ( emptyChunk == InvalidChunk )
        return false;

      // If emptyChunk_ points to valid Chunk, then chunk list is not empty.
      VK_assert( !chunks.empty() );
      // And there should be exactly 1 empty Chunk.
      VK_assert( 1 == countEmptyChunks() );

      const u32 lastChunk = u32(chunks.size() - 1);
      if ( lastChunk == deallocChunk )
        deallocChunk = emptyChunk;
      else if ( lastChunk != emptyChunk )
        std::swap( chunks[emptyChunk], chunks[lastChunk] );
      VK_assert( chunks[lastChunk].empty() );
      chunks.pop_back();

      if ( chunks.empty() ) {
        allocChunk = InvalidChunk;
        deallocChunk = InvalidChunk;
      }
      else {
        if ( deallocChunk == emptyChunk ) {
          deallocChunk = 0;
          VK_assert( !chunks[deallocChunk]->empty() );
        }
        if ( allocChunk == emptyChunk ) {
          allocChunk = lastChunk;
          VK_assert( !chunks[allocChunk]->empty() );
        }
      }

      emptyChunk = InvalidChunk;
      VK_assert( 0 == countEmptyChunks() );

      return true;
    }
    bool trimChunkList() {
      if ( chunks.empty() ) {
        VK_assert( allocChunk == InvalidChunk );
        VK_assert( deallocChunk == InvalidChunk );
      }

      if ( chunks.size() == chunks.capacity() )
        return false;

      chunks.shrink_to_fit();

      return true;
    }


    void* allocate() noexcept {

      VK_assert( !chunks.empty() );
      VK_assert(deallocChunk < chunks.size());
      VK_assert(allocChunk < chunks.size());
      VK_assert( countEmptyChunks() < 2 );

      if (allocChunk == InvalidChunk || chunks[allocChunk].full()){
        auto i = chunks.begin();
        const auto end = chunks.end();
        for (;; ++i){
          if (i == end) {
            chunks.emplace_back();
            allocChunk = u32(chunks.size());
            deallocChunk = 0;
            break;
          }
          if (!i->full()) {
            allocChunk = u32(i - chunks.begin());
            break;
          }
        }
      }
      VK_assert(allocChunk != InvalidChunk);
      VK_assert(!chunks[allocChunk].full());
      return chunks[allocChunk].allocate();
    }
    void* allocate(u32 chunkHint) noexcept {
      VK_assert(chunkHint < chunks.size());
      allocChunk = chunkHint;
      return allocate();
    }
    void  deallocate(void* p, u32 chunk) noexcept {

      VK_assert( countEmptyChunks() < 2 );

      auto foundChunk = chunk == InvalidChunk ? findDeallocChunk( p ) : chunk;
      VK_assert(chunks[foundChunk]->contains(p));

      deallocChunk = foundChunk;
      doDeallocate(p);
      VK_assert( countEmptyChunks() < 2 );
    }
  };


  inline static constexpr u64 alignTo(u64 value, u64 alignment) noexcept {
    const u64 alignMinusOne = alignment - 1;
    return (value + alignMinusOne) & ~alignMinusOne;
  }



  template <u8 BlocksPerChunk = u8(256)>
  class SmallObjectAllocator{
    FixedSizeAllocator<1,  BlocksPerChunk>  alloc1; //   8 bit
    FixedSizeAllocator<2,  BlocksPerChunk>  alloc2; //  16 bit
    FixedSizeAllocator<4,  BlocksPerChunk>  alloc4; //  32 bit
    FixedSizeAllocator<8,  BlocksPerChunk>  alloc8; //  64 bit
    FixedSizeAllocator<16, BlocksPerChunk> alloc16; // 128 bit
    FixedSizeAllocator<32, BlocksPerChunk> alloc32; // 256 bit
    FixedSizeAllocator<64, BlocksPerChunk> alloc64; // 512 bit

    inline static u32 getBlockSize(u64 size) noexcept {
      return std::bit_ceil(u32(size));
    }
    inline static u32 getBlockSize(u64 size, u64 alignment) noexcept {
      return getBlockSize(std::max(size, alignment));
    }

    struct ChunkHintImpl{
      u32 blockSize : 8;
      u32 chunkIndex : 24;
    };

    void* doAllocate(u32 blockSize) noexcept {
      switch (blockSize) {
        case 1:  return alloc1.allocate();
        case 2:  return alloc2.allocate();
        case 4:  return alloc4.allocate();
        case 8:  return alloc8.allocate();
        case 16: return alloc16.allocate();
        case 32: return alloc32.allocate();
        case 64: return alloc64.allocate();
        default: return nullptr;
      }
    }
    void* doAllocate(u32 blockSize, u32 chunkHint) noexcept {
      switch (blockSize) {
        case 1:  return alloc1.allocate(chunkHint);
        case 2:  return alloc2.allocate(chunkHint);
        case 4:  return alloc4.allocate(chunkHint);
        case 8:  return alloc8.allocate(chunkHint);
        case 16: return alloc16.allocate(chunkHint);
        case 32: return alloc32.allocate(chunkHint);
        case 64: return alloc64.allocate(chunkHint);
        default: return nullptr;
      }
    }
    void  doDeallocate(void* p, u32 blockSize, u32 chunk) noexcept {
      switch (blockSize) {
        case 1:  return alloc1.deallocate(p, chunk);
        case 2:  return alloc2.deallocate(p, chunk);
        case 4:  return alloc4.deallocate(p, chunk);
        case 8:  return alloc8.deallocate(p, chunk);
        case 16: return alloc16.deallocate(p, chunk);
        case 32: return alloc32.deallocate(p, chunk);
        case 64: return alloc64.deallocate(p, chunk);
        VK_no_default;
      }
    }

  public:

    enum class ChunkHint : u32;


    void* allocate(u64 size) noexcept {
      auto blockSize = getBlockSize(size);
      VK_assert(std::popcount(blockSize) == 1);
      return doAllocate(blockSize);
    }
    //void* allocate(u64 size, u64 alignment) noexcept {}
    void* allocate(u64 size, ChunkHint hint) noexcept {
      auto chunkHint = *reinterpret_cast<ChunkHintImpl*>(&hint);
      VK_assert(std::popcount(chunkHint.blockSize) == 1);
      if (auto blockSize = getBlockSize(size); blockSize == chunkHint.blockSize)
        return doAllocate(chunkHint.blockSize, chunkHint.chunkIndex);
      else
        return doAllocate(blockSize);
    }

    void  deallocate(void* p) noexcept {
      if (alloc1.tryDeallocate(p))
        return;
      if (alloc2.tryDeallocate(p))
        return;
      if (alloc4.tryDeallocate(p))
        return;
      if (alloc8.tryDeallocate(p))
        return;
      if (alloc16.tryDeallocate(p))
        return;
      if (alloc32.tryDeallocate(p))
        return;
      if (!alloc64.tryDeallocate(p))
        VK_unreachable;
    }
    void  deallocate(void* p, ChunkHint hint) noexcept {
      auto chunkHint = *reinterpret_cast<ChunkHintImpl*>(&hint);
      VK_assert(std::popcount(chunkHint.blockSize) == 1);
      doDeallocate(p, chunkHint.blockSize, chunkHint.chunkIndex);
    }
    void  deallocate(void* p, u64 size) noexcept {
      auto blockSize = getBlockSize(size);
      VK_assert(std::popcount(blockSize) == 1);
      doDeallocate(p, blockSize, InvalidChunk);
    }
    //void  deallocate(void* p, u64 size, u64 alignment) noexcept {}

    bool defragment() noexcept {
      bool result = false;
      result |= alloc1.trimEmptyChunk();
      result |= alloc2.trimEmptyChunk();
      result |= alloc4.trimEmptyChunk();
      result |= alloc8.trimEmptyChunk();
      result |= alloc16.trimEmptyChunk();
      result |= alloc32.trimEmptyChunk();
      result |= alloc64.trimEmptyChunk();
      result |= alloc1.trimChunkList();
      result |= alloc2.trimChunkList();
      result |= alloc4.trimChunkList();
      result |= alloc8.trimChunkList();
      result |= alloc16.trimChunkList();
      result |= alloc32.trimChunkList();
      result |= alloc64.trimChunkList();
      return result;
    }
  };


  template <typename T>
  class FixedAllocator{

  };




}



#endif//VALKYRIE_SMALL_OBJECT_ALLOCATOR_HPP
