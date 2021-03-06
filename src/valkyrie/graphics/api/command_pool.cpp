//
// Created by Maxwell on 2021-02-07.
//

#include <valkyrie/graphics/api/command_pool.hpp>
#include <valkyrie/status/generic_code.hpp>


using namespace valkyrie;
namespace api = graphics::api;
namespace core = valkyrie;

class api::command_pool::Impl{

  inline static void readMessages(api::command_pool::Impl* pImpl) noexcept {

    const auto msgProc = [pImpl](AgentMessage& msg) noexcept -> AgentStatus {

    };


    while (pImpl->msgCount.load()) {
      if (not (pImpl->urgentMailbox.tryReadMessage(msgProc) or pImpl->defaultMailbox.tryReadMessage(msgProc))) {
        const auto lazyResult = pImpl->lazyMailbox.tryReadMessage(msgProc);
        VK_assert(!lazyResult.strictlyEquivalent(AgentCode::MailboxIsEmpty));
      }
      --pImpl->msgCount;
    }
  }

  static void agentProc(api::command_pool::Impl* pImpl) noexcept {
    do {
      do {
        readMessages(pImpl);
      } while (pImpl->refCount.load());
      readMessages(pImpl);
    } while (pImpl->refCount.load());

    delete pImpl;
  }

  Atomic<u32>  refCount{1};
  Atomic<u32>  msgCount{0};

  AgentMailbox  urgentMailbox = AgentMailbox::deferCreation();
  AgentMailbox defaultMailbox = AgentMailbox::deferCreation();
  AgentMailbox    lazyMailbox = AgentMailbox::deferCreation();

  struct {

  } state;



public:
  Impl(status& resultStatus){

    maybe<AgentMailbox> result{code::ObjectNotInitialized};

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

    resultStatus = AgentCode::Success;
    return;

    returnError:
      resultStatus = result.status();
  }


};