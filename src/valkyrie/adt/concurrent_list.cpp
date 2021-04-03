//
// Created by maxwe on 2021-03-28.
//

#include <valkyrie/adt/concurrent_list.hpp>

#include <memory>


#define NOMINMAX
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

using namespace valkyrie;

namespace {
  inline impl::forward_ilist_node_base* node_cast(PSLIST_ENTRY entry) noexcept {
    return reinterpret_cast<impl::forward_ilist_node_base*>(entry);
  }
  inline impl::forward_ilist_header*    header_cast(PSLIST_HEADER header) noexcept {
    return reinterpret_cast<impl::forward_ilist_header*>(header);
  }
  inline PSLIST_HEADER                  header_cast(const impl::forward_ilist_header* header) noexcept {
    return reinterpret_cast<PSLIST_HEADER>(const_cast<impl::forward_ilist_header*>(header));
  }
  inline PSLIST_ENTRY                   node_cast(impl::forward_ilist_node_base* node) noexcept {
    return reinterpret_cast<PSLIST_ENTRY>(std::assume_aligned<impl::NodeAlignment>(node));
  }
  inline PSLIST_HEADER                  header_cast(impl::forward_ilist_header* header) noexcept {
    return reinterpret_cast<PSLIST_HEADER>(header);
  }


  class tmp_forward_clist : public impl::concurrent_forward_list{
  public:

  };


}

void impl::concurrent_forward_list::init() noexcept {
  InitializeSListHead(header_cast(&header));
}
u64  impl::concurrent_forward_list::native_length() const noexcept {
  return QueryDepthSList(header_cast(&header));
}

impl::forward_ilist_node_base* impl::concurrent_forward_list::clear() noexcept {
  //TODO: Fix buggy code.... there's a bad race condition here
  const auto len = size();
  auto* result = node_cast(InterlockedFlushSList(header_cast(&header)));
  reset_length(len);
  return result;
}

void impl::concurrent_forward_list::push_front(forward_ilist_node_base *node) noexcept {
  inc_length();
  InterlockedPushEntrySList(header_cast(&header), node_cast(node));
}
impl::forward_ilist_node_base* impl::concurrent_forward_list::pop_front() noexcept {
  return node_cast(InterlockedPopEntrySList(header_cast(&header)));
  dec_length();
}

/*void impl::concurrent_forward_list::push_back(forward_ilist_node_base *node) noexcept {

}
impl::forward_ilist_node_base* impl::concurrent_forward_list::pop_back() noexcept {

}*/

/*
void impl::concurrent_forward_list::insert_node(forward_ilist_node_base *after_node, forward_ilist_node_base *node) noexcept {
  atomic<forward_ilist_node_base*&> next(after_node->next);
  forward_ilist_node_base* tmp = next.load();
  while (!next.compare_exchange_weak(tmp, node));
  atomic<forward_ilist_node_base*&>(node->next).store(tmp);
}
impl::forward_ilist_node_base* impl::concurrent_forward_list::remove_node(forward_ilist_node_base *after_node) noexcept {
  atomic<forward_ilist_node_base*&> next(after_node->next);
  forward_ilist_node_base* next_node = next;
  forward_ilist_node_base* new_next;
  do {
    if (!next_node)
      break;
    new_next = atomic<forward_ilist_node_base*&>(next_node->next).load();
  } while (!next.compare_exchange_strong(next_node, new_next));
  return next_node;
}
*/


