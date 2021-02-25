//
// Created by Maxwell on 2021-02-07.
//

#include <valkyrie/Core/Error/GenericCode.hpp>
#include <valkyrie/Graphics/API/CommandPool.hpp>



using namespace valkyrie;
namespace api = Graphics::API;
namespace core = valkyrie::Core;
using core::Agent, core::AgentMailbox, core::AgentConcurrency;

class api::CommandPool::Impl{

  inline static void readMessages(api::CommandPool::Impl* pImpl) noexcept {

    const auto msgProc = [pImpl](core::AgentMessage& msg) noexcept -> core::AgentStatus {

    };


    while (pImpl->msgCount.load()) {
      if (not (pImpl->urgentMailbox.tryReadMessage(msgProc) or pImpl->defaultMailbox.tryReadMessage(msgProc))) {
        const auto lazyResult = pImpl->lazyMailbox.tryReadMessage(msgProc);
        VK_assert(!lazyResult.strictlyEquivalent(core::AgentCode::MailboxIsEmpty));
      }
      --pImpl->msgCount;
    }
  }

  static void agentProc(api::CommandPool::Impl* pImpl) noexcept {
    do {
      do {
        readMessages(pImpl);
      } while (pImpl->refCount.load());
      readMessages(pImpl);
    } while (pImpl->refCount.load());

    delete pImpl;
  }

  core::Atomic<u32>  refCount{1};
  core::Atomic<u32>  msgCount{0};

  AgentMailbox  urgentMailbox = AgentMailbox::deferCreation();
  AgentMailbox defaultMailbox = AgentMailbox::deferCreation();
  AgentMailbox    lazyMailbox = AgentMailbox::deferCreation();

  struct {

  } state;



public:
  Impl(core::Status& resultStatus){

    core::Maybe<AgentMailbox> result{core::Code::ObjectNotInitialized};

    if ((result = AgentMailbox::create(AgentConcurrency::MultiProducer)))
      urgentMailbox = std::move(result.value());
    else
      goto returnError;

    if ((result = AgentMailbox::create(AgentConcurrency::MultiProducer)))
      defaultMailbox = std::move(result.value());
    else
      goto returnError;

    if ((result = AgentMailbox::create(AgentConcurrency::MultiProducer)))
      lazyMailbox = std::move(result.value());
    else
      goto returnError;

    resultStatus = core::AgentCode::Success;
    return;

    returnError:
      resultStatus = result.status();
  }


};