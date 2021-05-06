//
// Created by Maxwell on 2020-12-30.
//

#ifndef VALKYRIE_INDIRECT_DOMAIN_HPP
#define VALKYRIE_INDIRECT_DOMAIN_HPP

#include <valkyrie/status/status_code.hpp>

namespace valkyrie{

  template <typename Domain>
  class indirect_domain;

  template <typename Domain>
  class indirect_domain : public status_domain{

    static_assert(singleton_c<Domain>, "Indirect domains must be singletons");

    template <typename> friend class status_code;

    using indirect_traits = singleton_traits<Domain>;

    using base_type = status_domain;
    using domain_type = Domain;
    using indirect_value_type = typename domain_type::value_type;
    using indirect_status_type = status_code<domain_type>;
    using indirect_error_type  = error_code<domain_type>;



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

    inline constexpr static uuid class_id = uuid{"c557643b-40a4-4866-9ec8-61b4b744ff7b"} ^ class_traits<domain_type>::id;

    template <typename T>
    struct mixin : T{

    protected:
      mixin() = default;
      template <typename ...Args> requires(ConstructibleFrom<StatusCode_, Args...>)
      mixin(std::in_place_t, const indirect_domain* pDomain, Args&& ...args)
      : T{std::in_place, pDomain, new RefCountedStatus(std::forward<Args>(args)...)}{}
      template <typename ...Args> requires(ConstructibleFrom<inner_value_type, Args...>)
      mixin(std::in_place_t, const indirect_domain* pDomain, Args&& ...args)
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
      friend class indirect_domain;
    };

    using value_type = RefCountedStatus*;

    using base_type::string_ref;

  private:
    using code_type = status_code<indirect_domain>;
    using redirected_type = StatusCode_;

    redirected_type&       redirect(status_code<void>& code) noexcept {
      return *static_cast<detail::StatusStorage<indirect_domain>&>(static_cast<code_type&>(code)).value();
    }
    const redirected_type& redirect(const status_code<void>& code) noexcept {
      return *static_cast<const detail::StatusStorage<indirect_domain>&>(static_cast<const code_type&>(code)).value();
    }
  public:

    constexpr indirect_domain() noexcept : status_domain(class_id){}

    string_ref name() const noexcept override{
      return _domain.path();
    }
  protected:
    bool doFailure(const status_code<void>& code) const noexcept override{
      VK_assert(code.domain() == *this);
      return _domain.doFailure(redirect(code));
    }
    Severity doSeverity(const status_code<void>& code) const noexcept override{
      VK_assert(code.domain() == *this);
      return _domain.doSeverity(redirect(code));
    }
    code doCode(const status_code<void>& code) const noexcept override{
      VK_assert(code.domain() == *this);
      return _domain.doCode(redirect(code));
    }
    string_ref doMessage(const status_code<void>& code) const noexcept override{
      VK_assert(code.domain() == *this);
      return _domain.doMessage(redirect(code));
    }
    bool doEquivalent(const status_code<void> & codeA, const status_code<void> & codeB) const noexcept override{
      VK_assert(codeA.domain() == *this);
      return _domain.doEquivalent(redirect(codeA), codeB);
    }
    VK_noreturn void doThrowException(const status_code<void>& code) const VK_throws override{
      VK_assert(code.domain() == *this);
      _domain.doThrowException(redirect(code));
    }
    void doErasedCopy(status_code<void>& To, const status_code<void>& From, size_t Bytes) const noexcept override{
      VK_assert(From.domain() == *this);
      VK_assert(Bytes == 16);
      new(&To) code_type(static_cast<const code_type&>(From));
    }
    void doErasedDestroy(status_code<void> & code, size_t Bytes) const noexcept override{
      VK_assert(code.domain() == *this);
      VK_assert(Bytes == 16);
      static_cast<code_type&>(code).~code_type();
    }

    inline static const indirect_domain& get() noexcept;
  };

  template <typename StatusCode_>
  inline const indirect_domain<StatusCode_>& indirect_domain<StatusCode_>::get() noexcept {
    constexpr static indirect_domain<StatusCode_> indirectDomainInstance{};
    return indirectDomainInstance;
  }
}

#endif//VALKYRIE_INDIRECT_DOMAIN_HPP
