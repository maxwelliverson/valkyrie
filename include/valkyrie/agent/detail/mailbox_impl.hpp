//
// Created by maxwe on 2021-04-25.
//

#ifndef VALKYRIE_AGENT_DETAIL_MAILBOX_IMPL_HPP
#define VALKYRIE_AGENT_DETAIL_MAILBOX_IMPL_HPP

#include <valkyrie/adt/tags.hpp>
#include <valkyrie/async/atomic.hpp>
#include <valkyrie/async/semaphore.hpp>
#include <valkyrie/status/result.hpp>


#include <valkyrie/memory/default_allocator.hpp>
#include <valkyrie/memory/allocator_storage.hpp>

#include "../message.hpp"

namespace valkyrie::impl{
  template <typename T>
  struct mailbox_traits;
  
  namespace mailbox{


    /// Forward Declarations

    class dynamic_base;
    template <u64 MsgLength>
    class static_base;

    class default_storage;
    class private_dynamic_storage;
    class private_static_storage;
    template <bool WriteCoherent>
    class default_dynamic_storage;
    template <bool WriteCoherent>
    class default_static_storage;

    template <typename Base, typename Storage>
    class private_impl;

    template <typename Base, typename Storage>
    class default_impl;




    /// Dispatch base

    template <typename Traits>
    struct get_base{
      using type = static_base<Traits::message_size>;
    };
    template <typename Traits> requires(Traits::message_size_is_dynamic)
    struct get_base<Traits>{
      using type = dynamic_base;
    };



    /// Dispatch storage

    template <typename Traits>
    struct get_storage{
      using type = default_storage;
    };
    template <typename Traits>
    requires(Traits::max_readers == 1 && Traits::max_writers == 1 && !Traits::message_size_is_dynamic)
    struct get_storage<Traits>{
      using type = private_static_storage;
    };
    template <typename Traits>
    requires(Traits::max_readers == 1 && Traits::max_writers == 1 && Traits::message_size_is_dynamic)
    struct get_storage<Traits>{
      using type = private_dynamic_storage;
    };
    template <typename Traits>
    requires(Traits::max_readers == 1 && Traits::max_writers != 1 && !Traits::message_size_is_dynamic)
    struct get_storage<Traits>{
      using type = default_static_storage<Traits::is_write_coherent>;
    };
    template <typename Traits>
    requires(Traits::max_readers == 1 && Traits::max_writers != 1 && Traits::message_size_is_dynamic)
    struct get_storage<Traits>{
      using type = default_dynamic_storage<Traits::is_write_coherent>;
    };



    /// Dispatch implementation

    template <typename Traits>
    struct get_impl;
    template <typename Traits> requires(Traits::max_readers == 1 && Traits::max_writers == 1)
    struct get_impl<Traits>{
      template <typename Base, typename Storage>
      using type = private_impl<Base, Storage>;
    };
    template <typename Traits> requires(Traits::max_readers == 1 && Traits::max_writers != 1)
    struct get_impl<Traits>{
      template <typename Base, typename Storage>
      using type = default_impl<Base, Storage>;
    };



    /// Complete type

    template <typename WriterSema, typename ReaderSema, typename Base>
    class complete : public Base{
    public:

      template <typename ...Args> requires std::constructible_from<Base, Args...>
      complete(u64 maxWriters, u64 maxReaders, Args&& ...args) noexcept
          : Base(std::forward<Args>(args)...),
            writerSemaphore(maxWriters),
            readerSemaphore(maxReaders){}



      inline bool acquire_write_perms() noexcept {
        return writerSemaphore.try_acquire();
      }
      inline void release_write_perms() noexcept {
        writerSemaphore.release();
      }

      inline bool acquire_read_perms() noexcept {
        return readerSemaphore.try_acquire();
      }
      inline void release_read_perms() noexcept {
        readerSemaphore.release();
      }


    private:
      [[no_unique_address]] WriterSema writerSemaphore;
      [[no_unique_address]] ReaderSema readerSemaphore;
    };
    
    
    
    

    class dynamic_base{


    protected:

      dynamic_base() noexcept : msgQueue(nullptr), queueLength(0){}
      dynamic_base(u64 length, system_status& status) noexcept;
      dynamic_base(const dynamic_base&) = delete;
      dynamic_base(dynamic_base&& ) noexcept;


      ~dynamic_base();


      VK_nodiscard inline void*    to_address(const u64 offset) const noexcept {
        return msgQueue + offset;
      }
      VK_nodiscard inline message* to_message(const u64 offset) const noexcept {
        return static_cast<message*>(to_address(offset));
      }
      VK_nodiscard inline u64      to_offset(const void* addr)  const noexcept {
        return static_cast<const byte*>(addr) - msgQueue;
      }

      VK_nodiscard inline bool     invalid_size(const u64 offset) const noexcept {
        return queueLength <= offset;
      }

      inline void normalize_offset(u64& offset) const noexcept {
        if ( offset >= queueLength ) VK_unlikely
          offset -= queueLength;
      }


      byte* msgQueue;
      u64   queueLength;

    public:

      static void get_alloc_minimums(u64& size, u64& align) noexcept;
    };

    template <u64 MsgLength>
    class static_base{

      struct alignas(message) msg_proxy{
        byte padding[MsgLength];
      };

      VK_nodiscard static msg_proxy* as_proxy(void* address) noexcept {
        return static_cast<msg_proxy*>(address);
      }
      VK_nodiscard static const msg_proxy* as_proxy(const void* address) noexcept {
        return static_cast<const msg_proxy*>(address);
      }
    protected:

      VK_constant u64 default_capacity = 32;
      VK_constant u64 message_size     = MsgLength;


      static_base() noexcept : static_base(default_allocator{}, default_capacity){}

      template <raw_allocator Alloc>
      explicit static_base(Alloc&& allocator) noexcept
          : static_base(std::forward<Alloc>(allocator), default_capacity){}

      explicit static_base(u64 capacity) noexcept
          : static_base(default_allocator{}, capacity){}

      template <raw_allocator Alloc>
      static_base(Alloc&& allocator, u64 capacity) noexcept
          : queueAllocator(make_any_allocator_reference(std::forward<Alloc>(allocator))),
            queueCapacity(capacity),
            msgQueue(static_cast<byte*>(
                queueAllocator.allocate_array(queueCapacity, message_size, alignof(message)))){ }

      template <composable_raw_allocator Alloc>
      static_base(Alloc&& allocator, u64 capacity) noexcept
          : queueAllocator(make_any_allocator_reference(std::forward<Alloc>(allocator))),
            queueCapacity(capacity),
            msgQueue(static_cast<byte*>(
                queueAllocator.try_allocate_array(queueCapacity,message_size, alignof(message)))){ }

      static_base(const static_base&) = delete;

      static_base(static_base&& other) noexcept
          : queueAllocator(std::move(other.queueAllocator)),
            queueCapacity(other.queueCapacity),
            msgQueue(other.msgQueue){
        other.queueCapacity = 0;
        other.msgQueue      = nullptr;
      }

      ~static_base() {
        if ( msgQueue ) {
          queueAllocator.deallocate_array(msgQueue, queueCapacity, message_size, alignof(message));
        }
      }

      inline void normalize_offset(u64& offset) const noexcept {
        if ( offset == ( queueCapacity * message_size )) VK_unlikely {
          offset = 0;
        }
      }

      VK_nodiscard inline bool     invalid_size(const u64 size) const noexcept {
        return size != message_size;
      }
      VK_nodiscard inline void*    to_address(const u64 offset) const noexcept {
        return msgQueue + offset;
        //return as_proxy(msgQueue) + offset;
      }
      VK_nodiscard inline message* to_message(const u64 offset) const noexcept {
        return static_cast<message*>(to_address(offset));
      }
      VK_nodiscard inline u64      to_offset(const void* address) const noexcept {
        return static_cast<const byte*>(address) - msgQueue;
      }



      any_allocator_reference queueAllocator;
      u64                     queueCapacity;
      byte*                   msgQueue;
    public:

    };


    


    class default_storage{
    protected:
      u64 readNext;
      u64 readEnd;
      u64 writeNext;
      u64 writeEnd;

      u64 readyMessageCount;
    };

    class private_dynamic_storage{
    protected:
      atomic<u64> readNext;
      atomic<u64> writeNext;
    };
    class private_static_storage{
    protected:
      u64 readNext;
      u64 writeNext;

      atomic<u64> msgCount;
    };

    template <bool WriteCoherent>
    class default_dynamic_storage{
    protected:
      VK_constant bool is_write_coherent = WriteCoherent;

      atomic<u64> readNext  = 0;
      atomic<u64> readEnd   = 0;
      atomic<u64> writeNext = 0;
    };
    template <bool WriteCoherent>
    class default_static_storage{
    protected:
      VK_constant bool is_write_coherent = WriteCoherent;

      u64         readNext = 0;
      atomic<u64> writeNext = 0;

      atomic<u64> slotsAvailable; //TODO: Figure out how to initialize this to the number of slots...
      atomic<u64> msgCount;
    };
    template <>
    class default_static_storage<true>{
    protected:
      VK_constant bool is_write_coherent = true;

      u64         readNext  = 0;
      atomic<u64> writeNext = 0;

      atomic<u64> msgCount = 0;
      binary_semaphore isWriting{1};
    };



    /*

     INTERFACE:

      VK_constant bool dynamic_message_sizes = exact_same_as<Base, dynamic_base>;
    public:
      using Base::Base;

      inline void*    do_begin_write(u64 size,     u64& nextOffset) noexcept;
      inline void*    do_try_begin_write(u64 size, u64& nextOffset) noexcept;
      inline void     do_end_write(message* msg,   u64  nextOffset) noexcept;

      inline message* do_begin_read() noexcept;
      inline message* do_try_begin_read() noexcept;
      inline void     do_end_read(message* msg) noexcept;
      */
    

#define VK_detect_invalid_size(size) VK_if(VK_debug_build(VK_assert( !this->invalid_size(size) ))VK_else(if (this->invalid_size(size)) VK_unlikely return nullptr))

    template <typename Base, typename Storage>
    class private_impl : public Base, public Storage{
      VK_constant bool dynamic_message_sizes = exact_same_as<Base, dynamic_base>;
    public:
      using Base::Base;

      inline void*    do_begin_write(u64 size,     u64& nextOffset) noexcept {

        VK_detect_invalid_size(size);

        if constexpr (dynamic_message_sizes) {
          const u64 writeOffset = this->writeNext.load(std::memory_order_acquire);
          u64 nextWrite   = writeOffset + size;

          do {
            // Get readOffset; if nextWrite is equal to or greater than readOffset,
            // wait for a message to be read, and then try again.
            const u64 readOffset = this->readNext.load(std::memory_order_acquire);
            if (nextWrite >= (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_unlikely {
              this->readNext.wait(readOffset, std::memory_order_acquire);
              continue;
            }
            break;
          } while(true);

          // nextOffset %= queueLength;
          this->normalize_offset(nextWrite);

          // Update nextOffset
          nextOffset = nextWrite;
          // Return the address to which the message can be written
          return this->to_address(writeOffset);
        }
        else {
          const u64 writeIndex = this->writeNext;
          this->msgCount.wait(this->queueCapacity, std::memory_order_acquire);
          ++this->writeNext;
          this->normalize_offset(this->writeNext);
          nextOffset = this->writeNext;
          return this->to_address(writeIndex);
        }
      }
      inline void*    do_try_begin_write(u64 size, u64& nextOffset) noexcept {

        VK_detect_invalid_size(size);

        if constexpr (dynamic_message_sizes) {

          const u64 writeOffset = this->writeNext.load(std::memory_order_acquire);
          u64 nextWrite   = writeOffset + size;

          const u64 readOffset = this->readNext.load(std::memory_order_acquire);
          if (nextWrite >= (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_unlikely
            return nullptr;

          // nextOffset %= queueLength;
          this->normalize_offset(nextWrite);

          // Update nextOffset
          nextOffset = nextWrite;
          // Return the address to which the message can be written
          return this->to_address(writeOffset);
        }
        else {
          const u64 writeIndex = this->writeNext;
          if ( this->msgCount == this->queueCapacity ) VK_unlikely
            return nullptr;
          this->writeNext += this->message_size;
          this->normalize_offset(this->writeNext);
          nextOffset = this->writeNext;
          return this->to_address(writeIndex);
        }
      }
      inline void     do_end_write(message* msg,   u64  nextOffset) noexcept {
        VK_assert( msg != nullptr );

        if constexpr (dynamic_message_sizes) {
          msg->info.nextOffset = narrow_cast<u32>(nextOffset);
          this->writeNext.store(nextOffset, std::memory_order_release);
          this->writeNext.notify_one();
        }
        else {
          ++this->msgCount;
          this->msgCount.notify_one();
        }
      }

      inline message* do_begin_read() noexcept {
        if constexpr (dynamic_message_sizes) {
          const u64 readOffset = this->readNext.load(std::memory_order_acquire);
          this->writeNext.wait(readOffset, std::memory_order_acquire);
          return this->to_message(readOffset);
        }
        else {
          this->msgCount.wait(0, std::memory_order_acquire);
          return this->to_message(this->readNext);
        }
      }
      inline message* do_try_begin_read() noexcept {

        if constexpr (dynamic_message_sizes) {
          const u64 readOffset = this->readNext.load(std::memory_order_acquire);
          if ( readOffset == this->writeNext ) VK_unlikely
            return nullptr;
          return this->to_message(readOffset);
        }
        else {
          if ( this->msgCount == 0 )
            return nullptr;
          return this->to_message(this->readNext);
        }
      }
      inline void     do_end_read(message* msg) noexcept {

        if constexpr (dynamic_message_sizes) {
          this->readNext.store(msg->info.nextOffset, std::memory_order_release);
          this->readNext.notify_one();
        }
        else {
          this->readNext = msg->info.nextOffset;
          --this->msgCount;
          this->msgCount.notify_one();
        }
      }
    };

    template <typename Base, typename Storage>
    class default_impl : public Base, public Storage{
      VK_constant bool dynamic_message_sizes = exact_same_as<Base, dynamic_base>;
    public:
      using Base::Base;

      //TODO: Implement all of these lol

      inline void*    do_begin_write(u64 size,     u64& nextOffset) noexcept {

        VK_detect_invalid_size(size);

        if constexpr ( Storage::is_write_coherent ) {
          u64 writeOffset;
          u64 nextWrite;
          u64 readOffset;

          do {
            if constexpr ( dynamic_message_sizes ) {
              // load readEnd and treat as though it is equal to writeNext
              writeOffset = this->readEnd.load(std::memory_order_acquire);

              // calculate the next message offset based on that
              nextWrite     = writeOffset + size;
              this->normalize_offset(nextWrite);

              // try to update writeNext, this will fail if the mailbox is already being written to.
              if ( this->writeNext.compare_exchange_weak(writeOffset, nextWrite, std::memory_order_acq_rel) ) {

                // load readOffset and make sure the mailbox has enough space available to write to
                readOffset = this->readNext.load(std::memory_order_acquire);
                if (nextWrite < (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_likely {
                  // if so, the operation is a success and we leave the loop
                  break;
                }

                // in the (ideally) rare case that the mailbox has insufficient space
                // reset writeNext so that other threads blocked threads can advance
                this->writeNext.store(writeOffset, std::memory_order_release);
                this->writeNext.notify_one();

                // wait for a message to be read before trying again
                this->readNext.wait(readOffset, std::memory_order_acquire);
                continue;
              }

              // wait for other writes to be finished before trying again
              this->readEnd.wait(writeOffset, std::memory_order_acquire);
            }
            else {
              this->isWriting.acquire();

              if (this->msgCount.load(std::memory_order_acquire) != this->queueCapacity ) VK_likely {
                writeOffset = this->writeNext.load(std::memory_order_acquire);

                nextWrite  = writeOffset + this->message_size;
                this->normalize_offset(nextWrite);
                break;
              }
              // release instead of keeping it locked while waiting for a message to be read
              // this is to prevent possible deadlock in weird edge cases
              this->isWriting.release();
              this->msgCount.wait(this->queueCapacity, std::memory_order_acquire);
            }
          } while(true);

          nextOffset = nextWrite;
          return this->to_address(writeOffset);
        }
        else {
          if constexpr ( dynamic_message_sizes ) {
            u64 writeOffset = this->writeNext.load(std::memory_order_acquire);
            u64 nextWrite;
            do {
              const u64 readOffset = this->readNext.load(std::memory_order_acquire);
              nextWrite = writeOffset + size;
              if (nextWrite >= (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_unlikely {
                this->readNext.wait(readOffset, std::memory_order_acq_rel);
                continue;
              }
              this->normalize_offset(nextWrite);
            } while(!this->writeNext.compare_exchange_strong(writeOffset, nextWrite, std::memory_order_acq_rel));
            nextOffset = nextWrite;
            return this->to_address(writeOffset);
          }
          else {
            u64 writeOffset = this->writeNext.load(std::memory_order_acquire);
            u64 nextWrite;

            do {
              this->msgCount.wait(this->queueCapacity, std::memory_order_acquire);
              nextWrite = writeOffset + this->message_size;
              this->normalize_offset(nextWrite);
            } while ( !this->writeNext.compare_exchange_strong(writeOffset, nextWrite, std::memory_order_acquire) );

            nextOffset = nextWrite;
            return this->to_address(writeOffset);
          }
        }
      }
      inline void*    do_try_begin_write(u64 size, u64& nextOffset) noexcept {
        VK_detect_invalid_size(size);

        if constexpr ( Storage::is_write_coherent ) {
          u64 writeOffset;
          u64 nextWrite;
          u64 readOffset;

          if constexpr ( dynamic_message_sizes ) {
            // load readEnd and treat as though it is equal to writeNext
            writeOffset = this->readEnd.load(std::memory_order_acquire);

            // calculate the next message offset based on that
            nextWrite     = writeOffset + size;
            this->normalize_offset(nextWrite);

            // try to update writeNext, this will fail if the mailbox is already being written to.
            if ( !this->writeNext.compare_exchange_strong(writeOffset, nextWrite, std::memory_order_acq_rel) ) {
              return nullptr;
            }

            // load readOffset and make sure the mailbox has enough space available to write to
            readOffset = this->readNext.load(std::memory_order_acquire);
            if (nextWrite >= (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_unlikely {
              // in the (ideally) rare case that the mailbox has insufficient space
              // reset writeNext so that other threads blocked threads can advance
              this->writeNext.store(writeOffset, std::memory_order_release);
              this->writeNext.notify_one();
              return nullptr;
            }

          }
          else {
            if (!this->isWriting.try_acquire()) return nullptr;
            if ( this->msgCount.load(std::memory_order_acquire) == this->queueCapacity ) {
              this->isWriting.release();
              return nullptr;
            }

            writeOffset = this->writeNext.load(std::memory_order_acquire);

            nextWrite  = writeOffset + this->message_size;
            this->normalize_offset(nextWrite);
          }

          nextOffset = nextWrite;
          return this->to_address(writeOffset);
        }
        else {
          if constexpr ( dynamic_message_sizes ) {
            u64 writeOffset = this->writeNext.load(std::memory_order_acquire);
            u64 nextWrite;
            u64 readOffset;
            do {
              readOffset = this->readNext.load(std::memory_order_acquire);
              nextWrite = writeOffset + size;

              if (nextWrite >= (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_unlikely {
                return nullptr;
              }

              this->normalize_offset(nextWrite);
            } while(!this->writeNext.compare_exchange_strong(writeOffset, nextWrite, std::memory_order_acq_rel));
            nextOffset = nextWrite;
            return this->to_address(writeOffset);
          }
          else {
            u64 writeOffset = this->writeNext.load(std::memory_order_acquire);
            u64 nextWrite;

            do {
              if ( this->msgCount.load(std::memory_order_acquire) == this->queueCapacity ) VK_unlikely {
                return nullptr;
              }
              nextWrite = writeOffset + this->message_size;
              this->normalize_offset(nextWrite);
            } while ( !this->writeNext.compare_exchange_strong(writeOffset, nextWrite, std::memory_order_acquire) );

            nextOffset = nextWrite;
            return this->to_address(writeOffset);
          }
        }
      }
      inline void     do_end_write(message* msg,   u64  nextOffset) noexcept {

        VK_assert( msg );

        if constexpr ( Storage::is_write_coherent ) {
          if constexpr ( dynamic_message_sizes ) {
            msg->info.nextOffset = narrow_cast<u32>(nextOffset);
            this->readEnd.store(nextOffset, std::memory_order_release);
            this->readEnd.notify_all();
          }
          else {
            this->msgCount.fetch_add(1, std::memory_order_acq_rel);
            this->msgCount.notify_one();
            this->isWriting.release();
          }
        }
        else {
          atomic<impl::message_info&> info{msg->info};

          // TODO: Implement
          if constexpr ( dynamic_message_sizes ) {

            if ( this-> )


          }
          else {

          }
        }


      }

      inline message* do_begin_read() noexcept;
      inline message* do_try_begin_read() noexcept;
      inline void     do_end_read(message* msg) noexcept;
    };




    /*template <bool SingleWriter,
        bool SingleReader>
    struct mailbox_storage{
      atomic<u64> nextReadOffset = 0;
      atomic<u64> lastReadOffset = 0;
      atomic<u64> nextWriteOffset = 0;
      atomic<u64> lastWriteOffset = 0;
    };
    template <>
    struct mailbox_storage<true, true>{
      atomic<u64> nextReadOffset = 0;
      atomic<u64> nextWriteOffset = 0;
    };



    template <bool SingleWriter,
        bool SingleReader,
        bool IsCoherent>
    struct mailbox_ops;


    template <>
    struct mailbox_ops<true, true, false> : mailbox_base, mailbox_storage<true, true>{

      using mailbox_base::mailbox_base;


      inline void*    do_begin_write(u64 size,     u64& nextOffset) noexcept {
        u64 writeOffset = this->nextWriteOffset.load(std::memory_order_acquire);
        void* address   = to_address(writeOffset);
        nextOffset      = writeOffset + size;

        do {
          const u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);
          if (nextOffset >= (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_unlikely {
            this->nextReadOffset.wait(readOffset, std::memory_order_acq_rel);
            continue;
          }
          break;
        } while(true);


        if (this->queueLength <= nextOffset) VK_unlikely nextOffset -= this->queueLength;

        nextOffset -= this->queueLength <= nextOffset ? this->queueLength : 0;

        return address;
      }
      inline void*    do_try_begin_write(u64 size, u64& nextOffset) noexcept {
        u64 writeOffset = this->nextWriteOffset.load(std::memory_order_acquire);
        nextOffset      = writeOffset + size;

        const u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);
        if (nextOffset >= (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_unlikely
          return nullptr;

        if (this->queueLength <= nextOffset) VK_unlikely
                                                 nextOffset -= this->queueLength;

        return to_address(writeOffset);
      }
      inline void     do_end_write(message* msg,   u64  nextOffset) noexcept {
        VK_assert( msg != nullptr );

        msg->info.store({
                            .nextOffset = narrow_cast<u32>(nextOffset),
                            .state      = message_state::enqueued
                        }, std::memory_order_release);
        this->nextWriteOffset.store(nextOffset, std::memory_order_release);
        this->nextWriteOffset.notify_one();
      }

      inline message* do_begin_read() noexcept {
        const u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);
        this->nextWriteOffset.wait(readOffset, std::memory_order_acq_rel);
        return to_message(readOffset);
      }
      inline message* do_try_begin_read() noexcept {
        const u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);
        if ( readOffset == this->nextWriteOffset.load(std::memory_order_acquire)) VK_unlikely
          return nullptr;
        return to_message(readOffset);
      }
      inline void     do_end_read(message* msg) noexcept {
        const u64 nextOffset = msg->nextMsgOffset;
        this->nextReadOffset.store(nextOffset, std::memory_order_release);
        this->nextReadOffset.notify_one();
      }
    };

    template <>
    struct mailbox_ops<false, true, false> : mailbox_base, mailbox_storage<false, true>{

      using mailbox_base::mailbox_base;


      inline void* do_begin_write(u64 size,     u64& nextOffset) noexcept {
        u64 writeOffset = nextWriteOffset.load();

        do {
          nextOffset = writeOffset + size;
          const u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);
          if (nextOffset >= (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_unlikely {
            this->nextReadOffset.wait(readOffset, std::memory_order_acq_rel);
            continue;
          }
          if ( queueLength <= nextOffset ) VK_unlikely
                                               nextOffset -= queueLength;
        } while(!nextWriteOffset.compare_exchange_strong(writeOffset, nextOffset, std::memory_order_acq_rel));

        return to_address(writeOffset);
      }
      inline void* do_try_begin_write(u64 size, u64& nextOffset) noexcept {
        u64 writeOffset = nextWriteOffset.load();

        do {
          nextOffset = writeOffset + size;
          const u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);
          if (nextOffset >= (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_unlikely
            return nullptr;
          if ( queueLength <= nextOffset ) VK_unlikely
                                               nextOffset -= queueLength;
        } while(!nextWriteOffset.compare_exchange_strong(writeOffset, nextOffset, std::memory_order_acq_rel));

        return to_address(writeOffset);
      }
      inline void  do_end_write(message* msg,  u64  nextOffset) noexcept {

        u64 addrOffset = to_offset(msg);

        if ( lastReadOffset == addrOffset ) {

          msg = to_message(nextOffset);

          while ( nextWriteOffset. ) {
            addrOffset = nextOffset;

            msg        = to_message(nextOffset);
            nextOffset = msg->nextMsgOffset;
          }

          lastReadOffset.store(nextOffset, std::memory_order_release);
          lastReadOffset.notify_one();
        }
        else {

        }
      }

      inline message* do_begin_read() noexcept {
        const u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);
        this->nextWriteOffset.wait(readOffset, std::memory_order_acq_rel);
        return static_cast<message*>(to_address(readOffset));
      }
      inline message* do_try_begin_read() noexcept {
        const u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);
        if ( readOffset == lastReadOffset ) VK_unlikely
          return nullptr;
        return static_cast<message*>(to_address(readOffset));
      }
      inline void     do_end_read(message* msg) noexcept {

      }
    };

    template <>
    struct mailbox_ops<true, false, false> : mailbox_base, mailbox_storage<true, false>{

      using mailbox_base::mailbox_base;


      inline void* do_begin_write(u64 size,     u64& nextOffset) noexcept { }
      inline void* do_try_begin_write(u64 size, u64& nextOffset) noexcept { }
      inline void  do_end_write(void* addr, u64 size, u64 nextOffset) noexcept { }

      inline message* do_begin_read() noexcept { }
      inline message* do_try_begin_read() noexcept { }
      inline void     do_end_read(message* msg) noexcept { }
    };

    template <>
    struct mailbox_ops<false, false, false> : mailbox_base, mailbox_storage<false, false>{

      using mailbox_base::mailbox_base;


      inline void* do_begin_write(u64 size,     u64& nextOffset) noexcept { }
      inline void* do_try_begin_write(u64 size, u64& nextOffset) noexcept { }
      inline void  do_end_write(void* addr, u64 size, u64 nextOffset) noexcept { }

      inline message* do_begin_read() noexcept { }
      inline message* do_try_begin_read() noexcept { }
      inline void     do_end_read(message* msg) noexcept { }
    };

    template <>
    struct mailbox_ops<true, false, true> : mailbox_base, mailbox_storage<true, false>{

      using mailbox_base::mailbox_base;


      inline void* do_begin_write(u64 size,     u64& nextOffset) noexcept { }
      inline void* do_try_begin_write(u64 size, u64& nextOffset) noexcept { }
      inline void  do_end_write(void* addr, u64 size, u64 nextOffset) noexcept { }

      inline message* do_begin_read() noexcept { }
      inline message* do_try_begin_read() noexcept { }
      inline void     do_end_read(message* msg) noexcept { }
    };

    template <>
    struct mailbox_ops<false, false, true> : mailbox_base, mailbox_storage<false, false>{

      using mailbox_base::mailbox_base;


      inline void* do_begin_write(u64 size,     u64& nextOffset) noexcept { }
      inline void* do_try_begin_write(u64 size, u64& nextOffset) noexcept { }
      inline void  do_end_write(void* addr, u64 size, u64 nextOffset) noexcept { }

      inline message* do_begin_read() noexcept { }
      inline message* do_try_begin_read() noexcept { }
      inline void     do_end_read(message* msg) noexcept { }
    };
*/
  }

  template <typename Desc>
  struct mailbox_impl{
    using traits = mailbox_traits<Desc>;
    using type = mailbox::complete<
        typename traits::writer_semaphore_type,
        typename traits::reader_semaphore_type,
        typename mailbox::get_impl<traits>::template
        type<typename mailbox::get_base<traits>::type,
             typename mailbox::get_storage<traits>::type>>;
  };
}

#endif  //VALKYRIE_AGENT_DETAIL_MAILBOX_IMPL_HPP
