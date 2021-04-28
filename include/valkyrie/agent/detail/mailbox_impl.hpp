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

    class fallback_storage;
    class private_dynamic_storage;
    class private_static_storage;
    template <bool WriteCoherent>
    class default_dynamic_storage;
    template <bool WriteCoherent>
    class inbox_storage;

    template <typename Base, typename Storage>
    class private_impl;

    template <typename Base, typename Storage>
    class inbox_impl;




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
      using type = fallback_storage;
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
    requires(Traits::max_readers == 1 && Traits::max_writers != 1)
    struct get_storage<Traits>{
      using type = inbox_storage<Traits::is_write_coherent>;
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
      using type = inbox_impl<Base, Storage>;
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

      explicit static_base(u64 capacity) noexcept
          : static_base(capacity, default_allocator{}){}

      static_base(u64 capacity, status& status) noexcept : static_base(capacity, default_allocator{}, status){}

      template <raw_allocator Alloc>
      static_base(u64 capacity, Alloc&& allocator, status& status_) noexcept
          : queueAllocator(make_any_allocator_reference(std::forward<Alloc>(allocator))),
            queueCapacity(capacity),
            msgQueue(){
#if VK_exceptions_enabled
        try {
          msgQueue = static_cast<byte*>(queueAllocator.allocate_array(queueCapacity, message_size, alignof(message)));
        }
        catch(std::exception& e) {
          // TODO: Idk
          status_ = detail::ou
        }
#else
#endif
        if
      }

      template <composable_allocator Alloc>
      static_base(u64 capacity, Alloc&& allocator, status& status_) noexcept
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
    class inbox_storage{
    protected:
      VK_constant bool is_write_coherent = WriteCoherent;

      u64         readNext = 0;
      atomic<u64> writeNext = 0;

      atomic<u64> msgCount;
    };
    template <>
    class inbox_storage<true>{
    protected:
      VK_constant bool is_write_coherent = true;

      u64 readNext  = 0;
      u64 writeNext = 0;

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
          const u64 writeOffset = atomic_load(this->writeNext);
          u64 nextWrite   = writeOffset + size;

          do {
            // Get readOffset; if nextWrite is equal to or greater than readOffset,
            // wait for a message to be read, and then try again.
            const u64 readOffset = atomic_load(this->readNext);
            if (nextWrite >= (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_unlikely {
              atomic_wait(this->readNext, readOffset);
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
          atomic_wait(this->msgCount, this->queueCapacity);
          ++this->writeNext;
          this->normalize_offset(this->writeNext);
          nextOffset = this->writeNext;
          return this->to_address(writeIndex);
        }
      }
      inline void*    do_try_begin_write(u64 size, u64& nextOffset) noexcept {

        VK_detect_invalid_size(size);

        if constexpr (dynamic_message_sizes) {

          const u64 writeOffset = atomic_load(this->writeNext);
          u64 nextWrite   = writeOffset + size;

          const u64 readOffset = atomic_load(this->readNext);
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
          msg->nextOffset = nextOffset;
          atomic_store(this->writeNext, nextOffset);
          atomic_notify_one(this->writeNext);
        }
        else {
          msg->nextOffset = nextOffset;
          ++this->msgCount;
          atomic_notify_one(this->msgCount);
        }
      }

      inline message* do_begin_read() noexcept {
        if constexpr (dynamic_message_sizes) {
          const u64 readOffset = atomic_load(this->readNext);
          atomic_wait(this->writeNext, readOffset);
          return this->to_message(readOffset);
        }
        else {
          atomic_wait(this->msgCount, 0);
          return this->to_message(this->readNext);
        }
      }
      inline message* do_try_begin_read() noexcept {

        if constexpr (dynamic_message_sizes) {
          const u64 readOffset = atomic_load(this->readNext);
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
          atomic_store(this->readNext, msg->nextOffset);
          atomic_notify_one(this->readNext);
        }
        else {
          this->readNext = msg->nextOffset;
          --this->msgCount;
          atomic_notify_one(this->msgCount);
        }
      }
    };

    template <typename Base, typename Storage>
    class inbox_impl : public Base, public Storage{
      VK_constant bool dynamic_message_sizes = exact_same_as<Base, dynamic_base>;

      VK_forceinline bool is_valid_write(const u64 w, const u64 n) noexcept {
        static_assert(dynamic_message_sizes,
                      "The two argument overload of is_valid_write cannot "
                      "be called in statically sized message code");
        const u64 r = atomic_load<memory_order::relaxed>(this->readNext);
        if ( w < n ) VK_likely
          return ( n < r || r <= w );
        return n < r;
      }
      VK_forceinline bool is_valid_write() noexcept {
        static_assert(!dynamic_message_sizes, "The zero argument overload of is_valid_write cannot "
                                              "be called in dynamically sized message code");
        return atomic_load<memory_order::relaxed>(this->msgCount) != this->queueCapacity;
      }

    public:
      using Base::Base;

      inline void*    do_begin_write(u64 size,     u64& nextOffset) noexcept {

        VK_detect_invalid_size(size);
        VK_using_enum memory_order;

        if constexpr ( Storage::is_write_coherent ) {
          u64 writeOffset;
          u64 nextWrite;
          u64 currentMsgCount;

          do {
            this->isWriting.acquire();

            writeOffset = this->writeNext;
            currentMsgCount = atomic_load(this->msgCount);

            if constexpr ( dynamic_message_sizes ) {

              // calculate the next message offset based on that
              nextWrite = writeOffset + size;

              // ensure there's sufficient space
              const u64 readOffset = atomic_load<relaxed>(this->readNext);

              /*// handle the rare case where the write wraps
              if ( nextWrite >= this->queueLength ) VK_unlikely {
                nextWrite -= this->queueLength;
                if ( nextWrite < readOffset && readOffset <= writeOffset ) VK_likely {
                  break;
                }
              }
              else {

              }*/

              if (nextWrite < (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_likely {
                // if so, the operation is a success, we normalize the next offset and leave the loop
                this->normalize_offset(nextWrite);
                break;
              }
            }
            else {
              if ( currentMsgCount != this->queueCapacity ) VK_likely {
                nextWrite = writeOffset + this->message_size;
                this->normalize_offset(nextWrite);
                break;
              }
            }

            // failure path, insufficient space
            this->isWriting.release();
            atomic_wait(this->msgCount, currentMsgCount);
          } while(true);

          nextOffset = nextWrite;
          return this->to_address(writeOffset);

          /*if constexpr ( dynamic_message_sizes ) {


            // try to update writeNext, this will fail if the mailbox is already being written to.
            if ( atomic_cas(this->writeNext, writeOffset,  nextWrite) ) {

              // load readOffset and make sure the mailbox has enough space available to write to
              readOffset = atomic_load(this->readNext);
              if (nextWrite < (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_likely {
                // if so, the operation is a success and we leave the loop
                break;
              }

              // in the (ideally) rare case that the mailbox has insufficient space
              // reset writeNext so that other threads blocked threads can advance
              atomic_store(this->writeNext, writeOffset);
              atomic_notify_one(this->writeNext);

              // wait for a message to be read before trying again
              atomic_wait(this->readNext, readOffset);
              continue;
            }

            // wait for other writes to be finished before trying again
            atomic_wait(this->readEnd, writeOffset);
          }
          else {
            this->isWriting.acquire();

            if (atomic_load(this->msgCount) != this->queueCapacity ) VK_likely {
              writeOffset = atomic_load(this->writeNext);

              nextWrite  = writeOffset + this->message_size;
              this->normalize_offset(nextWrite);
              break;
            }
            // release instead of keeping it locked while waiting for a message to be read
            // this is to prevent possible deadlock in weird edge cases
            this->isWriting.release();
            atomic_wait(this->msgCount, this->queueCapacity);
          }*/
        }
        else {
          if constexpr ( dynamic_message_sizes ) {
            u64 writeOffset = atomic_load<relaxed>(this->writeNext);
            u64 nextWrite;
            do {
              const u64 readOffset = atomic_load<relaxed>(this->readNext);
              nextWrite = writeOffset + size;
              if (nextWrite >= (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_unlikely {
                atomic_wait(this->readNext, readOffset);
                continue;
              }
              this->normalize_offset(nextWrite);
            } while(!atomic_cas(this->writeNext, writeOffset,  nextWrite));
            nextOffset = nextWrite;
            return this->to_address(writeOffset);
          }
          else {
            u64 writeOffset = atomic_load<relaxed>(this->writeNext);
            u64 nextWrite;

            do {
              atomic_wait(this->msgCount, this->queueCapacity);
              nextWrite = writeOffset + this->message_size;
              this->normalize_offset(nextWrite);
            } while ( !atomic_cas(this->writeNext, writeOffset,  nextWrite) );

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

          VK_using_enum memory_order;

          if ( !this->isWriting.try_acquire() ) {
            return nullptr;
          }

          writeOffset = this->writeNext;

          if constexpr ( dynamic_message_sizes ) {

            // calculate the next message offset based on that
            nextWrite = writeOffset + size;

            // ensure there's sufficient space
            const u64 readOffset = atomic_load<relaxed>(this->readNext);

            if (nextWrite >= (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_unlikely {
              // if so, the operation is a failure, and we release the write lock
              this->isWriting.release();
              return nullptr;
            }

            this->normalize_offset(nextWrite);
          }
          else {
            if ( atomic_load(this->msgCount) == this->queueCapacity ) VK_unlikely {
              this->isWriting.release();
              return nullptr;
            }
            nextWrite = writeOffset + this->message_size;
            this->normalize_offset(nextWrite);
          }

          nextOffset = nextWrite;
          return this->to_address(writeOffset);
        }
        else {
          if constexpr ( dynamic_message_sizes ) {
            u64 writeOffset = atomic_load(this->writeNext);
            u64 nextWrite;
            u64 readOffset;
            do {
              readOffset = atomic_load(this->readNext);
              nextWrite = writeOffset + size;

              if (nextWrite >= (readOffset + (this->queueLength * bool(readOffset < writeOffset)))) VK_unlikely {
                return nullptr;
              }

              this->normalize_offset(nextWrite);
            } while(!atomic_cas(this->writeNext, writeOffset,  nextWrite));
            nextOffset = nextWrite;
            return this->to_address(writeOffset);
          }
          else {
            u64 writeOffset = atomic_load(this->writeNext);
            u64 nextWrite;

            do {
              if ( atomic_load(this->msgCount) == this->queueCapacity ) VK_unlikely {
                return nullptr;
              }
              nextWrite = writeOffset + this->message_size;
              this->normalize_offset(nextWrite);
            } while ( !atomic_cas(this->writeNext, writeOffset,  nextWrite) );

            nextOffset = nextWrite;
            return this->to_address(writeOffset);
          }
        }
      }
      inline void     do_end_write(message* msg,   u64  nextOffset) noexcept {

        VK_assert( msg );

        if constexpr ( Storage::is_write_coherent ) {
          msg->nextOffset = nextOffset;
          atomic_fetch_add(this->msgCount, 1);
          atomic_notify_one(this->msgCount);
          this->isWriting.release();
        }
        else {

          msg->info64.nextOffset = nextOffset;
          atomic_fetch_add(this->msgCount, 1);
          atomic_store(msg->info64.state, message_state64::enqueued);
          atomic_notify_one(msg->info64.state);

          /*const u64 msgOffset = this->to_offset(msg);

          if constexpr ( dynamic_message_sizes ) {

            VK_using_enum memory_order;



            msg->info64.nextOffset = nextOffset;
            if (atomic_load<relaxed>(this->readEnd) == msgOffset) {

              u64             nextReadEnd = nextOffset;
              message*        nextMsg;
              message_state64 writtenState = message_state64::written;

              do {
                nextMsg = this->to_message(nextReadEnd);
                if ( !atomic_cas(nextMsg->info64.state,
                                 writtenState,
                                 message_state64::enqueued) ) VK_likely {
                  break;
                }
                nextReadEnd = nextMsg->info64.nextOffset;
              } while (true);

              atomic_store(msg->info64.state, message_state64::enqueued);
              atomic_store(this->readEnd, nextReadEnd);
              atomic_notify_one(this->readEnd);
            }
            else {
              atomic_store(msg->info64.state, message_state64::written);
            }
          }
          else {
            msg->info64.nextOffset = nextOffset;
            atomic_store(msg->info64.state, message_state64::enqueued);
            atomic_fetch_add(this->msgCount, 1);
            atomic_notify_one(msg->info64.state);
          }*/
        }
      }

      inline message* do_begin_read() noexcept {

        VK_using_enum memory_order;

        const u64 readOffset = dynamic_message_sizes ? atomic_load<relaxed>(this->readNext) : this->readNext;

        if constexpr ( Storage::is_write_coherent ) {
          atomic_wait(this->msgCount, 0);
          return this->to_message(readOffset);
        }
        else {
          message* msg = this->to_message(readOffset);
          atomic_wait(msg->info64.state, message_state64::invalid);
          VK_assert( atomic_load(msg->info64.state) == message_state64::enqueued );
          return msg;
        }
      }
      inline message* do_try_begin_read() noexcept {
        VK_using_enum memory_order;

        if constexpr ( Storage::is_write_coherent ) {
          if ( atomic_load(this->msgCount) == 0 )
            return nullptr;
          return this->to_message(dynamic_message_sizes ? atomic_load<relaxed>(this->readNext) : this->readNext);
        }
        else {
          message* msg = this->to_message(dynamic_message_sizes ? atomic_load<relaxed>(this->readNext) : this->readNext);
          if ( atomic_load(msg->info64.state) == message_state64::invalid )
            return nullptr;
          VK_assert( atomic_load(msg->info64.state) == message_state64::enqueued );
          return msg;
        }
      }
      inline void     do_end_read(message* msg) noexcept {

        VK_using_enum memory_order;

        if constexpr ( dynamic_message_sizes ) {
          constexpr static auto order = Storage::is_write_coherent ? relaxed : release;
          atomic_store<order>(this->readNext, msg->info64.nextOffset);
        }
        else {
          this->readNext = msg->info64.nextOffset;
        }
        atomic_fetch_sub(this->msgCount, 1);
        if constexpr ( dynamic_message_sizes && !Storage::is_write_coherent ) {
          atomic_notify_all(this->readNext);
        }
        else {
          atomic_notify_all(this->msgCount);
        }
      }
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
            atomic_wait(this->nextReadOffset, readOffset);
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
        atomic_store(this->nextWriteOffset, nextOffset);
        atomic_notify_one(this->nextWriteOffset);
      }

      inline message* do_begin_read() noexcept {
        const u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);
        atomic_wait(this->nextWriteOffset, readOffset);
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
        atomic_store(this->nextReadOffset, nextOffset);
        atomic_notify_one(this->nextReadOffset);
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
            atomic_wait(this->nextReadOffset, readOffset);
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
        atomic_wait(this->nextWriteOffset, readOffset);
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
