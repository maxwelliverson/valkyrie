//
// Created by maxwe on 2021-03-28.
//

#include <valkyrie/adt/concurrent_list.hpp>


#define NOMINMAX
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

using namespace valkyrie;

void impl::concurrent_forward_list::clear() noexcept {
}

void impl::concurrent_forward_list::push_front(cslist_node *node) noexcept {}
void impl::concurrent_forward_list::pop_front(cslist_node *node) noexcept {}
void impl::concurrent_forward_list::push_back(cslist_node *node) noexcept {}
void impl::concurrent_forward_list::pop_back(cslist_node *node) noexcept {}

void impl::concurrent_forward_list::insert_node(cslist_node *after_node, cslist_node *node) noexcept {}
void impl::concurrent_forward_list::remove_node(cslist_node *after_node) noexcept {

}


