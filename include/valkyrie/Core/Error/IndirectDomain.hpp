//
// Created by Maxwell on 2020-12-30.
//

#ifndef VALKYRIE_INDIRECT_DOMAIN_HPP
#define VALKYRIE_INDIRECT_DOMAIN_HPP

#include <valkyrie/Core/Error/Status.hpp>

namespace valkyrie::Core{
  template <typename StatusCode_>
  class IndirectDomain : public StatusDomain{
    template <typename> friend class StatusCode;
    using Base_ = StatusDomain;
    using domain_type = typename StatusCode_::domain_type;


    inline constexpr static domain_type _domain{};


    using inner_value_type = typename StatusCode_::value_type;

    struct RefCountedStatus : StatusCode_{

      template <typename ...Args> requires(ConstructibleFrom<StatusCode_, Args...>)
      RefCountedStatus(Args&& ...args)
      : StatusCode_(std::forward<Args>(args)...), refCount_(){}
      RefCountedStatus(const RefCountedStatus&) = delete;
      RefCountedStatus& operator=(const RefCountedStatus&) = delete;

      void     acquireRef() const noexcept {
        //refCount_.fetch_add(1, std::memory_order_relaxed);
        ++refCount_;
      }
      uint32_t releaseRef() const noexcept {
        return --refCount_;
      }


    private:
      mutable std::atomic_uint32_t refCount_{0};
    };



  public:

    inline constexpr static Uuid uuid = Uuid{"c557643b-40a4-4866-9ec8-61b4b744ff7b"} ^ class_traits<domain_type>::uuid;

    template <typename T>
    struct mixin : T{

    protected:
      mixin() = default;
      template <typename ...Args> requires(ConstructibleFrom<StatusCode_, Args...>)
      mixin(std::in_place_t, const IndirectDomain* pDomain, Args&& ...args)
      : T{std::in_place, pDomain, new RefCountedStatus(std::forward<Args>(args)...)}{}
      template <typename ...Args> requires(ConstructibleFrom<inner_value_type, Args...>)
      mixin(std::in_place_t, const IndirectDomain* pDomain, Args&& ...args)
      : T{std::in_place, pDomain, new RefCountedStatus(std::in_place, std::forward<Args>(args)...)}{}
      mixin(const mixin& other)     : T(other){
        incRef();
      }
      mixin(mixin&& other) noexcept : T(std::move(other)){
        other.ptr() = nullptr;
      }
      mixin& operator=(const mixin& other) {
        if (ptr() != other.ptr()) {
          decRef();
          T::value() = other.ptr();
          incRef();
        }
        return *this;
      }
      mixin& operator=(mixin&& other) noexcept {
        T::value() = other.ptr();
        other.ptr() = nullptr;
        return *this;
      }
      ~mixin() {
        decRef();
      }

    public:
      void clear() {
        decRef();
        T::clear();
      }

      inner_value_type& value() & noexcept {
        return T::value()->value();
      }
      const inner_value_type& value() const & noexcept {
        return T::value()->value();
      }
      inner_value_type&& value() && noexcept {
        return std::move(T::value()->value());
      }
      const inner_value_type&& value() const && noexcept {
        return std::move(T::value()->value());
      }

    protected:
      auto*& ptr() noexcept {
        return T::value();
      }
      const auto*& ptr() const noexcept {
        return T::value();
      }


      void incRef() {
        if (T::value())
          T::value()->acquireRef();
      }
      void decRef() {
        auto* pValue = T::value();
        if (pValue && !pValue->releaseRef()) {
          delete pValue;
        }
      }

    private:
      using T::clear;
      using T::value;

      template <typename>
      friend class IndirectDomain;
    };

    using value_type = RefCountedStatus*;

    using Base_::string_ref;

  private:
    using code_type = StatusCode<IndirectDomain>;
    using redirected_type = StatusCode_;

    redirected_type&       redirect(StatusCode<void>& code) noexcept {
      return *static_cast<Detail::StatusStorage<IndirectDomain>&>(static_cast<code_type&>(code)).value();
    }
    const redirected_type& redirect(const StatusCode<void>& code) noexcept {
      return *static_cast<const Detail::StatusStorage<IndirectDomain>&>(static_cast<const code_type&>(code)).value();
    }
  public:

    constexpr IndirectDomain() noexcept : StatusDomain(uuid){}

    StringRef name() const noexcept override{
      return _domain.path();
    }
  protected:
    bool doFailure(const StatusCode<void>& code) const noexcept override{
      VK_assert(code.domain() == *this);
      return _domain.doFailure(redirect(code));
    }
    Severity doSeverity(const StatusCode<void>& code) const noexcept override{
      VK_assert(code.domain() == *this);
      return _domain.doSeverity(redirect(code));
    }
    Code doCode(const StatusCode<void>& code) const noexcept override{
      VK_assert(code.domain() == *this);
      return _domain.doCode(redirect(code));
    }
    string_ref doMessage(const StatusCode<void>& code) const noexcept override{
      VK_assert(code.domain() == *this);
      return _domain.doMessage(redirect(code));
    }
    bool doEquivalent(const StatusCode<void> & codeA, const StatusCode<void> & codeB) const noexcept override{
      VK_assert(codeA.domain() == *this);
      return _domain.doEquivalent(redirect(codeA), codeB);
    }
    VK_noreturn void doThrowException(const StatusCode<void>& code) const VK_throws override{
      VK_assert(code.domain() == *this);
      _domain.doThrowException(redirect(code));
    }
    void doErasedCopy(StatusCode<void>& To, const StatusCode<void>& From, size_t Bytes) const noexcept override{
      VK_assert(From.domain() == *this);
      VK_assert(Bytes == 16);
      new(&To) code_type(static_cast<const code_type&>(From));
    }
    void doErasedDestroy(StatusCode<void> & code, size_t Bytes) const noexcept override{
      VK_assert(code.domain() == *this);
      VK_assert(Bytes == 16);
      static_cast<code_type&>(code).~code_type();
    }

    inline constexpr static const IndirectDomain& get() noexcept;
  };

  namespace Detail{
    template <typename StatusCode_>
    inline constexpr static IndirectDomain<StatusCode_> indirectDomainInstance{};
  }

  template <typename StatusCode_>
  inline constexpr const IndirectDomain<StatusCode_>& IndirectDomain<StatusCode_>::get() noexcept {
    return Detail::indirectDomainInstance<StatusCode_>;
  }
}

#endif//VALKYRIE_INDIRECT_DOMAIN_HPP
