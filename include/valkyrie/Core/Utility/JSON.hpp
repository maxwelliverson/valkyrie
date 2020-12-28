//
// Created by Maxwell on 2020-12-25.
//

#ifndef VALKYRIE_JSON_HPP
#define VALKYRIE_JSON_HPP

#include <valkyrie/Core/Utility/String.hpp>
#include <valkyrie/Core/Async/Atomic.hpp>

#include <memory>
#include <memory_resource>
#include <optional>
#include <deque>
#include <bit>
#include <vector>

namespace valkyrie::Core{



  enum class JsonType{
    Null,
    String,
    Boolean,
    Number,
    Object,
    Array
  };



  template <size_t BlockSize, u8 BlocksPerChunk = u8(256)>
  class FixedSizeAllocator{

    static_assert(BlockSize && !(BlockSize & (BlockSize - 1)),
                  "FixedSizeAllocator<BlockSize, BlocksPerChunk> -> BlockSize must be a power of 2");

    inline constexpr static u32    InvalidChunk = static_cast<u32>(-1);
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
    u32 allocChunk   = InvalidChunk;
    u32 deallocChunk = InvalidChunk;
    u32 emptyChunk   = InvalidChunk;
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

  public:
    FixedSizeAllocator() = default;
    FixedSizeAllocator(std::pmr::memory_resource* pChunkVectorResource) noexcept : chunks(0, pChunkVectorResource){}
    FixedSizeAllocator(std::pmr::memory_resource* pChunkVectorResource, std::pmr::memory_resource* pChunkResource) noexcept
        : chunks(0, pChunkVectorResource),
          pChunkResource(pChunkResource){}

    void setChunkResource(std::pmr::memory_resource* pResource) noexcept {
      pChunkResource = pResource;
    }
    std::pmr::memory_resource* getChunkResource() const noexcept {
      return pChunkResource;
    }



    void* allocate() noexcept {
      if (allocChunk == InvalidChunk || chunks[allocChunk].empty()){
        auto i = chunks.begin();
        const auto end = chunks.end();
        for (;; ++i){
          if (i == end) {
            chunks.emplace_back();
            allocChunk = u32(chunks.size());
            deallocChunk = 0;
            break;
          }
          if (!i->empty()) {
            allocChunk = u32(i - chunks.begin());
            break;
          }
        }
      }
      VK_assert(allocChunk != InvalidChunk);
      VK_assert(!chunks[allocChunk].empty());
      return chunks[allocChunk].allocate();
    }
    void  deallocate(void* p) noexcept {











      // Show that deallocChunk_ really owns the block at address p.
      assert( chunks[deallocChunk].contains( p ) );
      // Either of the next two assertions may fail if somebody tries to
      // delete the same block twice.
      assert( emptyChunk != deallocChunk );
      assert( !chunks[deallocChunk].empty() );
      // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
      assert( ( InvalidChunk == emptyChunk ) || chunks[emptyChunk].empty() );

      // call into the chunk, will adjust the inner list but won't release memory
      chunks[deallocChunk].deallocate(p);

      if ( chunks[deallocChunk].empty() )
      {
        assert( emptyChunk != deallocChunk );

        // deallocChunk_ is empty, but a Chunk is only released if there are 2
        // empty chunks.  Since emptyChunk_ may only point to a previously
        // cleared Chunk, if it points to something else besides deallocChunk_,
        // then FixedAllocator currently has 2 empty Chunks.
        if ( emptyChunk != InvalidChunk )
        {
          // If last Chunk is empty, just change what deallocChunk_
          // points to, and release the last.  Otherwise, swap an empty
          // Chunk with the last, and then release it.
          u32 lastChunk = u32(chunks.size() - 1);
          if ( lastChunk == deallocChunk )
            deallocChunk = emptyChunk;
          else if ( lastChunk != emptyChunk )
            std::swap( chunks[emptyChunk], chunks[lastChunk] );
          assert( chunks[lastChunk].empty() );
          chunks.pop_back();
          if ( ( allocChunk == lastChunk ) || chunks[emptyChunk].full() )
            allocChunk = deallocChunk;
        }
        emptyChunk = deallocChunk;
      }

      // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
      assert( ( emptyChunk == InvalidChunk ) || chunks[emptyChunk].empty() );
    }
  };

  class StringPool{
    struct String{
      Atomic<u32> refCount;
      u32         length;
      utf8        buffer[];
    };
    std::pmr::unsynchronized_pool_resource memoryResource;
    std::pmr::deque<>
  };

  class JsonFile;
  class JsonObject;


  class JsonKey{
    String identifier_;
  };
  class JsonValue{
    JsonType type_;
  public:
  };


  class JsonObjectDescriptor{

  };

  class JsonIterator{
    JsonObject* pObject;
  };


  class JsonNull    : public JsonValue{};
  class JsonString  : public JsonValue{
    std::pmr::u8string value_;
  };
  class JsonBoolean : public JsonValue{
    bool value_;
  };
  class JsonNumber  : public JsonValue{
    enum { INTEGER, FLOAT } type;
    union{
      int i_value_;
      float f_value_;
    };
  };
  class JsonArray   : public JsonValue{

  };
  class JsonObject  : public JsonValue{
    JsonObjectDescriptor* pDescriptor;
    class Entry{
    public:
      MutableStringView key()       noexcept { }
      StringView        key() const noexcept { }
      JsonValue& value()       noexcept { }
      const JsonValue& value() const noexcept{
        return const_cast<const JsonValue&>(const_cast<Entry*>(this)->value());
      }
    };
  };
}

#endif//VALKYRIE_JSON_HPP
