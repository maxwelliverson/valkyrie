//
// Created by Maxwell on 2020-11-15.
//

#ifndef VALKYRIE_ENGINE_STREAM_HPP
#define VALKYRIE_ENGINE_STREAM_HPP

#include <valkyrie/graphics/api/queue.hpp>

namespace valkyrie::graphics::engine{
  class Stream{
    struct {
      api::GraphicsQueue lowPriority;
      api::GraphicsQueue averagePriority;
      api::GraphicsQueue highPriority;
    } graphics, compute;
    api::TransferQueue transfer;
    api::SparseBindingQueue sparseBinding;
  public:

  };
}

#endif//VALKYRIE_ENGINE_STREAM_HPP
