//
// Created by Maxwell on 2021-02-08.
//

#ifndef VALKYRIE_AGENT_MAILBOX_HPP
#define VALKYRIE_AGENT_MAILBOX_HPP


#include <valkyrie/traits.hpp>
#include <valkyrie/async/atomic.hpp>
#include <valkyrie/async/semaphore.hpp>
#include <valkyrie/status/result.hpp>
#include <valkyrie/adt/tags.hpp>
#include <valkyrie/agent/message.hpp>

namespace valkyrie{

  struct mailbox_write_op{
    void* address;
    u64   nextOffset;
  };

  namespace impl{
    class mailbox_base{
    protected:

      mailbox_base() noexcept : msgQueue(nullptr), queueLength(0){}
      mailbox_base(u64 length, system_status& status) noexcept;
      mailbox_base(const mailbox_base&) = delete;
      mailbox_base(mailbox_base&& ) noexcept;


      ~mailbox_base();


      inline void* to_address(u64 offset) const noexcept {
        return msgQueue + offset;
      }


      byte* msgQueue;
      u64   queueLength;
    };

    template <bool SingleWriter,
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

    template <u32 Count>
    struct mailbox_semaphore{
      using type = counting_semaphore<Count>;
    };
    template <>
    struct mailbox_semaphore<NoLimit>{
      using type = noop_semaphore;
    };

    template <u32 Count>
    using mailbox_semaphore_t = typename mailbox_semaphore<Count>::type;


    VK_define_tag(mailbox_read);
    VK_define_tag(mailbox_write);


    template <u32 MaxWriterCount, u32 MaxReaderCount>
    class mailbox_semaphores : tagged<mailbox_semaphore_t<MaxWriterCount>, mailbox_write>,
                               tagged<mailbox_semaphore_t<MaxReaderCount>, mailbox_read>{
      using write_semaphore = tagged<mailbox_semaphore_t<MaxWriterCount>, mailbox_write>;
      using read_semaphore  = tagged<mailbox_semaphore_t<MaxReaderCount>, mailbox_read>;
    public:
      constexpr mailbox_semaphores() noexcept
          : write_semaphore(std::in_place, MaxWriterCount),
            read_semaphore(std::in_place, MaxReaderCount){}


      bool acquire_write_access() noexcept {
        return static_cast<write_semaphore*>(this)->get_value().try_acquire();
      }
      void release_write_access() noexcept {
        static_cast<write_semaphore*>(this)->get_value().release();
      }

      bool acquire_read_access() noexcept {
        return static_cast<read_semaphore*>(this)->get_value().try_acquire();
      }
      void release_read_access() noexcept {
        static_cast<read_semaphore*>(this)->get_value().release();
      }
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
      inline void     do_end_write(void*, u64, u64 nextOffset) noexcept {
        this->nextWriteOffset.store(nextOffset, std::memory_order_release);
        this->nextWriteOffset.notify_one();
      }

      inline message* do_begin_read() noexcept {
        const u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);
        this->nextWriteOffset.wait(readOffset, std::memory_order_acq_rel);
        return static_cast<message*>(to_address(readOffset));
      }
      inline message* do_try_begin_read() noexcept {
        const u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);
        if ( readOffset == this->nextWriteOffset.load(std::memory_order_acquire)) VK_unlikely
          return nullptr;
        return static_cast<message*>(to_address(readOffset));
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


      inline void* do_begin_write(u64 size,     u64& nextOffset) noexcept { }
      inline void* do_try_begin_write(u64 size, u64& nextOffset) noexcept { }
      inline void  do_end_write(void* addr, u64 size, u64 nextOffset) noexcept { }

      inline message* do_begin_read() noexcept { }
      inline message* do_try_begin_read() noexcept { }
      inline void     do_end_read(message* msg) noexcept { }
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



    template <u32 MaxWriterCount, u32 MaxReaderCount, bool IsCoherent>
    class mailbox;
  }



  
  template <u32 MaxWriterCount, u32 MaxReaderCount, bool IsCoherent>
  class basic_mailbox
      : public impl::mailbox_base,
        public impl::mailbox_storage<MaxWriterCount, MaxReaderCount>{
  protected:
    inline constexpr static bool SingleWriter = MaxWriterCount == 1;
    inline constexpr static bool SingleReader = MaxReaderCount == 1;
    inline constexpr static bool PointToPoint = SingleWriter && SingleReader;

    using base    = impl::mailbox_base;
    using storage = impl::mailbox_storage<MaxWriterCount, MaxReaderCount>;

    VK_nodiscard inline bool  has_available_space(const u64 writeOffset, const u64 nextWrite) const noexcept {
      if constexpr ( PointToPoint ) {
        const u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);
        return nextWrite < (readOffset + (this->queueSize * bool(readOffset < writeOffset)));
      } else {
        const u64 lastOffset = this->lastWriteOffset.load(std::memory_order_acquire);
        return nextWrite < (lastOffset + (this->queueSize * bool(lastOffset < writeOffset)));
      }
    }
    VK_nodiscard inline void* to_address(const u64 offset) const noexcept {
      return this->msgQueue + offset;
    }


    VK_forceinline void* do_begin_write(u64 size, u64& nextOffset) noexcept requires(PointToPoint) {}




  public:
    
    



    void* begin_write(u64 size, u64& nextOffset) noexcept {

      u64   writeOffset = this->nextWriteOffset.load(std::memory_order_acquire);
      void* address;

      if constexpr ( PointToPoint ) {
        address    = to_address(writeOffset);
        nextOffset = writeOffset + size;

        do {
          const u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);
          if (nextOffset >= (readOffset + (this->queueSize * bool(readOffset < writeOffset)))) VK_unlikely {
            this->nextReadOffset.wait(readOffset, std::memory_order_acq_rel);
            continue;
          }
          break;
        } while(true);


        if (this->queueLength <= nextOffset) VK_unlikely nextOffset -= this->queueLength;

        nextOffset -= this->queueLength <= nextOffset ? this->queueLength : 0;

        return address;

      } else {

      }
    }
    mailbox_write_op try_begin_write(u64 size) noexcept {
      u64        writeOffset = this->nextWriteOffset.load(std::memory_order_acquire);
      mailbox_write_op info;

      if constexpr ( PointToPoint ) {
        info.address    = nullptr;
        info.nextOffset = writeOffset + size;

        const u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);
        if (info.nextOffset >= (readOffset + (this->queueSize * bool(readOffset < writeOffset)))) VK_unlikely {
          return info;
        }

        if (this->queueLength <= info.nextOffset) VK_unlikely info.nextOffset -= this->queueLength;

        info.address    = to_address(writeOffset);
        info.nextOffset -= this->queueLength <= info.nextOffset ? this->queueLength : 0;

        return info;

      } else {

      }
    }
    void       end_write(mailbox_write_op info, u64 size) noexcept {
      if constexpr ( PointToPoint ) {

      }
    }

    message* begin_read() noexcept {
      u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);

      if constexpr ( SingleReader ) {
        this->nextWriteOffset.wait(readOffset, std::memory_order_acq_rel);
        return static_cast<message*>(to_address(readOffset));
      }
      else {

      }
    }
    message* try_begin_read() noexcept {
      u64 readOffset = this->nextReadOffset.load(std::memory_order_acquire);

      if constexpr ( SingleReader ) {
        if ( readOffset == this->nextWriteOffset.load(std::memory_order_acquire)) VK_unlikely
          return nullptr;
        return static_cast<message*>(to_address(readOffset));
      }
    }
    void     end_read(message* info) noexcept { }

  };


  using private_mailbox  = basic_mailbox<1,       1>;
  using output_mailbox   = basic_mailbox<1,       NoLimit>;
  using mailbox          = basic_mailbox<NoLimit, 1>;
  using dispatch_mailbox = basic_mailbox<NoLimit, NoLimit>;
}

#endif //VALKYRIE_AGENT_MAILBOX_HPP
